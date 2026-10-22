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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GUI_DWRITE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GUI_DWRITE_H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ DWriteContext DWriteContext;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ DWriteRenderingParams {
    ਦਸ਼ਮਲਵ gamma;
    ਦਸ਼ਮਲਵ enhancedContrast;
    ਦਸ਼ਮਲਵ clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    ਪੂਰਨ pixelGeometry;
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
    ਪੂਰਨ rendering_ਬਹੁਲਕ;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    ਪੂਰਨ ਪਾਠ_Antialias_ਬਹੁਲਕ;
} DWriteRenderingParams;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DWRITE_MAX_FONT_FEATURES 32

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ DWriteFontFeature {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਟੈਗ;	    // OpenType feature tag (4 bytes)
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

ਖਾਲੀ DWrite_Init(ਖਾਲੀ);
ਖਾਲੀ DWrite_Final(ਖਾਲੀ);

DWriteContext *DWriteContext_Open(ਖਾਲੀ);
ਖਾਲੀ DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ਅਚਲ RECT *rect);
ਖਾਲੀ DWrite_Context_ਸੈੱਟ_Font(DWriteContext *ctx, HFONT hFont);
ਖਾਲੀ DWrite_Context_Draw_ਪਾਠ(
	DWriteContext *ctx,
	ਅਚਲ WCHAR *ਪਾਠ,
	ਪੂਰਨ len,
	ਪੂਰਨ x,
	ਪੂਰਨ y,
	ਪੂਰਨ w,
	ਪੂਰਨ h,
	ਪੂਰਨ cell_ਚੌੜਾਈ_2,
	COLORREF color,
	UINT fuOptions,
	ਅਚਲ RECT *lprc,
	ਅਚਲ INT *lpDx);
ਖਾਲੀ DWriteContext_FillRect(DWriteContext *ctx, ਅਚਲ RECT *rc, COLORREF color);
ਖਾਲੀ DWrite_Context_Draw_ਸਤਰ(DWriteContext *ctx, ਪੂਰਨ x1, ਪੂਰਨ y1, ਪੂਰਨ x2, ਪੂਰਨ y2,
	COLORREF color);
ਖਾਲੀ DWrite_Context_ਸੈੱਟ_Pixel(DWriteContext *ctx, ਪੂਰਨ x, ਪੂਰਨ y, COLORREF color);
ਖਾਲੀ DWriteContext_Scroll(DWriteContext *ctx, ਪੂਰਨ x, ਪੂਰਨ y, ਅਚਲ RECT *rc);
ਖਾਲੀ DWriteContext_Flush(DWriteContext *ctx);
ਖਾਲੀ DWriteContext_Close(DWriteContext *ctx);

ਖਾਲੀ DWrite_Context_ਸੈੱਟ_Rendering_Params(
	DWriteContext *ctx,
	ਅਚਲ DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

ਖਾਲੀ DWrite_Context_ਸੈੱਟ_Font_Features(
	DWriteContext *ctx,
	ਅਚਲ DWriteFontFeature *features,
	ਪੂਰਨ ਗਿਣਤੀ_2);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ/*GUI_DWRITE_H*/
