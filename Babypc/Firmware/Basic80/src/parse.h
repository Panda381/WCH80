
// ****************************************************************************
//
//                             Parse
//
// ****************************************************************************

#ifndef _PARSE_H
#define _PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

// check alphabetic character 'A'..'Z'
Bool IsAlpha(u8 ch);

// check alphanumeric character '0'..'9' or 'A'..'Z'
Bool IsAlphaNum(u8 ch);

// check alphanumericdot character '.' or '0'..'9' or 'A'..'Z'
Bool IsAlphaNumDot(u8 ch);

// parse variable name
//  Returns:
//    bit 0..5: length of the name (1 to 63)
//    bit 6..7: type of variable VARTYPE_*
// VARTYPE_NUM (=0) ... integer number 'name'
// VARTYPE_FOR (=1) ... function 'name$()'
// VARTYPE_ARRAY (=2) ... array or function 'name()'
// VARTYPE_STR (=3) ... string 'name$'
// VAR_END (0xc0) ... invalid variable
u8 ParseVarName(u8* p);

// parse function name (returns function index 0.., or -1=not function)
//  p ... pointer to name
//  flag ... type and length of the name, as returned from ParseVarName()
int ParseFnc(u8* p, u8 flag);

// increment pointer and get next character from program line or from edit line (no error)
u8 CharAdd();

// increment pointer and get next character from program line or from edit line, skipping spaces (no error)
u8 NextChar();

// get current character from program line or from edit line, skipping spaces (no error)
u8 GetChar();

// check end of command line (returns True = continue, False = only checking syntax, break) (can raise SyntaxError)
Bool CheckEnd();

// parse signed number (saves result to LastResult) (can raise Overflow error)
// Value VAL_MIN cannot be entered.
val ParseNum();

// parse unsigned number (saves result to LastResult; not checking overflow) (no error)
u32 ParseUNum();

#ifdef __cplusplus
}
#endif

#endif // _PARSE_H
