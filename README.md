# C simple graphics library (sgl)
A simple cross-platform graphics library, based on OS native frameworks: X11, WinApi, Cocoa, etc.


Modules:
| name |     reference    |         sources        |         dependencies         |
| :--: | :--------------: | :--------------------: | :--------------------------: |
|  sgc |      colors      |          sgc.h         |                              |
|  sgk | keyboard & mouse |          sgk.h         |                              |
|  sgw |      window      | sgw.h, sgw.c, sgw/...  |           sgc, sgk           |
|  sgp |     painting     | sgp.h, sgp.c, sgf/...  |                              |
|  sgu |  user interface  |       sgu.h, sgu.c     |  sgk, sgp, marco.h, class.h  |
|  sgl |   full library   |       sgl.h, sgl.c     |   sgc, sgk, sgw, sgp, sgu    |


Link with flags:
 - Windows: -lgdi32
 - Unix:    -lX11
 - MacOS:   undone yet

To do:
 - macos realization
 - sgu module

Look for usage examples in main.c
