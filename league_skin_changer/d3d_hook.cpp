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
#include "d3d_hook.hpp"
#include "offsets.hpp"
#include "vmt_smart_hook.hpp"
#include "menu.hpp"
#include "skin_changer.hpp"
#include "skin_database.hpp"

#include "imgui_impl_dx9.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_extend.h"
#include "game_classes.hpp"

#include <Windows.h>
#include <cinttypes>
#include <memory>
#include <string>
#include <mutex>

#include <d3d9.h>
#include <d3d11.h>

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
std::unique_ptr<vmt_smart_hook> swap_chain_vmt = nullptr;

bool get_system_font_path( const std::string& name, std::string& path ) {
	char buffer[ MAX_PATH ];
	HKEY registryKey;

	GetWindowsDirectoryA( buffer, MAX_PATH );
	std::string fontsFolder = buffer + std::string( "\\Fonts\\" );

	if ( RegOpenKeyExA( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey ) ) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[ MAX_PATH ];
	uint8_t valueData[ MAX_PATH ];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>( &valueNameSize ),
			0,
			reinterpret_cast<DWORD*>( &valueType ),
			valueData,
			reinterpret_cast<DWORD*>( &valueDataSize ) );

		valueIndex++;

		if ( error == ERROR_NO_MORE_ITEMS ) {
			RegCloseKey( registryKey );
			return false;
		}

		if ( error || valueType != REG_SZ ) {
			continue;
		}

		if ( _strnicmp( name.data( ), valueName, name.size( ) ) == 0 ) {
			path = fontsFolder + std::string( (char*)valueData, valueDataSize );
			RegCloseKey( registryKey );
			return true;
		}
	} while ( true );

	return false;
}

static const ImWchar ranges[ ] =
{
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x0400, 0x044F, // Cyrillic
	0x0100, 0x017F, // Latin Extended-A
	0x0180, 0x024F, // Latin Extended-B
	0x2000, 0x206F, // General Punctuation
	0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
	0x31F0, 0x31FF, // Katakana Phonetic Extensions
	0xFF00, 0xFFEF, // Half-width characters
	0x4e00, 0x9FAF, // CJK Ideograms
	0,
};
namespace d3d_vtable {
	ID3D11Device* d3d11_device = nullptr;
	ID3D11DeviceContext* d3d11_device_context = nullptr;
	ID3D11RenderTargetView* main_render_target_view = nullptr;
	IDXGISwapChain* p_swap_chain = nullptr;
	void create_render_target( ) {
		ID3D11Texture2D* back_buffer;
		p_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
		d3d11_device->CreateRenderTargetView( back_buffer, NULL, &main_render_target_view );
		back_buffer->Release( );
	}

	void init_imgui( void* device, bool is_d3d11 = false ) {
		skin_database::load( );

		ImGui::CreateContext( );
		ImGui::StyleColorsDark( );

		ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		std::string font_path;
		if ( get_system_font_path( "Courier", font_path ) )
			ImGui::GetIO( ).Fonts->AddFontFromFileTTF( font_path.c_str( ), 14, 0, ranges );

		ImGui_ImplWin32_Init( *reinterpret_cast<HWND*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::Riot__g_window ) );

		if ( is_d3d11 ) {

			p_swap_chain = reinterpret_cast<IDXGISwapChain*>( device );

			p_swap_chain->GetDevice( __uuidof( d3d11_device ), reinterpret_cast<void**>( &( d3d11_device ) ) );

			d3d11_device->GetImmediateContext( &d3d11_device_context );

			create_render_target( );

			ImGui_ImplDX11_Init( d3d11_device, d3d11_device_context );
			ImGui_ImplDX11_CreateDeviceObjects( );

		} else
			ImGui_ImplDX9_Init( reinterpret_cast<IDirect3DDevice9*>( device ) );

		original_wndproc = *reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc );
		*reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc ) = wnd_proc;
	}

	void render( bool is_d3d11 = false ) {
		auto client = *reinterpret_cast<GameClient**>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GameClient );

		if ( client && client->game_state == GGameState_s::Running ) {
			skin_changer::update( );

			if ( menu_is_open ) {
				if ( is_d3d11 )
					ImGui_ImplDX11_NewFrame( );
				else
					ImGui_ImplDX9_NewFrame( );
				ImGui_ImplWin32_NewFrame( );
				ImGui::NewFrame( );

				menu::draw( );

				ImGui::EndFrame( );
				ImGui::Render( );

				if ( is_d3d11 ) {
					d3d11_device_context->OMSetRenderTargets( 1, &main_render_target_view, NULL );
					ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
				} else
					ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
			}
		}
	}

	struct dxgi_present {
		static long __stdcall hooked( IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags ) {
			std::call_once( init_device, [ & ] ( ) {

				init_imgui( p_swap_chain, true );
				} );

			render( true );

			return m_original( p_swap_chain, sync_interval, flags );
		}

		static decltype( &hooked ) m_original;
	};
	decltype( dxgi_present::m_original ) dxgi_present::m_original;

	struct dxgi_resize_buffers {
		static long __stdcall hooked( IDXGISwapChain* p_swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags ) {
			if ( main_render_target_view ) { main_render_target_view->Release( ); main_render_target_view = nullptr; }
			auto hr = m_original( p_swap_chain, buffer_count, width, height, new_format, swap_chain_flags );
			create_render_target( );
			return hr;
		}

		static decltype( &hooked ) m_original;
	};
	decltype( dxgi_resize_buffers::m_original ) dxgi_resize_buffers::m_original;

	struct end_scene {
		static long __stdcall hooked( IDirect3DDevice9* p_device ) {
			std::call_once( init_device, [ & ] ( ) {
				init_imgui( p_device );
				} );

			render( );

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

void d3d_hook::hook( ) {
	auto material_registry = reinterpret_cast<uintptr_t( __stdcall* )( )>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::functions::Riot__Renderer__MaterialRegistry__GetSingletonPtr )( );
	auto d3d_device = *reinterpret_cast<IDirect3DDevice9**>( material_registry + offsets::MaterialRegistry::D3DDevice );
	auto swap_chain = *reinterpret_cast<IDXGISwapChain**>( material_registry + offsets::MaterialRegistry::SwapChain );

	if ( d3d_device ) {
		d3d_device_vmt = std::make_unique<::vmt_smart_hook>( d3d_device );
		d3d_device_vmt->apply_hook<end_scene>( 42 );
		d3d_device_vmt->apply_hook<reset>( 16 );
	} else if ( swap_chain ) {
		swap_chain_vmt = std::make_unique<::vmt_smart_hook>( swap_chain );
		swap_chain_vmt->apply_hook<dxgi_present>( 8 );
		swap_chain_vmt->apply_hook<dxgi_resize_buffers>( 13 );
	}
}

void d3d_hook::unhook( ) {
	*reinterpret_cast<riot_wndproc*>( std::uintptr_t( GetModuleHandle( nullptr ) ) + offsets::global::GfxWinMsgProc ) = original_wndproc;
	if ( d3d_device_vmt )
		d3d_device_vmt->unhook( );
	if ( swap_chain_vmt )
		swap_chain_vmt->unhook( );
}
