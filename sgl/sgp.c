/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <stddef.h>
#include <unistd.h>
#include <math.h>
#include "sgp.h"

#define SG_SET(_t_,_l_,_r_) do{ const union{const void *_;_t_ *t;}_1_={(const void*)&(_l_)}; *_1_.t=(_r_); }while(0)

static char _sgm_converter(const void * const from,void * const to,const void * const size){
    memcpy(to,from,(size_t)size); return 1;
}

void sgm_cfg(SGM * const m,void * const c,const unsigned int w,const unsigned int h,const unsigned int color_bytes){
    m->flags=0;
    SG_SET(void*,m->c,c);
    SG_SET(int,m->color_bytes,color_bytes);
    SG_SET(int,m->x,0);
    SG_SET(int,m->y,0);
    SG_SET(int,m->w,w);
    SG_SET(int,m->h,h);
    SG_SET(int,m->_.x,0);
    SG_SET(int,m->_.y,0);
    SG_SET(int,m->_.w[0],w);
    SG_SET(int,m->_.h[0],h);
    SG_SET(int,m->_.w[1],w);
    SG_SET(int,m->_.h[1],h);
}

void sgm_sub(const SGM * const m,int x,int y,const unsigned int w,const unsigned int h,const unsigned char flags,SGM * const s){
    unsigned int l,r,u,b;
    x+=m->x; y+=m->y;
    if(flags & SGM_UNLIMITED){
        l=u=0;
        r=m->_.w[1];
        b=m->_.h[1];
    }else{
        r=m->_.w[0]+(l=m->_.x);
        b=m->_.h[0]+(u=m->_.y);
    }
    s->flags=0;
    SG_SET(void*,s->c,m->c);
    SG_SET(int,s->color_bytes,m->color_bytes);
    SG_SET(int,s->x,x);
    SG_SET(int,s->y,y);
    SG_SET(int,s->w,w);
    SG_SET(int,s->h,h);
    SG_SET(int,s->_.w[1],m->_.w[1]);
    SG_SET(int,s->_.h[1],m->_.h[1]);
    SG_SET(int,s->_.x,((unsigned int)x<l ? l : (unsigned int)x));
    SG_SET(int,s->_.y,((unsigned int)y<u ? u : (unsigned int)y));
    SG_SET(int,s->_.w[0],((unsigned int)x+w>r ? r-s->_.x : w));
    SG_SET(int,s->_.h[0],((unsigned int)y+h>b ? b-s->_.y : h));
}

void *sgm_at(const SGM * const m,const int _x,const int _y){
    const unsigned int x=_x+m->x;
    if(x<m->w){
        const unsigned int y=_y+m->y;
        if(y<m->h && ( (m->flags&SGM_UNLIMITED) || (x-m->_.x<m->_.w[0] && y-m->_.y<m->_.h[0]) ) )
            return m->c+(x+y*m->_.w[1])*m->color_bytes;
    }
    return NULL;
}

void sgm_set(const SGM * const m,const int x,const int y,const void * const c){
    void * const p=sgm_at(m,x,y);
    if(p) memcpy(p,c,m->color_bytes);
}

void sgm_swap(const SGM * const m,const int x,const int y,const void * const c1, const void * const c2){
    void * const c=sgm_at(m,x,y);
    if(c){
        if(!memcmp(c,c1,m->color_bytes)){
            memcpy(c,c2,m->color_bytes);
            return;
        }
        if(!memcmp(c,c2,m->color_bytes))
            memcpy(c,c1,m->color_bytes);
    }
}

void sgm_row(const SGM * const m,int x,const int y,const unsigned int l,const void * const c){
    const int e=x+l;
    for(;x<e;++x) sgm_set(m,x,y,c);
}

void sgm_column(const SGM * const m,const int x,int y,const unsigned int l,const void * const c){
    const int e=y+l;
    for(;y<e;++y) sgm_set(m,x,y,c);
}

