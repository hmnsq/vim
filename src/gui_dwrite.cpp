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

#অন্তর্ভাব <windows.h>
#অন্তর্ভাব <crtdbg.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <math.h>
#অন্তর্ভাব <d2d1.h>
#অন্তর্ভাব <d2d1helper.h>

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

#অন্তর্ভাব "gui_dwrite.h"

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
বাহ্যিক "C" HINSTANCE vimLoadLib(ধ্রুবক অক্ষর *name);

প্রকারলেখ পূর্ণসংখ্যা (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, পূর্ণসংখ্যা);
প্রকারলেখ HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ধ্রুবক D2D1_FACTORY_OPTIONS *, শূন্য **);
প্রকারলেখ HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

স্থির HINSTANCE hD2D1DLL = ফাঁকা;
স্থির HINSTANCE hDWriteDLL = ফাঁকা;

স্থির PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ফাঁকা;
স্থির PD2D1CREATEFACTORY pD2D1CreateFactory = ফাঁকা;
স্থির PDWRITECREATEFACTORY pDWriteCreateFactory = ফাঁকা;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    স্থির শূন্য
unload(HINSTANCE &hinst)
{
    যদি (hinst != ফাঁকা)
    {
	FreeLibrary(hinst);
	hinst = ফাঁকা;
    }
}
#endif // DYNAMIC_DIRECTX

