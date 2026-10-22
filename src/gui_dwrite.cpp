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
# ଯଦି WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#ଅନ୍ତର୍ଭାଵ <windows.h>
#ଅନ୍ତର୍ଭାଵ <crtdbg.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <d2d1.h>
#ଅନ୍ତର୍ଭାଵ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#ଯଦି defined(_MSC_VER) && (_MSC_VER < 1700)
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
#ଅନ୍ୟଥା
# include <dwrite.h>
#endif

#ଅନ୍ତର୍ଭାଵ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ଅନ୍ୟଥା
# define UNUSED
#endif

#ଯଦି (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ଅନ୍ୟଥା
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
ବାହ୍ୟ "C" HINSTANCE vimLoadLib(ଅଚଳ ଅକ୍ଷର *name);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ଅଚଳ D2D1_FACTORY_OPTIONS *, ଶୂନ୍ୟ **);
ପ୍ରକାରବ୍ୟାଖ୍ୟା HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ସ୍ଥିର HINSTANCE hD2D1DLL = ଖାଲି;
ସ୍ଥିର HINSTANCE hDWriteDLL = ଖାଲି;

ସ୍ଥିର PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ଖାଲି;
ସ୍ଥିର PD2D1CREATEFACTORY pD2D1CreateFactory = ଖାଲି;
ସ୍ଥିର PDWRITECREATEFACTORY pDWriteCreateFactory = ଖାଲି;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    ସ୍ଥିର ଶୂନ୍ୟ
unload(HINSTANCE &hinst)
{
    ଯଦି (hinst != ଖାଲି)
    {
	FreeLibrary(hinst);
	hinst = ଖାଲି;
    }
}
#endif // DYNAMIC_DIRECTX

template <ଶ୍ରେଣୀ T> ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ SafeRelease(T **ppT)
{
    ଯଦି (*ppT)
    {
	(*ppT)->Release();
	*ppT = ଖାଲି;
    }
}

    ସ୍ଥିର DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
    ବାଛ (ମୂଲ୍ୟ)
    {
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ସ୍ଥିତି 0:
	    ଫେରନ୍ତୁ DWRITE_PIXEL_GEOMETRY_FLAT;
	ସ୍ଥିତି 1:
	    ଫେରନ୍ତୁ DWRITE_PIXEL_GEOMETRY_RGB;
	ସ୍ଥିତି 2:
	    ଫେରନ୍ତୁ DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ToInt(DWRITE_PIXEL_GEOMETRY ମୂଲ୍ୟ)
{
    ବାଛ (ମୂଲ୍ୟ)
    {
	ସ୍ଥିତି DWRITE_PIXEL_GEOMETRY_FLAT:
	    ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି DWRITE_PIXEL_GEOMETRY_RGB:
	    ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି DWRITE_PIXEL_GEOMETRY_BGR:
	    ଫେରନ୍ତୁ 2;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ -1;
    }
}

    ସ୍ଥିର DWRITE_RENDERING_MODE
ToRenderingMode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
    ବାଛ (ମୂଲ୍ୟ)
    {
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ସ୍ଥିତି 0:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_DEFAULT;
	ସ୍ଥିତି 1:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_ALIASED;
	ସ୍ଥିତି 2:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	ସ୍ଥିତି 3:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	ସ୍ଥିତି 4:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	ସ୍ଥିତି 5:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	ସ୍ଥିତି 6:
	    ଫେରନ୍ତୁ DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    ସ୍ଥିର D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
    ବାଛ (ମୂଲ୍ୟ)
    {
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ସ୍ଥିତି 0:
	    ଫେରନ୍ତୁ D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	ସ୍ଥିତି 1:
	    ଫେରନ୍ତୁ D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	ସ୍ଥିତି 2:
	    ଫେରନ୍ତୁ D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	ସ୍ଥିତି 3:
	    ଫେରନ୍ତୁ D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ToInt(DWRITE_RENDERING_MODE ମୂଲ୍ୟ)
{
    ବାଛ (ମୂଲ୍ୟ)
    {
	ସ୍ଥିତି DWRITE_RENDERING_MODE_DEFAULT:
	    ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_ALIASED:
	    ଫେରନ୍ତୁ 1;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    ଫେରନ୍ତୁ 2;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    ଫେରନ୍ତୁ 3;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    ଫେରନ୍ତୁ 4;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    ଫେରନ୍ତୁ 5;
	ସ୍ଥିତି DWRITE_RENDERING_MODE_OUTLINE:
	    ଫେରନ୍ତୁ 6;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ -1;
    }
}

ଶ୍ରେଣୀ FontCache {
ସର୍ବସାଧାରଣ:
    ସଂରଚନା Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(ଖାଲି), pTextFormat(ଖାଲି), fontAscent(0.0f) {}
    };

ବ୍ୟକ୍ତିଗତ:
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mSize;
    Item *m_ବସ୍ତୁଗୁଡ଼ିକ;

ସର୍ବସାଧାରଣ:
    FontCache(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = 2) :
	mSize(size),
	m_ବସ୍ତୁଗୁଡ଼ିକ(ନୂତନ Item[size])
    {
    }

    ~FontCache()
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mSize; ++i)
	    SafeRelease(&m_ବସ୍ତୁଗୁଡ଼ିକ[i].pTextFormat);
	delete[] m_ବସ୍ତୁଗୁଡ଼ିକ;
    }

    ତାର୍କିକ get(HFONT hFont, Item &item)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = find(hFont);
	ଯଦି (n < 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
	item = m_ବସ୍ତୁଗୁଡ଼ିକ[n];
	slide(n);
	ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    ଶୂନ୍ୟ put(ଅଚଳ Item& item)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = find(item.hFont);
	ଯଦି (n < 0)
	    n = mSize - 1;
	ଯଦି (m_ବସ୍ତୁଗୁଡ଼ିକ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_ବସ୍ତୁଗୁଡ଼ିକ[n].pTextFormat);
	    ଯଦି (item.pTextFormat != ଖାଲି)
		item.pTextFormat->AddRef();
	}
	m_ବସ୍ତୁଗୁଡ଼ିକ[n] = item;
	slide(n);
    }

