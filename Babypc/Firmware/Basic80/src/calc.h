
// ****************************************************************************
//
//                        Calculate expression
//
// ****************************************************************************

#ifndef _CALC_H
#define _CALC_H

#ifdef __cplusplus
extern "C" {
#endif

// Fetch value (can raise error)
// - Called during syntax checking
// - On input, flags determine required result type.
void FetchVal();

// ABS function, absolute value (note: VAL_MIN stays unchanged, should be error) (no error)
// - Input and output to LastResult.
void FncAbs();

// negate value (note: VAL_MIN stays unchanged, should be error) (no error)
val NumNeg(val a);

// addition a + b (can raise Overflow error)
val NumAdd(val a, val b);

// subtraction a - b (can raise Overflow error)
val NumSub(val a, val b);

// multiplication a * b (can raise Overflow error)
val NumMul(val a, val b);

// division a / b (can raise Overflow error)
val NumDiv(val a, val b);

// power a ^ b (can raise Overflow error)
val NumPwr(val a, val b);

// bitwise AND a & b (no error)
val BitAND(val a, val b);

// bitwise OR a | b (no error)
val BitOR(val a, val b);

// compare a == b (no error)
val NumEqu(val a, val b);

// compare a > b (no error)
val NumGrt(val a, val b);

// compare a < b (no error)
val NumLes(val a, val b);

// string compare (strings are terminated with CH_QUOT character) (no error)
//  a,b ... string pointers
//  returns -1 if a<b, 0 if a==b, 1 if a>b
val StrCmp(uval a, uval b);

// compare string if equal a == b (no error)
//  a,b ... string pointers
val StrEqu(uval a, uval b);

// compare string if greater a > b (no error)
//  a,b ... string pointers
val StrGrt(uval a, uval b);

// compare string if less a < b (no error)
//  a,b ... string pointers
val StrLes(uval a, uval b);

// RND function - random number generator (number 1..max will be generated) (no error)
// - Input and output to LastResult.
void FncRnd();

// command RANDOMISE (no error)
void CmdRandomise();

// CODE function - get numeric code of 1st character in string (no error)
// - Input 'str' and output 'num' to LastResult.
void FncCode();

// store to calculator stack (can raise error)
// - Called during syntax checking from CLASS5.
void PushCalc(val num, u8 op, u8 prior);

// load from calculator stack (returns value) (can raise error)
// - Called during syntax checking from CLASS5.
val PopCalc(u8* op, u8* prior);

// evaluate expression, store result to LastResult (p = pointer to program line) (can raise error)
// - Called during syntax checking from CLASS5.
void EvalExp(u8* p);

// evaluate bracket expression (p = pointer to program line) (can raise error)
// - Called during syntax checking from CLASS5.
void Bracket(u8* p);

// scanning calculator expression, store result to LastResult (can raise error)
// - Called during syntax checking from CLASS5.
void ScanCalc();

// function READ, read data (returns result to LastResult)
void FncRead();

#ifdef __cplusplus
}
#endif

#endif // _CALC_H
