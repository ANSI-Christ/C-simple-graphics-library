/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include "sgu.h"

#include <stdlib.h>
#include <string.h>

#define SG_SET(_t_,_l_,_r_) do{ const union{const void *_;_t_ *t;}_1_={(const void*)&(_l_)}; *_1_.t=(_r_); }while(0)

static void _sgu_link(CLASS _SGU_NODE *w,CLASS _SGU_NODE *p){
    if(p){
        if(p->last){
            p->last->next=w;
            w->prev=p->last;
            p->last=w;
        }else p->child=p->last=w;
        SG_SET(void*,w->parent,p);
        p->onInsert(p,w);
    }else if( (p=(CLASS _SGU_NODE*)w->parent) ){
        if(w->prev) w->prev->next=w->next;
        else p->child=w->next;
        if(w->next) w->next->prev=w->prev;
        else p->last=w->prev;
        w->prev=w->next=NULL;
        SG_SET(void*,w->parent,NULL);
    }
}

static void _sgu_reorder(CLASS _SGU_NODE *w){
    while(w!=(void*)w->ui){
        if(w->next){
            if(w->prev) w->prev->next=w->next;
            else ((CLASS _SGU_NODE*)w->parent)->child=w->next;
            w->next->prev=w->prev;
            w->next=NULL;
            ((CLASS _SGU_NODE*)w->parent)->last->next=w;
            w->prev=((CLASS _SGU_NODE*)w->parent)->last;
            ((CLASS _SGU_NODE*)w->parent)->last=w;
        }
        w=(CLASS _SGU_NODE*)w->parent;
    }
}


static void *_sgu_find(CLASS SGU_WIDGET *w,const int x,const int y){
    void *p=NULL;
    if(w && sgm_at(&w->m,x-w->m.x,y-w->m.y)){
_mark:
        p=w; w=(CLASS SGU_WIDGET*)(((CLASS _SGU_NODE*)w)->last);
        while(w){
            if(w->able && sgm_at(&w->m,x-w->m.x,y-w->m.y))
                goto _mark;
            w=(CLASS SGU_WIDGET*)(((CLASS _SGU_NODE*)w)->prev);
        }
    }
    return p;
}

static void *_sgu_next(const CLASS _SGU_NODE *w){
    if(w->child) return w->child;
    if(w->next) return w->next;
    while(w->parent){
        w=(CLASS _SGU_NODE*)w->parent;
        if(w->next) return w->next;
    }
    return w->ui;
}

static void _sgu_default(void){}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void *sgu_insert(void * const widget,void * const parent){
    if(widget && parent && widget!=parent){
        _sgu_link(widget,NULL);
        _sgu_link(widget,parent);
        return ((CLASS SGU_WIDGET*)widget)->parent;
    } return NULL;
}

void *sgu_select(void * const widget){
    if(widget){
        CLASS SGU_UI * const ui=((CLASS SGU_WIDGET*)widget)->ui;
        ui->select=widget;
        ((CLASS SGU_WIDGET*)widget)->onSelect(widget);
        _sgu_reorder(ui->select);
        return ui->select;
    }
    return NULL;
}

char sgu_focused(const void * const widget){
    return ((const CLASS SGU_WIDGET*)widget)->ui->focus==widget;
}

char sgu_selected(const void * const widget){
    return ((const CLASS SGU_WIDGET*)widget)->ui->select==widget;
}

char sgu_moved(const void * const widget){
    const CLASS SGU_WIDGET * const w=(const CLASS SGU_WIDGET*)widget;
    if(widget==(void*)w->ui) return 0;
    return (w->x+w->m.x!=w->parent->m.x) | ((w->y+w->m.y!=w->parent->m.y)<<1);
}

