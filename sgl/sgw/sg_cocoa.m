/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#import <Cocoa/Cocoa.h>
#include "../sgw.h"

@interface ___CLASS_SGW_IMPL : NSObject{
    NSWindow *_window;
}

- (id)initWithSGWdefault;

+ (___CLASS_SGW_IMPL*)sgw_open;
- (void)sgw_close;
- (void)sgw_async:(const void*)p;
- (void)sgw_set_title:(const char *)title;
- (void)sgw_set_rect:(int)x y:(int)y w:(unsigned int)w h:(unsigned int)h;
- (void)sgw_get_cursor:(int*)x y:(int*)y;
- (void)sgw_get_rect:(int *)x y:(int *)y w:(unsigned int *)w h:(unsigned int *)h;
- (void)sgw_render:(const void*)buffer w:(unsigned int)w h:(unsigned int)h;
- (enum SGE)sgw_event:(int)t e:(SGE*)e;
@end


@implementation ___CLASS_SGW_IMPL

- (void)sgw_post:(short)subtype ptr:(void*)ptr {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

    NSEvent *event = [[[NSEvent alloc] otherEventWithType:NSApplicationDefined
                                      location:NSZeroPoint
                                 modifierFlags:0
                                     timestamp:0
                                  windowNumber:[_window windowNumber]
                                       context:ptr
                                       subtype:subtype
                                         data1:0
                                         data2:0] autorelease];
    [NSApp postEvent:event atStart:NO];

    [pool release];
}

- (void)_notification_print:(NSNotification *)notification {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
//    NSLog(@"notification: %@", [notification name]);
    [pool release];
}


- (void)windowDidMove:(NSNotification *)notification {
    [self sgw_post:SGE_RECTANGLE ptr:nil];
}

- (void)windowDidResize:(NSNotification *)notification {
    [self sgw_post:SGE_RECTANGLE ptr:nil];
}

- (BOOL)windowShouldClose:(id)sender {
    [self sgw_post:SGE_CLOSE ptr:nil];
    return NO;
}

- (id)initWithSGWdefault {
    if(self){
        NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
        [NSApplication sharedApplication];
        // policy regular

        _window=[[NSWindow alloc] initWithContentRect:NSMakeRect(50, 50, 100, 100)
              styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                backing:NSBackingStoreBuffered
                  defer:NO];
        if (_window) {
            [_window center];
            [_window setTitle:@" "];
            [_window makeKeyAndOrderFront:nil];

            [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(windowDidMove:)
                                                name:@"WindowDidMove"
                                                object:_window];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(windowDidResize:)
                                                name:@"WindowDidResize"
                                                object:_window];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(windowShallclose:)
                                                name:@"WindowShallClose"
                                                object:_window];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                selector:@selector(_notification_print:)
                                                name:nil
                                                object:_window];
        }else{
            /////
        }
        [pool release];
    }
    return self;
}

+ (___CLASS_SGW_IMPL*)sgw_open {
    return [[___CLASS_SGW_IMPL alloc] initWithSGWdefault];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_window close];
    [_window release];
    [super dealloc];
}

- (void)sgw_close {
    [self release];
}

- (void)sgw_set_title:(const char *)title {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

    [_window setTitle:[[[NSString alloc] initWithUTF8String:title] autorelease] ];

    [pool release];
}

- (void)sgw_set_style:(enum SGW)style {

}

- (void)sgw_set_rect:(int)x y:(int)y w:(unsigned int)w h:(unsigned int)h {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

    [_window setFrame:NSMakeRect(x, y, w, h) display:YES];

    [pool release];
}

- (void)sgw_get_rect:(int *)x y:(int *)y w:(unsigned int *)w h:(unsigned int *)h {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

    NSRect frame = [_window frame];
    *x = (int)frame.origin.x;
    *y = (int)frame.origin.y;
    *w = (int)frame.size.width;
    *h = (int)frame.size.height;

    [pool release];
}

- (void)sgw_get_cursor:(int*)x y:(int*)y {
    
}

- (void)sgw_async:(const void*)p {
    
}

- (enum SGE)sgw_event:(int)t e:(SGE*)e{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
    double timeout;

    if(t<0) timeout=[NSDate distantFuture];
    else if(t>0) timeout=t/1000.0;
    else timeout=[NSDate distantPast];

    NSDate *endDate = [[[NSDate alloc] initWithTimeIntervalSinceNow:timeout] autorelease];
    enum SGE result = SGE_NONE;

    while ([endDate timeIntervalSinceNow] > 0) {
        NSEvent *event = [[NSApp nextEventMatchingMask:NSAnyEventMask
                                               untilDate:endDate
                                                  inMode:NSDefaultRunLoopMode
                                                 dequeue:YES] retain];
        if (!event) break;

        [NSApp sendEvent:event];

        if ([event window] != _window)
            continue;

            // Обработка событий
        switch ( [event type] ) {
            case NSKeyDown:
                result = SGE_PRESS;
                goto exit_loop;
            case NSKeyUp:
                result = SGE_RELEASE;
                goto exit_loop;
            case NSApplicationDefined:
                result = [event subtype];
                goto exit_loop;

            default: break;
        }

        [event release];
    }

exit_loop:

    [pool release];
    return result;
}

