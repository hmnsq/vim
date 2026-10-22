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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_അക്കം_OPTIONS] =
{
    {"c",	FALSE, 0, ശൂന്യ, 0, FALSE},
    {"a",	FALSE, 0, ശൂന്യ, 0, FALSE},
    {"r",	FALSE, 0, ശൂന്യ, 0, FALSE},
    {"b",	FALSE, 0, ശൂന്യ, 0, FALSE},
    {"i",	FALSE, 0, ശൂന്യ, 0, FALSE},
    {"o",	FALSE, 0, ശൂന്യ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ഘടന prt_ps_font_S
{
    പൂർണ്ണസംഖ്യ		wx;
    പൂർണ്ണസംഖ്യ		uline_offset;
    പൂർണ്ണസംഖ്യ		uline_width;
    പൂർണ്ണസംഖ്യ		bbox_min_y;
    പൂർണ്ണസംഖ്യ		bbox_max_y;
    അക്ഷരം	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
സ്ഥിതം ഘടന prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
സ്ഥിതം ഘടന prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ}
};

// Pointer to current font set being used
സ്ഥിതം ഘടന prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ഘടന prt_ps_encoding_S
{
    അക്ഷരം	*encoding;
    അക്ഷരം	*cmap_encoding;
    പൂർണ്ണസംഖ്യ		needs_charset;
};

ഘടന prt_ps_charset_S
{
    അക്ഷരം	*charset;
    അക്ഷരം	*cmap_charset;
    പൂർണ്ണസംഖ്യ		has_charset;
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
സ്ഥിതം ഘടന prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ശൂന്യ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
സ്ഥിതം ഘടന prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ശൂന്യ,       CS_JIS_X_1983},
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
സ്ഥിതം ഘടന prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ശൂന്യ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ശൂന്യ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
സ്ഥിതം ഘടന prt_ps_charset_S sc_charsets[] =
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
സ്ഥിതം ഘടന prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ശൂന്യ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
സ്ഥിതം ഘടന prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ശൂന്യ,	CS_B5},
    {"CP950",	    ശൂന്യ,	CS_B5},
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
സ്ഥിതം ഘടന prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ശൂന്യ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
സ്ഥിതം ഘടന prt_ps_charset_S k_charsets[] =
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
ഘടന prt_ps_mbfont_S
{
    പൂർണ്ണസംഖ്യ				അക്കം_encodings;
    ഘടന prt_ps_encoding_S	*encodings;
    പൂർണ്ണസംഖ്യ				അക്കം_charsets;
    ഘടന prt_ps_charset_S	*charsets;
    അക്ഷരം			*ascii_enc;
    അക്ഷരം			*defcs;
};

സ്ഥിതം ഘടന prt_ps_mbfont_S prt_ps_mbfonts[] =
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

ഘടന prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    പൂർണ്ണസംഖ്യ     type;
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
സ്ഥിതം അക്ഷരം *prt_resource_types[] =
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

ഘടന prt_dsc_comment_S
{
    അക്ഷരം	*string;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		type;
};

ഘടന prt_dsc_വരി_S
{
    പൂർണ്ണസംഖ്യ		type;
    char_u	*string;
    പൂർണ്ണസംഖ്യ		len;
};


# define SIZEOF_CSTR(s)      (വലുപ്പം(s) - 1)
സ്ഥിതം ഘടന prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

സ്ഥിതം ശൂന്യം prt_write_file_len(char_u *buffer, പൂർണ്ണസംഖ്യ bytes);
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_next_dsc(ഘടന prt_dsc_വരി_S *p_dsc_വരി);

/*
 * Variables for the output PostScript file.
 */
സ്ഥിതം FILE *prt_ps_fd;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_file_error;
സ്ഥിതം char_u *prt_ps_file_name = ശൂന്യ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
സ്ഥിതം ദശാംശം prt_page_width;
സ്ഥിതം ദശാംശം prt_page_height;
സ്ഥിതം ദശാംശം prt_ഇടത്_margin;
സ്ഥിതം ദശാംശം prt_വലത്_margin;
സ്ഥിതം ദശാംശം prt_മുകൾ_margin;
സ്ഥിതം ദശാംശം prt_bottom_margin;
സ്ഥിതം ദശാംശം prt_വരി_height;
സ്ഥിതം ദശാംശം prt_first_വരി_height;
സ്ഥിതം ദശാംശം prt_char_width;
സ്ഥിതം ദശാംശം prt_സംഖ്യ_width;
സ്ഥിതം ദശാംശം prt_bgcol_offset;
സ്ഥിതം ദശാംശം prt_pos_x_moveto = 0.0;
സ്ഥിതം ദശാംശം prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_need_moveto;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_do_moveto;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_need_font;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_font;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_need_underline;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_underline;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_do_underline;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_need_fgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_fgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_need_bgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_do_bgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_bgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_new_bgcol;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_attribute_change;
സ്ഥിതം ദശാംശം prt_text_run;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_page_അക്കം;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_media;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_portrait;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_അക്കം_copies;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_duplex;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_tumble;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_collate;

/*
 * Buffers used when generating PostScript output
 */
സ്ഥിതം char_u prt_വരി_buffer[257];
സ്ഥിതം garray_T prt_ps_buffer;

സ്ഥിതം പൂർണ്ണസംഖ്യ prt_do_conv;
സ്ഥിതം vimconv_T prt_conv;

