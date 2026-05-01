/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/select.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>


static SGK _sgk_keyboard(void * const x11){
    XKeyEvent * const key=(XKeyEvent*)x11;
    KeySym k; unsigned char s[2]={0,0};
    key->state&=~(ControlMask|Mod1Mask);
    XLookupString(key,(void*)s,sizeof(s),&k,0);
    switch(k){
        #define _CASE(_f_,_t_) case XK_##_f_: return SGK_##_t_;
        _CASE(End,END);
        _CASE(Home,HOME);
        _CASE(Insert,INS);
        _CASE(Page_Up,PGU);
        _CASE(Page_Down,PGD);
        _CASE(Left,LEFT);
        _CASE(Right,RIGHT);
        _CASE(Up,UP);
        _CASE(Down,DOWN);
        _CASE(Return,ENTER);
        _CASE(Escape,ESC);
        _CASE(BackSpace,BCSP);
        _CASE(Tab,TAB);
        _CASE(Delete,DEL);
        _CASE(Shift_L,SHIFT);
        _CASE(Shift_R,SHIFT);
        _CASE(Control_L,CTRL);
        _CASE(Control_R,CTRL);
        _CASE(Alt_L,ALT);
        _CASE(Alt_R,ALT);
        _CASE(F1,F1);
        _CASE(F2,F2);
        _CASE(F3,F3);
        _CASE(F4,F4);
        _CASE(F5,F5);
        _CASE(F6,F6);
        _CASE(F7,F7);
        _CASE(F8,F8);
        _CASE(F9,F9);
        _CASE(F10,F10);
        _CASE(F11,F11);
        _CASE(F12,F12);
        #undef _CASE
        default:
            if(k>0x1F && k<256)
                return s[0];
            break;
    }
#ifdef SGK_DEBUG
    {
        extern int printf(const char *,...);
        struct keyborad_debug_is_on{char _};
        printf("x11 undefined key %d (0x%x) [%c] {%d}\n",(int)k,(int)k,s[0],key->keycode);
    }
#endif
    return 0;
}


static void _sgw_get_color_info(const XImage * const img,struct _sgw_color_info * const info){
    unsigned long tmp;
    info->pixel.bits=img->bits_per_pixel;
    info->pixel.bytes=info->pixel.bits / 8;
#define _CASE(c,v) \
    tmp=info->mask.c=img->v; info->shift.c=0; info->bits.c=0;\
    while(tmp && !(tmp & 1)){info->shift.c++; tmp>>=1;}\
    while(tmp & 1){info->bits.c++;tmp>>=1;}

    _CASE(r,red_mask)
    _CASE(g,green_mask)
    _CASE(b,blue_mask)
#undef _CASE
}

typedef struct{
    struct _sgw w;
    XImage *image;
    Display *display;
    Window window;
    Cursor cursor;
    GC gc;
    struct{
        Atom WM_PROTOCOLS;
        Atom WM_DELETE_WINDOW;
    }atom;
    int xconn, ctrl[2];
    unsigned int color_max;
    struct _sgw_color_info ci[1];
}sgw_x11;

#define SGW_UNCONST(_name_,_const_) sgw_x11 * const _name_ = (sgw_x11*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->ctrl[0]!=-1){
            close(w->ctrl[1]);
            close(w->ctrl[0]);
        }
        if(w->image){
            _w->deallocator(w->w.pixel);
            if(w->image->data!=(char*)w->w.pixel)
                _w->deallocator(w->image->data);
            w->image->data=NULL;
            XDestroyImage(w->image);
        }
        if(w->display){
            if(w->window){
                sgw_cursor(_w,1);
                XDestroyWindow(w->display,w->window);
                if(w->cursor!=None){
                    XFreeCursor(w->display,w->cursor);
                }
            }
            XCloseDisplay(w->display);
        }
        _w->deallocator(w);
    }
}

