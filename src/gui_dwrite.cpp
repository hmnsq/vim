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
# எனில் WINVER < 0x0600
#  error WINVER must be 0x0600 or above to use DirectWrite(DirectX)
# endif
#endif

#அந்தர்பாவ <windows.h>
#அந்தர்பாவ <crtdbg.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <math.h>
#அந்தர்பாவ <d2d1.h>
#அந்தர்பாவ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#எனில் defined(_MSC_VER) && (_MSC_VER < 1700)
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
#இல்லையெனில்
# include <dwrite.h>
#endif

#அந்தர்பாவ "gui_dwrite.h"

#ifdef __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL__in
# define __out		SAL__out
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#இல்லையெனில்
# define UNUSED
#endif

#எனில் (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#இல்லையெனில்
# define FINAL
#endif

#ifdef DYNAMIC_DIRECTX
வெளி "C" HINSTANCE vimLoadLib(மாறிலி எழுத்து *name);

வகைவரை முழு (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, முழு);
வகைவரை HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, மாறிலி D2D1_FACTORY_OPTIONS *, வெறுமை **);
வகைவரை HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

நிலையான HINSTANCE hD2D1DLL = வெற்று;
நிலையான HINSTANCE hDWriteDLL = வெற்று;

நிலையான PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = வெற்று;
நிலையான PD2D1CREATEFACTORY pD2D1CreateFactory = வெற்று;
நிலையான PDWRITECREATEFACTORY pDWriteCreateFactory = வெற்று;

#define GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#define D2D1CreateFactory		(*pD2D1CreateFactory)
#define DWriteCreateFactory		(*pDWriteCreateFactory)

    நிலையான வெறுமை
unload(HINSTANCE &hinst)
{
    எனில் (hinst != வெற்று)
    {
	FreeLibrary(hinst);
	hinst = வெற்று;
    }
}
#endif // DYNAMIC_DIRECTX

template <வகுப்பு T> உள்வரி வெறுமை SafeRelease(T **ppT)
{
    எனில் (*ppT)
    {
	(*ppT)->Release();
	*ppT = வெற்று;
    }
}

    நிலையான DWRITE_PIXEL_GEOMETRY
