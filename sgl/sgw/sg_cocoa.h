/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <stdio.h>

//#include <objc/message.h>
//#include <objc/runtime.h>

typedef void* OBJC_ID;
typedef void* OBJC_SEL;
typedef void* OBJC_CLASS;

extern OBJC_ID objc_msgSend(OBJC_ID, OBJC_SEL, ...);
extern OBJC_SEL sel_registerName(const char *);
extern OBJC_CLASS objc_getClass(const char *);

static void * const _objc_msgSend_ptr=objc_msgSend;

#define OBJC_MSGT(_t_,...)  ((_t_(*)(OBJC_ID, OBJC_SEL, ...))_objc_msgSend_ptr)(__VA_ARGS__)
#define OBJC_MSG(...)    OBJC_MSGT(OBJC_ID,__VA_ARGS__)
#define OBJC_SEL(...)    sel_registerName(__VA_ARGS__)
#define OBJC_CLASS(...)  objc_getClass(__VA_ARGS__)

//#include <AppKit/NSEvent.h>
//#include <AppKit/NSWindow.h>

#include <limits.h>
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

enum NSEventMask{
    NSAnyEventMask = ULONG_MAX
};

enum NSEvent{
    NSLeftMouseDown = 1,
    NSLeftMouseUp = 2,
    NSRightMouseDown = 3,
    NSRightMouseUp = 4,
    NSMouseMoved = 5,
    NSKeyDown = 10,
    NSKeyUp = 11,
    NSApplicationDefined = 15,
    NSScrollWheel = 22,
    NSOtherMouseDown = 25,
    NSOtherMouseUp = 26,
};

enum NSWindowMask{
    NSBorderlessWindowMask = 0,
    NSTitledWindowMask = 1<<0,
    NSClosableWindowMask = 1<<1,
    NSMiniaturizableWindowMask = 1<<2,
    NSResizableWindowMask = 1<<3,
    NSDefaultWindowMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
};

enum NSBackingStoreType{
    NSBackingStoreRetained = 0,
    NSBackingStoreNonretained = 1,
    NSBackingStoreBuffered = 2
};

enum NSCustom{
    NSClose=0,
    NSMove=1,
    NSResize=2,
    NSAsync=3,
};

/* **********************************************************
********************************************************** */

static struct{

    struct{
      OBJC_SEL
        regCls,
        alloc,
        addMethod,
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
        timeout,
        eventType,
        uint,
        getWindow,
        setWindow,
        setDelegate,
        postEvent,
        customEvent,
        customType,
        windowNumber;
    }sel;

    struct{
      OBJC_CLASS
        Object,
        Window,
        Event,
        Delegate,
        String,
        Value,
        Date,
        Loop;
    }cls;

    struct{
      OBJC_ID
        pool,
        app,
        loop;
    }id;

}OBJC;

static void OBJC_POST(OBJC_ID window,const int type,const void * const data){
    const NSPoint p={0,0};
    OBJC_ID event=OBJC_MSG(
        OBJC.cls.Event,OBJC.sel.customEvent, NSApplicationDefined,
        p,0,0, OBJC_MSG(window,OBJC.sel.windowNumber), NULL,0,0
    );
    OBJC_MSG(OBJC.id.app,OBJC.sel.postEvent,event,0);
}

static void _sgw_handler_close(OBJC_ID self){
    OBJC_POST(OBJC_MSG(self,OBJC.sel.getWindow),NSClose,NULL);
}

static void _sgw_handler_move(OBJC_ID self){
    OBJC_POST(OBJC_MSG(self,OBJC.sel.getWindow),NSMove,NULL);
}

static void _sgw_handler_resize(OBJC_ID self){
    OBJC_POST(OBJC_MSG(self,OBJC.sel.getWindow),NSResize,NULL);
}

static void _sgw_atexit(void){
    OBJC_MSG(OBJC.id.pool,OBJC_SEL("drain"));
}

