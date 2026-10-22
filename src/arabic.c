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

#شامل "vim.h"

#اگر defined(FEAT_ARABIC)

// Unicode values for Arabic characters.
#تعریف_کرو a_HAMZA				0x0621
#تعریف_کرو a_ALEF_MADDA			0x0622
#تعریف_کرو a_ALEF_HAMZA_ABOVE		0x0623
#تعریف_کرو a_WAW_HAMZA			0x0624
#تعریف_کرو a_ALEF_HAMZA_نیچے		0x0625
#تعریف_کرو a_YEH_HAMZA			0x0626
#تعریف_کرو a_ALEF				0x0627
#تعریف_کرو a_BEH				0x0628
#تعریف_کرو a_TEH_MARBUTA			0x0629
#تعریف_کرو a_TEH				0x062a
#تعریف_کرو a_THEH				0x062b
#تعریف_کرو a_JEEM				0x062c
#تعریف_کرو a_HAH				0x062d
#تعریف_کرو a_KHAH				0x062e
#تعریف_کرو a_DAL				0x062f
#تعریف_کرو a_THAL				0x0630
#تعریف_کرو a_REH				0x0631
#تعریف_کرو a_ZAIN				0x0632
#تعریف_کرو a_نظر_آیا				0x0633
#تعریف_کرو a_SHEEN				0x0634
#تعریف_کرو a_SAD				0x0635
#تعریف_کرو a_DAD				0x0636
#تعریف_کرو a_TAH				0x0637
#تعریف_کرو a_ZAH				0x0638
#تعریف_کرو a_AIN				0x0639
#تعریف_کرو a_GHAIN				0x063a
#تعریف_کرو a_TATWEEL			0x0640
#تعریف_کرو a_FEH				0x0641
#تعریف_کرو a_QAF				0x0642
#تعریف_کرو a_KAF				0x0643
#تعریف_کرو a_LAM				0x0644
#تعریف_کرو a_MEEM				0x0645
#تعریف_کرو a_NOON				0x0646
#تعریف_کرو a_HEH				0x0647
#تعریف_کرو a_WAW				0x0648
#تعریف_کرو a_ALEF_MAKSURA			0x0649
#تعریف_کرو a_YEH				0x064a
#تعریف_کرو a_FATHATAN			0x064b
#تعریف_کرو a_DAMMATAN			0x064c
#تعریف_کرو a_KASRATAN			0x064d
#تعریف_کرو a_FATHA				0x064e
#تعریف_کرو a_DAMMA				0x064f
#تعریف_کرو a_KASRA				0x0650
#تعریف_کرو a_SHADDA			0x0651
#تعریف_کرو a_SUKUN				0x0652
#تعریف_کرو a_MADDA_ABOVE			0x0653
#تعریف_کرو a_HAMZA_ABOVE			0x0654
#تعریف_کرو a_HAMZA_نیچے			0x0655

#تعریف_کرو a_PEH				0x067e
#تعریف_کرو a_TCHEH				0x0686
#تعریف_کرو a_JEH				0x0698
#تعریف_کرو a_FKAF				0x06a9
#تعریف_کرو a_GAF				0x06af
#تعریف_کرو a_FYEH				0x06cc

#تعریف_کرو a_s_LAM_ALEF_MADDA_ABOVE	0xfef5
#تعریف_کرو a_f_LAM_ALEF_MADDA_ABOVE	0xfef6
#تعریف_کرو a_s_LAM_ALEF_HAMZA_ABOVE	0xfef7
#تعریف_کرو a_f_LAM_ALEF_HAMZA_ABOVE	0xfef8
#تعریف_کرو a_s_LAM_ALEF_HAMZA_نیچے	0xfef9
#تعریف_کرو a_f_LAM_ALEF_HAMZA_نیچے	0xfefa
#تعریف_کرو a_s_LAM_ALEF			0xfefb
#تعریف_کرو a_f_LAM_ALEF			0xfefc

