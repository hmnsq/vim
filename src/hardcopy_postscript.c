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
    पूर्णांक		wx;
    पूर्णांक		uline_offset;
    पूर्णांक		uline_width;
    पूर्णांक		bbox_min_y;
    पूर्णांक		bbox_max_y;
    अक्षर	*(ps_fontname[4]);
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
    अक्षर	*encoding;
    अक्षर	*cmap_encoding;
    पूर्णांक		needs_charset;
};

संरचना prt_ps_charset_S
{
    अक्षर	*charset;
    अक्षर	*cmap_charset;
    पूर्णांक		has_charset;
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
    पूर्णांक				अंक_encodings;
    संरचना prt_ps_encoding_S	*encodings;
    पूर्णांक				अंक_charsets;
    संरचना prt_ps_charset_S	*charsets;
    अक्षर			*ascii_enc;
    अक्षर			*defcs;
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
    पूर्णांक     type;
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
स्थिर अक्षर *prt_resource_types[] =
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
    अक्षर	*string;
    पूर्णांक		len;
    पूर्णांक		type;
};

संरचना prt_dsc_ओळ_S
{
    पूर्णांक		type;
    char_u	*string;
    पूर्णांक		len;
};


# define SIZEOF_CSTR(s)      (आकार(s) - 1)
स्थिर संरचना prt_dsc_comment_S prt_dsc_table[] =
{
    {PRT_DSC_TITLE,       SIZEOF_CSTR(PRT_DSC_TITLE),     PRT_DSC_TITLE_TYPE},
    {PRT_DSC_VERSION,     SIZEOF_CSTR(PRT_DSC_VERSION),
							PRT_DSC_VERSION_TYPE},
    {PRT_DSC_ENDCOMMENTS, SIZEOF_CSTR(PRT_DSC_ENDCOMMENTS),
						     PRT_DSC_ENDCOMMENTS_TYPE}
};

स्थिर शून्य prt_write_file_len(char_u *buffer, पूर्णांक bytes);
स्थिर पूर्णांक prt_next_dsc(संरचना prt_dsc_ओळ_S *p_dsc_ओळ);

/*
 * Variables for the output PostScript file.
 */
स्थिर खाता *prt_ps_fd;
स्थिर पूर्णांक prt_file_error;
स्थिर char_u *prt_ps_file_name = रिक्त;

/*
 * Various offsets and dimensions in default PostScript user space (points).
 * Used for text positioning calculations
 */
स्थिर दशांश prt_page_width;
स्थिर दशांश prt_page_height;
स्थिर दशांश prt_डावा_margin;
स्थिर दशांश prt_उजवा_margin;
स्थिर दशांश prt_शीर्ष_margin;
स्थिर दशांश prt_bottom_margin;
स्थिर दशांश prt_ओळ_height;
स्थिर दशांश prt_first_ओळ_height;
स्थिर दशांश prt_char_width;
स्थिर दशांश prt_संख्या_width;
स्थिर दशांश prt_bgcol_offset;
स्थिर दशांश prt_pos_x_moveto = 0.0;
स्थिर दशांश prt_pos_y_moveto = 0.0;

/*
 * Various control variables used to decide when and how to change the
 * PostScript graphics state.
 */
स्थिर पूर्णांक prt_need_moveto;
स्थिर पूर्णांक prt_do_moveto;
स्थिर पूर्णांक prt_need_font;
स्थिर पूर्णांक prt_font;
स्थिर पूर्णांक prt_need_underline;
स्थिर पूर्णांक prt_underline;
स्थिर पूर्णांक prt_do_underline;
स्थिर पूर्णांक prt_need_fgcol;
स्थिर पूर्णांक prt_fgcol;
स्थिर पूर्णांक prt_need_bgcol;
स्थिर पूर्णांक prt_do_bgcol;
स्थिर पूर्णांक prt_bgcol;
स्थिर पूर्णांक prt_new_bgcol;
स्थिर पूर्णांक prt_attribute_change;
स्थिर दशांश prt_text_run;
स्थिर पूर्णांक prt_page_अंक;
स्थिर पूर्णांक prt_bufsiz;

/*
 * Variables controlling physical printing.
 */
स्थिर पूर्णांक prt_media;
स्थिर पूर्णांक prt_portrait;
स्थिर पूर्णांक prt_अंक_copies;
स्थिर पूर्णांक prt_duplex;
स्थिर पूर्णांक prt_tumble;
स्थिर पूर्णांक prt_collate;

/*
 * Buffers used when generating PostScript output
 */
स्थिर char_u prt_ओळ_buffer[257];
स्थिर garray_T prt_ps_buffer;

स्थिर पूर्णांक prt_do_conv;
स्थिर vimconv_T prt_conv;

स्थिर पूर्णांक prt_out_mbyte;
स्थिर पूर्णांक prt_custom_cmap;
स्थिर अक्षर prt_cmap[80];
स्थिर पूर्णांक prt_use_courier;
स्थिर पूर्णांक prt_in_ascii;
स्थिर पूर्णांक prt_half_width;
स्थिर अक्षर *prt_ascii_encoding;
स्थिर स्थिरांक char_u prt_hexchar[] = "0123456789abcdef";

    स्थिर शून्य
prt_write_file_raw_len(char_u *buffer, पूर्णांक bytes)
{
    जर (!prt_file_error
	    && ख_डालो(buffer, आकार(char_u), bytes, prt_ps_fd)
							     != (माप_प्रकार)bytes)
    {
	emsg(_(e_error_writing_to_postscript_output_file));
	prt_file_error = TRUE;
    }
}

    स्थिर शून्य
prt_write_file(char_u *buffer)
{
    prt_write_file_len(buffer, (पूर्णांक)STRLEN(buffer));
}

    स्थिर शून्य
prt_write_file_len(char_u *buffer, पूर्णांक bytes)
{
    prt_write_file_raw_len(buffer, bytes);
}

/*
 * Write a string.
 */
    स्थिर शून्य
prt_write_string(अक्षर *s)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer), "%s", s);
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write an int and a space.
 */
    स्थिर शून्य
prt_write_int(पूर्णांक i)
{
    प्र_लिखो((अक्षर *)prt_ओळ_buffer, "%d ", i);
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write a boolean and a space.
 */
    स्थिर शून्य
prt_write_boolean(पूर्णांक b)
{
    प्र_लिखो((अक्षर *)prt_ओळ_buffer, "%s ", (b ? "T" : "F"));
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write PostScript to re-encode and define the font.
 */
    स्थिर शून्य
prt_def_font(
    अक्षर	*new_name,
    अक्षर	*encoding,
    पूर्णांक		height,
    अक्षर	*font)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
			  "/_%s /VIM-%s /%s ref\n", new_name, encoding, font);
    prt_write_file(prt_ओळ_buffer);
    जर (prt_out_mbyte)
	प्र_लिखो((अक्षर *)prt_ओळ_buffer, "/%s %d %f /_%s sffs\n",
		       new_name, height, 500./prt_ps_courier_font.wx, new_name);
    अन्यथा
	vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write a line to define the CID font.
 */
    स्थिर शून्य
prt_def_cidfont(अक्षर *new_name, पूर्णांक height, अक्षर *cidfont)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
	      "/_%s /%s[/%s] vim_composefont\n", new_name, prt_cmap, cidfont);
    prt_write_file(prt_ओळ_buffer);
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
			     "/%s %d /_%s ffs\n", new_name, height, new_name);
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write a line to define a duplicate of a CID font
 */
    स्थिर शून्य
prt_dup_cidfont(अक्षर *original_name, अक्षर *new_name)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
				       "/%s %s d\n", new_name, original_name);
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Convert a real value into an integer and fractional part as integers, with
 * the fractional part being in the range [0,10^precision).  The fractional part
 * is also rounded based on the precision + 1'th fractional digit.
 */
    स्थिर शून्य
prt_real_bits(
    द्विगुण      real,
    पूर्णांक		precision,
    पूर्णांक		*pinteger,
    पूर्णांक		*pfraction)
{
    पूर्णांक     i;
    पूर्णांक     integer;
    दशांश   fraction;

    integer = (पूर्णांक)real;
    fraction = (दशांश)(real - integer);
    जर (real < (द्विगुण)integer)
	fraction = -fraction;
    साठी (i = 0; i < precision; i++)
	fraction *= 10.0;

    *pinteger = integer;
    *pfraction = (पूर्णांक)(fraction + 0.5);
}

