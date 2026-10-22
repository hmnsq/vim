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
બાહ્ય "C" {
#endif

પ્રકારવ્યાખ્યા સંરચના DWriteContext DWriteContext;

પ્રકારવ્યાખ્યા સંરચના DWriteRenderingParams {
    દશાંશ gamma;
    દશાંશ enhancedContrast;
    દશાંશ clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    પૂર્ણાંક pixelGeometry;
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
    પૂર્ણાંક renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    પૂર્ણાંક textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

પ્રકારવ્યાખ્યા સંરચના DWriteFontFeature {
    unsigned પૂર્ણાંક tag;	    // OpenType feature tag (4 bytes)
    unsigned પૂર્ણાંક parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

શૂન્ય DWrite_Init(શૂન્ય);
શૂન્ય DWrite_Final(શૂન્ય);

DWriteContext *DWriteContext_Open(શૂન્ય);
શૂન્ય DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, અચલ RECT *rect);
શૂન્ય DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
શૂન્ય DWriteContext_DrawText(
	DWriteContext *ctx,
	અચલ WCHAR *text,
	પૂર્ણાંક len,
	પૂર્ણાંક x,
	પૂર્ણાંક y,
	પૂર્ણાંક w,
	પૂર્ણાંક h,
	પૂર્ણાંક cellWidth,
	COLORREF color,
	UINT fuOptions,
	અચલ RECT *lprc,
	અચલ INT *lpDx);
શૂન્ય DWriteContext_FillRect(DWriteContext *ctx, અચલ RECT *rc, COLORREF color);
શૂન્ય DWrite_Context_Draw_લીટી(DWriteContext *ctx, પૂર્ણાંક x1, પૂર્ણાંક y1, પૂર્ણાંક x2, પૂર્ણાંક y2,
	COLORREF color);
શૂન્ય DWriteContext_SetPixel(DWriteContext *ctx, પૂર્ણાંક x, પૂર્ણાંક y, COLORREF color);
શૂન્ય DWriteContext_Scroll(DWriteContext *ctx, પૂર્ણાંક x, પૂર્ણાંક y, અચલ RECT *rc);
શૂન્ય DWriteContext_Flush(DWriteContext *ctx);
શૂન્ય DWriteContext_Close(DWriteContext *ctx);

શૂન્ય DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	અચલ DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

શૂન્ય DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	અચલ DWriteFontFeature *features,
	પૂર્ણાંક ગણતરી);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
