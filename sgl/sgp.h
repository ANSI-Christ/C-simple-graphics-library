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
void sgm_row(const SGM *m,int x1,int x2,int y,SGC c);
void sgm_column(const SGM *m,int x,int y1,int y2,SGC c);
void sgm_insert(const SGM *m,int x,int y,const SGM *i);
void sgm_point(const SGM *m,int x,int y,unsigned int w,SGC c);





#endif /* SG_PAINT */
