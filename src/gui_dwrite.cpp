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

#تعریف_کن WIN32_LEAN_AND_میانگین

#اگر_تعریف_نشده DYNAMIC_DIRECTX
# إذا WINVER < 0x0600
#  error WINVER must be 0x0600 or above مقصد use مستقیم_Write(مستقیم_X)
# endif
#پایان_اگر

#ضمن <windows.h>
#ضمن <crtdbg.h>
#ضمن <assert.h>
#ضمن <math.h>
#ضمن <d2d1.h>
#ضمن <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#إذا defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define درون_reads(s)
# define درون_reads_opt(s)
# define _Maybenull_
# define بیرون_writes(s)
# define بیرون_writes_opt(s)
# define بیرون_writes_مقصد(x, y)
# define بیرون_writes_مقصد_opt(x, y)
# define _Outptr_
#پایان_اگر

#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#وإلا
# include <dwrite.h>
#پایان_اگر

#ضمن "gui_dwrite.h"

#اگر_تعریف_شده __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_درون
# define __out		SAL_بیرون
#پایان_اگر

#اگر_تعریف_شده __MINGW32__
# define استفادهنشده __attribute__((استفادهنشده_2))
#وإلا
# define استفادهنشده
#پایان_اگر

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#پایان_اگر

#اگر_تعریف_شده DYNAMIC_DIRECTX
خارجي "C" HINSTANCE vim_بارگیری_Lib(ثابت حرف *name);

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

#تعریف_کن GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#تعریف_کن D2D1CreateFactory		(*pD2D1CreateFactory)
#تعریف_کن DWriteCreateFactory		(*pDWriteCreateFactory)

    ساكن فراغ
unload(HINSTANCE &hinst)
{
    إذا (hinst != عدم)
    {
	FreeLibrary(hinst);
	hinst = عدم;
    }
}
#پایان_اگر // DYNAMIC_DIRECTX

قالب <صنف T> مضمن فراغ امن_Release(T **ppT)
{
    إذا (*ppT)
    {
	(*ppT)->Release();
	*ppT = عدم;
    }
}

    ساكن DWRITE_PIXEL_GEOMETRY
