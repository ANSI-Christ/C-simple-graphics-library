/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SGL_MACOS_UNDERSTAND

struct WARNING{char _[-1];};

static char WARNING[4][1]={\
    {"if you read this, then in makefile do:"},
    {"1. C_FLAGS += -DSGL_MACOS_UNDERSTAND"},
    {"2. SOURCES_M += ./sgl/sgw/sg_cocoa.m"},
    {"3. LD_FLAGS += -framework Coroa"},
};

#endif