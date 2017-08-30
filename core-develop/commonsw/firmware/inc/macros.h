/*
 * Macros
 * Caltech Racing 2016
 */

#ifndef MACROS_H
#define MACROS_H

#define hiword(value) ( ( value >> 16 ) & 0xFFFF )
#define loword(value) ( value & 0xFFFF )
#define hibyte(value) ( ( value >> 8 ) & 0xFF )
#define lobyte(value) ( value & 0xFF )

#define getbit(a, k)  ( ( a[ k / 8 ] & ( 1 << ( k % 8 ) ) ) != 0 ) 
#define get8bit(a, k) ( ( getbit( a, k ) ) + \
                        ( getbit( a, ( k + 1 ) ) << 1 ) + \
                        ( getbit( a, ( k + 2 ) ) << 2 ) + \
                        ( getbit( a, ( k + 3 ) ) << 3 ) + \
                        ( getbit( a, ( k + 4 ) ) << 4 ) + \
                        ( getbit( a, ( k + 5 ) ) << 5 ) + \
                        ( getbit( a, ( k + 6 ) ) << 6 ) + \
                        ( getbit( a, ( k + 7 ) ) << 7 ) )

#endif /* MACROS_H */
