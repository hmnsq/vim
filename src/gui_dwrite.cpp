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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIN32_LEAN_AND_ਔਸਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DYNAMIC_DIRECTX
# ਜੇ WINVER < 0x0600
#  error WINVER must be 0x0600 or above ਮੰਜ਼ਿਲ use ਸਿੱਧਾ_Write(ਸਿੱਧਾ_X)
# endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <crtdbg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <d2d1.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#ਜੇ defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define ਵਿੱਚ_reads(s)
# define ਵਿੱਚ_reads_opt(s)
# define _Maybenull_
# define ਬਾਹਰਲਾ_writes(s)
# define ਬਾਹਰਲਾ_writes_opt(s)
# define ਬਾਹਰਲਾ_writes_ਮੰਜ਼ਿਲ(x, y)
# define ਬਾਹਰਲਾ_writes_ਮੰਜ਼ਿਲ_opt(x, y)
# define _Outptr_
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#ਨਹੀਂਤਾਂ
# include <dwrite.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "gui_dwrite.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_ਵਿੱਚ
# define __out		SAL_ਬਾਹਰਲਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#ਨਹੀਂਤਾਂ
# define ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਜੇ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ਨਹੀਂਤਾਂ
# define FINAL
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_DIRECTX
ਬਾਹਰੀ "C" HINSTANCE vim_ਲੱਦੋ_Lib(ਅਚਲ ਅੱਖਰ *name);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ਅਚਲ D2D1_FACTORY_OPTIONS *, ਖਾਲੀ **);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ਸਥਿਰ HINSTANCE hD2D1DLL = ਨਿਰਾ;
ਸਥਿਰ HINSTANCE hDWriteDLL = ਨਿਰਾ;

ਸਥਿਰ PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ਨਿਰਾ;
ਸਥਿਰ PD2D1CREATEFACTORY pD2D1CreateFactory = ਨਿਰਾ;
ਸਥਿਰ PDWRITECREATEFACTORY pDWriteCreateFactory = ਨਿਰਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ D2D1CreateFactory		(*pD2D1CreateFactory)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DWriteCreateFactory		(*pDWriteCreateFactory)

    ਸਥਿਰ ਖਾਲੀ
unload(HINSTANCE &hinst)
{
    ਜੇ (hinst != ਨਿਰਾ)
    {
	FreeLibrary(hinst);
	hinst = ਨਿਰਾ;
    }
}
#ਜੇ_ਅੰਤ // DYNAMIC_DIRECTX

ਸਾਂਚਾ <ਵਰਗ T> ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੁਰੱਖਿਅਤ_Release(T **ppT)
{
    ਜੇ (*ppT)
    {
	(*ppT)->Release();
	*ppT = ਨਿਰਾ;
    }
}

    ਸਥਿਰ DWRITE_PIXEL_GEOMETRY
