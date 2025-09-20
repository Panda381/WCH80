
// ****************************************************************************
//
//                                 Save program
//
// ****************************************************************************

#ifndef _SAVE_H
#define _SAVE_H

#ifdef __cplusplus
extern "C" {
#endif

#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
extern const u8 SlotData[SLOTDATANUM];
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
// default program slot 0
extern const u8 Slot0[SLOT0NUM];
#endif

// Slot0 base address - use this address for programming and reading Slot0 to avoid memory caching.
#define SLOTDATA ((const u8*)((u32)SlotData + FLASH_BASE))
#define SLOT0 ((const u8*)((u32)Slot0 + FLASH_BASE))

// clear unused space of program
void ClearFreeProg();

// clear unused space of variables
void ClearFreeVars();

// command Save
void CmdSave();

// command Load
void CmdLoad();

#ifdef __cplusplus
}
#endif

#endif // _SAVE_H
