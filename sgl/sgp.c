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

void sgm_row(const SGM * const m,int x,const int y,const unsigned int l,const  SGC c){
    const int e=x+l;
    for(;x<e;++x) sgm_set(m,x,y,c);
}

void sgm_column(const SGM * const m,const int x,int y,const unsigned int l,const SGC c){
    const int e=y+l;
    for(;y<e;++y) sgm_set(m,x,y,c);
}

void sgm_line(const SGM * const m,int x1,int y1,const int x2,const int y2,const unsigned int w,const SGC c){
    const int dx=abs(y2-y1), dy=abs(x2-x1), sx=y1 < y2 ? 1 : -1, sy=x1 < x2 ? 1 : -1;
    int e=dx-dy;
    sgm_circle(m,x2,y2,w,c);
    while(y1!=y2 || x1!=x2){
        const int e2=e<<1;
        sgm_circle(m,x1,y1,w,c);
        if(e2>-dy){
            e-=dy;
            y1+=sx;
        }
        if(e2<dx){
            e+=dx;
            x1+=sy;
        }
    }
}

void sgm_rect(const SGM * const m,const int x,const int y,const unsigned int w,const unsigned int h,const SGC c){
    sgm_row(m,x,y,w,c);
    sgm_column(m,x,y+1,h-2,c);
    sgm_column(m,x+w-1,y+1,h-2,c);
    sgm_row(m,x,y+h-1,w,c);
}

void sgm_square(const SGM * const m,const int x,const int y,const unsigned int w,const unsigned int h,const SGC c){
    int i,j;
    for(j=0;j<h;++j){
        const int cy=y+j;
        for(i=0;i<w;++i)
            sgm_set(m,x+i,cy,c);
    }
}

void sgm_circle(const SGM * const m,const int x,const int y,unsigned int r,const SGC c){
    if(r-->1){
        int t,dx=0,dy=r,delta=3-(r<<1);
        while(dx<=dy) {
            t=x-dy; r=dy<<1;
            sgm_row(m,t,y+dx,r,c);
            sgm_row(m,t,y-dx,r,c);
            t=x-dx; r=dx<<1;
            sgm_row(m,t,y+dy,r,c);
            sgm_row(m,t,y-dy,r,c);
            if (delta<0) delta+=(dx<<2)+6;
            else{delta+=((dx-dy)<<2)+10; --dy;}
            ++dx;
        }
    }else sgm_set(m,x,y,c);
}

static void _sgm_ring(const SGM * const m,const int x,const int y,const int dx, const int dy,const SGC c){
    sgm_set(m,x-dx,y-dy,c);
    sgm_set(m,x+dx,y-dy,c);
    sgm_set(m,x-dx,y+dy,c);
    sgm_set(m,x+dx,y+dy,c);
}

void sgm_ring(const SGM * const m,const int x,const int y,const unsigned int r,const SGC c){
    int dx=0,dy=r,delta=3-(r<<1);
    while(dx<dy) {
        _sgm_ring(m,x,y,dy,dx,c);
        _sgm_ring(m,x,y,dx,dy,c);
        if (delta<0) delta+=(dx<<2)+6;
        else delta+=((dx-(dy--))<<2)+10;
        ++dx;
    }
    if(dx==dy) _sgm_ring(m,x,y,dy,dx,c);
}


static char _sgm_fill_check(const SGM * const m,const int x,const int y,const SGC c,const SGC border){
    SGC *p=sgm_at(m,x,y);
    return p && *p!=border && *p!=c;
}

static void _sgm_fill_row(const SGM * const m,int x,const int y,const int dir,const int l,const int r,const SGC c,const SGC border){
    int xl=x, xr=x, yd;
    while(_sgm_fill_check(m,--xl,y,c,border));
    while(_sgm_fill_check(m,++xr,y,c,border));
    for(x=++xl;x<xr;++x)
        sgm_set(m,x,y,c);
    for(x=xl;x<xr;++x)
        if(_sgm_fill_check(m,x,(yd=y+dir),c,border))
            _sgm_fill_row(m,x,yd,dir,xl,xr-1,c,border);
    for(x=xl;x<l;++x)
        if(_sgm_fill_check(m,x,(yd=y-dir),c,border))
            _sgm_fill_row(m,x,yd,-dir,xl,xr-1,c,border);
    for(x=r;x<xr;++x)
        if(_sgm_fill_check(m,x,(yd=y-dir),c,border))
            _sgm_fill_row(m,x,yd,-dir,xl,xr-1,c,border);
}

void sgm_fill(const SGM * const m,const int x,const int y,const SGC c,const SGC border){
    if(sgm_at(m,x,y)) _sgm_fill_row(m,x,y,1,x,x,c,border);
}

