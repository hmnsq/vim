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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#నిర్వచితమైతే FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_పట్టిక_T mbfont_opts[OPT_MBFONT_అంకె_OPTIONS] =
{
    {"c",	FALSE, 0, ఖాళీ, 0, FALSE},
    {"a",	FALSE, 0, ఖాళీ, 0, FALSE},
    {"r",	FALSE, 0, ఖాళీ, 0, FALSE},
    {"b",	FALSE, 0, ఖాళీ, 0, FALSE},
    {"i",	FALSE, 0, ఖాళీ, 0, FALSE},
    {"o",	FALSE, 0, ఖాళీ, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_బఫరు_కొలత  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
నిర్మాణం prt_ps_font_S
{
    పూర్ణాంకం		wx;
    పూర్ణాంకం		uline_స్థానభ్రంశం;
    పూర్ణాంకం		uline_వెడల్పు;
    పూర్ణాంకం		bbox_min_y;
    పూర్ణాంకం		bbox_max_y;
    అక్షరం	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
స్థిర నిర్మాణం prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
స్థిర నిర్మాణం prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ}
};

// Pointer to current font set being used
స్థిర నిర్మాణం prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
నిర్మాణం prt_ps_encoding_S
{
    అక్షరం	*encoding;
    అక్షరం	*cmap_encoding;
    పూర్ణాంకం		needs_charset;
};

నిర్మాణం prt_ps_charset_S
{
    అక్షరం	*charset;
    అక్షరం	*cmap_charset;
    పూర్ణాంకం		has_charset;
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
స్థిర నిర్మాణం prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ఖాళీ,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
స్థిర నిర్మాణం prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ఖాళీ,       CS_JIS_X_1983},
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
స్థిర నిర్మాణం prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ఖాళీ,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ఖాళీ,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
స్థిర నిర్మాణం prt_ps_charset_S sc_charsets[] =
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
స్థిర నిర్మాణం prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ఖాళీ,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
స్థిర నిర్మాణం prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ఖాళీ,	CS_B5},
    {"CP950",	    ఖాళీ,	CS_B5},
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
స్థిర నిర్మాణం prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ఖాళీ,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
స్థిర నిర్మాణం prt_ps_charset_S k_charsets[] =
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
నిర్మాణం prt_ps_mbfont_S
{
    పూర్ణాంకం				అంకె_encodings;
    నిర్మాణం prt_ps_encoding_S	*encodings;
    పూర్ణాంకం				అంకె_charsets;
    నిర్మాణం prt_ps_charset_S	*charsets;
    అక్షరం			*ascii_enc;
    అక్షరం			*defcs;
};

స్థిర నిర్మాణం prt_ps_mbfont_S prt_ps_mbfonts[] =
{
    {
	ARRAY_నిడివి(j_encodings),
	j_encodings,
	ARRAY_నిడివి(j_charsets),
	j_charsets,
	"jis_roman",
	"JIS_X_1983"
    },
    {
	ARRAY_నిడివి(sc_encodings),
	sc_encodings,
	ARRAY_నిడివి(sc_charsets),
	sc_charsets,
	"gb_roman",
	"GB_2312-80"
    },
    {
	ARRAY_నిడివి(tc_encodings),
	tc_encodings,
	ARRAY_నిడివి(tc_charsets),
	tc_charsets,
	"cns_roman",
	"BIG5"
    },
    {
	ARRAY_నిడివి(k_encodings),
	k_encodings,
	ARRAY_నిడివి(k_charsets),
	k_charsets,
	"ks_roman",
	"KS_X_1992"
    }
};

నిర్మాణం prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    పూర్ణాంకం     type;
    char_u  title[256];
    char_u  సంస్కరణ_2[256];
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
# define PRT_PROLOG_సంస్కరణ  ((char_u *)"1.4")
# define PRT_CID_PROLOG_సంస్కరణ  ((char_u *)"1.0")

// String versions of PS resource types - indexed by constants above so don't
// re-order!
స్థిర అక్షరం *prt_resource_types[] =
{
    "procset",
    "encoding",
    "cmap"
};

// Strings to look for in a PS resource file
# define PRT_RESOURCE_శీర్షిక	    "%!PS-Adobe-"
# define PRT_RESOURCE_RESOURCE	    "Resource-"
# define PRT_RESOURCE_PROCSET	    "ProcSet"
# define PRT_RESOURCE_ENCODING	    "Encoding"
# define PRT_RESOURCE_CMAP	    "CMap"


// Data for table based DSC comment recognition, easy to extend if VIM needs to
// read more comments.
# define PRT_DSC_MISC_TYPE	    (-1)
# define PRT_DSC_TITLE_TYPE	    (1)
# define PRT_DSC_సంస్కరణ_TYPE	    (2)
# define PRT_DSC_ENDCOMMENTS_TYPE    (3)

# define PRT_DSC_TITLE		    "%%Title:"
# define PRT_DSC_సంస్కరణ		    "%%Version:"
# define PRT_DSC_ENDCOMMENTS	    "%%EndComments:"

నిర్మాణం prt_dsc_comment_S
{
    అక్షరం	*string;
    పూర్ణాంకం		len;
    పూర్ణాంకం		type;
};

నిర్మాణం prt_dsc_పంక్తి_S
{
    పూర్ణాంకం		type;
    char_u	*string;
    పూర్ణాంకం		len;
};


