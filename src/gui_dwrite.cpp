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

#ವ್ಯಾಖ್ಯಾನಿಸು WIN32_LEAN_AND_ಸರಾಸರಿ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DYNAMIC_DIRECTX
# ಆದರೆ WINVER < 0x0600
#  error WINVER must be 0x0600 or above ಗಮ್ಯ use ನೇರ_Write(ನೇರ_X)
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <windows.h>
#ಅನ್ತರ್ಭಾವ <crtdbg.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <d2d1.h>
#ಅನ್ತರ್ಭಾವ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#ಆದರೆ defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define ಒಳ_reads(s)
# define ಒಳ_reads_opt(s)
# define _Maybenull_
# define ಹೊರಹೋಗು_writes(s)
# define ಹೊರಹೋಗು_writes_opt(s)
# define ಹೊರಹೋಗು_writes_ಗಮ್ಯ(x, y)
# define ಹೊರಹೋಗು_writes_ಗಮ್ಯ_opt(x, y)
# define _Outptr_
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include <dwrite.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "gui_dwrite.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_ಒಳ
# define __out		SAL_ಹೊರಹೋಗು
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ಇಲ್ಲದಿದ್ದರೆ
# define FINAL
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_DIRECTX
ಬಾಹ್ಯ "C" HINSTANCE vim_ತುಂಬು_Lib(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ಸ್ಥಿರಾಂಕ D2D1_FACTORY_OPTIONS *, ಶೂನ್ಯ **);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ಸ್ಥಿರ HINSTANCE hD2D1DLL = ಖಾಲಿ;
ಸ್ಥಿರ HINSTANCE hDWriteDLL = ಖಾಲಿ;

ಸ್ಥಿರ PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ಖಾಲಿ;
ಸ್ಥಿರ PD2D1CREATEFACTORY pD2D1CreateFactory = ಖಾಲಿ;
ಸ್ಥಿರ PDWRITECREATEFACTORY pDWriteCreateFactory = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#ವ್ಯಾಖ್ಯಾನಿಸು D2D1CreateFactory		(*pD2D1CreateFactory)
#ವ್ಯಾಖ್ಯಾನಿಸು DWriteCreateFactory		(*pDWriteCreateFactory)

    ಸ್ಥಿರ ಶೂನ್ಯ
unload(HINSTANCE &hinst)
{
    ಆದರೆ (hinst != ಖಾಲಿ)
    {
	FreeLibrary(hinst);
	hinst = ಖಾಲಿ;
    }
}
#ಆದರೆ_ಅಂತ್ಯ // DYNAMIC_DIRECTX

