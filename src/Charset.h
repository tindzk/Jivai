#import "String.h"

/*
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * Taken from src/include/c.h
 */

#define Charset_HighBit 0x80

#define Charset_IsHighBitSet(c) \
	((unsigned char)(c) & Charset_HighBit)

String Charset_Latin1ToUTF8(String s);
