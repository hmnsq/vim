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

#اگر_غیر_تعریف_شدہ DYNAMIC_DIRECTX
# اگر WINVER < 0x0600
#  error WINVER must be 0x0600 or above مقصد use براہ_راست_Write(براہ_راست_X)
# endif
#اگر_ختم

#شامل <windows.h>
#شامل <crtdbg.h>
#شامل <assert.h>
#شامل <math.h>
#شامل <d2d1.h>
#شامل <d2d1helper.h>

// Disable these macros to compile with old VC and newer SDK (V8.1 or later).
#اگر defined(_MSC_VER) && (_MSC_VER < 1700)
# define _COM_Outptr_ __out
# define اندر_reads(s)
# define اندر_reads_opt(s)
# define _Maybenull_
# define برآمد_writes(s)
# define برآمد_writes_opt(s)
# define برآمد_writes_مقصد(x, y)
# define برآمد_writes_مقصد_opt(x, y)
# define _Outptr_
#اگر_ختم

#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
# include <dwrite_2.h>
#ورنہ
# include <dwrite.h>
#اگر_ختم

#شامل "gui_dwrite.h"

#اگر_تعریف_شدہ __MINGW32__
# define __maybenull	SAL__maybenull
# define __in		SAL_اندر
# define __out		SAL_برآمد
#اگر_ختم

#اگر_تعریف_شدہ __MINGW32__
# define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#ورنہ
# define غیر_مستعمل
#اگر_ختم

#اگر (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ورنہ
# define FINAL
#اگر_ختم

#اگر_تعریف_شدہ DYNAMIC_DIRECTX
خارجی "C" HINSTANCE vim_بھرو_Lib(ثابت حرف *name);

قسمتعریف صحیح (WINAPI *PGETUSERDEFAULTLOCALENAME)(LPWSTR, صحیح);
قسمتعریف HRESULT (WINAPI *PD2D1CREATEFACTORY)(D2D1_FACTORY_TYPE,
	REFIID, ثابت D2D1_FACTORY_OPTIONS *, خالی **);
قسمتعریف HRESULT (WINAPI *PDWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE,
	REFIID, IUnknown **);

ساکن HINSTANCE hD2D1DLL = تہی;
ساکن HINSTANCE hDWriteDLL = تہی;

ساکن PGETUSERDEFAULTLOCALENAME pGetUserDefaultLocaleName = تہی;
ساکن PD2D1CREATEFACTORY pD2D1CreateFactory = تہی;
ساکن PDWRITECREATEFACTORY pDWriteCreateFactory = تہی;

#تعریف_کرو GetUserDefaultLocaleName	(*pGetUserDefaultLocaleName)
#تعریف_کرو D2D1CreateFactory		(*pD2D1CreateFactory)
#تعریف_کرو DWriteCreateFactory		(*pDWriteCreateFactory)

    ساکن خالی
unload(HINSTANCE &hinst)
{
    اگر (hinst != تہی)
    {
	FreeLibrary(hinst);
	hinst = تہی;
    }
}
#اگر_ختم // DYNAMIC_DIRECTX

سانچہ <جماعت T> اندرونی خالی محفوظ_Release(T **ppT)
{
    اگر (*ppT)
    {
	(*ppT)->Release();
	*ppT = تہی;
    }
}

    ساکن DWRITE_PIXEL_GEOMETRY
مقصد_Pixel_Geometry(صحیح قدر)
{
    انتخاب (قدر)
    {
	طےشدہ:
	حالت 0:
	    واپس DWRITE_PIXEL_GEOMETRY_FLAT;
	حالت 1:
	    واپس DWRITE_PIXEL_GEOMETRY_RGB;
	حالت 2:
	    واپس DWRITE_PIXEL_GEOMETRY_BGR;
    }
}

    ساکن صحیح
مقصد_Int(DWRITE_PIXEL_GEOMETRY قدر)
{
    انتخاب (قدر)
    {
	حالت DWRITE_PIXEL_GEOMETRY_FLAT:
	    واپس 0;
	حالت DWRITE_PIXEL_GEOMETRY_RGB:
	    واپس 1;
	حالت DWRITE_PIXEL_GEOMETRY_BGR:
	    واپس 2;
	طےشدہ:
	    واپس -1;
    }
}

    ساکن DWRITE_RENDERING_منوال
مقصد_Rendering_منوال(صحیح قدر)
{
    انتخاب (قدر)
    {
	طےشدہ:
	حالت 0:
	    واپس DWRITE_RENDERING_منوال_DEFAULT;
	حالت 1:
	    واپس DWRITE_RENDERING_منوال_ALIASED;
	حالت 2:
	    واپس DWRITE_RENDERING_منوال_CLEARTYPE_GDI_CLASSIC;
	حالت 3:
	    واپس DWRITE_RENDERING_منوال_CLEARTYPE_GDI_NATURAL;
	حالت 4:
	    واپس DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL;
	حالت 5:
	    واپس DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL_متشاکل;
	حالت 6:
	    واپس DWRITE_RENDERING_منوال_OUTLINE;
    }
}

    ساکن D2_D1_متن_ANTIALIAS_منوال
مقصد_متن_Antialias_منوال(صحیح قدر)
{
    انتخاب (قدر)
    {
	طےشدہ:
	حالت 0:
	    واپس D2_D1_متن_ANTIALIAS_منوال_DEFAULT;
	حالت 1:
	    واپس D2_D1_متن_ANTIALIAS_منوال_CLEARTYPE;
	حالت 2:
	    واپس D2_D1_متن_ANTIALIAS_منوال_GRAYSCALE;
	حالت 3:
	    واپس D2_D1_متن_ANTIALIAS_منوال_ALIASED;
    }
}

    ساکن صحیح
مقصد_Int(DWRITE_RENDERING_منوال قدر)
{
    انتخاب (قدر)
    {
	حالت DWRITE_RENDERING_منوال_DEFAULT:
	    واپس 0;
	حالت DWRITE_RENDERING_منوال_ALIASED:
	    واپس 1;
	حالت DWRITE_RENDERING_منوال_CLEARTYPE_GDI_CLASSIC:
	    واپس 2;
	حالت DWRITE_RENDERING_منوال_CLEARTYPE_GDI_NATURAL:
	    واپس 3;
	حالت DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL:
	    واپس 4;
	حالت DWRITE_RENDERING_منوال_CLEARTYPE_NATURAL_متشاکل:
	    واپس 5;
	حالت DWRITE_RENDERING_منوال_OUTLINE:
	    واپس 6;
	طےشدہ:
	    واپس -1;
    }
}