- (void)sgw_render:(const void*)buffer w:(unsigned int)w h:(unsigned int)h {
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
    NSSize s=NSMakeSize(w,h);

    // Создаем битмап с внешним буфером (без сохранения в поле)
    NSBitmapImageRep *bitmap = [[[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:(unsigned char**)&buffer
                      pixelsWide:s.width
                      pixelsHigh:s.height
                   bitsPerSample:8
                 samplesPerPixel:4
                        hasAlpha:NO
                        isPlanar:NO
                  colorSpaceName:NSCalibratedRGBColorSpace
                     bytesPerRow:s.width*4
                    bitsPerPixel:32] autorelease];

    NSImage *image = [[[NSImage alloc] initWithSize:s] autorelease];
    [image addRepresentation:bitmap];

    // Обновляем слой (main thread-safe)
  /*  if ([NSThread isMainThread]) {
        [[self.contentView layer] setContents:image];
    } else {
        [[self.contentView.layer] performSelectorOnMainThread:@selector(setContents:)
                                                  withObject:image
                                               waitUntilDone:NO];
    }*/
    [pool release];
}

@end


#define SGW_UNCONST(_name_,_const_) sgw_objc * const _name_ = (sgw_objc*)({ const union{const void *_; void *w;}_1_={_const_}; _1_.w; })

typedef struct{
    struct _sgw w;
    id window;
    unsigned int color_max;
}sgw_objc;

static void _sgw_size(sgw_objc * const w){
    [w->window sgw_get_rect:&w->w.rectangle.x y:&w->w.rectangle.y w:&w->w.rectangle.w h:&w->w.rectangle.h];
}

static void _sgw_resize(sgw_objc * const w){
    const unsigned int size=w->w.rectangle.w*w->w.rectangle.h;
    if(size>w->color_max){
        w->color_max=size;
        w->w.deallocator(w->w.pixel);
        w->w.pixel=(SGC*)w->w.allocator(size*sizeof(*w->w.pixel));
    }
}

void sgw_close(SGW * const _w){
    if(_w){
        SGW_UNCONST(w,_w);
        if(w->window) [w->window sgw_close];
        w->w.deallocator(w->w.pixel);
        _w->deallocator(w);
    }
}

SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    if(!allocator){allocator=malloc;}
    if(!deallocator){deallocator=free;}{
    SGW_UNCONST(w,allocator(sizeof(*w)));
    while(w){
        memset(w,0,sizeof(*w));
        w->w.allocator=allocator;
        w->w.deallocator=deallocator;
        w->w.mode=SGW_XYWH|SGW_MUTABLE;
        if( !(w->window=[___CLASS_SGW_IMPL sgw_open]) )
            break;
        w->w.bitness=32;
        _sgw_size(w);
        _sgw_resize(w);
        return &w->w;
    }
    sgw_close(&w->w);
    return NULL;
}}

void sgw_title(SGW * const _w,const char * const title){
    SGW_UNCONST(w,_w);
    w->w.title=(title?title:" ");
    [w->window sgw_set_title:w->w.title];
}

void sgw_async(SGW * const _w,const void * const p){
    SGW_UNCONST(w,_w);
    [w->window sgw_async:p];
}

void sgw_render(SGW * const _w){
    SGW_UNCONST(w,_w);
//    [w->window sgw_render:_w->pixel w:_w->rectangle.w h:_w->rectangle.h];
    return; /*_sgc_convert(NULL,0,0,NULL);*/
}

void sgw_rect(SGW * const _w,const enum SGW mode,...){
    SGW_UNCONST(w,_w); int flags=0;

    if( (mode & SGW_MUTABLE) && !(w->w.mode & SGW_MUTABLE) ){
        flags^=1; w->w.mode=(w->w.mode & SGW_MODES) | SGW_MUTABLE;
    }

    if( (mode & SGW_MODES) && (w->w.mode & SGW_MUTABLE) ){
        if( (mode & SGW_MODES)<=SGW_XYWH ){
            va_list l;
            va_start(l,mode);
            if(mode & SGW_XY){ w->w.rectangle.x=va_arg(l,int); w->w.rectangle.y=va_arg(l,int); }
            if(mode & SGW_WH){ w->w.rectangle.w=va_arg(l,unsigned int); w->w.rectangle.h=va_arg(l,unsigned int); }
            va_end(l);
            flags|=2;
            w->w.mode=SGW_XYWH | (w->w.mode & SGW_STATES);
        }
    }

    if( (mode & SGW_FIXED) && !(w->w.mode & SGW_FIXED) ){
        flags^=1; w->w.mode=(w->w.mode & SGW_MODES) | SGW_FIXED;
    }

    if(flags & 1){
        [w->window sgw_set_style:(w->w.mode & SGW_STATES)];
        flags|=2;
    }

    if(flags & 2){
        [w->window sgw_set_rect:w->w.rectangle.x y:w->w.rectangle.y w:w->w.rectangle.w h:w->w.rectangle.h];
        _sgw_size(w);
        _sgw_resize(w);
    }
}

static char _sgk_press(SGK key,struct _sgw * const w,SGE * const e){// SGK * const keys, SGK * const pressed){
    if(!key) return 0;
    if(key>0xffff) w->keys|=key;
    else w->keys=(w->keys&~0xffff)|key;
    e->key=w->keys;
    return 1;
}

static int _sgk_release(SGK key,struct _sgw * const w,SGE * const e){// SGK * const keys, SGK * const released){
    if(!key) return 0;
    e->key=w->keys;
    if(key>0xffff) w->keys^=key;
    else w->keys&=~0xffff;
    return 1;
}

enum SGE sgw_event(SGW * const _w,const int t,SGE * const e) {
    SGW_UNCONST(w,_w);
    const enum SGE event=[w->window sgw_event:t e:e];
    switch(event){
        case SGE_RECTANGLE: _sgw_size(w); _sgw_resize(w); break;
        case SGE_CURSOR: [w->window sgw_get_cursor:&w->w.cursor.x y:&w->w.cursor.y]; break;
        case SGE_PRESS: _sgk_press(e->key,&w->w,e); break;
        case SGE_RELEASE: _sgk_release(e->key,&w->w,e); break;
        default: break;
    }
    return event;
}



#undef SGW_UNCONST
