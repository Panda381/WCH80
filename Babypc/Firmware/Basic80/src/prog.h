
// ****************************************************************************
//
//                                Program
//
// ****************************************************************************

#ifndef _PROG_H
#define _PROG_H

#ifdef __cplusplus
extern "C" {
#endif

// LIST command (no error)
void CmdList();

// RETURN command (can raise errors)
void CmdReturn();

// FOR command (can raise errors)
// - Called during syntax checking from CLASS5.
void CmdFor();

// NEXT command (can raise errors)
void CmdNext();

// IF command (no error)
// - Called during syntax checking from CLASS5.
void CmdIf();

// REM command (no error)
// - Called during syntax checking from CLASS5.
void CmdRem();

// move to next program line (returns next program line) (no error)
//  lineL lineH text NEWLINE ... program line, 0 = end of program
u8* NextLine(u8* p);

// find program line by line number (returns required line or higher or end of program) (no error)
//   prevline ... returns previous line (if prevline != NULL), or NULL if this is first line
u8* FindLine(int line, u8** prevline);

// find currect edited program line (no error)
u8* FindCurLine();

// GOTO program line (no error)
void CmdGoto();

// CONTINUE program (no error)
void CmdContinue();

// RUN program (no error)
void CmdRun();

// GOSUB command (can raise errors)
void CmdGosub();

// Auto list (no error)
void AutoList();

// Process command or check syntax in edit line (can raise errors)
void MainGo(u8* p);

// POKE command
// - Called during syntax checking from CLASS5.
void CmdPoke();

// MEMORY command
void CmdMemory();

// BEEP command
void CmdBeep();

// WAIT command
void CmdWait();

// DATA command
void CmdData();

#ifdef __cplusplus
}
#endif

#endif // _PROG_H
