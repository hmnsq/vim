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

#وصف_ڪريو WIN32_LEAN_AND_سراسري

#جيڪڏهن_وصف_نه_ٿيل DYNAMIC_DIRECTX
# إذا WINVER < 0x0600
#  error WINVER must be 0x0600 or above منزل use سڌو_Write(سڌو_X)
# endif
#جيڪڏهن_پورو

#ضمن <windows.h>
#ضمن <crtdbg.h>
#ضمن <assert.h>
#ضمن <math.h>
#ضمن <d2d1.h>
#ضمن <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#إذا defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define اندريون_reads(s)
# define اندريون_reads_opt(s)
# define _Maybenull_
# define ٻاهريون_writes(s)
# define ٻاهريون_writes_opt(s)
# define ٻاهريون_writes_منزل(x, y)
# define ٻاهريون_writes_منزل_opt(x, y)
# define _Outptr_
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#وإلا
# include <dwrite.h>
#جيڪڏهن_پورو

#ضمن "gui_dwrite.h"

#جيڪڏهن_وصف_ٿيل __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_اندريون
# define __out		SAL_ٻاهريون
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __MINGW32__
# define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#وإلا
# define اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DYNAMIC_DIRECTX
خارجي "C" HINSTANCE vim_ڀريو_Lib(ثابت حرف *name);

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

#وصف_ڪريو GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#وصف_ڪريو D2D1CreateFactory		(*pD2D1CreateFactory)
#وصف_ڪريو DWriteCreateFactory		(*pDWriteCreateFactory)

    ساكن فراغ
unload(HINSTANCE &hinst)
{
    إذا (hinst != عدم)
    {
	FreeLibrary(hinst);
	hinst = عدم;
    }
}
#جيڪڏهن_پورو // DYNAMIC_DIRECTX

سانچو <صنف T> مضمن فراغ محفوظ_Release(T **ppT)
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

    ساكن DWRITE_RENDERING_گهڻائي
منزل_Rendering_گهڻائي(صحيح قدر)
{
    بدل (قدر)
    {
	افتراضي:
	حالة 0:
	    أرجع DWRITE_RENDERING_گهڻائي_DEFAULT;
	حالة 1:
	    أرجع DWRITE_RENDERING_گهڻائي_ALIASED;
	حالة 2:
	    أرجع DWRITE_RENDERING_گهڻائي_CLEARTYPE_GDI_CLASSIC;
	حالة 3:
	    أرجع DWRITE_RENDERING_گهڻائي_CLEARTYPE_GDI_NATURAL;
	حالة 4:
	    أرجع DWRITE_RENDERING_گهڻائي_CLEARTYPE_NATURAL;
	حالة 5:
	    أرجع DWRITE_RENDERING_گهڻائي_CLEARTYPE_NATURAL_متناظر;
	حالة 6:
	    أرجع DWRITE_RENDERING_گهڻائي_OUTLINE;
    }
}

    ساكن D2_D1_متن_ANTIALIAS_گهڻائي
منزل_متن_Antialias_گهڻائي(صحيح قدر)
{
    بدل (قدر)
    {
	افتراضي:
	حالة 0:
	    أرجع D2_D1_متن_ANTIALIAS_گهڻائي_DEFAULT;
	حالة 1:
	    أرجع D2_D1_متن_ANTIALIAS_گهڻائي_CLEARTYPE;
	حالة 2:
	    أرجع D2_D1_متن_ANTIALIAS_گهڻائي_GRAYSCALE;
	حالة 3:
	    أرجع D2_D1_متن_ANTIALIAS_گهڻائي_ALIASED;
    }
}

    ساكن صحيح