# define SIZEOF_CSTR(s)      (పరిమాణం(s) - 1)
స్థిర నిర్మాణం prt_dsc_comment_S prt_dsc_పట్టిక[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_సంస్కరణ,     SIZEOF_CSTR(PRT_DSC_సంస్కరణ),
							PRT_DSC_సంస్కరణ_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

స్థిర శూన్య prt_write_file_len(char_u *బఫరు, పూర్ణాంకం బైట్లు);
స్థిర పూర్ణాంకం prt_తదుపరి_dsc(నిర్మాణం prt_dsc_పంక్తి_S *p_dsc_పంక్తి);

/*
 * Variables for the output PostScript file.
 */
స్థిర FILE *prt_ps_fd;
స్థిర పూర్ణాంకం prt_file_error;
స్థిర char_u *prt_ps_file_name = ఖాళీ;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
స్థిర దశాంశం prt_పుట_వెడల్పు;
స్థిర దశాంశం prt_పుట_ఎత్తు;
స్థిర దశాంశం prt_ఎడమ_margin;
స్థిర దశాంశం prt_కుడి_margin;
స్థిర దశాంశం prt_శిఖరం_margin;
స్థిర దశాంశం prt_bottom_margin;
స్థిర దశాంశం prt_పంక్తి_ఎత్తు;
స్థిర దశాంశం prt_మొదటి_పంక్తి_ఎత్తు;
స్థిర దశాంశం prt_char_వెడల్పు;
స్థిర దశాంశం prt_సంఖ్య_వెడల్పు;
స్థిర దశాంశం prt_bgcol_స్థానభ్రంశం;
స్థిర దశాంశం prt_pos_x_moveto = 0.0;
స్థిర దశాంశం prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
స్థిర పూర్ణాంకం prt_need_moveto;
స్థిర పూర్ణాంకం prt_do_moveto;
స్థిర పూర్ణాంకం prt_need_font;
స్థిర పూర్ణాంకం prt_font;
స్థిర పూర్ణాంకం prt_need_underline;
స్థిర పూర్ణాంకం prt_underline;
స్థిర పూర్ణాంకం prt_do_underline;
స్థిర పూర్ణాంకం prt_need_fgcol;
స్థిర పూర్ణాంకం prt_fgcol;
స్థిర పూర్ణాంకం prt_need_bgcol;
స్థిర పూర్ణాంకం prt_do_bgcol;
స్థిర పూర్ణాంకం prt_bgcol;
స్థిర పూర్ణాంకం prt_new_bgcol;
స్థిర పూర్ణాంకం prt_attribute_change;
స్థిర దశాంశం prt_పాఠ్యం_నిరంతరం;
స్థిర పూర్ణాంకం prt_పుట_అంకె;
స్థిర పూర్ణాంకం prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
స్థిర పూర్ణాంకం prt_media;
స్థిర పూర్ణాంకం prt_portrait;
స్థిర పూర్ణాంకం prt_అంకె_copies;
స్థిర పూర్ణాంకం prt_duplex;
స్థిర పూర్ణాంకం prt_tumble;
స్థిర పూర్ణాంకం prt_collate;

/*
 * Buffers used when generating PostScript output
 */
స్థిర char_u prt_పంక్తి_బఫరు[257];
స్థిర garray_T prt_ps_బఫరు;

స్థిర పూర్ణాంకం prt_do_conv;
స్థిర vimconv_T prt_conv;

స్థిర పూర్ణాంకం prt_బహిర్గతం_mbyte;
స్థిర పూర్ణాంకం prt_custom_cmap;
స్థిర అక్షరం prt_cmap[80];
స్థిర పూర్ణాంకం prt_use_courier;
స్థిర పూర్ణాంకం prt_లోపల_ascii;
స్థిర పూర్ణాంకం prt_half_వెడల్పు;
స్థిర అక్షరం *prt_ascii_encoding;
స్థిర స్థిరరాశి char_u prt_hexchar[] = "0123456789abcdef";

    స్థిర శూన్య
prt_write_file_raw_len(char_u *బఫరు, పూర్ణాంకం బైట్లు)
{
    ఒకవేళ (!prt_file_error
	    && fwrite(బఫరు, పరిమాణం(char_u), బైట్లు, prt_ps_fd)
							     != (size_t)బైట్లు)
    {
	emsg(_(e_error_writing_గమ్యం_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    స్థిర శూన్య
prt_write_file(char_u *బఫరు)
{
    prt_write_file_len(బఫరు, (పూర్ణాంకం)STRLEN(బఫరు));
}

    స్థిర శూన్య
prt_write_file_len(char_u *బఫరు, పూర్ణాంకం బైట్లు)
{
    prt_write_file_raw_len(బఫరు, బైట్లు);
}

/*
 * Write a string.
 */
    స్థిర శూన్య
prt_write_string(అక్షరం *s)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు), "%s", s);
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write an int and a space.
 */
    స్థిర శూన్య
prt_write_int(పూర్ణాంకం i)
{
    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "%d ", i);
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write a boolean and a space.
 */
    స్థిర శూన్య
prt_write_boolean(పూర్ణాంకం b)
{
    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    స్థిర శూన్య
prt_def_font(
    అక్షరం	*new_name,
    అక్షరం	*encoding,
    పూర్ణాంకం		ఎత్తు,
    అక్షరం	*font)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_పంక్తి_బఫరు);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
	sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "/%s %d %f /_%s sffs\n",
		       new_name, ఎత్తు, 500./prt_ps_courier_font.wx, new_name);
    లేకపోతే
	vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
			     "/%s %d /_%s ffs\n", new_name, ఎత్తు, new_name);
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write a line to define the CID font.
 */
    స్థిర శూన్య
prt_def_cidfont(అక్షరం *new_name, పూర్ణాంకం ఎత్తు, అక్షరం *cidfont)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_పంక్తి_బఫరు);
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
			     "/%s %d /_%s ffs\n", new_name, ఎత్తు, new_name);
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    స్థిర శూన్య
prt_dup_cidfont(అక్షరం *original_name, అక్షరం *new_name)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    స్థిర శూన్య
prt_real_బిట్లు(
    ద్విగుణ      real,
    పూర్ణాంకం		precision,
    పూర్ణాంకం		*pinteger,
    పూర్ణాంకం		*pfraction)
{
    పూర్ణాంకం     i;
    పూర్ణాంకం     integer;
    దశాంశం   fraction;

    integer = (పూర్ణాంకం)real;
    fraction = (దశాంశం)(real - integer);
    ఒకవేళ (real < (ద్విగుణ)integer)
	fraction = -fraction;
    కొరకు (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (పూర్ణాంకం)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    స్థిర శూన్య
prt_write_real(ద్విగుణ val, పూర్ణాంకం prec)
{
    పూర్ణాంకం     integer;
    పూర్ణాంకం     fraction;

    prt_real_బిట్లు(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "%d", integer);
    prt_write_file(prt_పంక్తి_బఫరు);
    // Only emit fraction if necessary
    ఒకవేళ (fraction != 0)
    {
	// Remove any trailing zeros
	అయితే ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((అక్షరం *)prt_పంక్తి_బఫరు, ".%0*d", prec, fraction);
	prt_write_file(prt_పంక్తి_బఫరు);
    }
    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, " ");
    prt_write_file(prt_పంక్తి_బఫరు);
}

/*
 * Write a line to define a numeric variable.
 */
    స్థిర శూన్య
prt_def_var(అక్షరం *name, ద్విగుణ విలువ, పూర్ణాంకం prec)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
								"/%s ", name);
    prt_write_file(prt_పంక్తి_బఫరు);
    prt_write_real(విలువ, prec);
    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "d\n");
    prt_write_file(prt_పంక్తి_బఫరు);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_గమ్యం_USER(scale, కొలత)    ((కొలత) * ((scale)/1000.0))

    స్థిర శూన్య
prt_flush_బఫరు(శూన్య)
{
    ఒకవేళ (prt_ps_బఫరు.ga_len <= 0)
	తిరిగి;

    // Any background color must be drawn first
    ఒకవేళ (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	పూర్ణాంకం     r, g, b;

	ఒకవేళ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_పాఠ్యం_నిరంతరం, 2);
	prt_write_real(prt_పంక్తి_ఎత్తు, 2);

	// Lastly add the color of the background
	r = ((గుర్తులేని)prt_new_bgcol & 0xff0000) >> 16;
	g = ((గుర్తులేని)prt_new_bgcol & 0xff00) >> 8;
	b = prt_new_bgcol & 0xff;
	prt_write_real(r / 255.0, 3);
	prt_write_real(g / 255.0, 3);
	prt_write_real(b / 255.0, 3);
	prt_write_string("bg\n");
    }
    // Draw underlines before the text as it makes it slightly easier to
    // find the starting point.
    ఒకవేళ (prt_do_underline)
    {
	ఒకవేళ (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Underline length of text run
	prt_write_real(prt_పాఠ్యం_నిరంతరం, 2);
	prt_write_string("ul\n");
    }
    // Draw the text
    ఒకవేళ (prt_బహిర్గతం_mbyte)
	prt_write_string("<");
    లేకపోతే
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_బఫరు.ga_దత్తాంశం, prt_ps_బఫరు.ga_len);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
	prt_write_string(">");
    లేకపోతే
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    ఒకవేళ (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    లేకపోతే // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_బఫరు);
    ga_init2(&prt_ps_బఫరు, పరిమాణం(అక్షరం), prt_bufsiz);
}


    స్థిర శూన్య
