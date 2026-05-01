/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <pthread.h>

#include <windows.h>
#include <windowsx.h>

static SGK _sgk_keyboard(const MSG * const msg){
    switch(msg->wParam){
        #define _CASE(_f_,_t_) case VK_##_f_: return SGK_##_t_;
        _CASE(END,END);
        _CASE(HOME,HOME);
        _CASE(LEFT,LEFT);
        _CASE(RIGHT,RIGHT);
        _CASE(UP,UP);
        _CASE(DOWN,DOWN);
        _CASE(RETURN,ENTER);
        _CASE(ESCAPE,ESC);
        _CASE(BACK,BCSP);
        _CASE(TAB,TAB);
        _CASE(DELETE,DEL);
        _CASE(SHIFT,SHIFT);
        _CASE(CONTROL,CTRL);
        _CASE(MENU,ALT);
        _CASE(PRIOR,PGU);
        _CASE(NEXT,PGD);
        _CASE(INSERT,INS);
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
        default:{
            BYTE s[256];
            if(GetKeyboardState(s)){
                WORD k;
                s[VK_CONTROL]=s[VK_MENU]=0;
                if(ToAscii(msg->wParam,msg->lParam,s,&k,0)==1 && LOBYTE(k)>0x1F)
                    return LOBYTE(k);
            }
        }
    }
#ifdef SGK_DEBUG
    {
        extern int printf(const char *,...);
        struct keyborad_debug_is_on{char _};
        printf("winapi undefined key %d (0x%x)\n",(int)msg->wParam,(int)msg->wParam);
    }
#endif
    return 0;
}

static void _sgw_get_color_info(const BITMAPINFO * const bm,struct _sgw_color_info * const info){
    info->pixel.bits=bm->bmiHeader.biBitCount;
    info->pixel.bytes=info->pixel.bits / 8;
    info->mask.r=0xFF0000; info->shift.r=16;
    info->mask.g=0x00FF00; info->shift.g=8;
    info->mask.b=0x0000FF; info->shift.b=0;
    info->bits.r=info->bits.g=info->bits.b=8;
}


typedef struct{
    struct _sgw w;
    void *local_buffer;
    HWND window;
    HCURSOR cursor[2];
    HDC dc;
    UINT message;
    BITMAPINFO bmi[1];
    struct _sgw_color_info ci[1];
    unsigned int color_max;
}sgw_win;

#define SGW_UNCONST(_name_,_const_) sgw_win * const _name_ = (sgw_win*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

#define WM_ASYNC_POINTER (WM_USER+1)

#define SGW_CLASS_NAME "SGW_CLASS"
#define SGW_STYLE (WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME)
static pthread_once_t _sgw_once=PTHREAD_ONCE_INIT;

static LRESULT CALLBACK _sgw_WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_CREATE: SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)((LPCREATESTRUCTA)lParam)->lpCreateParams); break;
        case WM_DESTROY: KillTimer(hWnd,1); break;
        case WM_CLOSE: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message=message; return (LRESULT)0;
        case WM_SYSKEYUP: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message=WM_KEYUP; return (LRESULT)0;
        case WM_SYSKEYDOWN: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message=WM_KEYDOWN; return (LRESULT)0;
        case WM_MOVE:
        case WM_SIZE: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message=message; break;
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

static void _sgw_class_close(void){
    UnregisterClassA(SGW_CLASS_NAME,0);
}

static void _sgw_class_init(void){
    extern int atexit(void(*)(void));
    {
        WNDCLASSA wc;
        memset(&wc,0,sizeof(wc));
        wc.lpfnWndProc=_sgw_WndProc;
        wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName=SGW_CLASS_NAME;
        if(RegisterClassA(&wc))
            atexit(_sgw_class_close);
    }
}

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->window){
            sgw_cursor(_w,1);
            ReleaseDC(w->window,w->dc);
            DestroyWindow(w->window);
        }
        if(w->cursor[0]){
            DestroyCursor(w->cursor[0]);
        }
        _w->deallocator(w->w.pixel);
        if(w->local_buffer!=(void*)w->w.pixel)
            _w->deallocator(w->local_buffer);
        _w->deallocator(w);
    }
}

static void _sgw_size(sgw_win * const w){
    RECT r; POINT p={0,0};
    GetClientRect(w->window,&r);
    w->w.rectangle.w=r.right; w->w.rectangle.h=r.bottom;
    ClientToScreen(w->window,&p);
    w->w.rectangle.x=p.x; w->w.rectangle.y=p.y;
}

