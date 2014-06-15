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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "citizen_group.hpp"

unsigned int CitizenGroup::count() const
{
  unsigned int ret = 0;
  foreach( t,_peoples) { ret += *t; }
  return ret;
}

unsigned int CitizenGroup::count( Age group ) const
{
  unsigned int tmin=0, tmax=0;
  switch( group )
  {
  case newborn: tmax=1; break;
  case child: tmin=childMin; tmax=8; break;
  case scholar: tmin=9; tmax=15; break;
  case student: tmin=16; tmax=20; break;
  case mature: tmin=matureMin; tmax=50; break;
  case aged: tmin=51; tmax=99; break;
  case longliver: tmin=99; tmax=99; break;
  }

  unsigned int ret=0;
  for( unsigned int i=tmin; i<=tmax; i++)
  {
    ret += _peoples[ i ];
  }

  return ret;
}

CitizenGroup CitizenGroup::retrieve(unsigned int rcount)
{
  CitizenGroup ret;

  if( rcount >= count() )
  {
    CitizenGroup ret = *this;
    clear();
    return ret;
  }

  while( rcount > 0 )
  {
    for( int age=newborn; age <= longliver; age++ )
    {
      unsigned int n = std::min( _peoples[ age ], 1u );

      ret[ age ] += n;
      _peoples[ age ] -= n;
      rcount -= n;

      if( rcount == 0 )
        break;
    }

    if( count() == 0 )
      break;
  }

  return ret;
}

unsigned int& CitizenGroup::operator [](unsigned int age)
{
  return _peoples[ age ];
}

CitizenGroup& CitizenGroup::operator += (const CitizenGroup& b)
{
  for( int index=newborn; index <= longliver; index++ )
  {
    _peoples[ index ] += b._peoples[ index ];
  }

  return *this;
}

bool CitizenGroup::empty() const {  return (_peoples.empty() || (0 == count())); }

void CitizenGroup::clear()
{
  foreach( t,_peoples ) { (*t) = 0; }
}

void CitizenGroup::makeOld()
{
  _peoples.pop_back();
  _peoples.insert( _peoples.begin(), 1, 0 );
}

VariantList CitizenGroup::save() const
{
  VariantList ret;

  int index=0;
  foreach( g,_peoples )
  {
    if( *g > 0 )
    {
      VariantList gv;
      gv.push_back( index );
      gv.push_back( *g );
      ret.push_back( gv );
    }
    index++;
  }

  return ret;
}

void CitizenGroup::load(const VariantList& stream)
{
  foreach( g, stream )
  {
    VariantList gv = (*g).toList();
    unsigned int age = gv.get( 0, 0u );
    unsigned int count = gv.get( 1, 0u );
    _peoples[ age ] = count;
  }
}

CitizenGroup::CitizenGroup()
{
  _peoples.resize( longliver+1 );
  _peoples.reserve( longliver+2 );
}
