/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SGL_MACOS_UNDERSTAND

struct WARNING{char _[-1];};

static char WARNING[4][1]={\
    {"if you read this, then do:"},
    {"1. gcc sg_cocoa.m -framework Cocoa -o sg_cocoa.o"},
    {"2. define SGL_MACOS_UNDERSTAND in makefile"},
    {"3. compile sgl with OBJECTS_ADD += sg_cocoa.o"}
};

#endif