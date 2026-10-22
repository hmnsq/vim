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
बाह्य "C" {
#endif

प्रकारनिर्देश संरचना DWriteContext DWriteContext;

प्रकारनिर्देश संरचना DWriteRenderingParams {
    भिन्न gamma;
    भिन्न enhancedContrast;
    भिन्न clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    पूर्णाङ्क pixelGeometry;
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
    पूर्णाङ्क renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    पूर्णाङ्क textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

प्रकारनिर्देश संरचना DWriteFontFeature {
    unsigned पूर्णाङ्क tag;	    // OpenType feature tag (4 bytes)
    unsigned पूर्णाङ्क parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

शून्य DWrite_Init(शून्य);
शून्य DWrite_Final(शून्य);

DWriteContext *DWriteContext_Open(शून्य);
शून्य DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ध्रुव RECT *rect);
शून्य DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
शून्य DWriteContext_DrawText(
	DWriteContext *ctx,
	ध्रुव WCHAR *text,
	पूर्णाङ्क len,
	पूर्णाङ्क x,
	पूर्णाङ्क y,
	पूर्णाङ्क w,
	पूर्णाङ्क h,
	पूर्णाङ्क cellWidth,
	COLORREF color,
	UINT fuOptions,
	ध्रुव RECT *lprc,
	ध्रुव INT *lpDx);
शून्य DWriteContext_FillRect(DWriteContext *ctx, ध्रुव RECT *rc, COLORREF color);
शून्य DWrite_Context_Draw_पङ्क्तिः(DWriteContext *ctx, पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2,
	COLORREF color);
शून्य DWriteContext_SetPixel(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color);
शून्य DWriteContext_Scroll(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc);
शून्य DWriteContext_Flush(DWriteContext *ctx);
शून्य DWriteContext_Close(DWriteContext *ctx);

शून्य DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ध्रुव DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

शून्य DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ध्रुव DWriteFontFeature *features,
	पूर्णाङ्क गणनम्);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