static void _sgw_resize(sgw_win * const w){
    const unsigned int size=w->w.rectangle.w*w->w.rectangle.h;
    if(size>w->color_max){
        w->color_max=size;
        w->w.deallocator(w->w.pixel);
        w->w.deallocator(w->local_buffer);
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
        w->local_buffer=w->w.allocator(size*w->ci->pixel.bytes);
        if(!w->w.pixel || !w->local_buffer)
            w->color_max=0;
    }
    w->bmi->bmiHeader.biWidth=w->w.rectangle.w;
    w->bmi->bmiHeader.biHeight=-w->w.rectangle.h;
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(!allocator){allocator=malloc;}
    if(!deallocator){deallocator=free;}{
    SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        memset(w,0,sizeof(*w));
        w->w.cursor.visible=1;
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        w->w.rectangle.flags=SGW_XYWH|SGW_MUTABLE;
        pthread_once(&_sgw_once,_sgw_class_init);
        if( !(w->window=CreateWindowA(SGW_CLASS_NAME," ",SGW_STYLE,50,50,100,100,NULL,NULL,NULL,w)) )
            break;
        if( !(w->cursor[0]=CreateCursor(NULL, 0,0, 1,1, "\xff","\x00")) )
            break;
        w->dc=GetDC(w->window);
        UpdateWindow(w->window);
        w->bmi->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        w->bmi->bmiHeader.biCompression=BI_RGB;
        w->bmi->bmiHeader.biPlanes=1;
        w->bmi->bmiHeader.biBitCount=32;
        w->cursor[1]=(HCURSOR)GetClassLongPtr(w->window,GCLP_HCURSOR);
        _sgw_get_color_info(w->bmi,w->ci);
        _sgw_size(w);
        _sgw_resize(w);
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char *title){
    SGW_UNCONST(w,_w);
    if(!title) title="";
    SetWindowTextA(w->window,title);
}

void sgw_cursor(SGW * const _w,const unsigned char visible){
    SGW_UNCONST(w,_w);
    if(w->w.cursor.visible!=visible){
        SetClassLongPtr(w->window,GCLP_HCURSOR,(LONG_PTR)w->cursor[(w->w.cursor.visible=visible)]);
        PostMessage(w->window, WM_SETCURSOR, (WPARAM)w->window, MAKELPARAM(HTCLIENT, 0));
    }
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
    PostMessage(w->window,WM_ASYNC_POINTER,(WPARAM)0,(LPARAM)p);
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
    const unsigned int width=_w->rectangle.w;
    const unsigned int height=_w->rectangle.h;
    _sgc_convert(w->w.pixel,width*height,w->ci,(unsigned char*)w->local_buffer);
    SetDIBitsToDevice(w->dc, 0,0, width,height, 0,0, 0,height, w->local_buffer, w->bmi, DIB_RGB_COLORS);
}

void sgw_rect(SGW * const _w,const enum SGW f,const int x,const int y,const unsigned int width,const unsigned int height){
    SGW_UNCONST(w,_w);
    int flags=0;

    if( (f & SGW_MUTABLE) && !(w->w.rectangle.flags & SGW_MUTABLE) ){
        flags^=1;
        SetWindowLongPtr(w->window,GWL_STYLE,GetWindowLongPtr(w->window,GWL_STYLE) | (WS_MAXIMIZEBOX|WS_THICKFRAME));
        w->w.rectangle.flags=(w->w.rectangle.flags & SGW_MODES) | SGW_MUTABLE;
    }

    if( (f & SGW_MODES) && (w->w.rectangle.flags & SGW_MUTABLE) ){
        if( (f & SGW_MODES)<=SGW_XYWH ){
            if(f & SGW_X) w->w.rectangle.x=x;
            if(f & SGW_Y) w->w.rectangle.y=y;
            if(f & SGW_W) w->w.rectangle.w=width;
            if(f & SGW_H) w->w.rectangle.h=height;
            flags|=2;
            w->w.rectangle.flags=SGW_XYWH | (w->w.rectangle.flags & SGW_STATES);
        }
    }

    if( (f & SGW_FIXED) && !(w->w.rectangle.flags & SGW_FIXED) ){
        flags^=1;
        SetWindowLongPtr(w->window,GWL_STYLE,GetWindowLongPtr(w->window,GWL_STYLE) & ~(WS_MAXIMIZEBOX|WS_THICKFRAME));
        w->w.rectangle.flags=(w->w.rectangle.flags & SGW_MODES) | SGW_FIXED;
    }

    if(flags){
        RECT r={0,0, w->w.rectangle.w,w->w.rectangle.h};
        AdjustWindowRectEx(&r,GetWindowLongPtr(w->window,GWL_STYLE),FALSE,GetWindowLongPtr(w->window,GWL_EXSTYLE));
        r.right-=r.left; r.bottom-=r.top; r.left+=w->w.rectangle.x; r.top+=w->w.rectangle.y;
        if(flags & 1) SetWindowPos(w->window,0,r.left+1,r.top,r.right+1,r.bottom+1,SWP_FRAMECHANGED);
        SetWindowPos(w->window,0,r.left,r.top,r.right,r.bottom,0);
        _sgw_size(w); _sgw_resize(w);
    }

}

static void _sge_unrepeat(sgw_win * const w,MSG *e){
    MSG next[1];
    while(PeekMessage(next,w->window,0,0,PM_NOREMOVE)){
        if(next->message!=e->message)
            break;
        GetMessage(e,w->window,0,0);
    }
}

enum SGE sgw_event(SGW * const _w,const int t,SGE * const e){
#define _SGW_RETURN_IF(_cond_,_val_) if(_cond_){SetTimer(w->window,(UINT_PTR)1,USER_TIMER_MAXIMUM,0); return _val_;} break
    SGW_UNCONST(w,_w);
    MSG message[1];
    SetTimer(w->window,(UINT_PTR)1,t>0?t:USER_TIMER_MAXIMUM,0);
    while(1){
        if(t) GetMessage(message,w->window,0,0);
        else if(!PeekMessage(message,w->window,0,0,PM_REMOVE))
            break;
        w->message=WM_NULL;
        DispatchMessage(message);
        if(w->message!=WM_NULL)
            message->message=w->message;
        switch(message->message){
            case WM_CLOSE:
                _SGW_RETURN_IF(1, SGE_CLOSE);
            case WM_TIMER:
                SetTimer(w->window,(UINT_PTR)1,USER_TIMER_MAXIMUM,0);
                if(t<0) break;
                return SGE_NONE;
            case WM_ASYNC_POINTER:
                e->async=(void*)message->lParam;
                _SGW_RETURN_IF(1, SGE_ASYNC);
            case WM_MOVE:
            case WM_SIZE:
                _sgw_size(w); _sgw_resize(w);
                _SGW_RETURN_IF(1, SGE_RECTANGLE);
            case WM_MOUSEMOVE:
                _sge_unrepeat(w,message);
                w->w.cursor.x=GET_X_LPARAM(message->lParam);
                w->w.cursor.y=GET_Y_LPARAM(message->lParam);
                _SGW_RETURN_IF(1, SGE_CURSOR);
            case WM_MOUSEWHEEL:
                e->scroll=(GET_WHEEL_DELTA_WPARAM(message->wParam)>0 ? SGE_SCROLL_UP : SGE_SCROLL_DOWN);
                _SGW_RETURN_IF(1,SGE_SCROLL);
            case WM_LBUTTONDOWN:  _SGW_RETURN_IF(_sgk_press(SGK_LB,&w->w,e), SGE_PRESS);
            case WM_LBUTTONUP:    _SGW_RETURN_IF(_sgk_release(SGK_LB,&w->w,e), SGE_RELEASE);
            case WM_RBUTTONDOWN:  _SGW_RETURN_IF(_sgk_press(SGK_RB,&w->w,e), SGE_PRESS);
            case WM_RBUTTONUP:    _SGW_RETURN_IF(_sgk_release(SGK_RB,&w->w,e), SGE_RELEASE);
            case WM_MBUTTONDOWN:  _SGW_RETURN_IF(_sgk_press(SGK_MB,&w->w,e), SGE_PRESS);
            case WM_MBUTTONUP:    _SGW_RETURN_IF(_sgk_release(SGK_MB,&w->w,e), SGE_RELEASE);
            case WM_KEYDOWN:      _SGW_RETURN_IF(_sgk_press(_sgk_keyboard(message),&w->w,e), SGE_PRESS);
            case WM_KEYUP:        _SGW_RETURN_IF(_sgk_release(_sgk_keyboard(message),&w->w,e), SGE_RELEASE);
        }
    }
    SetTimer(w->window,(UINT_PTR)1,t>0?t:USER_TIMER_MAXIMUM,0);
    return SGE_NONE;
#undef _SGW_RETURN_IF
}

#undef SGW_CLASS_NAME
#undef SGW_STYLE
#undef SGW_UNCONST
