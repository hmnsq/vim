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
# אם WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#ضمن <windows.h>
#ضمن <crtdbg.h>
#ضمن <assert.h>
#ضمن <math.h>
#ضمن <d2d1.h>
#ضمن <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#אם defined(_MSC_VER) && (_MSC_VER < 1700)
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
#אחרת
# include <dwrite.h>
#endif

#ضمن "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#אחרת
# define UNUSED
#endif

#אם (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#אחרת
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
خارجي "C" HINSTANCE vimLoadLib(ثابت حرف *name);

تعريفنوع صحيح (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, صحيح);
تعريفنوع HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ثابت D2D1_FACTORY_OPTIONS *, فراغ **);
تعريفنوع HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ساكن HINSTANCE hD2D1DLL = عدم;
ساكن HINSTANCE hDWriteDLL = عدم;

ساكن PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = عدم;
ساكن PD2D1CREATEFACTORY pD2D1CreateFactory = عدم;
ساكن PDWRITECREATEFACTORY pDWriteCreateFactory = عدم;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    ساكن فراغ
unload(HINSTANCE &hinst)
{
    אם (hinst != عدم)
    {
	FreeLibrary(hinst);
	hinst = عدم;
    }
}
#endif // DYNAMIC_DIRECTX

template <מחלקה T> مضمن فراغ SafeRelease(T **ppT)
{
    אם (*ppT)
    {
	(*ppT)->Release();
	*ppT = عدم;
    }
}

    ساكن DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(صحيح value)
{
    بدل (value)
    {
	افتراضي:
	حالة 0:
	    החזר DWRITE_PIXEL_GEOMETRY_FLAT;
	حالة 1:
	    החזר DWRITE_PIXEL_GEOMETRY_RGB;
	حالة 2:
	    החזר DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ساكن صحيح
ToInt(DWRITE_PIXEL_GEOMETRY value)
{
    بدل (value)
    {
	حالة DWRITE_PIXEL_GEOMETRY_FLAT:
	    החזר 0;
	حالة DWRITE_PIXEL_GEOMETRY_RGB:
	    החזר 1;
	حالة DWRITE_PIXEL_GEOMETRY_BGR:
	    החזר 2;
	افتراضي:
	    החזר -1;
    }
}

    ساكن DWRITE_RENDERING_MODE
ToRenderingMode(صحيح value)
{
    بدل (value)
    {
	افتراضي:
	حالة 0:
	    החזר DWRITE_RENDERING_MODE_DEFAULT;
	حالة 1:
	    החזר DWRITE_RENDERING_MODE_ALIASED;
	حالة 2:
	    החזר DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	حالة 3:
	    החזר DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	حالة 4:
	    החזר DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	حالة 5:
	    החזר DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	حالة 6:
	    החזר DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    ساكن D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(صحيح value)
{
    بدل (value)
    {
	افتراضي:
	حالة 0:
	    החזר D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	حالة 1:
	    החזר D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	حالة 2:
	    החזר D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	حالة 3:
	    החזר D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    ساكن صحيح
ToInt(DWRITE_RENDERING_MODE value)
{
    بدل (value)
    {
	حالة DWRITE_RENDERING_MODE_DEFAULT:
	    החזר 0;
	حالة DWRITE_RENDERING_MODE_ALIASED:
	    החזר 1;
	حالة DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    החזר 2;
	حالة DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    החזר 3;
	حالة DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    החזר 4;
	حالة DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    החזר 5;
	حالة DWRITE_RENDERING_MODE_OUTLINE:
	    החזר 6;
	افتراضي:
	    החזר -1;
    }
}

מחלקה FontCache {
عام:
    بنية Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(عدم), pTextFormat(عدم), fontAscent(0.0f) {}
    };

خاص:
    صحيح mSize;
    Item *mItems;

عام:
    FontCache(صحيح size = 2) :
	mSize(size),
	mItems(جديد Item[size])
    {
    }

    ~FontCache()
    {
	עבור (صحيح i = 0; i < mSize; ++i)
	    SafeRelease(&mItems[i].pTextFormat);
	delete[] mItems;
    }

    منطقي get(HFONT hFont, Item &item)
    {
	صحيح n = find(hFont);
	אם (n < 0)
	    החזר שקר;
	item = mItems[n];
	slide(n);
	החזר אמת;
    }

    فراغ put(ثابت Item& item)
    {
	صحيح n = find(item.hFont);
	אם (n < 0)
	    n = mSize - 1;
	אם (mItems[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&mItems[n].pTextFormat);
	    אם (item.pTextFormat != عدم)
		item.pTextFormat->AddRef();
	}
	mItems[n] = item;
	slide(n);
    }

خاص:
    صحيح find(HFONT hFont)
    {
	עבור (صحيح i = 0; i < mSize; ++i)
	{
	    אם (mItems[i].hFont == hFont)
		החזר i;
	}
	החזר -1;
    }

    فراغ slide(صحيح nextTop)
    {
	אם (nextTop == 0)
	    החזר;
	Item tmp = mItems[nextTop];
	עבור (صحيح i = nextTop - 1; i >= 0; --i)
	    mItems[i + 1] = mItems[i];
	mItems[0] = tmp;
    }
};

تعداد DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

بنية DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    منطقي mDrawing;
    منطقي mFallbackDC;

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
    صحيح mFontFeatureCount;

    // METHODS

    DWriteContext();

    virtual ~DWriteContext();

    HRESULT CreateDeviceResources();

    فراغ DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ثابت LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ثابت LOGFONTW &logFont);

    فراغ SetFont(HFONT hFont);

    فراغ Rebind();

    فراغ BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    فراغ DrawText(ثابت WCHAR *text, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cellWidth, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    فراغ FillRect(ثابت RECT *rc, COLORREF color);

    فراغ DrawLine(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color);

    فراغ SetPixel(صحيح x, صحيح y, COLORREF color);

    فراغ Scroll(صحيح x, صحيح y, ثابت RECT *rc);

    فراغ Flush();

    فراغ SetRenderingParams(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    فراغ SetFontFeatures(ثابت DWriteFontFeature *features, صحيح count);
};

מחלקה AdjustedGlyphRun : عام DWRITE_GLYPH_RUN
{
خاص:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عام:
    AdjustedGlyphRun(
	    ثابت DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحيح *glyphIndex,
	    صحيح cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جديد FLOAT[glyphRun->glyphCount])
    {
	أكد(cellWidth != 0.0f);
	עבור (UINT32 i = 0; i < glyphRun->glyphCount; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    אם (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    אחרת
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

    ساكن FLOAT adjustToCell(FLOAT value, FLOAT cellWidth)
    {
	صحيح cellCount = صحيح(floor(value / cellWidth + 0.5f));
	אם (cellCount < 1)
	    cellCount = 1;
	החזר cellCount * cellWidth;
    }
};

بنية TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ثابت INT *lpDx;
    صحيح cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    صحيح glyphIndex;
};

מחלקה TextRenderer FINAL : عام IDWriteTextRenderer
{
عام:
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
	__maybenull فراغ* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	החזר S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull فراغ* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	החזר S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull فراغ* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	عائم dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	החזר S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull فراغ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ثابت* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	החזר E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull فراغ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	החזר E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull فراغ* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL isRightToLeft UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	החזר E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull فراغ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ثابت* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ثابت* glyphRunDescription UNUSED,
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
	אם (pDWC_->mDWriteFactory2 != عدم)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = عدم;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		عدم,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		عدم,
		0,
		&enumerator);
	    אם (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		כלעוד (hasRun)
		{
		    ثابت DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		החזר S_OK;
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
	החזר S_OK;
    }

عام:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	החזר InterlockedIncrement(&cRefCount_);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long newCount = InterlockedDecrement(&cRefCount_);

	אם (newCount == 0)
	{
	    delete هذا;
	    החזר 0;
	}
	החזר newCount;
    }

    IFACEMETHOD(QueryInterface)(
	IID ثابت& riid,
	فراغ** ppvObject)
    {
	אם (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = هذا;
	}
	אחרת אם (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = هذا;
	}
	אחרת אם (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = هذا;
	}
	אחרת
	{
	    *ppvObject = عدم;
	    החזר E_FAIL;
	}

	החזר S_OK;
    }

خاص:
    long cRefCount_;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(عدم),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(عدم),
    mDrawing(שקר),
    mFallbackDC(שקר),
    mD2D1Factory(عدم),
    mRT(عدم),
    mGDIRT(عدم),
    mBrush(عدم),
    mBitmap(عدم),
    mDWriteFactory(عدم),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(عدم),
#endif
    mGdiInterop(عدم),
    mRenderingParams(عدم),
    mFontCache(8),
    mTextFormat(عدم),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    mFontFeatureCount(0)
{
    ZeroMemory(mFontFeatures, حجم(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), عدم,
	    reinterpret_cast<فراغ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    אם (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    אם (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    אם (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    אם (SUCCEEDED(hr))
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

    אם (mRT != عدم)
	החזר S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    אם (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<فراغ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    אם (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    אם (SUCCEEDED(hr))
	Rebind();

    החזר hr;
}

    فراغ
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(ثابت LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = عدم;

    IDWriteFont *font = عدم;
    IDWriteFontFamily *fontFamily = عدم;
    IDWriteLocalizedStrings *localizedFamilyNames = عدم;
    عائم fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    אם (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    אם (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    אם (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    אם (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    אם (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = عائم(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	אם (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	אחרת אם (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * عائم(fontMetrics.designUnitsPerEm)
		/ عائم(fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    אם (SUCCEEDED(hr))
    {
	אם (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    אם (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		عدم, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    אם (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    אם (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    אם (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    אם (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	אם (pFontAscent != عدم && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = عائم(fontMetrics.ascent + fontMetrics.descent);
	    אם (totalHeight != 0.0f)
		*pFontAscent = عائم(fontMetrics.ascent) / totalHeight;
	    אחרת
		*pFontAscent = 0.83f; // fallback
	}
    }
    אחרת
	SafeRelease(&pTextFormat);

    החזר hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = عدم;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    אם (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    החזר hr;
}

    فراغ
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    אם (mFontCache.get(hFont, item))
    {
	אם (item.pTextFormat != عدم)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = שקר;
	}
	אחרת
	    mFallbackDC = אמת;
	החזר;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    אם (GetObjectW(hFont, حجم(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    אם (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = שקר;
    }
    אחרת
	mFallbackDC = אמת;
    mFontCache.put(item);
}

    فراغ
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

    فراغ
DWriteContext::BindDC(HDC hdc, ثابت RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    אם (mRT == عدم)
	CreateDeviceResources();
    אחרת
    {
	Flush();
	Rebind();
    }
}

خارجي "C" فراغ redraw_later_clear(فراغ);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    بدل (mode)
    {
	افتراضي:
	حالة DM_GDI:
	    אם (mInteropHDC != عدم)
	    {
		mGDIRT->ReleaseDC(عدم);
		mInteropHDC = عدم;
	    }
	    אם (mDrawing)
	    {
		hr = mRT->EndDraw();
		אם (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = שקר;
	    }
	    اكسر;

	حالة DM_DIRECTX:
	    אם (mInteropHDC != عدم)
	    {
		mGDIRT->ReleaseDC(عدم);
		mInteropHDC = عدم;
	    }
	    אחרת אם (mDrawing == שקר)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = אמת;
	    }
	    اكسر;

	حالة DM_INTEROP:
	    אם (mDrawing == שקר)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = אמת;
	    }
	    אם (mInteropHDC == عدم)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    اكسر;
    }
    mDMode = mode;
    החזר hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    החזר mBrush;
}

    فراغ
DWriteContext::DrawText(ثابت WCHAR *text, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cellWidth, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    אם (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	אם (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	החזר;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = عدم;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    אם ((fuOptions & ETO_CLIPPED) && lprc != عدم)
    {
	mRT->PushAxisAlignedClip(
	    D2D1::RectF(FLOAT(lprc->left), FLOAT(lprc->top),
			FLOAT(lprc->right), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    אם (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	אם (mFontFeatureCount > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساكن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    ساكن ثابت صحيح numDefaults = حجم(defaultFeatures)
		/ حجم(defaultFeatures[0]);

	    IDWriteTypography *typography = عدم;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    אם (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		עבור (صحيح d = 0; d < numDefaults; ++d)
		{
		    صحيح overridden = 0;
		    עבור (صحيح u = 0; u < mFontFeatureCount; ++u)
		    {
			אם ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    اكسر;
			}
		    }
		    אם (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		עבור (صحيح i = 0; i < mFontFeatureCount; ++i)
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

	TextRenderer renderer(هذا);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    אם ((fuOptions & ETO_CLIPPED) && lprc != عدم)
	mRT->PopAxisAlignedClip();
}

    فراغ
DWriteContext::FillRect(ثابت RECT *rc, COLORREF color)
{
    אם (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    אחרת
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->left), FLOAT(rc->top),
		    FLOAT(rc->right), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::DrawLine(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color)
{
    אם (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, عدم);
	::LineTo(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    אחרת
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->DrawLine(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::SetPixel(صحيح x, صحيح y, COLORREF color)
{
    אם (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    אחרת
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->DrawLine(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Scroll(صحيح x, صحيح y, ثابت RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    אם (x >= 0)
    {
	srcRect.left = rc->left;
	srcRect.right = rc->right - x;
	destPoint.x = rc->left + x;
    }
    אחרת
    {
	srcRect.left = rc->left - x;
	srcRect.right = rc->right;
	destPoint.x = rc->left;
    }
    אם (y >= 0)
    {
	srcRect.top = rc->top;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->top + y;
    }
    אחרת
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

    فراغ
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    فراغ
DWriteContext::SetRenderingParams(
	ثابت DWriteRenderingParams *params)
{
    אם (mDWriteFactory == عدم)
	החזר;

    IDWriteRenderingParams *renderingParams = عدم;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    אם (params != عدم)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    אחרת
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    אם (SUCCEEDED(hr) && renderingParams != عدم)
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
    אם (params != عدم && mRenderingParams != عدم)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    החזר params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    فراغ
DWrite_Init(فراغ)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    אם (hD2D1DLL == عدم || hDWriteDLL == عدم)
    {
	DWrite_Final();
	החזר;
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

    فراغ
DWrite_Final(فراغ)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = عدم;
    pD2D1CreateFactory = عدم;
    pDWriteCreateFactory = عدم;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(فراغ)
{
#ifdef DYNAMIC_DIRECTX
    אם (pGetUserDefaultLocaleName == عدم || pD2D1CreateFactory == عدم
	    || pDWriteCreateFactory == عدم)
	החזר عدم;
#endif
    החזר جديد DWriteContext();
}

    فراغ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    אם (ctx != عدم)
	ctx->BindDC(hdc, rect);
}

    فراغ
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    אם (ctx != عدم)
	ctx->SetFont(hFont);
}

    فراغ
DWriteContext_DrawText(
	DWriteContext *ctx,
	ثابت WCHAR *text,
	صحيح len,
	صحيح x,
	صحيح y,
	صحيح w,
	صحيح h,
	صحيح cellWidth,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    אם (ctx != عدم)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    فراغ
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    אם (ctx != عدم)
	ctx->FillRect(rc, color);
}

    فراغ
DWriteContext_DrawLine(DWriteContext *ctx, صحيح x1, صحيح y1, صحيح x2, صحيح y2,
	COLORREF color)
{
    אם (ctx != عدم)
	ctx->DrawLine(x1, y1, x2, y2, color);
}

    فراغ
DWriteContext_SetPixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color)
{
    אם (ctx != عدم)
	ctx->SetPixel(x, y, color);
}

    فراغ
DWriteContext_Scroll(DWriteContext *ctx, صحيح x, صحيح y, ثابت RECT *rc)
{
    אם (ctx != عدم)
	ctx->Scroll(x, y, rc);
}

    فراغ
DWriteContext_Flush(DWriteContext *ctx)
{
    אם (ctx != عدم)
	ctx->Flush();
}

    فراغ
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    فراغ
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    אם (ctx != عدم)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    אם (ctx != عدم)
	החזר ctx->GetRenderingParams(params);
    אחרת
	החזר عدم;
}

    فراغ
DWriteContext::SetFontFeatures(
	ثابت DWriteFontFeature *features, صحيح count)
{
    אם (count > DWRITE_MAX_FONT_FEATURES)
	count = DWRITE_MAX_FONT_FEATURES;
    mFontFeatureCount = count;
    אם (count > 0 && features != عدم)
	memcpy(mFontFeatures, features, حجم(DWriteFontFeature) * count);
}

    فراغ
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح count)
{
    אם (ctx != عدم)
	ctx->SetFontFeatures(features, count);
}
