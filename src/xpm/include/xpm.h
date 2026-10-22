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

#جيڪڏهن_وصف_نه_ٿيل XPM_h
#وصف_ڪريو XPM_h

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
#وصف_ڪريو XpmFormat 3
#وصف_ڪريو Xpm_نسخو 4
#وصف_ڪريو XpmRevision 11
#وصف_ڪريو Xpm_Include_نسخو ((XpmFormat * 100 + Xpm_نسخو) * 100 + XpmRevision)

#جيڪڏهن_وصف_نه_ٿيل XPM_NUMBERS

#جيڪڏهن_وصف_ٿيل FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#وصف_ڪريو NEED_STRCASECMP		/* at least for MSVC++ */
#وإلا /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# وإلا /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#جيڪڏهن_پورو /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#إذا ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
تعريفنوع بنانشان ڊگهو Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#جيڪڏهن_پورو

/* make sure we know whether function prototypes are needed or not */
#جيڪڏهن_وصف_نه_ٿيل NeedFunctionPrototypes
# إذا defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# وإلا
#  define NeedFunctionPrototypes 0
# endif
#جيڪڏهن_پورو


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#وصف_ڪريو XpmColorError    1
#وصف_ڪريو XpmSuccess       0
#وصف_ڪريو XpmOpenFailed   -1
#وصف_ڪريو XpmFileInvalid  -2
#وصف_ڪريو Xpm_No_يادگيري     -3
#وصف_ڪريو XpmColorFailed  -4

