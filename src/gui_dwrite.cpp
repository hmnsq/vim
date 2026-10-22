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
# যদি WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#অন্তর্ভাৱ <windows.h>
#অন্তর্ভাৱ <crtdbg.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <math.h>
#অন্তর্ভাৱ <d2d1.h>
#অন্তর্ভাৱ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#যদি defined(_MSC_VER) && (_MSC_VER < 1700)
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
#অন্যথা
# include <dwrite.h>
#endif

#অন্তর্ভাৱ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#যদি (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#অন্যথা
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
বাহ্য "C" HINSTANCE vimLoadLib(ধ্রুৱ ৱর্ণ *name);

প্রকারনির্দেশ পূর্ণাঙ্ক (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, পূর্ণাঙ্ক);
প্রকারনির্দেশ HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ধ্রুৱ D2D1_FACTORY_OPTIONS *, শূন্য **);
প্রকারনির্দেশ HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

স্থির HINSTANCE hD2D1DLL = রিক্ত;
স্থির HINSTANCE hDWriteDLL = রিক্ত;

স্থির PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = রিক্ত;
স্থির PD2D1CREATEFACTORY pD2D1CreateFactory = রিক্ত;
স্থির PDWRITECREATEFACTORY pDWriteCreateFactory = রিক্ত;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    স্থির শূন্য
unload(HINSTANCE &hinst)
{
    যদি (hinst != রিক্ত)
    {
	FreeLibrary(hinst);
	hinst = রিক্ত;
    }
}
#endif // DYNAMIC_DIRECTX

