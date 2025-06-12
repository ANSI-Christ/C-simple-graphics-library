/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include "sgp.h"

void sgm_cfg(SGM * const m,SGC * const c,const unsigned int w,const unsigned int h){
    m->c=c;
    m->flags=0;
    m->x=m->_.x=0;
    m->y=m->_.y=0;
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
    s->flags=0;
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

SGC *sgm_at(const SGM * const m,const int _x,const int _y){
    const unsigned int x=_x+m->x;
    if(x<m->w){
        const unsigned int y=_y+m->y;
        if(y<m->h && ( (m->flags&SGM_UNLIMITED) || (x-m->_.x<m->_.w[0] && y-m->_.y<m->_.h[0]) ) )
            return m->c+(x+y*m->_.w[1]);
    }
    return NULL;
}

void sgm_set(const SGM * const m,const int x,const int y,const SGC c){
    SGC * const p=sgm_at(m,x,y);
    if(p) *p=c;
}

void sgm_insert(const SGM * const m,const int x,const int y,const SGM * const i){
    unsigned int w=i->w,h=i->h;
    const SGC *ci;
    SGC *cm;
    while(h--)
        while(w--)
            if( (cm=sgm_at(m,x+w,y+h)) && (ci=sgm_at(i,w,h)) )
                *cm=*ci;
}

void sgm_row(const SGM * const m,int x1,const int x2,const int y,const  SGC c){
    for(;x1<x2;++x1) sgm_set(m,x1,y,c);
}

void sgm_column(const SGM * const m,const int x,int y1,const int y2,const SGC c){
    for(;y1<y2;++y1) sgm_set(m,x,y1,c);
}

void sgm_point(const SGM * const m,const int x,const int y,unsigned int w,const SGC c){
    if(w-->1){
        int l,r,dx=0,dy=w,delta=3-(w<<1);
        while(dx<=dy) {
            l=x-dy; r=x+dy;
            sgm_row(m,l,r,y+dx,c);
            sgm_row(m,l,r,y-dx,c);
            l=x-dx; r=x+dx;
            sgm_row(m,l,r,y+dy,c);
            sgm_row(m,l,r,y-dy,c);
            if (delta<0) delta+=(dx<<2)+6;
            else{delta+=((dx-dy)<<2)+10; --dy;}
            ++dx;
        }
    }else sgm_set(m,x,y,c);
}
