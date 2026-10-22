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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_ARABIC)

// Unicode values for Arabic characters.
#ವ್ಯಾಖ್ಯಾನಿಸು a_HAMZA				0x0621
#ವ್ಯಾಖ್ಯಾನಿಸು a_ALEF_MADDA			0x0622
#ವ್ಯಾಖ್ಯಾನಿಸು a_ALEF_HAMZA_ABOVE		0x0623
#ವ್ಯಾಖ್ಯಾನಿಸು a_WAW_HAMZA			0x0624
#ವ್ಯಾಖ್ಯಾನಿಸು a_ALEF_HAMZA_ಕೆಳಗೆ		0x0625
#ವ್ಯಾಖ್ಯಾನಿಸು a_YEH_HAMZA			0x0626
#ವ್ಯಾಖ್ಯಾನಿಸು a_ALEF				0x0627
#ವ್ಯಾಖ್ಯಾನಿಸು a_BEH				0x0628
#ವ್ಯಾಖ್ಯಾನಿಸು a_TEH_MARBUTA			0x0629
#ವ್ಯಾಖ್ಯಾನಿಸು a_TEH				0x062a
#ವ್ಯಾಖ್ಯಾನಿಸು a_THEH				0x062b
#ವ್ಯಾಖ್ಯಾನಿಸು a_JEEM				0x062c
#ವ್ಯಾಖ್ಯಾನಿಸು a_HAH				0x062d
#ವ್ಯಾಖ್ಯಾನಿಸು a_KHAH				0x062e
#ವ್ಯಾಖ್ಯಾನಿಸು a_DAL				0x062f
#ವ್ಯಾಖ್ಯಾನಿಸು a_THAL				0x0630
#ವ್ಯಾಖ್ಯಾನಿಸು a_REH				0x0631
#ವ್ಯಾಖ್ಯಾನಿಸು a_ZAIN				0x0632
#ವ್ಯಾಖ್ಯಾನಿಸು a_ನೋಡಿದ				0x0633
#ವ್ಯಾಖ್ಯಾನಿಸು a_SHEEN				0x0634
#ವ್ಯಾಖ್ಯಾನಿಸು a_SAD				0x0635
#ವ್ಯಾಖ್ಯಾನಿಸು a_DAD				0x0636
#ವ್ಯಾಖ್ಯಾನಿಸು a_TAH				0x0637
#ವ್ಯಾಖ್ಯಾನಿಸು a_ZAH				0x0638
#ವ್ಯಾಖ್ಯಾನಿಸು a_AIN				0x0639
#ವ್ಯಾಖ್ಯಾನಿಸು a_GHAIN				0x063a
#ವ್ಯಾಖ್ಯಾನಿಸು a_TATWEEL			0x0640
#ವ್ಯಾಖ್ಯಾನಿಸು a_FEH				0x0641
#ವ್ಯಾಖ್ಯಾನಿಸು a_QAF				0x0642
#ವ್ಯಾಖ್ಯಾನಿಸು a_KAF				0x0643
#ವ್ಯಾಖ್ಯಾನಿಸು a_LAM				0x0644
#ವ್ಯಾಖ್ಯಾನಿಸು a_MEEM				0x0645
#ವ್ಯಾಖ್ಯಾನಿಸು a_NOON				0x0646
#ವ್ಯಾಖ್ಯಾನಿಸು a_HEH				0x0647
#ವ್ಯಾಖ್ಯಾನಿಸು a_WAW				0x0648
#ವ್ಯಾಖ್ಯಾನಿಸು a_ALEF_MAKSURA			0x0649
#ವ್ಯಾಖ್ಯಾನಿಸು a_YEH				0x064a
#ವ್ಯಾಖ್ಯಾನಿಸು a_FATHATAN			0x064b
#ವ್ಯಾಖ್ಯಾನಿಸು a_DAMMATAN			0x064c
#ವ್ಯಾಖ್ಯಾನಿಸು a_KASRATAN			0x064d
#ವ್ಯಾಖ್ಯಾನಿಸು a_FATHA				0x064e
#ವ್ಯಾಖ್ಯಾನಿಸು a_DAMMA				0x064f
#ವ್ಯಾಖ್ಯಾನಿಸು a_KASRA				0x0650
#ವ್ಯಾಖ್ಯಾನಿಸು a_SHADDA			0x0651
#ವ್ಯಾಖ್ಯಾನಿಸು a_SUKUN				0x0652
#ವ್ಯಾಖ್ಯಾನಿಸು a_MADDA_ABOVE			0x0653
#ವ್ಯಾಖ್ಯಾನಿಸು a_HAMZA_ABOVE			0x0654
#ವ್ಯಾಖ್ಯಾನಿಸು a_HAMZA_ಕೆಳಗೆ			0x0655

