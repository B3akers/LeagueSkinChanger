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

void character_data_stack::push( const char* model, int32_t skin ) {
	static const auto Push = reinterpret_cast<int( __thiscall* )( void*, const char* model, int32_t skinid, int32_t, bool update_spells, bool dont_update_hud, bool, bool change_particle, bool, char, const char*, int32_t, const char*, int32_t )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::CharacterDataStack__Push );
	Push( this, model, skin, 0, false, false, false, true, false, -1, "\x00", 0, "\x00", 0 );
}

void character_data_stack::update( bool change ) {
	static const auto Update = reinterpret_cast<void( __thiscall* )( void*, bool )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::CharacterDataStack__Update );
	Update( this, change );
}

void obj_ai_base::change_skin( const char* model, int32_t skin ) {

	//Update skinid in object class to appear name in scoreboard and fix for some things
	//
	reinterpret_cast<xor_value<int32_t>*>( std::uintptr_t( this ) + offsets::ai_base::SkinId )->encrypt( skin );

	this->get_character_data_stack( )->base_skin.skin = skin;

	//Lux has same skinid but diff model we have to handle it, game receives packets and calls Push function to change skin we do same but don't pushing new class but modify existing
	//
	if ( fnv::hash_runtime( this->get_character_data_stack( )->base_skin.model.str ) == FNV( "Lux" ) ) {
		if ( skin == 7 ) {
			if ( this->get_character_data_stack( )->stack.empty( ) ) {
				this->get_character_data_stack( )->push( model, skin );
				return;
			}
			auto& last = this->get_character_data_stack( )->stack.back( );
			last.skin = skin;
			last.model.str = model;
			last.model.length = strlen( model );
			last.model.capacity = last.model.length + 1;
		} else {
			//Make sure that stack for lux is cleared
			//
			this->get_character_data_stack( )->stack.clear( );
		}
	}

	this->get_character_data_stack( )->update( true );
}

obj_ai_base* obj_ai_minion::get_owner( ) {
	static const auto GetOwnerObject = reinterpret_cast<obj_ai_base * ( __thiscall* )( void* )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::GetOwnerObject );
	return GetOwnerObject( this );
}

bool obj_ai_minion::is_lane_minion( ) {
	return reinterpret_cast<xor_value<bool>*>( std::uintptr_t( this ) + offsets::ai_minion::IsLaneMinion )->decrypt( );
}