ساکن ساخت achar {
    غیرعلامتی c;
    غیرعلامتی isolated;
    غیرعلامتی initial;
    غیرعلامتی medial;
    غیرعلامتی final;
} achars[] = {
    {a_HAMZA, 0xfe80, 0, 0, 0},
    {a_ALEF_MADDA, 0xfe81, 0, 0, 0xfe82},
    {a_ALEF_HAMZA_ABOVE, 0xfe83, 0, 0, 0xfe84},
    {a_WAW_HAMZA, 0xfe85, 0, 0, 0xfe86},
    {a_ALEF_HAMZA_نیچے, 0xfe87, 0, 0, 0xfe88},
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
    {a_نظر_آیا, 0xfeb1, 0xfeb3, 0xfeb4, 0xfeb2},
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
    {a_HAMZA_نیچے, 0, 0, 0, 0},
    {a_PEH, 0xfb56, 0xfb58, 0xfb59, 0xfb57},
    {a_TCHEH, 0xfb7a, 0xfb7c, 0xfb7d, 0xfb7b},
    {a_JEH, 0xfb8a, 0, 0, 0xfb8b},
    {a_FKAF, 0xfb8e, 0xfb90, 0xfb91, 0xfb8f},
    {a_GAF, 0xfb92, 0xfb94, 0xfb95, 0xfb93},
    {a_FYEH, 0xfbfc, 0xfbfe, 0xfbff, 0xfbfd},
};

#تعریف_کرو a_BYTE_ترتیب_MARK		0xfeff

/*
 * Find the struct achar pointer to the given Arabic char.
 * Returns NULL if not found.
 */
    ساکن ساخت achar *
ڈھونڈو_achar(صحیح c)
{
    صحیح h, m, l;

    // using binary search to find c
    h = ARRAY_لمبائی(achars);
    l = 0;
    جب (l < h)
    {
	m = (h + l) / 2;
	اگر (achars[m].c == (غیرعلامتی)c)
	    واپس &achars[m];
	اگر ((غیرعلامتی)c < achars[m].c)
	    h = m;
	ورنہ
	    l = m + 1;
    }
    واپس تہی;
}

/*
 * Change shape - from Combination (2 char) to an Isolated
 */
    ساکن صحیح
chg_c_laa2i(صحیح hid_c)
{
    صحیح tempc;

    انتخاب (hid_c)
    {
	حالت a_ALEF_MADDA:
	    tempc = a_s_LAM_ALEF_MADDA_ABOVE;
	    توڑو;
	حالت a_ALEF_HAMZA_ABOVE:
	    tempc = a_s_LAM_ALEF_HAMZA_ABOVE;
	    توڑو;
	حالت a_ALEF_HAMZA_نیچے:
	    tempc = a_s_LAM_ALEF_HAMZA_نیچے;
	    توڑو;
	حالت a_ALEF:
	    tempc = a_s_LAM_ALEF;
	    توڑو;
	طےشدہ:
	    tempc = 0;
    }

    واپس tempc;
}

/*
 * Change shape - from Combination-Isolated to Final
 */
    ساکن صحیح
chg_c_laa2f(صحیح hid_c)
{
    صحیح tempc;

    انتخاب (hid_c)
    {
	حالت a_ALEF_MADDA:
	    tempc = a_f_LAM_ALEF_MADDA_ABOVE;
	    توڑو;
	حالت a_ALEF_HAMZA_ABOVE:
	    tempc = a_f_LAM_ALEF_HAMZA_ABOVE;
	    توڑو;
	حالت a_ALEF_HAMZA_نیچے:
	    tempc = a_f_LAM_ALEF_HAMZA_نیچے;
	    توڑو;
	حالت a_ALEF:
	    tempc = a_f_LAM_ALEF;
	    توڑو;
	طےشدہ:
	    tempc = 0;
    }

    واپس tempc;
}

