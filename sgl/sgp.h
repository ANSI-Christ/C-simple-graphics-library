/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_PAINT
#define SG_PAINT

enum SGA{
    SGA_XL = 1,
    SGA_XC = 2,
    SGA_XR = 4,
    SGA_YB = 8,
    SGA_YC = 16,
    SGA_YT = 32,
    SGA_DEFAULT = SGA_XL | SGA_YT
};


typedef struct{
    const char *bits;
    unsigned short bpw, bph;
}const SGB;


typedef struct{
    SGB *bm;
    unsigned short w,h;
    unsigned short gap_w, gap_h;
}SGF;

extern const SGF sgf_5x12;
extern const SGF *sgf_default;


typedef struct{
    char * const data;
    const int x, y;
    const unsigned int w, h;
    const unsigned int color_bytes;
    enum{ SGM_UNLIMITED=1 } flags;
    const struct{unsigned int x,y,w[2],h[2];}_;
}SGM;


void sgm_cfg(SGM *m,void *c,unsigned int w,unsigned int h,unsigned int color_bytes);
void sgm_sub(const SGM *m,int x,int y,unsigned int w,unsigned int h,unsigned char flags,SGM *s);

void *sgm_at(const SGM *m,int x,int y);

void sgm_convert(const SGM *m,const SGM *c,char (*converter)(const void *from,void *to,const void *arg),const void *arg);
void sgm_paste(const SGM *m,int x,int y,const SGM *p,char (*converter)(const void *from,void *to,const void *arg),const void *arg);

void sgm_set(const SGM *m,int x,int y,const void *c);
void sgm_swap(const SGM *m,int x,int y,const void *c1, const void *c2);

void sgm_row(const SGM *m,int x,int y,unsigned int l,const void *c);
void sgm_column(const SGM *m,int x,int y,unsigned int l,const void *c);

void sgm_line(const SGM *m,int x1,int y1,int x2,int y2,const void *c);
void sgm_fill(const SGM *m,int x,int y,const void *c,const void *border);

void sgm_rect(const SGM *m,int x,int y,unsigned int w,unsigned int h,const void *c);
void sgm_square(const SGM *m,int x,int y,unsigned int w,unsigned int h,const void *c);

void sgm_round(const SGM *m,int x,int y,unsigned int r,const void *c);
void sgm_circle(const SGM *m,int x,int y,unsigned int r,unsigned int rp,const void *c);

void sgm_oval(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const void * const c);
void sgm_ellipse(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const unsigned int rp,const void * const c);

void sgm_arc_cirlce(const SGM * const m,const int x,const int y,unsigned int r,const unsigned int rp,const double ang,const double rot,const void * const c);
void sgm_arc_ellipse(const SGM * const m,const int x,const int y,const unsigned int rw,const unsigned int rh,const unsigned int rp,const double ang,const double rot,const void * const c);

void sgm_string(const SGM * const m,int x,int y,const void *c,const SGF *f,enum SGA a,const char *s);

#endif /* SG_PAINT */
