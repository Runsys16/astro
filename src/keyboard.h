#ifndef KEYBOARD_H
#define KEYBOARD_H  1
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#include "main.h"
//--------------------------------------------------------------------------------------------------------------------
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')
//--------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------
#ifndef KEYBOARD_CPP
void KeyboardFuncCtrlAlt( unsigned char key, int x, int y);
void KeyboardFuncCtrl( unsigned char key, int x, int y);
void KeyboardFuncAlt( unsigned char key, int x, int y);

void KeyboardFunc( unsigned char key, int x, int y);
void KeyboardUpFunc( unsigned char key, int x, int y);
void SpecialFunc( int key, int x, int y);
void SpecialUpFunc( int key, int x, int y);
#endif

#endif