ବ୍ୟକ୍ତିଗତ:
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା find(HFONT hFont)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < mSize; ++i)
	{
	    ଯଦି (m_ବସ୍ତୁଗୁଡ଼ିକ[i].hFont == hFont)
		ଫେରନ୍ତୁ i;
	}
	ଫେରନ୍ତୁ -1;
    }

    ଶୂନ୍ୟ slide(ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_ଶୀର୍ଷ)
    {
	ଯଦି (next_ଶୀର୍ଷ == 0)
	    ଫେରନ୍ତୁ;
	Item tmp = m_ବସ୍ତୁଗୁଡ଼ିକ[next_ଶୀର୍ଷ];
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = next_ଶୀର୍ଷ - 1; i >= 0; --i)
	    m_ବସ୍ତୁଗୁଡ଼ିକ[i + 1] = m_ବସ୍ତୁଗୁଡ଼ିକ[i];
	m_ବସ୍ତୁଗୁଡ଼ିକ[0] = tmp;
    }
};

ଗଣନା DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ସଂରଚନା DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    ତାର୍କିକ mDrawing;
    ତାର୍କିକ mFallbackDC;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m_Font_Feature_ଗଣନା;

    // METHODS

    DWriteContext();

    ଆଭାସୀ ~DWriteContext();

    HRESULT CreateDeviceResources();

    ଶୂନ୍ୟ DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(ଅଚଳ LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(ଅଚଳ LOGFONTW &logFont);

    ଶୂନ୍ୟ SetFont(HFONT hFont);

    ଶୂନ୍ୟ Rebind();

    ଶୂନ୍ୟ BindDC(HDC hdc, ଅଚଳ RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    ଶୂନ୍ୟ DrawText(ଅଚଳ WCHAR *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cellWidth, COLORREF color,
	UINT fuOptions, ଅଚଳ RECT *lprc, ଅଚଳ INT *lpDx);

    ଶୂନ୍ୟ FillRect(ଅଚଳ RECT *rc, COLORREF color);

    ଶୂନ୍ୟ Draw_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y2, COLORREF color);

    ଶୂନ୍ୟ SetPixel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, COLORREF color);

    ଶୂନ୍ୟ Scroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ଅଚଳ RECT *rc);

    ଶୂନ୍ୟ Flush();

    ଶୂନ୍ୟ SetRenderingParams(
	    ଅଚଳ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    ଶୂନ୍ୟ SetFontFeatures(ଅଚଳ DWriteFontFeature *features, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2);
};