جماعت FontCache {
عوامی:
    ساخت شے_2 {
	HFONT              hFont;
	IDWrite_متن_Format* p_متن_Format;
	DWRITE_FONT_وزن font_وزن;
	DWRITE_FONT_STYLE  fontStyle;
	FLOAT              fontAscent;
	شے_2() : hFont(تہی), p_متن_Format(تہی), fontAscent(0.0f) {}
    };

نجی:
    صحیح m_سائز;
    شے_2 *m_اشیاء;

عوامی:
    FontCache(صحیح سائز = 2) :
	m_سائز(سائز),
	m_اشیاء(نیا شے_2[سائز])
    {
    }

    ~FontCache()
    {
	برائے (صحیح i = 0; i < m_سائز; ++i)
	    محفوظ_Release(&m_اشیاء[i].p_متن_Format);
	حذف[] m_اشیاء;
    }

    منطقی get(HFONT hFont, شے_2 &شے)
    {
	صحیح n = ڈھونڈو(hFont);
	اگر (n < 0)
	    واپس جھوٹ;
	شے = m_اشیاء[n];
	slide(n);
	واپس سچ;
    }

    خالی put(ثابت شے_2& شے)
    {
	صحیح n = ڈھونڈو(شے.hFont);
	اگر (n < 0)
	    n = m_سائز - 1;
	اگر (m_اشیاء[n].p_متن_Format != شے.p_متن_Format)
	{
	    محفوظ_Release(&m_اشیاء[n].p_متن_Format);
	    اگر (شے.p_متن_Format != تہی)
		شے.p_متن_Format->جوڑو_Ref();
	}
	m_اشیاء[n] = شے;
	slide(n);
    }

نجی:
    صحیح ڈھونڈو(HFONT hFont)
    {
	برائے (صحیح i = 0; i < m_سائز; ++i)
	{
	    اگر (m_اشیاء[i].hFont == hFont)
		واپس i;
	}
	واپس -1;
    }

    خالی slide(صحیح اگلا_چوٹی)
    {
	اگر (اگلا_چوٹی == 0)
	    واپس;
	شے_2 tmp = m_اشیاء[اگلا_چوٹی];
	برائے (صحیح i = اگلا_چوٹی - 1; i >= 0; --i)
	    m_اشیاء[i + 1] = m_اشیاء[i];
	m_اشیاء[0] = tmp;
    }
};

شمار Drawing_منوال {
    DM_GDI = 0,
    DM_DIRECTX = 1,
    DM_INTEROP = 2,
};

ساخت DWriteContext {
    HDC mHDC;
    RECT mBindRect;
    Drawing_منوال mDMode;
    HDC mInteropHDC;
    منطقی mDrawing;
    منطقی mFallbackDC;

    ID2D1Factory *mD2D1Factory;

    ID2_D1_DCRender_ہدف *mRT;
    ID2_D1_Gdi_Interop_Render_ہدف *mGDIRT;
    ID2D1SolidColorBrush *mBrush;
    ID2D1Bitmap *mBitmap;

    IDWriteFactory *mDWriteFactory;
#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    IDWriteFactory2 *mDWriteFactory2;
#اگر_ختم

    IDWriteGdiInterop *mGdiInterop;
    IDWriteRenderingParams *mRenderingParams;

    FontCache mFontCache;
    IDWrite_متن_Format *m_متن_Format;
    DWRITE_FONT_وزن m_Font_وزن;
    DWRITE_FONT_STYLE mFontStyle;
    FLOAT m_Font_سائز;       // Font size in pixels (em height)
    FLOAT mFontAscent;     // Font ascent in pixels

    D2_D1_متن_ANTIALIAS_منوال m_متن_Antialias_منوال;

    DWriteFontFeature mFontFeatures[DWRITE_MAX_FONT_FEATURES];
    صحیح m_Font_Feature_گنتی;

    // METHODS

    DWriteContext();

    مجازی ~DWriteContext();

    HRESULT CreateDeviceResources();

    خالی DiscardDeviceResources();

    HRESULT Create_متن_Format_From_LOGFONT(ثابت LOGFONTW &logFont,
	    IDWrite_متن_Format **pp_متن_Format, FLOAT *pFontAscent);

    HRESULT سیٹ_Font_By_LOGFONT(ثابت LOGFONTW &logFont);

    خالی سیٹ_Font(HFONT hFont);

    خالی Rebind();

    خالی BindDC(HDC hdc, ثابت RECT *rect);

    HRESULT سیٹ_Drawing_منوال(Drawing_منوال منوال);

    ID2D1Brush* SolidBrush(COLORREF color);

    خالی Draw_متن(ثابت WCHAR *متن, صحیح len,
	صحیح x, صحیح y, صحیح w, صحیح h, صحیح cell_چوڑائی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx);

    خالی FillRect(ثابت RECT *rc, COLORREF color);

    خالی Draw_سطر(صحیح x1, صحیح y1, صحیح x2, صحیح y2, COLORREF color);

    خالی سیٹ_Pixel(صحیح x, صحیح y, COLORREF color);

    خالی Scroll(صحیح x, صحیح y, ثابت RECT *rc);

    خالی Flush();

    خالی سیٹ_Rendering_Params(
	    ثابت DWriteRenderingParams *params);

    DWriteRenderingParams *GetRenderingParams(
	    DWriteRenderingParams *params);

    خالی سیٹ_Font_Features(ثابت DWriteFontFeature *features, صحیح گنتی);
};

