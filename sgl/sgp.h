/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_PAINT
#define SG_PAINT

typedef struct{
    const char *data;
    unsigned char begin, end;
    unsigned char bpw, bph;
}SGB;

typedef struct{
    const SGB *bitmap;
    unsigned short w, h;
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

extern const SGF sgf_5x11;
extern const SGF *sgf_default;

void sgf_rect(const SGF *f,const char *s,unsigned int *w,unsigned int *h);
const char *sgf_at(const SGF *f,enum SGF_ALIGN a,const char *s,int x,int y);



enum SGM{
    SGM_UNLIMITED=1
};

typedef struct{
    char * const data;
    const int x, y;
    const unsigned int w, h;
    const unsigned int color_bytes;
    enum SGM flags;
    const struct{unsigned int x,y,w[2],h[2];}_;
}SGM;

void sgm_cfg(SGM *m,void *c,unsigned int w,unsigned int h,unsigned int color_bytes);
void sgm_sub(const SGM *m,int x,int y,unsigned int w,unsigned int h,enum SGM flags,SGM *s);

void *sgm_at(const SGM *m,int x,int y);

int sgm_convert(const SGM *m,const SGM *c,char (*converter)(const void *from,void *to,void *arg),void *arg);
int sgm_paste(const SGM *m,int x,int y,const SGM *p,char (*converter)(const void *from,void *to,void *arg),void *arg);

void sgm_set(const SGM *m,int x,int y,const void *c);
void sgm_swap(const SGM *m,int x,int y,const void *c1, const void *c2);

void sgm_row(const SGM *m,int x,int y,unsigned int l,const void *c);
void sgm_column(const SGM *m,int x,int y,unsigned int l,const void *c);

void sgm_fill(const SGM *m,int x,int y,const void *c,const void *border);
void sgm_line(const SGM *m,int x1,int y1,int x2,int y2,unsigned int t,const void *c);

void sgm_rect(const SGM *m,int x,int y,unsigned int w,unsigned int h,unsigned int t,const void *c);
void sgm_square(const SGM *m,int x,int y,unsigned int w,unsigned int h,const void *c);

void sgm_round(const SGM *m,int x,int y,unsigned int r,const void *c);
void sgm_circle(const SGM *m,int x,int y,unsigned int r,unsigned int t,const void *c);

void sgm_oval(const SGM *m,int x,int y,unsigned int rx,unsigned int ry,const void *c);
void sgm_ellipse(const SGM *m,int x,int y,unsigned int rx,unsigned int ry,unsigned int t,const void *c);

void sgm_arc_cirlce(const SGM *m,int x,int y,unsigned int r,unsigned int t,double ang,double rot,const void *c);
void sgm_arc_ellipse(const SGM *m,int x,int y,unsigned int rx,unsigned int ry,unsigned int t,double ang,double rot,const void *c);

void sgm_bmp(const SGM *m,const char *name);
void sgm_string(const SGM * const m,int x,int y,const void *c,const SGF *f,enum SGF_ALIGN a,const char *s);




enum SGP{
    SGP_COPY = 0,
    SGP_ATTACH = 1
};

typedef struct{
    SGM * const m;
    const double x1,y1;
    const double x2,y2;
    const struct{SGM m;double dx,dy;}_;
}SGP;

typedef struct{
    const char *format;
    unsigned int division;
}SGA;



void sgp_cfg(SGP *p,enum SGP mode,const SGM *m,unsigned char default_box);
void sgp_init(SGP *p,void *c,unsigned int w,unsigned int h,unsigned int color_bytes,unsigned char default_box);

void sgp_set(const SGP *p,double x,double y,const void *c);
void sgp_box(SGP *p,double left,double top,double right,double bottom);

void *sgp_at(const SGP *p,double x,double y);
void *sgp_pixel(const SGP *p,int pixel_x,int pixel_y,double *x,double *y);

void sgp_point(const SGP *p,double x,double y,unsigned int r,const void *c);
void sgp_line(const SGP *p,double x1,double y1,double x2,double y2,unsigned int t,const void *c);

void sgp_round(const SGP *p,double x,double y,double r,const void *c);
void sgp_circle(const SGP *p,double x,double y,double r,unsigned int t,const void *c);

void sgp_oval(const SGP *p,double x,double y,double rx,double ry,const void *c);
void sgp_ellipse(const SGP *p,double x,double y,double rx,double ry,unsigned int t,const void *c);

void sgp_arc_cirlce(const SGP *p,double x,double y,double r,unsigned int t,double ang,double rot,const void *c);
void sgp_arc_ellipse(const SGP *p,double x,double y,double rx,double ry,unsigned int t,double ang,double rot,const void *c);

void sgp_string(const SGP *p,double x,double y,const void *c,const SGF *f,enum SGF_ALIGN a,const char *s);

void sgp_plot(const SGP *p,SGA x,SGA y,const SGF *f,const void *c1,const void *c2,SGP *plot);

#endif /* SG_PAINT */
