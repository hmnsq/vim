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

#اگر_تعریف_نشده XPM_h
#تعریف_کن XPM_h

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
#تعریف_کن XpmFormat 3
#تعریف_کن Xpm_نسخه 4
#تعریف_کن XpmRevision 11
#تعریف_کن Xpm_Include_نسخه ((XpmFormat * 100 + Xpm_نسخه) * 100 + XpmRevision)

#اگر_تعریف_نشده XPM_NUMBERS

#اگر_تعریف_شده FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#تعریف_کن NEED_STRCASECMP		/* at least for MSVC++ */
#وإلا /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# وإلا /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#پایان_اگر /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#إذا ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
تعريفنوع بیعلامت دراز Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#پایان_اگر

/* make sure we know whether function prototypes are needed or not */
#اگر_تعریف_نشده NeedFunctionPrototypes
# إذا defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# وإلا
#  define NeedFunctionPrototypes 0
# endif
#پایان_اگر


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#تعریف_کن XpmColorError    1
#تعریف_کن XpmSuccess       0
#تعریف_کن XpmOpenFailed   -1
#تعریف_کن XpmFileInvalid  -2
#تعریف_کن Xpm_No_حافظه     -3
#تعریف_کن XpmColorFailed  -4

تعريفنوع بنية {
    حرف *name;			/* Symbolic color name */
    حرف *مقدار;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_نماد;

تعريفنوع بنية {
    حرف *name;			/* name of the extension */
    بیعلامت صحيح nlines;	/* number of lines in this extension */
    حرف **سطرها;		/* pointer to the extension array of strings */
}      XpmExtension;

تعريفنوع بنية {
    حرف *string;		/* characters string */
    حرف *symbolic;		/* symbolic name */
    حرف *m_color;		/* monochrom default */
    حرف *g4_color;		/* 4 level grayscale default */
    حرف *g_color;		/* other level grayscale default */
    حرف *c_color;		/* color default */
}      XpmColor;

تعريفنوع بنية {
    بیعلامت صحيح عرض;		/* image width */
    بیعلامت صحيح ارتفاع;	/* image height */
    بیعلامت صحيح cpp;		/* number of characters per pixel */
    بیعلامت صحيح ncolors;	/* number of colors */
    XpmColor *color_جدول;	/* list of related colors */
    بیعلامت صحيح *داده;		/* image data */
}      XpmImage;

تعريفنوع بنية {
    بیعلامت دراز valuemask;	/* Specifies which attributes are defined */
    حرف *hints_cmt;		/* Comment of the hints section */
    حرف *colors_cmt;		/* Comment of the colors section */
    حرف *pixels_cmt;		/* Comment of the pixels section */
    بیعلامت صحيح x_hotspot;	/* Returns the x hotspot's coordinate */
    بیعلامت صحيح y_hotspot;	/* Returns the y hotspot's coordinate */
    بیعلامت صحيح nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

تعريفنوع صحيح (*XpmAllocColorFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    حرف*			/* colorname */,
    XColor*			/* xcolor */,
    فراغ*			/*بستگی*/
#پایان_اگر
);

تعريفنوع صحيح (*XpmFreeColorsFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    صحيح				/* npixels */,
    فراغ*			/*بستگی*/
#پایان_اگر
);

تعريفنوع بنية {
    بیعلامت دراز valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    بیعلامت صحيح عمق;			/* Specifies the depth */
    بیعلامت صحيح عرض;			/* Returns the width of the created
					   pixmap */
    بیعلامت صحيح ارتفاع;		/* Returns the height of the created
					   pixmap */
    بیعلامت صحيح x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    بیعلامت صحيح y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    بیعلامت صحيح cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    بیعلامت صحيح npixels;		/* Number of used pixels */
    Xpm_Color_نماد *colorsymbols;	/* List of color symbols to override */
    بیعلامت صحيح numsymbols;		/* Number of symbols */
    حرف *rgb_fname;			/* RGB text file name */
    بیعلامت صحيح nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    بیعلامت صحيح ncolors;               /* Number of colors */
    XpmColor *color_جدول;               /* List of colors */
/* 3.2 backward compatibility code */
    حرف *hints_cmt;                    /* Comment of the hints section */
    حرف *colors_cmt;                   /* Comment of the colors section */
    حرف *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    بیعلامت صحيح ماسک_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    بیعلامت صحيح closeness;		/* Allowable RGB deviation */
    بیعلامت صحيح red_closeness;		/* Allowable red deviation */
    بیعلامت صحيح green_closeness;	/* Allowable green deviation */
    بیعلامت صحيح blue_closeness;	/* Allowable blue deviation */
    صحيح color_کلید;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    صحيح nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    صحيح bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    فراغ *color_بستگی;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_صفات;

/* XpmAttributes value masks bits */
#تعریف_کن XpmVisual	   (1L<<0)
#تعریف_کن XpmColormap	   (1L<<1)
#تعریف_کن Xpm_عمق	   (1L<<2)
#تعریف_کن Xpm_اندازه		   (1L<<3)	/* width & height */
#تعریف_کن XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#تعریف_کن Xpm_Chars_در_هر_Pixel   (1L<<5)
#تعریف_کن XpmColorSymbols	   (1L<<6)
#تعریف_کن XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#تعریف_کن XpmInfos	   (1L<<8)
#تعریف_کن XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#تعریف_کن XpmReturnPixels	   (1L<<9)
#تعریف_کن XpmExtensions      (1L<<10)
#تعریف_کن XpmReturnExtensions XpmExtensions

#تعریف_کن XpmExactColors     (1L<<11)
#تعریف_کن XpmCloseness	   (1L<<12)
#تعریف_کن XpmRGBCloseness	   (1L<<13)
#تعریف_کن Xpm_Color_کلید	   (1L<<14)

#تعریف_کن Xpm_Color_جدول      (1L<<15)
#تعریف_کن Xpm_Return_Color_جدول Xpm_Color_جدول

#تعریف_کن XpmReturnAllocPixels (1L<<16)
#تعریف_کن XpmAllocCloseColors (1L<<17)
#تعریف_کن XpmBitmapFormat    (1L<<18)

#تعریف_کن XpmAllocColor      (1L<<19)
#تعریف_کن XpmFreeColors      (1L<<20)
#تعریف_کن Xpm_Color_بستگی    (1L<<21)


/* XpmInfo value masks bits */
#تعریف_کن XpmComments        XpmInfos
#تعریف_کن XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#اگر_تعریف_نشده FOR_MSW
#تعریف_کن XpmUndefPixel 0x80000000
#وإلا
/* int is only 16 bit for MSW */
#تعریف_کن XpmUndefPixel 0x8000
#پایان_اگر

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#تعریف_کن XPM_MONO	2
#تعریف_کن XPM_GREY4	3
#تعریف_کن XPM_GRAY4	3
#تعریف_کن XPM_GREY 	4
#تعریف_کن XPM_خاکستری 	4
#تعریف_کن XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#إذا NeedFunctionPrototypes
#تعریف_کن FUNC(f, t, p) خارجي t f p
#تعریف_کن LFUNC(f, t, p) ساكن t f p
#وإلا
#تعریف_کن FUNC(f, t, p) خارجي t f()
#تعریف_کن LFUNC(f, t, p) ساكن t f()
#پایان_اگر


/*
 * functions declarations
 */

#اگر_تعریف_شده __cplusplus
خارجي "C" {
#پایان_اگر

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_داده, صحيح, (Display *display,
					Drawable d,
					حرف **داده,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_صفات *صفات));

    FUNC(Xpm_Create_داده_From_Pixmap, صحيح, (Display *display,
					حرف ***داده_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_صفات *صفات));

    FUNC(Xpm_Read_File_مقصد_Pixmap, صحيح, (Display *display,
				    Drawable d,
				    حرف *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_صفات *صفات));

    FUNC(XpmWriteFileFromPixmap, صحيح, (Display *display,
				       حرف *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_صفات *صفات));
#پایان_اگر

    FUNC(Xpm_Create_Image_From_داده, صحيح, (Display *display,
				       حرف **داده,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_صفات *صفات));

    FUNC(Xpm_Create_داده_From_Image, صحيح, (Display *display,
				       حرف ***داده_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_صفات *صفات));

    FUNC(Xpm_Read_File_مقصد_Image, صحيح, (Display *display,
				   حرف *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_صفات *صفات));

    FUNC(XpmWriteFileFromImage, صحيح, (Display *display,
				      حرف *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_صفات *صفات));

    FUNC(Xpm_Create_Image_From_میانگیر, صحيح, (Display *display,
					 حرف *میانگیر,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_صفات *صفات));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_میانگیر, صحيح, (Display *display,
					  Drawable d,
					  حرف *میانگیر,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_صفات *صفات));

    FUNC(Xpm_Create_میانگیر_From_Image, صحيح, (Display *display,
					 حرف **میانگیر_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_صفات *صفات));

    FUNC(Xpm_Create_میانگیر_From_Pixmap, صحيح, (Display *display,
					  حرف **میانگیر_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_صفات *صفات));
#پایان_اگر
    FUNC(Xpm_Read_File_مقصد_میانگیر, صحيح, (حرف *filename, حرف **میانگیر_return));
    FUNC(Xpm_Write_File_From_میانگیر, صحيح, (حرف *filename, حرف *میانگیر));

    FUNC(Xpm_Read_File_مقصد_داده, صحيح, (حرف *filename, حرف ***داده_return));
    FUNC(Xpm_Write_File_From_داده, صحيح, (حرف *filename, حرف **داده));

    FUNC(Xpm_صفات_اندازه, صحيح, ());
    FUNC(Xpm_Free_صفات, فراغ, (Xpm_صفات *صفات));
    FUNC(XpmFreeExtensions, فراغ, (XpmExtension *extensions,
				   صحيح nextensions));

    FUNC(XpmFreeXpmImage, فراغ, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, فراغ, (XpmInfo *info));
    FUNC(XpmGetErrorString, حرف *, (صحيح errcode));
    FUNC(Xpm_Library_نسخه, صحيح, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_مقصد_Xpm_Image, صحيح, (حرف *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, صحيح, (حرف *filename,
					 XpmImage *image,
					 XpmInfo *info));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, صحيح, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    Xpm_صفات *صفات));
#پایان_اگر
    FUNC(XpmCreateImageFromXpmImage, صحيح, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_صفات *صفات));

    FUNC(XpmCreateXpmImageFromImage, صحيح, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_صفات *صفات));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, صحيح, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_صفات *صفات));
