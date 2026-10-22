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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_অঙ্ক_OPTIONS] =
{
    {"c",	FALSE, 0, ফাঁকা, 0, FALSE},
    {"a",	FALSE, 0, ফাঁকা, 0, FALSE},
    {"r",	FALSE, 0, ফাঁকা, 0, FALSE},
    {"b",	FALSE, 0, ফাঁকা, 0, FALSE},
    {"i",	FALSE, 0, ফাঁকা, 0, FALSE},
    {"o",	FALSE, 0, ফাঁকা, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_বাফার_আকার  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
কাঠামো prt_ps_font_S
{
    পূর্ণসংখ্যা		wx;
    পূর্ণসংখ্যা		uline_offset;
    পূর্ণসংখ্যা		uline_width;
    পূর্ণসংখ্যা		bbox_min_y;
    পূর্ণসংখ্যা		bbox_max_y;
    অক্ষর	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
স্থির কাঠামো prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
স্থির কাঠামো prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা}
};

// Pointer to current font set being used
স্থির কাঠামো prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
কাঠামো prt_ps_encoding_S
{
    অক্ষর	*encoding;
    অক্ষর	*cmap_encoding;
    পূর্ণসংখ্যা		needs_charset;
};

কাঠামো prt_ps_charset_S
{
    অক্ষর	*charset;
    অক্ষর	*cmap_charset;
    পূর্ণসংখ্যা		has_charset;
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
স্থির কাঠামো prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ফাঁকা,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
স্থির কাঠামো prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ফাঁকা,       CS_JIS_X_1983},
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
স্থির কাঠামো prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ফাঁকা,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ফাঁকা,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
স্থির কাঠামো prt_ps_charset_S sc_charsets[] =
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
স্থির কাঠামো prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ফাঁকা,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
স্থির কাঠামো prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ফাঁকা,	CS_B5},
    {"CP950",	    ফাঁকা,	CS_B5},
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
স্থির কাঠামো prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ফাঁকা,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
স্থির কাঠামো prt_ps_charset_S k_charsets[] =
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
কাঠামো prt_ps_mbfont_S
{
    পূর্ণসংখ্যা				অঙ্ক_encodings;
    কাঠামো prt_ps_encoding_S	*encodings;
    পূর্ণসংখ্যা				অঙ্ক_charsets;
    কাঠামো prt_ps_charset_S	*charsets;
    অক্ষর			*ascii_enc;
    অক্ষর			*defcs;
};

