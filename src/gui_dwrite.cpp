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

#تعریف_کرو WIN32_LEAN_AND_اوسط

#جے_بنا_تعریف DYNAMIC_DIRECTX
# إذا WINVER < 0x0600
#  error WINVER must be 0x0600 or above منزل use سدھا_Write(سدھا_X)
# endif
#جے_انت

#ضمن <windows.h>
#ضمن <crtdbg.h>
#ضمن <assert.h>
#ضمن <math.h>
#ضمن <d2d1.h>
#ضمن <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#إذا defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define وچ_reads(s)
# define وچ_reads_opt(s)
# define _Maybenull_
# define باہرلا_writes(s)
# define باہرلا_writes_opt(s)
# define باہرلا_writes_منزل(x, y)
# define باہرلا_writes_منزل_opt(x, y)
# define _Outptr_
#جے_انت

#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#وإلا
# include <dwrite.h>
#جے_انت

#ضمن "gui_dwrite.h"

#جے_تعریف_شدہ __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_وچ
# define __out		SAL_باہرلا
#جے_انت

#جے_تعریف_شدہ __MINGW32__
# define بن_ورتیا __attribute__((بن_ورتیا_2))
#وإلا
# define بن_ورتیا
#جے_انت

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#جے_انت

#جے_تعریف_شدہ DYNAMIC_DIRECTX
خارجي "C" HINSTANCE vim_لدو_Lib(ثابت حرف *name);

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

#تعریف_کرو GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#تعریف_کرو D2D1CreateFactory		(*pD2D1CreateFactory)
#تعریف_کرو DWriteCreateFactory		(*pDWriteCreateFactory)

    ساكن فراغ
unload(HINSTANCE &hinst)
{
    إذا (hinst != عدم)
    {
	FreeLibrary(hinst);
	hinst = عدم;
    }
}
#جے_انت // DYNAMIC_DIRECTX

سانچا <صنف T> مضمن فراغ محفوظ_Release(T **ppT)
{
    إذا (*ppT)
    {
	(*ppT)->Release();
	*ppT = عدم;
    }
}

    ساكن DWRITE_PIXEL_GEOMETRY
