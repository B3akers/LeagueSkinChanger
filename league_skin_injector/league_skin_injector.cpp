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
#include <iostream>
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include <tlhelp32.h>
#include <psapi.h>

std::vector<uint32_t> find_processes( std::wstring name )
{
	std::vector<uint32_t> list;

	HANDLE process_snap;
	PROCESSENTRY32 pe32;
	process_snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( process_snap == INVALID_HANDLE_VALUE )
		return list;

	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if ( Process32First( process_snap, &pe32 ) )
	{
		if ( pe32.szExeFile == name )
			list.push_back( pe32.th32ProcessID );

		while ( Process32Next( process_snap, &pe32 ) )
		{
			if ( pe32.szExeFile == name )
				list.push_back( pe32.th32ProcessID );
		}
	}

	CloseHandle( process_snap );

	return list;
}

using namespace std::chrono_literals;

bool is_injected( uint32_t pid )
{
	HMODULE hMods[ 1024 ];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, pid );

	if ( NULL == hProcess )
		return false;

	if ( EnumProcessModules( hProcess, hMods, sizeof( hMods ), &cbNeeded ) )
	{
		for ( i = 0; i < ( cbNeeded / sizeof( HMODULE ) ); i++ )
		{
			TCHAR szModName[ MAX_PATH ];
			if ( GetModuleBaseName( hProcess, hMods[ i ], szModName,
				sizeof( szModName ) / sizeof( TCHAR ) ) )
			{

				if ( wcscmp( szModName, L"league_skin_changer.dll" ) == 0 )
				{
					CloseHandle( hProcess );
					return true;
				}
			}
		}
	}

	CloseHandle( hProcess );

	return false;
}

void enable_debug_privilege( )
{
	HANDLE token;
	LUID value;
	TOKEN_PRIVILEGES tp;
	if ( OpenProcessToken( GetCurrentProcess( ), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token ) )
	{
		if ( LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &value ) )
		{
			tp.PrivilegeCount = 1;
			tp.Privileges[ 0 ].Luid = value;
			tp.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
			if ( AdjustTokenPrivileges( token, FALSE, &tp, sizeof( tp ), NULL, NULL ) )
				CloseHandle( token );
		}
	}
}

bool inject( uint32_t pid )
{
	TCHAR current_dir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, current_dir );

	auto dll_path = std::wstring( current_dir ) + L"\\league_skin_changer.dll";

	auto handle = OpenProcess( PROCESS_ALL_ACCESS, false, pid );

	if ( !handle || handle == INVALID_HANDLE_VALUE )
	{
		printf( "[-] Failed to open league process!\n" );
		return false;
	}

	FILETIME ft;
	SYSTEMTIME st;
	GetSystemTime( &st );
	SystemTimeToFileTime( &st, &ft );

	FILETIME create, exit, kernel, user;
	GetProcessTimes( handle, &create, &exit, &kernel, &user );

	auto delta = 10 - static_cast< int32_t >( ( *reinterpret_cast< uint64_t* >( &ft ) - *reinterpret_cast< uint64_t* >( &create.dwLowDateTime ) ) / 10000000U );
	if ( delta > 0 )
		std::this_thread::sleep_for( std::chrono::seconds( delta ) );

	auto dll_path_remote = VirtualAllocEx( handle, nullptr, ( dll_path.size( ) + 1 ) * sizeof( wchar_t ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

	if ( !dll_path_remote )
	{
		printf( "[-] Failed to alloc space!\n" );
		CloseHandle( handle );
		return false;
	}

	if ( !WriteProcessMemory( handle, dll_path_remote, dll_path.data( ), ( dll_path.size( ) + 1 ) * sizeof( wchar_t ), nullptr ) )
	{
		printf( "[-] Failed to write memory!\n" );
		VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
		CloseHandle( handle );
		return false;
	}

	auto thread = CreateRemoteThread( handle, nullptr, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( GetProcAddress( LoadLibrary( L"kernel32.dll" ), "LoadLibraryW" ) ), dll_path_remote, 0, nullptr );
	if ( !thread || thread == INVALID_HANDLE_VALUE )
	{
		printf( "[-] Failed to create thread!\n" );
		VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
		CloseHandle( handle );
		return false;
	}

	WaitForSingleObject( thread, INFINITE );
	CloseHandle( thread );
	VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
	CloseHandle( handle );
	printf( "[+] Injected successfully!\n" );
	return true;
}

int main( )
{
	enable_debug_privilege( );

	printf( "[+] Looking for league of legends processes...\n" );
	while ( true )
	{
		auto league_processes = find_processes( L"League of Legends.exe" );
		for ( auto& pid : league_processes )
		{
			if ( !is_injected( pid ) )
				inject( pid );
		}
		std::this_thread::sleep_for( 5s );
	}
	return 0;
}