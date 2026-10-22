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
extern "C" {
#endif

typedef sa_nracanaa DWriteContext DWriteContext;

typedef sa_nracanaa DWriteRenderingParams {
    float gamma;
    float enhancedContrast;
    float clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    පූර්ණ_සංඛ්‍යා pixelGeometry;
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
    පූර්ණ_සංඛ්‍යා renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    පූර්ණ_සංඛ්‍යා textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

typedef sa_nracanaa DWriteFontFeature {
    unsigned පූර්ණ_සංඛ්‍යා tag;	    // OpenType feature tag (4 bytes)
    unsigned පූර්ණ_සංඛ්‍යා parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

void DWrite_Init(void);
void DWrite_Final(void);

DWriteContext *DWriteContext_Open(void);
void DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, const RECT *rect);
void DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
void DWriteContext_DrawText(
	DWriteContext *ctx,
	const WCHAR *text,
	පූර්ණ_සංඛ්‍යා len,
	පූර්ණ_සංඛ්‍යා x,
	පූර්ණ_සංඛ්‍යා y,
	පූර්ණ_සංඛ්‍යා w,
	පූර්ණ_සංඛ්‍යා h,
	පූර්ණ_සංඛ්‍යා cellWidth,
	COLORREF color,
	UINT fuOptions,
	const RECT *lprc,
	const INT *lpDx);
void DWriteContext_FillRect(DWriteContext *ctx, const RECT *rc, COLORREF color);
void DWriteContext_DrawLine(DWriteContext *ctx, පූර්ණ_සංඛ්‍යා x1, පූර්ණ_සංඛ්‍යා y1, පූර්ණ_සංඛ්‍යා x2, පූර්ණ_සංඛ්‍යා y2,
	COLORREF color);
void DWriteContext_SetPixel(DWriteContext *ctx, පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y, COLORREF color);
void DWriteContext_Scroll(DWriteContext *ctx, පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y, const RECT *rc);
void DWriteContext_Flush(DWriteContext *ctx);
void DWriteContext_Close(DWriteContext *ctx);

void DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	const DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

void DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	const DWriteFontFeature *features,
	පූර්ණ_සංඛ්‍යා count);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
