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

#include "vim.h"
#include "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_NUM_OPTIONS] =
{
    {"c",	FALSE, 0, NULL, 0, FALSE},
    {"a",	FALSE, 0, NULL, 0, FALSE},
    {"r",	FALSE, 0, NULL, 0, FALSE},
    {"b",	FALSE, 0, NULL, 0, FALSE},
    {"i",	FALSE, 0, NULL, 0, FALSE},
    {"o",	FALSE, 0, NULL, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
sa_nracanaa prt_ps_font_S
{
    ຈຳນວນເຕັມ		wx;
    ຈຳນວນເຕັມ		uline_offset;
    ຈຳນວນເຕັມ		uline_width;
    ຈຳນວນເຕັມ		bbox_min_y;
    ຈຳນວນເຕັມ		bbox_max_y;
    var_na	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
static sa_nracanaa prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
static sa_nracanaa prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {NULL, NULL, NULL, NULL}
};

// Pointer to current font set being used
static sa_nracanaa prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
sa_nracanaa prt_ps_encoding_S
{
    var_na	*encoding;
    var_na	*cmap_encoding;
    ຈຳນວນເຕັມ		needs_charset;
};

sa_nracanaa prt_ps_charset_S
{
    var_na	*charset;
    var_na	*cmap_charset;
    ຈຳນວນເຕັມ		has_charset;
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
static sa_nracanaa prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", NULL,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
static sa_nracanaa prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  NULL,       CS_JIS_X_1983},
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
static sa_nracanaa prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    NULL,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     NULL,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
static sa_nracanaa prt_ps_charset_S sc_charsets[] =
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
static sa_nracanaa prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    NULL,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
static sa_nracanaa prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    NULL,	CS_B5},
    {"CP950",	    NULL,	CS_B5},
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
static sa_nracanaa prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", NULL,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
static sa_nracanaa prt_ps_charset_S k_charsets[] =
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
sa_nracanaa prt_ps_mbfont_S
{
    ຈຳນວນເຕັມ				num_encodings;
    sa_nracanaa prt_ps_encoding_S	*encodings;
    ຈຳນວນເຕັມ				num_charsets;
    sa_nracanaa prt_ps_charset_S	*charsets;
    var_na			*ascii_enc;
    var_na			*defcs;
};

static sa_nracanaa prt_ps_mbfont_S prt_ps_mbfonts[] =
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

sa_nracanaa prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    ຈຳນວນເຕັມ     type;
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
static var_na *prt_resource_types[] =
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

sa_nracanaa prt_dsc_comment_S
{
    var_na	*string;
    ຈຳນວນເຕັມ		len;
    ຈຳນວນເຕັມ		type;
};

sa_nracanaa prt_dsc_line_S
{
    ຈຳນວນເຕັມ		type;
    char_u	*string;
    ຈຳນວນເຕັມ		len;
};


# define SIZEOF_CSTR(s)      (parimaa_na(s) - 1)
static sa_nracanaa prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

static void prt_write_file_len(char_u *buffer, ຈຳນວນເຕັມ bytes);
static ຈຳນວນເຕັມ prt_next_dsc(sa_nracanaa prt_dsc_line_S *p_dsc_line);

/*
 * Variables for the output PostScript file.
 */
static FILE *prt_ps_fd;
static ຈຳນວນເຕັມ prt_file_error;
static char_u *prt_ps_file_name = NULL;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
static float prt_page_width;
static float prt_page_height;
static float prt_left_margin;
static float prt_right_margin;
static float prt_top_margin;
static float prt_bottom_margin;
static float prt_line_height;
static float prt_first_line_height;
static float prt_char_width;
static float prt_number_width;
static float prt_bgcol_offset;
static float prt_pos_x_moveto = 0.0;
static float prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
static ຈຳນວນເຕັມ prt_need_moveto;
static ຈຳນວນເຕັມ prt_do_moveto;
static ຈຳນວນເຕັມ prt_need_font;
static ຈຳນວນເຕັມ prt_font;
static ຈຳນວນເຕັມ prt_need_underline;
static ຈຳນວນເຕັມ prt_underline;
static ຈຳນວນເຕັມ prt_do_underline;
static ຈຳນວນເຕັມ prt_need_fgcol;
static ຈຳນວນເຕັມ prt_fgcol;
static ຈຳນວນເຕັມ prt_need_bgcol;
static ຈຳນວນເຕັມ prt_do_bgcol;
static ຈຳນວນເຕັມ prt_bgcol;
static ຈຳນວນເຕັມ prt_new_bgcol;
static ຈຳນວນເຕັມ prt_attribute_change;
static float prt_text_run;
static ຈຳນວນເຕັມ prt_page_num;
static ຈຳນວນເຕັມ prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
static ຈຳນວນເຕັມ prt_media;
static ຈຳນວນເຕັມ prt_portrait;
static ຈຳນວນເຕັມ prt_num_copies;
static ຈຳນວນເຕັມ prt_duplex;
static ຈຳນວນເຕັມ prt_tumble;
static ຈຳນວນເຕັມ prt_collate;

/*
 * Buffers used when generating PostScript output
 */
static char_u prt_line_buffer[257];
static garray_T prt_ps_buffer;

static ຈຳນວນເຕັມ prt_do_conv;
static vimconv_T prt_conv;

static ຈຳນວນເຕັມ prt_out_mbyte;
static ຈຳນວນເຕັມ prt_custom_cmap;
static var_na prt_cmap[80];
static ຈຳນວນເຕັມ prt_use_courier;
static ຈຳນວນເຕັມ prt_in_ascii;
static ຈຳນວນເຕັມ prt_half_width;
static var_na *prt_ascii_encoding;
static const char_u prt_hexchar[] = "0123456789abcdef";

    static void
prt_write_file_raw_len(char_u *buffer, ຈຳນວນເຕັມ bytes)
{
    ຖ້າ (!prt_file_error
	    && fwrite(buffer, parimaa_na(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    static void
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (ຈຳນວນເຕັມ)STRLEN(buffer));
}

    static void
prt_write_file_len(char_u *buffer, ຈຳນວນເຕັມ bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    static void
prt_write_string(var_na *s)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer), "%s", s);
    prt_write_file(prt_line_buffer);
}

/*
 * Write an int and a space.
 */
    static void
prt_write_int(ຈຳນວນເຕັມ i)
{
    sprintf((var_na *)prt_line_buffer, "%d ", i);
    prt_write_file(prt_line_buffer);
}

/*
 * Write a boolean and a space.
 */
    static void
prt_write_boolean(ຈຳນວນເຕັມ b)
{
    sprintf((var_na *)prt_line_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_line_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    static void
prt_def_font(
    var_na	*new_name,
    var_na	*encoding,
    ຈຳນວນເຕັມ		height,
    var_na	*font)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_line_buffer);
    ຖ້າ (prt_out_mbyte)
	sprintf((var_na *)prt_line_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    ບໍ່ດັ່ງນັ້ນ
	vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_line_buffer);
}

/*
 * Write a line to define the CID font.
 */
    static void
prt_def_cidfont(var_na *new_name, ຈຳນວນເຕັມ height, var_na *cidfont)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_line_buffer);
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_line_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    static void
prt_dup_cidfont(var_na *original_name, var_na *new_name)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_line_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    static void
prt_real_bits(
    dviguu_na      real,
    ຈຳນວນເຕັມ		precision,
    ຈຳນວນເຕັມ		*pinteger,
    ຈຳນວນເຕັມ		*pfraction)
{
    ຈຳນວນເຕັມ     i;
    ຈຳນວນເຕັມ     integer;
    float   fraction;

    integer = (ຈຳນວນເຕັມ)real;
    fraction = (float)(real - integer);
    ຖ້າ (real < (dviguu_na)integer)
	fraction = -fraction;
    ສຳລັບ (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (ຈຳນວນເຕັມ)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    static void
prt_write_real(dviguu_na val, ຈຳນວນເຕັມ prec)
{
    ຈຳນວນເຕັມ     integer;
    ຈຳນວນເຕັມ     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((var_na *)prt_line_buffer, "%d", integer);
    prt_write_file(prt_line_buffer);
    // Only emit fraction if necessary
    ຖ້າ (fraction != 0)
    {
	// Remove any trailing zeros
	ໃນຂະນະທີ່ ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((var_na *)prt_line_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_line_buffer);
    }
    sprintf((var_na *)prt_line_buffer, " ");
    prt_write_file(prt_line_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    static void
prt_def_var(var_na *name, dviguu_na value, ຈຳນວນເຕັມ prec)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
								"/%s ", name);
    prt_write_file(prt_line_buffer);
    prt_write_real(value, prec);
    sprintf((var_na *)prt_line_buffer, "d\n");
    prt_write_file(prt_line_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    static void
prt_flush_buffer(void)
{
    ຖ້າ (prt_ps_buffer.ga_len <= 0)
	ສົ່ງຄືນ;

    // Any background color must be drawn first
    ຖ້າ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	ຈຳນວນເຕັມ     r, g, b;

	ຖ້າ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_line_height, 2);

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
    ຖ້າ (prt_do_underline)
    {
	ຖ້າ (prt_do_moveto)
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
    ຖ້າ (prt_out_mbyte)
	prt_write_string("<");
    ບໍ່ດັ່ງນັ້ນ
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    ຖ້າ (prt_out_mbyte)
	prt_write_string(">");
    ບໍ່ດັ່ງນັ້ນ
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ຖ້າ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    ບໍ່ດັ່ງນັ້ນ // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, parimaa_na(var_na), prt_bufsiz);
}


    static void
prt_resource_name(char_u *filename, void *cookie)
{
    char_u *resource_filename = cookie;

    ຖ້າ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    ບໍ່ດັ່ງນັ້ນ
	STRCPY(resource_filename, filename);
}

    static ຈຳນວນເຕັມ
prt_find_resource(var_na *name, sa_nracanaa prt_ps_resource_S *resource)
{
    char_u	*buffer;
    ຈຳນວນເຕັມ		retval;

    buffer = alloc(MAXPATHL + 1);
    ຖ້າ (buffer == NULL)
	ສົ່ງຄືນ FALSE;

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
    ສົ່ງຄືນ retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
sa_nracanaa prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    ຈຳນວນເຕັມ     len;
    ຈຳນວນເຕັມ     line_start;
    ຈຳນວນເຕັມ     line_end;
};

static sa_nracanaa prt_resfile_buffer_S prt_resfile;

    static ຈຳນວນເຕັມ
prt_resfile_next_line(void)
{
    ຈຳນວນເຕັມ     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.line_end + 1;
    ໃນຂະນະທີ່ (idx < prt_resfile.len)
    {
	ຖ້າ (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    bha_nga;
	idx++;
    }
    prt_resfile.line_start = idx;

    ໃນຂະນະທີ່ (idx < prt_resfile.len)
    {
	ຖ້າ (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    bha_nga;
	idx++;
    }
    prt_resfile.line_end = idx;

    ສົ່ງຄືນ (idx < prt_resfile.len);
}

    static ຈຳນວນເຕັມ
prt_resfile_strncmp(ຈຳນວນເຕັມ offset, var_na *string, ຈຳນວນເຕັມ len)
{
    // Force not equal if string is longer than remainder of line
    ຖ້າ (len > (prt_resfile.line_end - (prt_resfile.line_start + offset)))
	ສົ່ງຄືນ 1;

    ສົ່ງຄືນ STRNCMP(&prt_resfile.buffer[prt_resfile.line_start + offset],
								string, len);
}

    static ຈຳນວນເຕັມ
prt_resfile_skip_nonws(ຈຳນວນເຕັມ offset)
{
    ຈຳນວນເຕັມ     idx;

    idx = prt_resfile.line_start + offset;
    ໃນຂະນະທີ່ (idx < prt_resfile.line_end)
    {
	ຖ້າ (SAFE_isspace(prt_resfile.buffer[idx]))
	    ສົ່ງຄືນ idx - prt_resfile.line_start;
	idx++;
    }
    ສົ່ງຄືນ -1;
}

    static ຈຳນວນເຕັມ
prt_resfile_skip_ws(ຈຳນວນເຕັມ offset)
{
    ຈຳນວນເຕັມ     idx;

    idx = prt_resfile.line_start + offset;
    ໃນຂະນະທີ່ (idx < prt_resfile.line_end)
    {
	ຖ້າ (!SAFE_isspace(prt_resfile.buffer[idx]))
	    ສົ່ງຄືນ idx - prt_resfile.line_start;
	idx++;
    }
    ສົ່ງຄືນ -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    static ຈຳນວນເຕັມ
prt_next_dsc(sa_nracanaa prt_dsc_line_S *p_dsc_line)
{
    ຈຳນວນເຕັມ     comment;
    ຈຳນວນເຕັມ     offset;

    // Move to start of next line
    ຖ້າ (!prt_resfile_next_line())
	ສົ່ງຄືນ FALSE;

    // DSC comments always start %%
    ຖ້າ (prt_resfile_strncmp(0, "%%", 2) != 0)
	ສົ່ງຄືນ FALSE;

    // Find type of DSC comment
    ສຳລັບ (comment = 0; comment < (ຈຳນວນເຕັມ)ARRAY_LENGTH(prt_dsc_table); comment++)
	ຖ້າ (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    bha_nga;

    ຖ້າ (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_line->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_line->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	ຖ້າ (offset == -1)
	    ສົ່ງຄືນ FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    ຖ້າ (offset == -1)
	ສົ່ງຄືນ FALSE;

    p_dsc_line->string = &prt_resfile.buffer[prt_resfile.line_start + offset];
    p_dsc_line->len = prt_resfile.line_end - (prt_resfile.line_start + offset);

    ສົ່ງຄືນ TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    static ຈຳນວນເຕັມ
prt_open_resource(sa_nracanaa prt_ps_resource_S *resource)
{
    ຈຳນວນເຕັມ		offset;
    ຈຳນວນເຕັມ		seen_all;
    ຈຳນວນເຕັມ		seen_title;
    ຈຳນວນເຕັມ		seen_version;
    FILE	*fd_resource;
    sa_nracanaa prt_dsc_line_S dsc_line;

    fd_resource = mch_fopen((var_na *)resource->filename, READBIN);
    ຖ້າ (fd_resource == NULL)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ສົ່ງຄືນ FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (ຈຳນວນເຕັມ)fread((var_na *)prt_resfile.buffer, parimaa_na(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    ຖ້າ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ສົ່ງຄືນ FALSE;
    }
    fclose(fd_resource);

    prt_resfile.line_end = -1;
    prt_resfile.line_start = 0;
    ຖ້າ (!prt_resfile_next_line())
	ສົ່ງຄືນ FALSE;

    offset = 0;

    ຖ້າ (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ສົ່ງຄືນ FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    ຖ້າ (offset == -1)
	ສົ່ງຄືນ FALSE;
    offset = prt_resfile_skip_ws(offset);
    ຖ້າ (offset == -1)
	ສົ່ງຄືນ FALSE;

    ຖ້າ (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ສົ່ງຄືນ FALSE;
    }
    offset += (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ຖ້າ (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (ຈຳນວນເຕັມ)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    ບໍ່ດັ່ງນັ້ນ
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ສົ່ງຄືນ FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    ໃນຂະນະທີ່ (!seen_all && prt_next_dsc(&dsc_line))
    {
	switch (dsc_line.type)
	{
	pak_sa PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_line.string, dsc_line.len);
	    seen_title = TRUE;
	    ຖ້າ (seen_version)
		seen_all = TRUE;
	    bha_nga;

	pak_sa PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_line.string, dsc_line.len);
	    seen_version = TRUE;
	    ຖ້າ (seen_title)
		seen_all = TRUE;
	    bha_nga;

	pak_sa PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    bha_nga;

	pak_sa PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    bha_nga;
	}
    }

    ຖ້າ (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ສົ່ງຄືນ FALSE;
    }

    ສົ່ງຄືນ TRUE;
}

    static ຈຳນວນເຕັມ
prt_check_resource(sa_nracanaa prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    ຖ້າ (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	ສົ່ງຄືນ FALSE;
    }

    // Other checks to be added as needed
    ສົ່ງຄືນ TRUE;
}

    static void
prt_dsc_start(void)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    static void
prt_dsc_noarg(var_na *comment)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_line_buffer);
}

    static void
prt_dsc_textline(var_na *comment, var_na *text)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_line_buffer);
}

    static void
prt_dsc_text(var_na *comment, var_na *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_line_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    static void
prt_dsc_ints(var_na *comment, ຈຳນວນເຕັມ count, ຈຳນວນເຕັມ *ints)
{
    ຈຳນວນເຕັມ		i;

    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_line_buffer);

    ສຳລັບ (i = 0; i < count; i++)
    {
	sprintf((var_na *)prt_line_buffer, " %d", ints[i]);
	prt_write_file(prt_line_buffer);
    }

    prt_write_string("\n");
}

    static void
prt_dsc_resources(
    var_na	*comment,	// if NULL add to previous
    var_na	*type,
    var_na	*string)
{
    ຖ້າ (comment != NULL)
	vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
						 "%%%%%s: %s", comment, type);
    ບໍ່ດັ່ງນັ້ນ
	vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_line_buffer);

    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
							     " %s\n", string);
    prt_write_file(prt_line_buffer);
}

    static void
prt_dsc_font_resource(var_na *resource, sa_nracanaa prt_ps_font_S *ps_font)
{
    ຈຳນວນເຕັມ     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    ສຳລັບ (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ຖ້າ (ps_font->ps_fontname[i] != NULL)
	    prt_dsc_resources(NULL, "font", ps_font->ps_fontname[i]);
}

    static void
prt_dsc_requirements(
    ຈຳນວນເຕັມ		duplex,
    ຈຳນວນເຕັມ		tumble,
    ຈຳນວນເຕັມ		collate,
    ຈຳນວນເຕັມ		color,
    ຈຳນວນເຕັມ		num_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ຖ້າ (!(duplex || collate || color || (num_copies > 1)))
	ສົ່ງຄືນ;

    sprintf((var_na *)prt_line_buffer, "%%%%Requirements:");
    prt_write_file(prt_line_buffer);

    ຖ້າ (duplex)
    {
	prt_write_string(" duplex");
	ຖ້າ (tumble)
	    prt_write_string("(tumble)");
    }
    ຖ້າ (collate)
	prt_write_string(" collate");
    ຖ້າ (color)
	prt_write_string(" color");
    ຖ້າ (num_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((var_na *)prt_line_buffer, "%d", num_copies);
	prt_write_file(prt_line_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    static void
prt_dsc_docmedia(
    var_na	*paper_name,
    dviguu_na	width,
    dviguu_na	height,
    dviguu_na	weight,
    var_na	*colour,
    var_na	*type)
{
    vim_snprintf((var_na *)prt_line_buffer, parimaa_na(prt_line_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_line_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    ຖ້າ (colour == NULL)
	prt_write_string("()");
    ບໍ່ດັ່ງນັ້ນ
	prt_write_string(colour);
    prt_write_string(" ");
    ຖ້າ (type == NULL)
	prt_write_string("()");
    ບໍ່ດັ່ງນັ້ນ
	prt_write_string(type);
    prt_write_string("\n");
}

    void
mch_print_cleanup(void)
{
    ຖ້າ (prt_out_mbyte)
    {
	ຈຳນວນເຕັມ     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	ສຳລັບ (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ຖ້າ (prt_ps_mb_font.ps_fontname[i] != NULL)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ຖ້າ (prt_do_conv)
    {
	convert_setup(&prt_conv, NULL, NULL);
	prt_do_conv = FALSE;
    }
    ຖ້າ (prt_ps_fd != NULL)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = NULL;
	prt_file_error = FALSE;
    }
    ຖ້າ (prt_ps_file_name != NULL)
	VIM_CLEAR(prt_ps_file_name);
}

    static float
to_device_units(ຈຳນວນເຕັມ idx, dviguu_na physsize, ຈຳນວນເຕັມ def_number)
{
    float	ret;
    ຈຳນວນເຕັມ		u;
    ຈຳນວນເຕັມ		nr;

    u = prt_get_unit(idx);
    ຖ້າ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_number;
    }
    ບໍ່ດັ່ງນັ້ນ
	nr = printer_opts[idx].number;

    switch (u)
    {
	pak_sa PRT_UNIT_INCH:
	    ret = (float)(nr * PRT_PS_DEFAULT_DPI);
	    bha_nga;
	pak_sa PRT_UNIT_MM:
	    ret = (float)(nr * PRT_PS_DEFAULT_DPI) / (float)25.4;
	    bha_nga;
	pak_sa PRT_UNIT_POINT:
	    ret = (float)nr;
	    bha_nga;
	pak_sa PRT_UNIT_PERC:
	default:
	    ret = (float)(physsize * nr) / 100;
	    bha_nga;
    }

    ສົ່ງຄືນ ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    static void
prt_page_margins(
    dviguu_na	width,
    dviguu_na	height,
    dviguu_na	*left,
    dviguu_na	*right,
    dviguu_na	*top,
    dviguu_na	*bottom)
{
    *left   = to_device_units(OPT_PRINT_LEFT, width, 10);
    *right  = width - to_device_units(OPT_PRINT_RIGHT, width, 5);
    *top    = height - to_device_units(OPT_PRINT_TOP, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    static void
prt_font_metrics(ຈຳນວນເຕັມ font_scale)
{
    prt_line_height = (float)font_scale;
    prt_char_width = (float)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    static ຈຳນວນເຕັມ
prt_get_cpl(void)
{
    ຖ້າ (prt_use_number())
    {
	prt_number_width = PRINT_NUMBER_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ຖ້າ (prt_out_mbyte)
	    prt_number_width /= 2;
	prt_left_margin += prt_number_width;
    }
    ບໍ່ດັ່ງນັ້ນ
	prt_number_width = 0.0;

    ສົ່ງຄືນ (ຈຳນວນເຕັມ)((prt_right_margin - prt_left_margin) / prt_char_width);
}

    static ຈຳນວນເຕັມ
prt_build_cid_fontname(ຈຳນວນເຕັມ font, char_u *name, ຈຳນວນເຕັມ name_len)
{
    var_na    *fontname;

    fontname = alloc(name_len + 1);
    ຖ້າ (fontname == NULL)
	ສົ່ງຄືນ FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ສົ່ງຄືນ TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    static ຈຳນວນເຕັມ
prt_get_lpp(void)
{
    ຈຳນວນເຕັມ lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (float)PRT_PS_FONT_TO_USER(prt_line_height,
					   prt_ps_font->bbox_min_y);
    ຖ້າ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (float)PRT_PS_FONT_TO_USER(prt_line_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_line_height = prt_line_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (ຈຳນວນເຕັມ)((prt_top_margin - prt_bottom_margin) / prt_line_height);

    // Adjust top margin if there is a header
    prt_top_margin -= prt_line_height * prt_header_height();

    ສົ່ງຄືນ lpp - prt_header_height();
}

    static ຈຳນວນເຕັມ
prt_match_encoding(
    var_na			*p_encoding,
    sa_nracanaa prt_ps_mbfont_S	*p_cmap,
    sa_nracanaa prt_ps_encoding_S	**pp_mbenc)
{
    ຈຳນວນເຕັມ				mbenc;
    ຈຳນວນເຕັມ				enc_len;
    sa_nracanaa prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = NULL;
    // Look for recognised encoding
    enc_len = (ຈຳນວນເຕັມ)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    ສຳລັບ (mbenc = 0; mbenc < p_cmap->num_encodings; mbenc++)
    {
	ຖ້າ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ສົ່ງຄືນ TRUE;
	}
	p_mbenc++;
    }
    ສົ່ງຄືນ FALSE;
}

    static ຈຳນວນເຕັມ
prt_match_charset(
    var_na		    *p_charset,
    sa_nracanaa prt_ps_mbfont_S  *p_cmap,
    sa_nracanaa prt_ps_charset_S **pp_mbchar)
{
    ຈຳນວນເຕັມ			    mbchar;
    ຈຳນວນເຕັມ			    char_len;
    sa_nracanaa prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ຖ້າ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (ຈຳນວນເຕັມ)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    ສຳລັບ (mbchar = 0; mbchar < p_cmap->num_charsets; mbchar++)
    {
	ຖ້າ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ສົ່ງຄືນ TRUE;
	}
	p_mbchar++;
    }
    ສົ່ງຄືນ FALSE;
}

    ຈຳນວນເຕັມ
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    ຈຳນວນເຕັມ		forceit UNUSED)
{
    ຈຳນວນເຕັມ		i;
    var_na	*paper_name;
    ຈຳນວນເຕັມ		paper_strlen;
    ຈຳນວນເຕັມ		fontsize;
    char_u	*p;
    dviguu_na      left;
    dviguu_na      right;
    dviguu_na      top;
    dviguu_na      bottom;
    ຈຳນວນເຕັມ		props;
    ຈຳນວນເຕັມ		cmap = 0;
    char_u	*p_encoding;
    sa_nracanaa prt_ps_encoding_S *p_mbenc;
    sa_nracanaa prt_ps_encoding_S *p_mbenc_first;
    sa_nracanaa prt_ps_charset_S  *p_mbchar = NULL;

# ຖ້າ 0
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
    ຖ້າ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ຖ້າ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = NULL;
    props = enc_canon_props(p_encoding);
    ຖ້າ (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	ຈຳນວນເຕັມ cmap_first = 0;

	p_mbenc_first = NULL;
	ສຳລັບ (cmap = 0; cmap < (ຈຳນວນເຕັມ)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    ຖ້າ (prt_match_encoding((var_na *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ຖ້າ (p_mbenc_first == NULL)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		ຖ້າ (prt_match_charset((var_na *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    bha_nga;
	    }

	// Use first encoding matched if no charset matched
	ຖ້າ (p_mbchar == NULL && p_mbenc_first != NULL)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != NULL);
    ຖ້າ (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == NULL);
	ຖ້າ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ຖ້າ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		ສົ່ງຄືນ FALSE;
	    }

	    // Add charset name if not empty
	    ຖ້າ (p_mbchar->cmap_charset != NULL)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, parimaa_na(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Add custom CMap character set name
	    ຖ້າ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		ສົ່ງຄືນ FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, parimaa_na(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ຖ້າ (p_mbenc->cmap_encoding != NULL && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < parimaa_na(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ຖ້າ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ສົ່ງຄືນ FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ຖ້າ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    ສົ່ງຄືນ FALSE;
	ຖ້າ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ຖ້າ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		ສົ່ງຄືນ FALSE;
	ຖ້າ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ຖ້າ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		ສົ່ງຄືນ FALSE;
	ຖ້າ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ຖ້າ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		ສົ່ງຄືນ FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ຖ້າ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ຖ້າ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    ບໍ່ດັ່ງນັ້ນ
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ຖ້າ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (var_na *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    ສຳລັບ (i = 0; i < (ຈຳນວນເຕັມ)PRT_MEDIASIZE_LEN; ++i)
	ຖ້າ (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    bha_nga;
    ຖ້າ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ຖ້າ (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &left, &right, &top,
								    &bottom);
    prt_left_margin = (float)left;
    prt_right_margin = (float)right;
    prt_top_margin = (float)top;
    prt_bottom_margin = (float)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    ສຳລັບ (p = p_pfn; (p = vim_strchr(p, ':')) != NULL; ++p)
	ຖ້າ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((var_na *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_line = prt_get_cpl();
    psettings->lines_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ຖ້າ (psettings->chars_per_line <= 0 || psettings->lines_per_page <= 0)
	ສົ່ງຄືນ FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_num_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    ຖ້າ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_num_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    ຖ້າ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ຖ້າ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ຖ້າ (psettings->outfile == NULL)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ຖ້າ (prt_ps_file_name == NULL)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ສົ່ງຄືນ FAIL;
	}
	prt_ps_fd = mch_fopen((var_na *)prt_ps_file_name, WRITEBIN);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	p = expand_env_save(psettings->outfile);
	ຖ້າ (p != NULL)
	{
	    prt_ps_fd = mch_fopen((var_na *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ຖ້າ (prt_ps_fd == NULL)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ສົ່ງຄືນ FAIL;
    }

    prt_bufsiz = psettings->chars_per_line;
    ຖ້າ (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, parimaa_na(var_na), prt_bufsiz);

    prt_page_num = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
prt_add_resource(sa_nracanaa prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((var_na *)resource->filename, READBIN);
    ຖ້າ (fd_resource == NULL)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ສົ່ງຄືນ FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (var_na *)resource->title);

    prt_dsc_textline("BeginDocument", (var_na *)resource->filename);

    ສຳລັບ (;;)
    {
	bytes_read = fread((var_na *)resource_buffer, parimaa_na(char_u),
			   parimaa_na(resource_buffer), fd_resource);
	ຖ້າ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ສົ່ງຄືນ FALSE;
	}
	ຖ້າ (bytes_read == 0)
	    bha_nga;
	prt_write_file_raw_len(resource_buffer, (ຈຳນວນເຕັມ)bytes_read);
	ຖ້າ (prt_file_error)
	{
	    fclose(fd_resource);
	    ສົ່ງຄືນ FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ສົ່ງຄືນ TRUE;
}

    ຈຳນວນເຕັມ
mch_print_begin(prt_settings_T *psettings)
{
    ຈຳນວນເຕັມ		bbox[4];
    dviguu_na      left;
    dviguu_na      right;
    dviguu_na      top;
    dviguu_na      bottom;
    sa_nracanaa prt_ps_resource_S *res_prolog;
    sa_nracanaa prt_ps_resource_S *res_encoding;
    var_na	buffer[256];
    char_u      *p_encoding;
    sa_nracanaa prt_ps_resource_S *res_cidfont;
    sa_nracanaa prt_ps_resource_S *res_cmap;
    ຈຳນວນເຕັມ		retval = FALSE;

    res_prolog = ALLOC_ONE(sa_nracanaa prt_ps_resource_S);
    res_encoding = ALLOC_ONE(sa_nracanaa prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(sa_nracanaa prt_ps_resource_S);
    res_cmap = ALLOC_ONE(sa_nracanaa prt_ps_resource_S);
    ຖ້າ (res_prolog == NULL || res_encoding == NULL
	    || res_cidfont == NULL || res_cmap == NULL)
	goto theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (var_na *)psettings->jobname);
    ຖ້າ (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(NULL), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &left, &right, &top, &bottom);
    bbox[0] = (ຈຳນວນເຕັມ)left;
    ຖ້າ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (ຈຳນວນເຕັມ)(top - (psettings->lines_per_page + prt_header_height())
						    * (dviguu_na)prt_line_height);
	bbox[2] = (ຈຳນວນເຕັມ)(left + psettings->chars_per_line
					       * (dviguu_na)prt_char_width + 0.5);
	bbox[3] = (ຈຳນວນເຕັມ)(top + 0.5);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (ຈຳນວນເຕັມ)bottom;
	bbox[2] = (ຈຳນວນເຕັມ)(left + ((psettings->lines_per_page
			      + prt_header_height()) * prt_line_height) + 0.5);
	bbox[3] = (ຈຳນວນເຕັມ)(bottom + psettings->chars_per_line
					       * (dviguu_na)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(dviguu_na)0, NULL, NULL);
    // Define fonts needed
    ຖ້າ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ຖ້າ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? NULL
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ຖ້າ (!prt_custom_cmap)
	    prt_dsc_resources(NULL, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ຖ້າ (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	goto theend;
    }
    ຖ້າ (!prt_open_resource(res_prolog))
	goto theend;
    ຖ້າ (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	goto theend;
    ຖ້າ (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ຖ້າ (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    goto theend;
	}
	ຖ້າ (!prt_open_resource(res_cidfont))
	    goto theend;
	ຖ້າ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    goto theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ຖ້າ (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ຖ້າ (*p_encoding == NUL
		|| !prt_find_resource((var_na *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    ຈຳນວນເຕັມ		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ຖ້າ (!(props & ENC_8BIT)
		    || !prt_find_resource((var_na *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ຖ້າ (!prt_find_resource((var_na *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    goto theend;
		}
	    }
	}
	ຖ້າ (!prt_open_resource(res_encoding))
	    goto theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	p_encoding = enc_skip(p_penc);
	ຖ້າ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ຖ້າ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ຖ້າ (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		goto theend;
	    }
	    ຖ້າ (!prt_open_resource(res_encoding))
		goto theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ຖ້າ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	ຖ້າ (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    goto theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ຖ້າ (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ຖ້າ (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    goto theend;
	}
	ຖ້າ (!prt_open_resource(res_cmap))
	    goto theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    ຖ້າ (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(NULL, "procset", buffer);

	ຖ້າ (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(NULL, "cmap", buffer);
	}
    }
    ຖ້າ (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(NULL, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# ບໍ່ດັ່ງນັ້ນ
					0
# endif
					, prt_num_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ຖ້າ (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ຖ້າ (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? NULL : "PageResources"),
							    &prt_ps_mb_font);
	ຖ້າ (!prt_custom_cmap)
	    prt_dsc_resources(NULL, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ຖ້າ (!prt_add_resource(res_prolog))
	goto theend;
    ຖ້າ (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ຖ້າ (!prt_add_resource(res_cidfont))
	    goto theend;
	ຖ້າ (prt_custom_cmap && !prt_add_resource(res_cmap))
	    goto theend;
    }

    ຖ້າ (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ຖ້າ (!prt_add_resource(res_encoding))
	    goto theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((ຈຳນວນເຕັມ)prt_mediasize[prt_media].width);
    prt_write_int((ຈຳນວນເຕັມ)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_num_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    ຖ້າ (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ຖ້າ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (var_na *)p_encoding, (ຈຳນວນເຕັມ)prt_line_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (var_na *)p_encoding, (ຈຳນວນເຕັມ)prt_line_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (var_na *)p_encoding, (ຈຳນວນເຕັມ)prt_line_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (var_na *)p_encoding, (ຈຳນວນເຕັມ)prt_line_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ຖ້າ (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ຖ້າ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (ຈຳນວນເຕັມ)prt_line_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ຖ້າ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != NULL)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ຖ້າ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (ຈຳນວນເຕັມ)prt_line_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	ບໍ່ດັ່ງນັ້ນ
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ຖ້າ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != NULL)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ຖ້າ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (ຈຳນວນເຕັມ)prt_line_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	ບໍ່ດັ່ງນັ້ນ
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ຖ້າ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != NULL)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ຖ້າ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (ຈຳນວນເຕັມ)prt_line_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	ບໍ່ດັ່ງນັ້ນ
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_line_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_line_height,
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

    ສົ່ງຄືນ retval;
}

    void
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_num);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ຖ້າ (!prt_file_error && psettings->outfile == NULL
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ຖ້າ (prt_ps_fd != NULL)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = NULL;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ຖ້າ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	ບໍ່ດັ່ງນັ້ນ
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    ຈຳນວນເຕັມ
mch_print_end_page(void)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ສົ່ງຄືນ !prt_file_error;
}

    ຈຳນວນເຕັມ
mch_print_begin_page(char_u *str UNUSED)
{
    ຈຳນວນເຕັມ		page_num[2];

    prt_page_num++;

    page_num[0] = page_num[1] = prt_page_num;
    prt_dsc_ints("Page", 2, page_num);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    ຖ້າ (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    ບໍ່ດັ່ງນັ້ນ
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ຖ້າ (!prt_portrait)
    {
	prt_write_int(-((ຈຳນວນເຕັມ)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ສົ່ງຄືນ !prt_file_error;
}

    ຈຳນວນເຕັມ
mch_print_blank_page(void)
{
    ສົ່ງຄືນ (mch_print_begin_page(NULL) ? (mch_print_end_page()) : FALSE);
}

static float prt_pos_x = 0;
static float prt_pos_y = 0;

    void
mch_print_start_line(ຈຳນວນເຕັມ margin, ຈຳນວນເຕັມ page_line)
{
    prt_pos_x = prt_left_margin;
    ຖ້າ (margin)
	prt_pos_x -= prt_number_width;

    prt_pos_y = prt_top_margin - prt_first_line_height -
					page_line * prt_line_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    ຈຳນວນເຕັມ
mch_print_text_out(char_u *textp, ຈຳນວນເຕັມ len UNUSED)
{
    char_u	*p = textp;
    ຈຳນວນເຕັມ		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    float       char_width;
    float       next_pos;
    ຈຳນວນເຕັມ		in_ascii;
    ຈຳນວນເຕັມ		half_width;
    char_u	*tofree = NULL;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ຖ້າ (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	ຖ້າ (prt_in_ascii)
	{
	    ຖ້າ (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ຖ້າ (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	ຖ້າ (half_width)
	    char_width /= 2;
	ຖ້າ (prt_half_width)
	{
	    ຖ້າ (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ຖ້າ (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	ຖ້າ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ຖ້າ (prt_need_font)
	{
	    ຖ້າ (!prt_in_ascii)
		prt_write_string("CF");
	    ບໍ່ດັ່ງນັ້ນ
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ຖ້າ (prt_need_fgcol)
	{
	    ຈຳນວນເຕັມ     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ຖ້າ (r == g && g == b)
		prt_write_string("g\n");
	    ບໍ່ດັ່ງນັ້ນ
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ຖ້າ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ຖ້າ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	ບໍ່ດັ່ງນັ້ນ
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ຖ້າ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ຖ້າ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ຖ້າ (p == NULL)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ຖ້າ (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	ໃນຂະນະທີ່ (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ຖ້າ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    switch (ch)
	    {
		pak_sa BS:   ga_append(&prt_ps_buffer, 'b'); bha_nga;
		pak_sa TAB:  ga_append(&prt_ps_buffer, 't'); bha_nga;
		pak_sa NL:   ga_append(&prt_ps_buffer, 'n'); bha_nga;
		pak_sa FF:   ga_append(&prt_ps_buffer, 'f'); bha_nga;
		pak_sa CAR:  ga_append(&prt_ps_buffer, 'r'); bha_nga;
		pak_sa '(':  ga_append(&prt_ps_buffer, '('); bha_nga;
		pak_sa ')':  ga_append(&prt_ps_buffer, ')'); bha_nga;
		pak_sa '\\': ga_append(&prt_ps_buffer, '\\'); bha_nga;

		default:
			   sprintf((var_na *)ch_buff, "%03o", (unsigned ຈຳນວນເຕັມ)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   bha_nga;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_right_margin) &&
		    ((next_pos - prt_right_margin) > (prt_right_margin*1e-5));

    ຖ້າ (need_break)
	prt_flush_buffer();

    ສົ່ງຄືນ need_break;
}

    void
mch_print_set_font(ຈຳນວນເຕັມ iBold, ຈຳນວນເຕັມ iItalic, ຈຳນວນເຕັມ iUnderline)
{
    ຈຳນວນເຕັມ		font = 0;

    ຖ້າ (iBold)
	font |= 0x01;
    ຖ້າ (iItalic)
	font |= 0x02;

    ຖ້າ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ຖ້າ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    void
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (ຈຳນວນເຕັມ)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    void
mch_print_set_fg(long_u fgcol)
{
    ຖ້າ (fgcol == (long_u)prt_fgcol)
	ສົ່ງຄືນ;

    prt_fgcol = (ຈຳນວນເຕັມ)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
