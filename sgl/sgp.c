/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "sgp.h"
#include "./sgf/sgf_5x12.c"

#define SG_SET(_t_,_l_,_r_) do{ const union{const void *_;_t_ *t;}_1_={(const void*)&(_l_)}; *_1_.t=(_r_); }while(0)

static char _sgm_converter(const void * const from,void * const to,const void * const size){
    memcpy(to,from,(size_t)size); return 1;
}

void sgm_cfg(SGM * const m,void * const c,const unsigned int w,const unsigned int h,const unsigned int color_bytes){
    m->flags=0;
    SG_SET(void*,m->data,c);
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
    int l,r,u,b;
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
    SG_SET(void*,s->data,m->data);
    SG_SET(int,s->color_bytes,m->color_bytes);
    SG_SET(int,s->x,x);
    SG_SET(int,s->y,y);
    SG_SET(int,s->w,w);
    SG_SET(int,s->h,h);
    SG_SET(int,s->_.w[1],m->_.w[1]);
    SG_SET(int,s->_.h[1],m->_.h[1]);

    if(x<(int)l) SG_SET(int,s->_.x,l);
    else SG_SET(int,s->_.x,x);

    if(y<(int)u) SG_SET(int,s->_.y,u);
    else SG_SET(int,s->_.y,y);

    if(x<(int)r) SG_SET(int,s->_.w[0],(x+(int)w>r ? r-s->_.x : w));
    else SG_SET(int,s->_.w[0],0);

    if(y<(int)b) SG_SET(int,s->_.h[0],(y+(int)h>b ? b-s->_.y : h));
    else SG_SET(int,s->_.h[0],0);
}

