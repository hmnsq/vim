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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XPM_h
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_h

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
#ವ್ಯಾಖ್ಯಾನಿಸು XpmFormat 3
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_ಆವೃತ್ತಿ 4
#ವ್ಯಾಖ್ಯಾನಿಸು XpmRevision 11
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Include_ಆವೃತ್ತಿ ((XpmFormat * 100 + Xpm_ಆವೃತ್ತಿ) * 100 + XpmRevision)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XPM_NUMBERS

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#ವ್ಯಾಖ್ಯಾನಿಸು NEED_STRCASECMP		/* at least for MSVC++ */
#ಇಲ್ಲದಿದ್ದರೆ /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# ಇಲ್ಲದಿದ್ದರೆ /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#ಆದರೆ_ಅಂತ್ಯ /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#ಆದರೆ ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#ಆದರೆ_ಅಂತ್ಯ

/* make sure we know whether function prototypes are needed or not */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NeedFunctionPrototypes
# ಆದರೆ defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# ಇಲ್ಲದಿದ್ದರೆ
#  define NeedFunctionPrototypes 0
# endif
#ಆದರೆ_ಅಂತ್ಯ


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#ವ್ಯಾಖ್ಯಾನಿಸು XpmColorError    1
#ವ್ಯಾಖ್ಯಾನಿಸು XpmSuccess       0
#ವ್ಯಾಖ್ಯಾನಿಸು XpmOpenFailed   -1
#ವ್ಯಾಖ್ಯಾನಿಸು XpmFileInvalid  -2
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_No_ಸ್ಮರಣೆ     -3
#ವ್ಯಾಖ್ಯಾನಿಸು XpmColorFailed  -4

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *name;			/* Symbolic color name */
    ಅಕ್ಷರ *ಮೌಲ್ಯ;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_ಚಿಹ್ನೆ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *name;			/* name of the extension */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nlines;	/* number of lines in this extension */
    ಅಕ್ಷರ **ಸಾಲುಗಳು;		/* pointer to the extension array of strings */
}      XpmExtension;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ *string;		/* characters string */
    ಅಕ್ಷರ *symbolic;		/* symbolic name */
    ಅಕ್ಷರ *m_color;		/* monochrom default */
    ಅಕ್ಷರ *g4_color;		/* 4 level grayscale default */
    ಅಕ್ಷರ *g_color;		/* other level grayscale default */
    ಅಕ್ಷರ *c_color;		/* color default */
}      XpmColor;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ;		/* image width */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎತ್ತರ;	/* image height */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpp;		/* number of characters per pixel */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ncolors;	/* number of colors */
    XpmColor *color_ಕೋಷ್ಟಕ;	/* list of related colors */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ದತ್ತಾಂಶ;		/* image data */
}      XpmImage;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ valuemask;	/* Specifies which attributes are defined */
    ಅಕ್ಷರ *hints_cmt;		/* Comment of the hints section */
    ಅಕ್ಷರ *colors_cmt;		/* Comment of the colors section */
    ಅಕ್ಷರ *pixels_cmt;		/* Comment of the pixels section */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ x_hotspot;	/* Returns the x hotspot's coordinate */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ y_hotspot;	/* Returns the y hotspot's coordinate */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*XpmAllocColorFunc)(
#ಆದರೆ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    ಅಕ್ಷರ*			/* colorname */,
    XColor*			/* xcolor */,
    ಶೂನ್ಯ*			/*ಆವರಣ*/
#ಆದರೆ_ಅಂತ್ಯ
);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*XpmFreeColorsFunc)(
#ಆದರೆ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    ಪೂರ್ಣಾಂಕ				/* npixels */,
    ಶೂನ್ಯ*			/*ಆವರಣ*/
#ಆದರೆ_ಅಂತ್ಯ
);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆಳ;			/* Specifies the depth */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ;			/* Returns the width of the created
					   pixmap */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಎತ್ತರ;		/* Returns the height of the created
					   pixmap */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ npixels;		/* Number of used pixels */
    Xpm_Color_ಚಿಹ್ನೆ *colorsymbols;	/* List of color symbols to override */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ numsymbols;		/* Number of symbols */
    ಅಕ್ಷರ *rgb_fname;			/* RGB text file name */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ncolors;               /* Number of colors */
    XpmColor *color_ಕೋಷ್ಟಕ;               /* List of colors */
