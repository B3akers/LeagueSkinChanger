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
#include <cinttypes>
#include <map>

namespace config {
	extern int32_t current_combo_skin_index;
	extern int32_t current_combo_ward_index;
	extern int32_t current_combo_minion_index;

	extern int32_t current_ward_skin_index;
	extern int32_t current_minion_skin_index;

	extern std::map<uint32_t, int32_t> current_combo_ally_skin_index;
	extern std::map<uint32_t, int32_t> current_combo_enemy_skin_index;
	extern std::map<uint32_t, int32_t> current_combo_jungle_mob_skin_index;

	extern std::map<uint32_t, int32_t> current_summoner_emotes;

	void save( );
	void load( );
	void reset( );
};

namespace menu {
	void draw( );
};

