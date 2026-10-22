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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_ARABIC)

// Unicode values for Arabic characters.
#నిర్వచించు a_HAMZA				0x0621
#నిర్వచించు a_ALEF_MADDA			0x0622
#నిర్వచించు a_ALEF_HAMZA_ABOVE		0x0623
#నిర్వచించు a_WAW_HAMZA			0x0624
#నిర్వచించు a_ALEF_HAMZA_క్రింద		0x0625
#నిర్వచించు a_YEH_HAMZA			0x0626
#నిర్వచించు a_ALEF				0x0627
#నిర్వచించు a_BEH				0x0628
#నిర్వచించు a_TEH_MARBUTA			0x0629
#నిర్వచించు a_TEH				0x062a
#నిర్వచించు a_THEH				0x062b
#నిర్వచించు a_JEEM				0x062c
#నిర్వచించు a_HAH				0x062d
#నిర్వచించు a_KHAH				0x062e
#నిర్వచించు a_DAL				0x062f
#నిర్వచించు a_THAL				0x0630
#నిర్వచించు a_REH				0x0631
#నిర్వచించు a_ZAIN				0x0632
#నిర్వచించు a_చూసిన				0x0633
#నిర్వచించు a_SHEEN				0x0634
#నిర్వచించు a_SAD				0x0635
#నిర్వచించు a_DAD				0x0636
#నిర్వచించు a_TAH				0x0637
#నిర్వచించు a_ZAH				0x0638
#నిర్వచించు a_AIN				0x0639
#నిర్వచించు a_GHAIN				0x063a
#నిర్వచించు a_TATWEEL			0x0640
#నిర్వచించు a_FEH				0x0641
#నిర్వచించు a_QAF				0x0642
#నిర్వచించు a_KAF				0x0643
#నిర్వచించు a_LAM				0x0644
#నిర్వచించు a_MEEM				0x0645
#నిర్వచించు a_NOON				0x0646
#నిర్వచించు a_HEH				0x0647
#నిర్వచించు a_WAW				0x0648
#నిర్వచించు a_ALEF_MAKSURA			0x0649
#నిర్వచించు a_YEH				0x064a
#నిర్వచించు a_FATHATAN			0x064b
#నిర్వచించు a_DAMMATAN			0x064c
#నిర్వచించు a_KASRATAN			0x064d
#నిర్వచించు a_FATHA				0x064e
#నిర్వచించు a_DAMMA				0x064f
#నిర్వచించు a_KASRA				0x0650
#నిర్వచించు a_SHADDA			0x0651
#నిర్వచించు a_SUKUN				0x0652
#నిర్వచించు a_MADDA_ABOVE			0x0653
#నిర్వచించు a_HAMZA_ABOVE			0x0654
#నిర్వచించు a_HAMZA_క్రింద			0x0655

#నిర్వచించు a_PEH				0x067e
#నిర్వచించు a_TCHEH				0x0686
#నిర్వచించు a_JEH				0x0698
#నిర్వచించు a_FKAF				0x06a9
#నిర్వచించు a_GAF				0x06af
#నిర్వచించు a_FYEH				0x06cc

#నిర్వచించు a_s_LAM_ALEF_MADDA_ABOVE	0xfef5
#నిర్వచించు a_f_LAM_ALEF_MADDA_ABOVE	0xfef6
#నిర్వచించు a_s_LAM_ALEF_HAMZA_ABOVE	0xfef7
#నిర్వచించు a_f_LAM_ALEF_HAMZA_ABOVE	0xfef8
#నిర్వచించు a_s_LAM_ALEF_HAMZA_క్రింద	0xfef9
#నిర్వచించు a_f_LAM_ALEF_HAMZA_క్రింద	0xfefa
#నిర్వచించు a_s_LAM_ALEF			0xfefb
#నిర్వచించు a_f_LAM_ALEF			0xfefc

