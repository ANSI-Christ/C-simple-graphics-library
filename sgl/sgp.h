/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_PAINT
#define SG_PAINT

typedef struct{
    const char *bits;
    unsigned short bpw, bph;
}const SGB;

typedef struct{
    SGB *bm;
    unsigned short w,h;
    unsigned short gap_w, gap_h;
}SGF;

enum SGF_ALIGN{
    SGF_XL = 0,
    SGF_XC = 1,
    SGF_XR = 2,
    SGF_YT = 0,
    SGF_YC = 4,
    SGF_YB = 8,
    SGF_DEFAULT = SGF_XL | SGF_YT
};

extern const SGF sgf_5x12;
extern const SGF *sgf_default;

void sgf_string_rect(const SGF *f,const char *s,unsigned int *w,unsigned int *h);
const char *sgf_string_at(const SGF *f,enum SGF_ALIGN a,const char *s,int x,int y);




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
void sgm_ellipse(const SGM * const m,const int x,const int y,const unsigned int rx,const unsigned int ry,const unsigned int rp,const void * const c);

void sgm_arc_cirlce(const SGM * const m,const int x,const int y,unsigned int r,const unsigned int rp,const double ang,const double rot,const void * const c);
void sgm_arc_ellipse(const SGM * const m,const int x,const int y,const unsigned int rx,const unsigned int ry,const unsigned int rp,const double ang,const double rot,const void * const c);

void sgm_string(const SGM * const m,int x,int y,const void *c,const SGF *f,enum SGF_ALIGN a,const char *s);


typedef struct{
    SGM m;
    const double x1,y1;
    const double x2,y2;
    const struct{double dx,dy;}_;
}SGP;

void sgp_cfg(SGP *p,const SGM *m,unsigned char default_box);
void sgp_init(SGP *p,void *c,unsigned int w,unsigned int h,unsigned int color_bytes,unsigned char default_box);

void sgp_box(SGP *p,double left,double top,double right,double bottom);

void *sgp_pixel(const SGP *p,int pixel_x,int pixel_y,double *x,double *y);

void sgp_point(const SGP *p,double x,double y,unsigned int r,const void *c);


#endif /* SG_PAINT */