স্থির কাঠামো prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_দৈর্ঘ্য(j_encodings),
	j_encodings,
	ARRAY_দৈর্ঘ্য(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_দৈর্ঘ্য(sc_encodings),
	sc_encodings,
	ARRAY_দৈর্ঘ্য(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_দৈর্ঘ্য(tc_encodings),
	tc_encodings,
	ARRAY_দৈর্ঘ্য(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_দৈর্ঘ্য(k_encodings),
	k_encodings,
	ARRAY_দৈর্ঘ্য(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

কাঠামো prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    পূর্ণসংখ্যা     type;
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
স্থির অক্ষর *prt_resource_types[] =
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

কাঠামো prt_dsc_comment_S
{
    অক্ষর	*string;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		type;
};

কাঠামো prt_dsc_সারি_S
{
    পূর্ণসংখ্যা		type;
    char_u	*string;
    পূর্ণসংখ্যা		len;
};


# define SIZEOF_CSTR(s)      (আকার(s) - 1)
স্থির কাঠামো prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

স্থির শূন্য prt_write_file_len(char_u *বাফার, পূর্ণসংখ্যা bytes);
স্থির পূর্ণসংখ্যা prt_পরবর্তী_dsc(কাঠামো prt_dsc_সারি_S *p_dsc_সারি);

/*
 * Variables for the output PostScript file.
 */
স্থির FILE *prt_ps_fd;
স্থির পূর্ণসংখ্যা prt_file_error;
স্থির char_u *prt_ps_file_name = ফাঁকা;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
স্থির দশমিক prt_page_width;
স্থির দশমিক prt_page_height;
স্থির দশমিক prt_বাম_margin;
স্থির দশমিক prt_ডান_margin;
স্থির দশমিক prt_শীর্ষ_margin;
স্থির দশমিক prt_bottom_margin;
স্থির দশমিক prt_সারি_height;
স্থির দশমিক prt_first_সারি_height;
স্থির দশমিক prt_char_width;
স্থির দশমিক prt_সংখ্যা_width;
স্থির দশমিক prt_bgcol_offset;
স্থির দশমিক prt_pos_x_moveto = 0.0;
স্থির দশমিক prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
স্থির পূর্ণসংখ্যা prt_need_moveto;
স্থির পূর্ণসংখ্যা prt_do_moveto;
স্থির পূর্ণসংখ্যা prt_need_font;
স্থির পূর্ণসংখ্যা prt_font;
স্থির পূর্ণসংখ্যা prt_need_underline;
স্থির পূর্ণসংখ্যা prt_underline;
স্থির পূর্ণসংখ্যা prt_do_underline;
স্থির পূর্ণসংখ্যা prt_need_fgcol;
স্থির পূর্ণসংখ্যা prt_fgcol;
স্থির পূর্ণসংখ্যা prt_need_bgcol;
স্থির পূর্ণসংখ্যা prt_do_bgcol;
স্থির পূর্ণসংখ্যা prt_bgcol;
স্থির পূর্ণসংখ্যা prt_new_bgcol;
স্থির পূর্ণসংখ্যা prt_attribute_change;
স্থির দশমিক prt_text_run;
স্থির পূর্ণসংখ্যা prt_page_অঙ্ক;
স্থির পূর্ণসংখ্যা prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
স্থির পূর্ণসংখ্যা prt_media;
স্থির পূর্ণসংখ্যা prt_portrait;
স্থির পূর্ণসংখ্যা prt_অঙ্ক_copies;
স্থির পূর্ণসংখ্যা prt_duplex;
স্থির পূর্ণসংখ্যা prt_tumble;
স্থির পূর্ণসংখ্যা prt_collate;

/*
 * Buffers used when generating PostScript output
 */
স্থির char_u prt_সারি_বাফার[257];
স্থির garray_T prt_ps_বাফার;

স্থির পূর্ণসংখ্যা prt_do_conv;
স্থির vimconv_T prt_conv;

স্থির পূর্ণসংখ্যা prt_out_mbyte;
স্থির পূর্ণসংখ্যা prt_custom_cmap;
স্থির অক্ষর prt_cmap[80];
স্থির পূর্ণসংখ্যা prt_use_courier;
স্থির পূর্ণসংখ্যা prt_in_ascii;
স্থির পূর্ণসংখ্যা prt_half_width;
স্থির অক্ষর *prt_ascii_encoding;
স্থির ধ্রুবক char_u prt_hexchar[] = "0123456789abcdef";

    স্থির শূন্য
prt_write_file_raw_len(char_u *বাফার, পূর্ণসংখ্যা bytes)
{
    যদি (!prt_file_error
	    && fwrite(বাফার, আকার(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    স্থির শূন্য
prt_write_file(char_u *বাফার)
{
    prt_write_file_len(বাফার, (পূর্ণসংখ্যা)STRLEN(বাফার));
}

    স্থির শূন্য
prt_write_file_len(char_u *বাফার, পূর্ণসংখ্যা bytes)
{
    prt_write_file_raw_len(বাফার, bytes);
}

/*
 * Write a string.
 */
    স্থির শূন্য
prt_write_string(অক্ষর *s)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার), "%s", s);
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write an int and a space.
 */
    স্থির শূন্য
prt_write_int(পূর্ণসংখ্যা i)
{
    sprintf((অক্ষর *)prt_সারি_বাফার, "%d ", i);
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write a boolean and a space.
 */
    স্থির শূন্য
prt_write_boolean(পূর্ণসংখ্যা b)
{
    sprintf((অক্ষর *)prt_সারি_বাফার, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    স্থির শূন্য
prt_def_font(
    অক্ষর	*new_name,
    অক্ষর	*encoding,
    পূর্ণসংখ্যা		height,
    অক্ষর	*font)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_সারি_বাফার);
    যদি (prt_out_mbyte)
	sprintf((অক্ষর *)prt_সারি_বাফার, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    অন্যথা
	vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write a line to define the CID font.
 */
    স্থির শূন্য
prt_def_cidfont(অক্ষর *new_name, পূর্ণসংখ্যা height, অক্ষর *cidfont)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_সারি_বাফার);
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    স্থির শূন্য
prt_dup_cidfont(অক্ষর *original_name, অক্ষর *new_name)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    স্থির শূন্য
prt_real_bits(
    দ্বিগুণ      real,
    পূর্ণসংখ্যা		precision,
    পূর্ণসংখ্যা		*pinteger,
    পূর্ণসংখ্যা		*pfraction)
{
    পূর্ণসংখ্যা     i;
    পূর্ণসংখ্যা     integer;
    দশমিক   fraction;

    integer = (পূর্ণসংখ্যা)real;
    fraction = (দশমিক)(real - integer);
    যদি (real < (দ্বিগুণ)integer)
	fraction = -fraction;
    জন্য (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (পূর্ণসংখ্যা)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    স্থির শূন্য
prt_write_real(দ্বিগুণ val, পূর্ণসংখ্যা prec)
{
    পূর্ণসংখ্যা     integer;
    পূর্ণসংখ্যা     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((অক্ষর *)prt_সারি_বাফার, "%d", integer);
    prt_write_file(prt_সারি_বাফার);
    // Only emit fraction if necessary
    যদি (fraction != 0)
    {
	// Remove any trailing zeros
	যখন ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((অক্ষর *)prt_সারি_বাফার, ".%0*d", prec, fraction);
	prt_write_file(prt_সারি_বাফার);
    }
    sprintf((অক্ষর *)prt_সারি_বাফার, " ");
    prt_write_file(prt_সারি_বাফার);
}

/*
 * Write a line to define a numeric variable.
 */
    স্থির শূন্য
prt_def_var(অক্ষর *name, দ্বিগুণ মান, পূর্ণসংখ্যা prec)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
								"/%s ", name);
    prt_write_file(prt_সারি_বাফার);
    prt_write_real(মান, prec);
    sprintf((অক্ষর *)prt_সারি_বাফার, "d\n");
    prt_write_file(prt_সারি_বাফার);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, আকার_2)    ((আকার_2) * ((scale)/1000.0))

    স্থির শূন্য
prt_flush_বাফার(শূন্য)
{
    যদি (prt_ps_বাফার.ga_len <= 0)
	ফেরত;

    // Any background color must be drawn first
    যদি (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	পূর্ণসংখ্যা     r, g, b;

	যদি (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_সারি_height, 2);

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
    যদি (prt_do_underline)
    {
	যদি (prt_do_moveto)
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
    যদি (prt_out_mbyte)
	prt_write_string("<");
    অন্যথা
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_বাফার.ga_তথ্য, prt_ps_বাফার.ga_len);
    যদি (prt_out_mbyte)
	prt_write_string(">");
    অন্যথা
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    যদি (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    অন্যথা // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_বাফার);
    ga_init2(&prt_ps_বাফার, আকার(অক্ষর), prt_bufsiz);
}


    স্থির শূন্য
prt_resource_name(char_u *filename, শূন্য *cookie)
{
    char_u *resource_filename = cookie;

    যদি (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    অন্যথা
	STRCPY(resource_filename, filename);
}

    স্থির পূর্ণসংখ্যা
prt_find_resource(অক্ষর *name, কাঠামো prt_ps_resource_S *resource)
{
    char_u	*বাফার;
    পূর্ণসংখ্যা		retval;

    বাফার = alloc(MAXPATHL + 1);
    যদি (বাফার == ফাঁকা)
	ফেরত FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(বাফার, "print");
    add_pathsep(বাফার);
    vim_strcat(বাফার, (char_u *)name, MAXPATHL);
    vim_strcat(বাফার, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_in_runtimepath(বাফার, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(বাফার);
    ফেরত retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_বাফার_LEN (2048)
কাঠামো prt_resfile_বাফার_S
{
    char_u  বাফার[PRT_FILE_বাফার_LEN];
    পূর্ণসংখ্যা     len;
    পূর্ণসংখ্যা     সারি_start;
    পূর্ণসংখ্যা     সারি_end;
};

স্থির কাঠামো prt_resfile_বাফার_S prt_resfile;

    স্থির পূর্ণসংখ্যা
prt_resfile_পরবর্তী_সারি(শূন্য)
{
    পূর্ণসংখ্যা     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.সারি_end + 1;
    যখন (idx < prt_resfile.len)
    {
	যদি (prt_resfile.বাফার[idx] != PSLF && prt_resfile.বাফার[idx] != PSCR)
	    বিরতি;
	idx++;
    }
    prt_resfile.সারি_start = idx;

    যখন (idx < prt_resfile.len)
    {
	যদি (prt_resfile.বাফার[idx] == PSLF || prt_resfile.বাফার[idx] == PSCR)
	    বিরতি;
	idx++;
    }
    prt_resfile.সারি_end = idx;

    ফেরত (idx < prt_resfile.len);
}

    স্থির পূর্ণসংখ্যা
prt_resfile_strncmp(পূর্ণসংখ্যা offset, অক্ষর *string, পূর্ণসংখ্যা len)
{
    // Force not equal if string is longer than remainder of line
    যদি (len > (prt_resfile.সারি_end - (prt_resfile.সারি_start + offset)))
	ফেরত 1;

    ফেরত STRNCMP(&prt_resfile.বাফার[prt_resfile.সারি_start + offset],
								string, len);
}

    স্থির পূর্ণসংখ্যা
prt_resfile_skip_nonws(পূর্ণসংখ্যা offset)
{
    পূর্ণসংখ্যা     idx;

    idx = prt_resfile.সারি_start + offset;
    যখন (idx < prt_resfile.সারি_end)
    {
	যদি (SAFE_isspace(prt_resfile.বাফার[idx]))
	    ফেরত idx - prt_resfile.সারি_start;
	idx++;
    }
    ফেরত -1;
}

    স্থির পূর্ণসংখ্যা
prt_resfile_skip_ws(পূর্ণসংখ্যা offset)
{
    পূর্ণসংখ্যা     idx;

    idx = prt_resfile.সারি_start + offset;
    যখন (idx < prt_resfile.সারি_end)
    {
	যদি (!SAFE_isspace(prt_resfile.বাফার[idx]))
	    ফেরত idx - prt_resfile.সারি_start;
	idx++;
    }
    ফেরত -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    স্থির পূর্ণসংখ্যা
prt_পরবর্তী_dsc(কাঠামো prt_dsc_সারি_S *p_dsc_সারি)
{
    পূর্ণসংখ্যা     comment;
    পূর্ণসংখ্যা     offset;

    // Move to start of next line
    যদি (!prt_resfile_পরবর্তী_সারি())
	ফেরত FALSE;

    // DSC comments always start %%
    যদি (prt_resfile_strncmp(0, "%%", 2) != 0)
	ফেরত FALSE;

    // Find type of DSC comment
    জন্য (comment = 0; comment < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(prt_dsc_table); comment++)
	যদি (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    বিরতি;

    যদি (comment != ARRAY_দৈর্ঘ্য(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_সারি->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    অন্যথা
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_সারি->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	যদি (offset == -1)
	    ফেরত FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    যদি (offset == -1)
	ফেরত FALSE;

    p_dsc_সারি->string = &prt_resfile.বাফার[prt_resfile.সারি_start + offset];
    p_dsc_সারি->len = prt_resfile.সারি_end - (prt_resfile.সারি_start + offset);

    ফেরত TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    স্থির পূর্ণসংখ্যা
prt_open_resource(কাঠামো prt_ps_resource_S *resource)
{
    পূর্ণসংখ্যা		offset;
    পূর্ণসংখ্যা		seen_all;
    পূর্ণসংখ্যা		seen_title;
    পূর্ণসংখ্যা		seen_version;
    FILE	*fd_resource;
    কাঠামো prt_dsc_সারি_S dsc_সারি;

    fd_resource = mch_fopen((অক্ষর *)resource->filename, READBIN);
    যদি (fd_resource == ফাঁকা)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	ফেরত FALSE;
    }
    CLEAR_FIELD(prt_resfile.বাফার);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (পূর্ণসংখ্যা)fread((অক্ষর *)prt_resfile.বাফার, আকার(char_u),
					    PRT_FILE_বাফার_LEN, fd_resource);
    যদি (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	ফেরত FALSE;
    }
    fclose(fd_resource);

    prt_resfile.সারি_end = -1;
    prt_resfile.সারি_start = 0;
    যদি (!prt_resfile_পরবর্তী_সারি())
	ফেরত FALSE;

    offset = 0;

    যদি (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	ফেরত FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    যদি (offset == -1)
	ফেরত FALSE;
    offset = prt_resfile_skip_ws(offset);
    যদি (offset == -1)
	ফেরত FALSE;

    যদি (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ফেরত FALSE;
    }
    offset += (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    যদি (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    অন্যথা যদি (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    অন্যথা যদি (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (পূর্ণসংখ্যা)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    অন্যথা
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ফেরত FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    যখন (!seen_all && prt_পরবর্তী_dsc(&dsc_সারি))
    {
	নির্বাচন (dsc_সারি.type)
	{
	ক্ষেত্র PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_সারি.string, dsc_সারি.len);
	    seen_title = TRUE;
	    যদি (seen_version)
		seen_all = TRUE;
	    বিরতি;

	ক্ষেত্র PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_সারি.string, dsc_সারি.len);
	    seen_version = TRUE;
	    যদি (seen_title)
		seen_all = TRUE;
	    বিরতি;

	ক্ষেত্র PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    বিরতি;

	ক্ষেত্র PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    বিরতি;
	}
    }

    যদি (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	ফেরত FALSE;
    }

    ফেরত TRUE;
}

    স্থির পূর্ণসংখ্যা
prt_check_resource(কাঠামো prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    যদি (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	ফেরত FALSE;
    }

    // Other checks to be added as needed
    ফেরত TRUE;
}

    স্থির শূন্য
prt_dsc_start(শূন্য)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    স্থির শূন্য
prt_dsc_noarg(অক্ষর *comment)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
							 "%%%%%s\n", comment);
    prt_write_file(prt_সারি_বাফার);
}

    স্থির শূন্য
prt_dsc_textline(অক্ষর *comment, অক্ষর *text)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_সারি_বাফার);
}

    স্থির শূন্য
prt_dsc_text(অক্ষর *comment, অক্ষর *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_সারি_বাফার);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    স্থির শূন্য
prt_dsc_ints(অক্ষর *comment, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা *ints)
{
    পূর্ণসংখ্যা		i;

    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
							  "%%%%%s:", comment);
    prt_write_file(prt_সারি_বাফার);

    জন্য (i = 0; i < গুনতি; i++)
    {
	sprintf((অক্ষর *)prt_সারি_বাফার, " %d", ints[i]);
	prt_write_file(prt_সারি_বাফার);
    }

    prt_write_string("\n");
}

    স্থির শূন্য
prt_dsc_resources(
    অক্ষর	*comment,	// if NULL add to previous
    অক্ষর	*type,
    অক্ষর	*string)
{
    যদি (comment != ফাঁকা)
	vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
						 "%%%%%s: %s", comment, type);
    অন্যথা
	vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
							    "%%%%+ %s", type);
    prt_write_file(prt_সারি_বাফার);

    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
							     " %s\n", string);
    prt_write_file(prt_সারি_বাফার);
}

    স্থির শূন্য
prt_dsc_font_resource(অক্ষর *resource, কাঠামো prt_ps_font_S *ps_font)
{
    পূর্ণসংখ্যা     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    জন্য (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	যদি (ps_font->ps_fontname[i] != ফাঁকা)
	    prt_dsc_resources(ফাঁকা, "font", ps_font->ps_fontname[i]);
}

    স্থির শূন্য
prt_dsc_requirements(
    পূর্ণসংখ্যা		duplex,
    পূর্ণসংখ্যা		tumble,
    পূর্ণসংখ্যা		collate,
    পূর্ণসংখ্যা		color,
    পূর্ণসংখ্যা		অঙ্ক_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    যদি (!(duplex || collate || color || (অঙ্ক_copies > 1)))
	ফেরত;

    sprintf((অক্ষর *)prt_সারি_বাফার, "%%%%Requirements:");
    prt_write_file(prt_সারি_বাফার);

    যদি (duplex)
    {
	prt_write_string(" duplex");
	যদি (tumble)
	    prt_write_string("(tumble)");
    }
    যদি (collate)
	prt_write_string(" collate");
    যদি (color)
	prt_write_string(" color");
    যদি (অঙ্ক_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((অক্ষর *)prt_সারি_বাফার, "%d", অঙ্ক_copies);
	prt_write_file(prt_সারি_বাফার);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    স্থির শূন্য
prt_dsc_docmedia(
    অক্ষর	*paper_name,
    দ্বিগুণ	width,
    দ্বিগুণ	height,
    দ্বিগুণ	weight,
    অক্ষর	*colour,
    অক্ষর	*type)
{
    vim_snprintf((অক্ষর *)prt_সারি_বাফার, আকার(prt_সারি_বাফার),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_সারি_বাফার);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    যদি (colour == ফাঁকা)
	prt_write_string("()");
    অন্যথা
	prt_write_string(colour);
    prt_write_string(" ");
    যদি (type == ফাঁকা)
	prt_write_string("()");
    অন্যথা
	prt_write_string(type);
    prt_write_string("\n");
}

    শূন্য
mch_print_cleanup(শূন্য)
{
    যদি (prt_out_mbyte)
    {
	পূর্ণসংখ্যা     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	জন্য (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    যদি (prt_ps_mb_font.ps_fontname[i] != ফাঁকা)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    যদি (prt_do_conv)
    {
	convert_setup(&prt_conv, ফাঁকা, ফাঁকা);
	prt_do_conv = FALSE;
    }
    যদি (prt_ps_fd != ফাঁকা)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ফাঁকা;
	prt_file_error = FALSE;
    }
    যদি (prt_ps_file_name != ফাঁকা)
	VIM_CLEAR(prt_ps_file_name);
}

    স্থির দশমিক
to_device_units(পূর্ণসংখ্যা idx, দ্বিগুণ physsize, পূর্ণসংখ্যা def_সংখ্যা)
{
    দশমিক	ret;
    পূর্ণসংখ্যা		u;
    পূর্ণসংখ্যা		nr;

    u = prt_get_unit(idx);
    যদি (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_সংখ্যা;
    }
    অন্যথা
	nr = printer_opts[idx].সংখ্যা;

    নির্বাচন (u)
    {
	ক্ষেত্র PRT_UNIT_INCH:
	    ret = (দশমিক)(nr * PRT_PS_DEFAULT_DPI);
	    বিরতি;
	ক্ষেত্র PRT_UNIT_MM:
	    ret = (দশমিক)(nr * PRT_PS_DEFAULT_DPI) / (দশমিক)25.4;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_POINT:
	    ret = (দশমিক)nr;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_PERC:
	পূর্বনির্ধারিত:
	    ret = (দশমিক)(physsize * nr) / 100;
	    বিরতি;
    }

    ফেরত ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    স্থির শূন্য
prt_page_margins(
    দ্বিগুণ	width,
    দ্বিগুণ	height,
    দ্বিগুণ	*বাম,
    দ্বিগুণ	*ডান,
    দ্বিগুণ	*শীর্ষ_2,
    দ্বিগুণ	*bottom)
{
    *বাম   = to_device_units(OPT_PRINT_বাম, width, 10);
    *ডান  = width - to_device_units(OPT_PRINT_ডান, width, 5);
    *শীর্ষ_2    = height - to_device_units(OPT_PRINT_শীর্ষ, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    স্থির শূন্য
prt_font_metrics(পূর্ণসংখ্যা font_scale)
{
    prt_সারি_height = (দশমিক)font_scale;
    prt_char_width = (দশমিক)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    স্থির পূর্ণসংখ্যা
prt_get_cpl(শূন্য)
{
    যদি (prt_use_সংখ্যা())
    {
	prt_সংখ্যা_width = PRINT_সংখ্যা_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	যদি (prt_out_mbyte)
	    prt_সংখ্যা_width /= 2;
	prt_বাম_margin += prt_সংখ্যা_width;
    }
    অন্যথা
	prt_সংখ্যা_width = 0.0;

    ফেরত (পূর্ণসংখ্যা)((prt_ডান_margin - prt_বাম_margin) / prt_char_width);
}

    স্থির পূর্ণসংখ্যা
prt_build_cid_fontname(পূর্ণসংখ্যা font, char_u *name, পূর্ণসংখ্যা name_len)
{
    অক্ষর    *fontname;

    fontname = alloc(name_len + 1);
    যদি (fontname == ফাঁকা)
	ফেরত FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    ফেরত TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    স্থির পূর্ণসংখ্যা
prt_get_lpp(শূন্য)
{
    পূর্ণসংখ্যা lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (দশমিক)PRT_PS_FONT_TO_USER(prt_সারি_height,
					   prt_ps_font->bbox_min_y);
    যদি ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (দশমিক)PRT_PS_FONT_TO_USER(prt_সারি_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_সারি_height = prt_সারি_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (পূর্ণসংখ্যা)((prt_শীর্ষ_margin - prt_bottom_margin) / prt_সারি_height);

    // Adjust top margin if there is a header
    prt_শীর্ষ_margin -= prt_সারি_height * prt_header_height();

    ফেরত lpp - prt_header_height();
}

    স্থির পূর্ণসংখ্যা
prt_match_encoding(
    অক্ষর			*p_encoding,
    কাঠামো prt_ps_mbfont_S	*p_cmap,
    কাঠামো prt_ps_encoding_S	**pp_mbenc)
{
    পূর্ণসংখ্যা				mbenc;
    পূর্ণসংখ্যা				enc_len;
    কাঠামো prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ফাঁকা;
    // Look for recognised encoding
    enc_len = (পূর্ণসংখ্যা)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    জন্য (mbenc = 0; mbenc < p_cmap->অঙ্ক_encodings; mbenc++)
    {
	যদি (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    ফেরত TRUE;
	}
	p_mbenc++;
    }
    ফেরত FALSE;
}

    স্থির পূর্ণসংখ্যা
prt_match_charset(
    অক্ষর		    *p_charset,
    কাঠামো prt_ps_mbfont_S  *p_cmap,
    কাঠামো prt_ps_charset_S **pp_mbchar)
{
    পূর্ণসংখ্যা			    mbchar;
    পূর্ণসংখ্যা			    char_len;
    কাঠামো prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    যদি (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (পূর্ণসংখ্যা)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    জন্য (mbchar = 0; mbchar < p_cmap->অঙ্ক_charsets; mbchar++)
    {
	যদি (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    ফেরত TRUE;
	}
	p_mbchar++;
    }
    ফেরত FALSE;
}

    পূর্ণসংখ্যা
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    পূর্ণসংখ্যা		forceit UNUSED)
{
    পূর্ণসংখ্যা		i;
    অক্ষর	*paper_name;
    পূর্ণসংখ্যা		paper_strlen;
    পূর্ণসংখ্যা		fontsize;
    char_u	*p;
    দ্বিগুণ      বাম;
    দ্বিগুণ      ডান;
    দ্বিগুণ      শীর্ষ_2;
    দ্বিগুণ      bottom;
    পূর্ণসংখ্যা		props;
    পূর্ণসংখ্যা		cmap = 0;
    char_u	*p_encoding;
    কাঠামো prt_ps_encoding_S *p_mbenc;
    কাঠামো prt_ps_encoding_S *p_mbenc_first;
    কাঠামো prt_ps_charset_S  *p_mbchar = ফাঁকা;

# যদি 0
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
    যদি (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    যদি (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ফাঁকা;
    props = enc_canon_props(p_encoding);
    যদি (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	পূর্ণসংখ্যা cmap_first = 0;

	p_mbenc_first = ফাঁকা;
	জন্য (cmap = 0; cmap < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(prt_ps_mbfonts); cmap++)
	    যদি (prt_match_encoding((অক্ষর *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		যদি (p_mbenc_first == ফাঁকা)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		যদি (prt_match_charset((অক্ষর *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    বিরতি;
	    }

	// Use first encoding matched if no charset matched
	যদি (p_mbchar == ফাঁকা && p_mbenc_first != ফাঁকা)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != ফাঁকা);
    যদি (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ফাঁকা);
	যদি (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    যদি ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		ফেরত FALSE;
	    }

	    // Add charset name if not empty
	    যদি (p_mbchar->cmap_charset != ফাঁকা)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, আকার(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	অন্যথা
	{
	    // Add custom CMap character set name
	    যদি (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		ফেরত FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, আকার(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	যদি (p_mbenc->cmap_encoding != ফাঁকা && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < আকার(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	যদি (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    ফেরত FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	যদি (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    ফেরত FALSE;
	যদি (mbfont_opts[OPT_MBFONT_BOLD].present)
	    যদি (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		ফেরত FALSE;
	যদি (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    যদি (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		ফেরত FALSE;
	যদি (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    যদি (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		ফেরত FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	যদি (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    যদি (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    অন্যথা
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    অন্যথা
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    যদি (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (অক্ষর *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    অন্যথা
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    জন্য (i = 0; i < (পূর্ণসংখ্যা)PRT_MEDIASIZE_LEN; ++i)
	যদি (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    বিরতি;
    যদি (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    যদি (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    অন্যথা
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &বাম, &ডান, &শীর্ষ_2,
								    &bottom);
    prt_বাম_margin = (দশমিক)বাম;
    prt_ডান_margin = (দশমিক)ডান;
    prt_শীর্ষ_margin = (দশমিক)শীর্ষ_2;
    prt_bottom_margin = (দশমিক)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    জন্য (p = p_pfn; (p = vim_strchr(p, ':')) != ফাঁকা; ++p)
	যদি (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((অক্ষর *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_সারি = prt_get_cpl();
    psettings->সারিগুলি_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    যদি (psettings->chars_per_সারি <= 0 || psettings->সারিগুলি_per_page <= 0)
	ফেরত FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_অঙ্ক_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    যদি (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    অন্যথা
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_অঙ্ক_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    যদি (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	যদি (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	অন্যথা যদি (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    যদি (psettings->outfile == ফাঁকা)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	যদি (prt_ps_file_name == ফাঁকা)
	{
	    emsg(_(e_cant_get_অস্থায়ী_file_name));
	    ফেরত FAIL;
	}
	prt_ps_fd = mch_fopen((অক্ষর *)prt_ps_file_name, WRITEBIN);
    }
    অন্যথা
    {
	p = expand_env_save(psettings->outfile);
	যদি (p != ফাঁকা)
	{
	    prt_ps_fd = mch_fopen((অক্ষর *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    যদি (prt_ps_fd == ফাঁকা)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	ফেরত FAIL;
    }

    prt_bufsiz = psettings->chars_per_সারি;
    যদি (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_বাফার, আকার(অক্ষর), prt_bufsiz);

    prt_page_অঙ্ক = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
prt_add_resource(কাঠামো prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_বাফার[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((অক্ষর *)resource->filename, READBIN);
    যদি (fd_resource == ফাঁকা)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	ফেরত FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (অক্ষর *)resource->title);

    prt_dsc_textline("BeginDocument", (অক্ষর *)resource->filename);

    জন্য (;;)
    {
	bytes_read = fread((অক্ষর *)resource_বাফার, আকার(char_u),
			   আকার(resource_বাফার), fd_resource);
	যদি (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    ফেরত FALSE;
	}
	যদি (bytes_read == 0)
	    বিরতি;
	prt_write_file_raw_len(resource_বাফার, (পূর্ণসংখ্যা)bytes_read);
	যদি (prt_file_error)
	{
	    fclose(fd_resource);
	    ফেরত FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    ফেরত TRUE;
}

    পূর্ণসংখ্যা
mch_print_begin(prt_settings_T *psettings)
{
    পূর্ণসংখ্যা		bbox[4];
    দ্বিগুণ      বাম;
    দ্বিগুণ      ডান;
    দ্বিগুণ      শীর্ষ_2;
    দ্বিগুণ      bottom;
    কাঠামো prt_ps_resource_S *res_prolog;
    কাঠামো prt_ps_resource_S *res_encoding;
    অক্ষর	বাফার[256];
    char_u      *p_encoding;
    কাঠামো prt_ps_resource_S *res_cidfont;
    কাঠামো prt_ps_resource_S *res_cmap;
    পূর্ণসংখ্যা		retval = FALSE;

    res_prolog = ALLOC_ONE(কাঠামো prt_ps_resource_S);
    res_encoding = ALLOC_ONE(কাঠামো prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(কাঠামো prt_ps_resource_S);
    res_cmap = ALLOC_ONE(কাঠামো prt_ps_resource_S);
    যদি (res_prolog == ফাঁকা || res_encoding == ফাঁকা
	    || res_cidfont == ফাঁকা || res_cmap == ফাঁকা)
	যাও theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (অক্ষর *)psettings->jobname);
    যদি (!get_user_name((char_u *)বাফার, 256))
	STRCPY(বাফার, "Unknown");
    prt_dsc_textline("For", বাফার);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(ফাঁকা), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &বাম, &ডান, &শীর্ষ_2, &bottom);
    bbox[0] = (পূর্ণসংখ্যা)বাম;
    যদি (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (পূর্ণসংখ্যা)(শীর্ষ_2 - (psettings->সারিগুলি_per_page + prt_header_height())
						    * (দ্বিগুণ)prt_সারি_height);
	bbox[2] = (পূর্ণসংখ্যা)(বাম + psettings->chars_per_সারি
					       * (দ্বিগুণ)prt_char_width + 0.5);
	bbox[3] = (পূর্ণসংখ্যা)(শীর্ষ_2 + 0.5);
    }
    অন্যথা
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (পূর্ণসংখ্যা)bottom;
	bbox[2] = (পূর্ণসংখ্যা)(বাম + ((psettings->সারিগুলি_per_page
			      + prt_header_height()) * prt_সারি_height) + 0.5);
	bbox[3] = (পূর্ণসংখ্যা)(bottom + psettings->chars_per_সারি
					       * (দ্বিগুণ)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(দ্বিগুণ)0, ফাঁকা, ফাঁকা);
    // Define fonts needed
    যদি (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    যদি (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ফাঁকা
				: "DocumentNeededResources"), &prt_ps_mb_font);
	যদি (!prt_custom_cmap)
	    prt_dsc_resources(ফাঁকা, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    যদি (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	যাও theend;
    }
    যদি (!prt_open_resource(res_prolog))
	যাও theend;
    যদি (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	যাও theend;
    যদি (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	যদি (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    যাও theend;
	}
	যদি (!prt_open_resource(res_cidfont))
	    যাও theend;
	যদি (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    যাও theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    যদি (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	যদি (*p_encoding == NUL
		|| !prt_find_resource((অক্ষর *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    পূর্ণসংখ্যা		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    যদি (!(props & ENC_8BIT)
		    || !prt_find_resource((অক্ষর *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		যদি (!prt_find_resource((অক্ষর *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    যাও theend;
		}
	    }
	}
	যদি (!prt_open_resource(res_encoding))
	    যাও theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    অন্যথা
    {
	p_encoding = enc_skip(p_penc);
	যদি (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	যদি (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    যদি (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		যাও theend;
	    }
	    যদি (!prt_open_resource(res_encoding))
		যাও theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    যদি (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	যদি (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    যাও theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    যদি (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	যদি (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    যাও theend;
	}
	যদি (!prt_open_resource(res_cmap))
	    যাও theend;
    }

    // List resources supplied
    STRCPY(বাফার, res_prolog->title);
    STRCAT(বাফার, " ");
    STRCAT(বাফার, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", বাফার);
    যদি (prt_out_mbyte)
    {
	STRCPY(বাফার, res_cidfont->title);
	STRCAT(বাফার, " ");
	STRCAT(বাফার, res_cidfont->version);
	prt_dsc_resources(ফাঁকা, "procset", বাফার);

	যদি (prt_custom_cmap)
	{
	    STRCPY(বাফার, res_cmap->title);
	    STRCAT(বাফার, " ");
	    STRCAT(বাফার, res_cmap->version);
	    prt_dsc_resources(ফাঁকা, "cmap", বাফার);
	}
    }
    যদি (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(বাফার, res_encoding->title);
	STRCAT(বাফার, " ");
	STRCAT(বাফার, res_encoding->version);
	prt_dsc_resources(ফাঁকা, "encoding", বাফার);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# অন্যথা
					0
# endif
					, prt_অঙ্ক_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    যদি (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    যদি (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ফাঁকা : "PageResources"),
							    &prt_ps_mb_font);
	যদি (!prt_custom_cmap)
	    prt_dsc_resources(ফাঁকা, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    যদি (!prt_add_resource(res_prolog))
	যাও theend;
    যদি (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	যদি (!prt_add_resource(res_cidfont))
	    যাও theend;
	যদি (prt_custom_cmap && !prt_add_resource(res_cmap))
	    যাও theend;
    }

    যদি (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	যদি (!prt_add_resource(res_encoding))
	    যাও theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((পূর্ণসংখ্যা)prt_mediasize[prt_media].width);
    prt_write_int((পূর্ণসংখ্যা)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_অঙ্ক_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    যদি (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	যদি (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (অক্ষর *)p_encoding, (পূর্ণসংখ্যা)prt_সারি_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (অক্ষর *)p_encoding, (পূর্ণসংখ্যা)prt_সারি_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (অক্ষর *)p_encoding, (পূর্ণসংখ্যা)prt_সারি_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (অক্ষর *)p_encoding, (পূর্ণসংখ্যা)prt_সারি_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    যদি (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	যদি (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (পূর্ণসংখ্যা)prt_সারি_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	যদি (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ফাঁকা)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    যদি (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (পূর্ণসংখ্যা)prt_সারি_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	অন্যথা
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	যদি (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ফাঁকা)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    যদি (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (পূর্ণসংখ্যা)prt_সারি_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	অন্যথা
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	যদি (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ফাঁকা)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    যদি (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (পূর্ণসংখ্যা)prt_সারি_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	অন্যথা
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_সারি_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_সারি_height,
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

    ফেরত retval;
}

    শূন্য
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_অঙ্ক);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    যদি (!prt_file_error && psettings->outfile == ফাঁকা
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	যদি (prt_ps_fd != ফাঁকা)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ফাঁকা;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	যদি (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	অন্যথা
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    পূর্ণসংখ্যা
mch_print_end_page(শূন্য)
{
    prt_flush_বাফার();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    ফেরত !prt_file_error;
}

    পূর্ণসংখ্যা
mch_print_begin_page(char_u *str UNUSED)
{
    পূর্ণসংখ্যা		page_অঙ্ক[2];

    prt_page_অঙ্ক++;

    page_অঙ্ক[0] = page_অঙ্ক[1] = prt_page_অঙ্ক;
    prt_dsc_ints("Page", 2, page_অঙ্ক);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    যদি (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    অন্যথা
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    যদি (!prt_portrait)
    {
	prt_write_int(-((পূর্ণসংখ্যা)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    ফেরত !prt_file_error;
}

    পূর্ণসংখ্যা
mch_print_blank_page(শূন্য)
{
    ফেরত (mch_print_begin_page(ফাঁকা) ? (mch_print_end_page()) : FALSE);
}

স্থির দশমিক prt_pos_x = 0;
স্থির দশমিক prt_pos_y = 0;

    শূন্য
mch_print_start_সারি(পূর্ণসংখ্যা margin, পূর্ণসংখ্যা page_সারি)
{
    prt_pos_x = prt_বাম_margin;
    যদি (margin)
	prt_pos_x -= prt_সংখ্যা_width;

    prt_pos_y = prt_শীর্ষ_margin - prt_first_সারি_height -
					page_সারি * prt_সারি_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    পূর্ণসংখ্যা
mch_print_text_out(char_u *textp, পূর্ণসংখ্যা len UNUSED)
{
    char_u	*p = textp;
    পূর্ণসংখ্যা		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    দশমিক       char_width;
    দশমিক       পরবর্তী_pos;
    পূর্ণসংখ্যা		in_ascii;
    পূর্ণসংখ্যা		half_width;
    char_u	*tofree = ফাঁকা;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    যদি (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	যদি (prt_in_ascii)
	{
	    যদি (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	অন্যথা যদি (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    যদি (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	যদি (half_width)
	    char_width /= 2;
	যদি (prt_half_width)
	{
	    যদি (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	অন্যথা যদি (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    যদি (prt_attribute_change)
    {
	prt_flush_বাফার();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	যদি (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	যদি (prt_need_font)
	{
	    যদি (!prt_in_ascii)
		prt_write_string("CF");
	    অন্যথা
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	যদি (prt_need_fgcol)
	{
	    পূর্ণসংখ্যা     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    যদি (r == g && g == b)
		prt_write_string("g\n");
	    অন্যথা
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	যদি (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    যদি (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	অন্যথা
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	যদি (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    যদি (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	যদি (p == ফাঁকা)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    যদি (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	যখন (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_বাফার, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_বাফার, ch);
	    p++;
	}
    }
    অন্যথা
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	যদি (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_বাফার, '\\');
	    নির্বাচন (ch)
	    {
		ক্ষেত্র BS:   ga_append(&prt_ps_বাফার, 'b'); বিরতি;
		ক্ষেত্র TAB:  ga_append(&prt_ps_বাফার, 't'); বিরতি;
		ক্ষেত্র NL:   ga_append(&prt_ps_বাফার, 'n'); বিরতি;
		ক্ষেত্র FF:   ga_append(&prt_ps_বাফার, 'f'); বিরতি;
		ক্ষেত্র CAR:  ga_append(&prt_ps_বাফার, 'r'); বিরতি;
		ক্ষেত্র '(':  ga_append(&prt_ps_বাফার, '('); বিরতি;
		ক্ষেত্র ')':  ga_append(&prt_ps_বাফার, ')'); বিরতি;
		ক্ষেত্র '\\': ga_append(&prt_ps_বাফার, '\\'); বিরতি;

		পূর্বনির্ধারিত:
			   sprintf((অক্ষর *)ch_buff, "%03o", (unsigned পূর্ণসংখ্যা)ch);
			   ga_append(&prt_ps_বাফার, ch_buff[0]);
			   ga_append(&prt_ps_বাফার, ch_buff[1]);
			   ga_append(&prt_ps_বাফার, ch_buff[2]);
			   বিরতি;
	    }
	}
	অন্যথা
	    ga_append(&prt_ps_বাফার, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    পরবর্তী_pos = prt_pos_x + prt_char_width;
    need_break = (পরবর্তী_pos > prt_ডান_margin) &&
		    ((পরবর্তী_pos - prt_ডান_margin) > (prt_ডান_margin*1e-5));

    যদি (need_break)
	prt_flush_বাফার();

    ফেরত need_break;
}

    শূন্য
mch_print_set_font(পূর্ণসংখ্যা iBold, পূর্ণসংখ্যা iItalic, পূর্ণসংখ্যা iUnderline)
{
    পূর্ণসংখ্যা		font = 0;

    যদি (iBold)
	font |= 0x01;
    যদি (iItalic)
	font |= 0x02;

    যদি (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    যদি (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    শূন্য
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (পূর্ণসংখ্যা)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    শূন্য
mch_print_set_fg(long_u fgcol)
{
    যদি (fgcol == (long_u)prt_fgcol)
	ফেরত;

    prt_fgcol = (পূর্ণসংখ্যা)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
