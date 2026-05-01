/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include "sgw.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

struct _sgw_color_info{

    struct{
        unsigned long r,g,b;
    }mask;

    struct{
        unsigned int r,g,b;
    }shift;

    struct{
        unsigned char r,g,b;
    }bits;

    struct{
        unsigned char bits, bytes;
    }pixel;

};

static void _sgc_convert(const SGC * const src,const unsigned int count,const struct _sgw_color_info * const info,unsigned char * const dst){
    const unsigned int bytes=info->pixel.bytes, shift_r=info->shift.r, shift_g=info->shift.g, shift_b=info->shift.b;
    const unsigned int r_max=(1<<info->bits.r)-1, g_max=(1<<info->bits.g)-1, b_max=(1<<info->bits.b)-1;
    unsigned int i,j;
    for(i=0;i<count;++i){
        const unsigned long r=(src[i].r*r_max)/255, g=(src[i].g*g_max)/255, b=(src[i].b*b_max)/255;
        const unsigned long pixel=(r<<shift_r) |(g<<shift_g) | (b<<shift_b);
        for(j=0;j<bytes;++j) dst[i*bytes+j]=(pixel>>(b*8)) & 255;
    }
}

static char _sgk_press(SGK key,struct _sgw * const w,SGE * const e){
    if(!key) return 0;
    if(key>0xffff) w->keys|=key;
    else w->keys=(w->keys&~0xffff)|key;
    e->key=w->keys;
    return 1;
}

static int _sgk_release(SGK key,struct _sgw * const w,SGE * const e){
    if(!key) return 0;
    e->key=w->keys;
    if(key>0xffff) w->keys^=key;
    else w->keys&=~0xffff;
    return 1;
}


SGC *sgw_pixel(SGW * const w,const unsigned int x,const unsigned int y){
    if(x<w->rectangle.w && y<w->rectangle.h)
        return w->pixel+y*w->rectangle.w+x;
    return NULL;
}

void sgw_fill(SGW * const w,const SGC c){
    unsigned int i=w->rectangle.w*w->rectangle.h;
    while(i) w->pixel[--i]=c;
}

void _sgw_unsused_funcs(void){
    _sgc_convert(NULL,0,0,NULL);
    _sgk_press(0,NULL,NULL);
    _sgk_release(0,NULL,NULL);
    SGC_RGB(0,0,0);
}

#include "./sgw/sg_impl.h"