/*
 * Write a real and a space.  Save bytes if real value has no fractional part!
 * We use prt_real_bits() as %f in sprintf uses the locale setting to decide
 * what decimal point character to use, but PS always requires a '.'.
 */
    स्थिर शून्य
prt_write_real(द्विगुण val, पूर्णांक prec)
{
    पूर्णांक     integer;
    पूर्णांक     fraction;

    prt_real_bits(val, prec, &integer, &fraction);
    // Emit integer part
    प्र_लिखो((अक्षर *)prt_ओळ_buffer, "%d", integer);
    prt_write_file(prt_ओळ_buffer);
    // Only emit fraction if necessary
    जर (fraction != 0)
    {
	// Remove any trailing zeros
	पर्यंत ((fraction % 10) == 0)
	{
	    prec--;
	    fraction /= 10;
	}
	// Emit fraction left padded with zeros
	प्र_लिखो((अक्षर *)prt_ओळ_buffer, ".%0*d", prec, fraction);
	prt_write_file(prt_ओळ_buffer);
    }
    प्र_लिखो((अक्षर *)prt_ओळ_buffer, " ");
    prt_write_file(prt_ओळ_buffer);
}

/*
 * Write a line to define a numeric variable.
 */
    स्थिर शून्य
prt_def_var(अक्षर *name, द्विगुण मूल्य, पूर्णांक prec)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
								"/%s ", name);
    prt_write_file(prt_ओळ_buffer);
    prt_write_real(मूल्य, prec);
    प्र_लिखो((अक्षर *)prt_ओळ_buffer, "d\n");
    prt_write_file(prt_ओळ_buffer);
}

// Convert size from font space to user space at current font scale
# define PRT_PS_FONT_TO_USER(scale, size)    ((size) * ((scale)/1000.0))

    स्थिर शून्य
prt_flush_buffer(शून्य)
{
    जर (prt_ps_buffer.ga_len <= 0)
	परत;

    // Any background color must be drawn first
    जर (prt_do_bgcol && (prt_new_bgcol != PRCOLOR_WHITE))
    {
	पूर्णांक     r, g, b;

	जर (prt_do_moveto)
	{
	    prt_write_real(prt_pos_x_moveto, 2);
	    prt_write_real(prt_pos_y_moveto, 2);
	    prt_write_string("m\n");
	    prt_do_moveto = FALSE;
	}

	// Size of rect of background color on which text is printed
	prt_write_real(prt_text_run, 2);
	prt_write_real(prt_ओळ_height, 2);

	// Lastly add the color of the background
	r = ((अचिन्हित)prt_new_bgcol & 0xff0000) >> 16;
	g = ((अचिन्हित)prt_new_bgcol & 0xff00) >> 8;
	b = prt_new_bgcol & 0xff;
	prt_write_real(r / 255.0, 3);
	prt_write_real(g / 255.0, 3);
	prt_write_real(b / 255.0, 3);
	prt_write_string("bg\n");
    }
    // Draw underlines before the text as it makes it slightly easier to
    // find the starting point.
    जर (prt_do_underline)
    {
	जर (prt_do_moveto)
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
    जर (prt_out_mbyte)
	prt_write_string("<");
    अन्यथा
	prt_write_string("(");
    prt_write_file_raw_len(prt_ps_buffer.ga_data, prt_ps_buffer.ga_len);
    जर (prt_out_mbyte)
	prt_write_string(">");
    अन्यथा
	prt_write_string(")");
    // Add a moveto if need be and use the appropriate show procedure
    जर (prt_do_moveto)
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
    ga_init2(&prt_ps_buffer, आकार(अक्षर), prt_bufsiz);
}


    स्थिर शून्य
prt_resource_name(char_u *filename, शून्य *cookie)
{
    char_u *resource_filename = cookie;

    जर (STRLEN(filename) >= MAXPATHL)
	*resource_filename = NUL;
    अन्यथा
	STRCPY(resource_filename, filename);
}

    स्थिर पूर्णांक
prt_find_resource(अक्षर *name, संरचना prt_ps_resource_S *resource)
{
    char_u	*buffer;
    पूर्णांक		retval;

    buffer = alloc(MAXPATHL + 1);
    जर (buffer == रिक्त)
	परत FALSE;

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
    परत retval;
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
    पूर्णांक     len;
    पूर्णांक     ओळ_start;
    पूर्णांक     ओळ_end;
};

स्थिर संरचना prt_resfile_buffer_S prt_resfile;

    स्थिर पूर्णांक
prt_resfile_next_ओळ(शून्य)
{
    पूर्णांक     idx;

    // Move to start of next line and then find end of line
    idx = prt_resfile.ओळ_end + 1;
    पर्यंत (idx < prt_resfile.len)
    {
	जर (prt_resfile.buffer[idx] != PSLF && prt_resfile.buffer[idx] != PSCR)
	    थांबा;
	idx++;
    }
    prt_resfile.ओळ_start = idx;

    पर्यंत (idx < prt_resfile.len)
    {
	जर (prt_resfile.buffer[idx] == PSLF || prt_resfile.buffer[idx] == PSCR)
	    थांबा;
	idx++;
    }
    prt_resfile.ओळ_end = idx;

    परत (idx < prt_resfile.len);
}

    स्थिर पूर्णांक
prt_resfile_strncmp(पूर्णांक offset, अक्षर *string, पूर्णांक len)
{
    // Force not equal if string is longer than remainder of line
    जर (len > (prt_resfile.ओळ_end - (prt_resfile.ओळ_start + offset)))
	परत 1;

    परत STRNCMP(&prt_resfile.buffer[prt_resfile.ओळ_start + offset],
								string, len);
}

    स्थिर पूर्णांक
prt_resfile_skip_nonws(पूर्णांक offset)
{
    पूर्णांक     idx;

    idx = prt_resfile.ओळ_start + offset;
    पर्यंत (idx < prt_resfile.ओळ_end)
    {
	जर (SAFE_isspace(prt_resfile.buffer[idx]))
	    परत idx - prt_resfile.ओळ_start;
	idx++;
    }
    परत -1;
}

    स्थिर पूर्णांक
prt_resfile_skip_ws(पूर्णांक offset)
{
    पूर्णांक     idx;

    idx = prt_resfile.ओळ_start + offset;
    पर्यंत (idx < prt_resfile.ओळ_end)
    {
	जर (!SAFE_isspace(prt_resfile.buffer[idx]))
	    परत idx - prt_resfile.ओळ_start;
	idx++;
    }
    परत -1;
}

// prt_next_dsc() - returns detail on next DSC comment line found.  Returns true
// if a DSC comment is found, else false
    स्थिर पूर्णांक
prt_next_dsc(संरचना prt_dsc_ओळ_S *p_dsc_ओळ)
{
    पूर्णांक     comment;
    पूर्णांक     offset;

    // Move to start of next line
    जर (!prt_resfile_next_ओळ())
	परत FALSE;

    // DSC comments always start %%
    जर (prt_resfile_strncmp(0, "%%", 2) != 0)
	परत FALSE;

    // Find type of DSC comment
    साठी (comment = 0; comment < (पूर्णांक)ARRAY_LENGTH(prt_dsc_table); comment++)
	जर (prt_resfile_strncmp(0, prt_dsc_table[comment].string,
					    prt_dsc_table[comment].len) == 0)
	    थांबा;

    जर (comment != ARRAY_LENGTH(prt_dsc_table))
    {
	// Return type of comment
	p_dsc_ओळ->type = prt_dsc_table[comment].type;
	offset = prt_dsc_table[comment].len;
    }
    अन्यथा
    {
	// Unrecognised DSC comment, skip to ws after comment leader
	p_dsc_ओळ->type = PRT_DSC_MISC_TYPE;
	offset = prt_resfile_skip_nonws(0);
	जर (offset == -1)
	    परत FALSE;
    }

    // Skip ws to comment value
    offset = prt_resfile_skip_ws(offset);
    जर (offset == -1)
	परत FALSE;

    p_dsc_ओळ->string = &prt_resfile.buffer[prt_resfile.ओळ_start + offset];
    p_dsc_ओळ->len = prt_resfile.ओळ_end - (prt_resfile.ओळ_start + offset);

    परत TRUE;
}

/*
 * Improved hand crafted parser to get the type, title, and version number of a
 * PS resource file so the file details can be added to the DSC header comments.
 */
    स्थिर पूर्णांक
prt_open_resource(संरचना prt_ps_resource_S *resource)
{
    पूर्णांक		offset;
    पूर्णांक		seen_all;
    पूर्णांक		seen_title;
    पूर्णांक		seen_version;
    खाता	*fd_resource;
    संरचना prt_dsc_ओळ_S dsc_ओळ;

    fd_resource = mch_fopen((अक्षर *)resource->filename, READBIN);
    जर (fd_resource == रिक्त)
    {
	semsg(_(e_cant_open_file_str_3), resource->filename);
	परत FALSE;
    }
    CLEAR_FIELD(prt_resfile.buffer);

    // Parse first line to ensure valid resource file
    prt_resfile.len = (पूर्णांक)ख_पढ़ो((अक्षर *)prt_resfile.buffer, आकार(char_u),
					    PRT_FILE_BUFFER_LEN, fd_resource);
    जर (ख_त्रुटि(fd_resource))
    {
	semsg(_(e_cant_read_postscript_resource_file_str),
		resource->filename);
	ख_बंद(fd_resource);
	परत FALSE;
    }
    ख_बंद(fd_resource);

    prt_resfile.ओळ_end = -1;
    prt_resfile.ओळ_start = 0;
    जर (!prt_resfile_next_ओळ())
	परत FALSE;

    offset = 0;

    जर (prt_resfile_strncmp(offset, PRT_RESOURCE_HEADER,
			       (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_HEADER)) != 0)
    {
	semsg(_(e_file_str_is_not_postscript_resource_file),
							   resource->filename);
	परत FALSE;
    }

    // Skip over any version numbers and following ws
    offset += (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_HEADER);
    offset = prt_resfile_skip_nonws(offset);
    जर (offset == -1)
	परत FALSE;
    offset = prt_resfile_skip_ws(offset);
    जर (offset == -1)
	परत FALSE;

    जर (prt_resfile_strncmp(offset, PRT_RESOURCE_RESOURCE,
			     (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE)) != 0)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	परत FALSE;
    }
    offset += (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_RESOURCE);

    // Decide type of resource in the file
    जर (prt_resfile_strncmp(offset, PRT_RESOURCE_PROCSET,
			      (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_PROCSET)) == 0)
	resource->type = PRT_RESOURCE_TYPE_PROCSET;
    अन्यथा जर (prt_resfile_strncmp(offset, PRT_RESOURCE_ENCODING,
			     (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_ENCODING)) == 0)
	resource->type = PRT_RESOURCE_TYPE_ENCODING;
    अन्यथा जर (prt_resfile_strncmp(offset, PRT_RESOURCE_CMAP,
				 (पूर्णांक)STRLEN_LITERAL(PRT_RESOURCE_CMAP)) == 0)
	resource->type = PRT_RESOURCE_TYPE_CMAP;
    अन्यथा
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	परत FALSE;
    }

    // Look for title and version of resource
    resource->title[0] = '\0';
    resource->version[0] = '\0';
    seen_title = FALSE;
    seen_version = FALSE;
    seen_all = FALSE;
    पर्यंत (!seen_all && prt_next_dsc(&dsc_ओळ))
    {
	निवड (dsc_ओळ.type)
	{
	स्थिती PRT_DSC_TITLE_TYPE:
	    vim_strncpy(resource->title, dsc_ओळ.string, dsc_ओळ.len);
	    seen_title = TRUE;
	    जर (seen_version)
		seen_all = TRUE;
	    थांबा;

	स्थिती PRT_DSC_VERSION_TYPE:
	    vim_strncpy(resource->version, dsc_ओळ.string, dsc_ओळ.len);
	    seen_version = TRUE;
	    जर (seen_title)
		seen_all = TRUE;
	    थांबा;

	स्थिती PRT_DSC_ENDCOMMENTS_TYPE:
	    // Won't find title or resource after this comment, stop searching
	    seen_all = TRUE;
	    थांबा;

	स्थिती PRT_DSC_MISC_TYPE:
	    // Not interested in whatever comment this line had
	    थांबा;
	}
    }

    जर (!seen_title || !seen_version)
    {
	semsg(_(e_file_str_is_not_supported_postscript_resource_file),
							   resource->filename);
	परत FALSE;
    }

    परत TRUE;
}

    स्थिर पूर्णांक
