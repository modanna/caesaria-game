// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "building.hpp"

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/trainee.hpp"
#include "core/utils.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "city/states.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace {
static Renderer::PassQueue buildingPassQueue=Renderer::PassQueue(1,Renderer::overlayAnimation);
}

class Building::Impl
{
public:
  typedef std::map<constants::walker::Type,int> TraineeMap;
  typedef std::set<walker::Type> WalkerTypeSet;
  typedef std::set<Service::Type> ServiceSet;

  TraineeMap traineeMap;  // current level of trainees working in the building (0..200)
  WalkerTypeSet reservedTrainees;  // a trainee is on the way
  ServiceSet reservedServices;  // a serviceWalker is on the way

  int stateDecreaseInterval;
  float cachedPopkoef;
};

Building::Building(const object::Type type, const Size& size )
: Construction( type, size ), _d( new Impl )
{
  setState( pr::inflammability, 1 );
  setState( pr::collapsibility, 1 );
  _d->cachedPopkoef = 1;
  _d->stateDecreaseInterval = game::Date::days2ticks( 1 );
}

Building::~Building() {}

void Building::initTerrain( Tile& ) {}

void Building::timeStep(const unsigned long time)
{
  if( game::Date::isWeekChanged() )
  {
    _updateBalanceKoeffs();
  }

  if( time % _d->stateDecreaseInterval == 1 )
  {
    updateState( pr::damage, _d->cachedPopkoef * state( pr::collapsibility ) );
    updateState( pr::fire, _d->cachedPopkoef * state( pr::inflammability ) );
  }

  Construction::timeStep(time);
}

void Building::storeGoods(good::Stock &stock, const int amount)
{
  std::string bldType = debugName();
  Logger::warning( "This building should not store any goods %s at [%d,%d]",
                   bldType.c_str(), pos().i(), pos().j() );
  try
  {
   //_CAESARIA_DEBUG_BREAK_IF("This building should not store any goods");
  }
  catch(...)
  {

  }
}

float Building::evaluateService(ServiceWalkerPtr walker)
{
   float res = 0.0;
   Service::Type service = walker->serviceType();
   if(_d->reservedServices.count(service) == 1)
   {
      // service is already reserved
      return 0.0;
   }

   switch(service)
   {
   case Service::engineer: res = state( pr::damage ); break;
   case Service::prefect: res = state( pr::fire ); break;
   default: break;
   }
   return res;
}

bool Building::build( const city::AreaInfo& info )
{
  Construction::build( info );

  switch( info.city->climate() )
  {
  case game::climate::northen: setState( pr::inflammability, 0.5 ); break;
  case game::climate::desert: setState( pr::inflammability, 2 ); break;
  default: break;
  }

  _updateBalanceKoeffs();

  return true;
}

void Building::reserveService(const Service::Type service) { _d->reservedServices.insert(service);}
bool Building::isServiceReserved( const Service::Type service ) { return _d->reservedServices.count(service)>0; }
void Building::cancelService(const Service::Type service){ _d->reservedServices.erase(service);}

void Building::applyService( ServiceWalkerPtr walker)
{
   // std::cout << "apply service" << std::endl;
   // remove service reservation
   Service::Type service = walker->serviceType();
   _d->reservedServices.erase(service);

   switch( service )
   {
   case Service::engineer: setState( pr::damage, 0 ); break;
   case Service::prefect: setState( pr::fire, 0 ); break;
   default: break;
   }
}

float Building::evaluateTrainee(walker::Type traineeType)
{
   float res = 0.0;

   if( _d->reservedTrainees.count(traineeType) == 1 )
   {
      // don't allow two reservations of the same type
      return 0.0;
   }

   int trValue = traineeValue( traineeType );
   if( trValue >= 0 )
   {
      res = (float)( 100 - trValue );
   }

   return res;
}

void Building::reserveTrainee(walker::Type traineeType) { _d->reservedTrainees.insert(traineeType); }
void Building::cancelTrainee(walker::Type traineeType) { _d->reservedTrainees.erase(traineeType);}

void Building::updateTrainee(  TraineeWalkerPtr walker )
{
   _d->reservedTrainees.erase( walker->type() );
   _d->traineeMap[ walker->type() ] += walker->value() ;
}

void Building::setTraineeValue(walker::Type type, int value)
{
  _d->traineeMap[ type ] = value;
}

int Building::traineeValue(walker::Type traineeType) const
{
  Impl::TraineeMap::iterator i = _d->traineeMap.find( traineeType );
  return i != _d->traineeMap.end() ? i->second : -1;
}

Renderer::PassQueue Building::passQueue() const {  return buildingPassQueue;}

void Building::_updateBalanceKoeffs()
{
  _d->cachedPopkoef = std::max<float>( statistic::getBalanceKoeff( _city() ), 0.1f );
  _d->cachedPopkoef *= _city()->getOption( PlayerCity::fireKoeff ) / 100.f;
}
