/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * PS printer stuff.
 *
 * Sources of information to help maintain the PS printing code:
 *
 * 1. PostScript Language Reference, 3rd Edition,
 *      Addison-Wesley, 1999, ISBN 0-201-37922-8
 * 2. PostScript Language Program Design,
 *      Addison-Wesley, 1988, ISBN 0-201-14396-8
 * 3. PostScript Tutorial and Cookbook,
 *      Addison Wesley, 1985, ISBN 0-201-10179-3
 * 4. PostScript Language Document Structuring Conventions Specification,
 *    version 3.0,
 *      Adobe Technote 5001, 25th September 1992
 * 5. PostScript Printer Description File Format Specification, Version 4.3,
 *      Adobe technote 5003, 9th February 1996
 * 6. Adobe Font Metrics File Format Specification, Version 4.1,
 *      Adobe Technote 5007, 7th October 1998
 * 7. Adobe CMap and CIDFont Files Specification, Version 1.0,
 *      Adobe Technote 5014, 8th October 1996
 * 8. Adobe CJKV Character Collections and CMaps for CID-Keyed Fonts,
 *      Adoboe Technote 5094, 8th September, 2001
 * 9. CJKV Information Processing, 2nd Edition,
 *      O'Reilly, 2002, ISBN 1-56592-224-7
 *
 * Some of these documents can be found in PDF form on Adobe's web site -
 * http://www.adobe.com
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_ਸਾਰਣੀ_T mbfont_opts[OPT_MBFONT_ਅੰਕ_OPTIONS] =
{
    {"c",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
    {"a",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
    {"r",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
    {"b",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
    {"i",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
    {"o",	FALSE, 0, ਨਿਰਾ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_ਬਫਰ_ਮਾਪ  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ਬਣਤਰ prt_ps_font_S
{
    ਪੂਰਨ		wx;
    ਪੂਰਨ		uline_ਵਿੱਥ;
    ਪੂਰਨ		uline_ਚੌੜਾਈ;
    ਪੂਰਨ		bbox_min_y;
    ਪੂਰਨ		bbox_max_y;
    ਅੱਖਰ	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ਸਥਿਰ ਬਣਤਰ prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ਸਥਿਰ ਬਣਤਰ prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ}
};

// Pointer to current font set being used
ਸਥਿਰ ਬਣਤਰ prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ਬਣਤਰ prt_ps_encoding_S
{
    ਅੱਖਰ	*encoding;
    ਅੱਖਰ	*cmap_encoding;
    ਪੂਰਨ		needs_charset;
};

ਬਣਤਰ prt_ps_charset_S
{
    ਅੱਖਰ	*charset;
    ਅੱਖਰ	*cmap_charset;
    ਪੂਰਨ		has_charset;
};

# define CS_JIS_C_1978   (0x01)
# define CS_JIS_X_1983   (0x02)
# define CS_JIS_X_1990   (0x04)
# define CS_NEC		(0x08)
# define CS_MSWINDOWS	(0x10)
# define CS_CP932	(0x20)
# define CS_KANJITALK6	(0x40)
# define CS_KANJITALK7   (0x80)

// Japanese encodings and charsets
ਸਥਿਰ ਬਣਤਰ prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ਨਿਰਾ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ਸਥਿਰ ਬਣਤਰ prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ਨਿਰਾ,       CS_JIS_X_1983},
    {"JIS_X_1990",  "Hojo",     CS_JIS_X_1990},
    {"NEC",	    "Ext",	CS_NEC},
    {"MSWINDOWS",   "90ms",     CS_MSWINDOWS},
    {"CP932",       "90ms",     CS_JIS_X_1983},
    {"KANJITALK6",  "83pv",     CS_KANJITALK6},
    {"KANJITALK7",  "90pv",     CS_KANJITALK7}
};

# define CS_GB_2312_80       (0x01)
# define CS_GBT_12345_90     (0x02)
# define CS_GBK2K	    (0x04)
# define CS_SC_MAC	    (0x08)
# define CS_GBT_90_MAC	    (0x10)
# define CS_GBK		    (0x20)
# define CS_SC_ISO10646      (0x40)

// Simplified Chinese encodings and charsets
ਸਥਿਰ ਬਣਤਰ prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ਨਿਰਾ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ਨਿਰਾ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ਸਥਿਰ ਬਣਤਰ prt_ps_charset_S sc_charsets[] =
{
    {"GB_2312-80",  "GB",       CS_GB_2312_80},
    {"GBT_12345-90","GBT",      CS_GBT_12345_90},
    {"MAC",	    "GBpc",	CS_SC_MAC},
    {"GBT-90_MAC",  "GBTpc",	CS_GBT_90_MAC},
    {"GBK",	    "GBK",	CS_GBK},
    {"GB18030",     "GBK2K",    CS_GBK2K},
    {"ISO10646",    "UniGB",    CS_SC_ISO10646}
};

# define CS_CNS_PLANE_1      (0x01)
# define CS_CNS_PLANE_2      (0x02)
# define CS_CNS_PLANE_1_2    (0x04)
# define CS_B5		    (0x08)
# define CS_ETEN		    (0x10)
# define CS_HK_GCCS	    (0x20)
# define CS_HK_SCS	    (0x40)
# define CS_HK_SCS_ETEN	    (0x80)
# define CS_MTHKL	    (0x100)
# define CS_MTHKS	    (0x200)
# define CS_DLHKL	    (0x400)
# define CS_DLHKS	    (0x800)
# define CS_TC_ISO10646	    (0x1000)

// Traditional Chinese encodings and charsets
ਸਥਿਰ ਬਣਤਰ prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ਨਿਰਾ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
    {"euc-tw",      "EUC",      CS_CNS_PLANE_1_2},
    {"big5",	    "B5",	(CS_B5|CS_ETEN|CS_HK_GCCS|CS_HK_SCS|
				    CS_HK_SCS_ETEN|CS_MTHKL|CS_MTHKS|CS_DLHKL|
								    CS_DLHKS)},
    {"cp950",       "B5",       CS_B5},
    {"ucs-2",       "UCS2",     CS_TC_ISO10646},
    {"utf-8",       "UTF8",     CS_TC_ISO10646},
    {"utf-16",      "UTF16",    CS_TC_ISO10646},
    {"utf-32",      "UTF32",    CS_TC_ISO10646}
};
ਸਥਿਰ ਬਣਤਰ prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ਨਿਰਾ,	CS_B5},
    {"CP950",	    ਨਿਰਾ,	CS_B5},
    {"ETEN",	    "ETen",	CS_ETEN},
    {"HK_GCCS",     "HKgccs",	CS_HK_GCCS},
    {"SCS",	    "HKscs",	CS_HK_SCS},
    {"SCS_ETEN",    "ETHK",     CS_HK_SCS_ETEN},
    {"MTHKL",       "HKm471",   CS_MTHKL},
    {"MTHKS",       "HKm314",   CS_MTHKS},
    {"DLHKL",       "HKdla",    CS_DLHKL},
    {"DLHKS",       "HKdlb",    CS_DLHKS},
    {"ISO10646",    "UniCNS",   CS_TC_ISO10646}
};

# define CS_KR_X_1992	    (0x01)
# define CS_KR_MAC	    (0x02)
# define CS_KR_X_1992_MS     (0x04)
# define CS_KR_ISO10646      (0x08)

// Korean encodings and charsets
ਸਥਿਰ ਬਣਤਰ prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ਨਿਰਾ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ਸਥਿਰ ਬਣਤਰ prt_ps_charset_S k_charsets[] =
{
    {"KS_X_1992",   "KSC",      CS_KR_X_1992},
    {"CP1361",      "KSC",      CS_KR_X_1992},
    {"MAC",	    "KSCpc",	CS_KR_MAC},
    {"MSWINDOWS",   "KSCms",    CS_KR_X_1992_MS},
    {"CP949",       "KSCms",    CS_KR_X_1992_MS},
    {"WANSUNG",     "KSCms",    CS_KR_X_1992_MS},
    {"ISO10646",    "UniKS",    CS_KR_ISO10646}
};

// Collections of encodings and charsets for multi-byte printing
ਬਣਤਰ prt_ps_mbfont_S
{
    ਪੂਰਨ				ਅੰਕ_encodings;
    ਬਣਤਰ prt_ps_encoding_S	*encodings;
    ਪੂਰਨ				ਅੰਕ_charsets;
    ਬਣਤਰ prt_ps_charset_S	*charsets;
    ਅੱਖਰ			*ascii_enc;
    ਅੱਖਰ			*defcs;
};

ਸਥਿਰ ਬਣਤਰ prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_ਲੰਮਾਈ(j_encodings),
	j_encodings,
	ARRAY_ਲੰਮਾਈ(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_ਲੰਮਾਈ(sc_encodings),
	sc_encodings,
	ARRAY_ਲੰਮਾਈ(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_ਲੰਮਾਈ(tc_encodings),
	tc_encodings,
	ARRAY_ਲੰਮਾਈ(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_ਲੰਮਾਈ(k_encodings),
	k_encodings,
	ARRAY_ਲੰਮਾਈ(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

ਬਣਤਰ prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    ਪੂਰਨ     type;
    char_u  title[256];
    char_u  ਸੰਸਕਰਣ_2[256];
};

// Types of PS resource file currently used
# define PRT_RESOURCE_TYPE_PROCSET   (0)
# define PRT_RESOURCE_TYPE_ENCODING  (1)
# define PRT_RESOURCE_TYPE_CMAP      (2)

// The PS prolog file version number has to match - if the prolog file is
// updated, increment the number in the file and here.  Version checking was
// added as of VIM 6.2.
// The CID prolog file version number behaves as per PS prolog.
// Table of VIM and prolog versions:
//
// VIM      Prolog  CIDProlog
// 6.2      1.3
// 7.0      1.4	    1.0
# define PRT_PROLOG_ਸੰਸਕਰਣ  ((char_u *)"1.4")
# define PRT_CID_PROLOG_ਸੰਸਕਰਣ  ((char_u *)"1.0")

// String versions of PS resource types - indexed by constants above so don't
// re-order!
ਸਥਿਰ ਅੱਖਰ *prt_resource_types[] =
{
    "procset",
    "encoding",
    "cmap"
};

// Strings to look for in a PS resource file
# define PRT_RESOURCE_ਸਿਰਲੇਖ	    "%!PS-Adobe-"
# define PRT_RESOURCE_RESOURCE	    "Resource-"
# define PRT_RESOURCE_PROCSET	    "ProcSet"
# define PRT_RESOURCE_ENCODING	    "Encoding"
# define PRT_RESOURCE_CMAP	    "CMap"


// Data for table based DSC comment recognition, easy to extend if VIM needs to
// read more comments.
# define PRT_DSC_MISC_TYPE	    (-1)
# define PRT_DSC_TITLE_TYPE	    (1)
# define PRT_DSC_ਸੰਸਕਰਣ_TYPE	    (2)
# define PRT_DSC_ENDCOMMENTS_TYPE    (3)

# define PRT_DSC_TITLE		    "%%Title:"
# define PRT_DSC_ਸੰਸਕਰਣ		    "%%Version:"
# define PRT_DSC_ENDCOMMENTS	    "%%EndComments:"

ਬਣਤਰ prt_dsc_comment_S
{
    ਅੱਖਰ	*string;
    ਪੂਰਨ		len;
    ਪੂਰਨ		type;
};

ਬਣਤਰ prt_dsc_ਸਤਰ_S
{
    ਪੂਰਨ		type;
    char_u	*string;
    ਪੂਰਨ		len;
};


# define SIZEOF_CSTR(s)      (ਆਕਾਰ(s) - 1)
ਸਥਿਰ ਬਣਤਰ prt_dsc_comment_S prt_dsc_ਸਾਰਣੀ[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_ਸੰਸਕਰਣ,     SIZEOF_CSTR(PRT_DSC_ਸੰਸਕਰਣ),
							PRT_DSC_ਸੰਸਕਰਣ_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ਸਥਿਰ ਖਾਲੀ prt_write_file_len(char_u *ਬਫਰ, ਪੂਰਨ ਬਾਈਟਾਂ);
ਸਥਿਰ ਪੂਰਨ prt_ਅਗਲਾ_dsc(ਬਣਤਰ prt_dsc_ਸਤਰ_S *p_dsc_ਸਤਰ);

/*
 * Variables for the output PostScript file.
 */
ਸਥਿਰ FILE *prt_ps_fd;
ਸਥਿਰ ਪੂਰਨ prt_file_error;
ਸਥਿਰ char_u *prt_ps_file_name = ਨਿਰਾ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਪੰਨਾ_ਚੌੜਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਪੰਨਾ_ਉਚਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਖੱਬਾ_margin;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਸੱਜਾ_margin;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਸਿਖਰ_margin;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_bottom_margin;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਸਤਰ_ਉਚਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਪਹਿਲਾ_ਸਤਰ_ਉਚਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_char_ਚੌੜਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਸੰਖਿਆ_ਚੌੜਾਈ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_bgcol_ਵਿੱਥ;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_pos_x_moveto = 0.0;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ਸਥਿਰ ਪੂਰਨ prt_need_moveto;
ਸਥਿਰ ਪੂਰਨ prt_do_moveto;
ਸਥਿਰ ਪੂਰਨ prt_need_font;
ਸਥਿਰ ਪੂਰਨ prt_font;
ਸਥਿਰ ਪੂਰਨ prt_need_underline;
ਸਥਿਰ ਪੂਰਨ prt_underline;
ਸਥਿਰ ਪੂਰਨ prt_do_underline;
ਸਥਿਰ ਪੂਰਨ prt_need_fgcol;
ਸਥਿਰ ਪੂਰਨ prt_fgcol;
ਸਥਿਰ ਪੂਰਨ prt_need_bgcol;
ਸਥਿਰ ਪੂਰਨ prt_do_bgcol;
ਸਥਿਰ ਪੂਰਨ prt_bgcol;
ਸਥਿਰ ਪੂਰਨ prt_new_bgcol;
ਸਥਿਰ ਪੂਰਨ prt_attribute_change;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_ਪਾਠ_ਲੜੀਵਾਰ;
ਸਥਿਰ ਪੂਰਨ prt_ਪੰਨਾ_ਅੰਕ;
ਸਥਿਰ ਪੂਰਨ prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ਸਥਿਰ ਪੂਰਨ prt_media;
ਸਥਿਰ ਪੂਰਨ prt_portrait;
ਸਥਿਰ ਪੂਰਨ prt_ਅੰਕ_copies;
ਸਥਿਰ ਪੂਰਨ prt_duplex;
ਸਥਿਰ ਪੂਰਨ prt_tumble;
ਸਥਿਰ ਪੂਰਨ prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ਸਥਿਰ char_u prt_ਸਤਰ_ਬਫਰ[257];
ਸਥਿਰ garray_T prt_ps_ਬਫਰ;

ਸਥਿਰ ਪੂਰਨ prt_do_conv;
ਸਥਿਰ vimconv_T prt_conv;

ਸਥਿਰ ਪੂਰਨ prt_ਬਾਹਰਲਾ_mbyte;
ਸਥਿਰ ਪੂਰਨ prt_custom_cmap;
ਸਥਿਰ ਅੱਖਰ prt_cmap[80];
ਸਥਿਰ ਪੂਰਨ prt_use_courier;
ਸਥਿਰ ਪੂਰਨ prt_ਵਿੱਚ_ascii;
ਸਥਿਰ ਪੂਰਨ prt_half_ਚੌੜਾਈ;
ਸਥਿਰ ਅੱਖਰ *prt_ascii_encoding;
ਸਥਿਰ ਅਚਲ char_u prt_hexchar[] = "0123456789abcdef";

    ਸਥਿਰ ਖਾਲੀ
prt_write_file_raw_len(char_u *ਬਫਰ, ਪੂਰਨ ਬਾਈਟਾਂ)
{
    ਜੇ (!prt_file_error
	    && fwrite(ਬਫਰ, ਆਕਾਰ(char_u), ਬਾਈਟਾਂ, prt_ps_fd)
							     != (size_t)ਬਾਈਟਾਂ)
    {
	emsg(_(e_error_writing_ਮੰਜ਼ਿਲ_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ਸਥਿਰ ਖਾਲੀ
prt_write_file(char_u *ਬਫਰ)
{
    prt_write_file_len(ਬਫਰ, (ਪੂਰਨ)STRLEN(ਬਫਰ));
}

    ਸਥਿਰ ਖਾਲੀ
prt_write_file_len(char_u *ਬਫਰ, ਪੂਰਨ ਬਾਈਟਾਂ)
{
    prt_write_file_raw_len(ਬਫਰ, ਬਾਈਟਾਂ);
}

/*
 * Write a string.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_write_string(ਅੱਖਰ *s)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ), "%s", s);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write an int and a space.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_write_int(ਪੂਰਨ i)
{
    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "%d ", i);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write a boolean and a space.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_write_boolean(ਪੂਰਨ b)
{
    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_def_font(
    ਅੱਖਰ	*new_name,
    ਅੱਖਰ	*encoding,
    ਪੂਰਨ		ਉਚਾਈ,
    ਅੱਖਰ	*font)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "/%s %d %f /_%s sffs\n",
		       new_name, ਉਚਾਈ, 500./prt_ps_courier_font.wx, new_name);
    ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
			     "/%s %d /_%s ffs\n", new_name, ਉਚਾਈ, new_name);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write a line to define the CID font.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_def_cidfont(ਅੱਖਰ *new_name, ਪੂਰਨ ਉਚਾਈ, ਅੱਖਰ *cidfont)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
			     "/%s %d /_%s ffs\n", new_name, ਉਚਾਈ, new_name);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ਸਥਿਰ ਖਾਲੀ
prt_dup_cidfont(ਅੱਖਰ *original_name, ਅੱਖਰ *new_name)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_real_ਬਿੱਟ(
    ਦੁੱਗਣਾ      real,
    ਪੂਰਨ		precision,
    ਪੂਰਨ		*pinteger,
    ਪੂਰਨ		*pfraction)
{
    ਪੂਰਨ     i;
    ਪੂਰਨ     integer;
    ਦਸ਼ਮਲਵ   fraction;

    integer = (ਪੂਰਨ)real;
    fraction = (ਦਸ਼ਮਲਵ)(real - integer);
    ਜੇ (real < (ਦੁੱਗਣਾ)integer)
	fraction = -fraction;
    ਲਈ (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (ਪੂਰਨ)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_write_real(ਦੁੱਗਣਾ val, ਪੂਰਨ prec)
{
    ਪੂਰਨ     integer;
    ਪੂਰਨ     fraction;

    prt_real_ਬਿੱਟ(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "%d", integer);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    // Only emit fraction if necessary
    ਜੇ (fraction != 0)
    {
	// Remove any trailing zeros
	ਜਦੋਂ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ".%0*d", prec, fraction);
	prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    }
    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, " ");
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

/*
 * Write a line to define a numeric variable.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_def_var(ਅੱਖਰ *name, ਦੁੱਗਣਾ ਮੁੱਲ, ਪੂਰਨ prec)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
								"/%s ", name);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    prt_write_real(ਮੁੱਲ, prec);
    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "d\n");
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(scale, ਮਾਪ)    ((ਮਾਪ) * ((scale)/1000.0))

    ਸਥਿਰ ਖਾਲੀ
prt_flush_ਬਫਰ(ਖਾਲੀ)
{
    ਜੇ (prt_ps_ਬਫਰ.ga_len <= 0)
	ਵਾਪਸ;

    // Any background color must be drawn first
    ਜੇ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	ਪੂਰਨ     r, g, b;

	ਜੇ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_ਪਾਠ_ਲੜੀਵਾਰ, 2);
	prt_write_real(prt_ਸਤਰ_ਉਚਾਈ, 2);

	// Lastly add the color of the background
	r = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)prt_new_bgcol & 0xff0000) >> 16;
	g = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)prt_new_bgcol & 0xff00) >> 8;
	b = prt_new_bgcol & 0xff;
	prt_write_real(r / 255.0, 3);
	prt_write_real(g / 255.0, 3);
	prt_write_real(b / 255.0, 3);
	prt_write_string("bg\n");
    }
    // Draw underlines before the text as it makes it slightly easier to
    // find the starting point.
    ਜੇ (prt_do_underline)
    {
	ਜੇ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Underline length of text run
	prt_write_real(prt_ਪਾਠ_ਲੜੀਵਾਰ, 2);
	prt_write_string("ul\n");
    }
    // Draw the text
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	prt_write_string("<");
    ਨਹੀਂਤਾਂ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_ਬਫਰ.ga_ਅੰਕੜੇ, prt_ps_ਬਫਰ.ga_len);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	prt_write_string(">");
    ਨਹੀਂਤਾਂ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ਜੇ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ਨਹੀਂਤਾਂ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_ਬਫਰ);
    ga_init2(&prt_ps_ਬਫਰ, ਆਕਾਰ(ਅੱਖਰ), prt_bufsiz);
}


    ਸਥਿਰ ਖਾਲੀ
prt_resource_name(char_u *filename, ਖਾਲੀ *cookie)
{
    char_u *resource_filename = cookie;

    ਜੇ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ਨਹੀਂਤਾਂ
	STRCPY(resource_filename, filename);
}

    ਸਥਿਰ ਪੂਰਨ
prt_ਲੱਭੋ_resource(ਅੱਖਰ *name, ਬਣਤਰ prt_ps_resource_S *resource)
{
    char_u	*ਬਫਰ;
    ਪੂਰਨ		retval;

    ਬਫਰ = alloc(MAXPATHL + 1);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(ਬਫਰ, "print");
    ਜੋੜੋ_pathsep(ਬਫਰ);
    vim_strcat(ਬਫਰ, (char_u *)name, MAXPATHL);
    vim_strcat(ਬਫਰ, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_ਵਿੱਚ_runtimepath(ਬਫਰ, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(ਬਫਰ);
    ਵਾਪਸ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_ਬਫਰ_LEN (2048)
ਬਣਤਰ prt_resfile_ਬਫਰ_S
{
    char_u  ਬਫਰ[PRT_FILE_ਬਫਰ_LEN];
    ਪੂਰਨ     len;
    ਪੂਰਨ     ਸਤਰ_ਸ਼ੁਰੂ;
    ਪੂਰਨ     ਸਤਰ_ਸਿਰਾ;
};

ਸਥਿਰ ਬਣਤਰ prt_resfile_ਬਫਰ_S prt_resfile;

    ਸਥਿਰ ਪੂਰਨ
prt_resfile_ਅਗਲਾ_ਸਤਰ(ਖਾਲੀ)
{
    ਪੂਰਨ     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.ਸਤਰ_ਸਿਰਾ + 1;
    ਜਦੋਂ (idx < prt_resfile.len)
    {
	ਜੇ (prt_resfile.ਬਫਰ[idx] != PSLF && prt_resfile.ਬਫਰ[idx] != PSCR)
	    ਰੋਕੋ;
	idx++;
    }
    prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ = idx;

    ਜਦੋਂ (idx < prt_resfile.len)
    {
	ਜੇ (prt_resfile.ਬਫਰ[idx] == PSLF || prt_resfile.ਬਫਰ[idx] == PSCR)
	    ਰੋਕੋ;
	idx++;
    }
    prt_resfile.ਸਤਰ_ਸਿਰਾ = idx;

    ਵਾਪਸ (idx < prt_resfile.len);
}

    ਸਥਿਰ ਪੂਰਨ
prt_resfile_strncmp(ਪੂਰਨ ਵਿੱਥ, ਅੱਖਰ *string, ਪੂਰਨ len)
{
    // Force not equal if string is longer than remainder of line
    ਜੇ (len > (prt_resfile.ਸਤਰ_ਸਿਰਾ - (prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ)))
	ਵਾਪਸ 1;

    ਵਾਪਸ STRNCMP(&prt_resfile.ਬਫਰ[prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ],
								string, len);
}

    ਸਥਿਰ ਪੂਰਨ
prt_resfile_skip_nonws(ਪੂਰਨ ਵਿੱਥ)
{
    ਪੂਰਨ     idx;

    idx = prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ;
    ਜਦੋਂ (idx < prt_resfile.ਸਤਰ_ਸਿਰਾ)
    {
	ਜੇ (ਸੁਰੱਖਿਅਤ_isspace(prt_resfile.ਬਫਰ[idx]))
	    ਵਾਪਸ idx - prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ;
	idx++;
    }
    ਵਾਪਸ -1;
}

    ਸਥਿਰ ਪੂਰਨ
prt_resfile_skip_ws(ਪੂਰਨ ਵਿੱਥ)
{
    ਪੂਰਨ     idx;

    idx = prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ;
    ਜਦੋਂ (idx < prt_resfile.ਸਤਰ_ਸਿਰਾ)
    {
	ਜੇ (!ਸੁਰੱਖਿਅਤ_isspace(prt_resfile.ਬਫਰ[idx]))
	    ਵਾਪਸ idx - prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ;
	idx++;
    }
    ਵਾਪਸ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ਸਥਿਰ ਪੂਰਨ
prt_ਅਗਲਾ_dsc(ਬਣਤਰ prt_dsc_ਸਤਰ_S *p_dsc_ਸਤਰ)
{
    ਪੂਰਨ     comment;
    ਪੂਰਨ     ਵਿੱਥ;

    // Move to start of next line
    ਜੇ (!prt_resfile_ਅਗਲਾ_ਸਤਰ())
	ਵਾਪਸ FALSE;

    // DSC comments always start %%
    ਜੇ (prt_resfile_strncmp(0, "%%", 2) != 0)
	ਵਾਪਸ FALSE;

    // Find type of DSC comment
    ਲਈ (comment = 0; comment < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(prt_dsc_ਸਾਰਣੀ); comment++)
	ਜੇ (prt_resfile_strncmp(0, prt_dsc_ਸਾਰਣੀ[comment].string,
					    prt_dsc_ਸਾਰਣੀ[comment].len) == 0)
	    ਰੋਕੋ;

    ਜੇ (comment != ARRAY_ਲੰਮਾਈ(prt_dsc_ਸਾਰਣੀ))
    {
	// Return type of comment
	p_dsc_ਸਤਰ->type = prt_dsc_ਸਾਰਣੀ[comment].type;
	ਵਿੱਥ = prt_dsc_ਸਾਰਣੀ[comment].len;
    }
    ਨਹੀਂਤਾਂ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_ਸਤਰ->type = PRT_DSC_MISC_TYPE;
	ਵਿੱਥ = prt_resfile_skip_nonws(0);
	ਜੇ (ਵਿੱਥ == -1)
	    ਵਾਪਸ FALSE;
    }

    // Skip ws to comment value
    ਵਿੱਥ = prt_resfile_skip_ws(ਵਿੱਥ);
    ਜੇ (ਵਿੱਥ == -1)
	ਵਾਪਸ FALSE;

    p_dsc_ਸਤਰ->string = &prt_resfile.ਬਫਰ[prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ];
    p_dsc_ਸਤਰ->len = prt_resfile.ਸਤਰ_ਸਿਰਾ - (prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ + ਵਿੱਥ);

    ਵਾਪਸ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ਸਥਿਰ ਪੂਰਨ
prt_open_resource(ਬਣਤਰ prt_ps_resource_S *resource)
{
    ਪੂਰਨ		ਵਿੱਥ;
    ਪੂਰਨ		ਵੇਖਿਆ_all;
    ਪੂਰਨ		ਵੇਖਿਆ_title;
    ਪੂਰਨ		ਵੇਖਿਆ_ਸੰਸਕਰਣ;
    FILE	*fd_resource;
    ਬਣਤਰ prt_dsc_ਸਤਰ_S dsc_ਸਤਰ;

    fd_resource = mch_fopen((ਅੱਖਰ *)resource->filename, READBIN);
    ਜੇ (fd_resource == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ਵਾਪਸ FALSE;
    }
    CLEAR_ਖੇਤਰ(prt_resfile.ਬਫਰ);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (ਪੂਰਨ)fread((ਅੱਖਰ *)prt_resfile.ਬਫਰ, ਆਕਾਰ(char_u),
					    PRT_FILE_ਬਫਰ_LEN, fd_resource);
    ਜੇ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ਵਾਪਸ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.ਸਤਰ_ਸਿਰਾ = -1;
    prt_resfile.ਸਤਰ_ਸ਼ੁਰੂ = 0;
    ਜੇ (!prt_resfile_ਅਗਲਾ_ਸਤਰ())
	ਵਾਪਸ FALSE;

    ਵਿੱਥ = 0;

    ਜੇ (prt_resfile_strncmp(ਵਿੱਥ, PRT_RESOURCE_ਸਿਰਲੇਖ,
			       (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_ਸਿਰਲੇਖ)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ਵਾਪਸ FALSE;
    }

    // Skip over any version numbers and following ws
    ਵਿੱਥ += (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_ਸਿਰਲੇਖ);
    ਵਿੱਥ = prt_resfile_skip_nonws(ਵਿੱਥ);
    ਜੇ (ਵਿੱਥ == -1)
	ਵਾਪਸ FALSE;
    ਵਿੱਥ = prt_resfile_skip_ws(ਵਿੱਥ);
    ਜੇ (ਵਿੱਥ == -1)
	ਵਾਪਸ FALSE;

    ਜੇ (prt_resfile_strncmp(ਵਿੱਥ, PRT_RESOURCE_RESOURCE,
			     (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ਵਾਪਸ FALSE;
    }
    ਵਿੱਥ += (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ਜੇ (prt_resfile_strncmp(ਵਿੱਥ, PRT_RESOURCE_PROCSET,
			      (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ਨਹੀਂਤਾਂ ਜੇ (prt_resfile_strncmp(ਵਿੱਥ, PRT_RESOURCE_ENCODING,
			     (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ਨਹੀਂਤਾਂ ਜੇ (prt_resfile_strncmp(ਵਿੱਥ, PRT_RESOURCE_CMAP,
				 (ਪੂਰਨ)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ਵਾਪਸ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->ਸੰਸਕਰਣ_2[0] = '\0';
    ਵੇਖਿਆ_title = FALSE;
    ਵੇਖਿਆ_ਸੰਸਕਰਣ = FALSE;
    ਵੇਖਿਆ_all = FALSE;
    ਜਦੋਂ (!ਵੇਖਿਆ_all && prt_ਅਗਲਾ_dsc(&dsc_ਸਤਰ))
    {
	ਚੋਣ (dsc_ਸਤਰ.type)
	{
	ਹਾਲਤ PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_ਸਤਰ.string, dsc_ਸਤਰ.len);
	    ਵੇਖਿਆ_title = TRUE;
	    ਜੇ (ਵੇਖਿਆ_ਸੰਸਕਰਣ)
		ਵੇਖਿਆ_all = TRUE;
	    ਰੋਕੋ;

	ਹਾਲਤ PRT_DSC_ਸੰਸਕਰਣ_TYPE:
	    vim_strncpy(resource->ਸੰਸਕਰਣ_2, dsc_ਸਤਰ.string, dsc_ਸਤਰ.len);
	    ਵੇਖਿਆ_ਸੰਸਕਰਣ = TRUE;
	    ਜੇ (ਵੇਖਿਆ_title)
		ਵੇਖਿਆ_all = TRUE;
	    ਰੋਕੋ;

	ਹਾਲਤ PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    ਵੇਖਿਆ_all = TRUE;
	    ਰੋਕੋ;

	ਹਾਲਤ PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    ਰੋਕੋ;
	}
    }

    ਜੇ (!ਵੇਖਿਆ_title || !ਵੇਖਿਆ_ਸੰਸਕਰਣ)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
prt_check_resource(ਬਣਤਰ prt_ps_resource_S *resource, char_u *ਸੰਸਕਰਣ_2)
{
    // Version number m.n should match, the revision number does not matter
    ਜੇ (STRNCMP(resource->ਸੰਸਕਰਣ_2, ਸੰਸਕਰਣ_2, STRLEN(ਸੰਸਕਰਣ_2)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_ਸੰਸਕਰਣ),
		resource->name);
	ਵਾਪਸ FALSE;
    }

    // Other checks to be added as needed
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_noarg(ਅੱਖਰ *comment)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
							 "%%%%%s\n", comment);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_textline(ਅੱਖਰ *comment, ਅੱਖਰ *ਪਾਠ)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
					       "%%%%%s: %s\n", comment, ਪਾਠ);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_ਪਾਠ(ਅੱਖਰ *comment, ਅੱਖਰ *ਪਾਠ)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
					     "%%%%%s: (%s)\n", comment, ਪਾਠ);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

# define prt_dsc_atend(c)	prt_dsc_ਪਾਠ((c), "atend")

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_ints(ਅੱਖਰ *comment, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ *ints)
{
    ਪੂਰਨ		i;

    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
							  "%%%%%s:", comment);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);

    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; i++)
    {
	sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, " %d", ints[i]);
	prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    }

    prt_write_string("\n");
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_resources(
    ਅੱਖਰ	*comment,	// if NULL add to previous
    ਅੱਖਰ	*type,
    ਅੱਖਰ	*string)
{
    ਜੇ (comment != ਨਿਰਾ)
	vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
						 "%%%%%s: %s", comment, type);
    ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
							    "%%%%+ %s", type);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);

    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
							     " %s\n", string);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_font_resource(ਅੱਖਰ *resource, ਬਣਤਰ prt_ps_font_S *ps_font)
{
    ਪੂਰਨ     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    ਲਈ (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ਜੇ (ps_font->ps_fontname[i] != ਨਿਰਾ)
	    prt_dsc_resources(ਨਿਰਾ, "font", ps_font->ps_fontname[i]);
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_requirements(
    ਪੂਰਨ		duplex,
    ਪੂਰਨ		tumble,
    ਪੂਰਨ		collate,
    ਪੂਰਨ		color,
    ਪੂਰਨ		ਅੰਕ_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ਜੇ (!(duplex || collate || color || (ਅੰਕ_copies > 1)))
	ਵਾਪਸ;

    sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "%%%%Requirements:");
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);

    ਜੇ (duplex)
    {
	prt_write_string(" duplex");
	ਜੇ (tumble)
	    prt_write_string("(tumble)");
    }
    ਜੇ (collate)
	prt_write_string(" collate");
    ਜੇ (color)
	prt_write_string(" color");
    ਜੇ (ਅੰਕ_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, "%d", ਅੰਕ_copies);
	prt_write_file(prt_ਸਤਰ_ਬਫਰ);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ਸਥਿਰ ਖਾਲੀ
prt_dsc_docmedia(
    ਅੱਖਰ	*paper_name,
    ਦੁੱਗਣਾ	ਚੌੜਾਈ,
    ਦੁੱਗਣਾ	ਉਚਾਈ,
    ਦੁੱਗਣਾ	ਭਾਰ,
    ਅੱਖਰ	*ਰੰਗ,
    ਅੱਖਰ	*type)
{
    vim_snprintf((ਅੱਖਰ *)prt_ਸਤਰ_ਬਫਰ, ਆਕਾਰ(prt_ਸਤਰ_ਬਫਰ),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_ਸਤਰ_ਬਫਰ);
    prt_write_real(ਚੌੜਾਈ, 2);
    prt_write_real(ਉਚਾਈ, 2);
    prt_write_real(ਭਾਰ, 2);
    ਜੇ (ਰੰਗ == ਨਿਰਾ)
	prt_write_string("()");
    ਨਹੀਂਤਾਂ
	prt_write_string(ਰੰਗ);
    prt_write_string(" ");
    ਜੇ (type == ਨਿਰਾ)
	prt_write_string("()");
    ਨਹੀਂਤਾਂ
	prt_write_string(type);
    prt_write_string("\n");
}

    ਖਾਲੀ
mch_print_cleanup(ਖਾਲੀ)
{
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	ਪੂਰਨ     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	ਲਈ (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ਜੇ (prt_ps_mb_font.ps_fontname[i] != ਨਿਰਾ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ਜੇ (prt_do_conv)
    {
	convert_ਤਿਆਰੀ(&prt_conv, ਨਿਰਾ, ਨਿਰਾ);
	prt_do_conv = FALSE;
    }
    ਜੇ (prt_ps_fd != ਨਿਰਾ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ਨਿਰਾ;
	prt_file_error = FALSE;
    }
    ਜੇ (prt_ps_file_name != ਨਿਰਾ)
	VIM_CLEAR(prt_ps_file_name);
}

    ਸਥਿਰ ਦਸ਼ਮਲਵ
ਮੰਜ਼ਿਲ_device_units(ਪੂਰਨ idx, ਦੁੱਗਣਾ physsize, ਪੂਰਨ def_ਸੰਖਿਆ)
{
    ਦਸ਼ਮਲਵ	ret;
    ਪੂਰਨ		u;
    ਪੂਰਨ		nr;

    u = prt_get_unit(idx);
    ਜੇ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ਸੰਖਿਆ;
    }
    ਨਹੀਂਤਾਂ
	nr = printer_opts[idx].ਸੰਖਿਆ;

    ਚੋਣ (u)
    {
	ਹਾਲਤ PRT_UNIT_INCH:
	    ret = (ਦਸ਼ਮਲਵ)(nr * PRT_PS_DEFAULT_DPI);
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_MM:
	    ret = (ਦਸ਼ਮਲਵ)(nr * PRT_PS_DEFAULT_DPI) / (ਦਸ਼ਮਲਵ)25.4;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_ਬਿੰਦੂ:
	    ret = (ਦਸ਼ਮਲਵ)nr;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_PERC:
	ਮੂਲ:
	    ret = (ਦਸ਼ਮਲਵ)(physsize * nr) / 100;
	    ਰੋਕੋ;
    }

    ਵਾਪਸ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ਸਥਿਰ ਖਾਲੀ
prt_ਪੰਨਾ_margins(
    ਦੁੱਗਣਾ	ਚੌੜਾਈ,
    ਦੁੱਗਣਾ	ਉਚਾਈ,
    ਦੁੱਗਣਾ	*ਖੱਬਾ,
    ਦੁੱਗਣਾ	*ਸੱਜਾ,
    ਦੁੱਗਣਾ	*ਸਿਖਰ,
    ਦੁੱਗਣਾ	*bottom)
{
    *ਖੱਬਾ   = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਖੱਬਾ, ਚੌੜਾਈ, 10);
    *ਸੱਜਾ  = ਚੌੜਾਈ - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸੱਜਾ, ਚੌੜਾਈ, 5);
    *ਸਿਖਰ    = ਉਚਾਈ - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸਿਖਰ, ਉਚਾਈ, 5);
    *bottom = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_BOT, ਉਚਾਈ, 5);
}

    ਸਥਿਰ ਖਾਲੀ
prt_font_metrics(ਪੂਰਨ font_scale)
{
    prt_ਸਤਰ_ਉਚਾਈ = (ਦਸ਼ਮਲਵ)font_scale;
    prt_char_ਚੌੜਾਈ = (ਦਸ਼ਮਲਵ)PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(font_scale, prt_ps_font->wx);
}


    ਸਥਿਰ ਪੂਰਨ
prt_get_cpl(ਖਾਲੀ)
{
    ਜੇ (prt_use_ਸੰਖਿਆ())
    {
	prt_ਸੰਖਿਆ_ਚੌੜਾਈ = PRINT_ਸੰਖਿਆ_ਚੌੜਾਈ * prt_char_ਚੌੜਾਈ;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	    prt_ਸੰਖਿਆ_ਚੌੜਾਈ /= 2;
	prt_ਖੱਬਾ_margin += prt_ਸੰਖਿਆ_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
	prt_ਸੰਖਿਆ_ਚੌੜਾਈ = 0.0;

    ਵਾਪਸ (ਪੂਰਨ)((prt_ਸੱਜਾ_margin - prt_ਖੱਬਾ_margin) / prt_char_ਚੌੜਾਈ);
}

    ਸਥਿਰ ਪੂਰਨ
prt_build_cid_fontname(ਪੂਰਨ font, char_u *name, ਪੂਰਨ name_len)
{
    ਅੱਖਰ    *fontname;

    fontname = alloc(name_len + 1);
    ਜੇ (fontname == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ਵਾਪਸ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ਸਥਿਰ ਪੂਰਨ
prt_get_lpp(ਖਾਲੀ)
{
    ਪੂਰਨ lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_ਵਿੱਥ = (ਦਸ਼ਮਲਵ)PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(prt_ਸਤਰ_ਉਚਾਈ,
					   prt_ps_font->bbox_min_y);
    ਜੇ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_ਵਿੱਥ -= (ਦਸ਼ਮਲਵ)PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(prt_ਸਤਰ_ਉਚਾਈ,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_ਪਹਿਲਾ_ਸਤਰ_ਉਚਾਈ = prt_ਸਤਰ_ਉਚਾਈ + prt_bgcol_ਵਿੱਥ;

    // Calculate lpp
    lpp = (ਪੂਰਨ)((prt_ਸਿਖਰ_margin - prt_bottom_margin) / prt_ਸਤਰ_ਉਚਾਈ);

    // Adjust top margin if there is a header
    prt_ਸਿਖਰ_margin -= prt_ਸਤਰ_ਉਚਾਈ * prt_ਸਿਰਲੇਖ_ਉਚਾਈ();

    ਵਾਪਸ lpp - prt_ਸਿਰਲੇਖ_ਉਚਾਈ();
}

    ਸਥਿਰ ਪੂਰਨ
prt_ਮੇਲ_encoding(
    ਅੱਖਰ			*p_encoding,
    ਬਣਤਰ prt_ps_mbfont_S	*p_cmap,
    ਬਣਤਰ prt_ps_encoding_S	**pp_mbenc)
{
    ਪੂਰਨ				mbenc;
    ਪੂਰਨ				enc_len;
    ਬਣਤਰ prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ਨਿਰਾ;
    // Look for recognised encoding
    enc_len = (ਪੂਰਨ)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    ਲਈ (mbenc = 0; mbenc < p_cmap->ਅੰਕ_encodings; mbenc++)
    {
	ਜੇ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ਵਾਪਸ TRUE;
	}
	p_mbenc++;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
prt_ਮੇਲ_charset(
    ਅੱਖਰ		    *p_charset,
    ਬਣਤਰ prt_ps_mbfont_S  *p_cmap,
    ਬਣਤਰ prt_ps_charset_S **pp_mbchar)
{
    ਪੂਰਨ			    mbchar;
    ਪੂਰਨ			    char_len;
    ਬਣਤਰ prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ਜੇ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (ਪੂਰਨ)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    ਲਈ (mbchar = 0; mbchar < p_cmap->ਅੰਕ_charsets; mbchar++)
    {
	ਜੇ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ਵਾਪਸ TRUE;
	}
	p_mbchar++;
    }
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    ਪੂਰਨ		forceit ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    ਅੱਖਰ	*paper_name;
    ਪੂਰਨ		paper_strlen;
    ਪੂਰਨ		fontsize;
    char_u	*p;
    ਦੁੱਗਣਾ      ਖੱਬਾ;
    ਦੁੱਗਣਾ      ਸੱਜਾ;
    ਦੁੱਗਣਾ      ਸਿਖਰ;
    ਦੁੱਗਣਾ      bottom;
    ਪੂਰਨ		props;
    ਪੂਰਨ		cmap = 0;
    char_u	*p_encoding;
    ਬਣਤਰ prt_ps_encoding_S *p_mbenc;
    ਬਣਤਰ prt_ps_encoding_S *p_mbenc_ਪਹਿਲਾ;
    ਬਣਤਰ prt_ps_charset_S  *p_mbchar = ਨਿਰਾ;

# ਜੇ 0
    /*
     * TODO:
     * If "forceit" is false: pop up a dialog to select:
     *	- printer name
     *	- copies
     *	- collated/uncollated
     *	- duplex off/long side/short side
     *	- paper size
     *	- portrait/landscape
     *	- font size
     *
     * If "forceit" is true: use the default printer and settings
     */
    ਜੇ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ਜੇ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ਨਿਰਾ;
    props = enc_canon_props(p_encoding);
    ਜੇ (!(props & ENC_8_ਦੋਆਧਾਰੀ_ਅੰਕ) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	ਪੂਰਨ cmap_ਪਹਿਲਾ = 0;

	p_mbenc_ਪਹਿਲਾ = ਨਿਰਾ;
	ਲਈ (cmap = 0; cmap < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(prt_ps_mbfonts); cmap++)
	    ਜੇ (prt_ਮੇਲ_encoding((ਅੱਖਰ *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ਜੇ (p_mbenc_ਪਹਿਲਾ == ਨਿਰਾ)
		{
		    p_mbenc_ਪਹਿਲਾ = p_mbenc;
		    cmap_ਪਹਿਲਾ = cmap;
		}
		ਜੇ (prt_ਮੇਲ_charset((ਅੱਖਰ *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    ਰੋਕੋ;
	    }

	// Use first encoding matched if no charset matched
	ਜੇ (p_mbchar == ਨਿਰਾ && p_mbenc_ਪਹਿਲਾ != ਨਿਰਾ)
	{
	    p_mbenc = p_mbenc_ਪਹਿਲਾ;
	    cmap = cmap_ਪਹਿਲਾ;
	}
    }

    prt_ਬਾਹਰਲਾ_mbyte = (p_mbenc != ਨਿਰਾ);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ਨਿਰਾ);
	ਜੇ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ਜੇ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_ਸੈੱਟ));
		ਵਾਪਸ FALSE;
	    }

	    // Add charset name if not empty
	    ਜੇ (p_mbchar->cmap_charset != ਨਿਰਾ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, ਆਕਾਰ(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Add custom CMap character set name
	    ਜੇ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_ਖਾਲੀ_ਥਾਵਾਂ_with_multi_byte_encoding));
		ਵਾਪਸ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, ਆਕਾਰ(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ਜੇ (p_mbenc->cmap_encoding != ਨਿਰਾ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < ਆਕਾਰ(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ਜੇ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ਵਾਪਸ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ਜੇ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].ਲੜੀ_ਲੰਬਾਈ))
	    ਵਾਪਸ FALSE;
	ਜੇ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ਜੇ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].ਲੜੀ_ਲੰਬਾਈ))
		ਵਾਪਸ FALSE;
	ਜੇ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ਜੇ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].ਲੜੀ_ਲੰਬਾਈ))
		ਵਾਪਸ FALSE;
	ਜੇ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ਜੇ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].ਲੜੀ_ਲੰਬਾਈ))
		ਵਾਪਸ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ਜੇ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ਜੇ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ਨਹੀਂਤਾਂ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ਨਹੀਂਤਾਂ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ਜੇ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (ਅੱਖਰ *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].ਲੜੀ_ਲੰਬਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    ਲਈ (i = 0; i < (ਪੂਰਨ)PRT_MEDIASIZE_LEN; ++i)
	ਜੇ (STRLEN(prt_mediasize[i].name) == (ਬਿਨਾਨਿਸ਼ਾਨੀ)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    ਰੋਕੋ;
    ਜੇ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ਜੇ (prt_portrait)
    {
	prt_ਪੰਨਾ_ਚੌੜਾਈ = prt_mediasize[i].ਚੌੜਾਈ;
	prt_ਪੰਨਾ_ਉਚਾਈ = prt_mediasize[i].ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	prt_ਪੰਨਾ_ਚੌੜਾਈ = prt_mediasize[i].ਉਚਾਈ;
	prt_ਪੰਨਾ_ਉਚਾਈ = prt_mediasize[i].ਚੌੜਾਈ;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_ਪੰਨਾ_margins(prt_ਪੰਨਾ_ਚੌੜਾਈ, prt_ਪੰਨਾ_ਉਚਾਈ, &ਖੱਬਾ, &ਸੱਜਾ, &ਸਿਖਰ,
								    &bottom);
    prt_ਖੱਬਾ_margin = (ਦਸ਼ਮਲਵ)ਖੱਬਾ;
    prt_ਸੱਜਾ_margin = (ਦਸ਼ਮਲਵ)ਸੱਜਾ;
    prt_ਸਿਖਰ_margin = (ਦਸ਼ਮਲਵ)ਸਿਖਰ;
    prt_bottom_margin = (ਦਸ਼ਮਲਵ)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    ਲਈ (p = p_pfn; (p = vim_strchr(p, ':')) != ਨਿਰਾ; ++p)
	ਜੇ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((ਅੱਖਰ *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ = prt_get_cpl();
    psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ਜੇ (psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ <= 0 || psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ <= 0)
	ਵਾਪਸ FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_ਅੰਕ_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    ਜੇ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_ਅੰਕ_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    ਜੇ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ਜੇ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ਜੇ (psettings->outfile == ਨਿਰਾ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ਜੇ (prt_ps_file_name == ਨਿਰਾ)
	{
	    emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	    ਵਾਪਸ FAIL;
	}
	prt_ps_fd = mch_fopen((ਅੱਖਰ *)prt_ps_file_name, WRITEBIN);
    }
    ਨਹੀਂਤਾਂ
    {
	p = expand_env_save(psettings->outfile);
	ਜੇ (p != ਨਿਰਾ)
	{
	    prt_ps_fd = mch_fopen((ਅੱਖਰ *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ਜੇ (prt_ps_fd == ਨਿਰਾ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ਵਾਪਸ FAIL;
    }

    prt_bufsiz = psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ;
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_ਬਫਰ, ਆਕਾਰ(ਅੱਖਰ), prt_bufsiz);

    prt_ਪੰਨਾ_ਅੰਕ = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
prt_ਜੋੜੋ_resource(ਬਣਤਰ prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_ਬਫਰ[512];
    size_t	ਬਾਈਟਾਂ_read;

    fd_resource = mch_fopen((ਅੱਖਰ *)resource->filename, READBIN);
    ਜੇ (fd_resource == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ਵਾਪਸ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (ਅੱਖਰ *)resource->title);

    prt_dsc_textline("BeginDocument", (ਅੱਖਰ *)resource->filename);

    ਲਈ (;;)
    {
	ਬਾਈਟਾਂ_read = fread((ਅੱਖਰ *)resource_ਬਫਰ, ਆਕਾਰ(char_u),
			   ਆਕਾਰ(resource_ਬਫਰ), fd_resource);
	ਜੇ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ਵਾਪਸ FALSE;
	}
	ਜੇ (ਬਾਈਟਾਂ_read == 0)
	    ਰੋਕੋ;
	prt_write_file_raw_len(resource_ਬਫਰ, (ਪੂਰਨ)ਬਾਈਟਾਂ_read);
	ਜੇ (prt_file_error)
	{
	    fclose(fd_resource);
	    ਵਾਪਸ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ਵਾਪਸ TRUE;
}

    ਪੂਰਨ
mch_print_begin(prt_settings_T *psettings)
{
    ਪੂਰਨ		bbox[4];
    ਦੁੱਗਣਾ      ਖੱਬਾ;
    ਦੁੱਗਣਾ      ਸੱਜਾ;
    ਦੁੱਗਣਾ      ਸਿਖਰ;
    ਦੁੱਗਣਾ      bottom;
    ਬਣਤਰ prt_ps_resource_S *res_prolog;
    ਬਣਤਰ prt_ps_resource_S *res_encoding;
    ਅੱਖਰ	ਬਫਰ[256];
    char_u      *p_encoding;
    ਬਣਤਰ prt_ps_resource_S *res_cidfont;
    ਬਣਤਰ prt_ps_resource_S *res_cmap;
    ਪੂਰਨ		retval = FALSE;

    res_prolog = ALLOC_ONE(ਬਣਤਰ prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ਬਣਤਰ prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ਬਣਤਰ prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ਬਣਤਰ prt_ps_resource_S);
    ਜੇ (res_prolog == ਨਿਰਾ || res_encoding == ਨਿਰਾ
	    || res_cidfont == ਨਿਰਾ || res_cmap == ਨਿਰਾ)
	ਜਾਓ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_ਸ਼ੁਰੂ();
    prt_dsc_textline("Title", (ਅੱਖਰ *)psettings->jobname);
    ਜੇ (!get_user_name((char_u *)ਬਫਰ, 256))
	STRCPY(ਬਫਰ, "Unknown");
    prt_dsc_textline("For", ਬਫਰ);
    prt_dsc_textline("Creator", VIM_ਸੰਸਕਰਣ_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(ਸਮਾਂ(ਨਿਰਾ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_ਪੰਨਾ_margins(prt_mediasize[prt_media].ਚੌੜਾਈ,
					    prt_mediasize[prt_media].ਉਚਾਈ,
					    &ਖੱਬਾ, &ਸੱਜਾ, &ਸਿਖਰ, &bottom);
    bbox[0] = (ਪੂਰਨ)ਖੱਬਾ;
    ਜੇ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (ਪੂਰਨ)(ਸਿਖਰ - (psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ + prt_ਸਿਰਲੇਖ_ਉਚਾਈ())
						    * (ਦੁੱਗਣਾ)prt_ਸਤਰ_ਉਚਾਈ);
	bbox[2] = (ਪੂਰਨ)(ਖੱਬਾ + psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ
					       * (ਦੁੱਗਣਾ)prt_char_ਚੌੜਾਈ + 0.5);
	bbox[3] = (ਪੂਰਨ)(ਸਿਖਰ + 0.5);
    }
    ਨਹੀਂਤਾਂ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (ਪੂਰਨ)bottom;
	bbox[2] = (ਪੂਰਨ)(ਖੱਬਾ + ((psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ
			      + prt_ਸਿਰਲੇਖ_ਉਚਾਈ()) * prt_ਸਤਰ_ਉਚਾਈ) + 0.5);
	bbox[3] = (ਪੂਰਨ)(bottom + psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ
					       * (ਦੁੱਗਣਾ)prt_char_ਚੌੜਾਈ + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].ਚੌੜਾਈ,
				prt_mediasize[prt_media].ਉਚਾਈ,
				(ਦੁੱਗਣਾ)0, ਨਿਰਾ, ਨਿਰਾ);
    // Define fonts needed
    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ਨਿਰਾ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ਜੇ (!prt_custom_cmap)
	    prt_dsc_resources(ਨਿਰਾ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ਜੇ (!prt_ਲੱਭੋ_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_ਲੱਭੋ_postscript_resource_file_str_ps), "prolog");
	ਜਾਓ theend;
    }
    ਜੇ (!prt_open_resource(res_prolog))
	ਜਾਓ theend;
    ਜੇ (!prt_check_resource(res_prolog, PRT_PROLOG_ਸੰਸਕਰਣ))
	ਜਾਓ theend;
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ਜੇ (!prt_ਲੱਭੋ_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_ਲੱਭੋ_postscript_resource_file_str_ps), "cidfont");
	    ਜਾਓ theend;
	}
	ਜੇ (!prt_open_resource(res_cidfont))
	    ਜਾਓ theend;
	ਜੇ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_ਸੰਸਕਰਣ))
	    ਜਾਓ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ਜੇ (*p_encoding == NUL
		|| !prt_ਲੱਭੋ_resource((ਅੱਖਰ *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    ਪੂਰਨ		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ਜੇ (!(props & ENC_8_ਦੋਆਧਾਰੀ_ਅੰਕ)
		    || !prt_ਲੱਭੋ_resource((ਅੱਖਰ *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ਜੇ (!prt_ਲੱਭੋ_resource((ਅੱਖਰ *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_ਲੱਭੋ_postscript_resource_file_str_ps),
			    p_encoding);
		    ਜਾਓ theend;
		}
	    }
	}
	ਜੇ (!prt_open_resource(res_encoding))
	    ਜਾਓ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ਨਹੀਂਤਾਂ
    {
	p_encoding = enc_skip(p_penc);
	ਜੇ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ਜੇ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ਜੇ (!prt_ਲੱਭੋ_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_ਲੱਭੋ_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		ਜਾਓ theend;
	    }
	    ਜੇ (!prt_open_resource(res_encoding))
		ਜਾਓ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ਜੇ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8_ਦੋਆਧਾਰੀ_ਅੰਕ))
    {
	// Set up encoding conversion if required
	ਜੇ (FAIL == convert_ਤਿਆਰੀ(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_ਮੰਜ਼ਿਲ_convert_ਮੰਜ਼ਿਲ_print_encoding_str), p_encoding);
	    ਜਾਓ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ਜੇ (!prt_ਲੱਭੋ_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_ਲੱਭੋ_postscript_resource_file_str_ps), prt_cmap);
	    ਜਾਓ theend;
	}
	ਜੇ (!prt_open_resource(res_cmap))
	    ਜਾਓ theend;
    }

    // List resources supplied
    STRCPY(ਬਫਰ, res_prolog->title);
    STRCAT(ਬਫਰ, " ");
    STRCAT(ਬਫਰ, res_prolog->ਸੰਸਕਰਣ_2);
    prt_dsc_resources("DocumentSuppliedResources", "procset", ਬਫਰ);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	STRCPY(ਬਫਰ, res_cidfont->title);
	STRCAT(ਬਫਰ, " ");
	STRCAT(ਬਫਰ, res_cidfont->ਸੰਸਕਰਣ_2);
	prt_dsc_resources(ਨਿਰਾ, "procset", ਬਫਰ);

	ਜੇ (prt_custom_cmap)
	{
	    STRCPY(ਬਫਰ, res_cmap->title);
	    STRCAT(ਬਫਰ, " ");
	    STRCAT(ਬਫਰ, res_cmap->ਸੰਸਕਰਣ_2);
	    prt_dsc_resources(ਨਿਰਾ, "cmap", ਬਫਰ);
	}
    }
    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte || prt_use_courier)
    {
	STRCPY(ਬਫਰ, res_encoding->title);
	STRCAT(ਬਫਰ, " ");
	STRCAT(ਬਫਰ, res_encoding->ਸੰਸਕਰਣ_2);
	prt_dsc_resources(ਨਿਰਾ, "encoding", ਬਫਰ);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ਨਹੀਂਤਾਂ
					0
# endif
					, prt_ਅੰਕ_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ਨਿਰਾ : "PageResources"),
							    &prt_ps_mb_font);
	ਜੇ (!prt_custom_cmap)
	    prt_dsc_resources(ਨਿਰਾ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ਜੇ (!prt_ਜੋੜੋ_resource(res_prolog))
	ਜਾਓ theend;
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ਜੇ (!prt_ਜੋੜੋ_resource(res_cidfont))
	    ਜਾਓ theend;
	ਜੇ (prt_custom_cmap && !prt_ਜੋੜੋ_resource(res_cmap))
	    ਜਾਓ theend;
    }

    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ਜੇ (!prt_ਜੋੜੋ_resource(res_encoding))
	    ਜਾਓ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((ਪੂਰਨ)prt_mediasize[prt_media].ਚੌੜਾਈ);
    prt_write_int((ਪੂਰਨ)prt_mediasize[prt_media].ਉਚਾਈ);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_ਅੰਕ_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    ਜੇ (!prt_ਬਾਹਰਲਾ_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ਜੇ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (ਅੱਖਰ *)p_encoding, (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (ਅੱਖਰ *)p_encoding, (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (ਅੱਖਰ *)p_encoding, (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (ਅੱਖਰ *)p_encoding, (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ਜੇ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ਜੇ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ਨਿਰਾ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ਜੇ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ਨਹੀਂਤਾਂ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ਜੇ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ਨਿਰਾ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ਜੇ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ਨਹੀਂਤਾਂ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ਜੇ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ਨਿਰਾ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ਜੇ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (ਪੂਰਨ)prt_ਸਤਰ_ਉਚਾਈ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ਨਹੀਂਤਾਂ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(prt_ਸਤਰ_ਉਚਾਈ,
						prt_ps_font->uline_ਵਿੱਥ), 2);
    prt_def_var("UW", PRT_PS_FONT_ਮੰਜ਼ਿਲ_USER(prt_ਸਤਰ_ਉਚਾਈ,
						 prt_ps_font->uline_ਚੌੜਾਈ), 2);
    prt_def_var("BO", prt_bgcol_ਵਿੱਥ, 2);

    prt_dsc_noarg("EndSetup");

    // Fail if any problems writing out to the PS file
    retval = !prt_file_error;

theend:
    vim_free(res_prolog);
    vim_free(res_encoding);
    vim_free(res_cidfont);
    vim_free(res_cmap);

    ਵਾਪਸ retval;
}

    ਖਾਲੀ
mch_print_ਸਿਰਾ(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_ਪੰਨਾ_ਅੰਕ);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ਜੇ (!prt_file_error && psettings->outfile == ਨਿਰਾ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ਜੇ (prt_ps_fd != ਨਿਰਾ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ਨਿਰਾ;
	}
	prt_ਸੁਨੇਹਾ((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ਜੇ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_ਮੰਜ਼ਿਲ_print_postscript_file));
	ਨਹੀਂਤਾਂ
	    prt_ਸੁਨੇਹਾ((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    ਪੂਰਨ
mch_print_ਸਿਰਾ_ਪੰਨਾ(ਖਾਲੀ)
{
    prt_flush_ਬਫਰ();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ਵਾਪਸ !prt_file_error;
}

    ਪੂਰਨ
mch_print_begin_ਪੰਨਾ(char_u *str ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ਪੰਨਾ_ਅੰਕ[2];

    prt_ਪੰਨਾ_ਅੰਕ++;

    ਪੰਨਾ_ਅੰਕ[0] = ਪੰਨਾ_ਅੰਕ[1] = prt_ਪੰਨਾ_ਅੰਕ;
    prt_dsc_ints("Page", 2, ਪੰਨਾ_ਅੰਕ);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_ਵਿੱਚ_ascii = !prt_ਬਾਹਰਲਾ_mbyte;
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
	prt_write_string("CF0 sf\n");
    ਨਹੀਂਤਾਂ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ਜੇ (!prt_portrait)
    {
	prt_write_int(-((ਪੂਰਨ)prt_mediasize[prt_media].ਚੌੜਾਈ));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ਵਾਪਸ !prt_file_error;
}

    ਪੂਰਨ
mch_print_blank_ਪੰਨਾ(ਖਾਲੀ)
{
    ਵਾਪਸ (mch_print_begin_ਪੰਨਾ(ਨਿਰਾ) ? (mch_print_ਸਿਰਾ_ਪੰਨਾ()) : FALSE);
}

ਸਥਿਰ ਦਸ਼ਮਲਵ prt_pos_x = 0;
ਸਥਿਰ ਦਸ਼ਮਲਵ prt_pos_y = 0;

    ਖਾਲੀ
mch_print_ਸ਼ੁਰੂ_ਸਤਰ(ਪੂਰਨ margin, ਪੂਰਨ ਪੰਨਾ_ਸਤਰ)
{
    prt_pos_x = prt_ਖੱਬਾ_margin;
    ਜੇ (margin)
	prt_pos_x -= prt_ਸੰਖਿਆ_ਚੌੜਾਈ;

    prt_pos_y = prt_ਸਿਖਰ_margin - prt_ਪਹਿਲਾ_ਸਤਰ_ਉਚਾਈ -
					ਪੰਨਾ_ਸਤਰ * prt_ਸਤਰ_ਉਚਾਈ;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_ਚੌੜਾਈ = FALSE;
}

    ਪੂਰਨ
mch_print_ਪਾਠ_ਬਾਹਰਲਾ(char_u *textp, ਪੂਰਨ len ਅਣਵਰਤਿਆ)
{
    char_u	*p = textp;
    ਪੂਰਨ		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    ਦਸ਼ਮਲਵ       char_ਚੌੜਾਈ;
    ਦਸ਼ਮਲਵ       ਅਗਲਾ_pos;
    ਪੂਰਨ		ਵਿੱਚ_ascii;
    ਪੂਰਨ		half_ਚੌੜਾਈ;
    char_u	*tofree = ਨਿਰਾ;

    char_ਚੌੜਾਈ = prt_char_ਚੌੜਾਈ;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ਜੇ (prt_use_courier)
    {
	ਵਿੱਚ_ascii = (len == 1 && *p < 0x80);
	ਜੇ (prt_ਵਿੱਚ_ascii)
	{
	    ਜੇ (!ਵਿੱਚ_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_ਵਿੱਚ_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_ਵਿੱਚ_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	half_ਚੌੜਾਈ = ((*mb_ptr2cells)(p) == 1);
	ਜੇ (half_ਚੌੜਾਈ)
	    char_ਚੌੜਾਈ /= 2;
	ਜੇ (prt_half_ਚੌੜਾਈ)
	{
	    ਜੇ (!half_ਚੌੜਾਈ)
	    {
		prt_half_ਚੌੜਾਈ = FALSE;
		prt_pos_x += prt_char_ਚੌੜਾਈ/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (half_ਚੌੜਾਈ)
	{
	    prt_half_ਚੌੜਾਈ = TRUE;
	    prt_pos_x += prt_char_ਚੌੜਾਈ/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ਜੇ (prt_attribute_change)
    {
	prt_flush_ਬਫਰ();
	// Reset count of number of chars that will be printed
	prt_ਪਾਠ_ਲੜੀਵਾਰ = 0;

	ਜੇ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ਜੇ (prt_need_font)
	{
	    ਜੇ (!prt_ਵਿੱਚ_ascii)
		prt_write_string("CF");
	    ਨਹੀਂਤਾਂ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ਜੇ (prt_need_fgcol)
	{
	    ਪੂਰਨ     r, g, b;
	    r = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)prt_fgcol & 0xff0000) >> 16;
	    g = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ਜੇ (r == g && g == b)
		prt_write_string("g\n");
	    ਨਹੀਂਤਾਂ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ਜੇ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ਜੇ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ਜੇ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ਜੇ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ਜੇ (p == ਨਿਰਾ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ਜੇ (prt_ਬਾਹਰਲਾ_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ਜਦੋਂ (len-- > 0)
	{
	    ch = prt_hexchar[(ਬਿਨਾਨਿਸ਼ਾਨੀ)(*p) >> 4];
	    ga_append(&prt_ps_ਬਫਰ, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_ਬਫਰ, ch);
	    p++;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ਜੇ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_ਬਫਰ, '\\');
	    ਚੋਣ (ch)
	    {
		ਹਾਲਤ BS:   ga_append(&prt_ps_ਬਫਰ, 'b'); ਰੋਕੋ;
		ਹਾਲਤ TAB:  ga_append(&prt_ps_ਬਫਰ, 't'); ਰੋਕੋ;
		ਹਾਲਤ NL:   ga_append(&prt_ps_ਬਫਰ, 'n'); ਰੋਕੋ;
		ਹਾਲਤ FF:   ga_append(&prt_ps_ਬਫਰ, 'f'); ਰੋਕੋ;
		ਹਾਲਤ CAR:  ga_append(&prt_ps_ਬਫਰ, 'r'); ਰੋਕੋ;
		ਹਾਲਤ '(':  ga_append(&prt_ps_ਬਫਰ, '('); ਰੋਕੋ;
		ਹਾਲਤ ')':  ga_append(&prt_ps_ਬਫਰ, ')'); ਰੋਕੋ;
		ਹਾਲਤ '\\': ga_append(&prt_ps_ਬਫਰ, '\\'); ਰੋਕੋ;

		ਮੂਲ:
			   sprintf((ਅੱਖਰ *)ch_buff, "%03o", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ch);
			   ga_append(&prt_ps_ਬਫਰ, ch_buff[0]);
			   ga_append(&prt_ps_ਬਫਰ, ch_buff[1]);
			   ga_append(&prt_ps_ਬਫਰ, ch_buff[2]);
			   ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ga_append(&prt_ps_ਬਫਰ, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_ਪਾਠ_ਲੜੀਵਾਰ += char_ਚੌੜਾਈ;
    prt_pos_x += char_ਚੌੜਾਈ;

    // The downside of fp - use relative error on right margin check
    ਅਗਲਾ_pos = prt_pos_x + prt_char_ਚੌੜਾਈ;
    need_break = (ਅਗਲਾ_pos > prt_ਸੱਜਾ_margin) &&
		    ((ਅਗਲਾ_pos - prt_ਸੱਜਾ_margin) > (prt_ਸੱਜਾ_margin*1e-5));

    ਜੇ (need_break)
	prt_flush_ਬਫਰ();

    ਵਾਪਸ need_break;
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_font(ਪੂਰਨ iBold, ਪੂਰਨ iItalic, ਪੂਰਨ iUnderline)
{
    ਪੂਰਨ		font = 0;

    ਜੇ (iBold)
	font |= 0x01;
    ਜੇ (iItalic)
	font |= 0x02;

    ਜੇ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ਜੇ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_bg(long_u bgcol)
{
    prt_bgcol = (ਪੂਰਨ)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_fg(long_u fgcol)
{
    ਜੇ (fgcol == (long_u)prt_fgcol)
	ਵਾਪਸ;

    prt_fgcol = (ਪੂਰਨ)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#ਜੇ_ਅੰਤ // FEAT_POSTSCRIPT