char sgu_resized(const void * const widget){
    const CLASS SGU_WIDGET * const w=(const CLASS SGU_WIDGET*)widget;
    return (w->w!=w->m.w) | ((w->h!=w->m.h)<<1);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */






CLASS_COMPILE(_SGU_NODE)(
    constructor(parent)(
        SG_SET(void*,self->onInsert,_sgu_default);
        if(parent){
            SG_SET(void*,self->ui,((CLASS _SGU_NODE*)parent)->ui);
            _sgu_link(self,parent);
        }
    ),
    destructor()(
        void * const rm=(self->ui?(void*)self->ui->deallocator:(void*)_sgu_default);
        CLASS _SGU_NODE *w;
        _sgu_link(self,NULL);
        while( (w=self->child) ){
            w->destructor(w);
            ((void(*)(void*))rm)(w);
        }
    )
)

CLASS_COMPILE(SGU_WIDGET)(
    constructor(parent)(
        if(!self) self=((CLASS SGU_WIDGET*)parent)->ui->allocator(sizeof(*self));
        super(self,parent);
        SG_SET(void*,self->core,_sgu_default);
        SG_SET(void*,self->onDraw,_sgu_default);
        SG_SET(void*,self->onEvent,_sgu_default);
        SG_SET(void*,self->onSelect,_sgu_default);
        SG_SET(void*,self->onUpdate,_sgu_default);
        self->able=self->visible=1;
    )
)



static char _sgu_basics(CLASS SGU_WIDGET * const w,const SGU * const sgu){
    if(w){
        CLASS SGU_UI * const ui=w->ui;
        const int cx=sgu->cursor.x, cy=sgu->cursor.y;
        if( (sgu->event & SGU_CURSOR) && (ui->flags & 16) ){
            const int dx=cx-w->m.x-ui->dxy[0], dy=cy-w->m.y-ui->dxy[1];
            int act=0;
            if(w->resizable){
                ui->flags|=32;
                act=ui->flags&15;
                if(ui->flags & 1){w->w+=dx;}
                if(ui->flags & 2){w->w-=dx; w->x+=dx;}
                if(ui->flags & 4){w->h+=dy;}
                if(ui->flags & 8){w->h-=dy; w->y+=dy;}
            }
            if((w->movable|w->dNd) && !act){
                ui->flags|=32;
                w->x=cx-w->parent->m.x-ui->dxy[0];
                w->y=cy-w->parent->m.y-ui->dxy[1];
                if(w->dNd) w->m.flags|=SGM_UNLIMITED;
            }
        }
        if( (sgu->event & SGU_PRESS) && (sgu->key & SGK_LB)){
            const int dx=ui->dxy[0]=cx-w->m.x, dy=ui->dxy[1]=cy-w->m.y;
            ui->flags |= 16 | ((dx<4)<<1) | ((dx>w->w-4)) | ((dy<4)<<3) | ((dy>w->h-4)<<2);
        }
        if( (sgu->event & SGU_RELEASE) && (sgu->key==SGK_LB) && ((ui->flags&=~31) & 32)){
            ui->flags&=~32;
            if(w->dNd && ui->block!=(void*)w){
                CLASS SGU_WIDGET * const from=w->parent, *to;
                w->m.flags&=~SGM_UNLIMITED;
                _sgu_link((CLASS _SGU_NODE*)w,NULL);
                to=(CLASS SGU_WIDGET*)_sgu_find(ui->block,cx,cy);
                _sgu_link((CLASS _SGU_NODE*)w,(CLASS _SGU_NODE*)(to?to:from));
                w->x=w->m.x-w->parent->m.x;
                w->y=w->m.y-w->parent->m.y;
            }
        }
        return ui->flags & 32;
    }
    return -1;
}

static void _sgu_events(CLASS SGU_UI * const ui,const SGU * const sgu){
    if( !(ui->flags & 32) ){
        if( (sgu->event & SGU_CURSOR) && (ui->focus=_sgu_find(ui->block,sgu->cursor.x,sgu->cursor.y))!=ui->select)
            return;
        if( (sgu->event & SGU_SCROLL) || ((sgu->event & (SGU_PRESS|SGU_RELEASE)) && (sgu->key & (SGK_LB|SGK_RB|SGK_MB))) )
            ui->select=sgu_select(ui->focus);
    }
    if(!_sgu_basics((CLASS SGU_WIDGET*)ui->select,sgu)){
        SGU event=*sgu;
        while(ui->select){
            CLASS SGU_WIDGET * const w=(CLASS SGU_WIDGET*)ui->select;
            event.cursor.x=sgu->cursor.x-w->m.x;
            event.cursor.y=sgu->cursor.y-w->m.y;
            w->core(w,&event);
            w->onEvent(w,&event);
            if(w==ui->select) break;
        }
    }
}

static void _sgu_focus(CLASS SGU_UI * const ui,const SGU * const sgu){
    if(ui->flags & 32) return;
    ui->focus=_sgu_find((CLASS SGU_WIDGET*)ui->block,sgu->cursor.x,sgu->cursor.y);
}

static void _sgu_draw(CLASS SGU_WIDGET * const ui){
    CLASS SGU_WIDGET *i;
    ui->onDraw(ui,&ui->m);
    for(i=(CLASS SGU_WIDGET*)_sgu_next((CLASS _SGU_NODE*)ui);i!=ui;i=(CLASS SGU_WIDGET*)_sgu_next((CLASS _SGU_NODE*)ui))
        i->onDraw(i,&i->m);
}

static void _sgu_update(CLASS SGU_WIDGET * const ui){
    CLASS SGU_WIDGET *i;
    ui->onDraw(ui,&ui->m);
    for(i=(CLASS SGU_WIDGET*)_sgu_next((CLASS _SGU_NODE*)ui);i!=ui;i=(CLASS SGU_WIDGET*)_sgu_next((CLASS _SGU_NODE*)ui))
        sgm_sub(&i->parent->m,i->x,i->y,i->w,i->h,i->m.flags,&i->m);
}

static void _sgu_ui_run(CLASS SGU_UI * const ui,SGC * const pixels,const unsigned int w,const unsigned int h,const SGU * const sgu){
    if(sgu->event & 0xF){
        sgm_cfg(&((CLASS SGU_WIDGET*)ui)->m,pixels,w,h,sizeof(*pixels));
        _sgu_events(ui,sgu);
        _sgu_update((CLASS SGU_WIDGET*)ui);
        _sgu_focus(ui,sgu);
        _sgu_draw((CLASS SGU_WIDGET*)ui);
    }
}

static void _sgu_ui_draw(const CLASS SGU_UI * const ui,const SGM * const m){
    sgm_square(m,0,0,m->w,m->h,&ui->color);
}

CLASS_COMPILE(SGU_UI)(
    constructor(allocator,deallocator)(
        if(!allocator) allocator=malloc;
        if(!deallocator) deallocator=free;
        if(!self){self=allocator(sizeof(*self)); memset(self,0,sizeof(*self)); if(0)(void)super;}

        super(self,NULL);
        SG_SET(void*,self->allocator,allocator);
        SG_SET(void*,self->deallocator,deallocator);
        SG_SET(void*,self->ui,self);
        SG_SET(void*,self->run,_sgu_ui_run);
        SG_SET(void*,self->onDraw,_sgu_ui_draw);
        self->focus=self->select=self->block=self;
    )
)

#undef SG_SET