స్థిర నిర్మాణం achar {
    గుర్తులేని c;
    గుర్తులేని isolated;
    గుర్తులేని initial;
    గుర్తులేని medial;
    గుర్తులేని final;
} achars[] = {
    {a_HAMZA, 0xfe80, 0, 0, 0},
    {a_ALEF_MADDA, 0xfe81, 0, 0, 0xfe82},
    {a_ALEF_HAMZA_ABOVE, 0xfe83, 0, 0, 0xfe84},
    {a_WAW_HAMZA, 0xfe85, 0, 0, 0xfe86},
    {a_ALEF_HAMZA_క్రింద, 0xfe87, 0, 0, 0xfe88},
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
    {a_చూసిన, 0xfeb1, 0xfeb3, 0xfeb4, 0xfeb2},
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
    {a_HAMZA_క్రింద, 0, 0, 0, 0},
    {a_PEH, 0xfb56, 0xfb58, 0xfb59, 0xfb57},
    {a_TCHEH, 0xfb7a, 0xfb7c, 0xfb7d, 0xfb7b},
    {a_JEH, 0xfb8a, 0, 0, 0xfb8b},
    {a_FKAF, 0xfb8e, 0xfb90, 0xfb91, 0xfb8f},
    {a_GAF, 0xfb92, 0xfb94, 0xfb95, 0xfb93},
    {a_FYEH, 0xfbfc, 0xfbfe, 0xfbff, 0xfbfd},
};

#నిర్వచించు a_BYTE_క్రమం_MARK		0xfeff

/*
 * Find the struct achar pointer to the given Arabic char.
 * Returns NULL if not found.
 */
    స్థిర నిర్మాణం achar *
కనుగొను_achar(పూర్ణాంకం c)
{
    పూర్ణాంకం h, m, l;

    // using binary search to find c
    h = ARRAY_నిడివి(achars);
    l = 0;
    అయితే (l < h)
    {
	m = (h + l) / 2;
	ఒకవేళ (achars[m].c == (గుర్తులేని)c)
	    తిరిగి &achars[m];
	ఒకవేళ ((గుర్తులేని)c < achars[m].c)
	    h = m;
	లేకపోతే
	    l = m + 1;
    }
    తిరిగి ఖాళీ;
}

/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    స్థిర పూర్ణాంకం
chg_c_laa2i(పూర్ణాంకం hid_c)
{
    పూర్ణాంకం tempc;

    ఎంపిక (hid_c)
    {
	స్థితి a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    ఆపు;
	స్థితి a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    ఆపు;
	స్థితి a_ALEF_HAMZA_క్రింద:
	    tempc = a_s_LAM_ALEF_HAMZA_క్రింద;
	    ఆపు;
	స్థితి a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    ఆపు;
	అప్రమేయ:
	    tempc = 0;
    }

    తిరిగి tempc;
}

/*
 * Change shape - from Combination-Isolated to Final
 */
    స్థిర పూర్ణాంకం
chg_c_laa2f(పూర్ణాంకం hid_c)
{
    పూర్ణాంకం tempc;

    ఎంపిక (hid_c)
    {
	స్థితి a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    ఆపు;
	స్థితి a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    ఆపు;
	స్థితి a_ALEF_HAMZA_క్రింద:
	    tempc = a_f_LAM_ALEF_HAMZA_క్రింద;
	    ఆపు;
	స్థితి a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    ఆపు;
	అప్రమేయ:
	    tempc = 0;
    }

    తిరిగి tempc;
}

/*
 * Returns whether it is possible to join the given letters
 */
    స్థిర పూర్ణాంకం
can_join(పూర్ణాంకం c1, పూర్ణాంకం c2)
{
    నిర్మాణం achar *a1 = కనుగొను_achar(c1);
    నిర్మాణం achar *a2 = కనుగొను_achar(c2);

    తిరిగి a1 && a2 && (a1->initial || a1->medial) && (a2->final || a2->medial);
}

