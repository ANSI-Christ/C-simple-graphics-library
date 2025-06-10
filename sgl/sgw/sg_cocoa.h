/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#include <objc/runtime.h>
#include <objc/message.h>
#include "macro.h"

#define _CLASS_GLOBAL(_0_,_1_,...) static Class OBJC_CLS(__VA_ARGS__);
#define _CLASS_INIT(_0_,_1_,...) OBJC_CLS(__VA_ARGS__)=objc_getClass(#__VA_ARGS__);
#define _CLASSES(...) M_FOREACH(_CLASS_GLOBAL,-,__VA_ARGS__) static void _objc_init_cls(void){M_FOREACH(_CLASS_INIT,-,__VA_ARGS__)}

#define _SELECTOR_GLOBAL(_0_,_1_,...) static SEL OBJC_SEL(__VA_ARGS__);
#define _SELECTOR_INIT(_0_,_1_,...) OBJC_SEL(__VA_ARGS__)=objc_getClass(#__VA_ARGS__);
#define _SELECTORS(...) M_FOREACH(_SELECTOR_GLOBAL,-,__VA_ARGS__) static void _objc_init_sel(void){M_FOREACH(_SELECTOR_INIT,-,__VA_ARGS__)}

#define OBJC_CLS(_1_) _objc_cls_##_1_
#define OBJC_SEL(_1_) _objc_sel_##_1_
#define OBJC_MSG(_obj_,_sel_,...) ((id(*)(id,SEL,...))objc_msgSend)(_obj_,OBJC_SEL(_sel_),__VA_ARGS__)

_CLASSES(
    NSWindow,
    NSString,
)

_SELECTORS(

)

static void _objc_init(void){
    _objc_init_cls()
    _objc_init_sel();
}



#undef _CLASS_GLOBAL
#undef _CLASS_INIT
#undef _CLASSES
#undef _SELECTOR_GLOBAL
#undef _SELECTOR_INIT
#undef _SELECTORS
#undef OBJC_MSG
#undef OBJC_CLS
#undef OBJC_SEL
