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

#నిర్వచితం_కాకపోతే XPM_h
#నిర్వచించు XPM_h

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
#నిర్వచించు XpmFormat 3
#నిర్వచించు Xpm_సంస్కరణ 4
#నిర్వచించు XpmRevision 11
#నిర్వచించు Xpm_Include_సంస్కరణ ((XpmFormat * 100 + Xpm_సంస్కరణ) * 100 + XpmRevision)

#నిర్వచితం_కాకపోతే XPM_NUMBERS

#నిర్వచితమైతే FOR_MSW
# define SYSV			/* uses memcpy string.h etc. */
# include <malloc.h>
# include "simx.h"		/* defines some X stuff using MSW types */
#నిర్వచించు NEED_STRCASECMP		/* at least for MSVC++ */
#లేకపోతే /* FOR_MSW */
# ifdef AMIGA
#  include "amigax.h"
# లేకపోతే /* not AMIGA */
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
# endif /* not AMIGA */
#అయితే_ముగింపు /* FOR_MSW */

/* let's define Pixel if it is not done yet */
#ఒకవేళ ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
రకనిర్వచనం గుర్తులేని దీర్ఘ Pixel;	/* Index into colormap */
# define PIXEL_ALREADY_TYPEDEFED
#అయితే_ముగింపు

/* make sure we know whether function prototypes are needed or not */
#నిర్వచితం_కాకపోతే NeedFunctionPrototypes
# ఒకవేళ defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#  define NeedFunctionPrototypes 1
# లేకపోతే
#  define NeedFunctionPrototypes 0
# endif
#అయితే_ముగింపు


/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#నిర్వచించు XpmColorError    1
#నిర్వచించు XpmSuccess       0
#నిర్వచించు XpmOpenFailed   -1
#నిర్వచించు XpmFileInvalid  -2
#నిర్వచించు Xpm_No_జ్ఞాపకం     -3
#నిర్వచించు XpmColorFailed  -4

