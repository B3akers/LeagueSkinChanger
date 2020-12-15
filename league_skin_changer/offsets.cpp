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
#include "offsets.hpp"
namespace offsets {
	namespace global {
		uint32_t Player = 0x34E0280;
		uint32_t ChampionManager = 0x34F3428;
		uint32_t Riot__g_window = 0x34E026C;
		uint32_t GfxWinMsgProc = 0x34FF420;
		uint32_t ManagerTemplate_AIMinionClient_ = 0x28A9C24;
		uint32_t ManagerTemplate_AIHero_ = 0x1C5B580;
		uint32_t GameClient = 0x34D93B4;
	};

	namespace ai_base {
		uint32_t CharacterDataStack = 0x2F38;
		uint32_t SkinId = 0xE9C;
	};

	namespace material_registry {
		uint32_t D3DDevice = 0x200;
		uint32_t SwapChain = 0x10C;
	};

	namespace ai_minion {
		uint32_t IsLaneMinion = 0x3780;
	};

	namespace functions {
		uint32_t Riot__Renderer__MaterialRegistry__GetSingletonPtr = 0x0099A6C0;
		uint32_t translateString_UNSAFE_DONOTUSE = 0xA9B710;
		uint32_t CharacterDataStack__Push = 0x1148C0;
		uint32_t CharacterDataStack__Update = 0x10ADC0;
		uint32_t GetOwnerObject = 0x16B810;
		uint32_t CharacterData__GetCharacterPackage = 0x23A740;
	};
};