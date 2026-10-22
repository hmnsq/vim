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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#ifdef FEAT_POSTSCRIPT

# include "hardcopy.h"
# include "hardcopy_postscript.h"

option_table_T mbfont_opts[OPT_MBFONT_अंक_OPTIONS] =
{
    {"c",	FALSE, 0, रिक्त, 0, FALSE},
    {"a",	FALSE, 0, रिक्त, 0, FALSE},
    {"r",	FALSE, 0, रिक्त, 0, FALSE},
    {"b",	FALSE, 0, रिक्त, 0, FALSE},
    {"i",	FALSE, 0, रिक्त, 0, FALSE},
    {"o",	FALSE, 0, रिक्त, 0, FALSE},
};

# define PRT_PS_DEFAULT_DPI	    (72)    // Default user space resolution
# define PRT_PS_DEFAULT_FONTSIZE     (10)
# define PRT_PS_DEFAULT_BUFFER_SIZE  (80)

// PS font names, must be in Roman, Bold, Italic, Bold-Italic order
संरचना prt_ps_font_S
{
    पूर्णाङ्क		wx;
    पूर्णाङ्क		uline_offset;
    पूर्णाङ्क		uline_width;
    पूर्णाङ्क		bbox_min_y;
    पूर्णाङ्क		bbox_max_y;
    वर्ण	*(ps_fontname[4]);
};

# define PRT_PS_FONT_ROMAN	(0)
# define PRT_PS_FONT_BOLD	(1)
# define PRT_PS_FONT_OBLIQUE	(2)
# define PRT_PS_FONT_BOLDOBLIQUE (3)

// Standard font metrics for Courier family
स्थिर संरचना prt_ps_font_S prt_ps_courier_font =
{
    600,
    -100, 50,
    -250, 805,
    {"Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique"}
};

// Generic font metrics for multi-byte fonts
स्थिर संरचना prt_ps_font_S prt_ps_mb_font =
{
    1000,
    -100, 50,
    -250, 805,
    {रिक्त, रिक्त, रिक्त, रिक्त}
};

// Pointer to current font set being used
स्थिर संरचना prt_ps_font_S* prt_ps_font;

// Structures to map user named encoding and mapping to PS equivalents for
// building CID font name
संरचना prt_ps_encoding_S
{
    वर्ण	*encoding;
    वर्ण	*cmap_encoding;
    पूर्णाङ्क		needs_charset;
};

