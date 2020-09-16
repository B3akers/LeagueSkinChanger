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

namespace offsets {
	namespace global {
		enum {
			Player = 0x34E0280,
			ChampionManager = 0x034D93A4,
			Riot__g_window = 0x34E026C,
			GfxWinMsgProc = 0x34FF420,
			ManagerTemplate_AIMinionClient_ = 0x1C3D790,
			ManagerTemplate_AIHero_ = 0x01C3B63C,
			GameClient = 0x34D93B4,
		};
	};

	namespace ai_base {
		enum {
			CharacterDataStack = 0x2F58,
			SkinId = 0xED4
		};
	};

	namespace material_registry {
		enum {
			D3DDevice = 0x200
		};
	};

	namespace ai_minion {
		enum {
			IsLaneMinion = 0x37A0
		};
	};

	namespace game_object {
		enum {
			Name = 0x6C,
			Team = 0x4C
		};
	};

	namespace functions {
		enum {
			Riot__Renderer__MaterialRegistry__GetSingletonPtr = 0x0099A6C0,
			translateString_UNSAFE_DONOTUSE = 0x00A7E720,
			CharacterDataStack__Push = 0x0013FD00,
			CharacterDataStack__Update = 0x00127B70,
			GetOwnerObject = 0x001738E0,
			CharacterData__GetCharacterPackage = 0x00233260
		};
	};
};