/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <unistd.h>
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
            unsigned char s[256];
            if(GetKeyboardState(s)){
                unsigned char k[2];
                s[VK_CONTROL]=s[VK_MENU]=0;
                if(ToAscii(msg->wParam,msg->lParam,s,k,0)==1 && k[0]>0x20)
                    return k[0];
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

typedef struct{
    struct _sgw w;
    void *local_buffer;
    unsigned int add_w,add_h;
    unsigned int color_max;
    unsigned char color_bytes;
    HWND window;
    HDC dc;
    UINT message;
    BITMAPINFO bmi[1];
}sgw_win;

#define SGW_UNCONST(_name_,_const_) sgw_win * const _name_ = (sgw_win*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

#define WM_ASYNC_POINTER (WM_USER+1)
#define WM_MINIMIZE (WM_USER+2)

#define SGW_CLASS_NAME L"SGW_CLASS"
static pthread_once_t _sgw_once=PTHREAD_ONCE_INIT;

static LRESULT CALLBACK _sgw_WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_CREATE: SetWindowLongPtr(hWnd,GWLP_USERDATA,((LPCREATESTRUCTA)lParam)->lpCreateParams); break;
        case WM_DESTROY: KillTimer(hWnd,1); break;
        case WM_CLOSE: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message=message; return (LRESULT)0;
        case WM_MOVE:
        case WM_SIZE: ((sgw_win*)GetWindowLongPtr(hWnd,GWLP_USERDATA))->message = (wParam==SIZE_MINIMIZED ? WM_MINIMIZE : message); break;
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

static void _sgw_class_close(void){
    UnregisterClass(SGW_CLASS_NAME,0);
}

static void _sgw_class_init(void){
    extern int atexit(void(*)(void));
    {
        WNDCLASS wc;
        memset(&wc,0,sizeof(wc));
        wc.lpfnWndProc=_sgw_WndProc;
        wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName=SGW_CLASS_NAME;
        if(RegisterClass(&wc))
            atexit(_sgw_class_close);
    }
}

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->window){
            ReleaseDC(w->window,w->dc);
            DestroyWindow(w->window);
        }
        _w->deallocator(w->w.pixel);
        if(w->local_buffer!=(void*)w->w.pixel)
            _w->deallocator(w->local_buffer);
        _w->deallocator(w);
    }
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(!allocator) allocator=malloc;
    if(!deallocator) deallocator=free;
{   SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        const int style=WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
        RECT rw[1],rc[1];
        memset(w,0,sizeof(*w));
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        pthread_once(&_sgw_once,_sgw_class_init);
        if( !(w->window=CreateWindow(SGW_CLASS_NAME,L" ",style,50,50,100,100,NULL,NULL,NULL,w)) )
            break;
        w->dc=GetDC(w->window);
        switch( (w->w.bitness=GetDeviceCaps(w->dc,BITSPIXEL)) ){
            case 15: case 16: w->color_bytes=2; break;
            case 24: case 32: w->color_bytes=4; break;
            default: w->color_bytes=1; break;
        }
        GetWindowRect(w->window,rw);
        GetClientRect(w->window,rc);
        w->add_w=(rw->right-rw->left)-(rc->right-rc->left);
        w->add_h=(rw->bottom-rw->top)-(rc->bottom-rc->top);
        UpdateWindow(w->window);
        w->bmi->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        w->bmi->bmiHeader.biCompression=BI_RGB;
        w->bmi->bmiHeader.biPlanes=1;
        w->bmi->bmiHeader.biBitCount=w->w.bitness;
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char *title){
    SGW_UNCONST(w,_w);
    SetWindowTextA(w->window,(w->w.title=title ? title : ""));
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
    PostMessage(w->window,WM_ASYNC_POINTER,(WPARAM)0,(LPARAM)p);
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
    const unsigned int width=w->bmi->bmiHeader.biWidth;
    const unsigned int height=-w->bmi->bmiHeader.biHeight;
    _sgc_convert(w->w.pixel,width*height,w->w.bitness,w->local_buffer);
    SetDIBitsToDevice(w->dc, 0,0, width,height, 0,0, 0,height, w->local_buffer, w->bmi, DIB_RGB_COLORS);
}