مقصد_Pixel_Geometry(صحيح مقدار)
{
    بدل (مقدار)
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
مقصد_Int(DWRITE_PIXEL_GEOMETRY مقدار)
{
    بدل (مقدار)
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

    ساكن DWRITE_RENDERING_نما
مقصد_Rendering_نما(صحيح مقدار)
{
    بدل (مقدار)
    {
	افتراضي:
	حالة 0:
	    أرجع DWRITE_RENDERING_نما_DEFAULT;
	حالة 1:
	    أرجع DWRITE_RENDERING_نما_ALIASED;
	حالة 2:
	    أرجع DWRITE_RENDERING_نما_CLEARTYPE_GDI_CLASSIC;
	حالة 3:
	    أرجع DWRITE_RENDERING_نما_CLEARTYPE_GDI_NATURAL;
	حالة 4:
	    أرجع DWRITE_RENDERING_نما_CLEARTYPE_NATURAL;
	حالة 5:
	    أرجع DWRITE_RENDERING_نما_CLEARTYPE_NATURAL_متقارن;
	حالة 6:
	    أرجع DWRITE_RENDERING_نما_OUTLINE;
    }
}

    ساكن D2_D1_متن_ANTIALIAS_نما
مقصد_متن_Antialias_نما(صحيح مقدار)
{
    بدل (مقدار)
    {
	افتراضي:
	حالة 0:
	    أرجع D2_D1_متن_ANTIALIAS_نما_DEFAULT;
	حالة 1:
	    أرجع D2_D1_متن_ANTIALIAS_نما_CLEARTYPE;
	حالة 2:
	    أرجع D2_D1_متن_ANTIALIAS_نما_GRAYSCALE;
	حالة 3:
	    أرجع D2_D1_متن_ANTIALIAS_نما_ALIASED;
    }
}

    ساكن صحيح
مقصد_Int(DWRITE_RENDERING_نما مقدار)
{
    بدل (مقدار)
    {
	حالة DWRITE_RENDERING_نما_DEFAULT:
	    أرجع 0;
	حالة DWRITE_RENDERING_نما_ALIASED:
	    أرجع 1;
	حالة DWRITE_RENDERING_نما_CLEARTYPE_GDI_CLASSIC:
	    أرجع 2;
	حالة DWRITE_RENDERING_نما_CLEARTYPE_GDI_NATURAL:
	    أرجع 3;
	حالة DWRITE_RENDERING_نما_CLEARTYPE_NATURAL:
	    أرجع 4;
	حالة DWRITE_RENDERING_نما_CLEARTYPE_NATURAL_متقارن:
	    أرجع 5;
	حالة DWRITE_RENDERING_نما_OUTLINE:
	    أرجع 6;
	افتراضي:
	    أرجع -1;
    }
}

صنف FontCache {
عام:
    بنية قلم_2 {
	HFONT              hFont;
	IDWrite_متن_Format* p_متن_Format;
	DWRITE_FONT_وزن font_وزن;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	قلم_2() : hFont(عدم), p_متن_Format(عدم), fontAscent(0.0f) {}
    };

خاص:
    صحيح m_اندازه;
    قلم_2 *m_اقلام;

عام:
    FontCache(صحيح اندازه = 2) :
	m_اندازه(اندازه),
	m_اقلام(جديد قلم_2[اندازه])
    {
    }

    ~FontCache()
    {
	لكل (صحيح i = 0; i < m_اندازه; ++i)
	    امن_Release(&m_اقلام[i].p_متن_Format);
	حذف[] m_اقلام;
    }

    منطقي get(HFONT hFont, قلم_2 &قلم)
    {
	صحيح n = پیدا_کن(hFont);
	إذا (n < 0)
	    أرجع خطأ;
	قلم = m_اقلام[n];
	slide(n);
	أرجع درست;
    }

    فراغ put(ثابت قلم_2& قلم)
    {
	صحيح n = پیدا_کن(قلم.hFont);
	إذا (n < 0)
	    n = m_اندازه - 1;
	إذا (m_اقلام[n].p_متن_Format != قلم.p_متن_Format)
	{
	    امن_Release(&m_اقلام[n].p_متن_Format);
	    إذا (قلم.p_متن_Format != عدم)
		قلم.p_متن_Format->جمع_کن_Ref();
	}
	m_اقلام[n] = قلم;
	slide(n);
    }

خاص:
    صحيح پیدا_کن(HFONT hFont)
    {
	لكل (صحيح i = 0; i < m_اندازه; ++i)
	{
	    إذا (m_اقلام[i].hFont == hFont)
		أرجع i;
	}
	أرجع -1;
    }

    فراغ slide(صحيح بعدی_بالاترین)
    {
	إذا (بعدی_بالاترین == 0)
	    أرجع;
	قلم_2 tmp = m_اقلام[بعدی_بالاترین];
	لكل (صحيح i = بعدی_بالاترین - 1; i >= 0; --i)
	    m_اقلام[i + 1] = m_اقلام[i];
	m_اقلام[0] = tmp;
    }
};

تعداد Drawing_نما {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

بنية DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_نما mDMode;
    HDC mInteropHDC;
    منطقي mDrawing;
    منطقي mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_هدف *mRT;
    ID2_D1_Gdi_Interop_Render_هدف *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#پایان_اگر

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_متن_Format *m_متن_Format;
    DWRITE_FONT_وزن m_Font_وزن;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_اندازه;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_متن_ANTIALIAS_نما m_متن_Antialias_نما;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    صحيح m_Font_Feature_شمار;

    // METHODS

    DWriteContext();

    مجازی ~DWriteContext();

    HRESULT CreateDeviceResources();

    فراغ DiscardDeviceResources();

    HRESULT Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	    IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent);

    HRESULT مجموعه_Font_By_LOGFONT(ثابت LOGFONTW &logFont);

    فراغ مجموعه_Font(HFONT hFont);

    فراغ Rebind();

    فراغ BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT مجموعه_Drawing_نما(Drawing_نما نما);

    ID2D1Brush* SolidBrush(COLORREF color);

    فراغ Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_عرض_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    فراغ FillRect(ثابت RECT *rc, COLORREF color);

    فراغ Draw_سطر(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color);

    فراغ مجموعه_Pixel(صحيح x, صحيح y, COLORREF color);

    فراغ Scroll(صحيح x, صحيح y, ثابت RECT *rc);

    فراغ Flush();

    فراغ مجموعه_Rendering_Params(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    فراغ مجموعه_Font_Features(ثابت DWriteFontFeature *features, صحيح شمار);
};

صنف Adjusted_Glyph_پیاپی : عام DWRITE_GLYPH_پیاپی
{
خاص:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عام:
    Adjusted_Glyph_پیاپی(
	    ثابت DWRITE_GLYPH_پیاپی *glyph_پیاپی,
	    FLOAT cell_عرض_2,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحيح *glyph_شاخص,
	    صحيح cch_متن) :
	DWRITE_GLYPH_پیاپی(*glyph_پیاپی),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جديد FLOAT[glyph_پیاپی->glyph_شمار])
    {
	أكد(cell_عرض_2 != 0.0f);
	لكل (UINT32 i = 0; i < glyph_پیاپی->glyph_شمار; ++i)
	{
	    FLOAT orig = glyph_پیاپی->glyphAdvances[i];
	    FLOAT adjusted;

	    إذا (*glyph_شاخص < cch_متن)
	    {
		adjusted = FLOAT(lpDx[*glyph_شاخص]);
		(*glyph_شاخص)++;
	    }
	    وإلا
	    {
		adjusted = adjust_مقصد_Cell(orig, cell_عرض_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_پیاپی()
    {
	mAccum += mDelta;
	حذف[] mAdjustedAdvances;
    }

    ساكن FLOAT adjust_مقصد_Cell(FLOAT مقدار, FLOAT cell_عرض_2)
    {
	صحيح cell_شمار = صحيح(floor(مقدار / cell_عرض_2 + 0.5f));
	إذا (cell_شمار < 1)
	    cell_شمار = 1;
	أرجع cell_شمار * cell_عرض_2;
    }
};

بنية متن_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_عرض_2;
    ثابت INT *lpDx;
    صحيح cch_متن;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT جابه_جایی_X;
    صحيح glyph_شاخص;
};

صنف متن_Renderer FINAL : عام IDWrite_متن_Renderer
{
عام:
    متن_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_شمار(0),
	pDWC_(pDWC)
    {
	جمع_کن_Ref();
    }

    // add "virtual" to avoid a compiler warning
    مجازی ~متن_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	أرجع S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	__out DWRITE_ماتریس* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_ماتریس_3_X2_F*>(transform));
	أرجع S_OK;
    }

    IFACEMETHOD(Get_Pixels_در_هر_Dip)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	__out FLOAT* pixels_در_هر_Dip)
    {
	عائم dpiX, استفادهنشده_2;
	pDWC_->mRT->GetDpi(&dpiX, &استفادهنشده_2);
	*pixels_در_هر_Dip = dpiX / 96.0f;
	أرجع S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	FLOAT baselineOriginX استفادهنشده,
	FLOAT baselineOriginY استفادهنشده,
	__in DWRITE_UNDERLINE ثابت* underline استفادهنشده,
	IUnknown* clientDrawingEffect استفادهنشده)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	FLOAT baselineOriginX استفادهنشده,
	FLOAT baselineOriginY استفادهنشده,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough استفادهنشده,
	IUnknown* clientDrawingEffect استفادهنشده)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull فراغ* clientDrawingContext استفادهنشده,
	FLOAT originX استفادهنشده,
	FLOAT originY استفادهنشده,
	IDWriteInlineObject* inlineObject استفادهنشده,
	BOOL isSideways استفادهنشده,
	BOOL is_راست_مقصد_چپ استفادهنشده,
	IUnknown* clientDrawingEffect استفادهنشده)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_پیاپی)(
	__maybenull فراغ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_نما measuring_نما استفادهنشده,
	__in DWRITE_GLYPH_پیاپی ثابت* glyph_پیاپی,
	__in DWRITE_GLYPH_پیاپی_DESCRIPTION ثابت* glyph_پیاپی_Description استفادهنشده,
	IUnknown* clientDrawingEffect استفادهنشده)
    {
	متن_Renderer_Context *context =
	    reinterpret_cast<متن_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_پیاپی adjusted_Glyph_پیاپی(glyph_پیاپی, context->cell_عرض_2,
		context->جابه_جایی_X, context->lpDx, &context->glyph_شاخص,
		context->cch_متن);

#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
	إذا (pDWC_->mDWriteFactory2 != عدم)
	{
	    IDWrite_Color_Glyph_پیاپی_Enumerator *enumerator = عدم;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_پیاپی(
		baselineOriginX + context->جابه_جایی_X,
		baselineOriginY,
		&adjusted_Glyph_پیاپی,
		عدم,
		DWRITE_MEASURING_نما_GDI_NATURAL,
		عدم,
		0,
		&enumerator);
	    إذا (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_پیاپی = TRUE;
		enumerator->Move_بعدی(&has_پیاپی);
		طالما (has_پیاپی)
		{
		    ثابت DWRITE_COLOR_GLYPH_پیاپی* color_Glyph_پیاپی;
		    enumerator->Get_Current_پیاپی(&color_Glyph_پیاپی);

		    pDWC_->mBrush->مجموعه_Color(color_Glyph_پیاپی->پیاپی_Color);
		    pDWC_->mRT->Draw_Glyph_پیاپی(
			    D2D1::Point2F(
				color_Glyph_پیاپی->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_پیاپی->glyph_پیاپی,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_نما_NATURAL);
		    enumerator->Move_بعدی(&has_پیاپی);
		}
		امن_Release(&enumerator);
		أرجع S_OK;
	    }
	}
#پایان_اگر

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_پیاپی(
		D2D1::Point2F(
		    baselineOriginX + context->جابه_جایی_X,
		    fixedBaselineY),
		&adjusted_Glyph_پیاپی,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_نما_NATURAL);
	أرجع S_OK;
    }

عام:
    IFACEMETHOD_(بیعلامت دراز, جمع_کن_Ref) ()
    {
	أرجع InterlockedIncrement(&c_Ref_شمار);
    }

    IFACEMETHOD_(بیعلامت دراز, Release) ()
    {
	دراز new_شمار_2 = InterlockedDecrement(&c_Ref_شمار);

	إذا (new_شمار_2 == 0)
	{
	    حذف هذا;
	    أرجع 0;
	}
	أرجع new_شمار_2;
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
    دراز c_Ref_شمار;
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
#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(عدم),
#پایان_اگر
    mGdiInterop(عدم),
    mRenderingParams(عدم),
    mFontCache(8),
    m_متن_Format(عدم),
    m_Font_وزن(DWRITE_FONT_وزن_نرمال),
    mFontStyle(DWRITE_FONT_STYLE_نرمال),
    m_Font_اندازه(0.0f),
    mFontAscent(0.0f),
    m_متن_Antialias_نما(D2_D1_متن_ANTIALIAS_نما_DEFAULT),
    m_Font_Feature_شمار(0)
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

#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
    إذا (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#پایان_اگر

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
    امن_Release(&m_متن_Format);
    امن_Release(&mRenderingParams);
    امن_Release(&mGdiInterop);
    امن_Release(&mDWriteFactory);
#اگر_تعریف_شده FEAT_DIRECTX_COLOR_EMOJI
    امن_Release(&mDWriteFactory2);
#پایان_اگر
    امن_Release(&mBitmap);
    امن_Release(&mBrush);
    امن_Release(&mGDIRT);
    امن_Release(&mRT);
    امن_Release(&mD2D1Factory);
}

    HRESULT
DWriteContext::CreateDeviceResources()
{
    HRESULT hr;

    إذا (mRT != عدم)
	أرجع S_OK;

    D2_D1_RENDER_هدف_PROPERTIES props = {
	D2_D1_RENDER_هدف_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_نما_IGNORE },
	0, 0,
	D2_D1_RENDER_هدف_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_هدف(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    إذا (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_هدف),
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
    امن_Release(&mBitmap);
    امن_Release(&mBrush);
    امن_Release(&mGDIRT);
    امن_Release(&mRT);
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
    IDWrite_Localized_رشتهها *localized_Family_نامها = عدم;
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
	hr = fontFamily->Get_Family_نامها(&localized_Family_نامها);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    إذا (SUCCEEDED(hr))
    {
	hr = localized_Family_نامها->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    إذا (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_اندازه = عائم(logFont.lf_ارتفاع);

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
	    font_اندازه = font_اندازه * عائم(fontMetrics.design_Units_در_هر_Em)
		/ عائم(fontMetrics.ascent + fontMetrics.descent + fontMetrics.سطر_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_درازی];
    إذا (SUCCEEDED(hr))
    {
	إذا (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_درازی) == 0)
	    hr = HRESULT_FROM_WIN32(Get_آخرین_Error());
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
	hr = p_متن_Format->مجموعه_متن_Alignment(DWRITE_متن_ALIGNMENT_LEADING);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->مجموعه_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->مجموعه_کلمه_Wrapping(DWRITE_کلمه_WRAPPING_NO_WRAP);

    امن_Release(&localized_Family_نامها);
    امن_Release(&fontFamily);
    امن_Release(&font);

    إذا (SUCCEEDED(hr))
    {
	*pp_متن_Format = p_متن_Format;
	إذا (pFontAscent != عدم && fontMetrics.design_Units_در_هر_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT مجموع_ارتفاع = عائم(fontMetrics.ascent + fontMetrics.descent);
	    إذا (مجموع_ارتفاع != 0.0f)
		*pFontAscent = عائم(fontMetrics.ascent) / مجموع_ارتفاع;
	    وإلا
		*pFontAscent = 0.83f; // fallback
	}
    }
    وإلا
	امن_Release(&p_متن_Format);

    أرجع hr;
}

    HRESULT
DWriteContext::مجموعه_Font_By_LOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = عدم;
    FLOAT fontAscent = 0.0f;

    hr = Create_متن_Format_From_LOGFONT(logFont, &p_متن_Format, &fontAscent);

    إذا (SUCCEEDED(hr))
    {
	امن_Release(&m_متن_Format);
	m_متن_Format = p_متن_Format;
	m_Font_وزن = static_cast<DWRITE_FONT_وزن>(logFont.lf_وزن);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_نرمال;
	mFontAscent = fontAscent;
    }

    أرجع hr;
}

    فراغ
DWriteContext::مجموعه_Font(HFONT hFont)
{
    FontCache::قلم_2 قلم;
    إذا (mFontCache.get(hFont, قلم))
    {
	إذا (قلم.p_متن_Format != عدم)
	{
	    قلم.p_متن_Format->جمع_کن_Ref();
	    امن_Release(&m_متن_Format);
	    m_متن_Format = قلم.p_متن_Format;
	    m_Font_وزن = قلم.font_وزن;
	    mFontStyle = قلم.fontStyle;
	    mFontAscent = قلم.fontAscent;
	    mFallbackDC = خطأ;
	}
	وإلا
	    mFallbackDC = درست;
	أرجع;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    إذا (GetObjectW(hFont, حجم(lf), &lf))
	hr = مجموعه_Font_By_LOGFONT(lf);

    قلم.hFont = hFont;
    إذا (SUCCEEDED(hr))
    {
	قلم.p_متن_Format = m_متن_Format;
	قلم.font_وزن = m_Font_وزن;
	قلم.fontStyle = mFontStyle;
	قلم.fontAscent = mFontAscent;
	mFallbackDC = خطأ;
    }
    وإلا
	mFallbackDC = درست;
    mFontCache.put(قلم);
}

    فراغ
DWriteContext::Rebind()
{
    امن_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->مجموعه_Transform(D2D1::عنصر_خنثی_ماتریس());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_نما_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::اندازه_U(mBindRect.راست - mBindRect.چپ,
		mBindRect.bottom - mBindRect.بالاترین),
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
DWriteContext::مجموعه_Drawing_نما(Drawing_نما نما)
{
    HRESULT hr = S_OK;

    بدل (نما)
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
		hr = mRT->انتها_Draw();
		إذا (hr == (HRESULT)D2_DERR_RECREATE_هدف)
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
		mDrawing = درست;
	    }
	    اكسر;

	حالة DM_INTEROP:
	    إذا (mDrawing == خطأ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = درست;
	    }
	    إذا (mInteropHDC == عدم)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_نما_COPY, &mInteropHDC);
	    اكسر;
    }
    mDMode = نما;
    أرجع hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->مجموعه_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    أرجع mBrush;
}

    فراغ
DWriteContext::Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_عرض_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    إذا (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = مجموعه_Drawing_نما(DM_INTEROP);
	إذا (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::مجموعه_متن_Color(mInteropHDC, color);
	    ::مجموعه_Bk_نما(mInteropHDC, ::Get_Bk_نما(mHDC));
	    ::Ext_متن_بیرون_W(mInteropHDC, x, y, fuOptions, lprc, متن, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	أرجع;
    }

    HRESULT hr;
    IDWrite_متن_Layout *متن_Layout = عدم;

    مجموعه_Drawing_نما(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
    {
	mRT->افزودن_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->چپ), FLOAT(lprc->بالاترین),
			FLOAT(lprc->راست), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_نما_ALIASED);
    }

    hr = mDWriteFactory->Create_متن_Layout(متن, len, m_متن_Format,
	    FLOAT(w), FLOAT(h), &متن_Layout);

    إذا (SUCCEEDED(hr))
    {
	DWRITE_متن_RANGE متن_Range = { 0, UINT32(len) };
	متن_Layout->مجموعه_Font_وزن(m_Font_وزن, متن_Range);
	متن_Layout->مجموعه_Font_Style(mFontStyle, متن_Range);

	إذا (m_Font_Feature_شمار > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساكن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_برچسب_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_برچسب_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_برچسب_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_برچسب_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_برچسب_KERNING, 1 },
	    };
	    ساكن ثابت صحيح رقم_Defaults = حجم(defaultFeatures)
		/ حجم(defaultFeatures[0]);

	    IDWriteTypography *typography = عدم;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    إذا (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		لكل (صحيح d = 0; d < رقم_Defaults; ++d)
		{
		    صحيح overridden = 0;
		    لكل (صحيح u = 0; u < m_Font_Feature_شمار; ++u)
		    {
			إذا ((DWRITE_FONT_FEATURE_برچسب)mFontFeatures[u].برچسب
				== defaultFeatures[d].name_برچسب)
			{
			    overridden = 1;
			    اكسر;
			}
		    }
		    إذا (!overridden)
			typography->جمع_کن_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		لكل (صحيح i = 0; i < m_Font_Feature_شمار; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_برچسب)mFontFeatures[i].برچسب,
			mFontFeatures[i].parameter
		    };
		    typography->جمع_کن_Font_Feature(ff);
		}
		متن_Layout->مجموعه_Typography(typography, متن_Range);
		امن_Release(&typography);
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
	متن_Renderer_Context context = { color, FLOAT(cell_عرض_2), lpDx, len,
		baselineY, 0.0f, 0 };

	متن_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    امن_Release(&متن_Layout);

    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
	mRT->برداشت_Axis_Aligned_Clip();
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
	مجموعه_Drawing_نما(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->چپ), FLOAT(rc->بالاترین),
		    FLOAT(rc->راست), FLOAT(rc->bottom)),
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
	::Move_مقصد_Ex(mInteropHDC, x1, y1, عدم);
	::سطر_مقصد(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    وإلا
    {
	مجموعه_Drawing_نما(DM_DIRECTX);
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::مجموعه_Pixel(صحيح x, صحيح y, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::مجموعه_Pixel(mInteropHDC, x, y, color);
    }
    وإلا
    {
	مجموعه_Drawing_نما(DM_DIRECTX);
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
    مجموعه_Drawing_نما(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_نقطه_2_U dest_نقطه;
    إذا (x >= 0)
    {
	srcRect.چپ = rc->چپ;
	srcRect.راست = rc->راست - x;
	dest_نقطه.x = rc->چپ + x;
    }
    وإلا
    {
	srcRect.چپ = rc->چپ - x;
	srcRect.راست = rc->راست;
	dest_نقطه.x = rc->چپ;
    }
    إذا (y >= 0)
    {
	srcRect.بالاترین = rc->بالاترین;
	srcRect.bottom = rc->bottom - y;
	dest_نقطه.y = rc->بالاترین + y;
    }
    وإلا
    {
	srcRect.بالاترین = rc->بالاترین - y;
	srcRect.bottom = rc->bottom;
	dest_نقطه.y = rc->بالاترین;
    }
    mBitmap->Copy_From_Render_هدف(&dest_نقطه, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_نقطه.x), FLOAT(dest_نقطه.y),
	    FLOAT(dest_نقطه.x + srcRect.راست - srcRect.چپ),
	    FLOAT(dest_نقطه.y + srcRect.bottom - srcRect.بالاترین)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_نما_نزدیکترین_NEIGHBOR, destRect);
}

    فراغ
DWriteContext::Flush()
{
    مجموعه_Drawing_نما(DM_GDI);
}

    فراغ
DWriteContext::مجموعه_Rendering_Params(
	ثابت DWriteRenderingParams *params)
{
    إذا (mDWriteFactory == عدم)
	أرجع;

    IDWriteRenderingParams *renderingParams = عدم;
    D2_D1_متن_ANTIALIAS_نما متن_Antialias_نما =
	D2_D1_متن_ANTIALIAS_نما_DEFAULT;
    HRESULT hr;
    إذا (params != عدم)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		مقصد_Pixel_Geometry(params->pixelGeometry),
		مقصد_Rendering_نما(params->rendering_نما), &renderingParams);
	متن_Antialias_نما = مقصد_متن_Antialias_نما(params->متن_Antialias_نما);
    }
    وإلا
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    إذا (SUCCEEDED(hr) && renderingParams != عدم)
    {
	امن_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_متن_Antialias_نما = متن_Antialias_نما;

	Flush();
	mRT->مجموعه_متن_Rendering_Params(mRenderingParams);
	mRT->مجموعه_متن_Antialias_نما(m_متن_Antialias_نما);
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
	params->pixelGeometry = مقصد_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_نما = مقصد_Int(mRenderingParams->Get_Rendering_نما());
	params->متن_Antialias_نما = m_متن_Antialias_نما;
    }
    أرجع params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    فراغ
DWrite_Init(فراغ)
{
#اگر_تعریف_شده DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_بارگیری_Lib("d2d1.dll");
    hDWriteDLL = vim_بارگیری_Lib("dwrite.dll");
    إذا (hD2D1DLL == عدم || hDWriteDLL == عدم)
    {
	DWrite_Final();
	أرجع;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_آدرس(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_آدرس(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_آدرس(hDWriteDLL,
	    "DWriteCreateFactory");
#پایان_اگر
}

    فراغ
DWrite_Final(فراغ)
{
#اگر_تعریف_شده DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = عدم;
    pD2D1CreateFactory = عدم;
    pDWriteCreateFactory = عدم;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#پایان_اگر
}

    DWriteContext *
DWriteContext_Open(فراغ)
{
#اگر_تعریف_شده DYNAMIC_DIRECTX
    إذا (pGetUserDefaultLocaleName == عدم || pD2D1CreateFactory == عدم
	    || pDWriteCreateFactory == عدم)
	أرجع عدم;
#پایان_اگر
    أرجع جديد DWriteContext();
}

    فراغ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    إذا (ctx != عدم)
	ctx->BindDC(hdc, rect);
}

    فراغ
DWrite_Context_مجموعه_Font(DWriteContext *ctx, HFONT hFont)
{
    إذا (ctx != عدم)
	ctx->مجموعه_Font(hFont);
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
	صحيح cell_عرض_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    إذا (ctx != عدم)
	ctx->Draw_متن(متن, len, x, y, w, h, cell_عرض_2, color,
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
DWrite_Context_مجموعه_Pixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->مجموعه_Pixel(x, y, color);
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
    حذف ctx;
}

    فراغ
DWrite_Context_مجموعه_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    إذا (ctx != عدم)
	ctx->مجموعه_Rendering_Params(params);
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
DWriteContext::مجموعه_Font_Features(
	ثابت DWriteFontFeature *features, صحيح شمار)
{
    إذا (شمار > DWRITE_MAX_FONT_FEATURES)
	شمار = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_شمار = شمار;
    إذا (شمار > 0 && features != عدم)
	کپی_حافظه(mFontFeatures, features, حجم(DWriteFontFeature) * شمار);
}

    فراغ
DWrite_Context_مجموعه_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح شمار)
{
    إذا (ctx != عدم)
	ctx->مجموعه_Font_Features(features, شمار);
}