template <শ্রেণী T> অন্তঃরেখ শূন্য SafeRelease(T **ppT)
{
    যদি (*ppT)
    {
	(*ppT)->Release();
	*ppT = ফাঁকা;
    }
}

    স্থির DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(পূর্ণসংখ্যা মান)
{
    নির্বাচন (মান)
    {
	পূর্বনির্ধারিত:
	ক্ষেত্র 0:
	    ফেরত DWRITE_PIXEL_GEOMETRY_FLAT;
	ক্ষেত্র 1:
	    ফেরত DWRITE_PIXEL_GEOMETRY_RGB;
	ক্ষেত্র 2:
	    ফেরত DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    স্থির পূর্ণসংখ্যা
ToInt(DWRITE_PIXEL_GEOMETRY মান)
{
    নির্বাচন (মান)
    {
	ক্ষেত্র DWRITE_PIXEL_GEOMETRY_FLAT:
	    ফেরত 0;
	ক্ষেত্র DWRITE_PIXEL_GEOMETRY_RGB:
	    ফেরত 1;
	ক্ষেত্র DWRITE_PIXEL_GEOMETRY_BGR:
	    ফেরত 2;
	পূর্বনির্ধারিত:
	    ফেরত -1;
    }
}

    স্থির DWRITE_RENDERING_MODE
ToRenderingMode(পূর্ণসংখ্যা মান)
{
    নির্বাচন (মান)
    {
	পূর্বনির্ধারিত:
	ক্ষেত্র 0:
	    ফেরত DWRITE_RENDERING_MODE_DEFAULT;
	ক্ষেত্র 1:
	    ফেরত DWRITE_RENDERING_MODE_ALIASED;
	ক্ষেত্র 2:
	    ফেরত DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	ক্ষেত্র 3:
	    ফেরত DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	ক্ষেত্র 4:
	    ফেরত DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	ক্ষেত্র 5:
	    ফেরত DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	ক্ষেত্র 6:
	    ফেরত DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    স্থির D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(পূর্ণসংখ্যা মান)
{
    নির্বাচন (মান)
    {
	পূর্বনির্ধারিত:
	ক্ষেত্র 0:
	    ফেরত D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	ক্ষেত্র 1:
	    ফেরত D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	ক্ষেত্র 2:
	    ফেরত D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	ক্ষেত্র 3:
	    ফেরত D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    স্থির পূর্ণসংখ্যা
ToInt(DWRITE_RENDERING_MODE মান)
{
    নির্বাচন (মান)
    {
	ক্ষেত্র DWRITE_RENDERING_MODE_DEFAULT:
	    ফেরত 0;
	ক্ষেত্র DWRITE_RENDERING_MODE_ALIASED:
	    ফেরত 1;
	ক্ষেত্র DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    ফেরত 2;
	ক্ষেত্র DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    ফেরত 3;
	ক্ষেত্র DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    ফেরত 4;
	ক্ষেত্র DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    ফেরত 5;
	ক্ষেত্র DWRITE_RENDERING_MODE_OUTLINE:
	    ফেরত 6;
	পূর্বনির্ধারিত:
	    ফেরত -1;
    }
}

শ্রেণী FontCache {
সর্বজনীন:
    কাঠামো Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(ফাঁকা), pTextFormat(ফাঁকা), fontAscent(0.0f) {}
    };

ব্যক্তিগত:
    পূর্ণসংখ্যা m_আকার;
    Item *m_বস্তুসমূহ;

সর্বজনীন:
    FontCache(পূর্ণসংখ্যা আকার_2 = 2) :
	m_আকার(আকার_2),
	m_বস্তুসমূহ(নতুন Item[আকার_2])
    {
    }

    ~FontCache()
    {
	জন্য (পূর্ণসংখ্যা i = 0; i < m_আকার; ++i)
	    SafeRelease(&m_বস্তুসমূহ[i].pTextFormat);
	delete[] m_বস্তুসমূহ;
    }

    যৌক্তিক get(HFONT hFont, Item &item)
    {
	পূর্ণসংখ্যা n = find(hFont);
	যদি (n < 0)
	    ফেরত মিথ্যা;
	item = m_বস্তুসমূহ[n];
	slide(n);
	ফেরত সত্য;
    }

    শূন্য put(ধ্রুবক Item& item)
    {
	পূর্ণসংখ্যা n = find(item.hFont);
	যদি (n < 0)
	    n = m_আকার - 1;
	যদি (m_বস্তুসমূহ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_বস্তুসমূহ[n].pTextFormat);
	    যদি (item.pTextFormat != ফাঁকা)
		item.pTextFormat->AddRef();
	}
	m_বস্তুসমূহ[n] = item;
	slide(n);
    }

ব্যক্তিগত:
    পূর্ণসংখ্যা find(HFONT hFont)
    {
	জন্য (পূর্ণসংখ্যা i = 0; i < m_আকার; ++i)
	{
	    যদি (m_বস্তুসমূহ[i].hFont == hFont)
		ফেরত i;
	}
	ফেরত -1;
    }

    শূন্য slide(পূর্ণসংখ্যা পরবর্তী_শীর্ষ)
    {
	যদি (পরবর্তী_শীর্ষ == 0)
	    ফেরত;
	Item tmp = m_বস্তুসমূহ[পরবর্তী_শীর্ষ];
	জন্য (পূর্ণসংখ্যা i = পরবর্তী_শীর্ষ - 1; i >= 0; --i)
	    m_বস্তুসমূহ[i + 1] = m_বস্তুসমূহ[i];
	m_বস্তুসমূহ[0] = tmp;
    }
};

গণনা DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

কাঠামো DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    যৌক্তিক mDrawing;
    যৌক্তিক mFallbackDC;

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
    FLOAT m_Font_আকার;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2D1_TEXT_ANTIALIAS_MODE mTextAntialiasMode;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    পূর্ণসংখ্যা m_Font_Feature_গুনতি;

    // METHODS

    DWriteContext();

    আভাসী ~DWriteContext();

    HRESULT CreateDeviceResources();

    শূন্য DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ধ্রুবক LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ধ্রুবক LOGFONTW &logFont);

    শূন্য SetFont(HFONT hFont);

    শূন্য Rebind();

    শূন্য BindDC(HDC hdc, ধ্রুবক RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    শূন্য DrawText(ধ্রুবক WCHAR *text, পূর্ণসংখ্যা len,
	পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h, পূর্ণসংখ্যা cellWidth, COLORREF color,
	UINT fuOptions, ধ্রুবক RECT *lprc, ধ্রুবক INT *lpDx);

    শূন্য FillRect(ধ্রুবক RECT *rc, COLORREF color);

    শূন্য Draw_সারি(পূর্ণসংখ্যা x1, পূর্ণসংখ্যা y1, পূর্ণসংখ্যা x2, পূর্ণসংখ্যা y2, COLORREF color);

    শূন্য SetPixel(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, COLORREF color);

    শূন্য Scroll(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, ধ্রুবক RECT *rc);

    শূন্য Flush();

    শূন্য SetRenderingParams(
	    ধ্রুবক DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    শূন্য SetFontFeatures(ধ্রুবক DWriteFontFeature *features, পূর্ণসংখ্যা গুনতি);
};

শ্রেণী AdjustedGlyphRun : সর্বজনীন DWRITE_GLYPH_RUN
{
ব্যক্তিগত:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

সর্বজনীন:
    AdjustedGlyphRun(
	    ধ্রুবক DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ধ্রুবক INT *lpDx,
	    পূর্ণসংখ্যা *glyph_সূচক,
	    পূর্ণসংখ্যা cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(নতুন FLOAT[glyphRun->glyph_গুনতি])
    {
	প্রতিজ্ঞা(cellWidth != 0.0f);
	জন্য (UINT32 i = 0; i < glyphRun->glyph_গুনতি; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    যদি (*glyph_সূচক < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyph_সূচক]);
		(*glyph_সূচক)++;
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
	পূর্ণসংখ্যা cell_গুনতি = পূর্ণসংখ্যা(floor(মান / cellWidth + 0.5f));
	যদি (cell_গুনতি < 1)
	    cell_গুনতি = 1;
	ফেরত cell_গুনতি * cellWidth;
    }
};

কাঠামো TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ধ্রুবক INT *lpDx;
    পূর্ণসংখ্যা cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    পূর্ণসংখ্যা glyph_সূচক;
};

শ্রেণী TextRenderer FINAL : সর্বজনীন IDWriteTextRenderer
{
সর্বজনীন:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_গুনতি(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    আভাসী ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ফেরত S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	ফেরত S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	দশমিক dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	ফেরত S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ধ্রুবক* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ফেরত E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull শূন্য* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ধ্রুবক* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ফেরত E_NOTIMPL;
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
	ফেরত E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull শূন্য* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ধ্রুবক* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ধ্রুবক* glyphRunDescription UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	TextRendererContext *context =
	    reinterpret_cast<TextRendererContext*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	AdjustedGlyphRun adjustedGlyphRun(glyphRun, context->cellWidth,
		context->offsetX, context->lpDx, &context->glyph_সূচক,
		context->cchText);

#ifdef FEAT_DIRECTX_COLOR_EMOJI
	যদি (pDWC_->mDWriteFactory2 != ফাঁকা)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = ফাঁকা;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		ফাঁকা,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		ফাঁকা,
		0,
		&enumerator);
	    যদি (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->Move_পরবর্তী(&hasRun);
		যখন (hasRun)
		{
		    ধ্রুবক DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
		    enumerator->GetCurrentRun(&colorGlyphRun);

		    pDWC_->mBrush->SetColor(colorGlyphRun->runColor);
		    pDWC_->mRT->DrawGlyphRun(
			    D2D1::Point2F(
				colorGlyphRun->baselineOriginX,
				fixedBaselineY),
			    &colorGlyphRun->glyphRun,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_MODE_NATURAL);
		    enumerator->Move_পরবর্তী(&hasRun);
		}
		SafeRelease(&enumerator);
		ফেরত S_OK;
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
	ফেরত S_OK;
    }

সর্বজনীন:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	ফেরত InterlockedIncrement(&c_Ref_গুনতি);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_গুনতি_2 = InterlockedDecrement(&c_Ref_গুনতি);

	যদি (new_গুনতি_2 == 0)
	{
	    delete এই;
	    ফেরত 0;
	}
	ফেরত new_গুনতি_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ধ্রুবক& riid,
	শূন্য** ppvObject)
    {
	যদি (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = এই;
	}
	অন্যথা যদি (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = এই;
	}
	অন্যথা যদি (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = এই;
	}
	অন্যথা
	{
	    *ppvObject = ফাঁকা;
	    ফেরত E_FAIL;
	}

	ফেরত S_OK;
    }

ব্যক্তিগত:
    long c_Ref_গুনতি;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ফাঁকা),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ফাঁকা),
    mDrawing(মিথ্যা),
    mFallbackDC(মিথ্যা),
    mD2D1Factory(ফাঁকা),
    mRT(ফাঁকা),
    mGDIRT(ফাঁকা),
    mBrush(ফাঁকা),
    mBitmap(ফাঁকা),
    mDWriteFactory(ফাঁকা),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ফাঁকা),
#endif
    mGdiInterop(ফাঁকা),
    mRenderingParams(ফাঁকা),
    mFontCache(8),
    mTextFormat(ফাঁকা),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    m_Font_আকার(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_গুনতি(0)
{
    ZeroMemory(mFontFeatures, আকার(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ফাঁকা,
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

    যদি (mRT != ফাঁকা)
	ফেরত S_OK;

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

    ফেরত hr;
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
DWriteContext::CreateTextFormatFromLOGFONT(ধ্রুবক LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ফাঁকা;

    IDWriteFont *font = ফাঁকা;
    IDWriteFontFamily *fontFamily = ফাঁকা;
    IDWriteLocalizedStrings *localizedFamilyNames = ফাঁকা;
    দশমিক font_আকার = 0;
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
	font_আকার = দশমিক(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	যদি (font_আকার < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_আকার = -font_আকার;
	}
	অন্যথা যদি (font_আকার > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_আকার = font_আকার * দশমিক(fontMetrics.designUnitsPerEm)
		/ দশমিক(fontMetrics.ascent + fontMetrics.descent + fontMetrics.সারি_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_দৈর্ঘ্য];
    যদি (SUCCEEDED(hr))
    {
	যদি (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_দৈর্ঘ্য) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    যদি (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		ফাঁকা, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		font_আকার,
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
	যদি (pFontAscent != ফাঁকা && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT যোগ_Height = দশমিক(fontMetrics.ascent + fontMetrics.descent);
	    যদি (যোগ_Height != 0.0f)
		*pFontAscent = দশমিক(fontMetrics.ascent) / যোগ_Height;
	    অন্যথা
		*pFontAscent = 0.83f; // fallback
	}
    }
    অন্যথা
	SafeRelease(&pTextFormat);

    ফেরত hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ধ্রুবক LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ফাঁকা;
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

    ফেরত hr;
}

    শূন্য
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    যদি (mFontCache.get(hFont, item))
    {
	যদি (item.pTextFormat != ফাঁকা)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = মিথ্যা;
	}
	অন্যথা
	    mFallbackDC = সত্য;
	ফেরত;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    যদি (GetObjectW(hFont, আকার(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    যদি (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = মিথ্যা;
    }
    অন্যথা
	mFallbackDC = সত্য;
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
	    D2D1::আকার_U(mBindRect.ডান - mBindRect.বাম,
		mBindRect.bottom - mBindRect.শীর্ষ_2),
	    props, &mBitmap);
}

    শূন্য
DWriteContext::BindDC(HDC hdc, ধ্রুবক RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    যদি (mRT == ফাঁকা)
	CreateDeviceResources();
    অন্যথা
    {
	Flush();
	Rebind();
    }
}

বাহ্যিক "C" শূন্য redraw_later_clear(শূন্য);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    নির্বাচন (mode)
    {
	পূর্বনির্ধারিত:
	ক্ষেত্র DM_GDI:
	    যদি (mInteropHDC != ফাঁকা)
	    {
		mGDIRT->ReleaseDC(ফাঁকা);
		mInteropHDC = ফাঁকা;
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
		mDrawing = মিথ্যা;
	    }
	    বিরতি;

	ক্ষেত্র DM_DIRECTX:
	    যদি (mInteropHDC != ফাঁকা)
	    {
		mGDIRT->ReleaseDC(ফাঁকা);
		mInteropHDC = ফাঁকা;
	    }
	    অন্যথা যদি (mDrawing == মিথ্যা)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = সত্য;
	    }
	    বিরতি;

	ক্ষেত্র DM_INTEROP:
	    যদি (mDrawing == মিথ্যা)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = সত্য;
	    }
	    যদি (mInteropHDC == ফাঁকা)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    বিরতি;
    }
    mDMode = mode;
    ফেরত hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ফেরত mBrush;
}

    শূন্য
DWriteContext::DrawText(ধ্রুবক WCHAR *text, পূর্ণসংখ্যা len,
	পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h, পূর্ণসংখ্যা cellWidth, COLORREF color,
	UINT fuOptions, ধ্রুবক RECT *lprc, ধ্রুবক INT *lpDx)
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
	ফেরত;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = ফাঁকা;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    যদি ((fuOptions & ETO_CLIPPED) && lprc != ফাঁকা)
    {
	mRT->ঠেলো_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->বাম), FLOAT(lprc->শীর্ষ_2),
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
	    স্থির ধ্রুবক DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    স্থির ধ্রুবক পূর্ণসংখ্যা অঙ্ক_Defaults = আকার(defaultFeatures)
		/ আকার(defaultFeatures[0]);

	    IDWriteTypography *typography = ফাঁকা;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    যদি (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		জন্য (পূর্ণসংখ্যা d = 0; d < অঙ্ক_Defaults; ++d)
		{
		    পূর্ণসংখ্যা overridden = 0;
		    জন্য (পূর্ণসংখ্যা u = 0; u < m_Font_Feature_গুনতি; ++u)
		    {
			যদি ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    বিরতি;
			}
		    }
		    যদি (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		জন্য (পূর্ণসংখ্যা i = 0; i < m_Font_Feature_গুনতি; ++i)
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

	TextRenderer renderer(এই);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    যদি ((fuOptions & ETO_CLIPPED) && lprc != ফাঁকা)
	mRT->বের_করো_Axis_Aligned_Clip();
}

    শূন্য
DWriteContext::FillRect(ধ্রুবক RECT *rc, COLORREF color)
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
		D2D1::RectF(FLOAT(rc->বাম), FLOAT(rc->শীর্ষ_2),
		    FLOAT(rc->ডান), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::Draw_সারি(পূর্ণসংখ্যা x1, পূর্ণসংখ্যা y1, পূর্ণসংখ্যা x2, পূর্ণসংখ্যা y2, COLORREF color)
{
    যদি (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, ফাঁকা);
	::সারি_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    অন্যথা
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_সারি(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::SetPixel(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, COLORREF color)
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
	mRT->Draw_সারি(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    শূন্য
DWriteContext::Scroll(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, ধ্রুবক RECT *rc)
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
	srcRect.শীর্ষ_2 = rc->শীর্ষ_2;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->শীর্ষ_2 + y;
    }
    অন্যথা
    {
	srcRect.শীর্ষ_2 = rc->শীর্ষ_2 - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->শীর্ষ_2;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.ডান - srcRect.বাম),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.শীর্ষ_2)
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
	ধ্রুবক DWriteRenderingParams *params)
{
    যদি (mDWriteFactory == ফাঁকা)
	ফেরত;

    IDWriteRenderingParams *renderingParams = ফাঁকা;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    যদি (params != ফাঁকা)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    অন্যথা
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    যদি (SUCCEEDED(hr) && renderingParams != ফাঁকা)
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
    যদি (params != ফাঁকা && mRenderingParams != ফাঁকা)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    ফেরত params;
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
    যদি (hD2D1DLL == ফাঁকা || hDWriteDLL == ফাঁকা)
    {
	DWrite_Final();
	ফেরত;
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
    pGetUserDefaultLocaleName = ফাঁকা;
    pD2D1CreateFactory = ফাঁকা;
    pDWriteCreateFactory = ফাঁকা;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(শূন্য)
{
#ifdef DYNAMIC_DIRECTX
    যদি (pGetUserDefaultLocaleName == ফাঁকা || pD2D1CreateFactory == ফাঁকা
	    || pDWriteCreateFactory == ফাঁকা)
	ফেরত ফাঁকা;
#endif
    ফেরত নতুন DWriteContext();
}

    শূন্য
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ধ্রুবক RECT *rect)
{
    যদি (ctx != ফাঁকা)
	ctx->BindDC(hdc, rect);
}

    শূন্য
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    যদি (ctx != ফাঁকা)
	ctx->SetFont(hFont);
}

    শূন্য
DWriteContext_DrawText(
	DWriteContext *ctx,
	ধ্রুবক WCHAR *text,
	পূর্ণসংখ্যা len,
	পূর্ণসংখ্যা x,
	পূর্ণসংখ্যা y,
	পূর্ণসংখ্যা w,
	পূর্ণসংখ্যা h,
	পূর্ণসংখ্যা cellWidth,
	COLORREF color,
	UINT fuOptions,
	ধ্রুবক RECT *lprc,
	ধ্রুবক INT *lpDx)
{
    যদি (ctx != ফাঁকা)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    শূন্য
DWriteContext_FillRect(DWriteContext *ctx, ধ্রুবক RECT *rc, COLORREF color)
{
    যদি (ctx != ফাঁকা)
	ctx->FillRect(rc, color);
}

    শূন্য
DWrite_Context_Draw_সারি(DWriteContext *ctx, পূর্ণসংখ্যা x1, পূর্ণসংখ্যা y1, পূর্ণসংখ্যা x2, পূর্ণসংখ্যা y2,
	COLORREF color)
{
    যদি (ctx != ফাঁকা)
	ctx->Draw_সারি(x1, y1, x2, y2, color);
}

    শূন্য
DWriteContext_SetPixel(DWriteContext *ctx, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, COLORREF color)
{
    যদি (ctx != ফাঁকা)
	ctx->SetPixel(x, y, color);
}

    শূন্য
DWriteContext_Scroll(DWriteContext *ctx, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, ধ্রুবক RECT *rc)
{
    যদি (ctx != ফাঁকা)
	ctx->Scroll(x, y, rc);
}

    শূন্য
DWriteContext_Flush(DWriteContext *ctx)
{
    যদি (ctx != ফাঁকা)
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
	ধ্রুবক DWriteRenderingParams *params)
{
    যদি (ctx != ফাঁকা)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    যদি (ctx != ফাঁকা)
	ফেরত ctx->GetRenderingParams(params);
    অন্যথা
	ফেরত ফাঁকা;
}

    শূন্য
DWriteContext::SetFontFeatures(
	ধ্রুবক DWriteFontFeature *features, পূর্ণসংখ্যা গুনতি)
{
    যদি (গুনতি > DWRITE_MAX_FONT_FEATURES)
	গুনতি = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_গুনতি = গুনতি;
    যদি (গুনতি > 0 && features != ফাঁকা)
	memcpy(mFontFeatures, features, আকার(DWriteFontFeature) * গুনতি);
}

    শূন্য
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ধ্রুবক DWriteFontFeature *features,
	পূর্ণসংখ্যা গুনতি)
{
    যদি (ctx != ফাঁকা)
	ctx->SetFontFeatures(features, গুনতি);
}
