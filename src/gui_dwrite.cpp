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
# എങ്കിൽ WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#അന്തര്ഭാവ <windows.h>
#അന്തര്ഭാവ <crtdbg.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <d2d1.h>
#അന്തര്ഭാവ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#എങ്കിൽ defined(_MSC_VER) && (_MSC_VER < 1700)
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
#അല്ലെങ്കിൽ
# include <dwrite.h>
#endif

#അന്തര്ഭാവ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#അല്ലെങ്കിൽ
# define UNUSED
#endif

#എങ്കിൽ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#അല്ലെങ്കിൽ
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
ബാഹ്യ "C" HINSTANCE vimLoadLib(സ്ഥിരം അക്ഷരം *name);

തരനിർവചനം പൂർണ്ണസംഖ്യ (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, പൂർണ്ണസംഖ്യ);
തരനിർവചനം HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, സ്ഥിരം D2D1_FACTORY_OPTIONS *, ശൂന്യം **);
തരനിർവചനം HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

സ്ഥിതം HINSTANCE hD2D1DLL = ശൂന്യ;
സ്ഥിതം HINSTANCE hDWriteDLL = ശൂന്യ;

സ്ഥിതം PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ശൂന്യ;
സ്ഥിതം PD2D1CREATEFACTORY pD2D1CreateFactory = ശൂന്യ;
സ്ഥിതം PDWRITECREATEFACTORY pDWriteCreateFactory = ശൂന്യ;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    സ്ഥിതം ശൂന്യം
unload(HINSTANCE &hinst)
{
    എങ്കിൽ (hinst != ശൂന്യ)
    {
	FreeLibrary(hinst);
	hinst = ശൂന്യ;
    }
}
#endif // DYNAMIC_DIRECTX

