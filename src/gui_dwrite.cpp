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

#నిర్వచించు WIN32_LEAN_AND_సగటు

#నిర్వచితం_కాకపోతే DYNAMIC_DIRECTX
# ఒకవేళ WINVER < 0x0600
#  error WINVER must be 0x0600 or above గమ్యం use ప్రత్యక్ష_Write(ప్రత్యక్ష_X)
# endif
#అయితే_ముగింపు

#అన్తర్భావ <windows.h>
#అన్తర్భావ <crtdbg.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <d2d1.h>
#అన్తర్భావ <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#ఒకవేళ defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define లోపల_reads(s)
# define లోపల_reads_opt(s)
# define _Maybenull_
# define బహిర్గతం_writes(s)
# define బహిర్గతం_writes_opt(s)
# define బహిర్గతం_writes_గమ్యం(x, y)
# define బహిర్గతం_writes_గమ్యం_opt(x, y)
# define _Outptr_
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#లేకపోతే
# include <dwrite.h>
#అయితే_ముగింపు

#అన్తర్భావ "gui_dwrite.h"

#నిర్వచితమైతే __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_లోపల
# define __out		SAL_బహిర్గతం
#అయితే_ముగింపు

#నిర్వచితమైతే __MINGW32__
# define వాడని __attribute__((వాడని_2))
#లేకపోతే
# define వాడని
#అయితే_ముగింపు

#ఒకవేళ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#లేకపోతే
# define FINAL
#అయితే_ముగింపు

#నిర్వచితమైతే DYNAMIC_DIRECTX
బాహ్య "C" HINSTANCE vim_ఎక్కించు_Lib(స్థిరరాశి అక్షరం *name);

రకనిర్వచనం పూర్ణాంకం (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, పూర్ణాంకం);
రకనిర్వచనం HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, స్థిరరాశి D2D1_FACTORY_OPTIONS *, శూన్య **);
రకనిర్వచనం HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

స్థిర HINSTANCE hD2D1DLL = ఖాళీ;
స్థిర HINSTANCE hDWriteDLL = ఖాళీ;

స్థిర PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = ఖాళీ;
స్థిర PD2D1CREATEFACTORY pD2D1CreateFactory = ఖాళీ;
స్థిర PDWRITECREATEFACTORY pDWriteCreateFactory = ఖాళీ;

#నిర్వచించు GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#నిర్వచించు D2D1CreateFactory		(*pD2D1CreateFactory)
#నిర్వచించు DWriteCreateFactory		(*pDWriteCreateFactory)

    స్థిర శూన్య
unload(HINSTANCE &hinst)
{
    ఒకవేళ (hinst != ఖాళీ)
    {
	FreeLibrary(hinst);
	hinst = ఖాళీ;
    }
}
#అయితే_ముగింపు // DYNAMIC_DIRECTX

మూస <తరగతి T> అంతఃవరుస శూన్య సురక్షిత_Release(T **ppT)
{
    ఒకవేళ (*ppT)
    {
	(*ppT)->Release();
	*ppT = ఖాళీ;
    }
}

    స్థిర DWRITE_PIXEL_GEOMETRY
