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
#include <tlhelp32.h>

std::vector<uint32_t> find_processes( std::wstring name ) {
	std::vector<uint32_t> list;

	HANDLE process_snap;
	PROCESSENTRY32 pe32;
	process_snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( process_snap == INVALID_HANDLE_VALUE )
		return list;

	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if ( Process32First( process_snap, &pe32 ) ) {
		if ( pe32.szExeFile == name )
			list.push_back( pe32.th32ProcessID );

		while ( Process32Next( process_snap, &pe32 ) ) {
			if ( pe32.szExeFile == name )
				list.push_back( pe32.th32ProcessID );
		}
	}

	CloseHandle( process_snap );

	return list;
}

int main( ) {
	TCHAR current_dir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, current_dir );

	auto dll_path = std::wstring( current_dir ) + L"\\league_skin_changer.dll";
	auto league_processes = find_processes( L"League of Legends.exe" );

	if ( league_processes.empty( ) ) {
		printf( "[-] Couldn't find league process!\n" );
		getchar( );
		return 0;
	}

	auto handle = OpenProcess( PROCESS_ALL_ACCESS, false, league_processes.front( ) );

	if ( !handle || handle == INVALID_HANDLE_VALUE ) {
		printf( "[-] Failed to open league process!\n" );
		getchar( );
		return 0;
	}

	auto dll_path_remote = VirtualAllocEx( handle, nullptr, ( dll_path.size( ) + 1 ) * sizeof( wchar_t ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

	if ( !dll_path_remote ) {
		printf( "[-] Failed to alloc space!\n" );
		CloseHandle( handle );
		getchar( );
		return 0;
	}

	if ( !WriteProcessMemory( handle, dll_path_remote, dll_path.data( ), ( dll_path.size( ) + 1 ) * sizeof( wchar_t ), nullptr ) ) {
		printf( "[-] Failed to write memory!\n" );
		VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
		CloseHandle( handle );
		getchar( );
		return 0;
	}

	auto thread = CreateRemoteThread( handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>( GetProcAddress( LoadLibrary( L"kernel32.dll" ), "LoadLibraryW" ) ), dll_path_remote, 0, nullptr );
	if ( !thread || thread == INVALID_HANDLE_VALUE ) {
		printf( "[-] Failed to create thread!\n" );
		VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
		CloseHandle( handle );
		getchar( );
		return 0;
	}

	WaitForSingleObject( thread, INFINITE );
	CloseHandle( thread );
	VirtualFreeEx( handle, dll_path_remote, 0, MEM_RELEASE );
	CloseHandle( handle );
	printf( "[+] Injected successfully!\n" );
	getchar( );
	return 0;
}