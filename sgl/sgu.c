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


static void *_sgu_find(CLASS _SGU_NODE *w,const int x,const int y){
    void *p=NULL;
    if(w && sgm_at(&w->m,x-w->m.x,y-w->m.y)){
_mark:
        p=w; w=w->last;
        while(w){
            if(((CLASS SGU_WIDGET*)w)->able && sgm_at(&w->m,x-w->m.x,y-w->m.y))
                goto _mark;
            w=w->prev;
        }
    }
    return p;
}

static void _sgu_draw(CLASS SGU_WIDGET * const w){
    CLASS _SGU_NODE *i=(CLASS _SGU_NODE*)w, *p;
    while(1)
        if( ((CLASS SGU_WIDGET*)i)->visible ){
            ((CLASS SGU_WIDGET*)i)->onDraw((p=i),&i->m);
            if( (i=i->child) ) continue;
            if( (i=p->next) ) continue;
            if(p->parent==w) return;
            if( (i=((CLASS _SGU_NODE*)p->parent)->next) ) continue;
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


static void _sgu_default(void){}



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
        SG_SET(void*,self->onDraw,_sgu_default);
        SG_SET(void*,self->onInput,_sgu_default);
        SG_SET(void*,self->onSelect,_sgu_default);
        SG_SET(void*,self->onUpdate,_sgu_default);
        self->able=self->visible=1;
    )
)


static void _sgu_ui_draw(CLASS SGU_UI *self,const SGM *m){
    sgm_square(m,0,0,m->w,m->h,&self->color);
}

CLASS_COMPILE(SGU_UI)(
    constructor(allocator,deallocator)(
        if(!allocator) allocator=malloc;
        if(!deallocator) deallocator=free;
        if(!self){self=allocator(sizeof(*self)); memset(self,0,sizeof(*self)); (void)super;}
        super(self,NULL);
        SG_SET(void*,self->allocator,allocator);
        SG_SET(void*,self->deallocator,deallocator);
        SG_SET(void*,self->ui,self);
        SG_SET(void*,self->onDraw,_sgu_ui_draw);
    )
)

#undef SG_SET