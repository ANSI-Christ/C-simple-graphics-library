#include <stdio.h>

#include "sgl/sgl.h"


int main(int argc,char **argv){
    SGW *w=sgw_open();
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
            case SGE_PRESS:
                printf("press %d (%c)\n",w->keys,(char)e->key);
                if(w->keys==SGK_ALT+'c'){
                    unsigned int size=w->rectangle.w*w->rectangle.h;
                    const SGC c=rand();
                    SGC * const p=w->pixel;
                    while(size) p[--size]=c;
                    sgw_render(w);
                }
                break;
        }

    }
    sgw_close(w);
    printf("exit\n");
    return 0;
}