void sgm_line(const SGM * const m,int x1,int y1,const int x2,const int y2,const void * const c){
    const int dx=abs(y2-y1), dy=abs(x2-x1), sx=y1 < y2 ? 1 : -1, sy=x1 < x2 ? 1 : -1;
    int e=dx-dy;
    sgm_set(m,x2,y2,c);
    while(y1!=y2 || x1!=x2){
        const int e2=e<<1;
        sgm_set(m,x1,y1,c);
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

void sgm_rect(const SGM * const m,const int x,const int y,const unsigned int w,const unsigned int h,const void * const c){
    sgm_row(m,x,y,w,c);
    sgm_column(m,x,y+1,h-2,c);
    sgm_column(m,x+w-1,y+1,h-2,c);
    sgm_row(m,x,y+h-1,w,c);
}

void sgm_square(const SGM * const m,const int x,const int y,const unsigned int w,const unsigned int h,const void * const c){
    unsigned int i,j;
    for(j=0;j<h;++j){
        const int cy=y+j;
        for(i=0;i<w;++i)
            sgm_set(m,x+i,cy,c);
    }
}

void sgm_circle(const SGM * const m,const int x,const int y,unsigned int r,const void * const c){
    if(r>1){
        int t,dx=0,dy=--r,delta=3-(r<<1);
        while(dx<dy) {
            t=x-dy; r=dy<<1;
            sgm_row(m,t,y+dx,r,c);
            sgm_row(m,t,y-dx,r,c);
            t=x-dx; r=dx<<1;
            sgm_row(m,t,y+dy,r,c);
            sgm_row(m,t,y-dy,r,c);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            t=x-dy; r=dy<<1;
            sgm_row(m,t,y+dx,r,c);
            sgm_row(m,t,y-dx,r,c);
        }
        return;
    }
    if(r) sgm_set(m,x,y,c);
}

void sgm_ring(const SGM * const m,const int x,const int y,unsigned int r,const unsigned int rp,const void * const c){
    if(r>1){
        int dx=0,dy=--r,delta=3-(r<<1);
        while(dx<dy) {
            const int x1=x-dx, x2=x+dx, x3=x-dy, x4=x+dy;
            const int y1=y-dy, y2=y+dy, y3=y-dx, y4=y+dx;
            sgm_circle(m,x2,y2,rp,c);
            sgm_circle(m,x1,y2,rp,c);
            sgm_circle(m,x1,y1,rp,c);
            sgm_circle(m,x2,y1,rp,c);
            sgm_circle(m,x4,y3,rp,c);
            sgm_circle(m,x3,y3,rp,c);
            sgm_circle(m,x3,y4,rp,c);
            sgm_circle(m,x4,y4,rp,c);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            sgm_circle(m,x2,y2,rp,c);
            sgm_circle(m,x1,y2,rp,c);
            sgm_circle(m,x1,y1,rp,c);
            sgm_circle(m,x2,y1,rp,c);
        }
        return;
    }
    if(r) sgm_circle(m,x,y,rp,c);
}

void sgm_ellipse(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const unsigned int rp,const void * const c){
    const long w2=rw*rw, h2=rh*rh, a2=h2<<1, a4=h2<<2, b2=w2<<1, b4=w2<<2;
    long d=a2*(rw-1)*rw+h2+b2*(1-h2);
    int dy=0,dx=rw;
    while(h2*dx>w2*dy){
        const int x1=x-dx, x2=x+dx;
        const int y1=y-dy, y2=y+dy;
        sgm_circle(m,x2,y2,rp,c);
        sgm_circle(m,x1,y2,rp,c);
        sgm_circle(m,x1,y1,rp,c);
        sgm_circle(m,x2,y1,rp,c);
        if (d>=0) d-=a4*(--dx);
        d+=b2*(3+(dy<<1)); ++dy;
    }
    d=b2*(dy+1)*dy+a2*(dx*(dx-2)+1)+(1-a2)*w2;
    while(dx>=0){
        const int x1=x-dx, x2=x+dx;
        const int y1=y-dy, y2=y+dy;
        sgm_circle(m,x2,y2,rp,c);
        sgm_circle(m,x1,y2,rp,c);
        sgm_circle(m,x1,y1,rp,c);
        sgm_circle(m,x2,y1,rp,c);
        if(d<=0){d+=b4*dy; ++dy;}
        d+=a2*(3-((--dx)<<1));
    }
}

void sgm_oval(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const void * const c){
    const long w2=rw*rw, h2=rh*rh, a2=h2<<1, a4=h2<<2, b2=w2<<1, b4=w2<<2;
    long d=a2*(rw-1)*rw+h2+b2*(1-h2);
    int dy=0,dx=rw;
    while(h2*dx>w2*dy){
        const unsigned int s=x-dx, l=dx<<1;
        sgm_row(m,s,y-dy,l,c);
        sgm_row(m,s,y+dy,l,c);
        if (d>=0) d-=a4*(--dx);
        d+=b2*(3+(dy<<1)); ++dy;
    }
    d=b2*(dy+1)*dy+a2*(dx*(dx-2)+1)+(1-a2)*w2;
    while(dx>=0){
        const unsigned int s=x-dx, l=dx<<1;
        sgm_row(m,s,y-dy,l,c);
        sgm_row(m,s,y+dy,l,c);
        if(d<=0){d+=b4*dy; ++dy;}
        d+=a2*(3-((--dx)<<1));
    }
}

static char _sgm_border_check(const int * const b,const int x,const int y){
    return (unsigned int)(x-b[0])<(unsigned int)b[1] && (unsigned int)(y-b[2])<(unsigned int)b[3];
}

static void _sgm_border_ring(int (* const b)[4],const int x,const int y,const unsigned int rw,const unsigned int rh,const float ang,const float rot){
    const float ang_end=ang+rot;
    const unsigned char end=((int)(ang_end/45.0f))&7;
    unsigned char i=((int)(ang/45.0f))&7;
    do{
        const float octant_start=i*45.0f;
        const float octant_end=(i+1)*45.0f;
        const float active_start=fmaxf(ang,octant_start)*M_PI/180.0f;
        const float active_end=fminf(ang_end,octant_end)*M_PI/180.0f;

        if(active_start<active_end){
            const int x1=x+rw*cosf(active_start);
            const int y1=y+rh*sinf(active_start);
            const int x2=x+rw*cosf(active_end);
            const int y2=y+rh*sinf(active_end);

            b[i][0]=fmin(fmin(x,x1),x2);
            b[i][1]=fmax(fmax(x,x1),x2)-b[i][0];
            b[i][2]=fmin(fmin(y,y1),y2);
            b[i][3]=fmax(fmax(y,y1),y2)-b[i][1];
        }
    }while((i=((i+1)&7))!=end);
}

void sgm_arc_ring(const SGM * const m,const int x,const int y,unsigned int r,const unsigned int rp,const float ang,const float rot,const void * const c){
    if(r>1){
        int dx=0,dy=--r,delta=3-(r<<1), b[8][4]={{0}};
        _sgm_border_ring(b,x,y,r,r,ang,rot);
        while(dx<dy) {
            const int x1=x-dx, x2=x+dx, x3=x-dy, x4=x+dy;
            const int y1=y-dy, y2=y+dy, y3=y-dx, y4=y+dx;
            if(_sgm_border_check(b[0],x2,y2)) sgm_circle(m,x2,y2,rp,c);
            if(_sgm_border_check(b[1],x1,y2)) sgm_circle(m,x1,y2,rp,c);
            if(_sgm_border_check(b[2],x1,y1)) sgm_circle(m,x1,y1,rp,c);
            if(_sgm_border_check(b[3],x2,y1)) sgm_circle(m,x2,y1,rp,c);
            if(_sgm_border_check(b[4],x4,y3)) sgm_circle(m,x4,y3,rp,c);
            if(_sgm_border_check(b[5],x3,y3)) sgm_circle(m,x3,y3,rp,c);
            if(_sgm_border_check(b[6],x3,y4)) sgm_circle(m,x3,y4,rp,c);
            if(_sgm_border_check(b[7],x4,y4)) sgm_circle(m,x4,y4,rp,c);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            if(_sgm_border_check(b[0],x2,y2)) sgm_circle(m,x2,y2,rp,c);
            if(_sgm_border_check(b[1],x1,y2)) sgm_circle(m,x1,y2,rp,c);
            if(_sgm_border_check(b[2],x1,y1)) sgm_circle(m,x1,y1,rp,c);
            if(_sgm_border_check(b[3],x2,y1)) sgm_circle(m,x2,y1,rp,c);
        }
    }
}

static char _sgm_cmp(const SGM * const m,const int x,const int y,const void * const c,const void * const border){
    const void * const p=sgm_at(m,x,y);
    return p && memcmp(p,border,m->color_bytes) && memcmp(p,c,m->color_bytes);
}

static void _sgm_fill_row(const SGM * const m,int x,const int y,const int dir,const int l,const int r,const void * const c,const void * const border){
    int xl=x, xr=x, yd;
    while(_sgm_cmp(m,--xl,y,c,border));
    while(_sgm_cmp(m,++xr,y,c,border));
    for(x=++xl;x<xr;++x)
        sgm_set(m,x,y,c);
    for(x=xl;x<xr;++x)
        if(_sgm_cmp(m,x,(yd=y+dir),c,border))
            _sgm_fill_row(m,x,yd,dir,xl,xr-1,c,border);
    for(x=xl;x<l;++x)
        if(_sgm_cmp(m,x,(yd=y-dir),c,border))
            _sgm_fill_row(m,x,yd,-dir,xl,xr-1,c,border);
    for(x=r;x<xr;++x)
        if(_sgm_cmp(m,x,(yd=y-dir),c,border))
            _sgm_fill_row(m,x,yd,-dir,xl,xr-1,c,border);
}

void sgm_fill(const SGM * const m,const int x,const int y,const void * const c,const void * const border){
    if(sgm_at(m,x,y)) _sgm_fill_row(m,x,y,1,x,x,c,border);
}

void sgm_paste(const SGM *m,const int x,const int y,const SGM * const p,char (*converter)(const void *from,void *to,const void *arg),const void *arg){
    unsigned int w=p->w,h=p->h;
    const void *from;
    void *to;
    if(!converter){
        if(m->color_bytes!=p->color_bytes)
            return;
        *(void**)&converter=_sgm_converter;
        arg=(const void*)(size_t)m->color_bytes;
    }
    while(h--)
        while(w--)
            if( (to=sgm_at(m,x+w,y+h)) && (from=sgm_at(p,w,h)) )
                converter(from,to,arg);
}

void sgm_convert(const SGM * const m,const SGM * const c,char (*converter)(const void *from,void *to,const void *arg),const void *arg){
    const float rx=(float)m->w/c->w;
    const float ry=(float)m->h/c->h;
    unsigned int dx,dy;
    const void *from, *last_from=NULL;
    void *to, *last_to;
    char cmp;
    if (!converter){
        if(m->color_bytes!=c->color_bytes)
            return;
        *(void**)&converter=_sgm_converter;
        arg=(const void*)(size_t)m->color_bytes;
    }
    for (dy=0;dy<c->h;++dy)
        for (dx=0;dx<c->w;++dx)
            if( (to=sgm_at(c,dx,dy)) && (from=sgm_at(m,dx*rx,dy*ry)) ){
                if(from!=last_from){
                    cmp=converter((last_from=from),(last_to=to),arg);
                    continue;
                }
                if(cmp) memcpy(to,last_to,c->color_bytes);
            }
}

#undef SG_SET