منزل_Int(DWRITE_RENDERING_گهڻائي قدر)
{
    بدل (قدر)
    {
	حالة DWRITE_RENDERING_گهڻائي_DEFAULT:
	    أرجع 0;
	حالة DWRITE_RENDERING_گهڻائي_ALIASED:
	    أرجع 1;
	حالة DWRITE_RENDERING_گهڻائي_CLEARTYPE_GDI_CLASSIC:
	    أرجع 2;
	حالة DWRITE_RENDERING_گهڻائي_CLEARTYPE_GDI_NATURAL:
	    أرجع 3;
	حالة DWRITE_RENDERING_گهڻائي_CLEARTYPE_NATURAL:
	    أرجع 4;
	حالة DWRITE_RENDERING_گهڻائي_CLEARTYPE_NATURAL_متناظر:
	    أرجع 5;
	حالة DWRITE_RENDERING_گهڻائي_OUTLINE:
	    أرجع 6;
	افتراضي:
	    أرجع -1;
    }
}

صنف FontCache {
عام:
    بنية شي_2 {
	HFONT              hFont;
	IDWrite_متن_Format* p_متن_Format;
	DWRITE_FONT_وزن font_وزن;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	شي_2() : hFont(عدم), p_متن_Format(عدم), fontAscent(0.0f) {}
    };

خاص:
    صحيح m_سائيز;
    شي_2 *m_شيون;

عام:
    FontCache(صحيح سائيز = 2) :
	m_سائيز(سائيز),
	m_شيون(جديد شي_2[سائيز])
    {
    }

    ~FontCache()
    {
	لكل (صحيح i = 0; i < m_سائيز; ++i)
	    محفوظ_Release(&m_شيون[i].p_متن_Format);
	ميٽيو[] m_شيون;
    }

    منطقي get(HFONT hFont, شي_2 &شي)
    {
	صحيح n = ڳوليو(hFont);
	إذا (n < 0)
	    أرجع خطأ;
	شي = m_شيون[n];
	slide(n);
	أرجع سچ;
    }

    فراغ put(ثابت شي_2& شي)
    {
	صحيح n = ڳوليو(شي.hFont);
	إذا (n < 0)
	    n = m_سائيز - 1;
	إذا (m_شيون[n].p_متن_Format != شي.p_متن_Format)
	{
	    محفوظ_Release(&m_شيون[n].p_متن_Format);
	    إذا (شي.p_متن_Format != عدم)
		شي.p_متن_Format->جوڙيو_Ref();
	}
	m_شيون[n] = شي;
	slide(n);
    }

خاص:
    صحيح ڳوليو(HFONT hFont)
    {
	لكل (صحيح i = 0; i < m_سائيز; ++i)
	{
	    إذا (m_شيون[i].hFont == hFont)
		أرجع i;
	}
	أرجع -1;
    }

    فراغ slide(صحيح ايندڙ_چوٽي)
    {
	إذا (ايندڙ_چوٽي == 0)
	    أرجع;
	شي_2 tmp = m_شيون[ايندڙ_چوٽي];
	لكل (صحيح i = ايندڙ_چوٽي - 1; i >= 0; --i)
	    m_شيون[i + 1] = m_شيون[i];
	m_شيون[0] = tmp;
    }
};

تعداد Drawing_گهڻائي {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

بنية DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_گهڻائي mDMode;
    HDC mInteropHDC;
    منطقي mDrawing;
    منطقي mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_هدف *mRT;
    ID2_D1_Gdi_Interop_Render_هدف *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#جيڪڏهن_پورو

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_متن_Format *m_متن_Format;
    DWRITE_FONT_وزن m_Font_وزن;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_سائيز;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_متن_ANTIALIAS_گهڻائي m_متن_Antialias_گهڻائي;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    صحيح m_Font_Feature_ڳڻپ;

    // METHODS

    DWriteContext();

    مجازي ~DWriteContext();

    HRESULT CreateDeviceResources();

    فراغ DiscardDeviceResources();

    HRESULT Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	    IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent);

    HRESULT سيٽ_Font_By_LOGFONT(ثابت LOGFONTW &logFont);

    فراغ سيٽ_Font(HFONT hFont);

    فراغ Rebind();

    فراغ BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT سيٽ_Drawing_گهڻائي(Drawing_گهڻائي گهڻائي);

    ID2D1Brush* SolidBrush(COLORREF color);

    فراغ Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_ويڪر_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    فراغ FillRect(ثابت RECT *rc, COLORREF color);

    فراغ Draw_سٽ(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color);

    فراغ سيٽ_Pixel(صحيح x, صحيح y, COLORREF color);

    فراغ Scroll(صحيح x, صحيح y, ثابت RECT *rc);

    فراغ Flush();

    فراغ سيٽ_Rendering_Params(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    فراغ سيٽ_Font_Features(ثابت DWriteFontFeature *features, صحيح ڳڻپ);
};

