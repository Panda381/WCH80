
// ****************************************************************************
//
//                                Variables
//
// ****************************************************************************

#ifndef _VAR_H
#define _VAR_H

#ifdef __cplusplus
extern "C" {
#endif

// CLEAR variables (no error)
void CmdClear();

// move to next variable (returns next variable) (no error)
u8* NextVar(u8* p);

// DIM command - create integer array; does not check if array exists (can raise errors)
//  LastResult = max. subscript index, VarDest points to array name (character A..Z).
void CmdDim();

// Store string variable, terminated with quote; does not check if variable exists
//	(p = pointer to string immediately after first quote ") (can raise errors)
//  VarDest points to string name, character A..Z.
void SetStr(u8* p);

// STR$ function - convert number to string (to Edit buffer; can raise errors)
//  Input 'num' and output 'str' to LastResult.
void FncStr();

// CHR$ function - convert character code to 1-character string (in Edit buffer, can raise errors)
//  Input 'num' and output 'str' to LastResult.
void FncChr();

// INKEY$ function - get key from keyboard (in Edit buffer,
//	save pointer to result to LastResult) (can raise errors)
void FncInkey();

// TL$ function - returns tail of string, without 1st character (no error)
//  Input 'str' and output 'str' to LastResult.
void FncStrTl();

// LET command (VarDest = variable name, LastResult = last result) (can raise errors)
void CmdLet();

// find variable and load its value (p = pointer to name of variable; returns pointer
// to data or NULL if not found) (can raise errors ERR_SUBSCRIPT or ERR_VARNFND)
//  var = to get pointer to variable, or NULL = not required
u8* GetVar(u8* p, u8** var);

// check LookVars state
//  req ... require numeric or string result - use FLAG_NUMRES or 0
//  set ... set numeric or string result - use FLAG_NUMRES or 0
// output: True = exit, False = continue
Bool CheckLookVars(int req, int set);

// scan variable or function (CharPtr = pointer to variable name;
//	returns result in LastResult) (can raise errors)
//  fncok ... functions are allowed
// - Called during syntax checking from CLASS5.
void LookVars(Bool fncok);

// PEEK function (no error)
// - Input and output 'num' to LastResult.
void FncPeek();

#ifdef __cplusplus
}
#endif

#endif // _VAR_H
