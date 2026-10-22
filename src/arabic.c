/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * arabic.c: functions for Arabic language
 *
 * Author: Nadim Shaikli & Isam Bayazidi
 * Farsi support and restructuring to make adding new letters easier by Ali
 * Gholami Rudi.  Further work by Ameretat Reith.
 */

/*
 * Sorted list of unicode Arabic characters.  Each entry holds the
 * presentation forms of a letter.
 *
 * Arabic characters are categorized into following types:
 *
 * Isolated	- iso-8859-6 form
 * Initial	- unicode form-B start
 * Medial	- unicode form-B middle
 * Final	- unicode form-B final
 * Stand-Alone	- unicode form-B isolated
 */

#include "vim.h"

#만약 defined(FEAT_ARABIC)

// Unicode values for Arabic characters.
#define a_HAMZA				0x0621
#define a_ALEF_MADDA			0x0622
#define a_ALEF_HAMZA_ABOVE		0x0623
#define a_WAW_HAMZA			0x0624
#define a_ALEF_HAMZA_BELOW		0x0625
#define a_YEH_HAMZA			0x0626
#define a_ALEF				0x0627
#define a_BEH				0x0628
#define a_TEH_MARBUTA			0x0629
#define a_TEH				0x062a
#define a_THEH				0x062b
#define a_JEEM				0x062c
#define a_HAH				0x062d
#define a_KHAH				0x062e
#define a_DAL				0x062f
#define a_THAL				0x0630
#define a_REH				0x0631
#define a_ZAIN				0x0632
#define a_SEEN				0x0633
#define a_SHEEN				0x0634
#define a_SAD				0x0635
#define a_DAD				0x0636
#define a_TAH				0x0637
#define a_ZAH				0x0638
#define a_AIN				0x0639
#define a_GHAIN				0x063a
#define a_TATWEEL			0x0640
#define a_FEH				0x0641
#define a_QAF				0x0642
#define a_KAF				0x0643
#define a_LAM				0x0644
#define a_MEEM				0x0645
#define a_NOON				0x0646
#define a_HEH				0x0647
#define a_WAW				0x0648
#define a_ALEF_MAKSURA			0x0649
#define a_YEH				0x064a
#define a_FATHATAN			0x064b
#define a_DAMMATAN			0x064c
#define a_KASRATAN			0x064d
#define a_FATHA				0x064e
#define a_DAMMA				0x064f
#define a_KASRA				0x0650
#define a_SHADDA			0x0651
#define a_SUKUN				0x0652
#define a_MADDA_ABOVE			0x0653
#define a_HAMZA_ABOVE			0x0654
#define a_HAMZA_BELOW			0x0655

#define a_PEH				0x067e
#define a_TCHEH				0x0686
#define a_JEH				0x0698
#define a_FKAF				0x06a9
#define a_GAF				0x06af
#define a_FYEH				0x06cc

#define a_s_LAM_ALEF_MADDA_ABOVE	0xfef5
#define a_f_LAM_ALEF_MADDA_ABOVE	0xfef6
#define a_s_LAM_ALEF_HAMZA_ABOVE	0xfef7
#define a_f_LAM_ALEF_HAMZA_ABOVE	0xfef8
#define a_s_LAM_ALEF_HAMZA_BELOW	0xfef9
#define a_f_LAM_ALEF_HAMZA_BELOW	0xfefa
#define a_s_LAM_ALEF			0xfefb
#define a_f_LAM_ALEF			0xfefc