#پایان_اگر
    FUNC(Xpm_Create_داده_From_Xpm_Image, صحيح, (حرف ***داده_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_داده, صحيح, (حرف **داده,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_میانگیر, صحيح, (حرف *میانگیر,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_میانگیر_From_Xpm_Image, صحيح, (حرف **میانگیر_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, صحيح, (حرف *filename,
				 صحيح *linenum_return,
				 صحيح *charnum_return));

    FUNC(XpmFree, فراغ, (فراغ *ptr));

#اگر_تعریف_شده __cplusplus
} /* for C++ V2.0 */
#پایان_اگر


/* backward compatibility */

/* for version 3.0c */
#تعریف_کن XpmPixmapColorError  XpmColorError
#تعریف_کن XpmPixmapSuccess     XpmSuccess
#تعریف_کن XpmPixmapOpenFailed  XpmOpenFailed
#تعریف_کن XpmPixmapFileInvalid XpmFileInvalid
#تعریف_کن Xpm_Pixmap_No_حافظه    Xpm_No_حافظه
#تعریف_کن XpmPixmapColorFailed XpmColorFailed

#تعریف_کن XpmReadPixmapFile(dpy, d, file, pix, ماسک, att) \
    Xpm_Read_File_مقصد_Pixmap(dpy, d, file, pix, ماسک, att)
#تعریف_کن XpmWritePixmapFile(dpy, file, pix, ماسک, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ماسک, att)

/* for version 3.0b */
#تعریف_کن PixmapColorError  XpmColorError
#تعریف_کن PixmapSuccess     XpmSuccess
#تعریف_کن PixmapOpenFailed  XpmOpenFailed
#تعریف_کن PixmapFileInvalid XpmFileInvalid
#تعریف_کن Pixmap_No_حافظه    Xpm_No_حافظه
#تعریف_کن PixmapColorFailed XpmColorFailed

#تعریف_کن Color_نماد Xpm_Color_نماد

#تعریف_کن XReadPixmapFile(dpy, d, file, pix, ماسک, att) \
    Xpm_Read_File_مقصد_Pixmap(dpy, d, file, pix, ماسک, att)
#تعریف_کن XWritePixmapFile(dpy, file, pix, ماسک, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ماسک, att)
#تعریف_کن XCreate_Pixmap_From_داده(dpy, d, داده, pix, ماسک, att) \
    Xpm_Create_Pixmap_From_داده(dpy, d, داده, pix, ماسک, att)
#تعریف_کن XCreate_داده_From_Pixmap(dpy, داده, pix, ماسک, att) \
    Xpm_Create_داده_From_Pixmap(dpy, داده, pix, ماسک, att)

#پایان_اگر /* XPM_NUMBERS */
#پایان_اگر