prt_check_resource(संरचना prt_ps_resource_S *resource, char_u *version)
{
    // Version number m.n should match, the revision number does not matter
    जर (STRNCMP(resource->version, version, STRLEN(version)) != 0)
    {
	semsg(_(e_str_resource_file_has_wrong_version),
		resource->name);
	परत FALSE;
    }

    // Other checks to be added as needed
    परत TRUE;
}

    स्थिर शून्य
prt_dsc_start(शून्य)
{
    prt_write_string("%!PS-Adobe-3.0\n");
}

    स्थिर शून्य
prt_dsc_noarg(अक्षर *comment)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
							 "%%%%%s\n", comment);
    prt_write_file(prt_ओळ_buffer);
}

    स्थिर शून्य
prt_dsc_textline(अक्षर *comment, अक्षर *text)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
					       "%%%%%s: %s\n", comment, text);
    prt_write_file(prt_ओळ_buffer);
}

    स्थिर शून्य
prt_dsc_text(अक्षर *comment, अक्षर *text)
{
    // TODO - should scan 'text' for any chars needing escaping!
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
					     "%%%%%s: (%s)\n", comment, text);
    prt_write_file(prt_ओळ_buffer);
}

# define prt_dsc_atend(c)	prt_dsc_text((c), "atend")

    स्थिर शून्य
prt_dsc_ints(अक्षर *comment, पूर्णांक मोजणी, पूर्णांक *ints)
{
    पूर्णांक		i;

    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
							  "%%%%%s:", comment);
    prt_write_file(prt_ओळ_buffer);

    साठी (i = 0; i < मोजणी; i++)
    {
	प्र_लिखो((अक्षर *)prt_ओळ_buffer, " %d", ints[i]);
	prt_write_file(prt_ओळ_buffer);
    }

    prt_write_string("\n");
}

    स्थिर शून्य
prt_dsc_resources(
    अक्षर	*comment,	// if NULL add to previous
    अक्षर	*type,
    अक्षर	*string)
{
    जर (comment != रिक्त)
	vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
						 "%%%%%s: %s", comment, type);
    अन्यथा
	vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
							    "%%%%+ %s", type);
    prt_write_file(prt_ओळ_buffer);

    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
							     " %s\n", string);
    prt_write_file(prt_ओळ_buffer);
}

    स्थिर शून्य
prt_dsc_font_resource(अक्षर *resource, संरचना prt_ps_font_S *ps_font)
{
    पूर्णांक     i;

    prt_dsc_resources(resource, "font",
				    ps_font->ps_fontname[PRT_PS_FONT_ROMAN]);
    साठी (i = PRT_PS_FONT_BOLD ; i <= PRT_PS_FONT_BOLDOBLIQUE ; i++)
	जर (ps_font->ps_fontname[i] != रिक्त)
	    prt_dsc_resources(रिक्त, "font", ps_font->ps_fontname[i]);
}

    स्थिर शून्य
prt_dsc_requirements(
    पूर्णांक		duplex,
    पूर्णांक		tumble,
    पूर्णांक		collate,
    पूर्णांक		color,
    पूर्णांक		अंक_copies)
{
    // Only output the comment if we need to.
    // Note: tumble is ignored if we are not duplexing
    जर (!(duplex || collate || color || (अंक_copies > 1)))
	परत;

    प्र_लिखो((अक्षर *)prt_ओळ_buffer, "%%%%Requirements:");
    prt_write_file(prt_ओळ_buffer);

    जर (duplex)
    {
	prt_write_string(" duplex");
	जर (tumble)
	    prt_write_string("(tumble)");
    }
    जर (collate)
	prt_write_string(" collate");
    जर (color)
	prt_write_string(" color");
    जर (अंक_copies > 1)
    {
	prt_write_string(" numcopies(");
	// Note: no space wanted so don't use prt_write_int()
	प्र_लिखो((अक्षर *)prt_ओळ_buffer, "%d", अंक_copies);
	prt_write_file(prt_ओळ_buffer);
	prt_write_string(")");
    }
    prt_write_string("\n");
}

    स्थिर शून्य
