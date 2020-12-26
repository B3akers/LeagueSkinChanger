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
#include <memory>
#include <map>
#include <unordered_map>

#include "offsets.hpp"

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(SIZE) uint8_t MACRO_CONCAT(_pad, __COUNTER__)[SIZE];

template <size_t Index, typename ReturnType, typename... Args>
ReturnType call_virtual( void* instance, Args... args )
{
	using Fn = ReturnType( __thiscall* )( void*, Args... );

	auto function = ( *reinterpret_cast< Fn** >( instance ) )[ Index ];
	return function( instance, args... );
}

enum class GGameState_s : std::int32_t
{
	LoadingScreen = 0,
	Connecting = 1,
	Running = 2,
	Poaused = 3,
	Finished = 4,
	Exiting = 5
};

enum class SummonerEmoteSlot : std::int32_t
{
	None = -1,
	Top = 0,
	Right = 1,
	Bottom = 2,
	Left = 3,
	Middle = 4,
	Start = 5,
	Victory = 6,
	First_Blood = 7,
	Ace = 8,
	Missing = 9
};

inline const char* SummonerEmoteSlot_to_string( SummonerEmoteSlot slot )
{
	switch ( slot )
	{
	case SummonerEmoteSlot::Top:
		return "Top";
	case SummonerEmoteSlot::Right:
		return "Right";
	case SummonerEmoteSlot::Bottom:
		return "Bottom";
	case SummonerEmoteSlot::Left:
		return "Left";
	case SummonerEmoteSlot::Middle:
		return "Middle";
	case SummonerEmoteSlot::Start:
		return "Start";
	case SummonerEmoteSlot::Victory:
		return "Victory";
	case SummonerEmoteSlot::First_Blood:
		return "First_Blood";
	case SummonerEmoteSlot::Ace:
		return "Ace";
	case SummonerEmoteSlot::Missing:
		return "Missing";
	default:
		return "Unknown";
	}
}

class GameClient
{
	PAD( 0x8 );
public:
	GGameState_s game_state;
};

class AString
{
public:
	const char* str;
	std::size_t length;
	std::size_t capacity;
};

class Champion
{
public:
	class Skin
	{
	public:
		std::int32_t skin_id;
		AString skin_name;
	};
private:
	PAD( 0x4 );
public:
	AString champion_name;
	PAD( 0x48 );
	std::vector<Skin> skins;
	PAD( 0x8 );
};

class ChampionManager
{
	PAD( 0xC );
public:
	std::vector<Champion*> champions;
};

class CharacterStackData
{
public:
	AString model;
	std::int32_t skin;
	PAD( 0x20 );
	bool update_spells;
	bool dont_update_hud;
	bool change_particle;
	PAD( 0x1 );
	PAD( 0xC );
};

class CharacterDataStack
{
public:
	std::vector<CharacterStackData> stack;
	CharacterStackData base_skin;

	void update( bool change );
	void push( const char* model, std::int32_t skin );
};

class r3dTexture;

class SummonerEmote
{
public:
	std::int32_t id;
	PAD( 0x24 );
	AString emote_name;
};

class SummonerEmoteUserComponent
{
public:
	std::unordered_map<SummonerEmoteSlot, SummonerEmote*>& emotes( ) { return *reinterpret_cast< std::unordered_map<SummonerEmoteSlot, SummonerEmote*>* >( std::uintptr_t( this ) + offsets::SummonerEmoteUserComponent::Emotes ); }
public:
	void set_emote_id_for_slot( SummonerEmoteSlot slot, std::int32_t emote_id );
public:
	static SummonerEmote* get_summoner_emote_data( std::int32_t id );
	static std::uintptr_t rtti;
};

class ComponentHost
{
private:
	std::map<std::uintptr_t, void*>& components( ) { return *reinterpret_cast< std::map<std::uintptr_t, void*>* >( std::uintptr_t( this ) + offsets::ComponentHost::Components ); }
public:
	template<typename T>
	T* get_component( )
	{
		auto compoment = this->components( ).find( T::rtti );
		return compoment != this->components( ).end( ) ? *reinterpret_cast< T** >( *reinterpret_cast< std::uintptr_t* >( compoment->second ) + 0x0008 ) : nullptr;
	}
};

class GameObject
{
public:
	std::string& name( ) { return *reinterpret_cast< std::string* >( std::uintptr_t( this ) + offsets::GameObject::Name ); }
	std::int32_t get_team( ) { return *reinterpret_cast< std::int32_t* >( std::uintptr_t( this ) + offsets::GameObject::Team ); }

	ComponentHost* get_component_host( ) { return call_virtual<1, ComponentHost*>( this ); }
};

class AIBaseCommon : public GameObject
{
public:
	CharacterDataStack* get_character_data_stack( ) { return reinterpret_cast< CharacterDataStack* >( std::uintptr_t( this ) + offsets::AIBaseCommon::CharacterDataStack ); }

	void change_skin( const char* model, std::int32_t skin );
};

class AIHero : public AIBaseCommon
{
public:

};

class AIMinionClient : public AIBaseCommon
{
public:
	AIBaseCommon* get_gold_redirect_target( );
	bool is_lane_minion( );
};

template <class T>
class ManagerTemplate
{
	PAD( 0x4 );
public:
	T** list;
	std::size_t length;
	std::size_t capacity;
};