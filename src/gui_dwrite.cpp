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
# اگر WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#شامل <windows.h>
#شامل <crtdbg.h>
#شامل <assert.h>
#شامل <math.h>
#شامل <d2d1.h>
#شامل <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#اگر defined(_MSC_VER) && (_MSC_VER < 1700)
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
#وگرنه
# include <dwrite.h>
#endif

#شامل "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#وگرنه
# define UNUSED
#endif

#اگر (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وگرنه
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
بیرونی "C" HINSTANCE vimLoadLib(ثابت نویسه *name);

تعریفنوع صحیح (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, صحیح);
تعریفنوع HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ثابت D2D1_FACTORY_OPTIONS *, تهی **);
تعریفنوع HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ایستا HINSTANCE hD2D1DLL = پوچ;
ایستا HINSTANCE hDWriteDLL = پوچ;

ایستا PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = پوچ;
ایستا PD2D1CREATEFACTORY pD2D1CreateFactory = پوچ;
ایستا PDWRITECREATEFACTORY pDWriteCreateFactory = پوچ;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    ایستا تهی
unload(HINSTANCE &hinst)
{
    اگر (hinst != پوچ)
    {
	FreeLibrary(hinst);
	hinst = پوچ;
    }
}
#endif // DYNAMIC_DIRECTX