ਮੰਜ਼ਿਲ_Pixel_Geometry(ਪੂਰਨ ਮੁੱਲ)
{
    ਚੋਣ (ਮੁੱਲ)
    {
	ਮੂਲ:
	ਹਾਲਤ 0:
	    ਵਾਪਸ DWRITE_PIXEL_GEOMETRY_FLAT;
	ਹਾਲਤ 1:
	    ਵਾਪਸ DWRITE_PIXEL_GEOMETRY_RGB;
	ਹਾਲਤ 2:
	    ਵਾਪਸ DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ਸਥਿਰ ਪੂਰਨ
ਮੰਜ਼ਿਲ_Int(DWRITE_PIXEL_GEOMETRY ਮੁੱਲ)
{
    ਚੋਣ (ਮੁੱਲ)
    {
	ਹਾਲਤ DWRITE_PIXEL_GEOMETRY_FLAT:
	    ਵਾਪਸ 0;
	ਹਾਲਤ DWRITE_PIXEL_GEOMETRY_RGB:
	    ਵਾਪਸ 1;
	ਹਾਲਤ DWRITE_PIXEL_GEOMETRY_BGR:
	    ਵਾਪਸ 2;
	ਮੂਲ:
	    ਵਾਪਸ -1;
    }
}

    ਸਥਿਰ DWRITE_RENDERING_ਬਹੁਲਕ
ਮੰਜ਼ਿਲ_Rendering_ਬਹੁਲਕ(ਪੂਰਨ ਮੁੱਲ)
{
    ਚੋਣ (ਮੁੱਲ)
    {
	ਮੂਲ:
	ਹਾਲਤ 0:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_DEFAULT;
	ਹਾਲਤ 1:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_ALIASED;
	ਹਾਲਤ 2:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_GDI_CLASSIC;
	ਹਾਲਤ 3:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_GDI_NATURAL;
	ਹਾਲਤ 4:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_NATURAL;
	ਹਾਲਤ 5:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_NATURAL_ਸਮਮਿਤ;
	ਹਾਲਤ 6:
	    ਵਾਪਸ DWRITE_RENDERING_ਬਹੁਲਕ_OUTLINE;
    }
}

    ਸਥਿਰ D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ
ਮੰਜ਼ਿਲ_ਪਾਠ_Antialias_ਬਹੁਲਕ(ਪੂਰਨ ਮੁੱਲ)
{
    ਚੋਣ (ਮੁੱਲ)
    {
	ਮੂਲ:
	ਹਾਲਤ 0:
	    ਵਾਪਸ D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_DEFAULT;
	ਹਾਲਤ 1:
	    ਵਾਪਸ D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_CLEARTYPE;
	ਹਾਲਤ 2:
	    ਵਾਪਸ D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_GRAYSCALE;
	ਹਾਲਤ 3:
	    ਵਾਪਸ D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_ALIASED;
    }
}

    ਸਥਿਰ ਪੂਰਨ
ਮੰਜ਼ਿਲ_Int(DWRITE_RENDERING_ਬਹੁਲਕ ਮੁੱਲ)
{
    ਚੋਣ (ਮੁੱਲ)
    {
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_DEFAULT:
	    ਵਾਪਸ 0;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_ALIASED:
	    ਵਾਪਸ 1;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_GDI_CLASSIC:
	    ਵਾਪਸ 2;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_GDI_NATURAL:
	    ਵਾਪਸ 3;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_NATURAL:
	    ਵਾਪਸ 4;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_CLEARTYPE_NATURAL_ਸਮਮਿਤ:
	    ਵਾਪਸ 5;
	ਹਾਲਤ DWRITE_RENDERING_ਬਹੁਲਕ_OUTLINE:
	    ਵਾਪਸ 6;
	ਮੂਲ:
	    ਵਾਪਸ -1;
    }
}

ਵਰਗ FontCache {
ਜਨਤਕ:
    ਬਣਤਰ ਵਸਤੂ_2 {
	HFONT              hFont;
	IDWrite_ਪਾਠ_Format* p_ਪਾਠ_Format;
	DWRITE_FONT_ਭਾਰ font_ਭਾਰ;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	ਵਸਤੂ_2() : hFont(ਨਿਰਾ), p_ਪਾਠ_Format(ਨਿਰਾ), fontAscent(0.0f) {}
    };

ਨਿਜੀ:
    ਪੂਰਨ m_ਮਾਪ;
    ਵਸਤੂ_2 *m_ਚੀਜ਼ਾਂ;

ਜਨਤਕ:
    FontCache(ਪੂਰਨ ਮਾਪ = 2) :
	m_ਮਾਪ(ਮਾਪ),
	m_ਚੀਜ਼ਾਂ(ਨਵਾਂ ਵਸਤੂ_2[ਮਾਪ])
    {
    }

    ~FontCache()
    {
	ਲਈ (ਪੂਰਨ i = 0; i < m_ਮਾਪ; ++i)
	    ਸੁਰੱਖਿਅਤ_Release(&m_ਚੀਜ਼ਾਂ[i].p_ਪਾਠ_Format);
	ਮਿਟਾਓ[] m_ਚੀਜ਼ਾਂ;
    }

    ਤਾਰਕਿਕ get(HFONT hFont, ਵਸਤੂ_2 &ਵਸਤੂ)
    {
	ਪੂਰਨ n = ਲੱਭੋ(hFont);
	ਜੇ (n < 0)
	    ਵਾਪਸ ਝੂਠ;
	ਵਸਤੂ = m_ਚੀਜ਼ਾਂ[n];
	slide(n);
	ਵਾਪਸ ਸੱਚ;
    }

    ਖਾਲੀ put(ਅਚਲ ਵਸਤੂ_2& ਵਸਤੂ)
    {
	ਪੂਰਨ n = ਲੱਭੋ(ਵਸਤੂ.hFont);
	ਜੇ (n < 0)
	    n = m_ਮਾਪ - 1;
	ਜੇ (m_ਚੀਜ਼ਾਂ[n].p_ਪਾਠ_Format != ਵਸਤੂ.p_ਪਾਠ_Format)
	{
	    ਸੁਰੱਖਿਅਤ_Release(&m_ਚੀਜ਼ਾਂ[n].p_ਪਾਠ_Format);
	    ਜੇ (ਵਸਤੂ.p_ਪਾਠ_Format != ਨਿਰਾ)
		ਵਸਤੂ.p_ਪਾਠ_Format->ਜੋੜੋ_Ref();
	}
	m_ਚੀਜ਼ਾਂ[n] = ਵਸਤੂ;
	slide(n);
    }

ਨਿਜੀ:
    ਪੂਰਨ ਲੱਭੋ(HFONT hFont)
    {
	ਲਈ (ਪੂਰਨ i = 0; i < m_ਮਾਪ; ++i)
	{
	    ਜੇ (m_ਚੀਜ਼ਾਂ[i].hFont == hFont)
		ਵਾਪਸ i;
	}
	ਵਾਪਸ -1;
    }

    ਖਾਲੀ slide(ਪੂਰਨ ਅਗਲਾ_ਸਿਖਰ)
    {
	ਜੇ (ਅਗਲਾ_ਸਿਖਰ == 0)
	    ਵਾਪਸ;
	ਵਸਤੂ_2 tmp = m_ਚੀਜ਼ਾਂ[ਅਗਲਾ_ਸਿਖਰ];
	ਲਈ (ਪੂਰਨ i = ਅਗਲਾ_ਸਿਖਰ - 1; i >= 0; --i)
	    m_ਚੀਜ਼ਾਂ[i + 1] = m_ਚੀਜ਼ਾਂ[i];
	m_ਚੀਜ਼ਾਂ[0] = tmp;
    }
};

ਗਿਣਤੀ Drawing_ਬਹੁਲਕ {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ਬਣਤਰ DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_ਬਹੁਲਕ mDMode;
    HDC mInteropHDC;
    ਤਾਰਕਿਕ mDrawing;
    ਤਾਰਕਿਕ mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_ਨਿਸ਼ਾਨਾ *mRT;
    ID2_D1_Gdi_Interop_Render_ਨਿਸ਼ਾਨਾ *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#ਜੇ_ਅੰਤ

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_ਪਾਠ_Format *m_ਪਾਠ_Format;
    DWRITE_FONT_ਭਾਰ m_Font_ਭਾਰ;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_ਮਾਪ;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ m_ਪਾਠ_Antialias_ਬਹੁਲਕ;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    ਪੂਰਨ m_Font_Feature_ਗਿਣਤੀ;

    // METHODS

    DWriteContext();

    ਆਭਾਸੀ ~DWriteContext();

    HRESULT CreateDeviceResources();

    ਖਾਲੀ DiscardDeviceResources();

    HRESULT Create_ਪਾਠ_Format_From_LOGFONT(ਅਚਲ LOGFONTW &logFont,
	    IDWrite_ਪਾਠ_Format **pp_ਪਾਠ_Format, FLOAT *pFontAscent);

    HRESULT ਸੈੱਟ_Font_By_LOGFONT(ਅਚਲ LOGFONTW &logFont);

    ਖਾਲੀ ਸੈੱਟ_Font(HFONT hFont);

    ਖਾਲੀ Rebind();

    ਖਾਲੀ BindDC(HDC hdc, ਅਚਲ RECT *rect);

    HRESULT ਸੈੱਟ_Drawing_ਬਹੁਲਕ(Drawing_ਬਹੁਲਕ ਬਹੁਲਕ);

    ID2D1Brush* SolidBrush(COLORREF color);

    ਖਾਲੀ Draw_ਪਾਠ(ਅਚਲ WCHAR *ਪਾਠ, ਪੂਰਨ len,
	ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h, ਪੂਰਨ cell_ਚੌੜਾਈ_2, COLORREF color,
	UINT fuOptions, ਅਚਲ RECT *lprc, ਅਚਲ INT *lpDx);

    ਖਾਲੀ FillRect(ਅਚਲ RECT *rc, COLORREF color);

    ਖਾਲੀ Draw_ਸਤਰ(ਪੂਰਨ x1, ਪੂਰਨ y1, ਪੂਰਨ x2, ਪੂਰਨ y2, COLORREF color);

    ਖਾਲੀ ਸੈੱਟ_Pixel(ਪੂਰਨ x, ਪੂਰਨ y, COLORREF color);

    ਖਾਲੀ Scroll(ਪੂਰਨ x, ਪੂਰਨ y, ਅਚਲ RECT *rc);

    ਖਾਲੀ Flush();

    ਖਾਲੀ ਸੈੱਟ_Rendering_Params(
	    ਅਚਲ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    ਖਾਲੀ ਸੈੱਟ_Font_Features(ਅਚਲ DWriteFontFeature *features, ਪੂਰਨ ਗਿਣਤੀ_2);
};

ਵਰਗ Adjusted_Glyph_ਲੜੀਵਾਰ : ਜਨਤਕ DWRITE_GLYPH_ਲੜੀਵਾਰ
{
ਨਿਜੀ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

ਜਨਤਕ:
    Adjusted_Glyph_ਲੜੀਵਾਰ(
	    ਅਚਲ DWRITE_GLYPH_ਲੜੀਵਾਰ *glyph_ਲੜੀਵਾਰ,
	    FLOAT cell_ਚੌੜਾਈ_2,
	    FLOAT &accum,
	    ਅਚਲ INT *lpDx,
	    ਪੂਰਨ *glyph_ਸੂਚਕਾਂਕ,
	    ਪੂਰਨ cch_ਪਾਠ) :
	DWRITE_GLYPH_ਲੜੀਵਾਰ(*glyph_ਲੜੀਵਾਰ),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(ਨਵਾਂ FLOAT[glyph_ਲੜੀਵਾਰ->glyph_ਗਿਣਤੀ])
    {
	ਪ੍ਰਤਿਜ੍ਞਾ(cell_ਚੌੜਾਈ_2 != 0.0f);
	ਲਈ (UINT32 i = 0; i < glyph_ਲੜੀਵਾਰ->glyph_ਗਿਣਤੀ; ++i)
	{
	    FLOAT orig = glyph_ਲੜੀਵਾਰ->glyphAdvances[i];
	    FLOAT adjusted;

	    ਜੇ (*glyph_ਸੂਚਕਾਂਕ < cch_ਪਾਠ)
	    {
		adjusted = FLOAT(lpDx[*glyph_ਸੂਚਕਾਂਕ]);
		(*glyph_ਸੂਚਕਾਂਕ)++;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		adjusted = adjust_ਮੰਜ਼ਿਲ_Cell(orig, cell_ਚੌੜਾਈ_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_ਲੜੀਵਾਰ()
    {
	mAccum += mDelta;
	ਮਿਟਾਓ[] mAdjustedAdvances;
    }

    ਸਥਿਰ FLOAT adjust_ਮੰਜ਼ਿਲ_Cell(FLOAT ਮੁੱਲ, FLOAT cell_ਚੌੜਾਈ_2)
    {
	ਪੂਰਨ cell_ਗਿਣਤੀ = ਪੂਰਨ(floor(ਮੁੱਲ / cell_ਚੌੜਾਈ_2 + 0.5f));
	ਜੇ (cell_ਗਿਣਤੀ < 1)
	    cell_ਗਿਣਤੀ = 1;
	ਵਾਪਸ cell_ਗਿਣਤੀ * cell_ਚੌੜਾਈ_2;
    }
};

ਬਣਤਰ ਪਾਠ_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_ਚੌੜਾਈ_2;
    ਅਚਲ INT *lpDx;
    ਪੂਰਨ cch_ਪਾਠ;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT ਵਿੱਥ_X;
    ਪੂਰਨ glyph_ਸੂਚਕਾਂਕ;
};

ਵਰਗ ਪਾਠ_Renderer FINAL : ਜਨਤਕ IDWrite_ਪਾਠ_Renderer
{
ਜਨਤਕ:
    ਪਾਠ_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_ਗਿਣਤੀ(0),
	pDWC_(pDWC)
    {
	ਜੋੜੋ_Ref();
    }

    // add "virtual" to avoid a compiler warning
    ਆਭਾਸੀ ~ਪਾਠ_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ਵਾਪਸ S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	__out DWRITE_ਮੈਟ੍ਰਿਕਸ* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_ਮੈਟ੍ਰਿਕਸ_3_X2_F*>(transform));
	ਵਾਪਸ S_OK;
    }

    IFACEMETHOD(Get_Pixels_ਪ੍ਰਤੀ_Dip)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	__out FLOAT* pixels_ਪ੍ਰਤੀ_Dip)
    {
	ਦਸ਼ਮਲਵ dpiX, ਅਣਵਰਤਿਆ_2;
	pDWC_->mRT->GetDpi(&dpiX, &ਅਣਵਰਤਿਆ_2);
	*pixels_ਪ੍ਰਤੀ_Dip = dpiX / 96.0f;
	ਵਾਪਸ S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	FLOAT baselineOriginX ਅਣਵਰਤਿਆ,
	FLOAT baselineOriginY ਅਣਵਰਤਿਆ,
	__in DWRITE_UNDERLINE ਅਚਲ* underline ਅਣਵਰਤਿਆ,
	IUnknown* clientDrawingEffect ਅਣਵਰਤਿਆ)
    {
	ਵਾਪਸ E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	FLOAT baselineOriginX ਅਣਵਰਤਿਆ,
	FLOAT baselineOriginY ਅਣਵਰਤਿਆ,
	__in DWRITE_STRIKETHROUGH ਅਚਲ* strikethrough ਅਣਵਰਤਿਆ,
	IUnknown* clientDrawingEffect ਅਣਵਰਤਿਆ)
    {
	ਵਾਪਸ E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull ਖਾਲੀ* clientDrawingContext ਅਣਵਰਤਿਆ,
	FLOAT originX ਅਣਵਰਤਿਆ,
	FLOAT originY ਅਣਵਰਤਿਆ,
	IDWriteInlineObject* inlineObject ਅਣਵਰਤਿਆ,
	BOOL isSideways ਅਣਵਰਤਿਆ,
	BOOL is_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ ਅਣਵਰਤਿਆ,
	IUnknown* clientDrawingEffect ਅਣਵਰਤਿਆ)
    {
	ਵਾਪਸ E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_ਲੜੀਵਾਰ)(
	__maybenull ਖਾਲੀ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_ਬਹੁਲਕ measuring_ਬਹੁਲਕ ਅਣਵਰਤਿਆ,
	__in DWRITE_GLYPH_ਲੜੀਵਾਰ ਅਚਲ* glyph_ਲੜੀਵਾਰ,
	__in DWRITE_GLYPH_ਲੜੀਵਾਰ_DESCRIPTION ਅਚਲ* glyph_ਲੜੀਵਾਰ_Description ਅਣਵਰਤਿਆ,
	IUnknown* clientDrawingEffect ਅਣਵਰਤਿਆ)
    {
	ਪਾਠ_Renderer_Context *context =
	    reinterpret_cast<ਪਾਠ_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_ਲੜੀਵਾਰ adjusted_Glyph_ਲੜੀਵਾਰ(glyph_ਲੜੀਵਾਰ, context->cell_ਚੌੜਾਈ_2,
		context->ਵਿੱਥ_X, context->lpDx, &context->glyph_ਸੂਚਕਾਂਕ,
		context->cch_ਪਾਠ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
	ਜੇ (pDWC_->mDWriteFactory2 != ਨਿਰਾ)
	{
	    IDWrite_Color_Glyph_ਲੜੀਵਾਰ_Enumerator *enumerator = ਨਿਰਾ;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_ਲੜੀਵਾਰ(
		baselineOriginX + context->ਵਿੱਥ_X,
		baselineOriginY,
		&adjusted_Glyph_ਲੜੀਵਾਰ,
		ਨਿਰਾ,
		DWRITE_MEASURING_ਬਹੁਲਕ_GDI_NATURAL,
		ਨਿਰਾ,
		0,
		&enumerator);
	    ਜੇ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_ਲੜੀਵਾਰ = TRUE;
		enumerator->Move_ਅਗਲਾ(&has_ਲੜੀਵਾਰ);
		ਜਦੋਂ (has_ਲੜੀਵਾਰ)
		{
		    ਅਚਲ DWRITE_COLOR_GLYPH_ਲੜੀਵਾਰ* color_Glyph_ਲੜੀਵਾਰ;
		    enumerator->Get_Current_ਲੜੀਵਾਰ(&color_Glyph_ਲੜੀਵਾਰ);

		    pDWC_->mBrush->ਸੈੱਟ_Color(color_Glyph_ਲੜੀਵਾਰ->ਲੜੀਵਾਰ_Color);
		    pDWC_->mRT->Draw_Glyph_ਲੜੀਵਾਰ(
			    D2D1::Point2F(
				color_Glyph_ਲੜੀਵਾਰ->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_ਲੜੀਵਾਰ->glyph_ਲੜੀਵਾਰ,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_ਬਹੁਲਕ_NATURAL);
		    enumerator->Move_ਅਗਲਾ(&has_ਲੜੀਵਾਰ);
		}
		ਸੁਰੱਖਿਅਤ_Release(&enumerator);
		ਵਾਪਸ S_OK;
	    }
	}
#ਜੇ_ਅੰਤ

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_ਲੜੀਵਾਰ(
		D2D1::Point2F(
		    baselineOriginX + context->ਵਿੱਥ_X,
		    fixedBaselineY),
		&adjusted_Glyph_ਲੜੀਵਾਰ,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_ਬਹੁਲਕ_NATURAL);
	ਵਾਪਸ S_OK;
    }

ਜਨਤਕ:
    IFACEMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, ਜੋੜੋ_Ref) ()
    {
	ਵਾਪਸ InterlockedIncrement(&c_Ref_ਗਿਣਤੀ);
    }

    IFACEMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, Release) ()
    {
	ਲੰਮਾ new_ਗਿਣਤੀ_2 = InterlockedDecrement(&c_Ref_ਗਿਣਤੀ);

	ਜੇ (new_ਗਿਣਤੀ_2 == 0)
	{
	    ਮਿਟਾਓ ਇਹ;
	    ਵਾਪਸ 0;
	}
	ਵਾਪਸ new_ਗਿਣਤੀ_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ਅਚਲ& riid,
	ਖਾਲੀ** ppvObject)
    {
	ਜੇ (__uuidof(IDWrite_ਪਾਠ_Renderer) == riid)
	{
	    *ppvObject = ਇਹ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ਇਹ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ਇਹ;
	}
	ਨਹੀਂਤਾਂ
	{
	    *ppvObject = ਨਿਰਾ;
	    ਵਾਪਸ E_FAIL;
	}

	ਵਾਪਸ S_OK;
    }

ਨਿਜੀ:
    ਲੰਮਾ c_Ref_ਗਿਣਤੀ;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ਨਿਰਾ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ਨਿਰਾ),
    mDrawing(ਝੂਠ),
    mFallbackDC(ਝੂਠ),
    mD2D1Factory(ਨਿਰਾ),
    mRT(ਨਿਰਾ),
    mGDIRT(ਨਿਰਾ),
    mBrush(ਨਿਰਾ),
    mBitmap(ਨਿਰਾ),
    mDWriteFactory(ਨਿਰਾ),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ਨਿਰਾ),
#ਜੇ_ਅੰਤ
    mGdiInterop(ਨਿਰਾ),
    mRenderingParams(ਨਿਰਾ),
    mFontCache(8),
    m_ਪਾਠ_Format(ਨਿਰਾ),
    m_Font_ਭਾਰ(DWRITE_FONT_ਭਾਰ_ਸਾਧਾਰਨ),
    mFontStyle(DWRITE_FONT_STYLE_ਸਾਧਾਰਨ),
    m_Font_ਮਾਪ(0.0f),
    mFontAscent(0.0f),
    m_ਪਾਠ_Antialias_ਬਹੁਲਕ(D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_DEFAULT),
    m_Font_Feature_ਗਿਣਤੀ(0)
{
    Zero_ਸਮ੍ਰਿਤੀ(mFontFeatures, ਆਕਾਰ(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ਨਿਰਾ,
	    reinterpret_cast<ਖਾਲੀ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    ਜੇ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
    ਜੇ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#ਜੇ_ਅੰਤ

    ਜੇ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    ਜੇ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->CreateRenderingParams(&mRenderingParams);
	_RPT2(_CRT_WARN, "CreateRenderingParams: hr=%p p=%p\n", hr,
		mRenderingParams);
    }
}

DWriteContext::~DWriteContext()
{
    ਸੁਰੱਖਿਅਤ_Release(&m_ਪਾਠ_Format);
    ਸੁਰੱਖਿਅਤ_Release(&mRenderingParams);
    ਸੁਰੱਖਿਅਤ_Release(&mGdiInterop);
    ਸੁਰੱਖਿਅਤ_Release(&mDWriteFactory);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX_COLOR_EMOJI
    ਸੁਰੱਖਿਅਤ_Release(&mDWriteFactory2);
#ਜੇ_ਅੰਤ
    ਸੁਰੱਖਿਅਤ_Release(&mBitmap);
    ਸੁਰੱਖਿਅਤ_Release(&mBrush);
    ਸੁਰੱਖਿਅਤ_Release(&mGDIRT);
    ਸੁਰੱਖਿਅਤ_Release(&mRT);
    ਸੁਰੱਖਿਅਤ_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    ਜੇ (mRT != ਨਿਰਾ)
	ਵਾਪਸ S_OK;

    D2_D1_RENDER_ਨਿਸ਼ਾਨਾ_PROPERTIES props = {
	D2_D1_RENDER_ਨਿਸ਼ਾਨਾ_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ਬਹੁਲਕ_IGNORE },
	0, 0,
	D2_D1_RENDER_ਨਿਸ਼ਾਨਾ_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_ਨਿਸ਼ਾਨਾ(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    ਜੇ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_ਨਿਸ਼ਾਨਾ),
		reinterpret_cast<ਖਾਲੀ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    ਜੇ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    ਜੇ (SUCCEEDED(hr))
	Rebind();

    ਵਾਪਸ hr;
}

    ਖਾਲੀ
DWriteContext::DiscardDeviceResources()
{
    ਸੁਰੱਖਿਅਤ_Release(&mBitmap);
    ਸੁਰੱਖਿਅਤ_Release(&mBrush);
    ਸੁਰੱਖਿਅਤ_Release(&mGDIRT);
    ਸੁਰੱਖਿਅਤ_Release(&mRT);
}

    HRESULT
DWriteContext::Create_ਪਾਠ_Format_From_LOGFONT(ਅਚਲ LOGFONTW &logFont,
	IDWrite_ਪਾਠ_Format **pp_ਪਾਠ_Format, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWrite_ਪਾਠ_Format *p_ਪਾਠ_Format = ਨਿਰਾ;

    IDWriteFont *font = ਨਿਰਾ;
    IDWriteFontFamily *fontFamily = ਨਿਰਾ;
    IDWrite_Localized_ਲੜੀਆਂ *localized_Family_ਨਾਂ = ਨਿਰਾ;
    ਦਸ਼ਮਲਵ font_ਮਾਪ = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    ਜੇ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    ਜੇ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    ਜੇ (SUCCEEDED(hr))
    {
	hr = fontFamily->Get_Family_ਨਾਂ(&localized_Family_ਨਾਂ);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    ਜੇ (SUCCEEDED(hr))
    {
	hr = localized_Family_ਨਾਂ->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    ਜੇ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_ਮਾਪ = ਦਸ਼ਮਲਵ(logFont.lf_ਉਚਾਈ);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	ਜੇ (font_ਮਾਪ < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_ਮਾਪ = -font_ਮਾਪ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (font_ਮਾਪ > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_ਮਾਪ = font_ਮਾਪ * ਦਸ਼ਮਲਵ(fontMetrics.design_Units_ਪ੍ਰਤੀ_Em)
		/ ਦਸ਼ਮਲਵ(fontMetrics.ascent + fontMetrics.descent + fontMetrics.ਸਤਰ_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_ਲੰਮਾਈ];
    ਜੇ (SUCCEEDED(hr))
    {
	ਜੇ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_ਲੰਮਾਈ) == 0)
	    hr = HRESULT_FROM_WIN32(Get_ਆਖਰੀ_Error());
    }

    ਜੇ (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_ਪਾਠ_Format(
		familyName,
		ਨਿਰਾ, // no custom font collection
		font->Get_ਭਾਰ(),
		font->GetStyle(),
		font->GetStretch(),
		font_ਮਾਪ,
		localeName,
		&p_ਪਾਠ_Format);
    }

    ਜੇ (SUCCEEDED(hr))
	hr = p_ਪਾਠ_Format->ਸੈੱਟ_ਪਾਠ_Alignment(DWRITE_ਪਾਠ_ALIGNMENT_LEADING);

    ਜੇ (SUCCEEDED(hr))
	hr = p_ਪਾਠ_Format->ਸੈੱਟ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    ਜੇ (SUCCEEDED(hr))
	hr = p_ਪਾਠ_Format->ਸੈੱਟ_ਸ਼ਬਦ_Wrapping(DWRITE_ਸ਼ਬਦ_WRAPPING_NO_WRAP);

    ਸੁਰੱਖਿਅਤ_Release(&localized_Family_ਨਾਂ);
    ਸੁਰੱਖਿਅਤ_Release(&fontFamily);
    ਸੁਰੱਖਿਅਤ_Release(&font);

    ਜੇ (SUCCEEDED(hr))
    {
	*pp_ਪਾਠ_Format = p_ਪਾਠ_Format;
	ਜੇ (pFontAscent != ਨਿਰਾ && fontMetrics.design_Units_ਪ੍ਰਤੀ_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT ਕੁੱਲ_ਉਚਾਈ = ਦਸ਼ਮਲਵ(fontMetrics.ascent + fontMetrics.descent);
	    ਜੇ (ਕੁੱਲ_ਉਚਾਈ != 0.0f)
		*pFontAscent = ਦਸ਼ਮਲਵ(fontMetrics.ascent) / ਕੁੱਲ_ਉਚਾਈ;
	    ਨਹੀਂਤਾਂ
		*pFontAscent = 0.83f; // fallback
	}
    }
    ਨਹੀਂਤਾਂ
	ਸੁਰੱਖਿਅਤ_Release(&p_ਪਾਠ_Format);

    ਵਾਪਸ hr;
}

    HRESULT
DWriteContext::ਸੈੱਟ_Font_By_LOGFONT(ਅਚਲ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_ਪਾਠ_Format *p_ਪਾਠ_Format = ਨਿਰਾ;
    FLOAT fontAscent = 0.0f;

    hr = Create_ਪਾਠ_Format_From_LOGFONT(logFont, &p_ਪਾਠ_Format, &fontAscent);

    ਜੇ (SUCCEEDED(hr))
    {
	ਸੁਰੱਖਿਅਤ_Release(&m_ਪਾਠ_Format);
	m_ਪਾਠ_Format = p_ਪਾਠ_Format;
	m_Font_ਭਾਰ = static_cast<DWRITE_FONT_ਭਾਰ>(logFont.lf_ਭਾਰ);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_ਸਾਧਾਰਨ;
	mFontAscent = fontAscent;
    }

    ਵਾਪਸ hr;
}

    ਖਾਲੀ
DWriteContext::ਸੈੱਟ_Font(HFONT hFont)
{
    FontCache::ਵਸਤੂ_2 ਵਸਤੂ;
    ਜੇ (mFontCache.get(hFont, ਵਸਤੂ))
    {
	ਜੇ (ਵਸਤੂ.p_ਪਾਠ_Format != ਨਿਰਾ)
	{
	    ਵਸਤੂ.p_ਪਾਠ_Format->ਜੋੜੋ_Ref();
	    ਸੁਰੱਖਿਅਤ_Release(&m_ਪਾਠ_Format);
	    m_ਪਾਠ_Format = ਵਸਤੂ.p_ਪਾਠ_Format;
	    m_Font_ਭਾਰ = ਵਸਤੂ.font_ਭਾਰ;
	    mFontStyle = ਵਸਤੂ.fontStyle;
	    mFontAscent = ਵਸਤੂ.fontAscent;
	    mFallbackDC = ਝੂਠ;
	}
	ਨਹੀਂਤਾਂ
	    mFallbackDC = ਸੱਚ;
	ਵਾਪਸ;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    ਜੇ (GetObjectW(hFont, ਆਕਾਰ(lf), &lf))
	hr = ਸੈੱਟ_Font_By_LOGFONT(lf);

    ਵਸਤੂ.hFont = hFont;
    ਜੇ (SUCCEEDED(hr))
    {
	ਵਸਤੂ.p_ਪਾਠ_Format = m_ਪਾਠ_Format;
	ਵਸਤੂ.font_ਭਾਰ = m_Font_ਭਾਰ;
	ਵਸਤੂ.fontStyle = mFontStyle;
	ਵਸਤੂ.fontAscent = mFontAscent;
	mFallbackDC = ਝੂਠ;
    }
    ਨਹੀਂਤਾਂ
	mFallbackDC = ਸੱਚ;
    mFontCache.put(ਵਸਤੂ);
}

    ਖਾਲੀ
DWriteContext::Rebind()
{
    ਸੁਰੱਖਿਅਤ_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->ਸੈੱਟ_Transform(D2D1::ਪਛਾਣ_ਤੱਤ_ਮੈਟ੍ਰਿਕਸ());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ਬਹੁਲਕ_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::ਮਾਪ_U(mBindRect.ਸੱਜੇ - mBindRect.ਖੱਬੇ,
		mBindRect.bottom - mBindRect.ਸਿਖਰ),
	    props, &mBitmap);
}

    ਖਾਲੀ
DWriteContext::BindDC(HDC hdc, ਅਚਲ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    ਜੇ (mRT == ਨਿਰਾ)
	CreateDeviceResources();
    ਨਹੀਂਤਾਂ
    {
	Flush();
	Rebind();
    }
}

ਬਾਹਰੀ "C" ਖਾਲੀ redraw_later_clear(ਖਾਲੀ);

    HRESULT
DWriteContext::ਸੈੱਟ_Drawing_ਬਹੁਲਕ(Drawing_ਬਹੁਲਕ ਬਹੁਲਕ)
{
    HRESULT hr = S_OK;

    ਚੋਣ (ਬਹੁਲਕ)
    {
	ਮੂਲ:
	ਹਾਲਤ DM_GDI:
	    ਜੇ (mInteropHDC != ਨਿਰਾ)
	    {
		mGDIRT->ReleaseDC(ਨਿਰਾ);
		mInteropHDC = ਨਿਰਾ;
	    }
	    ਜੇ (mDrawing)
	    {
		hr = mRT->ਸਿਰਾ_Draw();
		ਜੇ (hr == (HRESULT)D2_DERR_RECREATE_ਨਿਸ਼ਾਨਾ)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = ਝੂਠ;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ DM_DIRECTX:
	    ਜੇ (mInteropHDC != ਨਿਰਾ)
	    {
		mGDIRT->ReleaseDC(ਨਿਰਾ);
		mInteropHDC = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (mDrawing == ਝੂਠ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ਸੱਚ;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ DM_INTEROP:
	    ਜੇ (mDrawing == ਝੂਠ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ਸੱਚ;
	    }
	    ਜੇ (mInteropHDC == ਨਿਰਾ)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_ਬਹੁਲਕ_COPY, &mInteropHDC);
	    ਰੋਕੋ;
    }
    mDMode = ਬਹੁਲਕ;
    ਵਾਪਸ hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->ਸੈੱਟ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ਵਾਪਸ mBrush;
}

    ਖਾਲੀ
DWriteContext::Draw_ਪਾਠ(ਅਚਲ WCHAR *ਪਾਠ, ਪੂਰਨ len,
	ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h, ਪੂਰਨ cell_ਚੌੜਾਈ_2, COLORREF color,
	UINT fuOptions, ਅਚਲ RECT *lprc, ਅਚਲ INT *lpDx)
{
    ਜੇ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_INTEROP);
	ਜੇ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::ਸੈੱਟ_ਪਾਠ_Color(mInteropHDC, color);
	    ::ਸੈੱਟ_Bk_ਬਹੁਲਕ(mInteropHDC, ::Get_Bk_ਬਹੁਲਕ(mHDC));
	    ::Ext_ਪਾਠ_ਬਾਹਰਲਾ_W(mInteropHDC, x, y, fuOptions, lprc, ਪਾਠ, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ਵਾਪਸ;
    }

    HRESULT hr;
    IDWrite_ਪਾਠ_Layout *ਪਾਠ_Layout = ਨਿਰਾ;

    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    ਜੇ ((fuOptions & ETO_CLIPPED) && lprc != ਨਿਰਾ)
    {
	mRT->ਧੱਕੋ_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ਖੱਬੇ), FLOAT(lprc->ਸਿਖਰ),
			FLOAT(lprc->ਸੱਜੇ), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_ਬਹੁਲਕ_ALIASED);
    }

    hr = mDWriteFactory->Create_ਪਾਠ_Layout(ਪਾਠ, len, m_ਪਾਠ_Format,
	    FLOAT(w), FLOAT(h), &ਪਾਠ_Layout);

    ਜੇ (SUCCEEDED(hr))
    {
	DWRITE_ਪਾਠ_RANGE ਪਾਠ_Range = { 0, UINT32(len) };
	ਪਾਠ_Layout->ਸੈੱਟ_Font_ਭਾਰ(m_Font_ਭਾਰ, ਪਾਠ_Range);
	ਪਾਠ_Layout->ਸੈੱਟ_Font_Style(mFontStyle, ਪਾਠ_Range);

	ਜੇ (m_Font_Feature_ਗਿਣਤੀ > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ਸਥਿਰ ਅਚਲ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_ਟੈਗ_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_ਟੈਗ_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ਟੈਗ_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ਟੈਗ_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ਟੈਗ_KERNING, 1 },
	    };
	    ਸਥਿਰ ਅਚਲ ਪੂਰਨ ਅੰਕ_Defaults = ਆਕਾਰ(defaultFeatures)
		/ ਆਕਾਰ(defaultFeatures[0]);

	    IDWriteTypography *typography = ਨਿਰਾ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    ਜੇ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		ਲਈ (ਪੂਰਨ d = 0; d < ਅੰਕ_Defaults; ++d)
		{
		    ਪੂਰਨ overridden = 0;
		    ਲਈ (ਪੂਰਨ u = 0; u < m_Font_Feature_ਗਿਣਤੀ; ++u)
		    {
			ਜੇ ((DWRITE_FONT_FEATURE_ਟੈਗ)mFontFeatures[u].ਟੈਗ
				== defaultFeatures[d].name_ਟੈਗ)
			{
			    overridden = 1;
			    ਰੋਕੋ;
			}
		    }
		    ਜੇ (!overridden)
			typography->ਜੋੜੋ_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		ਲਈ (ਪੂਰਨ i = 0; i < m_Font_Feature_ਗਿਣਤੀ; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_ਟੈਗ)mFontFeatures[i].ਟੈਗ,
			mFontFeatures[i].parameter
		    };
		    typography->ਜੋੜੋ_Font_Feature(ff);
		}
		ਪਾਠ_Layout->ਸੈੱਟ_Typography(typography, ਪਾਠ_Range);
		ਸੁਰੱਖਿਅਤ_Release(&typography);
	    }
	}

	// Calculate baseline using font ascent from font metrics.
	// Do NOT use GetLineMetrics() because it returns different values
	// depending on text content (e.g., when CJK characters trigger
	// font fallback, the metrics change).
	// Use the pre-calculated font ascent for all text to prevent
	// vertical shifts during redraw.
	FLOAT baselineY = roundf(FLOAT(y) + FLOAT(h) * mFontAscent + 0.5);

	ਪਾਠ_Renderer renderer(ਇਹ);
	ਪਾਠ_Renderer_Context context = { color, FLOAT(cell_ਚੌੜਾਈ_2), lpDx, len,
		baselineY, 0.0f, 0 };

	ਪਾਠ_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    ਸੁਰੱਖਿਅਤ_Release(&ਪਾਠ_Layout);

    ਜੇ ((fuOptions & ETO_CLIPPED) && lprc != ਨਿਰਾ)
	mRT->ਕੱਢੋ_Axis_Aligned_Clip();
}

    ਖਾਲੀ
