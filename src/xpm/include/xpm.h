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
* xpm.h:                                                                      *
*                                                                             *
*  XPM library                                                                *
*  Include file                                                               *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

/*
 * The code related to AMIGA has been added by
 * Lorens Younes (d93-hyo@nada.kth.se) 4/96
 */

#ifndef XPM_h
#define XPM_h

/*
 * first some identification numbers:
 * the version and revision numbers are determined with the following rule:
 * SO Major number = LIB minor version number.
 * SO Minor number = LIB sub-minor version number.
 * e.g: Xpm version 3.2f
 *      we forget the 3 which is the format number, 2 gives 2, and f gives 6.
 *      thus we have XpmVersion = 2 and XpmRevision = 6
 *      which gives  SOXPMLIBREV = 2.6
 *
 * Then the XpmIncludeVersion number is built from these numbers.
 */
#define XpmFormat 3
#define XpmVersion 4
#define XpmRevision 11
#define XpmIncludeVersion ((XpmFormat * 100 + XpmVersion) * 100 + XpmRevision)

#ifndef XPM_NUMBERS

#ifdef FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#define NEED_STRCASECMP		/* at least for MSVC++ */
#অন্যথা /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# অন্যথা /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#endif /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#যদি ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
প্রকারনির্দেশ unsigned long Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#endif

/* make sure we know whether function prototypes are needed or not */
#ifndef NeedFunctionPrototypes
# যদি defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# অন্যথা
#  define NeedFunctionPrototypes 0
# endif
#endif


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#define XpmColorError    1
#define XpmSuccess       0
#define XpmOpenFailed   -1
#define XpmFileInvalid  -2
#define XpmNoMemory     -3
#define XpmColorFailed  -4

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *name;			/* Symbolic color name */
    ৱর্ণ *মান;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_প্ৰতীক;

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *name;			/* name of the extension */
    unsigned পূর্ণাঙ্ক nlines;	/* number of lines in this extension */
    ৱর্ণ **সাৰিগুলি;		/* pointer to the extension array of strings */
}      XpmExtension;

প্রকারনির্দেশ সংরচনা {
    ৱর্ণ *string;		/* characters string */
    ৱর্ণ *symbolic;		/* symbolic name */
    ৱর্ণ *m_color;		/* monochrom default */
    ৱর্ণ *g4_color;		/* 4 level grayscale default */
    ৱর্ণ *g_color;		/* other level grayscale default */
    ৱর্ণ *c_color;		/* color default */
}      XpmColor;

প্রকারনির্দেশ সংরচনা {
    unsigned পূর্ণাঙ্ক width;		/* image width */
    unsigned পূর্ণাঙ্ক height;	/* image height */
    unsigned পূর্ণাঙ্ক cpp;		/* number of characters per pixel */
    unsigned পূর্ণাঙ্ক ncolors;	/* number of colors */
    XpmColor *colorTable;	/* list of related colors */
    unsigned পূর্ণাঙ্ক *data;		/* image data */
}      XpmImage;

