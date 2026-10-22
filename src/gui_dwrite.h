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
ಬಾಹ್ಯ "C" {
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ DWriteContext DWriteContext;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ DWriteRenderingParams {
    ಭಿನ್ನ gamma;
    ಭಿನ್ನ enhancedContrast;
    ಭಿನ್ನ clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    ಪೂರ್ಣಾಙ್ಕ pixelGeometry;
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
    ಪೂರ್ಣಾಙ್ಕ renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    ಪೂರ್ಣಾಙ್ಕ textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ DWriteFontFeature {
    unsigned ಪೂರ್ಣಾಙ್ಕ tag;	    // OpenType feature tag (4 bytes)
    unsigned ಪೂರ್ಣಾಙ್ಕ parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

ಶೂನ್ಯ DWrite_Init(ಶೂನ್ಯ);
ಶೂನ್ಯ DWrite_Final(ಶೂನ್ಯ);

DWriteContext *DWriteContext_Open(ಶೂನ್ಯ);
ಶೂನ್ಯ DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ಧ್ರುವ RECT *rect);
ಶೂನ್ಯ DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
ಶೂನ್ಯ DWriteContext_DrawText(
	DWriteContext *ctx,
	ಧ್ರುವ WCHAR *text,
	ಪೂರ್ಣಾಙ್ಕ len,
	ಪೂರ್ಣಾಙ್ಕ x,
	ಪೂರ್ಣಾಙ್ಕ y,
	ಪೂರ್ಣಾಙ್ಕ w,
	ಪೂರ್ಣಾಙ್ಕ h,
	ಪೂರ್ಣಾಙ್ಕ cellWidth,
	COLORREF color,
	UINT fuOptions,
	ಧ್ರುವ RECT *lprc,
	ಧ್ರುವ INT *lpDx);
ಶೂನ್ಯ DWriteContext_FillRect(DWriteContext *ctx, ಧ್ರುವ RECT *rc, COLORREF color);
ಶೂನ್ಯ DWrite_Context_Draw_ಸಾಲು(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x1, ಪೂರ್ಣಾಙ್ಕ y1, ಪೂರ್ಣಾಙ್ಕ x2, ಪೂರ್ಣಾಙ್ಕ y2,
	COLORREF color);
ಶೂನ್ಯ DWriteContext_SetPixel(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, COLORREF color);
ಶೂನ್ಯ DWriteContext_Scroll(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಧ್ರುವ RECT *rc);
ಶೂನ್ಯ DWriteContext_Flush(DWriteContext *ctx);
ಶೂನ್ಯ DWriteContext_Close(DWriteContext *ctx);

ಶೂನ್ಯ DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ಧ್ರುವ DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

ಶೂನ್ಯ DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ಧ್ರುವ DWriteFontFeature *features,
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
