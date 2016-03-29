engine.log("CaesarIA: load modules started")

var modules = [
                  "namespaces",
                  "common",
                  "g_config",
                  "g_config_goods",
                  "vector",
                  "if_session",
                  "if_city",
                  "if_picture",
                  "if_datetime",
                  "if_overlay",
                  "if_gui_widgets",
                  "if_gui_window",
                  "g_ui",
                  "init_game",
                  "g_config_colors",
                  "g_config_languages",
                  "g_config_ranks",
                  "g_config_movie",
                  "g_config_steam",
                  "g_config_minimap",
                  "g_config_climate",                  
                  "init_render",
                  "game_pantheon",
                  "game_ui_dialogs",
                  "game_sound_player",
                  "game_ui_dialogs_audio_opts",
                  "game_ui_dialogs_video_options",
                  "game_ui_dialog_player_salary_settings",
                  "game_ui_infobox_temple",
                  "game_ui_infobox_manager",
                  "game_ui_infobox_engineering",
                  "mission_events",
                  "mission_timescale",
                  "splash_common",
                  "sim_ui_dialogs_mission_targets",
                  "sim_ui_dialogs_city_settings",
                  "sim_ui_dialogs_speed_settings",
                  "sim_ui_dialogs_gift_emperor",
                  "sim_ui_topmenu_debug",
                  "sim_ui_topmenu",
                  "sim_hotkeys",
                  "mission_common",
                  "lobby",
                  "lobby_steam",
                  "lobby_ui_dialogs_package_options",
                  "lobby_ui_loadgame_loadmission"
              ]

for (var i in modules)
{
    engine.log("Loading module " + modules[i])
    engine.loadModule(":/system/"+modules[i]+".js")
}