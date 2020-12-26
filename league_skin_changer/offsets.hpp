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

namespace offsets
{
	namespace GameObject
	{
		enum
		{
			Name = 0x6C,
			Team = 0x4C
		};
	};

	namespace ComponentHost
	{
		enum
		{
			Components = 0x00
		};
	}

	namespace SummonerEmoteUserComponent
	{
		enum
		{
			Emotes = 0x000C
		};
	}

	namespace global
	{
		extern std::uint32_t Player;
		extern std::uint32_t ChampionManager;
		extern std::uint32_t Riot__g_window;
		extern std::uint32_t GfxWinMsgProc;
		extern std::uint32_t ManagerTemplate_AIMinionClient_;
		extern std::uint32_t ManagerTemplate_AIHero_;
		extern std::uint32_t GameClient;
	};

	namespace AIBaseCommon
	{
		extern std::uint32_t CharacterDataStack;
		extern std::uint32_t SkinId;
	};

	namespace MaterialRegistry
	{
		extern std::uint32_t D3DDevice;
		extern std::uint32_t SwapChain;
	};

	namespace AIMinionClient
	{
		extern std::uint32_t IsLaneMinion;
	};

	namespace functions
	{
		extern std::uint32_t Riot__Renderer__MaterialRegistry__GetSingletonPtr;
		extern std::uint32_t translateString_UNSAFE_DONOTUSE;
		extern std::uint32_t CharacterDataStack__Push;
		extern std::uint32_t CharacterDataStack__Update;
		extern std::uint32_t GetGoldRedirectTarget;
		extern std::uint32_t CharacterData__GetCharacterPackage;
		extern std::uint32_t SummonerEmoteUserComponent__GetSummonerEmoteData;
		extern std::uint32_t SummonerEmoteUserComponent__SetEmoteIdForSlot;
	};
};