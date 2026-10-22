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
#अण्णहा /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# अण्णहा /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#endif /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#जइ ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
प्रकारनिर्देश unsigned long Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#endif

/* make sure we know whether function prototypes are needed or not */
#ifndef NeedFunctionPrototypes
# जइ defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# अण्णहा
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

प्रकारनिर्देश संरचना {
    वर्ण *name;			/* Symbolic color name */
    वर्ण *मानम्;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_चिह्नम्;

प्रकारनिर्देश संरचना {
    वर्ण *name;			/* name of the extension */
    unsigned पूर्णाङ्क nlines;	/* number of lines in this extension */
    वर्ण **पङ्क्तयः;		/* pointer to the extension array of strings */
}      XpmExtension;

प्रकारनिर्देश संरचना {
    वर्ण *string;		/* characters string */
    वर्ण *symbolic;		/* symbolic name */
    वर्ण *m_color;		/* monochrom default */
    वर्ण *g4_color;		/* 4 level grayscale default */
    वर्ण *g_color;		/* other level grayscale default */
    वर्ण *c_color;		/* color default */
}      XpmColor;

प्रकारनिर्देश संरचना {
    unsigned पूर्णाङ्क width;		/* image width */
    unsigned पूर्णाङ्क height;	/* image height */
    unsigned पूर्णाङ्क cpp;		/* number of characters per pixel */
    unsigned पूर्णाङ्क ncolors;	/* number of colors */
    XpmColor *colorTable;	/* list of related colors */
    unsigned पूर्णाङ्क *data;		/* image data */
}      XpmImage;

प्रकारनिर्देश संरचना {
    unsigned long valuemask;	/* Specifies which attributes are defined */
    वर्ण *hints_cmt;		/* Comment of the hints section */
    वर्ण *colors_cmt;		/* Comment of the colors section */
    वर्ण *pixels_cmt;		/* Comment of the pixels section */
    unsigned पूर्णाङ्क x_hotspot;	/* Returns the x hotspot's coordinate */
    unsigned पूर्णाङ्क y_hotspot;	/* Returns the y hotspot's coordinate */
    unsigned पूर्णाङ्क nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

प्रकारनिर्देश पूर्णाङ्क (*XpmAllocColorFunc)(
#जइ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    वर्ण*			/* colorname */,
    XColor*			/* xcolor */,
    शून्य*			/* closure */
#endif
);

प्रकारनिर्देश पूर्णाङ्क (*XpmFreeColorsFunc)(
#जइ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    पूर्णाङ्क				/* npixels */,
    शून्य*			/* closure */
#endif
);

प्रकारनिर्देश संरचना {
    unsigned long valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    unsigned पूर्णाङ्क depth;			/* Specifies the depth */
    unsigned पूर्णाङ्क width;			/* Returns the width of the created
					   pixmap */
    unsigned पूर्णाङ्क height;		/* Returns the height of the created
					   pixmap */
    unsigned पूर्णाङ्क x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    unsigned पूर्णाङ्क y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    unsigned पूर्णाङ्क cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    unsigned पूर्णाङ्क npixels;		/* Number of used pixels */
    Xpm_Color_चिह्नम् *colorsymbols;	/* List of color symbols to override */
    unsigned पूर्णाङ्क numsymbols;		/* Number of symbols */
    वर्ण *rgb_fname;			/* RGB text file name */
    unsigned पूर्णाङ्क nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    unsigned पूर्णाङ्क ncolors;               /* Number of colors */
    XpmColor *colorTable;               /* List of colors */
/* 3.2 backward compatibility code */
    वर्ण *hints_cmt;                    /* Comment of the hints section */
    वर्ण *colors_cmt;                   /* Comment of the colors section */
    वर्ण *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    unsigned पूर्णाङ्क mask_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    unsigned पूर्णाङ्क closeness;		/* Allowable RGB deviation */
    unsigned पूर्णाङ्क red_closeness;		/* Allowable red deviation */
    unsigned पूर्णाङ्क green_closeness;	/* Allowable green deviation */
    unsigned पूर्णाङ्क blue_closeness;	/* Allowable blue deviation */
    पूर्णाङ्क color_कुञ्चिका;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    पूर्णाङ्क nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    पूर्णाङ्क bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    शून्य *color_closure;		/* Application private data to pass to
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
#define Xpm_Color_कुञ्चिका	   (1L<<14)

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
#अण्णहा
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
#जइ NeedFunctionPrototypes
#define FUNC(f, t, p) बाह्य t f p
#define LFUNC(f, t, p) स्थिर t f p
#अण्णहा
#define FUNC(f, t, p) बाह्य t f()
#define LFUNC(f, t, p) स्थिर t f()
#endif


/*
 * functions declarations
 */

#ifdef __cplusplus
बाह्य "C" {
#endif

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#जइ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromData, पूर्णाङ्क, (Display *display,
					Drawable d,
					वर्ण **data,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					XpmAttributes *attributes));

    FUNC(XpmCreateDataFromPixmap, पूर्णाङ्क, (Display *display,
					वर्ण ***data_return,
					Pixmap pixmap,
					Pixmap shapemask,
					XpmAttributes *attributes));

    FUNC(XpmReadFileToPixmap, पूर्णाङ्क, (Display *display,
				    Drawable d,
				    वर्ण *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    XpmAttributes *attributes));

    FUNC(XpmWriteFileFromPixmap, पूर्णाङ्क, (Display *display,
				       वर्ण *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       XpmAttributes *attributes));
#endif

    FUNC(XpmCreateImageFromData, पूर्णाङ्क, (Display *display,
				       वर्ण **data,
				       XImage **image_return,
				       XImage **shapemask_return,
				       XpmAttributes *attributes));

    FUNC(XpmCreateDataFromImage, पूर्णाङ्क, (Display *display,
				       वर्ण ***data_return,
				       XImage *image,
				       XImage *shapeimage,
				       XpmAttributes *attributes));

    FUNC(XpmReadFileToImage, पूर्णाङ्क, (Display *display,
				   वर्ण *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   XpmAttributes *attributes));

    FUNC(XpmWriteFileFromImage, पूर्णाङ्क, (Display *display,
				      वर्ण *filename,
				      XImage *image,
				      XImage *shapeimage,
				      XpmAttributes *attributes));

    FUNC(XpmCreateImageFromBuffer, पूर्णाङ्क, (Display *display,
					 वर्ण *buffer,
					 XImage **image_return,
					 XImage **shapemask_return,
					 XpmAttributes *attributes));
#जइ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromBuffer, पूर्णाङ्क, (Display *display,
					  Drawable d,
					  वर्ण *buffer,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromImage, पूर्णाङ्क, (Display *display,
					 वर्ण **buffer_return,
					 XImage *image,
					 XImage *shapeimage,
					 XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromPixmap, पूर्णाङ्क, (Display *display,
					  वर्ण **buffer_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  XpmAttributes *attributes));
#endif
    FUNC(XpmReadFileToBuffer, पूर्णाङ्क, (वर्ण *filename, वर्ण **buffer_return));
    FUNC(XpmWriteFileFromBuffer, पूर्णाङ्क, (वर्ण *filename, वर्ण *buffer));

    FUNC(XpmReadFileToData, पूर्णाङ्क, (वर्ण *filename, वर्ण ***data_return));
    FUNC(XpmWriteFileFromData, पूर्णाङ्क, (वर्ण *filename, वर्ण **data));

    FUNC(XpmAttributesSize, पूर्णाङ्क, ());
    FUNC(XpmFreeAttributes, शून्य, (XpmAttributes *attributes));
    FUNC(XpmFreeExtensions, शून्य, (XpmExtension *extensions,
				   पूर्णाङ्क nextensions));

    FUNC(XpmFreeXpmImage, शून्य, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, शून्य, (XpmInfo *info));
    FUNC(XpmGetErrorString, वर्ण *, (पूर्णाङ्क errcode));
    FUNC(XpmLibraryVersion, पूर्णाङ्क, ());

    /* XpmImage functions */
    FUNC(XpmReadFileToXpmImage, पूर्णाङ्क, (वर्ण *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, पूर्णाङ्क, (वर्ण *filename,
					 XpmImage *image,
					 XpmInfo *info));
#जइ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, पूर्णाङ्क, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateImageFromXpmImage, पूर्णाङ्क, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   XpmAttributes *attributes));

    FUNC(XpmCreateXpmImageFromImage, पूर्णाङ्क, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   XpmAttributes *attributes));
#जइ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, पूर्णाङ्क, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateDataFromXpmImage, पूर्णाङ्क, (वर्ण ***data_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromData, पूर्णाङ्क, (वर्ण **data,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromBuffer, पूर्णाङ्क, (वर्ण *buffer,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmCreateBufferFromXpmImage, पूर्णाङ्क, (वर्ण **buffer_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, पूर्णाङ्क, (वर्ण *filename,
				 पूर्णाङ्क *linenum_return,
				 पूर्णाङ्क *charnum_return));

    FUNC(XpmFree, शून्य, (शून्य *ptr));

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

#define Color_चिह्नम् Xpm_Color_चिह्नम्

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
