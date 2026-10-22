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

प्रकारव्याख्या संरचना DWriteContext DWriteContext;

प्रकारव्याख्या संरचना DWriteRenderingParams {
    दशांश gamma;
    दशांश enhancedContrast;
    दशांश clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    पूर्णांक pixelGeometry;
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
    पूर्णांक renderingMode;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    पूर्णांक textAntialiasMode;
} DWriteRenderingParams;

#define DWRITE_MAX_FONT_FEATURES 32

प्रकारव्याख्या संरचना DWriteFontFeature {
    अचिन्हित पूर्णांक tag;	    // OpenType feature tag (4 bytes)
    अचिन्हित पूर्णांक parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

शून्य DWrite_Init(शून्य);
शून्य DWrite_Final(शून्य);

DWriteContext *DWriteContext_Open(शून्य);
शून्य DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, स्थिरांक RECT *rect);
शून्य DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont);
शून्य DWriteContext_DrawText(
	DWriteContext *ctx,
	स्थिरांक WCHAR *text,
	पूर्णांक len,
	पूर्णांक x,
	पूर्णांक y,
	पूर्णांक w,
	पूर्णांक h,
	पूर्णांक cellWidth,
	COLORREF color,
	UINT fuOptions,
	स्थिरांक RECT *lprc,
	स्थिरांक INT *lpDx);
शून्य DWriteContext_FillRect(DWriteContext *ctx, स्थिरांक RECT *rc, COLORREF color);
शून्य DWrite_Context_Draw_ओळ(DWriteContext *ctx, पूर्णांक x1, पूर्णांक y1, पूर्णांक x2, पूर्णांक y2,
	COLORREF color);
शून्य DWriteContext_SetPixel(DWriteContext *ctx, पूर्णांक x, पूर्णांक y, COLORREF color);
शून्य DWriteContext_Scroll(DWriteContext *ctx, पूर्णांक x, पूर्णांक y, स्थिरांक RECT *rc);
शून्य DWriteContext_Flush(DWriteContext *ctx);
शून्य DWriteContext_Close(DWriteContext *ctx);

शून्य DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	स्थिरांक DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

शून्य DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	स्थिरांक DWriteFontFeature *features,
	पूर्णांक मोजणी);

#ifdef __cplusplus
}
#endif
#endif/*GUI_DWRITE_H*/
