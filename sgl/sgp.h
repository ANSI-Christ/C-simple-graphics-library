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
    enum{ SGM_UNLIMITED=1 } flags;
    struct{unsigned int x,y,w[2],h[2];}_;
}SGM;

void sgm_cfg(SGM *m,SGC *c,unsigned int w,unsigned int h);
void sgm_sub(const SGM *m,int x,int y,unsigned int w,unsigned int h,int flags,SGM *s);

SGC *sgm_at(const SGM *m,int x,int y);

void sgm_set(const SGM *m,int x,int y,SGC c);
void sgm_insert(const SGM *m,int x,int y,const SGM *i);

void sgm_row(const SGM *m,int x,int y,unsigned int l,SGC c);
void sgm_column(const SGM *m,int x,int y,unsigned int l,SGC c);

void sgm_fill(const SGM *m,int x,int y,SGC c,SGC border);
void sgm_line(const SGM *m,int x1,int y1,int x2,int y2,unsigned int w,SGC c);

void sgm_rect(const SGM *m,int x,int y,unsigned int l,unsigned int h,SGC c);
void sgm_square(const SGM *m,int x,int y,unsigned int l,unsigned int h,SGC c);

void sgm_ring(const SGM *m,int x,int y,unsigned int r,SGC c);
void sgm_circle(const SGM *m,int x,int y,unsigned int r,SGC c);





#endif /* SG_PAINT */