جماعت Adjusted_Glyph_تسلسل : عوامی DWRITE_GLYPH_تسلسل
{
نجی:
    FLOAT &mAccum;
    FLOAT mDelta;
    FLOAT *mAdjustedAdvances;

عوامی:
    Adjusted_Glyph_تسلسل(
	    ثابت DWRITE_GLYPH_تسلسل *glyph_تسلسل,
	    FLOAT cell_چوڑائی_2,
	    FLOAT &accum,
	    ثابت INT *lpDx,
	    صحیح *glyph_اشاریہ,
	    صحیح cch_متن) :
	DWRITE_GLYPH_تسلسل(*glyph_تسلسل),
	mAccum(accum),
	mDelta(0.0f),
	mAdjustedAdvances(نیا FLOAT[glyph_تسلسل->glyph_گنتی])
    {
	أكد(cell_چوڑائی_2 != 0.0f);
	برائے (UINT32 i = 0; i < glyph_تسلسل->glyph_گنتی; ++i)
	{
	    FLOAT orig = glyph_تسلسل->glyphAdvances[i];
	    FLOAT adjusted;

	    اگر (*glyph_اشاریہ < cch_متن)
	    {
		adjusted = FLOAT(lpDx[*glyph_اشاریہ]);
		(*glyph_اشاریہ)++;
	    }
	    ورنہ
	    {
		adjusted = adjust_مقصد_Cell(orig, cell_چوڑائی_2);
	    }

	    mAdjustedAdvances[i] = adjusted;
	    mDelta += adjusted - orig;
	}
	glyphAdvances = mAdjustedAdvances;
    }

    ~Adjusted_Glyph_تسلسل()
    {
	mAccum += mDelta;
	حذف[] mAdjustedAdvances;
    }

    ساکن FLOAT adjust_مقصد_Cell(FLOAT قدر, FLOAT cell_چوڑائی_2)
    {
	صحیح cell_گنتی = صحیح(floor(قدر / cell_چوڑائی_2 + 0.5f));
	اگر (cell_گنتی < 1)
	    cell_گنتی = 1;
	واپس cell_گنتی * cell_چوڑائی_2;
    }
};

ساخت متن_Renderer_Context {
    // const fields.
    COLORREF color;
    FLOAT cell_چوڑائی_2;
    ثابت INT *lpDx;
    صحیح cch_متن;
    FLOAT baselineY;  // Fixed baseline Y coordinate

    // working fields.
    FLOAT انحراف_X;
    صحیح glyph_اشاریہ;
};

جماعت متن_Renderer FINAL : عوامی IDWrite_متن_Renderer
{
عوامی:
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
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	__out BOOL* isDisabled)
    {
	*isDisabled = FALSE;
	واپس S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	__out DWRITE_میٹرکس* transform)
    {
	// forward the render target's transform
	pDWC_->mRT->GetTransform(
		reinterpret_cast<D2_D1_میٹرکس_3_X2_F*>(transform));
	واپس S_OK;
    }

    IFACEMETHOD(Get_Pixels_فی_Dip)(
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	__out FLOAT* pixels_فی_Dip)
    {
	اعشاری dpiX, غیر_مستعمل_2;
	pDWC_->mRT->GetDpi(&dpiX, &غیر_مستعمل_2);
	*pixels_فی_Dip = dpiX / 96.0f;
	واپس S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	FLOAT baselineOriginX غیر_مستعمل,
	FLOAT baselineOriginY غیر_مستعمل,
	__in DWRITE_UNDERLINE ثابت* underline غیر_مستعمل,
	IUnknown* clientDrawingEffect غیر_مستعمل)
    {
	واپس E_NOTIMPL;
    }

    IFACEMETHOD(DrawStrikethrough)(
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	FLOAT baselineOriginX غیر_مستعمل,
	FLOAT baselineOriginY غیر_مستعمل,
	__in DWRITE_STRIKETHROUGH ثابت* strikethrough غیر_مستعمل,
	IUnknown* clientDrawingEffect غیر_مستعمل)
    {
	واپس E_NOTIMPL;
    }

    IFACEMETHOD(DrawInlineObject)(
	__maybenull خالی* clientDrawingContext غیر_مستعمل,
	FLOAT originX غیر_مستعمل,
	FLOAT originY غیر_مستعمل,
	IDWriteInlineObject* inlineObject غیر_مستعمل,
	BOOL isSideways غیر_مستعمل,
	BOOL is_دایاں_مقصد_بایاں غیر_مستعمل,
	IUnknown* clientDrawingEffect غیر_مستعمل)
    {
	واپس E_NOTIMPL;
    }

    IFACEMETHOD(Draw_Glyph_تسلسل)(
	__maybenull خالی* clientDrawingContext,
	FLOAT baselineOriginX,
	FLOAT baselineOriginY,
	DWRITE_MEASURING_منوال measuring_منوال غیر_مستعمل,
	__in DWRITE_GLYPH_تسلسل ثابت* glyph_تسلسل,
	__in DWRITE_GLYPH_تسلسل_DESCRIPTION ثابت* glyph_تسلسل_Description غیر_مستعمل,
	IUnknown* clientDrawingEffect غیر_مستعمل)
    {
	متن_Renderer_Context *context =
	    reinterpret_cast<متن_Renderer_Context*>(clientDrawingContext);

	// Use fixed baseline Y from context instead of DirectWrite's
	// baselineOriginY to prevent vertical shifts when font fallback
	// occurs (e.g., for CJK characters).
	FLOAT fixedBaselineY = context->baselineY;

	Adjusted_Glyph_تسلسل adjusted_Glyph_تسلسل(glyph_تسلسل, context->cell_چوڑائی_2,
		context->انحراف_X, context->lpDx, &context->glyph_اشاریہ,
		context->cch_متن);

#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
	اگر (pDWC_->mDWriteFactory2 != تہی)
	{
	    IDWrite_Color_Glyph_تسلسل_Enumerator *enumerator = تہی;
	    HRESULT hr = pDWC_->mDWriteFactory2->Translate_Color_Glyph_تسلسل(
		baselineOriginX + context->انحراف_X,
		baselineOriginY,
		&adjusted_Glyph_تسلسل,
		تہی,
		DWRITE_MEASURING_منوال_GDI_NATURAL,
		تہی,
		0,
		&enumerator);
	    اگر (SUCCEEDED(hr))
	    {
		// Draw by IDWriteFactory2 for color emoji
		BOOL has_تسلسل = TRUE;
		enumerator->Move_اگلا(&has_تسلسل);
		جب (has_تسلسل)
		{
		    ثابت DWRITE_COLOR_GLYPH_تسلسل* color_Glyph_تسلسل;
		    enumerator->Get_Current_تسلسل(&color_Glyph_تسلسل);

		    pDWC_->mBrush->سیٹ_Color(color_Glyph_تسلسل->تسلسل_Color);
		    pDWC_->mRT->Draw_Glyph_تسلسل(
			    D2D1::Point2F(
				color_Glyph_تسلسل->baselineOriginX,
				fixedBaselineY),
			    &color_Glyph_تسلسل->glyph_تسلسل,
			    pDWC_->mBrush,
			    DWRITE_MEASURING_منوال_NATURAL);
		    enumerator->Move_اگلا(&has_تسلسل);
		}
		محفوظ_Release(&enumerator);
		واپس S_OK;
	    }
	}
#اگر_ختم

	// Draw by IDWriteFactory (without color emoji)
	pDWC_->mRT->Draw_Glyph_تسلسل(
		D2D1::Point2F(
		    baselineOriginX + context->انحراف_X,
		    fixedBaselineY),
		&adjusted_Glyph_تسلسل,
		pDWC_->SolidBrush(context->color),
		DWRITE_MEASURING_منوال_NATURAL);
	واپس S_OK;
    }

