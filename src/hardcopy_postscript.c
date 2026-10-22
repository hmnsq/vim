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

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_رقم_OPTIONS] =
{
    {"c",	FALSE, 0, پوچ, 0, FALSE},
    {"a",	FALSE, 0, پوچ, 0, FALSE},
    {"r",	FALSE, 0, پوچ, 0, FALSE},
    {"b",	FALSE, 0, پوچ, 0, FALSE},
    {"i",	FALSE, 0, پوچ, 0, FALSE},
    {"o",	FALSE, 0, پوچ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ساختار prt_ps_font_S
{
    صحیح		wx;
    صحیح		uline_offset;
    صحیح		uline_width;
    صحیح		bbox_min_y;
    صحیح		bbox_max_y;
    نویسه	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ایستا ساختار prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ایستا ساختار prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {پوچ, پوچ, پوچ, پوچ}
};

// Pointer to current font set being used
ایستا ساختار prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ساختار prt_ps_encoding_S
{
    نویسه	*encoding;
    نویسه	*cmap_encoding;
    صحیح		needs_charset;
};

ساختار prt_ps_charset_S
{
    نویسه	*charset;
    نویسه	*cmap_charset;
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
ایستا ساختار prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", پوچ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ایستا ساختار prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  پوچ,       CS_JIS_X_1983},
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
ایستا ساختار prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    پوچ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     پوچ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ایستا ساختار prt_ps_charset_S sc_charsets[] =
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
ایستا ساختار prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    پوچ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
ایستا ساختار prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    پوچ,	CS_B5},
    {"CP950",	    پوچ,	CS_B5},
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
ایستا ساختار prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", پوچ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ایستا ساختار prt_ps_charset_S k_charsets[] =
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
ساختار prt_ps_mbfont_S
{
    صحیح				رقم_encodings;
    ساختار prt_ps_encoding_S	*encodings;
    صحیح				رقم_charsets;
    ساختار prt_ps_charset_S	*charsets;
    نویسه			*ascii_enc;
    نویسه			*defcs;
};

ایستا ساختار prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_LENGTH(j_encodings),
	j_encodings,
	ARRAY_LENGTH(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_LENGTH(sc_encodings),
	sc_encodings,
	ARRAY_LENGTH(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_LENGTH(tc_encodings),
	tc_encodings,
	ARRAY_LENGTH(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_LENGTH(k_encodings),
	k_encodings,
	ARRAY_LENGTH(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

ساختار prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    صحیح     type;
    char_u  title[256];
    char_u  version[256];
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
# define PRT_PROLOG_VERSION  ((char_u *)"1.4")
# define PRT_CID_PROLOG_VERSION  ((char_u *)"1.0")

// String versions of PS resource types - indexed by constants above so don't
// re-order!
ایستا نویسه *prt_resource_types[] =
{
    "procset",
    "encoding",
    "cmap"
};

// Strings to look for in a PS resource file
# define PRT_RESOURCE_HEADER	    "%!PS-Adobe-"
# define PRT_RESOURCE_RESOURCE	    "Resource-"
# define PRT_RESOURCE_PROCSET	    "ProcSet"
# define PRT_RESOURCE_ENCODING	    "Encoding"
# define PRT_RESOURCE_CMAP	    "CMap"


// Data for table based DSC comment recognition, easy to extend if VIM needs to
// read more comments.
# define PRT_DSC_MISC_TYPE	    (-1)
# define PRT_DSC_TITLE_TYPE	    (1)
# define PRT_DSC_VERSION_TYPE	    (2)
# define PRT_DSC_ENDCOMMENTS_TYPE    (3)

# define PRT_DSC_TITLE		    "%%Title:"
# define PRT_DSC_VERSION		    "%%Version:"
# define PRT_DSC_ENDCOMMENTS	    "%%EndComments:"

ساختار prt_dsc_comment_S
{
    نویسه	*string;
    صحیح		len;
    صحیح		type;
};

ساختار prt_dsc_سطر_S
{
    صحیح		type;
    char_u	*string;
    صحیح		len;
};


# define SIZEOF_CSTR(s)      (اندازه(s) - 1)
ایستا ساختار prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ایستا تهی prt_write_file_len(char_u *buffer, صحیح bytes);
ایستا صحیح prt_next_dsc(ساختار prt_dsc_سطر_S *p_dsc_سطر);

/*
 * Variables for the output PostScript file.
 */
ایستا FILE *prt_ps_fd;
ایستا صحیح prt_file_error;
ایستا char_u *prt_ps_file_name = پوچ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ایستا اعشاری prt_page_width;
ایستا اعشاری prt_page_height;
ایستا اعشاری prt_چپ_margin;
ایستا اعشاری prt_راست_margin;
ایستا اعشاری prt_بالاترین_margin;
ایستا اعشاری prt_bottom_margin;
ایستا اعشاری prt_سطر_height;
ایستا اعشاری prt_first_سطر_height;
ایستا اعشاری prt_char_width;
ایستا اعشاری prt_عدد_width;
ایستا اعشاری prt_bgcol_offset;
ایستا اعشاری prt_pos_x_moveto = 0.0;
ایستا اعشاری prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ایستا صحیح prt_need_moveto;
ایستا صحیح prt_do_moveto;
ایستا صحیح prt_need_font;
ایستا صحیح prt_font;
ایستا صحیح prt_need_underline;
ایستا صحیح prt_underline;
ایستا صحیح prt_do_underline;
ایستا صحیح prt_need_fgcol;
ایستا صحیح prt_fgcol;
ایستا صحیح prt_need_bgcol;
ایستا صحیح prt_do_bgcol;
ایستا صحیح prt_bgcol;
ایستا صحیح prt_new_bgcol;
ایستا صحیح prt_attribute_change;
ایستا اعشاری prt_text_run;
ایستا صحیح prt_page_رقم;
ایستا صحیح prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ایستا صحیح prt_media;
ایستا صحیح prt_portrait;
ایستا صحیح prt_رقم_copies;
ایستا صحیح prt_duplex;
ایستا صحیح prt_tumble;
ایستا صحیح prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ایستا char_u prt_سطر_buffer[257];
ایستا garray_T prt_ps_buffer;

ایستا صحیح prt_do_conv;
ایستا vimconv_T prt_conv;

ایستا صحیح prt_out_mbyte;
ایستا صحیح prt_custom_cmap;
ایستا نویسه prt_cmap[80];
ایستا صحیح prt_use_courier;
ایستا صحیح prt_in_ascii;
ایستا صحیح prt_half_width;
ایستا نویسه *prt_ascii_encoding;
ایستا ثابت char_u prt_hexchar[] = "0123456789abcdef";

    ایستا تهی
prt_write_file_raw_len(char_u *buffer, صحیح bytes)
{
    اگر (!prt_file_error
	    && fwrite(buffer, اندازه(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ایستا تهی
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (صحیح)STRLEN(buffer));
}

    ایستا تهی
prt_write_file_len(char_u *buffer, صحیح bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    ایستا تهی
prt_write_string(نویسه *s)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer), "%s", s);
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write an int and a space.
 */
    ایستا تهی
prt_write_int(صحیح i)
{
    sprintf((نویسه *)prt_سطر_buffer, "%d ", i);
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write a boolean and a space.
 */
    ایستا تهی
prt_write_boolean(صحیح b)
{
    sprintf((نویسه *)prt_سطر_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ایستا تهی
prt_def_font(
    نویسه	*new_name,
    نویسه	*encoding,
    صحیح		height,
    نویسه	*font)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_سطر_buffer);
    اگر (prt_out_mbyte)
	sprintf((نویسه *)prt_سطر_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    وگرنه
	vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write a line to define the CID font.
 */
    ایستا تهی
prt_def_cidfont(نویسه *new_name, صحیح height, نویسه *cidfont)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_سطر_buffer);
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ایستا تهی
prt_dup_cidfont(نویسه *original_name, نویسه *new_name)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_سطر_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ایستا تهی
prt_real_bits(
    دوگانه      real,
    صحیح		precision,
    صحیح		*pinteger,
    صحیح		*pfraction)
{
    صحیح     i;
    صحیح     integer;
    اعشاری   fraction;

    integer = (صحیح)real;
    fraction = (اعشاری)(real - integer);
    اگر (real < (دوگانه)integer)
	fraction = -fraction;
    برای (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (صحیح)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ایستا تهی
prt_write_real(دوگانه val, صحیح prec)
{
    صحیح     integer;
    صحیح     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((نویسه *)prt_سطر_buffer, "%d", integer);
    prt_write_file(prt_سطر_buffer);
    // Only emit fraction if necessary
    اگر (fraction != 0)
    {
	// Remove any trailing zeros
	وقتی ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((نویسه *)prt_سطر_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_سطر_buffer);
    }
    sprintf((نویسه *)prt_سطر_buffer, " ");
    prt_write_file(prt_سطر_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    ایستا تهی
prt_def_var(نویسه *name, دوگانه مقدار, صحیح prec)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
								"/%s ", name);
    prt_write_file(prt_سطر_buffer);
    prt_write_real(مقدار, prec);
    sprintf((نویسه *)prt_سطر_buffer, "d\n");
    prt_write_file(prt_سطر_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    ایستا تهی
prt_flush_buffer(تهی)
{
    اگر (prt_ps_buffer.ga_len <= 0)
	بازگشت;

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
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_سطر_height, 2);

	// Lastly add the color of the background
	r = ((unsigned)prt_new_bgcol & 0xff0000) >> 16;
	g = ((unsigned)prt_new_bgcol & 0xff00) >> 8;
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
	prt_write_real(prt_text_run, 2);
	prt_write_string("ul\n");
    }
    // Draw the text
    اگر (prt_out_mbyte)
	prt_write_string("<");
    وگرنه
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    اگر (prt_out_mbyte)
	prt_write_string(">");
    وگرنه
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
    وگرنه // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, اندازه(نویسه), prt_bufsiz);
}


    ایستا تهی
prt_resource_name(char_u *filename, تهی *cookie)
{
    char_u *resource_filename = cookie;

    اگر (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    وگرنه
	STRCPY(resource_filename, filename);
}

    ایستا صحیح
prt_find_resource(نویسه *name, ساختار prt_ps_resource_S *resource)
{
    char_u	*buffer;
    صحیح		retval;

    buffer = alloc(MAXPATHL + 1);
    اگر (buffer == پوچ)
	بازگشت FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(buffer, "print");
    add_pathsep(buffer);
    vim_strcat(buffer, (char_u *)name, MAXPATHL);
    vim_strcat(buffer, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_in_runtimepath(buffer, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(buffer);
    بازگشت retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
ساختار prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    صحیح     len;
    صحیح     سطر_start;
    صحیح     سطر_end;
};

ایستا ساختار prt_resfile_buffer_S prt_resfile;

    ایستا صحیح
prt_resfile_next_سطر(تهی)
{
    صحیح     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.سطر_end + 1;
    وقتی (idx < prt_resfile.len)
    {
	اگر (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    بشکن;
	idx++;
    }
    prt_resfile.سطر_start = idx;

    وقتی (idx < prt_resfile.len)
    {
	اگر (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    بشکن;
	idx++;
    }
    prt_resfile.سطر_end = idx;

    بازگشت (idx < prt_resfile.len);
}

    ایستا صحیح
prt_resfile_strncmp(صحیح offset, نویسه *string, صحیح len)
{
    // Force not equal if string is longer than remainder of line
    اگر (len > (prt_resfile.سطر_end - (prt_resfile.سطر_start + offset)))
	بازگشت 1;

    بازگشت STRNCMP(&prt_resfile.buffer[prt_resfile.سطر_start + offset],
								string, len);
}

    ایستا صحیح
prt_resfile_skip_nonws(صحیح offset)
{
    صحیح     idx;

    idx = prt_resfile.سطر_start + offset;
    وقتی (idx < prt_resfile.سطر_end)
    {
	اگر (SAFE_isspace(prt_resfile.buffer[idx]))
	    بازگشت idx - prt_resfile.سطر_start;
	idx++;
    }
    بازگشت -1;
}

    ایستا صحیح
prt_resfile_skip_ws(صحیح offset)
{
    صحیح     idx;

    idx = prt_resfile.سطر_start + offset;
    وقتی (idx < prt_resfile.سطر_end)
    {
	اگر (!SAFE_isspace(prt_resfile.buffer[idx]))
	    بازگشت idx - prt_resfile.سطر_start;
	idx++;
    }
    بازگشت -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ایستا صحیح
prt_next_dsc(ساختار prt_dsc_سطر_S *p_dsc_سطر)
{
    صحیح     comment;
    صحیح     offset;

    // Move to start of next line
    اگر (!prt_resfile_next_سطر())
	بازگشت FALSE;

    // DSC comments always start %%
    اگر (prt_resfile_strncmp(0, "%%", 2) != 0)
	بازگشت FALSE;

    // Find type of DSC comment
    برای (comment = 0; comment < (صحیح)ARRAY_LENGTH(prt_dsc_table); comment++)
	اگر (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    بشکن;

    اگر (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_سطر->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    وگرنه
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_سطر->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	اگر (offset == -1)
	    بازگشت FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    اگر (offset == -1)
	بازگشت FALSE;

    p_dsc_سطر->string = &prt_resfile.buffer[prt_resfile.سطر_start + offset];
    p_dsc_سطر->len = prt_resfile.سطر_end - (prt_resfile.سطر_start + offset);

    بازگشت TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ایستا صحیح
prt_open_resource(ساختار prt_ps_resource_S *resource)
{
    صحیح		offset;
    صحیح		seen_all;
    صحیح		seen_title;
    صحیح		seen_version;
    FILE	*fd_resource;
    ساختار prt_dsc_سطر_S dsc_سطر;

    fd_resource = mch_fopen((نویسه *)resource->filename, READBIN);
    اگر (fd_resource == پوچ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	بازگشت FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (صحیح)fread((نویسه *)prt_resfile.buffer, اندازه(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    اگر (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	بازگشت FALSE;
    }
    fclose(fd_resource);

    prt_resfile.سطر_end = -1;
    prt_resfile.سطر_start = 0;
    اگر (!prt_resfile_next_سطر())
	بازگشت FALSE;

    offset = 0;

    اگر (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (صحیح)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	بازگشت FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (صحیح)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    اگر (offset == -1)
	بازگشت FALSE;
    offset = prt_resfile_skip_ws(offset);
    اگر (offset == -1)
	بازگشت FALSE;

    اگر (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (صحیح)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	بازگشت FALSE;
    }
    offset += (صحیح)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    اگر (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (صحیح)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    وگرنه اگر (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (صحیح)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    وگرنه اگر (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (صحیح)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    وگرنه
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	بازگشت FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    وقتی (!seen_all && prt_next_dsc(&dsc_سطر))
    {
	انتخاب (dsc_سطر.type)
	{
	حالت PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_سطر.string, dsc_سطر.len);
	    seen_title = TRUE;
	    اگر (seen_version)
		seen_all = TRUE;
	    بشکن;

	حالت PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_سطر.string, dsc_سطر.len);
	    seen_version = TRUE;
	    اگر (seen_title)
		seen_all = TRUE;
	    بشکن;

	حالت PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    بشکن;

	حالت PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    بشکن;
	}
    }

    اگر (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	بازگشت FALSE;
    }

    بازگشت TRUE;
}

    ایستا صحیح
prt_check_resource(ساختار prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    اگر (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	بازگشت FALSE;
    }

    // Other checks to be added as needed
    بازگشت TRUE;
}

    ایستا تهی
prt_dsc_start(تهی)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ایستا تهی
prt_dsc_noarg(نویسه *comment)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_سطر_buffer);
}

    ایستا تهی
prt_dsc_textline(نویسه *comment, نویسه *text)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_سطر_buffer);
}

    ایستا تهی
prt_dsc_text(نویسه *comment, نویسه *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_سطر_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    ایستا تهی
prt_dsc_ints(نویسه *comment, صحیح شمار, صحیح *ints)
{
    صحیح		i;

    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_سطر_buffer);

    برای (i = 0; i < شمار; i++)
    {
	sprintf((نویسه *)prt_سطر_buffer, " %d", ints[i]);
	prt_write_file(prt_سطر_buffer);
    }

    prt_write_string("\n");
}

    ایستا تهی
prt_dsc_resources(
    نویسه	*comment,	// if NULL add to previous
    نویسه	*type,
    نویسه	*string)
{
    اگر (comment != پوچ)
	vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
						 "%%%%%s: %s", comment, type);
    وگرنه
	vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_سطر_buffer);

    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
							     " %s\n", string);
    prt_write_file(prt_سطر_buffer);
}

    ایستا تهی
prt_dsc_font_resource(نویسه *resource, ساختار prt_ps_font_S *ps_font)
{
    صحیح     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    برای (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	اگر (ps_font->ps_fontname[i] != پوچ)
	    prt_dsc_resources(پوچ, "font", ps_font->ps_fontname[i]);
}

    ایستا تهی
prt_dsc_requirements(
    صحیح		duplex,
    صحیح		tumble,
    صحیح		collate,
    صحیح		color,
    صحیح		رقم_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    اگر (!(duplex || collate || color || (رقم_copies > 1)))
	بازگشت;

    sprintf((نویسه *)prt_سطر_buffer, "%%%%Requirements:");
    prt_write_file(prt_سطر_buffer);

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
    اگر (رقم_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((نویسه *)prt_سطر_buffer, "%d", رقم_copies);
	prt_write_file(prt_سطر_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ایستا تهی
prt_dsc_docmedia(
    نویسه	*paper_name,
    دوگانه	width,
    دوگانه	height,
    دوگانه	weight,
    نویسه	*colour,
    نویسه	*type)
{
    vim_snprintf((نویسه *)prt_سطر_buffer, اندازه(prt_سطر_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_سطر_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    اگر (colour == پوچ)
	prt_write_string("()");
    وگرنه
	prt_write_string(colour);
    prt_write_string(" ");
    اگر (type == پوچ)
	prt_write_string("()");
    وگرنه
	prt_write_string(type);
    prt_write_string("\n");
}

    تهی
mch_print_cleanup(تهی)
{
    اگر (prt_out_mbyte)
    {
	صحیح     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	برای (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    اگر (prt_ps_mb_font.ps_fontname[i] != پوچ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    اگر (prt_do_conv)
    {
	convert_setup(&prt_conv, پوچ, پوچ);
	prt_do_conv = FALSE;
    }
    اگر (prt_ps_fd != پوچ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = پوچ;
	prt_file_error = FALSE;
    }
    اگر (prt_ps_file_name != پوچ)
	VIM_CLEAR(prt_ps_file_name);
}

    ایستا اعشاری
to_device_units(صحیح idx, دوگانه physsize, صحیح def_عدد)
{
    اعشاری	ret;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_عدد;
    }
    وگرنه
	nr = printer_opts[idx].عدد;

    انتخاب (u)
    {
	حالت PRT_UNIT_INCH:
	    ret = (اعشاری)(nr * PRT_PS_DEFAULT_DPI);
	    بشکن;
	حالت PRT_UNIT_MM:
	    ret = (اعشاری)(nr * PRT_PS_DEFAULT_DPI) / (اعشاری)25.4;
	    بشکن;
	حالت PRT_UNIT_POINT:
	    ret = (اعشاری)nr;
	    بشکن;
	حالت PRT_UNIT_PERC:
	پیشفرض:
	    ret = (اعشاری)(physsize * nr) / 100;
	    بشکن;
    }

    بازگشت ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ایستا تهی
prt_page_margins(
    دوگانه	width,
    دوگانه	height,
    دوگانه	*چپ,
    دوگانه	*راست,
    دوگانه	*بالاترین,
    دوگانه	*bottom)
{
    *چپ   = to_device_units(OPT_PRINT_چپ, width, 10);
    *راست  = width - to_device_units(OPT_PRINT_راست, width, 5);
    *بالاترین    = height - to_device_units(OPT_PRINT_بالاترین, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    ایستا تهی
prt_font_metrics(صحیح font_scale)
{
    prt_سطر_height = (اعشاری)font_scale;
    prt_char_width = (اعشاری)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    ایستا صحیح
prt_get_cpl(تهی)
{
    اگر (prt_use_عدد())
    {
	prt_عدد_width = PRINT_عدد_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	اگر (prt_out_mbyte)
	    prt_عدد_width /= 2;
	prt_چپ_margin += prt_عدد_width;
    }
    وگرنه
	prt_عدد_width = 0.0;

    بازگشت (صحیح)((prt_راست_margin - prt_چپ_margin) / prt_char_width);
}

    ایستا صحیح
prt_build_cid_fontname(صحیح font, char_u *name, صحیح name_len)
{
    نویسه    *fontname;

    fontname = alloc(name_len + 1);
    اگر (fontname == پوچ)
	بازگشت FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    بازگشت TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ایستا صحیح
prt_get_lpp(تهی)
{
    صحیح lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (اعشاری)PRT_PS_FONT_TO_USER(prt_سطر_height,
					   prt_ps_font->bbox_min_y);
    اگر ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (اعشاری)PRT_PS_FONT_TO_USER(prt_سطر_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_سطر_height = prt_سطر_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (صحیح)((prt_بالاترین_margin - prt_bottom_margin) / prt_سطر_height);

    // Adjust top margin if there is a header
    prt_بالاترین_margin -= prt_سطر_height * prt_header_height();

    بازگشت lpp - prt_header_height();
}

    ایستا صحیح
prt_match_encoding(
    نویسه			*p_encoding,
    ساختار prt_ps_mbfont_S	*p_cmap,
    ساختار prt_ps_encoding_S	**pp_mbenc)
{
    صحیح				mbenc;
    صحیح				enc_len;
    ساختار prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = پوچ;
    // Look for recognised encoding
    enc_len = (صحیح)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    برای (mbenc = 0; mbenc < p_cmap->رقم_encodings; mbenc++)
    {
	اگر (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    بازگشت TRUE;
	}
	p_mbenc++;
    }
    بازگشت FALSE;
}

    ایستا صحیح
prt_match_charset(
    نویسه		    *p_charset,
    ساختار prt_ps_mbfont_S  *p_cmap,
    ساختار prt_ps_charset_S **pp_mbchar)
{
    صحیح			    mbchar;
    صحیح			    char_len;
    ساختار prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    اگر (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (صحیح)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    برای (mbchar = 0; mbchar < p_cmap->رقم_charsets; mbchar++)
    {
	اگر (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    بازگشت TRUE;
	}
	p_mbchar++;
    }
    بازگشت FALSE;
}

    صحیح
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    صحیح		forceit UNUSED)
{
    صحیح		i;
    نویسه	*paper_name;
    صحیح		paper_strlen;
    صحیح		fontsize;
    char_u	*p;
    دوگانه      چپ;
    دوگانه      راست;
    دوگانه      بالاترین;
    دوگانه      bottom;
    صحیح		props;
    صحیح		cmap = 0;
    char_u	*p_encoding;
    ساختار prt_ps_encoding_S *p_mbenc;
    ساختار prt_ps_encoding_S *p_mbenc_first;
    ساختار prt_ps_charset_S  *p_mbchar = پوچ;

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
    p_mbenc = پوچ;
    props = enc_canon_props(p_encoding);
    اگر (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	صحیح cmap_first = 0;

	p_mbenc_first = پوچ;
	برای (cmap = 0; cmap < (صحیح)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    اگر (prt_match_encoding((نویسه *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		اگر (p_mbenc_first == پوچ)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		اگر (prt_match_charset((نویسه *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    بشکن;
	    }

	// Use first encoding matched if no charset matched
	اگر (p_mbchar == پوچ && p_mbenc_first != پوچ)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != پوچ);
    اگر (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == پوچ);
	اگر (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    اگر ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		بازگشت FALSE;
	    }

	    // Add charset name if not empty
	    اگر (p_mbchar->cmap_charset != پوچ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, اندازه(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	وگرنه
	{
	    // Add custom CMap character set name
	    اگر (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		بازگشت FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, اندازه(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	اگر (p_mbenc->cmap_encoding != پوچ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < اندازه(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	اگر (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    بازگشت FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	اگر (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    بازگشت FALSE;
	اگر (mbfont_opts[OPT_MBFONT_BOLD].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		بازگشت FALSE;
	اگر (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		بازگشت FALSE;
	اگر (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    اگر (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		بازگشت FALSE;

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
	    وگرنه
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    وگرنه
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
	paper_name = (نویسه *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    وگرنه
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    برای (i = 0; i < (صحیح)PRT_MEDIASIZE_LEN; ++i)
	اگر (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    بشکن;
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
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    وگرنه
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &چپ, &راست, &بالاترین,
								    &bottom);
    prt_چپ_margin = (اعشاری)چپ;
    prt_راست_margin = (اعشاری)راست;
    prt_بالاترین_margin = (اعشاری)بالاترین;
    prt_bottom_margin = (اعشاری)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    برای (p = p_pfn; (p = vim_strchr(p, ':')) != پوچ; ++p)
	اگر (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((نویسه *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_سطر = prt_get_cpl();
    psettings->سطرها_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    اگر (psettings->chars_per_سطر <= 0 || psettings->سطرها_per_page <= 0)
	بازگشت FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_رقم_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    اگر (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    وگرنه
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_رقم_copies = 1;
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
	وگرنه اگر (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    اگر (psettings->outfile == پوچ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	اگر (prt_ps_file_name == پوچ)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    بازگشت FAIL;
	}
	prt_ps_fd = mch_fopen((نویسه *)prt_ps_file_name, WRITEBIN);
    }
    وگرنه
    {
	p = expand_env_save(psettings->outfile);
	اگر (p != پوچ)
	{
	    prt_ps_fd = mch_fopen((نویسه *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    اگر (prt_ps_fd == پوچ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	بازگشت FAIL;
    }

    prt_bufsiz = psettings->chars_per_سطر;
    اگر (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, اندازه(نویسه), prt_bufsiz);

    prt_page_رقم = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    بازگشت OK;
}

    ایستا صحیح
prt_add_resource(ساختار prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((نویسه *)resource->filename, READBIN);
    اگر (fd_resource == پوچ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	بازگشت FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (نویسه *)resource->title);

    prt_dsc_textline("BeginDocument", (نویسه *)resource->filename);

    برای (;;)
    {
	bytes_read = fread((نویسه *)resource_buffer, اندازه(char_u),
			   اندازه(resource_buffer), fd_resource);
	اگر (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    بازگشت FALSE;
	}
	اگر (bytes_read == 0)
	    بشکن;
	prt_write_file_raw_len(resource_buffer, (صحیح)bytes_read);
	اگر (prt_file_error)
	{
	    fclose(fd_resource);
	    بازگشت FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    بازگشت TRUE;
}

    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    صحیح		bbox[4];
    دوگانه      چپ;
    دوگانه      راست;
    دوگانه      بالاترین;
    دوگانه      bottom;
    ساختار prt_ps_resource_S *res_prolog;
    ساختار prt_ps_resource_S *res_encoding;
    نویسه	buffer[256];
    char_u      *p_encoding;
    ساختار prt_ps_resource_S *res_cidfont;
    ساختار prt_ps_resource_S *res_cmap;
    صحیح		retval = FALSE;

    res_prolog = ALLOC_ONE(ساختار prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ساختار prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ساختار prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ساختار prt_ps_resource_S);
    اگر (res_prolog == پوچ || res_encoding == پوچ
	    || res_cidfont == پوچ || res_cmap == پوچ)
	برو theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (نویسه *)psettings->jobname);
    اگر (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(پوچ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &چپ, &راست, &بالاترین, &bottom);
    bbox[0] = (صحیح)چپ;
    اگر (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (صحیح)(بالاترین - (psettings->سطرها_per_page + prt_header_height())
						    * (دوگانه)prt_سطر_height);
	bbox[2] = (صحیح)(چپ + psettings->chars_per_سطر
					       * (دوگانه)prt_char_width + 0.5);
	bbox[3] = (صحیح)(بالاترین + 0.5);
    }
    وگرنه
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (صحیح)bottom;
	bbox[2] = (صحیح)(چپ + ((psettings->سطرها_per_page
			      + prt_header_height()) * prt_سطر_height) + 0.5);
	bbox[3] = (صحیح)(bottom + psettings->chars_per_سطر
					       * (دوگانه)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(دوگانه)0, پوچ, پوچ);
    // Define fonts needed
    اگر (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    اگر (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? پوچ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	اگر (!prt_custom_cmap)
	    prt_dsc_resources(پوچ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    اگر (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	برو theend;
    }
    اگر (!prt_open_resource(res_prolog))
	برو theend;
    اگر (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	برو theend;
    اگر (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	اگر (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    برو theend;
	}
	اگر (!prt_open_resource(res_cidfont))
	    برو theend;
	اگر (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    برو theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    اگر (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	اگر (*p_encoding == NUL
		|| !prt_find_resource((نویسه *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    صحیح		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    اگر (!(props & ENC_8BIT)
		    || !prt_find_resource((نویسه *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		اگر (!prt_find_resource((نویسه *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    برو theend;
		}
	    }
	}
	اگر (!prt_open_resource(res_encoding))
	    برو theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    وگرنه
    {
	p_encoding = enc_skip(p_penc);
	اگر (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	اگر (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    اگر (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		برو theend;
	    }
	    اگر (!prt_open_resource(res_encoding))
		برو theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    اگر (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	اگر (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    برو theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    اگر (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	اگر (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    برو theend;
	}
	اگر (!prt_open_resource(res_cmap))
	    برو theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    اگر (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(پوچ, "procset", buffer);

	اگر (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(پوچ, "cmap", buffer);
	}
    }
    اگر (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(پوچ, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# وگرنه
					0
# endif
					, prt_رقم_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    اگر (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    اگر (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? پوچ : "PageResources"),
							    &prt_ps_mb_font);
	اگر (!prt_custom_cmap)
	    prt_dsc_resources(پوچ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    اگر (!prt_add_resource(res_prolog))
	برو theend;
    اگر (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	اگر (!prt_add_resource(res_cidfont))
	    برو theend;
	اگر (prt_custom_cmap && !prt_add_resource(res_cmap))
	    برو theend;
    }

    اگر (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	اگر (!prt_add_resource(res_encoding))
	    برو theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((صحیح)prt_mediasize[prt_media].width);
    prt_write_int((صحیح)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_رقم_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    اگر (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	اگر (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (نویسه *)p_encoding, (صحیح)prt_سطر_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (نویسه *)p_encoding, (صحیح)prt_سطر_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (نویسه *)p_encoding, (صحیح)prt_سطر_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (نویسه *)p_encoding, (صحیح)prt_سطر_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    اگر (prt_out_mbyte)
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
	prt_def_cidfont("CF0", (صحیح)prt_سطر_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != پوچ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (صحیح)prt_سطر_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	وگرنه
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != پوچ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (صحیح)prt_سطر_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	وگرنه
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	اگر (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != پوچ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    اگر (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (صحیح)prt_سطر_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	وگرنه
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_سطر_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_سطر_height,
						 prt_ps_font->uline_width), 2);
    prt_def_var("BO", prt_bgcol_offset, 2);

    prt_dsc_noarg("EndSetup");

    // Fail if any problems writing out to the PS file
    retval = !prt_file_error;

theend:
    vim_free(res_prolog);
    vim_free(res_encoding);
    vim_free(res_cidfont);
    vim_free(res_cmap);

    بازگشت retval;
}

    تهی
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_رقم);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    اگر (!prt_file_error && psettings->outfile == پوچ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	اگر (prt_ps_fd != پوچ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = پوچ;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	اگر (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	وگرنه
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    صحیح
mch_print_end_page(تهی)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    بازگشت !prt_file_error;
}

    صحیح
mch_print_begin_page(char_u *str UNUSED)
{
    صحیح		page_رقم[2];

    prt_page_رقم++;

    page_رقم[0] = page_رقم[1] = prt_page_رقم;
    prt_dsc_ints("Page", 2, page_رقم);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    اگر (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    وگرنه
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    اگر (!prt_portrait)
    {
	prt_write_int(-((صحیح)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    بازگشت !prt_file_error;
}

    صحیح
mch_print_blank_page(تهی)
{
    بازگشت (mch_print_begin_page(پوچ) ? (mch_print_end_page()) : FALSE);
}

ایستا اعشاری prt_pos_x = 0;
ایستا اعشاری prt_pos_y = 0;

    تهی
mch_print_start_سطر(صحیح margin, صحیح page_سطر)
{
    prt_pos_x = prt_چپ_margin;
    اگر (margin)
	prt_pos_x -= prt_عدد_width;

    prt_pos_y = prt_بالاترین_margin - prt_first_سطر_height -
					page_سطر * prt_سطر_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    صحیح
mch_print_text_out(char_u *textp, صحیح len UNUSED)
{
    char_u	*p = textp;
    صحیح		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    اعشاری       char_width;
    اعشاری       next_pos;
    صحیح		in_ascii;
    صحیح		half_width;
    char_u	*tofree = پوچ;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    اگر (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	اگر (prt_in_ascii)
	{
	    اگر (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	وگرنه اگر (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    اگر (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	اگر (half_width)
	    char_width /= 2;
	اگر (prt_half_width)
	{
	    اگر (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	وگرنه اگر (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    اگر (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	اگر (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	اگر (prt_need_font)
	{
	    اگر (!prt_in_ascii)
		prt_write_string("CF");
	    وگرنه
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	اگر (prt_need_fgcol)
	{
	    صحیح     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    اگر (r == g && g == b)
		prt_write_string("g\n");
	    وگرنه
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
	وگرنه
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
	اگر (p == پوچ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    اگر (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	وقتی (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    وگرنه
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
	    ga_append(&prt_ps_buffer, '\\');
	    انتخاب (ch)
	    {
		حالت BS:   ga_append(&prt_ps_buffer, 'b'); بشکن;
		حالت TAB:  ga_append(&prt_ps_buffer, 't'); بشکن;
		حالت NL:   ga_append(&prt_ps_buffer, 'n'); بشکن;
		حالت FF:   ga_append(&prt_ps_buffer, 'f'); بشکن;
		حالت CAR:  ga_append(&prt_ps_buffer, 'r'); بشکن;
		حالت '(':  ga_append(&prt_ps_buffer, '('); بشکن;
		حالت ')':  ga_append(&prt_ps_buffer, ')'); بشکن;
		حالت '\\': ga_append(&prt_ps_buffer, '\\'); بشکن;

		پیشفرض:
			   sprintf((نویسه *)ch_buff, "%03o", (unsigned صحیح)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   بشکن;
	    }
	}
	وگرنه
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_راست_margin) &&
		    ((next_pos - prt_راست_margin) > (prt_راست_margin*1e-5));

    اگر (need_break)
	prt_flush_buffer();

    بازگشت need_break;
}

    تهی
mch_print_set_font(صحیح iBold, صحیح iItalic, صحیح iUnderline)
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

    تهی
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (صحیح)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    تهی
mch_print_set_fg(long_u fgcol)
{
    اگر (fgcol == (long_u)prt_fgcol)
	بازگشت;

    prt_fgcol = (صحیح)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
