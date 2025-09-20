
// ****************************************************************************
//
//                                 Save program
//
// ****************************************************************************

#include "def.h"

	.section	.text

#if USE_SAVE_ASM		// 1=use ASM in Save module

// ----------------------------------------------------------------------------
// clear unused space of program
// ----------------------------------------------------------------------------

.global ClearFreeProg
ClearFreeProg:
	lhu	a0,RAM_PROGNUM_OFF(gp)	// A0 <- program size ProgNum
	li	a2,0xff			// A2 <- byte to clear
	addi	a3,gp,RAM_PROG_OFF	// A3 <- program buffer Prog[0]
	addi	a4,a3,PROGNUM		// A4 <- max. end of program buffer &Prog[PROGNUM]
ClearFreeProg_Clr:
	add	a1,a0,a3		// A1 <- pointer to end of data 'p'
	bge	a1,a4,8f		// exit of buffer is full
1:	sb	a2,0(a1)		// clear one byte
	addi	a1,a1,1			// A1 <- increase pointer
	bne	a1,a4,1b		// next byte
8:	ret

// ----------------------------------------------------------------------------
// clear unused space of variables
// ----------------------------------------------------------------------------

.global ClearFreeVars
ClearFreeVars:
	lhu	a0,RAM_VARSNUM_OFF(gp)	// A0 <- variables size VarsNum
	li	a2,VAR_END		// A2 <- byte to clear
	addi	a3,gp,RAM_VARS_OFF	// A3 <- variable buffer Vars[0]
	addi	a4,a3,VARSNUM		// A4 <- max. end of variable buffer &Vars[VARSNUM]
	j	ClearFreeProg_Clr

// ----------------------------------------------------------------------------
// command Save
// ----------------------------------------------------------------------------

.global CmdSave
CmdSave:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// stop running (no error report)
	sh	zero,RAM_PROGLINE_OFF(gp) // clear program line

	// check program slot
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	li	a1,SLOTMAX		// A1 <- max. slot number
	bgtu	a0,a1,9f		// exit if slot number is invalid

	// clear unused space of program
	call	ClearFreeProg

	// check slot 0
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	beqz	a0,5f			// skip if slot is 0

// --- save to CPU2 (slot 1..11)

	// lock CPU communication
	call	CPU_IntLock

	// send request
1:	call	CPU_SendReady		// sender ready?
	beqz	a0,1b			// wait for send ready
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	addi	a0,a0,CPU_CMD_SAVE1 - 1	// prepare command
	call	CPU_Send		// send command

	// flush receiver
	call	CPU_RecvReady		// receiver ready?
	beqz	a0,1f			// not ready
	call	CPU_Recv		// destroy received character

	// save data
1:	li	s0,0			// S0 <- index
2:	call	CPU_SendReady		// sender ready?
	beqz	a0,2b			// wait for send ready
	addi	a1,gp,RAM_PROG_OFF	// A1 <- program buffer Prog
	add	a1,a1,s0		// A1 <- pointer to program buffer
	lbu	a0,0(a1)		// A0 <- byte to send
	addi	s0,s0,1			// increment data index
	call	CPU_Send		// send byte
	li	a0,50			// A0 <- wait time 50 us
	call	WaitUs			// short delay
	li	a1,SLOTNUM		// A1 <- slot size
	blt	s0,a1,2b		// next byte

	// wait for end of flashing
1:	call	CPU_RecvReady		// receiver ready?
	beqz	a0,1b			// wait for receiver

	// synchronize
	call	CPU_SyncInit

	// unlock CPU communication
	call	CPU_IntUnlock
	j	9f

// --- save to CPU1 (slot 0)