template <വർഗ്ഗം T> വരിയിൽ ശൂന്യം SafeRelease(T **ppT)
{
    എങ്കിൽ (*ppT)
    {
	(*ppT)->Release();
	*ppT = ശൂന്യ;
    }
}

    സ്ഥിതം DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(പൂർണ്ണസംഖ്യ മൂല്യം)
{
    തിരഞ്ഞെടുപ്പ് (മൂല്യം)
    {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ 0:
	    തിരികെ DWRITE_PIXEL_GEOMETRY_FLAT;
	അവസ്ഥ 1:
	    തിരികെ DWRITE_PIXEL_GEOMETRY_RGB;
	അവസ്ഥ 2:
	    തിരികെ DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ToInt(DWRITE_PIXEL_GEOMETRY മൂല്യം)
{
    തിരഞ്ഞെടുപ്പ് (മൂല്യം)
    {
	അവസ്ഥ DWRITE_PIXEL_GEOMETRY_FLAT:
	    തിരികെ 0;
	അവസ്ഥ DWRITE_PIXEL_GEOMETRY_RGB:
	    തിരികെ 1;
	അവസ്ഥ DWRITE_PIXEL_GEOMETRY_BGR:
	    തിരികെ 2;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ -1;
    }
}

    സ്ഥിതം DWRITE_RENDERING_MODE
ToRenderingMode(പൂർണ്ണസംഖ്യ മൂല്യം)
{
    തിരഞ്ഞെടുപ്പ് (മൂല്യം)
    {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ 0:
	    തിരികെ DWRITE_RENDERING_MODE_DEFAULT;
	അവസ്ഥ 1:
	    തിരികെ DWRITE_RENDERING_MODE_ALIASED;
	അവസ്ഥ 2:
	    തിരികെ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	അവസ്ഥ 3:
	    തിരികെ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	അവസ്ഥ 4:
	    തിരികെ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	അവസ്ഥ 5:
	    തിരികെ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	അവസ്ഥ 6:
	    തിരികെ DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    സ്ഥിതം D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(പൂർണ്ണസംഖ്യ മൂല്യം)
{
    തിരഞ്ഞെടുപ്പ് (മൂല്യം)
    {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ 0:
	    തിരികെ D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	അവസ്ഥ 1:
	    തിരികെ D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	അവസ്ഥ 2:
	    തിരികെ D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	അവസ്ഥ 3:
	    തിരികെ D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
ToInt(DWRITE_RENDERING_MODE മൂല്യം)
{
    തിരഞ്ഞെടുപ്പ് (മൂല്യം)
    {
	അവസ്ഥ DWRITE_RENDERING_MODE_DEFAULT:
	    തിരികെ 0;
	അവസ്ഥ DWRITE_RENDERING_MODE_ALIASED:
	    തിരികെ 1;
	അവസ്ഥ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    തിരികെ 2;
	അവസ്ഥ DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    തിരികെ 3;
	അവസ്ഥ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    തിരികെ 4;
	അവസ്ഥ DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    തിരികെ 5;
	അവസ്ഥ DWRITE_RENDERING_MODE_OUTLINE:
	    തിരികെ 6;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ -1;
    }
}

വർഗ്ഗം FontCache {
പൊതു:
    ഘടന Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(ശൂന്യ), pTextFormat(ശൂന്യ), fontAscent(0.0f) {}
    };

സ്വകാര്യ:
    പൂർണ്ണസംഖ്യ mSize;
    Item *m_ഇനങ്ങൾ;

പൊതു:
    FontCache(പൂർണ്ണസംഖ്യ size = 2) :
	mSize(size),
	m_ഇനങ്ങൾ(പുതിയ Item[size])
    {
    }

    ~FontCache()
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mSize; ++i)
	    SafeRelease(&m_ഇനങ്ങൾ[i].pTextFormat);
	delete[] m_ഇനങ്ങൾ;
    }

    തർക്കം get(HFONT hFont, Item &item)
    {
	പൂർണ്ണസംഖ്യ n = find(hFont);
	എങ്കിൽ (n < 0)
	    തിരികെ അസത്യം;
	item = m_ഇനങ്ങൾ[n];
	slide(n);
	തിരികെ സത്യം;
    }

    ശൂന്യം put(സ്ഥിരം Item& item)
    {
	പൂർണ്ണസംഖ്യ n = find(item.hFont);
	എങ്കിൽ (n < 0)
	    n = mSize - 1;
	എങ്കിൽ (m_ഇനങ്ങൾ[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_ഇനങ്ങൾ[n].pTextFormat);
	    എങ്കിൽ (item.pTextFormat != ശൂന്യ)
		item.pTextFormat->AddRef();
	}
	m_ഇനങ്ങൾ[n] = item;
	slide(n);
    }

സ്വകാര്യ:
    പൂർണ്ണസംഖ്യ find(HFONT hFont)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < mSize; ++i)
	{
	    എങ്കിൽ (m_ഇനങ്ങൾ[i].hFont == hFont)
		തിരികെ i;
	}
	തിരികെ -1;
    }

    ശൂന്യം slide(പൂർണ്ണസംഖ്യ next_മുകൾ)
    {
	എങ്കിൽ (next_മുകൾ == 0)
	    തിരികെ;
	Item tmp = m_ഇനങ്ങൾ[next_മുകൾ];
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = next_മുകൾ - 1; i >= 0; --i)
	    m_ഇനങ്ങൾ[i + 1] = m_ഇനങ്ങൾ[i];
	m_ഇനങ്ങൾ[0] = tmp;
    }
};

