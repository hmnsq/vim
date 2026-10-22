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
# જો WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#અન્તર્ભાવ <windows.h>
#અન્તર્ભાવ <crtdbg.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <d2d1.h>
#અન્તર્ભાવ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#જો defined(_MSC_VER) && (_MSC_VER < 1700)
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
#નહીંતર
# include <dwrite.h>
#endif

#અન્તર્ભાવ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#નહીંતર
# define UNUSED
#endif

#જો (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#નહીંતર
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
બાહ્ય "C" HINSTANCE vimLoadLib(અચલ અક્ષર *name);

પ્રકારવ્યાખ્યા પૂર્ણાંક (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, પૂર્ણાંક);
પ્રકારવ્યાખ્યા HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, અચલ D2D1_FACTORY_OPTIONS *, શૂન્ય **);
પ્રકારવ્યાખ્યા HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

સ્થિર HINSTANCE hD2D1DLL = ખાલી;
સ્થિર HINSTANCE hDWriteDLL = ખાલી;

સ્થિર PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ખાલી;
સ્થિર PD2D1CREATEFACTORY pD2D1CreateFactory = ખાલી;
સ્થિર PDWRITECREATEFACTORY pDWriteCreateFactory = ખાલી;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    સ્થિર શૂન્ય
unload(HINSTANCE &hinst)
{
    જો (hinst != ખાલી)
    {
	FreeLibrary(hinst);
	hinst = ખાલી;
    }
}
#endif // DYNAMIC_DIRECTX