/* 3.2 backward compatibility code */
    ಅಕ್ಷರ *hints_cmt;                    /* Comment of the hints section */
    ಅಕ್ಷರ *colors_cmt;                   /* Comment of the colors section */
    ಅಕ್ಷರ *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ closeness;		/* Allowable RGB deviation */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ red_closeness;		/* Allowable red deviation */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ green_closeness;	/* Allowable green deviation */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ blue_closeness;	/* Allowable blue deviation */
    ಪೂರ್ಣಾಂಕ color_ಕೀಲಿ;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    ಪೂರ್ಣಾಂಕ nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    ಪೂರ್ಣಾಂಕ bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    ಶೂನ್ಯ *color_ಆವರಣ;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_ಗುಣಲಕ್ಷಣಗಳು;

/* XpmAttributes value masks bits */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmVisual	   (1L<<0)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmColormap	   (1L<<1)
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_ಆಳ	   (1L<<2)
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_ಗಾತ್ರ		   (1L<<3)	/* width & height */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Chars_ಪ್ರತಿ_Pixel   (1L<<5)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmColorSymbols	   (1L<<6)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmInfos	   (1L<<8)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmReturnPixels	   (1L<<9)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmExtensions      (1L<<10)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmReturnExtensions XpmExtensions

#ವ್ಯಾಖ್ಯಾನಿಸು XpmExactColors     (1L<<11)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmCloseness	   (1L<<12)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmRGBCloseness	   (1L<<13)
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Color_ಕೀಲಿ	   (1L<<14)

#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Color_ಕೋಷ್ಟಕ      (1L<<15)
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Return_Color_ಕೋಷ್ಟಕ Xpm_Color_ಕೋಷ್ಟಕ

#ವ್ಯಾಖ್ಯಾನಿಸು XpmReturnAllocPixels (1L<<16)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmAllocCloseColors (1L<<17)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmBitmapFormat    (1L<<18)

#ವ್ಯಾಖ್ಯಾನಿಸು XpmAllocColor      (1L<<19)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmFreeColors      (1L<<20)
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Color_ಆವರಣ    (1L<<21)


/* XpmInfo value masks bits */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmComments        XpmInfos
#ವ್ಯಾಖ್ಯಾನಿಸು XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FOR_MSW
#ವ್ಯಾಖ್ಯಾನಿಸು XpmUndefPixel 0x80000000
#ಇಲ್ಲದಿದ್ದರೆ
/* int is only 16 bit for MSW */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmUndefPixel 0x8000
#ಆದರೆ_ಅಂತ್ಯ

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_MONO	2
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_GREY4	3
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_GRAY4	3
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_GREY 	4
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_ಬೂದು 	4
#ವ್ಯಾಖ್ಯಾನಿಸು XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#ಆದರೆ NeedFunctionPrototypes
#ವ್ಯಾಖ್ಯಾನಿಸು FUNC(f, t, p) ಬಾಹ್ಯ t f p
#ವ್ಯಾಖ್ಯಾನಿಸು LFUNC(f, t, p) ಸ್ಥಿರ t f p
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು FUNC(f, t, p) ಬಾಹ್ಯ t f()
#ವ್ಯಾಖ್ಯಾನಿಸು LFUNC(f, t, p) ಸ್ಥಿರ t f()
#ಆದರೆ_ಅಂತ್ಯ


