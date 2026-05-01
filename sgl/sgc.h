/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_COLOR_H
#define SG_COLOR_H

typedef struct{
    unsigned char r, g, b;
}SGC;

static SGC SGC_RGB(const unsigned char r,const unsigned char g,const unsigned char b){
    const SGC c={r,g,b}; return c;
}

#define SGC_BLACK        SGC_RGB(0, 0, 0)
#define SGC_RED          SGC_RGB(255, 0, 0)
#define SGC_GREEN        SGC_RGB(0, 255, 0)
#define SGC_BLUE         SGC_RGB(0, 0, 255)
#define SGC_YELLOW       SGC_RGB(255, 255, 0)
#define SGC_ORANGE       SGC_RGB(255, 128, 0)
#define SGC_GREY         SGC_RGB(128, 128, 128)
#define SGC_DARK_GREY    SGC_RGB(88, 88, 88)
#define SGC_LIGHT_GREY   SGC_RGB(195, 195, 195)
#define SGC_CYAN         SGC_RGB(0, 255, 255)
#define SGC_MAGENTA      SGC_RGB(255, 0, 255)
#define SGC_PURPLE       SGC_RGB(128, 0, 128)
#define SGC_PINK         SGC_RGB(255, 192, 203)
#define SGC_LIME         SGC_RGB(0, 255, 0)
#define SGC_TEAL         SGC_RGB(0, 128, 128)
#define SGC_NAVY         SGC_RGB(0, 0, 128)
#define SGC_MAROON       SGC_RGB(128, 0, 0)
#define SGC_OLIVE        SGC_RGB(128, 128, 0)
#define SGC_SILVER       SGC_RGB(192, 192, 192)
#define SGC_GOLD         SGC_RGB(255, 215, 0)
#define SGC_INDIGO       SGC_RGB(75, 0, 130)
#define SGC_VIOLET       SGC_RGB(238, 130, 238)
#define SGC_CORAL        SGC_RGB(255, 127, 80)
#define SGC_SALMON       SGC_RGB(250, 128, 114)
#define SGC_TURQUOISE    SGC_RGB(64, 224, 208)
#define SGC_BROWN        SGC_RGB(165, 42, 42)
#define SGC_BEIGE        SGC_RGB(245, 245, 220)
#define SGC_LAVENDER     SGC_RGB(230, 230, 250)
#define SGC_WHITE        SGC_RGB(255, 255, 255)

#endif /* SG_COLOR_H */
