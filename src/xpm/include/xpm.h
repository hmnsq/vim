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
প্রকারলেখ unsigned long Pixel;	/* Index into colormap */
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

প্রকারলেখ কাঠামো {
    অক্ষর *name;			/* Symbolic color name */
    অক্ষর *মান;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_প্রতীক;

প্রকারলেখ কাঠামো {
    অক্ষর *name;			/* name of the extension */
    unsigned পূর্ণসংখ্যা nlines;	/* number of lines in this extension */
    অক্ষর **সারিগুলি;		/* pointer to the extension array of strings */
}      XpmExtension;

প্রকারলেখ কাঠামো {
    অক্ষর *string;		/* characters string */
    অক্ষর *symbolic;		/* symbolic name */
    অক্ষর *m_color;		/* monochrom default */
    অক্ষর *g4_color;		/* 4 level grayscale default */
    অক্ষর *g_color;		/* other level grayscale default */
    অক্ষর *c_color;		/* color default */
}      XpmColor;

প্রকারলেখ কাঠামো {
    unsigned পূর্ণসংখ্যা width;		/* image width */
    unsigned পূর্ণসংখ্যা height;	/* image height */
    unsigned পূর্ণসংখ্যা cpp;		/* number of characters per pixel */
    unsigned পূর্ণসংখ্যা ncolors;	/* number of colors */
    XpmColor *colorTable;	/* list of related colors */
    unsigned পূর্ণসংখ্যা *তথ্য;		/* image data */
}      XpmImage;

প্রকারলেখ কাঠামো {
    unsigned long valuemask;	/* Specifies which attributes are defined */
    অক্ষর *hints_cmt;		/* Comment of the hints section */
    অক্ষর *colors_cmt;		/* Comment of the colors section */
    অক্ষর *pixels_cmt;		/* Comment of the pixels section */
    unsigned পূর্ণসংখ্যা x_hotspot;	/* Returns the x hotspot's coordinate */
    unsigned পূর্ণসংখ্যা y_hotspot;	/* Returns the y hotspot's coordinate */
    unsigned পূর্ণসংখ্যা nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

প্রকারলেখ পূর্ণসংখ্যা (*XpmAllocColorFunc)(
#যদি NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    অক্ষর*			/* colorname */,
    XColor*			/* xcolor */,
    শূন্য*			/* closure */
#endif
);

প্রকারলেখ পূর্ণসংখ্যা (*XpmFreeColorsFunc)(
#যদি NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    পূর্ণসংখ্যা				/* npixels */,
    শূন্য*			/* closure */
#endif
);

প্রকারলেখ কাঠামো {
    unsigned long valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    unsigned পূর্ণসংখ্যা depth;			/* Specifies the depth */
    unsigned পূর্ণসংখ্যা width;			/* Returns the width of the created
					   pixmap */
    unsigned পূর্ণসংখ্যা height;		/* Returns the height of the created
					   pixmap */
    unsigned পূর্ণসংখ্যা x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    unsigned পূর্ণসংখ্যা y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    unsigned পূর্ণসংখ্যা cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    unsigned পূর্ণসংখ্যা npixels;		/* Number of used pixels */
    Xpm_Color_প্রতীক *colorsymbols;	/* List of color symbols to override */
    unsigned পূর্ণসংখ্যা numsymbols;		/* Number of symbols */
    অক্ষর *rgb_fname;			/* RGB text file name */
    unsigned পূর্ণসংখ্যা nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    unsigned পূর্ণসংখ্যা ncolors;               /* Number of colors */
    XpmColor *colorTable;               /* List of colors */
/* 3.2 backward compatibility code */
    অক্ষর *hints_cmt;                    /* Comment of the hints section */
    অক্ষর *colors_cmt;                   /* Comment of the colors section */
    অক্ষর *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    unsigned পূর্ণসংখ্যা mask_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    unsigned পূর্ণসংখ্যা closeness;		/* Allowable RGB deviation */
    unsigned পূর্ণসংখ্যা red_closeness;		/* Allowable red deviation */
    unsigned পূর্ণসংখ্যা green_closeness;	/* Allowable green deviation */
    unsigned পূর্ণসংখ্যা blue_closeness;	/* Allowable blue deviation */
    পূর্ণসংখ্যা color_চাবি;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    পূর্ণসংখ্যা nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    পূর্ণসংখ্যা bitmap_format;			/* Specify the format of 1bit depth
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
#define Xpm_আকার		   (1L<<3)	/* width & height */
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
#define FUNC(f, t, p) বাহ্যিক t f p
#define LFUNC(f, t, p) স্থির t f p
#অন্যথা
#define FUNC(f, t, p) বাহ্যিক t f()
#define LFUNC(f, t, p) স্থির t f()
#endif


/*
 * functions declarations
 */

#ifdef __cplusplus
বাহ্যিক "C" {
#endif

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_তথ্য, পূর্ণসংখ্যা, (Display *display,
					Drawable d,
					অক্ষর **তথ্য,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					XpmAttributes *attributes));

    FUNC(Xpm_Create_তথ্য_From_Pixmap, পূর্ণসংখ্যা, (Display *display,
					অক্ষর ***তথ্য_return,
					Pixmap pixmap,
					Pixmap shapemask,
					XpmAttributes *attributes));

    FUNC(XpmReadFileToPixmap, পূর্ণসংখ্যা, (Display *display,
				    Drawable d,
				    অক্ষর *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    XpmAttributes *attributes));

    FUNC(XpmWriteFileFromPixmap, পূর্ণসংখ্যা, (Display *display,
				       অক্ষর *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       XpmAttributes *attributes));
#endif

    FUNC(Xpm_Create_Image_From_তথ্য, পূর্ণসংখ্যা, (Display *display,
				       অক্ষর **তথ্য,
				       XImage **image_return,
				       XImage **shapemask_return,
				       XpmAttributes *attributes));

    FUNC(Xpm_Create_তথ্য_From_Image, পূর্ণসংখ্যা, (Display *display,
				       অক্ষর ***তথ্য_return,
				       XImage *image,
				       XImage *shapeimage,
				       XpmAttributes *attributes));

    FUNC(XpmReadFileToImage, পূর্ণসংখ্যা, (Display *display,
				   অক্ষর *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   XpmAttributes *attributes));

    FUNC(XpmWriteFileFromImage, পূর্ণসংখ্যা, (Display *display,
				      অক্ষর *filename,
				      XImage *image,
				      XImage *shapeimage,
				      XpmAttributes *attributes));

    FUNC(Xpm_Create_Image_From_বাফার, পূর্ণসংখ্যা, (Display *display,
					 অক্ষর *বাফার,
					 XImage **image_return,
					 XImage **shapemask_return,
					 XpmAttributes *attributes));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_বাফার, পূর্ণসংখ্যা, (Display *display,
					  Drawable d,
					  অক্ষর *বাফার,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  XpmAttributes *attributes));

    FUNC(Xpm_Create_বাফার_From_Image, পূর্ণসংখ্যা, (Display *display,
					 অক্ষর **বাফার_return,
					 XImage *image,
					 XImage *shapeimage,
					 XpmAttributes *attributes));

    FUNC(Xpm_Create_বাফার_From_Pixmap, পূর্ণসংখ্যা, (Display *display,
					  অক্ষর **বাফার_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  XpmAttributes *attributes));
#endif
    FUNC(Xpm_Read_File_To_বাফার, পূর্ণসংখ্যা, (অক্ষর *filename, অক্ষর **বাফার_return));
    FUNC(Xpm_Write_File_From_বাফার, পূর্ণসংখ্যা, (অক্ষর *filename, অক্ষর *বাফার));

    FUNC(Xpm_Read_File_To_তথ্য, পূর্ণসংখ্যা, (অক্ষর *filename, অক্ষর ***তথ্য_return));
    FUNC(Xpm_Write_File_From_তথ্য, পূর্ণসংখ্যা, (অক্ষর *filename, অক্ষর **তথ্য));

    FUNC(Xpm_Attributes_আকার, পূর্ণসংখ্যা, ());
    FUNC(XpmFreeAttributes, শূন্য, (XpmAttributes *attributes));
    FUNC(XpmFreeExtensions, শূন্য, (XpmExtension *extensions,
				   পূর্ণসংখ্যা nextensions));

    FUNC(XpmFreeXpmImage, শূন্য, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, শূন্য, (XpmInfo *info));
    FUNC(XpmGetErrorString, অক্ষর *, (পূর্ণসংখ্যা errcode));
    FUNC(XpmLibraryVersion, পূর্ণসংখ্যা, ());

    /* XpmImage functions */
    FUNC(XpmReadFileToXpmImage, পূর্ণসংখ্যা, (অক্ষর *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, পূর্ণসংখ্যা, (অক্ষর *filename,
					 XpmImage *image,
					 XpmInfo *info));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, পূর্ণসংখ্যা, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateImageFromXpmImage, পূর্ণসংখ্যা, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   XpmAttributes *attributes));

    FUNC(XpmCreateXpmImageFromImage, পূর্ণসংখ্যা, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   XpmAttributes *attributes));