prt_dsc_docmedia(
    अक्षर	*paper_name,
    द्विगुण	width,
    द्विगुण	height,
    द्विगुण	weight,
    अक्षर	*colour,
    अक्षर	*type)
{
    vim_snprintf((अक्षर *)prt_ओळ_buffer, आकार(prt_ओळ_buffer),
					"%%%%DocumentMedia: %s ", paper_name);
    prt_write_file(prt_ओळ_buffer);
    prt_write_real(width, 2);
    prt_write_real(height, 2);
    prt_write_real(weight, 2);
    जर (colour == रिक्त)
	prt_write_string("()");
    अन्यथा
	prt_write_string(colour);
    prt_write_string(" ");
    जर (type == रिक्त)
	prt_write_string("()");
    अन्यथा
	prt_write_string(type);
    prt_write_string("\n");
}

    शून्य
mch_print_cleanup(शून्य)
{
    जर (prt_out_mbyte)
    {
	पूर्णांक     i;

	// Free off all CID font names created, but first clear duplicate
	// pointers to the same string (when the same font is used for more than
	// one style).
	साठी (i = PRT_PS_FONT_ROMAN; i <= PRT_PS_FONT_BOLDOBLIQUE; i++)
	{
	    जर (prt_ps_mb_font.ps_fontname[i] != रिक्त)
		VIM_CLEAR(prt_ps_mb_font.ps_fontname[i]);
	}
    }

    जर (prt_do_conv)
    {
	convert_setup(&prt_conv, रिक्त, रिक्त);
	prt_do_conv = FALSE;
    }
    जर (prt_ps_fd != रिक्त)
    {
	ख_बंद(prt_ps_fd);
	prt_ps_fd = रिक्त;
	prt_file_error = FALSE;
    }
    जर (prt_ps_file_name != रिक्त)
	VIM_CLEAR(prt_ps_file_name);
}

    स्थिर दशांश
to_device_units(पूर्णांक idx, द्विगुण physsize, पूर्णांक def_संख्या)
{
    दशांश	ret;
    पूर्णांक		u;
    पूर्णांक		nr;

    u = prt_get_unit(idx);
    जर (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_संख्या;
    }
    अन्यथा
	nr = printer_opts[idx].संख्या;

    निवड (u)
    {
	स्थिती PRT_UNIT_INCH:
	    ret = (दशांश)(nr * PRT_PS_DEFAULT_DPI);
	    थांबा;
	स्थिती PRT_UNIT_MM:
	    ret = (दशांश)(nr * PRT_PS_DEFAULT_DPI) / (दशांश)25.4;
	    थांबा;
	स्थिती PRT_UNIT_POINT:
	    ret = (दशांश)nr;
	    थांबा;
	स्थिती PRT_UNIT_PERC:
	पूर्वनिर्धारित:
	    ret = (दशांश)(physsize * nr) / 100;
	    थांबा;
    }

    परत ret;
}

/*
 * Calculate margins for given width and height from printoptions settings.
 */
    स्थिर शून्य
prt_page_margins(
    द्विगुण	width,
    द्विगुण	height,
    द्विगुण	*डावा,
    द्विगुण	*उजवा,
    द्विगुण	*शीर्ष,
    द्विगुण	*bottom)
{
    *डावा   = to_device_units(OPT_PRINT_डावा, width, 10);
    *उजवा  = width - to_device_units(OPT_PRINT_उजवा, width, 5);
    *शीर्ष    = height - to_device_units(OPT_PRINT_शीर्ष, height, 5);
    *bottom = to_device_units(OPT_PRINT_BOT, height, 5);
}

    स्थिर शून्य
prt_font_metrics(पूर्णांक font_scale)
{
    prt_ओळ_height = (दशांश)font_scale;
    prt_char_width = (दशांश)PRT_PS_FONT_TO_USER(font_scale, prt_ps_font->wx);
}


    स्थिर पूर्णांक
prt_get_cpl(शून्य)
{
    जर (prt_use_संख्या())
    {
	prt_संख्या_width = PRINT_संख्या_WIDTH * prt_char_width;
	// If we are outputting multi-byte characters then line numbers will be
	// printed with half width characters
	जर (prt_out_mbyte)
	    prt_संख्या_width /= 2;
	prt_डावा_margin += prt_संख्या_width;
    }
    अन्यथा
	prt_संख्या_width = 0.0;

    परत (पूर्णांक)((prt_उजवा_margin - prt_डावा_margin) / prt_char_width);
}

    स्थिर पूर्णांक