prt_resource_name(char_u *filename, శూన్య *cookie)
{
    char_u *resource_filename = cookie;

    ఒకవేళ (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    లేకపోతే
	STRCPY(resource_filename, filename);
}

    స్థిర పూర్ణాంకం
prt_కనుగొను_resource(అక్షరం *name, నిర్మాణం prt_ps_resource_S *resource)
{
    char_u	*బఫరు;
    పూర్ణాంకం		retval;

    బఫరు = alloc(MAXPATHL + 1);
    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి FALSE;

    vim_strncpy(resource->name, (char_u *)name, 63);
    // Look for named resource file in runtimepath
    STRCPY(బఫరు, "print");
    కూడు_pathsep(బఫరు);
    vim_strcat(బఫరు, (char_u *)name, MAXPATHL);
    vim_strcat(బఫరు, (char_u *)".ps", MAXPATHL);
    resource->filename[0] = NUL;
    retval = (do_లోపల_runtimepath(బఫరు, 0, prt_resource_name,
							   resource->filename)
	    && resource->filename[0] != NUL);
    vim_free(బఫరు);
    తిరిగి retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_బఫరు_LEN (2048)
నిర్మాణం prt_resfile_బఫరు_S
{
    char_u  బఫరు[PRT_FILE_బఫరు_LEN];
    పూర్ణాంకం     len;
    పూర్ణాంకం     పంక్తి_ప్రారంభం;
    పూర్ణాంకం     పంక్తి_చివర;
};

స్థిర నిర్మాణం prt_resfile_బఫరు_S prt_resfile;

    స్థిర పూర్ణాంకం
prt_resfile_తదుపరి_పంక్తి(శూన్య)
{
    పూర్ణాంకం     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.పంక్తి_చివర + 1;
    అయితే (idx < prt_resfile.len)
    {
	ఒకవేళ (prt_resfile.బఫరు[idx] != PSLF && prt_resfile.బఫరు[idx] != PSCR)
	    ఆపు;
	idx++;
    }
    prt_resfile.పంక్తి_ప్రారంభం = idx;

    అయితే (idx < prt_resfile.len)
    {
	ఒకవేళ (prt_resfile.బఫరు[idx] == PSLF || prt_resfile.బఫరు[idx] == PSCR)
	    ఆపు;
	idx++;
    }
    prt_resfile.పంక్తి_చివర = idx;

    తిరిగి (idx < prt_resfile.len);
}

    స్థిర పూర్ణాంకం
prt_resfile_strncmp(పూర్ణాంకం స్థానభ్రంశం, అక్షరం *string, పూర్ణాంకం len)
{
    // Force not equal if string is longer than remainder of line
    ఒకవేళ (len > (prt_resfile.పంక్తి_చివర - (prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం)))
	తిరిగి 1;

    తిరిగి STRNCMP(&prt_resfile.బఫరు[prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం],
								string, len);
}

    స్థిర పూర్ణాంకం
prt_resfile_skip_nonws(పూర్ణాంకం స్థానభ్రంశం)
{
    పూర్ణాంకం     idx;

    idx = prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం;
    అయితే (idx < prt_resfile.పంక్తి_చివర)
    {
	ఒకవేళ (సురక్షిత_isspace(prt_resfile.బఫరు[idx]))
	    తిరిగి idx - prt_resfile.పంక్తి_ప్రారంభం;
	idx++;
    }
    తిరిగి -1;
}

    స్థిర పూర్ణాంకం
prt_resfile_skip_ws(పూర్ణాంకం స్థానభ్రంశం)
{
    పూర్ణాంకం     idx;

    idx = prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం;
    అయితే (idx < prt_resfile.పంక్తి_చివర)
    {
	ఒకవేళ (!సురక్షిత_isspace(prt_resfile.బఫరు[idx]))
	    తిరిగి idx - prt_resfile.పంక్తి_ప్రారంభం;
	idx++;
    }
    తిరిగి -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    స్థిర పూర్ణాంకం
prt_తదుపరి_dsc(నిర్మాణం prt_dsc_పంక్తి_S *p_dsc_పంక్తి)
{
    పూర్ణాంకం     comment;
    పూర్ణాంకం     స్థానభ్రంశం;

    // Move to start of next line
    ఒకవేళ (!prt_resfile_తదుపరి_పంక్తి())
	తిరిగి FALSE;

    // DSC comments always start %%
    ఒకవేళ (prt_resfile_strncmp(0, "%%", 2) != 0)
	తిరిగి FALSE;

    // Find type of DSC comment
    కొరకు (comment = 0; comment < (పూర్ణాంకం)ARRAY_నిడివి(prt_dsc_పట్టిక); comment++)
	ఒకవేళ (prt_resfile_strncmp(0, prt_dsc_పట్టిక[comment].string,
					    prt_dsc_పట్టిక[comment].len) == 0)
	    ఆపు;

    ఒకవేళ (comment != ARRAY_నిడివి(prt_dsc_పట్టిక))
    {
	// Return type of comment
	p_dsc_పంక్తి->type = prt_dsc_పట్టిక[comment].type;
	స్థానభ్రంశం = prt_dsc_పట్టిక[comment].len;
    }
    లేకపోతే
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_పంక్తి->type = PRT_DSC_MISC_TYPE;
	స్థానభ్రంశం = prt_resfile_skip_nonws(0);
	ఒకవేళ (స్థానభ్రంశం == -1)
	    తిరిగి FALSE;
    }

    // Skip ws to comment value
    స్థానభ్రంశం = prt_resfile_skip_ws(స్థానభ్రంశం);
    ఒకవేళ (స్థానభ్రంశం == -1)
	తిరిగి FALSE;

    p_dsc_పంక్తి->string = &prt_resfile.బఫరు[prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం];
    p_dsc_పంక్తి->len = prt_resfile.పంక్తి_చివర - (prt_resfile.పంక్తి_ప్రారంభం + స్థానభ్రంశం);

    తిరిగి TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    స్థిర పూర్ణాంకం
prt_open_resource(నిర్మాణం prt_ps_resource_S *resource)
{
    పూర్ణాంకం		స్థానభ్రంశం;
    పూర్ణాంకం		చూసిన_all;
    పూర్ణాంకం		చూసిన_title;
    పూర్ణాంకం		చూసిన_సంస్కరణ;
    FILE	*fd_resource;
    నిర్మాణం prt_dsc_పంక్తి_S dsc_పంక్తి;

    fd_resource = mch_fopen((అక్షరం *)resource->filename, READBIN);
    ఒకవేళ (fd_resource == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	తిరిగి FALSE;
    }
    CLEAR_క్షేత్రం(prt_resfile.బఫరు);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (పూర్ణాంకం)fread((అక్షరం *)prt_resfile.బఫరు, పరిమాణం(char_u),
					    PRT_FILE_బఫరు_LEN, fd_resource);
    ఒకవేళ (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	తిరిగి FALSE;
    }
    fclose(fd_resource);

    prt_resfile.పంక్తి_చివర = -1;
    prt_resfile.పంక్తి_ప్రారంభం = 0;
    ఒకవేళ (!prt_resfile_తదుపరి_పంక్తి())
	తిరిగి FALSE;

    స్థానభ్రంశం = 0;

    ఒకవేళ (prt_resfile_strncmp(స్థానభ్రంశం, PRT_RESOURCE_శీర్షిక,
			       (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_శీర్షిక)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	తిరిగి FALSE;
    }

    // Skip over any version numbers and following ws
    స్థానభ్రంశం += (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_శీర్షిక);
    స్థానభ్రంశం = prt_resfile_skip_nonws(స్థానభ్రంశం);
    ఒకవేళ (స్థానభ్రంశం == -1)
	తిరిగి FALSE;
    స్థానభ్రంశం = prt_resfile_skip_ws(స్థానభ్రంశం);
    ఒకవేళ (స్థానభ్రంశం == -1)
	తిరిగి FALSE;

    ఒకవేళ (prt_resfile_strncmp(స్థానభ్రంశం, PRT_RESOURCE_RESOURCE,
			     (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	తిరిగి FALSE;
    }
    స్థానభ్రంశం += (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    ఒకవేళ (prt_resfile_strncmp(స్థానభ్రంశం, PRT_RESOURCE_PROCSET,
			      (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    లేకపోతే ఒకవేళ (prt_resfile_strncmp(స్థానభ్రంశం, PRT_RESOURCE_ENCODING,
			     (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    లేకపోతే ఒకవేళ (prt_resfile_strncmp(స్థానభ్రంశం, PRT_RESOURCE_CMAP,
				 (పూర్ణాంకం)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    లేకపోతే
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	తిరిగి FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->సంస్కరణ_2[0] = '\0';
    చూసిన_title = FALSE;
    చూసిన_సంస్కరణ = FALSE;
    చూసిన_all = FALSE;
    అయితే (!చూసిన_all && prt_తదుపరి_dsc(&dsc_పంక్తి))
    {
	ఎంపిక (dsc_పంక్తి.type)
	{
	స్థితి PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_పంక్తి.string, dsc_పంక్తి.len);
	    చూసిన_title = TRUE;
	    ఒకవేళ (చూసిన_సంస్కరణ)
		చూసిన_all = TRUE;
	    ఆపు;

	స్థితి PRT_DSC_సంస్కరణ_TYPE:
	    vim_strncpy(resource->సంస్కరణ_2, dsc_పంక్తి.string, dsc_పంక్తి.len);
	    చూసిన_సంస్కరణ = TRUE;
	    ఒకవేళ (చూసిన_title)
		చూసిన_all = TRUE;
	    ఆపు;

	స్థితి PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    చూసిన_all = TRUE;
	    ఆపు;

	స్థితి PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    ఆపు;
	}
    }

    ఒకవేళ (!చూసిన_title || !చూసిన_సంస్కరణ)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

    స్థిర పూర్ణాంకం
prt_check_resource(నిర్మాణం prt_ps_resource_S *resource, char_u *సంస్కరణ_2)
{
    // Version number m.n should match, the revision number does not matter
    ఒకవేళ (STRNCMP(resource->సంస్కరణ_2, సంస్కరణ_2, STRLEN(సంస్కరణ_2)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_సంస్కరణ),
		resource->name);
	తిరిగి FALSE;
    }

    // Other checks to be added as needed
    తిరిగి TRUE;
}

    స్థిర శూన్య
prt_dsc_ప్రారంభం(శూన్య)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    స్థిర శూన్య
prt_dsc_noarg(అక్షరం *comment)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
							 "%%%%%s\n", comment);
    prt_write_file(prt_పంక్తి_బఫరు);
}

    స్థిర శూన్య
prt_dsc_textline(అక్షరం *comment, అక్షరం *పాఠ్యం)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
					       "%%%%%s: %s\n", comment, పాఠ్యం);
    prt_write_file(prt_పంక్తి_బఫరు);
}

    స్థిర శూన్య
prt_dsc_పాఠ్యం(అక్షరం *comment, అక్షరం *పాఠ్యం)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
					     "%%%%%s: (%s)\n", comment, పాఠ్యం);
    prt_write_file(prt_పంక్తి_బఫరు);
}

# define prt_dsc_atend(c)	prt_dsc_పాఠ్యం((c), "atend")

    స్థిర శూన్య
prt_dsc_ints(అక్షరం *comment, పూర్ణాంకం లెక్క, పూర్ణాంకం *ints)
{
    పూర్ణాంకం		i;

    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
							  "%%%%%s:", comment);
    prt_write_file(prt_పంక్తి_బఫరు);

    కొరకు (i = 0; i < లెక్క; i++)
    {
	sprintf((అక్షరం *)prt_పంక్తి_బఫరు, " %d", ints[i]);
	prt_write_file(prt_పంక్తి_బఫరు);
    }

    prt_write_string("\n");
}

    స్థిర శూన్య
prt_dsc_resources(
    అక్షరం	*comment,	// if NULL add to previous
    అక్షరం	*type,
    అక్షరం	*string)
{
    ఒకవేళ (comment != ఖాళీ)
	vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
						 "%%%%%s: %s", comment, type);
    లేకపోతే
	vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
							    "%%%%+ %s", type);
    prt_write_file(prt_పంక్తి_బఫరు);

    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
							     " %s\n", string);
    prt_write_file(prt_పంక్తి_బఫరు);
}

    స్థిర శూన్య
prt_dsc_font_resource(అక్షరం *resource, నిర్మాణం prt_ps_font_S *ps_font)
{
    పూర్ణాంకం     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    కొరకు (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	ఒకవేళ (ps_font->ps_fontname[i] != ఖాళీ)
	    prt_dsc_resources(ఖాళీ, "font", ps_font->ps_fontname[i]);
}

    స్థిర శూన్య
prt_dsc_requirements(
    పూర్ణాంకం		duplex,
    పూర్ణాంకం		tumble,
    పూర్ణాంకం		collate,
    పూర్ణాంకం		color,
    పూర్ణాంకం		అంకె_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    ఒకవేళ (!(duplex || collate || color || (అంకె_copies > 1)))
	తిరిగి;

    sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "%%%%Requirements:");
    prt_write_file(prt_పంక్తి_బఫరు);

    ఒకవేళ (duplex)
    {
	prt_write_string(" duplex");
	ఒకవేళ (tumble)
	    prt_write_string("(tumble)");
    }
    ఒకవేళ (collate)
	prt_write_string(" collate");
    ఒకవేళ (color)
	prt_write_string(" color");
    ఒకవేళ (అంకె_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((అక్షరం *)prt_పంక్తి_బఫరు, "%d", అంకె_copies);
	prt_write_file(prt_పంక్తి_బఫరు);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    స్థిర శూన్య
prt_dsc_docmedia(
    అక్షరం	*paper_name,
    ద్విగుణ	వెడల్పు,
    ద్విగుణ	ఎత్తు,
    ద్విగుణ	బరువు,
    అక్షరం	*రంగు,
    అక్షరం	*type)
{
    vim_snprintf((అక్షరం *)prt_పంక్తి_బఫరు, పరిమాణం(prt_పంక్తి_బఫరు),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_పంక్తి_బఫరు);
    prt_write_real(వెడల్పు, 2);
    prt_write_real(ఎత్తు, 2);
    prt_write_real(బరువు, 2);
    ఒకవేళ (రంగు == ఖాళీ)
	prt_write_string("()");
    లేకపోతే
	prt_write_string(రంగు);
    prt_write_string(" ");
    ఒకవేళ (type == ఖాళీ)
	prt_write_string("()");
    లేకపోతే
	prt_write_string(type);
    prt_write_string("\n");
}

    శూన్య
mch_print_cleanup(శూన్య)
{
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	పూర్ణాంకం     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	కొరకు (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    ఒకవేళ (prt_ps_mb_font.ps_fontname[i] != ఖాళీ)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    ఒకవేళ (prt_do_conv)
    {
	convert_అమరిక(&prt_conv, ఖాళీ, ఖాళీ);
	prt_do_conv = FALSE;
    }
    ఒకవేళ (prt_ps_fd != ఖాళీ)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ఖాళీ;
	prt_file_error = FALSE;
    }
    ఒకవేళ (prt_ps_file_name != ఖాళీ)
	VIM_CLEAR(prt_ps_file_name);
}

    స్థిర దశాంశం
గమ్యం_device_units(పూర్ణాంకం idx, ద్విగుణ physsize, పూర్ణాంకం def_సంఖ్య)
{
    దశాంశం	ret;
    పూర్ణాంకం		u;
    పూర్ణాంకం		nr;

    u = prt_get_unit(idx);
    ఒకవేళ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_సంఖ్య;
    }
    లేకపోతే
	nr = printer_opts[idx].సంఖ్య;

    ఎంపిక (u)
    {
	స్థితి PRT_UNIT_INCH:
	    ret = (దశాంశం)(nr * PRT_PS_DEFAULT_DPI);
	    ఆపు;
	స్థితి PRT_UNIT_MM:
	    ret = (దశాంశం)(nr * PRT_PS_DEFAULT_DPI) / (దశాంశం)25.4;
	    ఆపు;
	స్థితి PRT_UNIT_బిందువు:
	    ret = (దశాంశం)nr;
	    ఆపు;
	స్థితి PRT_UNIT_PERC:
	అప్రమేయ:
	    ret = (దశాంశం)(physsize * nr) / 100;
	    ఆపు;
    }

    తిరిగి ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    స్థిర శూన్య
prt_పుట_margins(
    ద్విగుణ	వెడల్పు,
    ద్విగుణ	ఎత్తు,
    ద్విగుణ	*ఎడమ_2,
    ద్విగుణ	*కుడి_2,
    ద్విగుణ	*శిఖరం,
    ద్విగుణ	*bottom)
{
    *ఎడమ_2   = గమ్యం_device_units(OPT_PRINT_ఎడమ, వెడల్పు, 10);
    *కుడి_2  = వెడల్పు - గమ్యం_device_units(OPT_PRINT_కుడి, వెడల్పు, 5);
    *శిఖరం    = ఎత్తు - గమ్యం_device_units(OPT_PRINT_శిఖరం, ఎత్తు, 5);
    *bottom = గమ్యం_device_units(OPT_PRINT_BOT, ఎత్తు, 5);
}

    స్థిర శూన్య
prt_font_metrics(పూర్ణాంకం font_scale)
{
    prt_పంక్తి_ఎత్తు = (దశాంశం)font_scale;
    prt_char_వెడల్పు = (దశాంశం)PRT_PS_FONT_గమ్యం_USER(font_scale, prt_ps_font->wx);
}


    స్థిర పూర్ణాంకం
prt_get_cpl(శూన్య)
{
    ఒకవేళ (prt_use_సంఖ్య())
    {
	prt_సంఖ్య_వెడల్పు = PRINT_సంఖ్య_వెడల్పు * prt_char_వెడల్పు;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	ఒకవేళ (prt_బహిర్గతం_mbyte)
	    prt_సంఖ్య_వెడల్పు /= 2;
	prt_ఎడమ_margin += prt_సంఖ్య_వెడల్పు;
    }
    లేకపోతే
	prt_సంఖ్య_వెడల్పు = 0.0;

    తిరిగి (పూర్ణాంకం)((prt_కుడి_margin - prt_ఎడమ_margin) / prt_char_వెడల్పు);
}

    స్థిర పూర్ణాంకం
prt_build_cid_fontname(పూర్ణాంకం font, char_u *name, పూర్ణాంకం name_len)
{
    అక్షరం    *fontname;

    fontname = alloc(name_len + 1);
    ఒకవేళ (fontname == ఖాళీ)
	తిరిగి FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    తిరిగి TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    స్థిర పూర్ణాంకం
prt_get_lpp(శూన్య)
{
    పూర్ణాంకం lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_స్థానభ్రంశం = (దశాంశం)PRT_PS_FONT_గమ్యం_USER(prt_పంక్తి_ఎత్తు,
					   prt_ps_font->bbox_min_y);
    ఒకవేళ ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_స్థానభ్రంశం -= (దశాంశం)PRT_PS_FONT_గమ్యం_USER(prt_పంక్తి_ఎత్తు,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_మొదటి_పంక్తి_ఎత్తు = prt_పంక్తి_ఎత్తు + prt_bgcol_స్థానభ్రంశం;

    // Calculate lpp
    lpp = (పూర్ణాంకం)((prt_శిఖరం_margin - prt_bottom_margin) / prt_పంక్తి_ఎత్తు);

    // Adjust top margin if there is a header
    prt_శిఖరం_margin -= prt_పంక్తి_ఎత్తు * prt_శీర్షిక_ఎత్తు();

    తిరిగి lpp - prt_శీర్షిక_ఎత్తు();
}

    స్థిర పూర్ణాంకం
prt_సరిపోలిక_encoding(
    అక్షరం			*p_encoding,
    నిర్మాణం prt_ps_mbfont_S	*p_cmap,
    నిర్మాణం prt_ps_encoding_S	**pp_mbenc)
{
    పూర్ణాంకం				mbenc;
    పూర్ణాంకం				enc_len;
    నిర్మాణం prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ఖాళీ;
    // Look for recognised encoding
    enc_len = (పూర్ణాంకం)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    కొరకు (mbenc = 0; mbenc < p_cmap->అంకె_encodings; mbenc++)
    {
	ఒకవేళ (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    తిరిగి TRUE;
	}
	p_mbenc++;
    }
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
prt_సరిపోలిక_charset(
    అక్షరం		    *p_charset,
    నిర్మాణం prt_ps_mbfont_S  *p_cmap,
    నిర్మాణం prt_ps_charset_S **pp_mbchar)
{
    పూర్ణాంకం			    mbchar;
    పూర్ణాంకం			    char_len;
    నిర్మాణం prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    ఒకవేళ (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (పూర్ణాంకం)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    కొరకు (mbchar = 0; mbchar < p_cmap->అంకె_charsets; mbchar++)
    {
	ఒకవేళ (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    తిరిగి TRUE;
	}
	p_mbchar++;
    }
    తిరిగి FALSE;
}

    పూర్ణాంకం
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    పూర్ణాంకం		forceit వాడని)
{
    పూర్ణాంకం		i;
    అక్షరం	*paper_name;
    పూర్ణాంకం		paper_strlen;
    పూర్ణాంకం		fontsize;
    char_u	*p;
    ద్విగుణ      ఎడమ_2;
    ద్విగుణ      కుడి_2;
    ద్విగుణ      శిఖరం;
    ద్విగుణ      bottom;
    పూర్ణాంకం		props;
    పూర్ణాంకం		cmap = 0;
    char_u	*p_encoding;
    నిర్మాణం prt_ps_encoding_S *p_mbenc;
    నిర్మాణం prt_ps_encoding_S *p_mbenc_మొదటి;
    నిర్మాణం prt_ps_charset_S  *p_mbchar = ఖాళీ;

# ఒకవేళ 0
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
    ఒకవేళ (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    ఒకవేళ (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ఖాళీ;
    props = enc_canon_props(p_encoding);
    ఒకవేళ (!(props & ENC_8_బిట్టు) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	పూర్ణాంకం cmap_మొదటి = 0;

	p_mbenc_మొదటి = ఖాళీ;
	కొరకు (cmap = 0; cmap < (పూర్ణాంకం)ARRAY_నిడివి(prt_ps_mbfonts); cmap++)
	    ఒకవేళ (prt_సరిపోలిక_encoding((అక్షరం *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		ఒకవేళ (p_mbenc_మొదటి == ఖాళీ)
		{
		    p_mbenc_మొదటి = p_mbenc;
		    cmap_మొదటి = cmap;
		}
		ఒకవేళ (prt_సరిపోలిక_charset((అక్షరం *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    ఆపు;
	    }

	// Use first encoding matched if no charset matched
	ఒకవేళ (p_mbchar == ఖాళీ && p_mbenc_మొదటి != ఖాళీ)
	{
	    p_mbenc = p_mbenc_మొదటి;
	    cmap = cmap_మొదటి;
	}
    }

    prt_బహిర్గతం_mbyte = (p_mbenc != ఖాళీ);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ఖాళీ);
	ఒకవేళ (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    ఒకవేళ ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_సమితి));
		తిరిగి FALSE;
	    }

	    // Add charset name if not empty
	    ఒకవేళ (p_mbchar->cmap_charset != ఖాళీ)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, పరిమాణం(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	లేకపోతే
	{
	    // Add custom CMap character set name
	    ఒకవేళ (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_ఖాళీలు_with_multi_byte_encoding));
		తిరిగి FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, పరిమాణం(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	ఒకవేళ (p_mbenc->cmap_encoding != ఖాళీ && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < పరిమాణం(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	ఒకవేళ (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    తిరిగి FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	ఒకవేళ (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].అక్షరశ్రేణి_పొడవు))
	    తిరిగి FALSE;
	ఒకవేళ (mbfont_opts[OPT_MBFONT_BOLD].present)
	    ఒకవేళ (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].అక్షరశ్రేణి_పొడవు))
		తిరిగి FALSE;
	ఒకవేళ (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    ఒకవేళ (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].అక్షరశ్రేణి_పొడవు))
		తిరిగి FALSE;
	ఒకవేళ (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    ఒకవేళ (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].అక్షరశ్రేణి_పొడవు))
		తిరిగి FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	ఒకవేళ (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    ఒకవేళ (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    లేకపోతే
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    లేకపోతే
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    ఒకవేళ (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (అక్షరం *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].అక్షరశ్రేణి_పొడవు;
    }
    లేకపోతే
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    కొరకు (i = 0; i < (పూర్ణాంకం)PRT_MEDIASIZE_LEN; ++i)
	ఒకవేళ (STRLEN(prt_mediasize[i].name) == (గుర్తులేని)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    ఆపు;
    ఒకవేళ (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    ఒకవేళ (prt_portrait)
    {
	prt_పుట_వెడల్పు = prt_mediasize[i].వెడల్పు;
	prt_పుట_ఎత్తు = prt_mediasize[i].ఎత్తు;
    }
    లేకపోతే
    {
	prt_పుట_వెడల్పు = prt_mediasize[i].ఎత్తు;
	prt_పుట_ఎత్తు = prt_mediasize[i].వెడల్పు;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_పుట_margins(prt_పుట_వెడల్పు, prt_పుట_ఎత్తు, &ఎడమ_2, &కుడి_2, &శిఖరం,
								    &bottom);
    prt_ఎడమ_margin = (దశాంశం)ఎడమ_2;
    prt_కుడి_margin = (దశాంశం)కుడి_2;
    prt_శిఖరం_margin = (దశాంశం)శిఖరం;
    prt_bottom_margin = (దశాంశం)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    కొరకు (p = p_pfn; (p = vim_strchr(p, ':')) != ఖాళీ; ++p)
	ఒకవేళ (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((అక్షరం *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_చొప్పున_పంక్తి = prt_get_cpl();
    psettings->పంక్తులు_చొప్పున_పుట = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    ఒకవేళ (psettings->chars_చొప్పున_పంక్తి <= 0 || psettings->పంక్తులు_చొప్పున_పుట <= 0)
	తిరిగి FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_అంకె_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    ఒకవేళ (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    లేకపోతే
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_అంకె_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    ఒకవేళ (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	ఒకవేళ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	లేకపోతే ఒకవేళ (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    ఒకవేళ (psettings->outfile == ఖాళీ)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	ఒకవేళ (prt_ps_file_name == ఖాళీ)
	{
	    emsg(_(e_cant_get_తాత్కాలిక_file_name));
	    తిరిగి FAIL;
	}
	prt_ps_fd = mch_fopen((అక్షరం *)prt_ps_file_name, WRITEBIN);
    }
    లేకపోతే
    {
	p = expand_env_save(psettings->outfile);
	ఒకవేళ (p != ఖాళీ)
	{
	    prt_ps_fd = mch_fopen((అక్షరం *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    ఒకవేళ (prt_ps_fd == ఖాళీ)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	తిరిగి FAIL;
    }

    prt_bufsiz = psettings->chars_చొప్పున_పంక్తి;
    ఒకవేళ (prt_బహిర్గతం_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_బఫరు, పరిమాణం(అక్షరం), prt_bufsiz);

    prt_పుట_అంకె = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
prt_కూడు_resource(నిర్మాణం prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_బఫరు[512];
    size_t	బైట్లు_read;

    fd_resource = mch_fopen((అక్షరం *)resource->filename, READBIN);
    ఒకవేళ (fd_resource == ఖాళీ)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	తిరిగి FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (అక్షరం *)resource->title);

    prt_dsc_textline("BeginDocument", (అక్షరం *)resource->filename);

    కొరకు (;;)
    {
	బైట్లు_read = fread((అక్షరం *)resource_బఫరు, పరిమాణం(char_u),
			   పరిమాణం(resource_బఫరు), fd_resource);
	ఒకవేళ (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    తిరిగి FALSE;
	}
	ఒకవేళ (బైట్లు_read == 0)
	    ఆపు;
	prt_write_file_raw_len(resource_బఫరు, (పూర్ణాంకం)బైట్లు_read);
	ఒకవేళ (prt_file_error)
	{
	    fclose(fd_resource);
	    తిరిగి FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    తిరిగి TRUE;
}

    పూర్ణాంకం
mch_print_begin(prt_settings_T *psettings)
{
    పూర్ణాంకం		bbox[4];
    ద్విగుణ      ఎడమ_2;
    ద్విగుణ      కుడి_2;
    ద్విగుణ      శిఖరం;
    ద్విగుణ      bottom;
    నిర్మాణం prt_ps_resource_S *res_prolog;
    నిర్మాణం prt_ps_resource_S *res_encoding;
    అక్షరం	బఫరు[256];
    char_u      *p_encoding;
    నిర్మాణం prt_ps_resource_S *res_cidfont;
    నిర్మాణం prt_ps_resource_S *res_cmap;
    పూర్ణాంకం		retval = FALSE;

    res_prolog = ALLOC_ONE(నిర్మాణం prt_ps_resource_S);
    res_encoding = ALLOC_ONE(నిర్మాణం prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(నిర్మాణం prt_ps_resource_S);
    res_cmap = ALLOC_ONE(నిర్మాణం prt_ps_resource_S);
    ఒకవేళ (res_prolog == ఖాళీ || res_encoding == ఖాళీ
	    || res_cidfont == ఖాళీ || res_cmap == ఖాళీ)
	వెళ్ళు theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_ప్రారంభం();
    prt_dsc_textline("Title", (అక్షరం *)psettings->jobname);
    ఒకవేళ (!get_user_name((char_u *)బఫరు, 256))
	STRCPY(బఫరు, "Unknown");
    prt_dsc_textline("For", బఫరు);
    prt_dsc_textline("Creator", VIM_సంస్కరణ_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(సమయం(ఖాళీ), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_పుట_margins(prt_mediasize[prt_media].వెడల్పు,
					    prt_mediasize[prt_media].ఎత్తు,
					    &ఎడమ_2, &కుడి_2, &శిఖరం, &bottom);
    bbox[0] = (పూర్ణాంకం)ఎడమ_2;
    ఒకవేళ (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (పూర్ణాంకం)(శిఖరం - (psettings->పంక్తులు_చొప్పున_పుట + prt_శీర్షిక_ఎత్తు())
						    * (ద్విగుణ)prt_పంక్తి_ఎత్తు);
	bbox[2] = (పూర్ణాంకం)(ఎడమ_2 + psettings->chars_చొప్పున_పంక్తి
					       * (ద్విగుణ)prt_char_వెడల్పు + 0.5);
	bbox[3] = (పూర్ణాంకం)(శిఖరం + 0.5);
    }
    లేకపోతే
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (పూర్ణాంకం)bottom;
	bbox[2] = (పూర్ణాంకం)(ఎడమ_2 + ((psettings->పంక్తులు_చొప్పున_పుట
			      + prt_శీర్షిక_ఎత్తు()) * prt_పంక్తి_ఎత్తు) + 0.5);
	bbox[3] = (పూర్ణాంకం)(bottom + psettings->chars_చొప్పున_పంక్తి
					       * (ద్విగుణ)prt_char_వెడల్పు + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].వెడల్పు,
				prt_mediasize[prt_media].ఎత్తు,
				(ద్విగుణ)0, ఖాళీ, ఖాళీ);
    // Define fonts needed
    ఒకవేళ (!prt_బహిర్గతం_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ఖాళీ
				: "DocumentNeededResources"), &prt_ps_mb_font);
	ఒకవేళ (!prt_custom_cmap)
	    prt_dsc_resources(ఖాళీ, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    ఒకవేళ (!prt_కనుగొను_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_కనుగొను_postscript_resource_file_str_ps), "prolog");
	వెళ్ళు theend;
    }
    ఒకవేళ (!prt_open_resource(res_prolog))
	వెళ్ళు theend;
    ఒకవేళ (!prt_check_resource(res_prolog, PRT_PROLOG_సంస్కరణ))
	వెళ్ళు theend;
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	// Look for required version of multi-byte printing procset
	ఒకవేళ (!prt_కనుగొను_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_కనుగొను_postscript_resource_file_str_ps), "cidfont");
	    వెళ్ళు theend;
	}
	ఒకవేళ (!prt_open_resource(res_cidfont))
	    వెళ్ళు theend;
	ఒకవేళ (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_సంస్కరణ))
	    వెళ్ళు theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    ఒకవేళ (!prt_బహిర్గతం_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	ఒకవేళ (*p_encoding == NUL
		|| !prt_కనుగొను_resource((అక్షరం *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    పూర్ణాంకం		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    ఒకవేళ (!(props & ENC_8_బిట్టు)
		    || !prt_కనుగొను_resource((అక్షరం *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		ఒకవేళ (!prt_కనుగొను_resource((అక్షరం *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_కనుగొను_postscript_resource_file_str_ps),
			    p_encoding);
		    వెళ్ళు theend;
		}
	    }
	}
	ఒకవేళ (!prt_open_resource(res_encoding))
	    వెళ్ళు theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    లేకపోతే
    {
	p_encoding = enc_skip(p_penc);
	ఒకవేళ (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	ఒకవేళ (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    ఒకవేళ (!prt_కనుగొను_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_కనుగొను_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		వెళ్ళు theend;
	    }
	    ఒకవేళ (!prt_open_resource(res_encoding))
		వెళ్ళు theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    ఒకవేళ (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8_బిట్టు))
    {
	// Set up encoding conversion if required
	ఒకవేళ (FAIL == convert_అమరిక(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_గమ్యం_convert_గమ్యం_print_encoding_str), p_encoding);
	    వెళ్ళు theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    ఒకవేళ (prt_బహిర్గతం_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	ఒకవేళ (!prt_కనుగొను_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_కనుగొను_postscript_resource_file_str_ps), prt_cmap);
	    వెళ్ళు theend;
	}
	ఒకవేళ (!prt_open_resource(res_cmap))
	    వెళ్ళు theend;
    }

    // List resources supplied
    STRCPY(బఫరు, res_prolog->title);
    STRCAT(బఫరు, " ");
    STRCAT(బఫరు, res_prolog->సంస్కరణ_2);
    prt_dsc_resources("DocumentSuppliedResources", "procset", బఫరు);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	STRCPY(బఫరు, res_cidfont->title);
	STRCAT(బఫరు, " ");
	STRCAT(బఫరు, res_cidfont->సంస్కరణ_2);
	prt_dsc_resources(ఖాళీ, "procset", బఫరు);

	ఒకవేళ (prt_custom_cmap)
	{
	    STRCPY(బఫరు, res_cmap->title);
	    STRCAT(బఫరు, " ");
	    STRCAT(బఫరు, res_cmap->సంస్కరణ_2);
	    prt_dsc_resources(ఖాళీ, "cmap", బఫరు);
	}
    }
    ఒకవేళ (!prt_బహిర్గతం_mbyte || prt_use_courier)
    {
	STRCPY(బఫరు, res_encoding->title);
	STRCAT(బఫరు, " ");
	STRCAT(బఫరు, res_encoding->సంస్కరణ_2);
	prt_dsc_resources(ఖాళీ, "encoding", బఫరు);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# లేకపోతే
					0
# endif
					, prt_అంకె_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    ఒకవేళ (!prt_బహిర్గతం_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ఖాళీ : "PageResources"),
							    &prt_ps_mb_font);
	ఒకవేళ (!prt_custom_cmap)
	    prt_dsc_resources(ఖాళీ, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    ఒకవేళ (!prt_కూడు_resource(res_prolog))
	వెళ్ళు theend;
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	ఒకవేళ (!prt_కూడు_resource(res_cidfont))
	    వెళ్ళు theend;
	ఒకవేళ (prt_custom_cmap && !prt_కూడు_resource(res_cmap))
	    వెళ్ళు theend;
    }

    ఒకవేళ (!prt_బహిర్గతం_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	ఒకవేళ (!prt_కూడు_resource(res_encoding))
	    వెళ్ళు theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((పూర్ణాంకం)prt_mediasize[prt_media].వెడల్పు);
    prt_write_int((పూర్ణాంకం)prt_mediasize[prt_media].ఎత్తు);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_అంకె_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    ఒకవేళ (!prt_బహిర్గతం_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	ఒకవేళ (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (అక్షరం *)p_encoding, (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (అక్షరం *)p_encoding, (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (అక్షరం *)p_encoding, (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (అక్షరం *)p_encoding, (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	ఒకవేళ (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	ఒకవేళ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ఖాళీ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    ఒకవేళ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	లేకపోతే
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	ఒకవేళ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ఖాళీ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    ఒకవేళ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	లేకపోతే
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	ఒకవేళ (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ఖాళీ)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    ఒకవేళ (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (పూర్ణాంకం)prt_పంక్తి_ఎత్తు,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	లేకపోతే
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_గమ్యం_USER(prt_పంక్తి_ఎత్తు,
						prt_ps_font->uline_స్థానభ్రంశం), 2);
    prt_def_var("UW", PRT_PS_FONT_గమ్యం_USER(prt_పంక్తి_ఎత్తు,
						 prt_ps_font->uline_వెడల్పు), 2);
    prt_def_var("BO", prt_bgcol_స్థానభ్రంశం, 2);

    prt_dsc_noarg("EndSetup");

    // Fail if any problems writing out to the PS file
    retval = !prt_file_error;

theend:
    vim_free(res_prolog);
    vim_free(res_encoding);
    vim_free(res_cidfont);
    vim_free(res_cmap);

    తిరిగి retval;
}

    శూన్య
mch_print_చివర(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_పుట_అంకె);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    ఒకవేళ (!prt_file_error && psettings->outfile == ఖాళీ
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	ఒకవేళ (prt_ps_fd != ఖాళీ)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ఖాళీ;
	}
	prt_సందేశం((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	ఒకవేళ (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_గమ్యం_print_postscript_file));
	లేకపోతే
	    prt_సందేశం((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    పూర్ణాంకం
mch_print_చివర_పుట(శూన్య)
{
    prt_flush_బఫరు();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    తిరిగి !prt_file_error;
}

    పూర్ణాంకం
mch_print_begin_పుట(char_u *str వాడని)
{
    పూర్ణాంకం		పుట_అంకె[2];

    prt_పుట_అంకె++;

    పుట_అంకె[0] = పుట_అంకె[1] = prt_పుట_అంకె;
    prt_dsc_ints("Page", 2, పుట_అంకె);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_లోపల_ascii = !prt_బహిర్గతం_mbyte;
    ఒకవేళ (prt_బహిర్గతం_mbyte)
	prt_write_string("CF0 sf\n");
    లేకపోతే
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    ఒకవేళ (!prt_portrait)
    {
	prt_write_int(-((పూర్ణాంకం)prt_mediasize[prt_media].వెడల్పు));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    తిరిగి !prt_file_error;
}

    పూర్ణాంకం
mch_print_blank_పుట(శూన్య)
{
    తిరిగి (mch_print_begin_పుట(ఖాళీ) ? (mch_print_చివర_పుట()) : FALSE);
}

స్థిర దశాంశం prt_pos_x = 0;
స్థిర దశాంశం prt_pos_y = 0;

    శూన్య
mch_print_ప్రారంభం_పంక్తి(పూర్ణాంకం margin, పూర్ణాంకం పుట_పంక్తి)
{
    prt_pos_x = prt_ఎడమ_margin;
    ఒకవేళ (margin)
	prt_pos_x -= prt_సంఖ్య_వెడల్పు;

    prt_pos_y = prt_శిఖరం_margin - prt_మొదటి_పంక్తి_ఎత్తు -
					పుట_పంక్తి * prt_పంక్తి_ఎత్తు;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_వెడల్పు = FALSE;
}

    పూర్ణాంకం
mch_print_పాఠ్యం_బహిర్గతం(char_u *textp, పూర్ణాంకం len వాడని)
{
    char_u	*p = textp;
    పూర్ణాంకం		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    దశాంశం       char_వెడల్పు;
    దశాంశం       తదుపరి_pos;
    పూర్ణాంకం		లోపల_ascii;
    పూర్ణాంకం		half_వెడల్పు;
    char_u	*tofree = ఖాళీ;

    char_వెడల్పు = prt_char_వెడల్పు;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    ఒకవేళ (prt_use_courier)
    {
	లోపల_ascii = (len == 1 && *p < 0x80);
	ఒకవేళ (prt_లోపల_ascii)
	{
	    ఒకవేళ (!లోపల_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_లోపల_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	లేకపోతే ఒకవేళ (లోపల_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_లోపల_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	half_వెడల్పు = ((*mb_ptr2cells)(p) == 1);
	ఒకవేళ (half_వెడల్పు)
	    char_వెడల్పు /= 2;
	ఒకవేళ (prt_half_వెడల్పు)
	{
	    ఒకవేళ (!half_వెడల్పు)
	    {
		prt_half_వెడల్పు = FALSE;
		prt_pos_x += prt_char_వెడల్పు/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	లేకపోతే ఒకవేళ (half_వెడల్పు)
	{
	    prt_half_వెడల్పు = TRUE;
	    prt_pos_x += prt_char_వెడల్పు/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    ఒకవేళ (prt_attribute_change)
    {
	prt_flush_బఫరు();
	// Reset count of number of chars that will be printed
	prt_పాఠ్యం_నిరంతరం = 0;

	ఒకవేళ (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	ఒకవేళ (prt_need_font)
	{
	    ఒకవేళ (!prt_లోపల_ascii)
		prt_write_string("CF");
	    లేకపోతే
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	ఒకవేళ (prt_need_fgcol)
	{
	    పూర్ణాంకం     r, g, b;
	    r = ((గుర్తులేని)prt_fgcol & 0xff0000) >> 16;
	    g = ((గుర్తులేని)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    ఒకవేళ (r == g && g == b)
		prt_write_string("g\n");
	    లేకపోతే
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	ఒకవేళ (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    ఒకవేళ (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	లేకపోతే
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	ఒకవేళ (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    ఒకవేళ (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	ఒకవేళ (p == ఖాళీ)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    ఒకవేళ (prt_బహిర్గతం_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	అయితే (len-- > 0)
	{
	    ch = prt_hexchar[(గుర్తులేని)(*p) >> 4];
	    ga_append(&prt_ps_బఫరు, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_బఫరు, ch);
	    p++;
	}
    }
    లేకపోతే
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	ఒకవేళ (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_బఫరు, '\\');
	    ఎంపిక (ch)
	    {
		స్థితి BS:   ga_append(&prt_ps_బఫరు, 'b'); ఆపు;
		స్థితి TAB:  ga_append(&prt_ps_బఫరు, 't'); ఆపు;
		స్థితి NL:   ga_append(&prt_ps_బఫరు, 'n'); ఆపు;
		స్థితి FF:   ga_append(&prt_ps_బఫరు, 'f'); ఆపు;
		స్థితి CAR:  ga_append(&prt_ps_బఫరు, 'r'); ఆపు;
		స్థితి '(':  ga_append(&prt_ps_బఫరు, '('); ఆపు;
		స్థితి ')':  ga_append(&prt_ps_బఫరు, ')'); ఆపు;
		స్థితి '\\': ga_append(&prt_ps_బఫరు, '\\'); ఆపు;

		అప్రమేయ:
			   sprintf((అక్షరం *)ch_buff, "%03o", (గుర్తులేని పూర్ణాంకం)ch);
			   ga_append(&prt_ps_బఫరు, ch_buff[0]);
			   ga_append(&prt_ps_బఫరు, ch_buff[1]);
			   ga_append(&prt_ps_బఫరు, ch_buff[2]);
			   ఆపు;
	    }
	}
	లేకపోతే
	    ga_append(&prt_ps_బఫరు, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_పాఠ్యం_నిరంతరం += char_వెడల్పు;
    prt_pos_x += char_వెడల్పు;

    // The downside of fp - use relative error on right margin check
    తదుపరి_pos = prt_pos_x + prt_char_వెడల్పు;
    need_break = (తదుపరి_pos > prt_కుడి_margin) &&
		    ((తదుపరి_pos - prt_కుడి_margin) > (prt_కుడి_margin*1e-5));

    ఒకవేళ (need_break)
	prt_flush_బఫరు();

    తిరిగి need_break;
}

    శూన్య
mch_print_సమితి_font(పూర్ణాంకం iBold, పూర్ణాంకం iItalic, పూర్ణాంకం iUnderline)
{
    పూర్ణాంకం		font = 0;

    ఒకవేళ (iBold)
	font |= 0x01;
    ఒకవేళ (iItalic)
	font |= 0x02;

    ఒకవేళ (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    ఒకవేళ (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    శూన్య
mch_print_సమితి_bg(long_u bgcol)
{
    prt_bgcol = (పూర్ణాంకం)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    శూన్య
mch_print_సమితి_fg(long_u fgcol)
{
    ఒకవేళ (fgcol == (long_u)prt_fgcol)
	తిరిగి;

    prt_fgcol = (పూర్ణాంకం)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#అయితే_ముగింపు // FEAT_POSTSCRIPT
