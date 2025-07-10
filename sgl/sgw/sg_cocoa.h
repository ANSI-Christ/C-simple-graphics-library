/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <limits.h>
//#include <objc/message.h>
//#include <objc/runtime.h>

typedef void* Ivar;
typedef void* OBJC_ID;
typedef void* OBJC_SEL;
typedef void* OBJC_CLASS;

extern Ivar class_getClassVariable(OBJC_CLASS,const char*);
extern size_t ivar_getOffset(Ivar);
extern OBJC_ID objc_msgSend(OBJC_ID, OBJC_SEL, ...);
extern OBJC_SEL sel_registerName(const char *);
extern OBJC_CLASS objc_getClass(const char *);

static void * const _objc_msgSend_ptr=objc_msgSend;

static int _objc_load_constant(OBJC_CLASS cls,const char * const name,size_t *const var){
    Ivar ivar=class_getClassVariable(cls,name);
    if(!ivar) return -1;
    *var=*(size_t*)((const char*)cls+ivar_getOffset(ivar));
    return 0;
}

#define OBJC_GET(...)    objc_msgSend_stret(__VA_ARGS__)
#define OBJC_MSGT(_t_,...)  ((_t_(*)(OBJC_ID, OBJC_SEL, ...))_objc_msgSend_ptr)(__VA_ARGS__)
#define OBJC_MSG(...)    OBJC_MSGT(OBJC_ID,__VA_ARGS__)
#define OBJC_VAR(...)    _objc_load_constant(__VA_ARGS__)
#define OBJC_SEL(...)    sel_registerName(__VA_ARGS__)
#define OBJC_CLASS(...)  objc_getClass(__VA_ARGS__)

//#include <AppKit/NSEvent.h>
//#include <AppKit/NSWindow.h>


#if SIZE_MAX==UINT_MAX
typedef float CGFloat;
#else
typedef double CGFloat;
#endif

typedef struct{
    CGFloat x,y;
}NSPoint;

typedef struct{
    CGFloat width,height;
}NSSize;

typedef struct{
    NSPoint origin;
    NSSize size;
}NSRect;

/* **********************************************************
********************************************************** */

static struct{

    struct{
      OBJC_ID
        app,
        loop;
    }id;

    struct{
      OBJC_SEL
        alloc,
        init,
        string,
        title,
        show,
        close,
        origin_x,
        origin_y,
        size_w,
        size_h,
        cursor_x,
        cursor_y,
        frame,
        setFrame,
        initWithRect,
        valueWithRect,
        nextEvent,
        sendEvent,
        application,
        timeout,
        eventType,
        uint;
    }sel;

    struct{
      OBJC_CLASS
        Window,
        Event,
        String,
        Value,
        App,
        Date,
        Loop;
    }cls;

    struct{
      size_t
        NSBackingStoreBuffered,
        NSBackingStoreRetained,
        NSBackingStoreNonretained;
    }buffering;

    struct{
      size_t
        NSTitledWindowMask,
        NSClosableWindowMask,
        NSMiniaturizableWindowMask,
        NSResizableWindowMask,
        NSBorderlessWindowMask;
    }style;

    struct{
      size_t
        NSLeftMouseDownMask,
        NSLeftMouseUpMask,
        NSRightMouseDownMask,
        NSRightMouseUpMask,
        NSMouseMovedMask,
        NSLeftMouseDraggedMask,
        NSRightMouseDraggedMask,
        NSKeyDownMask,
        NSKeyUpMask,
        NSFlagsChangedMask,
        NSAnyEventMask;
    }mask;

    struct{
      int
        NSClose,
        NSMove,
        NSResize,
        NSLeftMouseDown,
        NSLeftMouseUp,
        NSRightMouseDown,
        NSRightMouseUp,
        NSMouseMoved,
        NSLeftMouseDragged,
        NSKeyDown,
        NSKeyUp;
    }event;

}OBJC;

