/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <stdarg.h>
#include <string.h>
#include "sgw.h"

SGC *sgw_pixel(SGW * const w,const unsigned int x,const unsigned int y){
    return w->pixel+y*w->rectangle.w+x;
}

void sgw_fill(SGW * const w,const SGC c){
    unsigned int i=w->rectangle.w*w->rectangle.h;
    while(i) w->pixel[--i]=c;
}

static void _sgc_convert(const SGC *c32,const unsigned int size,const unsigned char bits,void * const out){
    switch(bits){
        case 32: case 24:
            if(c32!=(const SGC*)out) memcpy(out,c32,size<<2);
            return;
        case 16:{
            unsigned int i=0;
            unsigned short * const p=(unsigned short*)out;
            for(;i<size;++i){
                const SGC c=c32[i];
                const unsigned char r=c>>16, g=c>>8, b=c;
                p[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            }
        } return;
        case 15:{
            unsigned int i=0;
            unsigned short * const p=(unsigned short*)out;
            for(;i<size;++i){
                const SGC c=c32[i];
                const unsigned char r=c>>16, g=c>>8, b=c;
                p[i] = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
            }
        } return;
        case 8:{
            unsigned int i=0;
            unsigned char * const p=(unsigned char*)out;
            for(;i<size;++i){
                const SGC c=c32[i];
                const unsigned char r=c>>16, g=c>>8, b=c;
                p[i] = (r+g+b)/3;
            }
        } return;
        case 1:{
            unsigned int i=0;
            unsigned char * const p=(unsigned char*)out;
            for(;i<size;++i){
                const SGC c=c32[i];
                const unsigned char r=c>>16, g=c>>8, b=c;
                p[i] = ((r+g+b)/3 > 128)*255;
            }
        } return;
    }
}

static char _sgk_press(SGK key,struct _sgw * const w,SGE * const e){// SGK * const keys, SGK * const pressed){
    if(!key) return 0;
    if(key>0xffff) w->keys|=key;
    else w->keys=(w->keys&~0xffff)|key;
    e->key=w->keys;
    return 1;
}

static int _sgk_release(SGK key,struct _sgw * const w,SGE * const e){// SGK * const keys, SGK * const released){
    if(!key) return 0;
    e->key=w->keys;
    if(key>0xffff) w->keys^=key;
    else w->keys&=~0xffff;
    return 1;
}

#include "./sgw/sg_impl.h"
