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

#تعریف_کړه WIN32_LEAN_AND_منځنۍ

#که_نه_تعریف_شوی DYNAMIC_DIRECTX
# إذا WINVER < 0x0600
#  error WINVER must be 0x0600 or above منزل use مستقیم_Write(مستقیم_X)
# endif
#د_که_پای

#ضمن <windows.h>
#ضمن <crtdbg.h>
#ضمن <assert.h>
#ضمن <math.h>
#ضمن <d2d1.h>
#ضمن <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#إذا defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define دننه_reads(s)
# define دننه_reads_opt(s)
# define _Maybenull_
# define بهر_writes(s)
# define بهر_writes_opt(s)
# define بهر_writes_منزل(x, y)
# define بهر_writes_منزل_opt(x, y)
# define _Outptr_
#د_که_پای

#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#وإلا
# include <dwrite.h>
#د_که_پای

#ضمن "gui_dwrite.h"

#که_تعریف_شوی __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_دننه
# define __out		SAL_بهر
#د_که_پای

#که_تعریف_شوی __MINGW32__
# define ناکارول_شوی __attribute__((ناکارول_شوی_2))
#وإلا
# define ناکارول_شوی
#د_که_پای

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#د_که_پای

#که_تعریف_شوی DYNAMIC_DIRECTX
خارجي "C" HINSTANCE vim_بارول_Lib(ثابت حرف *name);

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

#تعریف_کړه GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#تعریف_کړه D2D1CreateFactory		(*pD2D1CreateFactory)
#تعریف_کړه DWriteCreateFactory		(*pDWriteCreateFactory)

    ساكن فراغ
unload(HINSTANCE &hinst)
{
    إذا (hinst != عدم)
    {
	FreeLibrary(hinst);
	hinst = عدم;
    }
}
#د_که_پای // DYNAMIC_DIRECTX

کينډۍ <صنف T> مضمن فراغ خوندي_Release(T **ppT)
{
    إذا (*ppT)
    {
	(*ppT)->Release();
	*ppT = عدم;
    }
}

    ساكن DWRITE_PIXEL_GEOMETRY
