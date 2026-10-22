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

#define WIN32_LEAN_AND_MEAN

#ifndef DYNAMIC_DIRECTX
# ಯದಿ WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#ಅನ್ತರ್ಭಾವ <windows.h>
#ಅನ್ತರ್ಭಾವ <crtdbg.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <d2d1.h>
#ಅನ್ತರ್ಭಾವ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#ಯದಿ defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define _In_reads_(s)
# define _In_reads_opt_(s)
# define _Maybenull_
# define _Out_writes_(s)
# define _Out_writes_opt_(s)
# define _Out_writes_to_(x, y)
# define _Out_writes_to_opt_(x, y)
# define _Outptr_
#endif

#ifdef FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#ಅನ್ಯಥಾ
# include <dwrite.h>
#endif

#ಅನ್ತರ್ಭಾವ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ಅನ್ಯಥಾ
# define UNUSED
#endif

#ಯದಿ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ಅನ್ಯಥಾ
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
ಬಾಹ್ಯ "C" HINSTANCE vimLoadLib(ಧ್ರುವ ವರ್ಣ *name);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ಧ್ರುವ D2D1_FACTORY_OPTIONS *, ಶೂನ್ಯ **);
ಪ್ರಕಾರನಿರ್ದೇಶ HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ಸ್ಥಿರ HINSTANCE hD2D1DLL = ರಿಕ್ತ;
ಸ್ಥಿರ HINSTANCE hDWriteDLL = ರಿಕ್ತ;

ಸ್ಥಿರ PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ರಿಕ್ತ;
ಸ್ಥಿರ PD2D1CREATEFACTORY pD2D1CreateFactory = ರಿಕ್ತ;
ಸ್ಥಿರ PDWRITECREATEFACTORY pDWriteCreateFactory = ರಿಕ್ತ;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    ಸ್ಥಿರ ಶೂನ್ಯ
unload(HINSTANCE &hinst)
{
    ಯದಿ (hinst != ರಿಕ್ತ)
    {
	FreeLibrary(hinst);
	hinst = ರಿಕ್ತ;
    }
}
#endif // DYNAMIC_DIRECTX

