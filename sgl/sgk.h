/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_KEY_H
#define SG_KEY_H

typedef enum{
    SGK_LEFT=0x1,
    SGK_RIGHT=0x2,
    SGK_UP=0x3,
    SGK_DOWN=0x4,
    SGK_HOME=0x5,
    SGK_END=0x6,
    SGK_INS=0x7,
    SGK_BCSP='\b',
    SGK_TAB='\t',
    SGK_ENTER='\n',
    SGK_PGU,
    SGK_PGD,
    SGK_F1,
    SGK_F2,
    SGK_F3,
    SGK_F4,
    SGK_F5,
    SGK_F6,
    SGK_F7,
    SGK_F8,
    SGK_F9,
    SGK_F10,
    SGK_F11,
    SGK_F12,
    SGK_ESC=0x1B,
    SGK_SPACE=' ',
    SGK_DEL=0x7F,
    SGK_LB=1<<8,
    SGK_RB=1<<9,
    SGK_MB=1<<10,
    SGK_CTRL=1<<11,
    SGK_ALT=1<<12,
    SGK_SHIFT=1<<13,
}SGK;

#endif /* SG_KEY_H */
