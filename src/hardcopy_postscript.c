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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_ಅಂಕೆ_OPTIONS] =
{
    {"c",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
    {"a",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
    {"r",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
    {"b",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
    {"i",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
    {"o",	FALSE, 0, ರಿಕ್ತ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ಸಂರಚನಾ prt_ps_font_S
{
    ಪೂರ್ಣಾಙ್ಕ		wx;
    ಪೂರ್ಣಾಙ್ಕ		uline_offset;
    ಪೂರ್ಣಾಙ್ಕ		uline_width;
    ಪೂರ್ಣಾಙ್ಕ		bbox_min_y;
    ಪೂರ್ಣಾಙ್ಕ		bbox_max_y;
    ವರ್ಣ	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ}
};

// Pointer to current font set being used
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ಸಂರಚನಾ prt_ps_encoding_S
{
    ವರ್ಣ	*encoding;
    ವರ್ಣ	*cmap_encoding;
    ಪೂರ್ಣಾಙ್ಕ		needs_charset;
};

ಸಂರಚನಾ prt_ps_charset_S
{
    ವರ್ಣ	*charset;
    ವರ್ಣ	*cmap_charset;
    ಪೂರ್ಣಾಙ್ಕ		has_charset;
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
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ರಿಕ್ತ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ರಿಕ್ತ,       CS_JIS_X_1983},
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
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ರಿಕ್ತ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ರಿಕ್ತ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_charset_S sc_charsets[] =
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
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ರಿಕ್ತ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ರಿಕ್ತ,	CS_B5},
    {"CP950",	    ರಿಕ್ತ,	CS_B5},
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
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ರಿಕ್ತ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_charset_S k_charsets[] =
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
ಸಂರಚನಾ prt_ps_mbfont_S
{
    ಪೂರ್ಣಾಙ್ಕ				ಅಂಕೆ_encodings;
    ಸಂರಚನಾ prt_ps_encoding_S	*encodings;
    ಪೂರ್ಣಾಙ್ಕ				ಅಂಕೆ_charsets;
    ಸಂರಚನಾ prt_ps_charset_S	*charsets;
    ವರ್ಣ			*ascii_enc;
    ವರ್ಣ			*defcs;
};

ಸ್ಥಿರ ಸಂರಚನಾ prt_ps_mbfont_S prt_ps_mbfonts[] =
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

ಸಂರಚನಾ prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    ಪೂರ್ಣಾಙ್ಕ     type;
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
ಸ್ಥಿರ ವರ್ಣ *prt_resource_types[] =
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

ಸಂರಚನಾ prt_dsc_comment_S
{
    ವರ್ಣ	*string;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		type;
};

ಸಂರಚನಾ prt_dsc_ಸಾಲು_S
{
    ಪೂರ್ಣಾಙ್ಕ		type;
    char_u	*string;
    ಪೂರ್ಣಾಙ್ಕ		len;
};


# define SIZEOF_CSTR(s)      (ಪರಿಮಾಣ(s) - 1)
ಸ್ಥಿರ ಸಂರಚನಾ prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ಸ್ಥಿರ ಶೂನ್ಯ prt_write_file_len(char_u *buffer, ಪೂರ್ಣಾಙ್ಕ bytes);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_next_dsc(ಸಂರಚನಾ prt_dsc_ಸಾಲು_S *p_dsc_ಸಾಲು);

/*
 * Variables for the output PostScript file.
 */
ಸ್ಥಿರ FILE *prt_ps_fd;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_file_error;
ಸ್ಥಿರ char_u *prt_ps_file_name = ರಿಕ್ತ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ಸ್ಥಿರ ಭಿನ್ನ prt_page_width;
ಸ್ಥಿರ ಭಿನ್ನ prt_page_height;
ಸ್ಥಿರ ಭಿನ್ನ prt_ಎಡ_margin;
ಸ್ಥಿರ ಭಿನ್ನ prt_ಬಲ_margin;
ಸ್ಥಿರ ಭಿನ್ನ prt_ತುದಿ_margin;
ಸ್ಥಿರ ಭಿನ್ನ prt_bottom_margin;
ಸ್ಥಿರ ಭಿನ್ನ prt_ಸಾಲು_height;
ಸ್ಥಿರ ಭಿನ್ನ prt_first_ಸಾಲು_height;
ಸ್ಥಿರ ಭಿನ್ನ prt_char_width;
ಸ್ಥಿರ ಭಿನ್ನ prt_ಸಂಖ್ಯೆ_width;
ಸ್ಥಿರ ಭಿನ್ನ prt_bgcol_offset;
ಸ್ಥಿರ ಭಿನ್ನ prt_pos_x_moveto = 0.0;
ಸ್ಥಿರ ಭಿನ್ನ prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_need_moveto;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_do_moveto;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_need_font;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_font;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_need_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_do_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_need_fgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_fgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_need_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_do_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_new_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_attribute_change;
ಸ್ಥಿರ ಭಿನ್ನ prt_text_run;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_page_ಅಂಕೆ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_media;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_portrait;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_ಅಂಕೆ_copies;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_duplex;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_tumble;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ಸ್ಥಿರ char_u prt_ಸಾಲು_buffer[257];
ಸ್ಥಿರ garray_T prt_ps_buffer;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_do_conv;
ಸ್ಥಿರ vimconv_T prt_conv;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_out_mbyte;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_custom_cmap;
ಸ್ಥಿರ ವರ್ಣ prt_cmap[80];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_use_courier;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_in_ascii;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_half_width;
ಸ್ಥಿರ ವರ್ಣ *prt_ascii_encoding;
ಸ್ಥಿರ ಧ್ರುವ char_u prt_hexchar[] = "0123456789abcdef";

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file_raw_len(char_u *buffer, ಪೂರ್ಣಾಙ್ಕ bytes)
{
    ಯದಿ (!prt_file_error
	    && fwrite(buffer, ಪರಿಮಾಣ(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buffer));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file_len(char_u *buffer, ಪೂರ್ಣಾಙ್ಕ bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_string(ವರ್ಣ *s)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer), "%s", s);
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write an int and a space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_int(ಪೂರ್ಣಾಙ್ಕ i)
{
    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "%d ", i);
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write a boolean and a space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_boolean(ಪೂರ್ಣಾಙ್ಕ b)
{
    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_font(
    ವರ್ಣ	*new_name,
    ವರ್ಣ	*encoding,
    ಪೂರ್ಣಾಙ್ಕ		height,
    ವರ್ಣ	*font)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_ಸಾಲು_buffer);
    ಯದಿ (prt_out_mbyte)
	sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    ಅನ್ಯಥಾ
	vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write a line to define the CID font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_cidfont(ವರ್ಣ *new_name, ಪೂರ್ಣಾಙ್ಕ height, ವರ್ಣ *cidfont)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_ಸಾಲು_buffer);
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dup_cidfont(ವರ್ಣ *original_name, ವರ್ಣ *new_name)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_real_bits(
    ದ್ವಿಗುಣ      real,
    ಪೂರ್ಣಾಙ್ಕ		precision,
    ಪೂರ್ಣಾಙ್ಕ		*pinteger,
    ಪೂರ್ಣಾಙ್ಕ		*pfraction)
{
    ಪೂರ್ಣಾಙ್ಕ     i;
    ಪೂರ್ಣಾಙ್ಕ     integer;
    ಭಿನ್ನ   fraction;

    integer = (ಪೂರ್ಣಾಙ್ಕ)real;
    fraction = (ಭಿನ್ನ)(real - integer);
    ಯದಿ (real < (ದ್ವಿಗುಣ)integer)
	fraction = -fraction;
    ಪ್ರತಿ (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (ಪೂರ್ಣಾಙ್ಕ)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_real(ದ್ವಿಗುಣ val, ಪೂರ್ಣಾಙ್ಕ prec)
{
    ಪೂರ್ಣಾಙ್ಕ     integer;
    ಪೂರ್ಣಾಙ್ಕ     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "%d", integer);
    prt_write_file(prt_ಸಾಲು_buffer);
    // Only emit fraction if necessary
    ಯದಿ (fraction != 0)
    {
	// Remove any trailing zeros
	ಯಾವತ್ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_ಸಾಲು_buffer);
    }
    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, " ");
    prt_write_file(prt_ಸಾಲು_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_var(ವರ್ಣ *name, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ prec)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
								"/%s ", name);
    prt_write_file(prt_ಸಾಲು_buffer);
    prt_write_real(ಮೌಲ್ಯ, prec);
    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "d\n");
    prt_write_file(prt_ಸಾಲು_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_flush_buffer(ಶೂನ್ಯ)
{
    ಯದಿ (prt_ps_buffer.ga_len <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Any background color must be drawn first
    ಯದಿ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	ಪೂರ್ಣಾಙ್ಕ     r, g, b;

	ಯದಿ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_ಸಾಲು_height, 2);

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
    ಯದಿ (prt_do_underline)
    {
	ಯದಿ (prt_do_moveto)
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
    ಯದಿ (prt_out_mbyte)
	prt_write_string("<");
    ಅನ್ಯಥಾ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    ಯದಿ (prt_out_mbyte)
	prt_write_string(">");
    ಅನ್ಯಥಾ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ಯದಿ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ಅನ್ಯಥಾ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, ಪರಿಮಾಣ(ವರ್ಣ), prt_bufsiz);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
prt_resource_name(char_u *filename, ಶೂನ್ಯ *cookie)
{
    char_u *resource_filename = cookie;

    ಯದಿ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ಅನ್ಯಥಾ
	STRCPY(resource_filename, filename);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_find_resource(ವರ್ಣ *name, ಸಂರಚನಾ prt_ps_resource_S *resource)
{
    char_u	*buffer;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    buffer = alloc(MAXPATHL + 1);
    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

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
    ಪ್ರತ್ಯಾಗಮ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
ಸಂರಚನಾ prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    ಪೂರ್ಣಾಙ್ಕ     len;
    ಪೂರ್ಣಾಙ್ಕ     ಸಾಲು_start;
    ಪೂರ್ಣಾಙ್ಕ     ಸಾಲು_end;
};

ಸ್ಥಿರ ಸಂರಚನಾ prt_resfile_buffer_S prt_resfile;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_resfile_next_ಸಾಲು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.ಸಾಲು_end + 1;
    ಯಾವತ್ (idx < prt_resfile.len)
    {
	ಯದಿ (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    ಭಙ್ಗ;
	idx++;
    }
    prt_resfile.ಸಾಲು_start = idx;

    ಯಾವತ್ (idx < prt_resfile.len)
    {
	ಯದಿ (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    ಭಙ್ಗ;
	idx++;
    }
    prt_resfile.ಸಾಲು_end = idx;

    ಪ್ರತ್ಯಾಗಮ (idx < prt_resfile.len);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_resfile_strncmp(ಪೂರ್ಣಾಙ್ಕ offset, ವರ್ಣ *string, ಪೂರ್ಣಾಙ್ಕ len)
{
    // Force not equal if string is longer than remainder of line
    ಯದಿ (len > (prt_resfile.ಸಾಲು_end - (prt_resfile.ಸಾಲು_start + offset)))
	ಪ್ರತ್ಯಾಗಮ 1;

    ಪ್ರತ್ಯಾಗಮ STRNCMP(&prt_resfile.buffer[prt_resfile.ಸಾಲು_start + offset],
								string, len);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_resfile_skip_nonws(ಪೂರ್ಣಾಙ್ಕ offset)
{
    ಪೂರ್ಣಾಙ್ಕ     idx;

    idx = prt_resfile.ಸಾಲು_start + offset;
    ಯಾವತ್ (idx < prt_resfile.ಸಾಲು_end)
    {
	ಯದಿ (SAFE_isspace(prt_resfile.buffer[idx]))
	    ಪ್ರತ್ಯಾಗಮ idx - prt_resfile.ಸಾಲು_start;
	idx++;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_resfile_skip_ws(ಪೂರ್ಣಾಙ್ಕ offset)
{
    ಪೂರ್ಣಾಙ್ಕ     idx;

    idx = prt_resfile.ಸಾಲು_start + offset;
    ಯಾವತ್ (idx < prt_resfile.ಸಾಲು_end)
    {
	ಯದಿ (!SAFE_isspace(prt_resfile.buffer[idx]))
	    ಪ್ರತ್ಯಾಗಮ idx - prt_resfile.ಸಾಲು_start;
	idx++;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_next_dsc(ಸಂರಚನಾ prt_dsc_ಸಾಲು_S *p_dsc_ಸಾಲು)
{
    ಪೂರ್ಣಾಙ್ಕ     comment;
    ಪೂರ್ಣಾಙ್ಕ     offset;

    // Move to start of next line
    ಯದಿ (!prt_resfile_next_ಸಾಲು())
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // DSC comments always start %%
    ಯದಿ (prt_resfile_strncmp(0, "%%", 2) != 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Find type of DSC comment
    ಪ್ರತಿ (comment = 0; comment < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(prt_dsc_table); comment++)
	ಯದಿ (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    ಭಙ್ಗ;

    ಯದಿ (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_ಸಾಲು->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    ಅನ್ಯಥಾ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_ಸಾಲು->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	ಯದಿ (offset == -1)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    ಯದಿ (offset == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    p_dsc_ಸಾಲು->string = &prt_resfile.buffer[prt_resfile.ಸಾಲು_start + offset];
    p_dsc_ಸಾಲು->len = prt_resfile.ಸಾಲು_end - (prt_resfile.ಸಾಲು_start + offset);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_open_resource(ಸಂರಚನಾ prt_ps_resource_S *resource)
{
    ಪೂರ್ಣಾಙ್ಕ		offset;
    ಪೂರ್ಣಾಙ್ಕ		seen_all;
    ಪೂರ್ಣಾಙ್ಕ		seen_title;
    ಪೂರ್ಣಾಙ್ಕ		seen_version;
    FILE	*fd_resource;
    ಸಂರಚನಾ prt_dsc_ಸಾಲು_S dsc_ಸಾಲು;

    fd_resource = mch_fopen((ವರ್ಣ *)resource->filename, READBIN);
    ಯದಿ (fd_resource == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (ಪೂರ್ಣಾಙ್ಕ)fread((ವರ್ಣ *)prt_resfile.buffer, ಪರಿಮಾಣ(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    ಯದಿ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.ಸಾಲು_end = -1;
    prt_resfile.ಸಾಲು_start = 0;
    ಯದಿ (!prt_resfile_next_ಸಾಲು())
	ಪ್ರತ್ಯಾಗಮ FALSE;

    offset = 0;

    ಯದಿ (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    ಯದಿ (offset == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    offset = prt_resfile_skip_ws(offset);
    ಯದಿ (offset == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    offset += (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ಯದಿ (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ಅನ್ಯಥಾ ಯದಿ (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ಅನ್ಯಥಾ ಯದಿ (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    ಯಾವತ್ (!seen_all && prt_next_dsc(&dsc_ಸಾಲು))
    {
	ವಿಕಲ್ಪ (dsc_ಸಾಲು.type)
	{
	ಪಕ್ಷ PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_ಸಾಲು.string, dsc_ಸಾಲು.len);
	    seen_title = TRUE;
	    ಯದಿ (seen_version)
		seen_all = TRUE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_ಸಾಲು.string, dsc_ಸಾಲು.len);
	    seen_version = TRUE;
	    ಯದಿ (seen_title)
		seen_all = TRUE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_check_resource(ಸಂರಚನಾ prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    ಯದಿ (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Other checks to be added as needed
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_start(ಶೂನ್ಯ)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_noarg(ವರ್ಣ *comment)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_ಸಾಲು_buffer);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_textline(ವರ್ಣ *comment, ವರ್ಣ *text)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_ಸಾಲು_buffer);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_text(ವರ್ಣ *comment, ವರ್ಣ *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_ಸಾಲು_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_ints(ವರ್ಣ *comment, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ *ints)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_ಸಾಲು_buffer);

    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; i++)
    {
	sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, " %d", ints[i]);
	prt_write_file(prt_ಸಾಲು_buffer);
    }

    prt_write_string("\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_resources(
    ವರ್ಣ	*comment,	// if NULL add to previous
    ವರ್ಣ	*type,
    ವರ್ಣ	*string)
{
    ಯದಿ (comment != ರಿಕ್ತ)
	vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
						 "%%%%%s: %s", comment, type);
    ಅನ್ಯಥಾ
	vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_ಸಾಲು_buffer);

    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
							     " %s\n", string);
    prt_write_file(prt_ಸಾಲು_buffer);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_font_resource(ವರ್ಣ *resource, ಸಂರಚನಾ prt_ps_font_S *ps_font)
{
    ಪೂರ್ಣಾಙ್ಕ     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    ಪ್ರತಿ (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ಯದಿ (ps_font->ps_fontname[i] != ರಿಕ್ತ)
	    prt_dsc_resources(ರಿಕ್ತ, "font", ps_font->ps_fontname[i]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_requirements(
    ಪೂರ್ಣಾಙ್ಕ		duplex,
    ಪೂರ್ಣಾಙ್ಕ		tumble,
    ಪೂರ್ಣಾಙ್ಕ		collate,
    ಪೂರ್ಣಾಙ್ಕ		color,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ಯದಿ (!(duplex || collate || color || (ಅಂಕೆ_copies > 1)))
	ಪ್ರತ್ಯಾಗಮ;

    sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "%%%%Requirements:");
    prt_write_file(prt_ಸಾಲು_buffer);

    ಯದಿ (duplex)
    {
	prt_write_string(" duplex");
	ಯದಿ (tumble)
	    prt_write_string("(tumble)");
    }
    ಯದಿ (collate)
	prt_write_string(" collate");
    ಯದಿ (color)
	prt_write_string(" color");
    ಯದಿ (ಅಂಕೆ_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, "%d", ಅಂಕೆ_copies);
	prt_write_file(prt_ಸಾಲು_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_docmedia(
    ವರ್ಣ	*paper_name,
    ದ್ವಿಗುಣ	width,
    ದ್ವಿಗುಣ	height,
    ದ್ವಿಗುಣ	weight,
    ವರ್ಣ	*colour,
    ವರ್ಣ	*type)
{
    vim_snprintf((ವರ್ಣ *)prt_ಸಾಲು_buffer, ಪರಿಮಾಣ(prt_ಸಾಲು_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_ಸಾಲು_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    ಯದಿ (colour == ರಿಕ್ತ)
	prt_write_string("()");
    ಅನ್ಯಥಾ
	prt_write_string(colour);
    prt_write_string(" ");
    ಯದಿ (type == ರಿಕ್ತ)
	prt_write_string("()");
    ಅನ್ಯಥಾ
	prt_write_string(type);
    prt_write_string("\n");
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    ಯದಿ (prt_out_mbyte)
    {
	ಪೂರ್ಣಾಙ್ಕ     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	ಪ್ರತಿ (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ಯದಿ (prt_ps_mb_font.ps_fontname[i] != ರಿಕ್ತ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ಯದಿ (prt_do_conv)
    {
	convert_setup(&prt_conv, ರಿಕ್ತ, ರಿಕ್ತ);
	prt_do_conv = FALSE;
    }
    ಯದಿ (prt_ps_fd != ರಿಕ್ತ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ರಿಕ್ತ;
	prt_file_error = FALSE;
    }
    ಯದಿ (prt_ps_file_name != ರಿಕ್ತ)
	VIM_CLEAR(prt_ps_file_name);
}

    ಸ್ಥಿರ ಭಿನ್ನ
to_device_units(ಪೂರ್ಣಾಙ್ಕ idx, ದ್ವಿಗುಣ physsize, ಪೂರ್ಣಾಙ್ಕ def_ಸಂಖ್ಯೆ)
{
    ಭಿನ್ನ	ret;
    ಪೂರ್ಣಾಙ್ಕ		u;
    ಪೂರ್ಣಾಙ್ಕ		nr;

    u = prt_get_unit(idx);
    ಯದಿ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಅನ್ಯಥಾ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ವಿಕಲ್ಪ (u)
    {
	ಪಕ್ಷ PRT_UNIT_INCH:
	    ret = (ಭಿನ್ನ)(nr * PRT_PS_DEFAULT_DPI);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_MM:
	    ret = (ಭಿನ್ನ)(nr * PRT_PS_DEFAULT_DPI) / (ಭಿನ್ನ)25.4;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_POINT:
	    ret = (ಭಿನ್ನ)nr;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_PERC:
	ಸ್ವಭಾವ:
	    ret = (ಭಿನ್ನ)(physsize * nr) / 100;
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_page_margins(
    ದ್ವಿಗುಣ	width,
    ದ್ವಿಗುಣ	height,
    ದ್ವಿಗುಣ	*ಎಡ,
    ದ್ವಿಗುಣ	*ಬಲ,
    ದ್ವಿಗುಣ	*ತುದಿ,
    ದ್ವಿಗುಣ	*bottom)
{
    *ಎಡ   = to_device_units(OPT_PRINT_ಎಡ, width, 10);
    *ಬಲ  = width - to_device_units(OPT_PRINT_ಬಲ, width, 5);
    *ತುದಿ    = height - to_device_units(OPT_PRINT_ತುದಿ, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_font_metrics(ಪೂರ್ಣಾಙ್ಕ font_scale)
{
    prt_ಸಾಲು_height = (ಭಿನ್ನ)font_scale;
    prt_char_width = (ಭಿನ್ನ)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_get_cpl(ಶೂನ್ಯ)
{
    ಯದಿ (prt_use_ಸಂಖ್ಯೆ())
    {
	prt_ಸಂಖ್ಯೆ_width = PRINT_ಸಂಖ್ಯೆ_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ಯದಿ (prt_out_mbyte)
	    prt_ಸಂಖ್ಯೆ_width /= 2;
	prt_ಎಡ_margin += prt_ಸಂಖ್ಯೆ_width;
    }
    ಅನ್ಯಥಾ
	prt_ಸಂಖ್ಯೆ_width = 0.0;

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)((prt_ಬಲ_margin - prt_ಎಡ_margin) / prt_char_width);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_build_cid_fontname(ಪೂರ್ಣಾಙ್ಕ font, char_u *name, ಪೂರ್ಣಾಙ್ಕ name_len)
{
    ವರ್ಣ    *fontname;

    fontname = alloc(name_len + 1);
    ಯದಿ (fontname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (ಭಿನ್ನ)PRT_PS_FONT_TO_USER(prt_ಸಾಲು_height,
					   prt_ps_font->bbox_min_y);
    ಯದಿ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (ಭಿನ್ನ)PRT_PS_FONT_TO_USER(prt_ಸಾಲು_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_ಸಾಲು_height = prt_ಸಾಲು_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (ಪೂರ್ಣಾಙ್ಕ)((prt_ತುದಿ_margin - prt_bottom_margin) / prt_ಸಾಲು_height);

    // Adjust top margin if there is a header
    prt_ತುದಿ_margin -= prt_ಸಾಲು_height * prt_header_height();

    ಪ್ರತ್ಯಾಗಮ lpp - prt_header_height();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_match_encoding(
    ವರ್ಣ			*p_encoding,
    ಸಂರಚನಾ prt_ps_mbfont_S	*p_cmap,
    ಸಂರಚನಾ prt_ps_encoding_S	**pp_mbenc)
{
    ಪೂರ್ಣಾಙ್ಕ				mbenc;
    ಪೂರ್ಣಾಙ್ಕ				enc_len;
    ಸಂರಚನಾ prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ರಿಕ್ತ;
    // Look for recognised encoding
    enc_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    ಪ್ರತಿ (mbenc = 0; mbenc < p_cmap->ಅಂಕೆ_encodings; mbenc++)
    {
	ಯದಿ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	p_mbenc++;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_match_charset(
    ವರ್ಣ		    *p_charset,
    ಸಂರಚನಾ prt_ps_mbfont_S  *p_cmap,
    ಸಂರಚನಾ prt_ps_charset_S **pp_mbchar)
{
    ಪೂರ್ಣಾಙ್ಕ			    mbchar;
    ಪೂರ್ಣಾಙ್ಕ			    char_len;
    ಸಂರಚನಾ prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ಯದಿ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    ಪ್ರತಿ (mbchar = 0; mbchar < p_cmap->ಅಂಕೆ_charsets; mbchar++)
    {
	ಯದಿ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	p_mbchar++;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    ಪೂರ್ಣಾಙ್ಕ		forceit UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ವರ್ಣ	*paper_name;
    ಪೂರ್ಣಾಙ್ಕ		paper_strlen;
    ಪೂರ್ಣಾಙ್ಕ		fontsize;
    char_u	*p;
    ದ್ವಿಗುಣ      ಎಡ;
    ದ್ವಿಗುಣ      ಬಲ;
    ದ್ವಿಗುಣ      ತುದಿ;
    ದ್ವಿಗುಣ      bottom;
    ಪೂರ್ಣಾಙ್ಕ		props;
    ಪೂರ್ಣಾಙ್ಕ		cmap = 0;
    char_u	*p_encoding;
    ಸಂರಚನಾ prt_ps_encoding_S *p_mbenc;
    ಸಂರಚನಾ prt_ps_encoding_S *p_mbenc_first;
    ಸಂರಚನಾ prt_ps_charset_S  *p_mbchar = ರಿಕ್ತ;

# ಯದಿ 0
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
    ಯದಿ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ಯದಿ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ರಿಕ್ತ;
    props = enc_canon_props(p_encoding);
    ಯದಿ (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	ಪೂರ್ಣಾಙ್ಕ cmap_first = 0;

	p_mbenc_first = ರಿಕ್ತ;
	ಪ್ರತಿ (cmap = 0; cmap < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    ಯದಿ (prt_match_encoding((ವರ್ಣ *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ಯದಿ (p_mbenc_first == ರಿಕ್ತ)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		ಯದಿ (prt_match_charset((ವರ್ಣ *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    ಭಙ್ಗ;
	    }

	// Use first encoding matched if no charset matched
	ಯದಿ (p_mbchar == ರಿಕ್ತ && p_mbenc_first != ರಿಕ್ತ)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != ರಿಕ್ತ);
    ಯದಿ (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ರಿಕ್ತ);
	ಯದಿ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ಯದಿ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }

	    // Add charset name if not empty
	    ಯದಿ (p_mbchar->cmap_charset != ರಿಕ್ತ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, ಪರಿಮಾಣ(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Add custom CMap character set name
	    ಯದಿ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, ಪರಿಮಾಣ(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ಯದಿ (p_mbenc->cmap_encoding != ರಿಕ್ತ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < ಪರಿಮಾಣ(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ಯದಿ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ಯದಿ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ಯದಿ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ಯದಿ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ಯದಿ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		ಪ್ರತ್ಯಾಗಮ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ಯದಿ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ಯದಿ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ಅನ್ಯಥಾ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ಅನ್ಯಥಾ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ಯದಿ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (ವರ್ಣ *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    ಅನ್ಯಥಾ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)PRT_MEDIASIZE_LEN; ++i)
	ಯದಿ (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    ಭಙ್ಗ;
    ಯದಿ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ಯದಿ (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    ಅನ್ಯಥಾ
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &ಎಡ, &ಬಲ, &ತುದಿ,
								    &bottom);
    prt_ಎಡ_margin = (ಭಿನ್ನ)ಎಡ;
    prt_ಬಲ_margin = (ಭಿನ್ನ)ಬಲ;
    prt_ತುದಿ_margin = (ಭಿನ್ನ)ತುದಿ;
    prt_bottom_margin = (ಭಿನ್ನ)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    ಪ್ರತಿ (p = p_pfn; (p = vim_strchr(p, ':')) != ರಿಕ್ತ; ++p)
	ಯದಿ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((ವರ್ಣ *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_ಸಾಲು = prt_get_cpl();
    psettings->ಸಾಲುಗಳು_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ಯದಿ (psettings->chars_per_ಸಾಲು <= 0 || psettings->ಸಾಲುಗಳು_per_page <= 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_ಅಂಕೆ_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    ಯದಿ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ಅನ್ಯಥಾ
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_ಅಂಕೆ_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    ಯದಿ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ಯದಿ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ಯದಿ (psettings->outfile == ರಿಕ್ತ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ಯದಿ (prt_ps_file_name == ರಿಕ್ತ)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	prt_ps_fd = mch_fopen((ವರ್ಣ *)prt_ps_file_name, WRITEBIN);
    }
    ಅನ್ಯಥಾ
    {
	p = expand_env_save(psettings->outfile);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    prt_ps_fd = mch_fopen((ವರ್ಣ *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ಯದಿ (prt_ps_fd == ರಿಕ್ತ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    prt_bufsiz = psettings->chars_per_ಸಾಲು;
    ಯದಿ (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, ಪರಿಮಾಣ(ವರ್ಣ), prt_bufsiz);

    prt_page_ಅಂಕೆ = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_add_resource(ಸಂರಚನಾ prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((ವರ್ಣ *)resource->filename, READBIN);
    ಯದಿ (fd_resource == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (ವರ್ಣ *)resource->title);

    prt_dsc_textline("BeginDocument", (ವರ್ಣ *)resource->filename);

    ಪ್ರತಿ (;;)
    {
	bytes_read = fread((ವರ್ಣ *)resource_buffer, ಪರಿಮಾಣ(char_u),
			   ಪರಿಮಾಣ(resource_buffer), fd_resource);
	ಯದಿ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಯದಿ (bytes_read == 0)
	    ಭಙ್ಗ;
	prt_write_file_raw_len(resource_buffer, (ಪೂರ್ಣಾಙ್ಕ)bytes_read);
	ಯದಿ (prt_file_error)
	{
	    fclose(fd_resource);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin(prt_settings_T *psettings)
{
    ಪೂರ್ಣಾಙ್ಕ		bbox[4];
    ದ್ವಿಗುಣ      ಎಡ;
    ದ್ವಿಗುಣ      ಬಲ;
    ದ್ವಿಗುಣ      ತುದಿ;
    ದ್ವಿಗುಣ      bottom;
    ಸಂರಚನಾ prt_ps_resource_S *res_prolog;
    ಸಂರಚನಾ prt_ps_resource_S *res_encoding;
    ವರ್ಣ	buffer[256];
    char_u      *p_encoding;
    ಸಂರಚನಾ prt_ps_resource_S *res_cidfont;
    ಸಂರಚನಾ prt_ps_resource_S *res_cmap;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    res_prolog = ALLOC_ONE(ಸಂರಚನಾ prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ಸಂರಚನಾ prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ಸಂರಚನಾ prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ಸಂರಚನಾ prt_ps_resource_S);
    ಯದಿ (res_prolog == ರಿಕ್ತ || res_encoding == ರಿಕ್ತ
	    || res_cidfont == ರಿಕ್ತ || res_cmap == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (ವರ್ಣ *)psettings->jobname);
    ಯದಿ (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(ರಿಕ್ತ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &ಎಡ, &ಬಲ, &ತುದಿ, &bottom);
    bbox[0] = (ಪೂರ್ಣಾಙ್ಕ)ಎಡ;
    ಯದಿ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (ಪೂರ್ಣಾಙ್ಕ)(ತುದಿ - (psettings->ಸಾಲುಗಳು_per_page + prt_header_height())
						    * (ದ್ವಿಗುಣ)prt_ಸಾಲು_height);
	bbox[2] = (ಪೂರ್ಣಾಙ್ಕ)(ಎಡ + psettings->chars_per_ಸಾಲು
					       * (ದ್ವಿಗುಣ)prt_char_width + 0.5);
	bbox[3] = (ಪೂರ್ಣಾಙ್ಕ)(ತುದಿ + 0.5);
    }
    ಅನ್ಯಥಾ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (ಪೂರ್ಣಾಙ್ಕ)bottom;
	bbox[2] = (ಪೂರ್ಣಾಙ್ಕ)(ಎಡ + ((psettings->ಸಾಲುಗಳು_per_page
			      + prt_header_height()) * prt_ಸಾಲು_height) + 0.5);
	bbox[3] = (ಪೂರ್ಣಾಙ್ಕ)(bottom + psettings->chars_per_ಸಾಲು
					       * (ದ್ವಿಗುಣ)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(ದ್ವಿಗುಣ)0, ರಿಕ್ತ, ರಿಕ್ತ);
    // Define fonts needed
    ಯದಿ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ಯದಿ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ರಿಕ್ತ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ಯದಿ (!prt_custom_cmap)
	    prt_dsc_resources(ರಿಕ್ತ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ಯದಿ (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	ಗಚ್ಛ theend;
    }
    ಯದಿ (!prt_open_resource(res_prolog))
	ಗಚ್ಛ theend;
    ಯದಿ (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	ಗಚ್ಛ theend;
    ಯದಿ (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ಯದಿ (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (!prt_open_resource(res_cidfont))
	    ಗಚ್ಛ theend;
	ಯದಿ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    ಗಚ್ಛ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ಯದಿ (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ಯದಿ (*p_encoding == NUL
		|| !prt_find_resource((ವರ್ಣ *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    ಪೂರ್ಣಾಙ್ಕ		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ಯದಿ (!(props & ENC_8BIT)
		    || !prt_find_resource((ವರ್ಣ *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ಯದಿ (!prt_find_resource((ವರ್ಣ *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    ಗಚ್ಛ theend;
		}
	    }
	}
	ಯದಿ (!prt_open_resource(res_encoding))
	    ಗಚ್ಛ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ಅನ್ಯಥಾ
    {
	p_encoding = enc_skip(p_penc);
	ಯದಿ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ಯದಿ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ಯದಿ (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		ಗಚ್ಛ theend;
	    }
	    ಯದಿ (!prt_open_resource(res_encoding))
		ಗಚ್ಛ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ಯದಿ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	ಯದಿ (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    ಗಚ್ಛ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ಯದಿ (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ಯದಿ (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (!prt_open_resource(res_cmap))
	    ಗಚ್ಛ theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    ಯದಿ (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(ರಿಕ್ತ, "procset", buffer);

	ಯದಿ (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(ರಿಕ್ತ, "cmap", buffer);
	}
    }
    ಯದಿ (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(ರಿಕ್ತ, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ಅನ್ಯಥಾ
					0
# endif
					, prt_ಅಂಕೆ_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ಯದಿ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ಯದಿ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ರಿಕ್ತ : "PageResources"),
							    &prt_ps_mb_font);
	ಯದಿ (!prt_custom_cmap)
	    prt_dsc_resources(ರಿಕ್ತ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ಯದಿ (!prt_add_resource(res_prolog))
	ಗಚ್ಛ theend;
    ಯದಿ (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ಯದಿ (!prt_add_resource(res_cidfont))
	    ಗಚ್ಛ theend;
	ಯದಿ (prt_custom_cmap && !prt_add_resource(res_cmap))
	    ಗಚ್ಛ theend;
    }

    ಯದಿ (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ಯದಿ (!prt_add_resource(res_encoding))
	    ಗಚ್ಛ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((ಪೂರ್ಣಾಙ್ಕ)prt_mediasize[prt_media].width);
    prt_write_int((ಪೂರ್ಣಾಙ್ಕ)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_ಅಂಕೆ_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    ಯದಿ (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ಯದಿ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (ವರ್ಣ *)p_encoding, (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (ವರ್ಣ *)p_encoding, (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (ವರ್ಣ *)p_encoding, (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (ವರ್ಣ *)p_encoding, (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ಯದಿ (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ಯದಿ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ಯದಿ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ರಿಕ್ತ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ಯದಿ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ಅನ್ಯಥಾ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ಯದಿ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ರಿಕ್ತ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ಯದಿ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ಅನ್ಯಥಾ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ಯದಿ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ರಿಕ್ತ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ಯದಿ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (ಪೂರ್ಣಾಙ್ಕ)prt_ಸಾಲು_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ಅನ್ಯಥಾ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_ಸಾಲು_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_ಸಾಲು_height,
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

    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಶೂನ್ಯ
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_ಅಂಕೆ);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ಯದಿ (!prt_file_error && psettings->outfile == ರಿಕ್ತ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ಯದಿ (prt_ps_fd != ರಿಕ್ತ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ರಿಕ್ತ;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ಯದಿ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ಅನ್ಯಥಾ
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_end_page(ಶೂನ್ಯ)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ಪ್ರತ್ಯಾಗಮ !prt_file_error;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin_page(char_u *str UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		page_ಅಂಕೆ[2];

    prt_page_ಅಂಕೆ++;

    page_ಅಂಕೆ[0] = page_ಅಂಕೆ[1] = prt_page_ಅಂಕೆ;
    prt_dsc_ints("Page", 2, page_ಅಂಕೆ);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    ಯದಿ (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    ಅನ್ಯಥಾ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ಯದಿ (!prt_portrait)
    {
	prt_write_int(-((ಪೂರ್ಣಾಙ್ಕ)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ಪ್ರತ್ಯಾಗಮ !prt_file_error;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_blank_page(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (mch_print_begin_page(ರಿಕ್ತ) ? (mch_print_end_page()) : FALSE);
}

ಸ್ಥಿರ ಭಿನ್ನ prt_pos_x = 0;
ಸ್ಥಿರ ಭಿನ್ನ prt_pos_y = 0;

    ಶೂನ್ಯ
mch_print_start_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ margin, ಪೂರ್ಣಾಙ್ಕ page_ಸಾಲು)
{
    prt_pos_x = prt_ಎಡ_margin;
    ಯದಿ (margin)
	prt_pos_x -= prt_ಸಂಖ್ಯೆ_width;

    prt_pos_y = prt_ತುದಿ_margin - prt_first_ಸಾಲು_height -
					page_ಸಾಲು * prt_ಸಾಲು_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_text_out(char_u *textp, ಪೂರ್ಣಾಙ್ಕ len UNUSED)
{
    char_u	*p = textp;
    ಪೂರ್ಣಾಙ್ಕ		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    ಭಿನ್ನ       char_width;
    ಭಿನ್ನ       next_pos;
    ಪೂರ್ಣಾಙ್ಕ		in_ascii;
    ಪೂರ್ಣಾಙ್ಕ		half_width;
    char_u	*tofree = ರಿಕ್ತ;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ಯದಿ (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	ಯದಿ (prt_in_ascii)
	{
	    ಯದಿ (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ಯದಿ (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	ಯದಿ (half_width)
	    char_width /= 2;
	ಯದಿ (prt_half_width)
	{
	    ಯದಿ (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ಯದಿ (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	ಯದಿ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ಯದಿ (prt_need_font)
	{
	    ಯದಿ (!prt_in_ascii)
		prt_write_string("CF");
	    ಅನ್ಯಥಾ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ಯದಿ (prt_need_fgcol)
	{
	    ಪೂರ್ಣಾಙ್ಕ     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ಯದಿ (r == g && g == b)
		prt_write_string("g\n");
	    ಅನ್ಯಥಾ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ಯದಿ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ಯದಿ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ಅನ್ಯಥಾ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ಯದಿ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ಯದಿ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ಯದಿ (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ಯಾವತ್ (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ಯದಿ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    ವಿಕಲ್ಪ (ch)
	    {
		ಪಕ್ಷ BS:   ga_append(&prt_ps_buffer, 'b'); ಭಙ್ಗ;
		ಪಕ್ಷ TAB:  ga_append(&prt_ps_buffer, 't'); ಭಙ್ಗ;
		ಪಕ್ಷ NL:   ga_append(&prt_ps_buffer, 'n'); ಭಙ್ಗ;
		ಪಕ್ಷ FF:   ga_append(&prt_ps_buffer, 'f'); ಭಙ್ಗ;
		ಪಕ್ಷ CAR:  ga_append(&prt_ps_buffer, 'r'); ಭಙ್ಗ;
		ಪಕ್ಷ '(':  ga_append(&prt_ps_buffer, '('); ಭಙ್ಗ;
		ಪಕ್ಷ ')':  ga_append(&prt_ps_buffer, ')'); ಭಙ್ಗ;
		ಪಕ್ಷ '\\': ga_append(&prt_ps_buffer, '\\'); ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			   sprintf((ವರ್ಣ *)ch_buff, "%03o", (unsigned ಪೂರ್ಣಾಙ್ಕ)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_ಬಲ_margin) &&
		    ((next_pos - prt_ಬಲ_margin) > (prt_ಬಲ_margin*1e-5));

    ಯದಿ (need_break)
	prt_flush_buffer();

    ಪ್ರತ್ಯಾಗಮ need_break;
}

    ಶೂನ್ಯ
mch_print_set_font(ಪೂರ್ಣಾಙ್ಕ iBold, ಪೂರ್ಣಾಙ್ಕ iItalic, ಪೂರ್ಣಾಙ್ಕ iUnderline)
{
    ಪೂರ್ಣಾಙ್ಕ		font = 0;

    ಯದಿ (iBold)
	font |= 0x01;
    ಯದಿ (iItalic)
	font |= 0x02;

    ಯದಿ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ಯದಿ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    ಶೂನ್ಯ
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (ಪೂರ್ಣಾಙ್ಕ)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    ಶೂನ್ಯ
mch_print_set_fg(long_u fgcol)
{
    ಯದಿ (fgcol == (long_u)prt_fgcol)
	ಪ್ರತ್ಯಾಗಮ;

    prt_fgcol = (ಪೂರ್ಣಾಙ್ಕ)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
