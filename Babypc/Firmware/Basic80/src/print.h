
// ****************************************************************************
//
//                             Print
//
// ****************************************************************************

#ifndef _PRINT_H
#define _PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

// command CLS - clear display (no error)
void CmdCls();

// print RAW character to display (returns False on display overflow)
Bool PrintCh(u8 ch);

// print space character (returns False on display overflow)
Bool PrintSpc();

// print space characters with count (returns False on display overflow)
Bool PrintSpcNum(int num);

// print spaces to position (returns False on display overflow)
// - Continues on the next line if the position is beyond the end of the line.
Bool PrintSpcTo(int pos);

// print tabulator (returns False on display overflow)
Bool PrintTab();

// print new line (returns False on display overflow)
Bool PrintCR();

// clear to end of display (no error)
void ClrEod();

// print character and enable leading spaces, if it is not a space (returns False on display overflow) (no error)
Bool PrintChLead(u8 ch);

// print token 0xD4..0xFF (returns False on display overflow) (no error)
Bool PrintToken(u8 ch);

// print string or token, terminated with CH_QUOT character (returns False on display overflow) (no error)
Bool PrintString(const char* txt);

// decode unsigned number into temporary buffer, with ending CH_QUOT character (returns start of the number) (no error)
u8* DecTempUNum(u32 num);

// decode number into temporary buffer, with ending CH_QUOT character (returns start of the number) (no error)
u8* DecTempNum(val num);

// print number (returns False on display overflow) (no error)
Bool PrintNum(val num);

// print BASIC line without line number (p = points after line number) (returns False on display overflow) (no error)
Bool PrintBas(const u8* p);

// print BASIC line with line number (p = points to line number) (returns False on display overflow) (no error)
Bool PrintBasNum(const u8* p);

// PRINT command (can raise errors)
// - Called during syntax checking from CLASS5.
void CmdPrint();

#ifdef __cplusplus
}
#endif

#endif // _PRINT_H
