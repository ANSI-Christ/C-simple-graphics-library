/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_UI_H
#define SG_UI_H

#include "class.h"

#include "sgk.h"
#include "sgp.h"

typedef struct{

    enum{
        SGI_PRESS = 1,
        SGI_RELEASE = 2,
        SGI_SCROLL = 4,
        SGI_CURSOR = 8
    }event;

    SGK key;

    struct{
        int x,y;
    }cursor;

    enum{
        SGI_UP = -1,
        SGI_DOWN = 1,
    }scroll;

}SGU;

#define CLASS_BEGIN__SGU_WIDGET \
    constructor(void *parent)(\
        self->visible=self->able=1;\
    ),\
    public(\
        struct{\
            void (*draw)(void *self);\
            void (*update)(void *self);\
            void (*select)(void *self);\
            void (*insert)(void *other);\
            void (*input)(void *self,SGU *input);\
        }hook;\
        CLASS SGU_UI * const ui;\
        CLASS SGU_WIDGET * const parent;\
        void (*onInput)(void *self,SGU *input);\
        int x,y;\
        int width,height;\
        unsigned char able:1;\
        unsigned char visible:1;\
        unsigned char movable:1;\
        unsigned char resizable:1;\
        unsigned char detachable:1;\
    ),\
    private(\
        CLASS SGU_WIDGET *child,*prev,*next,*last;\
        SGM m;\
    )
CLASS_END(SGU_WIDGET);




#endif /* SG_UI_H */