prt_build_cid_fontname(पूर्णांक font, char_u *name, पूर्णांक name_len)
{
    अक्षर    *fontname;

    fontname = alloc(name_len + 1);
    जर (fontname == रिक्त)
	परत FALSE;
    vim_strncpy((char_u *)fontname, name, name_len);
    prt_ps_mb_font.ps_fontname[font] = fontname;

    परत TRUE;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    स्थिर पूर्णांक
prt_get_lpp(शून्य)
{
    पूर्णांक lpp;

    /*
     * Calculate offset to lower left corner of background rect based on actual
     * font height (based on its bounding box) and the line height, handling the
     * case where the font height can exceed the line height.
     */
    prt_bgcol_offset = (दशांश)PRT_PS_FONT_TO_USER(prt_ओळ_height,
					   prt_ps_font->bbox_min_y);
    जर ((prt_ps_font->bbox_max_y - prt_ps_font->bbox_min_y) < 1000.0)
    {
	prt_bgcol_offset -= (दशांश)PRT_PS_FONT_TO_USER(prt_ओळ_height,
				(1000.0 - (prt_ps_font->bbox_max_y -
					    prt_ps_font->bbox_min_y)) / 2);
    }

    // Get height for topmost line based on background rect offset.
    prt_first_ओळ_height = prt_ओळ_height + prt_bgcol_offset;

    // Calculate lpp
    lpp = (पूर्णांक)((prt_शीर्ष_margin - prt_bottom_margin) / prt_ओळ_height);

    // Adjust top margin if there is a header
    prt_शीर्ष_margin -= prt_ओळ_height * prt_header_height();

    परत lpp - prt_header_height();
}

    स्थिर पूर्णांक
prt_match_encoding(
    अक्षर			*p_encoding,
    संरचना prt_ps_mbfont_S	*p_cmap,
    संरचना prt_ps_encoding_S	**pp_mbenc)
{
    पूर्णांक				mbenc;
    पूर्णांक				enc_len;
    संरचना prt_ps_encoding_S	*p_mbenc;

    *pp_mbenc = रिक्त;
    // Look for recognised encoding
    enc_len = (पूर्णांक)STRLEN(p_encoding);
    p_mbenc = p_cmap->encodings;
    साठी (mbenc = 0; mbenc < p_cmap->अंक_encodings; mbenc++)
    {
	जर (STRNICMP(p_mbenc->encoding, p_encoding, enc_len) == 0)
	{
	    *pp_mbenc = p_mbenc;
	    परत TRUE;
	}
	p_mbenc++;
    }
    परत FALSE;
}

    स्थिर पूर्णांक
prt_match_charset(
    अक्षर		    *p_charset,
    संरचना prt_ps_mbfont_S  *p_cmap,
    संरचना prt_ps_charset_S **pp_mbchar)
{
    पूर्णांक			    mbchar;
    पूर्णांक			    char_len;
    संरचना prt_ps_charset_S *p_mbchar;

    // Look for recognised character set, using default if one is not given
    जर (*p_charset == NUL)
	p_charset = p_cmap->defcs;
    char_len = (पूर्णांक)STRLEN(p_charset);
    p_mbchar = p_cmap->charsets;
    साठी (mbchar = 0; mbchar < p_cmap->अंक_charsets; mbchar++)
    {
	जर (STRNICMP(p_mbchar->charset, p_charset, char_len) == 0)
	{
	    *pp_mbchar = p_mbchar;
	    परत TRUE;
	}
	p_mbchar++;
    }
    परत FALSE;
}

    पूर्णांक
mch_print_init(
    prt_settings_T *psettings,
    char_u	*jobname,
    पूर्णांक		forceit UNUSED)
{
    पूर्णांक		i;
    अक्षर	*paper_name;
    पूर्णांक		paper_strlen;
    पूर्णांक		fontsize;
    char_u	*p;
    द्विगुण      डावा;
    द्विगुण      उजवा;
    द्विगुण      शीर्ष;
    द्विगुण      bottom;
    पूर्णांक		props;
    पूर्णांक		cmap = 0;
    char_u	*p_encoding;
    संरचना prt_ps_encoding_S *p_mbenc;
    संरचना prt_ps_encoding_S *p_mbenc_first;
    संरचना prt_ps_charset_S  *p_mbchar = रिक्त;

# जर 0
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
    जर (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
# endif

    /*
     * Set up font and encoding.
     */
    p_encoding = enc_skip(p_penc);
    जर (*p_encoding == NUL)
	p_encoding = enc_skip(p_enc);

    // Look for a multi-byte font that matches the encoding and character set.
    // Only look if multi-byte character set is defined, or using multi-byte
    // encoding other than Unicode.  This is because a Unicode encoding does not
    // uniquely identify a CJK character set to use.
    p_mbenc = रिक्त;
    props = enc_canon_props(p_encoding);
    जर (!(props & ENC_8BIT) && ((*p_pmcs != NUL) || !(props & ENC_UNICODE)))
    {
	पूर्णांक cmap_first = 0;

	p_mbenc_first = रिक्त;
	साठी (cmap = 0; cmap < (पूर्णांक)ARRAY_LENGTH(prt_ps_mbfonts); cmap++)
	    जर (prt_match_encoding((अक्षर *)p_encoding, &prt_ps_mbfonts[cmap],
								    &p_mbenc))
	    {
		जर (p_mbenc_first == रिक्त)
		{
		    p_mbenc_first = p_mbenc;
		    cmap_first = cmap;
		}
		जर (prt_match_charset((अक्षर *)p_pmcs, &prt_ps_mbfonts[cmap],
								   &p_mbchar))
		    थांबा;
	    }

	// Use first encoding matched if no charset matched
	जर (p_mbchar == रिक्त && p_mbenc_first != रिक्त)
	{
	    p_mbenc = p_mbenc_first;
	    cmap = cmap_first;
	}
    }

    prt_out_mbyte = (p_mbenc != रिक्त);
    जर (prt_out_mbyte)
    {
	// Build CMap name - will be same for all multi-byte fonts used
	prt_cmap[0] = NUL;

	prt_custom_cmap = (p_mbchar == रिक्त);
	जर (!prt_custom_cmap)
	{
	    // Check encoding and character set are compatible
	    जर ((p_mbenc->needs_charset & p_mbchar->has_charset) == 0)
	    {
		emsg(_(e_incompatible_multi_byte_encoding_and_character_set));
		परत FALSE;
	    }

	    // Add charset name if not empty
	    जर (p_mbchar->cmap_charset != रिक्त)
	    {
		vim_strncpy((char_u *)prt_cmap,
		      (char_u *)p_mbchar->cmap_charset, आकार(prt_cmap) - 3);
		STRCAT(prt_cmap, "-");
	    }
	}
	अन्यथा
	{
	    // Add custom CMap character set name
	    जर (*p_pmcs == NUL)
	    {
		emsg(_(e_printmbcharset_cannot_be_empty_with_multi_byte_encoding));
		परत FALSE;
	    }
	    vim_strncpy((char_u *)prt_cmap, p_pmcs, आकार(prt_cmap) - 3);
	    STRCAT(prt_cmap, "-");
	}

	// CMap name ends with (optional) encoding name and -H for horizontal
	जर (p_mbenc->cmap_encoding != रिक्त && STRLEN(prt_cmap)
		      + STRLEN(p_mbenc->cmap_encoding) + 3 < आकार(prt_cmap))
	{
	    STRCAT(prt_cmap, p_mbenc->cmap_encoding);
	    STRCAT(prt_cmap, "-");
	}
	STRCAT(prt_cmap, "H");

	जर (!mbfont_opts[OPT_MBFONT_REGULAR].present)
	{
	    emsg(_(e_no_default_font_specified_for_multi_byte_printing));
	    परत FALSE;
	}

	// Derive CID font names with fallbacks if not defined
	जर (!prt_build_cid_fontname(PRT_PS_FONT_ROMAN,
				    mbfont_opts[OPT_MBFONT_REGULAR].string,
				    mbfont_opts[OPT_MBFONT_REGULAR].म_माप))
	    परत FALSE;
	जर (mbfont_opts[OPT_MBFONT_BOLD].present)
	    जर (!prt_build_cid_fontname(PRT_PS_FONT_BOLD,
					mbfont_opts[OPT_MBFONT_BOLD].string,
					mbfont_opts[OPT_MBFONT_BOLD].म_माप))
		परत FALSE;
	जर (mbfont_opts[OPT_MBFONT_OBLIQUE].present)
	    जर (!prt_build_cid_fontname(PRT_PS_FONT_OBLIQUE,
					mbfont_opts[OPT_MBFONT_OBLIQUE].string,
					mbfont_opts[OPT_MBFONT_OBLIQUE].म_माप))
		परत FALSE;
	जर (mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].present)
	    जर (!prt_build_cid_fontname(PRT_PS_FONT_BOLDOBLIQUE,
				   mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].string,
				  mbfont_opts[OPT_MBFONT_BOLDOBLIQUE].म_माप))
		परत FALSE;

	// Check if need to use Courier for ASCII code range, and if so pick up
	// the encoding to use
	prt_use_courier = mbfont_opts[OPT_MBFONT_USECOURIER].present &&
	    (TOLOWER_ASC(mbfont_opts[OPT_MBFONT_USECOURIER].string[0]) == 'y');
	जर (prt_use_courier)
	{
	    // Use national ASCII variant unless ASCII wanted
	    जर (mbfont_opts[OPT_MBFONT_ASCII].present &&
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
    जर (printer_opts[OPT_PRINT_PAPER].present)
    {
	paper_name = (अक्षर *)printer_opts[OPT_PRINT_PAPER].string;
	paper_strlen = printer_opts[OPT_PRINT_PAPER].म_माप;
    }
    अन्यथा
    {
	paper_name = "A4";
	paper_strlen = 2;
    }
    साठी (i = 0; i < (पूर्णांक)PRT_MEDIASIZE_LEN; ++i)
	जर (STRLEN(prt_mediasize[i].name) == (अचिन्हित)paper_strlen
		&& STRNICMP(prt_mediasize[i].name, paper_name,
							   paper_strlen) == 0)
	    थांबा;
    जर (i == PRT_MEDIASIZE_LEN)
	i = 0;
    prt_media = i;

    /*
     * Set PS pagesize based on media dimensions and print orientation.
     * Note: Media and page sizes have defined meanings in PostScript and should
     * be kept distinct.  Media is the paper (or transparency, or ...) that is
     * printed on, whereas the page size is the area that the PostScript
     * interpreter renders into.
     */
    जर (prt_portrait)
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
    prt_page_margins(prt_page_width, prt_page_height, &डावा, &उजवा, &शीर्ष,
								    &bottom);
    prt_डावा_margin = (दशांश)डावा;
    prt_उजवा_margin = (दशांश)उजवा;
    prt_शीर्ष_margin = (दशांश)शीर्ष;
    prt_bottom_margin = (दशांश)bottom;

    /*
     * Set up the font size.
     */
    fontsize = PRT_PS_DEFAULT_FONTSIZE;
    साठी (p = p_pfn; (p = vim_strchr(p, ':')) != रिक्त; ++p)
	जर (p[1] == 'h' && VIM_ISDIGIT(p[2]))
	    fontsize = म_से_प((अक्षर *)p + 2);
    prt_font_metrics(fontsize);

    /*
     * Return the number of characters per line, and lines per page for the
     * generic print code.
     */
    psettings->chars_per_ओळ = prt_get_cpl();
    psettings->ओळी_per_page = prt_get_lpp();

    // Catch margin settings that leave no space for output!
    जर (psettings->chars_per_ओळ <= 0 || psettings->ओळी_per_page <= 0)
	परत FAIL;

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
    जर (prt_collate)
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
    जर (printer_opts[OPT_PRINT_DUPLEX].present)
    {
	जर (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "off", 3) == 0)
	{
	    prt_duplex = FALSE;
	    psettings->duplex = 0;
	}
	अन्यथा जर (STRNICMP(printer_opts[OPT_PRINT_DUPLEX].string, "short", 5)
									 == 0)
	    prt_tumble = TRUE;
    }

    // For now user abort not supported
    psettings->user_abort = 0;

    // If the user didn't specify a file name, use a temp file.
    जर (psettings->outfile == रिक्त)
    {
	prt_ps_file_name = vim_tempname('p', TRUE);
	जर (prt_ps_file_name == रिक्त)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    परत FAIL;
	}
	prt_ps_fd = mch_fopen((अक्षर *)prt_ps_file_name, WRITEBIN);
    }
    अन्यथा
    {
	p = expand_env_save(psettings->outfile);
	जर (p != रिक्त)
	{
	    prt_ps_fd = mch_fopen((अक्षर *)p, WRITEBIN);
	    vim_free(p);
	}
    }
    जर (prt_ps_fd == रिक्त)
    {
	emsg(_(e_cant_open_postscript_output_file));
	mch_print_cleanup();
	परत FAIL;
    }

    prt_bufsiz = psettings->chars_per_ओळ;
    जर (prt_out_mbyte)
	prt_bufsiz *= 2;
    ga_init2(&prt_ps_buffer, आकार(अक्षर), prt_bufsiz);

    prt_page_अंक = 0;

    prt_attribute_change = FALSE;
    prt_need_moveto = FALSE;
    prt_need_font = FALSE;
    prt_need_fgcol = FALSE;
    prt_need_bgcol = FALSE;
    prt_need_underline = FALSE;

    prt_file_error = FALSE;

    परत OK;
}

    स्थिर पूर्णांक
prt_add_resource(संरचना prt_ps_resource_S *resource)
{
    खाता*	fd_resource;
    char_u	resource_buffer[512];
    माप_प्रकार	bytes_read;

    fd_resource = mch_fopen((अक्षर *)resource->filename, READBIN);
    जर (fd_resource == रिक्त)
    {
	semsg(_(e_cant_open_file_str_2), resource->filename);
	परत FALSE;
    }
    prt_dsc_resources("BeginResource", prt_resource_types[resource->type],
						     (अक्षर *)resource->title);

    prt_dsc_textline("BeginDocument", (अक्षर *)resource->filename);

    साठी (;;)
    {
	bytes_read = ख_पढ़ो((अक्षर *)resource_buffer, आकार(char_u),
			   आकार(resource_buffer), fd_resource);
	जर (ख_त्रुटि(fd_resource))
	{
	    semsg(_(e_cant_read_postscript_resource_file_str),
							    resource->filename);
	    ख_बंद(fd_resource);
	    परत FALSE;
	}
	जर (bytes_read == 0)
	    थांबा;
	prt_write_file_raw_len(resource_buffer, (पूर्णांक)bytes_read);
	जर (prt_file_error)
	{
	    ख_बंद(fd_resource);
	    परत FALSE;
	}
    }
    ख_बंद(fd_resource);

    prt_dsc_noarg("EndDocument");

    prt_dsc_noarg("EndResource");

    परत TRUE;
}

    पूर्णांक
mch_print_begin(prt_settings_T *psettings)
{
    पूर्णांक		bbox[4];
    द्विगुण      डावा;
    द्विगुण      उजवा;
    द्विगुण      शीर्ष;
    द्विगुण      bottom;
    संरचना prt_ps_resource_S *res_prolog;
    संरचना prt_ps_resource_S *res_encoding;
    अक्षर	buffer[256];
    char_u      *p_encoding;
    संरचना prt_ps_resource_S *res_cidfont;
    संरचना prt_ps_resource_S *res_cmap;
    पूर्णांक		retval = FALSE;

    res_prolog = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_encoding = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_cidfont = ALLOC_ONE(संरचना prt_ps_resource_S);
    res_cmap = ALLOC_ONE(संरचना prt_ps_resource_S);
    जर (res_prolog == रिक्त || res_encoding == रिक्त
	    || res_cidfont == रिक्त || res_cmap == रिक्त)
	जा theend;

    /*
     * PS DSC Header comments - no PS code!
     */
    prt_dsc_start();
    prt_dsc_textline("Title", (अक्षर *)psettings->jobname);
    जर (!get_user_name((char_u *)buffer, 256))
	STRCPY(buffer, "Unknown");
    prt_dsc_textline("For", buffer);
    prt_dsc_textline("Creator", VIM_VERSION_LONG);
    // Note: to ensure Clean8bit I don't think we can use LC_TIME

    prt_dsc_textline("CreationDate", get_ctime(समय(रिक्त), FALSE));
    prt_dsc_textline("DocumentData", "Clean8Bit");
    prt_dsc_textline("Orientation", prt_portrait ? "Portrait" : "Landscape");
    prt_dsc_atend("Pages");
    prt_dsc_textline("PageOrder", "Ascend");
    // The bbox does not change with orientation - it is always in the default
    // user coordinate system!  We have to recalculate right and bottom
    // coordinates based on the font metrics for the bbox to be accurate.
    prt_page_margins(prt_mediasize[prt_media].width,
					    prt_mediasize[prt_media].height,
					    &डावा, &उजवा, &शीर्ष, &bottom);
    bbox[0] = (पूर्णांक)डावा;
    जर (prt_portrait)
    {
	// In portrait printing the fixed point is the top left corner so we
	// derive the bbox from that point.  We have the expected cpl chars
	// across the media and lpp lines down the media.
	bbox[1] = (पूर्णांक)(शीर्ष - (psettings->ओळी_per_page + prt_header_height())
						    * (द्विगुण)prt_ओळ_height);
	bbox[2] = (पूर्णांक)(डावा + psettings->chars_per_ओळ
					       * (द्विगुण)prt_char_width + 0.5);
	bbox[3] = (पूर्णांक)(शीर्ष + 0.5);
    }
    अन्यथा
    {
	// In landscape printing the fixed point is the bottom left corner so we
	// derive the bbox from that point.  We have lpp chars across the media
	// and cpl lines up the media.
	bbox[1] = (पूर्णांक)bottom;
	bbox[2] = (पूर्णांक)(डावा + ((psettings->ओळी_per_page
			      + prt_header_height()) * prt_ओळ_height) + 0.5);
	bbox[3] = (पूर्णांक)(bottom + psettings->chars_per_ओळ
					       * (द्विगुण)prt_char_width + 0.5);
    }
    prt_dsc_ints("BoundingBox", 4, bbox);
    // The media width and height does not change with landscape printing!
    prt_dsc_docmedia(prt_mediasize[prt_media].name,
				prt_mediasize[prt_media].width,
				prt_mediasize[prt_media].height,
				(द्विगुण)0, रिक्त, रिक्त);
    // Define fonts needed
    जर (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("DocumentNeededResources", &prt_ps_courier_font);
    जर (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? रिक्त
				: "DocumentNeededResources"), &prt_ps_mb_font);
	जर (!prt_custom_cmap)
	    prt_dsc_resources(रिक्त, "cmap", prt_cmap);
    }

    // Search for external resources VIM supplies
    जर (!prt_find_resource("prolog", res_prolog))
    {
	semsg(_(e_cant_find_postscript_resource_file_str_ps), "prolog");
	जा theend;
    }
    जर (!prt_open_resource(res_prolog))
	जा theend;
    जर (!prt_check_resource(res_prolog, PRT_PROLOG_VERSION))
	जा theend;
    जर (prt_out_mbyte)
    {
	// Look for required version of multi-byte printing procset
	जर (!prt_find_resource("cidfont", res_cidfont))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), "cidfont");
	    जा theend;
	}
	जर (!prt_open_resource(res_cidfont))
	    जा theend;
	जर (!prt_check_resource(res_cidfont, PRT_CID_PROLOG_VERSION))
	    जा theend;
    }

    // Find an encoding to use for printing.
    // Check 'printencoding'. If not set or not found, then use 'encoding'. If
    // that cannot be found then default to "latin1".
    // Note: VIM specific encoding header is always skipped.
    जर (!prt_out_mbyte)
    {
	p_encoding = enc_skip(p_penc);
	जर (*p_encoding == NUL
		|| !prt_find_resource((अक्षर *)p_encoding, res_encoding))
	{
	    // 'printencoding' not set or not supported - find alternate
	    पूर्णांक		props;

	    p_encoding = enc_skip(p_enc);
	    props = enc_canon_props(p_encoding);
	    जर (!(props & ENC_8BIT)
		    || !prt_find_resource((अक्षर *)p_encoding, res_encoding))
		// 8-bit 'encoding' is not supported
	    {
		// Use latin1 as default printing encoding
		p_encoding = (char_u *)"latin1";
		जर (!prt_find_resource((अक्षर *)p_encoding, res_encoding))
		{
		    semsg(_(e_cant_find_postscript_resource_file_str_ps),
			    p_encoding);
		    जा theend;
		}
	    }
	}
	जर (!prt_open_resource(res_encoding))
	    जा theend;
	// For the moment there are no checks on encoding resource files to
	// perform
    }
    अन्यथा
    {
	p_encoding = enc_skip(p_penc);
	जर (*p_encoding == NUL)
	    p_encoding = enc_skip(p_enc);
	जर (prt_use_courier)
	{
	    // Include ASCII range encoding vector
	    जर (!prt_find_resource(prt_ascii_encoding, res_encoding))
	    {
		semsg(_(e_cant_find_postscript_resource_file_str_ps),
							  prt_ascii_encoding);
		जा theend;
	    }
	    जर (!prt_open_resource(res_encoding))
		जा theend;
	    // For the moment there are no checks on encoding resource files to
	    // perform
	}
    }

    prt_conv.vc_type = CONV_NONE;
    जर (!(enc_canon_props(p_enc) & enc_canon_props(p_encoding) & ENC_8BIT))
    {
	// Set up encoding conversion if required
	जर (FAIL == convert_setup(&prt_conv, p_enc, p_encoding))
	{
	    semsg(_(e_unable_to_convert_to_print_encoding_str), p_encoding);
	    जा theend;
	}
	prt_do_conv = TRUE;
    }
    prt_do_conv = prt_conv.vc_type != CONV_NONE;

    जर (prt_out_mbyte && prt_custom_cmap)
    {
	// Find user supplied CMap
	जर (!prt_find_resource(prt_cmap, res_cmap))
	{
	    semsg(_(e_cant_find_postscript_resource_file_str_ps), prt_cmap);
	    जा theend;
	}
	जर (!prt_open_resource(res_cmap))
	    जा theend;
    }

    // List resources supplied
    STRCPY(buffer, res_prolog->title);
    STRCAT(buffer, " ");
    STRCAT(buffer, res_prolog->version);
    prt_dsc_resources("DocumentSuppliedResources", "procset", buffer);
    जर (prt_out_mbyte)
    {
	STRCPY(buffer, res_cidfont->title);
	STRCAT(buffer, " ");
	STRCAT(buffer, res_cidfont->version);
	prt_dsc_resources(रिक्त, "procset", buffer);

	जर (prt_custom_cmap)
	{
	    STRCPY(buffer, res_cmap->title);
	    STRCAT(buffer, " ");
	    STRCAT(buffer, res_cmap->version);
	    prt_dsc_resources(रिक्त, "cmap", buffer);
	}
    }
    जर (!prt_out_mbyte || prt_use_courier)
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
    जर (!prt_out_mbyte || prt_use_courier)
	prt_dsc_font_resource("PageResources", &prt_ps_courier_font);
    जर (prt_out_mbyte)
    {
	prt_dsc_font_resource((prt_use_courier ? रिक्त : "PageResources"),
							    &prt_ps_mb_font);
	जर (!prt_custom_cmap)
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
    जर (!prt_add_resource(res_prolog))
	जा theend;
    जर (prt_out_mbyte)
    {
	// Add CID font procset, and any user supplied CMap
	जर (!prt_add_resource(res_cidfont))
	    जा theend;
	जर (prt_custom_cmap && !prt_add_resource(res_cmap))
	    जा theend;
    }

    जर (!prt_out_mbyte || prt_use_courier)
	// There will be only one Roman font encoding to be included in the PS
	// file.
	जर (!prt_add_resource(res_encoding))
	    जा theend;

    prt_dsc_noarg("EndProlog");

    /*
     * PS Document setup - must appear after the prolog
     */
    prt_dsc_noarg("BeginSetup");

    // Device setup - page size and number of uncollated copies
    prt_write_int((पूर्णांक)prt_mediasize[prt_media].width);
    prt_write_int((पूर्णांक)prt_mediasize[prt_media].height);
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
    जर (!prt_out_mbyte || prt_use_courier)
    {
	// When using Courier for ASCII range when printing multi-byte, need to
	// pick up ASCII encoding to use with it.
	जर (prt_use_courier)
	    p_encoding = (char_u *)prt_ascii_encoding;
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_def_font("F0", (अक्षर *)p_encoding, (पूर्णांक)prt_ओळ_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_def_font("F1", (अक्षर *)p_encoding, (पूर्णांक)prt_ओळ_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLD]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_def_font("F2", (अक्षर *)p_encoding, (पूर्णांक)prt_ओळ_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	prt_def_font("F3", (अक्षर *)p_encoding, (पूर्णांक)prt_ओळ_height,
		     prt_ps_courier_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
    }
    जर (prt_out_mbyte)
    {
	// Define the CID fonts to be used in the job.	Typically CJKV fonts do
	// not have an italic form being a western style, so where no font is
	// defined for these faces VIM falls back to an existing face.
	// Note: if using Courier for the ASCII range then the printout will
	// have bold/italic/bolditalic regardless of the setting of printmbfont.
	prt_dsc_resources("IncludeResource", "font",
			  prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);
	जर (!prt_custom_cmap)
	    prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	prt_def_cidfont("CF0", (पूर्णांक)prt_ओळ_height,
			prt_ps_mb_font.ps_fontname[PRT_PS_FONT_ROMAN]);

	जर (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	    जर (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF1", (पूर्णांक)prt_ओळ_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLD]);
	}
	अन्यथा
	    // Use ROMAN for BOLD
	    prt_dup_cidfont("CF0", "CF1");

	जर (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	    जर (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF2", (पूर्णांक)prt_ओळ_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_OBLIQUE]);
	}
	अन्यथा
	    // Use ROMAN for OBLIQUE
	    prt_dup_cidfont("CF0", "CF2");

	जर (prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE] != रिक्त)
	{
	    prt_dsc_resources("IncludeResource", "font",
			      prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	    जर (!prt_custom_cmap)
		prt_dsc_resources("IncludeResource", "cmap", prt_cmap);
	    prt_def_cidfont("CF3", (पूर्णांक)prt_ओळ_height,
			    prt_ps_mb_font.ps_fontname[PRT_PS_FONT_BOLDOBLIQUE]);
	}
	अन्यथा
	    // Use BOLD for BOLDOBLIQUE
	    prt_dup_cidfont("CF1", "CF3");
    }

    // Misc constant vars used for underlining and background rects
    prt_def_var("UO", PRT_PS_FONT_TO_USER(prt_ओळ_height,
						prt_ps_font->uline_offset), 2);
    prt_def_var("UW", PRT_PS_FONT_TO_USER(prt_ओळ_height,
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

    परत retval;
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

    जर (!prt_file_error && psettings->outfile == रिक्त
					&& !got_int && !psettings->user_abort)
    {
	// Close the file first.
	जर (prt_ps_fd != रिक्त)
	{
	    ख_बंद(prt_ps_fd);
	    prt_ps_fd = रिक्त;
	}
	prt_message((char_u *)_("Sending to printer..."));

	// Not printing to a file: use 'printexpr' to print the file.
	जर (eval_printexpr(prt_ps_file_name, psettings->arguments) == FAIL)
	    emsg(_(e_failed_to_print_postscript_file));
	अन्यथा
	    prt_message((char_u *)_("Print job sent."));
    }

    mch_print_cleanup();
}

    पूर्णांक
mch_print_end_page(शून्य)
{
    prt_flush_buffer();

    prt_write_string("re sp\n");

    prt_dsc_noarg("PageTrailer");

    परत !prt_file_error;
}

    पूर्णांक
mch_print_begin_page(char_u *str UNUSED)
{
    पूर्णांक		page_अंक[2];

    prt_page_अंक++;

    page_अंक[0] = page_अंक[1] = prt_page_अंक;
    prt_dsc_ints("Page", 2, page_अंक);

    prt_dsc_noarg("BeginPageSetup");

    prt_write_string("sv\n0 g\n");
    prt_in_ascii = !prt_out_mbyte;
    जर (prt_out_mbyte)
	prt_write_string("CF0 sf\n");
    अन्यथा
	prt_write_string("F0 sf\n");
    prt_fgcol = PRCOLOR_BLACK;
    prt_bgcol = PRCOLOR_WHITE;
    prt_font = PRT_PS_FONT_ROMAN;

    // Set up page transformation for landscape printing.
    जर (!prt_portrait)
    {
	prt_write_int(-((पूर्णांक)prt_mediasize[prt_media].width));
	prt_write_string("sl\n");
    }

    prt_dsc_noarg("EndPageSetup");

    // We have reset the font attributes, force setting them again.
    prt_curr_bg = (long_u)0xffffffff;
    prt_curr_fg = (long_u)0xffffffff;
    prt_curr_bold = MAYBE;

    परत !prt_file_error;
}

    पूर्णांक
mch_print_blank_page(शून्य)
{
    परत (mch_print_begin_page(रिक्त) ? (mch_print_end_page()) : FALSE);
}

स्थिर दशांश prt_pos_x = 0;
स्थिर दशांश prt_pos_y = 0;

    शून्य
mch_print_start_ओळ(पूर्णांक margin, पूर्णांक page_ओळ)
{
    prt_pos_x = prt_डावा_margin;
    जर (margin)
	prt_pos_x -= prt_संख्या_width;

    prt_pos_y = prt_शीर्ष_margin - prt_first_ओळ_height -
					page_ओळ * prt_ओळ_height;

    prt_attribute_change = TRUE;
    prt_need_moveto = TRUE;
    prt_half_width = FALSE;
}

    पूर्णांक
mch_print_text_out(char_u *textp, पूर्णांक len UNUSED)
{
    char_u	*p = textp;
    पूर्णांक		need_break;
    char_u	ch;
    char_u      ch_buff[8];
    दशांश       char_width;
    दशांश       next_pos;
    पूर्णांक		in_ascii;
    पूर्णांक		half_width;
    char_u	*tofree = रिक्त;

    char_width = prt_char_width;

    // Ideally VIM would create a rearranged CID font to combine a Roman and
    // CJKV font to do what VIM is doing here - use a Roman font for characters
    // in the ASCII range, and the original CID font for everything else.
    // The problem is that GhostScript still (as of 8.13) does not support
    // rearranged fonts even though they have been documented by Adobe for 7
    // years!  If they ever do, a lot of this code will disappear.
    जर (prt_use_courier)
    {
	in_ascii = (len == 1 && *p < 0x80);
	जर (prt_in_ascii)
	{
	    जर (!in_ascii)
	    {
		// No longer in ASCII range - need to switch font
		prt_in_ascii = FALSE;
		prt_need_font = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	अन्यथा जर (in_ascii)
	{
	    // Now in ASCII range - need to switch font
	    prt_in_ascii = TRUE;
	    prt_need_font = TRUE;
	    prt_attribute_change = TRUE;
	}
    }
    जर (prt_out_mbyte)
    {
	half_width = ((*mb_ptr2cells)(p) == 1);
	जर (half_width)
	    char_width /= 2;
	जर (prt_half_width)
	{
	    जर (!half_width)
	    {
		prt_half_width = FALSE;
		prt_pos_x += prt_char_width/4;
		prt_need_moveto = TRUE;
		prt_attribute_change = TRUE;
	    }
	}
	अन्यथा जर (half_width)
	{
	    prt_half_width = TRUE;
	    prt_pos_x += prt_char_width/4;
	    prt_need_moveto = TRUE;
	    prt_attribute_change = TRUE;
	}
    }

    // Output any required changes to the graphics state, after flushing any
    // text buffered so far.
    जर (prt_attribute_change)
    {
	prt_flush_buffer();
	// Reset count of number of chars that will be printed
	prt_text_run = 0;

	जर (prt_need_moveto)
	{
	    prt_pos_x_moveto = prt_pos_x;
	    prt_pos_y_moveto = prt_pos_y;
	    prt_do_moveto = TRUE;

	    prt_need_moveto = FALSE;
	}
	जर (prt_need_font)
	{
	    जर (!prt_in_ascii)
		prt_write_string("CF");
	    अन्यथा
		prt_write_string("F");
	    prt_write_int(prt_font);
	    prt_write_string("sf\n");
	    prt_need_font = FALSE;
	}
	जर (prt_need_fgcol)
	{
	    पूर्णांक     r, g, b;
	    r = ((अचिन्हित)prt_fgcol & 0xff0000) >> 16;
	    g = ((अचिन्हित)prt_fgcol & 0xff00) >> 8;
	    b = prt_fgcol & 0xff;

	    prt_write_real(r / 255.0, 3);
	    जर (r == g && g == b)
		prt_write_string("g\n");
	    अन्यथा
	    {
		prt_write_real(g / 255.0, 3);
		prt_write_real(b / 255.0, 3);
		prt_write_string("r\n");
	    }
	    prt_need_fgcol = FALSE;
	}

	जर (prt_bgcol != PRCOLOR_WHITE)
	{
	    prt_new_bgcol = prt_bgcol;
	    जर (prt_need_bgcol)
		prt_do_bgcol = TRUE;
	}
	अन्यथा
	    prt_do_bgcol = FALSE;
	prt_need_bgcol = FALSE;

	जर (prt_need_underline)
	    prt_do_underline = prt_underline;
	prt_need_underline = FALSE;

	prt_attribute_change = FALSE;
    }

    जर (prt_do_conv)
    {
	// Convert from multi-byte to 8-bit encoding
	tofree = p = string_convert(&prt_conv, p, &len);
	जर (p == रिक्त)
	{
	    p = (char_u *)"";
	    len = 0;
	}
    }

    जर (prt_out_mbyte)
    {
	// Multi-byte character strings are represented more efficiently as hex
	// strings when outputting clean 8 bit PS.
	पर्यंत (len-- > 0)
	{
	    ch = prt_hexchar[(अचिन्हित)(*p) >> 4];
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
	जर (ch < 32 || ch == '(' || ch == ')' || ch == '\\')
	{
	    // Convert non-printing characters to either their escape or octal
	    // sequence, ensures PS sent over a serial line does not interfere
	    // with the comms protocol.
	    ga_append(&prt_ps_buffer, '\\');
	    निवड (ch)
	    {
		स्थिती BS:   ga_append(&prt_ps_buffer, 'b'); थांबा;
		स्थिती TAB:  ga_append(&prt_ps_buffer, 't'); थांबा;
		स्थिती NL:   ga_append(&prt_ps_buffer, 'n'); थांबा;
		स्थिती FF:   ga_append(&prt_ps_buffer, 'f'); थांबा;
		स्थिती CAR:  ga_append(&prt_ps_buffer, 'r'); थांबा;
		स्थिती '(':  ga_append(&prt_ps_buffer, '('); थांबा;
		स्थिती ')':  ga_append(&prt_ps_buffer, ')'); थांबा;
		स्थिती '\\': ga_append(&prt_ps_buffer, '\\'); थांबा;

		पूर्वनिर्धारित:
			   प्र_लिखो((अक्षर *)ch_buff, "%03o", (अचिन्हित पूर्णांक)ch);
			   ga_append(&prt_ps_buffer, ch_buff[0]);
			   ga_append(&prt_ps_buffer, ch_buff[1]);
			   ga_append(&prt_ps_buffer, ch_buff[2]);
			   थांबा;
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
    need_break = (next_pos > prt_उजवा_margin) &&
		    ((next_pos - prt_उजवा_margin) > (prt_उजवा_margin*1e-5));

    जर (need_break)
	prt_flush_buffer();

    परत need_break;
}

    शून्य
mch_print_set_font(पूर्णांक iBold, पूर्णांक iItalic, पूर्णांक iUnderline)
{
    पूर्णांक		font = 0;

    जर (iBold)
	font |= 0x01;
    जर (iItalic)
	font |= 0x02;

    जर (font != prt_font)
    {
	prt_font = font;
	prt_attribute_change = TRUE;
	prt_need_font = TRUE;
    }
    जर (prt_underline != iUnderline)
    {
	prt_underline = iUnderline;
	prt_attribute_change = TRUE;
	prt_need_underline = TRUE;
    }
}

    शून्य
mch_print_set_bg(long_u bgcol)
{
    prt_bgcol = (पूर्णांक)bgcol;
    prt_attribute_change = TRUE;
    prt_need_bgcol = TRUE;
}

    शून्य
mch_print_set_fg(long_u fgcol)
{
    जर (fgcol == (long_u)prt_fgcol)
	परत;

    prt_fgcol = (पूर्णांक)fgcol;
    prt_attribute_change = TRUE;
    prt_need_fgcol = TRUE;
}

#endif // FEAT_POSTSCRIPT