تعريفنوع بنية {
    حرف *name;			/* Symbolic color name */
    حرف *قدر;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_نشان;

تعريفنوع بنية {
    حرف *name;			/* name of the extension */
    بنانشان صحيح nlines;	/* number of lines in this extension */
    حرف **سٽون;		/* pointer to the extension array of strings */
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
    بنانشان صحيح ويڪر;		/* image width */
    بنانشان صحيح اوچائي;	/* image height */
    بنانشان صحيح cpp;		/* number of characters per pixel */
    بنانشان صحيح ncolors;	/* number of colors */
    XpmColor *color_جدول;	/* list of related colors */
    بنانشان صحيح *معلومات;		/* image data */
}      XpmImage;

تعريفنوع بنية {
    بنانشان ڊگهو valuemask;	/* Specifies which attributes are defined */
    حرف *hints_cmt;		/* Comment of the hints section */
    حرف *colors_cmt;		/* Comment of the colors section */
    حرف *pixels_cmt;		/* Comment of the pixels section */
    بنانشان صحيح x_hotspot;	/* Returns the x hotspot's coordinate */
    بنانشان صحيح y_hotspot;	/* Returns the y hotspot's coordinate */
    بنانشان صحيح nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

تعريفنوع صحيح (*XpmAllocColorFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    حرف*			/* colorname */,
    XColor*			/* xcolor */,
    فراغ*			/*گهيرو*/
#جيڪڏهن_پورو
);

تعريفنوع صحيح (*XpmFreeColorsFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    صحيح				/* npixels */,
    فراغ*			/*گهيرو*/
#جيڪڏهن_پورو
);

تعريفنوع بنية {
    بنانشان ڊگهو valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    بنانشان صحيح اونهائي;			/* Specifies the depth */
    بنانشان صحيح ويڪر;			/* Returns the width of the created
					   pixmap */
    بنانشان صحيح اوچائي;		/* Returns the height of the created
					   pixmap */
    بنانشان صحيح x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    بنانشان صحيح y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    بنانشان صحيح cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    بنانشان صحيح npixels;		/* Number of used pixels */
    Xpm_Color_نشان *colorsymbols;	/* List of color symbols to override */
    بنانشان صحيح numsymbols;		/* Number of symbols */
    حرف *rgb_fname;			/* RGB text file name */
    بنانشان صحيح nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    بنانشان صحيح ncolors;               /* Number of colors */
    XpmColor *color_جدول;               /* List of colors */
/* 3.2 backward compatibility code */
    حرف *hints_cmt;                    /* Comment of the hints section */
    حرف *colors_cmt;                   /* Comment of the colors section */
    حرف *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    بنانشان صحيح نقاب_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    بنانشان صحيح closeness;		/* Allowable RGB deviation */
    بنانشان صحيح red_closeness;		/* Allowable red deviation */
    بنانشان صحيح green_closeness;	/* Allowable green deviation */
    بنانشان صحيح blue_closeness;	/* Allowable blue deviation */
    صحيح color_ڪنجي;			/* Use colors from this color set */

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
    فراغ *color_گهيرو;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_خاصيتون;

/* XpmAttributes value masks bits */
#وصف_ڪريو XpmVisual	   (1L<<0)
#وصف_ڪريو XpmColormap	   (1L<<1)
#وصف_ڪريو Xpm_اونهائي	   (1L<<2)
#وصف_ڪريو Xpm_سائيز		   (1L<<3)	/* width & height */
#وصف_ڪريو XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#وصف_ڪريو Xpm_Chars_في_Pixel   (1L<<5)
#وصف_ڪريو XpmColorSymbols	   (1L<<6)
#وصف_ڪريو XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#وصف_ڪريو XpmInfos	   (1L<<8)
#وصف_ڪريو XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#وصف_ڪريو XpmReturnPixels	   (1L<<9)
#وصف_ڪريو XpmExtensions      (1L<<10)
#وصف_ڪريو XpmReturnExtensions XpmExtensions

#وصف_ڪريو XpmExactColors     (1L<<11)
#وصف_ڪريو XpmCloseness	   (1L<<12)
#وصف_ڪريو XpmRGBCloseness	   (1L<<13)
#وصف_ڪريو Xpm_Color_ڪنجي	   (1L<<14)

#وصف_ڪريو Xpm_Color_جدول      (1L<<15)
#وصف_ڪريو Xpm_Return_Color_جدول Xpm_Color_جدول

#وصف_ڪريو XpmReturnAllocPixels (1L<<16)
#وصف_ڪريو XpmAllocCloseColors (1L<<17)
#وصف_ڪريو XpmBitmapFormat    (1L<<18)

#وصف_ڪريو XpmAllocColor      (1L<<19)
#وصف_ڪريو XpmFreeColors      (1L<<20)
#وصف_ڪريو Xpm_Color_گهيرو    (1L<<21)


/* XpmInfo value masks bits */
#وصف_ڪريو XpmComments        XpmInfos
#وصف_ڪريو XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#جيڪڏهن_وصف_نه_ٿيل FOR_MSW
#وصف_ڪريو XpmUndefPixel 0x80000000
#وإلا
/* int is only 16 bit for MSW */
#وصف_ڪريو XpmUndefPixel 0x8000
#جيڪڏهن_پورو

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#وصف_ڪريو XPM_MONO	2
#وصف_ڪريو XPM_GREY4	3
#وصف_ڪريو XPM_GRAY4	3
#وصف_ڪريو XPM_GREY 	4
#وصف_ڪريو XPM_ناسي 	4
#وصف_ڪريو XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#إذا NeedFunctionPrototypes
#وصف_ڪريو FUNC(f, t, p) خارجي t f p
#وصف_ڪريو LFUNC(f, t, p) ساكن t f p
#وإلا
#وصف_ڪريو FUNC(f, t, p) خارجي t f()
#وصف_ڪريو LFUNC(f, t, p) ساكن t f()
#جيڪڏهن_پورو


/*
 * functions declarations
 */

#جيڪڏهن_وصف_ٿيل __cplusplus
خارجي "C" {
#جيڪڏهن_پورو

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_معلومات, صحيح, (Display *display,
					Drawable d,
					حرف **معلومات,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Create_معلومات_From_Pixmap, صحيح, (Display *display,
					حرف ***معلومات_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Read_File_منزل_Pixmap, صحيح, (Display *display,
				    Drawable d,
				    حرف *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_خاصيتون *خاصيتون));

    FUNC(XpmWriteFileFromPixmap, صحيح, (Display *display,
				       حرف *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_خاصيتون *خاصيتون));
#جيڪڏهن_پورو

    FUNC(Xpm_Create_Image_From_معلومات, صحيح, (Display *display,
				       حرف **معلومات,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Create_معلومات_From_Image, صحيح, (Display *display,
				       حرف ***معلومات_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Read_File_منزل_Image, صحيح, (Display *display,
				   حرف *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_خاصيتون *خاصيتون));

    FUNC(XpmWriteFileFromImage, صحيح, (Display *display,
				      حرف *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Create_Image_From_بفر, صحيح, (Display *display,
					 حرف *بفر,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_خاصيتون *خاصيتون));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_بفر, صحيح, (Display *display,
					  Drawable d,
					  حرف *بفر,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Create_بفر_From_Image, صحيح, (Display *display,
					 حرف **بفر_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_خاصيتون *خاصيتون));

    FUNC(Xpm_Create_بفر_From_Pixmap, صحيح, (Display *display,
					  حرف **بفر_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_خاصيتون *خاصيتون));
#جيڪڏهن_پورو
    FUNC(Xpm_Read_File_منزل_بفر, صحيح, (حرف *filename, حرف **بفر_return));
    FUNC(Xpm_Write_File_From_بفر, صحيح, (حرف *filename, حرف *بفر));

    FUNC(Xpm_Read_File_منزل_معلومات, صحيح, (حرف *filename, حرف ***معلومات_return));
    FUNC(Xpm_Write_File_From_معلومات, صحيح, (حرف *filename, حرف **معلومات));

    FUNC(Xpm_خاصيتون_سائيز, صحيح, ());
    FUNC(Xpm_Free_خاصيتون, فراغ, (Xpm_خاصيتون *خاصيتون));
    FUNC(XpmFreeExtensions, فراغ, (XpmExtension *extensions,
				   صحيح nextensions));

    FUNC(XpmFreeXpmImage, فراغ, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, فراغ, (XpmInfo *info));
    FUNC(XpmGetErrorString, حرف *, (صحيح errcode));
    FUNC(Xpm_Library_نسخو, صحيح, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_منزل_Xpm_Image, صحيح, (حرف *filename,
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
					    Xpm_خاصيتون *خاصيتون));
#جيڪڏهن_پورو
    FUNC(XpmCreateImageFromXpmImage, صحيح, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_خاصيتون *خاصيتون));

    FUNC(XpmCreateXpmImageFromImage, صحيح, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_خاصيتون *خاصيتون));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, صحيح, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_خاصيتون *خاصيتون));
#جيڪڏهن_پورو
    FUNC(Xpm_Create_معلومات_From_Xpm_Image, صحيح, (حرف ***معلومات_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_معلومات, صحيح, (حرف **معلومات,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_بفر, صحيح, (حرف *بفر,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_بفر_From_Xpm_Image, صحيح, (حرف **بفر_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, صحيح, (حرف *filename,
				 صحيح *linenum_return,
				 صحيح *charnum_return));

    FUNC(XpmFree, فراغ, (فراغ *ptr));

#جيڪڏهن_وصف_ٿيل __cplusplus
} /* for C++ V2.0 */
#جيڪڏهن_پورو


/* backward compatibility */

/* for version 3.0c */
#وصف_ڪريو XpmPixmapColorError  XpmColorError
#وصف_ڪريو XpmPixmapSuccess     XpmSuccess
#وصف_ڪريو XpmPixmapOpenFailed  XpmOpenFailed
#وصف_ڪريو XpmPixmapFileInvalid XpmFileInvalid
#وصف_ڪريو Xpm_Pixmap_No_يادگيري    Xpm_No_يادگيري
#وصف_ڪريو XpmPixmapColorFailed XpmColorFailed

#وصف_ڪريو XpmReadPixmapFile(dpy, d, file, pix, نقاب, att) \
    Xpm_Read_File_منزل_Pixmap(dpy, d, file, pix, نقاب, att)
#وصف_ڪريو XpmWritePixmapFile(dpy, file, pix, نقاب, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, نقاب, att)

/* for version 3.0b */
#وصف_ڪريو PixmapColorError  XpmColorError
#وصف_ڪريو PixmapSuccess     XpmSuccess
#وصف_ڪريو PixmapOpenFailed  XpmOpenFailed
#وصف_ڪريو PixmapFileInvalid XpmFileInvalid
#وصف_ڪريو Pixmap_No_يادگيري    Xpm_No_يادگيري
#وصف_ڪريو PixmapColorFailed XpmColorFailed

#وصف_ڪريو Color_نشان Xpm_Color_نشان

#وصف_ڪريو XReadPixmapFile(dpy, d, file, pix, نقاب, att) \
    Xpm_Read_File_منزل_Pixmap(dpy, d, file, pix, نقاب, att)
#وصف_ڪريو XWritePixmapFile(dpy, file, pix, نقاب, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, نقاب, att)
#وصف_ڪريو XCreate_Pixmap_From_معلومات(dpy, d, معلومات, pix, نقاب, att) \
    Xpm_Create_Pixmap_From_معلومات(dpy, d, معلومات, pix, نقاب, att)
#وصف_ڪريو XCreate_معلومات_From_Pixmap(dpy, معلومات, pix, نقاب, att) \
    Xpm_Create_معلومات_From_Pixmap(dpy, معلومات, pix, نقاب, att)

#جيڪڏهن_پورو /* XPM_NUMBERS */
#جيڪڏهن_پورو
