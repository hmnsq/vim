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
ବାହ୍ୟ "C" {
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା DWriteContext DWriteContext;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା DWriteRenderingParams {
    ଦଶମିକ gamma;
    ଦଶମିକ enhancedContrast;
    ଦଶମିକ clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pixelGeometry;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା DWriteFontFeature {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tag;	    // OpenType feature tag (4 bytes)
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

ଶୂନ୍ୟ DWrite_Init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ DWrite_Final(ଶୂନ୍ୟ);

DWriteContext *DWriteContext_Open(ଶୂନ୍ୟ);
ଶୂନ୍ୟ DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ଅଚଳ RECT *rect);
ଶୂନ୍ୟ DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
ଶୂନ୍ୟ DWriteContext_DrawText(
	DWriteContext *ctx,
	ଅଚଳ WCHAR *text,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା h,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cellWidth,
	COLORREF color,
	UINT fuOptions,
	ଅଚଳ RECT *lprc,
	ଅଚଳ INT *lpDx);
ଶୂନ୍ୟ DWriteContext_FillRect(DWriteContext *ctx, ଅଚଳ RECT *rc, COLORREF color);
ଶୂନ୍ୟ DWrite_Context_Draw_ଧାଡ଼ି(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y2,
	COLORREF color);
ଶୂନ୍ୟ DWriteContext_SetPixel(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, COLORREF color);
ଶୂନ୍ୟ DWriteContext_Scroll(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ଅଚଳ RECT *rc);
ଶୂନ୍ୟ DWriteContext_Flush(DWriteContext *ctx);
ଶୂନ୍ୟ DWriteContext_Close(DWriteContext *ctx);

ଶୂନ୍ୟ DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ଅଚଳ DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

ଶୂନ୍ୟ DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ଅଚଳ DWriteFontFeature *features,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
