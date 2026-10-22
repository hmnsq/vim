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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GUI_DWRITE_H
#ವ್ಯಾಖ್ಯಾನಿಸು GUI_DWRITE_H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ DWriteContext DWriteContext;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ DWriteRenderingParams {
    ದಶಮಾಂಶ gamma;
    ದಶಮಾಂಶ enhancedContrast;
    ದಶಮಾಂಶ clearTypeLevel;
    /*
     * pixelGeometry:
     *	0 - DWRITE_PIXEL_GEOMETRY_FLAT
     *	1 - DWRITE_PIXEL_GEOMETRY_RGB
     *	2 - DWRITE_PIXEL_GEOMETRY_BGR
     */
    ಪೂರ್ಣಾಂಕ pixelGeometry;
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
    ಪೂರ್ಣಾಂಕ rendering_ಬಹುಲಕ;
    /*
     * antialiasMode:
     *	0 - D2D1_TEXT_ANTIALIAS_MODE_DEFAULT
     *	1 - D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE
     *	2 - D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
     *	3 - D2D1_TEXT_ANTIALIAS_MODE_ALIASED
     */
    ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_Antialias_ಬಹುಲಕ;
} DWriteRenderingParams;

#ವ್ಯಾಖ್ಯಾನಿಸು DWRITE_MAX_FONT_FEATURES 32

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ DWriteFontFeature {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಟ್ಯಾಗ್;	    // OpenType feature tag (4 bytes)
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ parameter; // Feature parameter (0 = disable, 1 = enable)
} DWriteFontFeature;

ಶೂನ್ಯ DWrite_Init(ಶೂನ್ಯ);
ಶೂನ್ಯ DWrite_Final(ಶೂನ್ಯ);

DWriteContext *DWriteContext_Open(ಶೂನ್ಯ);
ಶೂನ್ಯ DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ಸ್ಥಿರಾಂಕ RECT *rect);
ಶೂನ್ಯ DWrite_Context_ಗಣ_Font(DWriteContext *ctx, HFONT hFont);
ಶೂನ್ಯ DWrite_Context_Draw_ಪಠ್ಯ(
	DWriteContext *ctx,
	ಸ್ಥಿರಾಂಕ WCHAR *ಪಠ್ಯ,
	ಪೂರ್ಣಾಂಕ len,
	ಪೂರ್ಣಾಂಕ x,
	ಪೂರ್ಣಾಂಕ y,
	ಪೂರ್ಣಾಂಕ w,
	ಪೂರ್ಣಾಂಕ h,
	ಪೂರ್ಣಾಂಕ cell_ಅಗಲ_2,
	COLORREF color,
	UINT fuOptions,
	ಸ್ಥಿರಾಂಕ RECT *lprc,
	ಸ್ಥಿರಾಂಕ INT *lpDx);
ಶೂನ್ಯ DWriteContext_FillRect(DWriteContext *ctx, ಸ್ಥಿರಾಂಕ RECT *rc, COLORREF color);
ಶೂನ್ಯ DWrite_Context_Draw_ಸಾಲು(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x1, ಪೂರ್ಣಾಂಕ y1, ಪೂರ್ಣಾಂಕ x2, ಪೂರ್ಣಾಂಕ y2,
	COLORREF color);
ಶೂನ್ಯ DWrite_Context_ಗಣ_Pixel(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, COLORREF color);
ಶೂನ್ಯ DWriteContext_Scroll(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಸ್ಥಿರಾಂಕ RECT *rc);
ಶೂನ್ಯ DWriteContext_Flush(DWriteContext *ctx);
ಶೂನ್ಯ DWriteContext_Close(DWriteContext *ctx);

ಶೂನ್ಯ DWrite_Context_ಗಣ_Rendering_Params(
	DWriteContext *ctx,
	ಸ್ಥಿರಾಂಕ DWriteRenderingParams *params);

DWriteRenderingParams *DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params);

ಶೂನ್ಯ DWrite_Context_ಗಣ_Font_Features(
	DWriteContext *ctx,
	ಸ್ಥಿರಾಂಕ DWriteFontFeature *features,
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ/*GUI_DWRITE_H*/