DWriteContext::FillRect(ਅਚਲ RECT *rc, COLORREF color)
{
    ਜੇ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ਖੱਬੇ), FLOAT(rc->ਸਿਖਰ),
		    FLOAT(rc->ਸੱਜੇ), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    ਖਾਲੀ
DWriteContext::Draw_ਸਤਰ(ਪੂਰਨ x1, ਪੂਰਨ y1, ਪੂਰਨ x2, ਪੂਰਨ y2, COLORREF color)
{
    ਜੇ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_ਮੰਜ਼ਿਲ_Ex(mInteropHDC, x1, y1, ਨਿਰਾ);
	::ਸਤਰ_ਮੰਜ਼ਿਲ(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_DIRECTX);
	mRT->Draw_ਸਤਰ(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    ਖਾਲੀ
DWriteContext::ਸੈੱਟ_Pixel(ਪੂਰਨ x, ਪੂਰਨ y, COLORREF color)
{
    ਜੇ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::ਸੈੱਟ_Pixel(mInteropHDC, x, y, color);
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_ਸਤਰ(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    ਖਾਲੀ
DWriteContext::Scroll(ਪੂਰਨ x, ਪੂਰਨ y, ਅਚਲ RECT *rc)
{
    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_ਬਿੰਦੂ_2_U dest_ਬਿੰਦੂ;
    ਜੇ (x >= 0)
    {
	srcRect.ਖੱਬੇ = rc->ਖੱਬੇ;
	srcRect.ਸੱਜੇ = rc->ਸੱਜੇ - x;
	dest_ਬਿੰਦੂ.x = rc->ਖੱਬੇ + x;
    }
    ਨਹੀਂਤਾਂ
    {
	srcRect.ਖੱਬੇ = rc->ਖੱਬੇ - x;
	srcRect.ਸੱਜੇ = rc->ਸੱਜੇ;
	dest_ਬਿੰਦੂ.x = rc->ਖੱਬੇ;
    }
    ਜੇ (y >= 0)
    {
	srcRect.ਸਿਖਰ = rc->ਸਿਖਰ;
	srcRect.bottom = rc->bottom - y;
	dest_ਬਿੰਦੂ.y = rc->ਸਿਖਰ + y;
    }
    ਨਹੀਂਤਾਂ
    {
	srcRect.ਸਿਖਰ = rc->ਸਿਖਰ - y;
	srcRect.bottom = rc->bottom;
	dest_ਬਿੰਦੂ.y = rc->ਸਿਖਰ;
    }
    mBitmap->Copy_From_Render_ਨਿਸ਼ਾਨਾ(&dest_ਬਿੰਦੂ, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_ਬਿੰਦੂ.x), FLOAT(dest_ਬਿੰਦੂ.y),
	    FLOAT(dest_ਬਿੰਦੂ.x + srcRect.ਸੱਜੇ - srcRect.ਖੱਬੇ),
	    FLOAT(dest_ਬਿੰਦੂ.y + srcRect.bottom - srcRect.ਸਿਖਰ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_ਬਹੁਲਕ_ਸਭ_ਤੋਂ_ਨੇੜੇ_NEIGHBOR, destRect);
}

    ਖਾਲੀ
DWriteContext::Flush()
{
    ਸੈੱਟ_Drawing_ਬਹੁਲਕ(DM_GDI);
}

    ਖਾਲੀ
DWriteContext::ਸੈੱਟ_Rendering_Params(
	ਅਚਲ DWriteRenderingParams *params)
{
    ਜੇ (mDWriteFactory == ਨਿਰਾ)
	ਵਾਪਸ;

    IDWriteRenderingParams *renderingParams = ਨਿਰਾ;
    D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ ਪਾਠ_Antialias_ਬਹੁਲਕ =
	D2_D1_ਪਾਠ_ANTIALIAS_ਬਹੁਲਕ_DEFAULT;
    HRESULT hr;
    ਜੇ (params != ਨਿਰਾ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ਮੰਜ਼ਿਲ_Pixel_Geometry(params->pixelGeometry),
		ਮੰਜ਼ਿਲ_Rendering_ਬਹੁਲਕ(params->rendering_ਬਹੁਲਕ), &renderingParams);
	ਪਾਠ_Antialias_ਬਹੁਲਕ = ਮੰਜ਼ਿਲ_ਪਾਠ_Antialias_ਬਹੁਲਕ(params->ਪਾਠ_Antialias_ਬਹੁਲਕ);
    }
    ਨਹੀਂਤਾਂ
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    ਜੇ (SUCCEEDED(hr) && renderingParams != ਨਿਰਾ)
    {
	ਸੁਰੱਖਿਅਤ_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_ਪਾਠ_Antialias_ਬਹੁਲਕ = ਪਾਠ_Antialias_ਬਹੁਲਕ;

	Flush();
	mRT->ਸੈੱਟ_ਪਾਠ_Rendering_Params(mRenderingParams);
	mRT->ਸੈੱਟ_ਪਾਠ_Antialias_ਬਹੁਲਕ(m_ਪਾਠ_Antialias_ਬਹੁਲਕ);
    }
}

    DWriteRenderingParams *
DWriteContext::GetRenderingParams(
	DWriteRenderingParams *params)
{
    ਜੇ (params != ਨਿਰਾ && mRenderingParams != ਨਿਰਾ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ਮੰਜ਼ਿਲ_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_ਬਹੁਲਕ = ਮੰਜ਼ਿਲ_Int(mRenderingParams->Get_Rendering_ਬਹੁਲਕ());
	params->ਪਾਠ_Antialias_ਬਹੁਲਕ = m_ਪਾਠ_Antialias_ਬਹੁਲਕ;
    }
    ਵਾਪਸ params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    ਖਾਲੀ
DWrite_Init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_ਲੱਦੋ_Lib("d2d1.dll");
    hDWriteDLL = vim_ਲੱਦੋ_Lib("dwrite.dll");
    ਜੇ (hD2D1DLL == ਨਿਰਾ || hDWriteDLL == ਨਿਰਾ)
    {
	DWrite_Final();
	ਵਾਪਸ;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_ਪਤਾ(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_ਪਤਾ(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_ਪਤਾ(hDWriteDLL,
	    "DWriteCreateFactory");
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
DWrite_Final(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ਨਿਰਾ;
    pD2D1CreateFactory = ਨਿਰਾ;
    pDWriteCreateFactory = ਨਿਰਾ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#ਜੇ_ਅੰਤ
}

    DWriteContext *
DWriteContext_Open(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DYNAMIC_DIRECTX
    ਜੇ (pGetUserDefaultLocaleName == ਨਿਰਾ || pD2D1CreateFactory == ਨਿਰਾ
	    || pDWriteCreateFactory == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਵਾਂ DWriteContext();
}

    ਖਾਲੀ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ਅਚਲ RECT *rect)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->BindDC(hdc, rect);
}

    ਖਾਲੀ
DWrite_Context_ਸੈੱਟ_Font(DWriteContext *ctx, HFONT hFont)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->ਸੈੱਟ_Font(hFont);
}

    ਖਾਲੀ
DWrite_Context_Draw_ਪਾਠ(
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
	ਅਚਲ INT *lpDx)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->Draw_ਪਾਠ(ਪਾਠ, len, x, y, w, h, cell_ਚੌੜਾਈ_2, color,
		fuOptions, lprc, lpDx);
}

    ਖਾਲੀ
DWriteContext_FillRect(DWriteContext *ctx, ਅਚਲ RECT *rc, COLORREF color)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->FillRect(rc, color);
}

    ਖਾਲੀ
DWrite_Context_Draw_ਸਤਰ(DWriteContext *ctx, ਪੂਰਨ x1, ਪੂਰਨ y1, ਪੂਰਨ x2, ਪੂਰਨ y2,
	COLORREF color)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->Draw_ਸਤਰ(x1, y1, x2, y2, color);
}

    ਖਾਲੀ
DWrite_Context_ਸੈੱਟ_Pixel(DWriteContext *ctx, ਪੂਰਨ x, ਪੂਰਨ y, COLORREF color)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->ਸੈੱਟ_Pixel(x, y, color);
}

    ਖਾਲੀ
DWriteContext_Scroll(DWriteContext *ctx, ਪੂਰਨ x, ਪੂਰਨ y, ਅਚਲ RECT *rc)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->Scroll(x, y, rc);
}

    ਖਾਲੀ
