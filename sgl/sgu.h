/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_UI_H
#define SG_UI_H

#include "sgk.h"
#include "sgp.h"
#include "./sgu/class.h"

#include <stddef.h>


void *sgu_select(void *widget);
void *sgu_insert(void *widget,void *parent);

char sgu_moved(const void *widget);
char sgu_resized(const void *widget);
char sgu_focused(const void *widget);
char sgu_selected(const void *widget);




typedef struct{

    enum SGU{
        SGU_PRESS   = 1<<0,
        SGU_RELEASE = 1<<1,
        SGU_SCROLL  = 1<<2,
        SGU_CURSOR  = 1<<3,
    }event;

    SGK key;

    struct{
        int x, y;
    }cursor;

    enum{
        SGI_UP = -1,
        SGI_DOWN = 1,
    }scroll;

}SGU;


#define CLASS_BEGIN___SGU_NODE \
    abstract,\
    constructor(void *parent)(),\
    public(\
        CLASS SGU_UI * const ui;\
        CLASS SGU_WIDGET * const parent;\
        void (*onInsert)(void*self,void *other);\
    ),\
    private(\
        CLASS _SGU_NODE *child,*prev,*next,*last;\
    )
CLASS_END(_SGU_NODE);


#define CLASS_BEGIN__SGU_WIDGET \
    extends(_SGU_NODE),\
    constructor(void *parent)(),\
    public(\
        void (*core)(void *self,SGU *event);\
        void (*onDraw)(void *self,const SGM*);\
        void (*onUpdate)(void *self);\
        void (*onSelect)(void *self);\
        void (*onEvent)(void *self,SGU *event);\
        int x,y;\
        unsigned int w,h;\
        unsigned char able:1;\
        unsigned char visible:1;\
        unsigned char movable:1;\
        unsigned char resizable:1;\
        unsigned char dNd:1;\
    ),\
    private(\
        SGM m;\
    )
CLASS_END(SGU_WIDGET);


#define CLASS_BEGIN__SGU_UI \
    extends(SGU_WIDGET),\
    constructor(unsigned int color_bytes,void*(*allocator)(size_t),void(*deallocator)(void*))(),\
    public(\
        void*(* const allocator)(size_t);\
        void(* const deallocator)(void*);\
        void(* const run)(void *self,void *pixels,unsigned int w,unsigned int h,const SGU *sgu);\
        void *userData;\
        const void* color;\
    ),\
    private(\
        void *focus, *select, *block;\
        unsigned int color_bytes;\
        int flags, dxy[2];\
    )
CLASS_END(SGU_UI);




#endif /* SG_UI_H */
