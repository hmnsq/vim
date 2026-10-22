/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* simx.h: 0.1a                                                                *
*                                                                             *
* This emulates some Xlib functionality for MSW. It's not a general solution, *
* it is close related to XPM-lib. It is only intended to satisfy what is need *
* there. Thus allowing to read XPM files under MS windows.                    *
*                                                                             *
* Developed by HeDu 3/94 (hedu@cul-ipn.uni-kiel.de)                           *
\*****************************************************************************/


#నిర్వచితం_కాకపోతే _SIMX_H
#నిర్వచించు _SIMX_H

#నిర్వచితమైతే FOR_MSW

#అన్తర్భావ "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#ఒకవేళ __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#నిర్వచించు FUNC(f, t, p) బాహ్య t f p
#నిర్వచించు LFUNC(f, t, p) స్థిర t f p
#లేకపోతే /* k&R */
#నిర్వచించు FUNC(f, t, p) బాహ్య t f()
#నిర్వచించు LFUNC(f, t, p) స్థిర t f()
#అయితే_ముగింపు


FUNC(boundCheckingMalloc, శూన్య *, (దీర్ఘ s));
FUNC(boundCheckingCalloc, శూన్య *, (దీర్ఘ అంకె, దీర్ఘ s));
FUNC(boundCheckingRealloc, శూన్య *, (శూన్య *p, దీర్ఘ s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

రకనిర్వచనం HDC Display;			/* this should be similar */
రకనిర్వచనం శూన్య *Screen;			/* not used */
రకనిర్వచనం శూన్య *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

రకనిర్వచనం శూన్య *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

రకనిర్వచనం COLORREF Pixel;

#నిర్వచించు PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

రకనిర్వచనం నిర్మాణం {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

రకనిర్వచనం నిర్మాణం {
    HBITMAP bitmap;
    గుర్తులేని పూర్ణాంకం వెడల్పు;
    గుర్తులేని పూర్ణాంకం ఎత్తు;
    గుర్తులేని పూర్ణాంకం లోతు;
}      XImage;

#ఒకవేళ defined(__cplusplus) || defined(c_plusplus)
బాహ్య "C" {
#అయితే_ముగింపు
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefault_లోతు, పూర్ణాంకం, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, పూర్ణాంకం, (Display *, Colormap *, అక్షరం *, XColor *));
    FUNC(XAllocColor, పూర్ణాంకం, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, శూన్య, (Display *display, Colormap *colormap,
			      XColor *xcolors, పూర్ణాంకం ncolors));
    FUNC(XFreeColors, పూర్ణాంకం, (Display *d, Colormap cmap,
			    గుర్తులేని దీర్ఘ pixels[],
			    పూర్ణాంకం npixels, గుర్తులేని దీర్ఘ planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, పూర్ణాంకం లోతు, పూర్ణాంకం format,
				  పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు,
				  పూర్ణాంకం pad, పూర్ణాంకం foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, శూన్య /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, శూన్య, (XImage *));
#ఒకవేళ defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#అయితే_ముగింపు /* cplusplus */

#నిర్వచించు ZPixmap 1			/* not really used */
#నిర్వచించు XYBitmap 1			/* not really used */

#నిర్వచితం_కాకపోతే True
#నిర్వచించు True 1
#నిర్వచించు False 0
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే Bool
రకనిర్వచనం BOOL Bool;		/* take MSW bool */
#అయితే_ముగింపు
/* make these local here, simx.c gets the same from xpm.h */
#నిర్వచనం_రద్దు LFUNC
#నిర్వచనం_రద్దు FUNC

#అయితే_ముగింపు /* def FOR_MSW */

#అయితే_ముగింపు /* _SIMX_H */
