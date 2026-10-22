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

#شامل "vim.h"
#شامل "version.h"

#اگر_تعریف_شدہ FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_جدول_T mbfont_opts[OPT_MBFONT_عدد_OPTIONS] =
{
    {"c",	FALSE, 0, تہی, 0, FALSE},
    {"a",	FALSE, 0, تہی, 0, FALSE},
    {"r",	FALSE, 0, تہی, 0, FALSE},
    {"b",	FALSE, 0, تہی, 0, FALSE},
    {"i",	FALSE, 0, تہی, 0, FALSE},
    {"o",	FALSE, 0, تہی, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_بفر_سائز  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ساخت prt_ps_font_S
{
    صحیح		wx;
    صحیح		uline_انحراف;
    صحیح		uline_چوڑائی;
    صحیح		bbox_min_y;
    صحیح		bbox_max_y;
    حرف	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ساکن ساخت prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ساکن ساخت prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {تہی, تہی, تہی, تہی}
};

// Pointer to current font set being used
ساکن ساخت prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ساخت prt_ps_encoding_S
{
    حرف	*encoding;
    حرف	*cmap_encoding;
    صحیح		needs_charset;
};

ساخت prt_ps_charset_S
{
    حرف	*charset;
    حرف	*cmap_charset;
    صحیح		has_charset;
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
ساکن ساخت prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", تہی,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ساکن ساخت prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  تہی,       CS_JIS_X_1983},
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
ساکن ساخت prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    تہی,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     تہی,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ساکن ساخت prt_ps_charset_S sc_charsets[] =
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
ساکن ساخت prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    تہی,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
ساکن ساخت prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    تہی,	CS_B5},
    {"CP950",	    تہی,	CS_B5},
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
ساکن ساخت prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", تہی,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ساکن ساخت prt_ps_charset_S k_charsets[] =
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
ساخت prt_ps_mbfont_S
{
    صحیح				عدد_encodings;
    ساخت prt_ps_encoding_S	*encodings;
    صحیح				عدد_charsets;
    ساخت prt_ps_charset_S	*charsets;
    حرف			*ascii_enc;
    حرف			*defcs;
};

ساکن ساخت prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_لمبائی(j_encodings),
	j_encodings,
	ARRAY_لمبائی(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_لمبائی(sc_encodings),
	sc_encodings,
	ARRAY_لمبائی(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_لمبائی(tc_encodings),
	tc_encodings,
	ARRAY_لمبائی(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_لمبائی(k_encodings),
	k_encodings,
	ARRAY_لمبائی(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

ساخت prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    صحیح     type;
    char_u  title[256];
    char_u  نسخہ_2[256];
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
# define PRT_PROLOG_نسخہ  ((char_u *)"1.4")
# define PRT_CID_PROLOG_نسخہ  ((char_u *)"1.0")

// String versions of PS resource types - indexed by constants above so don't
// re-order!
ساکن حرف *prt_resource_types[] =
{
    "procset",
    "encoding",
    "cmap"
};

// Strings to look for in a PS resource file
# define PRT_RESOURCE_سرخی	    "%!PS-Adobe-"
# define PRT_RESOURCE_RESOURCE	    "Resource-"
# define PRT_RESOURCE_PROCSET	    "ProcSet"
# define PRT_RESOURCE_ENCODING	    "Encoding"
# define PRT_RESOURCE_CMAP	    "CMap"


// Data for table based DSC comment recognition, easy to extend if VIM needs to
// read more comments.
# define PRT_DSC_MISC_TYPE	    (-1)
# define PRT_DSC_TITLE_TYPE	    (1)
# define PRT_DSC_نسخہ_TYPE	    (2)
# define PRT_DSC_ENDCOMMENTS_TYPE    (3)

# define PRT_DSC_TITLE		    "%%Title:"
# define PRT_DSC_نسخہ		    "%%Version:"
# define PRT_DSC_ENDCOMMENTS	    "%%EndComments:"

ساخت prt_dsc_comment_S
{
    حرف	*string;
    صحیح		len;
    صحیح		type;
};

ساخت prt_dsc_سطر_S
{
    صحیح		type;
    char_u	*string;
    صحیح		len;
};


# define SIZEOF_CSTR(s)      (حجم(s) - 1)
ساکن ساخت prt_dsc_comment_S prt_dsc_جدول[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_نسخہ,     SIZEOF_CSTR(PRT_DSC_نسخہ),
							PRT_DSC_نسخہ_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ساکن خالی prt_write_file_len(char_u *بفر, صحیح بائٹیں);
ساکن صحیح prt_اگلا_dsc(ساخت prt_dsc_سطر_S *p_dsc_سطر);

/*
 * Variables for the output PostScript file.
 */
ساکن FILE *prt_ps_fd;
ساکن صحیح prt_file_error;
ساکن char_u *prt_ps_file_name = تہی;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ساکن اعشاری prt_صفحہ_چوڑائی;
ساکن اعشاری prt_صفحہ_اونچائی;
ساکن اعشاری prt_بایاں_margin;
ساکن اعشاری prt_دایاں_margin;
ساکن اعشاری prt_چوٹی_margin;
ساکن اعشاری prt_bottom_margin;
ساکن اعشاری prt_سطر_اونچائی;
ساکن اعشاری prt_پہلا_سطر_اونچائی;
ساکن اعشاری prt_char_چوڑائی;
ساکن اعشاری prt_نمبر_چوڑائی;
ساکن اعشاری prt_bgcol_انحراف;
ساکن اعشاری prt_pos_x_moveto = 0.0;
ساکن اعشاری prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ساکن صحیح prt_need_moveto;
ساکن صحیح prt_do_moveto;
ساکن صحیح prt_need_font;
ساکن صحیح prt_font;
ساکن صحیح prt_need_underline;
ساکن صحیح prt_underline;
ساکن صحیح prt_do_underline;
ساکن صحیح prt_need_fgcol;
ساکن صحیح prt_fgcol;
ساکن صحیح prt_need_bgcol;
ساکن صحیح prt_do_bgcol;
ساکن صحیح prt_bgcol;
ساکن صحیح prt_new_bgcol;
ساکن صحیح prt_attribute_change;
ساکن اعشاری prt_متن_تسلسل;
ساکن صحیح prt_صفحہ_عدد;
ساکن صحیح prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ساکن صحیح prt_media;
ساکن صحیح prt_portrait;
ساکن صحیح prt_عدد_copies;
ساکن صحیح prt_duplex;
ساکن صحیح prt_tumble;
ساکن صحیح prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ساکن char_u prt_سطر_بفر[257];
ساکن garray_T prt_ps_بفر;

ساکن صحیح prt_do_conv;
ساکن vimconv_T prt_conv;

ساکن صحیح prt_برآمد_mbyte;
ساکن صحیح prt_custom_cmap;
ساکن حرف prt_cmap[80];
ساکن صحیح prt_use_courier;
ساکن صحیح prt_اندر_ascii;
ساکن صحیح prt_half_چوڑائی;
ساکن حرف *prt_ascii_encoding;
ساکن ثابت char_u prt_hexchar[] = "0123456789abcdef";

    ساکن خالی
prt_write_file_raw_len(char_u *بفر, صحیح بائٹیں)
{
    اگر (!prt_file_error
	    && fwrite(بفر, حجم(char_u), بائٹیں, prt_ps_fd)
							     != (size_t)بائٹیں)
    {
	emsg(_(e_error_writing_مقصد_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ساکن خالی
prt_write_file(char_u *بفر)
{
    prt_write_file_len(بفر, (صحیح)STRLEN(بفر));
}

    ساکن خالی
prt_write_file_len(char_u *بفر, صحیح بائٹیں)
{
    prt_write_file_raw_len(بفر, بائٹیں);
}

/*
 * Write a string.
 */
    ساکن خالی
prt_write_string(حرف *s)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر), "%s", s);
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write an int and a space.
 */
    ساکن خالی
prt_write_int(صحیح i)
{
    sprintf((حرف *)prt_سطر_بفر, "%d ", i);
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write a boolean and a space.
 */
    ساکن خالی
prt_write_boolean(صحیح b)
{
    sprintf((حرف *)prt_سطر_بفر, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ساکن خالی
prt_def_font(
    حرف	*new_name,
    حرف	*encoding,
    صحیح		اونچائی,
    حرف	*font)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_سطر_بفر);
    اگر (prt_برآمد_mbyte)
	sprintf((حرف *)prt_سطر_بفر, "/%s %d %f /_%s sffs\n",
		       new_name, اونچائی, 500./prt_ps_courier_font.wx, new_name);
    ورنہ
	vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
			     "/%s %d /_%s ffs\n", new_name, اونچائی, new_name);
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write a line to define the CID font.
 */
    ساکن خالی
prt_def_cidfont(حرف *new_name, صحیح اونچائی, حرف *cidfont)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_سطر_بفر);
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
			     "/%s %d /_%s ffs\n", new_name, اونچائی, new_name);
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ساکن خالی
prt_dup_cidfont(حرف *original_name, حرف *new_name)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_سطر_بفر);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ساکن خالی
prt_real_بٹ(
    دوہرا      real,
    صحیح		precision,
    صحیح		*pinteger,
    صحیح		*pfraction)
{
    صحیح     i;
    صحیح     integer;
    اعشاری   fraction;

    integer = (صحیح)real;
    fraction = (اعشاری)(real - integer);
    اگر (real < (دوہرا)integer)
	fraction = -fraction;
    برائے (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (صحیح)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ساکن خالی
prt_write_real(دوہرا val, صحیح prec)
{
    صحیح     integer;
    صحیح     fraction;

    prt_real_بٹ(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((حرف *)prt_سطر_بفر, "%d", integer);
    prt_write_file(prt_سطر_بفر);
    // Only emit fraction if necessary
    اگر (fraction != 0)
    {
	// Remove any trailing zeros
	جب ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((حرف *)prt_سطر_بفر, ".%0*d", prec, fraction);
	prt_write_file(prt_سطر_بفر);
    }
    sprintf((حرف *)prt_سطر_بفر, " ");
    prt_write_file(prt_سطر_بفر);
}

/*
 * Write a line to define a numeric variable.
 */
    ساکن خالی
prt_def_var(حرف *name, دوہرا قدر, صحیح prec)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
								"/%s ", name);
    prt_write_file(prt_سطر_بفر);
    prt_write_real(قدر, prec);
    sprintf((حرف *)prt_سطر_بفر, "d\n");
    prt_write_file(prt_سطر_بفر);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_مقصد_USER(scale, سائز)    ((سائز) * ((scale)/1000.0))

    ساکن خالی
prt_flush_بفر(خالی)
{
    اگر (prt_ps_بفر.ga_len <= 0)
	واپس;

    // Any background color must be drawn first
    اگر (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	صحیح     r, g, b;

	اگر (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_متن_تسلسل, 2);
	prt_write_real(prt_سطر_اونچائی, 2);

	// Lastly add the color of the background
	r = ((غیرعلامتی)prt_new_bgcol & 0xff0000) >> 16;
	g = ((غیرعلامتی)prt_new_bgcol & 0xff00) >> 8;
	b = prt_new_bgcol & 0xff;
	prt_write_real(r / 255.0, 3);
	prt_write_real(g / 255.0, 3);
	prt_write_real(b / 255.0, 3);
	prt_write_string("bg\n");
    }
    // Draw underlines before the text as it makes it slightly easier to
    // find the starting point.
    اگر (prt_do_underline)
    {
	اگر (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Underline length of text run
	prt_write_real(prt_متن_تسلسل, 2);
	prt_write_string("ul\n");
    }
    // Draw the text
    اگر (prt_برآمد_mbyte)
	prt_write_string("<");
    ورنہ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_بفر.ga_ڈیٹا, prt_ps_بفر.ga_len);
    اگر (prt_برآمد_mbyte)
	prt_write_string(">");
    ورنہ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    اگر (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ورنہ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_بفر);
    ga_init2(&prt_ps_بفر, حجم(حرف), prt_bufsiz);
}


    ساکن خالی
prt_resource_name(char_u *filename, خالی *cookie)
{
    char_u *resource_filename = cookie;

    اگر (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ورنہ
	STRCPY(resource_filename, filename);
}

    ساکن صحیح
prt_ڈھونڈو_resource(حرف *name, ساخت prt_ps_resource_S *resource)
{
    char_u	*بفر;
    صحیح		retval;

    بفر = alloc(MAXPATHL + 1);
    اگر (بفر == تہی)
	واپس FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(بفر, "print");
    جوڑو_pathsep(بفر);
    vim_strcat(بفر, (char_u *)name, MAXPATHL);
    vim_strcat(بفر, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_اندر_runtimepath(بفر, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(بفر);
    واپس retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_بفر_LEN (2048)
ساخت prt_resfile_بفر_S
{
    char_u  بفر[PRT_FILE_بفر_LEN];
    صحیح     len;
    صحیح     سطر_آغاز;
    صحیح     سطر_انتہا;
};

ساکن ساخت prt_resfile_بفر_S prt_resfile;

    ساکن صحیح
prt_resfile_اگلا_سطر(خالی)
{
    صحیح     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.سطر_انتہا + 1;
    جب (idx < prt_resfile.len)
    {
	اگر (prt_resfile.بفر[idx] != PSLF && prt_resfile.بفر[idx] != PSCR)
	    توڑو;
	idx++;
    }
    prt_resfile.سطر_آغاز = idx;

    جب (idx < prt_resfile.len)
    {
	اگر (prt_resfile.بفر[idx] == PSLF || prt_resfile.بفر[idx] == PSCR)
	    توڑو;
	idx++;
    }
    prt_resfile.سطر_انتہا = idx;

    واپس (idx < prt_resfile.len);
}

    ساکن صحیح
prt_resfile_strncmp(صحیح انحراف, حرف *string, صحیح len)
{
    // Force not equal if string is longer than remainder of line
    اگر (len > (prt_resfile.سطر_انتہا - (prt_resfile.سطر_آغاز + انحراف)))
	واپس 1;

    واپس STRNCMP(&prt_resfile.بفر[prt_resfile.سطر_آغاز + انحراف],
								string, len);
}

    ساکن صحیح
prt_resfile_skip_nonws(صحیح انحراف)
{
    صحیح     idx;

    idx = prt_resfile.سطر_آغاز + انحراف;
    جب (idx < prt_resfile.سطر_انتہا)
    {
	اگر (محفوظ_isspace(prt_resfile.بفر[idx]))
	    واپس idx - prt_resfile.سطر_آغاز;
	idx++;
    }
    واپس -1;
}

    ساکن صحیح
prt_resfile_skip_ws(صحیح انحراف)
{
    صحیح     idx;

    idx = prt_resfile.سطر_آغاز + انحراف;
    جب (idx < prt_resfile.سطر_انتہا)
    {
	اگر (!محفوظ_isspace(prt_resfile.بفر[idx]))
	    واپس idx - prt_resfile.سطر_آغاز;
	idx++;
    }
    واپس -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ساکن صحیح
prt_اگلا_dsc(ساخت prt_dsc_سطر_S *p_dsc_سطر)
{
    صحیح     comment;
    صحیح     انحراف;

    // Move to start of next line
    اگر (!prt_resfile_اگلا_سطر())
	واپس FALSE;

    // DSC comments always start %%
    اگر (prt_resfile_strncmp(0, "%%", 2) != 0)
	واپس FALSE;

    // Find type of DSC comment
    برائے (comment = 0; comment < (صحیح)ARRAY_لمبائی(prt_dsc_جدول); comment++)
	اگر (prt_resfile_strncmp(0, prt_dsc_جدول[comment].string,
					    prt_dsc_جدول[comment].len) == 0)
	    توڑو;

    اگر (comment != ARRAY_لمبائی(prt_dsc_جدول))
    {
	// Return type of comment
	p_dsc_سطر->type = prt_dsc_جدول[comment].type;
	انحراف = prt_dsc_جدول[comment].len;
    }
    ورنہ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_سطر->type = PRT_DSC_MISC_TYPE;
	انحراف = prt_resfile_skip_nonws(0);
	اگر (انحراف == -1)
	    واپس FALSE;
    }

    // Skip ws to comment value
    انحراف = prt_resfile_skip_ws(انحراف);
    اگر (انحراف == -1)
	واپس FALSE;

    p_dsc_سطر->string = &prt_resfile.بفر[prt_resfile.سطر_آغاز + انحراف];
    p_dsc_سطر->len = prt_resfile.سطر_انتہا - (prt_resfile.سطر_آغاز + انحراف);

    واپس TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ساکن صحیح
prt_open_resource(ساخت prt_ps_resource_S *resource)
{
    صحیح		انحراف;
    صحیح		نظر_آیا_all;
    صحیح		نظر_آیا_title;
    صحیح		نظر_آیا_نسخہ;
    FILE	*fd_resource;
    ساخت prt_dsc_سطر_S dsc_سطر;

    fd_resource = mch_fopen((حرف *)resource->filename, READBIN);
    اگر (fd_resource == تہی)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	واپس FALSE;
    }
    CLEAR_میدان(prt_resfile.بفر);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (صحیح)fread((حرف *)prt_resfile.بفر, حجم(char_u),
					    PRT_FILE_بفر_LEN, fd_resource);
    اگر (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	واپس FALSE;
    }
    fclose(fd_resource);

    prt_resfile.سطر_انتہا = -1;
    prt_resfile.سطر_آغاز = 0;
    اگر (!prt_resfile_اگلا_سطر())
	واپس FALSE;

    انحراف = 0;

    اگر (prt_resfile_strncmp(انحراف, PRT_RESOURCE_سرخی,
			       (صحیح)STRLEN_LITERAL(PRT_RESOURCE_سرخی)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	واپس FALSE;
    }

    // Skip over any version numbers and following ws
    انحراف += (صحیح)STRLEN_LITERAL(PRT_RESOURCE_سرخی);
    انحراف = prt_resfile_skip_nonws(انحراف);
    اگر (انحراف == -1)
	واپس FALSE;
    انحراف = prt_resfile_skip_ws(انحراف);
    اگر (انحراف == -1)
	واپس FALSE;

    اگر (prt_resfile_strncmp(انحراف, PRT_RESOURCE_RESOURCE,
			     (صحیح)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	واپس FALSE;
    }
    انحراف += (صحیح)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    اگر (prt_resfile_strncmp(انحراف, PRT_RESOURCE_PROCSET,
			      (صحیح)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ورنہ اگر (prt_resfile_strncmp(انحراف, PRT_RESOURCE_ENCODING,
			     (صحیح)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ورنہ اگر (prt_resfile_strncmp(انحراف, PRT_RESOURCE_CMAP,
				 (صحیح)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ورنہ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	واپس FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->نسخہ_2[0] = '\0';
    نظر_آیا_title = FALSE;
    نظر_آیا_نسخہ = FALSE;
    نظر_آیا_all = FALSE;
    جب (!نظر_آیا_all && prt_اگلا_dsc(&dsc_سطر))
    {
	انتخاب (dsc_سطر.type)
	{
	حالت PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_سطر.string, dsc_سطر.len);
	    نظر_آیا_title = TRUE;
	    اگر (نظر_آیا_نسخہ)
		نظر_آیا_all = TRUE;
	    توڑو;

	حالت PRT_DSC_نسخہ_TYPE:
	    vim_strncpy(resource->نسخہ_2, dsc_سطر.string, dsc_سطر.len);
	    نظر_آیا_نسخہ = TRUE;
	    اگر (نظر_آیا_title)
		نظر_آیا_all = TRUE;
	    توڑو;

	حالت PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    نظر_آیا_all = TRUE;
	    توڑو;

	حالت PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    توڑو;
	}
    }

    اگر (!نظر_آیا_title || !نظر_آیا_نسخہ)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	واپس FALSE;
    }

    واپس TRUE;
}

    ساکن صحیح
prt_check_resource(ساخت prt_ps_resource_S *resource, char_u *نسخہ_2)
{
    // Version number m.n should match, the revision number does not matter
    اگر (STRNCMP(resource->نسخہ_2, نسخہ_2, STRLEN(نسخہ_2)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_نسخہ),
		resource->name);
	واپس FALSE;
    }

    // Other checks to be added as needed
    واپس TRUE;
}

    ساکن خالی
prt_dsc_آغاز(خالی)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ساکن خالی
prt_dsc_noarg(حرف *comment)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
							 "%%%%%s\n", comment);
    prt_write_file(prt_سطر_بفر);
}

    ساکن خالی
prt_dsc_textline(حرف *comment, حرف *متن)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
					       "%%%%%s: %s\n", comment, متن);
    prt_write_file(prt_سطر_بفر);
}

    ساکن خالی
prt_dsc_متن(حرف *comment, حرف *متن)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
					     "%%%%%s: (%s)\n", comment, متن);
    prt_write_file(prt_سطر_بفر);
}

# define prt_dsc_atend(c)	prt_dsc_متن((c), "atend")

    ساکن خالی
prt_dsc_ints(حرف *comment, صحیح گنتی, صحیح *ints)
{
    صحیح		i;

    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
							  "%%%%%s:", comment);
    prt_write_file(prt_سطر_بفر);

    برائے (i = 0; i < گنتی; i++)
    {
	sprintf((حرف *)prt_سطر_بفر, " %d", ints[i]);
	prt_write_file(prt_سطر_بفر);
    }

    prt_write_string("\n");
}

    ساکن خالی
prt_dsc_resources(
    حرف	*comment,	// if NULL add to previous
    حرف	*type,
    حرف	*string)
{
    اگر (comment != تہی)
	vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
						 "%%%%%s: %s", comment, type);
    ورنہ
	vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
							    "%%%%+ %s", type);
    prt_write_file(prt_سطر_بفر);

    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
							     " %s\n", string);
    prt_write_file(prt_سطر_بفر);
}

    ساکن خالی
prt_dsc_font_resource(حرف *resource, ساخت prt_ps_font_S *ps_font)
{
    صحیح     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    برائے (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	اگر (ps_font->ps_fontname[i] != تہی)
	    prt_dsc_resources(تہی, "font", ps_font->ps_fontname[i]);
}

    ساکن خالی
prt_dsc_requirements(
    صحیح		duplex,
    صحیح		tumble,
    صحیح		collate,
    صحیح		color,
    صحیح		عدد_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    اگر (!(duplex || collate || color || (عدد_copies > 1)))
	واپس;

    sprintf((حرف *)prt_سطر_بفر, "%%%%Requirements:");
    prt_write_file(prt_سطر_بفر);

    اگر (duplex)
    {
	prt_write_string(" duplex");
	اگر (tumble)
	    prt_write_string("(tumble)");
    }
    اگر (collate)
	prt_write_string(" collate");
    اگر (color)
	prt_write_string(" color");
    اگر (عدد_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((حرف *)prt_سطر_بفر, "%d", عدد_copies);
	prt_write_file(prt_سطر_بفر);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ساکن خالی
prt_dsc_docmedia(
    حرف	*paper_name,
    دوہرا	چوڑائی,
    دوہرا	اونچائی,
    دوہرا	وزن,
    حرف	*رنگ,
    حرف	*type)
{
    vim_snprintf((حرف *)prt_سطر_بفر, حجم(prt_سطر_بفر),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_سطر_بفر);
    prt_write_real(چوڑائی, 2);
    prt_write_real(اونچائی, 2);
    prt_write_real(وزن, 2);
    اگر (رنگ == تہی)
	prt_write_string("()");
    ورنہ
	prt_write_string(رنگ);
    prt_write_string(" ");
    اگر (type == تہی)
	prt_write_string("()");
    ورنہ
	prt_write_string(type);
    prt_write_string("\n");
}

    خالی
mch_print_cleanup(خالی)
{
    اگر (prt_برآمد_mbyte)
    {
	صحیح     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	برائے (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    اگر (prt_ps_mb_font.ps_fontname[i] != تہی)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    اگر (prt_do_conv)
    {
	convert_تیاری(&prt_conv, تہی, تہی);
	prt_do_conv = FALSE;
    }
    اگر (prt_ps_fd != تہی)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = تہی;
	prt_file_error = FALSE;
    }
    اگر (prt_ps_file_name != تہی)
	VIM_CLEAR(prt_ps_file_name);
}

    ساکن اعشاری
مقصد_device_units(صحیح idx, دوہرا physsize, صحیح def_نمبر)
{
    اعشاری	ret;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_نمبر;
    }
    ورنہ
	nr = printer_opts[idx].نمبر;

    انتخاب (u)
    {
	حالت PRT_UNIT_INCH:
	    ret = (اعشاری)(nr * PRT_PS_DEFAULT_DPI);
	    توڑو;
	حالت PRT_UNIT_MM:
	    ret = (اعشاری)(nr * PRT_PS_DEFAULT_DPI) / (اعشاری)25.4;
	    توڑو;
	حالت PRT_UNIT_نقطہ:
	    ret = (اعشاری)nr;
	    توڑو;
	حالت PRT_UNIT_PERC:
	طےشدہ:
	    ret = (اعشاری)(physsize * nr) / 100;
	    توڑو;
    }

    واپس ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ساکن خالی
prt_صفحہ_margins(
    دوہرا	چوڑائی,
    دوہرا	اونچائی,
    دوہرا	*بایاں,
    دوہرا	*دایاں,
    دوہرا	*چوٹی,
    دوہرا	*bottom)
{
    *بایاں   = مقصد_device_units(OPT_PRINT_بایاں, چوڑائی, 10);
    *دایاں  = چوڑائی - مقصد_device_units(OPT_PRINT_دایاں, چوڑائی, 5);
    *چوٹی    = اونچائی - مقصد_device_units(OPT_PRINT_چوٹی, اونچائی, 5);
    *bottom = مقصد_device_units(OPT_PRINT_BOT, اونچائی, 5);
}

    ساکن خالی
prt_font_metrics(صحیح font_scale)
{
    prt_سطر_اونچائی = (اعشاری)font_scale;
    prt_char_چوڑائی = (اعشاری)PRT_PS_FONT_مقصد_USER(font_scale, prt_ps_font->wx);
}


    ساکن صحیح
prt_get_cpl(خالی)
{
    اگر (prt_use_نمبر())
    {
	prt_نمبر_چوڑائی = PRINT_نمبر_چوڑائی * prt_char_چوڑائی;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	اگر (prt_برآمد_mbyte)
	    prt_نمبر_چوڑائی /= 2;
	prt_بایاں_margin += prt_نمبر_چوڑائی;
    }
    ورنہ
	prt_نمبر_چوڑائی = 0.0;

    واپس (صحیح)((prt_دایاں_margin - prt_بایاں_margin) / prt_char_چوڑائی);
}

    ساکن صحیح
prt_build_cid_fontname(صحیح font, char_u *name, صحیح name_len)
{
    حرف    *fontname;

    fontname = alloc(name_len + 1);
    اگر (fontname == تہی)
	واپس FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    واپس TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ساکن صحیح
prt_get_lpp(خالی)
{
    صحیح lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_انحراف = (اعشاری)PRT_PS_FONT_مقصد_USER(prt_سطر_اونچائی,
					   prt_ps_font->bbox_min_y);
    اگر ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_انحراف -= (اعشاری)PRT_PS_FONT_مقصد_USER(prt_سطر_اونچائی,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_پہلا_سطر_اونچائی = prt_سطر_اونچائی + prt_bgcol_انحراف;

    // Calculate lpp
    lpp = (صحیح)((prt_چوٹی_margin - prt_bottom_margin) / prt_سطر_اونچائی);

    // Adjust top margin if there is a header
    prt_چوٹی_margin -= prt_سطر_اونچائی * prt_سرخی_اونچائی();

    واپس lpp - prt_سرخی_اونچائی();
}

    ساکن صحیح
prt_مطابقت_encoding(
    حرف			*p_encoding,
    ساخت prt_ps_mbfont_S	*p_cmap,
    ساخت prt_ps_encoding_S	**pp_mbenc)
{
    صحیح				mbenc;
    صحیح				enc_len;
    ساخت prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = تہی;
    // Look for recognised encoding
    enc_len = (صحیح)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    برائے (mbenc = 0; mbenc < p_cmap->عدد_encodings; mbenc++)
    {
	اگر (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    واپس TRUE;
	}
	p_mbenc++;
    }
    واپس FALSE;
}

    ساکن صحیح
prt_مطابقت_charset(
    حرف		    *p_charset,
    ساخت prt_ps_mbfont_S  *p_cmap,
    ساخت prt_ps_charset_S **pp_mbchar)
{
    صحیح			    mbchar;
    صحیح			    char_len;
    ساخت prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    اگر (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (صحیح)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    برائے (mbchar = 0; mbchar < p_cmap->عدد_charsets; mbchar++)
    {
	اگر (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    واپس TRUE;
	}
	p_mbchar++;
    }
    واپس FALSE;
}

    صحیح
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    صحیح		forceit غیر_مستعمل)
{
    صحیح		i;
    حرف	*paper_name;
    صحیح		paper_strlen;
    صحیح		fontsize;
    char_u	*p;
    دوہرا      بایاں;
    دوہرا      دایاں;
    دوہرا      چوٹی;
    دوہرا      bottom;
    صحیح		props;
    صحیح		cmap = 0;
    char_u	*p_encoding;
    ساخت prt_ps_encoding_S *p_mbenc;
    ساخت prt_ps_encoding_S *p_mbenc_پہلا;
    ساخت prt_ps_charset_S  *p_mbchar = تہی;

# اگر 0
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
    اگر (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    اگر (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = تہی;
    props = enc_canon_props(p_encoding);
    اگر (!(props & ENC_8_ثنائی_ہندسہ) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	صحیح cmap_پہلا = 0;

	p_mbenc_پہلا = تہی;
	برائے (cmap = 0; cmap < (صحیح)ARRAY_لمبائی(prt_ps_mbfonts); cmap++)
	    اگر (prt_مطابقت_encoding((حرف *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		اگر (p_mbenc_پہلا == تہی)
		{
		    p_mbenc_پہلا = p_mbenc;
		    cmap_پہلا = cmap;
		}
		اگر (prt_مطابقت_charset((حرف *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    توڑو;
	    }

	// Use first encoding matched if no charset matched
	اگر (p_mbchar == تہی && p_mbenc_پہلا != تہی)
	{
	    p_mbenc = p_mbenc_پہلا;
	    cmap = cmap_پہلا;
	}
    }

    prt_برآمد_mbyte = (p_mbenc != تہی);
    اگر (prt_برآمد_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == تہی);
	اگر (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    اگر ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_سیٹ));
		واپس FALSE;
	    }

	    // Add charset name if not empty
	    اگر (p_mbchar->cmap_charset != تہی)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, حجم(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ورنہ
	{
	    // Add custom CMap character set name
	    اگر (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_خالی_خانے_with_multi_byte_encoding));
		واپس FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, حجم(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	اگر (p_mbenc->cmap_encoding != تہی && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < حجم(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	اگر (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    واپس FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	اگر (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].لڑی_لمبائی))
	    واپس FALSE;
	اگر (mbfont_opts[OPT_MBFONT_BOLD].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].لڑی_لمبائی))
		واپس FALSE;
	اگر (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].لڑی_لمبائی))
		واپس FALSE;
	اگر (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].لڑی_لمبائی))
		واپس FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	اگر (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    اگر (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ورنہ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ورنہ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    اگر (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (حرف *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].لڑی_لمبائی;
    }
    ورنہ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    برائے (i = 0; i < (صحیح)PRT_MEDIASIZE_LEN; ++i)
	اگر (STRLEN(prt_mediasize[i].name) == (غیرعلامتی)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    توڑو;
    اگر (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    اگر (prt_portrait)
    {
	prt_صفحہ_چوڑائی = prt_mediasize[i].چوڑائی;
	prt_صفحہ_اونچائی = prt_mediasize[i].اونچائی;
    }
    ورنہ
    {
	prt_صفحہ_چوڑائی = prt_mediasize[i].اونچائی;
	prt_صفحہ_اونچائی = prt_mediasize[i].چوڑائی;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_صفحہ_margins(prt_صفحہ_چوڑائی, prt_صفحہ_اونچائی, &بایاں, &دایاں, &چوٹی,
								    &bottom);
    prt_بایاں_margin = (اعشاری)بایاں;
    prt_دایاں_margin = (اعشاری)دایاں;
    prt_چوٹی_margin = (اعشاری)چوٹی;
    prt_bottom_margin = (اعشاری)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    برائے (p = p_pfn; (p = vim_strchr(p, ':')) != تہی; ++p)
	اگر (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((حرف *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_فی_سطر = prt_get_cpl();
    psettings->سطریں_فی_صفحہ = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    اگر (psettings->chars_فی_سطر <= 0 || psettings->سطریں_فی_صفحہ <= 0)
	واپس FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_عدد_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    اگر (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ورنہ
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_عدد_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    اگر (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ورنہ اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    اگر (psettings->outfile == تہی)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	اگر (prt_ps_file_name == تہی)
	{
	    emsg(_(e_cant_get_عارضی_file_name));
	    واپس FAIL;
	}
	prt_ps_fd = mch_fopen((حرف *)prt_ps_file_name, WRITEBIN);
    }
    ورنہ
    {
	p = expand_env_save(psettings->outfile);
	اگر (p != تہی)
	{
	    prt_ps_fd = mch_fopen((حرف *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    اگر (prt_ps_fd == تہی)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	واپس FAIL;
    }

    prt_bufsiz = psettings->chars_فی_سطر;
    اگر (prt_برآمد_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_بفر, حجم(حرف), prt_bufsiz);

    prt_صفحہ_عدد = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    واپس OK;
}

    ساکن صحیح
prt_جوڑو_resource(ساخت prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_بفر[512];
    size_t	بائٹیں_read;

    fd_resource = mch_fopen((حرف *)resource->filename, READBIN);
    اگر (fd_resource == تہی)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	واپس FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (حرف *)resource->title);

    prt_dsc_textline("BeginDocument", (حرف *)resource->filename);

    برائے (;;)
    {
	بائٹیں_read = fread((حرف *)resource_بفر, حجم(char_u),
			   حجم(resource_بفر), fd_resource);
	اگر (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    واپس FALSE;
	}
	اگر (بائٹیں_read == 0)
	    توڑو;
	prt_write_file_raw_len(resource_بفر, (صحیح)بائٹیں_read);
	اگر (prt_file_error)
	{
	    fclose(fd_resource);
	    واپس FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    واپس TRUE;
}

    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    صحیح		bbox[4];
    دوہرا      بایاں;
    دوہرا      دایاں;
    دوہرا      چوٹی;
    دوہرا      bottom;
    ساخت prt_ps_resource_S *res_prolog;
    ساخت prt_ps_resource_S *res_encoding;
    حرف	بفر[256];
    char_u      *p_encoding;
    ساخت prt_ps_resource_S *res_cidfont;
    ساخت prt_ps_resource_S *res_cmap;
    صحیح		retval = FALSE;

    res_prolog = ALLOC_ONE(ساخت prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ساخت prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ساخت prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ساخت prt_ps_resource_S);
    اگر (res_prolog == تہی || res_encoding == تہی
	    || res_cidfont == تہی || res_cmap == تہی)
	جاؤ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_آغاز();
    prt_dsc_textline("Title", (حرف *)psettings->jobname);
    اگر (!get_user_name((char_u *)بفر, 256))
	STRCPY(بفر, "Unknown");
    prt_dsc_textline("For", بفر);
    prt_dsc_textline("Creator", VIM_نسخہ_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(وقت(تہی), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_صفحہ_margins(prt_mediasize[prt_media].چوڑائی,
					    prt_mediasize[prt_media].اونچائی,
					    &بایاں, &دایاں, &چوٹی, &bottom);
    bbox[0] = (صحیح)بایاں;
    اگر (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (صحیح)(چوٹی - (psettings->سطریں_فی_صفحہ + prt_سرخی_اونچائی())
						    * (دوہرا)prt_سطر_اونچائی);
	bbox[2] = (صحیح)(بایاں + psettings->chars_فی_سطر
					       * (دوہرا)prt_char_چوڑائی + 0.5);
	bbox[3] = (صحیح)(چوٹی + 0.5);
    }
    ورنہ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (صحیح)bottom;
	bbox[2] = (صحیح)(بایاں + ((psettings->سطریں_فی_صفحہ
			      + prt_سرخی_اونچائی()) * prt_سطر_اونچائی) + 0.5);
	bbox[3] = (صحیح)(bottom + psettings->chars_فی_سطر
					       * (دوہرا)prt_char_چوڑائی + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].چوڑائی,
				prt_mediasize[prt_media].اونچائی,
				(دوہرا)0, تہی, تہی);
    // Define fonts needed
    اگر (!prt_برآمد_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    اگر (prt_برآمد_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? تہی
				: "DocumentNeededResources"), &prt_ps_mb_font);
	اگر (!prt_custom_cmap)
	    prt_dsc_resources(تہی, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    اگر (!prt_ڈھونڈو_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_ڈھونڈو_postscript_resource_file_str_ps), "prolog");
	جاؤ theend;
    }
    اگر (!prt_open_resource(res_prolog))
	جاؤ theend;
    اگر (!prt_check_resource(res_prolog, PRT_PROLOG_نسخہ))
	جاؤ theend;
    اگر (prt_برآمد_mbyte)
    {
	// Look for required version of multi-byte printing procset
	اگر (!prt_ڈھونڈو_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_ڈھونڈو_postscript_resource_file_str_ps), "cidfont");
	    جاؤ theend;
	}
	اگر (!prt_open_resource(res_cidfont))
	    جاؤ theend;
	اگر (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_نسخہ))
	    جاؤ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    اگر (!prt_برآمد_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	اگر (*p_encoding == NUL
		|| !prt_ڈھونڈو_resource((حرف *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    صحیح		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    اگر (!(props & ENC_8_ثنائی_ہندسہ)
		    || !prt_ڈھونڈو_resource((حرف *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		اگر (!prt_ڈھونڈو_resource((حرف *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_ڈھونڈو_postscript_resource_file_str_ps),
			    p_encoding);
		    جاؤ theend;
		}
	    }
	}
	اگر (!prt_open_resource(res_encoding))
	    جاؤ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ورنہ
    {
	p_encoding = enc_skip(p_penc);
	اگر (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	اگر (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    اگر (!prt_ڈھونڈو_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_ڈھونڈو_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		جاؤ theend;
	    }
	    اگر (!prt_open_resource(res_encoding))
		جاؤ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    اگر (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8_ثنائی_ہندسہ))
    {
	// Set up encoding conversion if required
	اگر (FAIL == convert_تیاری(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_مقصد_convert_مقصد_print_encoding_str), p_encoding);
	    جاؤ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    اگر (prt_برآمد_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	اگر (!prt_ڈھونڈو_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_ڈھونڈو_postscript_resource_file_str_ps), prt_cmap);
	    جاؤ theend;
	}
	اگر (!prt_open_resource(res_cmap))
	    جاؤ theend;
    }

    // List resources supplied
    STRCPY(بفر, res_prolog->title);
    STRCAT(بفر, " ");
    STRCAT(بفر, res_prolog->نسخہ_2);
    prt_dsc_resources("DocumentSuppliedResources", "procset", بفر);
    اگر (prt_برآمد_mbyte)
    {
	STRCPY(بفر, res_cidfont->title);
	STRCAT(بفر, " ");
	STRCAT(بفر, res_cidfont->نسخہ_2);
	prt_dsc_resources(تہی, "procset", بفر);

	اگر (prt_custom_cmap)
	{
	    STRCPY(بفر, res_cmap->title);
	    STRCAT(بفر, " ");
	    STRCAT(بفر, res_cmap->نسخہ_2);
	    prt_dsc_resources(تہی, "cmap", بفر);
	}
    }
    اگر (!prt_برآمد_mbyte || prt_use_courier)
    {
	STRCPY(بفر, res_encoding->title);
	STRCAT(بفر, " ");
	STRCAT(بفر, res_encoding->نسخہ_2);
	prt_dsc_resources(تہی, "encoding", بفر);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ورنہ
					0
# endif
					, prt_عدد_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    اگر (!prt_برآمد_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    اگر (prt_برآمد_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? تہی : "PageResources"),
							    &prt_ps_mb_font);
	اگر (!prt_custom_cmap)
	    prt_dsc_resources(تہی, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    اگر (!prt_جوڑو_resource(res_prolog))
	جاؤ theend;
    اگر (prt_برآمد_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	اگر (!prt_جوڑو_resource(res_cidfont))
	    جاؤ theend;
	اگر (prt_custom_cmap && !prt_جوڑو_resource(res_cmap))
	    جاؤ theend;
    }

    اگر (!prt_برآمد_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	اگر (!prt_جوڑو_resource(res_encoding))
	    جاؤ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((صحیح)prt_mediasize[prt_media].چوڑائی);
    prt_write_int((صحیح)prt_mediasize[prt_media].اونچائی);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_عدد_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    اگر (!prt_برآمد_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	اگر (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (حرف *)p_encoding, (صحیح)prt_سطر_اونچائی,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (حرف *)p_encoding, (صحیح)prt_سطر_اونچائی,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (حرف *)p_encoding, (صحیح)prt_سطر_اونچائی,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (حرف *)p_encoding, (صحیح)prt_سطر_اونچائی,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    اگر (prt_برآمد_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	اگر (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (صحیح)prt_سطر_اونچائی,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != تہی)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (صحیح)prt_سطر_اونچائی,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ورنہ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != تہی)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (صحیح)prt_سطر_اونچائی,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ورنہ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != تہی)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (صحیح)prt_سطر_اونچائی,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ورنہ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_مقصد_USER(prt_سطر_اونچائی,
						prt_ps_font->uline_انحراف), 2);
    prt_def_var("UW", PRT_PS_FONT_مقصد_USER(prt_سطر_اونچائی,
						 prt_ps_font->uline_چوڑائی), 2);
    prt_def_var("BO", prt_bgcol_انحراف, 2);

    prt_dsc_noarg("EndSetup");

    // Fail if any problems writing out to the PS file
    retval = !prt_file_error;

theend:
    vim_free(res_prolog);
    vim_free(res_encoding);
    vim_free(res_cidfont);
    vim_free(res_cmap);

    واپس retval;
}

    خالی
mch_print_انتہا(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_صفحہ_عدد);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    اگر (!prt_file_error && psettings->outfile == تہی
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	اگر (prt_ps_fd != تہی)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = تہی;
	}
	prt_پیغام((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	اگر (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_مقصد_print_postscript_file));
	ورنہ
	    prt_پیغام((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحیح
mch_print_انتہا_صفحہ(خالی)
{
    prt_flush_بفر();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    واپس !prt_file_error;
}

    صحیح
mch_print_begin_صفحہ(char_u *str غیر_مستعمل)
{
    صحیح		صفحہ_عدد[2];

    prt_صفحہ_عدد++;

    صفحہ_عدد[0] = صفحہ_عدد[1] = prt_صفحہ_عدد;
    prt_dsc_ints("Page", 2, صفحہ_عدد);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_اندر_ascii = !prt_برآمد_mbyte;
    اگر (prt_برآمد_mbyte)
	prt_write_string("CF0 sf\n");
    ورنہ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    اگر (!prt_portrait)
    {
	prt_write_int(-((صحیح)prt_mediasize[prt_media].چوڑائی));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    واپس !prt_file_error;
}

    صحیح
mch_print_blank_صفحہ(خالی)
{
    واپس (mch_print_begin_صفحہ(تہی) ? (mch_print_انتہا_صفحہ()) : FALSE);
}

ساکن اعشاری prt_pos_x = 0;
ساکن اعشاری prt_pos_y = 0;

    خالی
mch_print_آغاز_سطر(صحیح margin, صحیح صفحہ_سطر)
{
    prt_pos_x = prt_بایاں_margin;
    اگر (margin)
	prt_pos_x -= prt_نمبر_چوڑائی;

    prt_pos_y = prt_چوٹی_margin - prt_پہلا_سطر_اونچائی -
					صفحہ_سطر * prt_سطر_اونچائی;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_چوڑائی = FALSE;
}

    صحیح
mch_print_متن_برآمد(char_u *textp, صحیح len غیر_مستعمل)
{
    char_u	*p = textp;
    صحیح		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    اعشاری       char_چوڑائی;
    اعشاری       اگلا_pos;
    صحیح		اندر_ascii;
    صحیح		half_چوڑائی;
    char_u	*tofree = تہی;

    char_چوڑائی = prt_char_چوڑائی;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    اگر (prt_use_courier)
    {
	اندر_ascii = (len == 1 && *p < 0x80);
	اگر (prt_اندر_ascii)
	{
	    اگر (!اندر_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_اندر_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ورنہ اگر (اندر_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_اندر_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    اگر (prt_برآمد_mbyte)
    {
	half_چوڑائی = ((*mb_ptr2cells)(p) == 1);
	اگر (half_چوڑائی)
	    char_چوڑائی /= 2;
	اگر (prt_half_چوڑائی)
	{
	    اگر (!half_چوڑائی)
	    {
		prt_half_چوڑائی = FALSE;
		prt_pos_x += prt_char_چوڑائی/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ورنہ اگر (half_چوڑائی)
	{
	    prt_half_چوڑائی = TRUE;
	    prt_pos_x += prt_char_چوڑائی/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    اگر (prt_attribute_change)
    {
	prt_flush_بفر();
	// Reset count of number of chars that will be printed
	prt_متن_تسلسل = 0;

	اگر (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	اگر (prt_need_font)
	{
	    اگر (!prt_اندر_ascii)
		prt_write_string("CF");
	    ورنہ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	اگر (prt_need_fgcol)
	{
	    صحیح     r, g, b;
	    r = ((غیرعلامتی)prt_fgcol & 0xff0000) >> 16;
	    g = ((غیرعلامتی)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    اگر (r == g && g == b)
		prt_write_string("g\n");
	    ورنہ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	اگر (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    اگر (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ورنہ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	اگر (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    اگر (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	اگر (p == تہی)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    اگر (prt_برآمد_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	جب (len-- > 0)
	{
	    ch = prt_hexchar[(غیرعلامتی)(*p) >> 4];
	    ga_append(&prt_ps_بفر, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_بفر, ch);
	    p++;
	}
    }
    ورنہ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	اگر (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_بفر, '\\');
	    انتخاب (ch)
	    {
		حالت BS:   ga_append(&prt_ps_بفر, 'b'); توڑو;
		حالت TAB:  ga_append(&prt_ps_بفر, 't'); توڑو;
		حالت NL:   ga_append(&prt_ps_بفر, 'n'); توڑو;
		حالت FF:   ga_append(&prt_ps_بفر, 'f'); توڑو;
		حالت CAR:  ga_append(&prt_ps_بفر, 'r'); توڑو;
		حالت '(':  ga_append(&prt_ps_بفر, '('); توڑو;
		حالت ')':  ga_append(&prt_ps_بفر, ')'); توڑو;
		حالت '\\': ga_append(&prt_ps_بفر, '\\'); توڑو;

		طےشدہ:
			   sprintf((حرف *)ch_buff, "%03o", (غیرعلامتی صحیح)ch);
			   ga_append(&prt_ps_بفر, ch_buff[0]);
			   ga_append(&prt_ps_بفر, ch_buff[1]);
			   ga_append(&prt_ps_بفر, ch_buff[2]);
			   توڑو;
	    }
	}
	ورنہ
	    ga_append(&prt_ps_بفر, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_متن_تسلسل += char_چوڑائی;
    prt_pos_x += char_چوڑائی;

    // The downside of fp - use relative error on right margin check
    اگلا_pos = prt_pos_x + prt_char_چوڑائی;
    need_break = (اگلا_pos > prt_دایاں_margin) &&
		    ((اگلا_pos - prt_دایاں_margin) > (prt_دایاں_margin*1e-5));

    اگر (need_break)
	prt_flush_بفر();

    واپس need_break;
}

    خالی
mch_print_سیٹ_font(صحیح iBold, صحیح iItalic, صحیح iUnderline)
{
    صحیح		font = 0;

    اگر (iBold)
	font |= 0x01;
    اگر (iItalic)
	font |= 0x02;

    اگر (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    اگر (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    خالی
mch_print_سیٹ_bg(long_u bgcol)
{
    prt_bgcol = (صحیح)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    خالی
mch_print_سیٹ_fg(long_u fgcol)
{
    اگر (fgcol == (long_u)prt_fgcol)
	واپس;

    prt_fgcol = (صحیح)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#اگر_ختم // FEAT_POSTSCRIPT
