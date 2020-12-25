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

namespace offsets {
	namespace game_object {
		enum {
			Name = 0x6C,
			Team = 0x4C
		};
	};

	namespace global {
		extern uint32_t Player;
		extern uint32_t ChampionManager;
		extern uint32_t Riot__g_window;
		extern uint32_t GfxWinMsgProc;
		extern uint32_t ManagerTemplate_AIMinionClient_;
		extern uint32_t ManagerTemplate_AIHero_;
		extern uint32_t GameClient;
	};

	namespace ai_base {
		extern uint32_t CharacterDataStack;
		extern uint32_t SkinId;
	};

	namespace material_registry {
		extern uint32_t D3DDevice;
		extern uint32_t SwapChain;
	};

	namespace ai_minion {
		extern uint32_t IsLaneMinion;
	};

	namespace functions {
		extern uint32_t Riot__Renderer__MaterialRegistry__GetSingletonPtr;
		extern uint32_t translateString_UNSAFE_DONOTUSE;
		extern uint32_t CharacterDataStack__Push;
		extern uint32_t CharacterDataStack__Update;
		extern uint32_t GetGoldRedirectTarget;
		extern uint32_t CharacterData__GetCharacterPackage;
	};
};