ଶ୍ରେଣୀ AdjustedGlyphRun : ସର୍ବସାଧାରଣ DWRITE_GLYPH_RUN
{
ବ୍ୟକ୍ତିଗତ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

ସର୍ବସାଧାରଣ:
    AdjustedGlyphRun(
	    ଅଚଳ DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    ଅଚଳ INT *lpDx,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *glyphIndex,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(ନୂତନ FLOAT[glyphRun->glyph_ଗଣନା])
    {
	ପ୍ରତିଜ୍ଞା(cellWidth != 0.0f);
	ପାଇଁ (UINT32 i = 0; i < glyphRun->glyph_ଗଣନା; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    ଯଦି (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    ଅନ୍ୟଥା
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

    ସ୍ଥିର FLOAT adjustToCell(FLOAT ମୂଲ୍ୟ, FLOAT cellWidth)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cell_ଗଣନା = ପୂର୍ଣ୍ଣସଂଖ୍ୟା(floor(ମୂଲ୍ୟ / cellWidth + 0.5f));
	ଯଦି (cell_ଗଣନା < 1)
	    cell_ଗଣନା = 1;
	ଫେରନ୍ତୁ cell_ଗଣନା * cellWidth;
    }
};

ସଂରଚନା TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    ଅଚଳ INT *lpDx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା glyphIndex;
};

ଶ୍ରେଣୀ TextRenderer FINAL : ସର୍ବସାଧାରଣ IDWriteTextRenderer
{
ସର୍ବସାଧାରଣ:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_ଗଣନା(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    ଆଭାସୀ ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	ଫେରନ୍ତୁ S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	ଫେରନ୍ତୁ S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	ଦଶମିକ dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	ଫେରନ୍ତୁ S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE ଅଚଳ* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ଫେରନ୍ତୁ E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH ଅଚଳ* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ଫେରନ୍ତୁ E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_ଡାହାଣ_To_ବାମ UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	ଫେରନ୍ତୁ E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull ଶୂନ୍ୟ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN ଅଚଳ* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION ଅଚଳ* glyphRunDescription UNUSED,
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
	ଯଦି (pDWC_->mDWriteFactory2 != ଖାଲି)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = ଖାଲି;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		ଖାଲି,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		ଖାଲି,
		0,
		&enumerator);
	    ଯଦି (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		ଯେତେବେଳେ (hasRun)
		{
		    ଅଚଳ DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		ଫେରନ୍ତୁ S_OK;
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
	ଫେରନ୍ତୁ S_OK;
    }

ସର୍ବସାଧାରଣ:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	ଫେରନ୍ତୁ InterlockedIncrement(&c_Ref_ଗଣନା);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_ଗଣନା_2 = InterlockedDecrement(&c_Ref_ଗଣନା);

	ଯଦି (new_ଗଣନା_2 == 0)
	{
	    delete ଏହି;
	    ଫେରନ୍ତୁ 0;
	}
	ଫେରନ୍ତୁ new_ଗଣନା_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ଅଚଳ& riid,
	ଶୂନ୍ୟ** ppvObject)
    {
	ଯଦି (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = ଏହି;
	}
	ଅନ୍ୟଥା ଯଦି (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ଏହି;
	}
	ଅନ୍ୟଥା ଯଦି (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ଏହି;
	}
	ଅନ୍ୟଥା
	{
	    *ppvObject = ଖାଲି;
	    ଫେରନ୍ତୁ E_FAIL;
	}

	ଫେରନ୍ତୁ S_OK;
    }

ବ୍ୟକ୍ତିଗତ:
    long c_Ref_ଗଣନା;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ଖାଲି),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ଖାଲି),
    mDrawing(ମିଥ୍ୟା),
    mFallbackDC(ମିଥ୍ୟା),
    mD2D1Factory(ଖାଲି),
    mRT(ଖାଲି),
    mGDIRT(ଖାଲି),
    mBrush(ଖାଲି),
    mBitmap(ଖାଲି),
    mDWriteFactory(ଖାଲି),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ଖାଲି),