/*
 * functions declarations
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#ಆದರೆ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ, (Display *display,
					Drawable d,
					ಅಕ್ಷರ **ದತ್ತಾಂಶ,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Create_ದತ್ತಾಂಶ_From_Pixmap, ಪೂರ್ಣಾಂಕ, (Display *display,
					ಅಕ್ಷರ ***ದತ್ತಾಂಶ_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Read_File_ಗಮ್ಯ_Pixmap, ಪೂರ್ಣಾಂಕ, (Display *display,
				    Drawable d,
				    ಅಕ್ಷರ *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(XpmWriteFileFromPixmap, ಪೂರ್ಣಾಂಕ, (Display *display,
				       ಅಕ್ಷರ *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ_ಅಂತ್ಯ

    FUNC(Xpm_Create_Image_From_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ, (Display *display,
				       ಅಕ್ಷರ **ದತ್ತಾಂಶ,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Create_ದತ್ತಾಂಶ_From_Image, ಪೂರ್ಣಾಂಕ, (Display *display,
				       ಅಕ್ಷರ ***ದತ್ತಾಂಶ_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Read_File_ಗಮ್ಯ_Image, ಪೂರ್ಣಾಂಕ, (Display *display,
				   ಅಕ್ಷರ *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(XpmWriteFileFromImage, ಪೂರ್ಣಾಂಕ, (Display *display,
				      ಅಕ್ಷರ *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Create_Image_From_ಬಫರ್, ಪೂರ್ಣಾಂಕ, (Display *display,
					 ಅಕ್ಷರ *ಬಫರ್,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_ಬಫರ್, ಪೂರ್ಣಾಂಕ, (Display *display,
					  Drawable d,
					  ಅಕ್ಷರ *ಬಫರ್,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Create_ಬಫರ್_From_Image, ಪೂರ್ಣಾಂಕ, (Display *display,
					 ಅಕ್ಷರ **ಬಫರ್_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(Xpm_Create_ಬಫರ್_From_Pixmap, ಪೂರ್ಣಾಂಕ, (Display *display,
					  ಅಕ್ಷರ **ಬಫರ್_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ_ಅಂತ್ಯ
    FUNC(Xpm_Read_File_ಗಮ್ಯ_ಬಫರ್, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ಬಫರ್_return));
    FUNC(Xpm_Write_File_From_ಬಫರ್, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename, ಅಕ್ಷರ *ಬಫರ್));

    FUNC(Xpm_Read_File_ಗಮ್ಯ_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename, ಅಕ್ಷರ ***ದತ್ತಾಂಶ_return));
    FUNC(Xpm_Write_File_From_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ದತ್ತಾಂಶ));

    FUNC(Xpm_ಗುಣಲಕ್ಷಣಗಳು_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ, ());
    FUNC(Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು, ಶೂನ್ಯ, (Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
    FUNC(XpmFreeExtensions, ಶೂನ್ಯ, (XpmExtension *extensions,
				   ಪೂರ್ಣಾಂಕ nextensions));

    FUNC(XpmFreeXpmImage, ಶೂನ್ಯ, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, ಶೂನ್ಯ, (XpmInfo *info));
    FUNC(XpmGetErrorString, ಅಕ್ಷರ *, (ಪೂರ್ಣಾಂಕ errcode));
    FUNC(Xpm_Library_ಆವೃತ್ತಿ, ಪೂರ್ಣಾಂಕ, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_ಗಮ್ಯ_Xpm_Image, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename,
					 XpmImage *image,
					 XpmInfo *info));
#ಆದರೆ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, ಪೂರ್ಣಾಂಕ, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ_ಅಂತ್ಯ
    FUNC(XpmCreateImageFromXpmImage, ಪೂರ್ಣಾಂಕ, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));

    FUNC(XpmCreateXpmImageFromImage, ಪೂರ್ಣಾಂಕ, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, ಪೂರ್ಣಾಂಕ, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_ಗುಣಲಕ್ಷಣಗಳು *ಗುಣಲಕ್ಷಣಗಳು));
#ಆದರೆ_ಅಂತ್ಯ
    FUNC(Xpm_Create_ದತ್ತಾಂಶ_From_Xpm_Image, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ ***ದತ್ತಾಂಶ_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ **ದತ್ತಾಂಶ,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_ಬಫರ್, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *ಬಫರ್,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_ಬಫರ್_From_Xpm_Image, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ **ಬಫರ್_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, ಪೂರ್ಣಾಂಕ, (ಅಕ್ಷರ *filename,
				 ಪೂರ್ಣಾಂಕ *linenum_return,
				 ಪೂರ್ಣಾಂಕ *charnum_return));

    FUNC(XpmFree, ಶೂನ್ಯ, (ಶೂನ್ಯ *ptr));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
} /* for C++ V2.0 */
#ಆದರೆ_ಅಂತ್ಯ


