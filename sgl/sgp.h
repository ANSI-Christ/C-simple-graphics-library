/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_PAINT
#define SG_PAINT

#include "sgc.h"

typedef struct{
    char * const c;
    const int x,y;
    const unsigned int w,h,color_bytes;
    enum{ SGM_UNLIMITED=1 } flags;
    const struct{unsigned int x,y,w[2],h[2];}_;
}SGM;

void sgm_cfg(SGM *m,void *c,unsigned int w,unsigned int h,unsigned int color_bytes);
void sgm_sub(const SGM *m,int x,int y,unsigned int w,unsigned int h,unsigned char flags,SGM *s);

void *sgm_at(const SGM *m,int x,int y);

void sgm_set(const SGM *m,int x,int y,const void *c);
void sgm_insert(const SGM *m,int x,int y,const SGM *i);
void sgm_change(const SGM *m,int x,int y,const void *c1, const void *c2);

void sgm_row(const SGM *m,int x,int y,unsigned int l,const void *c);
void sgm_column(const SGM *m,int x,int y,unsigned int l,const void *c);

void sgm_line(const SGM *m,int x1,int y1,int x2,int y2,const void *c);
void sgm_fill(const SGM *m,int x,int y,const void *c,const void *border);

void sgm_rect(const SGM *m,int x,int y,unsigned int w,unsigned int h,const void *c);
void sgm_square(const SGM *m,int x,int y,unsigned int w,unsigned int h,const void *c);

void sgm_circle(const SGM *m,int x,int y,unsigned int r,const void *c);
void sgm_ring(const SGM *m,int x,int y,unsigned int r,unsigned int rp,const void *c);

void sgm_oval(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const void * const c);
void sgm_ellipse(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const unsigned int rp,const void * const c);





#endif /* SG_PAINT */
