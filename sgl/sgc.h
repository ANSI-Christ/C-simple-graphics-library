/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_COLOR_H
#define SG_COLOR_H

typedef unsigned int SGC;

SGC SGC_RGB(unsigned char red,unsigned char green,unsigned char blue,...);
#define SGC_RGB(_r_,_g_,_b_,...)  ( ((__VA_ARGS__ +0)<<24) | (((unsigned char)(_r_))<<16) | (((unsigned char)(_g_))<<8) | (unsigned char)(_b_) )

enum{
    SGC_BLACK        = SGC_RGB(0, 0, 0),
    SGC_RED          = SGC_RGB(255, 0, 0),
    SGC_GREEN        = SGC_RGB(0, 255, 0),
    SGC_BLUE         = SGC_RGB(0, 0, 255),
    SGC_YELLOW       = SGC_RGB(255, 255, 0),
    SGC_ORANGE       = SGC_RGB(255, 128, 0),
    SGC_GREY         = SGC_RGB(128, 128, 128),
    SGC_DARK_GREY    = SGC_RGB(88, 88, 88),
    SGC_LIGHT_GREY   = SGC_RGB(195, 195, 195),
    SGC_CYAN         = SGC_RGB(0, 255, 255),
    SGC_MAGENTA      = SGC_RGB(255, 0, 255),
    SGC_PURPLE       = SGC_RGB(128, 0, 128),
    SGC_PINK         = SGC_RGB(255, 192, 203),
    SGC_LIME         = SGC_RGB(0, 255, 0),
    SGC_TEAL         = SGC_RGB(0, 128, 128),
    SGC_NAVY         = SGC_RGB(0, 0, 128),
    SGC_MAROON       = SGC_RGB(128, 0, 0),
    SGC_OLIVE        = SGC_RGB(128, 128, 0),
    SGC_SILVER       = SGC_RGB(192, 192, 192),
    SGC_GOLD         = SGC_RGB(255, 215, 0),
    SGC_INDIGO       = SGC_RGB(75, 0, 130),
    SGC_VIOLET       = SGC_RGB(238, 130, 238),
    SGC_CORAL        = SGC_RGB(255, 127, 80),
    SGC_SALMON       = SGC_RGB(250, 128, 114),
    SGC_TURQUOISE    = SGC_RGB(64, 224, 208),
    SGC_BROWN        = SGC_RGB(165, 42, 42),
    SGC_BEIGE        = SGC_RGB(245, 245, 220),
    SGC_LAVENDER     = SGC_RGB(230, 230, 250),
    SGC_WHITE        = SGC_RGB(255, 255, 255),
};

#endif /* SG_COLOR_H */