void *sgm_at(const SGM * const m,const int _x,const int _y){
    const unsigned int x=_x+m->x;
    if(m->flags&SGM_UNLIMITED){
        if(x<m->_.w[1]){
            const unsigned int y=_y+m->y;
            if(y<m->_.h[1]) return m->data+(x+y*m->_.w[1])*m->color_bytes;
        }
        return NULL;
    }
    if(x-m->_.x<m->_.w[0]){
        const unsigned int y=_y+m->y;
        if(y-m->_.y<m->_.h[0]) return m->data+(x+y*m->_.w[1])*m->color_bytes;
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

void sgm_line(const SGM * const m,int x1,int y1,const int x2,const int y2,const unsigned int rp,const void * const c){
    const int dx=abs(y2-y1), dy=abs(x2-x1), sx=y1 < y2 ? 1 : -1, sy=x1 < x2 ? 1 : -1;
    int e=dx-dy;
    sgm_set(m,x2,y2,c);
    while(y1!=y2 || x1!=x2){
        const int e2=e<<1;
        sgm_round(m,x1,y1,rp,c);
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

void sgm_round(const SGM * const m,const int x,const int y,unsigned int r,const void * const c){
    if(r>1){
        int t,dx=0,dy=--r,delta=3-(r<<1);
        while(dx<dy) {
            t=x-dx; r=dx<<1;
            sgm_row(m,t,y-dy,r,c);
            sgm_row(m,t,y+dy,r,c);
            t=x-dy; r=dy<<1;
            sgm_row(m,t,y-dx,r,c);
            sgm_row(m,t,y+dx,r,c);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            t=x-dx; r=dy<<1;
            sgm_row(m,t,y-dx,r,c);
            sgm_row(m,t,y+dx,r,c);
        }
        return;
    }
    if(r) sgm_set(m,x,y,c);
}

void sgm_circle(const SGM * const m,const int x,const int y,unsigned int r,const unsigned int rp,const void * const c){
    if(r>1){
        int dx=0,dy=--r,delta=3-(r<<1);
        while(dx<dy) {
            const int x1=x-dx, x2=x+dx, x3=x-dy, x4=x+dy;
            const int y1=y-dy, y2=y+dy, y3=y-dx, y4=y+dx;
            sgm_round(m,x1,y1,rp,c);
            sgm_round(m,x2,y1,rp,c);
            sgm_round(m,x3,y3,rp,c);
            sgm_round(m,x4,y3,rp,c);
            sgm_round(m,x3,y4,rp,c);
            sgm_round(m,x4,y4,rp,c);
            sgm_round(m,x1,y2,rp,c);
            sgm_round(m,x2,y2,rp,c);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            sgm_round(m,x1,y1,rp,c);
            sgm_round(m,x2,y1,rp,c);
            sgm_round(m,x1,y2,rp,c);
            sgm_round(m,x2,y2,rp,c);
        }
        return;
    }
    if(r) sgm_round(m,x,y,rp,c);
}

void sgm_ellipse(const SGM * const m,const int x,const int y,const unsigned int _rw,const unsigned int _rh,const unsigned int rp,const void * const c){
    const unsigned int rw=_rw-1, rh=_rh-1;
    const long w2=rw*rw, h2=rh*rh, a2=h2<<1, a4=h2<<2, b2=w2<<1, b4=w2<<2;
    long d=a2*(rw-1)*rw+h2+b2*(1-h2);
    int dy=0,dx=rw;
    while(h2*dx>w2*dy){
        const int x1=x-dx, x2=x+dx;
        const int y1=y-dy, y2=y+dy;
        sgm_round(m,x1,y1,rp,c);
        sgm_round(m,x2,y1,rp,c);
        sgm_round(m,x1,y2,rp,c);
        sgm_round(m,x2,y2,rp,c);
        if (d>=0) d-=a4*(--dx);
        d+=b2*(3+(dy<<1)); ++dy;
    }
    d=b2*(dy+1)*dy+a2*(dx*(dx-2)+1)+(1-a2)*w2;
    while(dx>=0){
        const int x1=x-dx, x2=x+dx;
        const int y1=y-dy, y2=y+dy;
        sgm_round(m,x1,y1,rp,c);
        sgm_round(m,x2,y1,rp,c);
        sgm_round(m,x1,y2,rp,c);
        sgm_round(m,x2,y2,rp,c);
        if(d<=0){d+=b4*dy; ++dy;}
        d+=a2*(3-((--dx)<<1));
    }
}

void sgm_oval(const SGM * const m,const int x,const int y,const unsigned int _rw,const unsigned int _rh,const void * const c){
    const unsigned int rw=_rw-1, rh=_rh-1;
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

static void _sgm_border(int (* const b)[4],const int x,const int y,const unsigned int rw,const unsigned int rh,const double _ang,const double _rot){
    const double rot=(fabs(_rot)<360.?_rot:360.), ang=fmod(_ang,360)+360;
    const double ang_st=fmod(rot<0.? ang+360.+rot : ang,360.), ang_end=ang_st+fabs(rot);
    const int x1=x+rw*cos(ang_st*M_PI/180.), y1=y-rh*sin(ang_st*M_PI/180.), x2=x+rw*cos(ang_end*M_PI/180.), y2=y-rh*sin(ang_end*M_PI/180.);
    const unsigned char st=ang_st/90., end=ang_end/90.;
    unsigned char i=end+1;
    char cnt=3-(end-st);
    int *p;

    b[0][0]=b[3][0]=x;
    b[1][0]=b[2][0]=x-rw;

    b[0][2]=b[1][2]=y-rh;
    b[2][2]=b[3][2]=y;

    b[0][1]=b[1][1]=b[2][1]=b[3][1]=rw;
    b[0][3]=b[1][3]=b[2][3]=b[3][3]=rh;

    p=b[st];
    switch(st){
        case 0: p[1]=x1-p[0];  p[3]=y1-p[2];  break;
        case 1: p[1]=x1-p[0];  p[3]+=p[2]-y1; p[2]=y1; break;
        case 2: p[1]+=p[0]-x1; p[3]+=p[2]-y1; p[0]=x1; p[2]=y1; break;
        case 3: p[1]+=p[0]-x1; p[3]=y1-p[2];  p[0]=x1; break;
    }

    if(end-st==4){
        p=b[4];
        switch(st){
            case 0: p[0]=x2;   p[2]=y2; p[1]=x+rw-x2;    p[3]=y-y2; break;
            case 1: p[0]=x-rw; p[2]=y2; p[1]=x2-(x-rw); p[3]=y-y2; break;
            case 2: p[0]=x-rw; p[2]=y;  p[1]=x2-(x-rw); p[3]=y2-y; break;
            case 3: p[0]=x2;   p[2]=y;  p[1]=x+rw-x2;   p[3]=y2-y; break;
        }
    }else{
        p=b[end&3];
        b[4][0]=b[4][1]=0;
        switch(end&3){
            case 0: p[1]+=p[0]-x2; p[3]+=y2-p[2]; p[0]=x2; p[2]=y2; break;
            case 1: p[1]+=p[0]-x2; p[3]=y2-p[2];  p[0]=x2; break;
            case 2: p[1]=x2-p[0];  p[3]=y2-p[2];  break;
            case 3: p[1]=x2-p[0];  p[3]+=p[2]-y2; p[2]=y2; break;
        }
    }

    for(;cnt>0;--cnt,++i) b[i&3][1]=0;
}

void sgm_arc_circle(const SGM * const m,const int x,const int y,const unsigned int r,const unsigned int rp,const double ang,const double rot,const void * const c){
#define _SGDRAW(_i_,_x_,_y_) if(_sgm_border_check(b[_i_],_x_,_y_) || _sgm_border_check(b[4],_x_,_y_)) sgm_round(m,_x_,_y_,rp,c);
    if(r>1){
        int dx=0,dy=r-1,delta=3-(dy<<1), b[5][4];
        _sgm_border(b,x,y,r,r,ang,rot);
        while(dx<dy) {
            const int x1=x-dx, x2=x+dx, x3=x-dy, x4=x+dy;
            const int y1=y-dy, y2=y+dy, y3=y-dx, y4=y+dx;
            _SGDRAW(1,x1,y1);
            _SGDRAW(0,x2,y1);
            _SGDRAW(1,x3,y3);
            _SGDRAW(0,x4,y3);
            _SGDRAW(2,x3,y4);
            _SGDRAW(3,x4,y4);
            _SGDRAW(2,x1,y2);
            _SGDRAW(3,x2,y2);
            if (delta<0) delta+=(dx<<2)+6;
            else delta+=((dx-(dy--))<<2)+10;
            ++dx;
        }
        if(dx==dy){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            _SGDRAW(1,x1,y1);
            _SGDRAW(0,x2,y1);
            _SGDRAW(2,x1,y2);
            _SGDRAW(3,x2,y2);
        }
    }
#undef _SGDRAW
}

void sgm_arc_ellipse(const SGM * const m,const int x,const int y,const unsigned int _rw,const unsigned int _rh,const unsigned int rp,const double ang,const double rot,const void * const c){
#define _SGDRAW(_i_,_x_,_y_) if(_sgm_border_check(b[_i_],_x_,_y_) || _sgm_border_check(b[4],_x_,_y_)) sgm_round(m,_x_,_y_,rp,c);
    if(_rw && _rh){
        const unsigned int rw=_rw-1, rh=_rh-1;
        const long w2=rw*rw, h2=rh*rh, a2=h2<<1, a4=h2<<2, b2=w2<<1, b4=w2<<2;
        long d=a2*(rw-1)*rw+h2+b2*(1-h2);
        int dy=0,dx=rw, b[5][4];
        _sgm_border(b,x,y,rw,rh,ang,rot);
        while(h2*dx>w2*dy){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            _SGDRAW(1,x1,y1);
            _SGDRAW(0,x2,y1);
            _SGDRAW(2,x1,y2);
            _SGDRAW(3,x2,y2);
            if (d>=0) d-=a4*(--dx);
            d+=b2*(3+(dy<<1)); ++dy;
        }
        d=b2*(dy+1)*dy+a2*(dx*(dx-2)+1)+(1-a2)*w2;
        while(dx>=0){
            const int x1=x-dx, x2=x+dx;
            const int y1=y-dy, y2=y+dy;
            _SGDRAW(1,x1,y1);
            _SGDRAW(0,x2,y1);
            _SGDRAW(2,x1,y2);
            _SGDRAW(3,x2,y2);
            if(d<=0){d+=b4*dy; ++dy;}
            d+=a2*(3-((--dx)<<1));
        }
    }
#undef _SGDRAW
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
    if(!border){
        unsigned int i,j;
        for(i=0;i<m->h;++i)
            for(j=0;j<m->w;++j)
                sgm_set(m,j,i,c);
        return;
    }
    if(sgm_at(m,x,y)) _sgm_fill_row(m,x,y,1,x,x,c,border);
}

void sgm_paste(const SGM * const m,const int x,const int y,const SGM * const p,char (*converter)(const void *from,void *to,const void *arg),const void *arg){
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
    const void *from, *last_from=NULL;
    void *to, *last_to;
    unsigned int dx,dy;
    char cmp=0;
    if(!converter){
        if(m->color_bytes!=c->color_bytes)
            return;
        *(void**)&converter=_sgm_converter;
        arg=(const void*)(size_t)m->color_bytes;
    }
    for(dy=0;dy<c->h;++dy)
        for(dx=0;dx<c->w;++dx)
            if( (to=sgm_at(c,dx,dy)) && (from=sgm_at(m,dx*rx,dy*ry)) ){
                if(from!=last_from){
                    cmp=converter((last_from=from),(last_to=to),arg);
                    continue;
                }
                if(cmp) memcpy(to,last_to,c->color_bytes);
            }
}

void sgm_bmp(const SGM * const m,const char * const name){
    FILE * const f=fopen(name,"wb");
    if(f){
        const unsigned int palitra_count=(m->color_bytes==1)*256, palitra_size=palitra_count*sizeof(int), extra_color=0;
        const unsigned char extra_bytes=(m->w*(4-m->color_bytes))&3;
        const struct{
            unsigned int a,b,c,d,e,f;
            unsigned short g,h;
            unsigned int i,j,k,l,m,n;
        }header[1]={{
            14+40 + palitra_size + m->color_bytes*m->w*m->h + m->h*extra_bytes, 0,
            14+40 + palitra_size, 40, m->w, m->h, 1, m->color_bytes<<3, 0,0,0,0,
            1<<(m->color_bytes<<3),0
        }};
        unsigned int i,j;

        fwrite(((((const union{unsigned char _; int e;}){1}).e==1)?"BM":"MB"),2,1,f);
        fwrite(header,sizeof(header),1,f);
        for(i=0;i<palitra_count;++i){
            const int c=(i&0x3)<<16 | (i&0x1c)<<8 | (i&0xe0);
            fwrite(&c,sizeof(c),1,f);
        }
        for(i=m->h-1;i<m->h;--i){
            for(j=0;j<m->w;++j){
                const void * const c=sgm_at(m,j,i);
                fwrite((c?c:&extra_color),m->color_bytes,1,f);
            }
            for(j=0;j<extra_bytes;++j)
                fwrite(&extra_color,1,1,f);
        }
        fclose(f);
    }
}


struct _sgm_symb{
    const void *data;
    unsigned int size;
};

static char _sgm_char2symb(const char * const from,void * const to,const struct _sgm_symb * const p){
    if(*from){
        memcpy(to,p->data,p->size);
        return 1;
    }return 0;
}

static int _sgf_dy(const char *s,const unsigned int ofs,const unsigned int interval,const unsigned char align){
    switch(align&(SGF_YB|SGF_YC|SGF_YT)){
        case SGF_YT: return 0;
        case SGF_YC:{
            int res=1;
            while( (s=strchr(s,'\n')) ){++s; ++res;}
            return (res*ofs-interval)>>1;
        }
        default:{
            int res=1;
            while( (s=strchr(s,'\n')) ){++s; ++res;}
            return (res*ofs-interval);
        }
    }
}

static int _sgf_dx(const char * const s,const unsigned int ofs,const unsigned int interval,const unsigned char align){
    switch(align&(SGF_XL|SGF_XR|SGF_XC)){
        case SGF_XL: return 0;
        case SGF_XC: return (strcspn(s,"\n")*ofs-interval)>>1;
        default: return (strcspn(s,"\n")*ofs-interval);
    }
}

void sgm_string(const SGM * const m,const int x,const int y,const void * const c,const SGF *f,const enum SGF_ALIGN a,const char *s){
    if(!*s) return;
    if(!f) f=sgf_default;
{   const struct _sgm_symb info[1]={{c,m->color_bytes}};
    const unsigned int char_begin=f->bm->cb, char_end=1+f->bm->ce;
    const unsigned int mask=1<<(f->bm->bpw-1);
    const unsigned int ox=f->w+f->gap_w, oy=f->h+f->gap_h;
    const unsigned short w=((f->bm->bpw-1)>>3)+1, h=f->bm->bph*w;

    int dy=y-_sgf_dy(s,oy,f->gap_h,a);
    int dx=x-_sgf_dx(s,ox,f->gap_w,a);
    unsigned int i,j;

    SGM m_char[1], m_symb[1];
    void * const converter=_sgm_char2symb;
    char _buffer[1024], *p=( (unsigned int)f->bm->bpw*f->bm->bph<sizeof(_buffer) ? _buffer : malloc((unsigned int)f->bm->bpw*f->bm->bph<sizeof(_buffer)));

    if(!p) return;
    sgm_cfg(m_char,p,f->bm->bpw,f->bm->bph,sizeof(char));

    for(;*s;dx+=ox){
        const unsigned char id=*(s++);
        if(id=='\n'){
            dy+=oy;
            dx=x-_sgf_dx(s,ox,f->gap_w,a);
            continue;
        }

        if(id>=char_begin && id<char_end){
            const char *bm=f->bm->bits+(id-char_begin)*h;
            for(i=0,p=m_char->data;i<m_char->h;++i,bm+=w)
                for(j=0;j<m_char->w;++j,++p)
                    *p=*(bm+((f->bm->bpw-1-j)>>3)) & (mask>>j);
            sgm_sub(m,dx,dy,f->w,f->h,0,m_symb);
            sgm_convert(m_char,m_symb,(char(*)(const void*,void*,const void*))converter,info);
        }
    }
    if(m_char->data!=_buffer)
        free(m_char->data);
}}

void sgf_string_rect(const SGF *f,const char *s,unsigned int * const w,unsigned int * const h){
    *w=*h=0;
    if(!f) f=sgf_default;
{   const char *p;
    unsigned int dx=0,dy=0,l;
    while( (p=strchr(s,'\n')) ){
        ++dy; l=(size_t)(p-s); s=p+1;
        if(l>dx) dx=l;
    }
    *w=dx*(f->w+f->gap_w)-f->gap_w;
    *h=dy*(f->h+f->gap_h)-f->gap_h;
}}

const char *sgf_string_at(const SGF *f,const enum SGF_ALIGN a,const char *s,const int x,const int y){
    if(!f) f=sgf_default;
{   const unsigned int ox=f->w+f->gap_w, oy=f->h+f->gap_h;
    int i;

    if( y<(i=-_sgf_dy(s,oy,f->gap_h,a)) )
        return (const char*)1;
    while( y>=(i+=oy) ){
        const char * const p=strchr(s,'\n');
        if(!p) return (const char*)2;
        s=p+1;
    }

    if( x<(i=-_sgf_dx(s,ox,f->gap_w,a)) )
        return s;
    while( *s && x>=(i+=ox) ){
        if(*s=='\n') break;
        ++s;
    }
    return s;
}}


void sgp_init(SGP * const p,void * const c,const unsigned int w,const unsigned int h,const unsigned int color_bytes,const unsigned char default_box){
    sgm_cfg(&p->m,c,w,h,color_bytes);
    if(default_box) sgp_box(p,0,0,w,h);
}

void sgp_cfg(SGP * const p,const SGM * const m,const unsigned char default_box){
    memcpy(p,m,sizeof(*m));
    if(default_box) sgp_box(p,0,0,m->w,m->h);
}

void sgp_box(SGP * const p,const double x1,const double y1,const double x2,const double y2){
    SG_SET(double,p->x1,x1);
    SG_SET(double,p->y1,y1);
    SG_SET(double,p->x2,x2);
    SG_SET(double,p->y2,y2);
    SG_SET(double,p->_.dx,p->m.w/fabs(x1-x2));
    SG_SET(double,p->_.dy,p->m.h/fabs(y1-y2));
}

static double _sgp_interpolation(const double x1,const double y1,const double x2,double y2,const double y3){
    if(fabs( (y2-=y1) )<1.e-5) return x1;
    return x1+(y3-y1)*((x2-x1)/(y2));
}

static void _sgp_convert(const SGP * const p,double * const x, double * const y){
    *x=_sgp_interpolation(0,p->x1,p->m.w,p->x2,*x);
    *y=_sgp_interpolation(0,p->y1,p->m.h,p->y2,*y);
}

void *sgp_pixel(const SGP * const p,const int pixel_x,const int pixel_y,double * const x,double * const y){
    void * const c=sgm_at(&p->m,pixel_x,pixel_y);
    if(c){
        if(x) *x=_sgp_interpolation(p->x1,0,p->x2,p->m.w,pixel_x);
        if(y) *y=_sgp_interpolation(p->y1,0,p->y2,p->m.h,pixel_y);
        return c;
    } return NULL;
}

void sgp_point(const SGP * const p,double x,double y,const unsigned int r,const void * const c){
    _sgp_convert(p,&x,&y);
    sgm_round(&p->m,x,y,r,c);
}

void sgp_line(const SGP *p,double x1,double y1,double x2,double y2,const unsigned int rp,const void *c){
    _sgp_convert(p,&x1,&y1);
    _sgp_convert(p,&x2,&y2);
    sgm_line(&p->m,x1,y1,x2,y2,rp,c);
}

#undef SG_SET