static struct achar {
    unsigned c;
    unsigned isolated;
    unsigned initial;
    unsigned medial;
    unsigned final;
} achars[] = {
    {a_HAMZA, 0xfe80, 0, 0, 0},
    {a_ALEF_MADDA, 0xfe81, 0, 0, 0xfe82},
    {a_ALEF_HAMZA_ABOVE, 0xfe83, 0, 0, 0xfe84},
    {a_WAW_HAMZA, 0xfe85, 0, 0, 0xfe86},
    {a_ALEF_HAMZA_BELOW, 0xfe87, 0, 0, 0xfe88},
    {a_YEH_HAMZA, 0xfe89, 0xfe8b, 0xfe8c, 0xfe8a},
    {a_ALEF, 0xfe8d, 0, 0, 0xfe8e},
    {a_BEH, 0xfe8f, 0xfe91, 0xfe92, 0xfe90},
    {a_TEH_MARBUTA, 0xfe93, 0, 0, 0xfe94},
    {a_TEH, 0xfe95, 0xfe97, 0xfe98, 0xfe96},
    {a_THEH, 0xfe99, 0xfe9b, 0xfe9c, 0xfe9a},
    {a_JEEM, 0xfe9d, 0xfe9f, 0xfea0, 0xfe9e},
    {a_HAH, 0xfea1, 0xfea3, 0xfea4, 0xfea2},
    {a_KHAH, 0xfea5, 0xfea7, 0xfea8, 0xfea6},
    {a_DAL, 0xfea9, 0, 0, 0xfeaa},
    {a_THAL, 0xfeab, 0, 0, 0xfeac},
    {a_REH, 0xfead, 0, 0, 0xfeae},
    {a_ZAIN, 0xfeaf, 0, 0, 0xfeb0},
    {a_SEEN, 0xfeb1, 0xfeb3, 0xfeb4, 0xfeb2},
    {a_SHEEN, 0xfeb5, 0xfeb7, 0xfeb8, 0xfeb6},
    {a_SAD, 0xfeb9, 0xfebb, 0xfebc, 0xfeba},
    {a_DAD, 0xfebd, 0xfebf, 0xfec0, 0xfebe},
    {a_TAH, 0xfec1, 0xfec3, 0xfec4, 0xfec2},
    {a_ZAH, 0xfec5, 0xfec7, 0xfec8, 0xfec6},
    {a_AIN, 0xfec9, 0xfecb, 0xfecc, 0xfeca},
    {a_GHAIN, 0xfecd, 0xfecf, 0xfed0, 0xfece},
    {a_TATWEEL, 0, 0x0640, 0x0640, 0x0640},
    {a_FEH, 0xfed1, 0xfed3, 0xfed4, 0xfed2},
    {a_QAF, 0xfed5, 0xfed7, 0xfed8, 0xfed6},
    {a_KAF, 0xfed9, 0xfedb, 0xfedc, 0xfeda},
    {a_LAM, 0xfedd, 0xfedf, 0xfee0, 0xfede},
    {a_MEEM, 0xfee1, 0xfee3, 0xfee4, 0xfee2},
    {a_NOON, 0xfee5, 0xfee7, 0xfee8, 0xfee6},
    {a_HEH, 0xfee9, 0xfeeb, 0xfeec, 0xfeea},
    {a_WAW, 0xfeed, 0, 0, 0xfeee},
    {a_ALEF_MAKSURA, 0xfeef, 0, 0, 0xfef0},
    {a_YEH, 0xfef1, 0xfef3, 0xfef4, 0xfef2},
    {a_FATHATAN, 0xfe70, 0, 0, 0},
    {a_DAMMATAN, 0xfe72, 0, 0, 0},
    {a_KASRATAN, 0xfe74, 0, 0, 0},
    {a_FATHA, 0xfe76, 0, 0xfe77, 0},
    {a_DAMMA, 0xfe78, 0, 0xfe79, 0},
    {a_KASRA, 0xfe7a, 0, 0xfe7b, 0},
    {a_SHADDA, 0xfe7c, 0, 0xfe7c, 0},
    {a_SUKUN, 0xfe7e, 0, 0xfe7f, 0},
    {a_MADDA_ABOVE, 0, 0, 0, 0},
    {a_HAMZA_ABOVE, 0, 0, 0, 0},
    {a_HAMZA_BELOW, 0, 0, 0, 0},
    {a_PEH, 0xfb56, 0xfb58, 0xfb59, 0xfb57},
    {a_TCHEH, 0xfb7a, 0xfb7c, 0xfb7d, 0xfb7b},
    {a_JEH, 0xfb8a, 0, 0, 0xfb8b},
    {a_FKAF, 0xfb8e, 0xfb90, 0xfb91, 0xfb8f},
    {a_GAF, 0xfb92, 0xfb94, 0xfb95, 0xfb93},
    {a_FYEH, 0xfbfc, 0xfbfe, 0xfbff, 0xfbfd},
};

#define a_BYTE_ORDER_MARK		0xfeff

/*
 * Find the struct achar pointer to the given Arabic char.
 * Returns NULL if not found.
 */
    static struct achar *
find_achar(정수 c)
{
    정수 h, m, l;

    // using binary search to find c
    h = ARRAY_LENGTH(achars);
    l = 0;
    동안 (l < h)
    {
	m = (h + l) / 2;
	만약 (achars[m].c == (unsigned)c)
	    반환 &achars[m];
	만약 ((unsigned)c < achars[m].c)
	    h = m;
	아니면
	    l = m + 1;
    }
    반환 NULL;
}