static int _sgw_objc_init(void){
    static char init=-1;
                                        return -1; // FIX ME
    if(!init) return 0;

    /* selectors */
    #define OBJC_LOAD(_sel_,_name_) if( !(OBJC.sel._sel_ = OBJC_SEL(_name_)) ) return -1
    OBJC_LOAD(regCls,"registerClass");
    OBJC_LOAD(alloc,"alloc");
    OBJC_LOAD(addMethod,"addInstanceMethod:imp:types:");
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
    OBJC_LOAD(timeout,"dateWithTimeIntervalSinceNow:");
    OBJC_LOAD(eventType,"type");
    OBJC_LOAD(uint,"unsignedIntegerValue");
    OBJC_LOAD(getWindow, "window");
    OBJC_LOAD(setWindow, "setWindow:");
    OBJC_LOAD(setDelegate, "setDelegate:");
    OBJC_LOAD(postEvent, "postEvent:atStart:");
    OBJC_LOAD(customType, "subtype");
    OBJC_LOAD(customEvent, "otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:");
    OBJC_LOAD(windowNumber, "windowNumber");
    #undef OBJC_LOAD

    OBJC.id.pool=OBJC_MSG(OBJC_MSG(OBJC_CLASS("NSAutoreleasePool"), OBJC.sel.alloc), OBJC.sel.init);
    OBJC.id.app = OBJC_MSG(OBJC_CLASS("NSApplication"), OBJC_SEL("sharedApplication"));
    OBJC.id.loop = OBJC_MSG(OBJC.cls.String, OBJC.sel.string, "kCFRunLoopDefaultMode");
    atexit(_sgw_atexit);

    /* classes */
    #define OBJC_LOAD(_cls_,_name_) if( !(OBJC.cls._cls_ = OBJC_CLASS(_name_)) ) return -1
    OBJC_LOAD(Object,"NSObject");
    OBJC_LOAD(Window,"NSWindow");
    OBJC_LOAD(String,"NSString");
    OBJC_LOAD(Value,"NSValue");
    OBJC_LOAD(Event,"NSEvent");
    OBJC_LOAD(Date,"NSDate");
    OBJC_LOAD(Loop,"NSRunLoop");
    /* FIX ME delegate to handle window events 
    OBJC.cls.Delegate=OBJC_MSG(OBJC.cls.Object,OBJC.sel.alloc);
    OBJC_MSG(OBJC.cls.Delegate,OBJC_SEL("setName:"),"SGW_DELEGATE");
    OBJC_MSG(OBJC.cls.Delegate,OBJC.sel.addMethod,OBJC_SEL("windowDidMove:"),_sgw_handler_move,"v@:@");
    OBJC_MSG(OBJC.cls.Delegate,OBJC.sel.addMethod,OBJC_SEL("windowDidResize:"),_sgw_handler_resize,"v@:@");
    OBJC_MSG(OBJC.cls.Delegate,OBJC.sel.addMethod,OBJC_SEL("windowDidClose:"),_sgw_handler_close,"v@:@");
    OBJC_MSG(OBJC.cls.Delegate,OBJC.sel.regCls);
    */
    #undef OBJC_LOAD

    OBJC.id.app = OBJC_MSG(OBJC_CLASS("NSApplication"), OBJC_SEL("sharedApplication"));
    OBJC.id.loop = OBJC_MSG(OBJC.cls.String, OBJC.sel.string, "kCFRunLoopDefaultMode");

    return (init=0);
}

typedef struct{
    struct _sgw w;
    OBJC_ID window;
    OBJC_ID delegate;
    unsigned int color_max;
}sgw_objc;

#define SGW_UNCONST(_name_,_const_) sgw_objc * const _name_ = (sgw_objc*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

void sgw_close(SGW * const _w){
    SGW_UNCONST(w,_w);
    if(w){
        if(w->window){
            OBJC_MSG(w->window, OBJC.sel.setDelegate,NULL);
            OBJC_MSG(w->window, OBJC.sel.close);
        }
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
        if( !(w->delegate=OBJC_MSG(OBJC_MSG(OBJC.cls.Delegate, OBJC.sel.alloc), OBJC.sel.init)) )
            break;
        if( !(w->window=OBJC_MSG(
            OBJC_MSG(OBJC.cls.Window, OBJC.sel.alloc),
            OBJC.sel.initWithRect,
            OBJC_MSG(OBJC.cls.Value,OBJC.sel.valueWithRect, rect,
            NSDefaultWindowMask, NSBackingStoreBuffered,0)))
        )break;
        OBJC_MSG(w->delegate,OBJC.sel.setWindow,w->window);
        OBJC_MSG(w->window,OBJC.sel.setDelegate,w->delegate);
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
    int flags=0;

    if( (mode & SGW_MODES) && (w->w.mode & SGW_MUTABLE) ){
        if( (mode & SGW_MODES)<=SGW_XYWH ){
            va_list l;
            va_start(l,mode);
            if(mode & SGW_XY){ w->w.rectangle.x=va_arg(l,int); w->w.rectangle.y=va_arg(l,int); }
            if(mode & SGW_WH){ w->w.rectangle.w=va_arg(l,unsigned int); w->w.rectangle.h=va_arg(l,unsigned int); }
            va_end(l);
            flags|=1;
            w->w.mode=SGW_XYWH | (w->w.mode & SGW_STATES);
        }
    }

    if(flags & 1){
        const NSRect rect={{w->w.rectangle.x,w->w.rectangle.y},{w->w.rectangle.w,w->w.rectangle.h}};
        OBJC_MSG(w->window, OBJC.sel.setFrame, rect, 1);
        _sgw_size(w);
        _sgw_resize(w);
    }
}


enum SGE sgw_event(SGW * const _w,const int t,SGE * const e) {
    SGW_UNCONST(w,_w);
    OBJC_ID timeout=OBJC_MSG(OBJC.cls.Date, OBJC.sel.timeout, (t<0?1.0e10:t/1000.0));
    while(1){
        OBJC_ID event=OBJC_MSG(OBJC.id.app, OBJC.sel.nextEvent, NSAnyEventMask, timeout, OBJC.id.loop,1);
        if(!event) break;
        if(OBJC_MSG(event,OBJC.sel.getWindow)!=w->window){
            OBJC_MSG(OBJC.id.app, OBJC.sel.sendEvent, event);
            continue;
        }
        switch(OBJC_MSGT(int,event, OBJC.sel.eventType)){
            case NSApplicationDefined:
                switch(OBJC_MSGT(short,event,OBJC.sel.customType)){
                    case NSClose: return SGE_CLOSE;
                    case NSMove: return SGE_CLOSE;
                    case NSResize: return SGE_RECTANGLE;
                    case NSAsync: return SGE_ASYNC;
                    default: break;
                }
                break;
            default: printf("usual message\n"); break;
        }
    }
    return SGE_NONE;
}

#undef SGW_UNCONST
