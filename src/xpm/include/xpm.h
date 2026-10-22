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

#که_نه_تعریف_شوی XPM_h
#تعریف_کړه XPM_h

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
#تعریف_کړه XpmFormat 3
#تعریف_کړه Xpm_نسخه 4
#تعریف_کړه XpmRevision 11
#تعریف_کړه Xpm_Include_نسخه ((XpmFormat * 100 + Xpm_نسخه) * 100 + XpmRevision)

#که_نه_تعریف_شوی XPM_NUMBERS

#که_تعریف_شوی FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#تعریف_کړه NEED_STRCASECMP		/* at least for MSVC++ */
#وإلا /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# وإلا /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#د_که_پای /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#إذا ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
تعريفنوع بینښه اوږد Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#د_که_پای

/* make sure we know whether function prototypes are needed or not */
#که_نه_تعریف_شوی NeedFunctionPrototypes
# إذا defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# وإلا
#  define NeedFunctionPrototypes 0
# endif
#د_که_پای


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#تعریف_کړه XpmColorError    1
#تعریف_کړه XpmSuccess       0
#تعریف_کړه XpmOpenFailed   -1
#تعریف_کړه XpmFileInvalid  -2
#تعریف_کړه Xpm_No_حافظه     -3
#تعریف_کړه XpmColorFailed  -4

