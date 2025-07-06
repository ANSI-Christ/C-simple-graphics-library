/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <sys/ioctl.h>
#include <sys/select.h>

#ifndef FIONREAD
    #if defined(__FreeBSD) || defined(__APPLE__)
        #include <sys/filio.h>
    #elif defined(_AIX) || defined(__hpux) || defined(__sun)
        #include <stropts.h>
    #endif
#endif

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


typedef struct{
    struct _sgw w;
    XImage *image;
    Display *display;
    Window window;
    GC gc;
    int screen;
    int xconn, ctrl[2];
    unsigned int color_max;
    unsigned char color_bytes;
    struct{
        Atom close;
    }atom;
}sgw_x11;

#define SGW_UNCONST(_name_,_const_) sgw_x11 * const _name_ = (sgw_x11*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->ctrl[0]!=-1){
            close(w->ctrl[1]);
            close(w->ctrl[0]);
        }
        if(w->display){
            if(w->window)
                XDestroyWindow(w->display,w->window);
            XCloseDisplay(w->display);
        }
        if(w->image){
            _w->deallocator(w->w.pixel);
            if(w->image->data!=(char*)w->w.pixel)
                _w->deallocator(w->image->data);
            w->image->data=NULL;
            XDestroyImage(w->image);
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
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
        if(w->color_bytes<4){
            w->w.deallocator(w->image->data);
            w->image->data=(char*)w->w.allocator(size*w->color_bytes);
        }else w->image->data=(char*)w->w.pixel;
    }
    w->image->width=w->w.rectangle.w;
    w->image->height=w->w.rectangle.h;
    w->image->bytes_per_line=w->image->width*w->color_bytes;
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(!allocator) allocator=malloc;
    if(!deallocator) deallocator=free;
{   SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        memset(w,0,sizeof(*w));
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        w->w.mode=SGW_NORMAL|SGW_MUTABLE;
        w->ctrl[0]=w->ctrl[1]=-1;
        if(pipe(w->ctrl))
            break;
        if( !(w->display=XOpenDisplay(NULL)) )
            break;
        w->screen=DefaultScreen(w->display);
        w->xconn=XConnectionNumber(w->display);
        switch( (w->w.bitness=DefaultDepth(w->display,w->screen)) ){
            case 15: case 16: w->color_bytes=2; break;
            case 24: case 32: w->color_bytes=4; break;
            default: w->color_bytes=1; break;
        }
        w->gc=DefaultGC(w->display,XDefaultScreen(w->display));
        w->atom.close=XInternAtom(w->display,"WM_DELETE_WINDOW",0);
        if( !(w->window=XCreateSimpleWindow(w->display,RootWindow(w->display,w->screen),50,50,50,50,1,BlackPixel(w->display,w->screen),WhitePixel(w->display,w->screen))) )
            break;
        if( !(w->image=XCreateImage(w->display,DefaultVisual(w->display,w->screen),w->w.bitness,ZPixmap,0,NULL,50,50,XBitmapPad(w->display),0)) )
            break;
        XSetWMProtocols(w->display,w->window,(Atom*)&w->atom,sizeof(w->atom)/sizeof(Atom));
        XSelectInput(w->display,w->window,ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
        XMapRaised(w->display,w->window);
        XFlush(w->display);
        _sgw_size(w);
        _sgw_resize(w);
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char *title){
    SGW_UNCONST(w,_w);
    XStoreName(w->display,w->window,(w->w.title=title ? title : ""));
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
    const int x=write(w->ctrl[1],&p,sizeof(p));
    return; if(x){}
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
    _sgc_convert(w->w.pixel,w->image->width*w->image->height,w->w.bitness,w->image->data);
    XPutImage(w->display,w->window,w->gc,w->image,0,0,0,0,w->image->width,w->image->height);
}

void sgw_rect(SGW * const _w,const enum SGW mode,...){
    SGW_UNCONST(w,_w);
    XSizeHints hints={.flags=PMinSize|PMaxSize};
    int flags=0;

    if( (mode & SGW_MUTABLE) && !(w->w.mode & SGW_MUTABLE) ){
        flags|=1;
        hints.min_width=10; hints.min_height=2; hints.max_width=hints.max_height=~(1<<(sizeof(hints.max_width)*8-1));
        w->w.mode=(w->w.mode & SGW_MODES) | SGW_MUTABLE;
    }

    if( (mode & SGW_MODES) && (w->w.mode & SGW_MUTABLE) ){
        if( (mode & SGW_MODES)<=SGW_XYWH ){
            va_list l;
            va_start(l,mode);
            if(mode & SGW_XY){ w->w.rectangle.x=va_arg(l,int); w->w.rectangle.y=va_arg(l,int); }
            if(mode & SGW_WH){ w->w.rectangle.w=va_arg(l,unsigned int); w->w.rectangle.h=va_arg(l,unsigned int); }
            va_end(l);
            flags|=2|4;
            w->w.mode=SGW_NORMAL | (w->w.mode & SGW_STATES);
        }
    }

    if( (mode & SGW_FIXED) && !(w->w.mode & SGW_FIXED) ){
        flags|=1;
        hints.min_width=hints.max_width=w->w.rectangle.w; hints.min_height=hints.max_height=w->w.rectangle.h;
        w->w.mode=(w->w.mode & SGW_MODES) | SGW_FIXED;
    }

    if(flags & 1) XSetWMNormalHints(w->display,w->window,&hints);
    if(flags & (2|1)) XMoveResizeWindow(w->display,w->window,w->w.rectangle.x,w->w.rectangle.y,w->w.rectangle.w,w->w.rectangle.h);
    if(flags & 4){ _sgw_size(w); _sgw_resize(w); }
}

static void _sge_unrepeat(sgw_x11 * const w,XEvent *e){
    XEvent next[1];
    while(XPending(w->display)>0){
        XPeekEvent(w->display,next);
        if(next->type!=e->type)
            break;
        XNextEvent(w->display,e);
    }
}

static int _sge_wait(const sgw_x11 * const w,const int t){
    struct timeval tm[1]={{t/1000,(t%1000)*1000}};
    fd_set set[1]; FD_ZERO(set); FD_SET(w->xconn,set); FD_SET(w->ctrl[0],set);
    switch(select((w->xconn>w->ctrl[0]?w->xconn:w->ctrl[0])+1,set,NULL,NULL,t<0 ? NULL : tm)){
        case -1: return -1;
        case 0: return 0;
    }
    if(FD_ISSET(w->ctrl[0],set)) return 1;
    if(FD_ISSET(w->xconn,set)) return 2;
    return 0;
}

enum SGE sgw_event(SGW * const _w,const int t,SGE *e){
    SGW_UNCONST(w,_w);
    int var;
    #ifdef FIONREAD
    if(!ioctl(w->ctrl[0],FIONREAD,&var) && var>0) var=1; else
    #endif
    if(XPending(w->display)>0) var=2;
    else var=_sge_wait(w,t);
    switch(var){
        case -1: return SGE_CLOSE;
        case 1:{
            const int bytes=read(w->ctrl[0],&e->async,sizeof(e->async));
            return SGE_ASYNC; if(bytes){} break;
        }
        case 2: do{
            XEvent message[1];
            XNextEvent(w->display,message);
            switch(message->type){
                case ClientMessage:
                    if((Atom)(message->xclient.data.l[0])==w->atom.close)
                        return SGE_CLOSE;
                    break;
                case MotionNotify:
                    _sge_unrepeat(w,message);
                    w->w.cursor.x=message->xmotion.x;
                    w->w.cursor.y=message->xmotion.y;
                    return SGE_CURSOR;
                case ConfigureNotify:
                    _sge_unrepeat(w,message);
                    if(message->xconfigure.send_event) break;
                    message->xconfigure.border_width>>=1;
                    {const int tmp[4]={message->xconfigure.x,message->xconfigure.y,message->xconfigure.width-message->xconfigure.border_width,message->xconfigure.height-message->xconfigure.border_width};
                    if(!memcmp(&w->w.rectangle,tmp,sizeof(_w->rectangle))) break;
                    memcpy(&w->w.rectangle,tmp,sizeof(_w->rectangle));}
                    _sgw_resize(w);
                    return SGE_RECTANGLE;
                case KeyPress:
                    if(_sgk_press(_sgk_keyboard(message),&w->w.keys,&e->key))
                        return SGE_PRESS;
                    break;
                case KeyRelease:
                    if(_sgk_release(_sgk_keyboard(message),&w->w.keys,&e->key))
                        return SGE_RELEASE;
                    break;
                case ButtonPress:
                    switch(message->xbutton.button){
                        case Button1: if(_sgk_press(SGK_LB,&w->w.keys,&e->key)) return SGE_PRESS; break;
                        case Button2: if(_sgk_press(SGK_MB,&w->w.keys,&e->key)) return SGE_PRESS; break;
                        case Button3: if(_sgk_press(SGK_RB,&w->w.keys,&e->key)) return SGE_PRESS; break;
                        case Button4: e->scroll=SGE_SCROLL_UP; return SGE_SCROLL;
                        case Button5: e->scroll=SGE_SCROLL_DOWN; return SGE_SCROLL;
                    } break;
                case ButtonRelease:
                    switch(message->xbutton.button){
                        case Button1: if(_sgk_release(SGK_LB,&w->w.keys,&e->key)) return SGE_RELEASE; break;
                        case Button2: if(_sgk_release(SGK_MB,&w->w.keys,&e->key)) return SGE_RELEASE; break;
                        case Button3: if(_sgk_release(SGK_RB,&w->w.keys,&e->key)) return SGE_RELEASE; break;
                        case Button4: e->scroll=SGE_SCROLL_UP; return SGE_SCROLL;
                        case Button5: e->scroll=SGE_SCROLL_DOWN; return SGE_SCROLL;
                    }
                    break;
            }
        }while(XPending(w->display)>0);
    }
    return SGE_NONE;
}

#undef SGW_UNCONST