منزل_Pixel_Geometry(صحيح قدر)
{
    بدل (قدر)
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
منزل_Int(DWRITE_PIXEL_GEOMETRY قدر)
{
    بدل (قدر)
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

    ساكن DWRITE_RENDERING_منوال
منزل_Rendering_منوال(صحيح قدر)
{
    بدل (قدر)
    {
	افتراضي:
	حالة 0:
	    أرجع DWRITE_RENDERING_منوال_DEFAULT;
	حالة 1:
	    أرجع DWRITE_RENDERING_منوال_ALIASED;
	حالة 2:
	    أرجع DWRITE_RENDERING_منوال_CLEARTYPE_GDI_CLASSIC;
	حالة 3:
	    أرجع DWRITE_RENDERING_منوال_CLEARTYPE_GDI_NATURAL;
	حالة 4:
	    أرجع DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL;
	حالة 5:
	    أرجع DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL_متشاکل;
	حالة 6:
	    أرجع DWRITE_RENDERING_منوال_OUTLINE;
    }
}

    ساكن D2_D1_متن_ANTIALIAS_منوال
منزل_متن_Antialias_منوال(صحيح قدر)
{
    بدل (قدر)
    {
	افتراضي:
	حالة 0:
	    أرجع D2_D1_متن_ANTIALIAS_منوال_DEFAULT;
	حالة 1:
	    أرجع D2_D1_متن_ANTIALIAS_منوال_CLEARTYPE;
	حالة 2:
	    أرجع D2_D1_متن_ANTIALIAS_منوال_GRAYSCALE;
	حالة 3:
	    أرجع D2_D1_متن_ANTIALIAS_منوال_ALIASED;
    }
}

    ساكن صحيح
منزل_Int(DWRITE_RENDERING_منوال قدر)
{
    بدل (قدر)
    {
	حالة DWRITE_RENDERING_منوال_DEFAULT:
	    أرجع 0;
	حالة DWRITE_RENDERING_منوال_ALIASED:
	    أرجع 1;
	حالة DWRITE_RENDERING_منوال_CLEARTYPE_GDI_CLASSIC:
	    أرجع 2;
	حالة DWRITE_RENDERING_منوال_CLEARTYPE_GDI_NATURAL:
	    أرجع 3;
	حالة DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL:
	    أرجع 4;
	حالة DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL_متشاکل:
	    أرجع 5;
	حالة DWRITE_RENDERING_منوال_OUTLINE:
	    أرجع 6;
	افتراضي:
	    أرجع -1;
    }
}

صنف FontCache {
عام:
    بنية شے_2 {
	HFONT              hFont;
	IDWrite_متن_Format* p_متن_Format;
	DWRITE_FONT_بھار font_بھار;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	شے_2() : hFont(عدم), p_متن_Format(عدم), fontAscent(0.0f) {}
    };

خاص:
    صحيح m_ناپ;
    شے_2 *m_اشیاء;

عام:
    FontCache(صحيح ناپ = 2) :
	m_ناپ(ناپ),
	m_اشیاء(جديد شے_2[ناپ])
    {
    }

    ~FontCache()
    {
	لكل (صحيح i = 0; i < m_ناپ; ++i)
	    محفوظ_Release(&m_اشیاء[i].p_متن_Format);
	مٹاؤ[] m_اشیاء;
    }

    منطقي get(HFONT hFont, شے_2 &شے)
    {
	صحيح n = لبھو(hFont);
	إذا (n < 0)
	    أرجع خطأ;
	شے = m_اشیاء[n];
	slide(n);
	أرجع سچ;
    }

    فراغ put(ثابت شے_2& شے)
    {
	صحيح n = لبھو(شے.hFont);
	إذا (n < 0)
	    n = m_ناپ - 1;
	إذا (m_اشیاء[n].p_متن_Format != شے.p_متن_Format)
	{
	    محفوظ_Release(&m_اشیاء[n].p_متن_Format);
	    إذا (شے.p_متن_Format != عدم)
		شے.p_متن_Format->جوڑو_Ref();
	}
	m_اشیاء[n] = شے;
	slide(n);
    }

خاص:
    صحيح لبھو(HFONT hFont)
    {
	لكل (صحيح i = 0; i < m_ناپ; ++i)
	{
	    إذا (m_اشیاء[i].hFont == hFont)
		أرجع i;
	}
	أرجع -1;
    }

    فراغ slide(صحيح اگلا_چوٹی)
    {
	إذا (اگلا_چوٹی == 0)
	    أرجع;
	شے_2 tmp = m_اشیاء[اگلا_چوٹی];
	لكل (صحيح i = اگلا_چوٹی - 1; i >= 0; --i)
	    m_اشیاء[i + 1] = m_اشیاء[i];
	m_اشیاء[0] = tmp;
    }
};

تعداد Drawing_منوال {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

بنية DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_منوال mDMode;
    HDC mInteropHDC;
    منطقي mDrawing;
    منطقي mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_نشانہ *mRT;
    ID2_D1_Gdi_Interop_Render_نشانہ *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#جے_انت

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_متن_Format *m_متن_Format;
    DWRITE_FONT_بھار m_Font_بھار;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_ناپ;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_متن_ANTIALIAS_منوال m_متن_Antialias_منوال;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    صحيح m_Font_Feature_گنتی;

    // METHODS

    DWriteContext();

    مجازی ~DWriteContext();

    HRESULT CreateDeviceResources();

    فراغ DiscardDeviceResources();

    HRESULT Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	    IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent);

    HRESULT سیٹ_Font_By_LOGFONT(ثابت LOGFONTW &logFont);

    فراغ سیٹ_Font(HFONT hFont);

    فراغ Rebind();

    فراغ BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT سیٹ_Drawing_منوال(Drawing_منوال منوال);

    ID2D1Brush* SolidBrush(COLORREF color);

    فراغ Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_چوڑائی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    فراغ FillRect(ثابت RECT *rc, COLORREF color);

    فراغ Draw_سطر(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color);

    فراغ سیٹ_Pixel(صحيح x, صحيح y, COLORREF color);

    فراغ Scroll(صحيح x, صحيح y, ثابت RECT *rc);

    فراغ Flush();

    فراغ سیٹ_Rendering_Params(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    فراغ سیٹ_Font_Features(ثابت DWriteFontFeature *features, صحيح گنتی);
};

صنف Adjusted_Glyph_لگاتار : عام DWRITE_GLYPH_لگاتار
{
خاص:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عام:
    Adjusted_Glyph_لگاتار(
	    ثابت DWRITE_GLYPH_لگاتار *glyph_لگاتار,
	    FLOAT cell_چوڑائی_2,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحيح *glyph_اشاریہ,
	    صحيح cch_متن) :
	DWRITE_GLYPH_لگاتار(*glyph_لگاتار),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جديد FLOAT[glyph_لگاتار->glyph_گنتی])
    {
	أكد(cell_چوڑائی_2 != 0.0f);
	لكل (UINT32 i = 0; i < glyph_لگاتار->glyph_گنتی; ++i)
	{
	    FLOAT orig = glyph_لگاتار->glyphAdvances[i];
	    FLOAT adjusted;

	    إذا (*glyph_اشاریہ < cch_متن)
	    {
		adjusted = FLOAT(lpDx[*glyph_اشاریہ]);
		(*glyph_اشاریہ)++;
	    }
	    وإلا
	    {
		adjusted = adjust_منزل_Cell(orig, cell_چوڑائی_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_لگاتار()
    {
	mAccum += mDelta;
	مٹاؤ[] mAdjustedAdvances;
    }

    ساكن FLOAT adjust_منزل_Cell(FLOAT قدر, FLOAT cell_چوڑائی_2)
    {
	صحيح cell_گنتی = صحيح(floor(قدر / cell_چوڑائی_2 + 0.5f));
	إذا (cell_گنتی < 1)
	    cell_گنتی = 1;
	أرجع cell_گنتی * cell_چوڑائی_2;
    }
};

بنية متن_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_چوڑائی_2;
    ثابت INT *lpDx;
    صحيح cch_متن;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT وتھ_X;
    صحيح glyph_اشاریہ;
};

صنف متن_Renderer FINAL : عام IDWrite_متن_Renderer
{
عام:
    متن_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_گنتی(0),
	pDWC_(pDWC)
    {
	جوڑو_Ref();
    }

    // add "virtual" to avoid a compiler warning
    مجازی ~متن_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	أرجع S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	__out DWRITE_میٹرکس* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_میٹرکس_3_X2_F*>(transform));
	أرجع S_OK;
    }

    IFACEMETHOD(Get_Pixels_فی_Dip)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	__out FLOAT* pixels_فی_Dip)
    {
	عائم dpiX, بن_ورتیا_2;
	pDWC_->mRT->GetDpi(&dpiX, &بن_ورتیا_2);
	*pixels_فی_Dip = dpiX / 96.0f;
	أرجع S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	FLOAT baselineOriginX بن_ورتیا,
	FLOAT baselineOriginY بن_ورتیا,
	__in DWRITE_UNDERLINE ثابت* underline بن_ورتیا,
	IUnknown* clientDrawingEffect بن_ورتیا)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	FLOAT baselineOriginX بن_ورتیا,
	FLOAT baselineOriginY بن_ورتیا,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough بن_ورتیا,
	IUnknown* clientDrawingEffect بن_ورتیا)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull فراغ* clientDrawingContext بن_ورتیا,
	FLOAT originX بن_ورتیا,
	FLOAT originY بن_ورتیا,
	IDWriteInlineObject* inlineObject بن_ورتیا,
	BOOL isSideways بن_ورتیا,
	BOOL is_دایاں_منزل_بایاں بن_ورتیا,
	IUnknown* clientDrawingEffect بن_ورتیا)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_لگاتار)(
	__maybenull فراغ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_منوال measuring_منوال بن_ورتیا,
	__in DWRITE_GLYPH_لگاتار ثابت* glyph_لگاتار,
	__in DWRITE_GLYPH_لگاتار_DESCRIPTION ثابت* glyph_لگاتار_Description بن_ورتیا,
	IUnknown* clientDrawingEffect بن_ورتیا)
    {
	متن_Renderer_Context *context =
	    reinterpret_cast<متن_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_لگاتار adjusted_Glyph_لگاتار(glyph_لگاتار, context->cell_چوڑائی_2,
		context->وتھ_X, context->lpDx, &context->glyph_اشاریہ,
		context->cch_متن);

#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
	إذا (pDWC_->mDWriteFactory2 != عدم)
	{
	    IDWrite_Color_Glyph_لگاتار_Enumerator *enumerator = عدم;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_لگاتار(
		baselineOriginX + context->وتھ_X,
		baselineOriginY,
		&adjusted_Glyph_لگاتار,
		عدم,
		DWRITE_MEASURING_منوال_GDI_NATURAL,
		عدم,
		0,
		&enumerator);
	    إذا (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_لگاتار = TRUE;
		enumerator->Move_اگلا(&has_لگاتار);
		طالما (has_لگاتار)
		{
		    ثابت DWRITE_COLOR_GLYPH_لگاتار* color_Glyph_لگاتار;
		    enumerator->Get_Current_لگاتار(&color_Glyph_لگاتار);

		    pDWC_->mBrush->سیٹ_Color(color_Glyph_لگاتار->لگاتار_Color);
		    pDWC_->mRT->Draw_Glyph_لگاتار(
			    D2D1::Point2F(
				color_Glyph_لگاتار->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_لگاتار->glyph_لگاتار,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_منوال_NATURAL);
		    enumerator->Move_اگلا(&has_لگاتار);
		}
		محفوظ_Release(&enumerator);
		أرجع S_OK;
	    }
	}
#جے_انت

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_لگاتار(
		D2D1::Point2F(
		    baselineOriginX + context->وتھ_X,
		    fixedBaselineY),
		&adjusted_Glyph_لگاتار,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_منوال_NATURAL);
	أرجع S_OK;
    }

عام:
    IFACEMETHOD_(بنانشانی لما, جوڑو_Ref) ()
    {
	أرجع InterlockedIncrement(&c_Ref_گنتی);
    }

    IFACEMETHOD_(بنانشانی لما, Release) ()
    {
	لما new_گنتی_2 = InterlockedDecrement(&c_Ref_گنتی);

	إذا (new_گنتی_2 == 0)
	{
	    مٹاؤ هذا;
	    أرجع 0;
	}
	أرجع new_گنتی_2;
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
    لما c_Ref_گنتی;
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
#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(عدم),
#جے_انت
    mGdiInterop(عدم),
    mRenderingParams(عدم),
    mFontCache(8),
    m_متن_Format(عدم),
    m_Font_بھار(DWRITE_FONT_بھار_عام),
    mFontStyle(DWRITE_FONT_STYLE_عام),
    m_Font_ناپ(0.0f),
    mFontAscent(0.0f),
    m_متن_Antialias_منوال(D2_D1_متن_ANTIALIAS_منوال_DEFAULT),
    m_Font_Feature_گنتی(0)
{
    Zero_حافظہ(mFontFeatures, حجم(mFontFeatures));
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

#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    إذا (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#جے_انت

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
    محفوظ_Release(&m_متن_Format);
    محفوظ_Release(&mRenderingParams);
    محفوظ_Release(&mGdiInterop);
    محفوظ_Release(&mDWriteFactory);
#جے_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    محفوظ_Release(&mDWriteFactory2);
#جے_انت
    محفوظ_Release(&mBitmap);
    محفوظ_Release(&mBrush);
    محفوظ_Release(&mGDIRT);
    محفوظ_Release(&mRT);
    محفوظ_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    إذا (mRT != عدم)
	أرجع S_OK;

    D2_D1_RENDER_نشانہ_PROPERTIES props = {
	D2_D1_RENDER_نشانہ_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_منوال_IGNORE },
	0, 0,
	D2_D1_RENDER_نشانہ_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_نشانہ(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    إذا (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_نشانہ),
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
    محفوظ_Release(&mBitmap);
    محفوظ_Release(&mBrush);
    محفوظ_Release(&mGDIRT);
    محفوظ_Release(&mRT);
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
    IDWrite_Localized_لڑیاں *localized_Family_ناں = عدم;
    عائم font_ناپ = 0;
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
	hr = fontFamily->Get_Family_ناں(&localized_Family_ناں);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    إذا (SUCCEEDED(hr))
    {
	hr = localized_Family_ناں->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    إذا (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_ناپ = عائم(logFont.lf_اچائی);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	إذا (font_ناپ < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_ناپ = -font_ناپ;
	}
	وإلا إذا (font_ناپ > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_ناپ = font_ناپ * عائم(fontMetrics.design_Units_فی_Em)
		/ عائم(fontMetrics.ascent + fontMetrics.descent + fontMetrics.سطر_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_لمائی];
    إذا (SUCCEEDED(hr))
    {
	إذا (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_لمائی) == 0)
	    hr = HRESULT_FROM_WIN32(Get_آخری_Error());
    }

    إذا (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_متن_Format(
		familyName,
		عدم, // no custom font collection
		font->Get_بھار(),
		font->GetStyle(),
		font->GetStretch(),
		font_ناپ,
		localeName,
		&p_متن_Format);
    }

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_متن_Alignment(DWRITE_متن_ALIGNMENT_LEADING);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_لفظ_Wrapping(DWRITE_لفظ_WRAPPING_NO_WRAP);

    محفوظ_Release(&localized_Family_ناں);
    محفوظ_Release(&fontFamily);
    محفوظ_Release(&font);

    إذا (SUCCEEDED(hr))
    {
	*pp_متن_Format = p_متن_Format;
	إذا (pFontAscent != عدم && fontMetrics.design_Units_فی_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT کل_اچائی = عائم(fontMetrics.ascent + fontMetrics.descent);
	    إذا (کل_اچائی != 0.0f)
		*pFontAscent = عائم(fontMetrics.ascent) / کل_اچائی;
	    وإلا
		*pFontAscent = 0.83f; // fallback
	}
    }
    وإلا
	محفوظ_Release(&p_متن_Format);

    أرجع hr;
}

    HRESULT
DWriteContext::سیٹ_Font_By_LOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = عدم;
    FLOAT fontAscent = 0.0f;

    hr = Create_متن_Format_From_LOGFONT(logFont, &p_متن_Format, &fontAscent);

    إذا (SUCCEEDED(hr))
    {
	محفوظ_Release(&m_متن_Format);
	m_متن_Format = p_متن_Format;
	m_Font_بھار = static_cast<DWRITE_FONT_بھار>(logFont.lf_بھار);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_عام;
	mFontAscent = fontAscent;
    }

    أرجع hr;
}

    فراغ
DWriteContext::سیٹ_Font(HFONT hFont)
{
    FontCache::شے_2 شے;
    إذا (mFontCache.get(hFont, شے))
    {
	إذا (شے.p_متن_Format != عدم)
	{
	    شے.p_متن_Format->جوڑو_Ref();
	    محفوظ_Release(&m_متن_Format);
	    m_متن_Format = شے.p_متن_Format;
	    m_Font_بھار = شے.font_بھار;
	    mFontStyle = شے.fontStyle;
	    mFontAscent = شے.fontAscent;
	    mFallbackDC = خطأ;
	}
	وإلا
	    mFallbackDC = سچ;
	أرجع;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    إذا (GetObjectW(hFont, حجم(lf), &lf))
	hr = سیٹ_Font_By_LOGFONT(lf);

    شے.hFont = hFont;
    إذا (SUCCEEDED(hr))
    {
	شے.p_متن_Format = m_متن_Format;
	شے.font_بھار = m_Font_بھار;
	شے.fontStyle = mFontStyle;
	شے.fontAscent = mFontAscent;
	mFallbackDC = خطأ;
    }
    وإلا
	mFallbackDC = سچ;
    mFontCache.put(شے);
}

    فراغ
DWriteContext::Rebind()
{
    محفوظ_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->سیٹ_Transform(D2D1::پچھان_تت_میٹرکس());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_منوال_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::ناپ_U(mBindRect.سجے - mBindRect.کھبے,
		mBindRect.bottom - mBindRect.چوٹی),
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
DWriteContext::سیٹ_Drawing_منوال(Drawing_منوال منوال)
{
    HRESULT hr = S_OK;

    بدل (منوال)
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
		hr = mRT->سرا_Draw();
		إذا (hr == (HRESULT)D2_DERR_RECREATE_نشانہ)
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
		mDrawing = سچ;
	    }
	    اكسر;

	حالة DM_INTEROP:
	    إذا (mDrawing == خطأ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = سچ;
	    }
	    إذا (mInteropHDC == عدم)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_منوال_COPY, &mInteropHDC);
	    اكسر;
    }
    mDMode = منوال;
    أرجع hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->سیٹ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    أرجع mBrush;
}

    فراغ
DWriteContext::Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_چوڑائی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    إذا (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = سیٹ_Drawing_منوال(DM_INTEROP);
	إذا (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::سیٹ_متن_Color(mInteropHDC, color);
	    ::سیٹ_Bk_منوال(mInteropHDC, ::Get_Bk_منوال(mHDC));
	    ::Ext_متن_باہرلا_W(mInteropHDC, x, y, fuOptions, lprc, متن, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	أرجع;
    }

    HRESULT hr;
    IDWrite_متن_Layout *متن_Layout = عدم;

    سیٹ_Drawing_منوال(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
    {
	mRT->ڈالو_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->کھبے), FLOAT(lprc->چوٹی),
			FLOAT(lprc->سجے), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_منوال_ALIASED);
    }

    hr = mDWriteFactory->Create_متن_Layout(متن, len, m_متن_Format,
	    FLOAT(w), FLOAT(h), &متن_Layout);

    إذا (SUCCEEDED(hr))
    {
	DWRITE_متن_RANGE متن_Range = { 0, UINT32(len) };
	متن_Layout->سیٹ_Font_بھار(m_Font_بھار, متن_Range);
	متن_Layout->سیٹ_Font_Style(mFontStyle, متن_Range);

	إذا (m_Font_Feature_گنتی > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساكن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_ٹیگ_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_ٹیگ_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٹیگ_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٹیگ_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٹیگ_KERNING, 1 },
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
		    لكل (صحيح u = 0; u < m_Font_Feature_گنتی; ++u)
		    {
			إذا ((DWRITE_FONT_FEATURE_ٹیگ)mFontFeatures[u].ٹیگ
				== defaultFeatures[d].name_ٹیگ)
			{
			    overridden = 1;
			    اكسر;
			}
		    }
		    إذا (!overridden)
			typography->جوڑو_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		لكل (صحيح i = 0; i < m_Font_Feature_گنتی; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_ٹیگ)mFontFeatures[i].ٹیگ,
			mFontFeatures[i].parameter
		    };
		    typography->جوڑو_Font_Feature(ff);
		}
		متن_Layout->سیٹ_Typography(typography, متن_Range);
		محفوظ_Release(&typography);
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
	متن_Renderer_Context context = { color, FLOAT(cell_چوڑائی_2), lpDx, len,
		baselineY, 0.0f, 0 };

	متن_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    محفوظ_Release(&متن_Layout);

    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
	mRT->نکالو_Axis_Aligned_Clip();
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
	سیٹ_Drawing_منوال(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->کھبے), FLOAT(rc->چوٹی),
		    FLOAT(rc->سجے), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Draw_سطر(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_منزل_Ex(mInteropHDC, x1, y1, عدم);
	::سطر_منزل(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    وإلا
    {
	سیٹ_Drawing_منوال(DM_DIRECTX);
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::سیٹ_Pixel(صحيح x, صحيح y, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::سیٹ_Pixel(mInteropHDC, x, y, color);
    }
    وإلا
    {
	سیٹ_Drawing_منوال(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Scroll(صحيح x, صحيح y, ثابت RECT *rc)
{
    سیٹ_Drawing_منوال(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_نقطہ_2_U dest_نقطہ;
    إذا (x >= 0)
    {
	srcRect.کھبے = rc->کھبے;
	srcRect.سجے = rc->سجے - x;
	dest_نقطہ.x = rc->کھبے + x;
    }
    وإلا
    {
	srcRect.کھبے = rc->کھبے - x;
	srcRect.سجے = rc->سجے;
	dest_نقطہ.x = rc->کھبے;
    }
    إذا (y >= 0)
    {
	srcRect.چوٹی = rc->چوٹی;
	srcRect.bottom = rc->bottom - y;
	dest_نقطہ.y = rc->چوٹی + y;
    }
    وإلا
    {
	srcRect.چوٹی = rc->چوٹی - y;
	srcRect.bottom = rc->bottom;
	dest_نقطہ.y = rc->چوٹی;
    }
    mBitmap->Copy_From_Render_نشانہ(&dest_نقطہ, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_نقطہ.x), FLOAT(dest_نقطہ.y),
	    FLOAT(dest_نقطہ.x + srcRect.سجے - srcRect.کھبے),
	    FLOAT(dest_نقطہ.y + srcRect.bottom - srcRect.چوٹی)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_منوال_سبھ_توں_نیڑے_NEIGHBOR, destRect);
}

    فراغ
DWriteContext::Flush()
{
    سیٹ_Drawing_منوال(DM_GDI);
}

    فراغ
DWriteContext::سیٹ_Rendering_Params(
	ثابت DWriteRenderingParams *params)
{
    إذا (mDWriteFactory == عدم)
	أرجع;

    IDWriteRenderingParams *renderingParams = عدم;
    D2_D1_متن_ANTIALIAS_منوال متن_Antialias_منوال =
	D2_D1_متن_ANTIALIAS_منوال_DEFAULT;
    HRESULT hr;
    إذا (params != عدم)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		منزل_Pixel_Geometry(params->pixelGeometry),
		منزل_Rendering_منوال(params->rendering_منوال), &renderingParams);
	متن_Antialias_منوال = منزل_متن_Antialias_منوال(params->متن_Antialias_منوال);
    }
    وإلا
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    إذا (SUCCEEDED(hr) && renderingParams != عدم)
    {
	محفوظ_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_متن_Antialias_منوال = متن_Antialias_منوال;

	Flush();
	mRT->سیٹ_متن_Rendering_Params(mRenderingParams);
	mRT->سیٹ_متن_Antialias_منوال(m_متن_Antialias_منوال);
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
	params->rendering_منوال = منزل_Int(mRenderingParams->Get_Rendering_منوال());
	params->متن_Antialias_منوال = m_متن_Antialias_منوال;
    }
    أرجع params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    فراغ
DWrite_Init(فراغ)
{
#جے_تعریف_شدہ DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_لدو_Lib("d2d1.dll");
    hDWriteDLL = vim_لدو_Lib("dwrite.dll");
    إذا (hD2D1DLL == عدم || hDWriteDLL == عدم)
    {
	DWrite_Final();
	أرجع;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_پتہ(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_پتہ(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_پتہ(hDWriteDLL,
	    "DWriteCreateFactory");
#جے_انت
}

    فراغ
DWrite_Final(فراغ)
{
#جے_تعریف_شدہ DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = عدم;
    pD2D1CreateFactory = عدم;
    pDWriteCreateFactory = عدم;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#جے_انت
}

    DWriteContext *
DWriteContext_Open(فراغ)
{
#جے_تعریف_شدہ DYNAMIC_DIRECTX
    إذا (pGetUserDefaultLocaleName == عدم || pD2D1CreateFactory == عدم
	    || pDWriteCreateFactory == عدم)
	أرجع عدم;
#جے_انت
    أرجع جديد DWriteContext();
}

    فراغ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    إذا (ctx != عدم)
	ctx->BindDC(hdc, rect);
}

    فراغ
DWrite_Context_سیٹ_Font(DWriteContext *ctx, HFONT hFont)
{
    إذا (ctx != عدم)
	ctx->سیٹ_Font(hFont);
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
	صحيح cell_چوڑائی_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    إذا (ctx != عدم)
	ctx->Draw_متن(متن, len, x, y, w, h, cell_چوڑائی_2, color,
		fuOptions, lprc, lpDx);
}

    فراغ
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->FillRect(rc, color);
}

    فراغ
DWrite_Context_Draw_سطر(DWriteContext *ctx, صحيح x1, صحيح y1, صحيح x2, صحيح y2,
	COLORREF color)
{
    إذا (ctx != عدم)
	ctx->Draw_سطر(x1, y1, x2, y2, color);
}

    فراغ
DWrite_Context_سیٹ_Pixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->سیٹ_Pixel(x, y, color);
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
    مٹاؤ ctx;
}

    فراغ
DWrite_Context_سیٹ_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    إذا (ctx != عدم)
	ctx->سیٹ_Rendering_Params(params);
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
DWriteContext::سیٹ_Font_Features(
	ثابت DWriteFontFeature *features, صحيح گنتی)
{
    إذا (گنتی > DWRITE_MAX_FONT_FEATURES)
	گنتی = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_گنتی = گنتی;
    إذا (گنتی > 0 && features != عدم)
	حافظہ_نقل(mFontFeatures, features, حجم(DWriteFontFeature) * گنتی);
}

    فراغ
DWrite_Context_سیٹ_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح گنتی)
{
    إذا (ctx != عدم)
	ctx->سیٹ_Font_Features(features, گنتی);
}