static void _sgw_resize(sgw_win * const w,const unsigned int width,const unsigned int height){
    const unsigned int size=width*height;
    if(size>w->color_max){
        w->color_max=size;
        w->w.deallocator(w->w.pixel);
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
        if(w->color_bytes<4){
            w->w.deallocator(w->local_buffer);
            w->local_buffer=w->w.allocator(size*w->color_bytes);
        }else w->local_buffer=w->w.pixel;
    }
    w->bmi->bmiHeader.biWidth=w->w.rectangle.w=width;
    w->bmi->bmiHeader.biHeight=w->w.rectangle.h=-height;
}

void sgw_rect(SGW * const _w,const int x,const int y,const unsigned int width,const unsigned int height){
    SGW_UNCONST(w,_w);
    _sgw_resize(w,width,height);
    SetWindowPos(w->window,0,(w->w.rectangle.x=x),(w->w.rectangle.y=y),width+w->add_w,height+w->add_h,0);
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
    SGW_UNCONST(w,_w);
    MSG message[1];
    if(t<0){
        SetTimer(w->window,(UINT_PTR)1,USER_TIMER_MAXIMUM,0);
        GetMessage(message,w->window,0,0);
    }else if(t>0){
        SetTimer(w->window,(UINT_PTR)1,t,0);
        GetMessage(message,w->window,0,0);
        SetTimer(w->window,(UINT_PTR)1,USER_TIMER_MAXIMUM,0);
    }else if(!PeekMessage(message,w->window,0,0,PM_REMOVE))
        return SGE_NONE;

    w->message=WM_NULL;
    DispatchMessage(message);
    if(w->message!=WM_NULL)
        message->message=w->message;

    switch(message->message){
        case WM_CLOSE:
            return SGE_CLOSE;
        case WM_TIMER:
            if(t>0) return SGE_TIMEOUT;
            return SGE_UNKNOWN;
        case WM_ASYNC_POINTER:
            e->async=(void*)message->lParam;
            return SGE_ASYNC;
        case WM_MOVE:
        case WM_SIZE:{
            RECT r[1]; GetWindowRect(w->window,r);
            _sgw_resize(w,r->right-w->add_w-(w->w.rectangle.x=r->left),r->bottom-w->add_h-(w->w.rectangle.y=r->top));
        }   return SGE_RECTANGLE;
        case WM_MINIMIZE:
            w->w.rectangle.x=w->w.rectangle.y=-100;
            w->w.rectangle.w=w->w.rectangle.h=0;
            return SGE_RECTANGLE;
        case WM_MOUSEMOVE:
            _sge_unrepeat(w,message);
            w->w.cursor.x=GET_X_LPARAM(message->lParam);
            w->w.cursor.y=GET_Y_LPARAM(message->lParam);
            return SGE_CURSOR;
        case WM_MOUSEWHEEL:
            e->scroll=(GET_WHEEL_DELTA_WPARAM(message->wParam)>0 ? SGE_SCROLL_UP : SGE_SCROLL_DOWN);
            return SGE_SCROLL;
        case WM_LBUTTONDOWN: return _sgk_press(SGK_LB,&w->w.keys,&e->key);
        case WM_LBUTTONUP:   return _sgk_release(SGK_LB,&w->w.keys,&e->key);
        case WM_RBUTTONDOWN: return _sgk_press(SGK_RB,&w->w.keys,&e->key);
        case WM_RBUTTONUP:   return _sgk_release(SGK_RB,&w->w.keys,&e->key);
        case WM_MBUTTONDOWN: return _sgk_press(SGK_MB,&w->w.keys,&e->key);
        case WM_MBUTTONUP:   return _sgk_release(SGK_MB,&w->w.keys,&e->key);
        case WM_SYSKEYDOWN:  return _sgk_press(SGK_ALT,&w->w.keys,&e->key);
        case WM_SYSKEYUP:    return _sgk_release(SGK_ALT,&w->w.keys,&e->key);
        case WM_KEYDOWN:     return _sgk_press(_sgk_keyboard(message),&w->w.keys,&e->key);
        case WM_KEYUP:       return _sgk_release(_sgk_keyboard(message),&w->w.keys,&e->key);
    }
    return SGE_UNKNOWN;
}

#undef SGW_CLASS_NAME
#undef SGW_UNCONST
