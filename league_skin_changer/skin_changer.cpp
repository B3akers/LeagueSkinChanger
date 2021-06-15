/* This file is part of LeagueSkinChanger by b3akers, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) b3akers 2020
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "skin_changer.hpp"
#include "offsets.hpp"
#include "d3d_hook.hpp"
#include "skin_database.hpp"
#include "menu.hpp"
#include "fnv_hash.hpp"
#include "game_classes.hpp"
#include "autoupdater.hpp"

#include <Windows.h>
#include <cinttypes>
#include <thread>
#include <mutex>

std::once_flag change_skins;

void skin_changer::update( )
{
	auto league_module = std::uintptr_t( GetModuleHandle( nullptr ) );
	auto player = *reinterpret_cast< AIBaseCommon** >( league_module + offsets::global::Player );
	auto heroes = *reinterpret_cast< ManagerTemplate<AIHero>** >( league_module + offsets::global::ManagerTemplate_AIHero_ );
	auto minions = *reinterpret_cast< ManagerTemplate<AIMinionClient>** >( league_module + offsets::global::ManagerTemplate_AIMinionClient_ );

	// Change skins for champions when skin changer was loaded
	//
	std::call_once( change_skins, [ & ]( )
		{
			if ( player )
			{
				if ( config::current_combo_skin_index > 0 )
				{
					auto& values = skin_database::champions_skins[ fnv::hash_runtime( player->get_character_data_stack( )->base_skin.model.str ) ];
					player->change_skin( values[ config::current_combo_skin_index - 1 ].model_name.c_str( ), values[ config::current_combo_skin_index - 1 ].skin_id );
				}

				auto summoner_component = player->get_component_host( )->get_component<SummonerEmoteUserComponent>( );

				if ( summoner_component )
				{
					for ( auto& it : config::current_summoner_emotes )
					{
						auto emote_slot = summoner_component->emotes().find((SummonerEmoteSlot)it.first);
						if(emote_slot != summoner_component->emotes().end() && emote_slot->second != nullptr)
							summoner_component->set_emote_id_for_slot( ( SummonerEmoteSlot ) it.first, it.second );
					}
				}
			}

			auto my_team = player ? player->get_team( ) : 100;
			for ( std::size_t i = 0; i < heroes->length; i++ )
			{
				auto hero = heroes->list[ i ];
				if ( hero == player )
					continue;

				auto is_enemy = my_team != hero->get_team( );

				auto& config_array = is_enemy ? config::current_combo_enemy_skin_index : config::current_combo_ally_skin_index;

				auto champion_name_hash = fnv::hash_runtime( hero->get_character_data_stack( )->base_skin.model.str );

				auto config_entry = config_array.find( champion_name_hash );
				if ( config_entry == config_array.end( ) )
					continue;

				if ( config_entry->second > 0 )
				{
					auto& values = skin_database::champions_skins[ champion_name_hash ];
					hero->change_skin( values[ config_entry->second - 1 ].model_name.c_str( ), values[ config_entry->second - 1 ].skin_id );
				}
			}
		} );

	for ( std::size_t i = 0; i < heroes->length; i++ )
	{
		auto hero = heroes->list[ i ];
		// Fix for champions like elise with second form which is applied by pushing character data via server with original skinid we have to handle it
		//
		if ( hero->get_character_data_stack( )->stack.size( ) > 0 )
		{
			auto& last = hero->get_character_data_stack( )->stack.back( );
			if ( last.skin != hero->get_character_data_stack( )->base_skin.skin )
			{
				last.skin = hero->get_character_data_stack( )->base_skin.skin;
				hero->get_character_data_stack( )->update( true );
			}
		}
	}

	static const auto change_skin_for_object = [ ]( AIBaseCommon* obj, std::int32_t skin ) -> void
	{
		if ( skin == -1 )
			return;

		if ( obj->get_character_data_stack( )->base_skin.skin != skin )
		{
			obj->get_character_data_stack( )->base_skin.skin = skin;
			obj->get_character_data_stack( )->update( true );
		}
	};

	for ( std::size_t i = 0; i < minions->length; i++ )
	{
		auto minion = minions->list[ i ];
		auto owner = minion->get_gold_redirect_target( );

		if ( owner )
		{
			// - TODO - better way
			//

			// Objects like nidalee trap, illaoi mace are wards by riot so we need to check it by names
			//
			auto hash = fnv::hash_runtime( minion->get_character_data_stack( )->base_skin.model.str );
			if ( hash == FNV( "JammerDevice" )
				|| hash == FNV( "SightWard" )
				|| hash == FNV( "YellowTrinket" )
				|| hash == FNV( "VisionWard" )
			   	|| hash == FNV( "TestCubeRender10Vision" ) ) // Kindred mark in objects like minions and champions.
			{

				// Change ward skin only for localplayer or for all if we are in replay mode
				//
				if (!player || owner == player)
				{
					if (hash == FNV("TestCubeRender10Vision")) // Set kindred mark to default.
						change_skin_for_object(minion, 0);
					else
						change_skin_for_object(minion, config::current_ward_skin_index);
				}
				
				continue;
			}

			// Change skins for minions created by player
			//
			change_skin_for_object( minion, owner->get_character_data_stack( )->base_skin.skin );
		} else
		{
			if ( minion->is_lane_minion( ) )
			{
				// Change skin for lane minions
				//

				if ( player && player->get_team( ) == 200 )
					change_skin_for_object( minion, config::current_minion_skin_index * 2 + 1 );
				else
					change_skin_for_object( minion, config::current_minion_skin_index * 2 );
			} else
			{
				// Change skin for other minions like baron, blue etc.
				//

				auto hash = fnv::hash_runtime( minion->get_character_data_stack( )->base_skin.model.str );
				auto config_entry = config::current_combo_jungle_mob_skin_index.find( hash );
				if ( config_entry == config::current_combo_jungle_mob_skin_index.end( ) || config_entry->second == 0 )
					continue;

				change_skin_for_object( minion, config_entry->second - 1 );
			}

		}
	}
}

void skin_changer::init( )
{
#ifdef DEBUG
	AllocConsole( );
	freopen( "CONIN$", "r", stdin );
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );
#endif

	// First load only gameClient signature to game not crashes.
	autoupdater::start( true );

	// Wait for game to start
	//
	using namespace std::chrono_literals;
	auto client = *reinterpret_cast< GameClient** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GameClient );
	while ( !client || client->game_state != GGameState_s::Running )
	{
#ifdef DEBUG
		switch (client->game_state)
		{
		case GGameState_s::LoadingScreen:
			printf("Game is on loading screen.\n");
			break;
		case GGameState_s::Finished:
			printf("Game finished.\n");
			break;
		default:
			printf("Unknown game state.\n");
			break;
		}
#endif
		std::this_thread::sleep_for( 1s );
		client = *reinterpret_cast< GameClient** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GameClient );
	}

	std::this_thread::sleep_for( 500ms );

	// Now load others signature.
	autoupdater::start(false);

	std::this_thread::sleep_for(100ms);

	config::load( );
	d3d_hook::hook( );

	while ( true )
	{

		if ( GetAsyncKeyState( VK_F7 ) )
			break;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for( 200ms );
	}

	d3d_hook::unhook( );
#ifdef DEBUG
	FreeConsole( );
#endif
	FreeLibraryAndExitThread( my_module, 0 );
}

HMODULE skin_changer::my_module;
