/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * Multibyte extensions partly by Sung-Hoon Baek
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * mbyte.c: Code specifically for handling multi-byte characters.
 *
 * The encoding used in the core is set with 'encoding'.  When 'encoding' is
 * changed, the following four variables are set (for speed).
 * Currently these types of character encodings are supported:
 *
 * "enc_dbcs"	    When non-zero it tells the type of double byte character
 *		    encoding (Chinese, Korean, Japanese, etc.).
 *		    The cell width on the display is equal to the number of
 *		    bytes.  (exception: DBCS_JPNU with first byte 0x8e)
 *		    Recognizing the first or second byte is difficult, it
 *		    requires checking a byte sequence from the start.
 * "enc_utf8"	    When TRUE use Unicode characters in UTF-8 encoding.
 *		    The cell width on the display needs to be determined from
 *		    the character value.
 *		    Recognizing bytes is easy: 0xxx.xxxx is a single-byte
 *		    char, 10xx.xxxx is a trailing byte, 11xx.xxxx is a leading
 *		    byte of a multi-byte character.
 *		    To make things complicated, up to six composing characters
 *		    are allowed.  These are drawn on top of the first char.
 *		    For most editing the sequence of bytes with composing
 *		    characters included is considered to be one character.
 * "enc_unicode"    When 2 use 16-bit Unicode characters (or UTF-16).
 *		    When 4 use 32-but Unicode characters.
 *		    Internally characters are stored in UTF-8 encoding to
 *		    avoid NUL bytes.  Conversion happens when doing I/O.
 *		    "enc_utf8" will also be TRUE.
 *
 * "has_mbyte" is set when "enc_dbcs" or "enc_utf8" is non-zero.
 *
 * If none of these is TRUE, 8-bit bytes are used for a character.  The
 * encoding isn't currently specified (TODO).
 *
 * 'encoding' specifies the encoding used in the core.  This is in registers,
 * text manipulation, buffers, etc.  Conversion has to be done when characters
 * in another encoding are received or send:
 *
 *		       clipboard
 *			   ^
 *			   | (2)
 *			   V
 *		   +---------------+
 *	      (1)  |		   | (3)
 *  keyboard ----->|	 core	   |-----> display
 *		   |		   |
 *		   +---------------+
 *			   ^
 *			   | (4)
 *			   V
 *			 file
 *
 * (1) Typed characters arrive in the current locale.  Conversion is to be
 *     done when 'encoding' is different from 'termencoding'.
 * (2) Text will be made available with the encoding specified with
 *     'encoding'.  If this is not sufficient, system-specific conversion
 *     might be required.
 * (3) For the GUI the correct font must be selected, no conversion done.
 *     Otherwise, conversion is to be done when 'encoding' differs from
 *     'termencoding'.  (Different in the GTK+ 2 port -- 'termencoding'
 *     is always used for both input and output and must always be set to
 *     "utf-8".  gui_mch_init() does this automatically.)
 * (4) The encoding of the file is specified with 'fileencoding'.  Conversion
 *     is to be done when it's different from 'encoding'.
 *
 * The viminfo file is a special case: Only text is converted, not file names.
 * Vim scripts may contain an ":encoding" command.  This has an effect for
 * some commands, like ":menutrans"
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef WIN32UNIX
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
#  ifdef __CYGWIN__
    // ControlMask from <X11/X.h> (included in "vim.h") is conflicting with
    // <w32api/windows.h> (included in <X11/Xwindows.h>).
#   undef ControlMask
#  endif
#  include <X11/Xwindows.h>
#  define WINBYTE wBYTE
# ಅನ್ಯಥಾ
#  include <windows.h>
#  define WINBYTE BYTE
# endif
# ifdef WIN32
#  undef WIN32	    // Some windows.h define WIN32, we don't want that here.
# endif
#ಅನ್ಯಥಾ
# define WINBYTE BYTE
#endif

#ಯದಿ defined(MSWIN) || defined(WIN32UNIX)
# include <winnls.h>
#endif

#ifdef FEAT_GUI_X11
# include <X11/Intrinsic.h>
#endif
#ifdef X_LOCALE
# include <X11/Xlocale.h>
# ಯದಿ !defined(HAVE_MBLEN) && !defined(mblen)
#  define mblen _Xmblen
# endif
#endif

#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_char2len(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_char2bytes(ಪೂರ್ಣಾಙ್ಕ c, char_u *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_ptr2len(char_u *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_ptr2len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ utf_ptr2cells_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_char2cells(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_ptr2cells_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_ptr2char(char_u *p);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dbcs_head_off(char_u *base, char_u *p);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ utf_ptr2char_and_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ *lenp);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ utf_ptr2char_and_len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ *lenp);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ utf_iscomposinglike_char(ಪೂರ್ಣಾಙ್ಕ c1, ಪೂರ್ಣಾಙ್ಕ c2);
#ifdef FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cw_ಮೌಲ್ಯ(ಪೂರ್ಣಾಙ್ಕ c);
#endif

/*
 * Lookup table to quickly get the length in bytes of a UTF-8 character from
 * the first byte of a UTF-8 string.
 * Bytes which are illegal when used as the first byte have a 1.
 * The NUL byte has length 1.
 */
ಸ್ಥಿರ ವರ್ಣ utf8len_tab[256] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1,
};

/*
 * Like utf8len_tab above, but using a zero for illegal lead bytes.
 */
ಸ್ಥಿರ ವರ್ಣ utf8len_tab_zero[256] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,0,0,
};


/*
 * Canonical encoding names and their properties.
 * "iso-8859-n" is handled by enc_canonize() directly.
 */
ಸ್ಥಿರ ಸಂರಚನಾ
{   ವರ್ಣ *name;		ಪೂರ್ಣಾಙ್ಕ prop;		ಪೂರ್ಣಾಙ್ಕ codepage;}
enc_canon_table[] =
{
#define IDX_LATIN_1	0
    {"latin1",		ENC_8BIT + ENC_LATIN1,	1252},
#define IDX_ISO_2	1
    {"iso-8859-2",	ENC_8BIT,		0},
#define IDX_ISO_3	2
    {"iso-8859-3",	ENC_8BIT,		0},
#define IDX_ISO_4	3
    {"iso-8859-4",	ENC_8BIT,		0},
#define IDX_ISO_5	4
    {"iso-8859-5",	ENC_8BIT,		0},
#define IDX_ISO_6	5
    {"iso-8859-6",	ENC_8BIT,		0},
#define IDX_ISO_7	6
    {"iso-8859-7",	ENC_8BIT,		0},
#define IDX_ISO_8	7
    {"iso-8859-8",	ENC_8BIT,		0},
#define IDX_ISO_9	8
    {"iso-8859-9",	ENC_8BIT,		0},
#define IDX_ISO_10	9
    {"iso-8859-10",	ENC_8BIT,		0},
#define IDX_ISO_11	10
    {"iso-8859-11",	ENC_8BIT,		0},
#define IDX_ISO_13	11
    {"iso-8859-13",	ENC_8BIT,		0},
#define IDX_ISO_14	12
    {"iso-8859-14",	ENC_8BIT,		0},
#define IDX_ISO_15	13
    {"iso-8859-15",	ENC_8BIT + ENC_LATIN9,	0},
#define IDX_KOI8_R	14
    {"koi8-r",		ENC_8BIT,		0},
#define IDX_KOI8_U	15
    {"koi8-u",		ENC_8BIT,		0},
#define IDX_UTF8	16
    {"utf-8",		ENC_UNICODE,		0},
#define IDX_UCS2	17
    {"ucs-2",		ENC_UNICODE + ENC_ENDIAN_B + ENC_2BYTE, 0},
#define IDX_UCS2LE	18
    {"ucs-2le",		ENC_UNICODE + ENC_ENDIAN_L + ENC_2BYTE, 0},
#define IDX_UTF16	19
    {"utf-16",		ENC_UNICODE + ENC_ENDIAN_B + ENC_2WORD, 0},
#define IDX_UTF16BE	20
    {"utf-16be",		ENC_UNICODE + ENC_ENDIAN_B + ENC_2WORD, 0},
#define IDX_UTF16LE	21
    {"utf-16le",	ENC_UNICODE + ENC_ENDIAN_L + ENC_2WORD, 0},
#define IDX_UCS4	22
    {"ucs-4",		ENC_UNICODE + ENC_ENDIAN_B + ENC_4BYTE, 0},
#define IDX_UCS4LE	23
    {"ucs-4le",		ENC_UNICODE + ENC_ENDIAN_L + ENC_4BYTE, 0},

    // For debugging DBCS encoding on Unix.
#define IDX_DEBUG	24
    {"debug",		ENC_DBCS,		DBCS_DEBUG},
#define IDX_EUC_JP	25
    {"euc-jp",		ENC_DBCS,		DBCS_JPNU},
#define IDX_SJIS	26
    {"sjis",		ENC_DBCS,		DBCS_JPN},
#define IDX_EUC_KR	27
    {"euc-kr",		ENC_DBCS,		DBCS_KORU},
#define IDX_EUC_CN	28
    {"euc-cn",		ENC_DBCS,		DBCS_CHSU},
#define IDX_EUC_TW	29
    {"euc-tw",		ENC_DBCS,		DBCS_CHTU},
#define IDX_BIG5	30
    {"big5",		ENC_DBCS,		DBCS_CHT},

    // MS-DOS and MS-Windows codepages are included here, so that they can be
    // used on Unix too.  Most of them are similar to ISO-8859 encodings, but
    // not exactly the same.
#define IDX_CP437	31
    {"cp437",		ENC_8BIT,		437}, // like iso-8859-1
#define IDX_CP737	32
    {"cp737",		ENC_8BIT,		737}, // like iso-8859-7
#define IDX_CP775	33
    {"cp775",		ENC_8BIT,		775}, // Baltic
#define IDX_CP850	34
    {"cp850",		ENC_8BIT,		850}, // like iso-8859-4
#define IDX_CP852	35
    {"cp852",		ENC_8BIT,		852}, // like iso-8859-1
#define IDX_CP855	36
    {"cp855",		ENC_8BIT,		855}, // like iso-8859-2
#define IDX_CP857	37
    {"cp857",		ENC_8BIT,		857}, // like iso-8859-5
#define IDX_CP860	38
    {"cp860",		ENC_8BIT,		860}, // like iso-8859-9
#define IDX_CP861	39
    {"cp861",		ENC_8BIT,		861}, // like iso-8859-1
#define IDX_CP862	40
    {"cp862",		ENC_8BIT,		862}, // like iso-8859-1
#define IDX_CP863	41
    {"cp863",		ENC_8BIT,		863}, // like iso-8859-8
#define IDX_CP865	42
    {"cp865",		ENC_8BIT,		865}, // like iso-8859-1
#define IDX_CP866	43
    {"cp866",		ENC_8BIT,		866}, // like iso-8859-5
#define IDX_CP869	44
    {"cp869",		ENC_8BIT,		869}, // like iso-8859-7
#define IDX_CP874	45
    {"cp874",		ENC_8BIT,		874}, // Thai
#define IDX_CP932	46
    {"cp932",		ENC_DBCS,		DBCS_JPN},
#define IDX_CP936	47
    {"cp936",		ENC_DBCS,		DBCS_CHS},
#define IDX_CP949	48
    {"cp949",		ENC_DBCS,		DBCS_KOR},
#define IDX_CP950	49
    {"cp950",		ENC_DBCS,		DBCS_CHT},
#define IDX_CP1250	50
    {"cp1250",		ENC_8BIT,		1250}, // Czech, Polish, etc.
#define IDX_CP1251	51
    {"cp1251",		ENC_8BIT,		1251}, // Cyrillic
    // cp1252 is considered to be equal to latin1
#define IDX_CP1253	52
    {"cp1253",		ENC_8BIT,		1253}, // Greek
#define IDX_CP1254	53
    {"cp1254",		ENC_8BIT,		1254}, // Turkish
#define IDX_CP1255	54
    {"cp1255",		ENC_8BIT,		1255}, // Hebrew
#define IDX_CP1256	55
    {"cp1256",		ENC_8BIT,		1256}, // Arabic
#define IDX_CP1257	56
    {"cp1257",		ENC_8BIT,		1257}, // Baltic
#define IDX_CP1258	57
    {"cp1258",		ENC_8BIT,		1258}, // Vietnamese

#define IDX_MACROMAN	58
    {"macroman",	ENC_8BIT + ENC_MACROMAN, 0},	// Mac OS
#define IDX_DECMCS	59
    {"dec-mcs",		ENC_8BIT,		0},	// DEC MCS
#define IDX_HPROMAN8	60
    {"hp-roman8",	ENC_8BIT,		0},	// HP Roman8
#define IDX_ಎಣಿಕೆ	61
};

/*
 * Aliases for encoding names.
 */
ಸ್ಥಿರ ಸಂರಚನಾ
{   ವರ್ಣ *name;		ಪೂರ್ಣಾಙ್ಕ canon;}
enc_alias_table[] =
{
    {"ansi",		IDX_LATIN_1},
    {"iso-8859-1",	IDX_LATIN_1},
    {"iso-8859",	IDX_LATIN_1},
    {"latin2",		IDX_ISO_2},
    {"latin3",		IDX_ISO_3},
    {"latin4",		IDX_ISO_4},
    {"cyrillic",	IDX_ISO_5},
    {"arabic",		IDX_ISO_6},
    {"greek",		IDX_ISO_7},
#ifdef MSWIN
    {"hebrew",		IDX_CP1255},
#ಅನ್ಯಥಾ
    {"hebrew",		IDX_ISO_8},
#endif
    {"latin5",		IDX_ISO_9},
    {"turkish",		IDX_ISO_9}, // ?
    {"latin6",		IDX_ISO_10},
    {"nordic",		IDX_ISO_10}, // ?
    {"thai",		IDX_ISO_11}, // ?
    {"latin7",		IDX_ISO_13},
    {"latin8",		IDX_ISO_14},
    {"latin9",		IDX_ISO_15},
    {"utf8",		IDX_UTF8},
    {"unicode",		IDX_UCS2},
    {"ucs2",		IDX_UCS2},
    {"ucs2be",		IDX_UCS2},
    {"ucs-2be",		IDX_UCS2},
    {"ucs2le",		IDX_UCS2LE},
    {"utf16",		IDX_UTF16},
    {"utf16be",		IDX_UTF16BE},
    {"utf-16be",	IDX_UTF16BE},
    {"utf16le",		IDX_UTF16LE},
    {"ucs4",		IDX_UCS4},
    {"ucs4be",		IDX_UCS4},
    {"ucs-4be",		IDX_UCS4},
    {"ucs4le",		IDX_UCS4LE},
    {"utf32",		IDX_UCS4},
    {"utf-32",		IDX_UCS4},
    {"utf32be",		IDX_UCS4},
    {"utf-32be",	IDX_UCS4},
    {"utf32le",		IDX_UCS4LE},
    {"utf-32le",	IDX_UCS4LE},
    {"932",		IDX_CP932},
    {"949",		IDX_CP949},
    {"936",		IDX_CP936},
    {"gbk",		IDX_CP936},
    {"950",		IDX_CP950},
    {"eucjp",		IDX_EUC_JP},
    {"unix-jis",	IDX_EUC_JP},
    {"ujis",		IDX_EUC_JP},
    {"shift-jis",	IDX_SJIS},
    {"pck",		IDX_SJIS},	// Sun: PCK
    {"euckr",		IDX_EUC_KR},
    {"5601",		IDX_EUC_KR},	// Sun: KS C 5601
    {"euccn",		IDX_EUC_CN},
    {"gb2312",		IDX_EUC_CN},
    {"euctw",		IDX_EUC_TW},
#ಯದಿ defined(MSWIN) || defined(WIN32UNIX) || defined(MACOS_X)
    {"japan",		IDX_CP932},
    {"korea",		IDX_CP949},
    {"prc",		IDX_CP936},
    {"chinese",		IDX_CP936},
    {"taiwan",		IDX_CP950},
    {"big5",		IDX_CP950},
#ಅನ್ಯಥಾ
    {"japan",		IDX_EUC_JP},
    {"korea",		IDX_EUC_KR},
    {"prc",		IDX_EUC_CN},
    {"chinese",		IDX_EUC_CN},
    {"taiwan",		IDX_EUC_TW},
    {"cp950",		IDX_BIG5},
    {"950",		IDX_BIG5},
#endif
    {"mac",		IDX_MACROMAN},
    {"mac-roman",	IDX_MACROMAN},
    {ರಿಕ್ತ,		0}
};

#ifndef CP_UTF8
# define CP_UTF8 65001	// magic number from winnls.h
#endif

/*
 * Find encoding "name" in the list of canonical encoding names.
 * Returns -1 if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
enc_canon_ಹುಡುಕು(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < IDX_ಎಣಿಕೆ; ++i)
	ಯದಿ (STRCMP(name, enc_canon_table[i].name) == 0)
	    ಪ್ರತ್ಯಾಗಮ i;
    ಪ್ರತ್ಯಾಗಮ -1;
}


/*
 * Find canonical encoding "name" in the list and return its properties.
 * Returns 0 if not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
enc_canon_props(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    i = enc_canon_ಹುಡುಕು(name);
    ಯದಿ (i >= 0)
	ಪ್ರತ್ಯಾಗಮ enc_canon_table[i].prop;
#ifdef MSWIN
    ಯದಿ (name[0] == 'c' && name[1] == 'p' && VIM_ISDIGIT(name[2]))
    {
	CPINFO	cpinfo;

	// Get info on this codepage to find out what it is.
	ಯದಿ (GetCPInfo(atoi((ವರ್ಣ *)name + 2), &cpinfo) != 0)
	{
	    ಯದಿ (cpinfo.MaxCharSize == 1) // some single-byte encoding
		ಪ್ರತ್ಯಾಗಮ ENC_8BIT;
	    ಯದಿ (cpinfo.MaxCharSize == 2
		    && (cpinfo.LeadByte[0] != 0 || cpinfo.LeadByte[1] != 0))
		// must be a DBCS encoding
		ಪ್ರತ್ಯಾಗಮ ENC_DBCS;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
    }
#endif
    ಯದಿ (STRNCMP(name, "2byte-", 6) == 0)
	ಪ್ರತ್ಯಾಗಮ ENC_DBCS;
    ಯದಿ (STRNCMP(name, "8bit-", 5) == 0 || STRNCMP(name, "iso-8859-", 9) == 0)
	ಪ್ರತ್ಯಾಗಮ ENC_8BIT;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Set up for using multi-byte characters.
 * Called in three cases:
 * - by main() to initialize (p_enc == NULL)
 * - by set_init_1() after 'encoding' was set to its default.
 * - by do_set() when 'encoding' has been set.
 * p_enc must have been passed through enc_canonize() already.
 * Sets the "enc_unicode", "enc_utf8", "enc_dbcs" and "has_mbyte" flags.
 * Fills mb_bytelen_tab[] and returns NULL when there are no problems.
 * When there is something wrong: Returns an error message and doesn't change
 * anything.
 */
    ವರ್ಣ *
mb_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		enc_dbcs_new = 0;
#ಯದಿ defined(USE_ICONV) && !defined(MSWIN) && !defined(WIN32UNIX) \
	&& !defined(MACOS_CONVERT)
# define LEN_FROM_CONV
    vimconv_T	vimconv;
    char_u	*p;
