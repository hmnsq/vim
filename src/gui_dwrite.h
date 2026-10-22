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
بیرونی "C" {
#endif

تعریفنوع ساختار DWriteContext DWriteContext;

تعریفنوع ساختار DWriteRenderingParams {
    اعشاری gamma;
    اعشاری enhancedContrast;
    اعشاری clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    صحیح pixelGeometry;
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
    صحیح renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    صحیح textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

تعریفنوع ساختار DWriteFontFeature {
    unsigned صحیح tag;	    // OpenType feature tag (4 bytes)
    unsigned صحیح parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

تهی DWrite_Init(تهی);
تهی DWrite_Final(تهی);

DWriteContext *DWriteContext_Open(تهی);
تهی DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect);
تهی DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
تهی DWriteContext_DrawText(
	DWriteContext *ctx,
	ثابت WCHAR *text,
	صحیح len,
	صحیح x,
	صحیح y,
	صحیح w,
	صحیح h,
	صحیح cellWidth,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx);
تهی DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color);
تهی DWrite_Context_Draw_سطر(DWriteContext *ctx, صحیح x1, صحیح y1, صحیح x2, صحیح y2,
	COLORREF color);
تهی DWriteContext_SetPixel(DWriteContext *ctx, صحیح x, صحیح y, COLORREF color);
تهی DWriteContext_Scroll(DWriteContext *ctx, صحیح x, صحیح y, ثابت RECT *rc);
تهی DWriteContext_Flush(DWriteContext *ctx);
تهی DWriteContext_Close(DWriteContext *ctx);

تهی DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

تهی DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحیح شمار);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