/* backward compatibility */

/* for version 3.0c */
#ವ್ಯಾಖ್ಯಾನಿಸು XpmPixmapColorError  XpmColorError
#ವ್ಯಾಖ್ಯಾನಿಸು XpmPixmapSuccess     XpmSuccess
#ವ್ಯಾಖ್ಯಾನಿಸು XpmPixmapOpenFailed  XpmOpenFailed
#ವ್ಯಾಖ್ಯಾನಿಸು XpmPixmapFileInvalid XpmFileInvalid
#ವ್ಯಾಖ್ಯಾನಿಸು Xpm_Pixmap_No_ಸ್ಮರಣೆ    Xpm_No_ಸ್ಮರಣೆ
#ವ್ಯಾಖ್ಯಾನಿಸು XpmPixmapColorFailed XpmColorFailed

#ವ್ಯಾಖ್ಯಾನಿಸು XpmReadPixmapFile(dpy, d, file, pix, ಮುಖವಾಡ, att) \
    Xpm_Read_File_ಗಮ್ಯ_Pixmap(dpy, d, file, pix, ಮುಖವಾಡ, att)
#ವ್ಯಾಖ್ಯಾನಿಸು XpmWritePixmapFile(dpy, file, pix, ಮುಖವಾಡ, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ಮುಖವಾಡ, att)

/* for version 3.0b */
#ವ್ಯಾಖ್ಯಾನಿಸು PixmapColorError  XpmColorError
#ವ್ಯಾಖ್ಯಾನಿಸು PixmapSuccess     XpmSuccess
#ವ್ಯಾಖ್ಯಾನಿಸು PixmapOpenFailed  XpmOpenFailed
#ವ್ಯಾಖ್ಯಾನಿಸು PixmapFileInvalid XpmFileInvalid
#ವ್ಯಾಖ್ಯಾನಿಸು Pixmap_No_ಸ್ಮರಣೆ    Xpm_No_ಸ್ಮರಣೆ
#ವ್ಯಾಖ್ಯಾನಿಸು PixmapColorFailed XpmColorFailed

#ವ್ಯಾಖ್ಯಾನಿಸು Color_ಚಿಹ್ನೆ Xpm_Color_ಚಿಹ್ನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು XReadPixmapFile(dpy, d, file, pix, ಮುಖವಾಡ, att) \
    Xpm_Read_File_ಗಮ್ಯ_Pixmap(dpy, d, file, pix, ಮುಖವಾಡ, att)
#ವ್ಯಾಖ್ಯಾನಿಸು XWritePixmapFile(dpy, file, pix, ಮುಖವಾಡ, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ಮುಖವಾಡ, att)
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_Pixmap_From_ದತ್ತಾಂಶ(dpy, d, ದತ್ತಾಂಶ, pix, ಮುಖವಾಡ, att) \
    Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dpy, d, ದತ್ತಾಂಶ, pix, ಮುಖವಾಡ, att)
#ವ್ಯಾಖ್ಯಾನಿಸು XCreate_ದತ್ತಾಂಶ_From_Pixmap(dpy, ದತ್ತಾಂಶ, pix, ಮುಖವಾಡ, att) \
    Xpm_Create_ದತ್ತಾಂಶ_From_Pixmap(dpy, ದತ್ತಾಂಶ, pix, ಮುಖವಾಡ, att)

#ಆದರೆ_ಅಂತ್ಯ /* XPM_NUMBERS */
#ಆದರೆ_ಅಂತ್ಯ
