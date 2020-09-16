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
#include "d3d9_hook.hpp"
#include "offsets.hpp"
#include "vmt_smart_hook.hpp"
#include "menu.hpp"
#include "skin_changer.hpp"

#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_extend.h"
#include "game_classes.hpp"

#include <Windows.h>
#include <cinttypes>
#include <memory>
#include <string>
#include <mutex>

#include <d3d9.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

bool menu_is_open = false;

using riot_wndproc = LRESULT( __cdecl* )( HWND, UINT, WPARAM, LPARAM );
riot_wndproc original_wndproc;
LRESULT __cdecl wnd_proc( HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param ) {
	if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, w_param, l_param ) )
		return true;

	if ( msg == WM_KEYDOWN && w_param == VK_INSERT ) {
		menu_is_open = !menu_is_open;
		if ( !menu_is_open )
			config::save( );
	}

	return original_wndproc( hwnd, msg, w_param, l_param );
}

std::once_flag init_device;
std::unique_ptr<vmt_smart_hook> d3d_device_vmt = nullptr;

namespace d3d_vtable {
	struct end_scene {
		static long __stdcall hooked( IDirect3DDevice9* p_device ) {
			std::call_once( init_device, [ & ] ( ) {
				ImGui::CreateContext( );
				ImGui::StyleColorsDark( );

				ImGui_ImplWin32_Init( *reinterpret_cast<HWND*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Riot__g_window ) );
				ImGui_ImplDX9_Init( p_device );

				original_wndproc = *reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc );
				*reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc ) = wnd_proc;
			} );

			auto client = *reinterpret_cast<game_client**>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GameClient );
	
			if ( client && client->game_state == game_state_stage::running ) {
				skin_changer::update( );

				if ( menu_is_open ) {
					ImGui_ImplDX9_NewFrame( );
					ImGui_ImplWin32_NewFrame( );
					ImGui::NewFrame( );

					menu::draw( );

					ImGui::EndFrame( );
					ImGui::Render( );
					ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
				}
			}

			return m_original( p_device );
		}
		static decltype( &hooked ) m_original;
	};
	decltype( end_scene::m_original ) end_scene::m_original;

	struct reset {
		static long __stdcall hooked( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* parametrs ) {

			ImGui_ImplDX9_InvalidateDeviceObjects( );

			auto hr = m_original( device, parametrs );

			if ( hr >= 0 )
				ImGui_ImplDX9_CreateDeviceObjects( );

			return hr;

		}
		static decltype( &hooked ) m_original;
	};
	decltype( reset::m_original ) reset::m_original;
};
using namespace d3d_vtable;

void d3d9_hook::hook( ) {
	auto material_registry = reinterpret_cast<uintptr_t( __stdcall* )( )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::Riot__Renderer__MaterialRegistry__GetSingletonPtr )( );
	auto d3d_device = *reinterpret_cast<IDirect3DDevice9**>( material_registry + offsets::material_registry::D3DDevice );

	d3d_device_vmt = std::make_unique<::vmt_smart_hook>( d3d_device );
	d3d_device_vmt->apply_hook<end_scene>( 42 );
	d3d_device_vmt->apply_hook<reset>( 16 );
}

void d3d9_hook::unhook( ) {
	*reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc ) = original_wndproc;
	d3d_device_vmt->unhook( );
}