صنف Adjusted_Glyph_لاڳيتو : عام DWRITE_GLYPH_لاڳيتو
{
خاص:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عام:
    Adjusted_Glyph_لاڳيتو(
	    ثابت DWRITE_GLYPH_لاڳيتو *glyph_لاڳيتو,
	    FLOAT cell_ويڪر_2,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحيح *glyph_اشاريو,
	    صحيح cch_متن) :
	DWRITE_GLYPH_لاڳيتو(*glyph_لاڳيتو),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(جديد FLOAT[glyph_لاڳيتو->glyph_ڳڻپ])
    {
	أكد(cell_ويڪر_2 != 0.0f);
	لكل (UINT32 i = 0; i < glyph_لاڳيتو->glyph_ڳڻپ; ++i)
	{
	    FLOAT orig = glyph_لاڳيتو->glyphAdvances[i];
	    FLOAT adjusted;

	    إذا (*glyph_اشاريو < cch_متن)
	    {
		adjusted = FLOAT(lpDx[*glyph_اشاريو]);
		(*glyph_اشاريو)++;
	    }
	    وإلا
	    {
		adjusted = adjust_منزل_Cell(orig, cell_ويڪر_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_لاڳيتو()
    {
	mAccum += mDelta;
	ميٽيو[] mAdjustedAdvances;
    }

    ساكن FLOAT adjust_منزل_Cell(FLOAT قدر, FLOAT cell_ويڪر_2)
    {
	صحيح cell_ڳڻپ = صحيح(floor(قدر / cell_ويڪر_2 + 0.5f));
	إذا (cell_ڳڻپ < 1)
	    cell_ڳڻپ = 1;
	أرجع cell_ڳڻپ * cell_ويڪر_2;
    }
};

بنية متن_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_ويڪر_2;
    ثابت INT *lpDx;
    صحيح cch_متن;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT وٿي_X;
    صحيح glyph_اشاريو;
};

صنف متن_Renderer FINAL : عام IDWrite_متن_Renderer
{
عام:
    متن_Renderer(
	    DWriteContext* pDWC) :
	c_Ref_ڳڻپ(0),
	pDWC_(pDWC)
    {
	جوڙيو_Ref();
    }

    // add "virtual" to avoid a compiler warning
    مجازي ~متن_Renderer()
    {
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	أرجع S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	__out DWRITE_ميٽرڪس* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_ميٽرڪس_3_X2_F*>(transform));
	أرجع S_OK;
    }

    IFACEMETHOD(Get_Pixels_في_Dip)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	__out FLOAT* pixels_في_Dip)
    {
	عائم dpiX, اڻ_استعمال_ٿيل_2;
	pDWC_->mRT->GetDpi(&dpiX, &اڻ_استعمال_ٿيل_2);
	*pixels_في_Dip = dpiX / 96.0f;
	أرجع S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	FLOAT baselineOriginX اڻ_استعمال_ٿيل,
	FLOAT baselineOriginY اڻ_استعمال_ٿيل,
	__in DWRITE_UNDERLINE ثابت* underline اڻ_استعمال_ٿيل,
	IUnknown* clientDrawingEffect اڻ_استعمال_ٿيل)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	FLOAT baselineOriginX اڻ_استعمال_ٿيل,
	FLOAT baselineOriginY اڻ_استعمال_ٿيل,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough اڻ_استعمال_ٿيل,
	IUnknown* clientDrawingEffect اڻ_استعمال_ٿيل)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull فراغ* clientDrawingContext اڻ_استعمال_ٿيل,
	FLOAT originX اڻ_استعمال_ٿيل,
	FLOAT originY اڻ_استعمال_ٿيل,
	IDWriteInlineObject* inlineObject اڻ_استعمال_ٿيل,
	BOOL isSideways اڻ_استعمال_ٿيل,
	BOOL is_ساڄو_منزل_کاٻو اڻ_استعمال_ٿيل,
	IUnknown* clientDrawingEffect اڻ_استعمال_ٿيل)
    {
	أرجع E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_لاڳيتو)(
	__maybenull فراغ* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_گهڻائي measuring_گهڻائي اڻ_استعمال_ٿيل,
	__in DWRITE_GLYPH_لاڳيتو ثابت* glyph_لاڳيتو,
	__in DWRITE_GLYPH_لاڳيتو_DESCRIPTION ثابت* glyph_لاڳيتو_Description اڻ_استعمال_ٿيل,
	IUnknown* clientDrawingEffect اڻ_استعمال_ٿيل)
    {
	متن_Renderer_Context *context =
	    reinterpret_cast<متن_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_لاڳيتو adjusted_Glyph_لاڳيتو(glyph_لاڳيتو, context->cell_ويڪر_2,
		context->وٿي_X, context->lpDx, &context->glyph_اشاريو,
		context->cch_متن);

#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
	إذا (pDWC_->mDWriteFactory2 != عدم)
	{
	    IDWrite_Color_Glyph_لاڳيتو_Enumerator *enumerator = عدم;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_لاڳيتو(
		baselineOriginX + context->وٿي_X,
		baselineOriginY,
		&adjusted_Glyph_لاڳيتو,
		عدم,
		DWRITE_MEASURING_گهڻائي_GDI_NATURAL,
		عدم,
		0,
		&enumerator);
	    إذا (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_لاڳيتو = TRUE;
		enumerator->Move_ايندڙ(&has_لاڳيتو);
		طالما (has_لاڳيتو)
		{
		    ثابت DWRITE_COLOR_GLYPH_لاڳيتو* color_Glyph_لاڳيتو;
		    enumerator->Get_Current_لاڳيتو(&color_Glyph_لاڳيتو);

		    pDWC_->mBrush->سيٽ_Color(color_Glyph_لاڳيتو->لاڳيتو_Color);
		    pDWC_->mRT->Draw_Glyph_لاڳيتو(
			    D2D1::Point2F(
				color_Glyph_لاڳيتو->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_لاڳيتو->glyph_لاڳيتو,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_گهڻائي_NATURAL);
		    enumerator->Move_ايندڙ(&has_لاڳيتو);
		}
		محفوظ_Release(&enumerator);
		أرجع S_OK;
	    }
	}
#جيڪڏهن_پورو

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_لاڳيتو(
		D2D1::Point2F(
		    baselineOriginX + context->وٿي_X,
		    fixedBaselineY),
		&adjusted_Glyph_لاڳيتو,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_گهڻائي_NATURAL);
	أرجع S_OK;
    }

عام:
    IFACEMETHOD_(بنانشان ڊگهو, جوڙيو_Ref) ()
    {
	أرجع InterlockedIncrement(&c_Ref_ڳڻپ);
    }

    IFACEMETHOD_(بنانشان ڊگهو, Release) ()
    {
	ڊگهو new_ڳڻپ_2 = InterlockedDecrement(&c_Ref_ڳڻپ);

	إذا (new_ڳڻپ_2 == 0)
	{
	    ميٽيو هذا;
	    أرجع 0;
	}
	أرجع new_ڳڻپ_2;
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
    ڊگهو c_Ref_ڳڻپ;
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
#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(عدم),
#جيڪڏهن_پورو
    mGdiInterop(عدم),
    mRenderingParams(عدم),
    mFontCache(8),
    m_متن_Format(عدم),
    m_Font_وزن(DWRITE_FONT_وزن_عام),
    mFontStyle(DWRITE_FONT_STYLE_عام),
    m_Font_سائيز(0.0f),
    mFontAscent(0.0f),
    m_متن_Antialias_گهڻائي(D2_D1_متن_ANTIALIAS_گهڻائي_DEFAULT),
    m_Font_Feature_ڳڻپ(0)
{
    Zero_يادگيري(mFontFeatures, حجم(mFontFeatures));
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

#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
    إذا (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_ٿيل FEAT_DIRECTX_COLOR_EMOJI
    محفوظ_Release(&mDWriteFactory2);
#جيڪڏهن_پورو
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

    D2_D1_RENDER_هدف_PROPERTIES props = {
	D2_D1_RENDER_هدف_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_گهڻائي_IGNORE },
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
    IDWrite_Localized_لڙيون *localized_Family_نالا = عدم;
    عائم font_سائيز = 0;
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
	hr = fontFamily->Get_Family_نالا(&localized_Family_نالا);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    إذا (SUCCEEDED(hr))
    {
	hr = localized_Family_نالا->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    إذا (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_سائيز = عائم(logFont.lf_اوچائي);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	إذا (font_سائيز < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_سائيز = -font_سائيز;
	}
	وإلا إذا (font_سائيز > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_سائيز = font_سائيز * عائم(fontMetrics.design_Units_في_Em)
		/ عائم(fontMetrics.ascent + fontMetrics.descent + fontMetrics.سٽ_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_ڊيگهائي];
    إذا (SUCCEEDED(hr))
    {
	إذا (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_ڊيگهائي) == 0)
	    hr = HRESULT_FROM_WIN32(Get_آخري_Error());
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
		font_سائيز,
		localeName,
		&p_متن_Format);
    }

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سيٽ_متن_Alignment(DWRITE_متن_ALIGNMENT_LEADING);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سيٽ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    إذا (SUCCEEDED(hr))
	hr = p_متن_Format->سيٽ_لفظ_Wrapping(DWRITE_لفظ_WRAPPING_NO_WRAP);

    محفوظ_Release(&localized_Family_نالا);
    محفوظ_Release(&fontFamily);
    محفوظ_Release(&font);

    إذا (SUCCEEDED(hr))
    {
	*pp_متن_Format = p_متن_Format;
	إذا (pFontAscent != عدم && fontMetrics.design_Units_في_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT ڪل_اوچائي = عائم(fontMetrics.ascent + fontMetrics.descent);
	    إذا (ڪل_اوچائي != 0.0f)
		*pFontAscent = عائم(fontMetrics.ascent) / ڪل_اوچائي;
	    وإلا
		*pFontAscent = 0.83f; // fallback
	}
    }
    وإلا
	محفوظ_Release(&p_متن_Format);

    أرجع hr;
}

    HRESULT
DWriteContext::سيٽ_Font_By_LOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = عدم;
    FLOAT fontAscent = 0.0f;

    hr = Create_متن_Format_From_LOGFONT(logFont, &p_متن_Format, &fontAscent);

    إذا (SUCCEEDED(hr))
    {
	محفوظ_Release(&m_متن_Format);
	m_متن_Format = p_متن_Format;
	m_Font_وزن = static_cast<DWRITE_FONT_وزن>(logFont.lf_وزن);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_عام;
	mFontAscent = fontAscent;
    }

    أرجع hr;
}

    فراغ
DWriteContext::سيٽ_Font(HFONT hFont)
{
    FontCache::شي_2 شي;
    إذا (mFontCache.get(hFont, شي))
    {
	إذا (شي.p_متن_Format != عدم)
	{
	    شي.p_متن_Format->جوڙيو_Ref();
	    محفوظ_Release(&m_متن_Format);
	    m_متن_Format = شي.p_متن_Format;
	    m_Font_وزن = شي.font_وزن;
	    mFontStyle = شي.fontStyle;
	    mFontAscent = شي.fontAscent;
	    mFallbackDC = خطأ;
	}
	وإلا
	    mFallbackDC = سچ;
	أرجع;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    إذا (GetObjectW(hFont, حجم(lf), &lf))
	hr = سيٽ_Font_By_LOGFONT(lf);

    شي.hFont = hFont;
    إذا (SUCCEEDED(hr))
    {
	شي.p_متن_Format = m_متن_Format;
	شي.font_وزن = m_Font_وزن;
	شي.fontStyle = mFontStyle;
	شي.fontAscent = mFontAscent;
	mFallbackDC = خطأ;
    }
    وإلا
	mFallbackDC = سچ;
    mFontCache.put(شي);
}

    فراغ
DWriteContext::Rebind()
{
    محفوظ_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->سيٽ_Transform(D2D1::سڃاڻپ_عنصر_ميٽرڪس());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_گهڻائي_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::سائيز_U(mBindRect.ساڄي - mBindRect.کاٻي,
		mBindRect.bottom - mBindRect.چوٽي),
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
DWriteContext::سيٽ_Drawing_گهڻائي(Drawing_گهڻائي گهڻائي)
{
    HRESULT hr = S_OK;

    بدل (گهڻائي)
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
		hr = mRT->پڇاڙي_Draw();
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
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_گهڻائي_COPY, &mInteropHDC);
	    اكسر;
    }
    mDMode = گهڻائي;
    أرجع hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->سيٽ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    أرجع mBrush;
}

    فراغ
DWriteContext::Draw_متن(ثابت WCHAR *متن, صحيح len,
	صحيح x, صحيح y, صحيح w, صحيح h, صحيح cell_ويڪر_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    إذا (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = سيٽ_Drawing_گهڻائي(DM_INTEROP);
	إذا (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::سيٽ_متن_Color(mInteropHDC, color);
	    ::سيٽ_Bk_گهڻائي(mInteropHDC, ::Get_Bk_گهڻائي(mHDC));
	    ::Ext_متن_ٻاهريون_W(mInteropHDC, x, y, fuOptions, lprc, متن, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	أرجع;
    }

    HRESULT hr;
    IDWrite_متن_Layout *متن_Layout = عدم;

    سيٽ_Drawing_گهڻائي(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
    {
	mRT->ڌڪو_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->کاٻي), FLOAT(lprc->چوٽي),
			FLOAT(lprc->ساڄي), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_گهڻائي_ALIASED);
    }

    hr = mDWriteFactory->Create_متن_Layout(متن, len, m_متن_Format,
	    FLOAT(w), FLOAT(h), &متن_Layout);

    إذا (SUCCEEDED(hr))
    {
	DWRITE_متن_RANGE متن_Range = { 0, UINT32(len) };
	متن_Layout->سيٽ_Font_وزن(m_Font_وزن, متن_Range);
	متن_Layout->سيٽ_Font_Style(mFontStyle, متن_Range);

	إذا (m_Font_Feature_ڳڻپ > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساكن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_ٽيگ_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_ٽيگ_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٽيگ_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٽيگ_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_ٽيگ_KERNING, 1 },
	    };
	    ساكن ثابت صحيح انگ_Defaults = حجم(defaultFeatures)
		/ حجم(defaultFeatures[0]);

	    IDWriteTypography *typography = عدم;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    إذا (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		لكل (صحيح d = 0; d < انگ_Defaults; ++d)
		{
		    صحيح overridden = 0;
		    لكل (صحيح u = 0; u < m_Font_Feature_ڳڻپ; ++u)
		    {
			إذا ((DWRITE_FONT_FEATURE_ٽيگ)mFontFeatures[u].ٽيگ
				== defaultFeatures[d].name_ٽيگ)
			{
			    overridden = 1;
			    اكسر;
			}
		    }
		    إذا (!overridden)
			typography->جوڙيو_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		لكل (صحيح i = 0; i < m_Font_Feature_ڳڻپ; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_ٽيگ)mFontFeatures[i].ٽيگ,
			mFontFeatures[i].parameter
		    };
		    typography->جوڙيو_Font_Feature(ff);
		}
		متن_Layout->سيٽ_Typography(typography, متن_Range);
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
	متن_Renderer_Context context = { color, FLOAT(cell_ويڪر_2), lpDx, len,
		baselineY, 0.0f, 0 };

	متن_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    محفوظ_Release(&متن_Layout);

    إذا ((fuOptions & ETO_CLIPPED) && lprc != عدم)
	mRT->ڪڍو_Axis_Aligned_Clip();
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
	سيٽ_Drawing_گهڻائي(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->کاٻي), FLOAT(rc->چوٽي),
		    FLOAT(rc->ساڄي), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Draw_سٽ(صحيح x1, صحيح y1, صحيح x2, صحيح y2, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_منزل_Ex(mInteropHDC, x1, y1, عدم);
	::سٽ_منزل(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    وإلا
    {
	سيٽ_Drawing_گهڻائي(DM_DIRECTX);
	mRT->Draw_سٽ(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::سيٽ_Pixel(صحيح x, صحيح y, COLORREF color)
{
    إذا (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::سيٽ_Pixel(mInteropHDC, x, y, color);
    }
    وإلا
    {
	سيٽ_Drawing_گهڻائي(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_سٽ(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    فراغ
DWriteContext::Scroll(صحيح x, صحيح y, ثابت RECT *rc)
{
    سيٽ_Drawing_گهڻائي(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_نقطو_2_U dest_نقطو;
    إذا (x >= 0)
    {
	srcRect.کاٻي = rc->کاٻي;
	srcRect.ساڄي = rc->ساڄي - x;
	dest_نقطو.x = rc->کاٻي + x;
    }
    وإلا
    {
	srcRect.کاٻي = rc->کاٻي - x;
	srcRect.ساڄي = rc->ساڄي;
	dest_نقطو.x = rc->کاٻي;
    }
    إذا (y >= 0)
    {
	srcRect.چوٽي = rc->چوٽي;
	srcRect.bottom = rc->bottom - y;
	dest_نقطو.y = rc->چوٽي + y;
    }
    وإلا
    {
	srcRect.چوٽي = rc->چوٽي - y;
	srcRect.bottom = rc->bottom;
	dest_نقطو.y = rc->چوٽي;
    }
    mBitmap->Copy_From_Render_هدف(&dest_نقطو, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_نقطو.x), FLOAT(dest_نقطو.y),
	    FLOAT(dest_نقطو.x + srcRect.ساڄي - srcRect.کاٻي),
	    FLOAT(dest_نقطو.y + srcRect.bottom - srcRect.چوٽي)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_گهڻائي_سڀ_کان_ويجهو_NEIGHBOR, destRect);
}

    فراغ
DWriteContext::Flush()
{
    سيٽ_Drawing_گهڻائي(DM_GDI);
}

    فراغ
DWriteContext::سيٽ_Rendering_Params(
	ثابت DWriteRenderingParams *params)
{
    إذا (mDWriteFactory == عدم)
	أرجع;

    IDWriteRenderingParams *renderingParams = عدم;
    D2_D1_متن_ANTIALIAS_گهڻائي متن_Antialias_گهڻائي =
	D2_D1_متن_ANTIALIAS_گهڻائي_DEFAULT;
    HRESULT hr;
    إذا (params != عدم)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		منزل_Pixel_Geometry(params->pixelGeometry),
		منزل_Rendering_گهڻائي(params->rendering_گهڻائي), &renderingParams);
	متن_Antialias_گهڻائي = منزل_متن_Antialias_گهڻائي(params->متن_Antialias_گهڻائي);
    }
    وإلا
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    إذا (SUCCEEDED(hr) && renderingParams != عدم)
    {
	محفوظ_Release(&mRenderingParams);
	mRenderingParams = renderingParams;
	m_متن_Antialias_گهڻائي = متن_Antialias_گهڻائي;

	Flush();
	mRT->سيٽ_متن_Rendering_Params(mRenderingParams);
	mRT->سيٽ_متن_Antialias_گهڻائي(m_متن_Antialias_گهڻائي);
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
	params->rendering_گهڻائي = منزل_Int(mRenderingParams->Get_Rendering_گهڻائي());
	params->متن_Antialias_گهڻائي = m_متن_Antialias_گهڻائي;
    }
    أرجع params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    فراغ
DWrite_Init(فراغ)
{
#جيڪڏهن_وصف_ٿيل DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_ڀريو_Lib("d2d1.dll");
    hDWriteDLL = vim_ڀريو_Lib("dwrite.dll");
    إذا (hD2D1DLL == عدم || hDWriteDLL == عدم)
    {
	DWrite_Final();
	أرجع;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_پتو(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_پتو(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_پتو(hDWriteDLL,
	    "DWriteCreateFactory");
#جيڪڏهن_پورو
}

    فراغ
DWrite_Final(فراغ)
{
#جيڪڏهن_وصف_ٿيل DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = عدم;
    pD2D1CreateFactory = عدم;
    pDWriteCreateFactory = عدم;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#جيڪڏهن_پورو
}

    DWriteContext *
DWriteContext_Open(فراغ)
{
#جيڪڏهن_وصف_ٿيل DYNAMIC_DIRECTX
    إذا (pGetUserDefaultLocaleName == عدم || pD2D1CreateFactory == عدم
	    || pDWriteCreateFactory == عدم)
	أرجع عدم;
#جيڪڏهن_پورو
    أرجع جديد DWriteContext();
}

    فراغ
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    إذا (ctx != عدم)
	ctx->BindDC(hdc, rect);
}

    فراغ
DWrite_Context_سيٽ_Font(DWriteContext *ctx, HFONT hFont)
{
    إذا (ctx != عدم)
	ctx->سيٽ_Font(hFont);
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
	صحيح cell_ويڪر_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    إذا (ctx != عدم)
	ctx->Draw_متن(متن, len, x, y, w, h, cell_ويڪر_2, color,
		fuOptions, lprc, lpDx);
}

    فراغ
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->FillRect(rc, color);
}

    فراغ
DWrite_Context_Draw_سٽ(DWriteContext *ctx, صحيح x1, صحيح y1, صحيح x2, صحيح y2,
	COLORREF color)
{
    إذا (ctx != عدم)
	ctx->Draw_سٽ(x1, y1, x2, y2, color);
}

    فراغ
DWrite_Context_سيٽ_Pixel(DWriteContext *ctx, صحيح x, صحيح y, COLORREF color)
{
    إذا (ctx != عدم)
	ctx->سيٽ_Pixel(x, y, color);
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
    ميٽيو ctx;
}

    فراغ
DWrite_Context_سيٽ_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    إذا (ctx != عدم)
	ctx->سيٽ_Rendering_Params(params);
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
DWriteContext::سيٽ_Font_Features(
	ثابت DWriteFontFeature *features, صحيح ڳڻپ)
{
    إذا (ڳڻپ > DWRITE_MAX_FONT_FEATURES)
	ڳڻپ = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_ڳڻپ = ڳڻپ;
    إذا (ڳڻپ > 0 && features != عدم)
	يادگيري_نقل(mFontFeatures, features, حجم(DWriteFontFeature) * ڳڻپ);
}

    فراغ
DWrite_Context_سيٽ_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحيح ڳڻپ)
{
    إذا (ctx != عدم)
	ctx->سيٽ_Font_Features(features, ڳڻپ);
}