عوامی:
    IFACEMETHOD_(غیرعلامتی طویل, جوڑو_Ref) ()
    {
	واپس InterlockedIncrement(&c_Ref_گنتی);
    }

    IFACEMETHOD_(غیرعلامتی طویل, Release) ()
    {
	طویل new_گنتی_2 = InterlockedDecrement(&c_Ref_گنتی);

	اگر (new_گنتی_2 == 0)
	{
	    حذف یہ;
	    واپس 0;
	}
	واپس new_گنتی_2;
    }

    IFACEMETHOD(QueryInterface)(
	IID ثابت& riid,
	خالی** ppvObject)
    {
	اگر (__uuidof(IDWrite_متن_Renderer) == riid)
	{
	    *ppvObject = یہ;
	}
	ورنہ اگر (__uuidof(IDWritePixelSnapping) == riid)
	{
	    *ppvObject = یہ;
	}
	ورنہ اگر (__uuidof(IUnknown) == riid)
	{
	    *ppvObject = یہ;
	}
	ورنہ
	{
	    *ppvObject = تہی;
	    واپس E_FAIL;
	}

	واپس S_OK;
    }

نجی:
    طویل c_Ref_گنتی;
    DWriteContext* pDWC_;
};

DWriteContext::DWriteContext() :
    mHDC(تہی),
    mBindRect(),
    mDMode(DM_GDI),
    mInteropHDC(تہی),
    mDrawing(جھوٹ),
    mFallbackDC(جھوٹ),
    mD2D1Factory(تہی),
    mRT(تہی),
    mGDIRT(تہی),
    mBrush(تہی),
    mBitmap(تہی),
    mDWriteFactory(تہی),
#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    mDWriteFactory2(تہی),
#اگر_ختم
    mGdiInterop(تہی),
    mRenderingParams(تہی),
    mFontCache(8),
    m_متن_Format(تہی),
    m_Font_وزن(DWRITE_FONT_وزن_معمول),
    mFontStyle(DWRITE_FONT_STYLE_معمول),
    m_Font_سائز(0.0f),
    mFontAscent(0.0f),
    m_متن_Antialias_منوال(D2_D1_متن_ANTIALIAS_منوال_DEFAULT),
    m_Font_Feature_گنتی(0)
{
    Zero_حافظہ(mFontFeatures, حجم(mFontFeatures));
    HRESULT hr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory), تہی,
	    reinterpret_cast<خالی**>(&mD2D1Factory));
    _RPT2(_CRT_WARN, "D2D1CreateFactory: hr=%p p=%p\n", hr, mD2D1Factory);

    اگر (SUCCEEDED(hr))
    {
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&mDWriteFactory));
	_RPT2(_CRT_WARN, "DWriteCreateFactory: hr=%p p=%p\n", hr,
		mDWriteFactory);
    }

#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    اگر (SUCCEEDED(hr))
    {
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&mDWriteFactory2));
	_RPT1(_CRT_WARN, "IDWriteFactory2: %s\n", SUCCEEDED(hr) ? "available" : "not available");
    }
#اگر_ختم

    اگر (SUCCEEDED(hr))
    {
	hr = mDWriteFactory->GetGdiInterop(&mGdiInterop);
	_RPT2(_CRT_WARN, "GetGdiInterop: hr=%p p=%p\n", hr, mGdiInterop);
    }

    اگر (SUCCEEDED(hr))
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
#اگر_تعریف_شدہ FEAT_DIRECTX_COLOR_EMOJI
    محفوظ_Release(&mDWriteFactory2);