ಮಾದರಿ <ವರ್ಗ T> ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಸುರಕ್ಷಿತ_Release(T **ppT)
{
    ಆದರೆ (*ppT)
    {
	(*ppT)->Release();
	*ppT = ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ DWRITE_PIXEL_GEOMETRY
ಗಮ್ಯ_Pixel_Geometry(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
    ಆಯ್ಕೆ (ಮೌಲ್ಯ)
    {
	ಪೂರ್ವನಿಯೋಜಿತ:
	ಸ್ಥಿತಿ 0:
	    ಹಿಂತಿರುಗಿ DWRITE_PIXEL_GEOMETRY_FLAT;
	ಸ್ಥಿತಿ 1:
	    ಹಿಂತಿರುಗಿ DWRITE_PIXEL_GEOMETRY_RGB;
	ಸ್ಥಿತಿ 2:
	    ಹಿಂತಿರುಗಿ DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಮ್ಯ_Int(DWRITE_PIXEL_GEOMETRY ಮೌಲ್ಯ)
{
    ಆಯ್ಕೆ (ಮೌಲ್ಯ)
    {
	ಸ್ಥಿತಿ DWRITE_PIXEL_GEOMETRY_FLAT:
	    ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ DWRITE_PIXEL_GEOMETRY_RGB:
	    ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ DWRITE_PIXEL_GEOMETRY_BGR:
	    ಹಿಂತಿರುಗಿ 2;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ -1;
    }
}

    ಸ್ಥಿರ DWRITE_RENDERING_ಬಹುಲಕ
ಗಮ್ಯ_Rendering_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
    ಆಯ್ಕೆ (ಮೌಲ್ಯ)
    {
	ಪೂರ್ವನಿಯೋಜಿತ:
	ಸ್ಥಿತಿ 0:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_DEFAULT;
	ಸ್ಥಿತಿ 1:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_ALIASED;
	ಸ್ಥಿತಿ 2:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_GDI_CLASSIC;
	ಸ್ಥಿತಿ 3:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_GDI_NATURAL;
	ಸ್ಥಿತಿ 4:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_NATURAL;
	ಸ್ಥಿತಿ 5:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_NATURAL_ಸಮಮಿತ;
	ಸ್ಥಿತಿ 6:
	    ಹಿಂತಿರುಗಿ DWRITE_RENDERING_ಬಹುಲಕ_OUTLINE;
    }
}

    ಸ್ಥಿರ D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ
ಗಮ್ಯ_ಪಠ್ಯ_Antialias_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
    ಆಯ್ಕೆ (ಮೌಲ್ಯ)
    {
	ಪೂರ್ವನಿಯೋಜಿತ:
	ಸ್ಥಿತಿ 0:
	    ಹಿಂತಿರುಗಿ D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_DEFAULT;
	ಸ್ಥಿತಿ 1:
	    ಹಿಂತಿರುಗಿ D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_CLEARTYPE;
	ಸ್ಥಿತಿ 2:
	    ಹಿಂತಿರುಗಿ D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_GRAYSCALE;
	ಸ್ಥಿತಿ 3:
	    ಹಿಂತಿರುಗಿ D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_ALIASED;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಮ್ಯ_Int(DWRITE_RENDERING_ಬಹುಲಕ ಮೌಲ್ಯ)
{
    ಆಯ್ಕೆ (ಮೌಲ್ಯ)
    {
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_DEFAULT:
	    ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_ALIASED:
	    ಹಿಂತಿರುಗಿ 1;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_GDI_CLASSIC:
	    ಹಿಂತಿರುಗಿ 2;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_GDI_NATURAL:
	    ಹಿಂತಿರುಗಿ 3;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_NATURAL:
	    ಹಿಂತಿರುಗಿ 4;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_CLEARTYPE_NATURAL_ಸಮಮಿತ:
	    ಹಿಂತಿರುಗಿ 5;
	ಸ್ಥಿತಿ DWRITE_RENDERING_ಬಹುಲಕ_OUTLINE:
	    ಹಿಂತಿರುಗಿ 6;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ -1;
    }
}

ವರ್ಗ FontCache {
ಸಾರ್ವಜನಿಕ:
    ರಚನೆ ಅಂಶ_2 {
	HFONT              hFont;
	IDWrite_ಪಠ್ಯ_Format* p_ಪಠ್ಯ_Format;
	DWRITE_FONT_ತೂಕ font_ತೂಕ;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	ಅಂಶ_2() : hFont(ಖಾಲಿ), p_ಪಠ್ಯ_Format(ಖಾಲಿ), fontAscent(0.0f) {}
    };

ಖಾಸಗಿ:
    ಪೂರ್ಣಾಂಕ m_ಗಾತ್ರ;
    ಅಂಶ_2 *m_ವಸ್ತುಗಳು;

ಸಾರ್ವಜನಿಕ:
    FontCache(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = 2) :
	m_ಗಾತ್ರ(ಗಾತ್ರ_2),
	m_ವಸ್ತುಗಳು(ಹೊಸ ಅಂಶ_2[ಗಾತ್ರ_2])
    {
    }

    ~FontCache()
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < m_ಗಾತ್ರ; ++i)
	    ಸುರಕ್ಷಿತ_Release(&m_ವಸ್ತುಗಳು[i].p_ಪಠ್ಯ_Format);
	ಅಳಿಸು[] m_ವಸ್ತುಗಳು;
    }

    ತಾರ್ಕಿಕ get(HFONT hFont, ಅಂಶ_2 &ಅಂಶ)
    {
	ಪೂರ್ಣಾಂಕ n = ಕಂಡುಹಿಡಿ(hFont);
	ಆದರೆ (n < 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಂಶ = m_ವಸ್ತುಗಳು[n];
	slide(n);
	ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಂಶ_2& ಅಂಶ)
    {
	ಪೂರ್ಣಾಂಕ n = ಕಂಡುಹಿಡಿ(ಅಂಶ.hFont);
	ಆದರೆ (n < 0)
	    n = m_ಗಾತ್ರ - 1;
	ಆದರೆ (m_ವಸ್ತುಗಳು[n].p_ಪಠ್ಯ_Format != ಅಂಶ.p_ಪಠ್ಯ_Format)
	{
	    ಸುರಕ್ಷಿತ_Release(&m_ವಸ್ತುಗಳು[n].p_ಪಠ್ಯ_Format);
	    ಆದರೆ (ಅಂಶ.p_ಪಠ್ಯ_Format != ಖಾಲಿ)
		ಅಂಶ.p_ಪಠ್ಯ_Format->ಕೂಡಿಸು_Ref();
	}
	m_ವಸ್ತುಗಳು[n] = ಅಂಶ;
	slide(n);
    }

ಖಾಸಗಿ:
    ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ(HFONT hFont)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < m_ಗಾತ್ರ; ++i)
	{
	    ಆದರೆ (m_ವಸ್ತುಗಳು[i].hFont == hFont)
		ಹಿಂತಿರುಗಿ i;
	}
	ಹಿಂತಿರುಗಿ -1;
    }

    ಶೂನ್ಯ slide(ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ತುದಿ)
    {
	ಆದರೆ (ಮುಂದಿನ_ತುದಿ == 0)
	    ಹಿಂತಿರುಗಿ;
	ಅಂಶ_2 tmp = m_ವಸ್ತುಗಳು[ಮುಂದಿನ_ತುದಿ];
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಮುಂದಿನ_ತುದಿ - 1; i >= 0; --i)
	    m_ವಸ್ತುಗಳು[i + 1] = m_ವಸ್ತುಗಳು[i];
	m_ವಸ್ತುಗಳು[0] = tmp;
    }
};

ಎಣಿಕೆ Drawing_ಬಹುಲಕ {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ರಚನೆ DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_ಬಹುಲಕ mDMode;
    HDC mInteropHDC;
    ತಾರ್ಕಿಕ mDrawing;
    ತಾರ್ಕಿಕ mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_ಗುರಿ *mRT;
    ID2_D1_Gdi_Interop_Render_ಗುರಿ *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#ಆದರೆ_ಅಂತ್ಯ

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_ಪಠ್ಯ_Format *m_ಪಠ್ಯ_Format;
    DWRITE_FONT_ತೂಕ m_Font_ತೂಕ;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_ಗಾತ್ರ;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ m_ಪಠ್ಯ_Antialias_ಬಹುಲಕ;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    ಪೂರ್ಣಾಂಕ m_Font_Feature_ಎಣಿಕೆ;

    // METHODS

    DWriteContext();

    ವಾಸ್ತವಿಕ ~DWriteContext();

    HRESULT CreateDeviceResources();

    ಶೂನ್ಯ DiscardDeviceResources();

    HRESULT Create_ಪಠ್ಯ_Format_From_LOGFONT(ಸ್ಥಿರಾಂಕ LOGFONTW &logFont,
	    IDWrite_ಪಠ್ಯ_Format **pp_ಪಠ್ಯ_Format, FLOAT *pFontAscent);

    HRESULT ಗಣ_Font_By_LOGFONT(ಸ್ಥಿರಾಂಕ LOGFONTW &logFont);

    ಶೂನ್ಯ ಗಣ_Font(HFONT hFont);

    ಶೂನ್ಯ Rebind();

    ಶೂನ್ಯ BindDC(HDC hdc, ಸ್ಥಿರಾಂಕ RECT *rect);

    HRESULT ಗಣ_Drawing_ಬಹುಲಕ(Drawing_ಬಹುಲಕ ಬಹುಲಕ);

    ID2D1Brush* SolidBrush(COLORREF color);

    ಶೂನ್ಯ Draw_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ WCHAR *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ len,
	ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h, ಪೂರ್ಣಾಂಕ cell_ಅಗಲ_2, COLORREF color,
	UINT fuOptions, ಸ್ಥಿರಾಂಕ RECT *lprc, ಸ್ಥಿರಾಂಕ INT *lpDx);

    ಶೂನ್ಯ FillRect(ಸ್ಥಿರಾಂಕ RECT *rc, COLORREF color);

    ಶೂನ್ಯ Draw_ಸಾಲು(ಪೂರ್ಣಾಂಕ x1, ಪೂರ್ಣಾಂಕ y1, ಪೂರ್ಣಾಂಕ x2, ಪೂರ್ಣಾಂಕ y2, COLORREF color);

    ಶೂನ್ಯ ಗಣ_Pixel(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, COLORREF color);

    ಶೂನ್ಯ Scroll(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಸ್ಥಿರಾಂಕ RECT *rc);

    ಶೂನ್ಯ Flush();

    ಶೂನ್ಯ ಗಣ_Rendering_Params(
	    ಸ್ಥಿರಾಂಕ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    ಶೂನ್ಯ ಗಣ_Font_Features(ಸ್ಥಿರಾಂಕ DWriteFontFeature *features, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);
};