template <ವರ್ಗ T> ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ SafeRelease(T **ppT)
{
    ಯದಿ (*ppT)
    {
	(*ppT)->Release();
	*ppT = ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ)
    {
	ಸ್ವಭಾವ:
	ಪಕ್ಷ 0:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_PIXEL_GEOMETRY_FLAT;
	ಪಕ್ಷ 1:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_PIXEL_GEOMETRY_RGB;
	ಪಕ್ಷ 2:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ToInt(DWRITE_PIXEL_GEOMETRY ಮೌಲ್ಯ)
{
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ)
    {
	ಪಕ್ಷ DWRITE_PIXEL_GEOMETRY_FLAT:
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ DWRITE_PIXEL_GEOMETRY_RGB:
	    ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ DWRITE_PIXEL_GEOMETRY_BGR:
	    ಪ್ರತ್ಯಾಗಮ 2;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
}

    ಸ್ಥಿರ DWRITE_RENDERING_MODE
ToRenderingMode(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ)
    {
	ಸ್ವಭಾವ:
	ಪಕ್ಷ 0:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_DEFAULT;
	ಪಕ್ಷ 1:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_ALIASED;
	ಪಕ್ಷ 2:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	ಪಕ್ಷ 3:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	ಪಕ್ಷ 4:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	ಪಕ್ಷ 5:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	ಪಕ್ಷ 6:
	    ಪ್ರತ್ಯಾಗಮ DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    ಸ್ಥಿರ D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ)
    {
	ಸ್ವಭಾವ:
	ಪಕ್ಷ 0:
	    ಪ್ರತ್ಯಾಗಮ D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	ಪಕ್ಷ 1:
	    ಪ್ರತ್ಯಾಗಮ D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	ಪಕ್ಷ 2:
	    ಪ್ರತ್ಯಾಗಮ D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	ಪಕ್ಷ 3:
	    ಪ್ರತ್ಯಾಗಮ D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ToInt(DWRITE_RENDERING_MODE ಮೌಲ್ಯ)
{
    ವಿಕಲ್ಪ (ಮೌಲ್ಯ)
    {
	ಪಕ್ಷ DWRITE_RENDERING_MODE_DEFAULT:
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_ALIASED:
	    ಪ್ರತ್ಯಾಗಮ 1;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    ಪ್ರತ್ಯಾಗಮ 2;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    ಪ್ರತ್ಯಾಗಮ 3;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    ಪ್ರತ್ಯಾಗಮ 4;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    ಪ್ರತ್ಯಾಗಮ 5;
	ಪಕ್ಷ DWRITE_RENDERING_MODE_OUTLINE:
	    ಪ್ರತ್ಯಾಗಮ 6;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ವರ್ಗ FontCache {
ಸಾರ್ವಜನಿಕ:
    ಸಂರಚನಾ Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(ರಿಕ್ತ), pTextFormat(ರಿಕ್ತ), fontAscent(0.0f) {}
    };

ಗುಪ್ತ:
    ಪೂರ್ಣಾಙ್ಕ mSize;
    Item *m_ವಸ್ತುಗಳು;

ಸಾರ್ವಜನಿಕ:
    FontCache(ಪೂರ್ಣಾಙ್ಕ size = 2) :
	mSize(size),
	m_ವಸ್ತುಗಳು(ನೂತನ Item[size])
    {
    }

    ~FontCache()
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mSize; ++i)
	    SafeRelease(&m_ವಸ್ತುಗಳು[i].pTextFormat);
	delete[] m_ವಸ್ತುಗಳು;
    }

    ತಾರ್ಕಿಕ get(HFONT hFont, Item &item)
    {
	ಪೂರ್ಣಾಙ್ಕ n = find(hFont);
	ಯದಿ (n < 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	item = m_ವಸ್ತುಗಳು[n];
	slide(n);
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ಶೂನ್ಯ put(ಧ್ರುವ Item& item)
    {
	ಪೂರ್ಣಾಙ್ಕ n = find(item.hFont);
	ಯದಿ (n < 0)
	    n = mSize - 1;
	ಯದಿ (m_ವಸ್ತುಗಳು[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_ವಸ್ತುಗಳು[n].pTextFormat);
	    ಯದಿ (item.pTextFormat != ರಿಕ್ತ)
		item.pTextFormat->AddRef();
	}
	m_ವಸ್ತುಗಳು[n] = item;
	slide(n);
    }

ಗುಪ್ತ:
    ಪೂರ್ಣಾಙ್ಕ find(HFONT hFont)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < mSize; ++i)
	{
	    ಯದಿ (m_ವಸ್ತುಗಳು[i].hFont == hFont)
		ಪ್ರತ್ಯಾಗಮ i;
	}
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಶೂನ್ಯ slide(ಪೂರ್ಣಾಙ್ಕ next_ತುದಿ)
    {
	ಯದಿ (next_ತುದಿ == 0)
	    ಪ್ರತ್ಯಾಗಮ;
	Item tmp = m_ವಸ್ತುಗಳು[next_ತುದಿ];
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = next_ತುದಿ - 1; i >= 0; --i)
	    m_ವಸ್ತುಗಳು[i + 1] = m_ವಸ್ತುಗಳು[i];
	m_ವಸ್ತುಗಳು[0] = tmp;
    }
};

ಗಣನಾ DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ಸಂರಚನಾ DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    ತಾರ್ಕಿಕ mDrawing;
    ತಾರ್ಕಿಕ mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2D1DCRenderTarget *mRT;
    ID2D1GdiInteropRenderTarget *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#endif

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWriteTextFormat *mTextFormat;
    DWRITE_FONT_WEIGHT mFontWeight;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT mFontSize;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2D1_TEXT_ANTIALIAS_MODE mTextAntialiasMode;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    ಪೂರ್ಣಾಙ್ಕ m_Font_Feature_ಎಣಿಕೆ;

    // METHODS

    DWriteContext();

    ಆಭಾಸ ~DWriteContext();

    HRESULT CreateDeviceResources();

    ಶೂನ್ಯ DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ಧ್ರುವ LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ಧ್ರುವ LOGFONTW &logFont);

    ಶೂನ್ಯ SetFont(HFONT hFont);

    ಶೂನ್ಯ Rebind();

    ಶೂನ್ಯ BindDC(HDC hdc, ಧ್ರುವ RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    ಶೂನ್ಯ DrawText(ಧ್ರುವ WCHAR *text, ಪೂರ್ಣಾಙ್ಕ len,
	ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h, ಪೂರ್ಣಾಙ್ಕ cellWidth, COLORREF color,
	UINT fuOptions, ಧ್ರುವ RECT *lprc, ಧ್ರುವ INT *lpDx);

    ಶೂನ್ಯ FillRect(ಧ್ರುವ RECT *rc, COLORREF color);

    ಶೂನ್ಯ Draw_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ x1, ಪೂರ್ಣಾಙ್ಕ y1, ಪೂರ್ಣಾಙ್ಕ x2, ಪೂರ್ಣಾಙ್ಕ y2, COLORREF color);

    ಶೂನ್ಯ SetPixel(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, COLORREF color);

    ಶೂನ್ಯ Scroll(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಧ್ರುವ RECT *rc);

    ಶೂನ್ಯ Flush();

    ಶೂನ್ಯ SetRenderingParams(
	    ಧ್ರುವ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    ಶೂನ್ಯ SetFontFeatures(ಧ್ರುವ DWriteFontFeature *features, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ);
};

ವರ್ಗ AdjustedGlyphRun : ಸಾರ್ವಜನಿಕ DWRITE_GLYPH_RUN
{
ಗುಪ್ತ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

ಸಾರ್ವಜನಿಕ:
    AdjustedGlyphRun(
	    ಧ್ರುವ DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ಧ್ರುವ INT *lpDx,
	    ಪೂರ್ಣಾಙ್ಕ *glyphIndex,
	    ಪೂರ್ಣಾಙ್ಕ cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(ನೂತನ FLOAT[glyphRun->glyph_ಎಣಿಕೆ])
    {
	ಪ್ರತಿಜ್ಞಾ(cellWidth != 0.0f);
	ಪ್ರತಿ (UINT32 i = 0; i < glyphRun->glyph_ಎಣಿಕೆ; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    ಯದಿ (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    ಅನ್ಯಥಾ
	    {
		adjusted = adjustToCell(orig, cellWidth);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~AdjustedGlyphRun()
    {
	mAccum += mDelta;
	delete[] mAdjustedAdvances;
    }

    ಸ್ಥಿರ FLOAT adjustToCell(FLOAT ಮೌಲ್ಯ, FLOAT cellWidth)
    {
	ಪೂರ್ಣಾಙ್ಕ cell_ಎಣಿಕೆ = ಪೂರ್ಣಾಙ್ಕ(floor(ಮೌಲ್ಯ / cellWidth + 0.5f));
	ಯದಿ (cell_ಎಣಿಕೆ < 1)
	    cell_ಎಣಿಕೆ = 1;
	ಪ್ರತ್ಯಾಗಮ cell_ಎಣಿಕೆ * cellWidth;
    }
};

ಸಂರಚನಾ TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ಧ್ರುವ INT *lpDx;
    ಪೂರ್ಣಾಙ್ಕ cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    ಪೂರ್ಣಾಙ್ಕ glyphIndex;
};

ವರ್ಗ TextRenderer FINAL : ಸಾರ್ವಜನಿಕ IDWriteTextRenderer
{
ಸಾರ್ವಜನಿಕ:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_ಎಣಿಕೆ(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    ಆಭಾಸ ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	ಭಿನ್ನ dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ಧ್ರುವ* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ಪ್ರತ್ಯಾಗಮ E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ಧ್ರುವ* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ಪ್ರತ್ಯಾಗಮ E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_ಬಲ_To_ಎಡ UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ಪ್ರತ್ಯಾಗಮ E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull ಶೂನ್ಯ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ಧ್ರುವ* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ಧ್ರುವ* glyphRunDescription UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	TextRendererContext *context =
	    reinterpret_cast<TextRendererContext*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	AdjustedGlyphRun adjustedGlyphRun(glyphRun, context->cellWidth,
		context->offsetX, context->lpDx, &context->glyphIndex,
		context->cchText);

#ifdef FEAT_DIRECTX_COLOR_EMOJI
	ಯದಿ (pDWC_->mDWriteFactory2 != ರಿಕ್ತ)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = ರಿಕ್ತ;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		ರಿಕ್ತ,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		ರಿಕ್ತ,
		0,
		&enumerator);
	    ಯದಿ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		ಯಾವತ್ (hasRun)
		{
		    ಧ್ರುವ DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
		    enumerator->GetCurrentRun(&colorGlyphRun);

		    pDWC_->mBrush->SetColor(colorGlyphRun->runColor);
		    pDWC_->mRT->DrawGlyphRun(
			    D2D1::Point2F(
				colorGlyphRun->baselineOriginX,
				fixedBaselineY),
			    &colorGlyphRun->glyphRun,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_MODE_NATURAL);
		    enumerator->MoveNext(&hasRun);
		}
		SafeRelease(&enumerator);
		ಪ್ರತ್ಯಾಗಮ S_OK;
	    }
	}
#endif

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->DrawGlyphRun(
		D2D1::Point2F(
		    baselineOriginX + context->offsetX,
		    fixedBaselineY),
		&adjustedGlyphRun,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_MODE_NATURAL);
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

ಸಾರ್ವಜನಿಕ:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	ಪ್ರತ್ಯಾಗಮ InterlockedIncrement(&c_Ref_ಎಣಿಕೆ);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_ಎಣಿಕೆ_2 = InterlockedDecrement(&c_Ref_ಎಣಿಕೆ);

	ಯದಿ (new_ಎಣಿಕೆ_2 == 0)
	{
	    delete ಏತತ್;
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಪ್ರತ್ಯಾಗಮ new_ಎಣಿಕೆ_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ಧ್ರುವ& riid,
	ಶೂನ್ಯ** ppvObject)
    {
	ಯದಿ (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = ಏತತ್;
	}
	ಅನ್ಯಥಾ ಯದಿ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ಏತತ್;
	}
	ಅನ್ಯಥಾ ಯದಿ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ಏತತ್;
	}
	ಅನ್ಯಥಾ
	{
	    *ppvObject = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ E_FAIL;
	}

	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

ಗುಪ್ತ:
    long c_Ref_ಎಣಿಕೆ;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ರಿಕ್ತ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ರಿಕ್ತ),
    mDrawing(ಅಸತ್ಯ),
    mFallbackDC(ಅಸತ್ಯ),
    mD2D1Factory(ರಿಕ್ತ),
    mRT(ರಿಕ್ತ),
    mGDIRT(ರಿಕ್ತ),
    mBrush(ರಿಕ್ತ),
    mBitmap(ರಿಕ್ತ),
    mDWriteFactory(ರಿಕ್ತ),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ರಿಕ್ತ),
#endif
    mGdiInterop(ರಿಕ್ತ),
    mRenderingParams(ರಿಕ್ತ),
    mFontCache(8),
    mTextFormat(ರಿಕ್ತ),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_ಎಣಿಕೆ(0)
{
    ZeroMemory(mFontFeatures, ಪರಿಮಾಣ(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ರಿಕ್ತ,
	    reinterpret_cast<ಶೂನ್ಯ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    ಯದಿ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    ಯದಿ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    ಯದಿ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    ಯದಿ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->CreateRenderingParams(&mRenderingParams);
	_RPT2(_CRT_WARN, "CreateRenderingParams: hr=%p p=%p\n", hr,
		mRenderingParams);
    }
}

DWriteContext::~DWriteContext()
{
    SafeRelease(&mTextFormat);
    SafeRelease(&mRenderingParams);
    SafeRelease(&mGdiInterop);
    SafeRelease(&mDWriteFactory);
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    SafeRelease(&mDWriteFactory2);
#endif
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
    SafeRelease(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    ಯದಿ (mRT != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    ಯದಿ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<ಶೂನ್ಯ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    ಯದಿ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    ಯದಿ (SUCCEEDED(hr))
	Rebind();

    ಪ್ರತ್ಯಾಗಮ hr;
}

    ಶೂನ್ಯ
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(ಧ್ರುವ LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ರಿಕ್ತ;

    IDWriteFont *font = ರಿಕ್ತ;
    IDWriteFontFamily *fontFamily = ರಿಕ್ತ;
    IDWriteLocalizedStrings *localizedFamilyNames = ರಿಕ್ತ;
    ಭಿನ್ನ fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    ಯದಿ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    ಯದಿ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    ಯದಿ (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    ಯದಿ (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    ಯದಿ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = ಭಿನ್ನ(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	ಯದಿ (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	ಅನ್ಯಥಾ ಯದಿ (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * ಭಿನ್ನ(fontMetrics.designUnitsPerEm)
		/ ಭಿನ್ನ(fontMetrics.ascent + fontMetrics.descent + fontMetrics.ಸಾಲು_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    ಯದಿ (SUCCEEDED(hr))
    {
	ಯದಿ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    ಯದಿ (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		ರಿಕ್ತ, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    ಯದಿ (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    ಯದಿ (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    ಯದಿ (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    ಯದಿ (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	ಯದಿ (pFontAscent != ರಿಕ್ತ && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = ಭಿನ್ನ(fontMetrics.ascent + fontMetrics.descent);
	    ಯದಿ (totalHeight != 0.0f)
		*pFontAscent = ಭಿನ್ನ(fontMetrics.ascent) / totalHeight;
	    ಅನ್ಯಥಾ
		*pFontAscent = 0.83f; // fallback
	}
    }
    ಅನ್ಯಥಾ
	SafeRelease(&pTextFormat);

    ಪ್ರತ್ಯಾಗಮ hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ಧ್ರುವ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ರಿಕ್ತ;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    ಯದಿ (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    ಪ್ರತ್ಯಾಗಮ hr;
}

    ಶೂನ್ಯ
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    ಯದಿ (mFontCache.get(hFont, item))
    {
	ಯದಿ (item.pTextFormat != ರಿಕ್ತ)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = ಅಸತ್ಯ;
	}
	ಅನ್ಯಥಾ
	    mFallbackDC = ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    ಯದಿ (GetObjectW(hFont, ಪರಿಮಾಣ(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    ಯದಿ (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = ಅಸತ್ಯ;
    }
    ಅನ್ಯಥಾ
	mFallbackDC = ಸತ್ಯ;
    mFontCache.put(item);
}

    ಶೂನ್ಯ
DWriteContext::Rebind()
{
    SafeRelease(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->SetTransform(D2D1::IdentityMatrix());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::SizeU(mBindRect.ಬಲ - mBindRect.ಎಡ,
		mBindRect.bottom - mBindRect.ತುದಿ),
	    props, &mBitmap);
}

    ಶೂನ್ಯ
DWriteContext::BindDC(HDC hdc, ಧ್ರುವ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    ಯದಿ (mRT == ರಿಕ್ತ)
	CreateDeviceResources();
    ಅನ್ಯಥಾ
    {
	Flush();
	Rebind();
    }
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ redraw_later_clear(ಶೂನ್ಯ);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    ವಿಕಲ್ಪ (mode)
    {
	ಸ್ವಭಾವ:
	ಪಕ್ಷ DM_GDI:
	    ಯದಿ (mInteropHDC != ರಿಕ್ತ)
	    {
		mGDIRT->ReleaseDC(ರಿಕ್ತ);
		mInteropHDC = ರಿಕ್ತ;
	    }
	    ಯದಿ (mDrawing)
	    {
		hr = mRT->EndDraw();
		ಯದಿ (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = ಅಸತ್ಯ;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ DM_DIRECTX:
	    ಯದಿ (mInteropHDC != ರಿಕ್ತ)
	    {
		mGDIRT->ReleaseDC(ರಿಕ್ತ);
		mInteropHDC = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mDrawing == ಅಸತ್ಯ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ಸತ್ಯ;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ DM_INTEROP:
	    ಯದಿ (mDrawing == ಅಸತ್ಯ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ಸತ್ಯ;
	    }
	    ಯದಿ (mInteropHDC == ರಿಕ್ತ)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    ಭಙ್ಗ;
    }
    mDMode = mode;
    ಪ್ರತ್ಯಾಗಮ hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ಪ್ರತ್ಯಾಗಮ mBrush;
}

    ಶೂನ್ಯ
DWriteContext::DrawText(ಧ್ರುವ WCHAR *text, ಪೂರ್ಣಾಙ್ಕ len,
	ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h, ಪೂರ್ಣಾಙ್ಕ cellWidth, COLORREF color,
	UINT fuOptions, ಧ್ರುವ RECT *lprc, ಧ್ರುವ INT *lpDx)
{
    ಯದಿ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	ಯದಿ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = ರಿಕ್ತ;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    ಯದಿ ((fuOptions & ETO_CLIPPED) && lprc != ರಿಕ್ತ)
    {
	mRT->ತಳ್ಳು_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ಎಡ), FLOAT(lprc->ತುದಿ),
			FLOAT(lprc->ಬಲ), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    ಯದಿ (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	ಯದಿ (m_Font_Feature_ಎಣಿಕೆ > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ಸ್ಥಿರ ಧ್ರುವ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Defaults = ಪರಿಮಾಣ(defaultFeatures)
		/ ಪರಿಮಾಣ(defaultFeatures[0]);

	    IDWriteTypography *typography = ರಿಕ್ತ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    ಯದಿ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ d = 0; d < ಅಂಕೆ_Defaults; ++d)
		{
		    ಪೂರ್ಣಾಙ್ಕ overridden = 0;
		    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ u = 0; u < m_Font_Feature_ಎಣಿಕೆ; ++u)
		    {
			ಯದಿ ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    ಭಙ್ಗ;
			}
		    }
		    ಯದಿ (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < m_Font_Feature_ಎಣಿಕೆ; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_TAG)mFontFeatures[i].tag,
			mFontFeatures[i].parameter
		    };
		    typography->AddFontFeature(ff);
		}
		textLayout->SetTypography(typography, textRange);
		SafeRelease(&typography);
	    }
	}

	// Calculate baseline using font ascent from font metrics.
	// Do NOT use GetLineMetrics() because it returns different values
	// depending on text content (e.g., when CJK characters trigger
	// font fallback, the metrics change).
	// Use the pre-calculated font ascent for all text to prevent
	// vertical shifts during redraw.
	FLOAT baselineY = roundf(FLOAT(y) + FLOAT(h) * mFontAscent + 0.5);

	TextRenderer renderer(ಏತತ್);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    ಯದಿ ((fuOptions & ETO_CLIPPED) && lprc != ರಿಕ್ತ)
	mRT->ತೆಗೆ_Axis_Aligned_Clip();
}

    ಶೂನ್ಯ
DWriteContext::FillRect(ಧ್ರುವ RECT *rc, COLORREF color)
{
    ಯದಿ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    ಅನ್ಯಥಾ
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ಎಡ), FLOAT(rc->ತುದಿ),
		    FLOAT(rc->ಬಲ), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::Draw_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ x1, ಪೂರ್ಣಾಙ್ಕ y1, ಪೂರ್ಣಾಙ್ಕ x2, ಪೂರ್ಣಾಙ್ಕ y2, COLORREF color)
{
    ಯದಿ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, ರಿಕ್ತ);
	::ಸಾಲು_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    ಅನ್ಯಥಾ
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_ಸಾಲು(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::SetPixel(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, COLORREF color)
{
    ಯದಿ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    ಅನ್ಯಥಾ
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_ಸಾಲು(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    ಶೂನ್ಯ
DWriteContext::Scroll(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಧ್ರುವ RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    ಯದಿ (x >= 0)
    {
	srcRect.ಎಡ = rc->ಎಡ;
	srcRect.ಬಲ = rc->ಬಲ - x;
	destPoint.x = rc->ಎಡ + x;
    }
    ಅನ್ಯಥಾ
    {
	srcRect.ಎಡ = rc->ಎಡ - x;
	srcRect.ಬಲ = rc->ಬಲ;
	destPoint.x = rc->ಎಡ;
    }
    ಯದಿ (y >= 0)
    {
	srcRect.ತುದಿ = rc->ತುದಿ;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->ತುದಿ + y;
    }
    ಅನ್ಯಥಾ
    {
	srcRect.ತುದಿ = rc->ತುದಿ - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->ತುದಿ;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.ಬಲ - srcRect.ಎಡ),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.ತುದಿ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    ಶೂನ್ಯ
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    ಶೂನ್ಯ
DWriteContext::SetRenderingParams(
	ಧ್ರುವ DWriteRenderingParams *params)
{
    ಯದಿ (mDWriteFactory == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    IDWriteRenderingParams *renderingParams = ರಿಕ್ತ;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    ಯದಿ (params != ರಿಕ್ತ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    ಅನ್ಯಥಾ
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    ಯದಿ (SUCCEEDED(hr) && renderingParams != ರಿಕ್ತ)
    {
	SafeRelease(&mRenderingParams);
	mRenderingParams = renderingParams;
	mTextAntialiasMode = textAntialiasMode;

	Flush();
	mRT->SetTextRenderingParams(mRenderingParams);
	mRT->SetTextAntialiasMode(mTextAntialiasMode);
    }
}

    DWriteRenderingParams *
DWriteContext::GetRenderingParams(
	DWriteRenderingParams *params)
{
    ಯದಿ (params != ರಿಕ್ತ && mRenderingParams != ರಿಕ್ತ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    ಪ್ರತ್ಯಾಗಮ params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    ಶೂನ್ಯ
DWrite_Init(ಶೂನ್ಯ)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    ಯದಿ (hD2D1DLL == ರಿಕ್ತ || hDWriteDLL == ರಿಕ್ತ)
    {
	DWrite_Final();
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)GetProcAddress(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)GetProcAddress(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)GetProcAddress(hDWriteDLL,
	    "DWriteCreateFactory");
#endif
}

    ಶೂನ್ಯ
DWrite_Final(ಶೂನ್ಯ)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ರಿಕ್ತ;
    pD2D1CreateFactory = ರಿಕ್ತ;
    pDWriteCreateFactory = ರಿಕ್ತ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(ಶೂನ್ಯ)
{
#ifdef DYNAMIC_DIRECTX
    ಯದಿ (pGetUserDefaultLocaleName == ರಿಕ್ತ || pD2D1CreateFactory == ರಿಕ್ತ
	    || pDWriteCreateFactory == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    ಪ್ರತ್ಯಾಗಮ ನೂತನ DWriteContext();
}

    ಶೂನ್ಯ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ಧ್ರುವ RECT *rect)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->BindDC(hdc, rect);
}

    ಶೂನ್ಯ
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->SetFont(hFont);
}

    ಶೂನ್ಯ
DWriteContext_DrawText(
	DWriteContext *ctx,
	ಧ್ರುವ WCHAR *text,
	ಪೂರ್ಣಾಙ್ಕ len,
	ಪೂರ್ಣಾಙ್ಕ x,
	ಪೂರ್ಣಾಙ್ಕ y,
	ಪೂರ್ಣಾಙ್ಕ w,
	ಪೂರ್ಣಾಙ್ಕ h,
	ಪೂರ್ಣಾಙ್ಕ cellWidth,
	COLORREF color,
	UINT fuOptions,
	ಧ್ರುವ RECT *lprc,
	ಧ್ರುವ INT *lpDx)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    ಶೂನ್ಯ
DWriteContext_FillRect(DWriteContext *ctx, ಧ್ರುವ RECT *rc, COLORREF color)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->FillRect(rc, color);
}

    ಶೂನ್ಯ
DWrite_Context_Draw_ಸಾಲು(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x1, ಪೂರ್ಣಾಙ್ಕ y1, ಪೂರ್ಣಾಙ್ಕ x2, ಪೂರ್ಣಾಙ್ಕ y2,
	COLORREF color)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->Draw_ಸಾಲು(x1, y1, x2, y2, color);
}

    ಶೂನ್ಯ
DWriteContext_SetPixel(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, COLORREF color)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->SetPixel(x, y, color);
}

    ಶೂನ್ಯ
DWriteContext_Scroll(DWriteContext *ctx, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಧ್ರುವ RECT *rc)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->Scroll(x, y, rc);
}

    ಶೂನ್ಯ
DWriteContext_Flush(DWriteContext *ctx)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->Flush();
}

    ಶೂನ್ಯ
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    ಶೂನ್ಯ
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ಧ್ರುವ DWriteRenderingParams *params)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ctx->GetRenderingParams(params);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಶೂನ್ಯ
DWriteContext::SetFontFeatures(
	ಧ್ರುವ DWriteFontFeature *features, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಯದಿ (ಎಣಿಕೆ > DWRITE_MAX_FONT_FEATURES)
	ಎಣಿಕೆ = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ಎಣಿಕೆ = ಎಣಿಕೆ;
    ಯದಿ (ಎಣಿಕೆ > 0 && features != ರಿಕ್ತ)
	memcpy(mFontFeatures, features, ಪರಿಮಾಣ(DWriteFontFeature) * ಎಣಿಕೆ);
}

    ಶೂನ್ಯ
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ಧ್ರುವ DWriteFontFeature *features,
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಯದಿ (ctx != ರಿಕ್ತ)
	ctx->SetFontFeatures(features, ಎಣಿಕೆ);
}
