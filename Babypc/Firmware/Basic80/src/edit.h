
// ****************************************************************************
//
//                                  Edit
//
// ****************************************************************************

#ifndef _EDIT_H
#define _EDIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Delete whole edit line (no error)
void ClrEdit();

// INPUT command
void CmdInput();

// add BASIC line to the program (CharPtr = start of program line;
// does not raise error - ignores command on memory overflow; line must not be 0)
void MainAdd(u16 line);

// edit (flag FLAG_INPUT determines if inputting data or editing code) (raise errors)
void DoEdit();

#ifdef __cplusplus
}
#endif

#endif // _EDIT_H