ವರ್ಗ Adjusted_Glyph_ಸತತ : ಸಾರ್ವಜನಿಕ DWRITE_GLYPH_ಸತತ
{
ಖಾಸಗಿ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

ಸಾರ್ವಜನಿಕ:
    Adjusted_Glyph_ಸತತ(
	    ಸ್ಥಿರಾಂಕ DWRITE_GLYPH_ಸತತ *glyph_ಸತತ,
	    FLOAT cell_ಅಗಲ_2,
	    FLOAT &accum,
	    ಸ್ಥಿರಾಂಕ INT *lpDx,
	    ಪೂರ್ಣಾಂಕ *glyph_ಸೂಚ್ಯಂಕ,
	    ಪೂರ್ಣಾಂಕ cch_ಪಠ್ಯ) :
	DWRITE_GLYPH_ಸತತ(*glyph_ಸತತ),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(ಹೊಸ FLOAT[glyph_ಸತತ->glyph_ಎಣಿಕೆ])
    {
	ಪ್ರತಿಜ್ಞಾ(cell_ಅಗಲ_2 != 0.0f);
	ಗಾಗಿ (UINT32 i = 0; i < glyph_ಸತತ->glyph_ಎಣಿಕೆ; ++i)
	{
	    FLOAT orig = glyph_ಸತತ->glyphAdvances[i];
	    FLOAT adjusted;

	    ಆದರೆ (*glyph_ಸೂಚ್ಯಂಕ < cch_ಪಠ್ಯ)
	    {
		adjusted = FLOAT(lpDx[*glyph_ಸೂಚ್ಯಂಕ]);
		(*glyph_ಸೂಚ್ಯಂಕ)++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		adjusted = adjust_ಗಮ್ಯ_Cell(orig, cell_ಅಗಲ_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_ಸತತ()
    {
	mAccum += mDelta;
	ಅಳಿಸು[] mAdjustedAdvances;
    }

    ಸ್ಥಿರ FLOAT adjust_ಗಮ್ಯ_Cell(FLOAT ಮೌಲ್ಯ, FLOAT cell_ಅಗಲ_2)
    {
	ಪೂರ್ಣಾಂಕ cell_ಎಣಿಕೆ = ಪೂರ್ಣಾಂಕ(floor(ಮೌಲ್ಯ / cell_ಅಗಲ_2 + 0.5f));
	ಆದರೆ (cell_ಎಣಿಕೆ < 1)
	    cell_ಎಣಿಕೆ = 1;
	ಹಿಂತಿರುಗಿ cell_ಎಣಿಕೆ * cell_ಅಗಲ_2;
    }
};

ರಚನೆ ಪಠ್ಯ_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_ಅಗಲ_2;
    ಸ್ಥಿರಾಂಕ INT *lpDx;
    ಪೂರ್ಣಾಂಕ cch_ಪಠ್ಯ;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT ಸ್ಥಾನಾಂತರ_X;
    ಪೂರ್ಣಾಂಕ glyph_ಸೂಚ್ಯಂಕ;
};

ವರ್ಗ ಪಠ್ಯ_Renderer FINAL : ಸಾರ್ವಜನಿಕ IDWrite_ಪಠ್ಯ_Renderer
{
ಸಾರ್ವಜನಿಕ:
    ಪಠ್ಯ_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_ಎಣಿಕೆ(0),
	pDWC_(pDWC)
    {
	ಕೂಡಿಸು_Ref();
    }

    // add "virtual" to avoid a compiler warning
    ವಾಸ್ತವಿಕ ~ಪಠ್ಯ_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ಹಿಂತಿರುಗಿ S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	__out DWRITE_ಮಾತೃಕೆ* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_ಮಾತೃಕೆ_3_X2_F*>(transform));
	ಹಿಂತಿರುಗಿ S_OK;
    }

    IFACEMETHOD(Get_Pixels_ಪ್ರತಿ_Dip)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	__out FLOAT* pixels_ಪ್ರತಿ_Dip)
    {
	ದಶಮಾಂಶ dpiX, ಬಳಸದ_2;
	pDWC_->mRT->GetDpi(&dpiX, &ಬಳಸದ_2);
	*pixels_ಪ್ರತಿ_Dip = dpiX / 96.0f;
	ಹಿಂತಿರುಗಿ S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	FLOAT baselineOriginX ಬಳಸದ,
	FLOAT baselineOriginY ಬಳಸದ,
	__in DWRITE_UNDERLINE ಸ್ಥಿರಾಂಕ* underline ಬಳಸದ,
	IUnknown* clientDrawingEffect ಬಳಸದ)
    {
	ಹಿಂತಿರುಗಿ E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	FLOAT baselineOriginX ಬಳಸದ,
	FLOAT baselineOriginY ಬಳಸದ,
	__in DWRITE_STRIKETHROUGH ಸ್ಥಿರಾಂಕ* strikethrough ಬಳಸದ,
	IUnknown* clientDrawingEffect ಬಳಸದ)
    {
	ಹಿಂತಿರುಗಿ E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext ಬಳಸದ,
	FLOAT originX ಬಳಸದ,
	FLOAT originY ಬಳಸದ,
	IDWriteInlineObject* inlineObject ಬಳಸದ,
	BOOL isSideways ಬಳಸದ,
	BOOL is_ಬಲ_ಗಮ್ಯ_ಎಡ ಬಳಸದ,
	IUnknown* clientDrawingEffect ಬಳಸದ)
    {
	ಹಿಂತಿರುಗಿ E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_ಸತತ)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_ಬಹುಲಕ measuring_ಬಹುಲಕ ಬಳಸದ,
	__in DWRITE_GLYPH_ಸತತ ಸ್ಥಿರಾಂಕ* glyph_ಸತತ,
	__in DWRITE_GLYPH_ಸತತ_DESCRIPTION ಸ್ಥಿರಾಂಕ* glyph_ಸತತ_Description ಬಳಸದ,
	IUnknown* clientDrawingEffect ಬಳಸದ)
    {
	ಪಠ್ಯ_Renderer_Context *context =
	    reinterpret_cast<ಪಠ್ಯ_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_ಸತತ adjusted_Glyph_ಸತತ(glyph_ಸತತ, context->cell_ಅಗಲ_2,
		context->ಸ್ಥಾನಾಂತರ_X, context->lpDx, &context->glyph_ಸೂಚ್ಯಂಕ,
		context->cch_ಪಠ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
	ಆದರೆ (pDWC_->mDWriteFactory2 != ಖಾಲಿ)
	{
	    IDWrite_Color_Glyph_ಸತತ_Enumerator *enumerator = ಖಾಲಿ;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_ಸತತ(
		baselineOriginX + context->ಸ್ಥಾನಾಂತರ_X,
		baselineOriginY,
		&adjusted_Glyph_ಸತತ,
		ಖಾಲಿ,
		DWRITE_MEASURING_ಬಹುಲಕ_GDI_NATURAL,
		ಖಾಲಿ,
		0,
		&enumerator);
	    ಆದರೆ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_ಸತತ = TRUE;
		enumerator->Move_ಮುಂದಿನ(&has_ಸತತ);
		ಆಗ (has_ಸತತ)
		{
		    ಸ್ಥಿರಾಂಕ DWRITE_COLOR_GLYPH_ಸತತ* color_Glyph_ಸತತ;
		    enumerator->Get_Current_ಸತತ(&color_Glyph_ಸತತ);

		    pDWC_->mBrush->ಗಣ_Color(color_Glyph_ಸತತ->ಸತತ_Color);
		    pDWC_->mRT->Draw_Glyph_ಸತತ(
			    D2D1::Point2F(
				color_Glyph_ಸತತ->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_ಸತತ->glyph_ಸತತ,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_ಬಹುಲಕ_NATURAL);
		    enumerator->Move_ಮುಂದಿನ(&has_ಸತತ);
		}
		ಸುರಕ್ಷಿತ_Release(&enumerator);
		ಹಿಂತಿರುಗಿ S_OK;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_ಸತತ(
		D2D1::Point2F(
		    baselineOriginX + context->ಸ್ಥಾನಾಂತರ_X,
		    fixedBaselineY),
		&adjusted_Glyph_ಸತತ,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_ಬಹುಲಕ_NATURAL);
	ಹಿಂತಿರುಗಿ S_OK;
    }

ಸಾರ್ವಜನಿಕ:
    IFACEMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, ಕೂಡಿಸು_Ref) ()
    {
	ಹಿಂತಿರುಗಿ InterlockedIncrement(&c_Ref_ಎಣಿಕೆ);
    }

    IFACEMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, Release) ()
    {
	ದೀರ್ಘ new_ಎಣಿಕೆ_2 = InterlockedDecrement(&c_Ref_ಎಣಿಕೆ);

	ಆದರೆ (new_ಎಣಿಕೆ_2 == 0)
	{
	    ಅಳಿಸು ಇದು;
	    ಹಿಂತಿರುಗಿ 0;
	}
	ಹಿಂತಿರುಗಿ new_ಎಣಿಕೆ_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ಸ್ಥಿರಾಂಕ& riid,
	ಶೂನ್ಯ** ppvObject)
    {
	ಆದರೆ (__uuidof(IDWrite_ಪಠ್ಯ_Renderer) == riid)
	{
	    *ppvObject = ಇದು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ಇದು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ಇದು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *ppvObject = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ E_FAIL;
	}

	ಹಿಂತಿರುಗಿ S_OK;
    }

ಖಾಸಗಿ:
    ದೀರ್ಘ c_Ref_ಎಣಿಕೆ;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ಖಾಲಿ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ಖಾಲಿ),
    mDrawing(ಸುಳ್ಳು),
    mFallbackDC(ಸುಳ್ಳು),
    mD2D1Factory(ಖಾಲಿ),
    mRT(ಖಾಲಿ),
    mGDIRT(ಖಾಲಿ),
    mBrush(ಖಾಲಿ),
    mBitmap(ಖಾಲಿ),
    mDWriteFactory(ಖಾಲಿ),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ಖಾಲಿ),