గమ్యం_Pixel_Geometry(పూర్ణాంకం విలువ)
{
    ఎంపిక (విలువ)
    {
	అప్రమేయ:
	స్థితి 0:
	    తిరిగి DWRITE_PIXEL_GEOMETRY_FLAT;
	స్థితి 1:
	    తిరిగి DWRITE_PIXEL_GEOMETRY_RGB;
	స్థితి 2:
	    తిరిగి DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    స్థిర పూర్ణాంకం
గమ్యం_Int(DWRITE_PIXEL_GEOMETRY విలువ)
{
    ఎంపిక (విలువ)
    {
	స్థితి DWRITE_PIXEL_GEOMETRY_FLAT:
	    తిరిగి 0;
	స్థితి DWRITE_PIXEL_GEOMETRY_RGB:
	    తిరిగి 1;
	స్థితి DWRITE_PIXEL_GEOMETRY_BGR:
	    తిరిగి 2;
	అప్రమేయ:
	    తిరిగి -1;
    }
}

    స్థిర DWRITE_RENDERING_బహుళకం
గమ్యం_Rendering_బహుళకం(పూర్ణాంకం విలువ)
{
    ఎంపిక (విలువ)
    {
	అప్రమేయ:
	స్థితి 0:
	    తిరిగి DWRITE_RENDERING_బహుళకం_DEFAULT;
	స్థితి 1:
	    తిరిగి DWRITE_RENDERING_బహుళకం_ALIASED;
	స్థితి 2:
	    తిరిగి DWRITE_RENDERING_బహుళకం_CLEARTYPE_GDI_CLASSIC;
	స్థితి 3:
	    తిరిగి DWRITE_RENDERING_బహుళకం_CLEARTYPE_GDI_NATURAL;
	స్థితి 4:
	    తిరిగి DWRITE_RENDERING_బహుళకం_CLEARTYPE_NATURAL;
	స్థితి 5:
	    తిరిగి DWRITE_RENDERING_బహుళకం_CLEARTYPE_NATURAL_సౌష్ఠవ;
	స్థితి 6:
	    తిరిగి DWRITE_RENDERING_బహుళకం_OUTLINE;
    }
}

    స్థిర D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం
గమ్యం_పాఠ్యం_Antialias_బహుళకం(పూర్ణాంకం విలువ)
{
    ఎంపిక (విలువ)
    {
	అప్రమేయ:
	స్థితి 0:
	    తిరిగి D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_DEFAULT;
	స్థితి 1:
	    తిరిగి D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_CLEARTYPE;
	స్థితి 2:
	    తిరిగి D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_GRAYSCALE;
	స్థితి 3:
	    తిరిగి D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_ALIASED;
    }
}

    స్థిర పూర్ణాంకం
గమ్యం_Int(DWRITE_RENDERING_బహుళకం విలువ)
{
    ఎంపిక (విలువ)
    {
	స్థితి DWRITE_RENDERING_బహుళకం_DEFAULT:
	    తిరిగి 0;
	స్థితి DWRITE_RENDERING_బహుళకం_ALIASED:
	    తిరిగి 1;
	స్థితి DWRITE_RENDERING_బహుళకం_CLEARTYPE_GDI_CLASSIC:
	    తిరిగి 2;
	స్థితి DWRITE_RENDERING_బహుళకం_CLEARTYPE_GDI_NATURAL:
	    తిరిగి 3;
	స్థితి DWRITE_RENDERING_బహుళకం_CLEARTYPE_NATURAL:
	    తిరిగి 4;
	స్థితి DWRITE_RENDERING_బహుళకం_CLEARTYPE_NATURAL_సౌష్ఠవ:
	    తిరిగి 5;
	స్థితి DWRITE_RENDERING_బహుళకం_OUTLINE:
	    తిరిగి 6;
	అప్రమేయ:
	    తిరిగి -1;
    }
}

తరగతి FontCache {
బహిరంగ:
    నిర్మాణం అంశం_2 {
	HFONT              hFont;
	IDWrite_పాఠ్యం_Format* p_పాఠ్యం_Format;
	DWRITE_FONT_బరువు font_బరువు;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	అంశం_2() : hFont(ఖాళీ), p_పాఠ్యం_Format(ఖాళీ), fontAscent(0.0f) {}
    };

అంతరంగ:
    పూర్ణాంకం m_కొలత;
    అంశం_2 *m_వస్తువులు;

బహిరంగ:
    FontCache(పూర్ణాంకం కొలత = 2) :
	m_కొలత(కొలత),
	m_వస్తువులు(కొత్త అంశం_2[కొలత])
    {
    }

    ~FontCache()
    {
	కొరకు (పూర్ణాంకం i = 0; i < m_కొలత; ++i)
	    సురక్షిత_Release(&m_వస్తువులు[i].p_పాఠ్యం_Format);
	తొలగించు[] m_వస్తువులు;
    }

    తార్కిక get(HFONT hFont, అంశం_2 &అంశం)
    {
	పూర్ణాంకం n = కనుగొను(hFont);
	ఒకవేళ (n < 0)
	    తిరిగి అబద్ధం;
	అంశం = m_వస్తువులు[n];
	slide(n);
	తిరిగి నిజం;
    }

    శూన్య put(స్థిరరాశి అంశం_2& అంశం)
    {
	పూర్ణాంకం n = కనుగొను(అంశం.hFont);
	ఒకవేళ (n < 0)
	    n = m_కొలత - 1;
	ఒకవేళ (m_వస్తువులు[n].p_పాఠ్యం_Format != అంశం.p_పాఠ్యం_Format)
	{
	    సురక్షిత_Release(&m_వస్తువులు[n].p_పాఠ్యం_Format);
	    ఒకవేళ (అంశం.p_పాఠ్యం_Format != ఖాళీ)
		అంశం.p_పాఠ్యం_Format->కూడు_Ref();
	}
	m_వస్తువులు[n] = అంశం;
	slide(n);
    }

అంతరంగ:
    పూర్ణాంకం కనుగొను(HFONT hFont)
    {
	కొరకు (పూర్ణాంకం i = 0; i < m_కొలత; ++i)
	{
	    ఒకవేళ (m_వస్తువులు[i].hFont == hFont)
		తిరిగి i;
	}
	తిరిగి -1;
    }

    శూన్య slide(పూర్ణాంకం తదుపరి_శిఖరం)
    {
	ఒకవేళ (తదుపరి_శిఖరం == 0)
	    తిరిగి;
	అంశం_2 tmp = m_వస్తువులు[తదుపరి_శిఖరం];
	కొరకు (పూర్ణాంకం i = తదుపరి_శిఖరం - 1; i >= 0; --i)
	    m_వస్తువులు[i + 1] = m_వస్తువులు[i];
	m_వస్తువులు[0] = tmp;
    }
};

గణన Drawing_బహుళకం {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

నిర్మాణం DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_బహుళకం mDMode;
    HDC mInteropHDC;
    తార్కిక mDrawing;
    తార్కిక mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_లక్ష్యం *mRT;
    ID2_D1_Gdi_Interop_Render_లక్ష్యం *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#అయితే_ముగింపు

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_పాఠ్యం_Format *m_పాఠ్యం_Format;
    DWRITE_FONT_బరువు m_Font_బరువు;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_కొలత;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం m_పాఠ్యం_Antialias_బహుళకం;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    పూర్ణాంకం m_Font_Feature_లెక్క;

    // METHODS

    DWriteContext();

    మాయా ~DWriteContext();

    HRESULT CreateDeviceResources();

    శూన్య DiscardDeviceResources();

    HRESULT Create_పాఠ్యం_Format_From_LOGFONT(స్థిరరాశి LOGFONTW &logFont,
	    IDWrite_పాఠ్యం_Format **pp_పాఠ్యం_Format, FLOAT *pFontAscent);

    HRESULT సమితి_Font_By_LOGFONT(స్థిరరాశి LOGFONTW &logFont);

    శూన్య సమితి_Font(HFONT hFont);

    శూన్య Rebind();

    శూన్య BindDC(HDC hdc, స్థిరరాశి RECT *rect);

    HRESULT సమితి_Drawing_బహుళకం(Drawing_బహుళకం బహుళకం);

    ID2D1Brush* SolidBrush(COLORREF color);

    శూన్య Draw_పాఠ్యం(స్థిరరాశి WCHAR *పాఠ్యం, పూర్ణాంకం len,
	పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h, పూర్ణాంకం cell_వెడల్పు_2, COLORREF color,
	UINT fuOptions, స్థిరరాశి RECT *lprc, స్థిరరాశి INT *lpDx);

    శూన్య FillRect(స్థిరరాశి RECT *rc, COLORREF color);

    శూన్య Draw_పంక్తి(పూర్ణాంకం x1, పూర్ణాంకం y1, పూర్ణాంకం x2, పూర్ణాంకం y2, COLORREF color);

    శూన్య సమితి_Pixel(పూర్ణాంకం x, పూర్ణాంకం y, COLORREF color);

    శూన్య Scroll(పూర్ణాంకం x, పూర్ణాంకం y, స్థిరరాశి RECT *rc);

    శూన్య Flush();

    శూన్య సమితి_Rendering_Params(
	    స్థిరరాశి DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    శూన్య సమితి_Font_Features(స్థిరరాశి DWriteFontFeature *features, పూర్ణాంకం లెక్క);
};

తరగతి Adjusted_Glyph_నిరంతరం : బహిరంగ DWRITE_GLYPH_నిరంతరం
{
అంతరంగ:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

బహిరంగ:
    Adjusted_Glyph_నిరంతరం(
	    స్థిరరాశి DWRITE_GLYPH_నిరంతరం *glyph_నిరంతరం,
	    FLOAT cell_వెడల్పు_2,
	    FLOAT &accum,
	    స్థిరరాశి INT *lpDx,
	    పూర్ణాంకం *glyph_సూచీ,
	    పూర్ణాంకం cch_పాఠ్యం) :
	DWRITE_GLYPH_నిరంతరం(*glyph_నిరంతరం),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(కొత్త FLOAT[glyph_నిరంతరం->glyph_లెక్క])
    {
	ప్రతిజ్ఞా(cell_వెడల్పు_2 != 0.0f);
	కొరకు (UINT32 i = 0; i < glyph_నిరంతరం->glyph_లెక్క; ++i)
	{
	    FLOAT orig = glyph_నిరంతరం->glyphAdvances[i];
	    FLOAT adjusted;

	    ఒకవేళ (*glyph_సూచీ < cch_పాఠ్యం)
	    {
		adjusted = FLOAT(lpDx[*glyph_సూచీ]);
		(*glyph_సూచీ)++;
	    }
	    లేకపోతే
	    {
		adjusted = adjust_గమ్యం_Cell(orig, cell_వెడల్పు_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_నిరంతరం()
    {
	mAccum += mDelta;
	తొలగించు[] mAdjustedAdvances;
    }

    స్థిర FLOAT adjust_గమ్యం_Cell(FLOAT విలువ, FLOAT cell_వెడల్పు_2)
    {
	పూర్ణాంకం cell_లెక్క = పూర్ణాంకం(floor(విలువ / cell_వెడల్పు_2 + 0.5f));
	ఒకవేళ (cell_లెక్క < 1)
	    cell_లెక్క = 1;
	తిరిగి cell_లెక్క * cell_వెడల్పు_2;
    }
};

నిర్మాణం పాఠ్యం_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_వెడల్పు_2;
    స్థిరరాశి INT *lpDx;
    పూర్ణాంకం cch_పాఠ్యం;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT స్థానభ్రంశం_X;
    పూర్ణాంకం glyph_సూచీ;
};

తరగతి పాఠ్యం_Renderer FINAL : బహిరంగ IDWrite_పాఠ్యం_Renderer
{
బహిరంగ:
    పాఠ్యం_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_లెక్క(0),
	pDWC_(pDWC)
    {
	కూడు_Ref();
    }

    // add "virtual" to avoid a compiler warning
    మాయా ~పాఠ్యం_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	తిరిగి S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	__out DWRITE_మాతృక* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_మాతృక_3_X2_F*>(transform));
	తిరిగి S_OK;
    }

    IFACEMETHOD(Get_Pixels_చొప్పున_Dip)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	__out FLOAT* pixels_చొప్పున_Dip)
    {
	దశాంశం dpiX, వాడని_2;
	pDWC_->mRT->GetDpi(&dpiX, &వాడని_2);
	*pixels_చొప్పున_Dip = dpiX / 96.0f;
	తిరిగి S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	FLOAT baselineOriginX వాడని,
	FLOAT baselineOriginY వాడని,
	__in DWRITE_UNDERLINE స్థిరరాశి* underline వాడని,
	IUnknown* clientDrawingEffect వాడని)
    {
	తిరిగి E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	FLOAT baselineOriginX వాడని,
	FLOAT baselineOriginY వాడని,
	__in DWRITE_STRIKETHROUGH స్థిరరాశి* strikethrough వాడని,
	IUnknown* clientDrawingEffect వాడని)
    {
	తిరిగి E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull శూన్య* clientDrawingContext వాడని,
	FLOAT originX వాడని,
	FLOAT originY వాడని,
	IDWriteInlineObject* inlineObject వాడని,
	BOOL isSideways వాడని,
	BOOL is_కుడి_గమ్యం_ఎడమ వాడని,
	IUnknown* clientDrawingEffect వాడని)
    {
	తిరిగి E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_నిరంతరం)(
	__maybenull శూన్య* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_బహుళకం measuring_బహుళకం వాడని,
	__in DWRITE_GLYPH_నిరంతరం స్థిరరాశి* glyph_నిరంతరం,
	__in DWRITE_GLYPH_నిరంతరం_DESCRIPTION స్థిరరాశి* glyph_నిరంతరం_Description వాడని,
	IUnknown* clientDrawingEffect వాడని)
    {
	పాఠ్యం_Renderer_Context *context =
	    reinterpret_cast<పాఠ్యం_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_నిరంతరం adjusted_Glyph_నిరంతరం(glyph_నిరంతరం, context->cell_వెడల్పు_2,
		context->స్థానభ్రంశం_X, context->lpDx, &context->glyph_సూచీ,
		context->cch_పాఠ్యం);

#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
	ఒకవేళ (pDWC_->mDWriteFactory2 != ఖాళీ)
	{
	    IDWrite_Color_Glyph_నిరంతరం_Enumerator *enumerator = ఖాళీ;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_నిరంతరం(
		baselineOriginX + context->స్థానభ్రంశం_X,
		baselineOriginY,
		&adjusted_Glyph_నిరంతరం,
		ఖాళీ,
		DWRITE_MEASURING_బహుళకం_GDI_NATURAL,
		ఖాళీ,
		0,
		&enumerator);
	    ఒకవేళ (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_నిరంతరం = TRUE;
		enumerator->Move_తదుపరి(&has_నిరంతరం);
		అయితే (has_నిరంతరం)
		{
		    స్థిరరాశి DWRITE_COLOR_GLYPH_నిరంతరం* color_Glyph_నిరంతరం;
		    enumerator->Get_Current_నిరంతరం(&color_Glyph_నిరంతరం);

		    pDWC_->mBrush->సమితి_Color(color_Glyph_నిరంతరం->నిరంతరం_Color);
		    pDWC_->mRT->Draw_Glyph_నిరంతరం(
			    D2D1::Point2F(
				color_Glyph_నిరంతరం->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_నిరంతరం->glyph_నిరంతరం,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_బహుళకం_NATURAL);
		    enumerator->Move_తదుపరి(&has_నిరంతరం);
		}
		సురక్షిత_Release(&enumerator);
		తిరిగి S_OK;
	    }
	}
#అయితే_ముగింపు

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_నిరంతరం(
		D2D1::Point2F(
		    baselineOriginX + context->స్థానభ్రంశం_X,
		    fixedBaselineY),
		&adjusted_Glyph_నిరంతరం,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_బహుళకం_NATURAL);
	తిరిగి S_OK;
    }

బహిరంగ:
    IFACEMETHOD_(గుర్తులేని దీర్ఘ, కూడు_Ref) ()
    {
	తిరిగి InterlockedIncrement(&c_Ref_లెక్క);
    }

    IFACEMETHOD_(గుర్తులేని దీర్ఘ, Release) ()
    {
	దీర్ఘ new_లెక్క_2 = InterlockedDecrement(&c_Ref_లెక్క);

	ఒకవేళ (new_లెక్క_2 == 0)
	{
	    తొలగించు ఇది;
	    తిరిగి 0;
	}
	తిరిగి new_లెక్క_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID స్థిరరాశి& riid,
	శూన్య** ppvObject)
    {
	ఒకవేళ (__uuidof(IDWrite_పాఠ్యం_Renderer) == riid)
	{
	    *ppvObject = ఇది;
	}
	లేకపోతే ఒకవేళ (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = ఇది;
	}
	లేకపోతే ఒకవేళ (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = ఇది;
	}
	లేకపోతే
	{
	    *ppvObject = ఖాళీ;
	    తిరిగి E_FAIL;
	}

	తిరిగి S_OK;
    }

అంతరంగ:
    దీర్ఘ c_Ref_లెక్క;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(ఖాళీ),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(ఖాళీ),
    mDrawing(అబద్ధం),
    mFallbackDC(అబద్ధం),
    mD2D1Factory(ఖాళీ),
    mRT(ఖాళీ),
    mGDIRT(ఖాళీ),
    mBrush(ఖాళీ),
    mBitmap(ఖాళీ),
    mDWriteFactory(ఖాళీ),
#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(ఖాళీ),
#అయితే_ముగింపు
    mGdiInterop(ఖాళీ),
    mRenderingParams(ఖాళీ),
    mFontCache(8),
    m_పాఠ్యం_Format(ఖాళీ),
    m_Font_బరువు(DWRITE_FONT_బరువు_సాధారణ),
    mFontStyle(DWRITE_FONT_STYLE_సాధారణ),
    m_Font_కొలత(0.0f),
    mFontAscent(0.0f),
    m_పాఠ్యం_Antialias_బహుళకం(D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_DEFAULT),
    m_Font_Feature_లెక్క(0)
{
    Zero_జ్ఞాపకం(mFontFeatures, పరిమాణం(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), ఖాళీ,
	    reinterpret_cast<శూన్య**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
    ఒకవేళ (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#అయితే_ముగింపు

    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->CreateRenderingParams(&mRenderingParams);
	_RPT2(_CRT_WARN, "CreateRenderingParams: hr=%p p=%p\n", hr,
		mRenderingParams);
    }
}

DWriteContext::~DWriteContext()
{
    సురక్షిత_Release(&m_పాఠ్యం_Format);
    సురక్షిత_Release(&mRenderingParams);
    సురక్షిత_Release(&mGdiInterop);
    సురక్షిత_Release(&mDWriteFactory);
#నిర్వచితమైతే FEAT_DIRECTX_COLOR_EMOJI
    సురక్షిత_Release(&mDWriteFactory2);
#అయితే_ముగింపు
    సురక్షిత_Release(&mBitmap);
    సురక్షిత_Release(&mBrush);
    సురక్షిత_Release(&mGDIRT);
    సురక్షిత_Release(&mRT);
    సురక్షిత_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    ఒకవేళ (mRT != ఖాళీ)
	తిరిగి S_OK;

    D2_D1_RENDER_లక్ష్యం_PROPERTIES props = {
	D2_D1_RENDER_లక్ష్యం_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_బహుళకం_IGNORE },
	0, 0,
	D2_D1_RENDER_లక్ష్యం_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_లక్ష్యం(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    ఒకవేళ (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_లక్ష్యం),
		reinterpret_cast<శూన్య**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    ఒకవేళ (SUCCEEDED(hr))
	Rebind();

    తిరిగి hr;
}

    శూన్య
DWriteContext::DiscardDeviceResources()
{
    సురక్షిత_Release(&mBitmap);
    సురక్షిత_Release(&mBrush);
    సురక్షిత_Release(&mGDIRT);
    సురక్షిత_Release(&mRT);
}

    HRESULT
DWriteContext::Create_పాఠ్యం_Format_From_LOGFONT(స్థిరరాశి LOGFONTW &logFont,
	IDWrite_పాఠ్యం_Format **pp_పాఠ్యం_Format, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWrite_పాఠ్యం_Format *p_పాఠ్యం_Format = ఖాళీ;

    IDWriteFont *font = ఖాళీ;
    IDWriteFontFamily *fontFamily = ఖాళీ;
    IDWrite_Localized_అక్షరశ్రేణులు *localized_Family_పేర్లు = ఖాళీ;
    దశాంశం font_కొలత = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = fontFamily->Get_Family_పేర్లు(&localized_Family_పేర్లు);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    ఒకవేళ (SUCCEEDED(hr))
    {
	hr = localized_Family_పేర్లు->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    ఒకవేళ (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_కొలత = దశాంశం(logFont.lf_ఎత్తు);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	ఒకవేళ (font_కొలత < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_కొలత = -font_కొలత;
	}
	లేకపోతే ఒకవేళ (font_కొలత > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_కొలత = font_కొలత * దశాంశం(fontMetrics.design_Units_చొప్పున_Em)
		/ దశాంశం(fontMetrics.ascent + fontMetrics.descent + fontMetrics.పంక్తి_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_నిడివి];
    ఒకవేళ (SUCCEEDED(hr))
    {
	ఒకవేళ (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_నిడివి) == 0)
	    hr = HRESULT_FROM_WIN32(Get_చివరి_Error());
    }

    ఒకవేళ (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_పాఠ్యం_Format(
		familyName,
		ఖాళీ, // no custom font collection
		font->Get_బరువు(),
		font->GetStyle(),
		font->GetStretch(),
		font_కొలత,
		localeName,
		&p_పాఠ్యం_Format);
    }

    ఒకవేళ (SUCCEEDED(hr))
	hr = p_పాఠ్యం_Format->సమితి_పాఠ్యం_Alignment(DWRITE_పాఠ్యం_ALIGNMENT_LEADING);

    ఒకవేళ (SUCCEEDED(hr))
	hr = p_పాఠ్యం_Format->సమితి_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    ఒకవేళ (SUCCEEDED(hr))
	hr = p_పాఠ్యం_Format->సమితి_మాట_Wrapping(DWRITE_మాట_WRAPPING_NO_WRAP);

    సురక్షిత_Release(&localized_Family_పేర్లు);
    సురక్షిత_Release(&fontFamily);
    సురక్షిత_Release(&font);

    ఒకవేళ (SUCCEEDED(hr))
    {
	*pp_పాఠ్యం_Format = p_పాఠ్యం_Format;
	ఒకవేళ (pFontAscent != ఖాళీ && fontMetrics.design_Units_చొప్పున_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT మొత్తం_ఎత్తు = దశాంశం(fontMetrics.ascent + fontMetrics.descent);
	    ఒకవేళ (మొత్తం_ఎత్తు != 0.0f)
		*pFontAscent = దశాంశం(fontMetrics.ascent) / మొత్తం_ఎత్తు;
	    లేకపోతే
		*pFontAscent = 0.83f; // fallback
	}
    }
    లేకపోతే
	సురక్షిత_Release(&p_పాఠ్యం_Format);

    తిరిగి hr;
}

    HRESULT
DWriteContext::సమితి_Font_By_LOGFONT(స్థిరరాశి LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_పాఠ్యం_Format *p_పాఠ్యం_Format = ఖాళీ;
    FLOAT fontAscent = 0.0f;

    hr = Create_పాఠ్యం_Format_From_LOGFONT(logFont, &p_పాఠ్యం_Format, &fontAscent);

    ఒకవేళ (SUCCEEDED(hr))
    {
	సురక్షిత_Release(&m_పాఠ్యం_Format);
	m_పాఠ్యం_Format = p_పాఠ్యం_Format;
	m_Font_బరువు = static_cast<DWRITE_FONT_బరువు>(logFont.lf_బరువు);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_సాధారణ;
	mFontAscent = fontAscent;
    }

    తిరిగి hr;
}

    శూన్య
DWriteContext::సమితి_Font(HFONT hFont)
{
    FontCache::అంశం_2 అంశం;
    ఒకవేళ (mFontCache.get(hFont, అంశం))
    {
	ఒకవేళ (అంశం.p_పాఠ్యం_Format != ఖాళీ)
	{
	    అంశం.p_పాఠ్యం_Format->కూడు_Ref();
	    సురక్షిత_Release(&m_పాఠ్యం_Format);
	    m_పాఠ్యం_Format = అంశం.p_పాఠ్యం_Format;
	    m_Font_బరువు = అంశం.font_బరువు;
	    mFontStyle = అంశం.fontStyle;
	    mFontAscent = అంశం.fontAscent;
	    mFallbackDC = అబద్ధం;
	}
	లేకపోతే
	    mFallbackDC = నిజం;
	తిరిగి;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    ఒకవేళ (GetObjectW(hFont, పరిమాణం(lf), &lf))
	hr = సమితి_Font_By_LOGFONT(lf);

    అంశం.hFont = hFont;
    ఒకవేళ (SUCCEEDED(hr))
    {
	అంశం.p_పాఠ్యం_Format = m_పాఠ్యం_Format;
	అంశం.font_బరువు = m_Font_బరువు;
	అంశం.fontStyle = mFontStyle;
	అంశం.fontAscent = mFontAscent;
	mFallbackDC = అబద్ధం;
    }
    లేకపోతే
	mFallbackDC = నిజం;
    mFontCache.put(అంశం);
}

    శూన్య
DWriteContext::Rebind()
{
    సురక్షిత_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->సమితి_Transform(D2D1::తటస్థం_మాతృక());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_బహుళకం_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::కొలత_U(mBindRect.కుడి - mBindRect.ఎడమ,
		mBindRect.bottom - mBindRect.శిఖరం),
	    props, &mBitmap);
}

    శూన్య
DWriteContext::BindDC(HDC hdc, స్థిరరాశి RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    ఒకవేళ (mRT == ఖాళీ)
	CreateDeviceResources();
    లేకపోతే
    {
	Flush();
	Rebind();
    }
}

బాహ్య "C" శూన్య redraw_later_clear(శూన్య);

    HRESULT
DWriteContext::సమితి_Drawing_బహుళకం(Drawing_బహుళకం బహుళకం)
{
    HRESULT hr = S_OK;

    ఎంపిక (బహుళకం)
    {
	అప్రమేయ:
	స్థితి DM_GDI:
	    ఒకవేళ (mInteropHDC != ఖాళీ)
	    {
		mGDIRT->ReleaseDC(ఖాళీ);
		mInteropHDC = ఖాళీ;
	    }
	    ఒకవేళ (mDrawing)
	    {
		hr = mRT->చివర_Draw();
		ఒకవేళ (hr == (HRESULT)D2_DERR_RECREATE_లక్ష్యం)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = అబద్ధం;
	    }
	    ఆపు;

	స్థితి DM_DIRECTX:
	    ఒకవేళ (mInteropHDC != ఖాళీ)
	    {
		mGDIRT->ReleaseDC(ఖాళీ);
		mInteropHDC = ఖాళీ;
	    }
	    లేకపోతే ఒకవేళ (mDrawing == అబద్ధం)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = నిజం;
	    }
	    ఆపు;

	స్థితి DM_INTEROP:
	    ఒకవేళ (mDrawing == అబద్ధం)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = నిజం;
	    }
	    ఒకవేళ (mInteropHDC == ఖాళీ)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_బహుళకం_COPY, &mInteropHDC);
	    ఆపు;
    }
    mDMode = బహుళకం;
    తిరిగి hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->సమితి_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    తిరిగి mBrush;
}

    శూన్య
DWriteContext::Draw_పాఠ్యం(స్థిరరాశి WCHAR *పాఠ్యం, పూర్ణాంకం len,
	పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h, పూర్ణాంకం cell_వెడల్పు_2, COLORREF color,
	UINT fuOptions, స్థిరరాశి RECT *lprc, స్థిరరాశి INT *lpDx)
{
    ఒకవేళ (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = సమితి_Drawing_బహుళకం(DM_INTEROP);
	ఒకవేళ (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::సమితి_పాఠ్యం_Color(mInteropHDC, color);
	    ::సమితి_Bk_బహుళకం(mInteropHDC, ::Get_Bk_బహుళకం(mHDC));
	    ::Ext_పాఠ్యం_బహిర్గతం_W(mInteropHDC, x, y, fuOptions, lprc, పాఠ్యం, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	తిరిగి;
    }

    HRESULT hr;
    IDWrite_పాఠ్యం_Layout *పాఠ్యం_Layout = ఖాళీ;

    సమితి_Drawing_బహుళకం(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    ఒకవేళ ((fuOptions & ETO_CLIPPED) && lprc != ఖాళీ)
    {
	mRT->నెట్టు_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->ఎడమ), FLOAT(lprc->శిఖరం),
			FLOAT(lprc->కుడి), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_బహుళకం_ALIASED);
    }

    hr = mDWriteFactory->Create_పాఠ్యం_Layout(పాఠ్యం, len, m_పాఠ్యం_Format,
	    FLOAT(w), FLOAT(h), &పాఠ్యం_Layout);

    ఒకవేళ (SUCCEEDED(hr))
    {
	DWRITE_పాఠ్యం_RANGE పాఠ్యం_Range = { 0, UINT32(len) };
	పాఠ్యం_Layout->సమితి_Font_బరువు(m_Font_బరువు, పాఠ్యం_Range);
	పాఠ్యం_Layout->సమితి_Font_Style(mFontStyle, పాఠ్యం_Range);

	ఒకవేళ (m_Font_Feature_లెక్క > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    స్థిర స్థిరరాశి DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_ట్యాగ్_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_ట్యాగ్_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ట్యాగ్_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ట్యాగ్_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ట్యాగ్_KERNING, 1 },
	    };
	    స్థిర స్థిరరాశి పూర్ణాంకం అంకె_Defaults = పరిమాణం(defaultFeatures)
		/ పరిమాణం(defaultFeatures[0]);

	    IDWriteTypography *typography = ఖాళీ;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    ఒకవేళ (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		కొరకు (పూర్ణాంకం d = 0; d < అంకె_Defaults; ++d)
		{
		    పూర్ణాంకం overridden = 0;
		    కొరకు (పూర్ణాంకం u = 0; u < m_Font_Feature_లెక్క; ++u)
		    {
			ఒకవేళ ((DWRITE_FONT_FEATURE_ట్యాగ్)mFontFeatures[u].ట్యాగ్
				== defaultFeatures[d].name_ట్యాగ్)
			{
			    overridden = 1;
			    ఆపు;
			}
		    }
		    ఒకవేళ (!overridden)
			typography->కూడు_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		కొరకు (పూర్ణాంకం i = 0; i < m_Font_Feature_లెక్క; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_ట్యాగ్)mFontFeatures[i].ట్యాగ్,
			mFontFeatures[i].parameter
		    };
		    typography->కూడు_Font_Feature(ff);
		}
		పాఠ్యం_Layout->సమితి_Typography(typography, పాఠ్యం_Range);
		సురక్షిత_Release(&typography);
	    }
	}

	// Calculate baseline using font ascent from font metrics.
	// Do NOT use GetLineMetrics() because it returns different values
	// depending on text content (e.g., when CJK characters trigger
	// font fallback, the metrics change).
	// Use the pre-calculated font ascent for all text to prevent
	// vertical shifts during redraw.
	FLOAT baselineY = roundf(FLOAT(y) + FLOAT(h) * mFontAscent + 0.5);

	పాఠ్యం_Renderer renderer(ఇది);
	పాఠ్యం_Renderer_Context context = { color, FLOAT(cell_వెడల్పు_2), lpDx, len,
		baselineY, 0.0f, 0 };

	పాఠ్యం_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    సురక్షిత_Release(&పాఠ్యం_Layout);

    ఒకవేళ ((fuOptions & ETO_CLIPPED) && lprc != ఖాళీ)
	mRT->తీయి_Axis_Aligned_Clip();
}

    శూన్య
DWriteContext::FillRect(స్థిరరాశి RECT *rc, COLORREF color)
{
    ఒకవేళ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    లేకపోతే
    {
	సమితి_Drawing_బహుళకం(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->ఎడమ), FLOAT(rc->శిఖరం),
		    FLOAT(rc->కుడి), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    శూన్య
DWriteContext::Draw_పంక్తి(పూర్ణాంకం x1, పూర్ణాంకం y1, పూర్ణాంకం x2, పూర్ణాంకం y2, COLORREF color)
{
    ఒకవేళ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_గమ్యం_Ex(mInteropHDC, x1, y1, ఖాళీ);
	::పంక్తి_గమ్యం(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    లేకపోతే
    {
	సమితి_Drawing_బహుళకం(DM_DIRECTX);
	mRT->Draw_పంక్తి(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    శూన్య
DWriteContext::సమితి_Pixel(పూర్ణాంకం x, పూర్ణాంకం y, COLORREF color)
{
    ఒకవేళ (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::సమితి_Pixel(mInteropHDC, x, y, color);
    }
    లేకపోతే
    {
	సమితి_Drawing_బహుళకం(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_పంక్తి(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    శూన్య
DWriteContext::Scroll(పూర్ణాంకం x, పూర్ణాంకం y, స్థిరరాశి RECT *rc)
{
    సమితి_Drawing_బహుళకం(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_బిందువు_2_U dest_బిందువు;
    ఒకవేళ (x >= 0)
    {
	srcRect.ఎడమ = rc->ఎడమ;
	srcRect.కుడి = rc->కుడి - x;
	dest_బిందువు.x = rc->ఎడమ + x;
    }
    లేకపోతే
    {
	srcRect.ఎడమ = rc->ఎడమ - x;
	srcRect.కుడి = rc->కుడి;
	dest_బిందువు.x = rc->ఎడమ;
    }
    ఒకవేళ (y >= 0)
    {
	srcRect.శిఖరం = rc->శిఖరం;
	srcRect.bottom = rc->bottom - y;
	dest_బిందువు.y = rc->శిఖరం + y;
    }
    లేకపోతే
    {
	srcRect.శిఖరం = rc->శిఖరం - y;
	srcRect.bottom = rc->bottom;
	dest_బిందువు.y = rc->శిఖరం;
    }
    mBitmap->Copy_From_Render_లక్ష్యం(&dest_బిందువు, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_బిందువు.x), FLOAT(dest_బిందువు.y),
	    FLOAT(dest_బిందువు.x + srcRect.కుడి - srcRect.ఎడమ),
	    FLOAT(dest_బిందువు.y + srcRect.bottom - srcRect.శిఖరం)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_బహుళకం_సమీప_NEIGHBOR, destRect);
}

    శూన్య
DWriteContext::Flush()
{
    సమితి_Drawing_బహుళకం(DM_GDI);
}

    శూన్య
DWriteContext::సమితి_Rendering_Params(
	స్థిరరాశి DWriteRenderingParams *params)
{
    ఒకవేళ (mDWriteFactory == ఖాళీ)
	తిరిగి;

    IDWriteRenderingParams *renderingParams = ఖాళీ;
    D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం పాఠ్యం_Antialias_బహుళకం =
	D2_D1_పాఠ్యం_ANTIALIAS_బహుళకం_DEFAULT;
    HRESULT hr;
    ఒకవేళ (params != ఖాళీ)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		గమ్యం_Pixel_Geometry(params->pixelGeometry),
		గమ్యం_Rendering_బహుళకం(params->rendering_బహుళకం), &renderingParams);
	పాఠ్యం_Antialias_బహుళకం = గమ్యం_పాఠ్యం_Antialias_బహుళకం(params->పాఠ్యం_Antialias_బహుళకం);
    }
    లేకపోతే
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    ఒకవేళ (SUCCEEDED(hr) && renderingParams != ఖాళీ)
    {
	సురక్షిత_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_పాఠ్యం_Antialias_బహుళకం = పాఠ్యం_Antialias_బహుళకం;

	Flush();
	mRT->సమితి_పాఠ్యం_Rendering_Params(mRenderingParams);
	mRT->సమితి_పాఠ్యం_Antialias_బహుళకం(m_పాఠ్యం_Antialias_బహుళకం);
    }
}

    DWriteRenderingParams *
DWriteContext::GetRenderingParams(
	DWriteRenderingParams *params)
{
    ఒకవేళ (params != ఖాళీ && mRenderingParams != ఖాళీ)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = గమ్యం_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_బహుళకం = గమ్యం_Int(mRenderingParams->Get_Rendering_బహుళకం());
	params->పాఠ్యం_Antialias_బహుళకం = m_పాఠ్యం_Antialias_బహుళకం;
    }
    తిరిగి params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    శూన్య
DWrite_Init(శూన్య)
{
#నిర్వచితమైతే DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_ఎక్కించు_Lib("d2d1.dll");
    hDWriteDLL = vim_ఎక్కించు_Lib("dwrite.dll");
    ఒకవేళ (hD2D1DLL == ఖాళీ || hDWriteDLL == ఖాళీ)
    {
	DWrite_Final();
	తిరిగి;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_చిరునామా(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_చిరునామా(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_చిరునామా(hDWriteDLL,
	    "DWriteCreateFactory");
#అయితే_ముగింపు
}

    శూన్య
DWrite_Final(శూన్య)
{
#నిర్వచితమైతే DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = ఖాళీ;
    pD2D1CreateFactory = ఖాళీ;
    pDWriteCreateFactory = ఖాళీ;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#అయితే_ముగింపు
}

    DWriteContext *
DWriteContext_Open(శూన్య)
{
#నిర్వచితమైతే DYNAMIC_DIRECTX
    ఒకవేళ (pGetUserDefaultLocaleName == ఖాళీ || pD2D1CreateFactory == ఖాళీ
	    || pDWriteCreateFactory == ఖాళీ)
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    తిరిగి కొత్త DWriteContext();
}

    శూన్య
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, స్థిరరాశి RECT *rect)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->BindDC(hdc, rect);
}

    శూన్య
DWrite_Context_సమితి_Font(DWriteContext *ctx, HFONT hFont)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->సమితి_Font(hFont);
}

    శూన్య
DWrite_Context_Draw_పాఠ్యం(
	DWriteContext *ctx,
	స్థిరరాశి WCHAR *పాఠ్యం,
	పూర్ణాంకం len,
	పూర్ణాంకం x,
	పూర్ణాంకం y,
	పూర్ణాంకం w,
	పూర్ణాంకం h,
	పూర్ణాంకం cell_వెడల్పు_2,
	COLORREF color,
	UINT fuOptions,
	స్థిరరాశి RECT *lprc,
	స్థిరరాశి INT *lpDx)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->Draw_పాఠ్యం(పాఠ్యం, len, x, y, w, h, cell_వెడల్పు_2, color,
		fuOptions, lprc, lpDx);
}

    శూన్య
DWriteContext_FillRect(DWriteContext *ctx, స్థిరరాశి RECT *rc, COLORREF color)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->FillRect(rc, color);
}

    శూన్య
DWrite_Context_Draw_పంక్తి(DWriteContext *ctx, పూర్ణాంకం x1, పూర్ణాంకం y1, పూర్ణాంకం x2, పూర్ణాంకం y2,
	COLORREF color)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->Draw_పంక్తి(x1, y1, x2, y2, color);
}

    శూన్య
DWrite_Context_సమితి_Pixel(DWriteContext *ctx, పూర్ణాంకం x, పూర్ణాంకం y, COLORREF color)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->సమితి_Pixel(x, y, color);
}

    శూన్య
DWriteContext_Scroll(DWriteContext *ctx, పూర్ణాంకం x, పూర్ణాంకం y, స్థిరరాశి RECT *rc)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->Scroll(x, y, rc);
}

    శూన్య
DWriteContext_Flush(DWriteContext *ctx)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->Flush();
}

    శూన్య
DWriteContext_Close(DWriteContext *ctx)
{
    తొలగించు ctx;
}

    శూన్య
DWrite_Context_సమితి_Rendering_Params(
	DWriteContext *ctx,
	స్థిరరాశి DWriteRenderingParams *params)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->సమితి_Rendering_Params(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    ఒకవేళ (ctx != ఖాళీ)
	తిరిగి ctx->GetRenderingParams(params);
    లేకపోతే
	తిరిగి ఖాళీ;
}

    శూన్య
DWriteContext::సమితి_Font_Features(
	స్థిరరాశి DWriteFontFeature *features, పూర్ణాంకం లెక్క)
{
    ఒకవేళ (లెక్క > DWRITE_MAX_FONT_FEATURES)
	లెక్క = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_లెక్క = లెక్క;
    ఒకవేళ (లెక్క > 0 && features != ఖాళీ)
	జ్ఞాపకం_నకలు(mFontFeatures, features, పరిమాణం(DWriteFontFeature) * లెక్క);
}

    శూన్య
DWrite_Context_సమితి_Font_Features(
	DWriteContext *ctx,
	స్థిరరాశి DWriteFontFeature *features,
	పూర్ణాంకం లెక్క)
{
    ఒకవేళ (ctx != ఖాళీ)
	ctx->సమితి_Font_Features(features, లెక్క);
}
