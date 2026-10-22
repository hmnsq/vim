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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XPM_h
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_h

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmFormat 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_ਸੰਸਕਰਣ 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmRevision 11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Include_ਸੰਸਕਰਣ ((XpmFormat * 100 + Xpm_ਸੰਸਕਰਣ) * 100 + XpmRevision)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XPM_NUMBERS

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEED_STRCASECMP		/* at least for MSVC++ */
#ਨਹੀਂਤਾਂ /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# ਨਹੀਂਤਾਂ /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#ਜੇ_ਅੰਤ /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#ਜੇ ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#ਜੇ_ਅੰਤ

/* make sure we know whether function prototypes are needed or not */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NeedFunctionPrototypes
# ਜੇ defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# ਨਹੀਂਤਾਂ
#  define NeedFunctionPrototypes 0
# endif
#ਜੇ_ਅੰਤ


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmColorError    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmSuccess       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmOpenFailed   -1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmFileInvalid  -2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_No_ਸਮ੍ਰਿਤੀ     -3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmColorFailed  -4

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *name;			/* Symbolic color name */
    ਅੱਖਰ *ਮੁੱਲ;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_ਚਿੰਨ੍ਹ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *name;			/* name of the extension */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nlines;	/* number of lines in this extension */
    ਅੱਖਰ **ਸਤਰਾਂ;		/* pointer to the extension array of strings */
}      XpmExtension;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ *string;		/* characters string */
    ਅੱਖਰ *symbolic;		/* symbolic name */
    ਅੱਖਰ *m_color;		/* monochrom default */
    ਅੱਖਰ *g4_color;		/* 4 level grayscale default */
    ਅੱਖਰ *g_color;		/* other level grayscale default */
    ਅੱਖਰ *c_color;		/* color default */
}      XpmColor;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ;		/* image width */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਚਾਈ;	/* image height */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpp;		/* number of characters per pixel */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ncolors;	/* number of colors */
    XpmColor *color_ਸਾਰਣੀ;	/* list of related colors */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ਅੰਕੜੇ;		/* image data */
}      XpmImage;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ valuemask;	/* Specifies which attributes are defined */
    ਅੱਖਰ *hints_cmt;		/* Comment of the hints section */
    ਅੱਖਰ *colors_cmt;		/* Comment of the colors section */
    ਅੱਖਰ *pixels_cmt;		/* Comment of the pixels section */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ x_hotspot;	/* Returns the x hotspot's coordinate */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ y_hotspot;	/* Returns the y hotspot's coordinate */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*XpmAllocColorFunc)(
#ਜੇ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    ਅੱਖਰ*			/* colorname */,
    XColor*			/* xcolor */,
    ਖਾਲੀ*			/*ਘੇਰਾ*/
#ਜੇ_ਅੰਤ
);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*XpmFreeColorsFunc)(
#ਜੇ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    ਪੂਰਨ				/* npixels */,
    ਖਾਲੀ*			/*ਘੇਰਾ*/
#ਜੇ_ਅੰਤ
);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਡੂੰਘਾਈ;			/* Specifies the depth */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ;			/* Returns the width of the created
					   pixmap */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਚਾਈ;		/* Returns the height of the created
					   pixmap */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ npixels;		/* Number of used pixels */
    Xpm_Color_ਚਿੰਨ੍ਹ *colorsymbols;	/* List of color symbols to override */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ numsymbols;		/* Number of symbols */
    ਅੱਖਰ *rgb_fname;			/* RGB text file name */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ncolors;               /* Number of colors */
    XpmColor *color_ਸਾਰਣੀ;               /* List of colors */
/* 3.2 backward compatibility code */
    ਅੱਖਰ *hints_cmt;                    /* Comment of the hints section */
    ਅੱਖਰ *colors_cmt;                   /* Comment of the colors section */
    ਅੱਖਰ *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ closeness;		/* Allowable RGB deviation */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ red_closeness;		/* Allowable red deviation */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ green_closeness;	/* Allowable green deviation */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ blue_closeness;	/* Allowable blue deviation */
    ਪੂਰਨ color_ਕੁੰਜੀ;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    ਪੂਰਨ nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    ਪੂਰਨ bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    ਖਾਲੀ *color_ਘੇਰਾ;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_ਗੁਣ;