#ಆದರೆ_ಅಂತ್ಯ
    mGdiInterop(ಖಾಲಿ),
    mRenderingParams(ಖಾಲಿ),
    mFontCache(8),
    m_ಪಠ್ಯ_Format(ಖಾಲಿ),
    m_Font_ತೂಕ(DWRITE_FONT_ತೂಕ_ಸಾಮಾನ್ಯ),
    mFontStyle(DWRITE_FONT_STYLE_ಸಾಮಾನ್ಯ),
    m_Font_ಗಾತ್ರ(0.0f),
    mFontAscent(0.0f),
    m_ಪಠ್ಯ_Antialias_ಬಹುಲಕ(D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_DEFAULT),
    m_Font_Feature_ಎಣಿಕೆ(0)
{
    Zero_ಸ್ಮರಣೆ(mFontFeatures, ಗಾತ್ರ(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ಖಾಲಿ,
	    reinterpret_cast<ಶೂನ್ಯ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
    ಆದರೆ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->CreateRenderingParams(&mRenderingParams);
	_RPT2(_CRT_WARN, "CreateRenderingParams: hr=%p p=%p\n", hr,
		mRenderingParams);
    }
}

DWriteContext::~DWriteContext()
{
    ಸುರಕ್ಷಿತ_Release(&m_ಪಠ್ಯ_Format);
    ಸುರಕ್ಷಿತ_Release(&mRenderingParams);
    ಸುರಕ್ಷಿತ_Release(&mGdiInterop);
    ಸುರಕ್ಷಿತ_Release(&mDWriteFactory);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX_COLOR_EMOJI
    ಸುರಕ್ಷಿತ_Release(&mDWriteFactory2);
#ಆದರೆ_ಅಂತ್ಯ
    ಸುರಕ್ಷಿತ_Release(&mBitmap);
    ಸುರಕ್ಷಿತ_Release(&mBrush);
    ಸುರಕ್ಷಿತ_Release(&mGDIRT);
    ಸುರಕ್ಷಿತ_Release(&mRT);
    ಸುರಕ್ಷಿತ_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    ಆದರೆ (mRT != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ S_OK;

    D2_D1_RENDER_ಗುರಿ_PROPERTIES props = {
	D2_D1_RENDER_ಗುರಿ_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ಬಹುಲಕ_IGNORE },
	0, 0,
	D2_D1_RENDER_ಗುರಿ_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_ಗುರಿ(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    ಆದರೆ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_ಗುರಿ),
		reinterpret_cast<ಶೂನ್ಯ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    ಆದರೆ (SUCCEEDED(hr))
	Rebind();

    ಹಿಂತಿರುಗಿ hr;
}

    ಶೂನ್ಯ
DWriteContext::DiscardDeviceResources()
{
    ಸುರಕ್ಷಿತ_Release(&mBitmap);
    ಸುರಕ್ಷಿತ_Release(&mBrush);
    ಸುರಕ್ಷಿತ_Release(&mGDIRT);
    ಸುರಕ್ಷಿತ_Release(&mRT);
}

    HRESULT
DWriteContext::Create_ಪಠ್ಯ_Format_From_LOGFONT(ಸ್ಥಿರಾಂಕ LOGFONTW &logFont,
	IDWrite_ಪಠ್ಯ_Format **pp_ಪಠ್ಯ_Format, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWrite_ಪಠ್ಯ_Format *p_ಪಠ್ಯ_Format = ಖಾಲಿ;

    IDWriteFont *font = ಖಾಲಿ;
    IDWriteFontFamily *fontFamily = ಖಾಲಿ;
    IDWrite_Localized_ಅಕ್ಷರಸರಣಿಗಳು *localized_Family_ಹೆಸರುಗಳು = ಖಾಲಿ;
    ದಶಮಾಂಶ font_ಗಾತ್ರ = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = fontFamily->Get_Family_ಹೆಸರುಗಳು(&localized_Family_ಹೆಸರುಗಳು);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    ಆದರೆ (SUCCEEDED(hr))
    {
	hr = localized_Family_ಹೆಸರುಗಳು->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    ಆದರೆ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_ಗಾತ್ರ = ದಶಮಾಂಶ(logFont.lf_ಎತ್ತರ);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	ಆದರೆ (font_ಗಾತ್ರ < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_ಗಾತ್ರ = -font_ಗಾತ್ರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font_ಗಾತ್ರ > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_ಗಾತ್ರ = font_ಗಾತ್ರ * ದಶಮಾಂಶ(fontMetrics.design_Units_ಪ್ರತಿ_Em)
		/ ದಶಮಾಂಶ(fontMetrics.ascent + fontMetrics.descent + fontMetrics.ಸಾಲು_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_ಉದ್ದಳತೆ];
    ಆದರೆ (SUCCEEDED(hr))
    {
	ಆದರೆ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_ಉದ್ದಳತೆ) == 0)
	    hr = HRESULT_FROM_WIN32(Get_ಕೊನೆಯ_Error());
    }

    ಆದರೆ (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_ಪಠ್ಯ_Format(
		familyName,
		ಖಾಲಿ, // no custom font collection
		font->Get_ತೂಕ(),
		font->GetStyle(),
		font->GetStretch(),
		font_ಗಾತ್ರ,
		localeName,
		&p_ಪಠ್ಯ_Format);
    }

    ಆದರೆ (SUCCEEDED(hr))
	hr = p_ಪಠ್ಯ_Format->ಗಣ_ಪಠ್ಯ_Alignment(DWRITE_ಪಠ್ಯ_ALIGNMENT_LEADING);

    ಆದರೆ (SUCCEEDED(hr))
	hr = p_ಪಠ್ಯ_Format->ಗಣ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    ಆದರೆ (SUCCEEDED(hr))
	hr = p_ಪಠ್ಯ_Format->ಗಣ_ಶಬ್ದ_Wrapping(DWRITE_ಶಬ್ದ_WRAPPING_NO_WRAP);

    ಸುರಕ್ಷಿತ_Release(&localized_Family_ಹೆಸರುಗಳು);
    ಸುರಕ್ಷಿತ_Release(&fontFamily);
    ಸುರಕ್ಷಿತ_Release(&font);

    ಆದರೆ (SUCCEEDED(hr))
    {
	*pp_ಪಠ್ಯ_Format = p_ಪಠ್ಯ_Format;
	ಆದರೆ (pFontAscent != ಖಾಲಿ && fontMetrics.design_Units_ಪ್ರತಿ_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT ಒಟ್ಟು_ಎತ್ತರ = ದಶಮಾಂಶ(fontMetrics.ascent + fontMetrics.descent);
	    ಆದರೆ (ಒಟ್ಟು_ಎತ್ತರ != 0.0f)
		*pFontAscent = ದಶಮಾಂಶ(fontMetrics.ascent) / ಒಟ್ಟು_ಎತ್ತರ;
	    ಇಲ್ಲದಿದ್ದರೆ
		*pFontAscent = 0.83f; // fallback
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸುರಕ್ಷಿತ_Release(&p_ಪಠ್ಯ_Format);

    ಹಿಂತಿರುಗಿ hr;
}

    HRESULT
DWriteContext::ಗಣ_Font_By_LOGFONT(ಸ್ಥಿರಾಂಕ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_ಪಠ್ಯ_Format *p_ಪಠ್ಯ_Format = ಖಾಲಿ;
    FLOAT fontAscent = 0.0f;

    hr = Create_ಪಠ್ಯ_Format_From_LOGFONT(logFont, &p_ಪಠ್ಯ_Format, &fontAscent);

    ಆದರೆ (SUCCEEDED(hr))
    {
	ಸುರಕ್ಷಿತ_Release(&m_ಪಠ್ಯ_Format);
	m_ಪಠ್ಯ_Format = p_ಪಠ್ಯ_Format;
	m_Font_ತೂಕ = static_cast<DWRITE_FONT_ತೂಕ>(logFont.lf_ತೂಕ);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_ಸಾಮಾನ್ಯ;
	mFontAscent = fontAscent;
    }

    ಹಿಂತಿರುಗಿ hr;
}

    ಶೂನ್ಯ
DWriteContext::ಗಣ_Font(HFONT hFont)
{
    FontCache::ಅಂಶ_2 ಅಂಶ;
    ಆದರೆ (mFontCache.get(hFont, ಅಂಶ))
    {
	ಆದರೆ (ಅಂಶ.p_ಪಠ್ಯ_Format != ಖಾಲಿ)
	{
	    ಅಂಶ.p_ಪಠ್ಯ_Format->ಕೂಡಿಸು_Ref();
	    ಸುರಕ್ಷಿತ_Release(&m_ಪಠ್ಯ_Format);
	    m_ಪಠ್ಯ_Format = ಅಂಶ.p_ಪಠ್ಯ_Format;
	    m_Font_ತೂಕ = ಅಂಶ.font_ತೂಕ;
	    mFontStyle = ಅಂಶ.fontStyle;
	    mFontAscent = ಅಂಶ.fontAscent;
	    mFallbackDC = ಸುಳ್ಳು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    mFallbackDC = ನಿಜ;
	ಹಿಂತಿರುಗಿ;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    ಆದರೆ (GetObjectW(hFont, ಗಾತ್ರ(lf), &lf))
	hr = ಗಣ_Font_By_LOGFONT(lf);

    ಅಂಶ.hFont = hFont;
    ಆದರೆ (SUCCEEDED(hr))
    {
	ಅಂಶ.p_ಪಠ್ಯ_Format = m_ಪಠ್ಯ_Format;
	ಅಂಶ.font_ತೂಕ = m_Font_ತೂಕ;
	ಅಂಶ.fontStyle = mFontStyle;
	ಅಂಶ.fontAscent = mFontAscent;
	mFallbackDC = ಸುಳ್ಳು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	mFallbackDC = ನಿಜ;
    mFontCache.put(ಅಂಶ);
}

    ಶೂನ್ಯ
DWriteContext::Rebind()
{
    ಸುರಕ್ಷಿತ_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->ಗಣ_Transform(D2D1::ತಟಸ್ಥ_ಮಾತೃಕೆ());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ಬಹುಲಕ_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::ಗಾತ್ರ_U(mBindRect.ಬಲ - mBindRect.ಎಡ,
		mBindRect.bottom - mBindRect.ತುದಿ),
	    props, &mBitmap);
}

    ಶೂನ್ಯ
DWriteContext::BindDC(HDC hdc, ಸ್ಥಿರಾಂಕ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    ಆದರೆ (mRT == ಖಾಲಿ)
	CreateDeviceResources();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Flush();
	Rebind();
    }
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ redraw_later_clear(ಶೂನ್ಯ);

    HRESULT
DWriteContext::ಗಣ_Drawing_ಬಹುಲಕ(Drawing_ಬಹುಲಕ ಬಹುಲಕ)
{
    HRESULT hr = S_OK;

    ಆಯ್ಕೆ (ಬಹುಲಕ)
    {
	ಪೂರ್ವನಿಯೋಜಿತ:
	ಸ್ಥಿತಿ DM_GDI:
	    ಆದರೆ (mInteropHDC != ಖಾಲಿ)
	    {
		mGDIRT->ReleaseDC(ಖಾಲಿ);
		mInteropHDC = ಖಾಲಿ;
	    }
	    ಆದರೆ (mDrawing)
	    {
		hr = mRT->ಕೊನೆ_Draw();
		ಆದರೆ (hr == (HRESULT)D2_DERR_RECREATE_ಗುರಿ)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = ಸುಳ್ಳು;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ DM_DIRECTX:
	    ಆದರೆ (mInteropHDC != ಖಾಲಿ)
	    {
		mGDIRT->ReleaseDC(ಖಾಲಿ);
		mInteropHDC = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mDrawing == ಸುಳ್ಳು)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ನಿಜ;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ DM_INTEROP:
	    ಆದರೆ (mDrawing == ಸುಳ್ಳು)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ನಿಜ;
	    }
	    ಆದರೆ (mInteropHDC == ಖಾಲಿ)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_ಬಹುಲಕ_COPY, &mInteropHDC);
	    ನಿಲ್ಲಿಸು;
    }
    mDMode = ಬಹುಲಕ;
    ಹಿಂತಿರುಗಿ hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->ಗಣ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ಹಿಂತಿರುಗಿ mBrush;
}

    ಶೂನ್ಯ
DWriteContext::Draw_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ WCHAR *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ len,
	ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h, ಪೂರ್ಣಾಂಕ cell_ಅಗಲ_2, COLORREF color,
	UINT fuOptions, ಸ್ಥಿರಾಂಕ RECT *lprc, ಸ್ಥಿರಾಂಕ INT *lpDx)
{
    ಆದರೆ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = ಗಣ_Drawing_ಬಹುಲಕ(DM_INTEROP);
	ಆದರೆ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::ಗಣ_ಪಠ್ಯ_Color(mInteropHDC, color);
	    ::ಗಣ_Bk_ಬಹುಲಕ(mInteropHDC, ::Get_Bk_ಬಹುಲಕ(mHDC));
	    ::Ext_ಪಠ್ಯ_ಹೊರಹೋಗು_W(mInteropHDC, x, y, fuOptions, lprc, ಪಠ್ಯ, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ಹಿಂತಿರುಗಿ;
    }

    HRESULT hr;
    IDWrite_ಪಠ್ಯ_Layout *ಪಠ್ಯ_Layout = ಖಾಲಿ;

    ಗಣ_Drawing_ಬಹುಲಕ(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    ಆದರೆ ((fuOptions & ETO_CLIPPED) && lprc != ಖಾಲಿ)
    {
	mRT->ತಳ್ಳು_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ಎಡ), FLOAT(lprc->ತುದಿ),
			FLOAT(lprc->ಬಲ), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_ಬಹುಲಕ_ALIASED);
    }

    hr = mDWriteFactory->Create_ಪಠ್ಯ_Layout(ಪಠ್ಯ, len, m_ಪಠ್ಯ_Format,
	    FLOAT(w), FLOAT(h), &ಪಠ್ಯ_Layout);

    ಆದರೆ (SUCCEEDED(hr))
    {
	DWRITE_ಪಠ್ಯ_RANGE ಪಠ್ಯ_Range = { 0, UINT32(len) };
	ಪಠ್ಯ_Layout->ಗಣ_Font_ತೂಕ(m_Font_ತೂಕ, ಪಠ್ಯ_Range);
	ಪಠ್ಯ_Layout->ಗಣ_Font_Style(mFontStyle, ಪಠ್ಯ_Range);

	ಆದರೆ (m_Font_Feature_ಎಣಿಕೆ > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್_KERNING, 1 },
	    };
	    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Defaults = ಗಾತ್ರ(defaultFeatures)
		/ ಗಾತ್ರ(defaultFeatures[0]);

	    IDWriteTypography *typography = ಖಾಲಿ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    ಆದರೆ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ d = 0; d < ಅಂಕೆ_Defaults; ++d)
		{
		    ಪೂರ್ಣಾಂಕ overridden = 0;
		    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ u = 0; u < m_Font_Feature_ಎಣಿಕೆ; ++u)
		    {
			ಆದರೆ ((DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್)mFontFeatures[u].ಟ್ಯಾಗ್
				== defaultFeatures[d].name_ಟ್ಯಾಗ್)
			{
			    overridden = 1;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಆದರೆ (!overridden)
			typography->ಕೂಡಿಸು_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < m_Font_Feature_ಎಣಿಕೆ; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_ಟ್ಯಾಗ್)mFontFeatures[i].ಟ್ಯಾಗ್,
			mFontFeatures[i].parameter
		    };
		    typography->ಕೂಡಿಸು_Font_Feature(ff);
		}
		ಪಠ್ಯ_Layout->ಗಣ_Typography(typography, ಪಠ್ಯ_Range);
		ಸುರಕ್ಷಿತ_Release(&typography);
	    }
	}

	// Calculate baseline using font ascent from font metrics.
	// Do NOT use GetLineMetrics() because it returns different values
	// depending on text content (e.g., when CJK characters trigger
	// font fallback, the metrics change).
	// Use the pre-calculated font ascent for all text to prevent
	// vertical shifts during redraw.
	FLOAT baselineY = roundf(FLOAT(y) + FLOAT(h) * mFontAscent + 0.5);

	ಪಠ್ಯ_Renderer renderer(ಇದು);
	ಪಠ್ಯ_Renderer_Context context = { color, FLOAT(cell_ಅಗಲ_2), lpDx, len,
		baselineY, 0.0f, 0 };

	ಪಠ್ಯ_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    ಸುರಕ್ಷಿತ_Release(&ಪಠ್ಯ_Layout);

    ಆದರೆ ((fuOptions & ETO_CLIPPED) && lprc != ಖಾಲಿ)
	mRT->ತೆಗೆ_Axis_Aligned_Clip();
}

    ಶೂನ್ಯ
