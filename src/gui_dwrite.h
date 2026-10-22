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
வெளி "C" {
#endif

வகைவரை கட்டமைப்பு DWriteContext DWriteContext;

வகைவரை கட்டமைப்பு DWriteRenderingParams {
    தசம gamma;
    தசம enhancedContrast;
    தசம clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    முழு pixelGeometry;
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
    முழு renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    முழு textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

வகைவரை கட்டமைப்பு DWriteFontFeature {
    unsigned முழு tag;	    // OpenType feature tag (4 bytes)
    unsigned முழு parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

வெறுமை DWrite_Init(வெறுமை);
வெறுமை DWrite_Final(வெறுமை);

DWriteContext *DWriteContext_Open(வெறுமை);
வெறுமை DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, மாறிலி RECT *rect);
வெறுமை DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
வெறுமை DWriteContext_DrawText(
	DWriteContext *ctx,
	மாறிலி WCHAR *text,
	முழு len,
	முழு x,
	முழு y,
	முழு w,
	முழு h,
	முழு cellWidth,
	COLORREF color,
	UINT fuOptions,
	மாறிலி RECT *lprc,
	மாறிலி INT *lpDx);
வெறுமை DWriteContext_FillRect(DWriteContext *ctx, மாறிலி RECT *rc, COLORREF color);
வெறுமை DWrite_Context_Draw_வரி(DWriteContext *ctx, முழு x1, முழு y1, முழு x2, முழு y2,
	COLORREF color);
வெறுமை DWriteContext_SetPixel(DWriteContext *ctx, முழு x, முழு y, COLORREF color);
வெறுமை DWriteContext_Scroll(DWriteContext *ctx, முழு x, முழு y, மாறிலி RECT *rc);
வெறுமை DWriteContext_Flush(DWriteContext *ctx);
வெறுமை DWriteContext_Close(DWriteContext *ctx);

வெறுமை DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	மாறிலி DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

வெறுமை DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	மாறிலி DWriteFontFeature *features,
	முழு மொத்தம்);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