#endif
    mGdiInterop(ଖାଲି),
    mRenderingParams(ଖାଲି),
    mFontCache(8),
    mTextFormat(ଖାଲି),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_ଗଣନା(0)
{
    ZeroMemory(mFontFeatures, ଆକାର(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ଖାଲି,
	    reinterpret_cast<ଶୂନ୍ୟ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    ଯଦି (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    ଯଦି (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    ଯଦି (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    ଯଦି (SUCCEEDED(hr))
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

    ଯଦି (mRT != ଖାଲି)
	ଫେରନ୍ତୁ S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    ଯଦି (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<ଶୂନ୍ୟ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    ଯଦି (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    ଯଦି (SUCCEEDED(hr))
	Rebind();

    ଫେରନ୍ତୁ hr;
}

    ଶୂନ୍ୟ
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(ଅଚଳ LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ଖାଲି;

    IDWriteFont *font = ଖାଲି;
    IDWriteFontFamily *fontFamily = ଖାଲି;
    IDWriteLocalizedStrings *localizedFamilyNames = ଖାଲି;
    ଦଶମିକ fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    ଯଦି (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    ଯଦି (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    ଯଦି (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    ଯଦି (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    ଯଦି (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = ଦଶମିକ(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	ଯଦି (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	ଅନ୍ୟଥା ଯଦି (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * ଦଶମିକ(fontMetrics.designUnitsPerEm)
		/ ଦଶମିକ(fontMetrics.ascent + fontMetrics.descent + fontMetrics.ଧାଡ଼ି_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    ଯଦି (SUCCEEDED(hr))
    {
	ଯଦି (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    ଯଦି (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		ଖାଲି, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    ଯଦି (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    ଯଦି (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    ଯଦି (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    ଯଦି (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	ଯଦି (pFontAscent != ଖାଲି && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = ଦଶମିକ(fontMetrics.ascent + fontMetrics.descent);
	    ଯଦି (totalHeight != 0.0f)
		*pFontAscent = ଦଶମିକ(fontMetrics.ascent) / totalHeight;
	    ଅନ୍ୟଥା
		*pFontAscent = 0.83f; // fallback
	}
    }
    ଅନ୍ୟଥା
	SafeRelease(&pTextFormat);

    ଫେରନ୍ତୁ hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(ଅଚଳ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ଖାଲି;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    ଯଦି (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    ଫେରନ୍ତୁ hr;
}

    ଶୂନ୍ୟ
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    ଯଦି (mFontCache.get(hFont, item))
    {
	ଯଦି (item.pTextFormat != ଖାଲି)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = ମିଥ୍ୟା;
	}
	ଅନ୍ୟଥା
	    mFallbackDC = ସତ୍ୟ;
	ଫେରନ୍ତୁ;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    ଯଦି (GetObjectW(hFont, ଆକାର(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    ଯଦି (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = ମିଥ୍ୟା;
    }
    ଅନ୍ୟଥା
	mFallbackDC = ସତ୍ୟ;
    mFontCache.put(item);
}

    ଶୂନ୍ୟ
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
	    D2D1::SizeU(mBindRect.ଡାହାଣ - mBindRect.ବାମ,
		mBindRect.bottom - mBindRect.ଶୀର୍ଷ),
	    props, &mBitmap);
}

    ଶୂନ୍ୟ
DWriteContext::BindDC(HDC hdc, ଅଚଳ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    ଯଦି (mRT == ଖାଲି)
	CreateDeviceResources();
    ଅନ୍ୟଥା
    {
	Flush();
	Rebind();
    }
}

ବାହ୍ୟ "C" ଶୂନ୍ୟ redraw_later_clear(ଶୂନ୍ୟ);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    ବାଛ (mode)
    {
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ସ୍ଥିତି DM_GDI:
	    ଯଦି (mInteropHDC != ଖାଲି)
	    {
		mGDIRT->ReleaseDC(ଖାଲି);
		mInteropHDC = ଖାଲି;
	    }
	    ଯଦି (mDrawing)
	    {
		hr = mRT->EndDraw();
		ଯଦି (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = ମିଥ୍ୟା;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି DM_DIRECTX:
	    ଯଦି (mInteropHDC != ଖାଲି)
	    {
		mGDIRT->ReleaseDC(ଖାଲି);
		mInteropHDC = ଖାଲି;
	    }
	    ଅନ୍ୟଥା ଯଦି (mDrawing == ମିଥ୍ୟା)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ସତ୍ୟ;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି DM_INTEROP:
	    ଯଦି (mDrawing == ମିଥ୍ୟା)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ସତ୍ୟ;
	    }
	    ଯଦି (mInteropHDC == ଖାଲି)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    ଭାଙ୍ଗ;
    }
    mDMode = mode;
    ଫେରନ୍ତୁ hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    ଫେରନ୍ତୁ mBrush;
}

    ଶୂନ୍ୟ
DWriteContext::DrawText(ଅଚଳ WCHAR *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cellWidth, COLORREF color,
	UINT fuOptions, ଅଚଳ RECT *lprc, ଅଚଳ INT *lpDx)
{
    ଯଦି (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	ଯଦି (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	ଫେରନ୍ତୁ;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = ଖାଲି;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    ଯଦି ((fuOptions & ETO_CLIPPED) && lprc != ଖାଲି)
    {
	mRT->ଠେଲ_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ବାମ), FLOAT(lprc->ଶୀର୍ଷ),
			FLOAT(lprc->ଡାହାଣ), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    ଯଦି (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	ଯଦି (m_Font_Feature_ଗଣନା > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ସ୍ଥିର ଅଚଳ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Defaults = ଆକାର(defaultFeatures)
		/ ଆକାର(defaultFeatures[0]);

	    IDWriteTypography *typography = ଖାଲି;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    ଯଦି (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = 0; d < ଅଙ୍କ_Defaults; ++d)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା overridden = 0;
		    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା u = 0; u < m_Font_Feature_ଗଣନା; ++u)
		    {
			ଯଦି ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଯଦି (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < m_Font_Feature_ଗଣନା; ++i)
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

	TextRenderer renderer(ଏହି);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    ଯଦି ((fuOptions & ETO_CLIPPED) && lprc != ଖାଲି)
	mRT->କାଢ଼_Axis_Aligned_Clip();
}

    ଶୂନ୍ୟ
DWriteContext::FillRect(ଅଚଳ RECT *rc, COLORREF color)
{
    ଯଦି (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    ଅନ୍ୟଥା
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ବାମ), FLOAT(rc->ଶୀର୍ଷ),
		    FLOAT(rc->ଡାହାଣ), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    ଶୂନ୍ୟ
DWriteContext::Draw_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y2, COLORREF color)
{
    ଯଦି (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, ଖାଲି);
	::ଧାଡ଼ି_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    ଅନ୍ୟଥା
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_ଧାଡ଼ି(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    ଶୂନ୍ୟ
DWriteContext::SetPixel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, COLORREF color)
{
    ଯଦି (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    ଅନ୍ୟଥା
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_ଧାଡ଼ି(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    ଶୂନ୍ୟ
DWriteContext::Scroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ଅଚଳ RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    ଯଦି (x >= 0)
    {
	srcRect.ବାମ = rc->ବାମ;
	srcRect.ଡାହାଣ = rc->ଡାହାଣ - x;
	destPoint.x = rc->ବାମ + x;
    }
    ଅନ୍ୟଥା
    {
	srcRect.ବାମ = rc->ବାମ - x;
	srcRect.ଡାହାଣ = rc->ଡାହାଣ;
	destPoint.x = rc->ବାମ;
    }
    ଯଦି (y >= 0)
    {
	srcRect.ଶୀର୍ଷ = rc->ଶୀର୍ଷ;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->ଶୀର୍ଷ + y;
    }
    ଅନ୍ୟଥା
    {
	srcRect.ଶୀର୍ଷ = rc->ଶୀର୍ଷ - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->ଶୀର୍ଷ;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.ଡାହାଣ - srcRect.ବାମ),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.ଶୀର୍ଷ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    ଶୂନ୍ୟ
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    ଶୂନ୍ୟ
DWriteContext::SetRenderingParams(
	ଅଚଳ DWriteRenderingParams *params)
{
    ଯଦି (mDWriteFactory == ଖାଲି)
	ଫେରନ୍ତୁ;

    IDWriteRenderingParams *renderingParams = ଖାଲି;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    ଯଦି (params != ଖାଲି)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    ଅନ୍ୟଥା
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    ଯଦି (SUCCEEDED(hr) && renderingParams != ଖାଲି)
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
    ଯଦି (params != ଖାଲି && mRenderingParams != ଖାଲି)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    ଫେରନ୍ତୁ params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    ଶୂନ୍ୟ
DWrite_Init(ଶୂନ୍ୟ)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    ଯଦି (hD2D1DLL == ଖାଲି || hDWriteDLL == ଖାଲି)
    {
	DWrite_Final();
	ଫେରନ୍ତୁ;
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

    ଶୂନ୍ୟ
DWrite_Final(ଶୂନ୍ୟ)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ଖାଲି;
    pD2D1CreateFactory = ଖାଲି;
    pDWriteCreateFactory = ଖାଲି;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(ଶୂନ୍ୟ)
{
#ifdef DYNAMIC_DIRECTX
    ଯଦି (pGetUserDefaultLocaleName == ଖାଲି || pD2D1CreateFactory == ଖାଲି
	    || pDWriteCreateFactory == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    ଫେରନ୍ତୁ ନୂତନ DWriteContext();
}

    ଶୂନ୍ୟ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ଅଚଳ RECT *rect)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->BindDC(hdc, rect);
}

    ଶୂନ୍ୟ
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->SetFont(hFont);
}

    ଶୂନ୍ୟ
DWriteContext_DrawText(
	DWriteContext *ctx,
	ଅଚଳ WCHAR *text,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା x,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା w,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା h,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cellWidth,
	COLORREF color,
	UINT fuOptions,
	ଅଚଳ RECT *lprc,
	ଅଚଳ INT *lpDx)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    ଶୂନ୍ୟ
DWriteContext_FillRect(DWriteContext *ctx, ଅଚଳ RECT *rc, COLORREF color)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->FillRect(rc, color);
}

    ଶୂନ୍ୟ
DWrite_Context_Draw_ଧାଡ଼ି(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y2,
	COLORREF color)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->Draw_ଧାଡ଼ି(x1, y1, x2, y2, color);
}

    ଶୂନ୍ୟ
DWriteContext_SetPixel(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, COLORREF color)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->SetPixel(x, y, color);
}

    ଶୂନ୍ୟ
DWriteContext_Scroll(DWriteContext *ctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ଅଚଳ RECT *rc)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->Scroll(x, y, rc);
}

    ଶୂନ୍ୟ
DWriteContext_Flush(DWriteContext *ctx)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->Flush();
}

    ଶୂନ୍ୟ
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    ଶୂନ୍ୟ
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	ଅଚଳ DWriteRenderingParams *params)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    ଯଦି (ctx != ଖାଲି)
	ଫେରନ୍ତୁ ctx->GetRenderingParams(params);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

    ଶୂନ୍ୟ
DWriteContext::SetFontFeatures(
	ଅଚଳ DWriteFontFeature *features, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ଯଦି (ଗଣନା_2 > DWRITE_MAX_FONT_FEATURES)
	ଗଣନା_2 = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ଗଣନା = ଗଣନା_2;
    ଯଦି (ଗଣନା_2 > 0 && features != ଖାଲି)
	memcpy(mFontFeatures, features, ଆକାର(DWriteFontFeature) * ଗଣନା_2);
}

    ଶୂନ୍ୟ
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	ଅଚଳ DWriteFontFeature *features,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ଯଦି (ctx != ଖାଲି)
	ctx->SetFontFeatures(features, ଗଣନା_2);
}