DWriteContext_Flush(DWriteContext *ctx)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->Flush();
}

    ਖਾਲੀ
DWriteContext_Close(DWriteContext *ctx)
{
    ਮਿਟਾਓ ctx;
}

    ਖਾਲੀ
DWrite_Context_ਸੈੱਟ_Rendering_Params(
	DWriteContext *ctx,
	ਅਚਲ DWriteRenderingParams *params)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->ਸੈੱਟ_Rendering_Params(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ਵਾਪਸ ctx->GetRenderingParams(params);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
DWriteContext::ਸੈੱਟ_Font_Features(
	ਅਚਲ DWriteFontFeature *features, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (ਗਿਣਤੀ_2 > DWRITE_MAX_FONT_FEATURES)
	ਗਿਣਤੀ_2 = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
    ਜੇ (ਗਿਣਤੀ_2 > 0 && features != ਨਿਰਾ)
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(mFontFeatures, features, ਆਕਾਰ(DWriteFontFeature) * ਗਿਣਤੀ_2);
}

    ਖਾਲੀ
DWrite_Context_ਸੈੱਟ_Font_Features(
	DWriteContext *ctx,
	ਅਚਲ DWriteFontFeature *features,
	ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (ctx != ਨਿਰਾ)
	ctx->ਸੈੱਟ_Font_Features(features, ਗਿਣਤੀ_2);
}
