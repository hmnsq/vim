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

#اگر_غیر_تعریف_شدہ GUI_DWRITE_H
#تعریف_کرو GUI_DWRITE_H

#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم

قسمتعریف ساخت DWriteContext DWriteContext;

قسمتعریف ساخت DWriteRenderingParams {
    اعشاری gamma;
    اعشاری enhancedContrast;
    اعشاری clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    صحیح pixelGeometry;
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
    صحیح rendering_منوال;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    صحیح متن_Antialias_منوال;
} DWriteRenderingParams;

#تعریف_کرو DWRITE_MAX_FONT_FEATURES 32

قسمتعریف ساخت DWriteFontFeature {
    غیرعلامتی صحیح لیبل;	    // OpenType feature tag (4 bytes)
    غیرعلامتی صحیح parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

خالی DWrite_Init(خالی);
خالی DWrite_Final(خالی);

DWriteContext *DWriteContext_Open(خالی);
خالی DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect);
خالی DWrite_Context_سیٹ_Font(DWriteContext *ctx, HFONT hFont);
خالی DWrite_Context_Draw_متن(
	DWriteContext *ctx,
	ثابت WCHAR *متن,
	صحیح len,
	صحیح x,
	صحیح y,
	صحیح w,
	صحیح h,
	صحیح cell_چوڑائی_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx);
خالی DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color);
خالی DWrite_Context_Draw_سطر(DWriteContext *ctx, صحیح x1, صحیح y1, صحیح x2, صحیح y2,
	COLORREF color);
خالی DWrite_Context_سیٹ_Pixel(DWriteContext *ctx, صحیح x, صحیح y, COLORREF color);
خالی DWriteContext_Scroll(DWriteContext *ctx, صحیح x, صحیح y, ثابت RECT *rc);
خالی DWriteContext_Flush(DWriteContext *ctx);
خالی DWriteContext_Close(DWriteContext *ctx);

خالی DWrite_Context_سیٹ_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

خالی DWrite_Context_سیٹ_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحیح گنتی);

#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم
#اگر_ختم/*GUI_DWRITE_H*/
