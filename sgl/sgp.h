/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_PAINT
#define SG_PAINT

#include "sgc.h"

typedef struct{
    SGC *c;
    unsigned int w,h;
    int x,y;
    enum{ SGM_LIMITED=0, SGM_UNLIMITED=1 } flags;
    struct{unsigned int x,y,w[2],h[2];}_;
}SGM;

SGC *sgm_at(const SGM *m,unsigned int x,unsigned int y);

void sgm_set(SGM *m,SGC *c,unsigned int w,unsigned int h);

void sgm_sub(const SGM *m,int x,int y,unsigned int w,unsigned int h,int flags,SGM *s);

#endif /* SG_PAINT */