संरचना prt_ps_charset_S
{
    वर्ण	*charset;
    वर्ण	*cmap_charset;
    पूर्णाङ्क		has_charset;
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
स्थिर संरचना prt_ps_encoding_S j_encodings[] =
{
    {"iso-2022-jp", रिक्त,       (CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990|
								    CS_NEC)},
    {"euc-jp",	    "EUC",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_JIS_X_1990)},
    {"sjis",	    "RKSJ",	(CS_JIS_C_1978|CS_JIS_X_1983|CS_MSWINDOWS|
						CS_KANJITALK6|CS_KANJITALK7)},
    {"cp932",       "RKSJ",     CS_JIS_X_1983},
    {"ucs-2",       "UCS2",     CS_JIS_X_1990},
    {"utf-8",       "UTF8" ,    CS_JIS_X_1990}
};
स्थिर संरचना prt_ps_charset_S j_charsets[] =
{
    {"JIS_C_1978",  "78",       CS_JIS_C_1978},
    {"JIS_X_1983",  रिक्त,       CS_JIS_X_1983},
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
स्थिर संरचना prt_ps_encoding_S sc_encodings[] =
{
    {"iso-2022",    रिक्त,       (CS_GB_2312_80|CS_GBT_12345_90)},
    {"gb18030",     रिक्त,       CS_GBK2K},
    {"euc-cn",      "EUC",      (CS_GB_2312_80|CS_GBT_12345_90|CS_SC_MAC|
								CS_GBT_90_MAC)},
    {"gbk",	    "EUC",	CS_GBK},
    {"ucs-2",       "UCS2",     CS_SC_ISO10646},
    {"utf-8",       "UTF8",     CS_SC_ISO10646}
};
स्थिर संरचना prt_ps_charset_S sc_charsets[] =
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
स्थिर संरचना prt_ps_encoding_S tc_encodings[] =
{
    {"iso-2022",    रिक्त,       (CS_CNS_PLANE_1|CS_CNS_PLANE_2)},
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
स्थिर संरचना prt_ps_charset_S tc_charsets[] =
{
    {"CNS_1992_1",  "CNS1",     CS_CNS_PLANE_1},
    {"CNS_1992_2",  "CNS2",     CS_CNS_PLANE_2},
    {"CNS_1993",    "CNS",      CS_CNS_PLANE_1_2},
    {"BIG5",	    रिक्त,	CS_B5},
    {"CP950",	    रिक्त,	CS_B5},
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
स्थिर संरचना prt_ps_encoding_S k_encodings[] =
{
    {"iso-2022-kr", रिक्त,       CS_KR_X_1992},
    {"euc-kr",      "EUC",      (CS_KR_X_1992|CS_KR_MAC)},
    {"johab",       "Johab",    CS_KR_X_1992},
    {"cp1361",      "Johab",    CS_KR_X_1992},
    {"uhc",	    "UHC",	CS_KR_X_1992_MS},
    {"cp949",       "UHC",      CS_KR_X_1992_MS},
    {"ucs-2",       "UCS2",     CS_KR_ISO10646},
    {"utf-8",       "UTF8",     CS_KR_ISO10646}
};
स्थिर संरचना prt_ps_charset_S k_charsets[] =
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
संरचना prt_ps_mbfont_S
{
    पूर्णाङ्क				अंक_encodings;
    संरचना prt_ps_encoding_S	*encodings;
    पूर्णाङ्क				अंक_charsets;
    संरचना prt_ps_charset_S	*charsets;
    वर्ण			*ascii_enc;
    वर्ण			*defcs;
};

स्थिर संरचना prt_ps_mbfont_S prt_ps_mbfonts[] =
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

संरचना prt_ps_resource_S
{
    char_u  name[64];
    char_u  filename[MAXPATHL + 1];
    पूर्णाङ्क     type;
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
स्थिर वर्ण *prt_resource_types[] =
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

संरचना prt_dsc_comment_S
{
    वर्ण	*string;
    पूर्णाङ्क		len;
    पूर्णाङ्क		type;
};

संरचना prt_dsc_पंक्ति_S
{
    पूर्णाङ्क		type;
    char_u	*string;
    पूर्णाङ्क		len;
};


# define SIZEOF_CSTR(s)      (परिमाण(s) - 1)
स्थिर संरचना prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

स्थिर शून्य prt_write_file_len(char_u *buffer, पूर्णाङ्क bytes);
स्थिर पूर्णाङ्क prt_next_dsc(संरचना prt_dsc_पंक्ति_S *p_dsc_पंक्ति);

/*
 * Variables for the output PostScript file.
 */
स्थिर FILE *prt_ps_fd;
स्थिर पूर्णाङ्क prt_file_error;
स्थिर char_u *prt_ps_file_name = रिक्त;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
स्थिर भिन्न prt_page_width;
स्थिर भिन्न prt_page_height;
स्थिर भिन्न prt_बायाँ_margin;
स्थिर भिन्न prt_दायाँ_margin;
स्थिर भिन्न prt_शीर्ष_margin;
स्थिर भिन्न prt_bottom_margin;
स्थिर भिन्न prt_पंक्ति_height;
स्थिर भिन्न prt_first_पंक्ति_height;
स्थिर भिन्न prt_char_width;
स्थिर भिन्न prt_संख्या_width;
स्थिर भिन्न prt_bgcol_offset;
स्थिर भिन्न prt_pos_x_moveto = 0.0;
स्थिर भिन्न prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
स्थिर पूर्णाङ्क prt_need_moveto;
स्थिर पूर्णाङ्क prt_do_moveto;
स्थिर पूर्णाङ्क prt_need_font;
स्थिर पूर्णाङ्क prt_font;
स्थिर पूर्णाङ्क prt_need_underline;
स्थिर पूर्णाङ्क prt_underline;
स्थिर पूर्णाङ्क prt_do_underline;
स्थिर पूर्णाङ्क prt_need_fgcol;
स्थिर पूर्णाङ्क prt_fgcol;
स्थिर पूर्णाङ्क prt_need_bgcol;
स्थिर पूर्णाङ्क prt_do_bgcol;
स्थिर पूर्णाङ्क prt_bgcol;
स्थिर पूर्णाङ्क prt_new_bgcol;
स्थिर पूर्णाङ्क prt_attribute_change;
स्थिर भिन्न prt_text_run;
स्थिर पूर्णाङ्क prt_page_अंक;
स्थिर पूर्णाङ्क prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
स्थिर पूर्णाङ्क prt_media;
स्थिर पूर्णाङ्क prt_portrait;
स्थिर पूर्णाङ्क prt_अंक_copies;
स्थिर पूर्णाङ्क prt_duplex;
स्थिर पूर्णाङ्क prt_tumble;
स्थिर पूर्णाङ्क prt_collate;

/*
 * Buffers used when generating PostScript output
 */
स्थिर char_u prt_पंक्ति_buffer[257];
स्थिर garray_T prt_ps_buffer;

स्थिर पूर्णाङ्क prt_do_conv;
स्थिर vimconv_T prt_conv;

स्थिर पूर्णाङ्क prt_out_mbyte;
स्थिर पूर्णाङ्क prt_custom_cmap;
स्थिर वर्ण prt_cmap[80];
स्थिर पूर्णाङ्क prt_use_courier;
स्थिर पूर्णाङ्क prt_in_ascii;
स्थिर पूर्णाङ्क prt_half_width;
स्थिर वर्ण *prt_ascii_encoding;
स्थिर ध्रुव char_u prt_hexchar[] = "0123456789abcdef";

    स्थिर शून्य
prt_write_file_raw_len(char_u *buffer, पूर्णाङ्क bytes)
{
    यदि (!prt_file_error
	    && fwrite(buffer, परिमाण(char_u), bytes, prt_ps_fd)
							     != (size_t)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    स्थिर शून्य
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (पूर्णाङ्क)STRLEN(buffer));
}

    स्थिर शून्य
prt_write_file_len(char_u *buffer, पूर्णाङ्क bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    स्थिर शून्य
prt_write_string(वर्ण *s)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer), "%s", s);
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write an int and a space.
 */
    स्थिर शून्य
prt_write_int(पूर्णाङ्क i)
{
    sprintf((वर्ण *)prt_पंक्ति_buffer, "%d ", i);
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write a boolean and a space.
 */
    स्थिर शून्य
prt_write_boolean(पूर्णाङ्क b)
{
    sprintf((वर्ण *)prt_पंक्ति_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    स्थिर शून्य
prt_def_font(
    वर्ण	*new_name,
    वर्ण	*encoding,
    पूर्णाङ्क		height,
    वर्ण	*font)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_पंक्ति_buffer);
    यदि (prt_out_mbyte)
	sprintf((वर्ण *)prt_पंक्ति_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    अन्यथा
	vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write a line to define the CID font.
 */
    स्थिर शून्य
prt_def_cidfont(वर्ण *new_name, पूर्णाङ्क height, वर्ण *cidfont)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_पंक्ति_buffer);
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    स्थिर शून्य
prt_dup_cidfont(वर्ण *original_name, वर्ण *new_name)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    स्थिर शून्य
prt_real_bits(
    द्विगुण      real,
    पूर्णाङ्क		precision,
    पूर्णाङ्क		*pinteger,
    पूर्णाङ्क		*pfraction)
{
    पूर्णाङ्क     i;
    पूर्णाङ्क     integer;
    भिन्न   fraction;

    integer = (पूर्णाङ्क)real;
    fraction = (भिन्न)(real - integer);
    यदि (real < (द्विगुण)integer)
	fraction = -fraction;
    प्रति (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (पूर्णाङ्क)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    स्थिर शून्य
prt_write_real(द्विगुण val, पूर्णाङ्क prec)
{
    पूर्णाङ्क     integer;
    पूर्णाङ्क     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    sprintf((वर्ण *)prt_पंक्ति_buffer, "%d", integer);
    prt_write_file(prt_पंक्ति_buffer);
    // Only emit fraction if necessary
    यदि (fraction != 0)
    {
	// Remove any trailing zeros
	यावत् ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	sprintf((वर्ण *)prt_पंक्ति_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_पंक्ति_buffer);
    }
    sprintf((वर्ण *)prt_पंक्ति_buffer, " ");
    prt_write_file(prt_पंक्ति_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    स्थिर शून्य
prt_def_var(वर्ण *name, द्विगुण मूल्य, पूर्णाङ्क prec)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
								"/%s ", name);
    prt_write_file(prt_पंक्ति_buffer);
    prt_write_real(मूल्य, prec);
    sprintf((वर्ण *)prt_पंक्ति_buffer, "d\n");
    prt_write_file(prt_पंक्ति_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    स्थिर शून्य
prt_flush_buffer(शून्य)
{
    यदि (prt_ps_buffer.ga_len <= 0)
	प्रत्यागम;

    // Any background color must be drawn first
    यदि (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	पूर्णाङ्क     r, g, b;

	यदि (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_पंक्ति_height, 2);

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
    यदि (prt_do_underline)
    {
	यदि (prt_do_moveto)
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
    यदि (prt_out_mbyte)
	prt_write_string("<");
    अन्यथा
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    यदि (prt_out_mbyte)
	prt_write_string(">");
    अन्यथा
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    यदि (prt_do_moveto)
    {
	prt_write_real(prt_pos_x_moveto, 2);
	prt_write_real(prt_pos_y_moveto, 2);
	// moveto and a show
	prt_write_string("ms\n");
	prt_do_moveto = FALSE;
    }
    अन्यथा // Simple show
	prt_write_string("s\n");

    ga_clear(&prt_ps_buffer);
    ga_init2(&prt_ps_buffer, परिमाण(वर्ण), prt_bufsiz);
}


    स्थिर शून्य
prt_resource_name(char_u *filename, शून्य *cookie)
{
    char_u *resource_filename = cookie;

    यदि (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    अन्यथा
	STRCPY(resource_filename, filename);
}

    स्थिर पूर्णाङ्क
prt_find_resource(वर्ण *name, संरचना prt_ps_resource_S *resource)
{
    char_u	*buffer;
    पूर्णाङ्क		retval;

    buffer = alloc(MAXPATHL + 1);
    यदि (buffer == रिक्त)
	प्रत्यागम FALSE;

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
    प्रत्यागम retval;
}

// PS CR and LF characters have platform independent values
# define PSLF  (0x0a)
# define PSCR  (0x0d)

// Static buffer to read initial comments in a resource file, some can have a
// couple of KB of comments!
# define PRT_FILE_BUFFER_LEN (2048)
संरचना prt_resfile_buffer_S
{
    char_u  buffer[PRT_FILE_BUFFER_LEN];
    पूर्णाङ्क     len;
    पूर्णाङ्क     पंक्ति_start;
    पूर्णाङ्क     पंक्ति_end;
};

स्थिर संरचना prt_resfile_buffer_S prt_resfile;

    स्थिर पूर्णाङ्क
prt_resfile_next_पंक्ति(शून्य)
{
    पूर्णाङ्क     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.पंक्ति_end + 1;
    यावत् (idx < prt_resfile.len)
    {
	यदि (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    भङ्ग;
	idx++;
    }
    prt_resfile.पंक्ति_start = idx;

    यावत् (idx < prt_resfile.len)
    {
	यदि (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    भङ्ग;
	idx++;
    }
    prt_resfile.पंक्ति_end = idx;

    प्रत्यागम (idx < prt_resfile.len);
}

    स्थिर पूर्णाङ्क
prt_resfile_strncmp(पूर्णाङ्क offset, वर्ण *string, पूर्णाङ्क len)
{
    // Force not equal if string is longer than remainder of line
    यदि (len > (prt_resfile.पंक्ति_end - (prt_resfile.पंक्ति_start + offset)))
	प्रत्यागम 1;

    प्रत्यागम STRNCMP(&prt_resfile.buffer[prt_resfile.पंक्ति_start + offset],
								string, len);
}

    स्थिर पूर्णाङ्क
prt_resfile_skip_nonws(पूर्णाङ्क offset)
{
    पूर्णाङ्क     idx;

    idx = prt_resfile.पंक्ति_start + offset;
    यावत् (idx < prt_resfile.पंक्ति_end)
    {
	यदि (SAFE_isspace(prt_resfile.buffer[idx]))
	    प्रत्यागम idx - prt_resfile.पंक्ति_start;
	idx++;
    }
    प्रत्यागम -1;
}

    स्थिर पूर्णाङ्क
prt_resfile_skip_ws(पूर्णाङ्क offset)
{
    पूर्णाङ्क     idx;

    idx = prt_resfile.पंक्ति_start + offset;
    यावत् (idx < prt_resfile.पंक्ति_end)
    {
	यदि (!SAFE_isspace(prt_resfile.buffer[idx]))
	    प्रत्यागम idx - prt_resfile.पंक्ति_start;
	idx++;
    }
    प्रत्यागम -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    स्थिर पूर्णाङ्क
prt_next_dsc(संरचना prt_dsc_पंक्ति_S *p_dsc_पंक्ति)
{
    पूर्णाङ्क     comment;
    पूर्णाङ्क     offset;

    // Move to start of next line
    यदि (!prt_resfile_next_पंक्ति())
	प्रत्यागम FALSE;

    // DSC comments always start %%
    यदि (prt_resfile_strncmp(0, "%%", 2) != 0)
	प्रत्यागम FALSE;

    // Find type of DSC comment
    प्रति (comment = 0; comment < (पूर्णाङ्क)ARRAY_LENGTH(prt_dsc_table); comment++)
	यदि (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    भङ्ग;

    यदि (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_पंक्ति->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    अन्यथा
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_पंक्ति->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	यदि (offset == -1)
	    प्रत्यागम FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    यदि (offset == -1)
	प्रत्यागम FALSE;

    p_dsc_पंक्ति->string = &prt_resfile.buffer[prt_resfile.पंक्ति_start + offset];
    p_dsc_पंक्ति->len = prt_resfile.पंक्ति_end - (prt_resfile.पंक्ति_start + offset);

    प्रत्यागम TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    स्थिर पूर्णाङ्क
prt_open_resource(संरचना prt_ps_resource_S *resource)
{
    पूर्णाङ्क		offset;
    पूर्णाङ्क		seen_all;
    पूर्णाङ्क		seen_title;
    पूर्णाङ्क		seen_version;
    FILE	*fd_resource;
    संरचना prt_dsc_पंक्ति_S dsc_पंक्ति;

    fd_resource = mch_fopen((वर्ण *)resource->filename, READBIN);
    यदि (fd_resource == रिक्त)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	प्रत्यागम FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (पूर्णाङ्क)fread((वर्ण *)prt_resfile.buffer, परिमाण(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    यदि (ferror(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	fclose(fd_resource);
	प्रत्यागम FALSE;
    }
    fclose(fd_resource);

    prt_resfile.पंक्ति_end = -1;
    prt_resfile.पंक्ति_start = 0;
    यदि (!prt_resfile_next_पंक्ति())
	प्रत्यागम FALSE;

    offset = 0;

    यदि (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	प्रत्यागम FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    यदि (offset == -1)
	प्रत्यागम FALSE;
    offset = prt_resfile_skip_ws(offset);
    यदि (offset == -1)
	प्रत्यागम FALSE;

    यदि (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	प्रत्यागम FALSE;
    }
    offset += (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    यदि (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    अन्यथा यदि (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    अन्यथा यदि (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (पूर्णाङ्क)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    अन्यथा
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	प्रत्यागम FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    यावत् (!seen_all && prt_next_dsc(&dsc_पंक्ति))
    {
	विकल्प (dsc_पंक्ति.type)
	{
	पक्ष PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_पंक्ति.string, dsc_पंक्ति.len);
	    seen_title = TRUE;
	    यदि (seen_version)
		seen_all = TRUE;
	    भङ्ग;

	पक्ष PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_पंक्ति.string, dsc_पंक्ति.len);
	    seen_version = TRUE;
	    यदि (seen_title)
		seen_all = TRUE;
	    भङ्ग;

	पक्ष PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    भङ्ग;

	पक्ष PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    भङ्ग;
	}
    }

    यदि (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
}

    स्थिर पूर्णाङ्क
prt_check_resource(संरचना prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    यदि (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	प्रत्यागम FALSE;
    }

    // Other checks to be added as needed
    प्रत्यागम TRUE;
}

    स्थिर शून्य
prt_dsc_start(शून्य)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    स्थिर शून्य
prt_dsc_noarg(वर्ण *comment)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_पंक्ति_buffer);
}

    स्थिर शून्य
prt_dsc_textline(वर्ण *comment, वर्ण *text)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_पंक्ति_buffer);
}

    स्थिर शून्य
prt_dsc_text(वर्ण *comment, वर्ण *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_पंक्ति_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    स्थिर शून्य
prt_dsc_ints(वर्ण *comment, पूर्णाङ्क गिनती, पूर्णाङ्क *ints)
{
    पूर्णाङ्क		i;

    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_पंक्ति_buffer);

    प्रति (i = 0; i < गिनती; i++)
    {
	sprintf((वर्ण *)prt_पंक्ति_buffer, " %d", ints[i]);
	prt_write_file(prt_पंक्ति_buffer);
    }

    prt_write_string("\n");
}

    स्थिर शून्य
prt_dsc_resources(
    वर्ण	*comment,	// if NULL add to previous
    वर्ण	*type,
    वर्ण	*string)
{
    यदि (comment != रिक्त)
	vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
						 "%%%%%s: %s", comment, type);
    अन्यथा
	vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_पंक्ति_buffer);

    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
							     " %s\n", string);
    prt_write_file(prt_पंक्ति_buffer);
}

    स्थिर शून्य
prt_dsc_font_resource(वर्ण *resource, संरचना prt_ps_font_S *ps_font)
{
    पूर्णाङ्क     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    प्रति (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	यदि (ps_font->ps_fontname[i] != रिक्त)
	    prt_dsc_resources(रिक्त, "font", ps_font->ps_fontname[i]);
}

    स्थिर शून्य
prt_dsc_requirements(
    पूर्णाङ्क		duplex,
    पूर्णाङ्क		tumble,
    पूर्णाङ्क		collate,
    पूर्णाङ्क		color,
    पूर्णाङ्क		अंक_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    यदि (!(duplex || collate || color || (अंक_copies > 1)))
	प्रत्यागम;

    sprintf((वर्ण *)prt_पंक्ति_buffer, "%%%%Requirements:");
    prt_write_file(prt_पंक्ति_buffer);

    यदि (duplex)
    {
	prt_write_string(" duplex");
	यदि (tumble)
	    prt_write_string("(tumble)");
    }
    यदि (collate)
	prt_write_string(" collate");
    यदि (color)
	prt_write_string(" color");
    यदि (अंक_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	sprintf((वर्ण *)prt_पंक्ति_buffer, "%d", अंक_copies);
	prt_write_file(prt_पंक्ति_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    स्थिर शून्य
prt_dsc_docmedia(
    वर्ण	*paper_name,
    द्विगुण	width,
    द्विगुण	height,
    द्विगुण	weight,
    वर्ण	*colour,
    वर्ण	*type)
{
    vim_snprintf((वर्ण *)prt_पंक्ति_buffer, परिमाण(prt_पंक्ति_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_पंक्ति_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    यदि (colour == रिक्त)
	prt_write_string("()");
    अन्यथा
	prt_write_string(colour);
    prt_write_string(" ");
    यदि (type == रिक्त)
	prt_write_string("()");
    अन्यथा
	prt_write_string(type);
    prt_write_string("\n");
}

    शून्य
mch_print_cleanup(शून्य)
{
    यदि (prt_out_mbyte)
    {
	पूर्णाङ्क     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	प्रति (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    यदि (prt_ps_mb_font.ps_fontname[i] != रिक्त)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    यदि (prt_do_conv)
    {
	convert_setup(&prt_conv, रिक्त, रिक्त);
	prt_do_conv = FALSE;
    }
    यदि (prt_ps_fd != रिक्त)
    {
	fclose(prt_ps_fd);
	prt_ps_fd = रिक्त;
	prt_file_error = FALSE;
    }
    यदि (prt_ps_file_name != रिक्त)
	VIM_CLEAR(prt_ps_file_name);
}

    स्थिर भिन्न
to_device_units(पूर्णाङ्क idx, द्विगुण physsize, पूर्णाङ्क def_संख्या)
{
    भिन्न	ret;
    पूर्णाङ्क		u;
    पूर्णाङ्क		nr;

    u = prt_get_unit(idx);
    यदि (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_संख्या;
    }
    अन्यथा
	nr = printer_opts[idx].संख्या;

    विकल्प (u)
    {
	पक्ष PRT_UNIT_INCH:
	    ret = (भिन्न)(nr * PRT_PS_DEFAULT_DPI);
	    भङ्ग;
	पक्ष PRT_UNIT_MM:
	    ret = (भिन्न)(nr * PRT_PS_DEFAULT_DPI) / (भिन्न)25.4;
	    भङ्ग;
	पक्ष PRT_UNIT_POINT:
	    ret = (भिन्न)nr;
	    भङ्ग;
	पक्ष PRT_UNIT_PERC:
	स्वभाव:
	    ret = (भिन्न)(physsize * nr) / 100;
	    भङ्ग;
    }

    प्रत्यागम ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    स्थिर शून्य
prt_page_margins(
    द्विगुण	width,
    द्विगुण	height,
    द्विगुण	*बायाँ,
    द्विगुण	*दायाँ,
    द्विगुण	*शीर्ष,
    द्विगुण	*bottom)
{
    *बायाँ   = to_device_units(OPT_PRINT_बायाँ, width, 10);
    *दायाँ  = width - to_device_units(OPT_PRINT_दायाँ, width, 5);
    *शीर्ष    = height - to_device_units(OPT_PRINT_शीर्ष, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    स्थिर शून्य
prt_font_metrics(पूर्णाङ्क font_scale)
{
    prt_पंक्ति_height = (भिन्न)font_scale;
    prt_char_width = (भिन्न)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    स्थिर पूर्णाङ्क
prt_get_cpl(शून्य)
{
    यदि (prt_use_संख्या())
    {
	prt_संख्या_width = PRINT_संख्या_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	यदि (prt_out_mbyte)
	    prt_संख्या_width /= 2;
	prt_बायाँ_margin += prt_संख्या_width;
    }
    अन्यथा
	prt_संख्या_width = 0.0;

    प्रत्यागम (पूर्णाङ्क)((prt_दायाँ_margin - prt_बायाँ_margin) / prt_char_width);
}

    स्थिर पूर्णाङ्क
prt_build_cid_fontname(पूर्णाङ्क font, char_u *name, पूर्णाङ्क name_len)
{
    वर्ण    *fontname;

    fontname = alloc(name_len + 1);
    यदि (fontname == रिक्त)
	प्रत्यागम FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    प्रत्यागम TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    स्थिर पूर्णाङ्क
prt_get_lpp(शून्य)
{
    पूर्णाङ्क lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (भिन्न)PRT_PS_FONT_TO_USER(prt_पंक्ति_height,
					   prt_ps_font->bbox_min_y);
    यदि ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (भिन्न)PRT_PS_FONT_TO_USER(prt_पंक्ति_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_पंक्ति_height = prt_पंक्ति_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (पूर्णाङ्क)((prt_शीर्ष_margin - prt_bottom_margin) / prt_पंक्ति_height);

    // Adjust top margin if there is a header
    prt_शीर्ष_margin -= prt_पंक्ति_height * prt_header_height();

    प्रत्यागम lpp - prt_header_height();
}

    स्थिर पूर्णाङ्क
prt_match_encoding(
    वर्ण			*p_encoding,
    संरचना prt_ps_mbfont_S	*p_cmap,
    संरचना prt_ps_encoding_S	**pp_mbenc)
{
    पूर्णाङ्क				mbenc;
    पूर्णाङ्क				enc_len;
    संरचना prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = रिक्त;
    // Look for recognised encoding
    enc_len = (पूर्णाङ्क)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    प्रति (mbenc = 0; mbenc < p_cmap->अंक_encodings; mbenc++)
    {
	यदि (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    प्रत्यागम TRUE;
	}
	p_mbenc++;
    }
    प्रत्यागम FALSE;
}

    स्थिर पूर्णाङ्क
prt_match_charset(
    वर्ण		    *p_charset,
    संरचना prt_ps_mbfont_S  *p_cmap,
    संरचना prt_ps_charset_S **pp_mbchar)
{
    पूर्णाङ्क			    mbchar;
    पूर्णाङ्क			    char_len;
    संरचना prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    यदि (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (पूर्णाङ्क)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    प्रति (mbchar = 0; mbchar < p_cmap->अंक_charsets; mbchar++)
    {
	यदि (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    प्रत्यागम TRUE;
	}
	p_mbchar++;
    }
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    पूर्णाङ्क		forceit UNUSED)
{
    पूर्णाङ्क		i;
    वर्ण	*paper_name;
    पूर्णाङ्क		paper_strlen;
    पूर्णाङ्क		fontsize;
    char_u	*p;
    द्विगुण      बायाँ;
    द्विगुण      दायाँ;
    द्विगुण      शीर्ष;
    द्विगुण      bottom;
    पूर्णाङ्क		props;
    पूर्णाङ्क		cmap = 0;
    char_u	*p_encoding;
    संरचना prt_ps_encoding_S *p_mbenc;
    संरचना prt_ps_encoding_S *p_mbenc_first;
    संरचना prt_ps_charset_S  *p_mbchar = रिक्त;

# यदि 0
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
    यदि (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    यदि (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = रिक्त;
    props = enc_canon_props(p_encoding);
    यदि (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	पूर्णाङ्क cmap_first = 0;

	p_mbenc_first = रिक्त;
	प्रति (cmap = 0; cmap < (पूर्णाङ्क)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    यदि (prt_match_encoding((वर्ण *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		यदि (p_mbenc_first == रिक्त)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		यदि (prt_match_charset((वर्ण *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    भङ्ग;
	    }

	// Use first encoding matched if no charset matched
	यदि (p_mbchar == रिक्त && p_mbenc_first != रिक्त)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != रिक्त);
    यदि (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == रिक्त);
	यदि (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    यदि ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		प्रत्यागम FALSE;
	    }

	    // Add charset name if not empty
	    यदि (p_mbchar->cmap_charset != रिक्त)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, परिमाण(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	अन्यथा
	{
	    // Add custom CMap character set name
	    यदि (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		प्रत्यागम FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, परिमाण(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	यदि (p_mbenc->cmap_encoding != रिक्त && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < परिमाण(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	यदि (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    प्रत्यागम FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	यदि (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].strlen))
	    प्रत्यागम FALSE;
	यदि (mbfont_opts[OPT_MBFONT_BOLD].present)
	    यदि (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].strlen))
		प्रत्यागम FALSE;
	यदि (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    यदि (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].strlen))
		प्रत्यागम FALSE;
	यदि (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    यदि (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].strlen))
		प्रत्यागम FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	यदि (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    यदि (mbfont_opts[OPT_MBFONT_ASCII].present &&
		(TOLOWER_ASC(mbfont_opts[OPT_MBFONT_ASCII].string[0]) == 'y'))
		prt_ascii_encoding = "ascii";
	    अन्यथा
		prt_ascii_encoding = prt_ps_mbfonts[cmap].ascii_enc;
	}

	prt_ps_font = &prt_ps_mb_font;
    }
    अन्यथा
    {
	prt_use_courier = FALSE;
	prt_ps_font = &prt_ps_courier_font;
    }

    /*
     * Find the size of the paper and set the margins.
     */
    prt_portrait = (!printer_opts[OPT_PRINT_PORTRAIT].present
	   || TOLOWER_ASC(printer_opts[OPT_PRINT_PORTRAIT].string[0]) == 'y');
    यदि (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (वर्ण *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].strlen;
    }
    अन्यथा
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    प्रति (i = 0; i < (पूर्णाङ्क)PRT_MEDIASIZE_LEN; ++i)
	यदि (STRLEN(prt_mediasize[i].name) == (unsigned)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    भङ्ग;
    यदि (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    यदि (prt_portrait)
    {
	prt_page_width = prt_mediasize[i].width;
	prt_page_height = prt_mediasize[i].height;
    }
    अन्यथा
    {
	prt_page_width = prt_mediasize[i].height;
	prt_page_height = prt_mediasize[i].width;
    }

    /*
     * Set PS page margins based on the PS pagesize, not the mediasize - this
     * needs to be done before the cpl and lpp are calculated.
     */
    prt_page_margins(prt_page_width, prt_page_height, &बायाँ, &दायाँ, &शीर्ष,
								    &bottom);
    prt_बायाँ_margin = (भिन्न)बायाँ;
    prt_दायाँ_margin = (भिन्न)दायाँ;
    prt_शीर्ष_margin = (भिन्न)शीर्ष;
    prt_bottom_margin = (भिन्न)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    प्रति (p = p_pfn; (p = vim_strchr(p, ':')) != रिक्त; ++p)
	यदि (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = atoi((वर्ण *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_पंक्ति = prt_get_cpl();
    psettings->पंक्तियाँ_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    यदि (psettings->chars_per_पंक्ति <= 0 || psettings->पंक्तियाँ_per_page <= 0)
	प्रत्यागम FAIL;

    /*
     * Sort out the number of copies to be printed.  PS by default will do
     * uncollated copies for you, so once we know how many uncollated copies are
     * wanted cache it away and lie to the generic code that we only want one
     * uncollated copy.
     */
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;
    prt_अंक_copies = 1;
    prt_collate = (!printer_opts[OPT_PRINT_COLLATE].present
	    || TOLOWER_ASC(printer_opts[OPT_PRINT_COLLATE].string[0]) == 'y');
    यदि (prt_collate)
    {
	// TODO: Get number of collated copies wanted.
	psettings->n_collated_copies = 1;
    }
    अन्यथा
    {
	// TODO: Get number of uncollated copies wanted and update the cached
	// count.
	prt_अंक_copies = 1;
    }

    psettings->jobname = jobname;

    /*
     * Set up printer duplex and tumble based on Duplex option setting - default
     * is long sided duplex printing (i.e. no tumble).
     */
    prt_duplex = TRUE;
    prt_tumble = FALSE;
    psettings->duplex = 1;
    यदि (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	यदि (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	अन्यथा यदि (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    यदि (psettings->outfile == रिक्त)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	यदि (prt_ps_file_name == रिक्त)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    प्रत्यागम FAIL;
	}
	prt_ps_fd = mch_fopen((वर्ण *)prt_ps_file_name, WRITEBIN);
    }
    अन्यथा
    {
	p = expand_env_save(psettings->outfile);
	यदि (p != रिक्त)
	{
	    prt_ps_fd = mch_fopen((वर्ण *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    यदि (prt_ps_fd == रिक्त)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	प्रत्यागम FAIL;
    }

    prt_bufsiz = psettings->chars_per_पंक्ति;
    यदि (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, परिमाण(वर्ण), prt_bufsiz);

    prt_page_अंक = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
prt_add_resource(संरचना prt_ps_resource_S *resource)
{
    FILE*	fd_resource;
    char_u	resource_buffer[512];
    size_t	bytes_read;

    fd_resource = mch_fopen((वर्ण *)resource->filename, READBIN);
    यदि (fd_resource == रिक्त)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	प्रत्यागम FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (वर्ण *)resource->title);

    prt_dsc_textline("BeginDocument", (वर्ण *)resource->filename);

    प्रति (;;)
    {
	bytes_read = fread((वर्ण *)resource_buffer, परिमाण(char_u),
			   परिमाण(resource_buffer), fd_resource);
	यदि (ferror(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    fclose(fd_resource);
	    प्रत्यागम FALSE;
	}
	यदि (bytes_read == 0)
	    भङ्ग;
	prt_write_file_raw_len(resource_buffer, (पूर्णाङ्क)bytes_read);
	यदि (prt_file_error)
	{
	    fclose(fd_resource);
	    प्रत्यागम FALSE;
	}
    }
    fclose(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    प्रत्यागम TRUE;
}

    पूर्णाङ्क
mch_print_begin(prt_settings_T *psettings)
{
    पूर्णाङ्क		bbox[4];
    द्विगुण      बायाँ;
    द्विगुण      दायाँ;
    द्विगुण      शीर्ष;
    द्विगुण      bottom;
    संरचना prt_ps_resource_S *res_prolog;
    संरचना prt_ps_resource_S *res_encoding;
    वर्ण	buffer[256];
    char_u      *p_encoding;
    संरचना prt_ps_resource_S *res_cidfont;
    संरचना prt_ps_resource_S *res_cmap;
    पूर्णाङ्क		retval = FALSE;

    res_prolog = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_encoding = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_cmap = ALLOC_ONE(संरचना prt_ps_resource_S);
    यदि (res_prolog == रिक्त || res_encoding == रिक्त
	    || res_cidfont == रिक्त || res_cmap == रिक्त)
	गच्छ theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (वर्ण *)psettings->jobname);
    यदि (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(time(रिक्त), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &बायाँ, &दायाँ, &शीर्ष, &bottom);
    bbox[0] = (पूर्णाङ्क)बायाँ;
    यदि (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (पूर्णाङ्क)(शीर्ष - (psettings->पंक्तियाँ_per_page + prt_header_height())
						    * (द्विगुण)prt_पंक्ति_height);
	bbox[2] = (पूर्णाङ्क)(बायाँ + psettings->chars_per_पंक्ति
					       * (द्विगुण)prt_char_width + 0.5);
	bbox[3] = (पूर्णाङ्क)(शीर्ष + 0.5);
    }
    अन्यथा
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (पूर्णाङ्क)bottom;
	bbox[2] = (पूर्णाङ्क)(बायाँ + ((psettings->पंक्तियाँ_per_page
			      + prt_header_height()) * prt_पंक्ति_height) + 0.5);
	bbox[3] = (पूर्णाङ्क)(bottom + psettings->chars_per_पंक्ति
					       * (द्विगुण)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(द्विगुण)0, रिक्त, रिक्त);
    // Define fonts needed
    यदि (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    यदि (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? रिक्त
				: "DocumentNeededResources"), &prt_ps_mb_font);
	यदि (!prt_custom_cmap)
	    prt_dsc_resources(रिक्त, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    यदि (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	गच्छ theend;
    }
    यदि (!prt_open_resource(res_prolog))
	गच्छ theend;
    यदि (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	गच्छ theend;
    यदि (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	यदि (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    गच्छ theend;
	}
	यदि (!prt_open_resource(res_cidfont))
	    गच्छ theend;
	यदि (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    गच्छ theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    यदि (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	यदि (*p_encoding == NUL
		|| !prt_find_resource((वर्ण *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    पूर्णाङ्क		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    यदि (!(props & ENC_8BIT)
		    || !prt_find_resource((वर्ण *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		यदि (!prt_find_resource((वर्ण *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    गच्छ theend;
		}
	    }
	}
	यदि (!prt_open_resource(res_encoding))
	    गच्छ theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    अन्यथा
    {
	p_encoding = enc_skip(p_penc);
	यदि (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	यदि (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    यदि (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		गच्छ theend;
	    }
	    यदि (!prt_open_resource(res_encoding))
		गच्छ theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    यदि (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	यदि (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    गच्छ theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    यदि (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	यदि (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    गच्छ theend;
	}
	यदि (!prt_open_resource(res_cmap))
	    गच्छ theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    यदि (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(रिक्त, "procset", buffer);

	यदि (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(रिक्त, "cmap", buffer);
	}
    }
    यदि (!prt_out_mbyte || prt_use_courier)
    {
	STRCPY(buffer, res_encoding->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_encoding->version);
	prt_dsc_resources(रिक्त, "encoding", buffer);
    }
    prt_dsc_requirements(prt_duplex, prt_tumble, prt_collate,
# ifdef FEAT_SYN_HL
					psettings->do_syntax
# अन्यथा
					0
# endif
					, prt_अंक_copies);
    prt_dsc_noarg("EndComments");

    /*
     * PS Document page defaults
     */
    prt_dsc_noarg("BeginDefaults");

    // List font resources most likely common to all pages
    यदि (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    यदि (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? रिक्त : "PageResources"),
							    &prt_ps_mb_font);
	यदि (!prt_custom_cmap)
	    prt_dsc_resources(रिक्त, "cmap", prt_cmap);
    }

    // Paper will be used for all pages
    prt_dsc_textline("PageMedia", prt_mediasize[prt_media].name);

    prt_dsc_noarg("EndDefaults");

    /*
     * PS Document prolog inclusion - all required procsets.
     */
    prt_dsc_noarg("BeginProlog");

    // Add required procsets - NOTE: order is important!
    यदि (!prt_add_resource(res_prolog))
	गच्छ theend;
    यदि (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	यदि (!prt_add_resource(res_cidfont))
	    गच्छ theend;
	यदि (prt_custom_cmap && !prt_add_resource(res_cmap))
	    गच्छ theend;
    }

    यदि (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	यदि (!prt_add_resource(res_encoding))
	    गच्छ theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((पूर्णाङ्क)prt_mediasize[prt_media].width);
    prt_write_int((पूर्णाङ्क)prt_mediasize[prt_media].height);
    prt_write_int(0);
    prt_write_string("sps\n");
    prt_write_int(prt_अंक_copies);
    prt_write_string("nc\n");
    prt_write_boolean(prt_duplex);
    prt_write_boolean(prt_tumble);
    prt_write_string("dt\n");
    prt_write_boolean(prt_collate);
    prt_write_string("c\n");

    // Font resource inclusion and definition
    यदि (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	यदि (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (वर्ण *)p_encoding, (पूर्णाङ्क)prt_पंक्ति_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (वर्ण *)p_encoding, (पूर्णाङ्क)prt_पंक्ति_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (वर्ण *)p_encoding, (पूर्णाङ्क)prt_पंक्ति_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (वर्ण *)p_encoding, (पूर्णाङ्क)prt_पंक्ति_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    यदि (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	यदि (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (पूर्णाङ्क)prt_पंक्ति_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	यदि (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    यदि (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (पूर्णाङ्क)prt_पंक्ति_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	अन्यथा
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	यदि (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    यदि (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (पूर्णाङ्क)prt_पंक्ति_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	अन्यथा
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	यदि (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    यदि (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (पूर्णाङ्क)prt_पंक्ति_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	अन्यथा
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_पंक्ति_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_पंक्ति_height,
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

    प्रत्यागम retval;
}

    शून्य
mch_print_end(prt_settings_T *psettings)
{
    prt_dsc_noarg("Trailer");

    /*
     * Output any info we don't know in toto until we finish
     */
    prt_dsc_ints("Pages", 1, &prt_page_अंक);

    prt_dsc_noarg("EOF");

    // Write CTRL-D to close serial communication link if used.
    // NOTHING MUST BE WRITTEN AFTER THIS!
    prt_write_file((char_u *)"\004");

    यदि (!prt_file_error && psettings->outfile == रिक्त
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	यदि (prt_ps_fd != रिक्त)
	{
	    fclose(prt_ps_fd);
	    prt_ps_fd = रिक्त;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	यदि (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	अन्यथा
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    पूर्णाङ्क
mch_print_end_page(शून्य)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    प्रत्यागम !prt_file_error;
}

    पूर्णाङ्क
mch_print_begin_page(char_u *str UNUSED)
{
    पूर्णाङ्क		page_अंक[2];

    prt_page_अंक++;

    page_अंक[0] = page_अंक[1] = prt_page_अंक;
    prt_dsc_ints("Page", 2, page_अंक);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    यदि (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    अन्यथा
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    यदि (!prt_portrait)
    {
	prt_write_int(-((पूर्णाङ्क)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    प्रत्यागम !prt_file_error;
}

    पूर्णाङ्क
mch_print_blank_page(शून्य)
{
    प्रत्यागम (mch_print_begin_page(रिक्त) ? (mch_print_end_page()) : FALSE);
}

स्थिर भिन्न prt_pos_x = 0;
स्थिर भिन्न prt_pos_y = 0;

    शून्य
mch_print_start_पंक्ति(पूर्णाङ्क margin, पूर्णाङ्क page_पंक्ति)
{
    prt_pos_x = prt_बायाँ_margin;
    यदि (margin)
	prt_pos_x -= prt_संख्या_width;

    prt_pos_y = prt_शीर्ष_margin - prt_first_पंक्ति_height -
					page_पंक्ति * prt_पंक्ति_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    पूर्णाङ्क
mch_print_text_out(char_u *textp, पूर्णाङ्क len UNUSED)
{
    char_u	*p = textp;
    पूर्णाङ्क		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    भिन्न       char_width;
    भिन्न       next_pos;
    पूर्णाङ्क		in_ascii;
    पूर्णाङ्क		half_width;
    char_u	*tofree = रिक्त;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    यदि (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	यदि (prt_in_ascii)
	{
	    यदि (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	अन्यथा यदि (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    यदि (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	यदि (half_width)
	    char_width /= 2;
	यदि (prt_half_width)
	{
	    यदि (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	अन्यथा यदि (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    यदि (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	यदि (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	यदि (prt_need_font)
	{
	    यदि (!prt_in_ascii)
		prt_write_string("CF");
	    अन्यथा
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	यदि (prt_need_fgcol)
	{
	    पूर्णाङ्क     r, g, b;
	    r = ((unsigned)prt_fgcol & 0xff0000) >> 16;
	    g = ((unsigned)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    यदि (r == g && g == b)
		prt_write_string("g\n");
	    अन्यथा
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	यदि (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    यदि (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	अन्यथा
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	यदि (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    यदि (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	यदि (p == रिक्त)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    यदि (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	यावत् (len-- > 0)
	{
	    ch = prt_hexchar[(unsigned)(*p) >> 4];
	    ga_append(&prt_ps_buffer, ch);
	    ch = prt_hexchar[(*p) & 0xf];
	    ga_append(&prt_ps_buffer, ch);
	    p++;
	}
    }
    अन्यथा
    {
	// Add next character to buffer of characters to output.
	// Note: One printed character may require several PS characters to
	// represent it, but we only count them as one printed character.
	ch = *p;
	यदि (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    विकल्प (ch)
	    {
		पक्ष BS:   ga_append(&prt_ps_buffer, 'b'); भङ्ग;
		पक्ष TAB:  ga_append(&prt_ps_buffer, 't'); भङ्ग;
		पक्ष NL:   ga_append(&prt_ps_buffer, 'n'); भङ्ग;
		पक्ष FF:   ga_append(&prt_ps_buffer, 'f'); भङ्ग;
		पक्ष CAR:  ga_append(&prt_ps_buffer, 'r'); भङ्ग;
		पक्ष '(':  ga_append(&prt_ps_buffer, '('); भङ्ग;
		पक्ष ')':  ga_append(&prt_ps_buffer, ')'); भङ्ग;
		पक्ष '\\': ga_append(&prt_ps_buffer, '\\'); भङ्ग;

		स्वभाव:
			   sprintf((वर्ण *)ch_buff, "%03o", (unsigned पूर्णाङ्क)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   भङ्ग;
	    }
	}
	अन्यथा
	    ga_append(&prt_ps_buffer, ch);
    }

    // Need to free any translated characters
    vim_free(tofree);

    prt_text_run += char_width;
    prt_pos_x += char_width;

    // The downside of fp - use relative error on right margin check
    next_pos = prt_pos_x + prt_char_width;
    need_break = (next_pos > prt_दायाँ_margin) &&
		    ((next_pos - prt_दायाँ_margin) > (prt_दायाँ_margin*1e-5));

    यदि (need_break)
	prt_flush_buffer();

    प्रत्यागम need_break;
}

    शून्य
mch_print_set_font(पूर्णाङ्क iBold, पूर्णाङ्क iItalic, पूर्णाङ्क iUnderline)
{
    पूर्णाङ्क		font = 0;

    यदि (iBold)
	font |= 0x01;
    यदि (iItalic)
	font |= 0x02;

    यदि (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    यदि (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    शून्य
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (पूर्णाङ्क)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    शून्य
mch_print_set_fg(long_u fgcol)
{
    यदि (fgcol == (long_u)prt_fgcol)
	प्रत्यागम;

    prt_fgcol = (पूर्णाङ्क)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
