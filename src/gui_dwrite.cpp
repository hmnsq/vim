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
# जर WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#अन्तर्भाव <windows.h>
#अन्तर्भाव <crtdbg.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <d2d1.h>
#अन्तर्भाव <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#जर defined(_MSC_VER) && (_MSC_VER < 1700)
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
#अन्यथा
# include <dwrite.h>
#endif

#अन्तर्भाव "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#अन्यथा
# define UNUSED
#endif

#जर (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#अन्यथा
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
बाह्य "C" HINSTANCE vimLoadLib(स्थिरांक अक्षर *name);

प्रकारव्याख्या पूर्णांक (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, पूर्णांक);
प्रकारव्याख्या HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, स्थिरांक D2D1_FACTORY_OPTIONS *, शून्य **);
प्रकारव्याख्या HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

स्थिर HINSTANCE hD2D1DLL = रिक्त;
स्थिर HINSTANCE hDWriteDLL = रिक्त;

स्थिर PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = रिक्त;
स्थिर PD2D1CREATEFACTORY pD2D1CreateFactory = रिक्त;
स्थिर PDWRITECREATEFACTORY pDWriteCreateFactory = रिक्त;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    स्थिर शून्य
unload(HINSTANCE &hinst)
{
    जर (hinst != रिक्त)
    {
	FreeLibrary(hinst);
	hinst = रिक्त;
    }
}
#endif // DYNAMIC_DIRECTX

