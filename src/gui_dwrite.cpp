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
# यदि WINVER < 0x0600
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
#यदि defined(_MSC_VER) && (_MSC_VER < 1700)
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

#यदि (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#अन्यथा
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
    यदि (hinst != रिक्त)
    {
	FreeLibrary(hinst);
	hinst = रिक्त;
    }
}
#endif // DYNAMIC_DIRECTX

ढाँचा <वर्ग T> अन्तःस्थ शून्य SafeRelease(T **ppT)
{
    यदि (*ppT)
    {
	(*ppT)->Release();
	*ppT = रिक्त;
    }
}

    स्थिर DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(पूर्णाङ्क मूल्य)
{
    विकल्प (मूल्य)
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
ToInt(DWRITE_PIXEL_GEOMETRY मूल्य)
{
    विकल्प (मूल्य)
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
ToRenderingMode(पूर्णाङ्क मूल्य)
{
    विकल्प (मूल्य)
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
ToTextAntialiasMode(पूर्णाङ्क मूल्य)
{
    विकल्प (मूल्य)
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
ToInt(DWRITE_RENDERING_MODE मूल्य)
{
    विकल्प (मूल्य)
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
    Item *m_वस्तुएँ;

सार्वजनिक:
    FontCache(पूर्णाङ्क size = 2) :
	mSize(size),
	m_वस्तुएँ(नूतन Item[size])
    {
    }

    ~FontCache()
    {
	प्रति (पूर्णाङ्क i = 0; i < mSize; ++i)
	    SafeRelease(&m_वस्तुएँ[i].pTextFormat);
	delete[] m_वस्तुएँ;
    }

    तार्किक get(HFONT hFont, Item &item)
    {
	पूर्णाङ्क n = find(hFont);
	यदि (n < 0)
	    प्रत्यागम असत्य;
	item = m_वस्तुएँ[n];
	slide(n);
	प्रत्यागम सत्य;
    }

    शून्य put(ध्रुव Item& item)
    {
	पूर्णाङ्क n = find(item.hFont);
	यदि (n < 0)
	    n = mSize - 1;
	यदि (m_वस्तुएँ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_वस्तुएँ[n].pTextFormat);
	    यदि (item.pTextFormat != रिक्त)
		item.pTextFormat->AddRef();
	}
	m_वस्तुएँ[n] = item;
	slide(n);
    }

गुप्त:
    पूर्णाङ्क find(HFONT hFont)
    {
	प्रति (पूर्णाङ्क i = 0; i < mSize; ++i)
	{
	    यदि (m_वस्तुएँ[i].hFont == hFont)
		प्रत्यागम i;
	}
	प्रत्यागम -1;
    }

    शून्य slide(पूर्णाङ्क next_शीर्ष)
    {
	यदि (next_शीर्ष == 0)
	    प्रत्यागम;
	Item tmp = m_वस्तुएँ[next_शीर्ष];
	प्रति (पूर्णाङ्क i = next_शीर्ष - 1; i >= 0; --i)
	    m_वस्तुएँ[i + 1] = m_वस्तुएँ[i];
	m_वस्तुएँ[0] = tmp;
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
    पूर्णाङ्क m_Font_Feature_गिनती;

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

    शून्य Draw_पंक्ति(पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2, COLORREF color);

    शून्य SetPixel(पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color);

    शून्य Scroll(पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc);

    शून्य Flush();

    शून्य SetRenderingParams(
	    ध्रुव DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    शून्य SetFontFeatures(ध्रुव DWriteFontFeature *features, पूर्णाङ्क गिनती);
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
	mAdjustedAdvances(नूतन FLOAT[glyphRun->glyph_गिनती])
    {
	प्रतिज्ञा(cellWidth != 0.0f);
	प्रति (UINT32 i = 0; i < glyphRun->glyph_गिनती; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    यदि (*glyphIndex < cchText)
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
	पूर्णाङ्क cell_गिनती = पूर्णाङ्क(न्यूनमान(मूल्य / cellWidth + 0.5f));
	यदि (cell_गिनती < 1)
	    cell_गिनती = 1;
	प्रत्यागम cell_गिनती * cellWidth;
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
	c_Ref_गिनती(0),
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
	BOOL is_दायाँ_To_बायाँ UNUSED,
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
	यदि (pDWC_->mDWriteFactory2 != रिक्त)
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
	    यदि (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		यावत् (hasRun)
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
    IFACEMETHOD_(अचिन्हित दीर्घ, AddRef) ()
    {
	प्रत्यागम InterlockedIncrement(&c_Ref_गिनती);
    }

    IFACEMETHOD_(अचिन्हित दीर्घ, Release) ()
    {
	दीर्घ new_गिनती_2 = InterlockedDecrement(&c_Ref_गिनती);

	यदि (new_गिनती_2 == 0)
	{
	    delete एतत्;
	    प्रत्यागम 0;
	}
	प्रत्यागम new_गिनती_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ध्रुव& riid,
	शून्य** ppvObject)
    {
	यदि (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = एतत्;
	}
	अन्यथा यदि (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = एतत्;
	}
	अन्यथा यदि (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = एतत्;
	}
	अन्यथा
	{
	    *ppvObject = रिक्त;
	    प्रत्यागम E_FAIL;
	}

	प्रत्यागम S_OK;
    }

गुप्त:
    दीर्घ c_Ref_गिनती;
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
    m_Font_Feature_गिनती(0)
{
    ZeroMemory(mFontFeatures, परिमाण(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), रिक्त,
	    reinterpret_cast<शून्य**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    यदि (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    यदि (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    यदि (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    यदि (SUCCEEDED(hr))
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

    यदि (mRT != रिक्त)
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

    यदि (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<शून्य**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    यदि (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    यदि (SUCCEEDED(hr))
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

    यदि (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    यदि (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    यदि (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    ब_अक्षर_प्रकार familyName[100];
    यदि (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    यदि (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = भिन्न(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	यदि (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	अन्यथा यदि (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * भिन्न(fontMetrics.designUnitsPerEm)
		/ भिन्न(fontMetrics.ascent + fontMetrics.descent + fontMetrics.पंक्ति_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    ब_अक्षर_प्रकार localeName[LOCALE_NAME_MAX_LENGTH];
    यदि (SUCCEEDED(hr))
    {
	यदि (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    यदि (SUCCEEDED(hr))
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

    यदि (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    यदि (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    यदि (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    यदि (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	यदि (pFontAscent != रिक्त && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = भिन्न(fontMetrics.ascent + fontMetrics.descent);
	    यदि (totalHeight != 0.0f)
		*pFontAscent = भिन्न(fontMetrics.ascent) / totalHeight;
	    अन्यथा
		*pFontAscent = 0.83f; // fallback
	}
    }
    अन्यथा
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

    यदि (SUCCEEDED(hr))
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
    यदि (mFontCache.get(hFont, item))
    {
	यदि (item.pTextFormat != रिक्त)
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
	प्रत्यागम;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    यदि (GetObjectW(hFont, परिमाण(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    यदि (SUCCEEDED(hr))
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
	    D2D1::SizeU(mBindRect.दायाँ - mBindRect.बायाँ,
		mBindRect.bottom - mBindRect.शीर्ष),
	    props, &mBitmap);
}

    शून्य
DWriteContext::BindDC(HDC hdc, ध्रुव RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    यदि (mRT == रिक्त)
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

    विकल्प (mode)
    {
	स्वभाव:
	पक्ष DM_GDI:
	    यदि (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    यदि (mDrawing)
	    {
		hr = mRT->EndDraw();
		यदि (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = असत्य;
	    }
	    भङ्ग;

	पक्ष DM_DIRECTX:
	    यदि (mInteropHDC != रिक्त)
	    {
		mGDIRT->ReleaseDC(रिक्त);
		mInteropHDC = रिक्त;
	    }
	    अन्यथा यदि (mDrawing == असत्य)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सत्य;
	    }
	    भङ्ग;

	पक्ष DM_INTEROP:
	    यदि (mDrawing == असत्य)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = सत्य;
	    }
	    यदि (mInteropHDC == रिक्त)
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
    यदि (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	यदि (SUCCEEDED(hr))
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
    यदि ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
    {
	mRT->धकेलो_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->बायाँ), FLOAT(lprc->शीर्ष),
			FLOAT(lprc->दायाँ), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    यदि (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	यदि (m_Font_Feature_गिनती > 0)
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
	    स्थिर ध्रुव पूर्णाङ्क अंक_Defaults = परिमाण(defaultFeatures)
		/ परिमाण(defaultFeatures[0]);

	    IDWriteTypography *typography = रिक्त;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    यदि (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		प्रति (पूर्णाङ्क d = 0; d < अंक_Defaults; ++d)
		{
		    पूर्णाङ्क overridden = 0;
		    प्रति (पूर्णाङ्क u = 0; u < m_Font_Feature_गिनती; ++u)
		    {
			यदि ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    भङ्ग;
			}
		    }
		    यदि (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		प्रति (पूर्णाङ्क i = 0; i < m_Font_Feature_गिनती; ++i)
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

    यदि ((fuOptions & ETO_CLIPPED) && lprc != रिक्त)
	mRT->निकालो_Axis_Aligned_Clip();
}

    शून्य
DWriteContext::FillRect(ध्रुव RECT *rc, COLORREF color)
{
    यदि (mDMode == DM_INTEROP)
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
		D2D1::RectF(FLOAT(rc->बायाँ), FLOAT(rc->शीर्ष),
		    FLOAT(rc->दायाँ), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::Draw_पंक्ति(पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2, COLORREF color)
{
    यदि (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, रिक्त);
	::पंक्ति_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    अन्यथा
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_पंक्ति(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    शून्य
DWriteContext::SetPixel(पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color)
{
    यदि (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    अन्यथा
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_पंक्ति(
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
    यदि (x >= 0)
    {
	srcRect.बायाँ = rc->बायाँ;
	srcRect.दायाँ = rc->दायाँ - x;
	destPoint.x = rc->बायाँ + x;
    }
    अन्यथा
    {
	srcRect.बायाँ = rc->बायाँ - x;
	srcRect.दायाँ = rc->दायाँ;
	destPoint.x = rc->बायाँ;
    }
    यदि (y >= 0)
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
	    FLOAT(destPoint.x + srcRect.दायाँ - srcRect.बायाँ),
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
	ध्रुव DWriteRenderingParams *params)
{
    यदि (mDWriteFactory == रिक्त)
	प्रत्यागम;

    IDWriteRenderingParams *renderingParams = रिक्त;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    यदि (params != रिक्त)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    अन्यथा
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    यदि (SUCCEEDED(hr) && renderingParams != रिक्त)
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
    यदि (params != रिक्त && mRenderingParams != रिक्त)
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
    यदि (hD2D1DLL == रिक्त || hDWriteDLL == रिक्त)
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
    यदि (pGetUserDefaultLocaleName == रिक्त || pD2D1CreateFactory == रिक्त
	    || pDWriteCreateFactory == रिक्त)
	प्रत्यागम रिक्त;
#endif
    प्रत्यागम नूतन DWriteContext();
}

    शून्य
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ध्रुव RECT *rect)
{
    यदि (ctx != रिक्त)
	ctx->BindDC(hdc, rect);
}

    शून्य
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    यदि (ctx != रिक्त)
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
    यदि (ctx != रिक्त)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    शून्य
DWriteContext_FillRect(DWriteContext *ctx, ध्रुव RECT *rc, COLORREF color)
{
    यदि (ctx != रिक्त)
	ctx->FillRect(rc, color);
}

    शून्य
DWrite_Context_Draw_पंक्ति(DWriteContext *ctx, पूर्णाङ्क x1, पूर्णाङ्क y1, पूर्णाङ्क x2, पूर्णाङ्क y2,
	COLORREF color)
{
    यदि (ctx != रिक्त)
	ctx->Draw_पंक्ति(x1, y1, x2, y2, color);
}

    शून्य
DWriteContext_SetPixel(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, COLORREF color)
{
    यदि (ctx != रिक्त)
	ctx->SetPixel(x, y, color);
}

    शून्य
DWriteContext_Scroll(DWriteContext *ctx, पूर्णाङ्क x, पूर्णाङ्क y, ध्रुव RECT *rc)
{
    यदि (ctx != रिक्त)
	ctx->Scroll(x, y, rc);
}

    शून्य
DWriteContext_Flush(DWriteContext *ctx)
{
    यदि (ctx != रिक्त)
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
    यदि (ctx != रिक्त)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    यदि (ctx != रिक्त)
	प्रत्यागम ctx->GetRenderingParams(params);
    अन्यथा
	प्रत्यागम रिक्त;
}

    शून्य
DWriteContext::SetFontFeatures(
	ध्रुव DWriteFontFeature *features, पूर्णाङ्क गिनती)
{
    यदि (गिनती > DWRITE_MAX_FONT_FEATURES)
	गिनती = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_गिनती = गिनती;
    यदि (गिनती > 0 && features != रिक्त)
	स_नकल(mFontFeatures, features, परिमाण(DWriteFontFeature) * गिनती);
}

    शून्य
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ध्रुव DWriteFontFeature *features,
	पूर्णाङ्क गिनती)
{
    यदि (ctx != रिक्त)
	ctx->SetFontFeatures(features, गिनती);
}