ToPixelGeometry(முழு மதிப்பு)
{
    தேர்வு (மதிப்பு)
    {
	இயல்பு:
	நிலை 0:
	    திரும்பு DWRITE_PIXEL_GEOMETRY_FLAT;
	நிலை 1:
	    திரும்பு DWRITE_PIXEL_GEOMETRY_RGB;
	நிலை 2:
	    திரும்பு DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    நிலையான முழு
ToInt(DWRITE_PIXEL_GEOMETRY மதிப்பு)
{
    தேர்வு (மதிப்பு)
    {
	நிலை DWRITE_PIXEL_GEOMETRY_FLAT:
	    திரும்பு 0;
	நிலை DWRITE_PIXEL_GEOMETRY_RGB:
	    திரும்பு 1;
	நிலை DWRITE_PIXEL_GEOMETRY_BGR:
	    திரும்பு 2;
	இயல்பு:
	    திரும்பு -1;
    }
}

    நிலையான DWRITE_RENDERING_MODE
ToRenderingMode(முழு மதிப்பு)
{
    தேர்வு (மதிப்பு)
    {
	இயல்பு:
	நிலை 0:
	    திரும்பு DWRITE_RENDERING_MODE_DEFAULT;
	நிலை 1:
	    திரும்பு DWRITE_RENDERING_MODE_ALIASED;
	நிலை 2:
	    திரும்பு DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC;
	நிலை 3:
	    திரும்பு DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL;
	நிலை 4:
	    திரும்பு DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL;
	நிலை 5:
	    திரும்பு DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC;
	நிலை 6:
	    திரும்பு DWRITE_RENDERING_MODE_OUTLINE;
    }
}

    நிலையான D2D1_TEXT_ANTIALIAS_MODE
ToTextAntialiasMode(முழு மதிப்பு)
{
    தேர்வு (மதிப்பு)
    {
	இயல்பு:
	நிலை 0:
	    திரும்பு D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
	நிலை 1:
	    திரும்பு D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
	நிலை 2:
	    திரும்பு D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	நிலை 3:
	    திரும்பு D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
    }
}

    நிலையான முழு
ToInt(DWRITE_RENDERING_MODE மதிப்பு)
{
    தேர்வு (மதிப்பு)
    {
	நிலை DWRITE_RENDERING_MODE_DEFAULT:
	    திரும்பு 0;
	நிலை DWRITE_RENDERING_MODE_ALIASED:
	    திரும்பு 1;
	நிலை DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC:
	    திரும்பு 2;
	நிலை DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL:
	    திரும்பு 3;
	நிலை DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL:
	    திரும்பு 4;
	நிலை DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC:
	    திரும்பு 5;
	நிலை DWRITE_RENDERING_MODE_OUTLINE:
	    திரும்பு 6;
	இயல்பு:
	    திரும்பு -1;
    }
}

வகுப்பு FontCache {
பொது:
    கட்டமைப்பு Item {
	HFONT              hFont;
	IDWriteTextFormat* pTextFormat;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	Item() : hFont(வெற்று), pTextFormat(வெற்று), fontAscent(0.0f) {}
    };

தனிப்பட்ட:
    முழு mSize;
    Item *m_பொருட்கள்;

பொது:
    FontCache(முழு size = 2) :
	mSize(size),
	m_பொருட்கள்(புதிய Item[size])
    {
    }

    ~FontCache()
    {
	க்கு (முழு i = 0; i < mSize; ++i)
	    SafeRelease(&m_பொருட்கள்[i].pTextFormat);
	delete[] m_பொருட்கள்;
    }

    தருக்க get(HFONT hFont, Item &item)
    {
	முழு n = find(hFont);
	எனில் (n < 0)
	    திரும்பு பொய்;
	item = m_பொருட்கள்[n];
	slide(n);
	திரும்பு உண்மை;
    }

    வெறுமை put(மாறிலி Item& item)
    {
	முழு n = find(item.hFont);
	எனில் (n < 0)
	    n = mSize - 1;
	எனில் (m_பொருட்கள்[n].pTextFormat != item.pTextFormat)
	{
	    SafeRelease(&m_பொருட்கள்[n].pTextFormat);
	    எனில் (item.pTextFormat != வெற்று)
		item.pTextFormat->AddRef();
	}
	m_பொருட்கள்[n] = item;
	slide(n);
    }

தனிப்பட்ட:
    முழு find(HFONT hFont)
    {
	க்கு (முழு i = 0; i < mSize; ++i)
	{
	    எனில் (m_பொருட்கள்[i].hFont == hFont)
		திரும்பு i;
	}
	திரும்பு -1;
    }

    வெறுமை slide(முழு next_உச்சி)
    {
	எனில் (next_உச்சி == 0)
	    திரும்பு;
	Item tmp = m_பொருட்கள்[next_உச்சி];
	க்கு (முழு i = next_உச்சி - 1; i >= 0; --i)
	    m_பொருட்கள்[i + 1] = m_பொருட்கள்[i];
	m_பொருட்கள்[0] = tmp;
    }
};

எண்ணிக்கை DrawingMode {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

கட்டமைப்பு DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    DrawingMode mDMode;
    HDC mInteropHDC;
    தருக்க mDrawing;
    தருக்க mFallbackDC;

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
    முழு m_Font_Feature_மொத்தம்;

    // METHODS

    DWriteContext();

    மெய்நிகர் ~DWriteContext();

    HRESULT CreateDeviceResources();

    வெறுமை DiscardDeviceResources();

    HRESULT CreateTextFormatFromLOGFONT(மாறிலி LOGFONTW &logFont,
	    IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent);

    HRESULT SetFontByLOGFONT(மாறிலி LOGFONTW &logFont);

    வெறுமை SetFont(HFONT hFont);

    வெறுமை Rebind();

    வெறுமை BindDC(HDC hdc, மாறிலி RECT *rect);

    HRESULT SetDrawingMode(DrawingMode mode);

    ID2D1Brush* SolidBrush(COLORREF color);

    வெறுமை DrawText(மாறிலி WCHAR *text, முழு len,
	முழு x, முழு y, முழு w, முழு h, முழு cellWidth, COLORREF color,
	UINT fuOptions, மாறிலி RECT *lprc, மாறிலி INT *lpDx);

    வெறுமை FillRect(மாறிலி RECT *rc, COLORREF color);

    வெறுமை Draw_வரி(முழு x1, முழு y1, முழு x2, முழு y2, COLORREF color);

    வெறுமை SetPixel(முழு x, முழு y, COLORREF color);

    வெறுமை Scroll(முழு x, முழு y, மாறிலி RECT *rc);

    வெறுமை Flush();

    வெறுமை SetRenderingParams(
	    மாறிலி DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    வெறுமை SetFontFeatures(மாறிலி DWriteFontFeature *features, முழு மொத்தம்);
};

வகுப்பு AdjustedGlyphRun : பொது DWRITE_GLYPH_RUN
{
தனிப்பட்ட:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

பொது:
    AdjustedGlyphRun(
	    மாறிலி DWRITE_GLYPH_RUN *glyphRun,
	    FLOAT cellWidth,
	    FLOAT &accum,
	    மாறிலி INT *lpDx,
	    முழு *glyphIndex,
	    முழு cchText) :
	DWRITE_GLYPH_RUN(*glyphRun),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(புதிய FLOAT[glyphRun->glyph_மொத்தம்])
    {
	ப்ரதிஜ்ஞா(cellWidth != 0.0f);
	க்கு (UINT32 i = 0; i < glyphRun->glyph_மொத்தம்; ++i)
	{
	    FLOAT orig = glyphRun->glyphAdvances[i];
	    FLOAT adjusted;

	    எனில் (*glyphIndex < cchText)
	    {
		adjusted = FLOAT(lpDx[*glyphIndex]);
		(*glyphIndex)++;
	    }
	    இல்லையெனில்
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

    நிலையான FLOAT adjustToCell(FLOAT மதிப்பு, FLOAT cellWidth)
    {
	முழு cell_மொத்தம் = முழு(floor(மதிப்பு / cellWidth + 0.5f));
	எனில் (cell_மொத்தம் < 1)
	    cell_மொத்தம் = 1;
	திரும்பு cell_மொத்தம் * cellWidth;
    }
};

கட்டமைப்பு TextRendererContext {
    // const fields.
    COLORREF color;
    FLOAT cellWidth;
    மாறிலி INT *lpDx;
    முழு cchText;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT offsetX;
    முழு glyphIndex;
};

வகுப்பு TextRenderer FINAL : பொது IDWriteTextRenderer
{
பொது:
    TextRenderer(
	    DWriteContext* pDWC) :
	c_Ref_மொத்தம்(0),
	pDWC_(pDWC)
    {
	AddRef();
    }

    // add "virtual" to avoid a compiler warning
    மெய்நிகர் ~TextRenderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	திரும்பு S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	__out DWRITE_MATRIX* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
	திரும்பு S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	__out FLOAT* pixelsPerDip)
    {
	தசம dpiX, unused;
	pDWC_->mRT->GetDpi(&dpiX, &unused);
	*pixelsPerDip = dpiX / 96.0f;
	திரும்பு S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_UNDERLINE மாறிலி* underline UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	திரும்பு E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	FLOAT baselineOriginX UNUSED,
	FLOAT baselineOriginY UNUSED,
	__in DWRITE_STRIKETHROUGH மாறிலி* strikethrough UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	திரும்பு E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull வெறுமை* clientDrawingContext UNUSED,
	FLOAT originX UNUSED,
	FLOAT originY UNUSED,
	IDWriteInlineObject* inlineObject UNUSED,
	BOOL isSideways UNUSED,
	BOOL is_வலது_To_இடது UNUSED,
	IUnknown* clientDrawingEffect UNUSED)
    {
	திரும்பு E_NOTIMPL;
    }

    IFACEMETHOD(DrawGlyphRun)(
	__maybenull வெறுமை* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode UNUSED,
	__in DWRITE_GLYPH_RUN மாறிலி* glyphRun,
	__in DWRITE_GLYPH_RUN_DESCRIPTION மாறிலி* glyphRunDescription UNUSED,
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
	எனில் (pDWC_->mDWriteFactory2 != வெற்று)
	{
	    IDWriteColorGlyphRunEnumerator *enumerator = வெற்று;
	    HRESULT hr = pDWC_->mDWriteFactory2->TranslateColorGlyphRun(
		baselineOriginX + context->offsetX,
		baselineOriginY,
		&adjustedGlyphRun,
		வெற்று,
		DWRITE_MEASURING_MODE_GDI_NATURAL,
		வெற்று,
		0,
		&enumerator);
	    எனில் (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL hasRun = TRUE;
		enumerator->MoveNext(&hasRun);
		போது (hasRun)
		{
		    மாறிலி DWRITE_COLOR_GLYPH_RUN* colorGlyphRun;
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
		திரும்பு S_OK;
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
	திரும்பு S_OK;
    }

பொது:
    IFACEMETHOD_(unsigned long, AddRef) ()
    {
	திரும்பு InterlockedIncrement(&c_Ref_மொத்தம்);
    }

    IFACEMETHOD_(unsigned long, Release) ()
    {
	long new_மொத்தம்_2 = InterlockedDecrement(&c_Ref_மொத்தம்);

	எனில் (new_மொத்தம்_2 == 0)
	{
	    delete இது;
	    திரும்பு 0;
	}
	திரும்பு new_மொத்தம்_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID மாறிலி& riid,
	வெறுமை** ppvObject)
    {
	எனில் (__uuidof(IDWriteTextRenderer) == riid)
	{
	    *ppvObject = இது;
	}
	இல்லையெனில் எனில் (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = இது;
	}
	இல்லையெனில் எனில் (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = இது;
	}
	இல்லையெனில்
	{
	    *ppvObject = வெற்று;
	    திரும்பு E_FAIL;
	}

	திரும்பு S_OK;
    }

தனிப்பட்ட:
    long c_Ref_மொத்தம்;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(வெற்று),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(வெற்று),
    mDrawing(பொய்),
    mFallbackDC(பொய்),
    mD2D1Factory(வெற்று),
    mRT(வெற்று),
    mGDIRT(வெற்று),
    mBrush(வெற்று),
    mBitmap(வெற்று),
    mDWriteFactory(வெற்று),
#ifdef FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(வெற்று),
#endif
    mGdiInterop(வெற்று),
    mRenderingParams(வெற்று),
    mFontCache(8),
    mTextFormat(வெற்று),
    mFontWeight(DWRITE_FONT_WEIGHT_NORMAL),
    mFontStyle(DWRITE_FONT_STYLE_NORMAL),
    mFontSize(0.0f),
    mFontAscent(0.0f),
    mTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT),
    m_Font_Feature_மொத்தம்(0)
{
    ZeroMemory(mFontFeatures, அளவு(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), வெற்று,
	    reinterpret_cast<வெறுமை**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    எனில் (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#ifdef FEAT_DIRECTX_COLOR_EMOJI
    எனில் (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#endif

    எனில் (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    எனில் (SUCCEEDED(hr))
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

    எனில் (mRT != வெற்று)
	திரும்பு S_OK;

    D2D1_RENDER_TARGET_PROPERTIES props = {
	D2D1_RENDER_TARGET_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE },
	0, 0,
	D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->CreateDCRenderTarget(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    எனில் (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2D1GdiInteropRenderTarget),
		reinterpret_cast<வெறுமை**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    எனில் (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    எனில் (SUCCEEDED(hr))
	Rebind();

    திரும்பு hr;
}

    வெறுமை
DWriteContext::DiscardDeviceResources()
{
    SafeRelease(&mBitmap);
    SafeRelease(&mBrush);
    SafeRelease(&mGDIRT);
    SafeRelease(&mRT);
}

    HRESULT
DWriteContext::CreateTextFormatFromLOGFONT(மாறிலி LOGFONTW &logFont,
	IDWriteTextFormat **ppTextFormat, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = வெற்று;

    IDWriteFont *font = வெற்று;
    IDWriteFontFamily *fontFamily = வெற்று;
    IDWriteLocalizedStrings *localizedFamilyNames = வெற்று;
    தசம fontSize = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    எனில் (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    எனில் (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    எனில் (SUCCEEDED(hr))
    {
	hr = fontFamily->GetFamilyNames(&localizedFamilyNames);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    எனில் (SUCCEEDED(hr))
    {
	hr = localizedFamilyNames->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    எனில் (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	fontSize = தசம(logFont.lfHeight);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	எனில் (fontSize < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    fontSize = -fontSize;
	}
	இல்லையெனில் எனில் (fontSize > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    fontSize = fontSize * தசம(fontMetrics.designUnitsPerEm)
		/ தசம(fontMetrics.ascent + fontMetrics.descent + fontMetrics.வரி_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    எனில் (SUCCEEDED(hr))
    {
	எனில் (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
	    hr = HRESULT_FROM_WIN32(GetLastError());
    }

    எனில் (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->CreateTextFormat(
		familyName,
		வெற்று, // no custom font collection
		font->GetWeight(),
		font->GetStyle(),
		font->GetStretch(),
		fontSize,
		localeName,
		&pTextFormat);
    }

    எனில் (SUCCEEDED(hr))
	hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    எனில் (SUCCEEDED(hr))
	hr = pTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    எனில் (SUCCEEDED(hr))
	hr = pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    SafeRelease(&localizedFamilyNames);
    SafeRelease(&fontFamily);
    SafeRelease(&font);

    எனில் (SUCCEEDED(hr))
    {
	*ppTextFormat = pTextFormat;
	எனில் (pFontAscent != வெற்று && fontMetrics.designUnitsPerEm != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT totalHeight = தசம(fontMetrics.ascent + fontMetrics.descent);
	    எனில் (totalHeight != 0.0f)
		*pFontAscent = தசம(fontMetrics.ascent) / totalHeight;
	    இல்லையெனில்
		*pFontAscent = 0.83f; // fallback
	}
    }
    இல்லையெனில்
	SafeRelease(&pTextFormat);

    திரும்பு hr;
}

    HRESULT
DWriteContext::SetFontByLOGFONT(மாறிலி LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWriteTextFormat *pTextFormat = வெற்று;
    FLOAT fontAscent = 0.0f;

    hr = CreateTextFormatFromLOGFONT(logFont, &pTextFormat, &fontAscent);

    எனில் (SUCCEEDED(hr))
    {
	SafeRelease(&mTextFormat);
	mTextFormat = pTextFormat;
	mFontWeight = static_cast<DWRITE_FONT_WEIGHT>(logFont.lfWeight);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_NORMAL;
	mFontAscent = fontAscent;
    }

    திரும்பு hr;
}

    வெறுமை
DWriteContext::SetFont(HFONT hFont)
{
    FontCache::Item item;
    எனில் (mFontCache.get(hFont, item))
    {
	எனில் (item.pTextFormat != வெற்று)
	{
	    item.pTextFormat->AddRef();
	    SafeRelease(&mTextFormat);
	    mTextFormat = item.pTextFormat;
	    mFontWeight = item.fontWeight;
	    mFontStyle = item.fontStyle;
	    mFontAscent = item.fontAscent;
	    mFallbackDC = பொய்;
	}
	இல்லையெனில்
	    mFallbackDC = உண்மை;
	திரும்பு;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    எனில் (GetObjectW(hFont, அளவு(lf), &lf))
	hr = SetFontByLOGFONT(lf);

    item.hFont = hFont;
    எனில் (SUCCEEDED(hr))
    {
	item.pTextFormat = mTextFormat;
	item.fontWeight = mFontWeight;
	item.fontStyle = mFontStyle;
	item.fontAscent = mFontAscent;
	mFallbackDC = பொய்;
    }
    இல்லையெனில்
	mFallbackDC = உண்மை;
    mFontCache.put(item);
}

    வெறுமை
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
	    D2D1::SizeU(mBindRect.வலது - mBindRect.இடது,
		mBindRect.bottom - mBindRect.உச்சி),
	    props, &mBitmap);
}

    வெறுமை
DWriteContext::BindDC(HDC hdc, மாறிலி RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    எனில் (mRT == வெற்று)
	CreateDeviceResources();
    இல்லையெனில்
    {
	Flush();
	Rebind();
    }
}

வெளி "C" வெறுமை redraw_later_clear(வெறுமை);

    HRESULT
DWriteContext::SetDrawingMode(DrawingMode mode)
{
    HRESULT hr = S_OK;

    தேர்வு (mode)
    {
	இயல்பு:
	நிலை DM_GDI:
	    எனில் (mInteropHDC != வெற்று)
	    {
		mGDIRT->ReleaseDC(வெற்று);
		mInteropHDC = வெற்று;
	    }
	    எனில் (mDrawing)
	    {
		hr = mRT->EndDraw();
		எனில் (hr == (HRESULT)D2DERR_RECREATE_TARGET)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = பொய்;
	    }
	    நிறுத்து;

	நிலை DM_DIRECTX:
	    எனில் (mInteropHDC != வெற்று)
	    {
		mGDIRT->ReleaseDC(வெற்று);
		mInteropHDC = வெற்று;
	    }
	    இல்லையெனில் எனில் (mDrawing == பொய்)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = உண்மை;
	    }
	    நிறுத்து;

	நிலை DM_INTEROP:
	    எனில் (mDrawing == பொய்)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = உண்மை;
	    }
	    எனில் (mInteropHDC == வெற்று)
		hr = mGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &mInteropHDC);
	    நிறுத்து;
    }
    mDMode = mode;
    திரும்பு hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->SetColor(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    திரும்பு mBrush;
}

    வெறுமை
DWriteContext::DrawText(மாறிலி WCHAR *text, முழு len,
	முழு x, முழு y, முழு w, முழு h, முழு cellWidth, COLORREF color,
	UINT fuOptions, மாறிலி RECT *lprc, மாறிலி INT *lpDx)
{
    எனில் (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = SetDrawingMode(DM_INTEROP);
	எனில் (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::SetTextColor(mInteropHDC, color);
	    ::SetBkMode(mInteropHDC, ::GetBkMode(mHDC));
	    ::ExtTextOutW(mInteropHDC, x, y, fuOptions, lprc, text, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	திரும்பு;
    }

    HRESULT hr;
    IDWriteTextLayout *textLayout = வெற்று;

    SetDrawingMode(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    எனில் ((fuOptions & ETO_CLIPPED) && lprc != வெற்று)
    {
	mRT->தள்ளு_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->இடது), FLOAT(lprc->உச்சி),
			FLOAT(lprc->வலது), FLOAT(lprc->bottom)),
	    D2D1_ANTIALIAS_MODE_ALIASED);
    }

    hr = mDWriteFactory->CreateTextLayout(text, len, mTextFormat,
	    FLOAT(w), FLOAT(h), &textLayout);

    எனில் (SUCCEEDED(hr))
    {
	DWRITE_TEXT_RANGE textRange = { 0, UINT32(len) };
	textLayout->SetFontWeight(mFontWeight, textRange);
	textLayout->SetFontStyle(mFontStyle, textRange);

	எனில் (m_Font_Feature_மொத்தம் > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    நிலையான மாறிலி DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_TAG_KERNING, 1 },
	    };
	    நிலையான மாறிலி முழு இலக்கம்_Defaults = அளவு(defaultFeatures)
		/ அளவு(defaultFeatures[0]);

	    IDWriteTypography *typography = வெற்று;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    எனில் (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		க்கு (முழு d = 0; d < இலக்கம்_Defaults; ++d)
		{
		    முழு overridden = 0;
		    க்கு (முழு u = 0; u < m_Font_Feature_மொத்தம்; ++u)
		    {
			எனில் ((DWRITE_FONT_FEATURE_TAG)mFontFeatures[u].tag
				== defaultFeatures[d].nameTag)
			{
			    overridden = 1;
			    நிறுத்து;
			}
		    }
		    எனில் (!overridden)
			typography->AddFontFeature(defaultFeatures[d]);
		}
		// Add user-specified features.
		க்கு (முழு i = 0; i < m_Font_Feature_மொத்தம்; ++i)
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

	TextRenderer renderer(இது);
	TextRendererContext context = { color, FLOAT(cellWidth), lpDx, len,
		baselineY, 0.0f, 0 };

	textLayout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    SafeRelease(&textLayout);

    எனில் ((fuOptions & ETO_CLIPPED) && lprc != வெற்று)
	mRT->எடு_Axis_Aligned_Clip();
}

    வெறுமை
DWriteContext::FillRect(மாறிலி RECT *rc, COLORREF color)
{
    எனில் (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    இல்லையெனில்
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->இடது), FLOAT(rc->உச்சி),
		    FLOAT(rc->வலது), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    வெறுமை
DWriteContext::Draw_வரி(முழு x1, முழு y1, முழு x2, முழு y2, COLORREF color)
{
    எனில் (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::MoveToEx(mInteropHDC, x1, y1, வெற்று);
	::வரி_To(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    இல்லையெனில்
    {
	SetDrawingMode(DM_DIRECTX);
	mRT->Draw_வரி(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    வெறுமை
DWriteContext::SetPixel(முழு x, முழு y, COLORREF color)
{
    எனில் (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::SetPixel(mInteropHDC, x, y, color);
    }
    இல்லையெனில்
    {
	SetDrawingMode(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_வரி(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    வெறுமை
DWriteContext::Scroll(முழு x, முழு y, மாறிலி RECT *rc)
{
    SetDrawingMode(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2D1_POINT_2U destPoint;
    எனில் (x >= 0)
    {
	srcRect.இடது = rc->இடது;
	srcRect.வலது = rc->வலது - x;
	destPoint.x = rc->இடது + x;
    }
    இல்லையெனில்
    {
	srcRect.இடது = rc->இடது - x;
	srcRect.வலது = rc->வலது;
	destPoint.x = rc->இடது;
    }
    எனில் (y >= 0)
    {
	srcRect.உச்சி = rc->உச்சி;
	srcRect.bottom = rc->bottom - y;
	destPoint.y = rc->உச்சி + y;
    }
    இல்லையெனில்
    {
	srcRect.உச்சி = rc->உச்சி - y;
	srcRect.bottom = rc->bottom;
	destPoint.y = rc->உச்சி;
    }
    mBitmap->CopyFromRenderTarget(&destPoint, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(destPoint.x), FLOAT(destPoint.y),
	    FLOAT(destPoint.x + srcRect.வலது - srcRect.இடது),
	    FLOAT(destPoint.y + srcRect.bottom - srcRect.உச்சி)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, destRect);
}

    வெறுமை
DWriteContext::Flush()
{
    SetDrawingMode(DM_GDI);
}

    வெறுமை
DWriteContext::SetRenderingParams(
	மாறிலி DWriteRenderingParams *params)
{
    எனில் (mDWriteFactory == வெற்று)
	திரும்பு;

    IDWriteRenderingParams *renderingParams = வெற்று;
    D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode =
	D2D1_TEXT_ANTIALIAS_MODE_DEFAULT;
    HRESULT hr;
    எனில் (params != வெற்று)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		ToPixelGeometry(params->pixelGeometry),
		ToRenderingMode(params->renderingMode), &renderingParams);
	textAntialiasMode = ToTextAntialiasMode(params->textAntialiasMode);
    }
    இல்லையெனில்
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    எனில் (SUCCEEDED(hr) && renderingParams != வெற்று)
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
    எனில் (params != வெற்று && mRenderingParams != வெற்று)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = ToInt(mRenderingParams->GetPixelGeometry());
	params->renderingMode = ToInt(mRenderingParams->GetRenderingMode());
	params->textAntialiasMode = mTextAntialiasMode;
    }
    திரும்பு params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    வெறுமை
DWrite_Init(வெறுமை)
{
#ifdef DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vimLoadLib("d2d1.dll");
    hDWriteDLL = vimLoadLib("dwrite.dll");
    எனில் (hD2D1DLL == வெற்று || hDWriteDLL == வெற்று)
    {
	DWrite_Final();
	திரும்பு;
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

    வெறுமை
DWrite_Final(வெறுமை)
{
#ifdef DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = வெற்று;
    pD2D1CreateFactory = வெற்று;
    pDWriteCreateFactory = வெற்று;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#endif
}

    DWriteContext *
DWriteContext_Open(வெறுமை)
{
#ifdef DYNAMIC_DIRECTX
    எனில் (pGetUserDefaultLocaleName == வெற்று || pD2D1CreateFactory == வெற்று
	    || pDWriteCreateFactory == வெற்று)
	திரும்பு வெற்று;
#endif
    திரும்பு புதிய DWriteContext();
}

    வெறுமை
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, மாறிலி RECT *rect)
{
    எனில் (ctx != வெற்று)
	ctx->BindDC(hdc, rect);
}

    வெறுமை
DWriteContext_SetFont(DWriteContext *ctx, HFONT hFont)
{
    எனில் (ctx != வெற்று)
	ctx->SetFont(hFont);
}

    வெறுமை
DWriteContext_DrawText(
	DWriteContext *ctx,
	மாறிலி WCHAR *text,
	முழு len,
	முழு x,
	முழு y,
	முழு w,
	முழு h,
	முழு cellWidth,
	COLORREF color,
	UINT fuOptions,
	மாறிலி RECT *lprc,
	மாறிலி INT *lpDx)
{
    எனில் (ctx != வெற்று)
	ctx->DrawText(text, len, x, y, w, h, cellWidth, color,
		fuOptions, lprc, lpDx);
}

    வெறுமை
DWriteContext_FillRect(DWriteContext *ctx, மாறிலி RECT *rc, COLORREF color)
{
    எனில் (ctx != வெற்று)
	ctx->FillRect(rc, color);
}

    வெறுமை
DWrite_Context_Draw_வரி(DWriteContext *ctx, முழு x1, முழு y1, முழு x2, முழு y2,
	COLORREF color)
{
    எனில் (ctx != வெற்று)
	ctx->Draw_வரி(x1, y1, x2, y2, color);
}

    வெறுமை
DWriteContext_SetPixel(DWriteContext *ctx, முழு x, முழு y, COLORREF color)
{
    எனில் (ctx != வெற்று)
	ctx->SetPixel(x, y, color);
}

    வெறுமை
DWriteContext_Scroll(DWriteContext *ctx, முழு x, முழு y, மாறிலி RECT *rc)
{
    எனில் (ctx != வெற்று)
	ctx->Scroll(x, y, rc);
}

    வெறுமை
DWriteContext_Flush(DWriteContext *ctx)
{
    எனில் (ctx != வெற்று)
	ctx->Flush();
}

    வெறுமை
DWriteContext_Close(DWriteContext *ctx)
{
    delete ctx;
}

    வெறுமை
DWriteContext_SetRenderingParams(
	DWriteContext *ctx,
	மாறிலி DWriteRenderingParams *params)
{
    எனில் (ctx != வெற்று)
	ctx->SetRenderingParams(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    எனில் (ctx != வெற்று)
	திரும்பு ctx->GetRenderingParams(params);
    இல்லையெனில்
	திரும்பு வெற்று;
}

    வெறுமை
DWriteContext::SetFontFeatures(
	மாறிலி DWriteFontFeature *features, முழு மொத்தம்)
{
    எனில் (மொத்தம் > DWRITE_MAX_FONT_FEATURES)
	மொத்தம் = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_மொத்தம் = மொத்தம்;
    எனில் (மொத்தம் > 0 && features != வெற்று)
	memcpy(mFontFeatures, features, அளவு(DWriteFontFeature) * மொத்தம்);
}

    வெறுமை
DWriteContext_SetFontFeatures(
	DWriteContext *ctx,
	மாறிலி DWriteFontFeature *features,
	முழு மொத்தம்)
{
    எனில் (ctx != வெற்று)
	ctx->SetFontFeatures(features, மொத்தம்);
}
