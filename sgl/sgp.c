/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include "sgp.h"

SGC *sgm_at(const SGM * const m,unsigned int x,unsigned int y){
    if( (x+=m->x)<m->w && (y+=m->y)<m->h && ( (m->flags&SGM_UNLIMITED) || (x-m->_.x<m->_.w[0] && y-m->_.y<m->_.h[0]) ) )
        return m->c+(x+y*m->_.w[1]);
    return NULL;
}

void sgm_set(SGM * const m,SGC * const c,const unsigned int w,const unsigned int h){
    m->c=c;
    m->x=m->_.x=0;
    m->y=m->_.y=0;
    m->flags=SGM_LIMITED;
    m->w=m->_.w[0]=m->_.w[1]=w;
    m->h=m->_.h[0]=m->_.h[1]=h;
}

void sgm_sub(const SGM * const m,int x,int y,const unsigned int w,const unsigned int h,const int flags,SGM * const s){
    unsigned int l,r,u,b;
    x+=m->x; y+=m->y;
    s->c=m->c;
    s->_.w[1]=m->_.w[1];
    s->_.h[1]=m->_.h[1];
    s->x=x; s->y=y;
    s->w=w; s->h=h;
    s->flags=SGM_LIMITED;
    if(flags & SGM_UNLIMITED){
        l=u=0;
        r=m->_.w[1];
        b=m->_.h[1];
    }else{
        r=m->_.w[0]+(l=m->_.x);
        b=m->_.h[0]+(u=m->_.y);
    }
    s->_.x=((unsigned int)x<l ? l : (unsigned int)x);
    s->_.y=((unsigned int)y<u ? u : (unsigned int)y);
    s->_.w[0]=((unsigned int)x+w>r ? r-s->_.x : w);
    s->_.h[0]=((unsigned int)y+h>b ? b-s->_.y : h);
}
