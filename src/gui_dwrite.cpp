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
# जइ WINVER < 0x0600
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
#जइ defined(_MSC_VER) && (_MSC_VER < 1700)
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
#अण्णहा
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
#अण्णहा
# define UNUSED
#endif

#जइ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#अण्णहा
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
बाह्य "C" HINSTANCE vimLoadLib(ध्रुव वर्ण *name);

प्रकारनिर्देश पूर्णाङ्क (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, पूर्णाङ्क);
प्रकारनिर्देश HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ध्रुव D2D1_FACTORY_OPTIONS *, शून्य **);
प्रकारनिर्देश HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
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
    जइ (hinst != रिक्त)
    {
	FreeLibrary(hinst);
	hinst = रिक्त;
    }
}
#endif // DYNAMIC_DIRECTX

template <वर्ग T> अन्तःस्थ शून्य SafeRelease(T **ppT)
{
    जइ (*ppT)
    {
	(*ppT)->Release();
	*ppT = रिक्त;
    }
}

    स्थिर DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(पूर्णाङ्क मानम्)
{
    विकल्प (मानम्)
    {
	स्वभाव:
	पक्ष 0:
	    प्रत्यागम DWRITE_PIXEL_GEOMETRY_FLAT;
	पक्ष 1:
	    प्रत्यागम DWRITE_PIXEL_GEOMETRY_RGB;
	पक्ष 2:
	    प्रत्यागम DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    स्थिर पूर्णाङ्क
ToInt(DWRITE_PIXEL_GEOMETRY मानम्)
{
    विकल्प (मानम्)
    {
	पक्ष DWRITE_PIXEL_GEOMETRY_FLAT:
	    प्रत्यागम 0;
	पक्ष DWRITE_PIXEL_GEOMETRY_RGB:
	    प्रत्यागम 1;
	पक्ष DWRITE_PIXEL_GEOMETRY_BGR:
	    प्रत्यागम 2;
	स्वभाव:
	    प्रत्यागम -1;
    }
}

    स्थिर DWRITE_RENDERING_MODE
ToRenderingMode(पूर्णाङ्क मानम्)
{
    विकल्प (मानम्)
    {
	स्वभाव:
	पक्ष 0:
	    प्रत्यागम DWRITE_RENDERING_MODE_DEFAULT;
	पक्ष 1:
	    प्रत्यागम DWRITE_RENDERING_MODE_ALIASED;
	पक्ष 2:
	    प्रत्यागम DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	पक्ष 3:
	    प्रत्यागम DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	पक्ष 4:
	    प्रत्यागम DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	पक्ष 5:
	    प्रत्यागम DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	पक्ष 6:
	    प्रत्यागम DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    स्थिर D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(पूर्णाङ्क मानम्)
{
    विकल्प (मानम्)
    {
	स्वभाव:
	पक्ष 0:
	    प्रत्यागम D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	पक्ष 1:
	    प्रत्यागम D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	पक्ष 2:
	    प्रत्यागम D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	पक्ष 3:
	    प्रत्यागम D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    स्थिर पूर्णाङ्क
ToInt(DWRITE_RENDERING_MODE मानम्)
{
    विकल्प (मानम्)
    {
	पक्ष DWRITE_RENDERING_MODE_DEFAULT:
	    प्रत्यागम 0;
	पक्ष DWRITE_RENDERING_MODE_ALIASED:
	    प्रत्यागम 1;
	पक्ष DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    प्रत्यागम 2;
	पक्ष DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    प्रत्यागम 3;
	पक्ष DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    प्रत्यागम 4;
	पक्ष DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    प्रत्यागम 5;
	पक्ष DWRITE_RENDERING_MODE_OUTLINE:
	    प्रत्यागम 6;
	स्वभाव:
	    प्रत्यागम -1;
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

गुप्त:
    पूर्णाङ्क mSize;
    Item *m_वस्तूनि;

सार्वजनिक:
    FontCache(पूर्णाङ्क size = 2) :
	mSize(size),
	m_वस्तूनि(नूतन Item[size])
    {
    }

    ~FontCache()
    {
	प्रति (पूर्णाङ्क i = 0; i < mSize; ++i)
	    SafeRelease(&m_वस्तूनि[i].pTextFormat);
	delete[] m_वस्तूनि;
    }

    तार्किक get(HFONT hFont, Item &item)
    {
	पूर्णाङ्क n = find(hFont);
	जइ (n < 0)
	    प्रत्यागम मिच्छ;
	item = m_वस्तूनि[n];
	slide(n);
	प्रत्यागम सच्च;
    }

    शून्य put(ध्रुव Item& item)
    {
	पूर्णाङ्क n = find(item.hFont);
	जइ (n < 0)
	    n = mSize - 1;
	जइ (m_वस्तूनि[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_वस्तूनि[n].pTextFormat);
	    जइ (item.pTextFormat != रिक्त)
		item.pTextFormat->AddRef();
	}
	m_वस्तूनि[n] = item;
	slide(n);
    }

गुप्त:
    पूर्णाङ्क find(HFONT hFont)
    {
	प्रति (पूर्णाङ्क i = 0; i < mSize; ++i)
	{
	    जइ (m_वस्तूनि[i].hFont == hFont)
		प्रत्यागम i;
	}
	प्रत्यागम -1;
    }

    शून्य slide(पूर्णाङ्क next_शिखरम्)
    {
	जइ (next_शिखरम् == 0)
	    प्रत्यागम;
	Item tmp = m_वस्तूनि[next_शिखरम्];
	प्रति (पूर्णाङ्क i = next_शिखरम् - 1; i >= 0; --i)
	    m_वस्तूनि[i + 1] = m_वस्तूनि[i];
	m_वस्तूनि[0] = tmp;
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
    पूर्णाङ्क m_Font_Feature_गणनम्;

    // METHODS

    DWriteContext();

    आभास ~DWriteContext();

    HRESULT CreateDeviceResources();

    शून्य DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ध्रुव LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ध्रुव LOGFONTW &logFont);

    शून्य SetFont(HFONT hFont);

    शून्य Rebind();

    शून्य BindDC(HDC hdc, ध्रुव RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    शून्य DrawText(ध्रुव WCHAR *text, पूर्णाङ्क len,
	पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h, पूर्णाङ्क cellWidth, COLORREF color,
	UINT fuOptions, ध्रुव RECT *lprc, ध्रुव INT *lpDx);

    शून्य FillRect(ध्रुव RECT *rc, COLORREF color);

    शून्य Draw_पङ्क्तिः(पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2, COLORREF color);

    शून्य SetPixel(पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color);

    शून्य Scroll(पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc);

    शून्य Flush();

    शून्य SetRenderingParams(
	    ध्रुव DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    शून्य SetFontFeatures(ध्रुव DWriteFontFeature *features, पूर्णाङ्क गणनम्);
};

वर्ग AdjustedGlyphRun : सार्वजनिक DWRITE_GLYPH_RUN
{
गुप्त:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

सार्वजनिक:
    AdjustedGlyphRun(
	    ध्रुव DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ध्रुव INT *lpDx,
	    पूर्णाङ्क *glyphIndex,
	    पूर्णाङ्क cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(नूतन FLOAT[glyphRun->glyph_गणनम्])
    {
	प्रतिज्ञा(cellWidth != 0.0f);
	प्रति (UINT32 i = 0; i < glyphRun->glyph_गणनम्; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    जइ (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    अण्णहा
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

    स्थिर FLOAT adjustToCell(FLOAT मानम्, FLOAT cellWidth)
    {
	पूर्णाङ्क cell_गणनम् = पूर्णाङ्क(floor(मानम् / cellWidth + 0.5f));
	जइ (cell_गणनम् < 1)
	    cell_गणनम् = 1;
	प्रत्यागम cell_गणनम् * cellWidth;
    }
};

संरचना TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ध्रुव INT *lpDx;
    पूर्णाङ्क cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    पूर्णाङ्क glyphIndex;
};

वर्ग TextRenderer FINAL : सार्वजनिक IDWriteTextRenderer
{
सार्वजनिक:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_गणनम्(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    आभास ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	प्रत्यागम S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	प्रत्यागम S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	भिन्न dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	प्रत्यागम S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ध्रुव* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	प्रत्यागम E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ध्रुव* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	प्रत्यागम E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull शून्य* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_दक्षिणः_To_वामः UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	प्रत्यागम E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull शून्य* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ध्रुव* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ध्रुव* glyphRunDescription UNUSED,
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
	जइ (pDWC_->mDWriteFactory2 != रिक्त)
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
	    जइ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		जाव (hasRun)
		{
		    ध्रुव DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		प्रत्यागम S_OK;
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
	प्रत्यागम S_OK;
    }

सार्वजनिक:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	प्रत्यागम InterlockedIncrement(&c_Ref_गणनम्);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_गणनम्_2 = InterlockedDecrement(&c_Ref_गणनम्);

	जइ (new_गणनम्_2 == 0)
	{
	    delete एतत्;
	    प्रत्यागम 0;
	}
	प्रत्यागम new_गणनम्_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ध्रुव& riid,
	शून्य** ppvObject)
    {
	जइ (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = एतत्;
	}
	अण्णहा जइ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = एतत्;
	}
	अण्णहा जइ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = एतत्;
	}
	अण्णहा
	{
	    *ppvObject = रिक्त;
	    प्रत्यागम E_FAIL;
	}

	प्रत्यागम S_OK;
    }

गुप्त:
    long c_Ref_गणनम्;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(रिक्त),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(रिक्त),
    mDrawing(मिच्छ),
    mFallbackDC(मिच्छ),
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
    m_Font_Feature_गणनम्(0)
{
    ZeroMemory(mFontFeatures, परिमाण(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), रिक्त,
	    reinterpret_cast<शून्य**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    जइ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    जइ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    जइ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    जइ (SUCCEEDED(hr))
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

    जइ (mRT != रिक्त)
	प्रत्यागम S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    जइ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<शून्य**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    जइ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    जइ (SUCCEEDED(hr))
	Rebind();

    प्रत्यागम hr;
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
DWriteContext::CreateTextFormatFromLOGFONT(ध्रुव LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = रिक्त;

    IDWriteFont *font = रिक्त;
    IDWriteFontFamily *fontFamily = रिक्त;
    IDWriteLocalizedStrings *localizedFamilyNames = रिक्त;
    भिन्न fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    जइ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    जइ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    जइ (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    जइ (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    जइ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = भिन्न(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	जइ (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	अण्णहा जइ (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * भिन्न(fontMetrics.designUnitsPerEm)
		/ भिन्न(fontMetrics.ascent + fontMetrics.descent + fontMetrics.पङ्क्तिः_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    जइ (SUCCEEDED(hr))
    {
	जइ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    जइ (SUCCEEDED(hr))
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

    जइ (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    जइ (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    जइ (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    जइ (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	जइ (pFontAscent != रिक्त && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = भिन्न(fontMetrics.ascent + fontMetrics.descent);
	    जइ (totalHeight != 0.0f)
		*pFontAscent = भिन्न(fontMetrics.ascent) / totalHeight;
	    अण्णहा
		*pFontAscent = 0.83f; // fallback
	}
    }
    अण्णहा
	SafeRelease(&pTextFormat);

    प्रत्यागम hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ध्रुव LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = रिक्त;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    जइ (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    प्रत्यागम hr;
}

    शून्य
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    जइ (mFontCache.get(hFont, item))
    {
	जइ (item.pTextFormat != रिक्त)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = मिच्छ;
	}
	अण्णहा
	    mFallbackDC = सच्च;
	प्रत्यागम;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    जइ (GetObjectW(hFont, परिमाण(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    जइ (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = मिच्छ;
    }
    अण्णहा
	mFallbackDC = सच्च;
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
	    D2D1::SizeU(mBindRect.दक्षिणः - mBindRect.वामः,
		mBindRect.bottom - mBindRect.शिखरम्),
	    props, &mBitmap);
}

    शून्य
DWriteContext::BindDC(HDC hdc, ध्रुव RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    जइ (mRT == रिक्त)
	CreateDeviceResources();
    अण्णहा
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

    विकल्प (mode)
    {
	स्वभाव:
	पक्ष DM_GDI:
	    जइ (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    जइ (mDrawing)
	    {
		hr = mRT->EndDraw();
		जइ (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = मिच्छ;
	    }
	    भङ्ग;

	पक्ष DM_DIRECTX:
	    जइ (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    अण्णहा जइ (mDrawing == मिच्छ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सच्च;
	    }
	    भङ्ग;

	पक्ष DM_INTEROP:
	    जइ (mDrawing == मिच्छ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सच्च;
	    }
	    जइ (mInteropHDC == रिक्त)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    भङ्ग;
    }
    mDMode = mode;
    प्रत्यागम hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    प्रत्यागम mBrush;
}

    शून्य
DWriteContext::DrawText(ध्रुव WCHAR *text, पूर्णाङ्क len,
	पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h, पूर्णाङ्क cellWidth, COLORREF color,
	UINT fuOptions, ध्रुव RECT *lprc, ध्रुव INT *lpDx)
{
    जइ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	जइ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	प्रत्यागम;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = रिक्त;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    जइ ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
    {
	mRT->प्रक्षिप_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->वामः), FLOAT(lprc->शिखरम्),
			FLOAT(lprc->दक्षिणः), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    जइ (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	जइ (m_Font_Feature_गणनम् > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    स्थिर ध्रुव DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    स्थिर ध्रुव पूर्णाङ्क अङ्कः_Defaults = परिमाण(defaultFeatures)
		/ परिमाण(defaultFeatures[0]);

	    IDWriteTypography *typography = रिक्त;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    जइ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		प्रति (पूर्णाङ्क d = 0; d < अङ्कः_Defaults; ++d)
		{
		    पूर्णाङ्क overridden = 0;
		    प्रति (पूर्णाङ्क u = 0; u < m_Font_Feature_गणनम्; ++u)
		    {
			जइ ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    भङ्ग;
			}
		    }
		    जइ (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		प्रति (पूर्णाङ्क i = 0; i < m_Font_Feature_गणनम्; ++i)
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

	TextRenderer renderer(एतत्);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    जइ ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
	mRT->निष्कासय_Axis_Aligned_Clip();
}

    शून्य
DWriteContext::FillRect(ध्रुव RECT *rc, COLORREF color)
{
    जइ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    अण्णहा
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->वामः), FLOAT(rc->शिखरम्),
		    FLOAT(rc->दक्षिणः), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::Draw_पङ्क्तिः(पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2, COLORREF color)
{
    जइ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, रिक्त);
	::पङ्क्तिः_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    अण्णहा
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_पङ्क्तिः(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::SetPixel(पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color)
{
    जइ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    अण्णहा
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_पङ्क्तिः(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::Scroll(पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    जइ (x >= 0)
    {
	srcRect.वामः = rc->वामः;
	srcRect.दक्षिणः = rc->दक्षिणः - x;
	destPoint.x = rc->वामः + x;
    }
    अण्णहा
    {
	srcRect.वामः = rc->वामः - x;
	srcRect.दक्षिणः = rc->दक्षिणः;
	destPoint.x = rc->वामः;
    }
    जइ (y >= 0)
    {
	srcRect.शिखरम् = rc->शिखरम्;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->शिखरम् + y;
    }
    अण्णहा
    {
	srcRect.शिखरम् = rc->शिखरम् - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->शिखरम्;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.दक्षिणः - srcRect.वामः),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.शिखरम्)
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
	ध्रुव DWriteRenderingParams *params)
{
    जइ (mDWriteFactory == रिक्त)
	प्रत्यागम;

    IDWriteRenderingParams *renderingParams = रिक्त;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    जइ (params != रिक्त)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    अण्णहा
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    जइ (SUCCEEDED(hr) && renderingParams != रिक्त)
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
    जइ (params != रिक्त && mRenderingParams != रिक्त)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    प्रत्यागम params;
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
    जइ (hD2D1DLL == रिक्त || hDWriteDLL == रिक्त)
    {
	DWrite_Final();
	प्रत्यागम;
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
    जइ (pGetUserDefaultLocaleName == रिक्त || pD2D1CreateFactory == रिक्त
	    || pDWriteCreateFactory == रिक्त)
	प्रत्यागम रिक्त;
#endif
    प्रत्यागम नूतन DWriteContext();
}

    शून्य
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ध्रुव RECT *rect)
{
    जइ (ctx != रिक्त)
	ctx->BindDC(hdc, rect);
}

    शून्य
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    जइ (ctx != रिक्त)
	ctx->SetFont(hFont);
}

    शून्य
DWriteContext_DrawText(
	DWriteContext *ctx,
	ध्रुव WCHAR *text,
	पूर्णाङ्क len,
	पूर्णाङ्क x,
	पूर्णाङ्क y,
	पूर्णाङ्क w,
	पूर्णाङ्क h,
	पूर्णाङ्क cellWidth,
	COLORREF color,
	UINT fuOptions,
	ध्रुव RECT *lprc,
	ध्रुव INT *lpDx)
{
    जइ (ctx != रिक्त)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    शून्य
DWriteContext_FillRect(DWriteContext *ctx, ध्रुव RECT *rc, COLORREF color)
{
    जइ (ctx != रिक्त)
	ctx->FillRect(rc, color);
}

    शून्य
DWrite_Context_Draw_पङ्क्तिः(DWriteContext *ctx, पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2,
	COLORREF color)
{
    जइ (ctx != रिक्त)
	ctx->Draw_पङ्क्तिः(x1, y1, x2, y2, color);
}

    शून्य
DWriteContext_SetPixel(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color)
{
    जइ (ctx != रिक्त)
	ctx->SetPixel(x, y, color);
}

    शून्य
DWriteContext_Scroll(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc)
{
    जइ (ctx != रिक्त)
	ctx->Scroll(x, y, rc);
}

    शून्य
DWriteContext_Flush(DWriteContext *ctx)
{
    जइ (ctx != रिक्त)
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
	ध्रुव DWriteRenderingParams *params)
{
    जइ (ctx != रिक्त)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    जइ (ctx != रिक्त)
	प्रत्यागम ctx->GetRenderingParams(params);
    अण्णहा
	प्रत्यागम रिक्त;
}

    शून्य
DWriteContext::SetFontFeatures(
	ध्रुव DWriteFontFeature *features, पूर्णाङ्क गणनम्)
{
    जइ (गणनम् > DWRITE_MAX_FONT_FEATURES)
	गणनम् = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_गणनम् = गणनम्;
    जइ (गणनम् > 0 && features != रिक्त)
	memcpy(mFontFeatures, features, परिमाण(DWriteFontFeature) * गणनम्);
}

    शून्य
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ध्रुव DWriteFontFeature *features,
	पूर्णाङ्क गणनम्)
{
    जइ (ctx != रिक्त)
	ctx->SetFontFeatures(features, गणनम्);
}