#endif

    ಯದಿ (p_enc == ರಿಕ್ತ)
    {
	// Just starting up: set the whole table to one's.
	ಪ್ರತಿ (i = 0; i < 256; ++i)
	    mb_bytelen_tab[i] = 1;
	ಒಳಹರಿವು_conv.vc_type = CONV_NONE;
	ಒಳಹರಿವು_conv.vc_factor = 1;
	output_conv.vc_type = CONV_NONE;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef MSWIN
    ಯದಿ (p_enc[0] == 'c' && p_enc[1] == 'p' && VIM_ISDIGIT(p_enc[2]))
    {
	CPINFO	cpinfo;

	// Get info on this codepage to find out what it is.
	ಯದಿ (GetCPInfo(atoi((ವರ್ಣ *)p_enc + 2), &cpinfo) != 0)
	{
	    ಯದಿ (cpinfo.MaxCharSize == 1)
	    {
		// some single-byte encoding
		enc_unicode = 0;
		enc_utf8 = FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (cpinfo.MaxCharSize == 2
		    && (cpinfo.LeadByte[0] != 0 || cpinfo.LeadByte[1] != 0))
	    {
		// must be a DBCS encoding, check below
		enc_dbcs_new = atoi((ವರ್ಣ *)p_enc + 2);
	    }
	    ಅನ್ಯಥಾ
		ಗಚ್ಛ codepage_invalid;
	}
	ಅನ್ಯಥಾ ಯದಿ (GetLastError() == ERROR_INVALID_PARAMETER)
	{
codepage_invalid:
	    ಪ್ರತ್ಯಾಗಮ N_(e_not_valid_codepage);
	}
    }
#endif
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p_enc, "8bit-", 5) == 0
	    || STRNCMP(p_enc, "iso-8859-", 9) == 0)
    {
	// Accept any "8bit-" or "iso-8859-" name.
	enc_unicode = 0;
	enc_utf8 = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p_enc, "2byte-", 6) == 0)
    {
#ifdef MSWIN
	// Windows: accept only valid codepage numbers, check below.
	ಯದಿ (p_enc[6] != 'c' || p_enc[7] != 'p'
			      || (enc_dbcs_new = atoi((ವರ್ಣ *)p_enc + 8)) == 0)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
#ಅನ್ಯಥಾ
	// Unix: accept any "2byte-" name, assume current locale.
	enc_dbcs_new = DBCS_2BYTE;
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ ((idx = enc_canon_ಹುಡುಕು(p_enc)) >= 0)
    {
	i = enc_canon_table[idx].prop;
	ಯದಿ (i & ENC_UNICODE)
	{
	    // Unicode
	    enc_utf8 = TRUE;
	    ಯದಿ (i & (ENC_2BYTE | ENC_2WORD))
		enc_unicode = 2;
	    ಅನ್ಯಥಾ ಯದಿ (i & ENC_4BYTE)
		enc_unicode = 4;
	    ಅನ್ಯಥಾ
		enc_unicode = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (i & ENC_DBCS)
	{
	    // 2byte, handle below
	    enc_dbcs_new = enc_canon_table[idx].codepage;
	}
	ಅನ್ಯಥಾ
	{
	    // Must be 8-bit.
	    enc_unicode = 0;
	    enc_utf8 = FALSE;
	}
    }
    ಅನ್ಯಥಾ    // Don't know what encoding this is, reject it.
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಯದಿ (enc_dbcs_new != 0)
    {
#ifdef MSWIN
	// Check if the DBCS code page is OK.
	ಯದಿ (!IsValidCodePage(enc_dbcs_new))
	    ಗಚ್ಛ codepage_invalid;
#endif
	enc_unicode = 0;
	enc_utf8 = FALSE;
    }
    enc_dbcs = enc_dbcs_new;
    has_mbyte = (enc_dbcs != 0 || enc_utf8);

#ಯದಿ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
    enc_codepage = encname2codepage(p_enc);
    enc_latin9 = (STRCMP(p_enc, "iso-8859-15") == 0);
#endif

    // Detect an encoding that uses latin1 characters.
    enc_latin1like = (enc_utf8 || STRCMP(p_enc, "latin1") == 0
					|| STRCMP(p_enc, "iso-8859-15") == 0);

    /*
     * Set the function pointers.
     */
    ಯದಿ (enc_utf8)
    {
	mb_ptr2len = utfc_ptr2len;
	mb_ptr2len_len = utfc_ptr2len_len;
	mb_char2len = utf_char2len;
	mb_char2bytes = utf_char2bytes;
	mb_ptr2cells = utf_ptr2cells;
	mb_ptr2cells_len = utf_ptr2cells_len;
	mb_char2cells = utf_char2cells;
	mb_off2cells = utf_off2cells;
	mb_ptr2char = utf_ptr2char;
	mb_head_off = utf_head_off;
    }
    ಅನ್ಯಥಾ ಯದಿ (enc_dbcs != 0)
    {
	mb_ptr2len = dbcs_ptr2len;
	mb_ptr2len_len = dbcs_ptr2len_len;
	mb_char2len = dbcs_char2len;
	mb_char2bytes = dbcs_char2bytes;
	mb_ptr2cells = dbcs_ptr2cells;
	mb_ptr2cells_len = dbcs_ptr2cells_len;
	mb_char2cells = dbcs_char2cells;
	mb_off2cells = dbcs_off2cells;
	mb_ptr2char = dbcs_ptr2char;
	mb_head_off = dbcs_head_off;
    }
    ಅನ್ಯಥಾ
    {
	mb_ptr2len = latin_ptr2len;
	mb_ptr2len_len = latin_ptr2len_len;
	mb_char2len = latin_char2len;
	mb_char2bytes = latin_char2bytes;
	mb_ptr2cells = latin_ptr2cells;
	mb_ptr2cells_len = latin_ptr2cells_len;
	mb_char2cells = latin_char2cells;
	mb_off2cells = latin_off2cells;
	mb_ptr2char = latin_ptr2char;
	mb_head_off = latin_head_off;
    }

    /*
     * Fill the mb_bytelen_tab[] for MB_BYTE2LEN().
     */
#ifdef LEN_FROM_CONV
    // When 'encoding' is different from the current locale mblen() won't
    // work.  Use conversion to "utf-8" instead.
    vimconv.vc_type = CONV_NONE;
    ಯದಿ (enc_dbcs)
    {
	p = enc_locale();
	ಯದಿ (p == ರಿಕ್ತ || STRCMP(p, p_enc) != 0)
	{
	    convert_setup(&vimconv, p_enc, (char_u *)"utf-8");
	    vimconv.vc_fail = TRUE;
	}
	vim_free(p);
    }
#endif

    ಪ್ರತಿ (i = 0; i < 256; ++i)
    {
	// Our own function to reliably check the length of UTF-8 characters,
	// independent of mblen().
	ಯದಿ (enc_utf8)
	    n = utf8len_tab[i];
	ಅನ್ಯಥಾ ಯದಿ (enc_dbcs == 0)
	    n = 1;
	ಅನ್ಯಥಾ
	{
#ಯದಿ defined(MSWIN) || defined(WIN32UNIX)
	    // enc_dbcs is set by setting 'fileencoding'.  It becomes a Windows
	    // CodePage identifier, which we can pass directly in to Windows
	    // API
	    n = IsDBCSLeadByteEx(enc_dbcs, (WINBYTE)i) ? 2 : 1;
#ಅನ್ಯಥಾ
# ಯದಿ defined(__amigaos4__) || defined(__ANDROID__) || \
				   !(defined(HAVE_MBLEN) || defined(X_LOCALE))
	    /*
	     * if mblen() is not available, character which MSB is turned on
	     * are treated as leading byte character. (note : This assumption
	     * is not always true.)
	     */
	    n = (i & 0x80) ? 2 : 1;
# ಅನ್ಯಥಾ
	    ವರ್ಣ buf[MB_MAXBYTES + 1];

	    ಯದಿ (i == NUL)	// just in case mblen() can't handle ""
		n = 1;
	    ಅನ್ಯಥಾ
	    {
		buf[0] = i;
		buf[1] = 0;
#  ifdef LEN_FROM_CONV
		ಯದಿ (vimconv.vc_type != CONV_NONE)
		{
		    /*
		     * string_convert() should fail when converting the first
		     * byte of a double-byte character.
		     */
		    p = string_convert(&vimconv, (char_u *)buf, ರಿಕ್ತ);
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			vim_free(p);
			n = 1;
		    }
		    ಅನ್ಯಥಾ
			n = 2;
		}
		ಅನ್ಯಥಾ
#  endif
		{
		    /*
		     * mblen() should return -1 for invalid (means the leading
		     * multibyte) character.  However there are some platforms
		     * where mblen() returns 0 for invalid character.
		     * Therefore, following condition includes 0.
		     */
		    vim_ignored = mblen(ರಿಕ್ತ, 0);  // First reset the state.
		    ಯದಿ (mblen(buf, (size_t)1) <= 0)
			n = 2;
		    ಅನ್ಯಥಾ
			n = 1;
		}
	    }
# endif
#endif
	}

	mb_bytelen_tab[i] = n;
    }

#ifdef LEN_FROM_CONV
    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
#endif

    // The cell width depends on the type of multi-byte characters.
    (ಶೂನ್ಯ)init_chartab();

    // When enc_utf8 is set or reset, (de)allocate ScreenLinesUC[]
    screenalloc(FALSE);

    // When using Unicode, set default for 'fileencodings'.
    ಯದಿ (enc_utf8 && !option_was_set((char_u *)"fencs"))
	set_fencs_unicode();

#ಯದಿ defined(HAVE_BIND_TEXTDOMAIN_CODESET) && defined(FEAT_GETTEXT)
    // GNU gettext 0.10.37 supports this feature: set the codeset used for
    // translated messages independently from the current locale.
    (ಶೂನ್ಯ)bind_textdomain_codeset(VIMPACKAGE,
					  enc_utf8 ? "utf-8" : (ವರ್ಣ *)p_enc);
#endif

#ifdef MSWIN
    // When changing 'encoding' while starting up, then convert the command
    // line arguments from the active codepage to 'encoding'.
    ಯದಿ (starting != 0)
	fix_arg_enc();
#endif

    // Fire an autocommand to let people do custom font setup. This must be
    // after Vim has been setup for the new encoding.
    apply_autocmds(EVENT_ENCODINGCHANGED, ರಿಕ್ತ, (char_u *)"", FALSE, curbuf);

#ifdef FEAT_SPELL
    // Need to reload spell dictionaries
    spell_reload();
#endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return the size of the BOM for the current buffer:
 * 0 - no BOM
 * 2 - UCS-2 or UTF-16 BOM
 * 4 - UCS-4 BOM
 * 3 - UTF-8 BOM
 */
    ಪೂರ್ಣಾಙ್ಕ
bomb_size(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ n = 0;

    ಯದಿ (curbuf->b_p_bomb && !curbuf->b_p_bin)
    {
	ಯದಿ (*curbuf->b_p_fenc == NUL)
	{
	    ಯದಿ (enc_utf8)
	    {
		ಯದಿ (enc_unicode != 0)
		    n = enc_unicode;
		ಅನ್ಯಥಾ
		    n = 3;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(curbuf->b_p_fenc, "utf-8") == 0)
	    n = 3;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(curbuf->b_p_fenc, "ucs-2", 5) == 0
		|| STRNCMP(curbuf->b_p_fenc, "utf-16", 6) == 0)
	    n = 2;
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(curbuf->b_p_fenc, "ucs-4", 5) == 0)
	    n = 4;
    }
    ಪ್ರತ್ಯಾಗಮ n;
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Remove all BOM from "s" by moving remaining text.
 */
    ಶೂನ್ಯ
remove_bom(char_u *s)
{
    ಯದಿ (!enc_utf8)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *p = s;

    ಯಾವತ್ ((p = vim_strbyte(p, 0xef)) != ರಿಕ್ತ)
    {
	ಯದಿ (p[1] == 0xbb && p[2] == 0xbf)
	    STRMOVE(p, p + 3);
	ಅನ್ಯಥಾ
	    ++p;
    }
}
#endif

/*
 * Get class of pointer:
 * 0 for blank or NUL
 * 1 for punctuation
 * 2 for an alphanumeric word character
 * >2 for other word characters, including CJK and emoji
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_get_class(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ mb_get_class_buf(p, curbuf);
}

    ಪೂರ್ಣಾಙ್ಕ
mb_get_class_buf(char_u *p, buf_T *buf)
{
    ಯದಿ (MB_BYTE2LEN(p[0]) == 1)
    {
	ಯದಿ (p[0] == NUL || VIM_ISWHITE(p[0]))
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (vim_iswordc_buf(p[0], buf))
	    ಪ್ರತ್ಯಾಗಮ 2;
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಯದಿ (enc_dbcs != 0 && p[0] != NUL && p[1] != NUL)
	ಪ್ರತ್ಯಾಗಮ dbcs_class(p[0], p[1]);
    ಯದಿ (enc_utf8)
	ಪ್ರತ್ಯಾಗಮ utf_class_buf(utf_ptr2char(p), buf);
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Get class of a double-byte character.  This always returns 3 or bigger.
 * TODO: Should return 1 for punctuation.
 */
    ಪೂರ್ಣಾಙ್ಕ
dbcs_class(unsigned lead, unsigned trail)
{
    ವಿಕಲ್ಪ (enc_dbcs)
    {
	// please add classify routine for your language in here

	ಪಕ್ಷ DBCS_JPNU:	// ?
	ಪಕ್ಷ DBCS_JPN:
	    {
		// JIS code classification
		unsigned ವರ್ಣ lb = lead;
		unsigned ವರ್ಣ tb = trail;

		// convert process code to JIS
#ಯದಿ defined(MSWIN) || defined(WIN32UNIX) || defined(MACOS_X)
		// process code is SJIS
		ಯದಿ (lb <= 0x9f)
		    lb = (lb - 0x81) * 2 + 0x21;
		ಅನ್ಯಥಾ
		    lb = (lb - 0xc1) * 2 + 0x21;
		ಯದಿ (tb <= 0x7e)
		    tb -= 0x1f;
		ಅನ್ಯಥಾ ಯದಿ (tb <= 0x9e)
		    tb -= 0x20;
		ಅನ್ಯಥಾ
		{
		    tb -= 0x7e;
		    lb += 1;
		}
#ಅನ್ಯಥಾ
		/*
		 * XXX: Code page identification can not use with all
		 *	    system! So, some other encoding information
		 *	    will be needed.
		 *	    In japanese: SJIS,EUC,UNICODE,(JIS)
		 *	    Note that JIS-code system don't use as
		 *	    process code in most system because it uses
		 *	    escape sequences(JIS is context depend encoding).
		 */
		// assume process code is JAPANESE-EUC
		lb &= 0x7f;
		tb &= 0x7f;
#endif
		// exceptions
		ವಿಕಲ್ಪ (lb << 8 | tb)
		{
		    ಪಕ್ಷ 0x2121: // ZENKAKU space
			ಪ್ರತ್ಯಾಗಮ 0;
		    ಪಕ್ಷ 0x2122: // TOU-TEN (Japanese comma)
		    ಪಕ್ಷ 0x2123: // KU-TEN (Japanese period)
		    ಪಕ್ಷ 0x2124: // ZENKAKU comma
		    ಪಕ್ಷ 0x2125: // ZENKAKU period
			ಪ್ರತ್ಯಾಗಮ 1;
		    ಪಕ್ಷ 0x213c: // prolongedsound handled as KATAKANA
			ಪ್ರತ್ಯಾಗಮ 13;
		}
		// sieved by KU code
		ವಿಕಲ್ಪ (lb)
		{
		    ಪಕ್ಷ 0x21:
		    ಪಕ್ಷ 0x22:
			// special symbols
			ಪ್ರತ್ಯಾಗಮ 10;
		    ಪಕ್ಷ 0x23:
			// alphanumeric
			ಪ್ರತ್ಯಾಗಮ 11;
		    ಪಕ್ಷ 0x24:
			// hiragana
			ಪ್ರತ್ಯಾಗಮ 12;
		    ಪಕ್ಷ 0x25:
			// katakana
			ಪ್ರತ್ಯಾಗಮ 13;
		    ಪಕ್ಷ 0x26:
			// greek
			ಪ್ರತ್ಯಾಗಮ 14;
		    ಪಕ್ಷ 0x27:
			// russian
			ಪ್ರತ್ಯಾಗಮ 15;
		    ಪಕ್ಷ 0x28:
			//ಸಾಲುಗಳು
			ಪ್ರತ್ಯಾಗಮ 16;
		    ಸ್ವಭಾವ:
			// kanji
			ಪ್ರತ್ಯಾಗಮ 17;
		}
	    }

	ಪಕ್ಷ DBCS_KORU:	// ?
	ಪಕ್ಷ DBCS_KOR:
	    {
		// KS code classification
		unsigned ವರ್ಣ c1 = lead;
		unsigned ವರ್ಣ c2 = trail;

		/*
		 * 20 : Hangul
		 * 21 : Hanja
		 * 22 : Symbols
		 * 23 : Alphanumeric/Roman Letter (Full width)
		 * 24 : Hangul Letter(Alphabet)
		 * 25 : Roman Numeral/Greek Letter
		 * 26 : Box Drawings
		 * 27 : Unit Symbols
		 * 28 : Circled/Parenthesized Letter
		 * 29 : Hiragana/Katakana
		 * 30 : Cyrillic Letter
		 */

		ಯದಿ (c1 >= 0xB0 && c1 <= 0xC8)
		    // Hangul
		    ಪ್ರತ್ಯಾಗಮ 20;
#ಯದಿ defined(MSWIN) || defined(WIN32UNIX)
		ಅನ್ಯಥಾ ಯದಿ (c1 <= 0xA0 || c2 <= 0xA0)
		    // Extended Hangul Region : MS UHC(Unified Hangul Code)
		    // c1: 0x81-0xA0 with c2: 0x41-0x5A, 0x61-0x7A, 0x81-0xFE
		    // c1: 0xA1-0xC6 with c2: 0x41-0x5A, 0x61-0x7A, 0x81-0xA0
		    ಪ್ರತ್ಯಾಗಮ 20;
#endif

		ಅನ್ಯಥಾ ಯದಿ (c1 >= 0xCA && c1 <= 0xFD)
		    // Hanja
		    ಪ್ರತ್ಯಾಗಮ 21;
		ಅನ್ಯಥಾ ವಿಕಲ್ಪ (c1)
		{
		    ಪಕ್ಷ 0xA1:
		    ಪಕ್ಷ 0xA2:
			// Symbols
			ಪ್ರತ್ಯಾಗಮ 22;
		    ಪಕ್ಷ 0xA3:
			// Alphanumeric
			ಪ್ರತ್ಯಾಗಮ 23;
		    ಪಕ್ಷ 0xA4:
			// Hangul Letter(Alphabet)
			ಪ್ರತ್ಯಾಗಮ 24;
		    ಪಕ್ಷ 0xA5:
			// Roman Numeral/Greek Letter
			ಪ್ರತ್ಯಾಗಮ 25;
		    ಪಕ್ಷ 0xA6:
			// Box Drawings
			ಪ್ರತ್ಯಾಗಮ 26;
		    ಪಕ್ಷ 0xA7:
			// Unit Symbols
			ಪ್ರತ್ಯಾಗಮ 27;
		    ಪಕ್ಷ 0xA8:
		    ಪಕ್ಷ 0xA9:
			ಯದಿ (c2 <= 0xAF)
			    ಪ್ರತ್ಯಾಗಮ 25;  // Roman Letter
			ಅನ್ಯಥಾ ಯದಿ (c2 >= 0xF6)
			    ಪ್ರತ್ಯಾಗಮ 22;  // Symbols
			ಅನ್ಯಥಾ
			    // Circled/Parenthesized Letter
			    ಪ್ರತ್ಯಾಗಮ 28;
		    ಪಕ್ಷ 0xAA:
		    ಪಕ್ಷ 0xAB:
			// Hiragana/Katakana
			ಪ್ರತ್ಯಾಗಮ 29;
		    ಪಕ್ಷ 0xAC:
			// Cyrillic Letter
			ಪ್ರತ್ಯಾಗಮ 30;
		}
	    }
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ 3;
}

/*
 * mb_char2len() function pointer.
 * Return length in bytes of character "c".
 * Returns 1 for a single-byte character.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_char2len(ಪೂರ್ಣಾಙ್ಕ c UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_char2len(
    ಪೂರ್ಣಾಙ್ಕ		c)
{
    ಯದಿ (c >= 0x100)
	ಪ್ರತ್ಯಾಗಮ 2;
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * mb_char2bytes() function pointer.
 * Convert a character to its bytes.
 * Returns the length in bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_char2bytes(ಪೂರ್ಣಾಙ್ಕ c, char_u *buf)
{
    buf[0] = c;
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_char2bytes(ಪೂರ್ಣಾಙ್ಕ c, char_u *buf)
{
    ಯದಿ (c >= 0x100)
    {
	buf[0] = (unsigned)c >> 8;
	buf[1] = c;
	// Never use a NUL byte, it causes lots of trouble.  It's an invalid
	// character anyway.
	ಯದಿ (buf[1] == NUL)
	    buf[1] = '\n';
	ಪ್ರತ್ಯಾಗಮ 2;
    }
    buf[0] = c;
    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Get byte length of character at "*p".  Returns zero when "*p" is NUL.
 * Used for mb_ptr2len() when 'encoding' latin.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_ptr2len(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ *p == NUL ? 0 : 1;
}

/*
 * Get byte length of character at "*p".  Returns zero when "*p" is NUL.
 * Used for mb_ptr2len() when 'encoding' DBCS.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_ptr2len(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // if the second byte is missing the length is 1
    len = MB_BYTE2LEN(*p);
    ಯದಿ (len == 2 && p[1] == NUL)
	len = 1;
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * mb_ptr2len_len() function pointer.
 * Like mb_ptr2len(), but limit to read "size" bytes.
 * Returns 0 for an empty string.
 * Returns 1 for an illegal char or an incomplete byte sequence.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_ptr2len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    ಯದಿ (size < 1 || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_ptr2len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (size < 1 || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (size == 1)
	ಪ್ರತ್ಯಾಗಮ 1;
    // Check that second byte is not missing.
    len = MB_BYTE2LEN(*p);
    ಯದಿ (len == 2 && p[1] == NUL)
	len = 1;
    ಪ್ರತ್ಯಾಗಮ len;
}

ಸಂರಚನಾ interval
{
    long first;
    long last;
};

/*
 * Return TRUE if "c" is in "table[size / sizeof(struct interval)]".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
intable(ಸಂರಚನಾ interval *table, size_t size, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ mid, bot, ತುದಿ;

    // first quick check for Latin1 etc. characters
    ಯದಿ (c < table[0].first)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // binary search in table
    bot = 0;
    ತುದಿ = (ಪೂರ್ಣಾಙ್ಕ)(size / ಪರಿಮಾಣ(ಸಂರಚನಾ interval) - 1);
    ಯಾವತ್ (ತುದಿ >= bot)
    {
	mid = (bot + ತುದಿ) / 2;
	ಯದಿ (table[mid].last < c)
	    bot = mid + 1;
	ಅನ್ಯಥಾ ಯದಿ (table[mid].first > c)
	    ತುದಿ = mid - 1;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// Sorted list of non-overlapping intervals of East Asian Ambiguous
// characters, generated with ../runtime/tools/unicode.vim.
ಸ್ಥಿರ ಸಂರಚನಾ interval ambiguous[] =
{
    {0x00a1, 0x00a1},
    {0x00a4, 0x00a4},
    {0x00a7, 0x00a8},
    {0x00aa, 0x00aa},
    {0x00ad, 0x00ae},
    {0x00b0, 0x00b4},
    {0x00b6, 0x00ba},
    {0x00bc, 0x00bf},
    {0x00c6, 0x00c6},
    {0x00d0, 0x00d0},
    {0x00d7, 0x00d8},
    {0x00de, 0x00e1},
    {0x00e6, 0x00e6},
    {0x00e8, 0x00ea},
    {0x00ec, 0x00ed},
    {0x00f0, 0x00f0},
    {0x00f2, 0x00f3},
    {0x00f7, 0x00fa},
    {0x00fc, 0x00fc},
    {0x00fe, 0x00fe},
    {0x0101, 0x0101},
    {0x0111, 0x0111},
    {0x0113, 0x0113},
    {0x011b, 0x011b},
    {0x0126, 0x0127},
    {0x012b, 0x012b},
    {0x0131, 0x0133},
    {0x0138, 0x0138},
    {0x013f, 0x0142},
    {0x0144, 0x0144},
    {0x0148, 0x014b},
    {0x014d, 0x014d},
    {0x0152, 0x0153},
    {0x0166, 0x0167},
    {0x016b, 0x016b},
    {0x01ce, 0x01ce},
    {0x01d0, 0x01d0},
    {0x01d2, 0x01d2},
    {0x01d4, 0x01d4},
    {0x01d6, 0x01d6},
    {0x01d8, 0x01d8},
    {0x01da, 0x01da},
    {0x01dc, 0x01dc},
    {0x0251, 0x0251},
    {0x0261, 0x0261},
    {0x02c4, 0x02c4},
    {0x02c7, 0x02c7},
    {0x02c9, 0x02cb},
    {0x02cd, 0x02cd},
    {0x02d0, 0x02d0},
    {0x02d8, 0x02db},
    {0x02dd, 0x02dd},
    {0x02df, 0x02df},
    {0x0300, 0x036f},
    {0x0391, 0x03a1},
    {0x03a3, 0x03a9},
    {0x03b1, 0x03c1},
    {0x03c3, 0x03c9},
    {0x0401, 0x0401},
    {0x0410, 0x044f},
    {0x0451, 0x0451},
    {0x2010, 0x2010},
    {0x2013, 0x2016},
    {0x2018, 0x2019},
    {0x201c, 0x201d},
    {0x2020, 0x2022},
    {0x2024, 0x2027},
    {0x2030, 0x2030},
    {0x2032, 0x2033},
    {0x2035, 0x2035},
    {0x203b, 0x203b},
    {0x203e, 0x203e},
    {0x2074, 0x2074},
    {0x207f, 0x207f},
    {0x2081, 0x2084},
    {0x20ac, 0x20ac},
    {0x2103, 0x2103},
    {0x2105, 0x2105},
    {0x2109, 0x2109},
    {0x2113, 0x2113},
    {0x2116, 0x2116},
    {0x2121, 0x2122},
    {0x2126, 0x2126},
    {0x212b, 0x212b},
    {0x2153, 0x2154},
    {0x215b, 0x215e},
    {0x2160, 0x216b},
    {0x2170, 0x2179},
    {0x2189, 0x2189},
    {0x2190, 0x2199},
    {0x21b8, 0x21b9},
    {0x21d2, 0x21d2},
    {0x21d4, 0x21d4},
    {0x21e7, 0x21e7},
    {0x2200, 0x2200},
    {0x2202, 0x2203},
    {0x2207, 0x2208},
    {0x220b, 0x220b},
    {0x220f, 0x220f},
    {0x2211, 0x2211},
    {0x2215, 0x2215},
    {0x221a, 0x221a},
    {0x221d, 0x2220},
    {0x2223, 0x2223},
    {0x2225, 0x2225},
    {0x2227, 0x222c},
    {0x222e, 0x222e},
    {0x2234, 0x2237},
    {0x223c, 0x223d},
    {0x2248, 0x2248},
    {0x224c, 0x224c},
    {0x2252, 0x2252},
    {0x2260, 0x2261},
    {0x2264, 0x2267},
    {0x226a, 0x226b},
    {0x226e, 0x226f},
    {0x2282, 0x2283},
    {0x2286, 0x2287},
    {0x2295, 0x2295},
    {0x2299, 0x2299},
    {0x22a5, 0x22a5},
    {0x22bf, 0x22bf},
    {0x2312, 0x2312},
    {0x2460, 0x24e9},
    {0x24eb, 0x254b},
    {0x2550, 0x2573},
    {0x2580, 0x258f},
    {0x2592, 0x2595},
    {0x25a0, 0x25a1},
    {0x25a3, 0x25a9},
    {0x25b2, 0x25b3},
    {0x25b6, 0x25b7},
    {0x25bc, 0x25bd},
    {0x25c0, 0x25c1},
    {0x25c6, 0x25c8},
    {0x25cb, 0x25cb},
    {0x25ce, 0x25d1},
    {0x25e2, 0x25e5},
    {0x25ef, 0x25ef},
    {0x2605, 0x2606},
    {0x2609, 0x2609},
    {0x260e, 0x260f},
    {0x261c, 0x261c},
    {0x261e, 0x261e},
    {0x2640, 0x2640},
    {0x2642, 0x2642},
    {0x2660, 0x2661},
    {0x2663, 0x2665},
    {0x2667, 0x266a},
    {0x266c, 0x266d},
    {0x266f, 0x266f},
    {0x269e, 0x269f},
    {0x26bf, 0x26bf},
    {0x26c6, 0x26cd},
    {0x26cf, 0x26d3},
    {0x26d5, 0x26e1},
    {0x26e3, 0x26e3},
    {0x26e8, 0x26e9},
    {0x26eb, 0x26f1},
    {0x26f4, 0x26f4},
    {0x26f6, 0x26f9},
    {0x26fb, 0x26fc},
    {0x26fe, 0x26ff},
    {0x273d, 0x273d},
    {0x2776, 0x277f},
    {0x2b56, 0x2b59},
    {0x3248, 0x324f},
    {0xe000, 0xf8ff},
    {0xfe00, 0xfe0f},
    {0xfffd, 0xfffd},
    {0x1f100, 0x1f10a},
    {0x1f110, 0x1f12d},
    {0x1f130, 0x1f169},
    {0x1f170, 0x1f18d},
    {0x1f18f, 0x1f190},
    {0x1f19b, 0x1f1ac},
    {0xe0100, 0xe01ef},
    {0xf0000, 0xffffd},
    {0x100000, 0x10fffd}
};

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * utf_char2cells() with different argument type for libvterm.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_uint2cells(UINT32_T c)
{
    ಯದಿ (c >= 0x100 && utf_iscomposing((ಪೂರ್ಣಾಙ್ಕ)c))
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ utf_char2cells((ಪೂರ್ಣಾಙ್ಕ)c);
}
#endif

/*
 * For UTF-8 character "c" return 2 for a double-width character, 1 for others.
 * Returns 4 or 6 for an unprintable character.
 * Is only correct for characters >= 0x80.
 * When p_ambw is "double", return 2 for a character with East Asian Width
 * class 'A'(mbiguous).
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_char2cells(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Sorted list of non-overlapping intervals of East Asian double width
    // characters, generated with ../runtime/tools/unicode.vim.
    ಸ್ಥಿರ ಸಂರಚನಾ interval doublewidth[] =
    {
	{0x1100, 0x115f},
	{0x231a, 0x231b},
	{0x2329, 0x232a},
	{0x23e9, 0x23ec},
	{0x23f0, 0x23f0},
	{0x23f3, 0x23f3},
	{0x25fd, 0x25fe},
	{0x2614, 0x2615},
	{0x2630, 0x2637},
	{0x2648, 0x2653},
	{0x267f, 0x267f},
	{0x268a, 0x268f},
	{0x2693, 0x2693},
	{0x26a1, 0x26a1},
	{0x26aa, 0x26ab},
	{0x26bd, 0x26be},
	{0x26c4, 0x26c5},
	{0x26ce, 0x26ce},
	{0x26d4, 0x26d4},
	{0x26ea, 0x26ea},
	{0x26f2, 0x26f3},
	{0x26f5, 0x26f5},
	{0x26fa, 0x26fa},
	{0x26fd, 0x26fd},
	{0x2705, 0x2705},
	{0x270a, 0x270b},
	{0x2728, 0x2728},
	{0x274c, 0x274c},
	{0x274e, 0x274e},
	{0x2753, 0x2755},
	{0x2757, 0x2757},
	{0x2795, 0x2797},
	{0x27b0, 0x27b0},
	{0x27bf, 0x27bf},
	{0x2b1b, 0x2b1c},
	{0x2b50, 0x2b50},
	{0x2b55, 0x2b55},
	{0x2e80, 0x2e99},
	{0x2e9b, 0x2ef3},
	{0x2f00, 0x2fd5},
	{0x2ff0, 0x303e},
	{0x3041, 0x3096},
	{0x3099, 0x30ff},
	{0x3105, 0x312f},
	{0x3131, 0x318e},
	{0x3190, 0x31e5},
	{0x31ef, 0x321e},
	{0x3220, 0x3247},
	{0x3250, 0xa48c},
	{0xa490, 0xa4c6},
	{0xa960, 0xa97c},
	{0xac00, 0xd7a3},
	{0xf900, 0xfaff},
	{0xfe10, 0xfe19},
	{0xfe30, 0xfe52},
	{0xfe54, 0xfe66},
	{0xfe68, 0xfe6b},
	{0xff01, 0xff60},
	{0xffe0, 0xffe6},
	{0x16fe0, 0x16fe3},
	{0x16ff0, 0x16ff1},
	{0x17000, 0x187f7},
	{0x18800, 0x18cd5},
	{0x18cff, 0x18d08},
	{0x1aff0, 0x1aff3},
	{0x1aff5, 0x1affb},
	{0x1affd, 0x1affe},
	{0x1b000, 0x1b122},
	{0x1b132, 0x1b132},
	{0x1b150, 0x1b152},
	{0x1b155, 0x1b155},
	{0x1b164, 0x1b167},
	{0x1b170, 0x1b2fb},
	{0x1d300, 0x1d356},
	{0x1d360, 0x1d376},
	{0x1f004, 0x1f004},
	{0x1f0cf, 0x1f0cf},
	{0x1f18e, 0x1f18e},
	{0x1f191, 0x1f19a},
	{0x1f200, 0x1f202},
	{0x1f210, 0x1f23b},
	{0x1f240, 0x1f248},
	{0x1f250, 0x1f251},
	{0x1f260, 0x1f265},
	{0x1f300, 0x1f320},
	{0x1f32d, 0x1f335},
	{0x1f337, 0x1f37c},
	{0x1f37e, 0x1f393},
	{0x1f3a0, 0x1f3ca},
	{0x1f3cf, 0x1f3d3},
	{0x1f3e0, 0x1f3f0},
	{0x1f3f4, 0x1f3f4},
	{0x1f3f8, 0x1f43e},
	{0x1f440, 0x1f440},
	{0x1f442, 0x1f4fc},
	{0x1f4ff, 0x1f53d},
	{0x1f54b, 0x1f54e},
	{0x1f550, 0x1f567},
	{0x1f57a, 0x1f57a},
	{0x1f595, 0x1f596},
	{0x1f5a4, 0x1f5a4},
	{0x1f5fb, 0x1f64f},
	{0x1f680, 0x1f6c5},
	{0x1f6cc, 0x1f6cc},
	{0x1f6d0, 0x1f6d2},
	{0x1f6d5, 0x1f6d7},
	{0x1f6dc, 0x1f6df},
	{0x1f6eb, 0x1f6ec},
	{0x1f6f4, 0x1f6fc},
	{0x1f7e0, 0x1f7eb},
	{0x1f7f0, 0x1f7f0},
	{0x1f90c, 0x1f93a},
	{0x1f93c, 0x1f945},
	{0x1f947, 0x1f9ff},
	{0x1fa70, 0x1fa7c},
	{0x1fa80, 0x1fa89},
	{0x1fa8f, 0x1fac6},
	{0x1face, 0x1fadc},
	{0x1fadf, 0x1fae9},
	{0x1faf0, 0x1faf8},
	{0x20000, 0x2fffd},
	{0x30000, 0x3fffd}
    };

    // Sorted list of non-overlapping intervals of Emoji characters that don't
    // have ambiguous or double width,
    // based on http://unicode.org/emoji/charts/emoji-list.html
    ಸ್ಥಿರ ಸಂರಚನಾ interval emoji_wide[] =
    {
	{0x23ed, 0x23ef},
	{0x23f1, 0x23f2},
	{0x23f8, 0x23fa},
	{0x24c2, 0x24c2},
	{0x261d, 0x261d},
	{0x26c8, 0x26c8},
	{0x26cf, 0x26cf},
	{0x26d1, 0x26d1},
	{0x26d3, 0x26d3},
	{0x26e9, 0x26e9},
	{0x26f0, 0x26f1},
	{0x26f7, 0x26f9},
	{0x270c, 0x270d},
	{0x2934, 0x2935},
	{0x1f170, 0x1f189},
	{0x1f1e6, 0x1f1ff},
	{0x1f321, 0x1f321},
	{0x1f324, 0x1f32c},
	{0x1f336, 0x1f336},
	{0x1f37d, 0x1f37d},
	{0x1f396, 0x1f397},
	{0x1f399, 0x1f39b},
	{0x1f39e, 0x1f39f},
	{0x1f3cb, 0x1f3ce},
	{0x1f3d4, 0x1f3df},
	{0x1f3f3, 0x1f3f5},
	{0x1f3f7, 0x1f3f7},
	{0x1f43f, 0x1f43f},
	{0x1f441, 0x1f441},
	{0x1f4fd, 0x1f4fd},
	{0x1f549, 0x1f54a},
	{0x1f56f, 0x1f570},
	{0x1f573, 0x1f579},
	{0x1f587, 0x1f587},
	{0x1f58a, 0x1f58d},
	{0x1f590, 0x1f590},
	{0x1f5a5, 0x1f5a5},
	{0x1f5a8, 0x1f5a8},
	{0x1f5b1, 0x1f5b2},
	{0x1f5bc, 0x1f5bc},
	{0x1f5c2, 0x1f5c4},
	{0x1f5d1, 0x1f5d3},
	{0x1f5dc, 0x1f5de},
	{0x1f5e1, 0x1f5e1},
	{0x1f5e3, 0x1f5e3},
	{0x1f5e8, 0x1f5e8},
	{0x1f5ef, 0x1f5ef},
	{0x1f5f3, 0x1f5f3},
	{0x1f5fa, 0x1f5fa},
	{0x1f6cb, 0x1f6cf},
	{0x1f6e0, 0x1f6e5},
	{0x1f6e9, 0x1f6e9},
	{0x1f6f0, 0x1f6f0},
	{0x1f6f3, 0x1f6f3}

#ifdef MACOS_X
	// Include SF Symbols 4 characters, which should be rendered as
	// double-width.  SF Symbols is an Apple-specific set of symbols and
	// icons for use in Apple operating systems.  They are included as
	// glyphs as part of the default San Francisco fonts shipped with
	// macOS.  The current version is SF Symbols 4.
	//
	// These Apple-specific glyphs are not part of standard Unicode, and
	// all of them are in the Supplementary Private Use Area-B range. The
	// exact range was determined by downloading the 'SF Symbols 4' app
	// from Apple (https://developer.apple.com/sf-symbols/), and then
	// selecting all symbols, copying them out, and inspecting the unicode
	// values of them.
	//
	// Note that these symbols are of varying widths, as they are symbols
	// representing different things ranging from a simple gear icon to an
	// airplane. Some of them are in fact wider than double-width, but Vim
	// doesn't support non-fixed-width font, and tagging them as
	// double-width is the best way to handle them.
	//
	// Also see https://en.wikipedia.org/wiki/San_Francisco_(sans-serif_typeface)#SF_Symbols
	, {0x100000, 0x1018c7}
#endif
    };

#ifdef FEAT_EVAL
    // Use the value from setcellwidths() at 0x80 and higher, unless the
    // character is not printable.
    ಯದಿ (c >= 0x80 && vim_isprintc(c))
    {
	ಪೂರ್ಣಾಙ್ಕ n = cw_ಮೌಲ್ಯ(c);
	ಯದಿ (n != 0)
	    ಪ್ರತ್ಯಾಗಮ n;
    }
#endif

    ಯದಿ (c >= 0x100)
    {
	ಯದಿ (!utf_printable(c))
	    ಪ್ರತ್ಯಾಗಮ 6;		// unprintable, displays <xxxx>
	ಯದಿ (intable(doublewidth, ಪರಿಮಾಣ(doublewidth), c))
	    ಪ್ರತ್ಯಾಗಮ 2;
	ಯದಿ (p_emoji && intable(emoji_wide, ಪರಿಮಾಣ(emoji_wide), c))
	    ಪ್ರತ್ಯಾಗಮ 2;
    }

    // Characters below 0x100 are influenced by 'isprint' option
    ಅನ್ಯಥಾ ಯದಿ (c >= 0x80 && !vim_isprintc(c))
	ಪ್ರತ್ಯಾಗಮ 4;		// unprintable, displays <xx>

    ಯದಿ (c >= 0x80 && *p_ambw == 'd' && intable(ambiguous, ಪರಿಮಾಣ(ambiguous), c))
	ಪ್ರತ್ಯಾಗಮ 2;

    ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * mb_ptr2cells() function pointer.
 * Return the number of display cells character at "*p" occupies.
 * This doesn't take care of unprintable characters, use ptr2cells() for that.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_ptr2cells(char_u *p UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಪೂರ್ಣಾಙ್ಕ
utf_ptr2cells(
    char_u	*p)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Need to convert to a character number.
    ಯದಿ (*p >= 0x80)
    {
	c = utf_ptr2char_and_len(p, &len);
	// An illegal byte is displayed as <xx>.
	ಯದಿ (len == 1 || c == NUL)
	    ಪ್ರತ್ಯಾಗಮ 4;
	// If the char is ASCII it must be an overlong sequence.
	ಯದಿ (c < 0x80)
	    ಪ್ರತ್ಯಾಗಮ vim_isprintc(c) ? char2cells(c) : 4;
	ಪ್ರತ್ಯಾಗಮ utf_char2cells(c);
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಪೂರ್ಣಾಙ್ಕ
dbcs_ptr2cells(char_u *p)
{
    // Number of cells is equal to number of bytes, except for euc-jp when
    // the first byte is 0x8e.
    ಯದಿ (enc_dbcs == DBCS_JPNU && *p == 0x8e)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ MB_BYTE2LEN(*p);
}

/*
 * mb_ptr2cells_len() function pointer.
 * Like mb_ptr2cells(), but limit string length to "size".
 * For an empty string or truncated character returns 1.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_ptr2cells_len(char_u *p UNUSED, ಪೂರ್ಣಾಙ್ಕ size UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf_ptr2cells_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Need to convert to a wide character.
    ಯದಿ (size > 0 && *p >= 0x80)
    {
	c = utf_ptr2char_and_len_len(p, size, &len);
	ಯದಿ (len > size)
	    ಪ್ರತ್ಯಾಗಮ 4;  // truncated, display as illegal byte
	// An illegal byte is displayed as <xx>.
	ಯದಿ (len == 1 || c == NUL)
	    ಪ್ರತ್ಯಾಗಮ 4;
	// If the char is ASCII it must be an overlong sequence.
	ಯದಿ (c < 0x80)
	    ಪ್ರತ್ಯಾಗಮ vim_isprintc(c) ? char2cells(c) : 4;
	ಪ್ರತ್ಯಾಗಮ utf_char2cells(c);
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_ptr2cells_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    // Number of cells is equal to number of bytes, except for euc-jp when
    // the first byte is 0x8e.
    ಯದಿ (size <= 1 || (enc_dbcs == DBCS_JPNU && *p == 0x8e))
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ MB_BYTE2LEN(*p);
}

/*
 * mb_char2cells() function pointer.
 * Return the number of display cells character "c" occupies.
 * Only takes care of multi-byte chars, not "^C" and such.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_char2cells(ಪೂರ್ಣಾಙ್ಕ c UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_char2cells(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Number of cells is equal to number of bytes, except for euc-jp when
    // the first byte is 0x8e.
    ಯದಿ (enc_dbcs == DBCS_JPNU && ((unsigned)c >> 8) == 0x8e)
	ಪ್ರತ್ಯಾಗಮ 1;
    // use the first byte
    ಪ್ರತ್ಯಾಗಮ MB_BYTE2LEN((unsigned)c >> 8);
}

/*
 * Return the number of cells occupied by string "p".
 * Stop at a NUL character.  When "len" >= 0 stop at character "p[len]".
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_string2cells(char_u *p, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ clen = 0;

    ಪ್ರತಿ (i = 0; (len < 0 || i < len) && p[i] != NUL; i += (*mb_ptr2len)(p + i))
	clen += (*mb_ptr2cells)(p + i);
    ಪ್ರತ್ಯಾಗಮ clen;
}

/*
 * mb_off2cells() function pointer.
 * Return number of display cells for char at ScreenLines[off].
 * We make sure that the offset used is less than "max_off".
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_off2cells(unsigned off UNUSED, unsigned max_off UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಪೂರ್ಣಾಙ್ಕ
dbcs_off2cells(unsigned off, unsigned max_off)
{
    // never check beyond end of the line
    ಯದಿ (off >= max_off)
	ಪ್ರತ್ಯಾಗಮ 1;

    // Number of cells is equal to number of bytes, except for euc-jp when
    // the first byte is 0x8e.
    ಯದಿ (enc_dbcs == DBCS_JPNU && Screen_ಸಾಲುಗಳು[off] == 0x8e)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ MB_BYTE2LEN(Screen_ಸಾಲುಗಳು[off]);
}

    ಪೂರ್ಣಾಙ್ಕ
utf_off2cells(unsigned off, unsigned max_off)
{
    ಪ್ರತ್ಯಾಗಮ (off + 1 < max_off && Screen_ಸಾಲುಗಳು[off + 1] == 0) ? 2 : 1;
}

/*
 * mb_ptr2char() function pointer.
 * Convert a byte sequence into a character.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_ptr2char(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ *p;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_ptr2char(char_u *p)
{
    ಯದಿ (MB_BYTE2LEN(*p) > 1 && p[1] != NUL)
	ಪ್ರತ್ಯಾಗಮ (p[0] << 8) + p[1];
    ಪ್ರತ್ಯಾಗಮ *p;
}

/*
 * Convert a UTF-8 byte sequence to a character number.
 * Returns the character and sets "*lenp" to its byte length.
 * Illegal bytes are returned as-is with a length of one.
 */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
utf_ptr2char_and_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (p[0] < 0x80)
    {
	*lenp = p[0] == NUL ? 0 : 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    len = utf8len_tab_zero[p[0]];
    ಯದಿ (len <= 1 || (p[1] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
    ಯದಿ (len == 2)
    {
	*lenp = 2;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[2] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x0f) << 12) + ((p[1] & 0x3f) << 6) + (p[2] & 0x3f);
    ಯದಿ (len == 3)
    {
	*lenp = 3;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[3] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12)
				 + ((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
    ಯದಿ (len == 4)
    {
	*lenp = 4;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[4] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18)
				+ ((p[2] & 0x3f) << 12)
				+ ((p[3] & 0x3f) << 6) + (p[4] & 0x3f);
    ಯದಿ (len == 5)
    {
	*lenp = 5;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[5] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    *lenp = 6;
    ಪ್ರತ್ಯಾಗಮ ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24)
				+ ((p[2] & 0x3f) << 18)
				+ ((p[3] & 0x3f) << 12)
				+ ((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
}

/*
 * Like utf_ptr2char_and_len(), but never reads beyond "size" bytes.
 * For an incomplete sequence "*lenp" is set to the expected length.
 */
    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
utf_ptr2char_and_len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (size < 1)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ NUL;
    }
    ಯದಿ (p[0] < 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    len = utf8len_tab_zero[p[0]];
    ಯದಿ (len <= 1)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }
    ಯದಿ (len > size)
    {
	ಪೂರ್ಣಾಙ್ಕ		i;

	// Incomplete sequence: validate continuation bytes within range.
	ಪ್ರತಿ (i = 1; i < size; ++i)
	    ಯದಿ ((p[i] & 0xc0) != 0x80)
	    {
		*lenp = 1;
		ಪ್ರತ್ಯಾಗಮ p[0];
	    }
	*lenp = len;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }
    ಯದಿ ((p[1] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
    ಯದಿ (len == 2)
    {
	*lenp = 2;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[2] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x0f) << 12) + ((p[1] & 0x3f) << 6) + (p[2] & 0x3f);
    ಯದಿ (len == 3)
    {
	*lenp = 3;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[3] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12)
				 + ((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
    ಯದಿ (len == 4)
    {
	*lenp = 4;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[4] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    c = ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18)
				+ ((p[2] & 0x3f) << 12)
				+ ((p[3] & 0x3f) << 6) + (p[4] & 0x3f);
    ಯದಿ (len == 5)
    {
	*lenp = 5;
	ಪ್ರತ್ಯಾಗಮ c;
    }
    ಯದಿ ((p[5] & 0xc0) != 0x80)
    {
	*lenp = 1;
	ಪ್ರತ್ಯಾಗಮ p[0];
    }

    *lenp = 6;
    ಪ್ರತ್ಯಾಗಮ ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24)
				+ ((p[2] & 0x3f) << 18)
				+ ((p[3] & 0x3f) << 12)
				+ ((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
}

    ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ
utf_iscomposinglike_char(ಪೂರ್ಣಾಙ್ಕ c1, ಪೂರ್ಣಾಙ್ಕ c2)
{
    ಯದಿ (utf_iscomposing(c2))
	ಪ್ರತ್ಯಾಗಮ TRUE;
#ifdef FEAT_ARABIC
    ಯದಿ (!arabic_maycombine(c2))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ arabic_combine(c1, c2);
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)c1;
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Convert a UTF-8 byte sequence to a character number.
 * If the sequence is illegal or truncated by a NUL the first byte is
 * returned.
 * For an overlong sequence this may return zero.
 * Does not include composing characters, of course.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_ptr2char(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಪ್ರತ್ಯಾಗಮ utf_ptr2char_and_len(p, &len);
}

/*
 * Convert a UTF-8 byte sequence to a wide character.
 * String is assumed to be terminated by NUL or after "n" bytes, whichever
 * comes first.
 * The function is safe in the sense that it never accesses memory beyond the
 * first "n" bytes of "s".
 *
 * On success, returns decoded codepoint, advances "s" to the beginning of
 * next character and decreases "n" accordingly.
 *
 * If end of string was reached, returns 0 and, if "n" > 0, advances "s" past
 * NUL byte.
 *
 * If byte sequence is illegal or incomplete, returns -1 and does not advance
 * "s".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf_safe_read_char_adv(char_u **s, size_t *n)
{
    ಪೂರ್ಣಾಙ್ಕ		c, k;

    ಯದಿ (*n == 0) // end of buffer
	ಪ್ರತ್ಯಾಗಮ 0;

    k = utf8len_tab_zero[**s];

    ಯದಿ (k == 1)
    {
	// ASCII character or NUL
	(*n)--;
	ಪ್ರತ್ಯಾಗಮ *(*s)++;
    }

    ಯದಿ ((size_t)k <= *n)
    {
	// We have a multibyte sequence and it isn't truncated by buffer
	// limits so utf_ptr2char() is safe to use. Or the first byte is
	// illegal (k=0), and it's also safe to use utf_ptr2char().
	c = utf_ptr2char(*s);

	// On failure, utf_ptr2char() returns the first byte, so here we
	// check equality with the first byte. The only non-ASCII character
	// which equals the first byte of its own UTF-8 representation is
	// U+00C3 (UTF-8: 0xC3 0x83), so need to check that special case too.
	// It's safe even if n=1, else we would have k=2 > n.
	ಯದಿ (c != (ಪೂರ್ಣಾಙ್ಕ)(**s) || (c == 0xC3 && (*s)[1] == 0x83))
	{
	    // byte sequence was successfully decoded
	    *s += k;
	    *n -= k;
	    ಪ್ರತ್ಯಾಗಮ c;
	}
    }

    // byte sequence is incomplete or illegal
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Get character at **pp and advance *pp to the next character.
 * Note: composing characters are skipped!
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_ptr2char_adv(char_u **pp)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    c = (*mb_ptr2char)(*pp);
    *pp += (*mb_ptr2len)(*pp);
    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Get character at **pp and advance *pp to the next character.
 * Note: composing characters are returned as separate characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_cptr2char_adv(char_u **pp)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    c = (*mb_ptr2char)(*pp);
    ಯದಿ (enc_utf8)
	*pp += utf_ptr2len(*pp);
    ಅನ್ಯಥಾ
	*pp += (*mb_ptr2len)(*pp);
    ಪ್ರತ್ಯಾಗಮ c;
}

#ಯದಿ defined(FEAT_ARABIC)
/*
 * Check if the character pointed to by "p2" is a composing character when it
 * comes after "p1".  For Arabic sometimes "ab" is replaced with "c", which
 * behaves like a composing character.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_composinglike(char_u *p1, char_u *p2)
{
    ಪೂರ್ಣಾಙ್ಕ		c2;

    c2 = utf_ptr2char(p2);
    ಯದಿ (utf_iscomposing(c2))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (!arabic_maycombine(c2))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ arabic_combine(utf_ptr2char(p1), c2);
}
#endif

/*
 * Convert a UTF-8 byte string to a wide character.  Also get up to MAX_MCO
 * composing characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
utfc_ptr2char(
    char_u	*p,
    ಪೂರ್ಣಾಙ್ಕ		*pcc)	// return: composing chars, last one is 0
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		cc_len;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    c = utf_ptr2char_and_len(p, &len);

    // Only accept a composing char when the first char isn't illegal.
    ಯದಿ ((len > 1 || *p < 0x80) && p[len] >= 0x80)
    {
	cc = utf_ptr2char_and_len(p + len, &cc_len);
	ಯದಿ (utf_iscomposinglike_char(c, cc))
	{
	    ಪ್ರತಿ (;;)
	    {
		pcc[i++] = cc;
		ಯದಿ (i == MAX_MCO)
		    ಭಙ್ಗ;
		len += cc_len;
		ಯದಿ (p[len] < 0x80)
		    ಭಙ್ಗ;
		cc = utf_ptr2char_and_len(p + len, &cc_len);
		ಯದಿ (!utf_iscomposing(cc))
		    ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (i < MAX_MCO)	// last composing char must be 0
	pcc[i] = 0;

    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Convert a UTF-8 byte string to a wide character.  Also get up to MAX_MCO
 * composing characters.  Use no more than p[maxlen].
 */
    ಪೂರ್ಣಾಙ್ಕ
utfc_ptr2char_len(
    char_u	*p,
    ಪೂರ್ಣಾಙ್ಕ		*pcc,	// return: composing chars, last one is 0
    ಪೂರ್ಣಾಙ್ಕ		maxlen)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		cc_len;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    c = utf_ptr2char_and_len_len(p, maxlen, &len);
    // Only accept a composing char when the first char isn't illegal.
    ಯದಿ ((len > 1 || *p < 0x80) && len < maxlen && p[len] >= 0x80)
    {
	cc = utf_ptr2char_and_len_len(p + len, maxlen - len, &cc_len);
	ಯದಿ (cc_len <= maxlen - len && utf_iscomposinglike_char(c, cc))
	{
	    ಪ್ರತಿ (;;)
	    {
		pcc[i++] = cc;
		ಯದಿ (i == MAX_MCO)
		    ಭಙ್ಗ;
		len += cc_len;
		ಯದಿ (len >= maxlen || p[len] < 0x80)
		    ಭಙ್ಗ;
		cc = utf_ptr2char_and_len_len(p + len, maxlen - len, &cc_len);
		ಯದಿ (cc_len > maxlen - len || !utf_iscomposing(cc))
		    ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (i < MAX_MCO)	// last composing char must be 0
	pcc[i] = 0;

    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Convert the character at screen position "off" to a sequence of bytes.
 * Includes the composing characters.
 * "buf" must at least have the length MB_MAXBYTES + 1.
 * Only to be used when ScreenLinesUC[off] != 0.
 * Returns the produced number of bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
utfc_char2bytes(ಪೂರ್ಣಾಙ್ಕ off, char_u *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;

    len = utf_char2bytes(Screen_ಸಾಲುಗಳು_UC[off], buf);
    ಪ್ರತಿ (i = 0; i < Screen_mco; ++i)
    {
	ಯದಿ (Screen_ಸಾಲುಗಳು_C[i][off] == 0)
	    ಭಙ್ಗ;
	len += utf_char2bytes(Screen_ಸಾಲುಗಳು_C[i][off], buf + len);
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Get the length of a UTF-8 byte sequence, not including any following
 * composing characters.
 * Returns 0 for "".
 * Returns 1 for an illegal byte sequence.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_ptr2len(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    utf_ptr2char_and_len(p, &len);
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Return length of UTF-8 character, obtained from the first byte.
 * "b" must be between 0 and 255!
 * Returns 1 for an invalid first byte value.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_byte2len(ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ utf8len_tab[b];
}

/*
 * Return length of UTF-8 character, obtained from the first byte.
 * "b" must be between 0 and 255!
 * Returns 0 for an invalid first byte value.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_byte2len_zero(ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ utf8len_tab_zero[b];
}

/*
 * Get the length of UTF-8 byte sequence "p[size]".  Does not include any
 * following composing characters.
 * Returns 1 for "".
 * Returns 1 for an illegal byte sequence (also in incomplete byte seq.).
 * Returns number > "size" for an incomplete byte sequence.
 * Never returns zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_ptr2len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    utf_ptr2char_and_len_len(p, size, &len);
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Return the number of bytes the UTF-8 encoding of the character at "p" takes.
 * This includes following composing characters.
 * Returns zero for NUL.
 */
    ಪೂರ್ಣಾಙ್ಕ
utfc_ptr2len(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		cc_len;
    ಪೂರ್ಣಾಙ್ಕ		b0 = *p;

    ಯದಿ (b0 == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (b0 < 0x80 && p[1] < 0x80)	// be quick for ASCII
	ಪ್ರತ್ಯಾಗಮ 1;

    // Skip over first UTF-8 char, stopping at a NUL byte.
    c = utf_ptr2char_and_len(p, &len);

    // Check for illegal byte.
    ಯದಿ (len == 1 && b0 >= 0x80)
	ಪ್ರತ್ಯಾಗಮ 1;

    /*
     * Check for composing characters.  We can handle only the first six, but
     * skip all of them (otherwise the cursor would get stuck).
     */
    ಯದಿ (p[len] < 0x80)
	ಪ್ರತ್ಯಾಗಮ len;

    cc = utf_ptr2char_and_len(p + len, &cc_len);
    ಯದಿ (!utf_iscomposinglike_char(c, cc))
	ಪ್ರತ್ಯಾಗಮ len;

    ಪ್ರತಿ (;;)
    {
	// Skip over composing char
	len += cc_len;
	ಯದಿ (p[len] < 0x80)
	    ಪ್ರತ್ಯಾಗಮ len;
	cc = utf_ptr2char_and_len(p + len, &cc_len);
	ಯದಿ (!utf_iscomposing(cc))
	    ಪ್ರತ್ಯಾಗಮ len;
    }
}

/*
 * Return the number of bytes the UTF-8 encoding of the character at "p[size]"
 * takes.  This includes following composing characters.
 * Returns 0 for an empty string.
 * Returns 1 for an illegal char or an incomplete byte sequence.
 */
    ಪೂರ್ಣಾಙ್ಕ
utfc_ptr2len_len(char_u *p, ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		cc_len;

    ಯದಿ (size < 1 || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (p[0] < 0x80 && (size == 1 || p[1] < 0x80)) // be quick for ASCII
	ಪ್ರತ್ಯಾಗಮ 1;

    // Skip over first UTF-8 char, stopping at a NUL byte.
    c = utf_ptr2char_and_len_len(p, size, &len);

    // Check for illegal byte and incomplete byte sequence.
    ಯದಿ ((len == 1 && p[0] >= 0x80) || len > size)
	ಪ್ರತ್ಯಾಗಮ 1;

    /*
     * Check for composing characters.  We can handle only the first six, but
     * skip all of them (otherwise the cursor would get stuck).
     */
    ಯದಿ (len >= size || p[len] < 0x80)
	ಪ್ರತ್ಯಾಗಮ len;

    cc = utf_ptr2char_and_len_len(p + len, size - len, &cc_len);
    ಯದಿ (cc_len > size - len || !utf_iscomposinglike_char(c, cc))
	ಪ್ರತ್ಯಾಗಮ len;

    ಯಾವತ್ (len < size)
    {
	// Skip over composing char
	len += cc_len;
	ಯದಿ (len >= size || p[len] < 0x80)
	    ಭಙ್ಗ;
	cc = utf_ptr2char_and_len_len(p + len, size - len, &cc_len);
	ಯದಿ (cc_len > size - len)
	    ಭಙ್ಗ;
	ಯದಿ (!utf_iscomposing(cc))
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Return the number of bytes the UTF-8 encoding of character "c" takes.
 * This does not include composing characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_char2len(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c < 0x80)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಯದಿ (c < 0x800)
	ಪ್ರತ್ಯಾಗಮ 2;
    ಯದಿ (c < 0x10000)
	ಪ್ರತ್ಯಾಗಮ 3;
    ಯದಿ (c < 0x200000)
	ಪ್ರತ್ಯಾಗಮ 4;
    ಯದಿ (c < 0x4000000)
	ಪ್ರತ್ಯಾಗಮ 5;
    ಪ್ರತ್ಯಾಗಮ 6;
}

/*
 * Convert Unicode character "c" to UTF-8 string in "buf[]".
 * Returns the number of bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_char2bytes(ಪೂರ್ಣಾಙ್ಕ c, char_u *buf)
{
    ಯದಿ (c < 0x80)		// 7 bits
    {
	buf[0] = c;
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಯದಿ (c < 0x800)		// 11 bits
    {
	buf[0] = 0xc0 + ((unsigned)c >> 6);
	buf[1] = 0x80 + (c & 0x3f);
	ಪ್ರತ್ಯಾಗಮ 2;
    }
    ಯದಿ (c < 0x10000)		// 16 bits
    {
	buf[0] = 0xe0 + ((unsigned)c >> 12);
	buf[1] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[2] = 0x80 + (c & 0x3f);
	ಪ್ರತ್ಯಾಗಮ 3;
    }
    ಯದಿ (c < 0x200000)		// 21 bits
    {
	buf[0] = 0xf0 + ((unsigned)c >> 18);
	buf[1] = 0x80 + (((unsigned)c >> 12) & 0x3f);
	buf[2] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[3] = 0x80 + (c & 0x3f);
	ಪ್ರತ್ಯಾಗಮ 4;
    }
    ಯದಿ (c < 0x4000000)		// 26 bits
    {
	buf[0] = 0xf8 + ((unsigned)c >> 24);
	buf[1] = 0x80 + (((unsigned)c >> 18) & 0x3f);
	buf[2] = 0x80 + (((unsigned)c >> 12) & 0x3f);
	buf[3] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[4] = 0x80 + (c & 0x3f);
	ಪ್ರತ್ಯಾಗಮ 5;
    }
				// 31 bits
    buf[0] = 0xfc + ((unsigned)c >> 30);
    buf[1] = 0x80 + (((unsigned)c >> 24) & 0x3f);
    buf[2] = 0x80 + (((unsigned)c >> 18) & 0x3f);
    buf[3] = 0x80 + (((unsigned)c >> 12) & 0x3f);
    buf[4] = 0x80 + (((unsigned)c >> 6) & 0x3f);
    buf[5] = 0x80 + (c & 0x3f);
    ಪ್ರತ್ಯಾಗಮ 6;
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * utf_iscomposing() with different argument type for libvterm.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_iscomposing_uint(UINT32_T c)
{
    ಪ್ರತ್ಯಾಗಮ utf_iscomposing((ಪೂರ್ಣಾಙ್ಕ)c);
}
#endif

/*
 * Return TRUE if "c" is a composing UTF-8 character.  This means it will be
 * drawn on top of the preceding character.
 * Based on code from Markus Kuhn.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_iscomposing(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Sorted list of non-overlapping intervals.
    // Generated by ../runtime/tools/unicode.vim.
    ಸ್ಥಿರ ಸಂರಚನಾ interval combining[] =
    {
	{0x0300, 0x036f},
	{0x0483, 0x0489},
	{0x0591, 0x05bd},
	{0x05bf, 0x05bf},
	{0x05c1, 0x05c2},
	{0x05c4, 0x05c5},
	{0x05c7, 0x05c7},
	{0x0610, 0x061a},
	{0x064b, 0x065f},
	{0x0670, 0x0670},
	{0x06d6, 0x06dc},
	{0x06df, 0x06e4},
	{0x06e7, 0x06e8},
	{0x06ea, 0x06ed},
	{0x0711, 0x0711},
	{0x0730, 0x074a},
	{0x07a6, 0x07b0},
	{0x07eb, 0x07f3},
	{0x07fd, 0x07fd},
	{0x0816, 0x0819},
	{0x081b, 0x0823},
	{0x0825, 0x0827},
	{0x0829, 0x082d},
	{0x0859, 0x085b},
	{0x0897, 0x089f},
	{0x08ca, 0x08e1},
	{0x08e3, 0x0902},
	{0x093a, 0x093a},
	{0x093c, 0x093c},
	{0x0941, 0x0948},
	{0x094d, 0x094d},
	{0x0951, 0x0957},
	{0x0962, 0x0963},
	{0x0981, 0x0981},
	{0x09bc, 0x09bc},
	{0x09c1, 0x09c4},
	{0x09cd, 0x09cd},
	{0x09e2, 0x09e3},
	{0x09fe, 0x09fe},
	{0x0a01, 0x0a02},
	{0x0a3c, 0x0a3c},
	{0x0a41, 0x0a42},
	{0x0a47, 0x0a48},
	{0x0a4b, 0x0a4d},
	{0x0a51, 0x0a51},
	{0x0a70, 0x0a71},
	{0x0a75, 0x0a75},
	{0x0a81, 0x0a82},
	{0x0abc, 0x0abc},
	{0x0ac1, 0x0ac5},
	{0x0ac7, 0x0ac8},
	{0x0acd, 0x0acd},
	{0x0ae2, 0x0ae3},
	{0x0afa, 0x0aff},
	{0x0b01, 0x0b01},
	{0x0b3c, 0x0b3c},
	{0x0b3f, 0x0b3f},
	{0x0b41, 0x0b44},
	{0x0b4d, 0x0b4d},
	{0x0b55, 0x0b56},
	{0x0b62, 0x0b63},
	{0x0b82, 0x0b82},
	{0x0bc0, 0x0bc0},
	{0x0bcd, 0x0bcd},
	{0x0c00, 0x0c00},
	{0x0c04, 0x0c04},
	{0x0c3c, 0x0c3c},
	{0x0c3e, 0x0c40},
	{0x0c46, 0x0c48},
	{0x0c4a, 0x0c4d},
	{0x0c55, 0x0c56},
	{0x0c62, 0x0c63},
	{0x0c81, 0x0c81},
	{0x0cbc, 0x0cbc},
	{0x0cbf, 0x0cbf},
	{0x0cc6, 0x0cc6},
	{0x0ccc, 0x0ccd},
	{0x0ce2, 0x0ce3},
	{0x0d00, 0x0d01},
	{0x0d3b, 0x0d3c},
	{0x0d41, 0x0d44},
	{0x0d4d, 0x0d4d},
	{0x0d62, 0x0d63},
	{0x0d81, 0x0d81},
	{0x0dca, 0x0dca},
	{0x0dd2, 0x0dd4},
	{0x0dd6, 0x0dd6},
	{0x0e31, 0x0e31},
	{0x0e34, 0x0e3a},
	{0x0e47, 0x0e4e},
	{0x0eb1, 0x0eb1},
	{0x0eb4, 0x0ebc},
	{0x0ec8, 0x0ece},
	{0x0f18, 0x0f19},
	{0x0f35, 0x0f35},
	{0x0f37, 0x0f37},
	{0x0f39, 0x0f39},
	{0x0f71, 0x0f7e},
	{0x0f80, 0x0f84},
	{0x0f86, 0x0f87},
	{0x0f8d, 0x0f97},
	{0x0f99, 0x0fbc},
	{0x0fc6, 0x0fc6},
	{0x102d, 0x1030},
	{0x1032, 0x1037},
	{0x1039, 0x103a},
	{0x103d, 0x103e},
	{0x1058, 0x1059},
	{0x105e, 0x1060},
	{0x1071, 0x1074},
	{0x1082, 0x1082},
	{0x1085, 0x1086},
	{0x108d, 0x108d},
	{0x109d, 0x109d},
	{0x135d, 0x135f},
	{0x1712, 0x1714},
	{0x1732, 0x1733},
	{0x1752, 0x1753},
	{0x1772, 0x1773},
	{0x17b4, 0x17b5},
	{0x17b7, 0x17bd},
	{0x17c6, 0x17c6},
	{0x17c9, 0x17d3},
	{0x17dd, 0x17dd},
	{0x180b, 0x180d},
	{0x180f, 0x180f},
	{0x1885, 0x1886},
	{0x18a9, 0x18a9},
	{0x1920, 0x1922},
	{0x1927, 0x1928},
	{0x1932, 0x1932},
	{0x1939, 0x193b},
	{0x1a17, 0x1a18},
	{0x1a1b, 0x1a1b},
	{0x1a56, 0x1a56},
	{0x1a58, 0x1a5e},
	{0x1a60, 0x1a60},
	{0x1a62, 0x1a62},
	{0x1a65, 0x1a6c},
	{0x1a73, 0x1a7c},
	{0x1a7f, 0x1a7f},
	{0x1ab0, 0x1ace},
	{0x1b00, 0x1b03},
	{0x1b34, 0x1b34},
	{0x1b36, 0x1b3a},
	{0x1b3c, 0x1b3c},
	{0x1b42, 0x1b42},
	{0x1b6b, 0x1b73},
	{0x1b80, 0x1b81},
	{0x1ba2, 0x1ba5},
	{0x1ba8, 0x1ba9},
	{0x1bab, 0x1bad},
	{0x1be6, 0x1be6},
	{0x1be8, 0x1be9},
	{0x1bed, 0x1bed},
	{0x1bef, 0x1bf1},
	{0x1c2c, 0x1c33},
	{0x1c36, 0x1c37},
	{0x1cd0, 0x1cd2},
	{0x1cd4, 0x1ce0},
	{0x1ce2, 0x1ce8},
	{0x1ced, 0x1ced},
	{0x1cf4, 0x1cf4},
	{0x1cf8, 0x1cf9},
	{0x1dc0, 0x1dff},
	{0x20d0, 0x20f0},
	{0x2cef, 0x2cf1},
	{0x2d7f, 0x2d7f},
	{0x2de0, 0x2dff},
	{0x302a, 0x302d},
	{0x3099, 0x309a},
	{0xa66f, 0xa672},
	{0xa674, 0xa67d},
	{0xa69e, 0xa69f},
	{0xa6f0, 0xa6f1},
	{0xa802, 0xa802},
	{0xa806, 0xa806},
	{0xa80b, 0xa80b},
	{0xa825, 0xa826},
	{0xa82c, 0xa82c},
	{0xa8c4, 0xa8c5},
	{0xa8e0, 0xa8f1},
	{0xa8ff, 0xa8ff},
	{0xa926, 0xa92d},
	{0xa947, 0xa951},
	{0xa980, 0xa982},
	{0xa9b3, 0xa9b3},
	{0xa9b6, 0xa9b9},
	{0xa9bc, 0xa9bd},
	{0xa9e5, 0xa9e5},
	{0xaa29, 0xaa2e},
	{0xaa31, 0xaa32},
	{0xaa35, 0xaa36},
	{0xaa43, 0xaa43},
	{0xaa4c, 0xaa4c},
	{0xaa7c, 0xaa7c},
	{0xaab0, 0xaab0},
	{0xaab2, 0xaab4},
	{0xaab7, 0xaab8},
	{0xaabe, 0xaabf},
	{0xaac1, 0xaac1},
	{0xaaec, 0xaaed},
	{0xaaf6, 0xaaf6},
	{0xabe5, 0xabe5},
	{0xabe8, 0xabe8},
	{0xabed, 0xabed},
	{0xfb1e, 0xfb1e},
	{0xfe00, 0xfe0f},
	{0xfe20, 0xfe2f},
	{0x101fd, 0x101fd},
	{0x102e0, 0x102e0},
	{0x10376, 0x1037a},
	{0x10a01, 0x10a03},
	{0x10a05, 0x10a06},
	{0x10a0c, 0x10a0f},
	{0x10a38, 0x10a3a},
	{0x10a3f, 0x10a3f},
	{0x10ae5, 0x10ae6},
	{0x10d24, 0x10d27},
	{0x10d69, 0x10d6d},
	{0x10eab, 0x10eac},
	{0x10efc, 0x10eff},
	{0x10f46, 0x10f50},
	{0x10f82, 0x10f85},
	{0x11001, 0x11001},
	{0x11038, 0x11046},
	{0x11070, 0x11070},
	{0x11073, 0x11074},
	{0x1107f, 0x11081},
	{0x110b3, 0x110b6},
	{0x110b9, 0x110ba},
	{0x110c2, 0x110c2},
	{0x11100, 0x11102},
	{0x11127, 0x1112b},
	{0x1112d, 0x11134},
	{0x11173, 0x11173},
	{0x11180, 0x11181},
	{0x111b6, 0x111be},
	{0x111c9, 0x111cc},
	{0x111cf, 0x111cf},
	{0x1122f, 0x11231},
	{0x11234, 0x11234},
	{0x11236, 0x11237},
	{0x1123e, 0x1123e},
	{0x11241, 0x11241},
	{0x112df, 0x112df},
	{0x112e3, 0x112ea},
	{0x11300, 0x11301},
	{0x1133b, 0x1133c},
	{0x11340, 0x11340},
	{0x11366, 0x1136c},
	{0x11370, 0x11374},
	{0x113bb, 0x113c0},
	{0x113ce, 0x113ce},
	{0x113d0, 0x113d0},
	{0x113d2, 0x113d2},
	{0x113e1, 0x113e2},
	{0x11438, 0x1143f},
	{0x11442, 0x11444},
	{0x11446, 0x11446},
	{0x1145e, 0x1145e},
	{0x114b3, 0x114b8},
	{0x114ba, 0x114ba},
	{0x114bf, 0x114c0},
	{0x114c2, 0x114c3},
	{0x115b2, 0x115b5},
	{0x115bc, 0x115bd},
	{0x115bf, 0x115c0},
	{0x115dc, 0x115dd},
	{0x11633, 0x1163a},
	{0x1163d, 0x1163d},
	{0x1163f, 0x11640},
	{0x116ab, 0x116ab},
	{0x116ad, 0x116ad},
	{0x116b0, 0x116b5},
	{0x116b7, 0x116b7},
	{0x1171d, 0x1171d},
	{0x1171f, 0x1171f},
	{0x11722, 0x11725},
	{0x11727, 0x1172b},
	{0x1182f, 0x11837},
	{0x11839, 0x1183a},
	{0x1193b, 0x1193c},
	{0x1193e, 0x1193e},
	{0x11943, 0x11943},
	{0x119d4, 0x119d7},
	{0x119da, 0x119db},
	{0x119e0, 0x119e0},
	{0x11a01, 0x11a0a},
	{0x11a33, 0x11a38},
	{0x11a3b, 0x11a3e},
	{0x11a47, 0x11a47},
	{0x11a51, 0x11a56},
	{0x11a59, 0x11a5b},
	{0x11a8a, 0x11a96},
	{0x11a98, 0x11a99},
	{0x11c30, 0x11c36},
	{0x11c38, 0x11c3d},
	{0x11c3f, 0x11c3f},
	{0x11c92, 0x11ca7},
	{0x11caa, 0x11cb0},
	{0x11cb2, 0x11cb3},
	{0x11cb5, 0x11cb6},
	{0x11d31, 0x11d36},
	{0x11d3a, 0x11d3a},
	{0x11d3c, 0x11d3d},
	{0x11d3f, 0x11d45},
	{0x11d47, 0x11d47},
	{0x11d90, 0x11d91},
	{0x11d95, 0x11d95},
	{0x11d97, 0x11d97},
	{0x11ef3, 0x11ef4},
	{0x11f00, 0x11f01},
	{0x11f36, 0x11f3a},
	{0x11f40, 0x11f40},
	{0x11f42, 0x11f42},
	{0x11f5a, 0x11f5a},
	{0x13440, 0x13440},
	{0x13447, 0x13455},
	{0x1611e, 0x16129},
	{0x1612d, 0x1612f},
	{0x16af0, 0x16af4},
	{0x16b30, 0x16b36},
	{0x16f4f, 0x16f4f},
	{0x16f8f, 0x16f92},
	{0x16fe4, 0x16fe4},
	{0x1bc9d, 0x1bc9e},
	{0x1cf00, 0x1cf2d},
	{0x1cf30, 0x1cf46},
	{0x1d167, 0x1d169},
	{0x1d17b, 0x1d182},
	{0x1d185, 0x1d18b},
	{0x1d1aa, 0x1d1ad},
	{0x1d242, 0x1d244},
	{0x1da00, 0x1da36},
	{0x1da3b, 0x1da6c},
	{0x1da75, 0x1da75},
	{0x1da84, 0x1da84},
	{0x1da9b, 0x1da9f},
	{0x1daa1, 0x1daaf},
	{0x1e000, 0x1e006},
	{0x1e008, 0x1e018},
	{0x1e01b, 0x1e021},
	{0x1e023, 0x1e024},
	{0x1e026, 0x1e02a},
	{0x1e08f, 0x1e08f},
	{0x1e130, 0x1e136},
	{0x1e2ae, 0x1e2ae},
	{0x1e2ec, 0x1e2ef},
	{0x1e4ec, 0x1e4ef},
	{0x1e5ee, 0x1e5ef},
	{0x1e8d0, 0x1e8d6},
	{0x1e944, 0x1e94a},
	{0xe0100, 0xe01ef}
    };

    ಪ್ರತ್ಯಾಗಮ intable(combining, ಪರಿಮಾಣ(combining), c);
}

/*
 * Return TRUE for characters that can be displayed in a normal way.
 * Only for characters of 0x100 and above!
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_printable(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Sorted list of non-overlapping intervals.
    // 0xd800-0xdfff is reserved for UTF-16, actually illegal.
    ಸ್ಥಿರ ಸಂರಚನಾ interval nonprint[] =
    {
	{0x070f, 0x070f}, {0x180b, 0x180e}, {0x200b, 0x200f}, {0x202a, 0x202e},
	{0x2060, 0x206f}, {0xd800, 0xdfff}, {0xfeff, 0xfeff}, {0xfff9, 0xfffb},
	{0xfffe, 0xffff}
    };

    ಪ್ರತ್ಯಾಗಮ !intable(nonprint, ಪರಿಮಾಣ(nonprint), c);
}

// Sorted list of non-overlapping intervals of all Emoji characters,
// based on http://unicode.org/emoji/charts/emoji-list.html
// Generated by ../runtime/tools/unicode.vim.
// Excludes 0x00a9 and 0x00ae because they are considered latin1.
ಸ್ಥಿರ ಸಂರಚನಾ interval emoji_all[] =
{
    {0x203c, 0x203c},
    {0x2049, 0x2049},
    {0x2122, 0x2122},
    {0x2139, 0x2139},
    {0x2194, 0x2199},
    {0x21a9, 0x21aa},
    {0x231a, 0x231b},
    {0x2328, 0x2328},
    {0x23cf, 0x23cf},
    {0x23e9, 0x23f3},
    {0x23f8, 0x23fa},
    {0x24c2, 0x24c2},
    {0x25aa, 0x25ab},
    {0x25b6, 0x25b6},
    {0x25c0, 0x25c0},
    {0x25fb, 0x25fe},
    {0x2600, 0x2604},
    {0x260e, 0x260e},
    {0x2611, 0x2611},
    {0x2614, 0x2615},
    {0x2618, 0x2618},
    {0x261d, 0x261d},
    {0x2620, 0x2620},
    {0x2622, 0x2623},
    {0x2626, 0x2626},
    {0x262a, 0x262a},
    {0x262e, 0x262f},
    {0x2638, 0x263a},
    {0x2640, 0x2640},
    {0x2642, 0x2642},
    {0x2648, 0x2653},
    {0x265f, 0x2660},
    {0x2663, 0x2663},
    {0x2665, 0x2666},
    {0x2668, 0x2668},
    {0x267b, 0x267b},
    {0x267e, 0x267f},
    {0x2692, 0x2697},
    {0x2699, 0x2699},
    {0x269b, 0x269c},
    {0x26a0, 0x26a1},
    {0x26a7, 0x26a7},
    {0x26aa, 0x26ab},
    {0x26b0, 0x26b1},
    {0x26bd, 0x26be},
    {0x26c4, 0x26c5},
    {0x26c8, 0x26c8},
    {0x26ce, 0x26cf},
    {0x26d1, 0x26d1},
    {0x26d3, 0x26d4},
    {0x26e9, 0x26ea},
    {0x26f0, 0x26f5},
    {0x26f7, 0x26fa},
    {0x26fd, 0x26fd},
    {0x2702, 0x2702},
    {0x2705, 0x2705},
    {0x2708, 0x270d},
    {0x270f, 0x270f},
    {0x2712, 0x2712},
    {0x2714, 0x2714},
    {0x2716, 0x2716},
    {0x271d, 0x271d},
    {0x2721, 0x2721},
    {0x2728, 0x2728},
    {0x2733, 0x2734},
    {0x2744, 0x2744},
    {0x2747, 0x2747},
    {0x274c, 0x274c},
    {0x274e, 0x274e},
    {0x2753, 0x2755},
    {0x2757, 0x2757},
    {0x2763, 0x2764},
    {0x2795, 0x2797},
    {0x27a1, 0x27a1},
    {0x27b0, 0x27b0},
    {0x27bf, 0x27bf},
    {0x2934, 0x2935},
    {0x2b05, 0x2b07},
    {0x2b1b, 0x2b1c},
    {0x2b50, 0x2b50},
    {0x2b55, 0x2b55},
    {0x3030, 0x3030},
    {0x303d, 0x303d},
    {0x3297, 0x3297},
    {0x3299, 0x3299},
    {0x1f004, 0x1f004},
    {0x1f0cf, 0x1f0cf},
    {0x1f170, 0x1f171},
    {0x1f17e, 0x1f17f},
    {0x1f18e, 0x1f18e},
    {0x1f191, 0x1f19a},
    {0x1f1e6, 0x1f1ff},
    {0x1f201, 0x1f202},
    {0x1f21a, 0x1f21a},
    {0x1f22f, 0x1f22f},
    {0x1f232, 0x1f23a},
    {0x1f250, 0x1f251},
    {0x1f300, 0x1f321},
    {0x1f324, 0x1f393},
    {0x1f396, 0x1f397},
    {0x1f399, 0x1f39b},
    {0x1f39e, 0x1f3f0},
    {0x1f3f3, 0x1f3f5},
    {0x1f3f7, 0x1f4fd},
    {0x1f4ff, 0x1f53d},
    {0x1f549, 0x1f54e},
    {0x1f550, 0x1f567},
    {0x1f56f, 0x1f570},
    {0x1f573, 0x1f57a},
    {0x1f587, 0x1f587},
    {0x1f58a, 0x1f58d},
    {0x1f590, 0x1f590},
    {0x1f595, 0x1f596},
    {0x1f5a4, 0x1f5a5},
    {0x1f5a8, 0x1f5a8},
    {0x1f5b1, 0x1f5b2},
    {0x1f5bc, 0x1f5bc},
    {0x1f5c2, 0x1f5c4},
    {0x1f5d1, 0x1f5d3},
    {0x1f5dc, 0x1f5de},
    {0x1f5e1, 0x1f5e1},
    {0x1f5e3, 0x1f5e3},
    {0x1f5e8, 0x1f5e8},
    {0x1f5ef, 0x1f5ef},
    {0x1f5f3, 0x1f5f3},
    {0x1f5fa, 0x1f64f},
    {0x1f680, 0x1f6c5},
    {0x1f6cb, 0x1f6d2},
    {0x1f6d5, 0x1f6d7},
    {0x1f6dc, 0x1f6e5},
    {0x1f6e9, 0x1f6e9},
    {0x1f6eb, 0x1f6ec},
    {0x1f6f0, 0x1f6f0},
    {0x1f6f3, 0x1f6fc},
    {0x1f7e0, 0x1f7eb},
    {0x1f7f0, 0x1f7f0},
    {0x1f90c, 0x1f93a},
    {0x1f93c, 0x1f945},
    {0x1f947, 0x1f9ff},
    {0x1fa70, 0x1fa7c},
    {0x1fa80, 0x1fa88},
    {0x1fa90, 0x1fabd},
    {0x1fabf, 0x1fac5},
    {0x1face, 0x1fadb},
    {0x1fae0, 0x1fae8},
    {0x1faf0, 0x1faf8}
};

/*
 * Get class of a Unicode character.
 * 0: white space
 * 1: punctuation
 * 2 or bigger: some class of word character.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_class(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ utf_class_buf(c, curbuf);
}

    ಪೂರ್ಣಾಙ್ಕ
utf_class_buf(ಪೂರ್ಣಾಙ್ಕ c, buf_T *buf)
{
    // sorted list of non-overlapping intervals
    ಸ್ಥಿರ ಸಂರಚನಾ clinterval
    {
	unsigned ಪೂರ್ಣಾಙ್ಕ first;
	unsigned ಪೂರ್ಣಾಙ್ಕ last;
	unsigned ಪೂರ್ಣಾಙ್ಕ class;
    } classes[] =
    {
	{0x037e, 0x037e, 1},		// Greek question mark
	{0x0387, 0x0387, 1},		// Greek ano teleia
	{0x055a, 0x055f, 1},		// Armenian punctuation
	{0x0589, 0x0589, 1},		// Armenian full stop
	{0x05be, 0x05be, 1},
	{0x05c0, 0x05c0, 1},
	{0x05c3, 0x05c3, 1},
	{0x05f3, 0x05f4, 1},
	{0x060c, 0x060c, 1},
	{0x061b, 0x061b, 1},
	{0x061f, 0x061f, 1},
	{0x066a, 0x066d, 1},
	{0x06d4, 0x06d4, 1},
	{0x0700, 0x070d, 1},		// Syriac punctuation
	{0x0964, 0x0965, 1},
	{0x0970, 0x0970, 1},
	{0x0df4, 0x0df4, 1},
	{0x0e4f, 0x0e4f, 1},
	{0x0e5a, 0x0e5b, 1},
	{0x0f04, 0x0f12, 1},
	{0x0f3a, 0x0f3d, 1},
	{0x0f85, 0x0f85, 1},
	{0x104a, 0x104f, 1},		// Myanmar punctuation
	{0x10fb, 0x10fb, 1},		// Georgian punctuation
	{0x1361, 0x1368, 1},		// Ethiopic punctuation
	{0x166d, 0x166e, 1},		// Canadian Syl. punctuation
	{0x1680, 0x1680, 0},
	{0x169b, 0x169c, 1},
	{0x16eb, 0x16ed, 1},
	{0x1735, 0x1736, 1},
	{0x17d4, 0x17dc, 1},		// Khmer punctuation
	{0x1800, 0x180a, 1},		// Mongolian punctuation
	{0x2000, 0x200b, 0},		// spaces
	{0x200c, 0x2027, 1},		// punctuation and symbols
	{0x2028, 0x2029, 0},
	{0x202a, 0x202e, 1},		// punctuation and symbols
	{0x202f, 0x202f, 0},
	{0x2030, 0x205e, 1},		// punctuation and symbols
	{0x205f, 0x205f, 0},
	{0x2060, 0x206f, 1},		// punctuation and symbols
	{0x2070, 0x207f, 0x2070},	// superscript
	{0x2080, 0x2094, 0x2080},	// subscript
	{0x20a0, 0x27ff, 1},		// all kinds of symbols
	{0x2800, 0x28ff, 0x2800},	// braille
	{0x2900, 0x2998, 1},		// arrows, brackets, etc.
	{0x29d8, 0x29db, 1},
	{0x29fc, 0x29fd, 1},
	{0x2e00, 0x2e7f, 1},		// supplemental punctuation
	{0x3000, 0x3000, 0},		// ideographic space
	{0x3001, 0x3020, 1},		// ideographic punctuation
	{0x3030, 0x3030, 1},
	{0x303d, 0x303d, 1},
	{0x3040, 0x309f, 0x3040},	// Hiragana
	{0x30a0, 0x30ff, 0x30a0},	// Katakana
	{0x3300, 0x9fff, 0x4e00},	// CJK Ideographs
	{0xac00, 0xd7a3, 0xac00},	// Hangul Syllables
	{0xf900, 0xfaff, 0x4e00},	// CJK Ideographs
	{0xfd3e, 0xfd3f, 1},
	{0xfe30, 0xfe6b, 1},		// punctuation forms
	{0xff00, 0xff0f, 1},		// half/fullwidth ASCII
	{0xff1a, 0xff20, 1},		// half/fullwidth ASCII
	{0xff3b, 0xff40, 1},		// half/fullwidth ASCII
	{0xff5b, 0xff65, 1},		// half/fullwidth ASCII
	{0x1d000, 0x1d24f, 1},		// Musical notation
	{0x1d400, 0x1d7ff, 1},		// Mathematical Alphanumeric Symbols
	{0x1f000, 0x1f2ff, 1},		// Game pieces; enclosed characters
	{0x1f300, 0x1f9ff, 1},		// Many symbol blocks
	{0x20000, 0x2a6df, 0x4e00},	// CJK Ideographs
	{0x2a700, 0x2b73f, 0x4e00},	// CJK Ideographs
	{0x2b740, 0x2b81f, 0x4e00},	// CJK Ideographs
	{0x2f800, 0x2fa1f, 0x4e00},	// CJK Ideographs
    };

    ಪೂರ್ಣಾಙ್ಕ bot = 0;
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ = ARRAY_LENGTH(classes) - 1;
    ಪೂರ್ಣಾಙ್ಕ mid;

    // First quick check for Latin1 characters, use 'iskeyword'.
    ಯದಿ (c < 0x100)
    {
	ಯದಿ (c == ' ' || c == '\t' || c == NUL || c == 0xa0)
	    ಪ್ರತ್ಯಾಗಮ 0;	    // blank
	ಯದಿ (vim_iswordc_buf(c, buf))
	    ಪ್ರತ್ಯಾಗಮ 2;	    // word character
	ಪ್ರತ್ಯಾಗಮ 1;	    // punctuation
    }

    // emoji
    ಯದಿ (intable(emoji_all, ಪರಿಮಾಣ(emoji_all), c))
	ಪ್ರತ್ಯಾಗಮ 3;

    // binary search in table
    ಯಾವತ್ (ತುದಿ >= bot)
    {
	mid = (bot + ತುದಿ) / 2;
	ಯದಿ (classes[mid].last < (unsigned ಪೂರ್ಣಾಙ್ಕ)c)
	    bot = mid + 1;
	ಅನ್ಯಥಾ ಯದಿ (classes[mid].first > (unsigned ಪೂರ್ಣಾಙ್ಕ)c)
	    ತುದಿ = mid - 1;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)classes[mid].class;
    }

    // most other characters are "word" characters
    ಪ್ರತ್ಯಾಗಮ 2;
}

    ಪೂರ್ಣಾಙ್ಕ
utf_ambiguous_width(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ c >= 0x80 && (intable(ambiguous, ಪರಿಮಾಣ(ambiguous), c)
	    || intable(emoji_all, ಪರಿಮಾಣ(emoji_all), c));
}

/*
 * Code for Unicode case-dependent operations.  Based on notes in
 * http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
 * This code uses simple case folding, not full case folding.
 * Last updated for Unicode 5.2.
 */

/*
 * The following tables are built by ../runtime/tools/unicode.vim.
 * They must be in numeric order, because we use binary search.
 * An entry such as {0x41,0x5a,1,32} means that Unicode characters in the
 * range from 0x41 to 0x5a inclusive, stepping by 1, are changed to
 * folded/upper/lower by adding 32.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ rangeStart;
    ಪೂರ್ಣಾಙ್ಕ rangeEnd;
    ಪೂರ್ಣಾಙ್ಕ step;
    ಪೂರ್ಣಾಙ್ಕ offset;
} convertStruct;

ಸ್ಥಿರ convertStruct foldCase[] =
{
	{0x41,0x5a,1,32},
	{0xb5,0xb5,-1,775},
	{0xc0,0xd6,1,32},
	{0xd8,0xde,1,32},
	{0x100,0x12e,2,1},
	{0x132,0x136,2,1},
	{0x139,0x147,2,1},
	{0x14a,0x176,2,1},
	{0x178,0x178,-1,-121},
	{0x179,0x17d,2,1},
	{0x17f,0x17f,-1,-268},
	{0x181,0x181,-1,210},
	{0x182,0x184,2,1},
	{0x186,0x186,-1,206},
	{0x187,0x187,-1,1},
	{0x189,0x18a,1,205},
	{0x18b,0x18b,-1,1},
	{0x18e,0x18e,-1,79},
	{0x18f,0x18f,-1,202},
	{0x190,0x190,-1,203},
	{0x191,0x191,-1,1},
	{0x193,0x193,-1,205},
	{0x194,0x194,-1,207},
	{0x196,0x196,-1,211},
	{0x197,0x197,-1,209},
	{0x198,0x198,-1,1},
	{0x19c,0x19c,-1,211},
	{0x19d,0x19d,-1,213},
	{0x19f,0x19f,-1,214},
	{0x1a0,0x1a4,2,1},
	{0x1a6,0x1a6,-1,218},
	{0x1a7,0x1a7,-1,1},
	{0x1a9,0x1a9,-1,218},
	{0x1ac,0x1ac,-1,1},
	{0x1ae,0x1ae,-1,218},
	{0x1af,0x1af,-1,1},
	{0x1b1,0x1b2,1,217},
	{0x1b3,0x1b5,2,1},
	{0x1b7,0x1b7,-1,219},
	{0x1b8,0x1bc,4,1},
	{0x1c4,0x1c4,-1,2},
	{0x1c5,0x1c5,-1,1},
	{0x1c7,0x1c7,-1,2},
	{0x1c8,0x1c8,-1,1},
	{0x1ca,0x1ca,-1,2},
	{0x1cb,0x1db,2,1},
	{0x1de,0x1ee,2,1},
	{0x1f1,0x1f1,-1,2},
	{0x1f2,0x1f4,2,1},
	{0x1f6,0x1f6,-1,-97},
	{0x1f7,0x1f7,-1,-56},
	{0x1f8,0x21e,2,1},
	{0x220,0x220,-1,-130},
	{0x222,0x232,2,1},
	{0x23a,0x23a,-1,10795},
	{0x23b,0x23b,-1,1},
	{0x23d,0x23d,-1,-163},
	{0x23e,0x23e,-1,10792},
	{0x241,0x241,-1,1},
	{0x243,0x243,-1,-195},
	{0x244,0x244,-1,69},
	{0x245,0x245,-1,71},
	{0x246,0x24e,2,1},
	{0x345,0x345,-1,116},
	{0x370,0x372,2,1},
	{0x376,0x376,-1,1},
	{0x37f,0x37f,-1,116},
	{0x386,0x386,-1,38},
	{0x388,0x38a,1,37},
	{0x38c,0x38c,-1,64},
	{0x38e,0x38f,1,63},
	{0x391,0x3a1,1,32},
	{0x3a3,0x3ab,1,32},
	{0x3c2,0x3c2,-1,1},
	{0x3cf,0x3cf,-1,8},
	{0x3d0,0x3d0,-1,-30},
	{0x3d1,0x3d1,-1,-25},
	{0x3d5,0x3d5,-1,-15},
	{0x3d6,0x3d6,-1,-22},
	{0x3d8,0x3ee,2,1},
	{0x3f0,0x3f0,-1,-54},
	{0x3f1,0x3f1,-1,-48},
	{0x3f4,0x3f4,-1,-60},
	{0x3f5,0x3f5,-1,-64},
	{0x3f7,0x3f7,-1,1},
	{0x3f9,0x3f9,-1,-7},
	{0x3fa,0x3fa,-1,1},
	{0x3fd,0x3ff,1,-130},
	{0x400,0x40f,1,80},
	{0x410,0x42f,1,32},
	{0x460,0x480,2,1},
	{0x48a,0x4be,2,1},
	{0x4c0,0x4c0,-1,15},
	{0x4c1,0x4cd,2,1},
	{0x4d0,0x52e,2,1},
	{0x531,0x556,1,48},
	{0x10a0,0x10c5,1,7264},
	{0x10c7,0x10cd,6,7264},
	{0x13f8,0x13fd,1,-8},
	{0x1c80,0x1c80,-1,-6222},
	{0x1c81,0x1c81,-1,-6221},
	{0x1c82,0x1c82,-1,-6212},
	{0x1c83,0x1c84,1,-6210},
	{0x1c85,0x1c85,-1,-6211},
	{0x1c86,0x1c86,-1,-6204},
	{0x1c87,0x1c87,-1,-6180},
	{0x1c88,0x1c88,-1,35267},
	{0x1c89,0x1c89,-1,1},
	{0x1c90,0x1cba,1,-3008},
	{0x1cbd,0x1cbf,1,-3008},
	{0x1e00,0x1e94,2,1},
	{0x1e9b,0x1e9b,-1,-58},
	{0x1e9e,0x1e9e,-1,-7615},
	{0x1ea0,0x1efe,2,1},
	{0x1f08,0x1f0f,1,-8},
	{0x1f18,0x1f1d,1,-8},
	{0x1f28,0x1f2f,1,-8},
	{0x1f38,0x1f3f,1,-8},
	{0x1f48,0x1f4d,1,-8},
	{0x1f59,0x1f5f,2,-8},
	{0x1f68,0x1f6f,1,-8},
	{0x1f88,0x1f8f,1,-8},
	{0x1f98,0x1f9f,1,-8},
	{0x1fa8,0x1faf,1,-8},
	{0x1fb8,0x1fb9,1,-8},
	{0x1fba,0x1fbb,1,-74},
	{0x1fbc,0x1fbc,-1,-9},
	{0x1fbe,0x1fbe,-1,-7173},
	{0x1fc8,0x1fcb,1,-86},
	{0x1fcc,0x1fcc,-1,-9},
	{0x1fd3,0x1fd3,-1,-7235},
	{0x1fd8,0x1fd9,1,-8},
	{0x1fda,0x1fdb,1,-100},
	{0x1fe3,0x1fe3,-1,-7219},
	{0x1fe8,0x1fe9,1,-8},
	{0x1fea,0x1feb,1,-112},
	{0x1fec,0x1fec,-1,-7},
	{0x1ff8,0x1ff9,1,-128},
	{0x1ffa,0x1ffb,1,-126},
	{0x1ffc,0x1ffc,-1,-9},
	{0x2126,0x2126,-1,-7517},
	{0x212a,0x212a,-1,-8383},
	{0x212b,0x212b,-1,-8262},
	{0x2132,0x2132,-1,28},
	{0x2160,0x216f,1,16},
	{0x2183,0x2183,-1,1},
	{0x24b6,0x24cf,1,26},
	{0x2c00,0x2c2f,1,48},
	{0x2c60,0x2c60,-1,1},
	{0x2c62,0x2c62,-1,-10743},
	{0x2c63,0x2c63,-1,-3814},
	{0x2c64,0x2c64,-1,-10727},
	{0x2c67,0x2c6b,2,1},
	{0x2c6d,0x2c6d,-1,-10780},
	{0x2c6e,0x2c6e,-1,-10749},
	{0x2c6f,0x2c6f,-1,-10783},
	{0x2c70,0x2c70,-1,-10782},
	{0x2c72,0x2c75,3,1},
	{0x2c7e,0x2c7f,1,-10815},
	{0x2c80,0x2ce2,2,1},
	{0x2ceb,0x2ced,2,1},
	{0x2cf2,0xa640,31054,1},
	{0xa642,0xa66c,2,1},
	{0xa680,0xa69a,2,1},
	{0xa722,0xa72e,2,1},
	{0xa732,0xa76e,2,1},
	{0xa779,0xa77b,2,1},
	{0xa77d,0xa77d,-1,-35332},
	{0xa77e,0xa786,2,1},
	{0xa78b,0xa78b,-1,1},
	{0xa78d,0xa78d,-1,-42280},
	{0xa790,0xa792,2,1},
	{0xa796,0xa7a8,2,1},
	{0xa7aa,0xa7aa,-1,-42308},
	{0xa7ab,0xa7ab,-1,-42319},
	{0xa7ac,0xa7ac,-1,-42315},
	{0xa7ad,0xa7ad,-1,-42305},
	{0xa7ae,0xa7ae,-1,-42308},
	{0xa7b0,0xa7b0,-1,-42258},
	{0xa7b1,0xa7b1,-1,-42282},
	{0xa7b2,0xa7b2,-1,-42261},
	{0xa7b3,0xa7b3,-1,928},
	{0xa7b4,0xa7c2,2,1},
	{0xa7c4,0xa7c4,-1,-48},
	{0xa7c5,0xa7c5,-1,-42307},
	{0xa7c6,0xa7c6,-1,-35384},
	{0xa7c7,0xa7c9,2,1},
	{0xa7cb,0xa7cb,-1,-42343},
	{0xa7cc,0xa7d0,4,1},
	{0xa7d6,0xa7da,2,1},
	{0xa7dc,0xa7dc,-1,-42561},
	{0xa7f5,0xa7f5,-1,1},
	{0xab70,0xabbf,1,-38864},
	{0xfb05,0xfb05,-1,1},
	{0xff21,0xff3a,1,32},
	{0x10400,0x10427,1,40},
	{0x104b0,0x104d3,1,40},
	{0x10570,0x1057a,1,39},
	{0x1057c,0x1058a,1,39},
	{0x1058c,0x10592,1,39},
	{0x10594,0x10595,1,39},
	{0x10c80,0x10cb2,1,64},
	{0x10d50,0x10d65,1,32},
	{0x118a0,0x118bf,1,32},
	{0x16e40,0x16e5f,1,32},
	{0x1e900,0x1e921,1,34}
};

/*
 * Generic conversion function for case operations.
 * Return the converted equivalent of "a", which is a UCS-4 character.  Use
 * the given conversion "table".  Uses binary search on "table".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf_convert(
    ಪೂರ್ಣಾಙ್ಕ			a,
    convertStruct	table[],
    ಪೂರ್ಣಾಙ್ಕ			tableSize)
{
    ಪೂರ್ಣಾಙ್ಕ start, mid, end; // indices into table
    ಪೂರ್ಣಾಙ್ಕ entries = tableSize / ಪರಿಮಾಣ(convertStruct);

    start = 0;
    end = entries;
    ಯಾವತ್ (start < end)
    {
	// need to search further
	mid = (end + start) / 2;
	ಯದಿ (table[mid].rangeEnd < a)
	    start = mid + 1;
	ಅನ್ಯಥಾ
	    end = mid;
    }
    ಯದಿ (start < entries
	    && table[start].rangeStart <= a
	    && a <= table[start].rangeEnd
	    && (a - table[start].rangeStart) % table[start].step == 0)
	ಪ್ರತ್ಯಾಗಮ (a + table[start].offset);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ a;
}

/*
 * Return the folded-case equivalent of "a", which is a UCS-4 character.  Uses
 * simple case folding.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_fold(ಪೂರ್ಣಾಙ್ಕ a)
{
    ಯದಿ (a < 0x80)
	// be fast for ASCII
	ಪ್ರತ್ಯಾಗಮ a >= 0x41 && a <= 0x5a ? a + 32 : a;
    ಪ್ರತ್ಯಾಗಮ utf_convert(a, foldCase, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(foldCase));
}

ಸ್ಥಿರ convertStruct toLower[] =
{
	{0x41,0x5a,1,32},
	{0xc0,0xd6,1,32},
	{0xd8,0xde,1,32},
	{0x100,0x12e,2,1},
	{0x130,0x130,-1,-199},
	{0x132,0x136,2,1},
	{0x139,0x147,2,1},
	{0x14a,0x176,2,1},
	{0x178,0x178,-1,-121},
	{0x179,0x17d,2,1},
	{0x181,0x181,-1,210},
	{0x182,0x184,2,1},
	{0x186,0x186,-1,206},
	{0x187,0x187,-1,1},
	{0x189,0x18a,1,205},
	{0x18b,0x18b,-1,1},
	{0x18e,0x18e,-1,79},
	{0x18f,0x18f,-1,202},
	{0x190,0x190,-1,203},
	{0x191,0x191,-1,1},
	{0x193,0x193,-1,205},
	{0x194,0x194,-1,207},
	{0x196,0x196,-1,211},
	{0x197,0x197,-1,209},
	{0x198,0x198,-1,1},
	{0x19c,0x19c,-1,211},
	{0x19d,0x19d,-1,213},
	{0x19f,0x19f,-1,214},
	{0x1a0,0x1a4,2,1},
	{0x1a6,0x1a6,-1,218},
	{0x1a7,0x1a7,-1,1},
	{0x1a9,0x1a9,-1,218},
	{0x1ac,0x1ac,-1,1},
	{0x1ae,0x1ae,-1,218},
	{0x1af,0x1af,-1,1},
	{0x1b1,0x1b2,1,217},
	{0x1b3,0x1b5,2,1},
	{0x1b7,0x1b7,-1,219},
	{0x1b8,0x1bc,4,1},
	{0x1c4,0x1c4,-1,2},
	{0x1c5,0x1c5,-1,1},
	{0x1c7,0x1c7,-1,2},
	{0x1c8,0x1c8,-1,1},
	{0x1ca,0x1ca,-1,2},
	{0x1cb,0x1db,2,1},
	{0x1de,0x1ee,2,1},
	{0x1f1,0x1f1,-1,2},
	{0x1f2,0x1f4,2,1},
	{0x1f6,0x1f6,-1,-97},
	{0x1f7,0x1f7,-1,-56},
	{0x1f8,0x21e,2,1},
	{0x220,0x220,-1,-130},
	{0x222,0x232,2,1},
	{0x23a,0x23a,-1,10795},
	{0x23b,0x23b,-1,1},
	{0x23d,0x23d,-1,-163},
	{0x23e,0x23e,-1,10792},
	{0x241,0x241,-1,1},
	{0x243,0x243,-1,-195},
	{0x244,0x244,-1,69},
	{0x245,0x245,-1,71},
	{0x246,0x24e,2,1},
	{0x370,0x372,2,1},
	{0x376,0x376,-1,1},
	{0x37f,0x37f,-1,116},
	{0x386,0x386,-1,38},
	{0x388,0x38a,1,37},
	{0x38c,0x38c,-1,64},
	{0x38e,0x38f,1,63},
	{0x391,0x3a1,1,32},
	{0x3a3,0x3ab,1,32},
	{0x3cf,0x3cf,-1,8},
	{0x3d8,0x3ee,2,1},
	{0x3f4,0x3f4,-1,-60},
	{0x3f7,0x3f7,-1,1},
	{0x3f9,0x3f9,-1,-7},
	{0x3fa,0x3fa,-1,1},
	{0x3fd,0x3ff,1,-130},
	{0x400,0x40f,1,80},
	{0x410,0x42f,1,32},
	{0x460,0x480,2,1},
	{0x48a,0x4be,2,1},
	{0x4c0,0x4c0,-1,15},
	{0x4c1,0x4cd,2,1},
	{0x4d0,0x52e,2,1},
	{0x531,0x556,1,48},
	{0x10a0,0x10c5,1,7264},
	{0x10c7,0x10cd,6,7264},
	{0x13a0,0x13ef,1,38864},
	{0x13f0,0x13f5,1,8},
	{0x1c89,0x1c89,-1,1},
	{0x1c90,0x1cba,1,-3008},
	{0x1cbd,0x1cbf,1,-3008},
	{0x1e00,0x1e94,2,1},
	{0x1e9e,0x1e9e,-1,-7615},
	{0x1ea0,0x1efe,2,1},
	{0x1f08,0x1f0f,1,-8},
	{0x1f18,0x1f1d,1,-8},
	{0x1f28,0x1f2f,1,-8},
	{0x1f38,0x1f3f,1,-8},
	{0x1f48,0x1f4d,1,-8},
	{0x1f59,0x1f5f,2,-8},
	{0x1f68,0x1f6f,1,-8},
	{0x1f88,0x1f8f,1,-8},
	{0x1f98,0x1f9f,1,-8},
	{0x1fa8,0x1faf,1,-8},
	{0x1fb8,0x1fb9,1,-8},
	{0x1fba,0x1fbb,1,-74},
	{0x1fbc,0x1fbc,-1,-9},
	{0x1fc8,0x1fcb,1,-86},
	{0x1fcc,0x1fcc,-1,-9},
	{0x1fd8,0x1fd9,1,-8},
	{0x1fda,0x1fdb,1,-100},
	{0x1fe8,0x1fe9,1,-8},
	{0x1fea,0x1feb,1,-112},
	{0x1fec,0x1fec,-1,-7},
	{0x1ff8,0x1ff9,1,-128},
	{0x1ffa,0x1ffb,1,-126},
	{0x1ffc,0x1ffc,-1,-9},
	{0x2126,0x2126,-1,-7517},
	{0x212a,0x212a,-1,-8383},
	{0x212b,0x212b,-1,-8262},
	{0x2132,0x2132,-1,28},
	{0x2160,0x216f,1,16},
	{0x2183,0x2183,-1,1},
	{0x24b6,0x24cf,1,26},
	{0x2c00,0x2c2f,1,48},
	{0x2c60,0x2c60,-1,1},
	{0x2c62,0x2c62,-1,-10743},
	{0x2c63,0x2c63,-1,-3814},
	{0x2c64,0x2c64,-1,-10727},
	{0x2c67,0x2c6b,2,1},
	{0x2c6d,0x2c6d,-1,-10780},
	{0x2c6e,0x2c6e,-1,-10749},
	{0x2c6f,0x2c6f,-1,-10783},
	{0x2c70,0x2c70,-1,-10782},
	{0x2c72,0x2c75,3,1},
	{0x2c7e,0x2c7f,1,-10815},
	{0x2c80,0x2ce2,2,1},
	{0x2ceb,0x2ced,2,1},
	{0x2cf2,0xa640,31054,1},
	{0xa642,0xa66c,2,1},
	{0xa680,0xa69a,2,1},
	{0xa722,0xa72e,2,1},
	{0xa732,0xa76e,2,1},
	{0xa779,0xa77b,2,1},
	{0xa77d,0xa77d,-1,-35332},
	{0xa77e,0xa786,2,1},
	{0xa78b,0xa78b,-1,1},
	{0xa78d,0xa78d,-1,-42280},
	{0xa790,0xa792,2,1},
	{0xa796,0xa7a8,2,1},
	{0xa7aa,0xa7aa,-1,-42308},
	{0xa7ab,0xa7ab,-1,-42319},
	{0xa7ac,0xa7ac,-1,-42315},
	{0xa7ad,0xa7ad,-1,-42305},
	{0xa7ae,0xa7ae,-1,-42308},
	{0xa7b0,0xa7b0,-1,-42258},
	{0xa7b1,0xa7b1,-1,-42282},
	{0xa7b2,0xa7b2,-1,-42261},
	{0xa7b3,0xa7b3,-1,928},
	{0xa7b4,0xa7c2,2,1},
	{0xa7c4,0xa7c4,-1,-48},
	{0xa7c5,0xa7c5,-1,-42307},
	{0xa7c6,0xa7c6,-1,-35384},
	{0xa7c7,0xa7c9,2,1},
	{0xa7cb,0xa7cb,-1,-42343},
	{0xa7cc,0xa7d0,4,1},
	{0xa7d6,0xa7da,2,1},
	{0xa7dc,0xa7dc,-1,-42561},
	{0xa7f5,0xa7f5,-1,1},
	{0xff21,0xff3a,1,32},
	{0x10400,0x10427,1,40},
	{0x104b0,0x104d3,1,40},
	{0x10570,0x1057a,1,39},
	{0x1057c,0x1058a,1,39},
	{0x1058c,0x10592,1,39},
	{0x10594,0x10595,1,39},
	{0x10c80,0x10cb2,1,64},
	{0x10d50,0x10d65,1,32},
	{0x118a0,0x118bf,1,32},
	{0x16e40,0x16e5f,1,32},
	{0x1e900,0x1e921,1,34}
};

// Note: UnicodeData.txt does not define U+1E9E as being the corresponding upper
// case letter for U+00DF (ß), however it is part of the toLower table
ಸ್ಥಿರ convertStruct toUpper[] =
{
	{0x61,0x7a,1,-32},
	{0xb5,0xb5,-1,743},
	{0xe0,0xf6,1,-32},
	{0xf8,0xfe,1,-32},
	{0xff,0xff,-1,121},
	{0x101,0x12f,2,-1},
	{0x131,0x131,-1,-232},
	{0x133,0x137,2,-1},
	{0x13a,0x148,2,-1},
	{0x14b,0x177,2,-1},
	{0x17a,0x17e,2,-1},
	{0x17f,0x17f,-1,-300},
	{0x180,0x180,-1,195},
	{0x183,0x185,2,-1},
	{0x188,0x18c,4,-1},
	{0x192,0x192,-1,-1},
	{0x195,0x195,-1,97},
	{0x199,0x199,-1,-1},
	{0x19a,0x19a,-1,163},
	{0x19b,0x19b,-1,42561},
	{0x19e,0x19e,-1,130},
	{0x1a1,0x1a5,2,-1},
	{0x1a8,0x1ad,5,-1},
	{0x1b0,0x1b4,4,-1},
	{0x1b6,0x1b9,3,-1},
	{0x1bd,0x1bd,-1,-1},
	{0x1bf,0x1bf,-1,56},
	{0x1c5,0x1c5,-1,-1},
	{0x1c6,0x1c6,-1,-2},
	{0x1c8,0x1c8,-1,-1},
	{0x1c9,0x1c9,-1,-2},
	{0x1cb,0x1cb,-1,-1},
	{0x1cc,0x1cc,-1,-2},
	{0x1ce,0x1dc,2,-1},
	{0x1dd,0x1dd,-1,-79},
	{0x1df,0x1ef,2,-1},
	{0x1f2,0x1f2,-1,-1},
	{0x1f3,0x1f3,-1,-2},
	{0x1f5,0x1f9,4,-1},
	{0x1fb,0x21f,2,-1},
	{0x223,0x233,2,-1},
	{0x23c,0x23c,-1,-1},
	{0x23f,0x240,1,10815},
	{0x242,0x247,5,-1},
	{0x249,0x24f,2,-1},
	{0x250,0x250,-1,10783},
	{0x251,0x251,-1,10780},
	{0x252,0x252,-1,10782},
	{0x253,0x253,-1,-210},
	{0x254,0x254,-1,-206},
	{0x256,0x257,1,-205},
	{0x259,0x259,-1,-202},
	{0x25b,0x25b,-1,-203},
	{0x25c,0x25c,-1,42319},
	{0x260,0x260,-1,-205},
	{0x261,0x261,-1,42315},
	{0x263,0x263,-1,-207},
	{0x264,0x264,-1,42343},
	{0x265,0x265,-1,42280},
	{0x266,0x266,-1,42308},
	{0x268,0x268,-1,-209},
	{0x269,0x269,-1,-211},
	{0x26a,0x26a,-1,42308},
	{0x26b,0x26b,-1,10743},
	{0x26c,0x26c,-1,42305},
	{0x26f,0x26f,-1,-211},
	{0x271,0x271,-1,10749},
	{0x272,0x272,-1,-213},
	{0x275,0x275,-1,-214},
	{0x27d,0x27d,-1,10727},
	{0x280,0x280,-1,-218},
	{0x282,0x282,-1,42307},
	{0x283,0x283,-1,-218},
	{0x287,0x287,-1,42282},
	{0x288,0x288,-1,-218},
	{0x289,0x289,-1,-69},
	{0x28a,0x28b,1,-217},
	{0x28c,0x28c,-1,-71},
	{0x292,0x292,-1,-219},
	{0x29d,0x29d,-1,42261},
	{0x29e,0x29e,-1,42258},
	{0x345,0x345,-1,84},
	{0x371,0x373,2,-1},
	{0x377,0x377,-1,-1},
	{0x37b,0x37d,1,130},
	{0x3ac,0x3ac,-1,-38},
	{0x3ad,0x3af,1,-37},
	{0x3b1,0x3c1,1,-32},
	{0x3c2,0x3c2,-1,-31},
	{0x3c3,0x3cb,1,-32},
	{0x3cc,0x3cc,-1,-64},
	{0x3cd,0x3ce,1,-63},
	{0x3d0,0x3d0,-1,-62},
	{0x3d1,0x3d1,-1,-57},
	{0x3d5,0x3d5,-1,-47},
	{0x3d6,0x3d6,-1,-54},
	{0x3d7,0x3d7,-1,-8},
	{0x3d9,0x3ef,2,-1},
	{0x3f0,0x3f0,-1,-86},
	{0x3f1,0x3f1,-1,-80},
	{0x3f2,0x3f2,-1,7},
	{0x3f3,0x3f3,-1,-116},
	{0x3f5,0x3f5,-1,-96},
	{0x3f8,0x3fb,3,-1},
	{0x430,0x44f,1,-32},
	{0x450,0x45f,1,-80},
	{0x461,0x481,2,-1},
	{0x48b,0x4bf,2,-1},
	{0x4c2,0x4ce,2,-1},
	{0x4cf,0x4cf,-1,-15},
	{0x4d1,0x52f,2,-1},
	{0x561,0x586,1,-48},
	{0x10d0,0x10fa,1,3008},
	{0x10fd,0x10ff,1,3008},
	{0x13f8,0x13fd,1,-8},
	{0x1c80,0x1c80,-1,-6254},
	{0x1c81,0x1c81,-1,-6253},
	{0x1c82,0x1c82,-1,-6244},
	{0x1c83,0x1c84,1,-6242},
	{0x1c85,0x1c85,-1,-6243},
	{0x1c86,0x1c86,-1,-6236},
	{0x1c87,0x1c87,-1,-6181},
	{0x1c88,0x1c88,-1,35266},
	{0x1c8a,0x1c8a,-1,-1},
	{0x1d79,0x1d79,-1,35332},
	{0x1d7d,0x1d7d,-1,3814},
	{0x1d8e,0x1d8e,-1,35384},
	{0x1e01,0x1e95,2,-1},
	{0x1e9b,0x1e9b,-1,-59},
	{0x1ea1,0x1eff,2,-1},
	{0x1f00,0x1f07,1,8},
	{0x1f10,0x1f15,1,8},
	{0x1f20,0x1f27,1,8},
	{0x1f30,0x1f37,1,8},
	{0x1f40,0x1f45,1,8},
	{0x1f51,0x1f57,2,8},
	{0x1f60,0x1f67,1,8},
	{0x1f70,0x1f71,1,74},
	{0x1f72,0x1f75,1,86},
	{0x1f76,0x1f77,1,100},
	{0x1f78,0x1f79,1,128},
	{0x1f7a,0x1f7b,1,112},
	{0x1f7c,0x1f7d,1,126},
	{0x1f80,0x1f87,1,8},
	{0x1f90,0x1f97,1,8},
	{0x1fa0,0x1fa7,1,8},
	{0x1fb0,0x1fb1,1,8},
	{0x1fb3,0x1fb3,-1,9},
	{0x1fbe,0x1fbe,-1,-7205},
	{0x1fc3,0x1fc3,-1,9},
	{0x1fd0,0x1fd1,1,8},
	{0x1fe0,0x1fe1,1,8},
	{0x1fe5,0x1fe5,-1,7},
	{0x1ff3,0x1ff3,-1,9},
	{0x214e,0x214e,-1,-28},
	{0x2170,0x217f,1,-16},
	{0x2184,0x2184,-1,-1},
	{0x24d0,0x24e9,1,-26},
	{0x2c30,0x2c5f,1,-48},
	{0x2c61,0x2c61,-1,-1},
	{0x2c65,0x2c65,-1,-10795},
	{0x2c66,0x2c66,-1,-10792},
	{0x2c68,0x2c6c,2,-1},
	{0x2c73,0x2c76,3,-1},
	{0x2c81,0x2ce3,2,-1},
	{0x2cec,0x2cee,2,-1},
	{0x2cf3,0x2cf3,-1,-1},
	{0x2d00,0x2d25,1,-7264},
	{0x2d27,0x2d2d,6,-7264},
	{0xa641,0xa66d,2,-1},
	{0xa681,0xa69b,2,-1},
	{0xa723,0xa72f,2,-1},
	{0xa733,0xa76f,2,-1},
	{0xa77a,0xa77c,2,-1},
	{0xa77f,0xa787,2,-1},
	{0xa78c,0xa791,5,-1},
	{0xa793,0xa793,-1,-1},
	{0xa794,0xa794,-1,48},
	{0xa797,0xa7a9,2,-1},
	{0xa7b5,0xa7c3,2,-1},
	{0xa7c8,0xa7ca,2,-1},
	{0xa7cd,0xa7d1,4,-1},
	{0xa7d7,0xa7db,2,-1},
	{0xa7f6,0xa7f6,-1,-1},
	{0xab53,0xab53,-1,-928},
	{0xab70,0xabbf,1,-38864},
	{0xff41,0xff5a,1,-32},
	{0x10428,0x1044f,1,-40},
	{0x104d8,0x104fb,1,-40},
	{0x10597,0x105a1,1,-39},
	{0x105a3,0x105b1,1,-39},
	{0x105b3,0x105b9,1,-39},
	{0x105bb,0x105bc,1,-39},
	{0x10cc0,0x10cf2,1,-64},
	{0x10d70,0x10d85,1,-32},
	{0x118c0,0x118df,1,-32},
	{0x16e60,0x16e7f,1,-32},
	{0x1e922,0x1e943,1,-34}
};

/*
 * Return the upper-case equivalent of "a", which is a UCS-4 character.  Use
 * simple case folding.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_toupper(ಪೂರ್ಣಾಙ್ಕ a)
{
    // If 'casemap' contains "keepascii" use ASCII style toupper().
    ಯದಿ (a < 128 && (cmp_flags & CMP_KEEPASCII))
	ಪ್ರತ್ಯಾಗಮ TOUPPER_ASC(a);

#ಯದಿ defined(HAVE_TOWUPPER) && defined(__STDC_ISO_10646__)
    // If towupper() is available and handles Unicode, use it.
    ಯದಿ (!(cmp_flags & CMP_INTERNAL))
	ಪ್ರತ್ಯಾಗಮ towupper(a);
#endif

    // For characters below 128 use locale sensitive toupper().
    ಯದಿ (a < 128)
	ಪ್ರತ್ಯಾಗಮ TOUPPER_LOC(a);

    // For any other characters use the above mapping table.
    ಪ್ರತ್ಯಾಗಮ utf_convert(a, toUpper, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(toUpper));
}

    ಪೂರ್ಣಾಙ್ಕ
utf_islower(ಪೂರ್ಣಾಙ್ಕ a)
{
    // German sharp s is lower case but has no upper case equivalent.
    ಪ್ರತ್ಯಾಗಮ (utf_toupper(a) != a) || a == 0xdf;
}

/*
 * Return the lower-case equivalent of "a", which is a UCS-4 character.  Use
 * simple case folding.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_tolower(ಪೂರ್ಣಾಙ್ಕ a)
{
    // If 'casemap' contains "keepascii" use ASCII style tolower().
    ಯದಿ (a < 128 && (cmp_flags & CMP_KEEPASCII))
	ಪ್ರತ್ಯಾಗಮ TOLOWER_ASC(a);

#ಯದಿ defined(HAVE_TOWLOWER) && defined(__STDC_ISO_10646__)
    // If towlower() is available and handles Unicode, use it.
    ಯದಿ (!(cmp_flags & CMP_INTERNAL))
	ಪ್ರತ್ಯಾಗಮ towlower(a);
#endif

    // For characters below 128 use locale sensitive tolower().
    ಯದಿ (a < 128)
	ಪ್ರತ್ಯಾಗಮ TOLOWER_LOC(a);

    // For any other characters use the above mapping table.
    ಪ್ರತ್ಯಾಗಮ utf_convert(a, toLower, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(toLower));
}

    ಪೂರ್ಣಾಙ್ಕ
utf_isupper(ಪೂರ್ಣಾಙ್ಕ a)
{
    ಪ್ರತ್ಯಾಗಮ (utf_tolower(a) != a);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf_strnicmp(
    char_u      *s1,
    char_u      *s2,
    size_t      n1,
    size_t      n2)
{
    ಪೂರ್ಣಾಙ್ಕ		c1, c2, cdiff;
    char_u	buffer[6];

    ಪ್ರತಿ (;;)
    {
	c1 = utf_safe_read_char_adv(&s1, &n1);
	c2 = utf_safe_read_char_adv(&s2, &n2);

	ಯದಿ (c1 <= 0 || c2 <= 0)
	    ಭಙ್ಗ;

	ಯದಿ (c1 == c2)
	    ಅನುವರ್ತ;

	cdiff = utf_fold(c1) - utf_fold(c2);
	ಯದಿ (cdiff != 0)
	    ಪ್ರತ್ಯಾಗಮ cdiff;
    }

    // some string ended or has an incomplete/illegal character sequence

    ಯದಿ (c1 == 0 || c2 == 0)
    {
	// some string ended. shorter string is smaller
	ಯದಿ (c1 == 0 && c2 == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪ್ರತ್ಯಾಗಮ c1 == 0 ? -1 : 1;
    }

    // Continue with bytewise comparison to produce some result that
    // would make comparison operations involving this function transitive.
    //
    // If only one string had an error, comparison should be made with
    // folded version of the other string. In this case it is enough
    // to fold just one character to determine the result of comparison.

    ಯದಿ (c1 != -1 && c2 == -1)
    {
	n1 = utf_char2bytes(utf_fold(c1), buffer);
	s1 = buffer;
    }
    ಅನ್ಯಥಾ ಯದಿ (c2 != -1 && c1 == -1)
    {
	n2 = utf_char2bytes(utf_fold(c2), buffer);
	s2 = buffer;
    }

    ಯಾವತ್ (n1 > 0 && n2 > 0 && *s1 != NUL && *s2 != NUL)
    {
	cdiff = (ಪೂರ್ಣಾಙ್ಕ)(*s1) - (ಪೂರ್ಣಾಙ್ಕ)(*s2);
	ಯದಿ (cdiff != 0)
	    ಪ್ರತ್ಯಾಗಮ cdiff;

	s1++;
	s2++;
	n1--;
	n2--;
    }

    ಯದಿ (n1 > 0 && *s1 == NUL)
	n1 = 0;
    ಯದಿ (n2 > 0 && *s2 == NUL)
	n2 = 0;

    ಯದಿ (n1 == 0 && n2 == 0)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ n1 == 0 ? -1 : 1;
}

/*
 * Version of strnicmp() that handles multi-byte characters.
 * Needed for Big5, Shift-JIS and UTF-8 encoding.  Other DBCS encodings can
 * probably use strnicmp(), because there are no ASCII characters in the
 * second byte.
 * Returns zero if s1 and s2 are equal (ignoring case), the difference between
 * two characters otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_strnicmp2(char_u *s1, char_u *s2, size_t n1, size_t n2)
{
    ಯದಿ (n1 == n2 || !enc_utf8)
	ಪ್ರತ್ಯಾಗಮ mb_strnicmp(s1, s2, n1);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ utf_strnicmp(s1, s2, n1, n2);
}

    ಪೂರ್ಣಾಙ್ಕ
mb_strnicmp(char_u *s1, char_u *s2, size_t nn)
{
    ಪೂರ್ಣಾಙ್ಕ		i, l;
    ಪೂರ್ಣಾಙ್ಕ		cdiff;
    ಪೂರ್ಣಾಙ್ಕ		n = (ಪೂರ್ಣಾಙ್ಕ)nn;

    ಯದಿ (enc_utf8)
    {
	ಪ್ರತ್ಯಾಗಮ utf_strnicmp(s1, s2, nn, nn);
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < n; i += l)
	{
	    ಯದಿ (s1[i] == NUL && s2[i] == NUL)	// both strings end
		ಪ್ರತ್ಯಾಗಮ 0;

	    l = (*mb_ptr2len)(s1 + i);
	    ಯದಿ (l <= 1)
	    {
		// Single byte: first check normally, then with ignore case.
		ಯದಿ (s1[i] != s2[i])
		{
		    cdiff = MB_TOLOWER(s1[i]) - MB_TOLOWER(s2[i]);
		    ಯದಿ (cdiff != 0)
			ಪ್ರತ್ಯಾಗಮ cdiff;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		// For non-Unicode multi-byte don't ignore case.
		ಯದಿ (l > n - i)
		    l = n - i;
		cdiff = STRNCMP(s1 + i, s2 + i, l);
		ಯದಿ (cdiff != 0)
		    ಪ್ರತ್ಯಾಗಮ cdiff;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * "g8": show bytes of the UTF-8 char under the cursor.  Doesn't matter what
 * 'encoding' has been set to.
 */
    ಶೂನ್ಯ
show_utf8(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		rlen = 0;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		clen;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Get the byte length of the char under the cursor, including composing
    // characters.
    ಸಾಲು = ml_get_cursor();
    len = utfc_ptr2len(ಸಾಲು);
    ಯದಿ (len == 0)
    {
	ಸಂದೇಶ("NUL");
	ಪ್ರತ್ಯಾಗಮ;
    }

    clen = 0;
    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	ಯದಿ (clen == 0)
	{
	    // start of (composing) character, get its length
	    ಯದಿ (i > 0)
	    {
		STRCPY(IObuff + rlen, "+ ");
		rlen += 2;
	    }
	    clen = utf_ptr2len(ಸಾಲು + i);
	}
	sprintf((ವರ್ಣ *)IObuff + rlen, "%02x ",
		(ಸಾಲು[i] == NL) ? NUL : ಸಾಲು[i]);  // NUL is stored as NL
	--clen;
	rlen += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(IObuff + rlen);
	ಯದಿ (rlen > IOSIZE - 20)
	    ಭಙ್ಗ;
    }

    ಸಂದೇಶ((ವರ್ಣ *)IObuff);
}

/*
 * mb_head_off() function pointer.
 * Return offset from "p" to the first byte of the character it points into.
 * If "p" points to the NUL at the end of the string return 0.
 * Returns 0 when already at the first byte of a character.
 */
    ಪೂರ್ಣಾಙ್ಕ
latin_head_off(char_u *base UNUSED, char_u *p UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dbcs_head_off(char_u *base, char_u *p)
{
    char_u	*q;

    // It can't be a trailing byte when not using DBCS, at the start of the
    // string or the previous byte can't start a double-byte.
    ಯದಿ (p <= base || MB_BYTE2LEN(p[-1]) == 1 || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // This is slow: need to start at the base and go forward until the
    // byte we are looking for.  Return 1 when we went past it, 0 otherwise.
    q = base;
    ಯಾವತ್ (q < p)
	q += dbcs_ptr2len(q);
    ಪ್ರತ್ಯಾಗಮ (q == p) ? 0 : 1;
}

/*
 * Special version of dbcs_head_off() that works for ScreenLines[], where
 * single-width DBCS_JPNU characters are stored separately.
 */
    ಪೂರ್ಣಾಙ್ಕ
dbcs_screen_head_off(char_u *base, char_u *p)
{
    char_u	*q;

    // It can't be a trailing byte when not using DBCS, at the start of the
    // string or the previous byte can't start a double-byte.
    // For euc-jp an 0x8e byte in the previous cell always means we have a
    // lead byte in the current cell.
    ಯದಿ (p <= base
	    || (enc_dbcs == DBCS_JPNU && p[-1] == 0x8e)
	    || MB_BYTE2LEN(p[-1]) == 1
	    || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // This is slow: need to start at the base and go forward until the
    // byte we are looking for.  Return 1 when we went past it, 0 otherwise.
    // For DBCS_JPNU look out for 0x8e, which means the second byte is not
    // stored as the next byte.
    q = base;
    ಯಾವತ್ (q < p)
    {
	ಯದಿ (enc_dbcs == DBCS_JPNU && *q == 0x8e)
	    ++q;
	ಅನ್ಯಥಾ
	    q += dbcs_ptr2len(q);
    }
    ಪ್ರತ್ಯಾಗಮ (q == p) ? 0 : 1;
}

/*
 * Return offset from "p" to the start of a character, including composing
 * characters.  "base" must be the start of the string, which must be NUL
 * terminated.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_head_off(char_u *base, char_u *p)
{
    char_u	*q;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		len;
#ifdef FEAT_ARABIC
    char_u	*j;
#endif

    ಯದಿ (*p < 0x80)		// be quick for ASCII
	ಪ್ರತ್ಯಾಗಮ 0;

    // Skip backwards over trailing bytes: 10xx.xxxx
    // Skip backwards again if on a composing char.
    ಪ್ರತಿ (q = p; ; --q)
    {
	// Move s to the last byte of this char.
	ಪ್ರತಿ (s = q; (s[1] & 0xc0) == 0x80; ++s)
	    ;
	// Move q to the first byte of this char.
	ಯಾವತ್ (q > base && (*q & 0xc0) == 0x80)
	    --q;
	// Check for illegal sequence. Do allow an illegal byte after where we
	// started.
	len = utf8len_tab[*q];
	ಯದಿ (len != (ಪೂರ್ಣಾಙ್ಕ)(s - q + 1) && len != (ಪೂರ್ಣಾಙ್ಕ)(p - q + 1))
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (q <= base)
	    ಭಙ್ಗ;

	c = utf_ptr2char(q);
	ಯದಿ (utf_iscomposing(c))
	    ಅನುವರ್ತ;

#ifdef FEAT_ARABIC
	ಯದಿ (arabic_maycombine(c))
	{
	    // Advance to get a sneak-peak at the next char
	    j = q;
	    --j;
	    // Move j to the first byte of this char.
	    ಯಾವತ್ (j > base && (*j & 0xc0) == 0x80)
		--j;
	    ಯದಿ (arabic_combine(utf_ptr2char(j), c))
		ಅನುವರ್ತ;
	}
#endif
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - q);
}

/*
 * Whether space is NOT allowed before/after 'c'.
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_eat_space(ಪೂರ್ಣಾಙ್ಕ cc)
{
    ಪ್ರತ್ಯಾಗಮ ((cc >= 0x2000 && cc <= 0x206F)	// General punctuations
	 || (cc >= 0x2e00 && cc <= 0x2e7f)	// Supplemental punctuations
	 || (cc >= 0x3000 && cc <= 0x303f)	// CJK symbols and punctuations
	 || (cc >= 0xff01 && cc <= 0xff0f)	// Full width ASCII punctuations
	 || (cc >= 0xff1a && cc <= 0xff20)	// ..
	 || (cc >= 0xff3b && cc <= 0xff40)	// ..
	 || (cc >= 0xff5b && cc <= 0xff65));	// ..
}

/*
 * Whether line break is allowed before "cc".
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_allow_break_before(ಪೂರ್ಣಾಙ್ಕ cc)
{
    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ BOL_prohibition_punct[] =
    {
	'!',
	'%',
	')',
	',',
	':',
	';',
	'>',
	'?',
	']',
	'}',
	0x2019, // ’ right single quotation mark
	0x201d, // ” right double quotation mark
	0x2020, // † dagger
	0x2021, // ‡ double dagger
	0x2026, // … horizontal ellipsis
	0x2030, // ‰ per mille sign
	0x2031, // ‱ per ten thousand sign
	0x203c, // ‼ double exclamation mark
	0x2047, // ⁇ double question mark
	0x2048, // ⁈ question exclamation mark
	0x2049, // ⁉ exclamation question mark
	0x2103, // ℃ degree celsius
	0x2109, // ℉ degree fahrenheit
	0x3001, // 、 ideographic comma
	0x3002, // 。 ideographic full stop
	0x3009, // 〉 right angle bracket
	0x300b, // 》 right double angle bracket
	0x300d, // 」 right corner bracket
	0x300f, // 』 right white corner bracket
	0x3011, // 】 right black lenticular bracket
	0x3015, // 〕 right tortoise shell bracket
	0x3017, // 〗 right white lenticular bracket
	0x3019, // 〙 right white tortoise shell bracket
	0x301b, // 〛 right white square bracket
	0xff01, // ！ fullwidth exclamation mark
	0xff09, // ） fullwidth right parenthesis
	0xff0c, // ， fullwidth comma
	0xff0e, // ． fullwidth full stop
	0xff1a, // ： fullwidth colon
	0xff1b, // ； fullwidth semicolon
	0xff1f, // ？ fullwidth question mark
	0xff3d, // ］ fullwidth right square bracket
	0xff5d, // ｝ fullwidth right curly bracket
    };

    ಪೂರ್ಣಾಙ್ಕ first = 0;
    ಪೂರ್ಣಾಙ್ಕ last  = ARRAY_LENGTH(BOL_prohibition_punct) - 1;
    ಪೂರ್ಣಾಙ್ಕ mid   = 0;

    ಯಾವತ್ (first < last)
    {
	mid = (first + last)/2;

	ಯದಿ (cc == BOL_prohibition_punct[mid])
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಅನ್ಯಥಾ ಯದಿ (cc > BOL_prohibition_punct[mid])
	    first = mid + 1;
	ಅನ್ಯಥಾ
	    last = mid - 1;
    }

    ಪ್ರತ್ಯಾಗಮ cc != BOL_prohibition_punct[first];
}

/*
 * Whether line break is allowed after "cc".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf_allow_break_after(ಪೂರ್ಣಾಙ್ಕ cc)
{
    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ EOL_prohibition_punct[] =
    {
	'(',
	'<',
	'[',
	'`',
	'{',
	//0x2014, // — em dash
	0x2018, // ‘ left single quotation mark
	0x201c, // “ left double quotation mark
	//0x2053, // ～ swung dash
	0x3008, // 〈 left angle bracket
	0x300a, // 《 left double angle bracket
	0x300c, // 「 left corner bracket
	0x300e, // 『 left white corner bracket
	0x3010, // 【 left black lenticular bracket
	0x3014, // 〔 left tortoise shell bracket
	0x3016, // 〖 left white lenticular bracket
	0x3018, // 〘 left white tortoise shell bracket
	0x301a, // 〚 left white square bracket
	0xff08, // （ fullwidth left parenthesis
	0xff3b, // ［ fullwidth left square bracket
	0xff5b, // ｛ fullwidth left curly bracket
    };

    ಪೂರ್ಣಾಙ್ಕ first = 0;
    ಪೂರ್ಣಾಙ್ಕ last  = ARRAY_LENGTH(EOL_prohibition_punct) - 1;
    ಪೂರ್ಣಾಙ್ಕ mid   = 0;

    ಯಾವತ್ (first < last)
    {
	mid = (first + last)/2;

	ಯದಿ (cc == EOL_prohibition_punct[mid])
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಅನ್ಯಥಾ ಯದಿ (cc > EOL_prohibition_punct[mid])
	    first = mid + 1;
	ಅನ್ಯಥಾ
	    last = mid - 1;
    }

    ಪ್ರತ್ಯಾಗಮ cc != EOL_prohibition_punct[first];
}

/*
 * Whether line break is allowed between "cc" and "ncc".
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_allow_break(ಪೂರ್ಣಾಙ್ಕ cc, ಪೂರ್ಣಾಙ್ಕ ncc)
{
    // don't break between two-letter punctuations
    ಯದಿ (cc == ncc
	    && (cc == 0x2014 // em dash
		|| cc == 0x2026)) // horizontal ellipsis
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ utf_allow_break_after(cc) && utf_allow_break_before(ncc);
}

/*
 * Copy a character from "*fp" to "*tp" and advance the pointers.
 */
    ಶೂನ್ಯ
mb_copy_char(char_u **fp, char_u **tp)
{
    ಪೂರ್ಣಾಙ್ಕ	    l = (*mb_ptr2len)(*fp);

    mch_memmove(*tp, *fp, (size_t)l);
    *tp += l;
    *fp += l;
}

/*
 * Return the offset from "p" to the first byte of a character.  When "p" is
 * at the start of a character 0 is returned, otherwise the offset to the next
 * character.  Can start anywhere in a stream of bytes.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_off_next(char_u *base, char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		head_off = (*mb_head_off)(base, p);

    ಯದಿ (head_off == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ (*mb_ptr2len)(p - head_off) - head_off;
}

/*
 * Return the offset from "p" to the last byte of the character it points
 * into.  Can start anywhere in a stream of bytes.
 * Composing characters are not included.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_tail_off(char_u *base, char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j;

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (enc_utf8)
    {
	// Find the last character that is 10xx.xxxx
	ಪ್ರತಿ (i = 0; (p[i + 1] & 0xc0) == 0x80; ++i)
	    ;
	// Check for illegal sequence.
	ಪ್ರತಿ (j = 0; p - j > base; ++j)
	    ಯದಿ ((p[-j] & 0xc0) != 0x80)
		ಭಙ್ಗ;
	ಯದಿ (utf8len_tab[p[-j]] != i + j + 1)
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಪ್ರತ್ಯಾಗಮ i;
    }

    // It can't be the first byte if a double-byte when not using DBCS, at the
    // end of the string or the byte can't start a double-byte.
    ಯದಿ (enc_dbcs == 0 || p[1] == NUL || MB_BYTE2LEN(*p) == 1)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Return 1 when on the lead byte, 0 when on the tail byte.
    ಪ್ರತ್ಯಾಗಮ 1 - dbcs_head_off(base, p);
}

/*
 * Find the next illegal byte sequence.
 */
    ಶೂನ್ಯ
utf_find_illegal(ಶೂನ್ಯ)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    vimconv_T	vimconv;
    char_u	*tofree = ರಿಕ್ತ;

    vimconv.vc_type = CONV_NONE;
    ಯದಿ (enc_utf8 && (enc_canon_props(curbuf->b_p_fenc) & ENC_8BIT))
    {
	// 'encoding' is "utf-8" but we are editing a 8-bit encoded file,
	// possibly a utf-8 file with illegal bytes.  Setup for conversion
	// from utf-8 to 'fileencoding'.
	convert_setup(&vimconv, p_enc, curbuf->b_p_fenc);
    }

    curwin->w_cursor.coladd = 0;
    ಪ್ರತಿ (;;)
    {
	p = ml_get_cursor();
	ಯದಿ (vimconv.vc_type != CONV_NONE)
	{
	    vim_free(tofree);
	    tofree = string_convert(&vimconv, p, ರಿಕ್ತ);
	    ಯದಿ (tofree == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    p = tofree;
	}

	ಯಾವತ್ (*p != NUL)
	{
	    // Illegal means that there are not enough trail bytes (checked by
	    // utf_ptr2len()) or too many of them (overlong sequence).
	    len = utf_ptr2len(p);
	    ಯದಿ (*p >= 0x80 && (len == 1
				     || utf_char2len(utf_ptr2char(p)) != len))
	    {
		ಯದಿ (vimconv.vc_type == CONV_NONE)
		    curwin->w_cursor.col += (colnr_T)(p - ml_get_cursor());
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ	    l;

		    len = (ಪೂರ್ಣಾಙ್ಕ)(p - tofree);
		    ಪ್ರತಿ (p = ml_get_cursor(); *p != NUL && len-- > 0; p += l)
		    {
			l = utf_ptr2len(p);
			curwin->w_cursor.col += l;
		    }
		}
		curwin->w_set_curswant = TRUE;
		ಗಚ್ಛ theend;
	    }
	    p += len;
	}
	ಯದಿ (curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }

    // didn't find it: don't move and beep
    curwin->w_cursor = pos;
    beep_flush();

theend:
    vim_free(tofree);
    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_GUI_GTK) || defined(FEAT_SPELL) || defined(FEAT_EVAL)
/*
 * Return TRUE if string "s" is a valid utf-8 string.
 * When "end" is NULL stop at the first NUL.  Otherwise stop at "end".
 */
    ಪೂರ್ಣಾಙ್ಕ
utf_valid_string(char_u *s, char_u *end)
{
    ಪೂರ್ಣಾಙ್ಕ		l;
    char_u	*p = s;

    ಯಾವತ್ (end == ರಿಕ್ತ ? *p != NUL : p < end)
    {
	l = utf8len_tab_zero[*p];
	ಯದಿ (l == 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;	// invalid lead byte
	ಯದಿ (end != ರಿಕ್ತ && p + l > end)
	    ಪ್ರತ್ಯಾಗಮ FALSE;	// incomplete byte sequence
	++p;
	ಯಾವತ್ (--l > 0)
	    ಯದಿ ((*p++ & 0xc0) != 0x80)
		ಪ್ರತ್ಯಾಗಮ FALSE;	// invalid trail byte
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * Special version of mb_tail_off() for use in ScreenLines[].
 */
    ಪೂರ್ಣಾಙ್ಕ
dbcs_screen_tail_off(char_u *base, char_u *p)
{
    // It can't be the first byte if a double-byte when not using DBCS, at the
    // end of the string or the byte can't start a double-byte.
    // For euc-jp an 0x8e byte always means we have a lead byte in the current
    // cell.
    ಯದಿ (*p == NUL || p[1] == NUL
	    || (enc_dbcs == DBCS_JPNU && *p == 0x8e)
	    || MB_BYTE2LEN(*p) == 1)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Return 1 when on the lead byte, 0 when on the tail byte.
    ಪ್ರತ್ಯಾಗಮ 1 - dbcs_screen_head_off(base, p);
}
#endif

/*
 * If the cursor moves on an trail byte, set the cursor on the lead byte.
 * Thus it moves left if necessary.
 * Return TRUE when the cursor was adjusted.
 */
    ಶೂನ್ಯ
mb_adjust_cursor(ಶೂನ್ಯ)
{
    mb_adjustpos(curbuf, &curwin->w_cursor);
}

/*
 * Adjust position "*lp" to point to the first byte of a multi-byte character.
 * If it points to a tail byte it's moved backwards to the head byte.
 */
    ಶೂನ್ಯ
mb_adjustpos(buf_T *buf, pos_T *lp)
{
    char_u	*p;

    ಯದಿ (lp->col > 0 || lp->coladd > 1)
    {
	p = ml_get_buf(buf, lp->lnum, FALSE);
	ಯದಿ (*p == NUL || ml_get_buf_len(buf, lp->lnum) < lp->col)
	    lp->col = 0;
	ಅನ್ಯಥಾ
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	// Reset "coladd" when the cursor would be on the right half of a
	// double-wide character.
	ಯದಿ (lp->coladd == 1
		&& p[lp->col] != TAB
		&& vim_isprintc((*mb_ptr2char)(p + lp->col))
		&& ptr2cells(p + lp->col) > 1)
	    lp->coladd = 0;
    }
}

/*
 * Return a pointer to the character before "*p", if there is one.
 */
    char_u *
mb_prevptr(
    char_u *ಸಾಲು,	// start of the string
    char_u *p)
{
    ಯದಿ (p > ಸಾಲು)
	MB_PTR_BACK(ಸಾಲು, p);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Return the character length of "str".  Each multi-byte character (with
 * following composing characters) counts as one.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_charlen(char_u *str)
{
    char_u	*p = str;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತಿ (ಎಣಿಕೆ = 0; *p != NUL; ಎಣಿಕೆ++)
	p += (*mb_ptr2len)(p);

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 * Like mb_charlen() but for a string with specified length.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_charlen_len(char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u	*p = str;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;

    ಪ್ರತಿ (ಎಣಿಕೆ = 0; *p != NUL && p < str + len; ಎಣಿಕೆ++)
	p += (*mb_ptr2len)(p);

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 * Try to un-escape a multi-byte character.
 * Used for the "to" and "from" part of a mapping.
 * Return the un-escaped string if it is a multi-byte character, and advance
 * "pp" to just after the bytes that formed it.
 * Return NULL if no multi-byte char was found.
 */
    char_u *
mb_unescape(char_u **pp)
{
    ಸ್ಥಿರ char_u	buf[6];
    ಪೂರ್ಣಾಙ್ಕ			n;
    ಪೂರ್ಣಾಙ್ಕ			m = 0;
    char_u		*str = *pp;

    // Must translate K_SPECIAL KS_SPECIAL KE_FILLER to K_SPECIAL and CSI
    // KS_EXTRA KE_CSI to CSI.
    // Maximum length of a utf-8 character is 4 bytes.
    ಪ್ರತಿ (n = 0; str[n] != NUL && m < 4; ++n)
    {
	ಯದಿ (str[n] == K_SPECIAL
		&& str[n + 1] == KS_SPECIAL
		&& str[n + 2] == KE_FILLER)
	{
	    buf[m++] = K_SPECIAL;
	    n += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ ((str[n] == K_SPECIAL
#ifdef FEAT_GUI
		    || str[n] == CSI
#endif
		 )
		&& str[n + 1] == KS_EXTRA
		&& str[n + 2] == (ಪೂರ್ಣಾಙ್ಕ)KE_CSI)
	{
	    buf[m++] = CSI;
	    n += 2;
	}
	ಅನ್ಯಥಾ ಯದಿ (str[n] == K_SPECIAL
#ifdef FEAT_GUI
		|| str[n] == CSI
#endif
		)
	    ಭಙ್ಗ;		// a special key can't be a multibyte char
	ಅನ್ಯಥಾ
	    buf[m++] = str[n];
	buf[m] = NUL;

	// Return a multi-byte character if it's found.  An illegal sequence
	// will result in a 1 here.
	ಯದಿ ((*mb_ptr2len)(buf) > 1)
	{
	    *pp = str + n + 1;
	    ಪ್ರತ್ಯಾಗಮ buf;
	}

	// Bail out quickly for ASCII.
	ಯದಿ (buf[0] < 128)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE if the character at "row"/"col" on the screen is the left side
 * of a double-width character.
 * Caller must make sure "row" and "col" are not invalid!
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_lefthalve(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪ್ರತ್ಯಾಗಮ (*mb_off2cells)(ಸಾಲು_Offset[row] + col,
					ಸಾಲು_Offset[row] + screen_Columns) > 1;
}

/*
 * Correct a position on the screen, if it's the right half of a double-wide
 * char move it to the left half.  Returns the corrected column.
 */
    ಪೂರ್ಣಾಙ್ಕ
mb_fix_col(ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ row)
{
    ಪೂರ್ಣಾಙ್ಕ off;

    col = check_col(col);
    row = check_row(row);
    off = ಸಾಲು_Offset[row] + col;
    ಯದಿ (has_mbyte && Screen_ಸಾಲುಗಳು != ರಿಕ್ತ && col > 0
	    && ((enc_dbcs
		    && Screen_ಸಾಲುಗಳು[off] != NUL
		    && dbcs_screen_head_off(Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[row],
					 Screen_ಸಾಲುಗಳು + off))
		|| (enc_utf8 && Screen_ಸಾಲುಗಳು[off] == 0
						  && Screen_ಸಾಲುಗಳು_UC[off] == 0)))
	ಪ್ರತ್ಯಾಗಮ col - 1;
    ಪ್ರತ್ಯಾಗಮ col;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ enc_alias_ಹುಡುಕು(char_u *name);

/*
 * Skip the Vim specific head of a 'encoding' name.
 */
    char_u *
enc_skip(char_u *p)
{
    ಯದಿ (STRNCMP(p, "2byte-", 6) == 0)
	ಪ್ರತ್ಯಾಗಮ p + 6;
    ಯದಿ (STRNCMP(p, "8bit-", 5) == 0)
	ಪ್ರತ್ಯಾಗಮ p + 5;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Find the canonical name for encoding "enc".
 * When the name isn't recognized, returns "enc" itself, but with all lower
 * case characters and '_' replaced with '-'.
 * Returns an allocated string.  NULL for out-of-memory.
 */
    char_u *
enc_canonize(char_u *enc)
{
    char_u	*r;
    char_u	*p, *s;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (STRCMP(enc, "default") == 0)
    {
#ifdef MSWIN
	// Use the system encoding, the default is always utf-8.
	r = enc_locale();
#ಅನ್ಯಥಾ
	// Use the default encoding as it's found by set_init_1().
	r = get_encoding_default();
#endif
	ಯದಿ (r == ರಿಕ್ತ)
	    r = (char_u *)ENC_DFLT;
	ಪ್ರತ್ಯಾಗಮ vim_strsave(r);
    }

    // copy "enc" to allocated memory, with room for two '-'
    r = alloc(STRLEN(enc) + 3);
    ಯದಿ (r == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Make it all lower case and replace '_' with '-'.
    p = r;
    ಪ್ರತಿ (s = enc; *s != NUL; ++s)
    {
	ಯದಿ (*s == '_')
	    *p++ = '-';
	ಅನ್ಯಥಾ
	    *p++ = TOLOWER_ASC(*s);
    }
    *p = NUL;

    // Skip "2byte-" and "8bit-".
    p = enc_skip(r);

    // Change "microsoft-cp" to "cp".  Used in some spell files.
    ಯದಿ (STRNCMP(p, "microsoft-cp", 12) == 0)
	STRMOVE(p, p + 10);

    // "iso8859" -> "iso-8859"
    ಯದಿ (STRNCMP(p, "iso8859", 7) == 0)
    {
	STRMOVE(p + 4, p + 3);
	p[3] = '-';
    }

    // "iso-8859n" -> "iso-8859-n"
    ಯದಿ (STRNCMP(p, "iso-8859", 8) == 0 && SAFE_isdigit(p[8]))
    {
	STRMOVE(p + 9, p + 8);
	p[8] = '-';
    }

    // "latin-N" -> "latinN"
    ಯದಿ (STRNCMP(p, "latin-", 6) == 0)
	STRMOVE(p + 5, p + 6);

    ಯದಿ (enc_canon_ಹುಡುಕು(p) >= 0)
    {
	// canonical name can be used unmodified
	ಯದಿ (p != r)
	    STRMOVE(r, p);
    }
    ಅನ್ಯಥಾ ಯದಿ ((i = enc_alias_ಹುಡುಕು(p)) >= 0)
    {
	// alias recognized, get canonical name
	vim_free(r);
	r = vim_strsave((char_u *)enc_canon_table[i].name);
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Search for an encoding alias of "name".
 * Returns -1 when not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
enc_alias_ಹುಡುಕು(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; enc_alias_table[i].name != ರಿಕ್ತ; ++i)
	ಯದಿ (STRCMP(name, enc_alias_table[i].name) == 0)
	    ಪ್ರತ್ಯಾಗಮ enc_alias_table[i].canon;
    ಪ್ರತ್ಯಾಗಮ -1;
}


#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
/*
 * Get the canonicalized encoding from the specified locale string "locale"
 * or from the environment variables LC_ALL, LC_CTYPE and LANG.
 * Returns an allocated string when successful, NULL when not.
 */
    char_u *
enc_locale_env(ವರ್ಣ *locale)
{
    ವರ್ಣ	*s = locale;
    ವರ್ಣ	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ವರ್ಣ	buf[50];

    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಯದಿ ((s = getenv("LC_ALL")) == ರಿಕ್ತ || *s == NUL)
	    ಯದಿ ((s = getenv("LC_CTYPE")) == ರಿಕ್ತ || *s == NUL)
		s = getenv("LANG");

    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // The most generic locale format is:
    // language[_territory][.codeset][@modifier][+special][,[sponsor][_revision]]
    // If there is a '.' remove the part before it.
    // if there is something after the codeset, remove it.
    // Make the name lowercase and replace '_' with '-'.
    // Exception: "ja_JP.EUC" == "euc-jp", "zh_CN.EUC" = "euc-cn",
    // "ko_KR.EUC" == "euc-kr"
    ಯದಿ ((p = (ವರ್ಣ *)vim_strchr((char_u *)s, '.')) != ರಿಕ್ತ)
    {
	ಯದಿ (p > s + 2 && STRNICMP(p + 1, "EUC", 3) == 0
			&& !SAFE_isalnum((ಪೂರ್ಣಾಙ್ಕ)p[4]) && p[4] != '-' && p[-3] == '_')
	{
	    // copy "XY.EUC" to "euc-XY" to buf[10]
	    STRCPY(buf + 10, "euc-");
	    buf[14] = p[-2];
	    buf[15] = p[-1];
	    buf[16] = 0;
	    s = buf + 10;
	}
	ಅನ್ಯಥಾ
	    s = p + 1;
    }
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf) - 1 && s[i] != NUL; ++i)
    {
	ಯದಿ (s[i] == '_' || s[i] == '-')
	    buf[i] = '-';
	ಅನ್ಯಥಾ ಯದಿ (SAFE_isalnum(s[i]))
	    buf[i] = TOLOWER_ASC(s[i]);
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
    buf[i] = NUL;

    ಪ್ರತ್ಯಾಗಮ enc_canonize((char_u *)buf);
}
#endif

/*
 * Get the canonicalized encoding of the current locale.
 * Returns an allocated string when successful, NULL when not.
 */
    char_u *
enc_locale(ಶೂನ್ಯ)
{
#ifdef MSWIN
    ವರ್ಣ	buf[50];
    long	acp = GetACP();

    ಯದಿ (acp == 1200)
	STRCPY(buf, "ucs-2le");
    ಅನ್ಯಥಾ ಯದಿ (acp == 1252)	    // cp1252 is used as latin1
	STRCPY(buf, "latin1");
    ಅನ್ಯಥಾ ಯದಿ (acp == 65001)
	STRCPY(buf, "utf-8");
    ಅನ್ಯಥಾ
	sprintf(buf, "cp%ld", acp);

    ಪ್ರತ್ಯಾಗಮ enc_canonize((char_u *)buf);
#ಅನ್ಯಥಾ
    ವರ್ಣ	*s;

# ifdef HAVE_NL_LANGINFO_CODESET
    ಯದಿ ((s = nl_langinfo(CODESET)) == ರಿಕ್ತ || *s == NUL)
# endif
# ಯದಿ defined(HAVE_LOCALE_H) || defined(X_LOCALE)
	ಯದಿ ((s = setlocale(LC_CTYPE, ರಿಕ್ತ)) == ರಿಕ್ತ || *s == NUL)
# endif
	    s = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ enc_locale_env(s);
#endif
}

#ಯದಿ defined(MSWIN) || defined(FEAT_CYGWIN_WIN32_CLIPBOARD)
/*
 * Convert an encoding name to an MS-Windows codepage.
 * Returns zero if no codepage can be figured out.
 */
    ಪೂರ್ಣಾಙ್ಕ
encname2codepage(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		cp;
    char_u	*p = name;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಯದಿ (STRNCMP(p, "8bit-", 5) == 0)
	p += 5;
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p_enc, "2byte-", 6) == 0)
	p += 6;

    ಯದಿ (p[0] == 'c' && p[1] == 'p')
	cp = atoi((ವರ್ಣ *)p + 2);
    ಅನ್ಯಥಾ ಯದಿ ((idx = enc_canon_ಹುಡುಕು(p)) >= 0)
	cp = enc_canon_table[idx].codepage;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (IsValidCodePage(cp))
	ಪ್ರತ್ಯಾಗಮ cp;
    ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

#ಯದಿ defined(USE_ICONV)

/*
 * Call iconv_open() with a check if iconv() works properly (there are broken
 * versions).
 * Returns (void *)-1 if failed.
 * (should return iconv_t, but that causes problems with prototypes).
 */
    ಶೂನ್ಯ *
my_iconv_open(char_u *to, char_u *from)
{
    iconv_t	fd;
# define ICONV_TESTLEN 400
    char_u	tobuf[ICONV_TESTLEN];
    ವರ್ಣ	*p;
    size_t	tolen;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	iconv_ok = -1;

    ಯದಿ (iconv_ok == FALSE)
	ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)-1;	// detected a broken iconv() previously

# ifdef DYNAMIC_ICONV
    // Check if the iconv.dll can be found.
    ಯದಿ (!iconv_enabled(TRUE))
	ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)-1;
# endif

    fd = iconv_open((ವರ್ಣ *)enc_skip(to), (ವರ್ಣ *)enc_skip(from));

    ಯದಿ (fd != (iconv_t)-1 && iconv_ok == -1)
    {
	/*
	 * Do a dummy iconv() call to check if it actually works.  There is a
	 * version of iconv() on Linux that is broken.  We can't ignore it,
	 * because it's wide-spread.  The symptoms are that after outputting
	 * the initial shift state the "to" pointer is NULL and conversion
	 * stops for no apparent reason after about 8160 characters.
	 */
	p = (ವರ್ಣ *)tobuf;
	tolen = ICONV_TESTLEN;
	(ಶೂನ್ಯ)iconv(fd, ರಿಕ್ತ, ರಿಕ್ತ, &p, &tolen);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    iconv_ok = FALSE;
	    iconv_close(fd);
	    fd = (iconv_t)-1;
	}
	ಅನ್ಯಥಾ
	    iconv_ok = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)fd;
}

/*
 * Convert the string "str[slen]" with iconv().
 * If "unconvlenp" is not NULL handle the string ending in an incomplete
 * sequence and set "*unconvlenp" to the length of it.
 * Returns the converted string in allocated memory.  NULL for an error.
 * If resultlenp is not NULL, sets it to the result length in bytes.
 */
    ಸ್ಥಿರ char_u *
iconv_string(
    vimconv_T	*vcp,
    char_u	*str,
    ಪೂರ್ಣಾಙ್ಕ		slen,
    ಪೂರ್ಣಾಙ್ಕ		*unconvlenp,
    ಪೂರ್ಣಾಙ್ಕ		*resultlenp)
{
    ಧ್ರುವ ವರ್ಣ	*from;
    size_t	fromlen;
    ವರ್ಣ	*to;
    size_t	tolen;
    size_t	len = 0;
    size_t	done = 0;
    char_u	*result = ರಿಕ್ತ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		l;

    from = (ವರ್ಣ *)str;
    fromlen = slen;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (len == 0 || ICONV_ERRNO == ICONV_E2BIG)
	{
	    // Allocate enough room for most conversions.  When re-allocating
	    // increase the buffer size.
	    len = len + fromlen * 2 + 40;
	    p = alloc(len);
	    ಯದಿ (p != ರಿಕ್ತ && done > 0)
		mch_memmove(p, result, done);
	    vim_free(result);
	    result = p;
	    ಯದಿ (result == ರಿಕ್ತ)	// out of memory
		ಭಙ್ಗ;
	}

	to = (ವರ್ಣ *)result + done;
	tolen = len - done - 2;
	// Avoid a warning for systems with a wrong iconv() prototype by
	// casting the second argument to void *.
	ಯದಿ (iconv(vcp->vc_fd, (ಶೂನ್ಯ *)&from, &fromlen, &to, &tolen)
								!= (size_t)-1)
	{
	    // Finished, append a NUL.
	    *to = NUL;
	    ಭಙ್ಗ;
	}

	// Check both ICONV_EINVAL and EINVAL, because the dynamically loaded
	// iconv library may use one of them.
	ಯದಿ (!vcp->vc_fail && unconvlenp != ರಿಕ್ತ
		&& (ICONV_ERRNO == ICONV_EINVAL || ICONV_ERRNO == EINVAL))
	{
	    // Handle an incomplete sequence at the end.
	    *to = NUL;
	    *unconvlenp = (ಪೂರ್ಣಾಙ್ಕ)fromlen;
	    ಭಙ್ಗ;
	}

	// Check both ICONV_EILSEQ and EILSEQ, because the dynamically loaded
	// iconv library may use one of them.
	ಅನ್ಯಥಾ ಯದಿ (!vcp->vc_fail
		&& (ICONV_ERRNO == ICONV_EILSEQ || ICONV_ERRNO == EILSEQ
		    || ICONV_ERRNO == ICONV_EINVAL || ICONV_ERRNO == EINVAL))
	{
	    // Can't convert: insert a '?' and skip a character.  This assumes
	    // conversion from 'encoding' to something else.  In other
	    // situations we don't know what to skip anyway.
	    *to++ = '?';
	    ಯದಿ ((*mb_ptr2cells)((char_u *)from) > 1)
		*to++ = '?';
	    ಯದಿ (enc_utf8)
		l = utfc_ptr2len_len((char_u *)from, (ಪೂರ್ಣಾಙ್ಕ)fromlen);
	    ಅನ್ಯಥಾ
	    {
		l = (*mb_ptr2len)((char_u *)from);
		ಯದಿ (l > (ಪೂರ್ಣಾಙ್ಕ)fromlen)
		    l = (ಪೂರ್ಣಾಙ್ಕ)fromlen;
	    }
	    from += l;
	    fromlen -= l;
	}
	ಅನ್ಯಥಾ ಯದಿ (ICONV_ERRNO != ICONV_E2BIG)
	{
	    // conversion failed
	    VIM_CLEAR(result);
	    ಭಙ್ಗ;
	}
	// Not enough room or skipping illegal sequence.
	done = to - (ವರ್ಣ *)result;
    }

    ಯದಿ (resultlenp != ರಿಕ್ತ && result != ರಿಕ್ತ)
	*resultlenp = (ಪೂರ್ಣಾಙ್ಕ)(to - (ವರ್ಣ *)result);
    ಪ್ರತ್ಯಾಗಮ result;
}

# ಯದಿ defined(DYNAMIC_ICONV)
/*
 * Dynamically load the "iconv.dll" on Win32.
 */

#  ifndef DYNAMIC_ICONV	    // must be generating prototypes
#   define HINSTANCE ಪೂರ್ಣಾಙ್ಕ
#  endif
ಸ್ಥಿರ HINSTANCE hIconvDLL = 0;
ಸ್ಥಿರ HINSTANCE hMsvcrtDLL = 0;

#  ifndef DYNAMIC_ICONV_DLL
#   define DYNAMIC_ICONV_DLL "iconv.dll"
#   define DYNAMIC_ICONV_DLL_ALT1 "libiconv.dll"
#   define DYNAMIC_ICONV_DLL_ALT2 "libiconv2.dll"
#   define DYNAMIC_ICONV_DLL_ALT3 "libiconv-2.dll"
#  endif
#  ifndef DYNAMIC_MSVCRT_DLL
#   define DYNAMIC_MSVCRT_DLL "msvcrt.dll"
#  endif

/*
 * Try opening the iconv.dll and return TRUE if iconv() can be used.
 */
    ಪೂರ್ಣಾಙ್ಕ
iconv_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಯದಿ (hIconvDLL != 0 && hMsvcrtDLL != 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // The iconv DLL file goes under different names, try them all.
    // Do the "2" version first, it's newer.
#  ifdef DYNAMIC_ICONV_DLL_ALT2
    ಯದಿ (hIconvDLL == 0)
	hIconvDLL = vimLoadLib(DYNAMIC_ICONV_DLL_ALT2);
#  endif
#  ifdef DYNAMIC_ICONV_DLL_ALT3
    ಯದಿ (hIconvDLL == 0)
	hIconvDLL = vimLoadLib(DYNAMIC_ICONV_DLL_ALT3);
#  endif
    ಯದಿ (hIconvDLL == 0)
	hIconvDLL = vimLoadLib(DYNAMIC_ICONV_DLL);
#  ifdef DYNAMIC_ICONV_DLL_ALT1
    ಯದಿ (hIconvDLL == 0)
	hIconvDLL = vimLoadLib(DYNAMIC_ICONV_DLL_ALT1);
#  endif

    ಯದಿ (hIconvDLL != 0)
	hMsvcrtDLL = vimLoadLib(DYNAMIC_MSVCRT_DLL);
    ಯದಿ (hIconvDLL == 0 || hMsvcrtDLL == 0)
    {
	// Only give the message when 'verbose' is set, otherwise it might be
	// done whenever a conversion is attempted.
	ಯದಿ (verbose && p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_str_str),
		    hIconvDLL == 0 ? DYNAMIC_ICONV_DLL : DYNAMIC_MSVCRT_DLL,
		    GetWin32Error());
	    verbose_leave();
	}
	iconv_end();
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    iconv	= (size_t (*)(iconv_t, ಧ್ರುವ ವರ್ಣ **,
			size_t *, ವರ್ಣ **, size_t *))
				GetProcAddress(hIconvDLL, "libiconv");
    iconv_open	= (iconv_t (*)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *))
				GetProcAddress(hIconvDLL, "libiconv_open");
    iconv_close	= (ಪೂರ್ಣಾಙ್ಕ (*)(iconv_t))
				GetProcAddress(hIconvDLL, "libiconv_close");
    iconvctl	= (ಪೂರ್ಣಾಙ್ಕ (*)(iconv_t, ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *))
				GetProcAddress(hIconvDLL, "libiconvctl");
    iconv_errno	= (ಪೂರ್ಣಾಙ್ಕ *(*)(ಶೂನ್ಯ))get_dll_import_func(hIconvDLL, "_errno");
    ಯದಿ (iconv_errno == ರಿಕ್ತ)
	iconv_errno = (ಪೂರ್ಣಾಙ್ಕ *(*)(ಶೂನ್ಯ))GetProcAddress(hMsvcrtDLL, "_errno");
    ಯದಿ (iconv == ರಿಕ್ತ || iconv_open == ರಿಕ್ತ || iconv_close == ರಿಕ್ತ
	    || iconvctl == ರಿಕ್ತ || iconv_errno == ರಿಕ್ತ)
    {
	iconv_end();
	ಯದಿ (verbose && p_verbose > 0)
	{
	    verbose_enter();
	    semsg(_(e_could_not_load_library_function_str), "for libiconv");
	    verbose_leave();
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಶೂನ್ಯ
iconv_end(ಶೂನ್ಯ)
{
    // Don't use iconv() when inputting or outputting characters.
    ಯದಿ (ಒಳಹರಿವು_conv.vc_type == CONV_ICONV)
	convert_setup(&ಒಳಹರಿವು_conv, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ (output_conv.vc_type == CONV_ICONV)
	convert_setup(&output_conv, ರಿಕ್ತ, ರಿಕ್ತ);

    ಯದಿ (hIconvDLL != 0)
	FreeLibrary(hIconvDLL);
    ಯದಿ (hMsvcrtDLL != 0)
	FreeLibrary(hMsvcrtDLL);
    hIconvDLL = 0;
    hMsvcrtDLL = 0;
}
# endif // DYNAMIC_ICONV
#endif // USE_ICONV

#ಯದಿ defined(FEAT_EVAL)
/*
 * "getimstatus()" function
 */
    ಶೂನ್ಯ
f_getimstatus(typval_T *argvars UNUSED, typval_T *rettv)
{
# ಯದಿ defined(HAVE_ಒಳಹರಿವು_METHOD)
    rettv->vval.v_ಸಂಖ್ಯೆ = im_get_status();
# endif
}

/*
 * iconv() function
 */
    ಶೂನ್ಯ
f_iconv(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*from, *to, *str;
    vimconv_T	vimconv;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string(&argvars[0]);
    from = enc_canonize(enc_skip(tv_get_string_buf(&argvars[1], buf1)));
    to = enc_canonize(enc_skip(tv_get_string_buf(&argvars[2], buf2)));
    vimconv.vc_type = CONV_NONE;
    convert_setup(&vimconv, from, to);

    // If the encodings are equal, no conversion needed.
    ಯದಿ (vimconv.vc_type == CONV_NONE)
	rettv->vval.v_string = vim_strsave(str);
    ಅನ್ಯಥಾ
	rettv->vval.v_string = string_convert(&vimconv, str, ರಿಕ್ತ);

    convert_setup(&vimconv, ರಿಕ್ತ, ರಿಕ್ತ);
    vim_free(from);
    vim_free(to);
}
#endif

/*
 * Setup "vcp" for conversion from "from" to "to".
 * The names must have been made canonical with enc_canonize().
 * vcp->vc_type must have been initialized to CONV_NONE.
 * Note: cannot be used for conversion from/to ucs-2 and ucs-4 (will use utf-8
 * instead).
 * Afterwards invoke with "from" and "to" equal to NULL to cleanup.
 * Return FAIL when conversion is not supported, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
convert_setup(vimconv_T *vcp, char_u *from, char_u *to)
{
    ಪ್ರತ್ಯಾಗಮ convert_setup_ext(vcp, from, TRUE, to, TRUE);
}

/*
 * As convert_setup(), but only when from_unicode_is_utf8 is TRUE will all
 * "from" unicode charsets be considered utf-8.  Same for "to".
 */
    ಪೂರ್ಣಾಙ್ಕ
convert_setup_ext(
    vimconv_T	*vcp,
    char_u	*from,
    ಪೂರ್ಣಾಙ್ಕ		from_unicode_is_utf8,
    char_u	*to,
    ಪೂರ್ಣಾಙ್ಕ		to_unicode_is_utf8)
{
    ಪೂರ್ಣಾಙ್ಕ		from_prop;
    ಪೂರ್ಣಾಙ್ಕ		to_prop;
    ಪೂರ್ಣಾಙ್ಕ		from_is_utf8;
    ಪೂರ್ಣಾಙ್ಕ		to_is_utf8;

    // Reset to no conversion.
#ifdef USE_ICONV
    ಯದಿ (vcp->vc_type == CONV_ICONV && vcp->vc_fd != (iconv_t)-1)
	iconv_close(vcp->vc_fd);
#endif
    vcp->vc_type = CONV_NONE;
    vcp->vc_factor = 1;
    vcp->vc_fail = FALSE;

    // No conversion when one of the names is empty or they are equal.
    ಯದಿ (from == ರಿಕ್ತ || *from == NUL || to == ರಿಕ್ತ || *to == NUL
						     || STRCMP(from, to) == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    from_prop = enc_canon_props(from);
    to_prop = enc_canon_props(to);
    ಯದಿ (from_unicode_is_utf8)
	from_is_utf8 = from_prop & ENC_UNICODE;
    ಅನ್ಯಥಾ
	from_is_utf8 = from_prop == ENC_UNICODE;
    ಯದಿ (to_unicode_is_utf8)
	to_is_utf8 = to_prop & ENC_UNICODE;
    ಅನ್ಯಥಾ
	to_is_utf8 = to_prop == ENC_UNICODE;

    ಯದಿ ((from_prop & ENC_LATIN1) && to_is_utf8)
    {
	// Internal latin1 -> utf-8 conversion.
	vcp->vc_type = CONV_TO_UTF8;
	vcp->vc_factor = 2;	// up to twice as long
    }
    ಅನ್ಯಥಾ ಯದಿ ((from_prop & ENC_LATIN9) && to_is_utf8)
    {
	// Internal latin9 -> utf-8 conversion.
	vcp->vc_type = CONV_9_TO_UTF8;
	vcp->vc_factor = 3;	// up to three as long (euro sign)
    }
    ಅನ್ಯಥಾ ಯದಿ (from_is_utf8 && (to_prop & ENC_LATIN1))
    {
	// Internal utf-8 -> latin1 conversion.
	vcp->vc_type = CONV_TO_LATIN1;
    }
    ಅನ್ಯಥಾ ಯದಿ (from_is_utf8 && (to_prop & ENC_LATIN9))
    {
	// Internal utf-8 -> latin9 conversion.
	vcp->vc_type = CONV_TO_LATIN9;
    }
#ifdef MSWIN
    // Win32-specific codepage <-> codepage conversion without iconv.
    ಅನ್ಯಥಾ ಯದಿ ((from_is_utf8 || encname2codepage(from) > 0)
	    && (to_is_utf8 || encname2codepage(to) > 0))
    {
	vcp->vc_type = CONV_CODEPAGE;
	vcp->vc_factor = 2;	// up to twice as long
	vcp->vc_cpfrom = from_is_utf8 ? 0 : encname2codepage(from);
	vcp->vc_cpto = to_is_utf8 ? 0 : encname2codepage(to);
    }
#endif
#ifdef MACOS_CONVERT
    ಅನ್ಯಥಾ ಯದಿ ((from_prop & ENC_MACROMAN) && (to_prop & ENC_LATIN1))
    {
	vcp->vc_type = CONV_MAC_LATIN1;
    }
    ಅನ್ಯಥಾ ಯದಿ ((from_prop & ENC_MACROMAN) && to_is_utf8)
    {
	vcp->vc_type = CONV_MAC_UTF8;
	vcp->vc_factor = 2;	// up to twice as long
    }
    ಅನ್ಯಥಾ ಯದಿ ((from_prop & ENC_LATIN1) && (to_prop & ENC_MACROMAN))
    {
	vcp->vc_type = CONV_LATIN1_MAC;
    }
    ಅನ್ಯಥಾ ಯದಿ (from_is_utf8 && (to_prop & ENC_MACROMAN))
    {
	vcp->vc_type = CONV_UTF8_MAC;
    }
#endif
#ifdef USE_ICONV
    ಅನ್ಯಥಾ
    {
	// Use iconv() for conversion.
	vcp->vc_fd = (iconv_t)my_iconv_open(
		to_is_utf8 ? (char_u *)"utf-8" : to,
		from_is_utf8 ? (char_u *)"utf-8" : from);
	ಯದಿ (vcp->vc_fd != (iconv_t)-1)
	{
	    vcp->vc_type = CONV_ICONV;
	    vcp->vc_factor = 4;	// could be longer too...
	}
    }
#endif
    ಯದಿ (vcp->vc_type == CONV_NONE)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_GUI) || defined(AMIGA) || defined(MSWIN)
/*
 * Do conversion on typed input characters in-place.
 * The input and output are not NUL terminated!
 * Returns the length after conversion.
 */
    ಪೂರ್ಣಾಙ್ಕ
convert_ಒಳಹರಿವು(char_u *ptr, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ maxlen)
{
    ಪ್ರತ್ಯಾಗಮ convert_ಒಳಹರಿವು_safe(ptr, len, maxlen, ರಿಕ್ತ, ರಿಕ್ತ);
}
#endif

/*
 * Like convert_input(), but when there is an incomplete byte sequence at the
 * end return that as an allocated string in "restp" and set "*restlenp" to
 * the length.  If "restp" is NULL it is not used.
 */
    ಪೂರ್ಣಾಙ್ಕ
convert_ಒಳಹರಿವು_safe(
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    char_u	**restp,
    ಪೂರ್ಣಾಙ್ಕ		*restlenp)
{
    char_u	*d;
    ಪೂರ್ಣಾಙ್ಕ		dlen = len;
    ಪೂರ್ಣಾಙ್ಕ		unconvertlen = 0;

    d = string_convert_ext(&ಒಳಹರಿವು_conv, ptr, &dlen,
					restp == ರಿಕ್ತ ? ರಿಕ್ತ : &unconvertlen);
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ dlen;

    ಯದಿ (dlen <= maxlen)
    {
	ಯದಿ (unconvertlen > 0)
	{
	    // Move the unconverted characters to allocated memory.
	    *restp = alloc(unconvertlen);
	    ಯದಿ (*restp != ರಿಕ್ತ)
		mch_memmove(*restp, ptr + len - unconvertlen, unconvertlen);
	    *restlenp = unconvertlen;
	}
	mch_memmove(ptr, d, dlen);
    }
    ಅನ್ಯಥಾ
	// result is too long, keep the unconverted text (the caller must
	// have done something wrong!)
	dlen = len;
    vim_free(d);
    ಪ್ರತ್ಯಾಗಮ dlen;
}

/*
 * Convert text "ptr[*lenp]" according to "vcp".
 * Returns the result in allocated memory and sets "*lenp".
 * When "lenp" is NULL, use NUL terminated strings.
 * Illegal chars are often changed to "?", unless vcp->vc_fail is set.
 * When something goes wrong, NULL is returned and "*lenp" is unchanged.
 */
    char_u *
string_convert(
    vimconv_T	*vcp,
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		*lenp)
{
    ಪ್ರತ್ಯಾಗಮ string_convert_ext(vcp, ptr, lenp, ರಿಕ್ತ);
}

/*
 * Like string_convert(), but when "unconvlenp" is not NULL and there are is
 * an incomplete sequence at the end it is not converted and "*unconvlenp" is
 * set to the number of remaining bytes.
 */
    char_u *
string_convert_ext(
    vimconv_T	*vcp,
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		*lenp,
    ಪೂರ್ಣಾಙ್ಕ		*unconvlenp)
{
    char_u	*retval = ರಿಕ್ತ;
    char_u	*d;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (lenp == ರಿಕ್ತ)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ptr);
    ಅನ್ಯಥಾ
	len = *lenp;
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ vim_strsave((char_u *)"");

    ವಿಕಲ್ಪ (vcp->vc_type)
    {
	ಪಕ್ಷ CONV_TO_UTF8:	// latin1 to utf-8 conversion
	    retval = alloc(len * 2 + 1);
	    ಯದಿ (retval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    d = retval;
	    ಪ್ರತಿ (i = 0; i < len; ++i)
	    {
		c = ptr[i];
		ಯದಿ (c < 0x80)
		    *d++ = c;
		ಅನ್ಯಥಾ
		{
		    *d++ = 0xc0 + ((unsigned)c >> 6);
		    *d++ = 0x80 + (c & 0x3f);
		}
	    }
	    *d = NUL;
	    ಯದಿ (lenp != ರಿಕ್ತ)
		*lenp = (ಪೂರ್ಣಾಙ್ಕ)(d - retval);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CONV_9_TO_UTF8:	// latin9 to utf-8 conversion
	    retval = alloc(len * 3 + 1);
	    ಯದಿ (retval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    d = retval;
	    ಪ್ರತಿ (i = 0; i < len; ++i)
	    {
		c = ptr[i];
		ವಿಕಲ್ಪ (c)
		{
		    ಪಕ್ಷ 0xa4: c = 0x20ac; ಭಙ್ಗ;   // euro
		    ಪಕ್ಷ 0xa6: c = 0x0160; ಭಙ್ಗ;   // S hat
		    ಪಕ್ಷ 0xa8: c = 0x0161; ಭಙ್ಗ;   // S -hat
		    ಪಕ್ಷ 0xb4: c = 0x017d; ಭಙ್ಗ;   // Z hat
		    ಪಕ್ಷ 0xb8: c = 0x017e; ಭಙ್ಗ;   // Z -hat
		    ಪಕ್ಷ 0xbc: c = 0x0152; ಭಙ್ಗ;   // OE
		    ಪಕ್ಷ 0xbd: c = 0x0153; ಭಙ್ಗ;   // oe
		    ಪಕ್ಷ 0xbe: c = 0x0178; ಭಙ್ಗ;   // Y
		}
		d += utf_char2bytes(c, d);
	    }
	    *d = NUL;
	    ಯದಿ (lenp != ರಿಕ್ತ)
		*lenp = (ಪೂರ್ಣಾಙ್ಕ)(d - retval);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CONV_TO_LATIN1:	// utf-8 to latin1 conversion
	ಪಕ್ಷ CONV_TO_LATIN9:	// utf-8 to latin9 conversion
	    retval = alloc(len + 1);
	    ಯದಿ (retval == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    d = retval;
	    ಪ್ರತಿ (i = 0; i < len; ++i)
	    {
		l = utf_ptr2len_len(ptr + i, len - i);
		ಯದಿ (l == 0)
		    *d++ = NUL;
		ಅನ್ಯಥಾ ಯದಿ (l == 1)
		{
		    ಪೂರ್ಣಾಙ್ಕ l_w = utf8len_tab_zero[ptr[i]];

		    ಯದಿ (l_w == 0)
		    {
			// Illegal utf-8 byte cannot be converted
			vim_free(retval);
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    }
		    ಯದಿ (unconvlenp != ರಿಕ್ತ && l_w > len - i)
		    {
			// Incomplete sequence at the end.
			*unconvlenp = len - i;
			ಭಙ್ಗ;
		    }
		    *d++ = ptr[i];
		}
		ಅನ್ಯಥಾ
		{
		    c = utf_ptr2char(ptr + i);
		    ಯದಿ (vcp->vc_type == CONV_TO_LATIN9)
			ವಿಕಲ್ಪ (c)
			{
			    ಪಕ್ಷ 0x20ac: c = 0xa4; ಭಙ್ಗ;   // euro
			    ಪಕ್ಷ 0x0160: c = 0xa6; ಭಙ್ಗ;   // S hat
			    ಪಕ್ಷ 0x0161: c = 0xa8; ಭಙ್ಗ;   // S -hat
			    ಪಕ್ಷ 0x017d: c = 0xb4; ಭಙ್ಗ;   // Z hat
			    ಪಕ್ಷ 0x017e: c = 0xb8; ಭಙ್ಗ;   // Z -hat
			    ಪಕ್ಷ 0x0152: c = 0xbc; ಭಙ್ಗ;   // OE
			    ಪಕ್ಷ 0x0153: c = 0xbd; ಭಙ್ಗ;   // oe
			    ಪಕ್ಷ 0x0178: c = 0xbe; ಭಙ್ಗ;   // Y
			    ಪಕ್ಷ 0xa4:
			    ಪಕ್ಷ 0xa6:
			    ಪಕ್ಷ 0xa8:
			    ಪಕ್ಷ 0xb4:
			    ಪಕ್ಷ 0xb8:
			    ಪಕ್ಷ 0xbc:
			    ಪಕ್ಷ 0xbd:
			    ಪಕ್ಷ 0xbe: c = 0x100; ಭಙ್ಗ; // not in latin9
			}
		    ಯದಿ (!utf_iscomposing(c))	// skip composing chars
		    {
			ಯದಿ (c < 0x100)
			    *d++ = c;
			ಅನ್ಯಥಾ ಯದಿ (vcp->vc_fail)
			{
			    vim_free(retval);
			    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			}
			ಅನ್ಯಥಾ
			{
			    *d++ = 0xbf;
			    ಯದಿ (utf_char2cells(c) > 1)
				*d++ = '?';
			}
		    }
		    i += l - 1;
		}
	    }
	    *d = NUL;
	    ಯದಿ (lenp != ರಿಕ್ತ)
		*lenp = (ಪೂರ್ಣಾಙ್ಕ)(d - retval);
	    ಭಙ್ಗ;

#ifdef MACOS_CONVERT
	ಪಕ್ಷ CONV_MAC_LATIN1:
	    retval = mac_string_convert(ptr, len, lenp, vcp->vc_fail,
					'm', 'l', unconvlenp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CONV_LATIN1_MAC:
	    retval = mac_string_convert(ptr, len, lenp, vcp->vc_fail,
					'l', 'm', unconvlenp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CONV_MAC_UTF8:
	    retval = mac_string_convert(ptr, len, lenp, vcp->vc_fail,
					'm', 'u', unconvlenp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ CONV_UTF8_MAC:
	    retval = mac_string_convert(ptr, len, lenp, vcp->vc_fail,
					'u', 'm', unconvlenp);
	    ಭಙ್ಗ;
#endif

#ifdef USE_ICONV
	ಪಕ್ಷ CONV_ICONV:	// conversion with output_conv.vc_fd
	    retval = iconv_string(vcp, ptr, len, unconvlenp, lenp);
	    ಭಙ್ಗ;
#endif
#ifdef MSWIN
	ಪಕ್ಷ CONV_CODEPAGE:		// codepage -> codepage
	{
	    ಪೂರ್ಣಾಙ್ಕ		retlen;
	    ಪೂರ್ಣಾಙ್ಕ		tmp_len;
	    short_u	*tmp;

	    // 1. codepage/UTF-8  ->  ucs-2.
	    ಯದಿ (vcp->vc_cpfrom == 0)
		tmp_len = utf8_to_utf16(ptr, len, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
	    {
		tmp_len = MultiByteToWideChar(vcp->vc_cpfrom,
					unconvlenp ? MB_ERR_INVALID_CHARS : 0,
					(ವರ್ಣ *)ptr, len, 0, 0);
		ಯದಿ (tmp_len == 0
			&& GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
		{
		    ಯದಿ (lenp != ರಿಕ್ತ)
			*lenp = 0;
		    ಯದಿ (unconvlenp != ರಿಕ್ತ)
			*unconvlenp = len;
		    retval = alloc(1);
		    ಯದಿ (retval)
			retval[0] = NUL;
		    ಪ್ರತ್ಯಾಗಮ retval;
		}
	    }
	    tmp = ALLOC_MULT(short_u, tmp_len);
	    ಯದಿ (tmp == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಯದಿ (vcp->vc_cpfrom == 0)
		utf8_to_utf16(ptr, len, tmp, unconvlenp);
	    ಅನ್ಯಥಾ
		MultiByteToWideChar(vcp->vc_cpfrom, 0,
			(ವರ್ಣ *)ptr, len, tmp, tmp_len);

	    // 2. ucs-2  ->  codepage/UTF-8.
	    ಯದಿ (vcp->vc_cpto == 0)
		retlen = utf16_to_utf8(tmp, tmp_len, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
		retlen = WideCharToMultiByte(vcp->vc_cpto, 0,
						    tmp, tmp_len, 0, 0, 0, 0);
	    retval = alloc(retlen + 1);
	    ಯದಿ (retval != ರಿಕ್ತ)
	    {
		ಯದಿ (vcp->vc_cpto == 0)
		    utf16_to_utf8(tmp, tmp_len, retval);
		ಅನ್ಯಥಾ
		    WideCharToMultiByte(vcp->vc_cpto, 0,
					  tmp, tmp_len,
					  (ವರ್ಣ *)retval, retlen, 0, 0);
		retval[retlen] = NUL;
		ಯದಿ (lenp != ರಿಕ್ತ)
		    *lenp = retlen;
	    }
	    vim_free(tmp);
	    ಭಙ್ಗ;
	}
#endif
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return 1 or 2 when "c" is in the cellwidth table.
 * Return 0 if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_cellwidth(ಪೂರ್ಣಾಙ್ಕ c UNUSED)
{
#ifdef FEAT_EVAL
    ಪ್ರತ್ಯಾಗಮ cw_ಮೌಲ್ಯ(c);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * Table set by setcellwidths().
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    long    first;
    long    last;
    ವರ್ಣ    width;
} cw_interval_T;

ಸ್ಥಿರ cw_interval_T	*cw_table = ರಿಕ್ತ;
ಸ್ಥಿರ size_t		cw_table_size = 0;

/*
 * Return 1 or 2 when "c" is in the cellwidth table.
 * Return 0 if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cw_ಮೌಲ್ಯ(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ mid, bot, ತುದಿ;

    ಯದಿ (cw_table == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // first quick check for Latin1 etc. characters
    ಯದಿ (c < cw_table[0].first)
	ಪ್ರತ್ಯಾಗಮ 0;

    // binary search in table
    bot = 0;
    ತುದಿ = (ಪೂರ್ಣಾಙ್ಕ)cw_table_size - 1;
    ಯಾವತ್ (ತುದಿ >= bot)
    {
	mid = (bot + ತುದಿ) / 2;
	ಯದಿ (cw_table[mid].last < c)
	    bot = mid + 1;
	ಅನ್ಯಥಾ ಯದಿ (cw_table[mid].first > c)
	    ತುದಿ = mid - 1;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ cw_table[mid].width;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_nr_compare(ಧ್ರುವ ಶೂನ್ಯ *a1, ಧ್ರುವ ಶೂನ್ಯ *a2)
{
    listitem_T *li1 = *(listitem_T **)a1;
    listitem_T *li2 = *(listitem_T **)a2;

    ಪ್ರತ್ಯಾಗಮ li1->li_tv.vval.v_ಸಂಖ್ಯೆ == li2->li_tv.vval.v_ಸಂಖ್ಯೆ ? 0 :
	li1->li_tv.vval.v_ಸಂಖ್ಯೆ > li2->li_tv.vval.v_ಸಂಖ್ಯೆ ? 1 : -1;
}

    ಶೂನ್ಯ
f_setcellwidths(typval_T *argvars, typval_T *rettv UNUSED)
{
    list_T	    *l;
    listitem_T	    *li;
    ಪೂರ್ಣಾಙ್ಕ		    item;
    ಪೂರ್ಣಾಙ್ಕ		    i;
    listitem_T	    **ptrs;
    cw_interval_T   *table = ರಿಕ್ತ;
    size_t	    table_size;
    cw_interval_T   *cw_table_save;
    size_t	    cw_table_size_save;
    ವರ್ಣ	    *error = ರಿಕ್ತ;

    ಯದಿ (check_for_nonnull_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    l = argvars[0].vval.v_list;
    table_size = (size_t)l->lv_len;
    ಯದಿ (table_size == 0)
	// Clearing the table.
	ಗಚ್ಛ update;

    ptrs = ALLOC_MULT(listitem_T *, table_size);
    ಯದಿ (ptrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Check that all entries are a list with three numbers, the range is
    // valid and the cell width is valid.
    item = 0;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	listitem_T *lili;
	varnumber_T n1;

	ಯದಿ (li->li_tv.v_type != VAR_LIST || li->li_tv.vval.v_list == ರಿಕ್ತ)
	{
	    semsg(_(e_list_item_nr_is_not_list), item);
	    vim_free(ptrs);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	lili = li->li_tv.vval.v_list->lv_first;
	ptrs[item] = lili;
	ಪ್ರತಿ (i = 0; lili != ರಿಕ್ತ; lili = lili->li_next, ++i)
	{
	    ಯದಿ (lili->li_tv.v_type != VAR_ಸಂಖ್ಯೆ)
		ಭಙ್ಗ;
	    ಯದಿ (i == 0)
	    {
		n1 = lili->li_tv.vval.v_ಸಂಖ್ಯೆ;
		ಯದಿ (n1 < 0x80)
		{
		    emsg(_(e_only_ಮೌಲ್ಯಗಳು_of_0x80_and_higher_supported));
		    vim_free(ptrs);
		    ಪ್ರತ್ಯಾಗಮ;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (i == 1 && lili->li_tv.vval.v_ಸಂಖ್ಯೆ < n1)
	    {
		semsg(_(e_list_item_nr_range_invalid), item);
		vim_free(ptrs);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (i == 2 && (lili->li_tv.vval.v_ಸಂಖ್ಯೆ < 1
					     || lili->li_tv.vval.v_ಸಂಖ್ಯೆ > 2))
	    {
		semsg(_(e_list_item_nr_cell_width_invalid), item);
		vim_free(ptrs);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
	ಯದಿ (i != 3)
	{
	    semsg(_(e_list_item_nr_does_not_contain_3_numbers), item);
	    vim_free(ptrs);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	++item;
    }

    // Sort the list on the first number.
    qsort((ಶೂನ್ಯ *)ptrs, table_size, ಪರಿಮಾಣ(listitem_T *), tv_nr_compare);

    table = ALLOC_MULT(cw_interval_T, table_size);
    ಯದಿ (table == ರಿಕ್ತ)
    {
	vim_free(ptrs);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Store the items in the new table.
    ಪ್ರತಿ (item = 0; (size_t)item < table_size; ++item)
    {
	listitem_T	*lili = ptrs[item];
	varnumber_T	n1;

	n1 = lili->li_tv.vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (item > 0 && n1 <= table[item - 1].last)
	{
	    semsg(_(e_overlapping_ranges_for_nr), (long)n1);
	    vim_free(ptrs);
	    vim_free(table);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	table[item].first = n1;
	lili = lili->li_next;
	table[item].last = lili->li_tv.vval.v_ಸಂಖ್ಯೆ;
	lili = lili->li_next;
	table[item].width = lili->li_tv.vval.v_ಸಂಖ್ಯೆ;
    }

    vim_free(ptrs);

update:
    cw_table_save = cw_table;
    cw_table_size_save = cw_table_size;
    cw_table = table;
    cw_table_size = table_size;

    // Check that the new value does not conflict with 'listchars' or
    // 'fillchars'.
    error = check_chars_options();
    ಯದಿ (error != ರಿಕ್ತ)
    {
	emsg(_(error));
	cw_table = cw_table_save;
	cw_table_size = cw_table_size_save;
	vim_free(table);
	ಪ್ರತ್ಯಾಗಮ;
    }

    vim_free(cw_table_save);
    changed_window_setting_all();
    redraw_all_later(UPD_CLEAR);
}

    ಶೂನ್ಯ
f_getcellwidths(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (size_t i = 0; i < cw_table_size; i++)
    {
	list_T *entry = list_alloc();
	ಯದಿ (entry == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (list_append_ಸಂಖ್ಯೆ(entry, (varnumber_T)cw_table[i].first) == FAIL
	   || list_append_ಸಂಖ್ಯೆ(entry, (varnumber_T)cw_table[i].last) == FAIL
	   || list_append_ಸಂಖ್ಯೆ(entry, (varnumber_T)cw_table[i].width) == FAIL
	   || list_append_list(rettv->vval.v_list, entry) == FAIL)
	{
	    list_free(entry);
	    ಭಙ್ಗ;
	}
    }
}

    ಶೂನ್ಯ
f_charclass(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (check_for_string_arg(argvars, 0) == FAIL
	    || argvars[0].vval.v_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_ಸಂಖ್ಯೆ = mb_get_class(argvars[0].vval.v_string);
}
#endif

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * encoding options.
 */
    char_u *
get_encoding_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= (ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(enc_canon_table) / ಪರಿಮಾಣ(enc_canon_table[0])))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ (char_u*)enc_canon_table[idx].name;
}