static void _sgw_size(sgw_x11 * const w){
    Window root; unsigned int border, depth;
    XGetGeometry(w->display,w->window,&root,&w->w.rectangle.x,&w->w.rectangle.y,&w->w.rectangle.w,&w->w.rectangle.h,&border,&depth);
    border>>=1; w->w.rectangle.w-=border; w->w.rectangle.h-=border;
}

static void _sgw_resize(sgw_x11 * const w){
    const unsigned int size=w->w.rectangle.w*w->w.rectangle.h;
    if(size>w->color_max){
        w->color_max=size;
        w->w.deallocator(w->w.pixel);
        w->w.deallocator(w->image->data);
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
        w->image->data=(char*)w->w.allocator(size*w->ci->pixel.bytes);
        if(!w->w.pixel || !w->image->data)
            w->color_max=0;
    }
    w->image->width=w->w.rectangle.w;
    w->image->height=w->w.rectangle.h;
    w->image->bytes_per_line=w->image->width*w->ci->pixel.bytes;
}

static char _sgw_create_cursor(sgw_x11 * const w){
    XColor c={0};
    Pixmap pm=XCreateBitmapFromData(w->display,w->window,"\x01",1,1);
    if(pm==None) return 0;
    w->cursor=XCreatePixmapCursor(w->display,pm,pm,&c,&c,0,0);
    XFreePixmap(w->display,pm);
    return w->cursor!=None;
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(!allocator){allocator=malloc;}
    if(!deallocator){deallocator=free;}{
    SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        int screen;
        memset(w,0,sizeof(*w));
        w->cursor=None;
        w->w.cursor.visible=1;
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        w->w.rectangle.flags=SGW_XYWH|SGW_MUTABLE;
        w->ctrl[0]=w->ctrl[1]=-1;
        if(pipe(w->ctrl))
            break;
        if( !(w->display=XOpenDisplay(NULL)) )
            break;
        XSynchronize(w->display,True);

        screen=DefaultScreen(w->display);
        w->xconn=XConnectionNumber(w->display);
        w->gc=DefaultGC(w->display,screen);
        if( !(w->window=XCreateSimpleWindow(w->display,RootWindow(w->display,screen),50,50,50,50,1,BlackPixel(w->display,screen),WhitePixel(w->display,screen))) )
            break;
        if( !(w->image=XCreateImage(w->display,DefaultVisual(w->display,screen),DefaultDepth(w->display,screen),ZPixmap,0,NULL,50,50,XBitmapPad(w->display),0)) )
            break;
        if( !_sgw_create_cursor(w) )
            break;
#define _SGW_ATOM(_1_) w->atom._1_=XInternAtom(w->display,#_1_,0)
        _SGW_ATOM(WM_PROTOCOLS);
        _SGW_ATOM(WM_DELETE_WINDOW);
#undef _SGW_ATOM
        XSetWMProtocols(w->display,w->window,&w->atom.WM_DELETE_WINDOW,1);
        XSelectInput(w->display,w->window,ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
        XMapRaised(w->display,w->window);

        _sgw_get_color_info(w->image,w->ci);
        _sgw_size(w);
        _sgw_resize(w);
        fcntl(w->ctrl[0],F_SETFL, (O_NONBLOCK | fcntl(w->ctrl[0],F_GETFL)) );
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char *title){
    SGW_UNCONST(w,_w);
    if(!title) title="";
    XStoreName(w->display,w->window,title);
}

void sgw_cursor(SGW * const _w,const unsigned char visible){
    SGW_UNCONST(w,_w);
    if(w->w.cursor.visible!=visible){
        if( (w->w.cursor.visible=visible) ) XUndefineCursor(w->display,w->window);
        else XDefineCursor(w->display,w->window,w->cursor);
    }
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
    const int x=write(w->ctrl[1],&p,sizeof(p));
    return; if(x){}
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
    _sgc_convert(w->w.pixel,w->image->width*w->image->height,w->ci,(unsigned char*)w->image->data);
    XSynchronize(w->display,False);
    XPutImage(w->display,w->window,w->gc,w->image,0,0,0,0,w->image->width,w->image->height);
    XSynchronize(w->display,True);
}

void sgw_rect(SGW * const _w,const enum SGW f,const int x,const int y,const unsigned int width,const unsigned int height){
    SGW_UNCONST(w,_w);
    XSizeHints hints;
    int flags=0;

    if( (f & SGW_MUTABLE) && !(w->w.rectangle.flags & SGW_MUTABLE) ){
        flags|=1;
        hints.min_width=10; hints.min_height=2; hints.max_width=hints.max_height=~(1<<(sizeof(hints.max_width)*8-1));
        w->w.rectangle.flags=(w->w.rectangle.flags & SGW_MODES) | SGW_MUTABLE;
    }

    if( (f & SGW_MODES) && (w->w.rectangle.flags & SGW_MUTABLE) ){
        if( (f & SGW_MODES)<=SGW_XYWH ){
            if(f & SGW_X) w->w.rectangle.x=x;
            if(f & SGW_Y) w->w.rectangle.y=y;
            if(f & SGW_W) w->w.rectangle.w=width;
            if(f & SGW_H) w->w.rectangle.h=height;
            flags|=2|4;
            w->w.rectangle.flags=SGW_XYWH | (w->w.rectangle.flags & SGW_STATES);
        }
    }

    if( (f & SGW_FIXED) && !(w->w.rectangle.flags & SGW_FIXED) ){
        flags|=1;
        hints.min_width=hints.max_width=w->w.rectangle.w; hints.min_height=hints.max_height=w->w.rectangle.h;
        w->w.rectangle.flags=(w->w.rectangle.flags & SGW_MODES) | SGW_FIXED;
    }

    if(flags & 1){ hints.flags=PMinSize|PMaxSize; XSetWMNormalHints(w->display,w->window,&hints); }
    if(flags & (2|1)) XMoveResizeWindow(w->display,w->window,w->w.rectangle.x,w->w.rectangle.y,w->w.rectangle.w,w->w.rectangle.h);
    if(flags & 4){ _sgw_size(w); _sgw_resize(w); }
}

static void _sgw_time_change(const struct timeval * const src,const long sec,const long usec,struct timeval * const t){
    t->tv_sec=src->tv_sec+sec;
    t->tv_usec=src->tv_usec+usec;
    t->tv_sec+=t->tv_usec/1000000;
    if( (t->tv_usec%=1000000)<0){
        t->tv_usec += 1000000;
        --t->tv_sec;
    }
}

static char _sge_fdread(const int fd,char *ptr){
    int n=sizeof(void*), c=read(fd,ptr,n);
    if(c<1) return 0;
    n-=c;
    while(n)
        if( (c=read(fd,(ptr+=c),n))>0 )
            n-=c;
    return 1;
}

static int _sge_wait(const sgw_x11 * const w,struct timeval * const t,void * const async){
    fd_set set[1];
    if(_sge_fdread(w->ctrl[0],(char*)async)) return 1;
    if(XPending(w->display)>0) return 2;
    FD_ZERO(set); FD_SET(w->xconn,set); FD_SET(w->ctrl[0],set);
    switch(select((w->xconn>w->ctrl[0]?w->xconn:w->ctrl[0])+1,set,NULL,NULL,t)){
        case -1: return -1;
        case 0: return 0;
    }
    if(FD_ISSET(w->ctrl[0],set) && _sge_fdread(w->ctrl[0],(char*)async)) return 1;
    if(XPending(w->display)>0) return 2;
    return 0;
}

enum SGE sgw_event(SGW * const _w,const int t,SGE *e){
    SGW_UNCONST(w,_w);
    struct timeval *tm,_tm, tm_stop;
    XEvent message[1];
    if(t<0){
        tm=NULL;
    }else{
        tm=&_tm;
        _tm.tv_sec=t/1000;
        _tm.tv_usec=(t%1000)*1000;
        gettimeofday(&tm_stop,NULL);
        _sgw_time_change(&tm_stop,_tm.tv_sec,_tm.tv_usec,&tm_stop);
    }
    while(1){
        switch(_sge_wait(w,tm,&e->async)){
            case -1: return (errno==EINTR) ? SGE_NONE : SGE_CLOSE;
            case 1: return SGE_ASYNC;
            case 2: do{
                XNextEvent(w->display,message);
                switch(message->type){
                    case ClientMessage:
                        if(message->xclient.message_type==w->atom.WM_PROTOCOLS &&  (Atom)(message->xclient.data.l[0])==w->atom.WM_DELETE_WINDOW)
                            return SGE_CLOSE;
                        break;
                    case MotionNotify:
                        while(XCheckTypedWindowEvent(w->display,w->window,MotionNotify,message)){}
                        w->w.cursor.x=message->xmotion.x;
                        w->w.cursor.y=message->xmotion.y;
                        return SGE_CURSOR;
                    case ConfigureNotify:
                        while(XCheckTypedWindowEvent(w->display,w->window,ConfigureNotify,message)){}
                        message->xconfigure.border_width>>=1;
                        {
                            const unsigned int sw=message->xconfigure.width-message->xconfigure.border_width, sh=message->xconfigure.height-message->xconfigure.border_width;
                            if(_w->rectangle.x==message->xconfigure.x && _w->rectangle.y==message->xconfigure.y && _w->rectangle.w==sw && _w->rectangle.h==sh) break;
                            w->w.rectangle.x=message->xconfigure.x; w->w.rectangle.y=message->xconfigure.y; w->w.rectangle.w=sw; w->w.rectangle.h=sh;
                        }
                        _sgw_resize(w);
                        return SGE_RECTANGLE;
                    case KeyPress:
                        if(_sgk_press(_sgk_keyboard(message),&w->w,e))
                            return SGE_PRESS;
                        break;
                    case KeyRelease:
                        if(_sgk_release(_sgk_keyboard(message),&w->w,e))
                            return SGE_RELEASE;
                        break;
                    case ButtonPress:
                        switch(message->xbutton.button){
                            case Button1: if(_sgk_press(SGK_LB,&w->w,e)) return SGE_PRESS; break;
                            case Button2: if(_sgk_press(SGK_MB,&w->w,e)) return SGE_PRESS; break;
                            case Button3: if(_sgk_press(SGK_RB,&w->w,e)) return SGE_PRESS; break;
                            case Button4: e->scroll=SGE_SCROLL_UP; return SGE_SCROLL;
                            case Button5: e->scroll=SGE_SCROLL_DOWN; return SGE_SCROLL;
                        } break;
                    case ButtonRelease:
                        switch(message->xbutton.button){
                            case Button1: if(_sgk_release(SGK_LB,&w->w,e)) return SGE_RELEASE; break;
                            case Button2: if(_sgk_release(SGK_MB,&w->w,e)) return SGE_RELEASE; break;
                            case Button3: if(_sgk_release(SGK_RB,&w->w,e)) return SGE_RELEASE; break;
                            case Button4: e->scroll=SGE_SCROLL_UP; return SGE_SCROLL;
                            case Button5: e->scroll=SGE_SCROLL_DOWN; return SGE_SCROLL;
                        }
                        break;
                }
            }while(XPending(w->display)>0);
        }
        if(t){
            if(tm){
                struct timeval tm_now;
                gettimeofday(&tm_now,NULL);
                _sgw_time_change(&tm_stop,-tm_now.tv_sec,-tm_now.tv_usec,tm);
                if(tm->tv_sec<0) break;
                else if(tm->tv_usec<=0) break;
            }
        }else break;
    }
    return SGE_NONE;
}

#undef SGW_UNCONST