/*
 * Returns whether it is possible to join the given letters
 */
    ساکن صحیح
can_join(صحیح c1, صحیح c2)
{
    ساخت achar *a1 = ڈھونڈو_achar(c1);
    ساخت achar *a2 = ڈھونڈو_achar(c2);

    واپس a1 && a2 && (a1->initial || a1->medial) && (a2->final || a2->medial);
}

/*
 * Check whether we are dealing with a character that could be regarded as an
 * Arabic combining character, need to check the character before this.
 */
    صحیح
arabic_maycombine(صحیح two)
{
    اگر (p_arshape && !p_tbidi)
	واپس (two == a_ALEF_MADDA
		    || two == a_ALEF_HAMZA_ABOVE
		    || two == a_ALEF_HAMZA_نیچے
		    || two == a_ALEF);
    واپس FALSE;
}

/*
 * Check whether we are dealing with Arabic combining characters.
 * Note: these are NOT really composing characters!
 */
    صحیح
arabic_combine(
    صحیح		one,	    // first character
    صحیح		two)	    // character just after "one"
{
    اگر (one == a_LAM)
	واپس arabic_maycombine(two);
    واپس FALSE;
}

/*
 * A_is_iso returns true if 'c' is an Arabic ISO-8859-6 character
 *		(alphabet/number/punctuation)
 */
    ساکن صحیح
A_is_iso(صحیح c)
{
    واپس ڈھونڈو_achar(c) != تہی;
}

/*
 * A_is_ok returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 */
    ساکن صحیح
A_is_ok(صحیح c)
{
    واپس (A_is_iso(c) || c == a_BYTE_ترتیب_MARK);
}

/*
 * A_is_valid returns true if 'c' is an Arabic 10646 (8859-6 or Form-B)
 *		with some exceptions/exclusions
 */
    ساکن صحیح
A_is_valid(صحیح c)
{
    واپس (A_is_ok(c) && c != a_HAMZA);
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
    صحیح
arabic_shape(
    صحیح		c,
    صحیح		*ccp,
    صحیح		*c1p,
    صحیح		پچھلا_c,
    صحیح		پچھلا_c1,
    صحیح		اگلا_c)
{
    صحیح		curr_c;
    صحیح		curr_laa;
    صحیح		پچھلا_laa;

    // Deal only with Arabic characters, pass back all others
    اگر (!A_is_ok(c))
	واپس c;

    curr_laa = arabic_combine(c, *c1p);
    پچھلا_laa = arabic_combine(پچھلا_c, پچھلا_c1);

    اگر (curr_laa)
    {
	اگر (A_is_valid(پچھلا_c) && can_join(پچھلا_c, a_LAM) && !پچھلا_laa)
	    curr_c = chg_c_laa2f(*c1p);
	ورنہ
	    curr_c = chg_c_laa2i(*c1p);

	// Remove the composing character
	*c1p = 0;
    }
    ورنہ
    {
	ساخت achar *curr_a = ڈھونڈو_achar(c);
	صحیح backward_combine = !پچھلا_laa && can_join(پچھلا_c, c);
	صحیح forward_combine = can_join(c, اگلا_c);

	اگر (backward_combine)
	{
	    اگر (forward_combine)
		curr_c = curr_a->medial;
	    ورنہ
		curr_c = curr_a->final;
	}
	ورنہ
	{
	    اگر (forward_combine)
		curr_c = curr_a->initial;
	    ورنہ
		curr_c = curr_a->isolated;
	}
    }

    // Character missing from the table means using original character.
    اگر (curr_c == NUL)
	curr_c = c;

    اگر (curr_c != c && ccp != تہی)
    {
	char_u buf[MB_MAXBYTES + 1];

	// Update the first byte of the character.
	(*mb_char2bytes)(curr_c, buf);
	*ccp = buf[0];
    }

    // Return the shaped character
    واپس curr_c;
}
#اگر_ختم // FEAT_ARABIC
