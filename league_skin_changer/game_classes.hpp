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
#pragma once
#include <vector>
#include <cinttypes>
#include <string>

#include "offsets.hpp"

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(SIZE) uint8_t MACRO_CONCAT(_pad, __COUNTER__)[SIZE];

namespace game_state_stage {
	enum values {
		loading_screen = 0,
		connecting = 1,
		running = 2,
		oaused = 3,
		finished = 4,
		exiting = 5
	};
};

class game_client {
	PAD( 0x8 );
public:
	int32_t game_state;
};

class riot_string {
public:
	const char* str;
	size_t length;
	size_t capacity;
};

class champion_skin_data {
public:
	int32_t skin_id;
	riot_string skin_name;
};

class champion_data {
	PAD( 0x4 );
public:
	riot_string champion_name;
	PAD( 0x48 );
	std::vector<champion_skin_data> skins;
	PAD( 0x8 );
};

class champion_manager {
	PAD( 0xC );
public:
	std::vector<champion_data*> champions;
};

class character_data {
public:
	riot_string model;
	int32_t skin;
	PAD( 0x20 );
	bool update_spells;
	bool dont_update_hud;
	bool change_particle;
	PAD( 0x1 );
	PAD( 0xC );
};

class character_data_stack {
public:
	std::vector<character_data> stack;
	character_data base_skin;

	void update( bool change );
	void push( const char* model, int32_t skin );
};

class game_object {
public:
	std::string& get_name( ) { return *reinterpret_cast<std::string*>( std::uintptr_t( this ) + offsets::game_object::Name ); }
	int32_t get_team( ) { return *reinterpret_cast<int32_t*>( std::uintptr_t( this ) + offsets::game_object::Team ); }
};

class obj_ai_base : public game_object {
public:
	character_data_stack* get_character_data_stack( ) { return reinterpret_cast<character_data_stack*>( std::uintptr_t( this ) + offsets::ai_base::CharacterDataStack ); }

	void change_skin( const char* model, int32_t skin );
};

class obj_ai_hero : public obj_ai_base {
public:

};

class obj_ai_minion : public obj_ai_base {
public:
	obj_ai_base* get_owner( );
	bool is_lane_minion( );
};

template <class T>
class manager_template {
	PAD( 0x4 );
public:
	T** list;
	size_t length;
	size_t capacity;
};

template <class T>
class manager_template_old {
	PAD( 0x24 );
public:
	std::vector<T> list;
};