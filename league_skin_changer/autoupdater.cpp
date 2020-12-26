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
#include "autoupdater.hpp"
#include "offsets.hpp"
#include "game_classes.hpp"

#include <Windows.h>
#include <vector>
#include <cinttypes>
#include <string>
#include <thread>
#include <chrono>

uint8_t* find_signature( const wchar_t* szModule, const char* szSignature )
{
	auto module = GetModuleHandle( szModule );
	static auto pattern_to_byte = [ ]( const char* pattern )
	{
		auto bytes = std::vector<int>{};
		auto start = const_cast< char* >( pattern );
		auto end = const_cast< char* >( pattern ) + strlen( pattern );

		for ( auto current = start; current < end; ++current )
		{
			if ( *current == '?' )
			{
				++current;
				if ( *current == '?' )
					++current;
				bytes.push_back( -1 );
			} else
			{
				bytes.push_back( strtoul( current, &current, 16 ) );
			}
		}
		return bytes;
	};

	auto dosHeader = ( PIMAGE_DOS_HEADER ) module;
	auto ntHeaders = ( PIMAGE_NT_HEADERS ) ( ( uint8_t* ) module + dosHeader->e_lfanew );
	auto textSection = IMAGE_FIRST_SECTION( ntHeaders );

	auto sizeOfImage = textSection->SizeOfRawData;
	auto patternBytes = pattern_to_byte( szSignature );
	auto scanBytes = reinterpret_cast< uint8_t* >( module ) + textSection->VirtualAddress;

	auto s = patternBytes.size( );
	auto d = patternBytes.data( );

	auto mbi = MEMORY_BASIC_INFORMATION{ 0 };
	uint8_t* next_check_address = 0;

	for ( auto i = 0ul; i < sizeOfImage - s; ++i )
	{
		bool found = true;
		for ( auto j = 0ul; j < s; ++j )
		{
			auto current_address = scanBytes + i + j;
			if ( current_address >= next_check_address )
			{
				if ( !VirtualQuery( reinterpret_cast< void* >( current_address ), &mbi, sizeof( mbi ) ) )
					break;

				if ( mbi.Protect == PAGE_NOACCESS )
				{
					i += ( ( std::uintptr_t( mbi.BaseAddress ) + mbi.RegionSize ) - ( std::uintptr_t( scanBytes ) + i ) );
					i--;
					found = false;
					break;
				} else
				{
					next_check_address = reinterpret_cast< uint8_t* >( mbi.BaseAddress ) + mbi.RegionSize;
				}
			}

			if ( scanBytes[ i + j ] != d[ j ] && d[ j ] != -1 )
			{
				found = false;
				break;
			}
		}
		if ( found )
		{
			return &scanBytes[ i ];
		}
	}
	return nullptr;
}

class offset_signature
{
public:
	std::vector<std::string> sigs;
	bool sub_base;
	bool read;
	int32_t additional;
	uint32_t* offset;
};

