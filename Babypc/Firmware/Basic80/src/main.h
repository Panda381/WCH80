
#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// Get character (Remap keys to ZX-80 characters; returns CH_NONE of no character)
u8 CharGet();

#if DODEBUG
void DebCh(u8 ch);
void Deb(int k);
#endif

#ifdef __cplusplus
}
#endif

#endif // _MAIN_H