5:
#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
	// clear unused space of variables
	call	ClearFreeVars

	// clear flash slot (address and size must be multiply of 256 B)
	la	a0,SlotData + FLASH_BASE // A0 <- SlotData address
	li	a1,SLOTDATANUM		// A1 <- SlotData size
	li	a2,1000			// A2 <- timeout
	call	Flash_Erase		// flash erase

	// write data
	la	a0,SlotData + FLASH_BASE // A0 <- SlotData address
	addi	a1,gp,RAM_VARS_OFF	// A1 <- variable buffer
	li	a2,SLOTDATANUM		// A2 <- SlotData size
	li	a3,1000			// A3 <- timeout
	call	Flash_Program		// flash program
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
	// clear flash slot (address and size must be multiply of 256 B)
	la	a0,Slot0 + FLASH_BASE	// A0 <- Slot0 address
	li	a1,SLOT0NUM		// A1 <- Slot0 size
	li	a2,1000			// A2 <- timeout
	call	Flash_Erase		// flash erase

	// write program
	la	a0,Slot0 + FLASH_BASE	// A0 <- Slot0 address
	addi	a1,gp,RAM_PROG_OFF	// A1 <- program buffer
	li	a2,SLOT0NUM		// A2 <- Slot0 size
	li	a3,1000			// A3 <- timeout
	call	Flash_Program		// flash program
#endif

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

// ----------------------------------------------------------------------------
// command Load
// ----------------------------------------------------------------------------

.global CmdLoad
CmdLoad:
	// push registers
	addi	sp,sp,-8
	sw	ra,0(sp)
	sw	s0,4(sp)

	// stop running (no error report)
	sh	zero,RAM_PROGLINE_OFF(gp) // clear program line

	// check program slot
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	li	a1,SLOTMAX		// A1 <- max. slot number
	bgtu	a0,a1,9f		// exit if slot number is invalid

	// clear program
	li	a0,2
	sh	a0,RAM_PROGNUM_OFF(gp)	// set ProgNum to 2
	sw	zero,RAM_PROG_OFF(gp)	// set Prog[0] and Prog[1] to 0 (= line number of last row = 0; this address is aligned OK)
	call	ClearFreeProg

	// check slot 0
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	beqz	a0,3f			// skip if slot 0

// --- load from CPU2 (slot 1..11)

	// lock CPU communication
	call	CPU_IntLock

	// send request
1:	call	CPU_SendReady		// sender ready?
	beqz	a0,1b			// wait for send ready
	lw	a0,RAM_LASTRESULT_OFF(gp) // A0 <- program slot from LastResult
	addi	a0,a0,CPU_CMD_LOAD1 - 1	// prepare command
	call	CPU_Send		// send command

	// load data
	li	s0,0			// S0 <- index
2:	call	CPU_RecvReady		// receiver ready?
	beqz	a0,2b			// wait for receiver ready
	call	CPU_Recv		// receive byte
	addi	a1,gp,RAM_PROG_OFF	// A1 <- program buffer Prog
	add	a1,a1,s0		// A1 <- pointer to program buffer
	sb	a0,0(a1)		// write byte
	addi	s0,s0,1			// increment data index
	li	a1,SLOTNUM		// A1 <- slot size
	blt	s0,a1,2b		// next byte

	// unlock CPU communication
	call	CPU_IntUnlock
	j	4f

// --- load from CPU1 (slot 0)

3:
#if SLOTDATANUM > 0	// size of data slot, multiply of 256, 0=do not use
	// clear variables
	call	CmdClear		// clear variables
	call	ClearFreeVars		// clear unused space of variables

	// load variables
	addi	a0,gp,RAM_VARS_OFF	// A0 <- variable buffer
	la	a1,SlotData + FLASH_BASE // A1 <- SlotData address
	li	a2,SLOTDATANUM		// A2 <- SlotData size
	call	memcpy			// copy variables

	// emergency mark end of variables
#if RAM_VARS_OFF+SLOTDATANUM-1 >= 2048
	la	a5,Vars + SLOTDATANUM-1	// A5 <- end of variable buffer
#else
	addi	a5,gp,RAM_VARS_OFF+SLOTDATANUM-1 // A5 <- end of variable buffer