std::vector<offset_signature> sigs = {
	{
		{
			"A1 ? ? ? ? 68 ? ? ? ? 8B 70 08",
			"A1 ? ? ? ? 83 C4 04 C6 40 36 15",
			"8B 35 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 04 C7 44 24 ? ? ? ? ? 89 44 24 18",
			"8B 0D ? ? ? ? FF B0 ? ? ? ? E8 ? ? ? ? A1"
		},
		true,
		true,
		0,
		&offsets::global::GameClient
	},
	{
		{
			"8B 0D ? ? ? ? 8B F8 81 C1 ? ? ? ? 57",
			"A1 ? ? ? ? 85 C0 74 18 84 C9",
			"8B 0D ? ? ? ? 8D 54 24 14 8B 3D"
		},
		true,
		true,
		0,
		&offsets::global::Player
	},
	{
		{
			"8B 0D ? ? ? ? 50 8D 44 24 18",
			"8B 35 ? ? ? ? C7 44 24 ? ? ? ? ? C7 44 24 ? ? ? ? ? E8",
			"8B 0D ? ? ? ? 53 FF 37",
			"B9 ? ? ? ? E8 ? ? ? ? 8D 88"
		},
		true,
		true,
		0,
		&offsets::global::ManagerTemplate_AIHero_
	},
	{
		{
			"89 1D ? ? ? ? 56 8D 4B 04",
			"8B 0D ? ? ? ? 83 C1 0C 89 14 24",
			"8B 0D ? ? ? ? 57 FF 74 24 08 E8 ? ? ? ? 8B F8"
		},
		true,
		true,
		0,
		&offsets::global::ChampionManager
	},
	{
		{
			"A1 ? ? ? ? 53 55 8B 6C 24 1C",
			"A1 ? ? ? ? F3 0F 10 40 ? F3 0F 11 44 24",
			"B9 ? ? ? ? E8 ? ? ? ? 8B 0D ? ? ? ? 3B C1 7F 1F 83 E9 01 89 0D ? ? ? ? 79 14 A1 ? ? ? ? 2B 05 ? ? ? ? C1 F8 02 48 A3 ? ? ? ? 8B 87"
		},
		true,
		true,
		0,
		&offsets::global::ManagerTemplate_AIMinionClient_
	},
	{
		{
			"3B 05 ? ? ? ? 75 72"
		},
		true,
		true,
		0,
		&offsets::global::Riot__g_window
	},
	{
		{
			"A1 ? ? ? ? 55 57 53",
			"A1 ? ? ? ? 53 55 57 85 C0"
		},
		true,
		true,
		0,
		&offsets::global::GfxWinMsgProc
	},
	{
		{
			"8D 8E ? ? ? ? FF 74 24 5C",
			"8D 8E ? ? ? ? FF 74 24 58"
		},
		false,
		true,
		0,
		&offsets::AIBaseCommon::CharacterDataStack
	},
	{
		{
			"80 BF ? ? ? ? ? 75 50 0F 31 33 C9 66 C7 87 ? ? ? ? ? ? 89 44 24 18"
		},
		false,
		true,
		0,
		&offsets::AIBaseCommon::SkinId
	},
	{
		{
			"8B 86 ? ? ? ? 89 4C 24 08",
			"8B B7 ? ? ? ? FF 70 14",
			"8B AE ? ? ? ? 85 FF 74 46"
		},
		false,
		true,
		0,
		&offsets::MaterialRegistry::D3DDevice
	},
	{
		{
			"8B 8E ? ? ? ? 52 57",
			"8B 8E ? ? ? ? FF B6 ? ? ? ? 51"
		},
		false,
		true,
		0,
		&offsets::MaterialRegistry::SwapChain
	},
	{
		{
			"8A 87 ? ? ? ? 88 4C 24 0B",
			"8A 87 ? ? ? ? 88 4C 24 0F 33 C9 0F B6 D0 84 C0 74 12",
			"8A 86 ? ? ? ? 88 4C 24 17",
			"8A 86 ? ? ? ? 88 4C 24 4C 33 C9 0F B6 D0 84 C0 74 1E",
			"8A 86 ? ? ? ? 88 4C 24 1B",
			"8A 87 ? ? ? ? 88 4C 24 13 33 C9 0F B6 D0 84 C0 74 15"
		},
		false,
		true,
		-1,
		&offsets::AIMinionClient::IsLaneMinion
	},
	{
		{
			"83 EC 4C 53 55 56 57 8B F9",
			"83 EC 0C 53 55 56 57 8B F9 8B 47 04",
			"E8 ? ? ? ? 8B 74 24 14 85 F6 74 1E 8B C7 F0 0F C1 46 ? 75 15 8B 06 8B CE FF 10 F0 0F C1 7E ? 4F 75 07 8B 06 8B CE FF 50 04 5F 5E 5B"
		},
		true,
		false,
		0,
		&offsets::functions::CharacterDataStack__Push
	},
	{
		{
			"83 EC 18 53 56 57 8D 44 24 20",
			"E8 ? ? ? ? 8D 4C 24 14 E8 ? ? ? ? 8B 07"
		},
		true,
		false,
		0,
		&offsets::functions::CharacterDataStack__Update
	},
	{
		{
			"A1 ? ? ? ? 85 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 60 14",
			"E8 ? ? ? ? 8B 80 ? ? ? ? 89 46 68"
		},
		true,
		false,
		0,
		&offsets::functions::Riot__Renderer__MaterialRegistry__GetSingletonPtr
	},
	{
		{
			"E8 ? ? ? ? 8B 0D ? ? ? ? 83 C4 04 8B F0 6A 0B",
			"83 EC 0C 56 8B 74 24 14 56 E8 ? ? ? ? 83 C4 04 89 74 24 04 89 44 24 08 A8 01",
		},
		true,
		false,
		0,
		&offsets::functions::translateString_UNSAFE_DONOTUSE
	},
	{
		{
			"E8 ? ? ? ? 8B F8 3B F7 0F 84",
			"E8 ? ? ? ? 8B F0 85 F6 74 33 8B 06",
			"E8 ? ? ? ? 85 C0 74 3A 8B CE",
			"E8 ? ? ? ? 39 44 24 1C 5F"
		},
		true,
		false,
		0,
		&offsets::functions::GetGoldRedirectTarget
	},
	{
		{
			"81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 56 8B B4 24 ? ? ? ? 8B C6",
			"E8 ? ? ? ? 8B 54 24 30 83 C4 08 89 44 24 10"
		},
		true,
		false,
		0,
		&offsets::functions::CharacterData__GetCharacterPackage
	},
	{
		{
			"C7 44 24 1C ? ? ? ? 8D 44 24 1C 8B CE 50 8D 44 24 10"
		},
		false,
		true,
		0,
		&SummonerEmoteUserComponent::rtti
	},
	{
		{
			"81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 56 FF B4 24 ? ? ? ? 8D 44 24 14",
			"E8 ? ? ? ? 83 C4 04 85 C0 74 65 57"
		},
		true,
		false,
		0,
		&offsets::functions::SummonerEmoteUserComponent__GetSummonerEmoteData
	},
	{
		{
			"83 EC 08 56 57 FF 74 24 14"
		},
		true,
		false,
		0,
		&offsets::functions::SummonerEmoteUserComponent__SetEmoteIdForSlot
	}
};

void autoupdater::start( )
{
	auto base = std::uintptr_t( GetModuleHandle( nullptr ) );

	//Invalid all
	// 
	for ( auto& sig : sigs )
		*sig.offset = 0;

	while ( true )
	{
		auto missing_offset = false;
		for ( auto& sig : sigs )
		{

			if ( *sig.offset != 0 )
				continue;

			for ( auto& pattern : sig.sigs )
			{
				auto address = find_signature( nullptr, pattern.c_str( ) );

				if ( !address )
				{
#ifdef DEBUG
					printf( "Signature failed: %s\n", pattern.c_str( ) );
#endif
					continue;
				}
				if ( sig.read )
					address = *reinterpret_cast< uint8_t** >( address + ( pattern.find_first_of( "?" ) / 3 ) );
				else if ( address[ 0 ] == 0xE8 )
					address = address + *reinterpret_cast< uint32_t* >( address + 1 ) + 5;

				if ( sig.sub_base )
					address -= base;

				address += sig.additional;

				*sig.offset = reinterpret_cast< uint32_t >( address );
				break;
			}

			if ( !*sig.offset )
			{
				missing_offset = true;
				break;
			}
		}

		if ( !missing_offset )
			break;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for( 2s );
	}
}