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

#اگر_غیر_تعریف_شدہ XPM_h
#تعریف_کرو XPM_h

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
#تعریف_کرو XpmFormat 3
#تعریف_کرو Xpm_نسخہ 4
#تعریف_کرو XpmRevision 11
#تعریف_کرو Xpm_Include_نسخہ ((XpmFormat * 100 + Xpm_نسخہ) * 100 + XpmRevision)

#اگر_غیر_تعریف_شدہ XPM_NUMBERS

#اگر_تعریف_شدہ FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#تعریف_کرو NEED_STRCASECMP		/* at least for MSVC++ */
#ورنہ /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# ورنہ /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#اگر_ختم /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#اگر ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
قسمتعریف غیرعلامتی طویل Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#اگر_ختم

/* make sure we know whether function prototypes are needed or not */
#اگر_غیر_تعریف_شدہ NeedFunctionPrototypes
# اگر defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# ورنہ
#  define NeedFunctionPrototypes 0
# endif
#اگر_ختم


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#تعریف_کرو XpmColorError    1
#تعریف_کرو XpmSuccess       0
#تعریف_کرو XpmOpenFailed   -1
#تعریف_کرو XpmFileInvalid  -2
#تعریف_کرو Xpm_No_حافظہ     -3
#تعریف_کرو XpmColorFailed  -4

