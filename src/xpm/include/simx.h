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


#اگر_غیر_تعریف_شدہ _SIMX_H
#تعریف_کرو _SIMX_H

#اگر_تعریف_شدہ FOR_MSW

#شامل "windows.h"			/* MS windows GDI types */

/*
 * minimal portability layer between ansi and KR C
 */
/* this comes from xpm.h, and is here again, to avoid complicated
    includes, since this is included from xpm.h */
/* these defines get undefed at the end of this file */
#اگر __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#تعریف_کرو FUNC(f, t, p) خارجی t f p
#تعریف_کرو LFUNC(f, t, p) ساکن t f p
#ورنہ /* k&R */
#تعریف_کرو FUNC(f, t, p) خارجی t f()
#تعریف_کرو LFUNC(f, t, p) ساکن t f()
#اگر_ختم


FUNC(boundCheckingMalloc, خالی *, (طویل s));
FUNC(boundCheckingCalloc, خالی *, (طویل عدد, طویل s));
FUNC(boundCheckingRealloc, خالی *, (خالی *p, طویل s));

/* define MSW types for X window types,
   I don't know much about MSW, but the following defines do the job */

قسمتعریف HDC Display;			/* this should be similar */
قسمتعریف خالی *Screen;			/* not used */
قسمتعریف خالی *Visual;			/* not used yet, is for GRAY, COLOR,
					 * MONO */

قسمتعریف خالی *Colormap;			/* should be COLORPALETTE, not done
					 * yet */

قسمتعریف COLORREF Pixel;

#تعریف_کرو PIXEL_ALREADY_TYPEDEFED		/* to let xpm.h know about it */

قسمتعریف ساخت {
    Pixel pixel;
    BYTE red, green, blue;
}      XColor;

قسمتعریف ساخت {
    HBITMAP bitmap;
    غیرعلامتی صحیح چوڑائی;
    غیرعلامتی صحیح اونچائی;
    غیرعلامتی صحیح گہرائی;
}      XImage;

#اگر defined(__cplusplus) || defined(c_plusplus)
خارجی "C" {
#اگر_ختم
/* some replacements for X... functions */

/* XDefaultXXX */
    FUNC(XDefaultVisual, Visual *, (Display *display, Screen *screen));
    FUNC(XDefaultScreen, Screen *, (Display *d));
    FUNC(XDefaultColormap, Colormap *, (Display *display, Screen *screen));
    FUNC(XDefault_گہرائی, صحیح, (Display *d, Screen *s));

/* color related */
    FUNC(XParseColor, صحیح, (Display *, Colormap *, حرف *, XColor *));
    FUNC(XAllocColor, صحیح, (Display *, Colormap *, XColor *));
    FUNC(XQueryColors, خالی, (Display *display, Colormap *colormap,
			      XColor *xcolors, صحیح ncolors));
    FUNC(XFreeColors, صحیح, (Display *d, Colormap cmap,
			    غیرعلامتی طویل pixels[],
			    صحیح npixels, غیرعلامتی طویل planes));
/* XImage */
    FUNC(XCreateImage, XImage *, (Display *, Visual *, صحیح گہرائی, صحیح format,
				  صحیح x, صحیح y, صحیح چوڑائی, صحیح اونچائی,
				  صحیح pad, صحیح foo));

/* free and destroy bitmap */
    FUNC(XDestroyImage, خالی /* ? */ , (XImage *));
/* free only, bitmap remains */
    FUNC(XImageFree, خالی, (XImage *));
#اگر defined(__cplusplus) || defined(c_plusplus)
} /* end of extern "C" */
#اگر_ختم /* cplusplus */

#تعریف_کرو ZPixmap 1			/* not really used */
#تعریف_کرو XYBitmap 1			/* not really used */

#اگر_غیر_تعریف_شدہ True
#تعریف_کرو True 1
#تعریف_کرو False 0
#اگر_ختم
#اگر_غیر_تعریف_شدہ Bool
قسمتعریف BOOL Bool;		/* take MSW bool */
#اگر_ختم
/* make these local here, simx.c gets the same from xpm.h */
#تعریف_منسوخ LFUNC
#تعریف_منسوخ FUNC

#اگر_ختم /* def FOR_MSW */

#اگر_ختم /* _SIMX_H */
