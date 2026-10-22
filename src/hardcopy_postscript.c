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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_இலக்கம்_OPTIONS] =
{
    {"c",	FALSE, 0, வெற்று, 0, FALSE},
    {"a",	FALSE, 0, வெற்று, 0, FALSE},
    {"r",	FALSE, 0, வெற்று, 0, FALSE},
    {"b",	FALSE, 0, வெற்று, 0, FALSE},
    {"i",	FALSE, 0, வெற்று, 0, FALSE},
    {"o",	FALSE, 0, வெற்று, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
கட்டமைப்பு prt_ps_font_S
{
    முழு		wx;
    முழு		uline_offset;
    முழு		uline_width;
    முழு		bbox_min_y;
    முழு		bbox_max_y;
    எழுத்து	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
நிலையான கட்டமைப்பு prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
நிலையான கட்டமைப்பு prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {வெற்று, வெற்று, வெற்று, வெற்று}
};

// Pointer to current font set being used
நிலையான கட்டமைப்பு prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
கட்டமைப்பு prt_ps_encoding_S
{
    எழுத்து	*encoding;
    எழுத்து	*cmap_encoding;
    முழு		needs_charset;
};

கட்டமைப்பு prt_ps_charset_S
{
    எழுத்து	*charset;
    எழுத்து	*cmap_charset;
    முழு		has_charset;
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
நிலையான கட்டமைப்பு prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", வெற்று,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
நிலையான கட்டமைப்பு prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  வெற்று,       CS_JIS_X_1983},
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
நிலையான கட்டமைப்பு prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    வெற்று,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     வெற்று,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
நிலையான கட்டமைப்பு prt_ps_charset_S sc_charsets[] =
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
நிலையான கட்டமைப்பு prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    வெற்று,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
நிலையான கட்டமைப்பு prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    வெற்று,	CS_B5},
    {"CP950",	    வெற்று,	CS_B5},
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
நிலையான கட்டமைப்பு prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", வெற்று,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
நிலையான கட்டமைப்பு prt_ps_charset_S k_charsets[] =
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
கட்டமைப்பு prt_ps_mbfont_S
{
    முழு				இலக்கம்_encodings;
    கட்டமைப்பு prt_ps_encoding_S	*encodings;
    முழு				இலக்கம்_charsets;
    கட்டமைப்பு prt_ps_charset_S	*charsets;
    எழுத்து			*ascii_enc;
    எழுத்து			*defcs;
};

நிலையான கட்டமைப்பு prt_ps_mbfont_S prt_ps_mbfonts[] =
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

கட்டமைப்பு prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    முழு     type;
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
நிலையான எழுத்து *prt_resource_types[] =
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

கட்டமைப்பு prt_dsc_comment_S
{
    எழுத்து	*string;
    முழு		len;
    முழு		type;
};

கட்டமைப்பு prt_dsc_வரி_S
{
    முழு		type;
    char_u	*string;
    முழு		len;
};


# define SIZEOF_CSTR(s)      (அளவு(s) - 1)
நிலையான கட்டமைப்பு prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

நிலையான வெறுமை prt_write_file_len(char_u *buffer, முழு bytes);
நிலையான முழு prt_next_dsc(கட்டமைப்பு prt_dsc_வரி_S *p_dsc_வரி);

/*
 * Variables for the output PostScript file.
 */
நிலையான FILE *prt_ps_fd;
நிலையான முழு prt_file_error;
நிலையான char_u *prt_ps_file_name = வெற்று;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
நிலையான தசம prt_page_width;
நிலையான தசம prt_page_height;
நிலையான தசம prt_இடது_margin;
நிலையான தசம prt_வலது_margin;
நிலையான தசம prt_உச்சி_margin;
நிலையான தசம prt_bottom_margin;
நிலையான தசம prt_வரி_height;
நிலையான தசம prt_first_வரி_height;
நிலையான தசம prt_char_width;
நிலையான தசம prt_எண்_width;
நிலையான தசம prt_bgcol_offset;
நிலையான தசம prt_pos_x_moveto = 0.0;
நிலையான தசம prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
நிலையான முழு prt_need_moveto;
நிலையான முழு prt_do_moveto;
நிலையான முழு prt_need_font;
நிலையான முழு prt_font;
நிலையான முழு prt_need_underline;
நிலையான முழு prt_underline;
நிலையான முழு prt_do_underline;
நிலையான முழு prt_need_fgcol;
நிலையான முழு prt_fgcol;
நிலையான முழு prt_need_bgcol;
நிலையான முழு prt_do_bgcol;
நிலையான முழு prt_bgcol;
நிலையான முழு prt_new_bgcol;
நிலையான முழு prt_attribute_change;
நிலையான தசம prt_text_run;
நிலையான முழு prt_page_இலக்கம்;
நிலையான முழு prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
நிலையான முழு prt_media;
நிலையான முழு prt_portrait;
நிலையான முழு prt_இலக்கம்_copies;
நிலையான முழு prt_duplex;
நிலையான முழு prt_tumble;
நிலையான முழு prt_collate;

/*
 * Buffers used when generating PostScript output
 */
நிலையான char_u prt_வரி_buffer[257];
நிலையான garray_T prt_ps_buffer;

நிலையான முழு prt_do_conv;
நிலையான vimconv_T prt_conv;

நிலையான முழு prt_out_mbyte;
நிலையான முழு prt_custom_cmap;
நிலையான எழுத்து prt_cmap[80];
நிலையான முழு prt_use_courier;
நிலையான முழு prt_in_ascii;
நிலையான முழு prt_half_width;
நிலையான எழுத்து *prt_ascii_encoding;
நிலையான மாறிலி char_u prt_hexchar[] = "0123456789abcdef";

    நிலையான வெறுமை
prt_write_file_raw_len(char_u *buffer, முழு bytes)
{
    எனில் (!prt_file_error
	    && fwrite(buffer, அளவு(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    நிலையான வெறுமை
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (முழு)STRLEN(buffer));
}

    நிலையான வெறுமை
prt_write_file_len(char_u *buffer, முழு bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    நிலையான வெறுமை
prt_write_string(எழுத்து *s)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer), "%s", s);
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write an int and a space.
 */
    நிலையான வெறுமை
prt_write_int(முழு i)
{
    sprintf((எழுத்து *)prt_வரி_buffer, "%d ", i);
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write a boolean and a space.
 */
    நிலையான வெறுமை
prt_write_boolean(முழு b)
{
    sprintf((எழுத்து *)prt_வரி_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    நிலையான வெறுமை
prt_def_font(
    எழுத்து	*new_name,
    எழுத்து	*encoding,
    முழு		height,
    எழுத்து	*font)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_வரி_buffer);
    எனில் (prt_out_mbyte)
	sprintf((எழுத்து *)prt_வரி_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    இல்லையெனில்
	vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write a line to define the CID font.
 */
    நிலையான வெறுமை
prt_def_cidfont(எழுத்து *new_name, முழு height, எழுத்து *cidfont)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_வரி_buffer);
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    நிலையான வெறுமை
prt_dup_cidfont(எழுத்து *original_name, எழுத்து *new_name)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_வரி_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    நிலையான வெறுமை
prt_real_bits(
    இரட்டை      real,
    முழு		precision,
    முழு		*pinteger,
    முழு		*pfraction)
{
    முழு     i;
    முழு     integer;
    தசம   fraction;

    integer = (முழு)real;
    fraction = (தசம)(real - integer);
    எனில் (real < (இரட்டை)integer)
	fraction = -fraction;
    க்கு (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (முழு)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    நிலையான வெறுமை
prt_write_real(இரட்டை val, முழு prec)
{
    முழு     integer;
    முழு     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((எழுத்து *)prt_வரி_buffer, "%d", integer);
    prt_write_file(prt_வரி_buffer);
    // Only emit fraction if necessary
    எனில் (fraction != 0)
    {
	// Remove any trailing zeros
	போது ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((எழுத்து *)prt_வரி_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_வரி_buffer);
    }
    sprintf((எழுத்து *)prt_வரி_buffer, " ");
    prt_write_file(prt_வரி_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    நிலையான வெறுமை
prt_def_var(எழுத்து *name, இரட்டை மதிப்பு, முழு prec)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
								"/%s ", name);
    prt_write_file(prt_வரி_buffer);
    prt_write_real(மதிப்பு, prec);
    sprintf((எழுத்து *)prt_வரி_buffer, "d\n");
    prt_write_file(prt_வரி_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    நிலையான வெறுமை
prt_flush_buffer(வெறுமை)
{
    எனில் (prt_ps_buffer.ga_len <= 0)
	திரும்பு;

    // Any background color must be drawn first
    எனில் (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	முழு     r, g, b;

	எனில் (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_வரி_height, 2);

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
    எனில் (prt_do_underline)
    {
	எனில் (prt_do_moveto)
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
    எனில் (prt_out_mbyte)
	prt_write_string("<");
    இல்லையெனில்
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    எனில் (prt_out_mbyte)
	prt_write_string(">");
    இல்லையெனில்
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    எனில் (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    இல்லையெனில் // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, அளவு(எழுத்து), prt_bufsiz);
}


    நிலையான வெறுமை
prt_resource_name(char_u *filename, வெறுமை *cookie)
{
    char_u *resource_filename = cookie;

    எனில் (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    இல்லையெனில்
	STRCPY(resource_filename, filename);
}

    நிலையான முழு
prt_find_resource(எழுத்து *name, கட்டமைப்பு prt_ps_resource_S *resource)
{
    char_u	*buffer;
    முழு		retval;

    buffer = alloc(MAXPATHL + 1);
    எனில் (buffer == வெற்று)
	திரும்பு FALSE;

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
    திரும்பு retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
கட்டமைப்பு prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    முழு     len;
    முழு     வரி_start;
    முழு     வரி_end;
};

நிலையான கட்டமைப்பு prt_resfile_buffer_S prt_resfile;

    நிலையான முழு
prt_resfile_next_வரி(வெறுமை)
{
    முழு     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.வரி_end + 1;
    போது (idx < prt_resfile.len)
    {
	எனில் (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    நிறுத்து;
	idx++;
    }
    prt_resfile.வரி_start = idx;

    போது (idx < prt_resfile.len)
    {
	எனில் (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    நிறுத்து;
	idx++;
    }
    prt_resfile.வரி_end = idx;

    திரும்பு (idx < prt_resfile.len);
}

    நிலையான முழு
prt_resfile_strncmp(முழு offset, எழுத்து *string, முழு len)
{
    // Force not equal if string is longer than remainder of line
    எனில் (len > (prt_resfile.வரி_end - (prt_resfile.வரி_start + offset)))
	திரும்பு 1;

    திரும்பு STRNCMP(&prt_resfile.buffer[prt_resfile.வரி_start + offset],
								string, len);
}

    நிலையான முழு
prt_resfile_skip_nonws(முழு offset)
{
    முழு     idx;

    idx = prt_resfile.வரி_start + offset;
    போது (idx < prt_resfile.வரி_end)
    {
	எனில் (SAFE_isspace(prt_resfile.buffer[idx]))
	    திரும்பு idx - prt_resfile.வரி_start;
	idx++;
    }
    திரும்பு -1;
}

    நிலையான முழு
prt_resfile_skip_ws(முழு offset)
{
    முழு     idx;

    idx = prt_resfile.வரி_start + offset;
    போது (idx < prt_resfile.வரி_end)
    {
	எனில் (!SAFE_isspace(prt_resfile.buffer[idx]))
	    திரும்பு idx - prt_resfile.வரி_start;
	idx++;
    }
    திரும்பு -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    நிலையான முழு
prt_next_dsc(கட்டமைப்பு prt_dsc_வரி_S *p_dsc_வரி)
{
    முழு     comment;
    முழு     offset;

    // Move to start of next line
    எனில் (!prt_resfile_next_வரி())
	திரும்பு FALSE;

    // DSC comments always start %%
    எனில் (prt_resfile_strncmp(0, "%%", 2) != 0)
	திரும்பு FALSE;

    // Find type of DSC comment
    க்கு (comment = 0; comment < (முழு)ARRAY_LENGTH(prt_dsc_table); comment++)
	எனில் (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    நிறுத்து;

    எனில் (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_வரி->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    இல்லையெனில்
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_வரி->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	எனில் (offset == -1)
	    திரும்பு FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    எனில் (offset == -1)
	திரும்பு FALSE;

    p_dsc_வரி->string = &prt_resfile.buffer[prt_resfile.வரி_start + offset];
    p_dsc_வரி->len = prt_resfile.வரி_end - (prt_resfile.வரி_start + offset);

    திரும்பு TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    நிலையான முழு
prt_open_resource(கட்டமைப்பு prt_ps_resource_S *resource)
{
    முழு		offset;
    முழு		seen_all;
    முழு		seen_title;
    முழு		seen_version;
    FILE	*fd_resource;
    கட்டமைப்பு prt_dsc_வரி_S dsc_வரி;

    fd_resource = mch_fopen((எழுத்து *)resource->filename, READBIN);
    எனில் (fd_resource == வெற்று)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	திரும்பு FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (முழு)fread((எழுத்து *)prt_resfile.buffer, அளவு(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    எனில் (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	திரும்பு FALSE;
    }
    fclose(fd_resource);

    prt_resfile.வரி_end = -1;
    prt_resfile.வரி_start = 0;
    எனில் (!prt_resfile_next_வரி())
	திரும்பு FALSE;

    offset = 0;

    எனில் (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (முழு)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	திரும்பு FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (முழு)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    எனில் (offset == -1)
	திரும்பு FALSE;
    offset = prt_resfile_skip_ws(offset);
    எனில் (offset == -1)
	திரும்பு FALSE;

    எனில் (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (முழு)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	திரும்பு FALSE;
    }
    offset += (முழு)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    எனில் (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (முழு)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    இல்லையெனில் எனில் (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (முழு)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    இல்லையெனில் எனில் (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (முழு)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    இல்லையெனில்
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	திரும்பு FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    போது (!seen_all && prt_next_dsc(&dsc_வரி))
    {
	தேர்வு (dsc_வரி.type)
	{
	நிலை PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_வரி.string, dsc_வரி.len);
	    seen_title = TRUE;
	    எனில் (seen_version)
		seen_all = TRUE;
	    நிறுத்து;

	நிலை PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_வரி.string, dsc_வரி.len);
	    seen_version = TRUE;
	    எனில் (seen_title)
		seen_all = TRUE;
	    நிறுத்து;

	நிலை PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    நிறுத்து;

	நிலை PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    நிறுத்து;
	}
    }

    எனில் (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

    நிலையான முழு
prt_check_resource(கட்டமைப்பு prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    எனில் (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	திரும்பு FALSE;
    }

    // Other checks to be added as needed
    திரும்பு TRUE;
}

    நிலையான வெறுமை
prt_dsc_start(வெறுமை)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    நிலையான வெறுமை
prt_dsc_noarg(எழுத்து *comment)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_வரி_buffer);
}

    நிலையான வெறுமை
prt_dsc_textline(எழுத்து *comment, எழுத்து *text)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_வரி_buffer);
}

    நிலையான வெறுமை
prt_dsc_text(எழுத்து *comment, எழுத்து *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_வரி_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    நிலையான வெறுமை
prt_dsc_ints(எழுத்து *comment, முழு மொத்தம், முழு *ints)
{
    முழு		i;

    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_வரி_buffer);

    க்கு (i = 0; i < மொத்தம்; i++)
    {
	sprintf((எழுத்து *)prt_வரி_buffer, " %d", ints[i]);
	prt_write_file(prt_வரி_buffer);
    }

    prt_write_string("\n");
}

    நிலையான வெறுமை
prt_dsc_resources(
    எழுத்து	*comment,	// if NULL add to previous
    எழுத்து	*type,
    எழுத்து	*string)
{
    எனில் (comment != வெற்று)
	vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
						 "%%%%%s: %s", comment, type);
    இல்லையெனில்
	vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_வரி_buffer);

    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
							     " %s\n", string);
    prt_write_file(prt_வரி_buffer);
}

    நிலையான வெறுமை
prt_dsc_font_resource(எழுத்து *resource, கட்டமைப்பு prt_ps_font_S *ps_font)
{
    முழு     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    க்கு (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	எனில் (ps_font->ps_fontname[i] != வெற்று)
	    prt_dsc_resources(வெற்று, "font", ps_font->ps_fontname[i]);
}

    நிலையான வெறுமை
prt_dsc_requirements(
    முழு		duplex,
    முழு		tumble,
    முழு		collate,
    முழு		color,
    முழு		இலக்கம்_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    எனில் (!(duplex || collate || color || (இலக்கம்_copies > 1)))
	திரும்பு;

    sprintf((எழுத்து *)prt_வரி_buffer, "%%%%Requirements:");
    prt_write_file(prt_வரி_buffer);

    எனில் (duplex)
    {
	prt_write_string(" duplex");
	எனில் (tumble)
	    prt_write_string("(tumble)");
    }
    எனில் (collate)
	prt_write_string(" collate");
    எனில் (color)
	prt_write_string(" color");
    எனில் (இலக்கம்_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((எழுத்து *)prt_வரி_buffer, "%d", இலக்கம்_copies);
	prt_write_file(prt_வரி_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    நிலையான வெறுமை
prt_dsc_docmedia(
    எழுத்து	*paper_name,
    இரட்டை	width,
    இரட்டை	height,
    இரட்டை	weight,
    எழுத்து	*colour,
    எழுத்து	*type)
{
    vim_snprintf((எழுத்து *)prt_வரி_buffer, அளவு(prt_வரி_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_வரி_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    எனில் (colour == வெற்று)
	prt_write_string("()");
    இல்லையெனில்
	prt_write_string(colour);
    prt_write_string(" ");
    எனில் (type == வெற்று)
	prt_write_string("()");
    இல்லையெனில்
	prt_write_string(type);
    prt_write_string("\n");
}

    வெறுமை
mch_print_cleanup(வெறுமை)
{
    எனில் (prt_out_mbyte)
    {
	முழு     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	க்கு (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    எனில் (prt_ps_mb_font.ps_fontname[i] != வெற்று)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    எனில் (prt_do_conv)
    {
	convert_setup(&prt_conv, வெற்று, வெற்று);
	prt_do_conv = FALSE;
    }
    எனில் (prt_ps_fd != வெற்று)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = வெற்று;
	prt_file_error = FALSE;
    }
    எனில் (prt_ps_file_name != வெற்று)
	VIM_CLEAR(prt_ps_file_name);
}

    நிலையான தசம
to_device_units(முழு idx, இரட்டை physsize, முழு def_எண்)
{
    தசம	ret;
    முழு		u;
    முழு		nr;

    u = prt_get_unit(idx);
    எனில் (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_எண்;
    }
    இல்லையெனில்
	nr = printer_opts[idx].எண்;

    தேர்வு (u)
    {
	நிலை PRT_UNIT_INCH:
	    ret = (தசம)(nr * PRT_PS_DEFAULT_DPI);
	    நிறுத்து;
	நிலை PRT_UNIT_MM:
	    ret = (தசம)(nr * PRT_PS_DEFAULT_DPI) / (தசம)25.4;
	    நிறுத்து;
	நிலை PRT_UNIT_POINT:
	    ret = (தசம)nr;
	    நிறுத்து;
	நிலை PRT_UNIT_PERC:
	இயல்பு:
	    ret = (தசம)(physsize * nr) / 100;
	    நிறுத்து;
    }

    திரும்பு ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    நிலையான வெறுமை
prt_page_margins(
    இரட்டை	width,
    இரட்டை	height,
    இரட்டை	*இடது,
    இரட்டை	*வலது,
    இரட்டை	*உச்சி,
    இரட்டை	*bottom)
{
    *இடது   = to_device_units(OPT_PRINT_இடது, width, 10);
    *வலது  = width - to_device_units(OPT_PRINT_வலது, width, 5);
    *உச்சி    = height - to_device_units(OPT_PRINT_உச்சி, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    நிலையான வெறுமை
prt_font_metrics(முழு font_scale)
{
    prt_வரி_height = (தசம)font_scale;
    prt_char_width = (தசம)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    நிலையான முழு
prt_get_cpl(வெறுமை)
{
    எனில் (prt_use_எண்())
    {
	prt_எண்_width = PRINT_எண்_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	எனில் (prt_out_mbyte)
	    prt_எண்_width /= 2;
	prt_இடது_margin += prt_எண்_width;
    }
    இல்லையெனில்
	prt_எண்_width = 0.0;

    திரும்பு (முழு)((prt_வலது_margin - prt_இடது_margin) / prt_char_width);
}

    நிலையான முழு
prt_build_cid_fontname(முழு font, char_u *name, முழு name_len)
{
    எழுத்து    *fontname;

    fontname = alloc(name_len + 1);
    எனில் (fontname == வெற்று)
	திரும்பு FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    திரும்பு TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    நிலையான முழு
prt_get_lpp(வெறுமை)
{
    முழு lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (தசம)PRT_PS_FONT_TO_USER(prt_வரி_height,
					   prt_ps_font->bbox_min_y);
    எனில் ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (தசம)PRT_PS_FONT_TO_USER(prt_வரி_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_வரி_height = prt_வரி_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (முழு)((prt_உச்சி_margin - prt_bottom_margin) / prt_வரி_height);

    // Adjust top margin if there is a header
    prt_உச்சி_margin -= prt_வரி_height * prt_header_height();

    திரும்பு lpp - prt_header_height();
}

    நிலையான முழு
prt_match_encoding(
    எழுத்து			*p_encoding,
    கட்டமைப்பு prt_ps_mbfont_S	*p_cmap,
    கட்டமைப்பு prt_ps_encoding_S	**pp_mbenc)
{
    முழு				mbenc;
    முழு				enc_len;
    கட்டமைப்பு prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = வெற்று;
    // Look for recognised encoding
    enc_len = (முழு)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    க்கு (mbenc = 0; mbenc < p_cmap->இலக்கம்_encodings; mbenc++)
    {
	எனில் (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    திரும்பு TRUE;
	}
	p_mbenc++;
    }
    திரும்பு FALSE;
}

    நிலையான முழு
prt_match_charset(
    எழுத்து		    *p_charset,
    கட்டமைப்பு prt_ps_mbfont_S  *p_cmap,
    கட்டமைப்பு prt_ps_charset_S **pp_mbchar)
{
    முழு			    mbchar;
    முழு			    char_len;
    கட்டமைப்பு prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    எனில் (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (முழு)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    க்கு (mbchar = 0; mbchar < p_cmap->இலக்கம்_charsets; mbchar++)
    {
	எனில் (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    திரும்பு TRUE;
	}
	p_mbchar++;
    }
    திரும்பு FALSE;
}

    முழு
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    முழு		forceit UNUSED)
{
    முழு		i;
    எழுத்து	*paper_name;
    முழு		paper_strlen;
    முழு		fontsize;
    char_u	*p;
    இரட்டை      இடது;
    இரட்டை      வலது;
    இரட்டை      உச்சி;
    இரட்டை      bottom;
    முழு		props;
    முழு		cmap = 0;
    char_u	*p_encoding;
    கட்டமைப்பு prt_ps_encoding_S *p_mbenc;
    கட்டமைப்பு prt_ps_encoding_S *p_mbenc_first;
    கட்டமைப்பு prt_ps_charset_S  *p_mbchar = வெற்று;

# எனில் 0
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
    எனில் (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    எனில் (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = வெற்று;
    props = enc_canon_props(p_encoding);
    எனில் (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	முழு cmap_first = 0;

	p_mbenc_first = வெற்று;
	க்கு (cmap = 0; cmap < (முழு)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    எனில் (prt_match_encoding((எழுத்து *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		எனில் (p_mbenc_first == வெற்று)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		எனில் (prt_match_charset((எழுத்து *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    நிறுத்து;
	    }

	// Use first encoding matched if no charset matched
	எனில் (p_mbchar == வெற்று && p_mbenc_first != வெற்று)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != வெற்று);
    எனில் (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == வெற்று);
	எனில் (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    எனில் ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		திரும்பு FALSE;
	    }

	    // Add charset name if not empty
	    எனில் (p_mbchar->cmap_charset != வெற்று)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, அளவு(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	இல்லையெனில்
	{
	    // Add custom CMap character set name
	    எனில் (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		திரும்பு FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, அளவு(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	எனில் (p_mbenc->cmap_encoding != வெற்று && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < அளவு(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	எனில் (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    திரும்பு FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	எனில் (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    திரும்பு FALSE;
	எனில் (mbfont_opts[OPT_MBFONT_BOLD].present)
	    எனில் (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		திரும்பு FALSE;
	எனில் (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    எனில் (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		திரும்பு FALSE;
	எனில் (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    எனில் (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		திரும்பு FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	எனில் (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    எனில் (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    இல்லையெனில்
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    இல்லையெனில்
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    எனில் (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (எழுத்து *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    இல்லையெனில்
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    க்கு (i = 0; i < (முழு)PRT_MEDIASIZE_LEN; ++i)
	எனில் (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    நிறுத்து;
    எனில் (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    எனில் (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    இல்லையெனில்
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &இடது, &வலது, &உச்சி,
								    &bottom);
    prt_இடது_margin = (தசம)இடது;
    prt_வலது_margin = (தசம)வலது;
    prt_உச்சி_margin = (தசம)உச்சி;
    prt_bottom_margin = (தசம)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    க்கு (p = p_pfn; (p = vim_strchr(p, ':')) != வெற்று; ++p)
	எனில் (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((எழுத்து *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_வரி = prt_get_cpl();
    psettings->வரிகள்_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    எனில் (psettings->chars_per_வரி <= 0 || psettings->வரிகள்_per_page <= 0)
	திரும்பு FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_இலக்கம்_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    எனில் (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    இல்லையெனில்
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_இலக்கம்_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    எனில் (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	எனில் (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	இல்லையெனில் எனில் (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    எனில் (psettings->outfile == வெற்று)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	எனில் (prt_ps_file_name == வெற்று)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    திரும்பு FAIL;
	}
	prt_ps_fd = mch_fopen((எழுத்து *)prt_ps_file_name, WRITEBIN);
    }
    இல்லையெனில்
    {
	p = expand_env_save(psettings->outfile);
	எனில் (p != வெற்று)
	{
	    prt_ps_fd = mch_fopen((எழுத்து *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    எனில் (prt_ps_fd == வெற்று)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	திரும்பு FAIL;
    }

    prt_bufsiz = psettings->chars_per_வரி;
    எனில் (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, அளவு(எழுத்து), prt_bufsiz);

    prt_page_இலக்கம் = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    திரும்பு OK;
}

    நிலையான முழு
prt_add_resource(கட்டமைப்பு prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((எழுத்து *)resource->filename, READBIN);
    எனில் (fd_resource == வெற்று)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	திரும்பு FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (எழுத்து *)resource->title);

    prt_dsc_textline("BeginDocument", (எழுத்து *)resource->filename);

    க்கு (;;)
    {
	bytes_read = fread((எழுத்து *)resource_buffer, அளவு(char_u),
			   அளவு(resource_buffer), fd_resource);
	எனில் (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    திரும்பு FALSE;
	}
	எனில் (bytes_read == 0)
	    நிறுத்து;
	prt_write_file_raw_len(resource_buffer, (முழு)bytes_read);
	எனில் (prt_file_error)
	{
	    fclose(fd_resource);
	    திரும்பு FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    திரும்பு TRUE;
}

    முழு
mch_print_begin(prt_settings_T *psettings)
{
    முழு		bbox[4];
    இரட்டை      இடது;
    இரட்டை      வலது;
    இரட்டை      உச்சி;
    இரட்டை      bottom;
    கட்டமைப்பு prt_ps_resource_S *res_prolog;
    கட்டமைப்பு prt_ps_resource_S *res_encoding;
    எழுத்து	buffer[256];
    char_u      *p_encoding;
    கட்டமைப்பு prt_ps_resource_S *res_cidfont;
    கட்டமைப்பு prt_ps_resource_S *res_cmap;
    முழு		retval = FALSE;

    res_prolog = ALLOC_ONE(கட்டமைப்பு prt_ps_resource_S);
    res_encoding = ALLOC_ONE(கட்டமைப்பு prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(கட்டமைப்பு prt_ps_resource_S);
    res_cmap = ALLOC_ONE(கட்டமைப்பு prt_ps_resource_S);
    எனில் (res_prolog == வெற்று || res_encoding == வெற்று
	    || res_cidfont == வெற்று || res_cmap == வெற்று)
	செல் theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (எழுத்து *)psettings->jobname);
    எனில் (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(வெற்று), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &இடது, &வலது, &உச்சி, &bottom);
    bbox[0] = (முழு)இடது;
    எனில் (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (முழு)(உச்சி - (psettings->வரிகள்_per_page + prt_header_height())
						    * (இரட்டை)prt_வரி_height);
	bbox[2] = (முழு)(இடது + psettings->chars_per_வரி
					       * (இரட்டை)prt_char_width + 0.5);
	bbox[3] = (முழு)(உச்சி + 0.5);
    }
    இல்லையெனில்
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (முழு)bottom;
	bbox[2] = (முழு)(இடது + ((psettings->வரிகள்_per_page
			      + prt_header_height()) * prt_வரி_height) + 0.5);
	bbox[3] = (முழு)(bottom + psettings->chars_per_வரி
					       * (இரட்டை)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(இரட்டை)0, வெற்று, வெற்று);
    // Define fonts needed
    எனில் (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    எனில் (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? வெற்று
				: "DocumentNeededResources"), &prt_ps_mb_font);
	எனில் (!prt_custom_cmap)
	    prt_dsc_resources(வெற்று, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    எனில் (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	செல் theend;
    }
    எனில் (!prt_open_resource(res_prolog))
	செல் theend;
    எனில் (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	செல் theend;
    எனில் (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	எனில் (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    செல் theend;
	}
	எனில் (!prt_open_resource(res_cidfont))
	    செல் theend;
	எனில் (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    செல் theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    எனில் (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	எனில் (*p_encoding == NUL
		|| !prt_find_resource((எழுத்து *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    முழு		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    எனில் (!(props & ENC_8BIT)
		    || !prt_find_resource((எழுத்து *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		எனில் (!prt_find_resource((எழுத்து *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    செல் theend;
		}
	    }
	}
	எனில் (!prt_open_resource(res_encoding))
	    செல் theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    இல்லையெனில்
    {
	p_encoding = enc_skip(p_penc);
	எனில் (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	எனில் (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    எனில் (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		செல் theend;
	    }
	    எனில் (!prt_open_resource(res_encoding))
		செல் theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    எனில் (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	எனில் (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    செல் theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    எனில் (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	எனில் (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    செல் theend;
	}
	எனில் (!prt_open_resource(res_cmap))
	    செல் theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    எனில் (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(வெற்று, "procset", buffer);

	எனில் (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(வெற்று, "cmap", buffer);
	}
    }
    எனில் (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(வெற்று, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# இல்லையெனில்
					0
# endif
					, prt_இலக்கம்_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    எனில் (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    எனில் (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? வெற்று : "PageResources"),
							    &prt_ps_mb_font);
	எனில் (!prt_custom_cmap)
	    prt_dsc_resources(வெற்று, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    எனில் (!prt_add_resource(res_prolog))
	செல் theend;
    எனில் (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	எனில் (!prt_add_resource(res_cidfont))
	    செல் theend;
	எனில் (prt_custom_cmap && !prt_add_resource(res_cmap))
	    செல் theend;
    }

    எனில் (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	எனில் (!prt_add_resource(res_encoding))
	    செல் theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((முழு)prt_mediasize[prt_media].width);
    prt_write_int((முழு)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_இலக்கம்_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    எனில் (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	எனில் (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (எழுத்து *)p_encoding, (முழு)prt_வரி_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (எழுத்து *)p_encoding, (முழு)prt_வரி_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (எழுத்து *)p_encoding, (முழு)prt_வரி_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (எழுத்து *)p_encoding, (முழு)prt_வரி_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    எனில் (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	எனில் (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (முழு)prt_வரி_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	எனில் (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != வெற்று)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    எனில் (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (முழு)prt_வரி_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	இல்லையெனில்
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	எனில் (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != வெற்று)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    எனில் (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (முழு)prt_வரி_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	இல்லையெனில்
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	எனில் (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != வெற்று)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    எனில் (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (முழு)prt_வரி_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	இல்லையெனில்
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_வரி_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_வரி_height,
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

    திரும்பு retval;
}

    வெறுமை
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_இலக்கம்);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    எனில் (!prt_file_error && psettings->outfile == வெற்று
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	எனில் (prt_ps_fd != வெற்று)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = வெற்று;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	எனில் (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	இல்லையெனில்
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    முழு
mch_print_end_page(வெறுமை)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    திரும்பு !prt_file_error;
}

    முழு
mch_print_begin_page(char_u *str UNUSED)
{
    முழு		page_இலக்கம்[2];

    prt_page_இலக்கம்++;

    page_இலக்கம்[0] = page_இலக்கம்[1] = prt_page_இலக்கம்;
    prt_dsc_ints("Page", 2, page_இலக்கம்);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    எனில் (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    இல்லையெனில்
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    எனில் (!prt_portrait)
    {
	prt_write_int(-((முழு)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    திரும்பு !prt_file_error;
}

    முழு
mch_print_blank_page(வெறுமை)
{
    திரும்பு (mch_print_begin_page(வெற்று) ? (mch_print_end_page()) : FALSE);
}

நிலையான தசம prt_pos_x = 0;
நிலையான தசம prt_pos_y = 0;

    வெறுமை
mch_print_start_வரி(முழு margin, முழு page_வரி)
{
    prt_pos_x = prt_இடது_margin;
    எனில் (margin)
	prt_pos_x -= prt_எண்_width;

    prt_pos_y = prt_உச்சி_margin - prt_first_வரி_height -
					page_வரி * prt_வரி_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    முழு
mch_print_text_out(char_u *textp, முழு len UNUSED)
{
    char_u	*p = textp;
    முழு		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    தசம       char_width;
    தசம       next_pos;
    முழு		in_ascii;
    முழு		half_width;
    char_u	*tofree = வெற்று;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    எனில் (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	எனில் (prt_in_ascii)
	{
	    எனில் (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	இல்லையெனில் எனில் (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    எனில் (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	எனில் (half_width)
	    char_width /= 2;
	எனில் (prt_half_width)
	{
	    எனில் (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	இல்லையெனில் எனில் (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    எனில் (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	எனில் (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	எனில் (prt_need_font)
	{
	    எனில் (!prt_in_ascii)
		prt_write_string("CF");
	    இல்லையெனில்
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	எனில் (prt_need_fgcol)
	{
	    முழு     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    எனில் (r == g && g == b)
		prt_write_string("g\n");
	    இல்லையெனில்
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	எனில் (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    எனில் (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	இல்லையெனில்
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	எனில் (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    எனில் (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	எனில் (p == வெற்று)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    எனில் (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	போது (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    இல்லையெனில்
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	எனில் (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    தேர்வு (ch)
	    {
		நிலை BS:   ga_append(&prt_ps_buffer, 'b'); நிறுத்து;
		நிலை TAB:  ga_append(&prt_ps_buffer, 't'); நிறுத்து;
		நிலை NL:   ga_append(&prt_ps_buffer, 'n'); நிறுத்து;
		நிலை FF:   ga_append(&prt_ps_buffer, 'f'); நிறுத்து;
		நிலை CAR:  ga_append(&prt_ps_buffer, 'r'); நிறுத்து;
		நிலை '(':  ga_append(&prt_ps_buffer, '('); நிறுத்து;
		நிலை ')':  ga_append(&prt_ps_buffer, ')'); நிறுத்து;
		நிலை '\\': ga_append(&prt_ps_buffer, '\\'); நிறுத்து;

		இயல்பு:
			   sprintf((எழுத்து *)ch_buff, "%03o", (unsigned முழு)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   நிறுத்து;
	    }
	}
	இல்லையெனில்
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_வலது_margin) &&
		    ((next_pos - prt_வலது_margin) > (prt_வலது_margin*1e-5));

    எனில் (need_break)
	prt_flush_buffer();

    திரும்பு need_break;
}

    வெறுமை
mch_print_set_font(முழு iBold, முழு iItalic, முழு iUnderline)
{
    முழு		font = 0;

    எனில் (iBold)
	font |= 0x01;
    எனில் (iItalic)
	font |= 0x02;

    எனில் (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    எனில் (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    வெறுமை
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (முழு)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    வெறுமை
mch_print_set_fg(long_u fgcol)
{
    எனில் (fgcol == (long_u)prt_fgcol)
	திரும்பு;

    prt_fgcol = (முழு)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