#اگر_ختم
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

    اگر (mRT != تہی)
	واپس S_OK;

    D2_D1_RENDER_ہدف_PROPERTIES props = {
	D2_D1_RENDER_ہدف_TYPE_DEFAULT,
	{ DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_منوال_IGNORE },
	0, 0,
	D2_D1_RENDER_ہدف_USAGE_GDI_COMPATIBLE,
	D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = mD2D1Factory->Create_DCRender_ہدف(&props, &mRT);
    _RPT2(_CRT_WARN, "CreateDCRenderTarget: hr=%p p=%p\n", hr, mRT);

    اگر (SUCCEEDED(hr))
    {
	// This always succeeds.
	mRT->QueryInterface(
		__uuidof(ID2_D1_Gdi_Interop_Render_ہدف),
		reinterpret_cast<خالی**>(&mGDIRT));
	_RPT1(_CRT_WARN, "GdiInteropRenderTarget: p=%p\n", mGDIRT);
    }

    اگر (SUCCEEDED(hr))
    {
	hr = mRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&mBrush);
	_RPT2(_CRT_WARN, "CreateSolidColorBrush: hr=%p p=%p\n", hr, mBrush);
    }

    اگر (SUCCEEDED(hr))
	Rebind();

    واپس hr;
}

    خالی
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
    IDWrite_متن_Format *p_متن_Format = تہی;

    IDWriteFont *font = تہی;
    IDWriteFontFamily *fontFamily = تہی;
    IDWrite_Localized_لڑیاں *localized_Family_نام = تہی;
    اعشاری font_سائز = 0;
    DWRITE_FONT_METRICS fontMetrics = {};

    اگر (SUCCEEDED(hr))
    {
	hr = mGdiInterop->CreateFontFromLOGFONT(&logFont, &font);
    }

    // Get the font family to which this font belongs.
    اگر (SUCCEEDED(hr))
    {
	hr = font->GetFontFamily(&fontFamily);
    }

    // Get the family names. This returns an object that encapsulates one or
    // more names with the same meaning but in different languages.
    اگر (SUCCEEDED(hr))
    {
	hr = fontFamily->Get_Family_نام(&localized_Family_نام);
    }

    // Get the family name at index zero. If we were going to display the name
    // we'd want to try to find one that matched the use locale, but for
    // purposes of creating a text format object any language will do.

    wchar_t familyName[100];
    اگر (SUCCEEDED(hr))
    {
	hr = localized_Family_نام->GetString(0, familyName,
		ARRAYSIZE(familyName));
    }

    اگر (SUCCEEDED(hr))
    {
	// Use lfHeight of the LOGFONT as font size.
	font_سائز = اعشاری(logFont.lf_اونچائی);

	font->GetMetrics(&fontMetrics);

	// Convert lfHeight to DirectWrite font size
	اگر (font_سائز < 0)
	{
	    // Negative lfHeight represents the font's em height in pixels
	    font_سائز = -font_سائز;
	}
	ورنہ اگر (font_سائز > 0)
	{
	    // Positive lfHeight represents the font's cell height (ascent + descent)
	    // Convert to em height
	    font_سائز = font_سائز * اعشاری(fontMetrics.design_Units_فی_Em)
		/ اعشاری(fontMetrics.ascent + fontMetrics.descent + fontMetrics.سطر_Gap);
	}
    }

    // The text format includes a locale name. Ideally, this would be the
    // language of the text, which may or may not be the same as the primary
    // language of the user. However, for our purposes the user locale will do.
    wchar_t localeName[LOCALE_NAME_MAX_لمبائی];
    اگر (SUCCEEDED(hr))
    {
	اگر (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_لمبائی) == 0)
	    hr = HRESULT_FROM_WIN32(Get_آخری_Error());
    }

    اگر (SUCCEEDED(hr))
    {
	// Create the text format object.
	hr = mDWriteFactory->Create_متن_Format(
		familyName,
		تہی, // no custom font collection
		font->Get_وزن(),
		font->GetStyle(),
		font->GetStretch(),
		font_سائز,
		localeName,
		&p_متن_Format);
    }

    اگر (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_متن_Alignment(DWRITE_متن_ALIGNMENT_LEADING);

    اگر (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_Paragraph_Alignment(
		DWRITE_PARAGRAPH_ALIGNMENT_FAR);

    اگر (SUCCEEDED(hr))
	hr = p_متن_Format->سیٹ_لفظ_Wrapping(DWRITE_لفظ_WRAPPING_NO_WRAP);

    محفوظ_Release(&localized_Family_نام);
    محفوظ_Release(&fontFamily);
    محفوظ_Release(&font);

    اگر (SUCCEEDED(hr))
    {
	*pp_متن_Format = p_متن_Format;
	اگر (pFontAscent != تہی && fontMetrics.design_Units_فی_Em != 0)
	{
	    // Calculate ascent ratio (0.0 to 1.0) relative to total font height
	    FLOAT کل_اونچائی = اعشاری(fontMetrics.ascent + fontMetrics.descent);
	    اگر (کل_اونچائی != 0.0f)
		*pFontAscent = اعشاری(fontMetrics.ascent) / کل_اونچائی;
	    ورنہ
		*pFontAscent = 0.83f; // fallback
	}
    }
    ورنہ
	محفوظ_Release(&p_متن_Format);

    واپس hr;
}

    HRESULT
DWriteContext::سیٹ_Font_By_LOGFONT(ثابت LOGFONTW &logFont)
{
    HRESULT hr = S_OK;
    IDWrite_متن_Format *p_متن_Format = تہی;
    FLOAT fontAscent = 0.0f;

    hr = Create_متن_Format_From_LOGFONT(logFont, &p_متن_Format, &fontAscent);

    اگر (SUCCEEDED(hr))
    {
	محفوظ_Release(&m_متن_Format);
	m_متن_Format = p_متن_Format;
	m_Font_وزن = static_cast<DWRITE_FONT_وزن>(logFont.lf_وزن);
	mFontStyle = logFont.lfItalic ? DWRITE_FONT_STYLE_ITALIC
	    : DWRITE_FONT_STYLE_معمول;
	mFontAscent = fontAscent;
    }

    واپس hr;
}

    خالی
DWriteContext::سیٹ_Font(HFONT hFont)
{
    FontCache::شے_2 شے;
    اگر (mFontCache.get(hFont, شے))
    {
	اگر (شے.p_متن_Format != تہی)
	{
	    شے.p_متن_Format->جوڑو_Ref();
	    محفوظ_Release(&m_متن_Format);
	    m_متن_Format = شے.p_متن_Format;
	    m_Font_وزن = شے.font_وزن;
	    mFontStyle = شے.fontStyle;
	    mFontAscent = شے.fontAscent;
	    mFallbackDC = جھوٹ;
	}
	ورنہ
	    mFallbackDC = سچ;
	واپس;
    }

    HRESULT hr = E_FAIL;
    LOGFONTW lf;
    اگر (GetObjectW(hFont, حجم(lf), &lf))
	hr = سیٹ_Font_By_LOGFONT(lf);

    شے.hFont = hFont;
    اگر (SUCCEEDED(hr))
    {
	شے.p_متن_Format = m_متن_Format;
	شے.font_وزن = m_Font_وزن;
	شے.fontStyle = mFontStyle;
	شے.fontAscent = mFontAscent;
	mFallbackDC = جھوٹ;
    }
    ورنہ
	mFallbackDC = سچ;
    mFontCache.put(شے);
}

    خالی
DWriteContext::Rebind()
{
    محفوظ_Release(&mBitmap);

    mRT->BindDC(mHDC, &mBindRect);
    mRT->سیٹ_Transform(D2D1::شناختی_عنصر_میٹرکس());

    D2D1_BITMAP_PROPERTIES props = {
	{DXGI_FORMAT_B8G8R8A8_UNORM, D2_D1_ALPHA_منوال_IGNORE},
	96.0f, 96.0f
    };
    mRT->CreateBitmap(
	    D2D1::سائز_U(mBindRect.دائیں - mBindRect.بائیں,
		mBindRect.bottom - mBindRect.چوٹی),
	    props, &mBitmap);
}

    خالی
DWriteContext::BindDC(HDC hdc, ثابت RECT *rect)
{
    mHDC = hdc;
    mBindRect = *rect;

    اگر (mRT == تہی)
	CreateDeviceResources();
    ورنہ
    {
	Flush();
	Rebind();
    }
}

خارجی "C" خالی redraw_later_clear(خالی);

    HRESULT
DWriteContext::سیٹ_Drawing_منوال(Drawing_منوال منوال)
{
    HRESULT hr = S_OK;

    انتخاب (منوال)
    {
	طےشدہ:
	حالت DM_GDI:
	    اگر (mInteropHDC != تہی)
	    {
		mGDIRT->ReleaseDC(تہی);
		mInteropHDC = تہی;
	    }
	    اگر (mDrawing)
	    {
		hr = mRT->انتہا_Draw();
		اگر (hr == (HRESULT)D2_DERR_RECREATE_ہدف)
		{
		    hr = S_OK;
		    DiscardDeviceResources();
		    CreateDeviceResources();
		    redraw_later_clear();
		}
		mDrawing = جھوٹ;
	    }
	    توڑو;

	حالت DM_DIRECTX:
	    اگر (mInteropHDC != تہی)
	    {
		mGDIRT->ReleaseDC(تہی);
		mInteropHDC = تہی;
	    }
	    ورنہ اگر (mDrawing == جھوٹ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = سچ;
	    }
	    توڑو;

	حالت DM_INTEROP:
	    اگر (mDrawing == جھوٹ)
	    {
		CreateDeviceResources();
		mRT->BeginDraw();
		mDrawing = سچ;
	    }
	    اگر (mInteropHDC == تہی)
		hr = mGDIRT->GetDC(D2_D1_DC_INITIALIZE_منوال_COPY, &mInteropHDC);
	    توڑو;
    }
    mDMode = منوال;
    واپس hr;
}

    ID2D1Brush*
DWriteContext::SolidBrush(COLORREF color)
{
    mBrush->سیٹ_Color(D2D1::ColorF(UINT32(GetRValue(color)) << 16 |
		UINT32(GetGValue(color)) << 8 | UINT32(GetBValue(color))));
    واپس mBrush;
}

    خالی
DWriteContext::Draw_متن(ثابت WCHAR *متن, صحیح len,
	صحیح x, صحیح y, صحیح w, صحیح h, صحیح cell_چوڑائی_2, COLORREF color,
	UINT fuOptions, ثابت RECT *lprc, ثابت INT *lpDx)
{
    اگر (mFallbackDC)
    {
	// Fall back to GDI rendering.
	HRESULT hr = سیٹ_Drawing_منوال(DM_INTEROP);
	اگر (SUCCEEDED(hr))
	{
	    HGDIOBJ hFont = ::GetCurrentObject(mHDC, OBJ_FONT);
	    HGDIOBJ hOldFont = ::SelectObject(mInteropHDC, hFont);
	    ::سیٹ_متن_Color(mInteropHDC, color);
	    ::سیٹ_Bk_منوال(mInteropHDC, ::Get_Bk_منوال(mHDC));
	    ::Ext_متن_برآمد_W(mInteropHDC, x, y, fuOptions, lprc, متن, len, lpDx);
	    ::SelectObject(mInteropHDC, hOldFont);
	}
	واپس;
    }

    HRESULT hr;
    IDWrite_متن_Layout *متن_Layout = تہی;

    سیٹ_Drawing_منوال(DM_DIRECTX);

    // Apply clipping if ETO_CLIPPED is specified
    اگر ((fuOptions & ETO_CLIPPED) && lprc != تہی)
    {
	mRT->ڈالو_Axis_Aligned_Clip(
	    D2D1::RectF(FLOAT(lprc->بائیں), FLOAT(lprc->چوٹی),
			FLOAT(lprc->دائیں), FLOAT(lprc->bottom)),
	    D2_D1_ANTIALIAS_منوال_ALIASED);
    }

    hr = mDWriteFactory->Create_متن_Layout(متن, len, m_متن_Format,
	    FLOAT(w), FLOAT(h), &متن_Layout);

    اگر (SUCCEEDED(hr))
    {
	DWRITE_متن_RANGE متن_Range = { 0, UINT32(len) };
	متن_Layout->سیٹ_Font_وزن(m_Font_وزن, متن_Range);
	متن_Layout->سیٹ_Font_Style(mFontStyle, متن_Range);

	اگر (m_Font_Feature_گنتی > 0)
	{
	    // Default OpenType features that DirectWrite normally enables.
	    // SetTypography() overrides all defaults, so we must
	    // re-add them here explicitly.
	    ساکن ثابت DWRITE_FONT_FEATURE defaultFeatures[] = {
		{ DWRITE_FONT_FEATURE_لیبل_CONTEXTUAL_ALTERNATES, 1 },
		{ DWRITE_FONT_FEATURE_لیبل_STANDARD_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_لیبل_CONTEXTUAL_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_لیبل_REQUIRED_LIGATURES, 1 },
		{ DWRITE_FONT_FEATURE_لیبل_KERNING, 1 },
	    };
	    ساکن ثابت صحیح عدد_Defaults = حجم(defaultFeatures)
		/ حجم(defaultFeatures[0]);

	    IDWriteTypography *typography = تہی;
	    hr = mDWriteFactory->CreateTypography(&typography);
	    اگر (SUCCEEDED(hr))
	    {
		// Add default features, skipping any that the user
		// has explicitly specified (either + or -).
		برائے (صحیح d = 0; d < عدد_Defaults; ++d)
		{
		    صحیح overridden = 0;
		    برائے (صحیح u = 0; u < m_Font_Feature_گنتی; ++u)
		    {
			اگر ((DWRITE_FONT_FEATURE_لیبل)mFontFeatures[u].لیبل
				== defaultFeatures[d].name_لیبل)
			{
			    overridden = 1;
			    توڑو;
			}
		    }
		    اگر (!overridden)
			typography->جوڑو_Font_Feature(defaultFeatures[d]);
		}
		// Add user-specified features.
		برائے (صحیح i = 0; i < m_Font_Feature_گنتی; ++i)
		{
		    DWRITE_FONT_FEATURE ff = {
			(DWRITE_FONT_FEATURE_لیبل)mFontFeatures[i].لیبل,
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

	متن_Renderer renderer(یہ);
	متن_Renderer_Context context = { color, FLOAT(cell_چوڑائی_2), lpDx, len,
		baselineY, 0.0f, 0 };

	متن_Layout->Draw(&context, &renderer, FLOAT(x), FLOAT(y));
    }

    محفوظ_Release(&متن_Layout);

    اگر ((fuOptions & ETO_CLIPPED) && lprc != تہی)
	mRT->نکالو_Axis_Aligned_Clip();
}

    خالی
DWriteContext::FillRect(ثابت RECT *rc, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HBRUSH hbr = ::CreateSolidBrush(color);
	::FillRect(mInteropHDC, rc, hbr);
	::DeleteObject(HGDIOBJ(hbr));
    }
    ورنہ
    {
	سیٹ_Drawing_منوال(DM_DIRECTX);
	mRT->FillRectangle(
		D2D1::RectF(FLOAT(rc->بائیں), FLOAT(rc->چوٹی),
		    FLOAT(rc->دائیں), FLOAT(rc->bottom)),
		SolidBrush(color));
    }
}

    خالی
DWriteContext::Draw_سطر(صحیح x1, صحیح y1, صحیح x2, صحیح y2, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	HPEN hpen = ::CreatePen(PS_SOLID, 1, color);
	HGDIOBJ old_pen = ::SelectObject(mInteropHDC, HGDIOBJ(hpen));
	::Move_مقصد_Ex(mInteropHDC, x1, y1, تہی);
	::سطر_مقصد(mInteropHDC, x2, y2);
	::SelectObject(mInteropHDC, old_pen);
	::DeleteObject(HGDIOBJ(hpen));
    }
    ورنہ
    {
	سیٹ_Drawing_منوال(DM_DIRECTX);
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x1), FLOAT(y1) + 0.5f),
		D2D1::Point2F(FLOAT(x2), FLOAT(y2) + 0.5f),
		SolidBrush(color));
    }
}

    خالی
