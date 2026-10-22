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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_ଅଙ୍କ_OPTIONS] =
{
    {"c",	FALSE, 0, ଖାଲି, 0, FALSE},
    {"a",	FALSE, 0, ଖାଲି, 0, FALSE},
    {"r",	FALSE, 0, ଖାଲି, 0, FALSE},
    {"b",	FALSE, 0, ଖାଲି, 0, FALSE},
    {"i",	FALSE, 0, ଖାଲି, 0, FALSE},
    {"o",	FALSE, 0, ଖାଲି, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ସଂରଚନା prt_ps_font_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uline_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uline_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bbox_min_y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bbox_max_y;
    ଅକ୍ଷର	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ସ୍ଥିର ସଂରଚନା prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ସ୍ଥିର ସଂରଚନା prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି}
};

// Pointer to current font set being used
ସ୍ଥିର ସଂରଚନା prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ସଂରଚନା prt_ps_encoding_S
{
    ଅକ୍ଷର	*encoding;
    ଅକ୍ଷର	*cmap_encoding;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		needs_charset;
};

ସଂରଚନା prt_ps_charset_S
{
    ଅକ୍ଷର	*charset;
    ଅକ୍ଷର	*cmap_charset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_charset;
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
ସ୍ଥିର ସଂରଚନା prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ଖାଲି,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ସ୍ଥିର ସଂରଚନା prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ଖାଲି,       CS_JIS_X_1983},
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
ସ୍ଥିର ସଂରଚନା prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ଖାଲି,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ଖାଲି,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ସ୍ଥିର ସଂରଚନା prt_ps_charset_S sc_charsets[] =
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
ସ୍ଥିର ସଂରଚନା prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ଖାଲି,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
ସ୍ଥିର ସଂରଚନା prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ଖାଲି,	CS_B5},
    {"CP950",	    ଖାଲି,	CS_B5},
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
ସ୍ଥିର ସଂରଚନା prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ଖାଲି,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ସ୍ଥିର ସଂରଚନା prt_ps_charset_S k_charsets[] =
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
ସଂରଚନା prt_ps_mbfont_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				ଅଙ୍କ_encodings;
    ସଂରଚନା prt_ps_encoding_S	*encodings;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				ଅଙ୍କ_charsets;
    ସଂରଚନା prt_ps_charset_S	*charsets;
    ଅକ୍ଷର			*ascii_enc;
    ଅକ୍ଷର			*defcs;
};

ସ୍ଥିର ସଂରଚନା prt_ps_mbfont_S prt_ps_mbfonts[] =
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

ସଂରଚନା prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     type;
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
ସ୍ଥିର ଅକ୍ଷର *prt_resource_types[] =
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

ସଂରଚନା prt_dsc_comment_S
{
    ଅକ୍ଷର	*string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
};

ସଂରଚନା prt_dsc_ଧାଡ଼ି_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type;
    char_u	*string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
};