প্রকারনির্দেশ সংরচনা {
    unsigned long valuemask;	/* Specifies which attributes are defined */
    ৱর্ণ *hints_cmt;		/* Comment of the hints section */
    ৱর্ণ *colors_cmt;		/* Comment of the colors section */
    ৱর্ণ *pixels_cmt;		/* Comment of the pixels section */
    unsigned পূর্ণাঙ্ক x_hotspot;	/* Returns the x hotspot's coordinate */
    unsigned পূর্ণাঙ্ক y_hotspot;	/* Returns the y hotspot's coordinate */
    unsigned পূর্ণাঙ্ক nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

প্রকারনির্দেশ পূর্ণাঙ্ক (*XpmAllocColorFunc)(
#যদি NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    ৱর্ণ*			/* colorname */,
    XColor*			/* xcolor */,
    শূন্য*			/* closure */
#endif
);

প্রকারনির্দেশ পূর্ণাঙ্ক (*XpmFreeColorsFunc)(
#যদি NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    পূর্ণাঙ্ক				/* npixels */,
    শূন্য*			/* closure */
#endif
);

প্রকারনির্দেশ সংরচনা {
    unsigned long valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    unsigned পূর্ণাঙ্ক depth;			/* Specifies the depth */
    unsigned পূর্ণাঙ্ক width;			/* Returns the width of the created
					   pixmap */
    unsigned পূর্ণাঙ্ক height;		/* Returns the height of the created
					   pixmap */
    unsigned পূর্ণাঙ্ক x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    unsigned পূর্ণাঙ্ক y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    unsigned পূর্ণাঙ্ক cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    unsigned পূর্ণাঙ্ক npixels;		/* Number of used pixels */
    Xpm_Color_প্ৰতীক *colorsymbols;	/* List of color symbols to override */
    unsigned পূর্ণাঙ্ক numsymbols;		/* Number of symbols */
    ৱর্ণ *rgb_fname;			/* RGB text file name */
    unsigned পূর্ণাঙ্ক nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    unsigned পূর্ণাঙ্ক ncolors;               /* Number of colors */
    XpmColor *colorTable;               /* List of colors */
/* 3.2 backward compatibility code */
    ৱর্ণ *hints_cmt;                    /* Comment of the hints section */
    ৱর্ণ *colors_cmt;                   /* Comment of the colors section */
    ৱর্ণ *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    unsigned পূর্ণাঙ্ক mask_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    unsigned পূর্ণাঙ্ক closeness;		/* Allowable RGB deviation */
    unsigned পূর্ণাঙ্ক red_closeness;		/* Allowable red deviation */
    unsigned পূর্ণাঙ্ক green_closeness;	/* Allowable green deviation */
    unsigned পূর্ণাঙ্ক blue_closeness;	/* Allowable blue deviation */
    পূর্ণাঙ্ক color_চাবি;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    পূর্ণাঙ্ক nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    পূর্ণাঙ্ক bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    শূন্য *color_closure;		/* Application private data to pass to
					   alloc_color and free_colors */

}      XpmAttributes;

/* XpmAttributes value masks bits */
#define XpmVisual	   (1L<<0)
#define XpmColormap	   (1L<<1)
#define XpmDepth	   (1L<<2)
#define XpmSize		   (1L<<3)	/* width & height */
#define XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#define XpmCharsPerPixel   (1L<<5)
#define XpmColorSymbols	   (1L<<6)
#define XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#define XpmInfos	   (1L<<8)
#define XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#define XpmReturnPixels	   (1L<<9)
#define XpmExtensions      (1L<<10)
#define XpmReturnExtensions XpmExtensions

#define XpmExactColors     (1L<<11)
#define XpmCloseness	   (1L<<12)
#define XpmRGBCloseness	   (1L<<13)
#define Xpm_Color_চাবি	   (1L<<14)

#define XpmColorTable      (1L<<15)
#define XpmReturnColorTable XpmColorTable

#define XpmReturnAllocPixels (1L<<16)
#define XpmAllocCloseColors (1L<<17)
#define XpmBitmapFormat    (1L<<18)

#define XpmAllocColor      (1L<<19)
#define XpmFreeColors      (1L<<20)
#define XpmColorClosure    (1L<<21)


/* XpmInfo value masks bits */
#define XpmComments        XpmInfos
#define XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#ifndef FOR_MSW
#define XpmUndefPixel 0x80000000
#অন্যথা
/* int is only 16 bit for MSW */
#define XpmUndefPixel 0x8000
#endif

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#define XPM_MONO	2
#define XPM_GREY4	3
#define XPM_GRAY4	3
#define XPM_GREY 	4
#define XPM_GRAY 	4
#define XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#যদি NeedFunctionPrototypes
#define FUNC(f, t, p) বাহ্য t f p
#define LFUNC(f, t, p) স্থির t f p
#অন্যথা
#define FUNC(f, t, p) বাহ্য t f()
#define LFUNC(f, t, p) স্থির t f()
#endif


/*
 * functions declarations
 */

#ifdef __cplusplus
বাহ্য "C" {
#endif

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromData, পূর্ণাঙ্ক, (Display *display,
					Drawable d,
					ৱর্ণ **data,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					XpmAttributes *attributes));

    FUNC(XpmCreateDataFromPixmap, পূর্ণাঙ্ক, (Display *display,
					ৱর্ণ ***data_return,
					Pixmap pixmap,
					Pixmap shapemask,
					XpmAttributes *attributes));

    FUNC(XpmReadFileToPixmap, পূর্ণাঙ্ক, (Display *display,
				    Drawable d,
				    ৱর্ণ *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    XpmAttributes *attributes));

    FUNC(XpmWriteFileFromPixmap, পূর্ণাঙ্ক, (Display *display,
				       ৱর্ণ *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       XpmAttributes *attributes));
#endif

    FUNC(XpmCreateImageFromData, পূর্ণাঙ্ক, (Display *display,
				       ৱর্ণ **data,
				       XImage **image_return,
				       XImage **shapemask_return,
				       XpmAttributes *attributes));

    FUNC(XpmCreateDataFromImage, পূর্ণাঙ্ক, (Display *display,
				       ৱর্ণ ***data_return,
				       XImage *image,
				       XImage *shapeimage,
				       XpmAttributes *attributes));

    FUNC(XpmReadFileToImage, পূর্ণাঙ্ক, (Display *display,
				   ৱর্ণ *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   XpmAttributes *attributes));

    FUNC(XpmWriteFileFromImage, পূর্ণাঙ্ক, (Display *display,
				      ৱর্ণ *filename,
				      XImage *image,
				      XImage *shapeimage,
				      XpmAttributes *attributes));

    FUNC(XpmCreateImageFromBuffer, পূর্ণাঙ্ক, (Display *display,
					 ৱর্ণ *buffer,
					 XImage **image_return,
					 XImage **shapemask_return,
					 XpmAttributes *attributes));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromBuffer, পূর্ণাঙ্ক, (Display *display,
					  Drawable d,
					  ৱর্ণ *buffer,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromImage, পূর্ণাঙ্ক, (Display *display,
					 ৱর্ণ **buffer_return,
					 XImage *image,
					 XImage *shapeimage,
					 XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromPixmap, পূর্ণাঙ্ক, (Display *display,
					  ৱর্ণ **buffer_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  XpmAttributes *attributes));
#endif
    FUNC(XpmReadFileToBuffer, পূর্ণাঙ্ক, (ৱর্ণ *filename, ৱর্ণ **buffer_return));
    FUNC(XpmWriteFileFromBuffer, পূর্ণাঙ্ক, (ৱর্ণ *filename, ৱর্ণ *buffer));

    FUNC(XpmReadFileToData, পূর্ণাঙ্ক, (ৱর্ণ *filename, ৱর্ণ ***data_return));
    FUNC(XpmWriteFileFromData, পূর্ণাঙ্ক, (ৱর্ণ *filename, ৱর্ণ **data));

    FUNC(XpmAttributesSize, পূর্ণাঙ্ক, ());
    FUNC(XpmFreeAttributes, শূন্য, (XpmAttributes *attributes));
    FUNC(XpmFreeExtensions, শূন্য, (XpmExtension *extensions,
				   পূর্ণাঙ্ক nextensions));

    FUNC(XpmFreeXpmImage, শূন্য, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, শূন্য, (XpmInfo *info));
    FUNC(XpmGetErrorString, ৱর্ণ *, (পূর্ণাঙ্ক errcode));
    FUNC(XpmLibraryVersion, পূর্ণাঙ্ক, ());

    /* XpmImage functions */
    FUNC(XpmReadFileToXpmImage, পূর্ণাঙ্ক, (ৱর্ণ *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, পূর্ণাঙ্ক, (ৱর্ণ *filename,
					 XpmImage *image,
					 XpmInfo *info));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, পূর্ণাঙ্ক, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateImageFromXpmImage, পূর্ণাঙ্ক, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   XpmAttributes *attributes));

    FUNC(XpmCreateXpmImageFromImage, পূর্ণাঙ্ক, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   XpmAttributes *attributes));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, পূর্ণাঙ্ক, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateDataFromXpmImage, পূর্ণাঙ্ক, (ৱর্ণ ***data_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromData, পূর্ণাঙ্ক, (ৱর্ণ **data,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromBuffer, পূর্ণাঙ্ক, (ৱর্ণ *buffer,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmCreateBufferFromXpmImage, পূর্ণাঙ্ক, (ৱর্ণ **buffer_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, পূর্ণাঙ্ক, (ৱর্ণ *filename,
				 পূর্ণাঙ্ক *linenum_return,
				 পূর্ণাঙ্ক *charnum_return));

    FUNC(XpmFree, শূন্য, (শূন্য *ptr));

#ifdef __cplusplus
} /* for C++ V2.0 */
#endif


/* backward compatibility */

/* for version 3.0c */
#define XpmPixmapColorError  XpmColorError
#define XpmPixmapSuccess     XpmSuccess
#define XpmPixmapOpenFailed  XpmOpenFailed
#define XpmPixmapFileInvalid XpmFileInvalid
#define XpmPixmapNoMemory    XpmNoMemory
#define XpmPixmapColorFailed XpmColorFailed

#define XpmReadPixmapFile(dpy, d, file, pix, mask, att) \
    XpmReadFileToPixmap(dpy, d, file, pix, mask, att)
#define XpmWritePixmapFile(dpy, file, pix, mask, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, mask, att)

/* for version 3.0b */
#define PixmapColorError  XpmColorError
#define PixmapSuccess     XpmSuccess
#define PixmapOpenFailed  XpmOpenFailed
#define PixmapFileInvalid XpmFileInvalid
#define PixmapNoMemory    XpmNoMemory
#define PixmapColorFailed XpmColorFailed

#define Color_প্ৰতীক Xpm_Color_প্ৰতীক

#define XReadPixmapFile(dpy, d, file, pix, mask, att) \
    XpmReadFileToPixmap(dpy, d, file, pix, mask, att)
#define XWritePixmapFile(dpy, file, pix, mask, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, mask, att)
#define XCreatePixmapFromData(dpy, d, data, pix, mask, att) \
    XpmCreatePixmapFromData(dpy, d, data, pix, mask, att)
#define XCreateDataFromPixmap(dpy, data, pix, mask, att) \
    XpmCreateDataFromPixmap(dpy, data, pix, mask, att)

#endif /* XPM_NUMBERS */
#endif