എണ്ണൽ DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ഘടന DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    തർക്കം mDrawing;
    തർക്കം mFallbackDC;

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
    പൂർണ്ണസംഖ്യ m_Font_Feature_എണ്ണം;

    // METHODS

    DWriteContext();

    മായാ ~DWriteContext();

    HRESULT CreateDeviceResources();

    ശൂന്യം DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(സ്ഥിരം LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(സ്ഥിരം LOGFONTW &logFont);

    ശൂന്യം SetFont(HFONT hFont);

    ശൂന്യം Rebind();

    ശൂന്യം BindDC(HDC hdc, സ്ഥിരം RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    ശൂന്യം DrawText(സ്ഥിരം WCHAR *text, പൂർണ്ണസംഖ്യ len,
	പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h, പൂർണ്ണസംഖ്യ cellWidth, COLORREF color,
	UINT fuOptions, സ്ഥിരം RECT *lprc, സ്ഥിരം INT *lpDx);

    ശൂന്യം FillRect(സ്ഥിരം RECT *rc, COLORREF color);

    ശൂന്യം Draw_വരി(പൂർണ്ണസംഖ്യ x1, പൂർണ്ണസംഖ്യ y1, പൂർണ്ണസംഖ്യ x2, പൂർണ്ണസംഖ്യ y2, COLORREF color);

    ശൂന്യം SetPixel(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, COLORREF color);

    ശൂന്യം Scroll(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, സ്ഥിരം RECT *rc);

    ശൂന്യം Flush();

    ശൂന്യം SetRenderingParams(
	    സ്ഥിരം DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    ശൂന്യം SetFontFeatures(സ്ഥിരം DWriteFontFeature *features, പൂർണ്ണസംഖ്യ എണ്ണം);
};

വർഗ്ഗം AdjustedGlyphRun : പൊതു DWRITE_GLYPH_RUN
{
സ്വകാര്യ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

പൊതു:
    AdjustedGlyphRun(
	    സ്ഥിരം DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    സ്ഥിരം INT *lpDx,
	    പൂർണ്ണസംഖ്യ *glyphIndex,
	    പൂർണ്ണസംഖ്യ cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(പുതിയ FLOAT[glyphRun->glyph_എണ്ണം])
    {
	പ്രതിജ്ഞാ(cellWidth != 0.0f);
	വേണ്ടി (UINT32 i = 0; i < glyphRun->glyph_എണ്ണം; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    എങ്കിൽ (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    അല്ലെങ്കിൽ
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

    സ്ഥിതം FLOAT adjustToCell(FLOAT മൂല്യം, FLOAT cellWidth)
    {
	പൂർണ്ണസംഖ്യ cell_എണ്ണം = പൂർണ്ണസംഖ്യ(floor(മൂല്യം / cellWidth + 0.5f));
	എങ്കിൽ (cell_എണ്ണം < 1)
	    cell_എണ്ണം = 1;
	തിരികെ cell_എണ്ണം * cellWidth;
    }
};

ഘടന TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    സ്ഥിരം INT *lpDx;
    പൂർണ്ണസംഖ്യ cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    പൂർണ്ണസംഖ്യ glyphIndex;
};

വർഗ്ഗം TextRenderer FINAL : പൊതു IDWriteTextRenderer
{
പൊതു:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_എണ്ണം(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    മായാ ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	തിരികെ S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	തിരികെ S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	ദശാംശം dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	തിരികെ S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE സ്ഥിരം* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	തിരികെ E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH സ്ഥിരം* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	തിരികെ E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull ശൂന്യം* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_വലത്_To_ഇടത് UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	തിരികെ E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull ശൂന്യം* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN സ്ഥിരം* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION സ്ഥിരം* glyphRunDescription UNUSED,
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
	എങ്കിൽ (pDWC_->mDWriteFactory2 != ശൂന്യ)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = ശൂന്യ;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		ശൂന്യ,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		ശൂന്യ,
		0,
		&enumerator);
	    എങ്കിൽ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		ആകുമ്പോൾ (hasRun)
		{
		    സ്ഥിരം DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		തിരികെ S_OK;
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
	തിരികെ S_OK;
    }

പൊതു:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	തിരികെ InterlockedIncrement(&c_Ref_എണ്ണം);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_എണ്ണം_2 = InterlockedDecrement(&c_Ref_എണ്ണം);

	എങ്കിൽ (new_എണ്ണം_2 == 0)
	{
	    delete ഇത്;
	    തിരികെ 0;
	}
	തിരികെ new_എണ്ണം_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID സ്ഥിരം& riid,
	ശൂന്യം** ppvObject)
    {
	എങ്കിൽ (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = ഇത്;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ഇത്;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ഇത്;
	}
	അല്ലെങ്കിൽ
	{
	    *ppvObject = ശൂന്യ;
	    തിരികെ E_FAIL;
	}

	തിരികെ S_OK;
    }

സ്വകാര്യ:
    long c_Ref_എണ്ണം;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ശൂന്യ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ശൂന്യ),
    mDrawing(അസത്യം),
    mFallbackDC(അസത്യം),
    mD2D1Factory(ശൂന്യ),
    mRT(ശൂന്യ),
    mGDIRT(ശൂന്യ),
    mBrush(ശൂന്യ),
    mBitmap(ശൂന്യ),
    mDWriteFactory(ശൂന്യ),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ശൂന്യ),
#endif
    mGdiInterop(ശൂന്യ),
    mRenderingParams(ശൂന്യ),
    mFontCache(8),
    mTextFormat(ശൂന്യ),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_എണ്ണം(0)
{
    ZeroMemory(mFontFeatures, വലുപ്പം(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ശൂന്യ,
	    reinterpret_cast<ശൂന്യം**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    എങ്കിൽ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    എങ്കിൽ (SUCCEEDED(hr))
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

    എങ്കിൽ (mRT != ശൂന്യ)
	തിരികെ S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    എങ്കിൽ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<ശൂന്യം**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    എങ്കിൽ (SUCCEEDED(hr))
	Rebind();

    തിരികെ hr;
}

    ശൂന്യം
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(സ്ഥിരം LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ശൂന്യ;

    IDWriteFont *font = ശൂന്യ;
    IDWriteFontFamily *fontFamily = ശൂന്യ;
    IDWriteLocalizedStrings *localizedFamilyNames = ശൂന്യ;
    ദശാംശം fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    എങ്കിൽ (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    എങ്കിൽ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = ദശാംശം(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	എങ്കിൽ (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * ദശാംശം(fontMetrics.designUnitsPerEm)
		/ ദശാംശം(fontMetrics.ascent + fontMetrics.descent + fontMetrics.വരി_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    എങ്കിൽ (SUCCEEDED(hr))
    {
	എങ്കിൽ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    എങ്കിൽ (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		ശൂന്യ, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    എങ്കിൽ (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    എങ്കിൽ (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    എങ്കിൽ (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    എങ്കിൽ (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	എങ്കിൽ (pFontAscent != ശൂന്യ && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = ദശാംശം(fontMetrics.ascent + fontMetrics.descent);
	    എങ്കിൽ (totalHeight != 0.0f)
		*pFontAscent = ദശാംശം(fontMetrics.ascent) / totalHeight;
	    അല്ലെങ്കിൽ
		*pFontAscent = 0.83f; // fallback
	}
    }
    അല്ലെങ്കിൽ
	SafeRelease(&pTextFormat);

    തിരികെ hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(സ്ഥിരം LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = ശൂന്യ;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    എങ്കിൽ (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    തിരികെ hr;
}

    ശൂന്യം
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    എങ്കിൽ (mFontCache.get(hFont, item))
    {
	എങ്കിൽ (item.pTextFormat != ശൂന്യ)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = അസത്യം;
	}
	അല്ലെങ്കിൽ
	    mFallbackDC = സത്യം;
	തിരികെ;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    എങ്കിൽ (GetObjectW(hFont, വലുപ്പം(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    എങ്കിൽ (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = അസത്യം;
    }
    അല്ലെങ്കിൽ
	mFallbackDC = സത്യം;
    mFontCache.put(item);
}

    ശൂന്യം
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
	    D2D1::SizeU(mBindRect.വലത് - mBindRect.ഇടത്,
		mBindRect.bottom - mBindRect.മുകൾ),
	    props, &mBitmap);
}

    ശൂന്യം
DWriteContext::BindDC(HDC hdc, സ്ഥിരം RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    എങ്കിൽ (mRT == ശൂന്യ)
	CreateDeviceResources();
    അല്ലെങ്കിൽ
    {
	Flush();
	Rebind();
    }
}

ബാഹ്യ "C" ശൂന്യം redraw_later_clear(ശൂന്യം);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    തിരഞ്ഞെടുപ്പ് (mode)
    {
	സ്ഥിരസ്ഥിതി:
	അവസ്ഥ DM_GDI:
	    എങ്കിൽ (mInteropHDC != ശൂന്യ)
	    {
		mGDIRT->ReleaseDC(ശൂന്യ);
		mInteropHDC = ശൂന്യ;
	    }
	    എങ്കിൽ (mDrawing)
	    {
		hr = mRT->EndDraw();
		എങ്കിൽ (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = അസത്യം;
	    }
	    നിർത്തുക;

	അവസ്ഥ DM_DIRECTX:
	    എങ്കിൽ (mInteropHDC != ശൂന്യ)
	    {
		mGDIRT->ReleaseDC(ശൂന്യ);
		mInteropHDC = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (mDrawing == അസത്യം)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = സത്യം;
	    }
	    നിർത്തുക;

	അവസ്ഥ DM_INTEROP:
	    എങ്കിൽ (mDrawing == അസത്യം)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = സത്യം;
	    }
	    എങ്കിൽ (mInteropHDC == ശൂന്യ)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    നിർത്തുക;
    }
    mDMode = mode;
    തിരികെ hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    തിരികെ mBrush;
}

    ശൂന്യം
DWriteContext::DrawText(സ്ഥിരം WCHAR *text, പൂർണ്ണസംഖ്യ len,
	പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h, പൂർണ്ണസംഖ്യ cellWidth, COLORREF color,
	UINT fuOptions, സ്ഥിരം RECT *lprc, സ്ഥിരം INT *lpDx)
{
    എങ്കിൽ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	എങ്കിൽ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	തിരികെ;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = ശൂന്യ;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    എങ്കിൽ ((fuOptions & ETO_CLIPPED) && lprc != ശൂന്യ)
    {
	mRT->തള്ളുക_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ഇടത്), FLOAT(lprc->മുകൾ),
			FLOAT(lprc->വലത്), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    എങ്കിൽ (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	എങ്കിൽ (m_Font_Feature_എണ്ണം > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    സ്ഥിതം സ്ഥിരം DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ അക്കം_Defaults = വലുപ്പം(defaultFeatures)
		/ വലുപ്പം(defaultFeatures[0]);

	    IDWriteTypography *typography = ശൂന്യ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    എങ്കിൽ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		വേണ്ടി (പൂർണ്ണസംഖ്യ d = 0; d < അക്കം_Defaults; ++d)
		{
		    പൂർണ്ണസംഖ്യ overridden = 0;
		    വേണ്ടി (പൂർണ്ണസംഖ്യ u = 0; u < m_Font_Feature_എണ്ണം; ++u)
		    {
			എങ്കിൽ ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    നിർത്തുക;
			}
		    }
		    എങ്കിൽ (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < m_Font_Feature_എണ്ണം; ++i)
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

	TextRenderer renderer(ഇത്);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    എങ്കിൽ ((fuOptions & ETO_CLIPPED) && lprc != ശൂന്യ)
	mRT->എടുക്കുക_Axis_Aligned_Clip();
}

    ശൂന്യം
DWriteContext::FillRect(സ്ഥിരം RECT *rc, COLORREF color)
{
    എങ്കിൽ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    അല്ലെങ്കിൽ
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ഇടത്), FLOAT(rc->മുകൾ),
		    FLOAT(rc->വലത്), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    ശൂന്യം
DWriteContext::Draw_വരി(പൂർണ്ണസംഖ്യ x1, പൂർണ്ണസംഖ്യ y1, പൂർണ്ണസംഖ്യ x2, പൂർണ്ണസംഖ്യ y2, COLORREF color)
{
    എങ്കിൽ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, ശൂന്യ);
	::വരി_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    അല്ലെങ്കിൽ
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_വരി(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    ശൂന്യം
DWriteContext::SetPixel(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, COLORREF color)
{
    എങ്കിൽ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    അല്ലെങ്കിൽ
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_വരി(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    ശൂന്യം
DWriteContext::Scroll(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, സ്ഥിരം RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    എങ്കിൽ (x >= 0)
    {
	srcRect.ഇടത് = rc->ഇടത്;
	srcRect.വലത് = rc->വലത് - x;
	destPoint.x = rc->ഇടത് + x;
    }
    അല്ലെങ്കിൽ
    {
	srcRect.ഇടത് = rc->ഇടത് - x;
	srcRect.വലത് = rc->വലത്;
	destPoint.x = rc->ഇടത്;
    }
    എങ്കിൽ (y >= 0)
    {
	srcRect.മുകൾ = rc->മുകൾ;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->മുകൾ + y;
    }
    അല്ലെങ്കിൽ
    {
	srcRect.മുകൾ = rc->മുകൾ - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->മുകൾ;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.വലത് - srcRect.ഇടത്),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.മുകൾ)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    ശൂന്യം
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    ശൂന്യം
DWriteContext::SetRenderingParams(
	സ്ഥിരം DWriteRenderingParams *params)
{
    എങ്കിൽ (mDWriteFactory == ശൂന്യ)
	തിരികെ;

    IDWriteRenderingParams *renderingParams = ശൂന്യ;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    എങ്കിൽ (params != ശൂന്യ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    അല്ലെങ്കിൽ
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    എങ്കിൽ (SUCCEEDED(hr) && renderingParams != ശൂന്യ)
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
    എങ്കിൽ (params != ശൂന്യ && mRenderingParams != ശൂന്യ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    തിരികെ params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    ശൂന്യം
DWrite_Init(ശൂന്യം)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    എങ്കിൽ (hD2D1DLL == ശൂന്യ || hDWriteDLL == ശൂന്യ)
    {
	DWrite_Final();
	തിരികെ;
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

    ശൂന്യം
DWrite_Final(ശൂന്യം)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ശൂന്യ;
    pD2D1CreateFactory = ശൂന്യ;
    pDWriteCreateFactory = ശൂന്യ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(ശൂന്യം)
{
#ifdef DYNAMIC_DIRECTX
    എങ്കിൽ (pGetUserDefaultLocaleName == ശൂന്യ || pD2D1CreateFactory == ശൂന്യ
	    || pDWriteCreateFactory == ശൂന്യ)
	തിരികെ ശൂന്യ;
#endif
    തിരികെ പുതിയ DWriteContext();
}

    ശൂന്യം
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, സ്ഥിരം RECT *rect)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->BindDC(hdc, rect);
}

    ശൂന്യം
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->SetFont(hFont);
}

    ശൂന്യം
DWriteContext_DrawText(
	DWriteContext *ctx,
	സ്ഥിരം WCHAR *text,
	പൂർണ്ണസംഖ്യ len,
	പൂർണ്ണസംഖ്യ x,
	പൂർണ്ണസംഖ്യ y,
	പൂർണ്ണസംഖ്യ w,
	പൂർണ്ണസംഖ്യ h,
	പൂർണ്ണസംഖ്യ cellWidth,
	COLORREF color,
	UINT fuOptions,
	സ്ഥിരം RECT *lprc,
	സ്ഥിരം INT *lpDx)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    ശൂന്യം
DWriteContext_FillRect(DWriteContext *ctx, സ്ഥിരം RECT *rc, COLORREF color)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->FillRect(rc, color);
}

    ശൂന്യം
DWrite_Context_Draw_വരി(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x1, പൂർണ്ണസംഖ്യ y1, പൂർണ്ണസംഖ്യ x2, പൂർണ്ണസംഖ്യ y2,
	COLORREF color)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->Draw_വരി(x1, y1, x2, y2, color);
}

    ശൂന്യം
DWriteContext_SetPixel(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, COLORREF color)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->SetPixel(x, y, color);
}

    ശൂന്യം
DWriteContext_Scroll(DWriteContext *ctx, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, സ്ഥിരം RECT *rc)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->Scroll(x, y, rc);
}

    ശൂന്യം
DWriteContext_Flush(DWriteContext *ctx)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->Flush();
}

    ശൂന്യം
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    ശൂന്യം
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	സ്ഥിരം DWriteRenderingParams *params)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	തിരികെ ctx->GetRenderingParams(params);
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    ശൂന്യം
DWriteContext::SetFontFeatures(
	സ്ഥിരം DWriteFontFeature *features, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (എണ്ണം > DWRITE_MAX_FONT_FEATURES)
	എണ്ണം = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_എണ്ണം = എണ്ണം;
    എങ്കിൽ (എണ്ണം > 0 && features != ശൂന്യ)
	memcpy(mFontFeatures, features, വലുപ്പം(DWriteFontFeature) * എണ്ണം);
}

    ശൂന്യം
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	സ്ഥിരം DWriteFontFeature *features,
	പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (ctx != ശൂന്യ)
	ctx->SetFontFeatures(features, എണ്ണം);
}