template <শ্ৰেণী T> অন্তঃস্থ শূন্য SafeRelease(T **ppT)
{
    যদি (*ppT)
    {
	(*ppT)->Release();
	*ppT = রিক্ত;
    }
}

    স্থির DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(পূর্ণাঙ্ক মান)
{
    ৱিকল্প (মান)
    {
	স্ৱভাৱ:
	পক্ষ 0:
	    ঘূৰাই DWRITE_PIXEL_GEOMETRY_FLAT;
	পক্ষ 1:
	    ঘূৰাই DWRITE_PIXEL_GEOMETRY_RGB;
	পক্ষ 2:
	    ঘূৰাই DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    স্থির পূর্ণাঙ্ক
ToInt(DWRITE_PIXEL_GEOMETRY মান)
{
    ৱিকল্প (মান)
    {
	পক্ষ DWRITE_PIXEL_GEOMETRY_FLAT:
	    ঘূৰাই 0;
	পক্ষ DWRITE_PIXEL_GEOMETRY_RGB:
	    ঘূৰাই 1;
	পক্ষ DWRITE_PIXEL_GEOMETRY_BGR:
	    ঘূৰাই 2;
	স্ৱভাৱ:
	    ঘূৰাই -1;
    }
}

    স্থির DWRITE_RENDERING_MODE
ToRenderingMode(পূর্ণাঙ্ক মান)
{
    ৱিকল্প (মান)
    {
	স্ৱভাৱ:
	পক্ষ 0:
	    ঘূৰাই DWRITE_RENDERING_MODE_DEFAULT;
	পক্ষ 1:
	    ঘূৰাই DWRITE_RENDERING_MODE_ALIASED;
	পক্ষ 2:
	    ঘূৰাই DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	পক্ষ 3:
	    ঘূৰাই DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	পক্ষ 4:
	    ঘূৰাই DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	পক্ষ 5:
	    ঘূৰাই DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	পক্ষ 6:
	    ঘূৰাই DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    স্থির D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(পূর্ণাঙ্ক মান)
{
    ৱিকল্প (মান)
    {
	স্ৱভাৱ:
	পক্ষ 0:
	    ঘূৰাই D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	পক্ষ 1:
	    ঘূৰাই D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	পক্ষ 2:
	    ঘূৰাই D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	পক্ষ 3:
	    ঘূৰাই D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    স্থির পূর্ণাঙ্ক
ToInt(DWRITE_RENDERING_MODE মান)
{
    ৱিকল্প (মান)
    {
	পক্ষ DWRITE_RENDERING_MODE_DEFAULT:
	    ঘূৰাই 0;
	পক্ষ DWRITE_RENDERING_MODE_ALIASED:
	    ঘূৰাই 1;
	পক্ষ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    ঘূৰাই 2;
	পক্ষ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    ঘূৰাই 3;
	পক্ষ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    ঘূৰাই 4;
	পক্ষ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    ঘূৰাই 5;
	পক্ষ DWRITE_RENDERING_MODE_OUTLINE:
	    ঘূৰাই 6;
	স্ৱভাৱ:
	    ঘূৰাই -1;
    }
}

শ্ৰেণী FontCache {
সার্ৱজনিক:
    সংরচনা Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(রিক্ত), pTextFormat(রিক্ত), fontAscent(0.0f) {}
    };

গুপ্ত:
    পূর্ণাঙ্ক mSize;
    Item *m_বস্তুসমূহ;

সার্ৱজনিক:
    FontCache(পূর্ণাঙ্ক size = 2) :
	mSize(size),
	m_বস্তুসমূহ(নূতন Item[size])
    {
    }

    ~FontCache()
    {
	বাবে (পূর্ণাঙ্ক i = 0; i < mSize; ++i)
	    SafeRelease(&m_বস্তুসমূহ[i].pTextFormat);
	delete[] m_বস্তুসমূহ;
    }

    তার্কিক get(HFONT hFont, Item &item)
    {
	পূর্ণাঙ্ক n = find(hFont);
	যদি (n < 0)
	    ঘূৰাই মিছা;
	item = m_বস্তুসমূহ[n];
	slide(n);
	ঘূৰাই সঁচা;
    }

    শূন্য put(ধ্রুৱ Item& item)
    {
	পূর্ণাঙ্ক n = find(item.hFont);
	যদি (n < 0)
	    n = mSize - 1;
	যদি (m_বস্তুসমূহ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_বস্তুসমূহ[n].pTextFormat);
	    যদি (item.pTextFormat != রিক্ত)
		item.pTextFormat->AddRef();
	}
	m_বস্তুসমূহ[n] = item;
	slide(n);
    }

গুপ্ত:
    পূর্ণাঙ্ক find(HFONT hFont)
    {
	বাবে (পূর্ণাঙ্ক i = 0; i < mSize; ++i)
	{
	    যদি (m_বস্তুসমূহ[i].hFont == hFont)
		ঘূৰাই i;
	}
	ঘূৰাই -1;
    }

    শূন্য slide(পূর্ণাঙ্ক next_শীৰ্ষ)
    {
	যদি (next_শীৰ্ষ == 0)
	    ঘূৰাই;
	Item tmp = m_বস্তুসমূহ[next_শীৰ্ষ];
	বাবে (পূর্ণাঙ্ক i = next_শীৰ্ষ - 1; i >= 0; --i)
	    m_বস্তুসমূহ[i + 1] = m_বস্তুসমূহ[i];
	m_বস্তুসমূহ[0] = tmp;
    }
};

গণনা DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

সংরচনা DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    তার্কিক mDrawing;
    তার্কিক mFallbackDC;

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
    পূর্ণাঙ্ক m_Font_Feature_গুনতি;

    // METHODS

    DWriteContext();

    আভাস ~DWriteContext();

    HRESULT CreateDeviceResources();

    শূন্য DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ধ্রুৱ LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ধ্রুৱ LOGFONTW &logFont);

    শূন্য SetFont(HFONT hFont);

    শূন্য Rebind();

    শূন্য BindDC(HDC hdc, ধ্রুৱ RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    শূন্য DrawText(ধ্রুৱ WCHAR *text, পূর্ণাঙ্ক len,
	পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h, পূর্ণাঙ্ক cellWidth, COLORREF color,
	UINT fuOptions, ধ্রুৱ RECT *lprc, ধ্রুৱ INT *lpDx);

    শূন্য FillRect(ধ্রুৱ RECT *rc, COLORREF color);

    শূন্য Draw_সাৰি(পূর্ণাঙ্ক x1, পূর্ণাঙ্ক y1, পূর্ণাঙ্ক x2, পূর্ণাঙ্ক y2, COLORREF color);

    শূন্য SetPixel(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, COLORREF color);

    শূন্য Scroll(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, ধ্রুৱ RECT *rc);

    শূন্য Flush();

    শূন্য SetRenderingParams(
	    ধ্রুৱ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    শূন্য SetFontFeatures(ধ্রুৱ DWriteFontFeature *features, পূর্ণাঙ্ক গুনতি);
};

শ্ৰেণী AdjustedGlyphRun : সার্ৱজনিক DWRITE_GLYPH_RUN
{
গুপ্ত:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

সার্ৱজনিক:
    AdjustedGlyphRun(
	    ধ্রুৱ DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ধ্রুৱ INT *lpDx,
	    পূর্ণাঙ্ক *glyphIndex,
	    পূর্ণাঙ্ক cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(নূতন FLOAT[glyphRun->glyph_গুনতি])
    {
	প্রতিজ্ঞা(cellWidth != 0.0f);
	বাবে (UINT32 i = 0; i < glyphRun->glyph_গুনতি; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    যদি (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    অন্যথা
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

    স্থির FLOAT adjustToCell(FLOAT মান, FLOAT cellWidth)
    {
	পূর্ণাঙ্ক cell_গুনতি = পূর্ণাঙ্ক(floor(মান / cellWidth + 0.5f));
	যদি (cell_গুনতি < 1)
	    cell_গুনতি = 1;
	ঘূৰাই cell_গুনতি * cellWidth;
    }
};

সংরচনা TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ধ্রুৱ INT *lpDx;
    পূর্ণাঙ্ক cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    পূর্ণাঙ্ক glyphIndex;
};

শ্ৰেণী TextRenderer FINAL : সার্ৱজনিক IDWriteTextRenderer
{
সার্ৱজনিক:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_গুনতি(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    আভাস ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ঘূৰাই S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	ঘূৰাই S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	ভিন্ন dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	ঘূৰাই S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ধ্রুৱ* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ঘূৰাই E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ধ্রুৱ* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ঘূৰাই E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_ডান_To_বাম UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ঘূৰাই E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull শূন্য* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ধ্রুৱ* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ধ্রুৱ* glyphRunDescription UNUSED,
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
	যদি (pDWC_->mDWriteFactory2 != রিক্ত)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = রিক্ত;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		রিক্ত,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		রিক্ত,
		0,
		&enumerator);
	    যদি (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		যেতিয়ালৈ (hasRun)
		{
		    ধ্রুৱ DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		ঘূৰাই S_OK;
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
	ঘূৰাই S_OK;
    }

সার্ৱজনিক:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	ঘূৰাই InterlockedIncrement(&c_Ref_গুনতি);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_গুনতি_2 = InterlockedDecrement(&c_Ref_গুনতি);

	যদি (new_গুনতি_2 == 0)
	{
	    delete এতত্;
	    ঘূৰাই 0;
	}
	ঘূৰাই new_গুনতি_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ধ্রুৱ& riid,
	শূন্য** ppvObject)
    {
	যদি (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = এতত্;
	}
	অন্যথা যদি (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = এতত্;
	}
	অন্যথা যদি (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = এতত্;
	}
	অন্যথা
	{
	    *ppvObject = রিক্ত;
	    ঘূৰাই E_FAIL;
	}

	ঘূৰাই S_OK;
    }

গুপ্ত:
    long c_Ref_গুনতি;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(রিক্ত),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(রিক্ত),
    mDrawing(মিছা),
    mFallbackDC(মিছা),
    mD2D1Factory(রিক্ত),
    mRT(রিক্ত),
    mGDIRT(রিক্ত),
    mBrush(রিক্ত),
    mBitmap(রিক্ত),
    mDWriteFactory(রিক্ত),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(রিক্ত),
#endif
    mGdiInterop(রিক্ত),
    mRenderingParams(রিক্ত),
    mFontCache(8),
    mTextFormat(রিক্ত),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_গুনতি(0)
{
    ZeroMemory(mFontFeatures, পরিমাণ(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), রিক্ত,
	    reinterpret_cast<শূন্য**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    যদি (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    যদি (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    যদি (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    যদি (SUCCEEDED(hr))
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

    যদি (mRT != রিক্ত)
	ঘূৰাই S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    যদি (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<শূন্য**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    যদি (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    যদি (SUCCEEDED(hr))
	Rebind();

    ঘূৰাই hr;
}

    শূন্য
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(ধ্রুৱ LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = রিক্ত;

    IDWriteFont *font = রিক্ত;
    IDWriteFontFamily *fontFamily = রিক্ত;
    IDWriteLocalizedStrings *localizedFamilyNames = রিক্ত;
    ভিন্ন fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    যদি (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    যদি (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    যদি (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    যদি (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    যদি (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = ভিন্ন(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	যদি (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	অন্যথা যদি (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * ভিন্ন(fontMetrics.designUnitsPerEm)
		/ ভিন্ন(fontMetrics.ascent + fontMetrics.descent + fontMetrics.সাৰি_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    যদি (SUCCEEDED(hr))
    {
	যদি (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    যদি (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		রিক্ত, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    যদি (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    যদি (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    যদি (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    যদি (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	যদি (pFontAscent != রিক্ত && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = ভিন্ন(fontMetrics.ascent + fontMetrics.descent);
	    যদি (totalHeight != 0.0f)
		*pFontAscent = ভিন্ন(fontMetrics.ascent) / totalHeight;
	    অন্যথা
		*pFontAscent = 0.83f; // fallback
	}
    }
    অন্যথা
	SafeRelease(&pTextFormat);

    ঘূৰাই hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ধ্রুৱ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = রিক্ত;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    যদি (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    ঘূৰাই hr;
}

    শূন্য
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    যদি (mFontCache.get(hFont, item))
    {
	যদি (item.pTextFormat != রিক্ত)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = মিছা;
	}
	অন্যথা
	    mFallbackDC = সঁচা;
	ঘূৰাই;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    যদি (GetObjectW(hFont, পরিমাণ(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    যদি (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = মিছা;
    }
    অন্যথা
	mFallbackDC = সঁচা;
    mFontCache.put(item);
}

    শূন্য
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
	    D2D1::SizeU(mBindRect.ডান - mBindRect.বাম,
		mBindRect.bottom - mBindRect.শীৰ্ষ),
	    props, &mBitmap);
}

    শূন্য
DWriteContext::BindDC(HDC hdc, ধ্রুৱ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    যদি (mRT == রিক্ত)
	CreateDeviceResources();
    অন্যথা
    {
	Flush();
	Rebind();
    }
}

বাহ্য "C" শূন্য redraw_later_clear(শূন্য);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    ৱিকল্প (mode)
    {
	স্ৱভাৱ:
	পক্ষ DM_GDI:
	    যদি (mInteropHDC != রিক্ত)
	    {
		mGDIRT->ReleaseDC(রিক্ত);
		mInteropHDC = রিক্ত;
	    }
	    যদি (mDrawing)
	    {
		hr = mRT->EndDraw();
		যদি (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = মিছা;
	    }
	    ভঙ্গ;

	পক্ষ DM_DIRECTX:
	    যদি (mInteropHDC != রিক্ত)
	    {
		mGDIRT->ReleaseDC(রিক্ত);
		mInteropHDC = রিক্ত;
	    }
	    অন্যথা যদি (mDrawing == মিছা)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = সঁচা;
	    }
	    ভঙ্গ;

	পক্ষ DM_INTEROP:
	    যদি (mDrawing == মিছা)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = সঁচা;
	    }
	    যদি (mInteropHDC == রিক্ত)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    ভঙ্গ;
    }
    mDMode = mode;
    ঘূৰাই hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ঘূৰাই mBrush;
}

    শূন্য
DWriteContext::DrawText(ধ্রুৱ WCHAR *text, পূর্ণাঙ্ক len,
	পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h, পূর্ণাঙ্ক cellWidth, COLORREF color,
	UINT fuOptions, ধ্রুৱ RECT *lprc, ধ্রুৱ INT *lpDx)
{
    যদি (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	যদি (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ঘূৰাই;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = রিক্ত;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    যদি ((fuOptions & ETO_CLIPPED) && lprc != রিক্ত)
    {
	mRT->ঠেলো_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->বাম), FLOAT(lprc->শীৰ্ষ),
			FLOAT(lprc->ডান), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    যদি (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	যদি (m_Font_Feature_গুনতি > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    স্থির ধ্রুৱ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    স্থির ধ্রুৱ পূর্ণাঙ্ক অঙ্ক_Defaults = পরিমাণ(defaultFeatures)
		/ পরিমাণ(defaultFeatures[0]);

	    IDWriteTypography *typography = রিক্ত;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    যদি (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		বাবে (পূর্ণাঙ্ক d = 0; d < অঙ্ক_Defaults; ++d)
		{
		    পূর্ণাঙ্ক overridden = 0;
		    বাবে (পূর্ণাঙ্ক u = 0; u < m_Font_Feature_গুনতি; ++u)
		    {
			যদি ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    ভঙ্গ;
			}
		    }
		    যদি (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		বাবে (পূর্ণাঙ্ক i = 0; i < m_Font_Feature_গুনতি; ++i)
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

	TextRenderer renderer(এতত্);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    যদি ((fuOptions & ETO_CLIPPED) && lprc != রিক্ত)
	mRT->বেৰ_কৰো_Axis_Aligned_Clip();
}

    শূন্য
DWriteContext::FillRect(ধ্রুৱ RECT *rc, COLORREF color)
{
    যদি (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    অন্যথা
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->বাম), FLOAT(rc->শীৰ্ষ),
		    FLOAT(rc->ডান), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::Draw_সাৰি(পূর্ণাঙ্ক x1, পূর্ণাঙ্ক y1, পূর্ণাঙ্ক x2, পূর্ণাঙ্ক y2, COLORREF color)
{
    যদি (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, রিক্ত);
	::সাৰি_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    অন্যথা
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_সাৰি(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::SetPixel(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, COLORREF color)
{
    যদি (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    অন্যথা
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_সাৰি(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::Scroll(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, ধ্রুৱ RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    যদি (x >= 0)
    {
	srcRect.বাম = rc->বাম;
	srcRect.ডান = rc->ডান - x;
	destPoint.x = rc->বাম + x;
    }
    অন্যথা
    {
	srcRect.বাম = rc->বাম - x;
	srcRect.ডান = rc->ডান;
	destPoint.x = rc->বাম;
    }
    যদি (y >= 0)
    {
	srcRect.শীৰ্ষ = rc->শীৰ্ষ;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->শীৰ্ষ + y;
    }
    অন্যথা
    {
	srcRect.শীৰ্ষ = rc->শীৰ্ষ - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->শীৰ্ষ;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.ডান - srcRect.বাম),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.শীৰ্ষ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    শূন্য
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    শূন্য
DWriteContext::SetRenderingParams(
	ধ্রুৱ DWriteRenderingParams *params)
{
    যদি (mDWriteFactory == রিক্ত)
	ঘূৰাই;

    IDWriteRenderingParams *renderingParams = রিক্ত;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    যদি (params != রিক্ত)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    অন্যথা
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    যদি (SUCCEEDED(hr) && renderingParams != রিক্ত)
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
    যদি (params != রিক্ত && mRenderingParams != রিক্ত)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    ঘূৰাই params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    শূন্য
DWrite_Init(শূন্য)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    যদি (hD2D1DLL == রিক্ত || hDWriteDLL == রিক্ত)
    {
	DWrite_Final();
	ঘূৰাই;
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

    শূন্য
DWrite_Final(শূন্য)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = রিক্ত;
    pD2D1CreateFactory = রিক্ত;
    pDWriteCreateFactory = রিক্ত;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(শূন্য)
{
#ifdef DYNAMIC_DIRECTX
    যদি (pGetUserDefaultLocaleName == রিক্ত || pD2D1CreateFactory == রিক্ত
	    || pDWriteCreateFactory == রিক্ত)
	ঘূৰাই রিক্ত;
#endif
    ঘূৰাই নূতন DWriteContext();
}

    শূন্য
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ধ্রুৱ RECT *rect)
{
    যদি (ctx != রিক্ত)
	ctx->BindDC(hdc, rect);
}

    শূন্য
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    যদি (ctx != রিক্ত)
	ctx->SetFont(hFont);
}

    শূন্য
DWriteContext_DrawText(
	DWriteContext *ctx,
	ধ্রুৱ WCHAR *text,
	পূর্ণাঙ্ক len,
	পূর্ণাঙ্ক x,
	পূর্ণাঙ্ক y,
	পূর্ণাঙ্ক w,
	পূর্ণাঙ্ক h,
	পূর্ণাঙ্ক cellWidth,
	COLORREF color,
	UINT fuOptions,
	ধ্রুৱ RECT *lprc,
	ধ্রুৱ INT *lpDx)
{
    যদি (ctx != রিক্ত)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    শূন্য
DWriteContext_FillRect(DWriteContext *ctx, ধ্রুৱ RECT *rc, COLORREF color)
{
    যদি (ctx != রিক্ত)
	ctx->FillRect(rc, color);
}

    শূন্য
DWrite_Context_Draw_সাৰি(DWriteContext *ctx, পূর্ণাঙ্ক x1, পূর্ণাঙ্ক y1, পূর্ণাঙ্ক x2, পূর্ণাঙ্ক y2,
	COLORREF color)
{
    যদি (ctx != রিক্ত)
	ctx->Draw_সাৰি(x1, y1, x2, y2, color);
}

    শূন্য
DWriteContext_SetPixel(DWriteContext *ctx, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, COLORREF color)
{
    যদি (ctx != রিক্ত)
	ctx->SetPixel(x, y, color);
}

    শূন্য
DWriteContext_Scroll(DWriteContext *ctx, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, ধ্রুৱ RECT *rc)
{
    যদি (ctx != রিক্ত)
	ctx->Scroll(x, y, rc);
}

    শূন্য
DWriteContext_Flush(DWriteContext *ctx)
{
    যদি (ctx != রিক্ত)
	ctx->Flush();
}

    শূন্য
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    শূন্য
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ধ্রুৱ DWriteRenderingParams *params)
{
    যদি (ctx != রিক্ত)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    যদি (ctx != রিক্ত)
	ঘূৰাই ctx->GetRenderingParams(params);
    অন্যথা
	ঘূৰাই রিক্ত;
}

    শূন্য
DWriteContext::SetFontFeatures(
	ধ্রুৱ DWriteFontFeature *features, পূর্ণাঙ্ক গুনতি)
{
    যদি (গুনতি > DWRITE_MAX_FONT_FEATURES)
	গুনতি = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_গুনতি = গুনতি;
    যদি (গুনতি > 0 && features != রিক্ত)
	memcpy(mFontFeatures, features, পরিমাণ(DWriteFontFeature) * গুনতি);
}

    শূন্য
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ধ্রুৱ DWriteFontFeature *features,
	পূর্ণাঙ্ক গুনতি)
{
    যদি (ctx != রিক্ত)
	ctx->SetFontFeatures(features, গুনতি);
}
