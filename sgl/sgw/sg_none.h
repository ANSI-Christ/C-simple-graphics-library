/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */


SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*)){
    return NULL; (void)allocator; (void)deallocator;
}

void sgw_close(SGW *w){
    return; (void)w;
}

void sgw_render(SGW *w){
    return; (void)w;
}

void sgw_async(SGW *w,const void *p){
    return; (void)w; (void)p;
}

void sgw_rect(SGW *w,enum SGW mode,...){
    return; (void)w; (void)mode;
}

void sgw_title(SGW *w,const char *title){
    return; (void)w; (void)title;
}

void sgw_cursor(SGW *w,unsigned char visible){
    return; (void)w; (void)visible;
}

enum SGE sgw_event(SGW *w,int t,SGE *e){
    return SGE_CLOSE; (void)w; (void)t; (void)e;
}
