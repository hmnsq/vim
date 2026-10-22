/* vi:set ts=8 sts=4 sw=4 noet: */
/*
 * Author: MURAOKA Taro <koron.kaoriya@gmail.com>
 *
 * Contributors:
 *  - Ken Takata
 *  - Yasuhiro Matsumoto
 *
 * Copyright (C) 2013 MURAOKA Taro <koron.kaoriya@gmail.com>
 * THIS FILE IS DISTRIBUTED UNDER THE VIM LICENSE.
 */

#ifndef GUI_DWRITE_H
#define GUI_DWRITE_H

#ifdef __cplusplus
বাহ্যিক "C" {
#endif

প্রকারলেখ কাঠামো DWriteContext DWriteContext;

প্রকারলেখ কাঠামো DWriteRenderingParams {
    দশমিক gamma;
    দশমিক enhancedContrast;
    দশমিক clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    পূর্ণসংখ্যা pixelGeometry;
    /*
     * renderingMode:
     *	0 - DWRITE_RENDERING_MODE_DEFAULT
     *	1 - DWRITE_RENDERING_MODE_ALIASED
     *	2 - DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC
     *	3 - DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL
     *	4 - DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL
     *	5 - DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC
     *	6 - DWRITE_RENDERING_MODE_OUTLINE
     */
    পূর্ণসংখ্যা renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    পূর্ণসংখ্যা textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

প্রকারলেখ কাঠামো DWriteFontFeature {
    unsigned পূর্ণসংখ্যা tag;	    // OpenType feature tag (4 bytes)
    unsigned পূর্ণসংখ্যা parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

শূন্য DWrite_Init(শূন্য);
শূন্য DWrite_Final(শূন্য);

DWriteContext *DWriteContext_Open(শূন্য);
শূন্য DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ধ্রুবক RECT *rect);
শূন্য DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
শূন্য DWriteContext_DrawText(
	DWriteContext *ctx,
	ধ্রুবক WCHAR *text,
	পূর্ণসংখ্যা len,
	পূর্ণসংখ্যা x,
	পূর্ণসংখ্যা y,
	পূর্ণসংখ্যা w,
	পূর্ণসংখ্যা h,
	পূর্ণসংখ্যা cellWidth,
	COLORREF color,
	UINT fuOptions,
	ধ্রুবক RECT *lprc,
	ধ্রুবক INT *lpDx);
শূন্য DWriteContext_FillRect(DWriteContext *ctx, ধ্রুবক RECT *rc, COLORREF color);
শূন্য DWrite_Context_Draw_সারি(DWriteContext *ctx, পূর্ণসংখ্যা x1, পূর্ণসংখ্যা y1, পূর্ণসংখ্যা x2, পূর্ণসংখ্যা y2,
	COLORREF color);
শূন্য DWriteContext_SetPixel(DWriteContext *ctx, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, COLORREF color);
শূন্য DWriteContext_Scroll(DWriteContext *ctx, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, ধ্রুবক RECT *rc);
শূন্য DWriteContext_Flush(DWriteContext *ctx);
শূন্য DWriteContext_Close(DWriteContext *ctx);

শূন্য DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ধ্রুবক DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

শূন্য DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ধ্রুবক DWriteFontFeature *features,
	পূর্ণসংখ্যা গুনতি);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
