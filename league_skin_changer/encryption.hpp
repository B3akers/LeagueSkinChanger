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
#include <inttypes.h>
#include <intrin.h>

#pragma pack( push, 4 )
template <typename t>
class xor_value {
	bool xor_key_was_init = 0;
	unsigned char bytes_xor_count;
	unsigned char bytes_xor_count_8;
	t xor_key;
	unsigned char value_index = 0;
	t values_table[ 4 ];
public:
	t decrypt( ) {
		if ( xor_key_was_init != 1 )
			return 0;

		auto xored_value = values_table[ value_index ];
		auto xor_key_value = xor_key;

		{
			auto xor_value_ptr = reinterpret_cast<uint32_t*>( &xor_key_value );
			for ( auto i = 0; i < bytes_xor_count; i++ )
				*( reinterpret_cast<uint32_t*>( &xored_value ) + i ) ^= ~xor_value_ptr[ i ];
		}

		{
			auto xor_value_ptr = reinterpret_cast<unsigned char*>( &xor_key_value );

			for ( auto i = sizeof( t ) - bytes_xor_count_8; i < sizeof( t ); ++i )
				*( reinterpret_cast<unsigned char*>( &xored_value ) + i ) ^= ~xor_value_ptr[ i ];
		}

		return xored_value;
	}

	void encrypt( t value ) {
		if ( !xor_key_was_init ) {
			if ( sizeof( t ) <= 2 ) {
				bytes_xor_count_8 = sizeof( t );
				bytes_xor_count = 0;
			} else {
				bytes_xor_count_8 = sizeof( t ) % 4;
				bytes_xor_count = ( sizeof( t ) - bytes_xor_count_8 ) / 4;
			}

			auto key = __rdtsc( );
			auto key_index = 0;

			for ( auto i = 0; i < sizeof( t ); i++ ) {
				*( reinterpret_cast<unsigned char*>( &xor_key ) + i ) = *( reinterpret_cast<unsigned char*>( &key ) + key_index++ );

				if ( key_index == 8 ) {
					key = __rdtsc( );
					key_index = 0;
				}
			}

			value_index = 0;
			xor_key_was_init = 1;
		}

		auto xored_value = value;
		auto xor_key_value = xor_key;

		{
			auto xor_value_ptr = reinterpret_cast<uint32_t*>( &xor_key_value );
			for ( auto i = 0; i < bytes_xor_count; i++ )
				*( reinterpret_cast<uint32_t*>( &xored_value ) + i ) ^= ~xor_value_ptr[ i ];
		}

		{
			auto xor_value_ptr = reinterpret_cast<unsigned char*>( &xor_key_value );

			for ( auto i = sizeof( t ) - bytes_xor_count_8; i < sizeof( t ); ++i )
				*( reinterpret_cast<unsigned char*>( &xored_value ) + i ) ^= ~xor_value_ptr[ i ];
		}

		auto new_value_index = uint8_t( value_index + 1 ) & 3;

		values_table[ new_value_index ] = xored_value;

		value_index = new_value_index;
	}
};
#pragma pack( pop )