DWriteContext::سیٹ_Pixel(صحیح x, صحیح y, COLORREF color)
{
    اگر (mDMode == DM_INTEROP)
    {
	// GDI functions are used before this call.  Keep using GDI.
	// (Switching to Direct2D causes terrible slowdown.)
	::سیٹ_Pixel(mInteropHDC, x, y, color);
    }
    ورنہ
    {
	سیٹ_Drawing_منوال(DM_DIRECTX);
	// Direct2D doesn't have SetPixel API.  Use DrawLine instead.
	mRT->Draw_سطر(
		D2D1::Point2F(FLOAT(x), FLOAT(y) + 0.5f),
		D2D1::Point2F(FLOAT(x+1), FLOAT(y) + 0.5f),
		SolidBrush(color));
    }
}

    خالی
DWriteContext::Scroll(صحیح x, صحیح y, ثابت RECT *rc)
{
    سیٹ_Drawing_منوال(DM_DIRECTX);
    mRT->Flush();

    D2D1_RECT_U srcRect;
    D2_D1_نقطہ_2_U dest_نقطہ;
    اگر (x >= 0)
    {
	srcRect.بائیں = rc->بائیں;
	srcRect.دائیں = rc->دائیں - x;
	dest_نقطہ.x = rc->بائیں + x;
    }
    ورنہ
    {
	srcRect.بائیں = rc->بائیں - x;
	srcRect.دائیں = rc->دائیں;
	dest_نقطہ.x = rc->بائیں;
    }
    اگر (y >= 0)
    {
	srcRect.چوٹی = rc->چوٹی;
	srcRect.bottom = rc->bottom - y;
	dest_نقطہ.y = rc->چوٹی + y;
    }
    ورنہ
    {
	srcRect.چوٹی = rc->چوٹی - y;
	srcRect.bottom = rc->bottom;
	dest_نقطہ.y = rc->چوٹی;
    }
    mBitmap->Copy_From_Render_ہدف(&dest_نقطہ, mRT, &srcRect);

    D2D1_RECT_F destRect = {
	    FLOAT(dest_نقطہ.x), FLOAT(dest_نقطہ.y),
	    FLOAT(dest_نقطہ.x + srcRect.دائیں - srcRect.بائیں),
	    FLOAT(dest_نقطہ.y + srcRect.bottom - srcRect.چوٹی)
    };
    mRT->DrawBitmap(mBitmap, destRect, 1.0F,
	    D2_D1_BITMAP_INTERPOLATION_منوال_قریب_ترین_NEIGHBOR, destRect);
}

    خالی