/* XpmAttributes value masks bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmVisual	   (1L<<0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmColormap	   (1L<<1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_ਡੂੰਘਾਈ	   (1L<<2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_ਮਾਪ		   (1L<<3)	/* width & height */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Chars_ਪ੍ਰਤੀ_Pixel   (1L<<5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmColorSymbols	   (1L<<6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmInfos	   (1L<<8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReturnPixels	   (1L<<9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmExtensions      (1L<<10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReturnExtensions XpmExtensions

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmExactColors     (1L<<11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmCloseness	   (1L<<12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmRGBCloseness	   (1L<<13)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Color_ਕੁੰਜੀ	   (1L<<14)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Color_ਸਾਰਣੀ      (1L<<15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Return_Color_ਸਾਰਣੀ Xpm_Color_ਸਾਰਣੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReturnAllocPixels (1L<<16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmAllocCloseColors (1L<<17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmBitmapFormat    (1L<<18)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmAllocColor      (1L<<19)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmFreeColors      (1L<<20)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Color_ਘੇਰਾ    (1L<<21)


/* XpmInfo value masks bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmComments        XpmInfos
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FOR_MSW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmUndefPixel 0x80000000
#ਨਹੀਂਤਾਂ
/* int is only 16 bit for MSW */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmUndefPixel 0x8000
#ਜੇ_ਅੰਤ

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_MONO	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_GREY4	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_GRAY4	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_GREY 	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_ਸਲੇਟੀ 	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#ਜੇ NeedFunctionPrototypes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUNC(f, t, p) ਬਾਹਰੀ t f p
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LFUNC(f, t, p) ਸਥਿਰ t f p
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUNC(f, t, p) ਬਾਹਰੀ t f()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LFUNC(f, t, p) ਸਥਿਰ t f()
#ਜੇ_ਅੰਤ


/*
 * functions declarations
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#ਜੇ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_ਅੰਕੜੇ, ਪੂਰਨ, (Display *display,
					Drawable d,
					ਅੱਖਰ **ਅੰਕੜੇ,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Create_ਅੰਕੜੇ_From_Pixmap, ਪੂਰਨ, (Display *display,
					ਅੱਖਰ ***ਅੰਕੜੇ_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Read_File_ਮੰਜ਼ਿਲ_Pixmap, ਪੂਰਨ, (Display *display,
				    Drawable d,
				    ਅੱਖਰ *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(XpmWriteFileFromPixmap, ਪੂਰਨ, (Display *display,
				       ਅੱਖਰ *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ_ਅੰਤ

    FUNC(Xpm_Create_Image_From_ਅੰਕੜੇ, ਪੂਰਨ, (Display *display,
				       ਅੱਖਰ **ਅੰਕੜੇ,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Create_ਅੰਕੜੇ_From_Image, ਪੂਰਨ, (Display *display,
				       ਅੱਖਰ ***ਅੰਕੜੇ_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Read_File_ਮੰਜ਼ਿਲ_Image, ਪੂਰਨ, (Display *display,
				   ਅੱਖਰ *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(XpmWriteFileFromImage, ਪੂਰਨ, (Display *display,
				      ਅੱਖਰ *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Create_Image_From_ਬਫਰ, ਪੂਰਨ, (Display *display,
					 ਅੱਖਰ *ਬਫਰ,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_ਬਫਰ, ਪੂਰਨ, (Display *display,
					  Drawable d,
					  ਅੱਖਰ *ਬਫਰ,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Create_ਬਫਰ_From_Image, ਪੂਰਨ, (Display *display,
					 ਅੱਖਰ **ਬਫਰ_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(Xpm_Create_ਬਫਰ_From_Pixmap, ਪੂਰਨ, (Display *display,
					  ਅੱਖਰ **ਬਫਰ_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ_ਅੰਤ
    FUNC(Xpm_Read_File_ਮੰਜ਼ਿਲ_ਬਫਰ, ਪੂਰਨ, (ਅੱਖਰ *filename, ਅੱਖਰ **ਬਫਰ_return));
    FUNC(Xpm_Write_File_From_ਬਫਰ, ਪੂਰਨ, (ਅੱਖਰ *filename, ਅੱਖਰ *ਬਫਰ));

    FUNC(Xpm_Read_File_ਮੰਜ਼ਿਲ_ਅੰਕੜੇ, ਪੂਰਨ, (ਅੱਖਰ *filename, ਅੱਖਰ ***ਅੰਕੜੇ_return));
    FUNC(Xpm_Write_File_From_ਅੰਕੜੇ, ਪੂਰਨ, (ਅੱਖਰ *filename, ਅੱਖਰ **ਅੰਕੜੇ));

    FUNC(Xpm_ਗੁਣ_ਮਾਪ, ਪੂਰਨ, ());
    FUNC(Xpm_Free_ਗੁਣ, ਖਾਲੀ, (Xpm_ਗੁਣ *ਗੁਣ));
    FUNC(XpmFreeExtensions, ਖਾਲੀ, (XpmExtension *extensions,
				   ਪੂਰਨ nextensions));

    FUNC(XpmFreeXpmImage, ਖਾਲੀ, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, ਖਾਲੀ, (XpmInfo *info));
    FUNC(XpmGetErrorString, ਅੱਖਰ *, (ਪੂਰਨ errcode));
    FUNC(Xpm_Library_ਸੰਸਕਰਣ, ਪੂਰਨ, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_ਮੰਜ਼ਿਲ_Xpm_Image, ਪੂਰਨ, (ਅੱਖਰ *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, ਪੂਰਨ, (ਅੱਖਰ *filename,
					 XpmImage *image,
					 XpmInfo *info));
#ਜੇ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, ਪੂਰਨ, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ_ਅੰਤ
    FUNC(XpmCreateImageFromXpmImage, ਪੂਰਨ, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_ਗੁਣ *ਗੁਣ));

    FUNC(XpmCreateXpmImageFromImage, ਪੂਰਨ, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, ਪੂਰਨ, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_ਗੁਣ *ਗੁਣ));
#ਜੇ_ਅੰਤ
    FUNC(Xpm_Create_ਅੰਕੜੇ_From_Xpm_Image, ਪੂਰਨ, (ਅੱਖਰ ***ਅੰਕੜੇ_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_ਅੰਕੜੇ, ਪੂਰਨ, (ਅੱਖਰ **ਅੰਕੜੇ,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_ਬਫਰ, ਪੂਰਨ, (ਅੱਖਰ *ਬਫਰ,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_ਬਫਰ_From_Xpm_Image, ਪੂਰਨ, (ਅੱਖਰ **ਬਫਰ_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, ਪੂਰਨ, (ਅੱਖਰ *filename,
				 ਪੂਰਨ *linenum_return,
				 ਪੂਰਨ *charnum_return));

    FUNC(XpmFree, ਖਾਲੀ, (ਖਾਲੀ *ptr));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
} /* for C++ V2.0 */
#ਜੇ_ਅੰਤ


/* backward compatibility */

/* for version 3.0c */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmPixmapColorError  XpmColorError
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmPixmapSuccess     XpmSuccess
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmPixmapOpenFailed  XpmOpenFailed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmPixmapFileInvalid XpmFileInvalid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xpm_Pixmap_No_ਸਮ੍ਰਿਤੀ    Xpm_No_ਸਮ੍ਰਿਤੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmPixmapColorFailed XpmColorFailed

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmReadPixmapFile(dpy, d, file, pix, ਨਕਾਬ, att) \
    Xpm_Read_File_ਮੰਜ਼ਿਲ_Pixmap(dpy, d, file, pix, ਨਕਾਬ, att)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XpmWritePixmapFile(dpy, file, pix, ਨਕਾਬ, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ਨਕਾਬ, att)

/* for version 3.0b */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PixmapColorError  XpmColorError
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PixmapSuccess     XpmSuccess
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PixmapOpenFailed  XpmOpenFailed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PixmapFileInvalid XpmFileInvalid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Pixmap_No_ਸਮ੍ਰਿਤੀ    Xpm_No_ਸਮ੍ਰਿਤੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PixmapColorFailed XpmColorFailed

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Color_ਚਿੰਨ੍ਹ Xpm_Color_ਚਿੰਨ੍ਹ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XReadPixmapFile(dpy, d, file, pix, ਨਕਾਬ, att) \
    Xpm_Read_File_ਮੰਜ਼ਿਲ_Pixmap(dpy, d, file, pix, ਨਕਾਬ, att)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XWritePixmapFile(dpy, file, pix, ਨਕਾਬ, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ਨਕਾਬ, att)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_Pixmap_From_ਅੰਕੜੇ(dpy, d, ਅੰਕੜੇ, pix, ਨਕਾਬ, att) \
    Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dpy, d, ਅੰਕੜੇ, pix, ਨਕਾਬ, att)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XCreate_ਅੰਕੜੇ_From_Pixmap(dpy, ਅੰਕੜੇ, pix, ਨਕਾਬ, att) \
    Xpm_Create_ਅੰਕੜੇ_From_Pixmap(dpy, ਅੰਕੜੇ, pix, ਨਕਾਬ, att)

#ਜੇ_ਅੰਤ /* XPM_NUMBERS */
#ਜੇ_ਅੰਤ