# define SIZEOF_CSTR(s)      (ଆକାର(s) - 1)
ସ୍ଥିର ସଂରଚନା prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ସ୍ଥିର ଶୂନ୍ୟ prt_write_file_len(char_u *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_next_dsc(ସଂରଚନା prt_dsc_ଧାଡ଼ି_S *p_dsc_ଧାଡ଼ି);

/*
 * Variables for the output PostScript file.
 */
ସ୍ଥିର FILE *prt_ps_fd;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_file_error;
ସ୍ଥିର char_u *prt_ps_file_name = ଖାଲି;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ସ୍ଥିର ଦଶମିକ prt_page_width;
ସ୍ଥିର ଦଶମିକ prt_page_height;
ସ୍ଥିର ଦଶମିକ prt_ବାମ_margin;
ସ୍ଥିର ଦଶମିକ prt_ଡାହାଣ_margin;
ସ୍ଥିର ଦଶମିକ prt_ଶୀର୍ଷ_margin;
ସ୍ଥିର ଦଶମିକ prt_bottom_margin;
ସ୍ଥିର ଦଶମିକ prt_ଧାଡ଼ି_height;
ସ୍ଥିର ଦଶମିକ prt_first_ଧାଡ଼ି_height;
ସ୍ଥିର ଦଶମିକ prt_char_width;
ସ୍ଥିର ଦଶମିକ prt_ସଂଖ୍ୟା_width;
ସ୍ଥିର ଦଶମିକ prt_bgcol_offset;
ସ୍ଥିର ଦଶମିକ prt_pos_x_moveto = 0.0;
ସ୍ଥିର ଦଶମିକ prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_need_moveto;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_do_moveto;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_need_font;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_font;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_need_underline;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_underline;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_do_underline;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_need_fgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_fgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_need_bgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_do_bgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_bgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_new_bgcol;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_attribute_change;
ସ୍ଥିର ଦଶମିକ prt_text_run;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_page_ଅଙ୍କ;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_media;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_portrait;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_ଅଙ୍କ_copies;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_duplex;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_tumble;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ସ୍ଥିର char_u prt_ଧାଡ଼ି_buffer[257];
ସ୍ଥିର garray_T prt_ps_buffer;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_do_conv;
ସ୍ଥିର vimconv_T prt_conv;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_out_mbyte;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_custom_cmap;
ସ୍ଥିର ଅକ୍ଷର prt_cmap[80];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_use_courier;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_in_ascii;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_half_width;
ସ୍ଥିର ଅକ୍ଷର *prt_ascii_encoding;
ସ୍ଥିର ଅଚଳ char_u prt_hexchar[] = "0123456789abcdef";

    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_file_raw_len(char_u *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes)
{
    ଯଦି (!prt_file_error
	    && fwrite(buffer, ଆକାର(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buffer));
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_file_len(char_u *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_string(ଅକ୍ଷର *s)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer), "%s", s);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write an int and a space.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "%d ", i);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write a boolean and a space.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_boolean(ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_def_font(
    ଅକ୍ଷର	*new_name,
    ଅକ୍ଷର	*encoding,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height,
    ଅକ୍ଷର	*font)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
    ଯଦି (prt_out_mbyte)
	sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write a line to define the CID font.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_def_cidfont(ଅକ୍ଷର *new_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height, ଅକ୍ଷର *cidfont)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_dup_cidfont(ଅକ୍ଷର *original_name, ଅକ୍ଷର *new_name)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_real_bits(
    ଦ୍ୱିଗୁଣ      real,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		precision,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pinteger,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pfraction)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     integer;
    ଦଶମିକ   fraction;

    integer = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)real;
    fraction = (ଦଶମିକ)(real - integer);
    ଯଦି (real < (ଦ୍ୱିଗୁଣ)integer)
	fraction = -fraction;
    ପାଇଁ (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_write_real(ଦ୍ୱିଗୁଣ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     integer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "%d", integer);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
    // Only emit fraction if necessary
    ଯଦି (fraction != 0)
    {
	// Remove any trailing zeros
	ଯେତେବେଳେ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_ଧାଡ଼ି_buffer);
    }
    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, " ");
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_def_var(ଅକ୍ଷର *name, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prec)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
								"/%s ", name);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
    prt_write_real(ମୂଲ୍ୟ, prec);
    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "d\n");
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    ସ୍ଥିର ଶୂନ୍ୟ
prt_flush_buffer(ଶୂନ୍ୟ)
{
    ଯଦି (prt_ps_buffer.ga_len <= 0)
	ଫେରନ୍ତୁ;

    // Any background color must be drawn first
    ଯଦି (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     r, g, b;

	ଯଦି (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_ଧାଡ଼ି_height, 2);

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
    ଯଦି (prt_do_underline)
    {
	ଯଦି (prt_do_moveto)
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
    ଯଦି (prt_out_mbyte)
	prt_write_string("<");
    ଅନ୍ୟଥା
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    ଯଦି (prt_out_mbyte)
	prt_write_string(">");
    ଅନ୍ୟଥା
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ଯଦି (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ଅନ୍ୟଥା // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, ଆକାର(ଅକ୍ଷର), prt_bufsiz);
}


    ସ୍ଥିର ଶୂନ୍ୟ
prt_resource_name(char_u *filename, ଶୂନ୍ୟ *cookie)
{
    char_u *resource_filename = cookie;

    ଯଦି (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ଅନ୍ୟଥା
	STRCPY(resource_filename, filename);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_find_resource(ଅକ୍ଷର *name, ସଂରଚନା prt_ps_resource_S *resource)
{
    char_u	*buffer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    buffer = alloc(MAXPATHL + 1);
    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

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
    ଫେରନ୍ତୁ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
ସଂରଚନା prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     ଧାଡ଼ି_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     ଧାଡ଼ି_end;
};

ସ୍ଥିର ସଂରଚନା prt_resfile_buffer_S prt_resfile;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_resfile_next_ଧାଡ଼ି(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.ଧାଡ଼ି_end + 1;
    ଯେତେବେଳେ (idx < prt_resfile.len)
    {
	ଯଦି (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    ଭାଙ୍ଗ;
	idx++;
    }
    prt_resfile.ଧାଡ଼ି_start = idx;

    ଯେତେବେଳେ (idx < prt_resfile.len)
    {
	ଯଦି (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    ଭାଙ୍ଗ;
	idx++;
    }
    prt_resfile.ଧାଡ଼ି_end = idx;

    ଫେରନ୍ତୁ (idx < prt_resfile.len);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_resfile_strncmp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ଅକ୍ଷର *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    // Force not equal if string is longer than remainder of line
    ଯଦି (len > (prt_resfile.ଧାଡ଼ି_end - (prt_resfile.ଧାଡ଼ି_start + offset)))
	ଫେରନ୍ତୁ 1;

    ଫେରନ୍ତୁ STRNCMP(&prt_resfile.buffer[prt_resfile.ଧାଡ଼ି_start + offset],
								string, len);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_resfile_skip_nonws(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     idx;

    idx = prt_resfile.ଧାଡ଼ି_start + offset;
    ଯେତେବେଳେ (idx < prt_resfile.ଧାଡ଼ି_end)
    {
	ଯଦି (SAFE_isspace(prt_resfile.buffer[idx]))
	    ଫେରନ୍ତୁ idx - prt_resfile.ଧାଡ଼ି_start;
	idx++;
    }
    ଫେରନ୍ତୁ -1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_resfile_skip_ws(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     idx;

    idx = prt_resfile.ଧାଡ଼ି_start + offset;
    ଯେତେବେଳେ (idx < prt_resfile.ଧାଡ଼ି_end)
    {
	ଯଦି (!SAFE_isspace(prt_resfile.buffer[idx]))
	    ଫେରନ୍ତୁ idx - prt_resfile.ଧାଡ଼ି_start;
	idx++;
    }
    ଫେରନ୍ତୁ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_next_dsc(ସଂରଚନା prt_dsc_ଧାଡ଼ି_S *p_dsc_ଧାଡ଼ି)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     comment;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     offset;

    // Move to start of next line
    ଯଦି (!prt_resfile_next_ଧାଡ଼ି())
	ଫେରନ୍ତୁ FALSE;

    // DSC comments always start %%
    ଯଦି (prt_resfile_strncmp(0, "%%", 2) != 0)
	ଫେରନ୍ତୁ FALSE;

    // Find type of DSC comment
    ପାଇଁ (comment = 0; comment < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(prt_dsc_table); comment++)
	ଯଦି (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    ଭାଙ୍ଗ;

    ଯଦି (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_ଧାଡ଼ି->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    ଅନ୍ୟଥା
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_ଧାଡ଼ି->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	ଯଦି (offset == -1)
	    ଫେରନ୍ତୁ FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    ଯଦି (offset == -1)
	ଫେରନ୍ତୁ FALSE;

    p_dsc_ଧାଡ଼ି->string = &prt_resfile.buffer[prt_resfile.ଧାଡ଼ି_start + offset];
    p_dsc_ଧାଡ଼ି->len = prt_resfile.ଧାଡ଼ି_end - (prt_resfile.ଧାଡ଼ି_start + offset);

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_open_resource(ସଂରଚନା prt_ps_resource_S *resource)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_all;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_title;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen_version;
    FILE	*fd_resource;
    ସଂରଚନା prt_dsc_ଧାଡ଼ି_S dsc_ଧାଡ଼ି;

    fd_resource = mch_fopen((ଅକ୍ଷର *)resource->filename, READBIN);
    ଯଦି (fd_resource == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fread((ଅକ୍ଷର *)prt_resfile.buffer, ଆକାର(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    ଯଦି (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ଫେରନ୍ତୁ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.ଧାଡ଼ି_end = -1;
    prt_resfile.ଧାଡ଼ି_start = 0;
    ଯଦି (!prt_resfile_next_ଧାଡ଼ି())
	ଫେରନ୍ତୁ FALSE;

    offset = 0;

    ଯଦି (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    ଯଦି (offset == -1)
	ଫେରନ୍ତୁ FALSE;
    offset = prt_resfile_skip_ws(offset);
    ଯଦି (offset == -1)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }
    offset += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ଯଦି (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ଅନ୍ୟଥା ଯଦି (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ଅନ୍ୟଥା ଯଦି (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ଅନ୍ୟଥା
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    ଯେତେବେଳେ (!seen_all && prt_next_dsc(&dsc_ଧାଡ଼ି))
    {
	ବାଛ (dsc_ଧାଡ଼ି.type)
	{
	ସ୍ଥିତି PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_ଧାଡ଼ି.string, dsc_ଧାଡ଼ି.len);
	    seen_title = TRUE;
	    ଯଦି (seen_version)
		seen_all = TRUE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_ଧାଡ଼ି.string, dsc_ଧାଡ଼ି.len);
	    seen_version = TRUE;
	    ଯଦି (seen_title)
		seen_all = TRUE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_check_resource(ସଂରଚନା prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    ଯଦି (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	ଫେରନ୍ତୁ FALSE;
    }

    // Other checks to be added as needed
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_start(ଶୂନ୍ୟ)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_noarg(ଅକ୍ଷର *comment)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_textline(ଅକ୍ଷର *comment, ଅକ୍ଷର *text)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_text(ଅକ୍ଷର *comment, ଅକ୍ଷର *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_ints(ଅକ୍ଷର *comment, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ints)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_ଧାଡ଼ି_buffer);

    ପାଇଁ (i = 0; i < ଗଣନା_2; i++)
    {
	sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, " %d", ints[i]);
	prt_write_file(prt_ଧାଡ଼ି_buffer);
    }

    prt_write_string("\n");
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_resources(
    ଅକ୍ଷର	*comment,	// if NULL add to previous
    ଅକ୍ଷର	*type,
    ଅକ୍ଷର	*string)
{
    ଯଦି (comment != ଖାଲି)
	vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
						 "%%%%%s: %s", comment, type);
    ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_ଧାଡ଼ି_buffer);

    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
							     " %s\n", string);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_font_resource(ଅକ୍ଷର *resource, ସଂରଚନା prt_ps_font_S *ps_font)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    ପାଇଁ (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ଯଦି (ps_font->ps_fontname[i] != ଖାଲି)
	    prt_dsc_resources(ଖାଲି, "font", ps_font->ps_fontname[i]);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_requirements(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		duplex,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tumble,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		collate,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		color,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ଯଦି (!(duplex || collate || color || (ଅଙ୍କ_copies > 1)))
	ଫେରନ୍ତୁ;

    sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "%%%%Requirements:");
    prt_write_file(prt_ଧାଡ଼ି_buffer);

    ଯଦି (duplex)
    {
	prt_write_string(" duplex");
	ଯଦି (tumble)
	    prt_write_string("(tumble)");
    }
    ଯଦି (collate)
	prt_write_string(" collate");
    ଯଦି (color)
	prt_write_string(" color");
    ଯଦି (ଅଙ୍କ_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, "%d", ଅଙ୍କ_copies);
	prt_write_file(prt_ଧାଡ଼ି_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_dsc_docmedia(
    ଅକ୍ଷର	*paper_name,
    ଦ୍ୱିଗୁଣ	width,
    ଦ୍ୱିଗୁଣ	height,
    ଦ୍ୱିଗୁଣ	weight,
    ଅକ୍ଷର	*colour,
    ଅକ୍ଷର	*type)
{
    vim_snprintf((ଅକ୍ଷର *)prt_ଧାଡ଼ି_buffer, ଆକାର(prt_ଧାଡ଼ି_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_ଧାଡ଼ି_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    ଯଦି (colour == ଖାଲି)
	prt_write_string("()");
    ଅନ୍ୟଥା
	prt_write_string(colour);
    prt_write_string(" ");
    ଯଦି (type == ଖାଲି)
	prt_write_string("()");
    ଅନ୍ୟଥା
	prt_write_string(type);
    prt_write_string("\n");
}

    ଶୂନ୍ୟ
mch_print_cleanup(ଶୂନ୍ୟ)
{
    ଯଦି (prt_out_mbyte)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	ପାଇଁ (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ଯଦି (prt_ps_mb_font.ps_fontname[i] != ଖାଲି)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ଯଦି (prt_do_conv)
    {
	convert_setup(&prt_conv, ଖାଲି, ଖାଲି);
	prt_do_conv = FALSE;
    }
    ଯଦି (prt_ps_fd != ଖାଲି)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ଖାଲି;
	prt_file_error = FALSE;
    }
    ଯଦି (prt_ps_file_name != ଖାଲି)
	VIM_CLEAR(prt_ps_file_name);
}

    ସ୍ଥିର ଦଶମିକ
to_device_units(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଦ୍ୱିଗୁଣ physsize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_ସଂଖ୍ୟା)
{
    ଦଶମିକ	ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    u = prt_get_unit(idx);
    ଯଦି (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ସଂଖ୍ୟା;
    }
    ଅନ୍ୟଥା
	nr = printer_opts[idx].ସଂଖ୍ୟା;

    ବାଛ (u)
    {
	ସ୍ଥିତି PRT_UNIT_INCH:
	    ret = (ଦଶମିକ)(nr * PRT_PS_DEFAULT_DPI);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_MM:
	    ret = (ଦଶମିକ)(nr * PRT_PS_DEFAULT_DPI) / (ଦଶମିକ)25.4;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_POINT:
	    ret = (ଦଶମିକ)nr;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_PERC:
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ret = (ଦଶମିକ)(physsize * nr) / 100;
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prt_page_margins(
    ଦ୍ୱିଗୁଣ	width,
    ଦ୍ୱିଗୁଣ	height,
    ଦ୍ୱିଗୁଣ	*ବାମ,
    ଦ୍ୱିଗୁଣ	*ଡାହାଣ,
    ଦ୍ୱିଗୁଣ	*ଶୀର୍ଷ,
    ଦ୍ୱିଗୁଣ	*bottom)
{
    *ବାମ   = to_device_units(OPT_PRINT_ବାମ, width, 10);
    *ଡାହାଣ  = width - to_device_units(OPT_PRINT_ଡାହାଣ, width, 5);
    *ଶୀର୍ଷ    = height - to_device_units(OPT_PRINT_ଶୀର୍ଷ, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prt_font_metrics(ପୂର୍ଣ୍ଣସଂଖ୍ୟା font_scale)
{
    prt_ଧାଡ଼ି_height = (ଦଶମିକ)font_scale;
    prt_char_width = (ଦଶମିକ)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_get_cpl(ଶୂନ୍ୟ)
{
    ଯଦି (prt_use_ସଂଖ୍ୟା())
    {
	prt_ସଂଖ୍ୟା_width = PRINT_ସଂଖ୍ୟା_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ଯଦି (prt_out_mbyte)
	    prt_ସଂଖ୍ୟା_width /= 2;
	prt_ବାମ_margin += prt_ସଂଖ୍ୟା_width;
    }
    ଅନ୍ୟଥା
	prt_ସଂଖ୍ୟା_width = 0.0;

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((prt_ଡାହାଣ_margin - prt_ବାମ_margin) / prt_char_width);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_build_cid_fontname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା font, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_len)
{
    ଅକ୍ଷର    *fontname;

    fontname = alloc(name_len + 1);
    ଯଦି (fontname == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_get_lpp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (ଦଶମିକ)PRT_PS_FONT_TO_USER(prt_ଧାଡ଼ି_height,
					   prt_ps_font->bbox_min_y);
    ଯଦି ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (ଦଶମିକ)PRT_PS_FONT_TO_USER(prt_ଧାଡ଼ି_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_ଧାଡ଼ି_height = prt_ଧାଡ଼ି_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((prt_ଶୀର୍ଷ_margin - prt_bottom_margin) / prt_ଧାଡ଼ି_height);

    // Adjust top margin if there is a header
    prt_ଶୀର୍ଷ_margin -= prt_ଧାଡ଼ି_height * prt_header_height();

    ଫେରନ୍ତୁ lpp - prt_header_height();
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_match_encoding(
    ଅକ୍ଷର			*p_encoding,
    ସଂରଚନା prt_ps_mbfont_S	*p_cmap,
    ସଂରଚନା prt_ps_encoding_S	**pp_mbenc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				mbenc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				enc_len;
    ସଂରଚନା prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ଖାଲି;
    // Look for recognised encoding
    enc_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    ପାଇଁ (mbenc = 0; mbenc < p_cmap->ଅଙ୍କ_encodings; mbenc++)
    {
	ଯଦି (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ଫେରନ୍ତୁ TRUE;
	}
	p_mbenc++;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_match_charset(
    ଅକ୍ଷର		    *p_charset,
    ସଂରଚନା prt_ps_mbfont_S  *p_cmap,
    ସଂରଚନା prt_ps_charset_S **pp_mbchar)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    mbchar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    char_len;
    ସଂରଚନା prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ଯଦି (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    ପାଇଁ (mbchar = 0; mbchar < p_cmap->ଅଙ୍କ_charsets; mbchar++)
    {
	ଯଦି (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ଫେରନ୍ତୁ TRUE;
	}
	p_mbchar++;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ଅକ୍ଷର	*paper_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		paper_strlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fontsize;
    char_u	*p;
    ଦ୍ୱିଗୁଣ      ବାମ;
    ଦ୍ୱିଗୁଣ      ଡାହାଣ;
    ଦ୍ୱିଗୁଣ      ଶୀର୍ଷ;
    ଦ୍ୱିଗୁଣ      bottom;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmap = 0;
    char_u	*p_encoding;
    ସଂରଚନା prt_ps_encoding_S *p_mbenc;
    ସଂରଚନା prt_ps_encoding_S *p_mbenc_first;
    ସଂରଚନା prt_ps_charset_S  *p_mbchar = ଖାଲି;

# ଯଦି 0
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
    ଯଦି (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ଯଦି (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ଖାଲି;
    props = enc_canon_props(p_encoding);
    ଯଦି (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmap_first = 0;

	p_mbenc_first = ଖାଲି;
	ପାଇଁ (cmap = 0; cmap < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    ଯଦି (prt_match_encoding((ଅକ୍ଷର *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ଯଦି (p_mbenc_first == ଖାଲି)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		ଯଦି (prt_match_charset((ଅକ୍ଷର *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    ଭାଙ୍ଗ;
	    }

	// Use first encoding matched if no charset matched
	ଯଦି (p_mbchar == ଖାଲି && p_mbenc_first != ଖାଲି)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != ଖାଲି);
    ଯଦି (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ଖାଲି);
	ଯଦି (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ଯଦି ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		ଫେରନ୍ତୁ FALSE;
	    }

	    // Add charset name if not empty
	    ଯଦି (p_mbchar->cmap_charset != ଖାଲି)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, ଆକାର(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ଅନ୍ୟଥା
	{
	    // Add custom CMap character set name
	    ଯଦି (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		ଫେରନ୍ତୁ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, ଆକାର(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ଯଦି (p_mbenc->cmap_encoding != ଖାଲି && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < ଆକାର(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ଯଦି (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ଫେରନ୍ତୁ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ଯଦି (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    ଫେରନ୍ତୁ FALSE;
	ଯଦି (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ଯଦି (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		ଫେରନ୍ତୁ FALSE;
	ଯଦି (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ଯଦି (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		ଫେରନ୍ତୁ FALSE;
	ଯଦି (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ଯଦି (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		ଫେରନ୍ତୁ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ଯଦି (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ଯଦି (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ଅନ୍ୟଥା
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ଅନ୍ୟଥା
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ଯଦି (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (ଅକ୍ଷର *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    ଅନ୍ୟଥା
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PRT_MEDIASIZE_LEN; ++i)
	ଯଦି (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    ଭାଙ୍ଗ;
    ଯଦି (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ଯଦି (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    ଅନ୍ୟଥା
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &ବାମ, &ଡାହାଣ, &ଶୀର୍ଷ,
								    &bottom);
    prt_ବାମ_margin = (ଦଶମିକ)ବାମ;
    prt_ଡାହାଣ_margin = (ଦଶମିକ)ଡାହାଣ;
    prt_ଶୀର୍ଷ_margin = (ଦଶମିକ)ଶୀର୍ଷ;
    prt_bottom_margin = (ଦଶମିକ)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    ପାଇଁ (p = p_pfn; (p = vim_strchr(p, ':')) != ଖାଲି; ++p)
	ଯଦି (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((ଅକ୍ଷର *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_ଧାଡ଼ି = prt_get_cpl();
    psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ଯଦି (psettings->chars_per_ଧାଡ଼ି <= 0 || psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page <= 0)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_ଅଙ୍କ_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    ଯଦି (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ଅନ୍ୟଥା
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_ଅଙ୍କ_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    ଯଦି (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ଯଦି (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ଅନ୍ୟଥା ଯଦି (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ଯଦି (psettings->outfile == ଖାଲି)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ଯଦି (prt_ps_file_name == ଖାଲି)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ଫେରନ୍ତୁ FAIL;
	}
	prt_ps_fd = mch_fopen((ଅକ୍ଷର *)prt_ps_file_name, WRITEBIN);
    }
    ଅନ୍ୟଥା
    {
	p = expand_env_save(psettings->outfile);
	ଯଦି (p != ଖାଲି)
	{
	    prt_ps_fd = mch_fopen((ଅକ୍ଷର *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ଯଦି (prt_ps_fd == ଖାଲି)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ଫେରନ୍ତୁ FAIL;
    }

    prt_bufsiz = psettings->chars_per_ଧାଡ଼ି;
    ଯଦି (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, ଆକାର(ଅକ୍ଷର), prt_bufsiz);

    prt_page_ଅଙ୍କ = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_add_resource(ସଂରଚନା prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((ଅକ୍ଷର *)resource->filename, READBIN);
    ଯଦି (fd_resource == ଖାଲି)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ଫେରନ୍ତୁ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (ଅକ୍ଷର *)resource->title);

    prt_dsc_textline("BeginDocument", (ଅକ୍ଷର *)resource->filename);

    ପାଇଁ (;;)
    {
	bytes_read = fread((ଅକ୍ଷର *)resource_buffer, ଆକାର(char_u),
			   ଆକାର(resource_buffer), fd_resource);
	ଯଦି (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ଫେରନ୍ତୁ FALSE;
	}
	ଯଦି (bytes_read == 0)
	    ଭାଙ୍ଗ;
	prt_write_file_raw_len(resource_buffer, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bytes_read);
	ଯଦି (prt_file_error)
	{
	    fclose(fd_resource);
	    ଫେରନ୍ତୁ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ଫେରନ୍ତୁ TRUE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin(prt_settings_T *psettings)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bbox[4];
    ଦ୍ୱିଗୁଣ      ବାମ;
    ଦ୍ୱିଗୁଣ      ଡାହାଣ;
    ଦ୍ୱିଗୁଣ      ଶୀର୍ଷ;
    ଦ୍ୱିଗୁଣ      bottom;
    ସଂରଚନା prt_ps_resource_S *res_prolog;
    ସଂରଚନା prt_ps_resource_S *res_encoding;
    ଅକ୍ଷର	buffer[256];
    char_u      *p_encoding;
    ସଂରଚନା prt_ps_resource_S *res_cidfont;
    ସଂରଚନା prt_ps_resource_S *res_cmap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    res_prolog = ALLOC_ONE(ସଂରଚନା prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ସଂରଚନା prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ସଂରଚନା prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ସଂରଚନା prt_ps_resource_S);
    ଯଦି (res_prolog == ଖାଲି || res_encoding == ଖାଲି
	    || res_cidfont == ଖାଲି || res_cmap == ଖାଲି)
	ଯାଅ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (ଅକ୍ଷର *)psettings->jobname);
    ଯଦି (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(ଖାଲି), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &ବାମ, &ଡାହାଣ, &ଶୀର୍ଷ, &bottom);
    bbox[0] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ବାମ;
    ଯଦି (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଶୀର୍ଷ - (psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page + prt_header_height())
						    * (ଦ୍ୱିଗୁଣ)prt_ଧାଡ଼ି_height);
	bbox[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ବାମ + psettings->chars_per_ଧାଡ଼ି
					       * (ଦ୍ୱିଗୁଣ)prt_char_width + 0.5);
	bbox[3] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଶୀର୍ଷ + 0.5);
    }
    ଅନ୍ୟଥା
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bottom;
	bbox[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ବାମ + ((psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page
			      + prt_header_height()) * prt_ଧାଡ଼ି_height) + 0.5);
	bbox[3] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(bottom + psettings->chars_per_ଧାଡ଼ି
					       * (ଦ୍ୱିଗୁଣ)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(ଦ୍ୱିଗୁଣ)0, ଖାଲି, ଖାଲି);
    // Define fonts needed
    ଯଦି (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ଯଦି (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ଖାଲି
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ଯଦି (!prt_custom_cmap)
	    prt_dsc_resources(ଖାଲି, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ଯଦି (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	ଯାଅ theend;
    }
    ଯଦି (!prt_open_resource(res_prolog))
	ଯାଅ theend;
    ଯଦି (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	ଯାଅ theend;
    ଯଦି (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ଯଦି (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    ଯାଅ theend;
	}
	ଯଦି (!prt_open_resource(res_cidfont))
	    ଯାଅ theend;
	ଯଦି (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    ଯାଅ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ଯଦି (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ଯଦି (*p_encoding == NUL
		|| !prt_find_resource((ଅକ୍ଷର *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ଯଦି (!(props & ENC_8BIT)
		    || !prt_find_resource((ଅକ୍ଷର *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ଯଦି (!prt_find_resource((ଅକ୍ଷର *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    ଯାଅ theend;
		}
	    }
	}
	ଯଦି (!prt_open_resource(res_encoding))
	    ଯାଅ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ଅନ୍ୟଥା
    {
	p_encoding = enc_skip(p_penc);
	ଯଦି (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ଯଦି (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ଯଦି (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		ଯାଅ theend;
	    }
	    ଯଦି (!prt_open_resource(res_encoding))
		ଯାଅ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ଯଦି (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	ଯଦି (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    ଯାଅ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ଯଦି (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ଯଦି (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    ଯାଅ theend;
	}
	ଯଦି (!prt_open_resource(res_cmap))
	    ଯାଅ theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    ଯଦି (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(ଖାଲି, "procset", buffer);

	ଯଦି (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(ଖାଲି, "cmap", buffer);
	}
    }
    ଯଦି (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(ଖାଲି, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ଅନ୍ୟଥା
					0
# endif
					, prt_ଅଙ୍କ_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ଯଦି (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ଯଦି (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ଖାଲି : "PageResources"),
							    &prt_ps_mb_font);
	ଯଦି (!prt_custom_cmap)
	    prt_dsc_resources(ଖାଲି, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ଯଦି (!prt_add_resource(res_prolog))
	ଯାଅ theend;
    ଯଦି (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ଯଦି (!prt_add_resource(res_cidfont))
	    ଯାଅ theend;
	ଯଦି (prt_custom_cmap && !prt_add_resource(res_cmap))
	    ଯାଅ theend;
    }

    ଯଦି (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ଯଦି (!prt_add_resource(res_encoding))
	    ଯାଅ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_mediasize[prt_media].width);
    prt_write_int((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_ଅଙ୍କ_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    ଯଦି (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ଯଦି (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (ଅକ୍ଷର *)p_encoding, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (ଅକ୍ଷର *)p_encoding, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (ଅକ୍ଷର *)p_encoding, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (ଅକ୍ଷର *)p_encoding, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ଯଦି (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ଯଦି (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ଯଦି (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ଖାଲି)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ଯଦି (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ଅନ୍ୟଥା
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ଯଦି (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ଖାଲି)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ଯଦି (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ଅନ୍ୟଥା
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ଯଦି (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ଖାଲି)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ଯଦି (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_ଧାଡ଼ି_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ଅନ୍ୟଥା
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_ଧାଡ଼ି_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_ଧାଡ଼ି_height,
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

    ଫେରନ୍ତୁ retval;
}

    ଶୂନ୍ୟ
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_ଅଙ୍କ);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ଯଦି (!prt_file_error && psettings->outfile == ଖାଲି
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ଯଦି (prt_ps_fd != ଖାଲି)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ଖାଲି;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ଯଦି (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ଅନ୍ୟଥା
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_end_page(ଶୂନ୍ୟ)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ଫେରନ୍ତୁ !prt_file_error;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin_page(char_u *str UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		page_ଅଙ୍କ[2];

    prt_page_ଅଙ୍କ++;

    page_ଅଙ୍କ[0] = page_ଅଙ୍କ[1] = prt_page_ଅଙ୍କ;
    prt_dsc_ints("Page", 2, page_ଅଙ୍କ);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    ଯଦି (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    ଅନ୍ୟଥା
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ଯଦି (!prt_portrait)
    {
	prt_write_int(-((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ଫେରନ୍ତୁ !prt_file_error;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_blank_page(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (mch_print_begin_page(ଖାଲି) ? (mch_print_end_page()) : FALSE);
}

ସ୍ଥିର ଦଶମିକ prt_pos_x = 0;
ସ୍ଥିର ଦଶମିକ prt_pos_y = 0;

    ଶୂନ୍ୟ
mch_print_start_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଧାଡ଼ି)
{
    prt_pos_x = prt_ବାମ_margin;
    ଯଦି (margin)
	prt_pos_x -= prt_ସଂଖ୍ୟା_width;

    prt_pos_y = prt_ଶୀର୍ଷ_margin - prt_first_ଧାଡ଼ି_height -
					page_ଧାଡ଼ି * prt_ଧାଡ଼ି_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_text_out(char_u *textp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len UNUSED)
{
    char_u	*p = textp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    ଦଶମିକ       char_width;
    ଦଶମିକ       next_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_ascii;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		half_width;
    char_u	*tofree = ଖାଲି;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ଯଦି (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	ଯଦି (prt_in_ascii)
	{
	    ଯଦି (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ଯଦି (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	ଯଦି (half_width)
	    char_width /= 2;
	ଯଦି (prt_half_width)
	{
	    ଯଦି (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ଯଦି (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	ଯଦି (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ଯଦି (prt_need_font)
	{
	    ଯଦି (!prt_in_ascii)
		prt_write_string("CF");
	    ଅନ୍ୟଥା
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ଯଦି (prt_need_fgcol)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ଯଦି (r == g && g == b)
		prt_write_string("g\n");
	    ଅନ୍ୟଥା
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ଯଦି (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ଯଦି (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ଅନ୍ୟଥା
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ଯଦି (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ଯଦି (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ଯଦି (p == ଖାଲି)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ଯଦି (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ଯେତେବେଳେ (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    ଅନ୍ୟଥା
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ଯଦି (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    ବାଛ (ch)
	    {
		ସ୍ଥିତି BS:   ga_append(&prt_ps_buffer, 'b'); ଭାଙ୍ଗ;
		ସ୍ଥିତି TAB:  ga_append(&prt_ps_buffer, 't'); ଭାଙ୍ଗ;
		ସ୍ଥିତି NL:   ga_append(&prt_ps_buffer, 'n'); ଭାଙ୍ଗ;
		ସ୍ଥିତି FF:   ga_append(&prt_ps_buffer, 'f'); ଭାଙ୍ଗ;
		ସ୍ଥିତି CAR:  ga_append(&prt_ps_buffer, 'r'); ଭାଙ୍ଗ;
		ସ୍ଥିତି '(':  ga_append(&prt_ps_buffer, '('); ଭାଙ୍ଗ;
		ସ୍ଥିତି ')':  ga_append(&prt_ps_buffer, ')'); ଭାଙ୍ଗ;
		ସ୍ଥିତି '\\': ga_append(&prt_ps_buffer, '\\'); ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			   sprintf((ଅକ୍ଷର *)ch_buff, "%03o", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_ଡାହାଣ_margin) &&
		    ((next_pos - prt_ଡାହାଣ_margin) > (prt_ଡାହାଣ_margin*1e-5));

    ଯଦି (need_break)
	prt_flush_buffer();

    ଫେରନ୍ତୁ need_break;
}

    ଶୂନ୍ୟ
mch_print_set_font(ପୂର୍ଣ୍ଣସଂଖ୍ୟା iBold, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iItalic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iUnderline)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		font = 0;

    ଯଦି (iBold)
	font |= 0x01;
    ଯଦି (iItalic)
	font |= 0x02;

    ଯଦି (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ଯଦି (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    ଶୂନ୍ୟ
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    ଶୂନ୍ୟ
mch_print_set_fg(long_u fgcol)
{
    ଯଦି (fgcol == (long_u)prt_fgcol)
	ଫେରନ୍ତୁ;

    prt_fgcol = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
