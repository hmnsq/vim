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
خارجي "C" {
#endif

تعريفنوع بنية DWriteContext DWriteContext;

تعريفنوع بنية DWriteRenderingParams {
    عائم gamma;
    عائم enhancedContrast;
    عائم clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    صحيح pixelGeometry;
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
    صحيح renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    صحيح textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

تعريفنوع بنية DWriteFontFeature {
    unsigned صحيح tag;	    // OpenType feature tag (4 bytes)
    unsigned صحيح parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

فراغ DWrite_Init(فراغ);
فراغ DWrite_Final(فراغ);

DWriteContext *DWriteContext_Open(فراغ);
فراغ DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect);
فراغ DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
فراغ DWriteContext_DrawText(
	DWriteContext *ctx,
	ثابت WCHAR *text,
	صحيح len,
	صحيح x,
	صحيح y,
	صحيح w,
	صحيح h,
	صحيح cellWidth,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx);
فراغ DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color);
فراغ DWrite_Context_Draw_سطر(DWriteContext *ctx, صحيح x1, صحيح y1, صحيح x2, صحيح y2,
	COLORREF color);
فراغ DWriteContext_SetPixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color);
فراغ DWriteContext_Scroll(DWriteContext *ctx, صحيح x, صحيح y, ثابت RECT *rc);
فراغ DWriteContext_Flush(DWriteContext *ctx);
فراغ DWriteContext_Close(DWriteContext *ctx);

فراغ DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

فراغ DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح تعداد_2);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
