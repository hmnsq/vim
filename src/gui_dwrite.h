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

#నిర్వచితం_కాకపోతే GUI_DWRITE_H
#నిర్వచించు GUI_DWRITE_H

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం DWriteContext DWriteContext;

రకనిర్వచనం నిర్మాణం DWriteRenderingParams {
    దశాంశం gamma;
    దశాంశం enhancedContrast;
    దశాంశం clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    పూర్ణాంకం pixelGeometry;
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
    పూర్ణాంకం rendering_బహుళకం;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    పూర్ణాంకం పాఠ్యం_Antialias_బహుళకం;
} DWriteRenderingParams;

#నిర్వచించు DWRITE_MAX_FONT_FEATURES 32

రకనిర్వచనం నిర్మాణం DWriteFontFeature {
    గుర్తులేని పూర్ణాంకం ట్యాగ్;	    // OpenType feature tag (4 bytes)
    గుర్తులేని పూర్ణాంకం parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

శూన్య DWrite_Init(శూన్య);
శూన్య DWrite_Final(శూన్య);

DWriteContext *DWriteContext_Open(శూన్య);
శూన్య DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, స్థిరరాశి RECT *rect);
శూన్య DWrite_Context_సమితి_Font(DWriteContext *ctx, HFONT hFont);
శూన్య DWrite_Context_Draw_పాఠ్యం(
	DWriteContext *ctx,
	స్థిరరాశి WCHAR *పాఠ్యం,
	పూర్ణాంకం len,
	పూర్ణాంకం x,
	పూర్ణాంకం y,
	పూర్ణాంకం w,
	పూర్ణాంకం h,
	పూర్ణాంకం cell_వెడల్పు_2,
	COLORREF color,
	UINT fuOptions,
	స్థిరరాశి RECT *lprc,
	స్థిరరాశి INT *lpDx);
శూన్య DWriteContext_FillRect(DWriteContext *ctx, స్థిరరాశి RECT *rc, COLORREF color);
శూన్య DWrite_Context_Draw_పంక్తి(DWriteContext *ctx, పూర్ణాంకం x1, పూర్ణాంకం y1, పూర్ణాంకం x2, పూర్ణాంకం y2,
	COLORREF color);
శూన్య DWrite_Context_సమితి_Pixel(DWriteContext *ctx, పూర్ణాంకం x, పూర్ణాంకం y, COLORREF color);
శూన్య DWriteContext_Scroll(DWriteContext *ctx, పూర్ణాంకం x, పూర్ణాంకం y, స్థిరరాశి RECT *rc);
శూన్య DWriteContext_Flush(DWriteContext *ctx);
శూన్య DWriteContext_Close(DWriteContext *ctx);

శూన్య DWrite_Context_సమితి_Rendering_Params(
	DWriteContext *ctx,
	స్థిరరాశి DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

శూన్య DWrite_Context_సమితి_Font_Features(
	DWriteContext *ctx,
	స్థిరరాశి DWriteFontFeature *features,
	పూర్ణాంకం లెక్క);

#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు
#అయితే_ముగింపు/*GUI_DWRITE_H*/