DWriteContext::Flush()
{
    سیٹ_Drawing_منوال(DM_GDI);
}

    خالی
DWriteContext::سیٹ_Rendering_Params(
	ثابت DWriteRenderingParams *params)
{
    اگر (mDWriteFactory == تہی)
	واپس;

    IDWriteRenderingParams *renderingParams = تہی;
    D2_D1_متن_ANTIALIAS_منوال متن_Antialias_منوال =
	D2_D1_متن_ANTIALIAS_منوال_DEFAULT;
    HRESULT hr;
    اگر (params != تہی)
    {
	hr = mDWriteFactory->CreateCustomRenderingParams(params->gamma,
		params->enhancedContrast, params->clearTypeLevel,
		مقصد_Pixel_Geometry(params->pixelGeometry),
		مقصد_Rendering_منوال(params->rendering_منوال), &renderingParams);
	متن_Antialias_منوال = مقصد_متن_Antialias_منوال(params->متن_Antialias_منوال);
    }
    ورنہ
	hr = mDWriteFactory->CreateRenderingParams(&renderingParams);
    اگر (SUCCEEDED(hr) && renderingParams != تہی)
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
    اگر (params != تہی && mRenderingParams != تہی)
    {
	params->gamma = mRenderingParams->GetGamma();
	params->enhancedContrast = mRenderingParams->GetEnhancedContrast();
	params->clearTypeLevel = mRenderingParams->GetClearTypeLevel();
	params->pixelGeometry = مقصد_Int(mRenderingParams->GetPixelGeometry());
	params->rendering_منوال = مقصد_Int(mRenderingParams->Get_Rendering_منوال());
	params->متن_Antialias_منوال = m_متن_Antialias_منوال;
    }
    واپس params;
}

