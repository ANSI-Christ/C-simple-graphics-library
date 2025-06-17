#include <stdio.h>

#include "sgl/sgl.h"


int main(int argc,char **argv){
    SGW *w=sgw_open();
    SGM m[1];
    SGC c;
    if(!w){
        printf("cant open window\n");
        return -1;
    }
    sgw_rect(w,50,50,800,600);

    while(1){
        SGE e[1];
        const enum SGE x=sgw_event(w,-1,e);

        if(x==SGE_CLOSE)
            break;

        switch(x){
            case SGE_SCROLL:
                c=rand();
                continue;
            case SGE_CURSOR:
                if(w->keys&SGK_LB){
                    sgm_cfg(m,w->pixel,w->rectangle.w,w->rectangle.h,sizeof(SGC));
                    sgm_round(m,w->cursor.x,w->cursor.y,5+rand()%20,&c);
                }
                if(w->keys&SGK_RB){
                    const int rect=5+rand()%20;
                    sgm_cfg(m,w->pixel,w->rectangle.w,w->rectangle.h,sizeof(SGC));
                    sgm_rect(m,w->cursor.x-(rect>>1),w->cursor.y-(rect>>1),rect,rect,&c);
                }
                break;
            case SGE_PRESS:
                printf("press %d (%c)\n",w->keys,(char)e->key);
                if(w->keys==SGK_ALT+'c'){
                    unsigned int size=w->rectangle.w*w->rectangle.h;
                    SGC * const p=w->pixel;
                    while(size) p[--size]=SGC_BLACK;
                }
                break;
        }
        sgw_render(w);
    }
    sgw_close(w);
    printf("exit\n");
    return 0;
}
