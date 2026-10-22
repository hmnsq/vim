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
# གལ་སྲིད WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#include <windows.h>
#include <crtdbg.h>
#include <assert.h>
#include <math.h>
#include <d2d1.h>
#include <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#གལ་སྲིད defined(_MSC_VER) && (_MSC_VER < 1700)
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
#དེ་མིན
# include <dwrite.h>
#endif

#include "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#དེ་མིན
# define UNUSED
#endif

#གལ་སྲིད (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#དེ་མིན
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
extern "C" HINSTANCE vimLoadLib(const var_na *name);

typedef ཧྲིལ (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, ཧྲིལ);
typedef HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, const D2D1_FACTORY_OPTIONS *, void **);
typedef HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

static HINSTANCE hD2D1DLL = NULL;
static HINSTANCE hDWriteDLL = NULL;

static PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = NULL;
static PD2D1CREATEFACTORY pD2D1CreateFactory = NULL;
static PDWRITECREATEFACTORY pDWriteCreateFactory = NULL;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    static void
unload(HINSTANCE &hinst)
{
    གལ་སྲིད (hinst != NULL)
    {
	FreeLibrary(hinst);
	hinst = NULL;
    }
}
#endif // DYNAMIC_DIRECTX

template <class T> anta_hstha void SafeRelease(T **ppT)
{
    གལ་སྲིད (*ppT)
    {
	(*ppT)->Release();
	*ppT = NULL;
    }
}

    static DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(ཧྲིལ value)
{
    switch (value)
    {
	default:
	pak_sa 0:
	    ལོག DWRITE_PIXEL_GEOMETRY_FLAT;
	pak_sa 1:
	    ལོག DWRITE_PIXEL_GEOMETRY_RGB;
	pak_sa 2:
	    ལོག DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    static ཧྲིལ
ToInt(DWRITE_PIXEL_GEOMETRY value)
{
    switch (value)
    {
	pak_sa DWRITE_PIXEL_GEOMETRY_FLAT:
	    ལོག 0;
	pak_sa DWRITE_PIXEL_GEOMETRY_RGB:
	    ལོག 1;
	pak_sa DWRITE_PIXEL_GEOMETRY_BGR:
	    ལོག 2;
	default:
	    ལོག -1;
    }
}

    static DWRITE_RENDERING_MODE
ToRenderingMode(ཧྲིལ value)
{
    switch (value)
    {
	default:
	pak_sa 0:
	    ལོག DWRITE_RENDERING_MODE_DEFAULT;
	pak_sa 1:
	    ལོག DWRITE_RENDERING_MODE_ALIASED;
	pak_sa 2:
	    ལོག DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	pak_sa 3:
	    ལོག DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	pak_sa 4:
	    ལོག DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	pak_sa 5:
	    ལོག DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	pak_sa 6:
	    ལོག DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    static D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(ཧྲིལ value)
{
    switch (value)
    {
	default:
	pak_sa 0:
	    ལོག D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	pak_sa 1:
	    ལོག D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	pak_sa 2:
	    ལོག D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	pak_sa 3:
	    ལོག D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    static ཧྲིལ
ToInt(DWRITE_RENDERING_MODE value)
{
    switch (value)
    {
	pak_sa DWRITE_RENDERING_MODE_DEFAULT:
	    ལོག 0;
	pak_sa DWRITE_RENDERING_MODE_ALIASED:
	    ལོག 1;
	pak_sa DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    ལོག 2;
	pak_sa DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    ལོག 3;
	pak_sa DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    ལོག 4;
	pak_sa DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    ལོག 5;
	pak_sa DWRITE_RENDERING_MODE_OUTLINE:
	    ལོག 6;
	default:
	    ལོག -1;
    }
}

class FontCache {
public:
    sa_nracanaa Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(NULL), pTextFormat(NULL), fontAscent(0.0f) {}
    };

private:
    ཧྲིལ mSize;
    Item *mItems;

public:
    FontCache(ཧྲིལ size = 2) :
	mSize(size),
	mItems(new Item[size])
    {
    }

    ~FontCache()
    {
	དོན་ལུ (ཧྲིལ i = 0; i < mSize; ++i)
	    SafeRelease(&mItems[i].pTextFormat);
	delete[] mItems;
    }

    bool get(HFONT hFont, Item &item)
    {
	ཧྲིལ n = find(hFont);
	གལ་སྲིད (n < 0)
	    ལོག false;
	item = mItems[n];
	slide(n);
	ལོག true;
    }

    void put(const Item& item)
    {
	ཧྲིལ n = find(item.hFont);
	གལ་སྲིད (n < 0)
	    n = mSize - 1;
	གལ་སྲིད (mItems[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&mItems[n].pTextFormat);
	    གལ་སྲིད (item.pTextFormat != NULL)
		item.pTextFormat->AddRef();
	}
	mItems[n] = item;
	slide(n);
    }

private:
    ཧྲིལ find(HFONT hFont)
    {
	དོན་ལུ (ཧྲིལ i = 0; i < mSize; ++i)
	{
	    གལ་སྲིད (mItems[i].hFont == hFont)
		ལོག i;
	}
	ལོག -1;
    }

    void slide(ཧྲིལ nextTop)
    {
	གལ་སྲིད (nextTop == 0)
	    ལོག;
	Item tmp = mItems[nextTop];
	དོན་ལུ (ཧྲིལ i = nextTop - 1; i >= 0; --i)
	    mItems[i + 1] = mItems[i];
	mItems[0] = tmp;
    }
};

ga_nanaa DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

sa_nracanaa DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    bool mDrawing;
    bool mFallbackDC;

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
    ཧྲིལ mFontFeatureCount;

    // METHODS

    DWriteContext();

    virtual ~DWriteContext();

    HRESULT CreateDeviceResources();

    void DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(const LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(const LOGFONTW &logFont);

    void SetFont(HFONT hFont);

    void Rebind();

    void BindDC(HDC hdc, const RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    void DrawText(const WCHAR *text, ཧྲིལ len,
	ཧྲིལ x, ཧྲིལ y, ཧྲིལ w, ཧྲིལ h, ཧྲིལ cellWidth, COLORREF color,
	UINT fuOptions, const RECT *lprc, const INT *lpDx);

    void FillRect(const RECT *rc, COLORREF color);

    void DrawLine(ཧྲིལ x1, ཧྲིལ y1, ཧྲིལ x2, ཧྲིལ y2, COLORREF color);

    void SetPixel(ཧྲིལ x, ཧྲིལ y, COLORREF color);

    void Scroll(ཧྲིལ x, ཧྲིལ y, const RECT *rc);

    void Flush();

    void SetRenderingParams(
	    const DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    void SetFontFeatures(const DWriteFontFeature *features, ཧྲིལ count);
};

class AdjustedGlyphRun : public DWRITE_GLYPH_RUN
{
private:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

public:
    AdjustedGlyphRun(
	    const DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    const INT *lpDx,
	    ཧྲིལ *glyphIndex,
	    ཧྲིལ cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(new FLOAT[glyphRun->glyphCount])
    {
	assert(cellWidth != 0.0f);
	དོན་ལུ (UINT32 i = 0; i < glyphRun->glyphCount; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    གལ་སྲིད (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    དེ་མིན
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

    static FLOAT adjustToCell(FLOAT value, FLOAT cellWidth)
    {
	ཧྲིལ cellCount = ཧྲིལ(floor(value / cellWidth + 0.5f));
	གལ་སྲིད (cellCount < 1)
	    cellCount = 1;
	ལོག cellCount * cellWidth;
    }
};

sa_nracanaa TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    const INT *lpDx;
    ཧྲིལ cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    ཧྲིལ glyphIndex;
};

class TextRenderer FINAL : public IDWriteTextRenderer
{
public:
    TextRenderer(
	    DWriteContext* pDWC) :
	cRefCount_(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    virtual ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull void* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ལོག S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull void* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	ལོག S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull void* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	float dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	ལོག S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull void* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE const* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ལོག E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull void* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH const* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ལོག E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull void* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL isRightToLeft UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ལོག E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull void* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN const* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription UNUSED,
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
	གལ་སྲིད (pDWC_->mDWriteFactory2 != NULL)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = NULL;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		NULL,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		NULL,
		0,
		&enumerator);
	    གལ་སྲིད (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		སྐབས (hasRun)
		{
		    const DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		ལོག S_OK;
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
	ལོག S_OK;
    }

public:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	ལོག InterlockedIncrement(&cRefCount_);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long newCount = InterlockedDecrement(&cRefCount_);

	གལ་སྲིད (newCount == 0)
	{
	    delete this;
	    ལོག 0;
	}
	ལོག newCount;
    }

    IFACEMETHOD(QueryInterface)(
	IID const& riid,
	void** ppvObject)
    {
	གལ་སྲིད (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = this;
	}
	དེ་མིན གལ་སྲིད (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = this;
	}
	དེ་མིན གལ་སྲིད (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = this;
	}
	དེ་མིན
	{
	    *ppvObject = NULL;
	    ལོག E_FAIL;
	}

	ལོག S_OK;
    }

private:
    long cRefCount_;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(NULL),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(NULL),
    mDrawing(false),
    mFallbackDC(false),
    mD2D1Factory(NULL),
    mRT(NULL),
    mGDIRT(NULL),
    mBrush(NULL),
    mBitmap(NULL),
    mDWriteFactory(NULL),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(NULL),
#endif
    mGdiInterop(NULL),
    mRenderingParams(NULL),
    mFontCache(8),
    mTextFormat(NULL),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    mFontFeatureCount(0)
{
    ZeroMemory(mFontFeatures, parimaa_na(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), NULL,
	    reinterpret_cast<void**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    གལ་སྲིད (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    གལ་སྲིད (SUCCEEDED(hr))
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

    གལ་སྲིད (mRT != NULL)
	ལོག S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    གལ་སྲིད (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<void**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    གལ་སྲིད (SUCCEEDED(hr))
	Rebind();

    ལོག hr;
}

    void
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(const LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = NULL;

    IDWriteFont *font = NULL;
    IDWriteFontFamily *fontFamily = NULL;
    IDWriteLocalizedStrings *localizedFamilyNames = NULL;
    float fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    གལ་སྲིད (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    གལ་སྲིད (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = float(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	གལ་སྲིད (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	དེ་མིན གལ་སྲིད (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * float(fontMetrics.designUnitsPerEm)
		/ float(fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    གལ་སྲིད (SUCCEEDED(hr))
    {
	གལ་སྲིད (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    གལ་སྲིད (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		NULL, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    གལ་སྲིད (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    གལ་སྲིད (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    གལ་སྲིད (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    གལ་སྲིད (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	གལ་སྲིད (pFontAscent != NULL && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = float(fontMetrics.ascent + fontMetrics.descent);
	    གལ་སྲིད (totalHeight != 0.0f)
		*pFontAscent = float(fontMetrics.ascent) / totalHeight;
	    དེ་མིན
		*pFontAscent = 0.83f; // fallback
	}
    }
    དེ་མིན
	SafeRelease(&pTextFormat);

    ལོག hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(const LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = NULL;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    གལ་སྲིད (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    ལོག hr;
}

    void
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    གལ་སྲིད (mFontCache.get(hFont, item))
    {
	གལ་སྲིད (item.pTextFormat != NULL)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = false;
	}
	དེ་མིན
	    mFallbackDC = true;
	ལོག;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    གལ་སྲིད (GetObjectW(hFont, parimaa_na(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    གལ་སྲིད (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = false;
    }
    དེ་མིན
	mFallbackDC = true;
    mFontCache.put(item);
}

    void
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
	    D2D1::SizeU(mBindRect.right - mBindRect.left,
		mBindRect.bottom - mBindRect.top),
	    props, &mBitmap);
}

    void
DWriteContext::BindDC(HDC hdc, const RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    གལ་སྲིད (mRT == NULL)
	CreateDeviceResources();
    དེ་མིན
    {
	Flush();
	Rebind();
    }
}

extern "C" void redraw_later_clear(void);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    switch (mode)
    {
	default:
	pak_sa DM_GDI:
	    གལ་སྲིད (mInteropHDC != NULL)
	    {
		mGDIRT->ReleaseDC(NULL);
		mInteropHDC = NULL;
	    }
	    གལ་སྲིད (mDrawing)
	    {
		hr = mRT->EndDraw();
		གལ་སྲིད (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = false;
	    }
	    bha_nga;

	pak_sa DM_DIRECTX:
	    གལ་སྲིད (mInteropHDC != NULL)
	    {
		mGDIRT->ReleaseDC(NULL);
		mInteropHDC = NULL;
	    }
	    དེ་མིན གལ་སྲིད (mDrawing == false)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = true;
	    }
	    bha_nga;

	pak_sa DM_INTEROP:
	    གལ་སྲིད (mDrawing == false)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = true;
	    }
	    གལ་སྲིད (mInteropHDC == NULL)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    bha_nga;
    }
    mDMode = mode;
    ལོག hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ལོག mBrush;
}

    void
DWriteContext::DrawText(const WCHAR *text, ཧྲིལ len,
	ཧྲིལ x, ཧྲིལ y, ཧྲིལ w, ཧྲིལ h, ཧྲིལ cellWidth, COLORREF color,
	UINT fuOptions, const RECT *lprc, const INT *lpDx)
{
    གལ་སྲིད (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	གལ་སྲིད (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ལོག;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = NULL;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    གལ་སྲིད ((fuOptions & ETO_CLIPPED) && lprc != NULL)
    {
	mRT->PushAxisAlignedClip(
	    D2D1::RectF(FLOAT(lprc->left), FLOAT(lprc->top),
			FLOAT(lprc->right), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    གལ་སྲིད (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	གལ་སྲིད (mFontFeatureCount > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    static const DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    static const ཧྲིལ numDefaults = parimaa_na(defaultFeatures)
		/ parimaa_na(defaultFeatures[0]);

	    IDWriteTypography *typography = NULL;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    གལ་སྲིད (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		དོན་ལུ (ཧྲིལ d = 0; d < numDefaults; ++d)
		{
		    ཧྲིལ overridden = 0;
		    དོན་ལུ (ཧྲིལ u = 0; u < mFontFeatureCount; ++u)
		    {
			གལ་སྲིད ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    bha_nga;
			}
		    }
		    གལ་སྲིད (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		དོན་ལུ (ཧྲིལ i = 0; i < mFontFeatureCount; ++i)
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

	TextRenderer renderer(this);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    གལ་སྲིད ((fuOptions & ETO_CLIPPED) && lprc != NULL)
	mRT->PopAxisAlignedClip();
}

    void
DWriteContext::FillRect(const RECT *rc, COLORREF color)
{
    གལ་སྲིད (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    དེ་མིན
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->left), FLOAT(rc->top),
		    FLOAT(rc->right), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    void
DWriteContext::DrawLine(ཧྲིལ x1, ཧྲིལ y1, ཧྲིལ x2, ཧྲིལ y2, COLORREF color)
{
    གལ་སྲིད (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, NULL);
	::LineTo(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    དེ་མིན
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->DrawLine(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    void
DWriteContext::SetPixel(ཧྲིལ x, ཧྲིལ y, COLORREF color)
{
    གལ་སྲིད (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    དེ་མིན
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->DrawLine(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    void
DWriteContext::Scroll(ཧྲིལ x, ཧྲིལ y, const RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    གལ་སྲིད (x >= 0)
    {
	srcRect.left = rc->left;
	srcRect.right = rc->right - x;
	destPoint.x = rc->left + x;
    }
    དེ་མིན
    {
	srcRect.left = rc->left - x;
	srcRect.right = rc->right;
	destPoint.x = rc->left;
    }
    གལ་སྲིད (y >= 0)
    {
	srcRect.top = rc->top;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->top + y;
    }
    དེ་མིན
    {
	srcRect.top = rc->top - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->top;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.right - srcRect.left),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.top)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    void
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    void
DWriteContext::SetRenderingParams(
	const DWriteRenderingParams *params)
{
    གལ་སྲིད (mDWriteFactory == NULL)
	ལོག;

    IDWriteRenderingParams *renderingParams = NULL;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    གལ་སྲིད (params != NULL)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    དེ་མིན
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    གལ་སྲིད (SUCCEEDED(hr) && renderingParams != NULL)
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
    གལ་སྲིད (params != NULL && mRenderingParams != NULL)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    ལོག params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    void
DWrite_Init(void)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    གལ་སྲིད (hD2D1DLL == NULL || hDWriteDLL == NULL)
    {
	DWrite_Final();
	ལོག;
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

    void
DWrite_Final(void)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = NULL;
    pD2D1CreateFactory = NULL;
    pDWriteCreateFactory = NULL;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(void)
{
#ifdef DYNAMIC_DIRECTX
    གལ་སྲིད (pGetUserDefaultLocaleName == NULL || pD2D1CreateFactory == NULL
	    || pDWriteCreateFactory == NULL)
	ལོག NULL;
#endif
    ལོག new DWriteContext();
}

    void
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, const RECT *rect)
{
    གལ་སྲིད (ctx != NULL)
	ctx->BindDC(hdc, rect);
}

    void
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    གལ་སྲིད (ctx != NULL)
	ctx->SetFont(hFont);
}

    void
DWriteContext_DrawText(
	DWriteContext *ctx,
	const WCHAR *text,
	ཧྲིལ len,
	ཧྲིལ x,
	ཧྲིལ y,
	ཧྲིལ w,
	ཧྲིལ h,
	ཧྲིལ cellWidth,
	COLORREF color,
	UINT fuOptions,
	const RECT *lprc,
	const INT *lpDx)
{
    གལ་སྲིད (ctx != NULL)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    void
DWriteContext_FillRect(DWriteContext *ctx, const RECT *rc, COLORREF color)
{
    གལ་སྲིད (ctx != NULL)
	ctx->FillRect(rc, color);
}

    void
DWriteContext_DrawLine(DWriteContext *ctx, ཧྲིལ x1, ཧྲིལ y1, ཧྲིལ x2, ཧྲིལ y2,
	COLORREF color)
{
    གལ་སྲིད (ctx != NULL)
	ctx->DrawLine(x1, y1, x2, y2, color);
}

    void
DWriteContext_SetPixel(DWriteContext *ctx, ཧྲིལ x, ཧྲིལ y, COLORREF color)
{
    གལ་སྲིད (ctx != NULL)
	ctx->SetPixel(x, y, color);
}

    void
DWriteContext_Scroll(DWriteContext *ctx, ཧྲིལ x, ཧྲིལ y, const RECT *rc)
{
    གལ་སྲིད (ctx != NULL)
	ctx->Scroll(x, y, rc);
}

    void
DWriteContext_Flush(DWriteContext *ctx)
{
    གལ་སྲིད (ctx != NULL)
	ctx->Flush();
}

    void
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    void
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	const DWriteRenderingParams *params)
{
    གལ་སྲིད (ctx != NULL)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    གལ་སྲིད (ctx != NULL)
	ལོག ctx->GetRenderingParams(params);
    དེ་མིན
	ལོག NULL;
}

    void
DWriteContext::SetFontFeatures(
	const DWriteFontFeature *features, ཧྲིལ count)
{
    གལ་སྲིད (count > DWRITE_MAX_FONT_FEATURES)
	count = DWRITE_MAX_FONT_FEATURES;
    mFontFeatureCount = count;
    གལ་སྲིད (count > 0 && features != NULL)
	memcpy(mFontFeatures, features, parimaa_na(DWriteFontFeature) * count);
}

    void
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	const DWriteFontFeature *features,
	ཧྲིལ count)
{
    གལ་སྲིད (ctx != NULL)
	ctx->SetFontFeatures(features, count);
}