تعريفنوع بنية {
    حرف *name;			/* Symbolic color name */
    حرف *ارزښت;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_نښه;

تعريفنوع بنية {
    حرف *name;			/* name of the extension */
    بینښه صحيح nlines;	/* number of lines in this extension */
    حرف **کرښې;		/* pointer to the extension array of strings */
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
    بینښه صحيح پلنوالی;		/* image width */
    بینښه صحيح لوړوالی;	/* image height */
    بینښه صحيح cpp;		/* number of characters per pixel */
    بینښه صحيح ncolors;	/* number of colors */
    XpmColor *color_جدول;	/* list of related colors */
    بینښه صحيح *معلومات;		/* image data */
}      XpmImage;

تعريفنوع بنية {
    بینښه اوږد valuemask;	/* Specifies which attributes are defined */
    حرف *hints_cmt;		/* Comment of the hints section */
    حرف *colors_cmt;		/* Comment of the colors section */
    حرف *pixels_cmt;		/* Comment of the pixels section */
    بینښه صحيح x_hotspot;	/* Returns the x hotspot's coordinate */
    بینښه صحيح y_hotspot;	/* Returns the y hotspot's coordinate */
    بینښه صحيح nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

تعريفنوع صحيح (*XpmAllocColorFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    حرف*			/* colorname */,
    XColor*			/* xcolor */,
    فراغ*			/*تړون*/
#د_که_پای
);

تعريفنوع صحيح (*XpmFreeColorsFunc)(
#إذا NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    صحيح				/* npixels */,
    فراغ*			/*تړون*/
#د_که_پای
);

تعريفنوع بنية {
    بینښه اوږد valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    بینښه صحيح ژوروالی;			/* Specifies the depth */
    بینښه صحيح پلنوالی;			/* Returns the width of the created
					   pixmap */
    بینښه صحيح لوړوالی;		/* Returns the height of the created
					   pixmap */
    بینښه صحيح x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    بینښه صحيح y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    بینښه صحيح cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    بینښه صحيح npixels;		/* Number of used pixels */
    Xpm_Color_نښه *colorsymbols;	/* List of color symbols to override */
    بینښه صحيح numsymbols;		/* Number of symbols */
    حرف *rgb_fname;			/* RGB text file name */
    بینښه صحيح nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    بینښه صحيح ncolors;               /* Number of colors */
    XpmColor *color_جدول;               /* List of colors */
/* 3.2 backward compatibility code */
    حرف *hints_cmt;                    /* Comment of the hints section */
    حرف *colors_cmt;                   /* Comment of the colors section */
    حرف *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    بینښه صحيح ماسک_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    بینښه صحيح closeness;		/* Allowable RGB deviation */
    بینښه صحيح red_closeness;		/* Allowable red deviation */
    بینښه صحيح green_closeness;	/* Allowable green deviation */
    بینښه صحيح blue_closeness;	/* Allowable blue deviation */
    صحيح color_کلی;			/* Use colors from this color set */

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
    فراغ *color_تړون;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_ځانګړتیاوې;

/* XpmAttributes value masks bits */
#تعریف_کړه XpmVisual	   (1L<<0)
#تعریف_کړه XpmColormap	   (1L<<1)
#تعریف_کړه Xpm_ژوروالی	   (1L<<2)
#تعریف_کړه Xpm_اندازه		   (1L<<3)	/* width & height */
#تعریف_کړه XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#تعریف_کړه Xpm_Chars_فی_Pixel   (1L<<5)
#تعریف_کړه XpmColorSymbols	   (1L<<6)
#تعریف_کړه XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#تعریف_کړه XpmInfos	   (1L<<8)
#تعریف_کړه XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#تعریف_کړه XpmReturnPixels	   (1L<<9)
#تعریف_کړه XpmExtensions      (1L<<10)
#تعریف_کړه XpmReturnExtensions XpmExtensions

#تعریف_کړه XpmExactColors     (1L<<11)
#تعریف_کړه XpmCloseness	   (1L<<12)
#تعریف_کړه XpmRGBCloseness	   (1L<<13)
#تعریف_کړه Xpm_Color_کلی	   (1L<<14)

#تعریف_کړه Xpm_Color_جدول      (1L<<15)
#تعریف_کړه Xpm_Return_Color_جدول Xpm_Color_جدول

#تعریف_کړه XpmReturnAllocPixels (1L<<16)
#تعریف_کړه XpmAllocCloseColors (1L<<17)
#تعریف_کړه XpmBitmapFormat    (1L<<18)

#تعریف_کړه XpmAllocColor      (1L<<19)
#تعریف_کړه XpmFreeColors      (1L<<20)
#تعریف_کړه Xpm_Color_تړون    (1L<<21)


/* XpmInfo value masks bits */
#تعریف_کړه XpmComments        XpmInfos
#تعریف_کړه XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#که_نه_تعریف_شوی FOR_MSW
#تعریف_کړه XpmUndefPixel 0x80000000
#وإلا
/* int is only 16 bit for MSW */
#تعریف_کړه XpmUndefPixel 0x8000
#د_که_پای

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#تعریف_کړه XPM_MONO	2
#تعریف_کړه XPM_GREY4	3
#تعریف_کړه XPM_GRAY4	3
#تعریف_کړه XPM_GREY 	4
#تعریف_کړه XPM_خړ 	4
#تعریف_کړه XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#إذا NeedFunctionPrototypes
#تعریف_کړه FUNC(f, t, p) خارجي t f p
#تعریف_کړه LFUNC(f, t, p) ساكن t f p
#وإلا
#تعریف_کړه FUNC(f, t, p) خارجي t f()
#تعریف_کړه LFUNC(f, t, p) ساكن t f()
#د_که_پای


/*
 * functions declarations
 */

#که_تعریف_شوی __cplusplus
خارجي "C" {
#د_که_پای

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_معلومات, صحيح, (Display *display,
					Drawable d,
					حرف **معلومات,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Create_معلومات_From_Pixmap, صحيح, (Display *display,
					حرف ***معلومات_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Read_File_منزل_Pixmap, صحيح, (Display *display,
				    Drawable d,
				    حرف *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(XpmWriteFileFromPixmap, صحيح, (Display *display,
				       حرف *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_ځانګړتیاوې *ځانګړتیاوې));
#د_که_پای

    FUNC(Xpm_Create_Image_From_معلومات, صحيح, (Display *display,
				       حرف **معلومات,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Create_معلومات_From_Image, صحيح, (Display *display,
				       حرف ***معلومات_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Read_File_منزل_Image, صحيح, (Display *display,
				   حرف *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(XpmWriteFileFromImage, صحيح, (Display *display,
				      حرف *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Create_Image_From_بفر, صحيح, (Display *display,
					 حرف *بفر,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_ځانګړتیاوې *ځانګړتیاوې));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_بفر, صحيح, (Display *display,
					  Drawable d,
					  حرف *بفر,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Create_بفر_From_Image, صحيح, (Display *display,
					 حرف **بفر_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(Xpm_Create_بفر_From_Pixmap, صحيح, (Display *display,
					  حرف **بفر_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_ځانګړتیاوې *ځانګړتیاوې));
#د_که_پای
    FUNC(Xpm_Read_File_منزل_بفر, صحيح, (حرف *filename, حرف **بفر_return));
    FUNC(Xpm_Write_File_From_بفر, صحيح, (حرف *filename, حرف *بفر));

    FUNC(Xpm_Read_File_منزل_معلومات, صحيح, (حرف *filename, حرف ***معلومات_return));
    FUNC(Xpm_Write_File_From_معلومات, صحيح, (حرف *filename, حرف **معلومات));

    FUNC(Xpm_ځانګړتیاوې_اندازه, صحيح, ());
    FUNC(Xpm_Free_ځانګړتیاوې, فراغ, (Xpm_ځانګړتیاوې *ځانګړتیاوې));
    FUNC(XpmFreeExtensions, فراغ, (XpmExtension *extensions,
				   صحيح nextensions));

    FUNC(XpmFreeXpmImage, فراغ, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, فراغ, (XpmInfo *info));
    FUNC(XpmGetErrorString, حرف *, (صحيح errcode));
    FUNC(Xpm_Library_نسخه, صحيح, ());

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
					    Xpm_ځانګړتیاوې *ځانګړتیاوې));
#د_که_پای
    FUNC(XpmCreateImageFromXpmImage, صحيح, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_ځانګړتیاوې *ځانګړتیاوې));

    FUNC(XpmCreateXpmImageFromImage, صحيح, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_ځانګړتیاوې *ځانګړتیاوې));
#إذا !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, صحيح, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_ځانګړتیاوې *ځانګړتیاوې));
#د_که_پای
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

#که_تعریف_شوی __cplusplus
} /* for C++ V2.0 */
#د_که_پای


/* backward compatibility */

/* for version 3.0c */
#تعریف_کړه XpmPixmapColorError  XpmColorError
#تعریف_کړه XpmPixmapSuccess     XpmSuccess
#تعریف_کړه XpmPixmapOpenFailed  XpmOpenFailed
#تعریف_کړه XpmPixmapFileInvalid XpmFileInvalid
#تعریف_کړه Xpm_Pixmap_No_حافظه    Xpm_No_حافظه
#تعریف_کړه XpmPixmapColorFailed XpmColorFailed

#تعریف_کړه XpmReadPixmapFile(dpy, d, file, pix, ماسک, att) \
    Xpm_Read_File_منزل_Pixmap(dpy, d, file, pix, ماسک, att)
#تعریف_کړه XpmWritePixmapFile(dpy, file, pix, ماسک, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ماسک, att)

/* for version 3.0b */
#تعریف_کړه PixmapColorError  XpmColorError
#تعریف_کړه PixmapSuccess     XpmSuccess
#تعریف_کړه PixmapOpenFailed  XpmOpenFailed
#تعریف_کړه PixmapFileInvalid XpmFileInvalid
#تعریف_کړه Pixmap_No_حافظه    Xpm_No_حافظه
#تعریف_کړه PixmapColorFailed XpmColorFailed

#تعریف_کړه Color_نښه Xpm_Color_نښه

#تعریف_کړه XReadPixmapFile(dpy, d, file, pix, ماسک, att) \
    Xpm_Read_File_منزل_Pixmap(dpy, d, file, pix, ماسک, att)
#تعریف_کړه XWritePixmapFile(dpy, file, pix, ماسک, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ماسک, att)
#تعریف_کړه XCreate_Pixmap_From_معلومات(dpy, d, معلومات, pix, ماسک, att) \
    Xpm_Create_Pixmap_From_معلومات(dpy, d, معلومات, pix, ماسک, att)
#تعریف_کړه XCreate_معلومات_From_Pixmap(dpy, معلومات, pix, ماسک, att) \
    Xpm_Create_معلومات_From_Pixmap(dpy, معلومات, pix, ماسک, att)

#د_که_پای /* XPM_NUMBERS */
#د_که_پای