#endif
	li	a4,CH_QUOT
	sb	a4,0(a5)		// emergency mark end of variables

	// find end of variables
	addi	a0,gp,RAM_VARS_OFF	// A0 <- pointer to variables Vars 'p'
	mv	s0,a0			// S0 <- last valid variable 'p0'
1:	lbu	a1,0(a0)		// A1 <- get variable type
	li	a2,VAR_END		// A2 <- end mark
	beq	a1,a2,3f		// break on end mark
#if RAM_VARS_OFF+SLOTDATANUM-1 >= 2048
	la	a5,Vars + SLOTDATANUM-1	// A5 <- end of variable buffer
#else
	addi	a5,gp,RAM_VARS_OFF+SLOTDATANUM-1 // A5 <- end of variable buffer
#endif
	bge	a0,a5,2f		// break if end of variables reached
	mv	s0,a0			// S0 <- last valid variable 'p0'
	call	NextVar			// find next variable
	j	1b

2:	mv	a0,s0			// use last valid variable
3:	sb	a2,0(a0)		// mark end of variables
	addi	a1,gp,RAM_VARS_OFF-1	// A1 <- pointer to variables Vars - 1
	sub	a0,a0,a1		// A0 <- number of bytes
	sh	a0,RAM_VARSNUM_OFF(gp)	// save new length of variables VarsNum
#endif

#if SLOT0NUM > 0	// save slot 0 size as multiply of 256, 0=do not use (use smaller value than SLOTNUM only during debug!)
	// load program from slot 0
	// - For the Slot0 address, we must use an alias with the FLASH_BASE base
	//   so that the cache does not apply after previous programming.
	addi	a0,gp,RAM_PROG_OFF	// A0 <- program buffer
	la	a1,Slot0 + FLASH_BASE	// A1 <- Slot0 address
	li	a2,SLOT0NUM		// A2 <- Slot0 size
	call	memcpy			// copy program
#else
	j	9f
#endif

// --- process program - get program size

	// pointer to program buffer
4:	addi	s0,gp,RAM_PROG_OFF	// S0 <- program buffer Prog 'p'
	mv	a5,s0			// A5 <- last valid line 'p0'
	li	a2,0xffff		// A2 <- invalid program line
	li	a3,NEWLINE		// A3 <- new line terminator
	addi	a4,s0,PROGNUM - 2	// A4 <- end of program buffer &Prog[PROGNUM-2]

	// check end of program
5:	lbu	a1,1(s0)		// A1 <- program line HIGH
	lbu	a0,0(s0)		// A0 <- program line LOW
	slli	a1,a1,8			// A1 <- program line HIGH << 8
	or	a0,a0,a1		// A0 <- program line
	beqz	a0,8f			// break on program end (line = 0)

	// invalid program line (= empty flash memory)
	beq	a0,a2,7f		// break and reset if program is not valid
	mv	a5,s0			// A5 <- last valid line 'p0'

	// find next line
	addi	s0,s0,2			// S0 <- skip line number
6:	lbu	a0,0(s0)		// A0 <- load byte
	addi	s0,s0,1			// increase pointer
	beq	a0,a3,1f		// break on NEWLINE
	bleu	s0,a4,6b		// next byte if not end of buffer

	// check buffer overflow
1:	bleu	s0,a4,5b		// continue loop if OK

	// use last valid line on error
7:	mv	s0,a5			// S0 <- last valid line

	// mark end of program
8:	sb	zero,0(s0)		// mark line 0 LOW
	sb	zero,1(s0)		// mark line 0 HIGH

	// set program length
	addi	a0,gp,RAM_PROG_OFF-2	// A0 <- program buffer - 2
	sub	a0,s0,a0		// A0 <- program length
	sh	a0,RAM_PROGNUM_OFF(gp)	// set program size

	// reset current line
	li	a0,1
	sh	a0,RAM_CURLINE_OFF(gp)	// set current line
	sh	a0,RAM_TOPLINE_OFF(gp)	// set top line

	// pop registers
9:	lw	s0,4(sp)
	lw	ra,0(sp)
	addi	sp,sp,8
	ret

#endif // USE_SAVE_ASM
