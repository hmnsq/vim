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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_અંક_OPTIONS] =
{
    {"c",	FALSE, 0, ખાલી, 0, FALSE},
    {"a",	FALSE, 0, ખાલી, 0, FALSE},
    {"r",	FALSE, 0, ખાલી, 0, FALSE},
    {"b",	FALSE, 0, ખાલી, 0, FALSE},
    {"i",	FALSE, 0, ખાલી, 0, FALSE},
    {"o",	FALSE, 0, ખાલી, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
સંરચના prt_ps_font_S
{
    પૂર્ણાંક		wx;
    પૂર્ણાંક		uline_offset;
    પૂર્ણાંક		uline_width;
    પૂર્ણાંક		bbox_min_y;
    પૂર્ણાંક		bbox_max_y;
    અક્ષર	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
સ્થિર સંરચના prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
સ્થિર સંરચના prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {ખાલી, ખાલી, ખાલી, ખાલી}
};

// Pointer to current font set being used
સ્થિર સંરચના prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
સંરચના prt_ps_encoding_S
{
    અક્ષર	*encoding;
    અક્ષર	*cmap_encoding;
    પૂર્ણાંક		needs_charset;
};

સંરચના prt_ps_charset_S
{
    અક્ષર	*charset;
    અક્ષર	*cmap_charset;
    પૂર્ણાંક		has_charset;
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
સ્થિર સંરચના prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", ખાલી,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
સ્થિર સંરચના prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  ખાલી,       CS_JIS_X_1983},
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
સ્થિર સંરચના prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    ખાલી,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     ખાલી,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
સ્થિર સંરચના prt_ps_charset_S sc_charsets[] =
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
સ્થિર સંરચના prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    ખાલી,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
સ્થિર સંરચના prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    ખાલી,	CS_B5},
    {"CP950",	    ખાલી,	CS_B5},
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
સ્થિર સંરચના prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", ખાલી,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
સ્થિર સંરચના prt_ps_charset_S k_charsets[] =
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
સંરચના prt_ps_mbfont_S
{
    પૂર્ણાંક				અંક_encodings;
    સંરચના prt_ps_encoding_S	*encodings;
    પૂર્ણાંક				અંક_charsets;
    સંરચના prt_ps_charset_S	*charsets;
    અક્ષર			*ascii_enc;
    અક્ષર			*defcs;
};

સ્થિર સંરચના prt_ps_mbfont_S prt_ps_mbfonts[] =
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

સંરચના prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    પૂર્ણાંક     type;
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
સ્થિર અક્ષર *prt_resource_types[] =
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

સંરચના prt_dsc_comment_S
{
    અક્ષર	*string;
    પૂર્ણાંક		len;
    પૂર્ણાંક		type;
};

સંરચના prt_dsc_લીટી_S
{
    પૂર્ણાંક		type;
    char_u	*string;
    પૂર્ણાંક		len;
};


# define SIZEOF_CSTR(s)      (કદ(s) - 1)
સ્થિર સંરચના prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

સ્થિર શૂન્ય prt_write_file_len(char_u *buffer, પૂર્ણાંક bytes);
સ્થિર પૂર્ણાંક prt_next_dsc(સંરચના prt_dsc_લીટી_S *p_dsc_લીટી);

/*
 * Variables for the output PostScript file.
 */
સ્થિર FILE *prt_ps_fd;
સ્થિર પૂર્ણાંક prt_file_error;
સ્થિર char_u *prt_ps_file_name = ખાલી;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
સ્થિર દશાંશ prt_page_width;
સ્થિર દશાંશ prt_page_height;
સ્થિર દશાંશ prt_ડાબું_margin;
સ્થિર દશાંશ prt_જમણું_margin;
સ્થિર દશાંશ prt_ટોચ_margin;
સ્થિર દશાંશ prt_bottom_margin;
સ્થિર દશાંશ prt_લીટી_height;
સ્થિર દશાંશ prt_first_લીટી_height;
સ્થિર દશાંશ prt_char_width;
સ્થિર દશાંશ prt_સંખ્યા_width;
સ્થિર દશાંશ prt_bgcol_offset;
સ્થિર દશાંશ prt_pos_x_moveto = 0.0;
સ્થિર દશાંશ prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
સ્થિર પૂર્ણાંક prt_need_moveto;
સ્થિર પૂર્ણાંક prt_do_moveto;
સ્થિર પૂર્ણાંક prt_need_font;
સ્થિર પૂર્ણાંક prt_font;
સ્થિર પૂર્ણાંક prt_need_underline;
સ્થિર પૂર્ણાંક prt_underline;
સ્થિર પૂર્ણાંક prt_do_underline;
સ્થિર પૂર્ણાંક prt_need_fgcol;
સ્થિર પૂર્ણાંક prt_fgcol;
સ્થિર પૂર્ણાંક prt_need_bgcol;
સ્થિર પૂર્ણાંક prt_do_bgcol;
સ્થિર પૂર્ણાંક prt_bgcol;
સ્થિર પૂર્ણાંક prt_new_bgcol;
સ્થિર પૂર્ણાંક prt_attribute_change;
સ્થિર દશાંશ prt_text_run;
સ્થિર પૂર્ણાંક prt_page_અંક;
સ્થિર પૂર્ણાંક prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
સ્થિર પૂર્ણાંક prt_media;
સ્થિર પૂર્ણાંક prt_portrait;
સ્થિર પૂર્ણાંક prt_અંક_copies;
સ્થિર પૂર્ણાંક prt_duplex;
સ્થિર પૂર્ણાંક prt_tumble;
સ્થિર પૂર્ણાંક prt_collate;

/*
 * Buffers used when generating PostScript output
 */
સ્થિર char_u prt_લીટી_buffer[257];
સ્થિર garray_T prt_ps_buffer;

સ્થિર પૂર્ણાંક prt_do_conv;
સ્થિર vimconv_T prt_conv;

સ્થિર પૂર્ણાંક prt_out_mbyte;
સ્થિર પૂર્ણાંક prt_custom_cmap;
સ્થિર અક્ષર prt_cmap[80];
સ્થિર પૂર્ણાંક prt_use_courier;
સ્થિર પૂર્ણાંક prt_in_ascii;
સ્થિર પૂર્ણાંક prt_half_width;
સ્થિર અક્ષર *prt_ascii_encoding;
સ્થિર અચલ char_u prt_hexchar[] = "0123456789abcdef";

    સ્થિર શૂન્ય
prt_write_file_raw_len(char_u *buffer, પૂર્ણાંક bytes)
{
    જો (!prt_file_error
	    && fwrite(buffer, કદ(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    સ્થિર શૂન્ય
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (પૂર્ણાંક)STRLEN(buffer));
}

    સ્થિર શૂન્ય
prt_write_file_len(char_u *buffer, પૂર્ણાંક bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    સ્થિર શૂન્ય
prt_write_string(અક્ષર *s)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer), "%s", s);
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write an int and a space.
 */
    સ્થિર શૂન્ય
prt_write_int(પૂર્ણાંક i)
{
    sprintf((અક્ષર *)prt_લીટી_buffer, "%d ", i);
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write a boolean and a space.
 */
    સ્થિર શૂન્ય
prt_write_boolean(પૂર્ણાંક b)
{
    sprintf((અક્ષર *)prt_લીટી_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    સ્થિર શૂન્ય
prt_def_font(
    અક્ષર	*new_name,
    અક્ષર	*encoding,
    પૂર્ણાંક		height,
    અક્ષર	*font)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_લીટી_buffer);
    જો (prt_out_mbyte)
	sprintf((અક્ષર *)prt_લીટી_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    નહીંતર
	vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write a line to define the CID font.
 */
    સ્થિર શૂન્ય
prt_def_cidfont(અક્ષર *new_name, પૂર્ણાંક height, અક્ષર *cidfont)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_લીટી_buffer);
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    સ્થિર શૂન્ય
prt_dup_cidfont(અક્ષર *original_name, અક્ષર *new_name)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    સ્થિર શૂન્ય
prt_real_bits(
    દ્વિગુણ      real,
    પૂર્ણાંક		precision,
    પૂર્ણાંક		*pinteger,
    પૂર્ણાંક		*pfraction)
{
    પૂર્ણાંક     i;
    પૂર્ણાંક     integer;
    દશાંશ   fraction;

    integer = (પૂર્ણાંક)real;
    fraction = (દશાંશ)(real - integer);
    જો (real < (દ્વિગુણ)integer)
	fraction = -fraction;
    માટે (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (પૂર્ણાંક)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    સ્થિર શૂન્ય
prt_write_real(દ્વિગુણ val, પૂર્ણાંક prec)
{
    પૂર્ણાંક     integer;
    પૂર્ણાંક     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((અક્ષર *)prt_લીટી_buffer, "%d", integer);
    prt_write_file(prt_લીટી_buffer);
    // Only emit fraction if necessary
    જો (fraction != 0)
    {
	// Remove any trailing zeros
	જ્યારે ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((અક્ષર *)prt_લીટી_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_લીટી_buffer);
    }
    sprintf((અક્ષર *)prt_લીટી_buffer, " ");
    prt_write_file(prt_લીટી_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    સ્થિર શૂન્ય
prt_def_var(અક્ષર *name, દ્વિગુણ કિંમત, પૂર્ણાંક prec)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
								"/%s ", name);
    prt_write_file(prt_લીટી_buffer);
    prt_write_real(કિંમત, prec);
    sprintf((અક્ષર *)prt_લીટી_buffer, "d\n");
    prt_write_file(prt_લીટી_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    સ્થિર શૂન્ય
prt_flush_buffer(શૂન્ય)
{
    જો (prt_ps_buffer.ga_len <= 0)
	પાછા;

    // Any background color must be drawn first
    જો (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	પૂર્ણાંક     r, g, b;

	જો (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_લીટી_height, 2);

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
    જો (prt_do_underline)
    {
	જો (prt_do_moveto)
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
    જો (prt_out_mbyte)
	prt_write_string("<");
    નહીંતર
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    જો (prt_out_mbyte)
	prt_write_string(">");
    નહીંતર
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    જો (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    નહીંતર // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, કદ(અક્ષર), prt_bufsiz);
}


    સ્થિર શૂન્ય
prt_resource_name(char_u *filename, શૂન્ય *cookie)
{
    char_u *resource_filename = cookie;

    જો (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    નહીંતર
	STRCPY(resource_filename, filename);
}

    સ્થિર પૂર્ણાંક
prt_find_resource(અક્ષર *name, સંરચના prt_ps_resource_S *resource)
{
    char_u	*buffer;
    પૂર્ણાંક		retval;

    buffer = alloc(MAXPATHL + 1);
    જો (buffer == ખાલી)
	પાછા FALSE;

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
    પાછા retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
સંરચના prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    પૂર્ણાંક     len;
    પૂર્ણાંક     લીટી_start;
    પૂર્ણાંક     લીટી_end;
};

સ્થિર સંરચના prt_resfile_buffer_S prt_resfile;

    સ્થિર પૂર્ણાંક
prt_resfile_next_લીટી(શૂન્ય)
{
    પૂર્ણાંક     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.લીટી_end + 1;
    જ્યારે (idx < prt_resfile.len)
    {
	જો (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    તોડો;
	idx++;
    }
    prt_resfile.લીટી_start = idx;

    જ્યારે (idx < prt_resfile.len)
    {
	જો (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    તોડો;
	idx++;
    }
    prt_resfile.લીટી_end = idx;

    પાછા (idx < prt_resfile.len);
}

    સ્થિર પૂર્ણાંક
prt_resfile_strncmp(પૂર્ણાંક offset, અક્ષર *string, પૂર્ણાંક len)
{
    // Force not equal if string is longer than remainder of line
    જો (len > (prt_resfile.લીટી_end - (prt_resfile.લીટી_start + offset)))
	પાછા 1;

    પાછા STRNCMP(&prt_resfile.buffer[prt_resfile.લીટી_start + offset],
								string, len);
}

    સ્થિર પૂર્ણાંક
prt_resfile_skip_nonws(પૂર્ણાંક offset)
{
    પૂર્ણાંક     idx;

    idx = prt_resfile.લીટી_start + offset;
    જ્યારે (idx < prt_resfile.લીટી_end)
    {
	જો (SAFE_isspace(prt_resfile.buffer[idx]))
	    પાછા idx - prt_resfile.લીટી_start;
	idx++;
    }
    પાછા -1;
}

    સ્થિર પૂર્ણાંક
prt_resfile_skip_ws(પૂર્ણાંક offset)
{
    પૂર્ણાંક     idx;

    idx = prt_resfile.લીટી_start + offset;
    જ્યારે (idx < prt_resfile.લીટી_end)
    {
	જો (!SAFE_isspace(prt_resfile.buffer[idx]))
	    પાછા idx - prt_resfile.લીટી_start;
	idx++;
    }
    પાછા -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    સ્થિર પૂર્ણાંક
prt_next_dsc(સંરચના prt_dsc_લીટી_S *p_dsc_લીટી)
{
    પૂર્ણાંક     comment;
    પૂર્ણાંક     offset;

    // Move to start of next line
    જો (!prt_resfile_next_લીટી())
	પાછા FALSE;

    // DSC comments always start %%
    જો (prt_resfile_strncmp(0, "%%", 2) != 0)
	પાછા FALSE;

    // Find type of DSC comment
    માટે (comment = 0; comment < (પૂર્ણાંક)ARRAY_LENGTH(prt_dsc_table); comment++)
	જો (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    તોડો;

    જો (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_લીટી->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    નહીંતર
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_લીટી->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	જો (offset == -1)
	    પાછા FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    જો (offset == -1)
	પાછા FALSE;

    p_dsc_લીટી->string = &prt_resfile.buffer[prt_resfile.લીટી_start + offset];
    p_dsc_લીટી->len = prt_resfile.લીટી_end - (prt_resfile.લીટી_start + offset);

    પાછા TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    સ્થિર પૂર્ણાંક
prt_open_resource(સંરચના prt_ps_resource_S *resource)
{
    પૂર્ણાંક		offset;
    પૂર્ણાંક		seen_all;
    પૂર્ણાંક		seen_title;
    પૂર્ણાંક		seen_version;
    FILE	*fd_resource;
    સંરચના prt_dsc_લીટી_S dsc_લીટી;

    fd_resource = mch_fopen((અક્ષર *)resource->filename, READBIN);
    જો (fd_resource == ખાલી)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	પાછા FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (પૂર્ણાંક)fread((અક્ષર *)prt_resfile.buffer, કદ(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    જો (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	પાછા FALSE;
    }
    fclose(fd_resource);

    prt_resfile.લીટી_end = -1;
    prt_resfile.લીટી_start = 0;
    જો (!prt_resfile_next_લીટી())
	પાછા FALSE;

    offset = 0;

    જો (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	પાછા FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    જો (offset == -1)
	પાછા FALSE;
    offset = prt_resfile_skip_ws(offset);
    જો (offset == -1)
	પાછા FALSE;

    જો (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	પાછા FALSE;
    }
    offset += (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    જો (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    નહીંતર જો (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    નહીંતર જો (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (પૂર્ણાંક)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    નહીંતર
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	પાછા FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    જ્યારે (!seen_all && prt_next_dsc(&dsc_લીટી))
    {
	પસંદગી (dsc_લીટી.type)
	{
	સ્થિતિ PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_લીટી.string, dsc_લીટી.len);
	    seen_title = TRUE;
	    જો (seen_version)
		seen_all = TRUE;
	    તોડો;

	સ્થિતિ PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_લીટી.string, dsc_લીટી.len);
	    seen_version = TRUE;
	    જો (seen_title)
		seen_all = TRUE;
	    તોડો;

	સ્થિતિ PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    તોડો;

	સ્થિતિ PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    તોડો;
	}
    }

    જો (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	પાછા FALSE;
    }

    પાછા TRUE;
}

    સ્થિર પૂર્ણાંક
prt_check_resource(સંરચના prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    જો (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	પાછા FALSE;
    }

    // Other checks to be added as needed
    પાછા TRUE;
}

    સ્થિર શૂન્ય
prt_dsc_start(શૂન્ય)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    સ્થિર શૂન્ય
prt_dsc_noarg(અક્ષર *comment)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_લીટી_buffer);
}

    સ્થિર શૂન્ય
prt_dsc_textline(અક્ષર *comment, અક્ષર *text)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_લીટી_buffer);
}

    સ્થિર શૂન્ય
prt_dsc_text(અક્ષર *comment, અક્ષર *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_લીટી_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    સ્થિર શૂન્ય
prt_dsc_ints(અક્ષર *comment, પૂર્ણાંક ગણતરી, પૂર્ણાંક *ints)
{
    પૂર્ણાંક		i;

    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_લીટી_buffer);

    માટે (i = 0; i < ગણતરી; i++)
    {
	sprintf((અક્ષર *)prt_લીટી_buffer, " %d", ints[i]);
	prt_write_file(prt_લીટી_buffer);
    }

    prt_write_string("\n");
}

    સ્થિર શૂન્ય
prt_dsc_resources(
    અક્ષર	*comment,	// if NULL add to previous
    અક્ષર	*type,
    અક્ષર	*string)
{
    જો (comment != ખાલી)
	vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
						 "%%%%%s: %s", comment, type);
    નહીંતર
	vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_લીટી_buffer);

    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
							     " %s\n", string);
    prt_write_file(prt_લીટી_buffer);
}

    સ્થિર શૂન્ય
prt_dsc_font_resource(અક્ષર *resource, સંરચના prt_ps_font_S *ps_font)
{
    પૂર્ણાંક     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    માટે (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	જો (ps_font->ps_fontname[i] != ખાલી)
	    prt_dsc_resources(ખાલી, "font", ps_font->ps_fontname[i]);
}

    સ્થિર શૂન્ય
prt_dsc_requirements(
    પૂર્ણાંક		duplex,
    પૂર્ણાંક		tumble,
    પૂર્ણાંક		collate,
    પૂર્ણાંક		color,
    પૂર્ણાંક		અંક_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    જો (!(duplex || collate || color || (અંક_copies > 1)))
	પાછા;

    sprintf((અક્ષર *)prt_લીટી_buffer, "%%%%Requirements:");
    prt_write_file(prt_લીટી_buffer);

    જો (duplex)
    {
	prt_write_string(" duplex");
	જો (tumble)
	    prt_write_string("(tumble)");
    }
    જો (collate)
	prt_write_string(" collate");
    જો (color)
	prt_write_string(" color");
    જો (અંક_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((અક્ષર *)prt_લીટી_buffer, "%d", અંક_copies);
	prt_write_file(prt_લીટી_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    સ્થિર શૂન્ય
prt_dsc_docmedia(
    અક્ષર	*paper_name,
    દ્વિગુણ	width,
    દ્વિગુણ	height,
    દ્વિગુણ	weight,
    અક્ષર	*colour,
    અક્ષર	*type)
{
    vim_snprintf((અક્ષર *)prt_લીટી_buffer, કદ(prt_લીટી_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_લીટી_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    જો (colour == ખાલી)
	prt_write_string("()");
    નહીંતર
	prt_write_string(colour);
    prt_write_string(" ");
    જો (type == ખાલી)
	prt_write_string("()");
    નહીંતર
	prt_write_string(type);
    prt_write_string("\n");
}

    શૂન્ય
mch_print_cleanup(શૂન્ય)
{
    જો (prt_out_mbyte)
    {
	પૂર્ણાંક     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	માટે (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    જો (prt_ps_mb_font.ps_fontname[i] != ખાલી)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    જો (prt_do_conv)
    {
	convert_setup(&prt_conv, ખાલી, ખાલી);
	prt_do_conv = FALSE;
    }
    જો (prt_ps_fd != ખાલી)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = ખાલી;
	prt_file_error = FALSE;
    }
    જો (prt_ps_file_name != ખાલી)
	VIM_CLEAR(prt_ps_file_name);
}

    સ્થિર દશાંશ
to_device_units(પૂર્ણાંક idx, દ્વિગુણ physsize, પૂર્ણાંક def_સંખ્યા)
{
    દશાંશ	ret;
    પૂર્ણાંક		u;
    પૂર્ણાંક		nr;

    u = prt_get_unit(idx);
    જો (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_સંખ્યા;
    }
    નહીંતર
	nr = printer_opts[idx].સંખ્યા;

    પસંદગી (u)
    {
	સ્થિતિ PRT_UNIT_INCH:
	    ret = (દશાંશ)(nr * PRT_PS_DEFAULT_DPI);
	    તોડો;
	સ્થિતિ PRT_UNIT_MM:
	    ret = (દશાંશ)(nr * PRT_PS_DEFAULT_DPI) / (દશાંશ)25.4;
	    તોડો;
	સ્થિતિ PRT_UNIT_POINT:
	    ret = (દશાંશ)nr;
	    તોડો;
	સ્થિતિ PRT_UNIT_PERC:
	મૂળભૂત:
	    ret = (દશાંશ)(physsize * nr) / 100;
	    તોડો;
    }

    પાછા ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    સ્થિર શૂન્ય
prt_page_margins(
    દ્વિગુણ	width,
    દ્વિગુણ	height,
    દ્વિગુણ	*ડાબું,
    દ્વિગુણ	*જમણું,
    દ્વિગુણ	*ટોચ,
    દ્વિગુણ	*bottom)
{
    *ડાબું   = to_device_units(OPT_PRINT_ડાબું, width, 10);
    *જમણું  = width - to_device_units(OPT_PRINT_જમણું, width, 5);
    *ટોચ    = height - to_device_units(OPT_PRINT_ટોચ, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    સ્થિર શૂન્ય
prt_font_metrics(પૂર્ણાંક font_scale)
{
    prt_લીટી_height = (દશાંશ)font_scale;
    prt_char_width = (દશાંશ)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    સ્થિર પૂર્ણાંક
prt_get_cpl(શૂન્ય)
{
    જો (prt_use_સંખ્યા())
    {
	prt_સંખ્યા_width = PRINT_સંખ્યા_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	જો (prt_out_mbyte)
	    prt_સંખ્યા_width /= 2;
	prt_ડાબું_margin += prt_સંખ્યા_width;
    }
    નહીંતર
	prt_સંખ્યા_width = 0.0;

    પાછા (પૂર્ણાંક)((prt_જમણું_margin - prt_ડાબું_margin) / prt_char_width);
}

    સ્થિર પૂર્ણાંક
prt_build_cid_fontname(પૂર્ણાંક font, char_u *name, પૂર્ણાંક name_len)
{
    અક્ષર    *fontname;

    fontname = alloc(name_len + 1);
    જો (fontname == ખાલી)
	પાછા FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    પાછા TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    સ્થિર પૂર્ણાંક
prt_get_lpp(શૂન્ય)
{
    પૂર્ણાંક lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (દશાંશ)PRT_PS_FONT_TO_USER(prt_લીટી_height,
					   prt_ps_font->bbox_min_y);
    જો ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (દશાંશ)PRT_PS_FONT_TO_USER(prt_લીટી_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_લીટી_height = prt_લીટી_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (પૂર્ણાંક)((prt_ટોચ_margin - prt_bottom_margin) / prt_લીટી_height);

    // Adjust top margin if there is a header
    prt_ટોચ_margin -= prt_લીટી_height * prt_header_height();

    પાછા lpp - prt_header_height();
}

    સ્થિર પૂર્ણાંક
prt_match_encoding(
    અક્ષર			*p_encoding,
    સંરચના prt_ps_mbfont_S	*p_cmap,
    સંરચના prt_ps_encoding_S	**pp_mbenc)
{
    પૂર્ણાંક				mbenc;
    પૂર્ણાંક				enc_len;
    સંરચના prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = ખાલી;
    // Look for recognised encoding
    enc_len = (પૂર્ણાંક)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    માટે (mbenc = 0; mbenc < p_cmap->અંક_encodings; mbenc++)
    {
	જો (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    પાછા TRUE;
	}
	p_mbenc++;
    }
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
prt_match_charset(
    અક્ષર		    *p_charset,
    સંરચના prt_ps_mbfont_S  *p_cmap,
    સંરચના prt_ps_charset_S **pp_mbchar)
{
    પૂર્ણાંક			    mbchar;
    પૂર્ણાંક			    char_len;
    સંરચના prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    જો (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (પૂર્ણાંક)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    માટે (mbchar = 0; mbchar < p_cmap->અંક_charsets; mbchar++)
    {
	જો (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    પાછા TRUE;
	}
	p_mbchar++;
    }
    પાછા FALSE;
}

    પૂર્ણાંક
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    પૂર્ણાંક		forceit UNUSED)
{
    પૂર્ણાંક		i;
    અક્ષર	*paper_name;
    પૂર્ણાંક		paper_strlen;
    પૂર્ણાંક		fontsize;
    char_u	*p;
    દ્વિગુણ      ડાબું;
    દ્વિગુણ      જમણું;
    દ્વિગુણ      ટોચ;
    દ્વિગુણ      bottom;
    પૂર્ણાંક		props;
    પૂર્ણાંક		cmap = 0;
    char_u	*p_encoding;
    સંરચના prt_ps_encoding_S *p_mbenc;
    સંરચના prt_ps_encoding_S *p_mbenc_first;
    સંરચના prt_ps_charset_S  *p_mbchar = ખાલી;

# જો 0
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
    જો (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    જો (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = ખાલી;
    props = enc_canon_props(p_encoding);
    જો (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	પૂર્ણાંક cmap_first = 0;

	p_mbenc_first = ખાલી;
	માટે (cmap = 0; cmap < (પૂર્ણાંક)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    જો (prt_match_encoding((અક્ષર *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		જો (p_mbenc_first == ખાલી)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		જો (prt_match_charset((અક્ષર *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    તોડો;
	    }

	// Use first encoding matched if no charset matched
	જો (p_mbchar == ખાલી && p_mbenc_first != ખાલી)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != ખાલી);
    જો (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == ખાલી);
	જો (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    જો ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		પાછા FALSE;
	    }

	    // Add charset name if not empty
	    જો (p_mbchar->cmap_charset != ખાલી)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, કદ(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	નહીંતર
	{
	    // Add custom CMap character set name
	    જો (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		પાછા FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, કદ(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	જો (p_mbenc->cmap_encoding != ખાલી && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < કદ(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	જો (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    પાછા FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	જો (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    પાછા FALSE;
	જો (mbfont_opts[OPT_MBFONT_BOLD].present)
	    જો (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		પાછા FALSE;
	જો (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    જો (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		પાછા FALSE;
	જો (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    જો (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		પાછા FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	જો (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    જો (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    નહીંતર
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    નહીંતર
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    જો (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (અક્ષર *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    નહીંતર
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    માટે (i = 0; i < (પૂર્ણાંક)PRT_MEDIASIZE_LEN; ++i)
	જો (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    તોડો;
    જો (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    જો (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    નહીંતર
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &ડાબું, &જમણું, &ટોચ,
								    &bottom);
    prt_ડાબું_margin = (દશાંશ)ડાબું;
    prt_જમણું_margin = (દશાંશ)જમણું;
    prt_ટોચ_margin = (દશાંશ)ટોચ;
    prt_bottom_margin = (દશાંશ)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    માટે (p = p_pfn; (p = vim_strchr(p, ':')) != ખાલી; ++p)
	જો (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((અક્ષર *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_લીટી = prt_get_cpl();
    psettings->લીટીઓ_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    જો (psettings->chars_per_લીટી <= 0 || psettings->લીટીઓ_per_page <= 0)
	પાછા FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_અંક_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    જો (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    નહીંતર
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_અંક_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    જો (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	જો (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	નહીંતર જો (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    જો (psettings->outfile == ખાલી)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	જો (prt_ps_file_name == ખાલી)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    પાછા FAIL;
	}
	prt_ps_fd = mch_fopen((અક્ષર *)prt_ps_file_name, WRITEBIN);
    }
    નહીંતર
    {
	p = expand_env_save(psettings->outfile);
	જો (p != ખાલી)
	{
	    prt_ps_fd = mch_fopen((અક્ષર *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    જો (prt_ps_fd == ખાલી)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	પાછા FAIL;
    }

    prt_bufsiz = psettings->chars_per_લીટી;
    જો (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, કદ(અક્ષર), prt_bufsiz);

    prt_page_અંક = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
prt_add_resource(સંરચના prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((અક્ષર *)resource->filename, READBIN);
    જો (fd_resource == ખાલી)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	પાછા FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (અક્ષર *)resource->title);

    prt_dsc_textline("BeginDocument", (અક્ષર *)resource->filename);

    માટે (;;)
    {
	bytes_read = fread((અક્ષર *)resource_buffer, કદ(char_u),
			   કદ(resource_buffer), fd_resource);
	જો (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    પાછા FALSE;
	}
	જો (bytes_read == 0)
	    તોડો;
	prt_write_file_raw_len(resource_buffer, (પૂર્ણાંક)bytes_read);
	જો (prt_file_error)
	{
	    fclose(fd_resource);
	    પાછા FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    પાછા TRUE;
}

    પૂર્ણાંક
mch_print_begin(prt_settings_T *psettings)
{
    પૂર્ણાંક		bbox[4];
    દ્વિગુણ      ડાબું;
    દ્વિગુણ      જમણું;
    દ્વિગુણ      ટોચ;
    દ્વિગુણ      bottom;
    સંરચના prt_ps_resource_S *res_prolog;
    સંરચના prt_ps_resource_S *res_encoding;
    અક્ષર	buffer[256];
    char_u      *p_encoding;
    સંરચના prt_ps_resource_S *res_cidfont;
    સંરચના prt_ps_resource_S *res_cmap;
    પૂર્ણાંક		retval = FALSE;

    res_prolog = ALLOC_ONE(સંરચના prt_ps_resource_S);
    res_encoding = ALLOC_ONE(સંરચના prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(સંરચના prt_ps_resource_S);
    res_cmap = ALLOC_ONE(સંરચના prt_ps_resource_S);
    જો (res_prolog == ખાલી || res_encoding == ખાલી
	    || res_cidfont == ખાલી || res_cmap == ખાલી)
	જાઓ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (અક્ષર *)psettings->jobname);
    જો (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(ખાલી), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &ડાબું, &જમણું, &ટોચ, &bottom);
    bbox[0] = (પૂર્ણાંક)ડાબું;
    જો (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (પૂર્ણાંક)(ટોચ - (psettings->લીટીઓ_per_page + prt_header_height())
						    * (દ્વિગુણ)prt_લીટી_height);
	bbox[2] = (પૂર્ણાંક)(ડાબું + psettings->chars_per_લીટી
					       * (દ્વિગુણ)prt_char_width + 0.5);
	bbox[3] = (પૂર્ણાંક)(ટોચ + 0.5);
    }
    નહીંતર
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (પૂર્ણાંક)bottom;
	bbox[2] = (પૂર્ણાંક)(ડાબું + ((psettings->લીટીઓ_per_page
			      + prt_header_height()) * prt_લીટી_height) + 0.5);
	bbox[3] = (પૂર્ણાંક)(bottom + psettings->chars_per_લીટી
					       * (દ્વિગુણ)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(દ્વિગુણ)0, ખાલી, ખાલી);
    // Define fonts needed
    જો (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    જો (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ખાલી
				: "DocumentNeededResources"), &prt_ps_mb_font);
	જો (!prt_custom_cmap)
	    prt_dsc_resources(ખાલી, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    જો (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	જાઓ theend;
    }
    જો (!prt_open_resource(res_prolog))
	જાઓ theend;
    જો (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	જાઓ theend;
    જો (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	જો (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    જાઓ theend;
	}
	જો (!prt_open_resource(res_cidfont))
	    જાઓ theend;
	જો (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    જાઓ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    જો (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	જો (*p_encoding == NUL
		|| !prt_find_resource((અક્ષર *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    પૂર્ણાંક		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    જો (!(props & ENC_8BIT)
		    || !prt_find_resource((અક્ષર *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		જો (!prt_find_resource((અક્ષર *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    જાઓ theend;
		}
	    }
	}
	જો (!prt_open_resource(res_encoding))
	    જાઓ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    નહીંતર
    {
	p_encoding = enc_skip(p_penc);
	જો (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	જો (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    જો (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		જાઓ theend;
	    }
	    જો (!prt_open_resource(res_encoding))
		જાઓ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    જો (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	જો (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    જાઓ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    જો (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	જો (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    જાઓ theend;
	}
	જો (!prt_open_resource(res_cmap))
	    જાઓ theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    જો (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(ખાલી, "procset", buffer);

	જો (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(ખાલી, "cmap", buffer);
	}
    }
    જો (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(ખાલી, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# નહીંતર
					0
# endif
					, prt_અંક_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    જો (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    જો (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? ખાલી : "PageResources"),
							    &prt_ps_mb_font);
	જો (!prt_custom_cmap)
	    prt_dsc_resources(ખાલી, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    જો (!prt_add_resource(res_prolog))
	જાઓ theend;
    જો (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	જો (!prt_add_resource(res_cidfont))
	    જાઓ theend;
	જો (prt_custom_cmap && !prt_add_resource(res_cmap))
	    જાઓ theend;
    }

    જો (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	જો (!prt_add_resource(res_encoding))
	    જાઓ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((પૂર્ણાંક)prt_mediasize[prt_media].width);
    prt_write_int((પૂર્ણાંક)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_અંક_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    જો (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	જો (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (અક્ષર *)p_encoding, (પૂર્ણાંક)prt_લીટી_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (અક્ષર *)p_encoding, (પૂર્ણાંક)prt_લીટી_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (અક્ષર *)p_encoding, (પૂર્ણાંક)prt_લીટી_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (અક્ષર *)p_encoding, (પૂર્ણાંક)prt_લીટી_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    જો (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	જો (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (પૂર્ણાંક)prt_લીટી_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	જો (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != ખાલી)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    જો (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (પૂર્ણાંક)prt_લીટી_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	નહીંતર
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	જો (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != ખાલી)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    જો (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (પૂર્ણાંક)prt_લીટી_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	નહીંતર
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	જો (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != ખાલી)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    જો (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (પૂર્ણાંક)prt_લીટી_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	નહીંતર
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_લીટી_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_લીટી_height,
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

    પાછા retval;
}

    શૂન્ય
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_અંક);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    જો (!prt_file_error && psettings->outfile == ખાલી
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	જો (prt_ps_fd != ખાલી)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = ખાલી;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	જો (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	નહીંતર
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    પૂર્ણાંક
mch_print_end_page(શૂન્ય)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    પાછા !prt_file_error;
}

    પૂર્ણાંક
mch_print_begin_page(char_u *str UNUSED)
{
    પૂર્ણાંક		page_અંક[2];

    prt_page_અંક++;

    page_અંક[0] = page_અંક[1] = prt_page_અંક;
    prt_dsc_ints("Page", 2, page_અંક);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    જો (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    નહીંતર
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    જો (!prt_portrait)
    {
	prt_write_int(-((પૂર્ણાંક)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    પાછા !prt_file_error;
}

    પૂર્ણાંક
mch_print_blank_page(શૂન્ય)
{
    પાછા (mch_print_begin_page(ખાલી) ? (mch_print_end_page()) : FALSE);
}

સ્થિર દશાંશ prt_pos_x = 0;
સ્થિર દશાંશ prt_pos_y = 0;

    શૂન્ય
mch_print_start_લીટી(પૂર્ણાંક margin, પૂર્ણાંક page_લીટી)
{
    prt_pos_x = prt_ડાબું_margin;
    જો (margin)
	prt_pos_x -= prt_સંખ્યા_width;

    prt_pos_y = prt_ટોચ_margin - prt_first_લીટી_height -
					page_લીટી * prt_લીટી_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    પૂર્ણાંક
mch_print_text_out(char_u *textp, પૂર્ણાંક len UNUSED)
{
    char_u	*p = textp;
    પૂર્ણાંક		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    દશાંશ       char_width;
    દશાંશ       next_pos;
    પૂર્ણાંક		in_ascii;
    પૂર્ણાંક		half_width;
    char_u	*tofree = ખાલી;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    જો (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	જો (prt_in_ascii)
	{
	    જો (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	નહીંતર જો (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    જો (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	જો (half_width)
	    char_width /= 2;
	જો (prt_half_width)
	{
	    જો (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	નહીંતર જો (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    જો (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	જો (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	જો (prt_need_font)
	{
	    જો (!prt_in_ascii)
		prt_write_string("CF");
	    નહીંતર
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	જો (prt_need_fgcol)
	{
	    પૂર્ણાંક     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    જો (r == g && g == b)
		prt_write_string("g\n");
	    નહીંતર
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	જો (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    જો (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	નહીંતર
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	જો (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    જો (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	જો (p == ખાલી)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    જો (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	જ્યારે (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    નહીંતર
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	જો (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    પસંદગી (ch)
	    {
		સ્થિતિ BS:   ga_append(&prt_ps_buffer, 'b'); તોડો;
		સ્થિતિ TAB:  ga_append(&prt_ps_buffer, 't'); તોડો;
		સ્થિતિ NL:   ga_append(&prt_ps_buffer, 'n'); તોડો;
		સ્થિતિ FF:   ga_append(&prt_ps_buffer, 'f'); તોડો;
		સ્થિતિ CAR:  ga_append(&prt_ps_buffer, 'r'); તોડો;
		સ્થિતિ '(':  ga_append(&prt_ps_buffer, '('); તોડો;
		સ્થિતિ ')':  ga_append(&prt_ps_buffer, ')'); તોડો;
		સ્થિતિ '\\': ga_append(&prt_ps_buffer, '\\'); તોડો;

		મૂળભૂત:
			   sprintf((અક્ષર *)ch_buff, "%03o", (unsigned પૂર્ણાંક)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   તોડો;
	    }
	}
	નહીંતર
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_જમણું_margin) &&
		    ((next_pos - prt_જમણું_margin) > (prt_જમણું_margin*1e-5));

    જો (need_break)
	prt_flush_buffer();

    પાછા need_break;
}

    શૂન્ય
mch_print_set_font(પૂર્ણાંક iBold, પૂર્ણાંક iItalic, પૂર્ણાંક iUnderline)
{
    પૂર્ણાંક		font = 0;

    જો (iBold)
	font |= 0x01;
    જો (iItalic)
	font |= 0x02;

    જો (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    જો (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    શૂન્ય
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (પૂર્ણાંક)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    શૂન્ય
mch_print_set_fg(long_u fgcol)
{
    જો (fgcol == (long_u)prt_fgcol)
	પાછા;

    prt_fgcol = (પૂર્ણાંક)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
