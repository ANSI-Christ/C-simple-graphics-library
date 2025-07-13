#define _SG_STR(_1_) #_1_
#define _SG_INC(_n_) _SG_STR(sg_##_n_)

#ifdef __APPLE__
    #ifndef SG_IMPL
        #define SG_IMPL _SG_INC(cocoa.h)
    #endif
#endif

#ifdef __WIN32
    #ifndef SG_IMPL
        #define SG_IMPL _SG_INC(winapi.h)
    #endif
#endif

#ifdef __unix__
    #ifndef SG_IMPL
        #define SG_IMPL _SG_INC(x11.h)
    #endif
#endif

#ifdef SG_IMPL
    #include SG_IMPL
#endif

#undef _SG_INC
#undef _SG_STR