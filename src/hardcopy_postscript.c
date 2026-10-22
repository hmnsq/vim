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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_ಕೋಷ್ಟಕ_T mbfont_opts[OPT_MBFONT_ಅಂಕೆ_OPTIONS] =
{
    {"c",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
    {"a",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
    {"r",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
    {"b",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
    {"i",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
    {"o",	FALSE, 0, ಖಾಲಿ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_ಬಫರ್_ಗಾತ್ರ  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
ರಚನೆ prt_ps_font_S
{
    ಪೂರ್ಣಾಂಕ		wx;
    ಪೂರ್ಣಾಂಕ		uline_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		uline_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		bbox_min_y;
    ಪೂರ್ಣಾಂಕ		bbox_max_y;
    ಅಕ್ಷರ	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
ಸ್ಥಿರ ರಚನೆ prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
ಸ್ಥಿರ ರಚನೆ prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ}
};

// Pointer to current font set being used
ಸ್ಥಿರ ರಚನೆ prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
ರಚನೆ prt_ps_encoding_S
{
    ಅಕ್ಷರ	*encoding;
    ಅಕ್ಷರ	*cmap_encoding;
    ಪೂರ್ಣಾಂಕ		needs_charset;
};

ರಚನೆ prt_ps_charset_S
{
    ಅಕ್ಷರ	*charset;
    ಅಕ್ಷರ	*cmap_charset;
    ಪೂರ್ಣಾಂಕ		has_charset;
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
ಸ್ಥಿರ ರಚನೆ prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ಖಾಲಿ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
ಸ್ಥಿರ ರಚನೆ prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ಖಾಲಿ,       CS_JIS_X_1983},
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
ಸ್ಥಿರ ರಚನೆ prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ಖಾಲಿ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ಖಾಲಿ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
ಸ್ಥಿರ ರಚನೆ prt_ps_charset_S sc_charsets[] =
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
ಸ್ಥಿರ ರಚನೆ prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ಖಾಲಿ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
ಸ್ಥಿರ ರಚನೆ prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ಖಾಲಿ,	CS_B5},
    {"CP950",	    ಖಾಲಿ,	CS_B5},
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
ಸ್ಥಿರ ರಚನೆ prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ಖಾಲಿ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
ಸ್ಥಿರ ರಚನೆ prt_ps_charset_S k_charsets[] =
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
ರಚನೆ prt_ps_mbfont_S
{
    ಪೂರ್ಣಾಂಕ				ಅಂಕೆ_encodings;
    ರಚನೆ prt_ps_encoding_S	*encodings;
    ಪೂರ್ಣಾಂಕ				ಅಂಕೆ_charsets;
    ರಚನೆ prt_ps_charset_S	*charsets;
    ಅಕ್ಷರ			*ascii_enc;
    ಅಕ್ಷರ			*defcs;
};

ಸ್ಥಿರ ರಚನೆ prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_ಉದ್ದಳತೆ(j_encodings),
	j_encodings,
	ARRAY_ಉದ್ದಳತೆ(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_ಉದ್ದಳತೆ(sc_encodings),
	sc_encodings,
	ARRAY_ಉದ್ದಳತೆ(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_ಉದ್ದಳತೆ(tc_encodings),
	tc_encodings,
	ARRAY_ಉದ್ದಳತೆ(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_ಉದ್ದಳತೆ(k_encodings),
	k_encodings,
	ARRAY_ಉದ್ದಳತೆ(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

ರಚನೆ prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    ಪೂರ್ಣಾಂಕ     type;
    char_u  title[256];
    char_u  ಆವೃತ್ತಿ_2[256];
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
# define PRT_PROLOG_ಆವೃತ್ತಿ  ((char_u *)"1.4")
# define PRT_CID_PROLOG_ಆವೃತ್ತಿ  ((char_u *)"1.0")

// String versions of PS resource types - indexed by constants above so don't
// re-order!
ಸ್ಥಿರ ಅಕ್ಷರ *prt_resource_types[] =
{
    "procset",
    "encoding",
    "cmap"
};

// Strings to look for in a PS resource file
# define PRT_RESOURCE_ಶೀರ್ಷಿಕೆ	    "%!PS-Adobe-"
# define PRT_RESOURCE_RESOURCE	    "Resource-"
# define PRT_RESOURCE_PROCSET	    "ProcSet"
# define PRT_RESOURCE_ENCODING	    "Encoding"
# define PRT_RESOURCE_CMAP	    "CMap"


// Data for table based DSC comment recognition, easy to extend if VIM needs to
// read more comments.
# define PRT_DSC_MISC_TYPE	    (-1)
# define PRT_DSC_TITLE_TYPE	    (1)
# define PRT_DSC_ಆವೃತ್ತಿ_TYPE	    (2)
# define PRT_DSC_ENDCOMMENTS_TYPE    (3)

# define PRT_DSC_TITLE		    "%%Title:"
# define PRT_DSC_ಆವೃತ್ತಿ		    "%%Version:"
# define PRT_DSC_ENDCOMMENTS	    "%%EndComments:"

ರಚನೆ prt_dsc_comment_S
{
    ಅಕ್ಷರ	*string;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		type;
};

ರಚನೆ prt_dsc_ಸಾಲು_S
{
    ಪೂರ್ಣಾಂಕ		type;
    char_u	*string;
    ಪೂರ್ಣಾಂಕ		len;
};


# define SIZEOF_CSTR(s)      (ಗಾತ್ರ(s) - 1)
ಸ್ಥಿರ ರಚನೆ prt_dsc_comment_S prt_dsc_ಕೋಷ್ಟಕ[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_ಆವೃತ್ತಿ,     SIZEOF_CSTR(PRT_DSC_ಆವೃತ್ತಿ),
							PRT_DSC_ಆವೃತ್ತಿ_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

ಸ್ಥಿರ ಶೂನ್ಯ prt_write_file_len(char_u *ಬಫರ್, ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_ಮುಂದಿನ_dsc(ರಚನೆ prt_dsc_ಸಾಲು_S *p_dsc_ಸಾಲು);

/*
 * Variables for the output PostScript file.
 */
ಸ್ಥಿರ FILE *prt_ps_fd;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_file_error;
ಸ್ಥಿರ char_u *prt_ps_file_name = ಖಾಲಿ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಪುಟ_ಅಗಲ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಪುಟ_ಎತ್ತರ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಎಡ_margin;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಬಲ_margin;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ತುದಿ_margin;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_bottom_margin;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಸಾಲು_ಎತ್ತರ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಮೊದಲ_ಸಾಲು_ಎತ್ತರ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_char_ಅಗಲ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಸಂಖ್ಯೆ_ಅಗಲ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_bgcol_ಸ್ಥಾನಾಂತರ;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_pos_x_moveto = 0.0;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_need_moveto;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_do_moveto;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_need_font;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_font;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_need_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_do_underline;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_need_fgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_fgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_need_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_do_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_new_bgcol;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_attribute_change;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_ಪಠ್ಯ_ಸತತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_ಪುಟ_ಅಂಕೆ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_media;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_portrait;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_ಅಂಕೆ_copies;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_duplex;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_tumble;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_collate;

/*
 * Buffers used when generating PostScript output
 */
ಸ್ಥಿರ char_u prt_ಸಾಲು_ಬಫರ್[257];
ಸ್ಥಿರ garray_T prt_ps_ಬಫರ್;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_do_conv;
ಸ್ಥಿರ vimconv_T prt_conv;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_ಹೊರಹೋಗು_mbyte;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_custom_cmap;
ಸ್ಥಿರ ಅಕ್ಷರ prt_cmap[80];
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_use_courier;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_ಒಳ_ascii;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_half_ಅಗಲ;
ಸ್ಥಿರ ಅಕ್ಷರ *prt_ascii_encoding;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char_u prt_hexchar[] = "0123456789abcdef";

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file_raw_len(char_u *ಬಫರ್, ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು)
{
    ಆದರೆ (!prt_file_error
	    && fwrite(ಬಫರ್, ಗಾತ್ರ(char_u), ಬೈಟುಗಳು, prt_ps_fd)
							     != (size_t)ಬೈಟುಗಳು)
    {
	emsg(_(e_error_writing_ಗಮ್ಯ_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file(char_u *ಬಫರ್)
{
    prt_write_file_len(ಬಫರ್, (ಪೂರ್ಣಾಂಕ)STRLEN(ಬಫರ್));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_file_len(char_u *ಬಫರ್, ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು)
{
    prt_write_file_raw_len(ಬಫರ್, ಬೈಟುಗಳು);
}

/*
 * Write a string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_string(ಅಕ್ಷರ *s)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್), "%s", s);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write an int and a space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_int(ಪೂರ್ಣಾಂಕ i)
{
    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "%d ", i);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write a boolean and a space.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_boolean(ಪೂರ್ಣಾಂಕ b)
{
    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_font(
    ಅಕ್ಷರ	*new_name,
    ಅಕ್ಷರ	*encoding,
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ,
    ಅಕ್ಷರ	*font)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "/%s %d %f /_%s sffs\n",
		       new_name, ಎತ್ತರ, 500./prt_ps_courier_font.wx, new_name);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
			     "/%s %d /_%s ffs\n", new_name, ಎತ್ತರ, new_name);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write a line to define the CID font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_cidfont(ಅಕ್ಷರ *new_name, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಅಕ್ಷರ *cidfont)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
			     "/%s %d /_%s ffs\n", new_name, ಎತ್ತರ, new_name);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dup_cidfont(ಅಕ್ಷರ *original_name, ಅಕ್ಷರ *new_name)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_real_ಬಿಟ್ಟುಗಳು(
    ದ್ವಿಗುಣ      real,
    ಪೂರ್ಣಾಂಕ		precision,
    ಪೂರ್ಣಾಂಕ		*pinteger,
    ಪೂರ್ಣಾಂಕ		*pfraction)
{
    ಪೂರ್ಣಾಂಕ     i;
    ಪೂರ್ಣಾಂಕ     integer;
    ದಶಮಾಂಶ   fraction;

    integer = (ಪೂರ್ಣಾಂಕ)real;
    fraction = (ದಶಮಾಂಶ)(real - integer);
    ಆದರೆ (real < (ದ್ವಿಗುಣ)integer)
	fraction = -fraction;
    ಗಾಗಿ (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (ಪೂರ್ಣಾಂಕ)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_write_real(ದ್ವಿಗುಣ val, ಪೂರ್ಣಾಂಕ prec)
{
    ಪೂರ್ಣಾಂಕ     integer;
    ಪೂರ್ಣಾಂಕ     fraction;

    prt_real_ಬಿಟ್ಟುಗಳು(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "%d", integer);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    // Only emit fraction if necessary
    ಆದರೆ (fraction != 0)
    {
	// Remove any trailing zeros
	ಆಗ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ".%0*d", prec, fraction);
	prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    }
    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, " ");
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

/*
 * Write a line to define a numeric variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_def_var(ಅಕ್ಷರ *name, ದ್ವಿಗುಣ ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ prec)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
								"/%s ", name);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    prt_write_real(ಮೌಲ್ಯ, prec);
    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "d\n");
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_ಗಮ್ಯ_USER(scale, ಗಾತ್ರ_2)    ((ಗಾತ್ರ_2) * ((scale)/1000.0))

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_flush_ಬಫರ್(ಶೂನ್ಯ)
{
    ಆದರೆ (prt_ps_ಬಫರ್.ga_len <= 0)
	ಹಿಂತಿರುಗಿ;

    // Any background color must be drawn first
    ಆದರೆ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	ಪೂರ್ಣಾಂಕ     r, g, b;

	ಆದರೆ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_ಪಠ್ಯ_ಸತತ, 2);
	prt_write_real(prt_ಸಾಲು_ಎತ್ತರ, 2);

	// Lastly add the color of the background
	r = ((ಚಿಹ್ನೆರಹಿತ)prt_new_bgcol & 0xff0000) >> 16;
	g = ((ಚಿಹ್ನೆರಹಿತ)prt_new_bgcol & 0xff00) >> 8;
	b = prt_new_bgcol & 0xff;
	prt_write_real(r / 255.0, 3);
	prt_write_real(g / 255.0, 3);
	prt_write_real(b / 255.0, 3);
	prt_write_string("bg\n");
    }
    // Draw underlines before the text as it makes it slightly easier to
    // find the starting point.
    ಆದರೆ (prt_do_underline)
    {
	ಆದರೆ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Underline length of text run
	prt_write_real(prt_ಪಠ್ಯ_ಸತತ, 2);
	prt_write_string("ul\n");
    }
    // Draw the text
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	prt_write_string("<");
    ಇಲ್ಲದಿದ್ದರೆ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_ಬಫರ್.ga_ದತ್ತಾಂಶ, prt_ps_ಬಫರ್.ga_len);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	prt_write_string(">");
    ಇಲ್ಲದಿದ್ದರೆ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ಆದರೆ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_ಬಫರ್);
    ga_init2(&prt_ps_ಬಫರ್, ಗಾತ್ರ(ಅಕ್ಷರ), prt_bufsiz);
}


    ಸ್ಥಿರ ಶೂನ್ಯ
prt_resource_name(char_u *filename, ಶೂನ್ಯ *cookie)
{
    char_u *resource_filename = cookie;

    ಆದರೆ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(resource_filename, filename);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_ಕಂಡುಹಿಡಿ_resource(ಅಕ್ಷರ *name, ರಚನೆ prt_ps_resource_S *resource)
{
    char_u	*ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		retval;

    ಬಫರ್ = alloc(MAXPATHL + 1);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(ಬಫರ್, "print");
    ಕೂಡಿಸು_pathsep(ಬಫರ್);
    vim_strcat(ಬಫರ್, (char_u *)name, MAXPATHL);
    vim_strcat(ಬಫರ್, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_ಒಳ_runtimepath(ಬಫರ್, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(ಬಫರ್);
    ಹಿಂತಿರುಗಿ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_ಬಫರ್_LEN (2048)
ರಚನೆ prt_resfile_ಬಫರ್_S
{
    char_u  ಬಫರ್[PRT_FILE_ಬಫರ್_LEN];
    ಪೂರ್ಣಾಂಕ     len;
    ಪೂರ್ಣಾಂಕ     ಸಾಲು_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ     ಸಾಲು_ಕೊನೆ;
};

ಸ್ಥಿರ ರಚನೆ prt_resfile_ಬಫರ್_S prt_resfile;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_resfile_ಮುಂದಿನ_ಸಾಲು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.ಸಾಲು_ಕೊನೆ + 1;
    ಆಗ (idx < prt_resfile.len)
    {
	ಆದರೆ (prt_resfile.ಬಫರ್[idx] != PSLF && prt_resfile.ಬಫರ್[idx] != PSCR)
	    ನಿಲ್ಲಿಸು;
	idx++;
    }
    prt_resfile.ಸಾಲು_ಆರಂಭ = idx;

    ಆಗ (idx < prt_resfile.len)
    {
	ಆದರೆ (prt_resfile.ಬಫರ್[idx] == PSLF || prt_resfile.ಬಫರ್[idx] == PSCR)
	    ನಿಲ್ಲಿಸು;
	idx++;
    }
    prt_resfile.ಸಾಲು_ಕೊನೆ = idx;

    ಹಿಂತಿರುಗಿ (idx < prt_resfile.len);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_resfile_strncmp(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಅಕ್ಷರ *string, ಪೂರ್ಣಾಂಕ len)
{
    // Force not equal if string is longer than remainder of line
    ಆದರೆ (len > (prt_resfile.ಸಾಲು_ಕೊನೆ - (prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ)))
	ಹಿಂತಿರುಗಿ 1;

    ಹಿಂತಿರುಗಿ STRNCMP(&prt_resfile.ಬಫರ್[prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ],
								string, len);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_resfile_skip_nonws(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ     idx;

    idx = prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ;
    ಆಗ (idx < prt_resfile.ಸಾಲು_ಕೊನೆ)
    {
	ಆದರೆ (ಸುರಕ್ಷಿತ_isspace(prt_resfile.ಬಫರ್[idx]))
	    ಹಿಂತಿರುಗಿ idx - prt_resfile.ಸಾಲು_ಆರಂಭ;
	idx++;
    }
    ಹಿಂತಿರುಗಿ -1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_resfile_skip_ws(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಪೂರ್ಣಾಂಕ     idx;

    idx = prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ;
    ಆಗ (idx < prt_resfile.ಸಾಲು_ಕೊನೆ)
    {
	ಆದರೆ (!ಸುರಕ್ಷಿತ_isspace(prt_resfile.ಬಫರ್[idx]))
	    ಹಿಂತಿರುಗಿ idx - prt_resfile.ಸಾಲು_ಆರಂಭ;
	idx++;
    }
    ಹಿಂತಿರುಗಿ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_ಮುಂದಿನ_dsc(ರಚನೆ prt_dsc_ಸಾಲು_S *p_dsc_ಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ     comment;
    ಪೂರ್ಣಾಂಕ     ಸ್ಥಾನಾಂತರ;

    // Move to start of next line
    ಆದರೆ (!prt_resfile_ಮುಂದಿನ_ಸಾಲು())
	ಹಿಂತಿರುಗಿ FALSE;

    // DSC comments always start %%
    ಆದರೆ (prt_resfile_strncmp(0, "%%", 2) != 0)
	ಹಿಂತಿರುಗಿ FALSE;

    // Find type of DSC comment
    ಗಾಗಿ (comment = 0; comment < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(prt_dsc_ಕೋಷ್ಟಕ); comment++)
	ಆದರೆ (prt_resfile_strncmp(0, prt_dsc_ಕೋಷ್ಟಕ[comment].string,
					    prt_dsc_ಕೋಷ್ಟಕ[comment].len) == 0)
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (comment != ARRAY_ಉದ್ದಳತೆ(prt_dsc_ಕೋಷ್ಟಕ))
    {
	// Return type of comment
	p_dsc_ಸಾಲು->type = prt_dsc_ಕೋಷ್ಟಕ[comment].type;
	ಸ್ಥಾನಾಂತರ = prt_dsc_ಕೋಷ್ಟಕ[comment].len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_ಸಾಲು->type = PRT_DSC_MISC_TYPE;
	ಸ್ಥಾನಾಂತರ = prt_resfile_skip_nonws(0);
	ಆದರೆ (ಸ್ಥಾನಾಂತರ == -1)
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    // Skip ws to comment value
    ಸ್ಥಾನಾಂತರ = prt_resfile_skip_ws(ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == -1)
	ಹಿಂತಿರುಗಿ FALSE;

    p_dsc_ಸಾಲು->string = &prt_resfile.ಬಫರ್[prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ];
    p_dsc_ಸಾಲು->len = prt_resfile.ಸಾಲು_ಕೊನೆ - (prt_resfile.ಸಾಲು_ಆರಂಭ + ಸ್ಥಾನಾಂತರ);

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_open_resource(ರಚನೆ prt_ps_resource_S *resource)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_all;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_title;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ_ಆವೃತ್ತಿ;
    FILE	*fd_resource;
    ರಚನೆ prt_dsc_ಸಾಲು_S dsc_ಸಾಲು;

    fd_resource = mch_fopen((ಅಕ್ಷರ *)resource->filename, READBIN);
    ಆದರೆ (fd_resource == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    CLEAR_ಕ್ಷೇತ್ರ(prt_resfile.ಬಫರ್);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (ಪೂರ್ಣಾಂಕ)fread((ಅಕ್ಷರ *)prt_resfile.ಬಫರ್, ಗಾತ್ರ(char_u),
					    PRT_FILE_ಬಫರ್_LEN, fd_resource);
    ಆದರೆ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.ಸಾಲು_ಕೊನೆ = -1;
    prt_resfile.ಸಾಲು_ಆರಂಭ = 0;
    ಆದರೆ (!prt_resfile_ಮುಂದಿನ_ಸಾಲು())
	ಹಿಂತಿರುಗಿ FALSE;

    ಸ್ಥಾನಾಂತರ = 0;

    ಆದರೆ (prt_resfile_strncmp(ಸ್ಥಾನಾಂತರ, PRT_RESOURCE_ಶೀರ್ಷಿಕೆ,
			       (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_ಶೀರ್ಷಿಕೆ)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Skip over any version numbers and following ws
    ಸ್ಥಾನಾಂತರ += (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_ಶೀರ್ಷಿಕೆ);
    ಸ್ಥಾನಾಂತರ = prt_resfile_skip_nonws(ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == -1)
	ಹಿಂತಿರುಗಿ FALSE;
    ಸ್ಥಾನಾಂತರ = prt_resfile_skip_ws(ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == -1)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (prt_resfile_strncmp(ಸ್ಥಾನಾಂತರ, PRT_RESOURCE_RESOURCE,
			     (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಸ್ಥಾನಾಂತರ += (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ಆದರೆ (prt_resfile_strncmp(ಸ್ಥಾನಾಂತರ, PRT_RESOURCE_PROCSET,
			      (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prt_resfile_strncmp(ಸ್ಥಾನಾಂತರ, PRT_RESOURCE_ENCODING,
			     (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prt_resfile_strncmp(ಸ್ಥಾನಾಂತರ, PRT_RESOURCE_CMAP,
				 (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->ಆವೃತ್ತಿ_2[0] = '\0';
    ನೋಡಿದ_title = FALSE;
    ನೋಡಿದ_ಆವೃತ್ತಿ = FALSE;
    ನೋಡಿದ_all = FALSE;
    ಆಗ (!ನೋಡಿದ_all && prt_ಮುಂದಿನ_dsc(&dsc_ಸಾಲು))
    {
	ಆಯ್ಕೆ (dsc_ಸಾಲು.type)
	{
	ಸ್ಥಿತಿ PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_ಸಾಲು.string, dsc_ಸಾಲು.len);
	    ನೋಡಿದ_title = TRUE;
	    ಆದರೆ (ನೋಡಿದ_ಆವೃತ್ತಿ)
		ನೋಡಿದ_all = TRUE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PRT_DSC_ಆವೃತ್ತಿ_TYPE:
	    vim_strncpy(resource->ಆವೃತ್ತಿ_2, dsc_ಸಾಲು.string, dsc_ಸಾಲು.len);
	    ನೋಡಿದ_ಆವೃತ್ತಿ = TRUE;
	    ಆದರೆ (ನೋಡಿದ_title)
		ನೋಡಿದ_all = TRUE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    ನೋಡಿದ_all = TRUE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (!ನೋಡಿದ_title || !ನೋಡಿದ_ಆವೃತ್ತಿ)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_check_resource(ರಚನೆ prt_ps_resource_S *resource, char_u *ಆವೃತ್ತಿ_2)
{
    // Version number m.n should match, the revision number does not matter
    ಆದರೆ (STRNCMP(resource->ಆವೃತ್ತಿ_2, ಆವೃತ್ತಿ_2, STRLEN(ಆವೃತ್ತಿ_2)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_ಆವೃತ್ತಿ),
		resource->name);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Other checks to be added as needed
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_ಆರಂಭ(ಶೂನ್ಯ)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_noarg(ಅಕ್ಷರ *comment)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
							 "%%%%%s\n", comment);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_textline(ಅಕ್ಷರ *comment, ಅಕ್ಷರ *ಪಠ್ಯ)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
					       "%%%%%s: %s\n", comment, ಪಠ್ಯ);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_ಪಠ್ಯ(ಅಕ್ಷರ *comment, ಅಕ್ಷರ *ಪಠ್ಯ)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
					     "%%%%%s: (%s)\n", comment, ಪಠ್ಯ);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

# define prt_dsc_atend(c)	prt_dsc_ಪಠ್ಯ((c), "atend")

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_ints(ಅಕ್ಷರ *comment, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ *ints)
{
    ಪೂರ್ಣಾಂಕ		i;

    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
							  "%%%%%s:", comment);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);

    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; i++)
    {
	sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, " %d", ints[i]);
	prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    }

    prt_write_string("\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_resources(
    ಅಕ್ಷರ	*comment,	// if NULL add to previous
    ಅಕ್ಷರ	*type,
    ಅಕ್ಷರ	*string)
{
    ಆದರೆ (comment != ಖಾಲಿ)
	vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
						 "%%%%%s: %s", comment, type);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
							    "%%%%+ %s", type);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);

    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
							     " %s\n", string);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_font_resource(ಅಕ್ಷರ *resource, ರಚನೆ prt_ps_font_S *ps_font)
{
    ಪೂರ್ಣಾಂಕ     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    ಗಾಗಿ (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ಆದರೆ (ps_font->ps_fontname[i] != ಖಾಲಿ)
	    prt_dsc_resources(ಖಾಲಿ, "font", ps_font->ps_fontname[i]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_requirements(
    ಪೂರ್ಣಾಂಕ		duplex,
    ಪೂರ್ಣಾಂಕ		tumble,
    ಪೂರ್ಣಾಂಕ		collate,
    ಪೂರ್ಣಾಂಕ		color,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ಆದರೆ (!(duplex || collate || color || (ಅಂಕೆ_copies > 1)))
	ಹಿಂತಿರುಗಿ;

    sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "%%%%Requirements:");
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);

    ಆದರೆ (duplex)
    {
	prt_write_string(" duplex");
	ಆದರೆ (tumble)
	    prt_write_string("(tumble)");
    }
    ಆದರೆ (collate)
	prt_write_string(" collate");
    ಆದರೆ (color)
	prt_write_string(" color");
    ಆದರೆ (ಅಂಕೆ_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, "%d", ಅಂಕೆ_copies);
	prt_write_file(prt_ಸಾಲು_ಬಫರ್);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_dsc_docmedia(
    ಅಕ್ಷರ	*paper_name,
    ದ್ವಿಗುಣ	ಅಗಲ,
    ದ್ವಿಗುಣ	ಎತ್ತರ,
    ದ್ವಿಗುಣ	ತೂಕ,
    ಅಕ್ಷರ	*ಬಣ್ಣ,
    ಅಕ್ಷರ	*type)
{
    vim_snprintf((ಅಕ್ಷರ *)prt_ಸಾಲು_ಬಫರ್, ಗಾತ್ರ(prt_ಸಾಲು_ಬಫರ್),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_ಸಾಲು_ಬಫರ್);
    prt_write_real(ಅಗಲ, 2);
    prt_write_real(ಎತ್ತರ, 2);
    prt_write_real(ತೂಕ, 2);
    ಆದರೆ (ಬಣ್ಣ == ಖಾಲಿ)
	prt_write_string("()");
    ಇಲ್ಲದಿದ್ದರೆ
	prt_write_string(ಬಣ್ಣ);
    prt_write_string(" ");
    ಆದರೆ (type == ಖಾಲಿ)
	prt_write_string("()");
    ಇಲ್ಲದಿದ್ದರೆ
	prt_write_string(type);
    prt_write_string("\n");
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	ಪೂರ್ಣಾಂಕ     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	ಗಾಗಿ (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ಆದರೆ (prt_ps_mb_font.ps_fontname[i] != ಖಾಲಿ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ಆದರೆ (prt_do_conv)
    {
	convert_ಸಿದ್ಧತೆ(&prt_conv, ಖಾಲಿ, ಖಾಲಿ);
	prt_do_conv = FALSE;
    }
    ಆದರೆ (prt_ps_fd != ಖಾಲಿ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ಖಾಲಿ;
	prt_file_error = FALSE;
    }
    ಆದರೆ (prt_ps_file_name != ಖಾಲಿ)
	VIM_CLEAR(prt_ps_file_name);
}

    ಸ್ಥಿರ ದಶಮಾಂಶ
ಗಮ್ಯ_device_units(ಪೂರ್ಣಾಂಕ idx, ದ್ವಿಗುಣ physsize, ಪೂರ್ಣಾಂಕ def_ಸಂಖ್ಯೆ)
{
    ದಶಮಾಂಶ	ret;
    ಪೂರ್ಣಾಂಕ		u;
    ಪೂರ್ಣಾಂಕ		nr;

    u = prt_get_unit(idx);
    ಆದರೆ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ಆಯ್ಕೆ (u)
    {
	ಸ್ಥಿತಿ PRT_UNIT_INCH:
	    ret = (ದಶಮಾಂಶ)(nr * PRT_PS_DEFAULT_DPI);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_MM:
	    ret = (ದಶಮಾಂಶ)(nr * PRT_PS_DEFAULT_DPI) / (ದಶಮಾಂಶ)25.4;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_ಬಿಂದು:
	    ret = (ದಶಮಾಂಶ)nr;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_PERC:
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ret = (ದಶಮಾಂಶ)(physsize * nr) / 100;
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prt_ಪುಟ_margins(
    ದ್ವಿಗುಣ	ಅಗಲ,
    ದ್ವಿಗುಣ	ಎತ್ತರ,
    ದ್ವಿಗುಣ	*ಎಡ_2,
    ದ್ವಿಗುಣ	*ಬಲ_2,
    ದ್ವಿಗುಣ	*ತುದಿ,
    ದ್ವಿಗುಣ	*bottom)
{
    *ಎಡ_2   = ಗಮ್ಯ_device_units(OPT_PRINT_ಎಡ, ಅಗಲ, 10);
    *ಬಲ_2  = ಅಗಲ - ಗಮ್ಯ_device_units(OPT_PRINT_ಬಲ, ಅಗಲ, 5);
    *ತುದಿ    = ಎತ್ತರ - ಗಮ್ಯ_device_units(OPT_PRINT_ತುದಿ, ಎತ್ತರ, 5);
    *bottom = ಗಮ್ಯ_device_units(OPT_PRINT_BOT, ಎತ್ತರ, 5);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prt_font_metrics(ಪೂರ್ಣಾಂಕ font_scale)
{
    prt_ಸಾಲು_ಎತ್ತರ = (ದಶಮಾಂಶ)font_scale;
    prt_char_ಅಗಲ = (ದಶಮಾಂಶ)PRT_PS_FONT_ಗಮ್ಯ_USER(font_scale, prt_ps_font->wx);
}


    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_get_cpl(ಶೂನ್ಯ)
{
    ಆದರೆ (prt_use_ಸಂಖ್ಯೆ())
    {
	prt_ಸಂಖ್ಯೆ_ಅಗಲ = PRINT_ಸಂಖ್ಯೆ_ಅಗಲ * prt_char_ಅಗಲ;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	    prt_ಸಂಖ್ಯೆ_ಅಗಲ /= 2;
	prt_ಎಡ_margin += prt_ಸಂಖ್ಯೆ_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	prt_ಸಂಖ್ಯೆ_ಅಗಲ = 0.0;

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)((prt_ಬಲ_margin - prt_ಎಡ_margin) / prt_char_ಅಗಲ);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_build_cid_fontname(ಪೂರ್ಣಾಂಕ font, char_u *name, ಪೂರ್ಣಾಂಕ name_len)
{
    ಅಕ್ಷರ    *fontname;

    fontname = alloc(name_len + 1);
    ಆದರೆ (fontname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_ಸ್ಥಾನಾಂತರ = (ದಶಮಾಂಶ)PRT_PS_FONT_ಗಮ್ಯ_USER(prt_ಸಾಲು_ಎತ್ತರ,
					   prt_ps_font->bbox_min_y);
    ಆದರೆ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_ಸ್ಥಾನಾಂತರ -= (ದಶಮಾಂಶ)PRT_PS_FONT_ಗಮ್ಯ_USER(prt_ಸಾಲು_ಎತ್ತರ,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_ಮೊದಲ_ಸಾಲು_ಎತ್ತರ = prt_ಸಾಲು_ಎತ್ತರ + prt_bgcol_ಸ್ಥಾನಾಂತರ;

    // Calculate lpp
    lpp = (ಪೂರ್ಣಾಂಕ)((prt_ತುದಿ_margin - prt_bottom_margin) / prt_ಸಾಲು_ಎತ್ತರ);

    // Adjust top margin if there is a header
    prt_ತುದಿ_margin -= prt_ಸಾಲು_ಎತ್ತರ * prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();

    ಹಿಂತಿರುಗಿ lpp - prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_ಹೊಂದಿಕೆ_encoding(
    ಅಕ್ಷರ			*p_encoding,
    ರಚನೆ prt_ps_mbfont_S	*p_cmap,
    ರಚನೆ prt_ps_encoding_S	**pp_mbenc)
{
    ಪೂರ್ಣಾಂಕ				mbenc;
    ಪೂರ್ಣಾಂಕ				enc_len;
    ರಚನೆ prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ಖಾಲಿ;
    // Look for recognised encoding
    enc_len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    ಗಾಗಿ (mbenc = 0; mbenc < p_cmap->ಅಂಕೆ_encodings; mbenc++)
    {
	ಆದರೆ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	p_mbenc++;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_ಹೊಂದಿಕೆ_charset(
    ಅಕ್ಷರ		    *p_charset,
    ರಚನೆ prt_ps_mbfont_S  *p_cmap,
    ರಚನೆ prt_ps_charset_S **pp_mbchar)
{
    ಪೂರ್ಣಾಂಕ			    mbchar;
    ಪೂರ್ಣಾಂಕ			    char_len;
    ರಚನೆ prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ಆದರೆ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    ಗಾಗಿ (mbchar = 0; mbchar < p_cmap->ಅಂಕೆ_charsets; mbchar++)
    {
	ಆದರೆ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	p_mbchar++;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    ಪೂರ್ಣಾಂಕ		forceit ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಅಕ್ಷರ	*paper_name;
    ಪೂರ್ಣಾಂಕ		paper_strlen;
    ಪೂರ್ಣಾಂಕ		fontsize;
    char_u	*p;
    ದ್ವಿಗುಣ      ಎಡ_2;
    ದ್ವಿಗುಣ      ಬಲ_2;
    ದ್ವಿಗುಣ      ತುದಿ;
    ದ್ವಿಗುಣ      bottom;
    ಪೂರ್ಣಾಂಕ		props;
    ಪೂರ್ಣಾಂಕ		cmap = 0;
    char_u	*p_encoding;
    ರಚನೆ prt_ps_encoding_S *p_mbenc;
    ರಚನೆ prt_ps_encoding_S *p_mbenc_ಮೊದಲ;
    ರಚನೆ prt_ps_charset_S  *p_mbchar = ಖಾಲಿ;

# ಆದರೆ 0
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
    ಆದರೆ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ಆದರೆ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ಖಾಲಿ;
    props = enc_canon_props(p_encoding);
    ಆದರೆ (!(props & ENC_8_ಬಿಟ್) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	ಪೂರ್ಣಾಂಕ cmap_ಮೊದಲ = 0;

	p_mbenc_ಮೊದಲ = ಖಾಲಿ;
	ಗಾಗಿ (cmap = 0; cmap < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(prt_ps_mbfonts); cmap++)
	    ಆದರೆ (prt_ಹೊಂದಿಕೆ_encoding((ಅಕ್ಷರ *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ಆದರೆ (p_mbenc_ಮೊದಲ == ಖಾಲಿ)
		{
		    p_mbenc_ಮೊದಲ = p_mbenc;
		    cmap_ಮೊದಲ = cmap;
		}
		ಆದರೆ (prt_ಹೊಂದಿಕೆ_charset((ಅಕ್ಷರ *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    ನಿಲ್ಲಿಸು;
	    }

	// Use first encoding matched if no charset matched
	ಆದರೆ (p_mbchar == ಖಾಲಿ && p_mbenc_ಮೊದಲ != ಖಾಲಿ)
	{
	    p_mbenc = p_mbenc_ಮೊದಲ;
	    cmap = cmap_ಮೊದಲ;
	}
    }

    prt_ಹೊರಹೋಗು_mbyte = (p_mbenc != ಖಾಲಿ);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ಖಾಲಿ);
	ಆದರೆ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ಆದರೆ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_ಗಣ));
		ಹಿಂತಿರುಗಿ FALSE;
	    }

	    // Add charset name if not empty
	    ಆದರೆ (p_mbchar->cmap_charset != ಖಾಲಿ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, ಗಾತ್ರ(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Add custom CMap character set name
	    ಆದರೆ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_ಖಾಲಿಗಳು_with_multi_byte_encoding));
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, ಗಾತ್ರ(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ಆದರೆ (p_mbenc->cmap_encoding != ಖಾಲಿ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < ಗಾತ್ರ(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ಆದರೆ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ಆದರೆ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ))
	    ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ಆದರೆ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ))
		ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ಆದರೆ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ))
		ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ಆದರೆ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ))
		ಹಿಂತಿರುಗಿ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ಆದರೆ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ಆದರೆ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ಇಲ್ಲದಿದ್ದರೆ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ಆದರೆ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (ಅಕ್ಷರ *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].ಅಕ್ಷರಸರಣಿ_ಉದ್ದ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)PRT_MEDIASIZE_LEN; ++i)
	ಆದರೆ (STRLEN(prt_mediasize[i].name) == (ಚಿಹ್ನೆರಹಿತ)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ಆದರೆ (prt_portrait)
    {
	prt_ಪುಟ_ಅಗಲ = prt_mediasize[i].ಅಗಲ;
	prt_ಪುಟ_ಎತ್ತರ = prt_mediasize[i].ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	prt_ಪುಟ_ಅಗಲ = prt_mediasize[i].ಎತ್ತರ;
	prt_ಪುಟ_ಎತ್ತರ = prt_mediasize[i].ಅಗಲ;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_ಪುಟ_margins(prt_ಪುಟ_ಅಗಲ, prt_ಪುಟ_ಎತ್ತರ, &ಎಡ_2, &ಬಲ_2, &ತುದಿ,
								    &bottom);
    prt_ಎಡ_margin = (ದಶಮಾಂಶ)ಎಡ_2;
    prt_ಬಲ_margin = (ದಶಮಾಂಶ)ಬಲ_2;
    prt_ತುದಿ_margin = (ದಶಮಾಂಶ)ತುದಿ;
    prt_bottom_margin = (ದಶಮಾಂಶ)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    ಗಾಗಿ (p = p_pfn; (p = vim_strchr(p, ':')) != ಖಾಲಿ; ++p)
	ಆದರೆ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((ಅಕ್ಷರ *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_ಪ್ರತಿ_ಸಾಲು = prt_get_cpl();
    psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ಆದರೆ (psettings->chars_ಪ್ರತಿ_ಸಾಲು <= 0 || psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ <= 0)
	ಹಿಂತಿರುಗಿ FAIL;

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
    ಆದರೆ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಆದರೆ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ಆದರೆ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ಆದರೆ (psettings->outfile == ಖಾಲಿ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ಆದರೆ (prt_ps_file_name == ಖಾಲಿ)
	{
	    emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	prt_ps_fd = mch_fopen((ಅಕ್ಷರ *)prt_ps_file_name, WRITEBIN);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = expand_env_save(psettings->outfile);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    prt_ps_fd = mch_fopen((ಅಕ್ಷರ *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ಆದರೆ (prt_ps_fd == ಖಾಲಿ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    prt_bufsiz = psettings->chars_ಪ್ರತಿ_ಸಾಲು;
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_ಬಫರ್, ಗಾತ್ರ(ಅಕ್ಷರ), prt_bufsiz);

    prt_ಪುಟ_ಅಂಕೆ = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_ಕೂಡಿಸು_resource(ರಚನೆ prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_ಬಫರ್[512];
    size_t	ಬೈಟುಗಳು_read;

    fd_resource = mch_fopen((ಅಕ್ಷರ *)resource->filename, READBIN);
    ಆದರೆ (fd_resource == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ಹಿಂತಿರುಗಿ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (ಅಕ್ಷರ *)resource->title);

    prt_dsc_textline("BeginDocument", (ಅಕ್ಷರ *)resource->filename);

    ಗಾಗಿ (;;)
    {
	ಬೈಟುಗಳು_read = fread((ಅಕ್ಷರ *)resource_ಬಫರ್, ಗಾತ್ರ(char_u),
			   ಗಾತ್ರ(resource_ಬಫರ್), fd_resource);
	ಆದರೆ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಆದರೆ (ಬೈಟುಗಳು_read == 0)
	    ನಿಲ್ಲಿಸು;
	prt_write_file_raw_len(resource_ಬಫರ್, (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು_read);
	ಆದರೆ (prt_file_error)
	{
	    fclose(fd_resource);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಪೂರ್ಣಾಂಕ
mch_print_begin(prt_settings_T *psettings)
{
    ಪೂರ್ಣಾಂಕ		bbox[4];
    ದ್ವಿಗುಣ      ಎಡ_2;
    ದ್ವಿಗುಣ      ಬಲ_2;
    ದ್ವಿಗುಣ      ತುದಿ;
    ದ್ವಿಗುಣ      bottom;
    ರಚನೆ prt_ps_resource_S *res_prolog;
    ರಚನೆ prt_ps_resource_S *res_encoding;
    ಅಕ್ಷರ	ಬಫರ್[256];
    char_u      *p_encoding;
    ರಚನೆ prt_ps_resource_S *res_cidfont;
    ರಚನೆ prt_ps_resource_S *res_cmap;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    res_prolog = ALLOC_ONE(ರಚನೆ prt_ps_resource_S);
    res_encoding = ALLOC_ONE(ರಚನೆ prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(ರಚನೆ prt_ps_resource_S);
    res_cmap = ALLOC_ONE(ರಚನೆ prt_ps_resource_S);
    ಆದರೆ (res_prolog == ಖಾಲಿ || res_encoding == ಖಾಲಿ
	    || res_cidfont == ಖಾಲಿ || res_cmap == ಖಾಲಿ)
	ಹೋಗು theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_ಆರಂಭ();
    prt_dsc_textline("Title", (ಅಕ್ಷರ *)psettings->jobname);
    ಆದರೆ (!get_user_name((char_u *)ಬಫರ್, 256))
	STRCPY(ಬಫರ್, "Unknown");
    prt_dsc_textline("For", ಬಫರ್);
    prt_dsc_textline("Creator", VIM_ಆವೃತ್ತಿ_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(ಸಮಯ(ಖಾಲಿ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_ಪುಟ_margins(prt_mediasize[prt_media].ಅಗಲ,
					    prt_mediasize[prt_media].ಎತ್ತರ,
					    &ಎಡ_2, &ಬಲ_2, &ತುದಿ, &bottom);
    bbox[0] = (ಪೂರ್ಣಾಂಕ)ಎಡ_2;
    ಆದರೆ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (ಪೂರ್ಣಾಂಕ)(ತುದಿ - (psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ + prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ())
						    * (ದ್ವಿಗುಣ)prt_ಸಾಲು_ಎತ್ತರ);
	bbox[2] = (ಪೂರ್ಣಾಂಕ)(ಎಡ_2 + psettings->chars_ಪ್ರತಿ_ಸಾಲು
					       * (ದ್ವಿಗುಣ)prt_char_ಅಗಲ + 0.5);
	bbox[3] = (ಪೂರ್ಣಾಂಕ)(ತುದಿ + 0.5);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (ಪೂರ್ಣಾಂಕ)bottom;
	bbox[2] = (ಪೂರ್ಣಾಂಕ)(ಎಡ_2 + ((psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ
			      + prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ()) * prt_ಸಾಲು_ಎತ್ತರ) + 0.5);
	bbox[3] = (ಪೂರ್ಣಾಂಕ)(bottom + psettings->chars_ಪ್ರತಿ_ಸಾಲು
					       * (ದ್ವಿಗುಣ)prt_char_ಅಗಲ + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].ಅಗಲ,
				prt_mediasize[prt_media].ಎತ್ತರ,
				(ದ್ವಿಗುಣ)0, ಖಾಲಿ, ಖಾಲಿ);
    // Define fonts needed
    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ಖಾಲಿ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ಆದರೆ (!prt_custom_cmap)
	    prt_dsc_resources(ಖಾಲಿ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ಆದರೆ (!prt_ಕಂಡುಹಿಡಿ_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps), "prolog");
	ಹೋಗು theend;
    }
    ಆದರೆ (!prt_open_resource(res_prolog))
	ಹೋಗು theend;
    ಆದರೆ (!prt_check_resource(res_prolog, PRT_PROLOG_ಆವೃತ್ತಿ))
	ಹೋಗು theend;
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ಆದರೆ (!prt_ಕಂಡುಹಿಡಿ_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps), "cidfont");
	    ಹೋಗು theend;
	}
	ಆದರೆ (!prt_open_resource(res_cidfont))
	    ಹೋಗು theend;
	ಆದರೆ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_ಆವೃತ್ತಿ))
	    ಹೋಗು theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ಆದರೆ (*p_encoding == NUL
		|| !prt_ಕಂಡುಹಿಡಿ_resource((ಅಕ್ಷರ *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    ಪೂರ್ಣಾಂಕ		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ಆದರೆ (!(props & ENC_8_ಬಿಟ್)
		    || !prt_ಕಂಡುಹಿಡಿ_resource((ಅಕ್ಷರ *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ಆದರೆ (!prt_ಕಂಡುಹಿಡಿ_resource((ಅಕ್ಷರ *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps),
			    p_encoding);
		    ಹೋಗು theend;
		}
	    }
	}
	ಆದರೆ (!prt_open_resource(res_encoding))
	    ಹೋಗು theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p_encoding = enc_skip(p_penc);
	ಆದರೆ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ಆದರೆ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ಆದರೆ (!prt_ಕಂಡುಹಿಡಿ_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		ಹೋಗು theend;
	    }
	    ಆದರೆ (!prt_open_resource(res_encoding))
		ಹೋಗು theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ಆದರೆ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8_ಬಿಟ್))
    {
	// Set up encoding conversion if required
	ಆದರೆ (FAIL == convert_ಸಿದ್ಧತೆ(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_ಗಮ್ಯ_convert_ಗಮ್ಯ_print_encoding_str), p_encoding);
	    ಹೋಗು theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ಆದರೆ (!prt_ಕಂಡುಹಿಡಿ_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps), prt_cmap);
	    ಹೋಗು theend;
	}
	ಆದರೆ (!prt_open_resource(res_cmap))
	    ಹೋಗು theend;
    }

    // List resources supplied
    STRCPY(ಬಫರ್, res_prolog->title);
    STRCAT(ಬಫರ್, " ");
    STRCAT(ಬಫರ್, res_prolog->ಆವೃತ್ತಿ_2);
    prt_dsc_resources("DocumentSuppliedResources", "procset", ಬಫರ್);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	STRCPY(ಬಫರ್, res_cidfont->title);
	STRCAT(ಬಫರ್, " ");
	STRCAT(ಬಫರ್, res_cidfont->ಆವೃತ್ತಿ_2);
	prt_dsc_resources(ಖಾಲಿ, "procset", ಬಫರ್);

	ಆದರೆ (prt_custom_cmap)
	{
	    STRCPY(ಬಫರ್, res_cmap->title);
	    STRCAT(ಬಫರ್, " ");
	    STRCAT(ಬಫರ್, res_cmap->ಆವೃತ್ತಿ_2);
	    prt_dsc_resources(ಖಾಲಿ, "cmap", ಬಫರ್);
	}
    }
    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte || prt_use_courier)
    {
	STRCPY(ಬಫರ್, res_encoding->title);
	STRCAT(ಬಫರ್, " ");
	STRCAT(ಬಫರ್, res_encoding->ಆವೃತ್ತಿ_2);
	prt_dsc_resources(ಖಾಲಿ, "encoding", ಬಫರ್);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ಇಲ್ಲದಿದ್ದರೆ
					0
# endif
					, prt_ಅಂಕೆ_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ಖಾಲಿ : "PageResources"),
							    &prt_ps_mb_font);
	ಆದರೆ (!prt_custom_cmap)
	    prt_dsc_resources(ಖಾಲಿ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ಆದರೆ (!prt_ಕೂಡಿಸು_resource(res_prolog))
	ಹೋಗು theend;
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ಆದರೆ (!prt_ಕೂಡಿಸು_resource(res_cidfont))
	    ಹೋಗು theend;
	ಆದರೆ (prt_custom_cmap && !prt_ಕೂಡಿಸು_resource(res_cmap))
	    ಹೋಗು theend;
    }

    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ಆದರೆ (!prt_ಕೂಡಿಸು_resource(res_encoding))
	    ಹೋಗು theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((ಪೂರ್ಣಾಂಕ)prt_mediasize[prt_media].ಅಗಲ);
    prt_write_int((ಪೂರ್ಣಾಂಕ)prt_mediasize[prt_media].ಎತ್ತರ);
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
    ಆದರೆ (!prt_ಹೊರಹೋಗು_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ಆದರೆ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (ಅಕ್ಷರ *)p_encoding, (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (ಅಕ್ಷರ *)p_encoding, (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (ಅಕ್ಷರ *)p_encoding, (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (ಅಕ್ಷರ *)p_encoding, (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ಆದರೆ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ಆದರೆ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ಖಾಲಿ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ಆದರೆ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ಆದರೆ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ಖಾಲಿ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ಆದರೆ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ಆದರೆ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ಖಾಲಿ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ಆದರೆ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (ಪೂರ್ಣಾಂಕ)prt_ಸಾಲು_ಎತ್ತರ,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_ಗಮ್ಯ_USER(prt_ಸಾಲು_ಎತ್ತರ,
						prt_ps_font->uline_ಸ್ಥಾನಾಂತರ), 2);
    prt_def_var("UW", PRT_PS_FONT_ಗಮ್ಯ_USER(prt_ಸಾಲು_ಎತ್ತರ,
						 prt_ps_font->uline_ಅಗಲ), 2);
    prt_def_var("BO", prt_bgcol_ಸ್ಥಾನಾಂತರ, 2);

    prt_dsc_noarg("EndSetup");

    // Fail if any problems writing out to the PS file
    retval = !prt_file_error;

theend:
    vim_free(res_prolog);
    vim_free(res_encoding);
    vim_free(res_cidfont);
    vim_free(res_cmap);

    ಹಿಂತಿರುಗಿ retval;
}

    ಶೂನ್ಯ
mch_print_ಕೊನೆ(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_ಪುಟ_ಅಂಕೆ);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ಆದರೆ (!prt_file_error && psettings->outfile == ಖಾಲಿ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ಆದರೆ (prt_ps_fd != ಖಾಲಿ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ಖಾಲಿ;
	}
	prt_ಸಂದೇಶ((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ಆದರೆ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_ಗಮ್ಯ_print_postscript_file));
	ಇಲ್ಲದಿದ್ದರೆ
	    prt_ಸಂದೇಶ((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    ಪೂರ್ಣಾಂಕ
mch_print_ಕೊನೆ_ಪುಟ(ಶೂನ್ಯ)
{
    prt_flush_ಬಫರ್();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ಹಿಂತಿರುಗಿ !prt_file_error;
}

    ಪೂರ್ಣಾಂಕ
mch_print_begin_ಪುಟ(char_u *str ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಅಂಕೆ[2];

    prt_ಪುಟ_ಅಂಕೆ++;

    ಪುಟ_ಅಂಕೆ[0] = ಪುಟ_ಅಂಕೆ[1] = prt_ಪುಟ_ಅಂಕೆ;
    prt_dsc_ints("Page", 2, ಪುಟ_ಅಂಕೆ);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_ಒಳ_ascii = !prt_ಹೊರಹೋಗು_mbyte;
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
	prt_write_string("CF0 sf\n");
    ಇಲ್ಲದಿದ್ದರೆ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ಆದರೆ (!prt_portrait)
    {
	prt_write_int(-((ಪೂರ್ಣಾಂಕ)prt_mediasize[prt_media].ಅಗಲ));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ಹಿಂತಿರುಗಿ !prt_file_error;
}

    ಪೂರ್ಣಾಂಕ
mch_print_blank_ಪುಟ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (mch_print_begin_ಪುಟ(ಖಾಲಿ) ? (mch_print_ಕೊನೆ_ಪುಟ()) : FALSE);
}

ಸ್ಥಿರ ದಶಮಾಂಶ prt_pos_x = 0;
ಸ್ಥಿರ ದಶಮಾಂಶ prt_pos_y = 0;

    ಶೂನ್ಯ
mch_print_ಆರಂಭ_ಸಾಲು(ಪೂರ್ಣಾಂಕ margin, ಪೂರ್ಣಾಂಕ ಪುಟ_ಸಾಲು)
{
    prt_pos_x = prt_ಎಡ_margin;
    ಆದರೆ (margin)
	prt_pos_x -= prt_ಸಂಖ್ಯೆ_ಅಗಲ;

    prt_pos_y = prt_ತುದಿ_margin - prt_ಮೊದಲ_ಸಾಲು_ಎತ್ತರ -
					ಪುಟ_ಸಾಲು * prt_ಸಾಲು_ಎತ್ತರ;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_ಅಗಲ = FALSE;
}

    ಪೂರ್ಣಾಂಕ
mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(char_u *textp, ಪೂರ್ಣಾಂಕ len ಬಳಸದ)
{
    char_u	*p = textp;
    ಪೂರ್ಣಾಂಕ		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    ದಶಮಾಂಶ       char_ಅಗಲ;
    ದಶಮಾಂಶ       ಮುಂದಿನ_pos;
    ಪೂರ್ಣಾಂಕ		ಒಳ_ascii;
    ಪೂರ್ಣಾಂಕ		half_ಅಗಲ;
    char_u	*tofree = ಖಾಲಿ;

    char_ಅಗಲ = prt_char_ಅಗಲ;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ಆದರೆ (prt_use_courier)
    {
	ಒಳ_ascii = (len == 1 && *p < 0x80);
	ಆದರೆ (prt_ಒಳ_ascii)
	{
	    ಆದರೆ (!ಒಳ_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_ಒಳ_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_ಒಳ_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	half_ಅಗಲ = ((*mb_ptr2cells)(p) == 1);
	ಆದರೆ (half_ಅಗಲ)
	    char_ಅಗಲ /= 2;
	ಆದರೆ (prt_half_ಅಗಲ)
	{
	    ಆದರೆ (!half_ಅಗಲ)
	    {
		prt_half_ಅಗಲ = FALSE;
		prt_pos_x += prt_char_ಅಗಲ/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (half_ಅಗಲ)
	{
	    prt_half_ಅಗಲ = TRUE;
	    prt_pos_x += prt_char_ಅಗಲ/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ಆದರೆ (prt_attribute_change)
    {
	prt_flush_ಬಫರ್();
	// Reset count of number of chars that will be printed
	prt_ಪಠ್ಯ_ಸತತ = 0;

	ಆದರೆ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ಆದರೆ (prt_need_font)
	{
	    ಆದರೆ (!prt_ಒಳ_ascii)
		prt_write_string("CF");
	    ಇಲ್ಲದಿದ್ದರೆ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ಆದರೆ (prt_need_fgcol)
	{
	    ಪೂರ್ಣಾಂಕ     r, g, b;
	    r = ((ಚಿಹ್ನೆರಹಿತ)prt_fgcol & 0xff0000) >> 16;
	    g = ((ಚಿಹ್ನೆರಹಿತ)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ಆದರೆ (r == g && g == b)
		prt_write_string("g\n");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ಆದರೆ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ಆದರೆ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ಆದರೆ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ಆದರೆ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ಆದರೆ (prt_ಹೊರಹೋಗು_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ಆಗ (len-- > 0)
	{
	    ch = prt_hexchar[(ಚಿಹ್ನೆರಹಿತ)(*p) >> 4];
	    ga_append(&prt_ps_ಬಫರ್, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_ಬಫರ್, ch);
	    p++;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ಆದರೆ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_ಬಫರ್, '\\');
	    ಆಯ್ಕೆ (ch)
	    {
		ಸ್ಥಿತಿ BS:   ga_append(&prt_ps_ಬಫರ್, 'b'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ TAB:  ga_append(&prt_ps_ಬಫರ್, 't'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ NL:   ga_append(&prt_ps_ಬಫರ್, 'n'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ FF:   ga_append(&prt_ps_ಬಫರ್, 'f'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ CAR:  ga_append(&prt_ps_ಬಫರ್, 'r'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '(':  ga_append(&prt_ps_ಬಫರ್, '('); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ')':  ga_append(&prt_ps_ಬಫರ್, ')'); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '\\': ga_append(&prt_ps_ಬಫರ್, '\\'); ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			   sprintf((ಅಕ್ಷರ *)ch_buff, "%03o", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ch);
			   ga_append(&prt_ps_ಬಫರ್, ch_buff[0]);
			   ga_append(&prt_ps_ಬಫರ್, ch_buff[1]);
			   ga_append(&prt_ps_ಬಫರ್, ch_buff[2]);
			   ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_append(&prt_ps_ಬಫರ್, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_ಪಠ್ಯ_ಸತತ += char_ಅಗಲ;
    prt_pos_x += char_ಅಗಲ;

    // The downside of fp - use relative error on right margin check
    ಮುಂದಿನ_pos = prt_pos_x + prt_char_ಅಗಲ;
    need_break = (ಮುಂದಿನ_pos > prt_ಬಲ_margin) &&
		    ((ಮುಂದಿನ_pos - prt_ಬಲ_margin) > (prt_ಬಲ_margin*1e-5));

    ಆದರೆ (need_break)
	prt_flush_ಬಫರ್();

    ಹಿಂತಿರುಗಿ need_break;
}

    ಶೂನ್ಯ
mch_print_ಗಣ_font(ಪೂರ್ಣಾಂಕ iBold, ಪೂರ್ಣಾಂಕ iItalic, ಪೂರ್ಣಾಂಕ iUnderline)
{
    ಪೂರ್ಣಾಂಕ		font = 0;

    ಆದರೆ (iBold)
	font |= 0x01;
    ಆದರೆ (iItalic)
	font |= 0x02;

    ಆದರೆ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ಆದರೆ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    ಶೂನ್ಯ
mch_print_ಗಣ_bg(long_u bgcol)
{
    prt_bgcol = (ಪೂರ್ಣಾಂಕ)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    ಶೂನ್ಯ
mch_print_ಗಣ_fg(long_u fgcol)
{
    ಆದರೆ (fgcol == (long_u)prt_fgcol)
	ಹಿಂತಿರುಗಿ;

    prt_fgcol = (ಪೂರ್ಣಾಂಕ)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_POSTSCRIPT