static int _sgw_objc_init(void){
    static char init=-1;
    if(!init) return 0;

    /* selectors */
    #define OBJC_LOAD(_sel_,_name_) if( !(OBJC.sel._sel_ = OBJC_SEL(_name_)) ) return -1
    OBJC_LOAD(alloc,"alloc");
    OBJC_LOAD(init,"init");
    OBJC_LOAD(string,"stringWithUTF8String:");
    OBJC_LOAD(title,"setTitle:");
    OBJC_LOAD(show,"makeKeyAndOrderFront:");
    OBJC_LOAD(close,"close");
    OBJC_LOAD(cursor_x,"x");
    OBJC_LOAD(cursor_y,"y");
    OBJC_LOAD(origin_x,"origin.x");
    OBJC_LOAD(origin_y,"origin.x");
    OBJC_LOAD(size_w,"size.width");
    OBJC_LOAD(size_h,"size.height");
    OBJC_LOAD(frame,"frame");
    OBJC_LOAD(setFrame,"setFrame:display:");
    OBJC_LOAD(initWithRect,"initWithContentRect:styleMask:backing:defer:");
    OBJC_LOAD(valueWithRect,"valueWithRect:");
    OBJC_LOAD(nextEvent,"nextEventMatchingMask:untilDate:inMode:dequeue:");
    OBJC_LOAD(sendEvent,"sendEvent:");
    OBJC_LOAD(application,"sharedApplication");
    OBJC_LOAD(timeout,"dateWithTimeIntervalSinceNow:");
    OBJC_LOAD(eventType,"type");
    OBJC_LOAD(uint,"unsignedIntegerValue");
    #undef OBJC_LOAD

    /* classes */
    #define OBJC_LOAD(_cls_,_name_) if( !(OBJC.cls._cls_ = OBJC_CLASS(_name_)) ) return -1
    OBJC_LOAD(Window,"NSWindow");
    OBJC_LOAD(String,"NSString");
    OBJC_LOAD(Value,"NSValue");
    OBJC_LOAD(App,"NSApplication");
    OBJC_LOAD(Event,"NSEvent");
    OBJC_LOAD(Date,"NSDate");
    OBJC_LOAD(Loop,"NSRunLoop");
    #undef OBJC_LOAD

    /* event masks */
    #define OBJC_LOAD(_1_) do{size_t _2_; if(OBJC_VAR(OBJC.cls.Event,#_1_,&_2_)) return -1; OBJC.mask._1_=_2_; }while(0)

    OBJC_LOAD(NSLeftMouseDownMask);
    OBJC_LOAD(NSLeftMouseUpMask);
    OBJC_LOAD(NSRightMouseDownMask);
    OBJC_LOAD(NSRightMouseUpMask);
    OBJC_LOAD(NSMouseMovedMask);
    OBJC_LOAD(NSLeftMouseDraggedMask);
    OBJC_LOAD(NSRightMouseDraggedMask);
    OBJC_LOAD(NSKeyDownMask);
    OBJC_LOAD(NSKeyUpMask);
    OBJC_LOAD(NSFlagsChangedMask);
    OBJC_LOAD(NSAnyEventMask);
    #undef OBJC_LOAD

    /* events */
    #define OBJC_LOAD(_1_) do{size_t _2_; if(OBJC_VAR(OBJC.cls.Event,#_1_,&_2_)) return -1; OBJC.event._1_=_2_; }while(0)
    OBJC_LOAD(NSClose);
    OBJC_LOAD(NSMove);
    OBJC_LOAD(NSResize);
    OBJC_LOAD(NSLeftMouseDown);
    OBJC_LOAD(NSLeftMouseUp);
    OBJC_LOAD(NSRightMouseDown);
    OBJC_LOAD(NSRightMouseUp);
    OBJC_LOAD(NSMouseMoved);
    OBJC_LOAD(NSLeftMouseDragged);
    OBJC_LOAD(NSKeyDown);
    OBJC_LOAD(NSKeyUp);
    #undef OBJC_LOAD

    /* window style */
    #define OBJC_LOAD(_1_) do{size_t _2_; if(OBJC_VAR(OBJC.cls.Window,#_1_,&_2_)) return -1; OBJC.style._1_=_2_; }while(0)
    OBJC_LOAD(NSTitledWindowMask);
    OBJC_LOAD(NSClosableWindowMask);
    OBJC_LOAD(NSMiniaturizableWindowMask);
    OBJC_LOAD(NSResizableWindowMask);
    OBJC_LOAD(NSBorderlessWindowMask);
    OBJC_LOAD(NSTitledWindowMask);
    #undef OBJC_LOAD

    /* buffering */
    #define OBJC_LOAD(_1_) do{size_t _2_; if(OBJC_VAR(OBJC.cls.Window,#_1_,&_2_)) return -1; OBJC.buffering._1_=_2_; }while(0)
    OBJC_LOAD(NSBackingStoreBuffered);
    OBJC_LOAD(NSBackingStoreRetained);
    OBJC_LOAD(NSBackingStoreNonretained);
    #undef OBJC_LOAD

    /* id */
    OBJC.id.app = OBJC_MSG(OBJC.cls.App, OBJC.sel.application);
    OBJC.id.loop = OBJC_MSG(OBJC.cls.String, OBJC.sel.string, "kCFRunLoopDefaultMode");

    return (init=0);
}

static void OBJC_windowMoveResize(OBJC_ID window,const int x,const int y,const int w,const int h){
    const NSRect rect={{x,y},{w,h}};
    OBJC_MSG(window, OBJC.sel.setFrame, rect, 1);
}

typedef struct{
    struct _sgw w;
    OBJC_ID window;
    unsigned int color_max;
}sgw_objc;

#define SGW_UNCONST(_name_,_const_) sgw_objc * const _name_ = (sgw_objc*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->window) OBJC_MSG(w->window, OBJC.sel.close);
        _w->deallocator(w);
    }
}

