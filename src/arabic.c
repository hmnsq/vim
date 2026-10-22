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

#ضمن "vim.h"

#إذا defined(FEAT_ARABIC)

// Unicode values for Arabic characters.
#وصف_ڪريو a_HAMZA				0x0621
#وصف_ڪريو a_ALEF_MADDA			0x0622
#وصف_ڪريو a_ALEF_HAMZA_ABOVE		0x0623
#وصف_ڪريو a_WAW_HAMZA			0x0624
#وصف_ڪريو a_ALEF_HAMZA_هيٺ		0x0625
#وصف_ڪريو a_YEH_HAMZA			0x0626
#وصف_ڪريو a_ALEF				0x0627
#وصف_ڪريو a_BEH				0x0628
#وصف_ڪريو a_TEH_MARBUTA			0x0629
#وصف_ڪريو a_TEH				0x062a
#وصف_ڪريو a_THEH				0x062b
#وصف_ڪريو a_JEEM				0x062c
#وصف_ڪريو a_HAH				0x062d
#وصف_ڪريو a_KHAH				0x062e
#وصف_ڪريو a_DAL				0x062f
#وصف_ڪريو a_THAL				0x0630
#وصف_ڪريو a_REH				0x0631
#وصف_ڪريو a_ZAIN				0x0632
#وصف_ڪريو a_ڏٺل				0x0633
#وصف_ڪريو a_SHEEN				0x0634
#وصف_ڪريو a_SAD				0x0635
#وصف_ڪريو a_DAD				0x0636
#وصف_ڪريو a_TAH				0x0637
#وصف_ڪريو a_ZAH				0x0638
#وصف_ڪريو a_AIN				0x0639
#وصف_ڪريو a_GHAIN				0x063a
#وصف_ڪريو a_TATWEEL			0x0640
#وصف_ڪريو a_FEH				0x0641
#وصف_ڪريو a_QAF				0x0642
#وصف_ڪريو a_KAF				0x0643
#وصف_ڪريو a_LAM				0x0644
#وصف_ڪريو a_MEEM				0x0645
#وصف_ڪريو a_NOON				0x0646
#وصف_ڪريو a_HEH				0x0647
#وصف_ڪريو a_WAW				0x0648
#وصف_ڪريو a_ALEF_MAKSURA			0x0649
#وصف_ڪريو a_YEH				0x064a
#وصف_ڪريو a_FATHATAN			0x064b
#وصف_ڪريو a_DAMMATAN			0x064c
#وصف_ڪريو a_KASRATAN			0x064d
#وصف_ڪريو a_FATHA				0x064e
#وصف_ڪريو a_DAMMA				0x064f
#وصف_ڪريو a_KASRA				0x0650
#وصف_ڪريو a_SHADDA			0x0651
#وصف_ڪريو a_SUKUN				0x0652
#وصف_ڪريو a_MADDA_ABOVE			0x0653
#وصف_ڪريو a_HAMZA_ABOVE			0x0654
#وصف_ڪريو a_HAMZA_هيٺ			0x0655

#وصف_ڪريو a_PEH				0x067e
#وصف_ڪريو a_TCHEH				0x0686
#وصف_ڪريو a_JEH				0x0698
#وصف_ڪريو a_FKAF				0x06a9
#وصف_ڪريو a_GAF				0x06af
#وصف_ڪريو a_FYEH				0x06cc

#وصف_ڪريو a_s_LAM_ALEF_MADDA_ABOVE	0xfef5
#وصف_ڪريو a_f_LAM_ALEF_MADDA_ABOVE	0xfef6
#وصف_ڪريو a_s_LAM_ALEF_HAMZA_ABOVE	0xfef7
#وصف_ڪريو a_f_LAM_ALEF_HAMZA_ABOVE	0xfef8
#وصف_ڪريو a_s_LAM_ALEF_HAMZA_هيٺ	0xfef9
#وصف_ڪريو a_f_LAM_ALEF_HAMZA_هيٺ	0xfefa
#وصف_ڪريو a_s_LAM_ALEF			0xfefb
#وصف_ڪريو a_f_LAM_ALEF			0xfefc

ساكن بنية achar {
    بنانشان c;
    بنانشان isolated;
    بنانشان initial;
    بنانشان medial;
    بنانشان final;
} achars[] = {
    {a_HAMZA, 0xfe80, 0, 0, 0},
    {a_ALEF_MADDA, 0xfe81, 0, 0, 0xfe82},
    {a_ALEF_HAMZA_ABOVE, 0xfe83, 0, 0, 0xfe84},
    {a_WAW_HAMZA, 0xfe85, 0, 0, 0xfe86},
    {a_ALEF_HAMZA_هيٺ, 0xfe87, 0, 0, 0xfe88},
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
    {a_ڏٺل, 0xfeb1, 0xfeb3, 0xfeb4, 0xfeb2},
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
    {a_HAMZA_هيٺ, 0, 0, 0, 0},
    {a_PEH, 0xfb56, 0xfb58, 0xfb59, 0xfb57},
    {a_TCHEH, 0xfb7a, 0xfb7c, 0xfb7d, 0xfb7b},
    {a_JEH, 0xfb8a, 0, 0, 0xfb8b},
    {a_FKAF, 0xfb8e, 0xfb90, 0xfb91, 0xfb8f},
    {a_GAF, 0xfb92, 0xfb94, 0xfb95, 0xfb93},
    {a_FYEH, 0xfbfc, 0xfbfe, 0xfbff, 0xfbfd},
};