DWriteContext::FillRect(ಸ್ಥಿರಾಂಕ RECT *rc, COLORREF color)
{
    ಆದರೆ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_Drawing_ಬಹುಲಕ(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ಎಡ), FLOAT(rc->ತುದಿ),
		    FLOAT(rc->ಬಲ), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::Draw_ಸಾಲು(ಪೂರ್ಣಾಂಕ x1, ಪೂರ್ಣಾಂಕ y1, ಪೂರ್ಣಾಂಕ x2, ಪೂರ್ಣಾಂಕ y2, COLORREF color)
{
    ಆದರೆ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_ಗಮ್ಯ_Ex(mInteropHDC, x1, y1, ಖಾಲಿ);
	::ಸಾಲು_ಗಮ್ಯ(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_Drawing_ಬಹುಲಕ(DM_DIRECTX);
	mRT->Draw_ಸಾಲು(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::ಗಣ_Pixel(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, COLORREF color)
{
    ಆದರೆ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::ಗಣ_Pixel(mInteropHDC, x, y, color);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಣ_Drawing_ಬಹುಲಕ(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_ಸಾಲು(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::Scroll(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಸ್ಥಿರಾಂಕ RECT *rc)
{
    ಗಣ_Drawing_ಬಹುಲಕ(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_ಬಿಂದು_2_U dest_ಬಿಂದು;
    ಆದರೆ (x >= 0)
    {
	srcRect.ಎಡ = rc->ಎಡ;
	srcRect.ಬಲ = rc->ಬಲ - x;
	dest_ಬಿಂದು.x = rc->ಎಡ + x;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	srcRect.ಎಡ = rc->ಎಡ - x;
	srcRect.ಬಲ = rc->ಬಲ;
	dest_ಬಿಂದು.x = rc->ಎಡ;
    }
    ಆದರೆ (y >= 0)
    {
	srcRect.ತುದಿ = rc->ತುದಿ;
	srcRect.bottom = rc->bottom - y;
	dest_ಬಿಂದು.y = rc->ತುದಿ + y;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	srcRect.ತುದಿ = rc->ತುದಿ - y;
	srcRect.bottom = rc->bottom;
	dest_ಬಿಂದು.y = rc->ತುದಿ;
    }
    mBitmap->Copy_From_Render_ಗುರಿ(&dest_ಬಿಂದು, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_ಬಿಂದು.x), FLOAT(dest_ಬಿಂದು.y),
	    FLOAT(dest_ಬಿಂದು.x + srcRect.ಬಲ - srcRect.ಎಡ),
	    FLOAT(dest_ಬಿಂದು.y + srcRect.bottom - srcRect.ತುದಿ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_ಬಹುಲಕ_ಹತ್ತಿರದ_NEIGHBOR, destRect);
}

    ಶೂನ್ಯ
DWriteContext::Flush()
{
    ಗಣ_Drawing_ಬಹುಲಕ(DM_GDI);
}

    ಶೂನ್ಯ
DWriteContext::ಗಣ_Rendering_Params(
	ಸ್ಥಿರಾಂಕ DWriteRenderingParams *params)
{
    ಆದರೆ (mDWriteFactory == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    IDWriteRenderingParams *renderingParams = ಖಾಲಿ;
    D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ ಪಠ್ಯ_Antialias_ಬಹುಲಕ =
	D2_D1_ಪಠ್ಯ_ANTIALIAS_ಬಹುಲಕ_DEFAULT;
    HRESULT hr;
    ಆದರೆ (params != ಖಾಲಿ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ಗಮ್ಯ_Pixel_Geometry(params->pixelGeometry),
		ಗಮ್ಯ_Rendering_ಬಹುಲಕ(params->rendering_ಬಹುಲಕ), &renderingParams);
	ಪಠ್ಯ_Antialias_ಬಹುಲಕ = ಗಮ್ಯ_ಪಠ್ಯ_Antialias_ಬಹುಲಕ(params->ಪಠ್ಯ_Antialias_ಬಹುಲಕ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    ಆದರೆ (SUCCEEDED(hr) && renderingParams != ಖಾಲಿ)
    {
	ಸುರಕ್ಷಿತ_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_ಪಠ್ಯ_Antialias_ಬಹುಲಕ = ಪಠ್ಯ_Antialias_ಬಹುಲಕ;

	Flush();
	mRT->ಗಣ_ಪಠ್ಯ_Rendering_Params(mRenderingParams);
	mRT->ಗಣ_ಪಠ್ಯ_Antialias_ಬಹುಲಕ(m_ಪಠ್ಯ_Antialias_ಬಹುಲಕ);
    }
}

    DWriteRenderingParams *
DWriteContext::GetRenderingParams(
	DWriteRenderingParams *params)
{
    ಆದರೆ (params != ಖಾಲಿ && mRenderingParams != ಖಾಲಿ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ಗಮ್ಯ_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_ಬಹುಲಕ = ಗಮ್ಯ_Int(mRenderingParams->Get_Rendering_ಬಹುಲಕ());
	params->ಪಠ್ಯ_Antialias_ಬಹುಲಕ = m_ಪಠ್ಯ_Antialias_ಬಹುಲಕ;
    }
    ಹಿಂತಿರುಗಿ params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    ಶೂನ್ಯ
DWrite_Init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_ತುಂಬು_Lib("d2d1.dll");
    hDWriteDLL = vim_ತುಂಬು_Lib("dwrite.dll");
    ಆದರೆ (hD2D1DLL == ಖಾಲಿ || hDWriteDLL == ಖಾಲಿ)
    {
	DWrite_Final();
	ಹಿಂತಿರುಗಿ;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_ವಿಳಾಸ(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_ವಿಳಾಸ(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_ವಿಳಾಸ(hDWriteDLL,
	    "DWriteCreateFactory");
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
DWrite_Final(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ಖಾಲಿ;
    pD2D1CreateFactory = ಖಾಲಿ;
    pDWriteCreateFactory = ಖಾಲಿ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#ಆದರೆ_ಅಂತ್ಯ
}

    DWriteContext *
DWriteContext_Open(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_DIRECTX
    ಆದರೆ (pGetUserDefaultLocaleName == ಖಾಲಿ || pD2D1CreateFactory == ಖಾಲಿ
	    || pDWriteCreateFactory == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಹೊಸ DWriteContext();
}

    ಶೂನ್ಯ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ಸ್ಥಿರಾಂಕ RECT *rect)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->BindDC(hdc, rect);
}

    ಶೂನ್ಯ
DWrite_Context_ಗಣ_Font(DWriteContext *ctx, HFONT hFont)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->ಗಣ_Font(hFont);
}

    ಶೂನ್ಯ
DWrite_Context_Draw_ಪಠ್ಯ(
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
	ಸ್ಥಿರಾಂಕ INT *lpDx)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->Draw_ಪಠ್ಯ(ಪಠ್ಯ, len, x, y, w, h, cell_ಅಗಲ_2, color,
		fuOptions, lprc, lpDx);
}

    ಶೂನ್ಯ
DWriteContext_FillRect(DWriteContext *ctx, ಸ್ಥಿರಾಂಕ RECT *rc, COLORREF color)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->FillRect(rc, color);
}

    ಶೂನ್ಯ
DWrite_Context_Draw_ಸಾಲು(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x1, ಪೂರ್ಣಾಂಕ y1, ಪೂರ್ಣಾಂಕ x2, ಪೂರ್ಣಾಂಕ y2,
	COLORREF color)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->Draw_ಸಾಲು(x1, y1, x2, y2, color);
}

    ಶೂನ್ಯ
DWrite_Context_ಗಣ_Pixel(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, COLORREF color)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->ಗಣ_Pixel(x, y, color);
}

    ಶೂನ್ಯ
DWriteContext_Scroll(DWriteContext *ctx, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಸ್ಥಿರಾಂಕ RECT *rc)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->Scroll(x, y, rc);
}

    ಶೂನ್ಯ
DWriteContext_Flush(DWriteContext *ctx)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->Flush();
}

    ಶೂನ್ಯ
DWriteContext_Close(DWriteContext *ctx)
{
    ಅಳಿಸು ctx;
}

    ಶೂನ್ಯ
DWrite_Context_ಗಣ_Rendering_Params(
	DWriteContext *ctx,
	ಸ್ಥಿರಾಂಕ DWriteRenderingParams *params)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->ಗಣ_Rendering_Params(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ctx->GetRenderingParams(params);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
DWriteContext::ಗಣ_Font_Features(
	ಸ್ಥಿರಾಂಕ DWriteFontFeature *features, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (ಎಣಿಕೆ_2 > DWRITE_MAX_FONT_FEATURES)
	ಎಣಿಕೆ_2 = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
    ಆದರೆ (ಎಣಿಕೆ_2 > 0 && features != ಖಾಲಿ)
	ಸ್ಮರಣೆ_ನಕಲು(mFontFeatures, features, ಗಾತ್ರ(DWriteFontFeature) * ಎಣಿಕೆ_2);
}

    ಶೂನ್ಯ
DWrite_Context_ಗಣ_Font_Features(
	DWriteContext *ctx,
	ಸ್ಥಿರಾಂಕ DWriteFontFeature *features,
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (ctx != ಖಾಲಿ)
	ctx->ಗಣ_Font_Features(features, ಎಣಿಕೆ_2);
}