/*
 * Check whether we are dealing with a character that could be regarded as an
 * Arabic combining character, need to check the character before this.
 */
    పూర్ణాంకం
arabic_maycombine(పూర్ణాంకం two)
{
    ఒకవేళ (p_arshape && !p_tbidi)
	తిరిగి (two == a_ALEF_MADDA
		    || two == a_ALEF_HAMZA_ABOVE
		    || two == a_ALEF_HAMZA_క్రింద
		    || two == a_ALEF);
    తిరిగి FALSE;
}

/*
 * Check whether we are dealing with Arabic combining characters.
 * Note: these are NOT really composing characters!
 */
    పూర్ణాంకం
arabic_combine(
    పూర్ణాంకం		one,	    // first character
    పూర్ణాంకం		two)	    // character just after "one"
{
    ఒకవేళ (one == a_LAM)
	తిరిగి arabic_maycombine(two);
    తిరిగి FALSE;
}

/*
 * A_is_iso returns true if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    స్థిర పూర్ణాంకం
A_is_iso(పూర్ణాంకం c)
{
    తిరిగి కనుగొను_achar(c) != ఖాళీ;
}

/*
 * A_is_ok returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    స్థిర పూర్ణాంకం
A_is_ok(పూర్ణాంకం c)
{
    తిరిగి (A_is_iso(c) || c == a_BYTE_క్రమం_MARK);
}

/*
 * A_is_valid returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    స్థిర పూర్ణాంకం
A_is_valid(పూర్ణాంకం c)
{
    తిరిగి (A_is_ok(c) && c != a_HAMZA);
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
    పూర్ణాంకం
arabic_shape(
    పూర్ణాంకం		c,
    పూర్ణాంకం		*ccp,
    పూర్ణాంకం		*c1p,
    పూర్ణాంకం		prev_c,
    పూర్ణాంకం		prev_c1,
    పూర్ణాంకం		తదుపరి_c)
{
    పూర్ణాంకం		curr_c;
    పూర్ణాంకం		curr_laa;
    పూర్ణాంకం		prev_laa;

    // Deal only with Arabic characters, pass back all others
    ఒకవేళ (!A_is_ok(c))
	తిరిగి c;

    curr_laa = arabic_combine(c, *c1p);
    prev_laa = arabic_combine(prev_c, prev_c1);

    ఒకవేళ (curr_laa)
    {
	ఒకవేళ (A_is_valid(prev_c) && can_join(prev_c, a_LAM) && !prev_laa)
	    curr_c = chg_c_laa2f(*c1p);
	లేకపోతే
	    curr_c = chg_c_laa2i(*c1p);

	// Remove the composing character
	*c1p = 0;
    }
    లేకపోతే
    {
	నిర్మాణం achar *curr_a = కనుగొను_achar(c);
	పూర్ణాంకం backward_combine = !prev_laa && can_join(prev_c, c);
	పూర్ణాంకం forward_combine = can_join(c, తదుపరి_c);

	ఒకవేళ (backward_combine)
	{
	    ఒకవేళ (forward_combine)
		curr_c = curr_a->medial;
	    లేకపోతే
		curr_c = curr_a->final;
	}
	లేకపోతే
	{
	    ఒకవేళ (forward_combine)
		curr_c = curr_a->initial;
	    లేకపోతే
		curr_c = curr_a->isolated;
	}
    }

    // Character missing from the table means using original character.
    ఒకవేళ (curr_c == NUL)
	curr_c = c;

    ఒకవేళ (curr_c != c && ccp != ఖాళీ)
    {
	char_u buf[MB_MAXBYTES + 1];

	// Update the first byte of the character.
	(*mb_char2bytes)(curr_c, buf);
	*ccp = buf[0];
    }

    // Return the shaped character
    తిరిగి curr_c;
}
#అయితే_ముగింపు // FEAT_ARABIC