////////////////////////////////////////////////////////////////////////////
// PUBLIC C INTERFACES

    خالی
DWrite_Init(خالی)
{
#اگر_تعریف_شدہ DYNAMIC_DIRECTX
    // Load libraries.
    hD2D1DLL = vim_بھرو_Lib("d2d1.dll");
    hDWriteDLL = vim_بھرو_Lib("dwrite.dll");
    اگر (hD2D1DLL == تہی || hDWriteDLL == تہی)
    {
	DWrite_Final();
	واپس;
    }
    // Get address of procedures.
    pGetUserDefaultLocaleName = (PGETUSERDEFAULTLOCALENAME)Get_Proc_پتہ(
	    GetModuleHandle("kernel32.dll"), "GetUserDefaultLocaleName");
    pD2D1CreateFactory = (PD2D1CREATEFACTORY)Get_Proc_پتہ(hD2D1DLL,
	    "D2D1CreateFactory");
    pDWriteCreateFactory = (PDWRITECREATEFACTORY)Get_Proc_پتہ(hDWriteDLL,
	    "DWriteCreateFactory");
#اگر_ختم
}

    خالی
DWrite_Final(خالی)
{
#اگر_تعریف_شدہ DYNAMIC_DIRECTX
    pGetUserDefaultLocaleName = تہی;
    pD2D1CreateFactory = تہی;
    pDWriteCreateFactory = تہی;
    unload(hDWriteDLL);
    unload(hD2D1DLL);
#اگر_ختم
}

    DWriteContext *
DWriteContext_Open(خالی)
{
#اگر_تعریف_شدہ DYNAMIC_DIRECTX
    اگر (pGetUserDefaultLocaleName == تہی || pD2D1CreateFactory == تہی
	    || pDWriteCreateFactory == تہی)
	واپس تہی;
#اگر_ختم
    واپس نیا DWriteContext();
}

    خالی
DWriteContext_BindDC(DWriteContext *ctx, HDC hdc, ثابت RECT *rect)
{
    اگر (ctx != تہی)
	ctx->BindDC(hdc, rect);
}

    خالی
DWrite_Context_سیٹ_Font(DWriteContext *ctx, HFONT hFont)
{
    اگر (ctx != تہی)
	ctx->سیٹ_Font(hFont);
}

    خالی
DWrite_Context_Draw_متن(
	DWriteContext *ctx,
	ثابت WCHAR *متن,
	صحیح len,
	صحیح x,
	صحیح y,
	صحیح w,
	صحیح h,
	صحیح cell_چوڑائی_2,
	COLORREF color,
	UINT fuOptions,
	ثابت RECT *lprc,
	ثابت INT *lpDx)
{
    اگر (ctx != تہی)
	ctx->Draw_متن(متن, len, x, y, w, h, cell_چوڑائی_2, color,
		fuOptions, lprc, lpDx);
}

    خالی
DWriteContext_FillRect(DWriteContext *ctx, ثابت RECT *rc, COLORREF color)
{
    اگر (ctx != تہی)
	ctx->FillRect(rc, color);
}

    خالی
DWrite_Context_Draw_سطر(DWriteContext *ctx, صحیح x1, صحیح y1, صحیح x2, صحیح y2,
	COLORREF color)
{
    اگر (ctx != تہی)
	ctx->Draw_سطر(x1, y1, x2, y2, color);
}

    خالی
DWrite_Context_سیٹ_Pixel(DWriteContext *ctx, صحیح x, صحیح y, COLORREF color)
{
    اگر (ctx != تہی)
	ctx->سیٹ_Pixel(x, y, color);
}

    خالی
DWriteContext_Scroll(DWriteContext *ctx, صحیح x, صحیح y, ثابت RECT *rc)
{
    اگر (ctx != تہی)
	ctx->Scroll(x, y, rc);
}

    خالی
DWriteContext_Flush(DWriteContext *ctx)
{
    اگر (ctx != تہی)
	ctx->Flush();
}

    خالی
DWriteContext_Close(DWriteContext *ctx)
{
    حذف ctx;
}

    خالی
DWrite_Context_سیٹ_Rendering_Params(
	DWriteContext *ctx,
	ثابت DWriteRenderingParams *params)
{
    اگر (ctx != تہی)
	ctx->سیٹ_Rendering_Params(params);
}

    DWriteRenderingParams *
DWriteContext_GetRenderingParams(
	DWriteContext *ctx,
	DWriteRenderingParams *params)
{
    اگر (ctx != تہی)
	واپس ctx->GetRenderingParams(params);
    ورنہ
	واپس تہی;
}

    خالی
DWriteContext::سیٹ_Font_Features(
	ثابت DWriteFontFeature *features, صحیح گنتی)
{
    اگر (گنتی > DWRITE_MAX_FONT_FEATURES)
	گنتی = DWRITE_MAX_FONT_FEATURES;
    m_Font_Feature_گنتی = گنتی;
    اگر (گنتی > 0 && features != تہی)
	حافظہ_نقل(mFontFeatures, features, حجم(DWriteFontFeature) * گنتی);
}

    خالی
DWrite_Context_سیٹ_Font_Features(
	DWriteContext *ctx,
	ثابت DWriteFontFeature *features,
	صحیح گنتی)
{
    اگر (ctx != تہی)
	ctx->سیٹ_Font_Features(features, گنتی);
}