#ವ್ಯಾಖ್ಯಾನಿಸು a_PEH				0x067e
#ವ್ಯಾಖ್ಯಾನಿಸು a_TCHEH				0x0686
#ವ್ಯಾಖ್ಯಾನಿಸು a_JEH				0x0698
#ವ್ಯಾಖ್ಯಾನಿಸು a_FKAF				0x06a9
#ವ್ಯಾಖ್ಯಾನಿಸು a_GAF				0x06af
#ವ್ಯಾಖ್ಯಾನಿಸು a_FYEH				0x06cc

#ವ್ಯಾಖ್ಯಾನಿಸು a_s_LAM_ALEF_MADDA_ABOVE	0xfef5
#ವ್ಯಾಖ್ಯಾನಿಸು a_f_LAM_ALEF_MADDA_ABOVE	0xfef6
#ವ್ಯಾಖ್ಯಾನಿಸು a_s_LAM_ALEF_HAMZA_ABOVE	0xfef7
#ವ್ಯಾಖ್ಯಾನಿಸು a_f_LAM_ALEF_HAMZA_ABOVE	0xfef8
#ವ್ಯಾಖ್ಯಾನಿಸು a_s_LAM_ALEF_HAMZA_ಕೆಳಗೆ	0xfef9
#ವ್ಯಾಖ್ಯಾನಿಸು a_f_LAM_ALEF_HAMZA_ಕೆಳಗೆ	0xfefa
#ವ್ಯಾಖ್ಯಾನಿಸು a_s_LAM_ALEF			0xfefb
#ವ್ಯಾಖ್ಯಾನಿಸು a_f_LAM_ALEF			0xfefc

ಸ್ಥಿರ ರಚನೆ achar {
    ಚಿಹ್ನೆರಹಿತ c;
    ಚಿಹ್ನೆರಹಿತ isolated;
    ಚಿಹ್ನೆರಹಿತ initial;
    ಚಿಹ್ನೆರಹಿತ medial;
    ಚಿಹ್ನೆರಹಿತ final;
} achars[] = {
    {a_HAMZA, 0xfe80, 0, 0, 0},
    {a_ALEF_MADDA, 0xfe81, 0, 0, 0xfe82},
    {a_ALEF_HAMZA_ABOVE, 0xfe83, 0, 0, 0xfe84},
    {a_WAW_HAMZA, 0xfe85, 0, 0, 0xfe86},
    {a_ALEF_HAMZA_ಕೆಳಗೆ, 0xfe87, 0, 0, 0xfe88},
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
    {a_ನೋಡಿದ, 0xfeb1, 0xfeb3, 0xfeb4, 0xfeb2},
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
    {a_HAMZA_ಕೆಳಗೆ, 0, 0, 0, 0},
    {a_PEH, 0xfb56, 0xfb58, 0xfb59, 0xfb57},
    {a_TCHEH, 0xfb7a, 0xfb7c, 0xfb7d, 0xfb7b},
    {a_JEH, 0xfb8a, 0, 0, 0xfb8b},
    {a_FKAF, 0xfb8e, 0xfb90, 0xfb91, 0xfb8f},
    {a_GAF, 0xfb92, 0xfb94, 0xfb95, 0xfb93},
    {a_FYEH, 0xfbfc, 0xfbfe, 0xfbff, 0xfbfd},
};

#ವ್ಯಾಖ್ಯಾನಿಸು a_BYTE_ಕ್ರಮ_MARK		0xfeff

/*
 * Find the struct achar pointer to the given Arabic char.
 * Returns NULL if not found.
 */
    ಸ್ಥಿರ ರಚನೆ achar *
ಕಂಡುಹಿಡಿ_achar(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ h, m, l;

    // using binary search to find c
    h = ARRAY_ಉದ್ದಳತೆ(achars);
    l = 0;
    ಆಗ (l < h)
    {
	m = (h + l) / 2;
	ಆದರೆ (achars[m].c == (ಚಿಹ್ನೆರಹಿತ)c)
	    ಹಿಂತಿರುಗಿ &achars[m];
	ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)c < achars[m].c)
	    h = m;
	ಇಲ್ಲದಿದ್ದರೆ
	    l = m + 1;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
chg_c_laa2i(ಪೂರ್ಣಾಂಕ hid_c)
{
    ಪೂರ್ಣಾಂಕ tempc;

    ಆಯ್ಕೆ (hid_c)
    {
	ಸ್ಥಿತಿ a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF_HAMZA_ಕೆಳಗೆ:
	    tempc = a_s_LAM_ALEF_HAMZA_ಕೆಳಗೆ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    tempc = 0;
    }

    ಹಿಂತಿರುಗಿ tempc;
}