#যদি !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, পূর্ণসংখ্যা, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    XpmAttributes *attributes));
#endif
    FUNC(Xpm_Create_তথ্য_From_Xpm_Image, পূর্ণসংখ্যা, (অক্ষর ***তথ্য_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_তথ্য, পূর্ণসংখ্যা, (অক্ষর **তথ্য,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_বাফার, পূর্ণসংখ্যা, (অক্ষর *বাফার,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_বাফার_From_Xpm_Image, পূর্ণসংখ্যা, (অক্ষর **বাফার_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, পূর্ণসংখ্যা, (অক্ষর *filename,
				 পূর্ণসংখ্যা *linenum_return,
				 পূর্ণসংখ্যা *charnum_return));

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

#define Color_প্রতীক Xpm_Color_প্রতীক

#define XReadPixmapFile(dpy, d, file, pix, mask, att) \
    XpmReadFileToPixmap(dpy, d, file, pix, mask, att)
#define XWritePixmapFile(dpy, file, pix, mask, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, mask, att)
#define XCreate_Pixmap_From_তথ্য(dpy, d, তথ্য, pix, mask, att) \
    Xpm_Create_Pixmap_From_তথ্য(dpy, d, তথ্য, pix, mask, att)
#define XCreate_তথ্য_From_Pixmap(dpy, তথ্য, pix, mask, att) \
    Xpm_Create_তথ্য_From_Pixmap(dpy, তথ্য, pix, mask, att)

#endif /* XPM_NUMBERS */
#endif