రకనిర్వచనం నిర్మాణం {
    అక్షరం *name;			/* Symbolic color name */
    అక్షరం *విలువ;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      Xpm_Color_చిహ్నం;

రకనిర్వచనం నిర్మాణం {
    అక్షరం *name;			/* name of the extension */
    గుర్తులేని పూర్ణాంకం nlines;	/* number of lines in this extension */
    అక్షరం **పంక్తులు;		/* pointer to the extension array of strings */
}      XpmExtension;

రకనిర్వచనం నిర్మాణం {
    అక్షరం *string;		/* characters string */
    అక్షరం *symbolic;		/* symbolic name */
    అక్షరం *m_color;		/* monochrom default */
    అక్షరం *g4_color;		/* 4 level grayscale default */
    అక్షరం *g_color;		/* other level grayscale default */
    అక్షరం *c_color;		/* color default */
}      XpmColor;

రకనిర్వచనం నిర్మాణం {
    గుర్తులేని పూర్ణాంకం వెడల్పు;		/* image width */
    గుర్తులేని పూర్ణాంకం ఎత్తు;	/* image height */
    గుర్తులేని పూర్ణాంకం cpp;		/* number of characters per pixel */
    గుర్తులేని పూర్ణాంకం ncolors;	/* number of colors */
    XpmColor *color_పట్టిక;	/* list of related colors */
    గుర్తులేని పూర్ణాంకం *దత్తాంశం;		/* image data */
}      XpmImage;

రకనిర్వచనం నిర్మాణం {
    గుర్తులేని దీర్ఘ valuemask;	/* Specifies which attributes are defined */
    అక్షరం *hints_cmt;		/* Comment of the hints section */
    అక్షరం *colors_cmt;		/* Comment of the colors section */
    అక్షరం *pixels_cmt;		/* Comment of the pixels section */
    గుర్తులేని పూర్ణాంకం x_hotspot;	/* Returns the x hotspot's coordinate */
    గుర్తులేని పూర్ణాంకం y_hotspot;	/* Returns the y hotspot's coordinate */
    గుర్తులేని పూర్ణాంకం nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

రకనిర్వచనం పూర్ణాంకం (*XpmAllocColorFunc)(
#ఒకవేళ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    అక్షరం*			/* colorname */,
    XColor*			/* xcolor */,
    శూన్య*			/*సంవృతం*/
#అయితే_ముగింపు
);

రకనిర్వచనం పూర్ణాంకం (*XpmFreeColorsFunc)(
#ఒకవేళ NeedFunctionPrototypes
    Display*			/* display */,
    Colormap			/* colormap */,
    Pixel*			/* pixels */,
    పూర్ణాంకం				/* npixels */,
    శూన్య*			/*సంవృతం*/
#అయితే_ముగింపు
);

రకనిర్వచనం నిర్మాణం {
    గుర్తులేని దీర్ఘ valuemask;		/* Specifies which attributes are
					   defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    గుర్తులేని పూర్ణాంకం లోతు;			/* Specifies the depth */
    గుర్తులేని పూర్ణాంకం వెడల్పు;			/* Returns the width of the created
					   pixmap */
    గుర్తులేని పూర్ణాంకం ఎత్తు;		/* Returns the height of the created
					   pixmap */
    గుర్తులేని పూర్ణాంకం x_hotspot;		/* Returns the x hotspot's
					   coordinate */
    గుర్తులేని పూర్ణాంకం y_hotspot;		/* Returns the y hotspot's
					   coordinate */
    గుర్తులేని పూర్ణాంకం cpp;			/* Specifies the number of char per
					   pixel */
    Pixel *pixels;			/* List of used color pixels */
    గుర్తులేని పూర్ణాంకం npixels;		/* Number of used pixels */
    Xpm_Color_చిహ్నం *colorsymbols;	/* List of color symbols to override */
    గుర్తులేని పూర్ణాంకం numsymbols;		/* Number of symbols */
    అక్షరం *rgb_fname;			/* RGB text file name */
    గుర్తులేని పూర్ణాంకం nextensions;		/* Number of extensions */
    XpmExtension *extensions;		/* List of extensions */

    గుర్తులేని పూర్ణాంకం ncolors;               /* Number of colors */
    XpmColor *color_పట్టిక;               /* List of colors */
/* 3.2 backward compatibility code */
    అక్షరం *hints_cmt;                    /* Comment of the hints section */
    అక్షరం *colors_cmt;                   /* Comment of the colors section */
    అక్షరం *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    గుర్తులేని పూర్ణాంకం ముసుగు_pixel;            /* Color table index of transparent
                                           color */

    /* Color Allocation Directives */
    Bool exactColors;			/* Only use exact colors for visual */
    గుర్తులేని పూర్ణాంకం closeness;		/* Allowable RGB deviation */
    గుర్తులేని పూర్ణాంకం red_closeness;		/* Allowable red deviation */
    గుర్తులేని పూర్ణాంకం green_closeness;	/* Allowable green deviation */
    గుర్తులేని పూర్ణాంకం blue_closeness;	/* Allowable blue deviation */
    పూర్ణాంకం color_కీలకం;			/* Use colors from this color set */

    Pixel *alloc_pixels;		/* Returns the list of alloc'ed color
					   pixels */
    పూర్ణాంకం nalloc_pixels;			/* Returns the number of alloc'ed
					   color pixels */

    Bool alloc_close_colors;    	/* Specify whether close colors should
					   be allocated using XAllocColor
					   or not */
    పూర్ణాంకం bitmap_format;			/* Specify the format of 1bit depth
					   images: ZPixmap or XYBitmap */

    /* Color functions */
    XpmAllocColorFunc alloc_color;	/* Application color allocator */
    XpmFreeColorsFunc free_colors;	/* Application color de-allocator */
    శూన్య *color_సంవృతం;		/* Application private data to pass to
					   alloc_color and free_colors */

}      Xpm_లక్షణాలు;

/* XpmAttributes value masks bits */
#నిర్వచించు XpmVisual	   (1L<<0)
#నిర్వచించు XpmColormap	   (1L<<1)
#నిర్వచించు Xpm_లోతు	   (1L<<2)
#నిర్వచించు Xpm_కొలత		   (1L<<3)	/* width & height */
#నిర్వచించు XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#నిర్వచించు Xpm_Chars_చొప్పున_Pixel   (1L<<5)
#నిర్వచించు XpmColorSymbols	   (1L<<6)
#నిర్వచించు XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#నిర్వచించు XpmInfos	   (1L<<8)
#నిర్వచించు XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#నిర్వచించు XpmReturnPixels	   (1L<<9)
#నిర్వచించు XpmExtensions      (1L<<10)
#నిర్వచించు XpmReturnExtensions XpmExtensions

#నిర్వచించు XpmExactColors     (1L<<11)
#నిర్వచించు XpmCloseness	   (1L<<12)
#నిర్వచించు XpmRGBCloseness	   (1L<<13)
#నిర్వచించు Xpm_Color_కీలకం	   (1L<<14)

#నిర్వచించు Xpm_Color_పట్టిక      (1L<<15)
#నిర్వచించు Xpm_Return_Color_పట్టిక Xpm_Color_పట్టిక

#నిర్వచించు XpmReturnAllocPixels (1L<<16)
#నిర్వచించు XpmAllocCloseColors (1L<<17)
#నిర్వచించు XpmBitmapFormat    (1L<<18)

#నిర్వచించు XpmAllocColor      (1L<<19)
#నిర్వచించు XpmFreeColors      (1L<<20)
#నిర్వచించు Xpm_Color_సంవృతం    (1L<<21)


/* XpmInfo value masks bits */
#నిర్వచించు XpmComments        XpmInfos
#నిర్వచించు XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#నిర్వచితం_కాకపోతే FOR_MSW
#నిర్వచించు XpmUndefPixel 0x80000000
#లేకపోతే
/* int is only 16 bit for MSW */
#నిర్వచించు XpmUndefPixel 0x8000
#అయితే_ముగింపు

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in XpmI.h
 */
#నిర్వచించు XPM_MONO	2
#నిర్వచించు XPM_GREY4	3
#నిర్వచించు XPM_GRAY4	3
#నిర్వచించు XPM_GREY 	4
#నిర్వచించు XPM_బూడిద 	4
#నిర్వచించు XPM_COLOR	5


/* macros for forward declarations of functions with prototypes */
#ఒకవేళ NeedFunctionPrototypes
#నిర్వచించు FUNC(f, t, p) బాహ్య t f p
#నిర్వచించు LFUNC(f, t, p) స్థిర t f p
#లేకపోతే
#నిర్వచించు FUNC(f, t, p) బాహ్య t f()
#నిర్వచించు LFUNC(f, t, p) స్థిర t f()
#అయితే_ముగింపు


/*
 * functions declarations
 */

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */
/* Same for Amiga! */

#ఒకవేళ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_దత్తాంశం, పూర్ణాంకం, (Display *display,
					Drawable d,
					అక్షరం **దత్తాంశం,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Create_దత్తాంశం_From_Pixmap, పూర్ణాంకం, (Display *display,
					అక్షరం ***దత్తాంశం_return,
					Pixmap pixmap,
					Pixmap shapemask,
					Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Read_File_గమ్యం_Pixmap, పూర్ణాంకం, (Display *display,
				    Drawable d,
				    అక్షరం *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(XpmWriteFileFromPixmap, పూర్ణాంకం, (Display *display,
				       అక్షరం *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       Xpm_లక్షణాలు *లక్షణాలు));
#అయితే_ముగింపు

    FUNC(Xpm_Create_Image_From_దత్తాంశం, పూర్ణాంకం, (Display *display,
				       అక్షరం **దత్తాంశం,
				       XImage **image_return,
				       XImage **shapemask_return,
				       Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Create_దత్తాంశం_From_Image, పూర్ణాంకం, (Display *display,
				       అక్షరం ***దత్తాంశం_return,
				       XImage *image,
				       XImage *shapeimage,
				       Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Read_File_గమ్యం_Image, పూర్ణాంకం, (Display *display,
				   అక్షరం *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(XpmWriteFileFromImage, పూర్ణాంకం, (Display *display,
				      అక్షరం *filename,
				      XImage *image,
				      XImage *shapeimage,
				      Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Create_Image_From_బఫరు, పూర్ణాంకం, (Display *display,
					 అక్షరం *బఫరు,
					 XImage **image_return,
					 XImage **shapemask_return,
					 Xpm_లక్షణాలు *లక్షణాలు));
#ఒకవేళ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(Xpm_Create_Pixmap_From_బఫరు, పూర్ణాంకం, (Display *display,
					  Drawable d,
					  అక్షరం *బఫరు,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Create_బఫరు_From_Image, పూర్ణాంకం, (Display *display,
					 అక్షరం **బఫరు_return,
					 XImage *image,
					 XImage *shapeimage,
					 Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(Xpm_Create_బఫరు_From_Pixmap, పూర్ణాంకం, (Display *display,
					  అక్షరం **బఫరు_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  Xpm_లక్షణాలు *లక్షణాలు));
#అయితే_ముగింపు
    FUNC(Xpm_Read_File_గమ్యం_బఫరు, పూర్ణాంకం, (అక్షరం *filename, అక్షరం **బఫరు_return));
    FUNC(Xpm_Write_File_From_బఫరు, పూర్ణాంకం, (అక్షరం *filename, అక్షరం *బఫరు));

    FUNC(Xpm_Read_File_గమ్యం_దత్తాంశం, పూర్ణాంకం, (అక్షరం *filename, అక్షరం ***దత్తాంశం_return));
    FUNC(Xpm_Write_File_From_దత్తాంశం, పూర్ణాంకం, (అక్షరం *filename, అక్షరం **దత్తాంశం));

    FUNC(Xpm_లక్షణాలు_కొలత, పూర్ణాంకం, ());
    FUNC(Xpm_Free_లక్షణాలు, శూన్య, (Xpm_లక్షణాలు *లక్షణాలు));
    FUNC(XpmFreeExtensions, శూన్య, (XpmExtension *extensions,
				   పూర్ణాంకం nextensions));

    FUNC(XpmFreeXpmImage, శూన్య, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, శూన్య, (XpmInfo *info));
    FUNC(XpmGetErrorString, అక్షరం *, (పూర్ణాంకం errcode));
    FUNC(Xpm_Library_సంస్కరణ, పూర్ణాంకం, ());

    /* XpmImage functions */
    FUNC(Xpm_Read_File_గమ్యం_Xpm_Image, పూర్ణాంకం, (అక్షరం *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, పూర్ణాంకం, (అక్షరం *filename,
					 XpmImage *image,
					 XpmInfo *info));
#ఒకవేళ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreatePixmapFromXpmImage, పూర్ణాంకం, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    Xpm_లక్షణాలు *లక్షణాలు));
#అయితే_ముగింపు
    FUNC(XpmCreateImageFromXpmImage, పూర్ణాంకం, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   Xpm_లక్షణాలు *లక్షణాలు));

    FUNC(XpmCreateXpmImageFromImage, పూర్ణాంకం, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   Xpm_లక్షణాలు *లక్షణాలు));
#ఒకవేళ !defined(FOR_MSW) && !defined(AMIGA)
    FUNC(XpmCreateXpmImageFromPixmap, పూర్ణాంకం, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    Xpm_లక్షణాలు *లక్షణాలు));
#అయితే_ముగింపు
    FUNC(Xpm_Create_దత్తాంశం_From_Xpm_Image, పూర్ణాంకం, (అక్షరం ***దత్తాంశం_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_దత్తాంశం, పూర్ణాంకం, (అక్షరం **దత్తాంశం,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(Xpm_Create_Xpm_Image_From_బఫరు, పూర్ణాంకం, (అక్షరం *బఫరు,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(Xpm_Create_బఫరు_From_Xpm_Image, పూర్ణాంకం, (అక్షరం **బఫరు_return,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmGetParseError, పూర్ణాంకం, (అక్షరం *filename,
				 పూర్ణాంకం *linenum_return,
				 పూర్ణాంకం *charnum_return));

    FUNC(XpmFree, శూన్య, (శూన్య *ptr));

#నిర్వచితమైతే __cplusplus
} /* for C++ V2.0 */
#అయితే_ముగింపు


/* backward compatibility */

/* for version 3.0c */
#నిర్వచించు XpmPixmapColorError  XpmColorError
#నిర్వచించు XpmPixmapSuccess     XpmSuccess
#నిర్వచించు XpmPixmapOpenFailed  XpmOpenFailed
#నిర్వచించు XpmPixmapFileInvalid XpmFileInvalid
#నిర్వచించు Xpm_Pixmap_No_జ్ఞాపకం    Xpm_No_జ్ఞాపకం
#నిర్వచించు XpmPixmapColorFailed XpmColorFailed

#నిర్వచించు XpmReadPixmapFile(dpy, d, file, pix, ముసుగు, att) \
    Xpm_Read_File_గమ్యం_Pixmap(dpy, d, file, pix, ముసుగు, att)
#నిర్వచించు XpmWritePixmapFile(dpy, file, pix, ముసుగు, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ముసుగు, att)

/* for version 3.0b */
#నిర్వచించు PixmapColorError  XpmColorError
#నిర్వచించు PixmapSuccess     XpmSuccess
#నిర్వచించు PixmapOpenFailed  XpmOpenFailed
#నిర్వచించు PixmapFileInvalid XpmFileInvalid
#నిర్వచించు Pixmap_No_జ్ఞాపకం    Xpm_No_జ్ఞాపకం
#నిర్వచించు PixmapColorFailed XpmColorFailed

#నిర్వచించు Color_చిహ్నం Xpm_Color_చిహ్నం

#నిర్వచించు XReadPixmapFile(dpy, d, file, pix, ముసుగు, att) \
    Xpm_Read_File_గమ్యం_Pixmap(dpy, d, file, pix, ముసుగు, att)
#నిర్వచించు XWritePixmapFile(dpy, file, pix, ముసుగు, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, ముసుగు, att)
#నిర్వచించు XCreate_Pixmap_From_దత్తాంశం(dpy, d, దత్తాంశం, pix, ముసుగు, att) \
    Xpm_Create_Pixmap_From_దత్తాంశం(dpy, d, దత్తాంశం, pix, ముసుగు, att)
#నిర్వచించు XCreate_దత్తాంశం_From_Pixmap(dpy, దత్తాంశం, pix, ముసుగు, att) \
    Xpm_Create_దత్తాంశం_From_Pixmap(dpy, దత్తాంశం, pix, ముసుగు, att)

#అయితే_ముగింపు /* XPM_NUMBERS */
#అయితే_ముగింపు