/*
 * Change shape - from Combination-Isolated to Final
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
chg_c_laa2f(ಪೂರ್ಣಾಂಕ hid_c)
{
    ಪೂರ್ಣಾಂಕ tempc;

    ಆಯ್ಕೆ (hid_c)
    {
	ಸ್ಥಿತಿ a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF_HAMZA_ಕೆಳಗೆ:
	    tempc = a_f_LAM_ALEF_HAMZA_ಕೆಳಗೆ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    tempc = 0;
    }

    ಹಿಂತಿರುಗಿ tempc;
}

/*
 * Returns whether it is possible to join the given letters
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_join(ಪೂರ್ಣಾಂಕ c1, ಪೂರ್ಣಾಂಕ c2)
{
    ರಚನೆ achar *a1 = ಕಂಡುಹಿಡಿ_achar(c1);
    ರಚನೆ achar *a2 = ಕಂಡುಹಿಡಿ_achar(c2);

    ಹಿಂತಿರುಗಿ a1 && a2 && (a1->initial || a1->medial) && (a2->final || a2->medial);
}

/*
 * Check whether we are dealing with a character that could be regarded as an
 * Arabic combining character, need to check the character before this.
 */
    ಪೂರ್ಣಾಂಕ
arabic_maycombine(ಪೂರ್ಣಾಂಕ two)
{
    ಆದರೆ (p_arshape && !p_tbidi)
	ಹಿಂತಿರುಗಿ (two == a_ALEF_MADDA
		    || two == a_ALEF_HAMZA_ABOVE
		    || two == a_ALEF_HAMZA_ಕೆಳಗೆ
		    || two == a_ALEF);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check whether we are dealing with Arabic combining characters.
 * Note: these are NOT really composing characters!
 */
    ಪೂರ್ಣಾಂಕ
arabic_combine(
    ಪೂರ್ಣಾಂಕ		one,	    // first character
    ಪೂರ್ಣಾಂಕ		two)	    // character just after "one"
{
    ಆದರೆ (one == a_LAM)
	ಹಿಂತಿರುಗಿ arabic_maycombine(two);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * A_is_iso returns true if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
A_is_iso(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_achar(c) != ಖಾಲಿ;
}

/*
 * A_is_ok returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
A_is_ok(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (A_is_iso(c) || c == a_BYTE_ಕ್ರಮ_MARK);
}

/*
 * A_is_valid returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
A_is_valid(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ (A_is_ok(c) && c != a_HAMZA);
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
    ಪೂರ್ಣಾಂಕ
arabic_shape(
    ಪೂರ್ಣಾಂಕ		c,
    ಪೂರ್ಣಾಂಕ		*ccp,
    ಪೂರ್ಣಾಂಕ		*c1p,
    ಪೂರ್ಣಾಂಕ		prev_c,
    ಪೂರ್ಣಾಂಕ		prev_c1,
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_c)
{
    ಪೂರ್ಣಾಂಕ		curr_c;
    ಪೂರ್ಣಾಂಕ		curr_laa;
    ಪೂರ್ಣಾಂಕ		prev_laa;

    // Deal only with Arabic characters, pass back all others
    ಆದರೆ (!A_is_ok(c))
	ಹಿಂತಿರುಗಿ c;

    curr_laa = arabic_combine(c, *c1p);
    prev_laa = arabic_combine(prev_c, prev_c1);

    ಆದರೆ (curr_laa)
    {
	ಆದರೆ (A_is_valid(prev_c) && can_join(prev_c, a_LAM) && !prev_laa)
	    curr_c = chg_c_laa2f(*c1p);
	ಇಲ್ಲದಿದ್ದರೆ
	    curr_c = chg_c_laa2i(*c1p);

	// Remove the composing character
	*c1p = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ರಚನೆ achar *curr_a = ಕಂಡುಹಿಡಿ_achar(c);
	ಪೂರ್ಣಾಂಕ backward_combine = !prev_laa && can_join(prev_c, c);
	ಪೂರ್ಣಾಂಕ forward_combine = can_join(c, ಮುಂದಿನ_c);

	ಆದರೆ (backward_combine)
	{
	    ಆದರೆ (forward_combine)
		curr_c = curr_a->medial;
	    ಇಲ್ಲದಿದ್ದರೆ
		curr_c = curr_a->final;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (forward_combine)
		curr_c = curr_a->initial;
	    ಇಲ್ಲದಿದ್ದರೆ
		curr_c = curr_a->isolated;
	}
    }

    // Character missing from the table means using original character.
    ಆದರೆ (curr_c == NUL)
	curr_c = c;

    ಆದರೆ (curr_c != c && ccp != ಖಾಲಿ)
    {
	char_u buf[MB_MAXBYTES + 1];

	// Update the first byte of the character.
	(*mb_char2bytes)(curr_c, buf);
	*ccp = buf[0];
    }

    // Return the shaped character
    ಹಿಂತಿರುಗಿ curr_c;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_ARABIC