#وصف_ڪريو a_BYTE_ترتيب_MARK		0xfeff

/*
 * Find the struct achar pointer to the given Arabic char.
 * Returns NULL if not found.
 */
    ساكن بنية achar *
ڳوليو_achar(صحيح c)
{
    صحيح h, m, l;

    // using binary search to find c
    h = ARRAY_ڊيگهائي(achars);
    l = 0;
    طالما (l < h)
    {
	m = (h + l) / 2;
	إذا (achars[m].c == (بنانشان)c)
	    أرجع &achars[m];
	إذا ((بنانشان)c < achars[m].c)
	    h = m;
	وإلا
	    l = m + 1;
    }
    أرجع عدم;
}

/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    ساكن صحيح
chg_c_laa2i(صحيح hid_c)
{
    صحيح tempc;

    بدل (hid_c)
    {
	حالة a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    اكسر;
	حالة a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    اكسر;
	حالة a_ALEF_HAMZA_هيٺ:
	    tempc = a_s_LAM_ALEF_HAMZA_هيٺ;
	    اكسر;
	حالة a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    اكسر;
	افتراضي:
	    tempc = 0;
    }

    أرجع tempc;
}

/*
 * Change shape - from Combination-Isolated to Final
 */
    ساكن صحيح
chg_c_laa2f(صحيح hid_c)
{
    صحيح tempc;

    بدل (hid_c)
    {
	حالة a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    اكسر;
	حالة a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    اكسر;
	حالة a_ALEF_HAMZA_هيٺ:
	    tempc = a_f_LAM_ALEF_HAMZA_هيٺ;
	    اكسر;
	حالة a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    اكسر;
	افتراضي:
	    tempc = 0;
    }

    أرجع tempc;
}

/*
 * Returns whether it is possible to join the given letters
 */
    ساكن صحيح
can_join(صحيح c1, صحيح c2)
{
    بنية achar *a1 = ڳوليو_achar(c1);
    بنية achar *a2 = ڳوليو_achar(c2);

    أرجع a1 && a2 && (a1->initial || a1->medial) && (a2->final || a2->medial);
}

/*
 * Check whether we are dealing with a character that could be regarded as an
 * Arabic combining character, need to check the character before this.
 */
    صحيح
arabic_maycombine(صحيح two)
{
    إذا (p_arshape && !p_tbidi)
	أرجع (two == a_ALEF_MADDA
		    || two == a_ALEF_HAMZA_ABOVE
		    || two == a_ALEF_HAMZA_هيٺ
		    || two == a_ALEF);
    أرجع FALSE;
}

/*
 * Check whether we are dealing with Arabic combining characters.
 * Note: these are NOT really composing characters!
 */
    صحيح
arabic_combine(
    صحيح		one,	    // first character
    صحيح		two)	    // character just after "one"
{
    إذا (one == a_LAM)
	أرجع arabic_maycombine(two);
    أرجع FALSE;
}

/*
 * A_is_iso returns true if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    ساكن صحيح
A_is_iso(صحيح c)
{
    أرجع ڳوليو_achar(c) != عدم;
}

/*
 * A_is_ok returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    ساكن صحيح
A_is_ok(صحيح c)
{
    أرجع (A_is_iso(c) || c == a_BYTE_ترتيب_MARK);
}

/*
 * A_is_valid returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    ساكن صحيح
A_is_valid(صحيح c)
{
    أرجع (A_is_ok(c) && c != a_HAMZA);
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
    صحيح
arabic_shape(
    صحيح		c,
    صحيح		*ccp,
    صحيح		*c1p,
    صحيح		prev_c,
    صحيح		prev_c1,
    صحيح		ايندڙ_c)
{
    صحيح		curr_c;
    صحيح		curr_laa;
    صحيح		prev_laa;

    // Deal only with Arabic characters, pass back all others
    إذا (!A_is_ok(c))
	أرجع c;

    curr_laa = arabic_combine(c, *c1p);
    prev_laa = arabic_combine(prev_c, prev_c1);

    إذا (curr_laa)
    {
	إذا (A_is_valid(prev_c) && can_join(prev_c, a_LAM) && !prev_laa)
	    curr_c = chg_c_laa2f(*c1p);
	وإلا
	    curr_c = chg_c_laa2i(*c1p);

	// Remove the composing character
	*c1p = 0;
    }
    وإلا
    {
	بنية achar *curr_a = ڳوليو_achar(c);
	صحيح backward_combine = !prev_laa && can_join(prev_c, c);
	صحيح forward_combine = can_join(c, ايندڙ_c);

	إذا (backward_combine)
	{
	    إذا (forward_combine)
		curr_c = curr_a->medial;
	    وإلا
		curr_c = curr_a->final;
	}
	وإلا
	{
	    إذا (forward_combine)
		curr_c = curr_a->initial;
	    وإلا
		curr_c = curr_a->isolated;
	}
    }

    // Character missing from the table means using original character.
    إذا (curr_c == NUL)
	curr_c = c;

    إذا (curr_c != c && ccp != عدم)
    {
	char_u buf[MB_MAXBYTES + 1];

	// Update the first byte of the character.
	(*mb_char2bytes)(curr_c, buf);
	*ccp = buf[0];
    }

    // Return the shaped character
    أرجع curr_c;
}
#جيڪڏهن_پورو // FEAT_ARABIC