template <વર્ગ T> અંતઃરેખ શૂન્ય SafeRelease(T **ppT)
{
    જો (*ppT)
    {
	(*ppT)->Release();
	*ppT = ખાલી;
    }
}

    સ્થિર DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(પૂર્ણાંક કિંમત)
{
    પસંદગી (કિંમત)
    {
	મૂળભૂત:
	સ્થિતિ 0:
	    પાછા DWRITE_PIXEL_GEOMETRY_FLAT;
	સ્થિતિ 1:
	    પાછા DWRITE_PIXEL_GEOMETRY_RGB;
	સ્થિતિ 2:
	    પાછા DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    સ્થિર પૂર્ણાંક
ToInt(DWRITE_PIXEL_GEOMETRY કિંમત)
{
    પસંદગી (કિંમત)
    {
	સ્થિતિ DWRITE_PIXEL_GEOMETRY_FLAT:
	    પાછા 0;
	સ્થિતિ DWRITE_PIXEL_GEOMETRY_RGB:
	    પાછા 1;
	સ્થિતિ DWRITE_PIXEL_GEOMETRY_BGR:
	    પાછા 2;
	મૂળભૂત:
	    પાછા -1;
    }
}

    સ્થિર DWRITE_RENDERING_MODE
ToRenderingMode(પૂર્ણાંક કિંમત)
{
    પસંદગી (કિંમત)
    {
	મૂળભૂત:
	સ્થિતિ 0:
	    પાછા DWRITE_RENDERING_MODE_DEFAULT;
	સ્થિતિ 1:
	    પાછા DWRITE_RENDERING_MODE_ALIASED;
	સ્થિતિ 2:
	    પાછા DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	સ્થિતિ 3:
	    પાછા DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	સ્થિતિ 4:
	    પાછા DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	સ્થિતિ 5:
	    પાછા DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	સ્થિતિ 6:
	    પાછા DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    સ્થિર D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(પૂર્ણાંક કિંમત)
{
    પસંદગી (કિંમત)
    {
	મૂળભૂત:
	સ્થિતિ 0:
	    પાછા D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	સ્થિતિ 1:
	    પાછા D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	સ્થિતિ 2:
	    પાછા D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	સ્થિતિ 3:
	    પાછા D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    સ્થિર પૂર્ણાંક
ToInt(DWRITE_RENDERING_MODE કિંમત)
{
    પસંદગી (કિંમત)
    {
	સ્થિતિ DWRITE_RENDERING_MODE_DEFAULT:
	    પાછા 0;
	સ્થિતિ DWRITE_RENDERING_MODE_ALIASED:
	    પાછા 1;
	સ્થિતિ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    પાછા 2;
	સ્થિતિ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    પાછા 3;
	સ્થિતિ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    પાછા 4;
	સ્થિતિ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    પાછા 5;
	સ્થિતિ DWRITE_RENDERING_MODE_OUTLINE:
	    પાછા 6;
	મૂળભૂત:
	    પાછા -1;
    }
}

વર્ગ FontCache {
જાહેર:
    સંરચના Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(ખાલી), pTextFormat(ખાલી), fontAscent(0.0f) {}
    };

ખાનગી:
    પૂર્ણાંક mSize;
    Item *m_વસ્તુઓ;

જાહેર:
    FontCache(પૂર્ણાંક size = 2) :
	mSize(size),
	m_વસ્તુઓ(નવું Item[size])
    {
    }

    ~FontCache()
    {
	માટે (પૂર્ણાંક i = 0; i < mSize; ++i)
	    SafeRelease(&m_વસ્તુઓ[i].pTextFormat);
	delete[] m_વસ્તુઓ;
    }

    તાર્કિક get(HFONT hFont, Item &item)
    {
	પૂર્ણાંક n = find(hFont);
	જો (n < 0)
	    પાછા ખોટું;
	item = m_વસ્તુઓ[n];
	slide(n);
	પાછા સાચું;
    }

    શૂન્ય put(અચલ Item& item)
    {
	પૂર્ણાંક n = find(item.hFont);
	જો (n < 0)
	    n = mSize - 1;
	જો (m_વસ્તુઓ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_વસ્તુઓ[n].pTextFormat);
	    જો (item.pTextFormat != ખાલી)
		item.pTextFormat->AddRef();
	}
	m_વસ્તુઓ[n] = item;
	slide(n);
    }

ખાનગી:
    પૂર્ણાંક find(HFONT hFont)
    {
	માટે (પૂર્ણાંક i = 0; i < mSize; ++i)
	{
	    જો (m_વસ્તુઓ[i].hFont == hFont)
		પાછા i;
	}
	પાછા -1;
    }

    શૂન્ય slide(પૂર્ણાંક next_ટોચ)
    {
	જો (next_ટોચ == 0)
	    પાછા;
	Item tmp = m_વસ્તુઓ[next_ટોચ];
	માટે (પૂર્ણાંક i = next_ટોચ - 1; i >= 0; --i)
	    m_વસ્તુઓ[i + 1] = m_વસ્તુઓ[i];
	m_વસ્તુઓ[0] = tmp;
    }
};

ગણના DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

સંરચના DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    તાર્કિક mDrawing;
    તાર્કિક mFallbackDC;

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
    પૂર્ણાંક m_Font_Feature_ગણતરી;

    // METHODS

    DWriteContext();

    આભાસી ~DWriteContext();

    HRESULT CreateDeviceResources();

    શૂન્ય DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(અચલ LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(અચલ LOGFONTW &logFont);

    શૂન્ય SetFont(HFONT hFont);

    શૂન્ય Rebind();

    શૂન્ય BindDC(HDC hdc, અચલ RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    શૂન્ય DrawText(અચલ WCHAR *text, પૂર્ણાંક len,
	પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h, પૂર્ણાંક cellWidth, COLORREF color,
	UINT fuOptions, અચલ RECT *lprc, અચલ INT *lpDx);

    શૂન્ય FillRect(અચલ RECT *rc, COLORREF color);

    શૂન્ય Draw_લીટી(પૂર્ણાંક x1, પૂર્ણાંક y1, પૂર્ણાંક x2, પૂર્ણાંક y2, COLORREF color);

    શૂન્ય SetPixel(પૂર્ણાંક x, પૂર્ણાંક y, COLORREF color);

    શૂન્ય Scroll(પૂર્ણાંક x, પૂર્ણાંક y, અચલ RECT *rc);

    શૂન્ય Flush();

    શૂન્ય SetRenderingParams(
	    અચલ DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    શૂન્ય SetFontFeatures(અચલ DWriteFontFeature *features, પૂર્ણાંક ગણતરી);
};

વર્ગ AdjustedGlyphRun : જાહેર DWRITE_GLYPH_RUN
{
ખાનગી:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

જાહેર:
    AdjustedGlyphRun(
	    અચલ DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    અચલ INT *lpDx,
	    પૂર્ણાંક *glyphIndex,
	    પૂર્ણાંક cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(નવું FLOAT[glyphRun->glyph_ગણતરી])
    {
	પ્રતિજ્ઞા(cellWidth != 0.0f);
	માટે (UINT32 i = 0; i < glyphRun->glyph_ગણતરી; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    જો (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    નહીંતર
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

    સ્થિર FLOAT adjustToCell(FLOAT કિંમત, FLOAT cellWidth)
    {
	પૂર્ણાંક cell_ગણતરી = પૂર્ણાંક(floor(કિંમત / cellWidth + 0.5f));
	જો (cell_ગણતરી < 1)
	    cell_ગણતરી = 1;
	પાછા cell_ગણતરી * cellWidth;
    }
};

સંરચના TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    અચલ INT *lpDx;
    પૂર્ણાંક cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    પૂર્ણાંક glyphIndex;
};

વર્ગ TextRenderer FINAL : જાહેર IDWriteTextRenderer
{
જાહેર:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_ગણતરી(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    આભાસી ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	પાછા S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	પાછા S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	દશાંશ dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	પાછા S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE અચલ* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	પાછા E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH અચલ* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	પાછા E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull શૂન્ય* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_જમણું_To_ડાબું UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	પાછા E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull શૂન્ય* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN અચલ* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION અચલ* glyphRunDescription UNUSED,
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
	જો (pDWC_->mDWriteFactory2 != ખાલી)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = ખાલી;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		ખાલી,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		ખાલી,
		0,
		&enumerator);
	    જો (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		જ્યારે (hasRun)
		{
		    અચલ DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		પાછા S_OK;
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
	પાછા S_OK;
    }

જાહેર:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	પાછા InterlockedIncrement(&c_Ref_ગણતરી);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_ગણતરી_2 = InterlockedDecrement(&c_Ref_ગણતરી);

	જો (new_ગણતરી_2 == 0)
	{
	    delete આ;
	    પાછા 0;
	}
	પાછા new_ગણતરી_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID અચલ& riid,
	શૂન્ય** ppvObject)
    {
	જો (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = આ;
	}
	નહીંતર જો (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = આ;
	}
	નહીંતર જો (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = આ;
	}
	નહીંતર
	{
	    *ppvObject = ખાલી;
	    પાછા E_FAIL;
	}

	પાછા S_OK;
    }

ખાનગી:
    long c_Ref_ગણતરી;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ખાલી),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ખાલી),
    mDrawing(ખોટું),
    mFallbackDC(ખોટું),
    mD2D1Factory(ખાલી),
    mRT(ખાલી),
    mGDIRT(ખાલી),
    mBrush(ખાલી),
    mBitmap(ખાલી),
    mDWriteFactory(ખાલી),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ખાલી),
#endif
    mGdiInterop(ખાલી),
    mRenderingParams(ખાલી),
    mFontCache(8),
    mTextFormat(ખાલી),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_ગણતરી(0)
{
    ZeroMemory(mFontFeatures, કદ(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ખાલી,
	    reinterpret_cast<શૂન્ય**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    જો (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    જો (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    જો (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    જો (SUCCEEDED(hr))
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

    જો (mRT != ખાલી)
	પાછા S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    જો (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<શૂન્ય**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    જો (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    જો (SUCCEEDED(hr))
	Rebind();

    પાછા hr;
}

    શૂન્ય
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(અચલ LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ખાલી;

    IDWriteFont *font = ખાલી;
    IDWriteFontFamily *fontFamily = ખાલી;
    IDWriteLocalizedStrings *localizedFamilyNames = ખાલી;
    દશાંશ fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    જો (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    જો (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    જો (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    જો (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    જો (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = દશાંશ(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	જો (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	નહીંતર જો (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * દશાંશ(fontMetrics.designUnitsPerEm)
		/ દશાંશ(fontMetrics.ascent + fontMetrics.descent + fontMetrics.લીટી_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    જો (SUCCEEDED(hr))
    {
	જો (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    જો (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		ખાલી, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    જો (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    જો (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    જો (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    જો (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	જો (pFontAscent != ખાલી && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = દશાંશ(fontMetrics.ascent + fontMetrics.descent);
	    જો (totalHeight != 0.0f)
		*pFontAscent = દશાંશ(fontMetrics.ascent) / totalHeight;
	    નહીંતર
		*pFontAscent = 0.83f; // fallback
	}
    }
    નહીંતર
	SafeRelease(&pTextFormat);

    પાછા hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(અચલ LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ખાલી;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    જો (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    પાછા hr;
}

    શૂન્ય
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    જો (mFontCache.get(hFont, item))
    {
	જો (item.pTextFormat != ખાલી)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = ખોટું;
	}
	નહીંતર
	    mFallbackDC = સાચું;
	પાછા;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    જો (GetObjectW(hFont, કદ(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    જો (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = ખોટું;
    }
    નહીંતર
	mFallbackDC = સાચું;
    mFontCache.put(item);
}

    શૂન્ય
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
	    D2D1::SizeU(mBindRect.જમણું - mBindRect.ડાબું,
		mBindRect.bottom - mBindRect.ટોચ),
	    props, &mBitmap);
}

    શૂન્ય
DWriteContext::BindDC(HDC hdc, અચલ RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    જો (mRT == ખાલી)
	CreateDeviceResources();
    નહીંતર
    {
	Flush();
	Rebind();
    }
}

બાહ્ય "C" શૂન્ય redraw_later_clear(શૂન્ય);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    પસંદગી (mode)
    {
	મૂળભૂત:
	સ્થિતિ DM_GDI:
	    જો (mInteropHDC != ખાલી)
	    {
		mGDIRT->ReleaseDC(ખાલી);
		mInteropHDC = ખાલી;
	    }
	    જો (mDrawing)
	    {
		hr = mRT->EndDraw();
		જો (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = ખોટું;
	    }
	    તોડો;

	સ્થિતિ DM_DIRECTX:
	    જો (mInteropHDC != ખાલી)
	    {
		mGDIRT->ReleaseDC(ખાલી);
		mInteropHDC = ખાલી;
	    }
	    નહીંતર જો (mDrawing == ખોટું)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = સાચું;
	    }
	    તોડો;

	સ્થિતિ DM_INTEROP:
	    જો (mDrawing == ખોટું)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = સાચું;
	    }
	    જો (mInteropHDC == ખાલી)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    તોડો;
    }
    mDMode = mode;
    પાછા hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    પાછા mBrush;
}

    શૂન્ય
DWriteContext::DrawText(અચલ WCHAR *text, પૂર્ણાંક len,
	પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h, પૂર્ણાંક cellWidth, COLORREF color,
	UINT fuOptions, અચલ RECT *lprc, અચલ INT *lpDx)
{
    જો (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	જો (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	પાછા;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = ખાલી;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    જો ((fuOptions & ETO_CLIPPED) && lprc != ખાલી)
    {
	mRT->ધકેલો_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ડાબું), FLOAT(lprc->ટોચ),
			FLOAT(lprc->જમણું), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    જો (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	જો (m_Font_Feature_ગણતરી > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    સ્થિર અચલ DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    સ્થિર અચલ પૂર્ણાંક અંક_Defaults = કદ(defaultFeatures)
		/ કદ(defaultFeatures[0]);

	    IDWriteTypography *typography = ખાલી;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    જો (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		માટે (પૂર્ણાંક d = 0; d < અંક_Defaults; ++d)
		{
		    પૂર્ણાંક overridden = 0;
		    માટે (પૂર્ણાંક u = 0; u < m_Font_Feature_ગણતરી; ++u)
		    {
			જો ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    તોડો;
			}
		    }
		    જો (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		માટે (પૂર્ણાંક i = 0; i < m_Font_Feature_ગણતરી; ++i)
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

	TextRenderer renderer(આ);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    જો ((fuOptions & ETO_CLIPPED) && lprc != ખાલી)
	mRT->કાઢો_Axis_Aligned_Clip();
}

    શૂન્ય
DWriteContext::FillRect(અચલ RECT *rc, COLORREF color)
{
    જો (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    નહીંતર
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ડાબું), FLOAT(rc->ટોચ),
		    FLOAT(rc->જમણું), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    શૂન્ય
DWriteContext::Draw_લીટી(પૂર્ણાંક x1, પૂર્ણાંક y1, પૂર્ણાંક x2, પૂર્ણાંક y2, COLORREF color)
{
    જો (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, ખાલી);
	::લીટી_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    નહીંતર
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_લીટી(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    શૂન્ય
DWriteContext::SetPixel(પૂર્ણાંક x, પૂર્ણાંક y, COLORREF color)
{
    જો (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    નહીંતર
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_લીટી(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    શૂન્ય
DWriteContext::Scroll(પૂર્ણાંક x, પૂર્ણાંક y, અચલ RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    જો (x >= 0)
    {
	srcRect.ડાબું = rc->ડાબું;
	srcRect.જમણું = rc->જમણું - x;
	destPoint.x = rc->ડાબું + x;
    }
    નહીંતર
    {
	srcRect.ડાબું = rc->ડાબું - x;
	srcRect.જમણું = rc->જમણું;
	destPoint.x = rc->ડાબું;
    }
    જો (y >= 0)
    {
	srcRect.ટોચ = rc->ટોચ;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->ટોચ + y;
    }
    નહીંતર
    {
	srcRect.ટોચ = rc->ટોચ - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->ટોચ;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.જમણું - srcRect.ડાબું),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.ટોચ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    શૂન્ય
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    શૂન્ય
DWriteContext::SetRenderingParams(
	અચલ DWriteRenderingParams *params)
{
    જો (mDWriteFactory == ખાલી)
	પાછા;

    IDWriteRenderingParams *renderingParams = ખાલી;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    જો (params != ખાલી)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    નહીંતર
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    જો (SUCCEEDED(hr) && renderingParams != ખાલી)
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
    જો (params != ખાલી && mRenderingParams != ખાલી)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    પાછા params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    શૂન્ય
DWrite_Init(શૂન્ય)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    જો (hD2D1DLL == ખાલી || hDWriteDLL == ખાલી)
    {
	DWrite_Final();
	પાછા;
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

    શૂન્ય
DWrite_Final(શૂન્ય)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ખાલી;
    pD2D1CreateFactory = ખાલી;
    pDWriteCreateFactory = ખાલી;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(શૂન્ય)
{
#ifdef DYNAMIC_DIRECTX
    જો (pGetUserDefaultLocaleName == ખાલી || pD2D1CreateFactory == ખાલી
	    || pDWriteCreateFactory == ખાલી)
	પાછા ખાલી;
#endif
    પાછા નવું DWriteContext();
}

    શૂન્ય
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, અચલ RECT *rect)
{
    જો (ctx != ખાલી)
	ctx->BindDC(hdc, rect);
}

    શૂન્ય
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    જો (ctx != ખાલી)
	ctx->SetFont(hFont);
}

    શૂન્ય
DWriteContext_DrawText(
	DWriteContext *ctx,
	અચલ WCHAR *text,
	પૂર્ણાંક len,
	પૂર્ણાંક x,
	પૂર્ણાંક y,
	પૂર્ણાંક w,
	પૂર્ણાંક h,
	પૂર્ણાંક cellWidth,
	COLORREF color,
	UINT fuOptions,
	અચલ RECT *lprc,
	અચલ INT *lpDx)
{
    જો (ctx != ખાલી)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    શૂન્ય
DWriteContext_FillRect(DWriteContext *ctx, અચલ RECT *rc, COLORREF color)
{
    જો (ctx != ખાલી)
	ctx->FillRect(rc, color);
}

    શૂન્ય
DWrite_Context_Draw_લીટી(DWriteContext *ctx, પૂર્ણાંક x1, પૂર્ણાંક y1, પૂર્ણાંક x2, પૂર્ણાંક y2,
	COLORREF color)
{
    જો (ctx != ખાલી)
	ctx->Draw_લીટી(x1, y1, x2, y2, color);
}

    શૂન્ય
DWriteContext_SetPixel(DWriteContext *ctx, પૂર્ણાંક x, પૂર્ણાંક y, COLORREF color)
{
    જો (ctx != ખાલી)
	ctx->SetPixel(x, y, color);
}

    શૂન્ય
DWriteContext_Scroll(DWriteContext *ctx, પૂર્ણાંક x, પૂર્ણાંક y, અચલ RECT *rc)
{
    જો (ctx != ખાલી)
	ctx->Scroll(x, y, rc);
}

    શૂન્ય
DWriteContext_Flush(DWriteContext *ctx)
{
    જો (ctx != ખાલી)
	ctx->Flush();
}

    શૂન્ય
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    શૂન્ય
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	અચલ DWriteRenderingParams *params)
{
    જો (ctx != ખાલી)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    જો (ctx != ખાલી)
	પાછા ctx->GetRenderingParams(params);
    નહીંતર
	પાછા ખાલી;
}

    શૂન્ય
DWriteContext::SetFontFeatures(
	અચલ DWriteFontFeature *features, પૂર્ણાંક ગણતરી)
{
    જો (ગણતરી > DWRITE_MAX_FONT_FEATURES)
	ગણતરી = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ગણતરી = ગણતરી;
    જો (ગણતરી > 0 && features != ખાલી)
	memcpy(mFontFeatures, features, કદ(DWriteFontFeature) * ગણતરી);
}

    શૂન્ય
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	અચલ DWriteFontFeature *features,
	પૂર્ણાંક ગણતરી)
{
    જો (ctx != ખાલી)
	ctx->SetFontFeatures(features, ગણતરી);
}
