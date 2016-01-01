//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Common.h
//	Version:		0.01
//	Data:		2005.12.20
//
//	Author:		Liu, Zemin
//	Company:	zmLAB
//
//-----------------------------------------------------------------------------
//
// 	Target: 		ATmega64
// 	Tool chain: 	WinAvr (GCC-AVR)
//
//
//-----------------------------------------------------------------------------
//	Required files:
//
//-----------------------------------------------------------------------------
//	Notes:
//
//
//-----------------------------------------------------------------------------
//	Revision History:
//		V0.01		2005.12.20
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef Common_h

#define	Common_h

typedef	unsigned char		U8;
typedef	signed char		S8;
typedef	unsigned short int	U16;
typedef	signed short int	S16;
typedef	unsigned long		U32;
typedef	signed long		S32;

typedef	S16				sSIZE;

typedef	void	(*FuncPointer)(U8); 
typedef	void	(*CmdFuncPointer)(void); 

// -- Control debug code generation
//#define	_Debug_

//#define	AntiChange

// ============= Macro definitions ===========================
#define	BitSet(word, bit_mask)		((word) |= (bit_mask))
#define	BitClr(word, bit_mask)		((word) &= ~(bit_mask))
#define	BitTest(word, bit_mask)		((word) & (bit_mask))
#define	BitAnd(word, bit_mask)		((word) &= (bit_mask))
#define	BitOr(word, bit_mask)		((word) |= (bit_mask))
#define	BitXor(word, bit_mask)		((word) ^= (bit_mask))

/*
#define	BitSet8(word, bit_mask)		((word) |= (bit_mask))
#define	BitClr8(word, bit_mask)		((word) &= ~(bit_mask))
#define	BitTest8(word, bit_mask)		((word) & (bit_mask))
#define	BitAnd8(word, bit_mask)		((word) &= (bit_mask))
#define	BitOr8(word, bit_mask)		((word) |= (bit_mask))
#define	BitXor8(word, bit_mask)		((word) ^= (bit_mask))

#define	BitSet16(word, bit_mask)		((word) |= (bit_mask))
#define	BitClr16(word, bit_mask)		((word) &= ~(bit_mask))
#define	BitTest16(word, bit_mask)	((word) & (bit_mask))
#define	BitAnd16(word, bit_mask)	((word) &= (bit_mask))
#define	BitOr16(word, bit_mask)		((word) |= (bit_mask))
#define	BitXor16(word, bit_mask)		((word) ^= (bit_mask))
*/

// ===========================================================
//	Function Prototype Declarations
// ===========================================================
//
U8	Min(U8 a, U8 b);
U8	Max(U8 a, U8 b);
U8	*BinToHex8(U8 binary, U8 *Str);
U8 	*BinToDec8(U8 binary, U8 *Str);
U8 	*BinToDec16(U16 binary, U8 *Str);
U8 	*BinToStr16(U16 binary, U8 *Str);
U8 	*BinToDec32(U32 binary, U8 *Str);
void	Delay(U16 count);
void	InitDelay(volatile U16 count);

#endif // Common_h 