قسمتعریف ساخت {
    حرف *name;			/* Symbolic color name */
    حرف *قدر;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_علامت;

قسمتعریف ساخت {
    حرف *name;			/* name of the extension */
    غیرعلامتی صحیح nlines;	/* number of lines in this extension */
    حرف **سطریں;		/* pointer to the extension array of strings */
}      XpmExtension;

قسمتعریف ساخت {
    حرف *string;		/* characters string */
    حرف *symbolic;		/* symbolic name */
    حرف *m_color;		/* monochrom default */
    حرف *g4_color;		/* 4 level grayscale default */
    حرف *g_color;		/* other level grayscale default */
    حرف *c_color;		/* color default */
}      XpmColor;

قسمتعریف ساخت {
    غیرعلامتی صحیح چوڑائی;		/* image width */
    غیرعلامتی صحیح اونچائی;	/* image height */
    غیرعلامتی صحیح cpp;		/* number of characters per pixel */
    غیرعلامتی صحیح ncolors;	/* number of colors */
    XpmColor *color_جدول;	/* list of related colors */
    غیرعلامتی صحیح *ڈیٹا;		/* image data */
}      XpmImage;

قسمتعریف ساخت {
    غیرعلامتی طویل valuemask;	/* Specifies which attributes are defined */
    حرف *hints_cmt;		/* Comment of the hints section */
    حرف *colors_cmt;		/* Comment of the colors section */
    حرف *pixels_cmt;		/* Comment of the pixels section */
    غیرعلامتی صحیح x_hotspot;	/* Returns the x hotspot's coordinate */
    غیرعلامتی صحیح y_hotspot;	/* Returns the y hotspot's coordinate */
    غیرعلامتی صحیح nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

قسمتعریف صحیح (*XpmAllocColorFunc)(
#اگر NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    حرف*			/* colorname */,
    XColor*			/* xcolor */,
    خالی*			/*احاطہ*/
#اگر_ختم
);

قسمتعریف صحیح (*XpmFreeColorsFunc)(
#اگر NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    صحیح				/* npixels */,
    خالی*			/*احاطہ*/
#اگر_ختم
);

قسمتعریف ساخت {
    غیرعلامتی طویل valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    غیرعلامتی صحیح گہرائی;			/* Specifies the depth */
    غیرعلامتی صحیح چوڑائی;			/* Returns the width of the created
					   pixmap */
    غیرعلامتی صحیح اونچائی;		/* Returns the height of the created
					   pixmap */
    غیرعلامتی صحیح x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    غیرعلامتی صحیح y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    غیرعلامتی صحیح cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    غیرعلامتی صحیح npixels;		/* Number of used pixels */
    Xpm_Color_علامت *colorsymbols;	/* List of color symbols to override */
    غیرعلامتی صحیح numsymbols;		/* Number of symbols */
    حرف *rgb_fname;			/* RGB text file name */
    غیرعلامتی صحیح nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    غیرعلامتی صحیح ncolors;               /* Number of colors */
    XpmColor *color_جدول;               /* List of colors */
/* 3.2 backward compatibility code */
    حرف *hints_cmt;                    /* Comment of the hints section */
    حرف *colors_cmt;                   /* Comment of the colors section */
    حرف *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    غیرعلامتی صحیح نقاب_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    غیرعلامتی صحیح closeness;		/* Allowable RGB deviation */
    غیرعلامتی صحیح red_closeness;		/* Allowable red deviation */
    غیرعلامتی صحیح green_closeness;	/* Allowable green deviation */
    غیرعلامتی صحیح blue_closeness;	/* Allowable blue deviation */
    صحیح color_کلید;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    صحیح nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    صحیح bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    خالی *color_احاطہ;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_اوصاف;

/* XpmAttributes value masks bits */
#تعریف_کرو XpmVisual	   (1L<<0)
#تعریف_کرو XpmColormap	   (1L<<1)
#تعریف_کرو Xpm_گہرائی	   (1L<<2)
#تعریف_کرو Xpm_سائز		   (1L<<3)	/* width & height */
#تعریف_کرو XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#تعریف_کرو Xpm_Chars_فی_Pixel   (1L<<5)
#تعریف_کرو XpmColorSymbols	   (1L<<6)
#تعریف_کرو XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#تعریف_کرو XpmInfos	   (1L<<8)
#تعریف_کرو XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#تعریف_کرو XpmReturnPixels	   (1L<<9)
#تعریف_کرو XpmExtensions      (1L<<10)
#تعریف_کرو XpmReturnExtensions XpmExtensions

#تعریف_کرو XpmExactColors     (1L<<11)
#تعریف_کرو XpmCloseness	   (1L<<12)
#تعریف_کرو XpmRGBCloseness	   (1L<<13)
#تعریف_کرو Xpm_Color_کلید	   (1L<<14)

#تعریف_کرو Xpm_Color_جدول      (1L<<15)
#تعریف_کرو Xpm_Return_Color_جدول Xpm_Color_جدول

#تعریف_کرو XpmReturnAllocPixels (1L<<16)
#تعریف_کرو XpmAllocCloseColors (1L<<17)
#تعریف_کرو XpmBitmapFormat    (1L<<18)

#تعریف_کرو XpmAllocColor      (1L<<19)
#تعریف_کرو XpmFreeColors      (1L<<20)
#تعریف_کرو Xpm_Color_احاطہ    (1L<<21)


/* XpmInfo value masks bits */
#تعریف_کرو XpmComments        XpmInfos
#تعریف_کرو XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#اگر_غیر_تعریف_شدہ FOR_MSW
#تعریف_کرو XpmUndefPixel 0x80000000
#ورنہ
/* int is only 16 bit for MSW */
#تعریف_کرو XpmUndefPixel 0x8000
#اگر_ختم

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#تعریف_کرو XPM_MONO	2
#تعریف_کرو XPM_GREY4	3
#تعریف_کرو XPM_GRAY4	3
#تعریف_کرو XPM_GREY 	4
#تعریف_کرو XPM_سرمئی 	4
#تعریف_کرو XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#اگر NeedFunctionPrototypes
#تعریف_کرو FUNC(f, t, p) خارجی t f p
#تعریف_کرو LFUNC(f, t, p) ساکن t f p
#ورنہ
#تعریف_کرو FUNC(f, t, p) خارجی t f()
#تعریف_کرو LFUNC(f, t, p) ساکن t f()
#اگر_ختم


/*
 * functions declarations
 */

#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#اگر !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_ڈیٹا, صحیح, (Display *display,
					Drawable d,
					حرف **ڈیٹا,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Create_ڈیٹا_From_Pixmap, صحیح, (Display *display,
					حرف ***ڈیٹا_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Read_File_مقصد_Pixmap, صحیح, (Display *display,
				    Drawable d,
				    حرف *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_اوصاف *اوصاف));

    FUNC(XpmWriteFileFromPixmap, صحیح, (Display *display,
				       حرف *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_اوصاف *اوصاف));
#اگر_ختم

    FUNC(Xpm_Create_Image_From_ڈیٹا, صحیح, (Display *display,
				       حرف **ڈیٹا,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Create_ڈیٹا_From_Image, صحیح, (Display *display,
				       حرف ***ڈیٹا_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Read_File_مقصد_Image, صحیح, (Display *display,
				   حرف *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_اوصاف *اوصاف));

    FUNC(XpmWriteFileFromImage, صحیح, (Display *display,
				      حرف *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Create_Image_From_بفر, صحیح, (Display *display,
					 حرف *بفر,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_اوصاف *اوصاف));
#اگر !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_بفر, صحیح, (Display *display,
					  Drawable d,
					  حرف *بفر,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Create_بفر_From_Image, صحیح, (Display *display,
					 حرف **بفر_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_اوصاف *اوصاف));

    FUNC(Xpm_Create_بفر_From_Pixmap, صحیح, (Display *display,
					  حرف **بفر_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_اوصاف *اوصاف));
#اگر_ختم
    FUNC(Xpm_Read_File_مقصد_بفر, صحیح, (حرف *filename, حرف **بفر_return));
    FUNC(Xpm_Write_File_From_بفر, صحیح, (حرف *filename, حرف *بفر));

    FUNC(Xpm_Read_File_مقصد_ڈیٹا, صحیح, (حرف *filename, حرف ***ڈیٹا_return));
    FUNC(Xpm_Write_File_From_ڈیٹا, صحیح, (حرف *filename, حرف **ڈیٹا));

    FUNC(Xpm_اوصاف_سائز, صحیح, ());
    FUNC(Xpm_Free_اوصاف, خالی, (Xpm_اوصاف *اوصاف));
    FUNC(XpmFreeExtensions, خالی, (XpmExtension *extensions,
				   صحیح nextensions));

    FUNC(XpmFreeXpmImage, خالی, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, خالی, (XpmInfo *info));
    FUNC(XpmGetErrorString, حرف *, (صحیح errcode));
    FUNC(Xpm_Library_نسخہ, صحیح, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_مقصد_Xpm_Image, صحیح, (حرف *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, صحیح, (حرف *filename,
					 XpmImage *image,
					 XpmInfo *info));
#اگر !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, صحیح, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    Xpm_اوصاف *اوصاف));
#اگر_ختم
    FUNC(XpmCreateImageFromXpmImage, صحیح, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_اوصاف *اوصاف));

    FUNC(XpmCreateXpmImageFromImage, صحیح, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_اوصاف *اوصاف));
#اگر !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, صحیح, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_اوصاف *اوصاف));
#اگر_ختم
    FUNC(Xpm_Create_ڈیٹا_From_Xpm_Image, صحیح, (حرف ***ڈیٹا_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_ڈیٹا, صحیح, (حرف **ڈیٹا,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_بفر, صحیح, (حرف *بفر,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_بفر_From_Xpm_Image, صحیح, (حرف **بفر_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, صحیح, (حرف *filename,
				 صحیح *linenum_return,
				 صحیح *charnum_return));

    FUNC(XpmFree, خالی, (خالی *ptr));

#اگر_تعریف_شدہ __cplusplus
} /* for C++ V2.0 */
#اگر_ختم


/* backward compatibility */

/* for version 3.0c */
#تعریف_کرو XpmPixmapColorError  XpmColorError
#تعریف_کرو XpmPixmapSuccess     XpmSuccess
#تعریف_کرو XpmPixmapOpenFailed  XpmOpenFailed
#تعریف_کرو XpmPixmapFileInvalid XpmFileInvalid
#تعریف_کرو Xpm_Pixmap_No_حافظہ    Xpm_No_حافظہ
#تعریف_کرو XpmPixmapColorFailed XpmColorFailed

#تعریف_کرو XpmReadPixmapFile(dpy, d, file, pix, نقاب, att) \
    Xpm_Read_File_مقصد_Pixmap(dpy, d, file, pix, نقاب, att)
#تعریف_کرو XpmWritePixmapFile(dpy, file, pix, نقاب, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, نقاب, att)

/* for version 3.0b */
#تعریف_کرو PixmapColorError  XpmColorError
#تعریف_کرو PixmapSuccess     XpmSuccess
#تعریف_کرو PixmapOpenFailed  XpmOpenFailed
#تعریف_کرو PixmapFileInvalid XpmFileInvalid
#تعریف_کرو Pixmap_No_حافظہ    Xpm_No_حافظہ
#تعریف_کرو PixmapColorFailed XpmColorFailed

#تعریف_کرو Color_علامت Xpm_Color_علامت

#تعریف_کرو XReadPixmapFile(dpy, d, file, pix, نقاب, att) \
    Xpm_Read_File_مقصد_Pixmap(dpy, d, file, pix, نقاب, att)
#تعریف_کرو XWritePixmapFile(dpy, file, pix, نقاب, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, نقاب, att)
#تعریف_کرو XCreate_Pixmap_From_ڈیٹا(dpy, d, ڈیٹا, pix, نقاب, att) \
    Xpm_Create_Pixmap_From_ڈیٹا(dpy, d, ڈیٹا, pix, نقاب, att)
#تعریف_کرو XCreate_ڈیٹا_From_Pixmap(dpy, ڈیٹا, pix, نقاب, att) \
    Xpm_Create_ڈیٹا_From_Pixmap(dpy, ڈیٹا, pix, نقاب, att)

#اگر_ختم /* XPM_NUMBERS */
#اگر_ختم