ढाँचा <वर्ग T> ओळीत शून्य SafeRelease(T **ppT)
{
    जर (*ppT)
    {
	(*ppT)->Release();
	*ppT = रिक्त;
    }
}

    स्थिर DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(पूर्णांक मूल्य)
{
    निवड (मूल्य)
    {
	पूर्वनिर्धारित:
	स्थिती 0:
	    परत DWRITE_PIXEL_GEOMETRY_FLAT;
	स्थिती 1:
	    परत DWRITE_PIXEL_GEOMETRY_RGB;
	स्थिती 2:
	    परत DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    स्थिर पूर्णांक
ToInt(DWRITE_PIXEL_GEOMETRY मूल्य)
{
    निवड (मूल्य)
    {
	स्थिती DWRITE_PIXEL_GEOMETRY_FLAT:
	    परत 0;
	स्थिती DWRITE_PIXEL_GEOMETRY_RGB:
	    परत 1;
	स्थिती DWRITE_PIXEL_GEOMETRY_BGR:
	    परत 2;
	पूर्वनिर्धारित:
	    परत -1;
    }
}

    स्थिर DWRITE_RENDERING_MODE
ToRenderingMode(पूर्णांक मूल्य)
{
    निवड (मूल्य)
    {
	पूर्वनिर्धारित:
	स्थिती 0:
	    परत DWRITE_RENDERING_MODE_DEFAULT;
	स्थिती 1:
	    परत DWRITE_RENDERING_MODE_ALIASED;
	स्थिती 2:
	    परत DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	स्थिती 3:
	    परत DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	स्थिती 4:
	    परत DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	स्थिती 5:
	    परत DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	स्थिती 6:
	    परत DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    स्थिर D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(पूर्णांक मूल्य)
{
    निवड (मूल्य)
    {
	पूर्वनिर्धारित:
	स्थिती 0:
	    परत D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	स्थिती 1:
	    परत D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	स्थिती 2:
	    परत D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	स्थिती 3:
	    परत D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    स्थिर पूर्णांक
ToInt(DWRITE_RENDERING_MODE मूल्य)
{
    निवड (मूल्य)
    {
	स्थिती DWRITE_RENDERING_MODE_DEFAULT:
	    परत 0;
	स्थिती DWRITE_RENDERING_MODE_ALIASED:
	    परत 1;
	स्थिती DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    परत 2;
	स्थिती DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    परत 3;
	स्थिती DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    परत 4;
	स्थिती DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    परत 5;
	स्थिती DWRITE_RENDERING_MODE_OUTLINE:
	    परत 6;
	पूर्वनिर्धारित:
	    परत -1;
    }
}

वर्ग FontCache {
सार्वजनिक:
    संरचना Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(रिक्त), pTextFormat(रिक्त), fontAscent(0.0f) {}
    };

खाजगी:
    पूर्णांक mSize;
    Item *m_वस्तुसंच;

सार्वजनिक:
    FontCache(पूर्णांक size = 2) :
	mSize(size),
	m_वस्तुसंच(नवीन Item[size])
    {
    }

    ~FontCache()
    {
	साठी (पूर्णांक i = 0; i < mSize; ++i)
	    SafeRelease(&m_वस्तुसंच[i].pTextFormat);
	delete[] m_वस्तुसंच;
    }

    तार्किक get(HFONT hFont, Item &item)
    {
	पूर्णांक n = find(hFont);
	जर (n < 0)
	    परत असत्य;
	item = m_वस्तुसंच[n];
	slide(n);
	परत सत्य;
    }

    शून्य put(स्थिरांक Item& item)
    {
	पूर्णांक n = find(item.hFont);
	जर (n < 0)
	    n = mSize - 1;
	जर (m_वस्तुसंच[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_वस्तुसंच[n].pTextFormat);
	    जर (item.pTextFormat != रिक्त)
		item.pTextFormat->AddRef();
	}
	m_वस्तुसंच[n] = item;
	slide(n);
    }

खाजगी:
    पूर्णांक find(HFONT hFont)
    {
	साठी (पूर्णांक i = 0; i < mSize; ++i)
	{
	    जर (m_वस्तुसंच[i].hFont == hFont)
		परत i;
	}
	परत -1;
    }

    शून्य slide(पूर्णांक next_शीर्ष)
    {
	जर (next_शीर्ष == 0)
	    परत;
	Item tmp = m_वस्तुसंच[next_शीर्ष];
	साठी (पूर्णांक i = next_शीर्ष - 1; i >= 0; --i)
	    m_वस्तुसंच[i + 1] = m_वस्तुसंच[i];
	m_वस्तुसंच[0] = tmp;
    }
};

गणना DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

संरचना DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    तार्किक mDrawing;
    तार्किक mFallbackDC;

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
    पूर्णांक m_Font_Feature_मोजणी;

    // METHODS

    DWriteContext();

    आभासी ~DWriteContext();

    HRESULT CreateDeviceResources();

    शून्य DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(स्थिरांक LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(स्थिरांक LOGFONTW &logFont);

    शून्य SetFont(HFONT hFont);

    शून्य Rebind();

    शून्य BindDC(HDC hdc, स्थिरांक RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    शून्य DrawText(स्थिरांक WCHAR *text, पूर्णांक len,
	पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h, पूर्णांक cellWidth, COLORREF color,
	UINT fuOptions, स्थिरांक RECT *lprc, स्थिरांक INT *lpDx);

    शून्य FillRect(स्थिरांक RECT *rc, COLORREF color);

    शून्य Draw_ओळ(पूर्णांक x1, पूर्णांक y1, पूर्णांक x2, पूर्णांक y2, COLORREF color);

    शून्य SetPixel(पूर्णांक x, पूर्णांक y, COLORREF color);

    शून्य Scroll(पूर्णांक x, पूर्णांक y, स्थिरांक RECT *rc);

    शून्य Flush();

    शून्य SetRenderingParams(
	    स्थिरांक DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    शून्य SetFontFeatures(स्थिरांक DWriteFontFeature *features, पूर्णांक मोजणी);
};

वर्ग AdjustedGlyphRun : सार्वजनिक DWRITE_GLYPH_RUN
{
खाजगी:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

सार्वजनिक:
    AdjustedGlyphRun(
	    स्थिरांक DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    स्थिरांक INT *lpDx,
	    पूर्णांक *glyphIndex,
	    पूर्णांक cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(नवीन FLOAT[glyphRun->glyph_मोजणी])
    {
	प्रतिज्ञा(cellWidth != 0.0f);
	साठी (UINT32 i = 0; i < glyphRun->glyph_मोजणी; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    जर (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    अन्यथा
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

    स्थिर FLOAT adjustToCell(FLOAT मूल्य, FLOAT cellWidth)
    {
	पूर्णांक cell_मोजणी = पूर्णांक(न्यूनमान(मूल्य / cellWidth + 0.5f));
	जर (cell_मोजणी < 1)
	    cell_मोजणी = 1;
	परत cell_मोजणी * cellWidth;
    }
};

संरचना TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    स्थिरांक INT *lpDx;
    पूर्णांक cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    पूर्णांक glyphIndex;
};

वर्ग TextRenderer FINAL : सार्वजनिक IDWriteTextRenderer
{
सार्वजनिक:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_मोजणी(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    आभासी ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	परत S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	परत S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	दशांश dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	परत S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE स्थिरांक* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	परत E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH स्थिरांक* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	परत E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_उजवा_To_डावा UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	परत E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull शून्य* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN स्थिरांक* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION स्थिरांक* glyphRunDescription UNUSED,
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
	जर (pDWC_->mDWriteFactory2 != रिक्त)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = रिक्त;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		रिक्त,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		रिक्त,
		0,
		&enumerator);
	    जर (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		पर्यंत (hasRun)
		{
		    स्थिरांक DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		परत S_OK;
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
	परत S_OK;
    }

सार्वजनिक:
    IFACEMETHOD_(अचिन्हित दीर्घ, AddRef) ()
    {
	परत InterlockedIncrement(&c_Ref_मोजणी);
    }

    IFACEMETHOD_(अचिन्हित दीर्घ, Release) ()
    {
	दीर्घ new_मोजणी_2 = InterlockedDecrement(&c_Ref_मोजणी);

	जर (new_मोजणी_2 == 0)
	{
	    delete हे;
	    परत 0;
	}
	परत new_मोजणी_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID स्थिरांक& riid,
	शून्य** ppvObject)
    {
	जर (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = हे;
	}
	अन्यथा जर (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = हे;
	}
	अन्यथा जर (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = हे;
	}
	अन्यथा
	{
	    *ppvObject = रिक्त;
	    परत E_FAIL;
	}

	परत S_OK;
    }

खाजगी:
    दीर्घ c_Ref_मोजणी;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(रिक्त),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(रिक्त),
    mDrawing(असत्य),
    mFallbackDC(असत्य),
    mD2D1Factory(रिक्त),
    mRT(रिक्त),
    mGDIRT(रिक्त),
    mBrush(रिक्त),
    mBitmap(रिक्त),
    mDWriteFactory(रिक्त),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(रिक्त),
#endif
    mGdiInterop(रिक्त),
    mRenderingParams(रिक्त),
    mFontCache(8),
    mTextFormat(रिक्त),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_मोजणी(0)
{
    ZeroMemory(mFontFeatures, आकार(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), रिक्त,
	    reinterpret_cast<शून्य**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    जर (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    जर (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    जर (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    जर (SUCCEEDED(hr))
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

    जर (mRT != रिक्त)
	परत S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    जर (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<शून्य**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    जर (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    जर (SUCCEEDED(hr))
	Rebind();

    परत hr;
}

    शून्य
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(स्थिरांक LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = रिक्त;

    IDWriteFont *font = रिक्त;
    IDWriteFontFamily *fontFamily = रिक्त;
    IDWriteLocalizedStrings *localizedFamilyNames = रिक्त;
    दशांश fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    जर (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    जर (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    जर (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    ब_अक्षर_प्रकार familyName[100];
    जर (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    जर (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = दशांश(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	जर (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	अन्यथा जर (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * दशांश(fontMetrics.designUnitsPerEm)
		/ दशांश(fontMetrics.ascent + fontMetrics.descent + fontMetrics.ओळ_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    ब_अक्षर_प्रकार localeName[LOCALE_NAME_MAX_LENGTH];
    जर (SUCCEEDED(hr))
    {
	जर (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    जर (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		रिक्त, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    जर (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    जर (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    जर (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    जर (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	जर (pFontAscent != रिक्त && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = दशांश(fontMetrics.ascent + fontMetrics.descent);
	    जर (totalHeight != 0.0f)
		*pFontAscent = दशांश(fontMetrics.ascent) / totalHeight;
	    अन्यथा
		*pFontAscent = 0.83f; // fallback
	}
    }
    अन्यथा
	SafeRelease(&pTextFormat);

    परत hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(स्थिरांक LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = रिक्त;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    जर (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    परत hr;
}

    शून्य
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    जर (mFontCache.get(hFont, item))
    {
	जर (item.pTextFormat != रिक्त)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = असत्य;
	}
	अन्यथा
	    mFallbackDC = सत्य;
	परत;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    जर (GetObjectW(hFont, आकार(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    जर (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = असत्य;
    }
    अन्यथा
	mFallbackDC = सत्य;
    mFontCache.put(item);
}

    शून्य
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
	    D2D1::SizeU(mBindRect.उजवा - mBindRect.डावा,
		mBindRect.bottom - mBindRect.शीर्ष),
	    props, &mBitmap);
}

    शून्य
DWriteContext::BindDC(HDC hdc, स्थिरांक RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    जर (mRT == रिक्त)
	CreateDeviceResources();
    अन्यथा
    {
	Flush();
	Rebind();
    }
}

बाह्य "C" शून्य redraw_later_clear(शून्य);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    निवड (mode)
    {
	पूर्वनिर्धारित:
	स्थिती DM_GDI:
	    जर (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    जर (mDrawing)
	    {
		hr = mRT->EndDraw();
		जर (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = असत्य;
	    }
	    थांबा;

	स्थिती DM_DIRECTX:
	    जर (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    अन्यथा जर (mDrawing == असत्य)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सत्य;
	    }
	    थांबा;

	स्थिती DM_INTEROP:
	    जर (mDrawing == असत्य)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सत्य;
	    }
	    जर (mInteropHDC == रिक्त)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    थांबा;
    }
    mDMode = mode;
    परत hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    परत mBrush;
}

    शून्य
DWriteContext::DrawText(स्थिरांक WCHAR *text, पूर्णांक len,
	पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h, पूर्णांक cellWidth, COLORREF color,
	UINT fuOptions, स्थिरांक RECT *lprc, स्थिरांक INT *lpDx)
{
    जर (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	जर (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	परत;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = रिक्त;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    जर ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
    {
	mRT->ढकला_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->डावा), FLOAT(lprc->शीर्ष),
			FLOAT(lprc->उजवा), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    जर (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	जर (m_Font_Feature_मोजणी > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    स्थिर स्थिरांक DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    स्थिर स्थिरांक पूर्णांक अंक_Defaults = आकार(defaultFeatures)
		/ आकार(defaultFeatures[0]);

	    IDWriteTypography *typography = रिक्त;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    जर (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		साठी (पूर्णांक d = 0; d < अंक_Defaults; ++d)
		{
		    पूर्णांक overridden = 0;
		    साठी (पूर्णांक u = 0; u < m_Font_Feature_मोजणी; ++u)
		    {
			जर ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    थांबा;
			}
		    }
		    जर (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		साठी (पूर्णांक i = 0; i < m_Font_Feature_मोजणी; ++i)
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

	TextRenderer renderer(हे);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    जर ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
	mRT->काढा_Axis_Aligned_Clip();
}

    शून्य
DWriteContext::FillRect(स्थिरांक RECT *rc, COLORREF color)
{
    जर (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    अन्यथा
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->डावा), FLOAT(rc->शीर्ष),
		    FLOAT(rc->उजवा), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::Draw_ओळ(पूर्णांक x1, पूर्णांक y1, पूर्णांक x2, पूर्णांक y2, COLORREF color)
{
    जर (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, रिक्त);
	::ओळ_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    अन्यथा
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_ओळ(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::SetPixel(पूर्णांक x, पूर्णांक y, COLORREF color)
{
    जर (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    अन्यथा
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_ओळ(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::Scroll(पूर्णांक x, पूर्णांक y, स्थिरांक RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    जर (x >= 0)
    {
	srcRect.डावा = rc->डावा;
	srcRect.उजवा = rc->उजवा - x;
	destPoint.x = rc->डावा + x;
    }
    अन्यथा
    {
	srcRect.डावा = rc->डावा - x;
	srcRect.उजवा = rc->उजवा;
	destPoint.x = rc->डावा;
    }
    जर (y >= 0)
    {
	srcRect.शीर्ष = rc->शीर्ष;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->शीर्ष + y;
    }
    अन्यथा
    {
	srcRect.शीर्ष = rc->शीर्ष - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->शीर्ष;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.उजवा - srcRect.डावा),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.शीर्ष)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    शून्य
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    शून्य
DWriteContext::SetRenderingParams(
	स्थिरांक DWriteRenderingParams *params)
{
    जर (mDWriteFactory == रिक्त)
	परत;

    IDWriteRenderingParams *renderingParams = रिक्त;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    जर (params != रिक्त)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    अन्यथा
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    जर (SUCCEEDED(hr) && renderingParams != रिक्त)
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
    जर (params != रिक्त && mRenderingParams != रिक्त)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    परत params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    शून्य
DWrite_Init(शून्य)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    जर (hD2D1DLL == रिक्त || hDWriteDLL == रिक्त)
    {
	DWrite_Final();
	परत;
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

    शून्य
DWrite_Final(शून्य)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = रिक्त;
    pD2D1CreateFactory = रिक्त;
    pDWriteCreateFactory = रिक्त;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(शून्य)
{
#ifdef DYNAMIC_DIRECTX
    जर (pGetUserDefaultLocaleName == रिक्त || pD2D1CreateFactory == रिक्त
	    || pDWriteCreateFactory == रिक्त)
	परत रिक्त;
#endif
    परत नवीन DWriteContext();
}

    शून्य
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, स्थिरांक RECT *rect)
{
    जर (ctx != रिक्त)
	ctx->BindDC(hdc, rect);
}

    शून्य
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    जर (ctx != रिक्त)
	ctx->SetFont(hFont);
}

    शून्य
DWriteContext_DrawText(
	DWriteContext *ctx,
	स्थिरांक WCHAR *text,
	पूर्णांक len,
	पूर्णांक x,
	पूर्णांक y,
	पूर्णांक w,
	पूर्णांक h,
	पूर्णांक cellWidth,
	COLORREF color,
	UINT fuOptions,
	स्थिरांक RECT *lprc,
	स्थिरांक INT *lpDx)
{
    जर (ctx != रिक्त)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    शून्य
DWriteContext_FillRect(DWriteContext *ctx, स्थिरांक RECT *rc, COLORREF color)
{
    जर (ctx != रिक्त)
	ctx->FillRect(rc, color);
}

    शून्य
DWrite_Context_Draw_ओळ(DWriteContext *ctx, पूर्णांक x1, पूर्णांक y1, पूर्णांक x2, पूर्णांक y2,
	COLORREF color)
{
    जर (ctx != रिक्त)
	ctx->Draw_ओळ(x1, y1, x2, y2, color);
}

    शून्य
DWriteContext_SetPixel(DWriteContext *ctx, पूर्णांक x, पूर्णांक y, COLORREF color)
{
    जर (ctx != रिक्त)
	ctx->SetPixel(x, y, color);
}

    शून्य
DWriteContext_Scroll(DWriteContext *ctx, पूर्णांक x, पूर्णांक y, स्थिरांक RECT *rc)
{
    जर (ctx != रिक्त)
	ctx->Scroll(x, y, rc);
}

    शून्य
DWriteContext_Flush(DWriteContext *ctx)
{
    जर (ctx != रिक्त)
	ctx->Flush();
}

    शून्य
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    शून्य
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	स्थिरांक DWriteRenderingParams *params)
{
    जर (ctx != रिक्त)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    जर (ctx != रिक्त)
	परत ctx->GetRenderingParams(params);
    अन्यथा
	परत रिक्त;
}

    शून्य
DWriteContext::SetFontFeatures(
	स्थिरांक DWriteFontFeature *features, पूर्णांक मोजणी)
{
    जर (मोजणी > DWRITE_MAX_FONT_FEATURES)
	मोजणी = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_मोजणी = मोजणी;
    जर (मोजणी > 0 && features != रिक्त)
	स_नकल(mFontFeatures, features, आकार(DWriteFontFeature) * मोजणी);
}

    शून्य
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	स्थिरांक DWriteFontFeature *features,
	पूर्णांक मोजणी)
{
    जर (ctx != रिक्त)
	ctx->SetFontFeatures(features, मोजणी);
}
