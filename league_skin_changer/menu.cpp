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
#include "menu.hpp"
#include "skin_database.hpp"
#include "fnv_hash.hpp"
#include "offsets.hpp"
#include "game_classes.hpp"
#include "../imgui/imgui.h"
#include "json.hpp"

#include <Windows.h>
#include <fstream>

using json = nlohmann::json;

int32_t config::current_combo_skin_index = 0;
int32_t config::current_combo_ward_index = 0;
int32_t config::current_combo_minion_index = 0;

int32_t config::current_ward_skin_index = -1;
int32_t config::current_minion_skin_index = -1;

std::map<uint32_t, int32_t> config::current_combo_jungle_mob_skin_index;

std::map<uint32_t, int32_t> config::current_combo_ally_skin_index;
std::map<uint32_t, int32_t> config::current_combo_enemy_skin_index;

std::map<uint32_t, int32_t> config::current_summoner_emotes;

auto config_json = json( );

std::vector<std::pair<SummonerEmoteSlot, std::string>> available_emote_slots;
std::int32_t current_emote_slot = 0;
std::int32_t current_emote = 0;

void config::save( )
{
	auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
	if ( player )
		config_json[ std::string( player->get_character_data_stack( )->base_skin.model.str ) + ".current_combo_skin_index" ] = current_combo_skin_index;

	config_json[ "current_combo_ward_index" ] = current_combo_ward_index;
	config_json[ "current_combo_minion_index" ] = current_combo_minion_index;
	config_json[ "current_ward_skin_index" ] = current_ward_skin_index;
	config_json[ "current_minion_skin_index" ] = current_minion_skin_index;

	for ( auto& it : config::current_combo_ally_skin_index )
		config_json[ "current_combo_ally_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_combo_enemy_skin_index )
		config_json[ "current_combo_enemy_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_combo_jungle_mob_skin_index )
		config_json[ "current_combo_jungle_mob_skin_index" ][ std::to_string( it.first ) ] = it.second;

	for ( auto& it : config::current_summoner_emotes )
		config_json[ "current_summoner_emotes" ][ std::to_string( it.first ) ] = it.second;

	auto out = std::ofstream( L"league_changer.json" );
	out << config_json.dump( );
	out.close( );
}

void config::load( )
{
	auto out = std::ifstream( L"league_changer.json" );
	if ( !out.good( ) )
		return;

	config_json = json::parse( out );

	auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
	if ( player )
		current_combo_skin_index = config_json.value( std::string( player->get_character_data_stack( )->base_skin.model.str ) + ".current_combo_skin_index", 0 );

	current_combo_ward_index = config_json.value( "current_combo_ward_index", 0 );
	current_combo_minion_index = config_json.value( "current_combo_minion_index", 0 );
	current_ward_skin_index = config_json.value( "current_ward_skin_index", -1 );
	current_minion_skin_index = config_json.value( "current_minion_skin_index", -1 );

	auto ally_skins = config_json.find( "current_combo_ally_skin_index" );
	if ( ally_skins != config_json.end( ) )
		for ( auto& it : ally_skins.value( ).items( ) )
			current_combo_ally_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto enemy_skins = config_json.find( "current_combo_enemy_skin_index" );
	if ( enemy_skins != config_json.end( ) )
		for ( auto& it : enemy_skins.value( ).items( ) )
			current_combo_enemy_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto jungle_mobs_skins = config_json.find( "current_combo_jungle_mob_skin_index" );
	if ( jungle_mobs_skins != config_json.end( ) )
		for ( auto& it : jungle_mobs_skins.value( ).items( ) )
			current_combo_jungle_mob_skin_index[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	auto summoner_emotes = config_json.find( "current_summoner_emotes" );
	if ( summoner_emotes != config_json.end( ) )
		for ( auto& it : summoner_emotes.value( ).items( ) )
			current_summoner_emotes[ std::stoul( it.key( ) ) ] = it.value( ).get<int32_t>( );

	out.close( );
}

void config::reset( )
{
	current_combo_skin_index = 0;
	current_combo_ward_index = 0;
	current_combo_minion_index = 0;

	current_ward_skin_index = -1;
	current_minion_skin_index = -1;

	current_combo_ally_skin_index.clear( );
	current_combo_enemy_skin_index.clear( );
	current_combo_jungle_mob_skin_index.clear( );
	current_summoner_emotes.clear( );
}

char str_buffer[ 256 ];
void menu::draw( )
{
	ImGui::Begin( "LeagueChanger" );
	{
		static auto vector_getter_skin = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<skin_database::skin_info>* >( vec );
			if ( idx < 0 || idx > static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = idx == 0 ? "Don't change" : vector.at( idx - 1 ).skin_name.c_str( );
			return true;
		};

		static auto vector_getter_ward_skin = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<std::pair<int32_t, std::string>>* >( vec );
			if ( idx < 0 || idx > static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = idx == 0 ? "Don't change" : vector.at( idx - 1 ).second.c_str( );
			return true;
		};

		static auto vector_getter_default = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<std::string>* >( vec );
			if ( idx < 0 || idx > static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = idx == 0 ? "Don't change" : vector.at( idx - 1 ).c_str( );
			return true;
		};

		static auto vector_getter_summoner_emote = [ ]( void* vec, int idx, const char** out_text )
		{
			auto& vector = *static_cast< std::vector<std::pair<int32_t, std::string>>* >( vec );
			if ( idx < 0 || idx >= static_cast< int >( vector.size( ) ) ) { return false; }
			*out_text = vector.at( idx ).second.c_str( );
			return true;
		};

		auto player = *reinterpret_cast< AIBaseCommon** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Player );
		if ( player )
		{
			auto& values = skin_database::champions_skins[ fnv::hash_runtime( player->get_character_data_stack( )->base_skin.model.str ) ];
			ImGui::Text( "Localplayer skins settings:" );

			if ( ImGui::Combo( "Current skin", &config::current_combo_skin_index, vector_getter_skin, static_cast< void* >( &values ), values.size( ) + 1 ) )
				if ( config::current_combo_skin_index > 0 )
					player->change_skin( values[ config::current_combo_skin_index - 1 ].model_name.c_str( ), values[ config::current_combo_skin_index - 1 ].skin_id );

			if ( ImGui::Combo( "Current ward skin", &config::current_combo_ward_index, vector_getter_ward_skin, static_cast< void* >( &skin_database::wards_skins ), skin_database::wards_skins.size( ) + 1 ) )
				config::current_ward_skin_index = config::current_combo_ward_index == 0 ? -1 : skin_database::wards_skins.at( config::current_combo_ward_index - 1 ).first;

			auto summoner_emote_component = player->get_component_host( )->get_component<SummonerEmoteUserComponent>( );
			if ( summoner_emote_component )
			{
				ImGui::Text( "Localplayer emotes settings:" );
				if ( available_emote_slots.empty( ) )
				{
					for ( auto& emote : summoner_emote_component->emotes( ) )
					{
						if ( !emote.second )
							continue;

						available_emote_slots.push_back( { emote.first, SummonerEmoteSlot_to_string( emote.first ) } );
					}
				}

				if ( !available_emote_slots.empty( ) )
				{
					if ( ImGui::Combo( "Summoner emote slot", &current_emote_slot, vector_getter_summoner_emote, static_cast< void* >( &available_emote_slots ), available_emote_slots.size( ) ) )
					{
						auto emote_id = summoner_emote_component->emotes( )[ available_emote_slots[ current_emote_slot ].first ]->id;
						auto it = std::find_if( skin_database::summoner_emotes.begin( ), skin_database::summoner_emotes.end( ),
							[ emote_id ]( const std::pair<SummonerEmote*, std::string>& element ) { return element.first->id == emote_id; } );

						if ( it != skin_database::summoner_emotes.end( ) )		
							current_emote = std::distance( skin_database::summoner_emotes.begin( ), it );		
					}

					if ( ImGui::Combo( "Summoner emote", &current_emote, vector_getter_summoner_emote, static_cast< void* >( &skin_database::summoner_emotes ), skin_database::summoner_emotes.size( ) ) )
					{
						auto emote_slot = available_emote_slots[ current_emote_slot ].first;
						auto emote_instance = skin_database::summoner_emotes[ current_emote ].first;

						summoner_emote_component->set_emote_id_for_slot( emote_slot, emote_instance->id );

						config::current_summoner_emotes[ static_cast< std::uint32_t >( emote_slot ) ] = emote_instance->id;
					}
				}
			}

			ImGui::Separator( );
		}

		ImGui::Text( "Global skins settings:" );
		if ( ImGui::Combo( "Current minion skin", &config::current_combo_minion_index, vector_getter_default, static_cast< void* >( &skin_database::minions_skins ), skin_database::minions_skins.size( ) + 1 ) )
			config::current_minion_skin_index = config::current_combo_minion_index - 1;
		ImGui::Separator( );
		ImGui::Text( "Jungle mobs skins settings:" );

		for ( auto& it : skin_database::jungle_mobs_skins )
		{
			snprintf( str_buffer, 256, "Current %s skin", it.name.c_str( ) );
			auto config_entry = config::current_combo_jungle_mob_skin_index.insert( { it.name_hashes.front( ),0 } );

			if ( ImGui::Combo( str_buffer, &config_entry.first->second, vector_getter_default, static_cast< void* >( &it.skins ), it.skins.size( ) + 1 ) )
			{
				for ( auto& hash : it.name_hashes )
					config::current_combo_jungle_mob_skin_index[ hash ] = config_entry.first->second;
			}
		}

		ImGui::Separator( );

		auto my_team = player ? player->get_team( ) : 100;
		auto heroes = *reinterpret_cast< ManagerTemplate<AIHero>** >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::ManagerTemplate_AIHero_ );

		ImGui::Text( "Other championss skins settings:" );

		auto last_team = int32_t( 0 );
		for ( size_t i = 0; i < heroes->length; i++ )
		{
			auto hero = heroes->list[ i ];
			if ( hero == player )
				continue;

			auto hero_team = hero->get_team( );
			auto is_enemy = hero_team != my_team;

			if ( last_team == 0 || hero_team != last_team )
			{
				if ( last_team != 0 )
					ImGui::Separator( );
				if ( !is_enemy )
					ImGui::Text( "	Ally champions" );
				else
					ImGui::Text( "	Enemy champions" );
				last_team = hero_team;
			}

			auto& config_array = is_enemy ? config::current_combo_enemy_skin_index : config::current_combo_ally_skin_index;

			auto champion_name_hash = fnv::hash_runtime( hero->get_character_data_stack( )->base_skin.model.str );
			auto config_entry = config_array.insert( { champion_name_hash,0 } );

			snprintf( str_buffer, 256, "Current skin (%s)##%X", hero->name( ).c_str( ), reinterpret_cast< uintptr_t >( hero ) );

			auto& values = skin_database::champions_skins[ champion_name_hash ];
			if ( ImGui::Combo( str_buffer, &config_entry.first->second, vector_getter_skin, static_cast< void* >( &values ), values.size( ) + 1 ) )
				if ( config_entry.first->second > 0 )
					hero->change_skin( values[ config_entry.first->second - 1 ].model_name.c_str( ), values[ config_entry.first->second - 1 ].skin_id );
		}
	}
	ImGui::End( );
}