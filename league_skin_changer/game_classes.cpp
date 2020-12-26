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
#include "game_classes.hpp"
#include "encryption.hpp"
#include "fnv_hash.hpp"

#include <Windows.h>

std::uintptr_t SummonerEmoteUserComponent::rtti = 0;

SummonerEmote* SummonerEmoteUserComponent::get_summoner_emote_data( std::int32_t id )
{
	static const auto GetSummonerEmoteData = reinterpret_cast< SummonerEmote * ( __cdecl* )( std::int32_t ) >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::SummonerEmoteUserComponent__GetSummonerEmoteData );
	return GetSummonerEmoteData( id );
}

void SummonerEmoteUserComponent::set_emote_id_for_slot( SummonerEmoteSlot slot, std::int32_t emote_id )
{
	static const auto SetEmoteIdForSlot = reinterpret_cast<void( __thiscall* )( void*, std::int32_t, SummonerEmoteSlot ) >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::SummonerEmoteUserComponent__SetEmoteIdForSlot );
	SetEmoteIdForSlot( this, emote_id, slot );
}

void CharacterDataStack::push( const char* model, std::int32_t skin )
{
	static const auto Push = reinterpret_cast< int( __thiscall* )( void*, const char* model, std::int32_t skinid, std::int32_t, bool update_spells, bool dont_update_hud, bool, bool change_particle, bool, char, const char*, std::int32_t, const char*, std::int32_t ) >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::CharacterDataStack__Push );
	Push( this, model, skin, 0, false, false, false, true, false, -1, "\x00", 0, "\x00", 0 );
}

void CharacterDataStack::update( bool change )
{
	static const auto Update = reinterpret_cast< void( __thiscall* )( void*, bool ) >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::CharacterDataStack__Update );
	Update( this, change );
}

void AIBaseCommon::change_skin( const char* model, std::int32_t skin )
{

	//Update skinid in object class to appear name in scoreboard and fix for some things
	//
	reinterpret_cast< xor_value<std::int32_t>* >( std::uintptr_t( this ) + offsets::AIBaseCommon::SkinId )->encrypt( skin );

	this->get_character_data_stack( )->base_skin.skin = skin;

	//Lux has same skinid but diff model we have to handle it, game receives packets and calls Push function to change skin we do same but don't pushing new class but modify existing
	//
	if ( fnv::hash_runtime( this->get_character_data_stack( )->base_skin.model.str ) == FNV( "Lux" ) )
	{
		if ( skin == 7 )
		{
			if ( this->get_character_data_stack( )->stack.empty( ) )
			{
				this->get_character_data_stack( )->push( model, skin );
				return;
			}
			auto& last = this->get_character_data_stack( )->stack.back( );
			last.skin = skin;
			last.model.str = model;
			last.model.length = strlen( model );
			last.model.capacity = last.model.length + 1;
		} else
		{
			//Make sure that stack for lux is cleared
			//
			this->get_character_data_stack( )->stack.clear( );
		}
	}

	this->get_character_data_stack( )->update( true );
}

AIBaseCommon* AIMinionClient::get_gold_redirect_target( )
{
	static const auto GetGoldRedirectTarget = reinterpret_cast< AIBaseCommon * ( __thiscall* )( void* ) >( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::GetGoldRedirectTarget );
	return GetGoldRedirectTarget( this );
}

bool AIMinionClient::is_lane_minion( )
{
	return reinterpret_cast< xor_value<bool>* >( std::uintptr_t( this ) + offsets::AIMinionClient::IsLaneMinion )->decrypt( );
}