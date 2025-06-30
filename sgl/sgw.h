/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_WINDOW_H
#define SG_WINDOW_H

#include <stddef.h>
#include "sgc.h"
#include "sgk.h"

enum SGE{
    SGE_CLOSE=0,
    SGE_NONE,
    SGE_TIMEOUT,
    SGE_RECTANGLE, /* SGW.rectangle */
    SGE_ASYNC,     /* SGE.rectangle */
    SGE_PRESS,     /* SGE.key */
    SGE_RELEASE,   /* SGE.key */
    SGE_CURSOR,    /* SGW.cursor */
    SGE_SCROLL,    /* SGE.scroll */
    SGE_UNKNOWN
};


typedef union{

    void *async;

    SGK key;

    enum{
        SGE_SCROLL_UP=-1,
        SGE_SCROLL_DOWN=1
    }scroll;

}SGE;


typedef struct _sgw{

    const char *title;

    SGC *pixel;

    void*(*allocator)(size_t);
    void(*deallocator)(void*);

    SGK keys;

    struct{
        int x,y;
        unsigned int w,h;
    }rectangle;

    struct{
        int x,y;
    }cursor;

    unsigned char bitness;

}const SGW;


SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*));

SGC *sgw_pixel(SGW *w,unsigned int x,unsigned int y);

void sgw_close(SGW *w);

void sgw_render(SGW *w);

void sgw_fill(SGW *w,SGC c);

void sgw_async(SGW *w,const void *p);

void sgw_title(SGW *w,const char *title);

void sgw_rect(SGW *w,int x,int y,unsigned int width,unsigned int height);

enum SGE sgw_event(SGW *w,int t,SGE *e);


#endif /* SG_WINDOW_H */