/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    static 정수
chg_c_laa2i(정수 hid_c)
{
    정수 tempc;

    switch (hid_c)
    {
	case a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_s_LAM_ALEF_HAMZA_BELOW;
	    break;
	case a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    break;
	default:
	    tempc = 0;
    }

    반환 tempc;
}

/*
 * Change shape - from Combination-Isolated to Final
 */
    static 정수
chg_c_laa2f(정수 hid_c)
{
    정수 tempc;

    switch (hid_c)
    {
	case a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    break;
	case a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    break;
	case a_ALEF_HAMZA_BELOW:
	    tempc = a_f_LAM_ALEF_HAMZA_BELOW;
	    break;
	case a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    break;
	default:
	    tempc = 0;
    }

    반환 tempc;
}

/*
 * Returns whether it is possible to join the given letters
 */
    static 정수
can_join(정수 c1, 정수 c2)
{
    struct achar *a1 = find_achar(c1);
    struct achar *a2 = find_achar(c2);

    반환 a1 && a2 && (a1->initial || a1->medial) && (a2->final || a2->medial);
}

/*
 * Check whether we are dealing with a character that could be regarded as an
 * Arabic combining character, need to check the character before this.
 */
    정수
arabic_maycombine(정수 two)
{
    만약 (p_arshape && !p_tbidi)
	반환 (two == a_ALEF_MADDA
		    || two == a_ALEF_HAMZA_ABOVE
		    || two == a_ALEF_HAMZA_BELOW
		    || two == a_ALEF);
    반환 FALSE;
}

/*
 * Check whether we are dealing with Arabic combining characters.
 * Note: these are NOT really composing characters!
 */
    정수
arabic_combine(
    정수		one,	    // first character
    정수		two)	    // character just after "one"
{
    만약 (one == a_LAM)
	반환 arabic_maycombine(two);
    반환 FALSE;
}

/*
 * A_is_iso returns true if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    static 정수
A_is_iso(정수 c)
{
    반환 find_achar(c) != NULL;
}

/*
 * A_is_ok returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    static 정수
A_is_ok(정수 c)
{
    반환 (A_is_iso(c) || c == a_BYTE_ORDER_MARK);
}

/*
 * A_is_valid returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    static 정수
A_is_valid(정수 c)
{
    반환 (A_is_ok(c) && c != a_HAMZA);
}

/*
 * Do Arabic shaping on character "c".  Returns the shaped character.
 * out:    "ccp" points to the first byte of the character to be shaped.
 * in/out: "c1p" points to the first composing char for "c".
 * in:     "prev_c"  is the previous character (not shaped)
 * in:     "prev_c1" is the first composing char for the previous char
 *		     (not shaped)
 * in:     "next_c"  is the next character (not shaped).
 */
    정수
arabic_shape(
    정수		c,
    정수		*ccp,
    정수		*c1p,
    정수		prev_c,
    정수		prev_c1,
    정수		next_c)
{
    정수		curr_c;
    정수		curr_laa;
    정수		prev_laa;

    // Deal only with Arabic characters, pass back all others
    만약 (!A_is_ok(c))
	반환 c;

    curr_laa = arabic_combine(c, *c1p);
    prev_laa = arabic_combine(prev_c, prev_c1);

    만약 (curr_laa)
    {
	만약 (A_is_valid(prev_c) && can_join(prev_c, a_LAM) && !prev_laa)
	    curr_c = chg_c_laa2f(*c1p);
	아니면
	    curr_c = chg_c_laa2i(*c1p);

	// Remove the composing character
	*c1p = 0;
    }
    아니면
    {
	struct achar *curr_a = find_achar(c);
	정수 backward_combine = !prev_laa && can_join(prev_c, c);
	정수 forward_combine = can_join(c, next_c);

	만약 (backward_combine)
	{
	    만약 (forward_combine)
		curr_c = curr_a->medial;
	    아니면
		curr_c = curr_a->final;
	}
	아니면
	{
	    만약 (forward_combine)
		curr_c = curr_a->initial;
	    아니면
		curr_c = curr_a->isolated;
	}
    }

    // Character missing from the table means using original character.
    만약 (curr_c == NUL)
	curr_c = c;

    만약 (curr_c != c && ccp != NULL)
    {
	char_u buf[MB_MAXBYTES + 1];

	// Update the first byte of the character.
	(*mb_char2bytes)(curr_c, buf);
	*ccp = buf[0];
    }

    // Return the shaped character
    반환 curr_c;
}
#endif // FEAT_ARABIC