منزل_Pixel_Geometry(صحيح ارزښت)
{
    بدل (ارزښت)
    {
	افتراضي:
	حالة 0:
	    أرجع DWRITE_PIXEL_GEOMETRY_FLAT;
	حالة 1:
	    أرجع DWRITE_PIXEL_GEOMETRY_RGB;
	حالة 2:
	    أرجع DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ساكن صحيح
منزل_Int(DWRITE_PIXEL_GEOMETRY ارزښت)
{
    بدل (ارزښت)
    {
	حالة DWRITE_PIXEL_GEOMETRY_FLAT:
	    أرجع 0;
	حالة DWRITE_PIXEL_GEOMETRY_RGB:
	    أرجع 1;
	حالة DWRITE_PIXEL_GEOMETRY_BGR:
	    أرجع 2;
	افتراضي:
	    أرجع -1;
    }
}

    ساكن DWRITE_RENDERING_ډېرپېښ
منزل_Rendering_ډېرپېښ(صحيح ارزښت)
{
    بدل (ارزښت)
    {
	افتراضي:
	حالة 0:
	    أرجع DWRITE_RENDERING_ډېرپېښ_DEFAULT;
	حالة 1:
	    أرجع DWRITE_RENDERING_ډېرپېښ_ALIASED;
	حالة 2:
	    أرجع DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_GDI_CLASSIC;
	حالة 3:
	    أرجع DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_GDI_NATURAL;
	حالة 4:
	    أرجع DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_NATURAL;
	حالة 5:
	    أرجع DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_NATURAL_متناظر;
	حالة 6:
	    أرجع DWRITE_RENDERING_ډېرپېښ_OUTLINE;
    }
}

    ساكن D2_D1_متن_ANTIALIAS_ډېرپېښ
منزل_متن_Antialias_ډېرپېښ(صحيح ارزښت)
{
    بدل (ارزښت)
    {
	افتراضي:
	حالة 0:
	    أرجع D2_D1_متن_ANTIALIAS_ډېرپېښ_DEFAULT;
	حالة 1:
	    أرجع D2_D1_متن_ANTIALIAS_ډېرپېښ_CLEARTYPE;
	حالة 2:
	    أرجع D2_D1_متن_ANTIALIAS_ډېرپېښ_GRAYSCALE;
	حالة 3:
	    أرجع D2_D1_متن_ANTIALIAS_ډېرپېښ_ALIASED;
    }
}

    ساكن صحيح
منزل_Int(DWRITE_RENDERING_ډېرپېښ ارزښت)
{
    بدل (ارزښت)
    {
	حالة DWRITE_RENDERING_ډېرپېښ_DEFAULT:
	    أرجع 0;
	حالة DWRITE_RENDERING_ډېرپېښ_ALIASED:
	    أرجع 1;
	حالة DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_GDI_CLASSIC:
	    أرجع 2;
	حالة DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_GDI_NATURAL:
	    أرجع 3;
	حالة DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_NATURAL:
	    أرجع 4;
	حالة DWRITE_RENDERING_ډېرپېښ_CLEARTYPE_NATURAL_متناظر:
	    أرجع 5;
	حالة DWRITE_RENDERING_ډېرپېښ_OUTLINE:
	    أرجع 6;
	افتراضي:
	    أرجع -1;
    }
}

صنف FontCache {
عام:
    بنية توکی_2 {
	HFONT              hFont;
	IDWrite_متن_Format* p_متن_Format;
	DWRITE_FONT_وزن font_وزن;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	توکی_2() : hFont(عدم), p_متن_Format(عدم), fontAscent(0.0f) {}
    };

خاص:
    صحيح m_اندازه;
    توکی_2 *m_توکي;

عام:
    FontCache(صحيح اندازه = 2) :
	m_اندازه(اندازه),
	m_توکي(جديد توکی_2[اندازه])
    {
    }

    ~FontCache()
    {
	لكل (صحيح i = 0; i < m_اندازه; ++i)
	    خوندي_Release(&m_توکي[i].p_متن_Format);
	ړنګول[] m_توکي;
    }

    منطقي get(HFONT hFont, توکی_2 &توکی)
    {
	صحيح n = پیدا_کړه(hFont);
	إذا (n < 0)
	    أرجع خطأ;
	توکی = m_توکي[n];
	slide(n);
	أرجع ریښتیا;
    }

    فراغ put(ثابت توکی_2& توکی)
    {
	صحيح n = پیدا_کړه(توکی.hFont);
	إذا (n < 0)
	    n = m_اندازه - 1;
	إذا (m_توکي[n].p_متن_Format != توکی.p_متن_Format)
	{
	    خوندي_Release(&m_توکي[n].p_متن_Format);
	    إذا (توکی.p_متن_Format != عدم)
		توکی.p_متن_Format->جمع_کړه_Ref();
	}
	m_توکي[n] = توکی;
	slide(n);
    }

خاص:
    صحيح پیدا_کړه(HFONT hFont)
    {
	لكل (صحيح i = 0; i < m_اندازه; ++i)
	{
	    إذا (m_توکي[i].hFont == hFont)
		أرجع i;
	}
	أرجع -1;
    }

    فراغ slide(صحيح بل_سر)
    {
	إذا (بل_سر == 0)
	    أرجع;
	توکی_2 tmp = m_توکي[بل_سر];
	لكل (صحيح i = بل_سر - 1; i >= 0; --i)
	    m_توکي[i + 1] = m_توکي[i];
	m_توکي[0] = tmp;
    }
};

تعداد Drawing_ډېرپېښ {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

بنية DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_ډېرپېښ mDMode;
    HDC mInteropHDC;
    منطقي mDrawing;
    منطقي mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_موخه *mRT;
    ID2_D1_Gdi_Interop_Render_موخه *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#د_که_پای

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_متن_Format *m_متن_Format;
    DWRITE_FONT_وزن m_Font_وزن;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_اندازه;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_متن_ANTIALIAS_ډېرپېښ m_متن_Antialias_ډېرپېښ;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    صحيح m_Font_Feature_شمېر;

    // METHODS

    DWriteContext();

    مجازي ~DWriteContext();

    HRESULT CreateDeviceResources();

    فراغ DiscardDeviceResources();

    HRESULT Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	    IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent);

    HRESULT سېټ_Font_By_LOGFONT(ثابت LOGFONTW &logFont);

    فراغ سېټ_Font(HFONT hFont);

    فراغ Rebind();

    فراغ BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT سېټ_Drawing_ډېرپېښ(Drawing_ډېرپېښ ډېرپېښ);

    ID2D1Brush* SolidBrush(COLORREF color);

    فراغ Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_پلنوالی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    فراغ FillRect(ثابت RECT *rc, COLORREF color);

    فراغ Draw_کرښه(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color);

    فراغ سېټ_Pixel(صحيح x, صحيح y, COLORREF color);

    فراغ Scroll(صحيح x, صحيح y, ثابت RECT *rc);

    فراغ Flush();

    فراغ سېټ_Rendering_Params(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    فراغ سېټ_Font_Features(ثابت DWriteFontFeature *features, صحيح شمېر);
};

صنف Adjusted_Glyph_پرلپسې : عام DWRITE_GLYPH_پرلپسې
{
خاص:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عام:
    Adjusted_Glyph_پرلپسې(
	    ثابت DWRITE_GLYPH_پرلپسې *glyph_پرلپسې,
	    FLOAT cell_پلنوالی_2,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحيح *glyph_شاخص,
	    صحيح cch_متن) :
	DWRITE_GLYPH_پرلپسې(*glyph_پرلپسې),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جديد FLOAT[glyph_پرلپسې->glyph_شمېر])
    {
	أكد(cell_پلنوالی_2 != 0.0f);
	لكل (UINT32 i = 0; i < glyph_پرلپسې->glyph_شمېر; ++i)
	{
	    FLOAT orig = glyph_پرلپسې->glyphAdvances[i];
	    FLOAT adjusted;

	    إذا (*glyph_شاخص < cch_متن)
	    {
		adjusted = FLOAT(lpDx[*glyph_شاخص]);
		(*glyph_شاخص)++;
	    }
	    وإلا
	    {
		adjusted = adjust_منزل_Cell(orig, cell_پلنوالی_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_پرلپسې()
    {
	mAccum += mDelta;
	ړنګول[] mAdjustedAdvances;
    }

    ساكن FLOAT adjust_منزل_Cell(FLOAT ارزښت, FLOAT cell_پلنوالی_2)
    {
	صحيح cell_شمېر = صحيح(floor(ارزښت / cell_پلنوالی_2 + 0.5f));
	إذا (cell_شمېر < 1)
	    cell_شمېر = 1;
	أرجع cell_شمېر * cell_پلنوالی_2;
    }
};

بنية متن_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_پلنوالی_2;
    ثابت INT *lpDx;
    صحيح cch_متن;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT دننه_واټن_X;
    صحيح glyph_شاخص;
};

صنف متن_Renderer FINAL : عام IDWrite_متن_Renderer
{
عام:
    متن_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_شمېر(0),
	pDWC_(pDWC)
    {
	جمع_کړه_Ref();
    }

    // add "virtual" to avoid a compiler warning
    مجازي ~متن_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	أرجع S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	__out DWRITE_ماتریکس* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_ماتریکس_3_X2_F*>(transform));
	أرجع S_OK;
    }

    IFACEMETHOD(Get_Pixels_فی_Dip)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	__out FLOAT* pixels_فی_Dip)
    {
	عائم dpiX, ناکارول_شوی_2;
	pDWC_->mRT->GetDpi(&dpiX, &ناکارول_شوی_2);
	*pixels_فی_Dip = dpiX / 96.0f;
	أرجع S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	FLOAT baselineOriginX ناکارول_شوی,
	FLOAT baselineOriginY ناکارول_شوی,
	__in DWRITE_UNDERLINE ثابت* underline ناکارول_شوی,
	IUnknown* clientDrawingEffect ناکارول_شوی)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	FLOAT baselineOriginX ناکارول_شوی,
	FLOAT baselineOriginY ناکارول_شوی,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough ناکارول_شوی,
	IUnknown* clientDrawingEffect ناکارول_شوی)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull فراغ* clientDrawingContext ناکارول_شوی,
	FLOAT originX ناکارول_شوی,
	FLOAT originY ناکارول_شوی,
	IDWriteInlineObject* inlineObject ناکارول_شوی,
	BOOL isSideways ناکارول_شوی,
	BOOL is_ښی_منزل_کیڼ ناکارول_شوی,
	IUnknown* clientDrawingEffect ناکارول_شوی)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_پرلپسې)(
	__maybenull فراغ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_ډېرپېښ measuring_ډېرپېښ ناکارول_شوی,
	__in DWRITE_GLYPH_پرلپسې ثابت* glyph_پرلپسې,
	__in DWRITE_GLYPH_پرلپسې_DESCRIPTION ثابت* glyph_پرلپسې_Description ناکارول_شوی,
	IUnknown* clientDrawingEffect ناکارول_شوی)
    {
	متن_Renderer_Context *context =
	    reinterpret_cast<متن_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_پرلپسې adjusted_Glyph_پرلپسې(glyph_پرلپسې, context->cell_پلنوالی_2,
		context->دننه_واټن_X, context->lpDx, &context->glyph_شاخص,
		context->cch_متن);

#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
	إذا (pDWC_->mDWriteFactory2 != عدم)
	{
	    IDWrite_Color_Glyph_پرلپسې_Enumerator *enumerator = عدم;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_پرلپسې(
		baselineOriginX + context->دننه_واټن_X,
		baselineOriginY,
		&adjusted_Glyph_پرلپسې,
		عدم,
		DWRITE_MEASURING_ډېرپېښ_GDI_NATURAL,
		عدم,
		0,
		&enumerator);
	    إذا (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_پرلپسې = TRUE;
		enumerator->Move_بل(&has_پرلپسې);
		طالما (has_پرلپسې)
		{
		    ثابت DWRITE_COLOR_GLYPH_پرلپسې* color_Glyph_پرلپسې;
		    enumerator->Get_Current_پرلپسې(&color_Glyph_پرلپسې);

		    pDWC_->mBrush->سېټ_Color(color_Glyph_پرلپسې->پرلپسې_Color);
		    pDWC_->mRT->Draw_Glyph_پرلپسې(
			    D2D1::Point2F(
				color_Glyph_پرلپسې->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_پرلپسې->glyph_پرلپسې,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_ډېرپېښ_NATURAL);
		    enumerator->Move_بل(&has_پرلپسې);
		}
		خوندي_Release(&enumerator);
		أرجع S_OK;
	    }
	}
#د_که_پای

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_پرلپسې(
		D2D1::Point2F(
		    baselineOriginX + context->دننه_واټن_X,
		    fixedBaselineY),
		&adjusted_Glyph_پرلپسې,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_ډېرپېښ_NATURAL);
	أرجع S_OK;
    }

عام:
    IFACEMETHOD_(بینښه اوږد, جمع_کړه_Ref) ()
    {
	أرجع InterlockedIncrement(&c_Ref_شمېر);
    }

    IFACEMETHOD_(بینښه اوږد, Release) ()
    {
	اوږد new_شمېر_2 = InterlockedDecrement(&c_Ref_شمېر);

	إذا (new_شمېر_2 == 0)
	{
	    ړنګول هذا;
	    أرجع 0;
	}
	أرجع new_شمېر_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ثابت& riid,
	فراغ** ppvObject)
    {
	إذا (__uuidof(IDWrite_متن_Renderer) == riid)
	{
	    *ppvObject = هذا;
	}
	وإلا إذا (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = هذا;
	}
	وإلا إذا (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = هذا;
	}
	وإلا
	{
	    *ppvObject = عدم;
	    أرجع E_FAIL;
	}

	أرجع S_OK;
    }

خاص:
    اوږد c_Ref_شمېر;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(عدم),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(عدم),
    mDrawing(خطأ),
    mFallbackDC(خطأ),
    mD2D1Factory(عدم),
    mRT(عدم),
    mGDIRT(عدم),
    mBrush(عدم),
    mBitmap(عدم),
    mDWriteFactory(عدم),
#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(عدم),
#د_که_پای
    mGdiInterop(عدم),
    mRenderingParams(عدم),
    mFontCache(8),
    m_متن_Format(عدم),
    m_Font_وزن(DWRITE_FONT_وزن_نورمال),
    mFontStyle(DWRITE_FONT_STYLE_نورمال),
    m_Font_اندازه(0.0f),
    mFontAscent(0.0f),
    m_متن_Antialias_ډېرپېښ(D2_D1_متن_ANTIALIAS_ډېرپېښ_DEFAULT),
    m_Font_Feature_شمېر(0)
{
    Zero_حافظه(mFontFeatures, حجم(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), عدم,
	    reinterpret_cast<فراغ**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    إذا (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
    إذا (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#د_که_پای

    إذا (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    إذا (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->CreateRenderingParams(&mRenderingParams);
	_RPT2(_CRT_WARN, "CreateRenderingParams: hr=%p p=%p\n", hr,
		mRenderingParams);
    }
}

DWriteContext::~DWriteContext()
{
    خوندي_Release(&m_متن_Format);
    خوندي_Release(&mRenderingParams);
    خوندي_Release(&mGdiInterop);
    خوندي_Release(&mDWriteFactory);
#که_تعریف_شوی FEAT_DIRECTX_COLOR_EMOJI
    خوندي_Release(&mDWriteFactory2);
#د_که_پای
    خوندي_Release(&mBitmap);
    خوندي_Release(&mBrush);
    خوندي_Release(&mGDIRT);
    خوندي_Release(&mRT);
    خوندي_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    إذا (mRT != عدم)
	أرجع S_OK;

    D2_D1_RENDER_موخه_PROPERTIES props = {
	D2_D1_RENDER_موخه_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ډېرپېښ_IGNORE },
	0, 0,
	D2_D1_RENDER_موخه_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_موخه(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    إذا (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_موخه),
		reinterpret_cast<فراغ**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    إذا (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    إذا (SUCCEEDED(hr))
	Rebind();

    أرجع hr;
}

    فراغ
DWriteContext::DiscardDeviceResources()
{
    خوندي_Release(&mBitmap);
    خوندي_Release(&mBrush);
    خوندي_Release(&mGDIRT);
    خوندي_Release(&mRT);
}

    HRESULT
DWriteContext::Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent)
{
    // Most of this function is copied from: https://github.com/Microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/multimedia/DirectWrite/RenderTest/TextHelpers.cpp
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = عدم;

    IDWriteFont *font = عدم;
    IDWriteFontFamily *fontFamily = عدم;
    IDWrite_Localized_مزي_ګانې *localized_Family_نومونه = عدم;
    عائم font_اندازه = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    إذا (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    إذا (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    إذا (SUCCEEDED(hr))
    {
	hr = fontFamily->Get_Family_نومونه(&localized_Family_نومونه);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    إذا (SUCCEEDED(hr))
    {
	hr = localized_Family_نومونه->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    إذا (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_اندازه = عائم(logFont.lf_لوړوالی);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	إذا (font_اندازه < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_اندازه = -font_اندازه;
	}
	وإلا إذا (font_اندازه > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_اندازه = font_اندازه * عائم(fontMetrics.design_Units_فی_Em)
		/ عائم(fontMetrics.ascent + fontMetrics.descent + fontMetrics.کرښه_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_اوږدتیا];
    إذا (SUCCEEDED(hr))
    {
	إذا (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_اوږدتیا) == 0)
	    hr = HRESULT_FROM_WIN32(Get_وروستی_Error());
    }

    إذا (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_متن_Format(
		familyName,
		عدم, // no custom font collection
		font->Get_وزن(),
		font->GetStyle(),
		font->GetStretch(),
		font_اندازه,
		localeName,
		&p_متن_Format);
    }

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سېټ_متن_Alignment(DWRITE_متن_ALIGNMENT_LEADING);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سېټ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سېټ_کلمه_Wrapping(DWRITE_کلمه_WRAPPING_NO_WRAP);

    خوندي_Release(&localized_Family_نومونه);
    خوندي_Release(&fontFamily);
    خوندي_Release(&font);

    إذا (SUCCEEDED(hr))
    {
	*pp_متن_Format = p_متن_Format;
	إذا (pFontAscent != عدم && fontMetrics.design_Units_فی_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT ټول_لوړوالی = عائم(fontMetrics.ascent + fontMetrics.descent);
	    إذا (ټول_لوړوالی != 0.0f)
		*pFontAscent = عائم(fontMetrics.ascent) / ټول_لوړوالی;
	    وإلا
		*pFontAscent = 0.83f; // fallback
	}
    }
    وإلا
	خوندي_Release(&p_متن_Format);

    أرجع hr;
}

    HRESULT
DWriteContext::سېټ_Font_By_LOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = عدم;
    FLOAT fontAscent = 0.0f;

    hr = Create_متن_Format_From_LOGFONT(logFont, &p_متن_Format, &fontAscent);

    إذا (SUCCEEDED(hr))
    {
	خوندي_Release(&m_متن_Format);
	m_متن_Format = p_متن_Format;
	m_Font_وزن = static_cast<DWRITE_FONT_وزن>(logFont.lf_وزن);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_نورمال;
	mFontAscent = fontAscent;
    }

    أرجع hr;
}

    فراغ
DWriteContext::سېټ_Font(HFONT hFont)
{
    FontCache::توکی_2 توکی;
    إذا (mFontCache.get(hFont, توکی))
    {
	إذا (توکی.p_متن_Format != عدم)
	{
	    توکی.p_متن_Format->جمع_کړه_Ref();
	    خوندي_Release(&m_متن_Format);
	    m_متن_Format = توکی.p_متن_Format;
	    m_Font_وزن = توکی.font_وزن;
	    mFontStyle = توکی.fontStyle;
	    mFontAscent = توکی.fontAscent;
	    mFallbackDC = خطأ;
	}
	وإلا
	    mFallbackDC = ریښتیا;
	أرجع;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    إذا (GetObjectW(hFont, حجم(lf), &lf))
	hr = سېټ_Font_By_LOGFONT(lf);

    توکی.hFont = hFont;
    إذا (SUCCEEDED(hr))
    {
	توکی.p_متن_Format = m_متن_Format;
	توکی.font_وزن = m_Font_وزن;
	توکی.fontStyle = mFontStyle;
	توکی.fontAscent = mFontAscent;
	mFallbackDC = خطأ;
    }
    وإلا
	mFallbackDC = ریښتیا;
    mFontCache.put(توکی);
}

    فراغ
DWriteContext::Rebind()
{
    خوندي_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->سېټ_Transform(D2D1::بې_اغېزه_عنصر_ماتریکس());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_ډېرپېښ_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::اندازه_U(mBindRect.ښی - mBindRect.کیڼ,
		mBindRect.bottom - mBindRect.سر_2),
	    props, &mBitmap);
}

    فراغ
DWriteContext::BindDC(HDC hdc, ثابت RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    إذا (mRT == عدم)
	CreateDeviceResources();
    وإلا
    {
	Flush();
	Rebind();
    }
}

خارجي "C" فراغ redraw_later_clear(فراغ);

    HRESULT
DWriteContext::سېټ_Drawing_ډېرپېښ(Drawing_ډېرپېښ ډېرپېښ)
{
    HRESULT hr = S_OK;

    بدل (ډېرپېښ)
    {
	افتراضي:
	حالة DM_GDI:
	    إذا (mInteropHDC != عدم)
	    {
		mGDIRT->ReleaseDC(عدم);
		mInteropHDC = عدم;
	    }
	    إذا (mDrawing)
	    {
		hr = mRT->پای_ټکی_Draw();
		إذا (hr == (HRESULT)D2_DERR_RECREATE_موخه)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = خطأ;
	    }
	    اكسر;

	حالة DM_DIRECTX:
	    إذا (mInteropHDC != عدم)
	    {
		mGDIRT->ReleaseDC(عدم);
		mInteropHDC = عدم;
	    }
	    وإلا إذا (mDrawing == خطأ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ریښتیا;
	    }
	    اكسر;

	حالة DM_INTEROP:
	    إذا (mDrawing == خطأ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = ریښتیا;
	    }
	    إذا (mInteropHDC == عدم)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_ډېرپېښ_COPY, &mInteropHDC);
	    اكسر;
    }
    mDMode = ډېرپېښ;
    أرجع hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->سېټ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    أرجع mBrush;
}

    فراغ
DWriteContext::Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_پلنوالی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    إذا (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = سېټ_Drawing_ډېرپېښ(DM_INTEROP);
	إذا (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::سېټ_متن_Color(mInteropHDC, color);
	    ::سېټ_Bk_ډېرپېښ(mInteropHDC, ::Get_Bk_ډېرپېښ(mHDC));
	    ::Ext_متن_بهر_W(mInteropHDC, x, y, fuOptions, lprc, متن, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	أرجع;
    }

    HRESULT hr;
    IDWrite_متن_Layout *متن_Layout = عدم;

    سېټ_Drawing_ډېرپېښ(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
    {
	mRT->ټېل_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->کیڼ), FLOAT(lprc->سر_2),
			FLOAT(lprc->ښی), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_ډېرپېښ_ALIASED);
    }

    hr = mDWriteFactory->Create_متن_Layout(متن, len, m_متن_Format,
	    FLOAT(w), FLOAT(h), &متن_Layout);

    إذا (SUCCEEDED(hr))
    {
	DWRITE_متن_RANGE متن_Range = { 0, UINT32(len) };
	متن_Layout->سېټ_Font_وزن(m_Font_وزن, متن_Range);
	متن_Layout->سېټ_Font_Style(mFontStyle, متن_Range);

	إذا (m_Font_Feature_شمېر > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساكن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_نښان_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_نښان_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_نښان_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_نښان_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_نښان_KERNING, 1 },
	    };
	    ساكن ثابت صحيح عدد_Defaults = حجم(defaultFeatures)
		/ حجم(defaultFeatures[0]);

	    IDWriteTypography *typography = عدم;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    إذا (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		لكل (صحيح d = 0; d < عدد_Defaults; ++d)
		{
		    صحيح overridden = 0;
		    لكل (صحيح u = 0; u < m_Font_Feature_شمېر; ++u)
		    {
			إذا ((DWRITE_FONT_FEATURE_نښان)mFontFeatures[u].نښان
				== defaultFeatures[d].name_نښان)
			{
			    overridden = 1;
			    اكسر;
			}
		    }
		    إذا (!overridden)
			typography->جمع_کړه_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		لكل (صحيح i = 0; i < m_Font_Feature_شمېر; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_نښان)mFontFeatures[i].نښان,
			mFontFeatures[i].parameter
		    };
		    typography->جمع_کړه_Font_Feature(ff);
		}
		متن_Layout->سېټ_Typography(typography, متن_Range);
		خوندي_Release(&typography);
	    }
	}

	// Calculate baseline using font ascent from font metrics.
	// Do NOT use GetLineMetrics() because it returns different values
	// depending on text content (e.g., when CJK characters trigger
	// font fallback, the metrics change).
	// Use the pre-calculated font ascent for all text to prevent
	// vertical shifts during redraw.
	FLOAT baselineY = roundf(FLOAT(y) + FLOAT(h) * mFontAscent + 0.5);

	متن_Renderer renderer(هذا);
	متن_Renderer_Context context = { color, FLOAT(cell_پلنوالی_2), lpDx, len,
		baselineY, 0.0f, 0 };

	متن_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    خوندي_Release(&متن_Layout);

    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
	mRT->راوباسه_Axis_Aligned_Clip();
}

    فراغ
DWriteContext::FillRect(ثابت RECT *rc, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    وإلا
    {
	سېټ_Drawing_ډېرپېښ(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->کیڼ), FLOAT(rc->سر_2),
		    FLOAT(rc->ښی), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Draw_کرښه(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_منزل_Ex(mInteropHDC, x1, y1, عدم);
	::کرښه_منزل(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    وإلا
    {
	سېټ_Drawing_ډېرپېښ(DM_DIRECTX);
	mRT->Draw_کرښه(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::سېټ_Pixel(صحيح x, صحيح y, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::سېټ_Pixel(mInteropHDC, x, y, color);
    }
    وإلا
    {
	سېټ_Drawing_ډېرپېښ(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_کرښه(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Scroll(صحيح x, صحيح y, ثابت RECT *rc)
{
    سېټ_Drawing_ډېرپېښ(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_ټکی_2_U dest_ټکی;
    إذا (x >= 0)
    {
	srcRect.کیڼ = rc->کیڼ;
	srcRect.ښی = rc->ښی - x;
	dest_ټکی.x = rc->کیڼ + x;
    }
    وإلا
    {
	srcRect.کیڼ = rc->کیڼ - x;
	srcRect.ښی = rc->ښی;
	dest_ټکی.x = rc->کیڼ;
    }
    إذا (y >= 0)
    {
	srcRect.سر_2 = rc->سر_2;
	srcRect.bottom = rc->bottom - y;
	dest_ټکی.y = rc->سر_2 + y;
    }
    وإلا
    {
	srcRect.سر_2 = rc->سر_2 - y;
	srcRect.bottom = rc->bottom;
	dest_ټکی.y = rc->سر_2;
    }
    mBitmap->Copy_From_Render_موخه(&dest_ټکی, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_ټکی.x), FLOAT(dest_ټکی.y),
	    FLOAT(dest_ټکی.x + srcRect.ښی - srcRect.کیڼ),
	    FLOAT(dest_ټکی.y + srcRect.bottom - srcRect.سر_2)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_ډېرپېښ_تر_ټولو_نږدې_NEIGHBOR, destRect);
}

    فراغ
DWriteContext::Flush()
{
    سېټ_Drawing_ډېرپېښ(DM_GDI);
}

    فراغ
DWriteContext::سېټ_Rendering_Params(
	ثابت DWriteRenderingParams *params)
{
    إذا (mDWriteFactory == عدم)
	أرجع;

    IDWriteRenderingParams *renderingParams = عدم;
    D2_D1_متن_ANTIALIAS_ډېرپېښ متن_Antialias_ډېرپېښ =
	D2_D1_متن_ANTIALIAS_ډېرپېښ_DEFAULT;
    HRESULT hr;
    إذا (params != عدم)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		منزل_Pixel_Geometry(params->pixelGeometry),
		منزل_Rendering_ډېرپېښ(params->rendering_ډېرپېښ), &renderingParams);
	متن_Antialias_ډېرپېښ = منزل_متن_Antialias_ډېرپېښ(params->متن_Antialias_ډېرپېښ);
    }
    وإلا
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    إذا (SUCCEEDED(hr) && renderingParams != عدم)
    {
	خوندي_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_متن_Antialias_ډېرپېښ = متن_Antialias_ډېرپېښ;

	Flush();
	mRT->سېټ_متن_Rendering_Params(mRenderingParams);
	mRT->سېټ_متن_Antialias_ډېرپېښ(m_متن_Antialias_ډېرپېښ);
    }
}

    DWriteRenderingParams *
DWriteContext::GetRenderingParams(
	DWriteRenderingParams *params)
{
    إذا (params != عدم && mRenderingParams != عدم)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = منزل_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_ډېرپېښ = منزل_Int(mRenderingParams->Get_Rendering_ډېرپېښ());
	params->متن_Antialias_ډېرپېښ = m_متن_Antialias_ډېرپېښ;
    }
    أرجع params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    فراغ
DWrite_Init(فراغ)
{
#که_تعریف_شوی DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_بارول_Lib("d2d1.dll");
    hDWriteDLL = vim_بارول_Lib("dwrite.dll");
    إذا (hD2D1DLL == عدم || hDWriteDLL == عدم)
    {
	DWrite_Final();
	أرجع;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_پته(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_پته(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_پته(hDWriteDLL,
	    "DWriteCreateFactory");
#د_که_پای
}

    فراغ
DWrite_Final(فراغ)
{
#که_تعریف_شوی DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = عدم;
    pD2D1CreateFactory = عدم;
    pDWriteCreateFactory = عدم;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#د_که_پای
}

    DWriteContext *
DWriteContext_Open(فراغ)
{
#که_تعریف_شوی DYNAMIC_DIRECTX
    إذا (pGetUserDefaultLocaleName == عدم || pD2D1CreateFactory == عدم
	    || pDWriteCreateFactory == عدم)
	أرجع عدم;
#د_که_پای
    أرجع جديد DWriteContext();
}

    فراغ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    إذا (ctx != عدم)
	ctx->BindDC(hdc, rect);
}

    فراغ
DWrite_Context_سېټ_Font(DWriteContext *ctx, HFONT hFont)
{
    إذا (ctx != عدم)
	ctx->سېټ_Font(hFont);
}

    فراغ
DWrite_Context_Draw_متن(
	DWriteContext *ctx,
	ثابت WCHAR *متن,
	صحيح len,
	صحيح x,
	صحيح y,
	صحيح w,
	صحيح h,
	صحيح cell_پلنوالی_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    إذا (ctx != عدم)
	ctx->Draw_متن(متن, len, x, y, w, h, cell_پلنوالی_2, color,
		fuOptions, lprc, lpDx);
}

    فراغ
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->FillRect(rc, color);
}

    فراغ
DWrite_Context_Draw_کرښه(DWriteContext *ctx, صحيح x1, صحيح y1, صحيح x2, صحيح y2,
	COLORREF color)
{
    إذا (ctx != عدم)
	ctx->Draw_کرښه(x1, y1, x2, y2, color);
}

    فراغ
DWrite_Context_سېټ_Pixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->سېټ_Pixel(x, y, color);
}

    فراغ
DWriteContext_Scroll(DWriteContext *ctx, صحيح x, صحيح y, ثابت RECT *rc)
{
    إذا (ctx != عدم)
	ctx->Scroll(x, y, rc);
}

    فراغ
DWriteContext_Flush(DWriteContext *ctx)
{
    إذا (ctx != عدم)
	ctx->Flush();
}

    فراغ
DWriteContext_Close(DWriteContext *ctx)
{
    ړنګول ctx;
}

    فراغ
DWrite_Context_سېټ_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    إذا (ctx != عدم)
	ctx->سېټ_Rendering_Params(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    إذا (ctx != عدم)
	أرجع ctx->GetRenderingParams(params);
    وإلا
	أرجع عدم;
}

    فراغ
DWriteContext::سېټ_Font_Features(
	ثابت DWriteFontFeature *features, صحيح شمېر)
{
    إذا (شمېر > DWRITE_MAX_FONT_FEATURES)
	شمېر = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_شمېر = شمېر;
    إذا (شمېر > 0 && features != عدم)
	حافظه_نقل(mFontFeatures, features, حجم(DWriteFontFeature) * شمېر);
}

    فراغ
DWrite_Context_سېټ_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح شمېر)
{
    إذا (ctx != عدم)
	ctx->سېټ_Font_Features(features, شمېر);
}
