#include <stdio.h>

#include "sgl/sgl.h"


int main(int argc,char **argv){
    SGW *w=sgw_open(NULL,NULL);
    SGM m[1];
    SGC c;
    if(!w){
        printf("cant open window\n");
        return -1;
    }
    sgw_rect(w,SGW_XYWH,50,50,800,600);
    sgm_cfg(m,w->pixel,w->rectangle.w,w->rectangle.h,sizeof(*w->pixel));

    while(1){
        SGE e[1];
        const enum SGE x=sgw_event(w,-1,e);

        if(x==SGE_CLOSE)
            break;

        switch(x){
            case SGE_SCROLL:
                c=rand();
                continue;
            case SGE_RECTANGLE:
                sgm_cfg(m,w->pixel,w->rectangle.w,w->rectangle.h,sizeof(*w->pixel));
                break;
            case SGE_CURSOR:
                if(w->keys&SGK_LB){
                    sgm_round(m,w->cursor.x,w->cursor.y,5+rand()%20,&c);
                }
                if(w->keys&SGK_RB){
                    const int rect=5+rand()%20;
                    sgm_rect(m,w->cursor.x-(rect>>1),w->cursor.y-(rect>>1),rect,rect,1,&c);
                }
                break;
            case SGE_PRESS:
                if(w->keys==SGK_ALT+'c')
                    sgw_fill(w,SGC_BLACK);
                break;
        }
        sgw_render(w);
    }
    sgw_close(w);
    printf("exit\n");
    return 0;
}