template <رده T> درونخطی تهی SafeRelease(T **ppT)
{
    اگر (*ppT)
    {
	(*ppT)->Release();
	*ppT = پوچ;
    }
}

    ایستا DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(صحیح مقدار)
{
    انتخاب (مقدار)
    {
	پیشفرض:
	حالت 0:
	    بازگشت DWRITE_PIXEL_GEOMETRY_FLAT;
	حالت 1:
	    بازگشت DWRITE_PIXEL_GEOMETRY_RGB;
	حالت 2:
	    بازگشت DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ایستا صحیح
ToInt(DWRITE_PIXEL_GEOMETRY مقدار)
{
    انتخاب (مقدار)
    {
	حالت DWRITE_PIXEL_GEOMETRY_FLAT:
	    بازگشت 0;
	حالت DWRITE_PIXEL_GEOMETRY_RGB:
	    بازگشت 1;
	حالت DWRITE_PIXEL_GEOMETRY_BGR:
	    بازگشت 2;
	پیشفرض:
	    بازگشت -1;
    }
}

    ایستا DWRITE_RENDERING_MODE
ToRenderingMode(صحیح مقدار)
{
    انتخاب (مقدار)
    {
	پیشفرض:
	حالت 0:
	    بازگشت DWRITE_RENDERING_MODE_DEFAULT;
	حالت 1:
	    بازگشت DWRITE_RENDERING_MODE_ALIASED;
	حالت 2:
	    بازگشت DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	حالت 3:
	    بازگشت DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	حالت 4:
	    بازگشت DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	حالت 5:
	    بازگشت DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	حالت 6:
	    بازگشت DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    ایستا D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(صحیح مقدار)
{
    انتخاب (مقدار)
    {
	پیشفرض:
	حالت 0:
	    بازگشت D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	حالت 1:
	    بازگشت D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	حالت 2:
	    بازگشت D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	حالت 3:
	    بازگشت D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    ایستا صحیح
ToInt(DWRITE_RENDERING_MODE مقدار)
{
    انتخاب (مقدار)
    {
	حالت DWRITE_RENDERING_MODE_DEFAULT:
	    بازگشت 0;
	حالت DWRITE_RENDERING_MODE_ALIASED:
	    بازگشت 1;
	حالت DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    بازگشت 2;
	حالت DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    بازگشت 3;
	حالت DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    بازگشت 4;
	حالت DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    بازگشت 5;
	حالت DWRITE_RENDERING_MODE_OUTLINE:
	    بازگشت 6;
	پیشفرض:
	    بازگشت -1;
    }
}

رده FontCache {
عمومی:
    ساختار Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(پوچ), pTextFormat(پوچ), fontAscent(0.0f) {}
    };

خصوصی:
    صحیح mSize;
    Item *m_اقلام;

عمومی:
    FontCache(صحیح size = 2) :
	mSize(size),
	m_اقلام(جدید Item[size])
    {
    }

    ~FontCache()
    {
	برای (صحیح i = 0; i < mSize; ++i)
	    SafeRelease(&m_اقلام[i].pTextFormat);
	delete[] m_اقلام;
    }

    منطقی get(HFONT hFont, Item &item)
    {
	صحیح n = find(hFont);
	اگر (n < 0)
	    بازگشت نادرست;
	item = m_اقلام[n];
	slide(n);
	بازگشت درست;
    }

    تهی put(ثابت Item& item)
    {
	صحیح n = find(item.hFont);
	اگر (n < 0)
	    n = mSize - 1;
	اگر (m_اقلام[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_اقلام[n].pTextFormat);
	    اگر (item.pTextFormat != پوچ)
		item.pTextFormat->AddRef();
	}
	m_اقلام[n] = item;
	slide(n);
    }

خصوصی:
    صحیح find(HFONT hFont)
    {
	برای (صحیح i = 0; i < mSize; ++i)
	{
	    اگر (m_اقلام[i].hFont == hFont)
		بازگشت i;
	}
	بازگشت -1;
    }

    تهی slide(صحیح next_بالاترین)
    {
	اگر (next_بالاترین == 0)
	    بازگشت;
	Item tmp = m_اقلام[next_بالاترین];
	برای (صحیح i = next_بالاترین - 1; i >= 0; --i)
	    m_اقلام[i + 1] = m_اقلام[i];
	m_اقلام[0] = tmp;
    }
};

شمارش DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ساختار DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    منطقی mDrawing;
    منطقی mFallbackDC;

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
    صحیح m_Font_Feature_شمار;

    // METHODS

    DWriteContext();

    مجازی ~DWriteContext();

    HRESULT CreateDeviceResources();

    تهی DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ثابت LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ثابت LOGFONTW &logFont);

    تهی SetFont(HFONT hFont);

    تهی Rebind();

    تهی BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    تهی DrawText(ثابت WCHAR *text, صحیح len,
	صحیح x, صحیح y, صحیح w, صحیح h, صحیح cellWidth, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    تهی FillRect(ثابت RECT *rc, COLORREF color);

    تهی Draw_سطر(صحیح x1, صحیح y1, صحیح x2, صحیح y2, COLORREF color);

    تهی SetPixel(صحیح x, صحیح y, COLORREF color);

    تهی Scroll(صحیح x, صحیح y, ثابت RECT *rc);

    تهی Flush();

    تهی SetRenderingParams(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    تهی SetFontFeatures(ثابت DWriteFontFeature *features, صحیح شمار);
};

رده AdjustedGlyphRun : عمومی DWRITE_GLYPH_RUN
{
خصوصی:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عمومی:
    AdjustedGlyphRun(
	    ثابت DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحیح *glyphIndex,
	    صحیح cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جدید FLOAT[glyphRun->glyph_شمار])
    {
	أكد(cellWidth != 0.0f);
	برای (UINT32 i = 0; i < glyphRun->glyph_شمار; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    اگر (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    وگرنه
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

    ایستا FLOAT adjustToCell(FLOAT مقدار, FLOAT cellWidth)
    {
	صحیح cell_شمار = صحیح(floor(مقدار / cellWidth + 0.5f));
	اگر (cell_شمار < 1)
	    cell_شمار = 1;
	بازگشت cell_شمار * cellWidth;
    }
};

ساختار TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ثابت INT *lpDx;
    صحیح cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    صحیح glyphIndex;
};

رده TextRenderer FINAL : عمومی IDWriteTextRenderer
{
عمومی:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_شمار(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    مجازی ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull تهی* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	بازگشت S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull تهی* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	بازگشت S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull تهی* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	اعشاری dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	بازگشت S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull تهی* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ثابت* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	بازگشت E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull تهی* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	بازگشت E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull تهی* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_راست_To_چپ UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	بازگشت E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull تهی* clientDrawingContext,
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
	اگر (pDWC_->mDWriteFactory2 != پوچ)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = پوچ;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		پوچ,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		پوچ,
		0,
		&enumerator);
	    اگر (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		وقتی (hasRun)
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
		بازگشت S_OK;
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
	بازگشت S_OK;
    }

عمومی:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	بازگشت InterlockedIncrement(&c_Ref_شمار);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_شمار_2 = InterlockedDecrement(&c_Ref_شمار);

	اگر (new_شمار_2 == 0)
	{
	    delete این;
	    بازگشت 0;
	}
	بازگشت new_شمار_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ثابت& riid,
	تهی** ppvObject)
    {
	اگر (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = این;
	}
	وگرنه اگر (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = این;
	}
	وگرنه اگر (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = این;
	}
	وگرنه
	{
	    *ppvObject = پوچ;
	    بازگشت E_FAIL;
	}

	بازگشت S_OK;
    }

خصوصی:
    long c_Ref_شمار;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(پوچ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(پوچ),
    mDrawing(نادرست),
    mFallbackDC(نادرست),
    mD2D1Factory(پوچ),
    mRT(پوچ),
    mGDIRT(پوچ),
    mBrush(پوچ),
    mBitmap(پوچ),
    mDWriteFactory(پوچ),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(پوچ),
#endif
    mGdiInterop(پوچ),
    mRenderingParams(پوچ),
    mFontCache(8),
    mTextFormat(پوچ),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_شمار(0)
{
    ZeroMemory(mFontFeatures, اندازه(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), پوچ,
	    reinterpret_cast<تهی**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    اگر (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    اگر (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    اگر (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    اگر (SUCCEEDED(hr))
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

    اگر (mRT != پوچ)
	بازگشت S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    اگر (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<تهی**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    اگر (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    اگر (SUCCEEDED(hr))
	Rebind();

    بازگشت hr;
}

    تهی
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
    IDWriteTextFormat *pTextFormat = پوچ;

    IDWriteFont *font = پوچ;
    IDWriteFontFamily *fontFamily = پوچ;
    IDWriteLocalizedStrings *localizedFamilyNames = پوچ;
    اعشاری fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    اگر (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    اگر (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    اگر (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    اگر (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    اگر (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = اعشاری(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	اگر (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	وگرنه اگر (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * اعشاری(fontMetrics.designUnitsPerEm)
		/ اعشاری(fontMetrics.ascent + fontMetrics.descent + fontMetrics.سطر_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    اگر (SUCCEEDED(hr))
    {
	اگر (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    اگر (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		پوچ, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    اگر (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    اگر (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    اگر (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    اگر (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	اگر (pFontAscent != پوچ && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = اعشاری(fontMetrics.ascent + fontMetrics.descent);
	    اگر (totalHeight != 0.0f)
		*pFontAscent = اعشاری(fontMetrics.ascent) / totalHeight;
	    وگرنه
		*pFontAscent = 0.83f; // fallback
	}
    }
    وگرنه
	SafeRelease(&pTextFormat);

    بازگشت hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = پوچ;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    اگر (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    بازگشت hr;
}

    تهی
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    اگر (mFontCache.get(hFont, item))
    {
	اگر (item.pTextFormat != پوچ)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = نادرست;
	}
	وگرنه
	    mFallbackDC = درست;
	بازگشت;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    اگر (GetObjectW(hFont, اندازه(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    اگر (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = نادرست;
    }
    وگرنه
	mFallbackDC = درست;
    mFontCache.put(item);
}

    تهی
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
	    D2D1::SizeU(mBindRect.راست - mBindRect.چپ,
		mBindRect.bottom - mBindRect.بالاترین),
	    props, &mBitmap);
}

    تهی
DWriteContext::BindDC(HDC hdc, ثابت RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    اگر (mRT == پوچ)
	CreateDeviceResources();
    وگرنه
    {
	Flush();
	Rebind();
    }
}

بیرونی "C" تهی redraw_later_clear(تهی);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    انتخاب (mode)
    {
	پیشفرض:
	حالت DM_GDI:
	    اگر (mInteropHDC != پوچ)
	    {
		mGDIRT->ReleaseDC(پوچ);
		mInteropHDC = پوچ;
	    }
	    اگر (mDrawing)
	    {
		hr = mRT->EndDraw();
		اگر (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = نادرست;
	    }
	    بشکن;

	حالت DM_DIRECTX:
	    اگر (mInteropHDC != پوچ)
	    {
		mGDIRT->ReleaseDC(پوچ);
		mInteropHDC = پوچ;
	    }
	    وگرنه اگر (mDrawing == نادرست)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = درست;
	    }
	    بشکن;

	حالت DM_INTEROP:
	    اگر (mDrawing == نادرست)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = درست;
	    }
	    اگر (mInteropHDC == پوچ)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    بشکن;
    }
    mDMode = mode;
    بازگشت hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    بازگشت mBrush;
}

    تهی
DWriteContext::DrawText(ثابت WCHAR *text, صحیح len,
	صحیح x, صحیح y, صحیح w, صحیح h, صحیح cellWidth, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    اگر (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	اگر (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	بازگشت;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = پوچ;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    اگر ((fuOptions & ETO_CLIPPED) && lprc != پوچ)
    {
	mRT->افزودن_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->چپ), FLOAT(lprc->بالاترین),
			FLOAT(lprc->راست), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    اگر (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	اگر (m_Font_Feature_شمار > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ایستا ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    ایستا ثابت صحیح رقم_Defaults = اندازه(defaultFeatures)
		/ اندازه(defaultFeatures[0]);

	    IDWriteTypography *typography = پوچ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    اگر (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		برای (صحیح d = 0; d < رقم_Defaults; ++d)
		{
		    صحیح overridden = 0;
		    برای (صحیح u = 0; u < m_Font_Feature_شمار; ++u)
		    {
			اگر ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    بشکن;
			}
		    }
		    اگر (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		برای (صحیح i = 0; i < m_Font_Feature_شمار; ++i)
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

	TextRenderer renderer(این);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    اگر ((fuOptions & ETO_CLIPPED) && lprc != پوچ)
	mRT->برداشت_Axis_Aligned_Clip();
}

    تهی
DWriteContext::FillRect(ثابت RECT *rc, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    وگرنه
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->چپ), FLOAT(rc->بالاترین),
		    FLOAT(rc->راست), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    تهی
DWriteContext::Draw_سطر(صحیح x1, صحیح y1, صحیح x2, صحیح y2, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, پوچ);
	::سطر_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    وگرنه
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    تهی
DWriteContext::SetPixel(صحیح x, صحیح y, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    وگرنه
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    تهی
DWriteContext::Scroll(صحیح x, صحیح y, ثابت RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    اگر (x >= 0)
    {
	srcRect.چپ = rc->چپ;
	srcRect.راست = rc->راست - x;
	destPoint.x = rc->چپ + x;
    }
    وگرنه
    {
	srcRect.چپ = rc->چپ - x;
	srcRect.راست = rc->راست;
	destPoint.x = rc->چپ;
    }
    اگر (y >= 0)
    {
	srcRect.بالاترین = rc->بالاترین;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->بالاترین + y;
    }
    وگرنه
    {
	srcRect.بالاترین = rc->بالاترین - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->بالاترین;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.راست - srcRect.چپ),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.بالاترین)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    تهی
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    تهی
DWriteContext::SetRenderingParams(
	ثابت DWriteRenderingParams *params)
{
    اگر (mDWriteFactory == پوچ)
	بازگشت;

    IDWriteRenderingParams *renderingParams = پوچ;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    اگر (params != پوچ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    وگرنه
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    اگر (SUCCEEDED(hr) && renderingParams != پوچ)
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
    اگر (params != پوچ && mRenderingParams != پوچ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    بازگشت params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    تهی
DWrite_Init(تهی)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    اگر (hD2D1DLL == پوچ || hDWriteDLL == پوچ)
    {
	DWrite_Final();
	بازگشت;
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

    تهی
DWrite_Final(تهی)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = پوچ;
    pD2D1CreateFactory = پوچ;
    pDWriteCreateFactory = پوچ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(تهی)
{
#ifdef DYNAMIC_DIRECTX
    اگر (pGetUserDefaultLocaleName == پوچ || pD2D1CreateFactory == پوچ
	    || pDWriteCreateFactory == پوچ)
	بازگشت پوچ;
#endif
    بازگشت جدید DWriteContext();
}

    تهی
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    اگر (ctx != پوچ)
	ctx->BindDC(hdc, rect);
}

    تهی
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    اگر (ctx != پوچ)
	ctx->SetFont(hFont);
}

    تهی
DWriteContext_DrawText(
	DWriteContext *ctx,
	ثابت WCHAR *text,
	صحیح len,
	صحیح x,
	صحیح y,
	صحیح w,
	صحیح h,
	صحیح cellWidth,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    اگر (ctx != پوچ)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    تهی
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    اگر (ctx != پوچ)
	ctx->FillRect(rc, color);
}

    تهی
DWrite_Context_Draw_سطر(DWriteContext *ctx, صحیح x1, صحیح y1, صحیح x2, صحیح y2,
	COLORREF color)
{
    اگر (ctx != پوچ)
	ctx->Draw_سطر(x1, y1, x2, y2, color);
}

    تهی
DWriteContext_SetPixel(DWriteContext *ctx, صحیح x, صحیح y, COLORREF color)
{
    اگر (ctx != پوچ)
	ctx->SetPixel(x, y, color);
}

    تهی
DWriteContext_Scroll(DWriteContext *ctx, صحیح x, صحیح y, ثابت RECT *rc)
{
    اگر (ctx != پوچ)
	ctx->Scroll(x, y, rc);
}

    تهی
DWriteContext_Flush(DWriteContext *ctx)
{
    اگر (ctx != پوچ)
	ctx->Flush();
}

    تهی
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    تهی
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    اگر (ctx != پوچ)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    اگر (ctx != پوچ)
	بازگشت ctx->GetRenderingParams(params);
    وگرنه
	بازگشت پوچ;
}

    تهی
DWriteContext::SetFontFeatures(
	ثابت DWriteFontFeature *features, صحیح شمار)
{
    اگر (شمار > DWRITE_MAX_FONT_FEATURES)
	شمار = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_شمار = شمار;
    اگر (شمار > 0 && features != پوچ)
	memcpy(mFontFeatures, features, اندازه(DWriteFontFeature) * شمار);
}

    تهی
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحیح شمار)
{
    اگر (ctx != پوچ)
	ctx->SetFontFeatures(features, شمار);
}
