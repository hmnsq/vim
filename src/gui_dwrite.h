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
ബാഹ്യ "C" {
#endif

തരനിർവചനം ഘടന DWriteContext DWriteContext;

തരനിർവചനം ഘടന DWriteRenderingParams {
    ദശാംശം gamma;
    ദശാംശം enhancedContrast;
    ദശാംശം clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    പൂർണ്ണസംഖ്യ pixelGeometry;
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
    പൂർണ്ണസംഖ്യ renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    പൂർണ്ണസംഖ്യ textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

തരനിർവചനം ഘടന DWriteFontFeature {
    unsigned പൂർണ്ണസംഖ്യ tag;	    // OpenType feature tag (4 bytes)
    unsigned പൂർണ്ണസംഖ്യ parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

ശൂന്യം DWrite_Init(ശൂന്യം);
ശൂന്യം DWrite_Final(ശൂന്യം);

DWriteContext *DWriteContext_Open(ശൂന്യം);
ശൂന്യം DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, സ്ഥിരം RECT *rect);
ശൂന്യം DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
ശൂന്യം DWriteContext_DrawText(
	DWriteContext *ctx,
	സ്ഥിരം WCHAR *text,
	പൂർണ്ണസംഖ്യ len,
	പൂർണ്ണസംഖ്യ x,
	പൂർണ്ണസംഖ്യ y,
	പൂർണ്ണസംഖ്യ w,
	പൂർണ്ണസംഖ്യ h,
	പൂർണ്ണസംഖ്യ cellWidth,
	COLORREF color,
	UINT fuOptions,
	സ്ഥിരം RECT *lprc,
	സ്ഥിരം INT *lpDx);
ശൂന്യം DWriteContext_FillRect(DWriteContext *ctx, സ്ഥിരം RECT *rc, COLORREF color);
ശൂന്യം DWrite_Context_Draw_വരി(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x1, പൂർണ്ണസംഖ്യ y1, പൂർണ്ണസംഖ്യ x2, പൂർണ്ണസംഖ്യ y2,
	COLORREF color);
ശൂന്യം DWriteContext_SetPixel(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, COLORREF color);
ശൂന്യം DWriteContext_Scroll(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, സ്ഥിരം RECT *rc);
ശൂന്യം DWriteContext_Flush(DWriteContext *ctx);
ശൂന്യം DWriteContext_Close(DWriteContext *ctx);

ശൂന്യം DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	സ്ഥിരം DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

ശൂന്യം DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	സ്ഥിരം DWriteFontFeature *features,
	പൂർണ്ണസംഖ്യ എണ്ണം);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