സ്ഥിതം പൂർണ്ണസംഖ്യ prt_out_mbyte;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_custom_cmap;
സ്ഥിതം അക്ഷരം prt_cmap[80];
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_use_courier;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_in_ascii;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_half_width;
സ്ഥിതം അക്ഷരം *prt_ascii_encoding;
സ്ഥിതം സ്ഥിരം char_u prt_hexchar[] = "0123456789abcdef";

    സ്ഥിതം ശൂന്യം
prt_write_file_raw_len(char_u *buffer, പൂർണ്ണസംഖ്യ bytes)
{
    എങ്കിൽ (!prt_file_error
	    && fwrite(buffer, വലുപ്പം(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    സ്ഥിതം ശൂന്യം
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (പൂർണ്ണസംഖ്യ)STRLEN(buffer));
}

    സ്ഥിതം ശൂന്യം
prt_write_file_len(char_u *buffer, പൂർണ്ണസംഖ്യ bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    സ്ഥിതം ശൂന്യം
prt_write_string(അക്ഷരം *s)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer), "%s", s);
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write an int and a space.
 */
    സ്ഥിതം ശൂന്യം
prt_write_int(പൂർണ്ണസംഖ്യ i)
{
    sprintf((അക്ഷരം *)prt_വരി_buffer, "%d ", i);
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write a boolean and a space.
 */
    സ്ഥിതം ശൂന്യം
prt_write_boolean(പൂർണ്ണസംഖ്യ b)
{
    sprintf((അക്ഷരം *)prt_വരി_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    സ്ഥിതം ശൂന്യം
prt_def_font(
    അക്ഷരം	*new_name,
    അക്ഷരം	*encoding,
    പൂർണ്ണസംഖ്യ		height,
    അക്ഷരം	*font)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_വരി_buffer);
    എങ്കിൽ (prt_out_mbyte)
	sprintf((അക്ഷരം *)prt_വരി_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write a line to define the CID font.
 */
    സ്ഥിതം ശൂന്യം
prt_def_cidfont(അക്ഷരം *new_name, പൂർണ്ണസംഖ്യ height, അക്ഷരം *cidfont)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_വരി_buffer);
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    സ്ഥിതം ശൂന്യം
prt_dup_cidfont(അക്ഷരം *original_name, അക്ഷരം *new_name)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_വരി_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    സ്ഥിതം ശൂന്യം
prt_real_bits(
    ഇരട്ട      real,
    പൂർണ്ണസംഖ്യ		precision,
    പൂർണ്ണസംഖ്യ		*pinteger,
    പൂർണ്ണസംഖ്യ		*pfraction)
{
    പൂർണ്ണസംഖ്യ     i;
    പൂർണ്ണസംഖ്യ     integer;
    ദശാംശം   fraction;

    integer = (പൂർണ്ണസംഖ്യ)real;
    fraction = (ദശാംശം)(real - integer);
    എങ്കിൽ (real < (ഇരട്ട)integer)
	fraction = -fraction;
    വേണ്ടി (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (പൂർണ്ണസംഖ്യ)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    സ്ഥിതം ശൂന്യം
prt_write_real(ഇരട്ട val, പൂർണ്ണസംഖ്യ prec)
{
    പൂർണ്ണസംഖ്യ     integer;
    പൂർണ്ണസംഖ്യ     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((അക്ഷരം *)prt_വരി_buffer, "%d", integer);
    prt_write_file(prt_വരി_buffer);
    // Only emit fraction if necessary
    എങ്കിൽ (fraction != 0)
    {
	// Remove any trailing zeros
	ആകുമ്പോൾ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((അക്ഷരം *)prt_വരി_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_വരി_buffer);
    }
    sprintf((അക്ഷരം *)prt_വരി_buffer, " ");
    prt_write_file(prt_വരി_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    സ്ഥിതം ശൂന്യം
prt_def_var(അക്ഷരം *name, ഇരട്ട മൂല്യം, പൂർണ്ണസംഖ്യ prec)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
								"/%s ", name);
    prt_write_file(prt_വരി_buffer);
    prt_write_real(മൂല്യം, prec);
    sprintf((അക്ഷരം *)prt_വരി_buffer, "d\n");
    prt_write_file(prt_വരി_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    സ്ഥിതം ശൂന്യം
prt_flush_buffer(ശൂന്യം)
{
    എങ്കിൽ (prt_ps_buffer.ga_len <= 0)
	തിരികെ;

    // Any background color must be drawn first
    എങ്കിൽ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	പൂർണ്ണസംഖ്യ     r, g, b;

	എങ്കിൽ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_വരി_height, 2);

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
    എങ്കിൽ (prt_do_underline)
    {
	എങ്കിൽ (prt_do_moveto)
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
    എങ്കിൽ (prt_out_mbyte)
	prt_write_string("<");
    അല്ലെങ്കിൽ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    എങ്കിൽ (prt_out_mbyte)
	prt_write_string(">");
    അല്ലെങ്കിൽ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    എങ്കിൽ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    അല്ലെങ്കിൽ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, വലുപ്പം(അക്ഷരം), prt_bufsiz);
}


    സ്ഥിതം ശൂന്യം
prt_resource_name(char_u *filename, ശൂന്യം *cookie)
{
    char_u *resource_filename = cookie;

    എങ്കിൽ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    അല്ലെങ്കിൽ
	STRCPY(resource_filename, filename);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_find_resource(അക്ഷരം *name, ഘടന prt_ps_resource_S *resource)
{
    char_u	*buffer;
    പൂർണ്ണസംഖ്യ		retval;

    buffer = alloc(MAXPATHL + 1);
    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ FALSE;

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
    തിരികെ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
ഘടന prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    പൂർണ്ണസംഖ്യ     len;
    പൂർണ്ണസംഖ്യ     വരി_start;
    പൂർണ്ണസംഖ്യ     വരി_end;
};

സ്ഥിതം ഘടന prt_resfile_buffer_S prt_resfile;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_resfile_next_വരി(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.വരി_end + 1;
    ആകുമ്പോൾ (idx < prt_resfile.len)
    {
	എങ്കിൽ (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    നിർത്തുക;
	idx++;
    }
    prt_resfile.വരി_start = idx;

    ആകുമ്പോൾ (idx < prt_resfile.len)
    {
	എങ്കിൽ (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    നിർത്തുക;
	idx++;
    }
    prt_resfile.വരി_end = idx;

    തിരികെ (idx < prt_resfile.len);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_resfile_strncmp(പൂർണ്ണസംഖ്യ offset, അക്ഷരം *string, പൂർണ്ണസംഖ്യ len)
{
    // Force not equal if string is longer than remainder of line
    എങ്കിൽ (len > (prt_resfile.വരി_end - (prt_resfile.വരി_start + offset)))
	തിരികെ 1;

    തിരികെ STRNCMP(&prt_resfile.buffer[prt_resfile.വരി_start + offset],
								string, len);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_resfile_skip_nonws(പൂർണ്ണസംഖ്യ offset)
{
    പൂർണ്ണസംഖ്യ     idx;

    idx = prt_resfile.വരി_start + offset;
    ആകുമ്പോൾ (idx < prt_resfile.വരി_end)
    {
	എങ്കിൽ (SAFE_isspace(prt_resfile.buffer[idx]))
	    തിരികെ idx - prt_resfile.വരി_start;
	idx++;
    }
    തിരികെ -1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_resfile_skip_ws(പൂർണ്ണസംഖ്യ offset)
{
    പൂർണ്ണസംഖ്യ     idx;

    idx = prt_resfile.വരി_start + offset;
    ആകുമ്പോൾ (idx < prt_resfile.വരി_end)
    {
	എങ്കിൽ (!SAFE_isspace(prt_resfile.buffer[idx]))
	    തിരികെ idx - prt_resfile.വരി_start;
	idx++;
    }
    തിരികെ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_next_dsc(ഘടന prt_dsc_വരി_S *p_dsc_വരി)
{
    പൂർണ്ണസംഖ്യ     comment;
    പൂർണ്ണസംഖ്യ     offset;

    // Move to start of next line
    എങ്കിൽ (!prt_resfile_next_വരി())
	തിരികെ FALSE;

    // DSC comments always start %%
    എങ്കിൽ (prt_resfile_strncmp(0, "%%", 2) != 0)
	തിരികെ FALSE;

    // Find type of DSC comment
    വേണ്ടി (comment = 0; comment < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(prt_dsc_table); comment++)
	എങ്കിൽ (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    നിർത്തുക;

    എങ്കിൽ (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_വരി->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    അല്ലെങ്കിൽ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_വരി->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	എങ്കിൽ (offset == -1)
	    തിരികെ FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    എങ്കിൽ (offset == -1)
	തിരികെ FALSE;

    p_dsc_വരി->string = &prt_resfile.buffer[prt_resfile.വരി_start + offset];
    p_dsc_വരി->len = prt_resfile.വരി_end - (prt_resfile.വരി_start + offset);

    തിരികെ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_open_resource(ഘടന prt_ps_resource_S *resource)
{
    പൂർണ്ണസംഖ്യ		offset;
    പൂർണ്ണസംഖ്യ		seen_all;
    പൂർണ്ണസംഖ്യ		seen_title;
    പൂർണ്ണസംഖ്യ		seen_version;
    FILE	*fd_resource;
    ഘടന prt_dsc_വരി_S dsc_വരി;

    fd_resource = mch_fopen((അക്ഷരം *)resource->filename, READBIN);
    എങ്കിൽ (fd_resource == ശൂന്യ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	തിരികെ FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (പൂർണ്ണസംഖ്യ)fread((അക്ഷരം *)prt_resfile.buffer, വലുപ്പം(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    എങ്കിൽ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	തിരികെ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.വരി_end = -1;
    prt_resfile.വരി_start = 0;
    എങ്കിൽ (!prt_resfile_next_വരി())
	തിരികെ FALSE;

    offset = 0;

    എങ്കിൽ (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	തിരികെ FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    എങ്കിൽ (offset == -1)
	തിരികെ FALSE;
    offset = prt_resfile_skip_ws(offset);
    എങ്കിൽ (offset == -1)
	തിരികെ FALSE;

    എങ്കിൽ (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	തിരികെ FALSE;
    }
    offset += (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    എങ്കിൽ (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    അല്ലെങ്കിൽ എങ്കിൽ (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    അല്ലെങ്കിൽ എങ്കിൽ (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    അല്ലെങ്കിൽ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	തിരികെ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    ആകുമ്പോൾ (!seen_all && prt_next_dsc(&dsc_വരി))
    {
	തിരഞ്ഞെടുപ്പ് (dsc_വരി.type)
	{
	അവസ്ഥ PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_വരി.string, dsc_വരി.len);
	    seen_title = TRUE;
	    എങ്കിൽ (seen_version)
		seen_all = TRUE;
	    നിർത്തുക;

	അവസ്ഥ PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_വരി.string, dsc_വരി.len);
	    seen_version = TRUE;
	    എങ്കിൽ (seen_title)
		seen_all = TRUE;
	    നിർത്തുക;

	അവസ്ഥ PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    നിർത്തുക;

	അവസ്ഥ PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_check_resource(ഘടന prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    എങ്കിൽ (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	തിരികെ FALSE;
    }

    // Other checks to be added as needed
    തിരികെ TRUE;
}

    സ്ഥിതം ശൂന്യം
prt_dsc_start(ശൂന്യം)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    സ്ഥിതം ശൂന്യം
prt_dsc_noarg(അക്ഷരം *comment)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_വരി_buffer);
}

    സ്ഥിതം ശൂന്യം
prt_dsc_textline(അക്ഷരം *comment, അക്ഷരം *text)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_വരി_buffer);
}

    സ്ഥിതം ശൂന്യം
prt_dsc_text(അക്ഷരം *comment, അക്ഷരം *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_വരി_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    സ്ഥിതം ശൂന്യം
prt_dsc_ints(അക്ഷരം *comment, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ *ints)
{
    പൂർണ്ണസംഖ്യ		i;

    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_വരി_buffer);

    വേണ്ടി (i = 0; i < എണ്ണം; i++)
    {
	sprintf((അക്ഷരം *)prt_വരി_buffer, " %d", ints[i]);
	prt_write_file(prt_വരി_buffer);
    }

    prt_write_string("\n");
}

    സ്ഥിതം ശൂന്യം
prt_dsc_resources(
    അക്ഷരം	*comment,	// if NULL add to previous
    അക്ഷരം	*type,
    അക്ഷരം	*string)
{
    എങ്കിൽ (comment != ശൂന്യ)
	vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
						 "%%%%%s: %s", comment, type);
    അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_വരി_buffer);

    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
							     " %s\n", string);
    prt_write_file(prt_വരി_buffer);
}

    സ്ഥിതം ശൂന്യം
prt_dsc_font_resource(അക്ഷരം *resource, ഘടന prt_ps_font_S *ps_font)
{
    പൂർണ്ണസംഖ്യ     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    വേണ്ടി (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	എങ്കിൽ (ps_font->ps_fontname[i] != ശൂന്യ)
	    prt_dsc_resources(ശൂന്യ, "font", ps_font->ps_fontname[i]);
}

    സ്ഥിതം ശൂന്യം
prt_dsc_requirements(
    പൂർണ്ണസംഖ്യ		duplex,
    പൂർണ്ണസംഖ്യ		tumble,
    പൂർണ്ണസംഖ്യ		collate,
    പൂർണ്ണസംഖ്യ		color,
    പൂർണ്ണസംഖ്യ		അക്കം_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    എങ്കിൽ (!(duplex || collate || color || (അക്കം_copies > 1)))
	തിരികെ;

    sprintf((അക്ഷരം *)prt_വരി_buffer, "%%%%Requirements:");
    prt_write_file(prt_വരി_buffer);

    എങ്കിൽ (duplex)
    {
	prt_write_string(" duplex");
	എങ്കിൽ (tumble)
	    prt_write_string("(tumble)");
    }
    എങ്കിൽ (collate)
	prt_write_string(" collate");
    എങ്കിൽ (color)
	prt_write_string(" color");
    എങ്കിൽ (അക്കം_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((അക്ഷരം *)prt_വരി_buffer, "%d", അക്കം_copies);
	prt_write_file(prt_വരി_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    സ്ഥിതം ശൂന്യം
prt_dsc_docmedia(
    അക്ഷരം	*paper_name,
    ഇരട്ട	width,
    ഇരട്ട	height,
    ഇരട്ട	weight,
    അക്ഷരം	*colour,
    അക്ഷരം	*type)
{
    vim_snprintf((അക്ഷരം *)prt_വരി_buffer, വലുപ്പം(prt_വരി_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_വരി_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    എങ്കിൽ (colour == ശൂന്യ)
	prt_write_string("()");
    അല്ലെങ്കിൽ
	prt_write_string(colour);
    prt_write_string(" ");
    എങ്കിൽ (type == ശൂന്യ)
	prt_write_string("()");
    അല്ലെങ്കിൽ
	prt_write_string(type);
    prt_write_string("\n");
}

    ശൂന്യം
mch_print_cleanup(ശൂന്യം)
{
    എങ്കിൽ (prt_out_mbyte)
    {
	പൂർണ്ണസംഖ്യ     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	വേണ്ടി (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    എങ്കിൽ (prt_ps_mb_font.ps_fontname[i] != ശൂന്യ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    എങ്കിൽ (prt_do_conv)
    {
	convert_setup(&prt_conv, ശൂന്യ, ശൂന്യ);
	prt_do_conv = FALSE;
    }
    എങ്കിൽ (prt_ps_fd != ശൂന്യ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ശൂന്യ;
	prt_file_error = FALSE;
    }
    എങ്കിൽ (prt_ps_file_name != ശൂന്യ)
	VIM_CLEAR(prt_ps_file_name);
}

    സ്ഥിതം ദശാംശം
to_device_units(പൂർണ്ണസംഖ്യ idx, ഇരട്ട physsize, പൂർണ്ണസംഖ്യ def_സംഖ്യ)
{
    ദശാംശം	ret;
    പൂർണ്ണസംഖ്യ		u;
    പൂർണ്ണസംഖ്യ		nr;

    u = prt_get_unit(idx);
    എങ്കിൽ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_സംഖ്യ;
    }
    അല്ലെങ്കിൽ
	nr = printer_opts[idx].സംഖ്യ;

    തിരഞ്ഞെടുപ്പ് (u)
    {
	അവസ്ഥ PRT_UNIT_INCH:
	    ret = (ദശാംശം)(nr * PRT_PS_DEFAULT_DPI);
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_MM:
	    ret = (ദശാംശം)(nr * PRT_PS_DEFAULT_DPI) / (ദശാംശം)25.4;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_POINT:
	    ret = (ദശാംശം)nr;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_PERC:
	സ്ഥിരസ്ഥിതി:
	    ret = (ദശാംശം)(physsize * nr) / 100;
	    നിർത്തുക;
    }

    തിരികെ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    സ്ഥിതം ശൂന്യം
prt_page_margins(
    ഇരട്ട	width,
    ഇരട്ട	height,
    ഇരട്ട	*ഇടത്,
    ഇരട്ട	*വലത്,
    ഇരട്ട	*മുകൾ,
    ഇരട്ട	*bottom)
{
    *ഇടത്   = to_device_units(OPT_PRINT_ഇടത്, width, 10);
    *വലത്  = width - to_device_units(OPT_PRINT_വലത്, width, 5);
    *മുകൾ    = height - to_device_units(OPT_PRINT_മുകൾ, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    സ്ഥിതം ശൂന്യം
prt_font_metrics(പൂർണ്ണസംഖ്യ font_scale)
{
    prt_വരി_height = (ദശാംശം)font_scale;
    prt_char_width = (ദശാംശം)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_get_cpl(ശൂന്യം)
{
    എങ്കിൽ (prt_use_സംഖ്യ())
    {
	prt_സംഖ്യ_width = PRINT_സംഖ്യ_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	എങ്കിൽ (prt_out_mbyte)
	    prt_സംഖ്യ_width /= 2;
	prt_ഇടത്_margin += prt_സംഖ്യ_width;
    }
    അല്ലെങ്കിൽ
	prt_സംഖ്യ_width = 0.0;

    തിരികെ (പൂർണ്ണസംഖ്യ)((prt_വലത്_margin - prt_ഇടത്_margin) / prt_char_width);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_build_cid_fontname(പൂർണ്ണസംഖ്യ font, char_u *name, പൂർണ്ണസംഖ്യ name_len)
{
    അക്ഷരം    *fontname;

    fontname = alloc(name_len + 1);
    എങ്കിൽ (fontname == ശൂന്യ)
	തിരികെ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    തിരികെ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_get_lpp(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (ദശാംശം)PRT_PS_FONT_TO_USER(prt_വരി_height,
					   prt_ps_font->bbox_min_y);
    എങ്കിൽ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (ദശാംശം)PRT_PS_FONT_TO_USER(prt_വരി_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_വരി_height = prt_വരി_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (പൂർണ്ണസംഖ്യ)((prt_മുകൾ_margin - prt_bottom_margin) / prt_വരി_height);

    // Adjust top margin if there is a header
    prt_മുകൾ_margin -= prt_വരി_height * prt_header_height();

    തിരികെ lpp - prt_header_height();
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_match_encoding(
    അക്ഷരം			*p_encoding,
    ഘടന prt_ps_mbfont_S	*p_cmap,
    ഘടന prt_ps_encoding_S	**pp_mbenc)
{
    പൂർണ്ണസംഖ്യ				mbenc;
    പൂർണ്ണസംഖ്യ				enc_len;
    ഘടന prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ശൂന്യ;
    // Look for recognised encoding
    enc_len = (പൂർണ്ണസംഖ്യ)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    വേണ്ടി (mbenc = 0; mbenc < p_cmap->അക്കം_encodings; mbenc++)
    {
	എങ്കിൽ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    തിരികെ TRUE;
	}
	p_mbenc++;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_match_charset(
    അക്ഷരം		    *p_charset,
    ഘടന prt_ps_mbfont_S  *p_cmap,
    ഘടന prt_ps_charset_S **pp_mbchar)
{
    പൂർണ്ണസംഖ്യ			    mbchar;
    പൂർണ്ണസംഖ്യ			    char_len;
    ഘടന prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    എങ്കിൽ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (പൂർണ്ണസംഖ്യ)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    വേണ്ടി (mbchar = 0; mbchar < p_cmap->അക്കം_charsets; mbchar++)
    {
	എങ്കിൽ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    തിരികെ TRUE;
	}
	p_mbchar++;
    }
    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    പൂർണ്ണസംഖ്യ		forceit UNUSED)
{
    പൂർണ്ണസംഖ്യ		i;
    അക്ഷരം	*paper_name;
    പൂർണ്ണസംഖ്യ		paper_strlen;
    പൂർണ്ണസംഖ്യ		fontsize;
    char_u	*p;
    ഇരട്ട      ഇടത്;
    ഇരട്ട      വലത്;
    ഇരട്ട      മുകൾ;
    ഇരട്ട      bottom;
    പൂർണ്ണസംഖ്യ		props;
    പൂർണ്ണസംഖ്യ		cmap = 0;
    char_u	*p_encoding;
    ഘടന prt_ps_encoding_S *p_mbenc;
    ഘടന prt_ps_encoding_S *p_mbenc_first;
    ഘടന prt_ps_charset_S  *p_mbchar = ശൂന്യ;

# എങ്കിൽ 0
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
    എങ്കിൽ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    എങ്കിൽ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ശൂന്യ;
    props = enc_canon_props(p_encoding);
    എങ്കിൽ (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	പൂർണ്ണസംഖ്യ cmap_first = 0;

	p_mbenc_first = ശൂന്യ;
	വേണ്ടി (cmap = 0; cmap < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    എങ്കിൽ (prt_match_encoding((അക്ഷരം *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		എങ്കിൽ (p_mbenc_first == ശൂന്യ)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		എങ്കിൽ (prt_match_charset((അക്ഷരം *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    നിർത്തുക;
	    }

	// Use first encoding matched if no charset matched
	എങ്കിൽ (p_mbchar == ശൂന്യ && p_mbenc_first != ശൂന്യ)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != ശൂന്യ);
    എങ്കിൽ (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ശൂന്യ);
	എങ്കിൽ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    എങ്കിൽ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		തിരികെ FALSE;
	    }

	    // Add charset name if not empty
	    എങ്കിൽ (p_mbchar->cmap_charset != ശൂന്യ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, വലുപ്പം(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Add custom CMap character set name
	    എങ്കിൽ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		തിരികെ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, വലുപ്പം(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	എങ്കിൽ (p_mbenc->cmap_encoding != ശൂന്യ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < വലുപ്പം(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	എങ്കിൽ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    തിരികെ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	എങ്കിൽ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    തിരികെ FALSE;
	എങ്കിൽ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    എങ്കിൽ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		തിരികെ FALSE;
	എങ്കിൽ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    എങ്കിൽ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		തിരികെ FALSE;
	എങ്കിൽ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    എങ്കിൽ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		തിരികെ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	എങ്കിൽ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    എങ്കിൽ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    അല്ലെങ്കിൽ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    അല്ലെങ്കിൽ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    എങ്കിൽ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (അക്ഷരം *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    അല്ലെങ്കിൽ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)PRT_MEDIASIZE_LEN; ++i)
	എങ്കിൽ (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    നിർത്തുക;
    എങ്കിൽ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    എങ്കിൽ (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    അല്ലെങ്കിൽ
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &ഇടത്, &വലത്, &മുകൾ,
								    &bottom);
    prt_ഇടത്_margin = (ദശാംശം)ഇടത്;
    prt_വലത്_margin = (ദശാംശം)വലത്;
    prt_മുകൾ_margin = (ദശാംശം)മുകൾ;
    prt_bottom_margin = (ദശാംശം)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    വേണ്ടി (p = p_pfn; (p = vim_strchr(p, ':')) != ശൂന്യ; ++p)
	എങ്കിൽ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((അക്ഷരം *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_വരി = prt_get_cpl();
    psettings->വരികൾ_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    എങ്കിൽ (psettings->chars_per_വരി <= 0 || psettings->വരികൾ_per_page <= 0)
	തിരികെ FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_അക്കം_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    എങ്കിൽ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    അല്ലെങ്കിൽ
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_അക്കം_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    എങ്കിൽ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	എങ്കിൽ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    എങ്കിൽ (psettings->outfile == ശൂന്യ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	എങ്കിൽ (prt_ps_file_name == ശൂന്യ)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    തിരികെ FAIL;
	}
	prt_ps_fd = mch_fopen((അക്ഷരം *)prt_ps_file_name, WRITEBIN);
    }
    അല്ലെങ്കിൽ
    {
	p = expand_env_save(psettings->outfile);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    prt_ps_fd = mch_fopen((അക്ഷരം *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    എങ്കിൽ (prt_ps_fd == ശൂന്യ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	തിരികെ FAIL;
    }

    prt_bufsiz = psettings->chars_per_വരി;
    എങ്കിൽ (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, വലുപ്പം(അക്ഷരം), prt_bufsiz);

    prt_page_അക്കം = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_add_resource(ഘടന prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((അക്ഷരം *)resource->filename, READBIN);
    എങ്കിൽ (fd_resource == ശൂന്യ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	തിരികെ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (അക്ഷരം *)resource->title);

    prt_dsc_textline("BeginDocument", (അക്ഷരം *)resource->filename);

    വേണ്ടി (;;)
    {
	bytes_read = fread((അക്ഷരം *)resource_buffer, വലുപ്പം(char_u),
			   വലുപ്പം(resource_buffer), fd_resource);
	എങ്കിൽ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    തിരികെ FALSE;
	}
	എങ്കിൽ (bytes_read == 0)
	    നിർത്തുക;
	prt_write_file_raw_len(resource_buffer, (പൂർണ്ണസംഖ്യ)bytes_read);
	എങ്കിൽ (prt_file_error)
	{
	    fclose(fd_resource);
	    തിരികെ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    തിരികെ TRUE;
}

    പൂർണ്ണസംഖ്യ
mch_print_begin(prt_settings_T *psettings)
{
    പൂർണ്ണസംഖ്യ		bbox[4];
    ഇരട്ട      ഇടത്;
    ഇരട്ട      വലത്;
    ഇരട്ട      മുകൾ;
    ഇരട്ട      bottom;
    ഘടന prt_ps_resource_S *res_prolog;
    ഘടന prt_ps_resource_S *res_encoding;
    അക്ഷരം	buffer[256];
    char_u      *p_encoding;
    ഘടന prt_ps_resource_S *res_cidfont;
    ഘടന prt_ps_resource_S *res_cmap;
    പൂർണ്ണസംഖ്യ		retval = FALSE;

    res_prolog = ALLOC_ONE(ഘടന prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ഘടന prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ഘടന prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ഘടന prt_ps_resource_S);
    എങ്കിൽ (res_prolog == ശൂന്യ || res_encoding == ശൂന്യ
	    || res_cidfont == ശൂന്യ || res_cmap == ശൂന്യ)
	പോകുക theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (അക്ഷരം *)psettings->jobname);
    എങ്കിൽ (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(ശൂന്യ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &ഇടത്, &വലത്, &മുകൾ, &bottom);
    bbox[0] = (പൂർണ്ണസംഖ്യ)ഇടത്;
    എങ്കിൽ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (പൂർണ്ണസംഖ്യ)(മുകൾ - (psettings->വരികൾ_per_page + prt_header_height())
						    * (ഇരട്ട)prt_വരി_height);
	bbox[2] = (പൂർണ്ണസംഖ്യ)(ഇടത് + psettings->chars_per_വരി
					       * (ഇരട്ട)prt_char_width + 0.5);
	bbox[3] = (പൂർണ്ണസംഖ്യ)(മുകൾ + 0.5);
    }
    അല്ലെങ്കിൽ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (പൂർണ്ണസംഖ്യ)bottom;
	bbox[2] = (പൂർണ്ണസംഖ്യ)(ഇടത് + ((psettings->വരികൾ_per_page
			      + prt_header_height()) * prt_വരി_height) + 0.5);
	bbox[3] = (പൂർണ്ണസംഖ്യ)(bottom + psettings->chars_per_വരി
					       * (ഇരട്ട)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(ഇരട്ട)0, ശൂന്യ, ശൂന്യ);
    // Define fonts needed
    എങ്കിൽ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    എങ്കിൽ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ശൂന്യ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	എങ്കിൽ (!prt_custom_cmap)
	    prt_dsc_resources(ശൂന്യ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    എങ്കിൽ (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	പോകുക theend;
    }
    എങ്കിൽ (!prt_open_resource(res_prolog))
	പോകുക theend;
    എങ്കിൽ (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	പോകുക theend;
    എങ്കിൽ (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	എങ്കിൽ (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    പോകുക theend;
	}
	എങ്കിൽ (!prt_open_resource(res_cidfont))
	    പോകുക theend;
	എങ്കിൽ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    പോകുക theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    എങ്കിൽ (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	എങ്കിൽ (*p_encoding == NUL
		|| !prt_find_resource((അക്ഷരം *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    പൂർണ്ണസംഖ്യ		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    എങ്കിൽ (!(props & ENC_8BIT)
		    || !prt_find_resource((അക്ഷരം *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		എങ്കിൽ (!prt_find_resource((അക്ഷരം *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    പോകുക theend;
		}
	    }
	}
	എങ്കിൽ (!prt_open_resource(res_encoding))
	    പോകുക theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    അല്ലെങ്കിൽ
    {
	p_encoding = enc_skip(p_penc);
	എങ്കിൽ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	എങ്കിൽ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    എങ്കിൽ (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		പോകുക theend;
	    }
	    എങ്കിൽ (!prt_open_resource(res_encoding))
		പോകുക theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    എങ്കിൽ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	എങ്കിൽ (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    പോകുക theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    എങ്കിൽ (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	എങ്കിൽ (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    പോകുക theend;
	}
	എങ്കിൽ (!prt_open_resource(res_cmap))
	    പോകുക theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    എങ്കിൽ (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(ശൂന്യ, "procset", buffer);

	എങ്കിൽ (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(ശൂന്യ, "cmap", buffer);
	}
    }
    എങ്കിൽ (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(ശൂന്യ, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# അല്ലെങ്കിൽ
					0
# endif
					, prt_അക്കം_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    എങ്കിൽ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    എങ്കിൽ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ശൂന്യ : "PageResources"),
							    &prt_ps_mb_font);
	എങ്കിൽ (!prt_custom_cmap)
	    prt_dsc_resources(ശൂന്യ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    എങ്കിൽ (!prt_add_resource(res_prolog))
	പോകുക theend;
    എങ്കിൽ (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	എങ്കിൽ (!prt_add_resource(res_cidfont))
	    പോകുക theend;
	എങ്കിൽ (prt_custom_cmap && !prt_add_resource(res_cmap))
	    പോകുക theend;
    }

    എങ്കിൽ (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	എങ്കിൽ (!prt_add_resource(res_encoding))
	    പോകുക theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((പൂർണ്ണസംഖ്യ)prt_mediasize[prt_media].width);
    prt_write_int((പൂർണ്ണസംഖ്യ)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_അക്കം_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    എങ്കിൽ (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	എങ്കിൽ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (അക്ഷരം *)p_encoding, (പൂർണ്ണസംഖ്യ)prt_വരി_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (അക്ഷരം *)p_encoding, (പൂർണ്ണസംഖ്യ)prt_വരി_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (അക്ഷരം *)p_encoding, (പൂർണ്ണസംഖ്യ)prt_വരി_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (അക്ഷരം *)p_encoding, (പൂർണ്ണസംഖ്യ)prt_വരി_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    എങ്കിൽ (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	എങ്കിൽ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (പൂർണ്ണസംഖ്യ)prt_വരി_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	എങ്കിൽ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ശൂന്യ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    എങ്കിൽ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (പൂർണ്ണസംഖ്യ)prt_വരി_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	അല്ലെങ്കിൽ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	എങ്കിൽ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ശൂന്യ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    എങ്കിൽ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (പൂർണ്ണസംഖ്യ)prt_വരി_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	അല്ലെങ്കിൽ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	എങ്കിൽ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ശൂന്യ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    എങ്കിൽ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (പൂർണ്ണസംഖ്യ)prt_വരി_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	അല്ലെങ്കിൽ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_വരി_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_വരി_height,
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

    തിരികെ retval;
}

    ശൂന്യം
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_അക്കം);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    എങ്കിൽ (!prt_file_error && psettings->outfile == ശൂന്യ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	എങ്കിൽ (prt_ps_fd != ശൂന്യ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ശൂന്യ;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	എങ്കിൽ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	അല്ലെങ്കിൽ
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    പൂർണ്ണസംഖ്യ
mch_print_end_page(ശൂന്യം)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    തിരികെ !prt_file_error;
}

    പൂർണ്ണസംഖ്യ
mch_print_begin_page(char_u *str UNUSED)
{
    പൂർണ്ണസംഖ്യ		page_അക്കം[2];

    prt_page_അക്കം++;

    page_അക്കം[0] = page_അക്കം[1] = prt_page_അക്കം;
    prt_dsc_ints("Page", 2, page_അക്കം);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    എങ്കിൽ (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    അല്ലെങ്കിൽ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    എങ്കിൽ (!prt_portrait)
    {
	prt_write_int(-((പൂർണ്ണസംഖ്യ)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    തിരികെ !prt_file_error;
}

    പൂർണ്ണസംഖ്യ
mch_print_blank_page(ശൂന്യം)
{
    തിരികെ (mch_print_begin_page(ശൂന്യ) ? (mch_print_end_page()) : FALSE);
}

സ്ഥിതം ദശാംശം prt_pos_x = 0;
സ്ഥിതം ദശാംശം prt_pos_y = 0;

    ശൂന്യം
mch_print_start_വരി(പൂർണ്ണസംഖ്യ margin, പൂർണ്ണസംഖ്യ page_വരി)
{
    prt_pos_x = prt_ഇടത്_margin;
    എങ്കിൽ (margin)
	prt_pos_x -= prt_സംഖ്യ_width;

    prt_pos_y = prt_മുകൾ_margin - prt_first_വരി_height -
					page_വരി * prt_വരി_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    പൂർണ്ണസംഖ്യ
mch_print_text_out(char_u *textp, പൂർണ്ണസംഖ്യ len UNUSED)
{
    char_u	*p = textp;
    പൂർണ്ണസംഖ്യ		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    ദശാംശം       char_width;
    ദശാംശം       next_pos;
    പൂർണ്ണസംഖ്യ		in_ascii;
    പൂർണ്ണസംഖ്യ		half_width;
    char_u	*tofree = ശൂന്യ;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    എങ്കിൽ (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	എങ്കിൽ (prt_in_ascii)
	{
	    എങ്കിൽ (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    എങ്കിൽ (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	എങ്കിൽ (half_width)
	    char_width /= 2;
	എങ്കിൽ (prt_half_width)
	{
	    എങ്കിൽ (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    എങ്കിൽ (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	എങ്കിൽ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	എങ്കിൽ (prt_need_font)
	{
	    എങ്കിൽ (!prt_in_ascii)
		prt_write_string("CF");
	    അല്ലെങ്കിൽ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	എങ്കിൽ (prt_need_fgcol)
	{
	    പൂർണ്ണസംഖ്യ     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    എങ്കിൽ (r == g && g == b)
		prt_write_string("g\n");
	    അല്ലെങ്കിൽ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	എങ്കിൽ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    എങ്കിൽ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	അല്ലെങ്കിൽ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	എങ്കിൽ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    എങ്കിൽ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    എങ്കിൽ (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ആകുമ്പോൾ (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	എങ്കിൽ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    തിരഞ്ഞെടുപ്പ് (ch)
	    {
		അവസ്ഥ BS:   ga_append(&prt_ps_buffer, 'b'); നിർത്തുക;
		അവസ്ഥ TAB:  ga_append(&prt_ps_buffer, 't'); നിർത്തുക;
		അവസ്ഥ NL:   ga_append(&prt_ps_buffer, 'n'); നിർത്തുക;
		അവസ്ഥ FF:   ga_append(&prt_ps_buffer, 'f'); നിർത്തുക;
		അവസ്ഥ CAR:  ga_append(&prt_ps_buffer, 'r'); നിർത്തുക;
		അവസ്ഥ '(':  ga_append(&prt_ps_buffer, '('); നിർത്തുക;
		അവസ്ഥ ')':  ga_append(&prt_ps_buffer, ')'); നിർത്തുക;
		അവസ്ഥ '\\': ga_append(&prt_ps_buffer, '\\'); നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			   sprintf((അക്ഷരം *)ch_buff, "%03o", (unsigned പൂർണ്ണസംഖ്യ)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_വലത്_margin) &&
		    ((next_pos - prt_വലത്_margin) > (prt_വലത്_margin*1e-5));

    എങ്കിൽ (need_break)
	prt_flush_buffer();

    തിരികെ need_break;
}

    ശൂന്യം
mch_print_set_font(പൂർണ്ണസംഖ്യ iBold, പൂർണ്ണസംഖ്യ iItalic, പൂർണ്ണസംഖ്യ iUnderline)
{
    പൂർണ്ണസംഖ്യ		font = 0;

    എങ്കിൽ (iBold)
	font |= 0x01;
    എങ്കിൽ (iItalic)
	font |= 0x02;

    എങ്കിൽ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    എങ്കിൽ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    ശൂന്യം
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (പൂർണ്ണസംഖ്യ)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    ശൂന്യം
mch_print_set_fg(long_u fgcol)
{
    എങ്കിൽ (fgcol == (long_u)prt_fgcol)
	തിരികെ;

    prt_fgcol = (പൂർണ്ണസംഖ്യ)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