static void _sgw_size(sgw_objc * const w){
    OBJC_ID frame=OBJC_MSG(w->window, OBJC.sel.frame);
    w->w.rectangle.x=OBJC_MSGT(CGFloat,frame, OBJC.sel.origin_x);
    w->w.rectangle.y=OBJC_MSGT(CGFloat,frame, OBJC.sel.origin_x);
    w->w.rectangle.w=OBJC_MSGT(CGFloat,frame, OBJC.sel.size_w);
    w->w.rectangle.h=OBJC_MSGT(CGFloat,frame, OBJC.sel.size_h);
}

static void _sgw_resize(sgw_objc * const w){
    const unsigned int size=w->w.rectangle.w*w->w.rectangle.h;
    if(size>w->color_max){
        w->color_max=size;
        w->w.deallocator(w->w.pixel);
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
/*        if(w->color_bytes<4){
            w->w.deallocator(w->image->data);
            w->image->data=(char*)w->w.allocator(size*w->color_bytes);
        }else w->image->data=(char*)w->w.pixel;*/
    }
/*    w->image->width=w->w.rectangle.w;
    w->image->height=w->w.rectangle.h;
    w->image->bytes_per_line=w->image->width*w->color_bytes;*/
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(_sgw_objc_init()) return NULL;
    if(!allocator){allocator=malloc;}
    if(!deallocator){deallocator=free;}{
    SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        const NSRect rect={{50,50},{50,50}};
        memset(w,0,sizeof(*w));
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        w->w.mode=SGW_XYWH|SGW_MUTABLE;
        if( !(w->window=OBJC_MSG(
            OBJC_MSG(OBJC.cls.Window, OBJC.sel.alloc),
            OBJC.sel.initWithRect,
            OBJC_MSG(OBJC.cls.Value,OBJC.sel.valueWithRect, rect,
            OBJC.style.NSTitledWindowMask | OBJC.style.NSClosableWindowMask | OBJC.style.NSMiniaturizableWindowMask,
            OBJC.buffering.NSBackingStoreBuffered,0)))
        )break;
        OBJC_MSG(w->window, OBJC.sel.show, 0);
/*        switch( (w->w.bitness=DefaultDepth(w->display,w->screen)) ){
            case 15: case 16: w->color_bytes=2; break;
            case 24: case 32: w->color_bytes=4; break;
            default: w->color_bytes=1; break;
        }*/
        _sgw_size(w);
        _sgw_resize(w);
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char * const title){
    SGW_UNCONST(w,_w);
    OBJC_MSG(w->window, OBJC.sel.title, OBJC_MSG(OBJC.cls.String,OBJC.sel.string, (w->w.title=title?title:" ")));
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
}

void sgw_rect(SGW * const _w,const enum SGW mode,...){
    SGW_UNCONST(w,_w);
}


enum SGE sgw_event(SGW * const _w,const int t,SGE * const e) {
    SGW_UNCONST(w,_w);
    OBJC_ID timeout=OBJC_MSG(OBJC.cls.Date, OBJC.sel.timeout, (t<0?1.0e10:t/1000.0));
    while(1){
        OBJC_ID event=OBJC_MSG(OBJC.id.app, OBJC.sel.nextEvent, OBJC.mask.NSAnyEventMask, timeout, OBJC.id.loop,1);
        if(!event){break;}{
        const int type=(size_t)OBJC_MSG(OBJC_MSG(event, OBJC.sel.eventType), OBJC.sel.uint);

        if(type==OBJC.event.NSClose){
            return SGE_CLOSE;
        }
        if(type==OBJC.event.NSMove){

        }
        if(type==OBJC.event.NSResize){

        }
        if(type==OBJC.event.NSKeyDown){

        }
        if(type==OBJC.event.NSKeyUp){

        }

/*        switch((size_t)OBJC_MSG(OBJC_MSG(event, OBJC.sel.eventType), OBJC.sel.uint)){
            case NSClose:
                return SGE_CLOSE;
            case NSMove:
            case NSResize:
                _sge_unrepeat();
                OBJC_windowRect();
                _sgw_resize(w);
                return SGE_RECTANGLE;
            case NSKeyDown: if(_sgk_press(_sgw_keyboard(key),w,e)) return SGE_PRESS; break;
            //    event->key.keyCode = (int)OBJC_MSG(nsEvent, OBJC_SEL("keyCode"));
            case NSKeyUp: if(_sgk_release(_sgw_keyboard(key),w,e)) return SGE_RELEASE; break;

            case NSLeftMouseDown:{
                OBJC_ID location = OBJC_MSG(nsEvent, OBJC_SEL("locationInWindow"));
                event->mouse.x = (int)OBJC_MSG(location, OBJC_SEL("x"));
                event->mouse.y = (int)OBJC_MSG(location, OBJC_SEL("y"));
                event->mouse.button = 1;
                return OBJC_WE_MOUSE_CLICK;
            }
            default: break;
        }*/
    }}
    return SGE_NONE;
}

#undef SGW_UNCONST
