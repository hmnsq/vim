/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * textobject.c: functions for text objects
 */
#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cls(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ skip_chars(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ಪೂರ್ಣಾಂಕ
findsent(ಪೂರ್ಣಾಂಕ dir, ದೀರ್ಘ ಎಣಿಕೆ_2)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		(*func)(pos_T *);
    ಪೂರ್ಣಾಂಕ		startlnum;
    ಪೂರ್ಣಾಂಕ		noskip = FALSE;	    // do not skip blanks
    ಪೂರ್ಣಾಂಕ		cpo_J;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_dot;

    pos = curwin->w_cursor;
    ಆದರೆ (dir == FORWARD)
	func = incl;
    ಇಲ್ಲದಿದ್ದರೆ
	func = decl;

    ಆಗ (ಎಣಿಕೆ_2--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ಆದರೆ (gchar_pos(&pos) == NUL)
	{
	    ಮಾಡು
		ಆದರೆ ((*func)(&pos) == -1)
		    ನಿಲ್ಲಿಸು;
	    ಆಗ (gchar_pos(&pos) == NUL);
	    ಆದರೆ (dir == FORWARD)
		ಹೋಗು ಸಿಕ್ಕಿತು;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == FORWARD && pos.ಲಂಬಸಾಲು == 0 &&
						ಆರಂಭ_PS(pos.lnum, NUL, FALSE))
	{
	    ಆದರೆ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ++pos.lnum;
	    ಹೋಗು ಸಿಕ್ಕಿತು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	ಸಿಕ್ಕಿತು_dot = FALSE;
	ಆಗ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ಖಾಲಿ)
	{
	    tpos = pos;
	    ಆದರೆ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (ಸಿಕ್ಕಿತು_dot)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (vim_strchr((char_u *) ".!?", c) != ಖಾಲಿ)
		ಸಿಕ್ಕಿತು_dot = TRUE;

	    ಆದರೆ (vim_strchr((char_u *) ")]\"'", c) != ಖಾಲಿ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ಖಾಲಿ;

	ಗಾಗಿ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ಆದರೆ (c == NUL || (pos.ಲಂಬಸಾಲು == 0 && ಆರಂಭ_PS(pos.lnum, NUL, FALSE)))
	    {
		ಆದರೆ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		ಮಾಡು
		    ಆದರೆ ((c = inc(&tpos)) == -1)
			ನಿಲ್ಲಿಸು;
		ಆಗ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ಖಾಲಿ);
		ಆದರೆ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ಆದರೆ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಆದರೆ ((*func)(&pos) == -1)
	    {
		ಆದರೆ (ಎಣಿಕೆ_2)
		    ಹಿಂತಿರುಗಿ FAIL;
		noskip = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
ಸಿಕ್ಕಿತು:
	    // skip white space
	ಆಗ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ಆದರೆ (incl(&pos) == -1)
		ನಿಲ್ಲಿಸು;

	ಆದರೆ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ಆದರೆ ((*func)(&pos) == -1)
	    {
		ಆದರೆ (ಎಣಿಕೆ_2)
		    ಹಿಂತಿರುಗಿ FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ++ಎಣಿಕೆ_2;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ಪೂರ್ಣಾಂಕ
findpar(
    ಪೂರ್ಣಾಂಕ		*pincl,	    // Return: TRUE if last char is to be included
    ಪೂರ್ಣಾಂಕ		dir,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		what,
    ಪೂರ್ಣಾಂಕ		both)
{
    linenr_T	curr;
    ಪೂರ್ಣಾಂಕ		did_skip;   // TRUE after separating lines have been skipped
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;	    // TRUE on first line
    ಪೂರ್ಣಾಂಕ		posix = (vim_strchr(p_cpo, CPO_PARA) != ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    linenr_T	fold_ಮೊದಲ;	// first line of a closed fold
    linenr_T	fold_ಕೊನೆಯ;	// last line of a closed fold
    ಪೂರ್ಣಾಂಕ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#ಆದರೆ_ಅಂತ್ಯ

    curr = curwin->w_cursor.lnum;

    ಆಗ (ಎಣಿಕೆ_2--)
    {
	did_skip = FALSE;
	ಗಾಗಿ (ಮೊದಲ = TRUE; ; ಮೊದಲ = FALSE)
	{
	    ಆದರೆ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ಆದರೆ (ಮೊದಲ && hasFolding(curr, &fold_ಮೊದಲ, &fold_ಕೊನೆಯ))
	    {
		curr = ((dir > 0) ? fold_ಕೊನೆಯ : fold_ಮೊದಲ) + dir;
		fold_skipped = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ಆದರೆ (!ಮೊದಲ && did_skip && (ಆರಂಭ_PS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    ಆದರೆ (fold_skipped)
		curr -= dir;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಆದರೆ (ಎಣಿಕೆ_2)
		    ಹಿಂತಿರುಗಿ FALSE;
		curr -= dir;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    setpcmark();
    ಆದರೆ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ಆದರೆ (curr == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && what != '}' && dir == FORWARD)
    {
	char_u *ಸಾಲು = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ಆದರೆ ((curwin->w_cursor.ಲಂಬಸಾಲು = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.ಲಂಬಸಾಲು;
	    curwin->w_cursor.ಲಂಬಸಾಲು -=
			     (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು);
	    *pincl = TRUE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ಗಾಗಿ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ಆದರೆ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    ನಿಲ್ಲಿಸು;
	++macro;
	ಆದರೆ (macro[0] == NUL)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ಪೂರ್ಣಾಂಕ
ಆರಂಭ_PS(linenr_T lnum, ಪೂರ್ಣಾಂಕ para, ಪೂರ್ಣಾಂಕ both)
{
    char_u	*s;

    s = ml_get(lnum);
    ಆದರೆ (*s == para || *s == '\f' || (both && *s == '}'))
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * The following routines do the word searches performed by the 'w', 'W',
 * 'b', 'B', 'e', and 'E' commands.
 */

/*
 * To perform these searches, characters are placed into one of three
 * classes, and transitions between classes determine word boundaries.
 *
 * The classes are:
 *
 * 0 - white space
 * 1 - punctuation
 * 2 or higher - keyword characters (letters, digits and underscore)
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cls(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    c;

    c = gchar_cursor();
    ಆದರೆ (c == ' ' || c == '\t' || c == NUL)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ಆದರೆ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ಹಿಂತಿರುಗಿ 1;

	// process code leading/trailing bytes
	ಹಿಂತಿರುಗಿ dbcs_class(((ಚಿಹ್ನೆರಹಿತ)c >> 8), (c & 0xFF));
    }
    ಆದರೆ (enc_utf8)
    {
	c = utf_class(c);
	ಆದರೆ (c != 0 && cls_bigword)
	    ಹಿಂತಿರುಗಿ 1;
	ಹಿಂತಿರುಗಿ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ಆದರೆ (cls_bigword)
	ಹಿಂತಿರುಗಿ 1;

    ಆದರೆ (vim_iswordc(c))
	ಹಿಂತಿರುಗಿ 2;
    ಹಿಂತಿರುಗಿ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ಪೂರ್ಣಾಂಕ
fwd_ಶಬ್ದ(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		bigword,    // "W", "E" or "B"
    ಪೂರ್ಣಾಂಕ		eol)
{
    ಪೂರ್ಣಾಂಕ		sclass;	    // starting class
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_ಸಾಲು;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ಆದರೆ (hasFolding(curwin->w_cursor.lnum, ಖಾಲಿ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#ಆದರೆ_ಅಂತ್ಯ
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	ಕೊನೆಯ_ಸಾಲು = (curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	i = inc_cursor();
	ಆದರೆ (i == -1 || (i >= 1 && ಕೊನೆಯ_ಸಾಲು)) // started at last char in file
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (i >= 1 && eol && ಎಣಿಕೆ_2 == 0)      // started at last char in line
	    ಹಿಂತಿರುಗಿ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ಆದರೆ (sclass != 0)
	    ಆಗ (cls() == sclass)
	    {
		i = inc_cursor();
		ಆದರೆ (i == -1 || (i >= 1 && eol && ಎಣಿಕೆ_2 == 0))
		    ಹಿಂತಿರುಗಿ OK;
	    }

	/*
	 * go to next non-white
	 */
	ಆಗ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0 && *ml_get_curline() == NUL)
		ನಿಲ್ಲಿಸು;

	    i = inc_cursor();
	    ಆದರೆ (i == -1 || (i >= 1 && eol && ಎಣಿಕೆ_2 == 0))
		ಹಿಂತಿರುಗಿ OK;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ಪೂರ್ಣಾಂಕ
bck_ಶಬ್ದ(ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ bigword, ಪೂರ್ಣಾಂಕ stop)
{
    ಪೂರ್ಣಾಂಕ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ಆದರೆ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ಖಾಲಿ))
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ
	sclass = cls();
	ಆದರೆ (dec_cursor() == -1)		// started at start of file
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ಆಗ (cls() == 0)
	    {
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    ಹೋಗು ಪೂರ್ಣಗೊಂಡ;
		ಆದರೆ (dec_cursor() == -1) // hit start of file, stop here
		    ಹಿಂತಿರುಗಿ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ಆದರೆ (skip_chars(cls(), BACKWARD))
		ಹಿಂತಿರುಗಿ OK;
	}

	inc_cursor();			// overshot - forward one
ಪೂರ್ಣಗೊಂಡ:
	stop = FALSE;
    }
    adjust_skipcol();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * end_word() - move to the end of the word
 *
 * There is an apparent bug in the 'e' motion of the real vi. At least on the
 * System V Release 3 version for the 80386. Unlike 'b' and 'w', the 'e'
 * motion crosses blank lines. When the real vi crosses a blank line in an
 * 'e' motion, the cursor is placed on the FIRST character of the next
 * non-blank line. The 'E' command, however, works correctly. Since this
 * appears to be a bug, I have not duplicated it here.
 *
 * Returns FAIL if end of the file was reached.
 *
 * If stop is TRUE and we are already on the end of a word, move one less.
 * If empty is TRUE stop on an empty line.
 */
    ಪೂರ್ಣಾಂಕ
ಕೊನೆ_ಶಬ್ದ(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		bigword,
    ಪೂರ್ಣಾಂಕ		stop,
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2)
{
    ಪೂರ್ಣಾಂಕ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ಆದರೆ (*p_sel == 'e' && VIsual_ಸಕ್ರಿಯ && VIsual_ಬಹುಲಕ == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ಆದರೆ (hasFolding(curwin->w_cursor.lnum, ಖಾಲಿ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#ಆದರೆ_ಅಂತ್ಯ
	sclass = cls();
	ಆದರೆ (inc_cursor() == -1)
	    ಹಿಂತಿರುಗಿ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ಆದರೆ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ಆದರೆ (skip_chars(sclass, FORWARD))
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ಆಗ (cls() == 0)
	    {
		ಆದರೆ (ಖಾಲಿಗಳು_2 && curwin->w_cursor.ಲಂಬಸಾಲು == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    ಹೋಗು ಪೂರ್ಣಗೊಂಡ;
		ಆದರೆ (inc_cursor() == -1)	    // hit end of file, stop here
		    ಹಿಂತಿರುಗಿ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ಆದರೆ (skip_chars(cls(), FORWARD))
		ಹಿಂತಿರುಗಿ FAIL;
	}
	dec_cursor();			// overshot - one char backward
ಪೂರ್ಣಗೊಂಡ:
	stop = FALSE;			// we move only one word less
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ಪೂರ್ಣಾಂಕ
bckend_ಶಬ್ದ(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		bigword,    // TRUE for "B"
    ಪೂರ್ಣಾಂಕ		eol)	    // TRUE: stop at end of line.
{
    ಪೂರ್ಣಾಂಕ		sclass;	    // starting class
    ಪೂರ್ಣಾಂಕ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಆಗ (--ಎಣಿಕೆ_2 >= 0)
    {
	sclass = cls();
	ಆದರೆ ((i = dec_cursor()) == -1)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (eol && i == 1)
	    ಹಿಂತಿರುಗಿ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ಆದರೆ (sclass != 0)
	{
	    ಆಗ (cls() == sclass)
		ಆದರೆ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ಹಿಂತಿರುಗಿ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ಆಗ (cls() == 0)
	{
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ಹಿಂತಿರುಗಿ OK;
	}
    }
    adjust_skipcol();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_chars(ಪೂರ್ಣಾಂಕ cclass, ಪೂರ್ಣಾಂಕ dir)
{
    ಆಗ (cls() == cclass)
	ಆದರೆ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಹಿಂಭಾಗ_ಒಳ_ಸಾಲು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		sclass;		    // starting class

    sclass = cls();
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)	    // stop at start of line
	    ನಿಲ್ಲಿಸು;
	dec_cursor();
	ಆದರೆ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ನಿಲ್ಲಿಸು;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(pos_T *posp)
{
    ಪೂರ್ಣಾಂಕ	    c;

    ಆಗ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ಆದರೆ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ನಿಲ್ಲಿಸು;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findsent_forward(
    ದೀರ್ಘ    ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ	    at_ಆರಂಭ_ಕಳುಹಿಸಿದ)	// cursor is at start of sentence
{
    ಆಗ (ಎಣಿಕೆ_2--)
    {
	findsent(FORWARD, 1L);
	ಆದರೆ (at_ಆರಂಭ_ಕಳುಹಿಸಿದ)
	    ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(&curwin->w_cursor);
	ಆದರೆ (ಎಣಿಕೆ_2 == 0 || at_ಆರಂಭ_ಕಳುಹಿಸಿದ)
	    decl(&curwin->w_cursor);
	at_ಆರಂಭ_ಕಳುಹಿಸಿದ = !at_ಆರಂಭ_ಕಳುಹಿಸಿದ;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ಪೂರ್ಣಾಂಕ
current_ಶಬ್ದ(
    oparg_T	*oap,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		include,	// TRUE: include word and white space
    ಪೂರ್ಣಾಂಕ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	ಆರಂಭ_pos;
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		inclusive = TRUE;
    ಪೂರ್ಣಾಂಕ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&ಆರಂಭ_pos);

    // Correct cursor when 'selection' is exclusive
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	ಹಿಂಭಾಗ_ಒಳ_ಸಾಲು();
	ಆರಂಭ_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ಆದರೆ ((cls() == 0) == include)
	{
	    ಆದರೆ (ಕೊನೆ_ಶಬ್ದ(1L, bigword, TRUE, TRUE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_ಶಬ್ದ(1L, bigword, TRUE);
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
		decl(&curwin->w_cursor);
	    ಇಲ್ಲದಿದ್ದರೆ
		oneleft();

	    ಆದರೆ (include)
		include_white = TRUE;
	}

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = ಆರಂಭ_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    oap->ಆರಂಭ = ಆರಂಭ_pos;
	    oap->motion_type = MCHAR;
	}
	--ಎಣಿಕೆ_2;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ಆಗ (ಎಣಿಕೆ_2 > 0)
    {
	inclusive = TRUE;
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ಆದರೆ (decl(&curwin->w_cursor) == -1)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (include != (cls() != 0))
	    {
		ಆದರೆ (bck_ಶಬ್ದ(1L, bigword, TRUE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (bckend_ಶಬ್ದ(1L, bigword, TRUE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		(ಶೂನ್ಯ)incl(&curwin->w_cursor);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ಆದರೆ (incl(&curwin->w_cursor) == -1)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (include != (cls() == 0))
	    {
		ಆದರೆ (fwd_ಶಬ್ದ(1L, bigword, TRUE) == FAIL && ಎಣಿಕೆ_2 > 1)
		    ಹಿಂತಿರುಗಿ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ಆದರೆ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಕೊನೆ_ಶಬ್ದ(1L, bigword, TRUE, TRUE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	--ಎಣಿಕೆ_2;
    }

    ಆದರೆ (include_white && (cls() != 0
		 || (curwin->w_cursor.ಲಂಬಸಾಲು == 0 && !inclusive)))
    {
	/*
	 * If we don't include white space at the end, move the start
	 * to include some white space there. This makes "daw" work
	 * better on the last word in a sentence (and "2daw" on last-but-one
	 * word).  Also when "2daw" deletes "word." at the end of the line
	 * (cursor is at start of next line).
	 * But don't delete white space at start of line (indent).
	 */
	pos = curwin->w_cursor;	// save cursor position
	curwin->w_cursor = ಆರಂಭ_pos;
	ಆದರೆ (oneleft() == OK)
	{
	    ಹಿಂಭಾಗ_ಒಳ_ಸಾಲು();
	    ಆದರೆ (cls() == 0 && curwin->w_cursor.ಲಂಬಸಾಲು > 0)
	    {
		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		    VIsual = curwin->w_cursor;
		ಇಲ್ಲದಿದ್ದರೆ
		    oap->ಆರಂಭ = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')
	{
	    VIsual_ಬಹುಲಕ = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	oap->inclusive = inclusive;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ಪೂರ್ಣಾಂಕ
current_ಕಳುಹಿಸಿದ(oparg_T *oap, ದೀರ್ಘ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ include)
{
    pos_T	ಆರಂಭ_pos;
    pos_T	pos;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_blank;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		at_ಆರಂಭ_ಕಳುಹಿಸಿದ;
    ದೀರ್ಘ	ncount;

    ಆರಂಭ_pos = curwin->w_cursor;
    pos = ಆರಂಭ_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && !EQUAL_POS(ಆರಂಭ_pos, VIsual))
    {
extend:
	ಆದರೆ (LT_POS(ಆರಂಭ_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_ಆರಂಭ_ಕಳುಹಿಸಿದ = TRUE;
	    decl(&pos);
	    ಆಗ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ಆದರೆ (!VIM_ISWHITE(c))
		{
		    at_ಆರಂಭ_ಕಳುಹಿಸಿದ = FALSE;
		    ನಿಲ್ಲಿಸು;
		}
		incl(&pos);
	    }
	    ಆದರೆ (!at_ಆರಂಭ_ಕಳುಹಿಸಿದ)
	    {
		findsent(BACKWARD, 1L);
		ಆದರೆ (EQUAL_POS(curwin->w_cursor, ಆರಂಭ_pos))
		    at_ಆರಂಭ_ಕಳುಹಿಸಿದ = TRUE;  // exactly at start of sentence
		ಇಲ್ಲದಿದ್ದರೆ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ಆದರೆ (include)	// "as" gets twice as much as "is"
		ಎಣಿಕೆ_2 *= 2;
	    ಆಗ (ಎಣಿಕೆ_2--)
	    {
		ಆದರೆ (at_ಆರಂಭ_ಕಳುಹಿಸಿದ)
		    ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ಆದರೆ (!at_ಆರಂಭ_ಕಳುಹಿಸಿದ || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_ಆರಂಭ_ಕಳುಹಿಸಿದ = !at_ಆರಂಭ_ಕಳುಹಿಸಿದ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_ಆರಂಭ_ಕಳುಹಿಸಿದ = TRUE;
	    // not just before a sentence
	    ಆದರೆ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_ಆರಂಭ_ಕಳುಹಿಸಿದ = FALSE;
		ಆಗ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ಆದರೆ (!VIM_ISWHITE(c))
		    {
			at_ಆರಂಭ_ಕಳುಹಿಸಿದ = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		    incl(&pos);
		}
		ಆದರೆ (at_ಆರಂಭ_ಕಳುಹಿಸಿದ)	// in the sentence
		    findsent(BACKWARD, 1L);
		ಇಲ್ಲದಿದ್ದರೆ		// in/before white before a sentence
		    curwin->w_cursor = ಆರಂಭ_pos;
	    }

	    ಆದರೆ (include)	// "as" gets twice as much as "is"
		ಎಣಿಕೆ_2 *= 2;
	    findsent_forward(ಎಣಿಕೆ_2, at_ಆರಂಭ_ಕಳುಹಿಸಿದ);
	    ಆದರೆ (*p_sel == 'e')
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	}
	ಹಿಂತಿರುಗಿ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ಆಗ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ಆದರೆ (EQUAL_POS(pos, curwin->w_cursor))
    {
	ಆರಂಭ_blank = TRUE;
	ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(&ಆರಂಭ_pos);	// go back to first blank
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆರಂಭ_blank = FALSE;
	findsent(BACKWARD, 1L);
	ಆರಂಭ_pos = curwin->w_cursor;
    }
    ಆದರೆ (include)
	ncount = ಎಣಿಕೆ_2 * 2;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ncount = ಎಣಿಕೆ_2;
	ಆದರೆ (ಆರಂಭ_blank)
	    --ncount;
    }
    ಆದರೆ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	decl(&curwin->w_cursor);

    ಆದರೆ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ಆದರೆ (ಆರಂಭ_blank)
	{
	    ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ಆದರೆ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    ಕಂಡುಹಿಡಿ_ಮೊದಲ_blank(&ಆರಂಭ_pos);
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ಆದರೆ (EQUAL_POS(ಆರಂಭ_pos, curwin->w_cursor))
	    ಹೋಗು extend;
	ಆದರೆ (*p_sel == 'e')
	    ++curwin->w_cursor.ಲಂಬಸಾಲು;
	VIsual = ಆರಂಭ_pos;
	VIsual_ಬಹುಲಕ = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// include a newline after the sentence, if there is one
	ಆದರೆ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    oap->inclusive = FALSE;
	oap->ಆರಂಭ = ಆರಂಭ_pos;
	oap->motion_type = MCHAR;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ಪೂರ್ಣಾಂಕ
current_ಖಂಡ(
    oparg_T	*oap,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		include,	// TRUE == include white space
    ಪೂರ್ಣಾಂಕ		what,		// '(', '{', etc.
    ಪೂರ್ಣಾಂಕ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ಖಾಲಿ;
    pos_T	ಆರಂಭ_pos;
    pos_T	*ಕೊನೆ_pos;
    pos_T	old_ಆರಂಭ, old_ಕೊನೆ;
    char_u	*save_cpo;
    ಪೂರ್ಣಾಂಕ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_ಕೊನೆ = curwin->w_cursor;		// remember where we started
    old_ಆರಂಭ = old_ಕೊನೆ;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ಆದರೆ (what == '{')		// ignore indent
	    ಆಗ (inindent(1))
		ಆದರೆ (inc_cursor() != 0)
		    ನಿಲ್ಲಿಸು;
	ಆದರೆ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ಆರಂಭ = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ಇಲ್ಲದಿದ್ದರೆ
	old_ಕೊನೆ = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ಖಾಲಿ ? "%M" : "%");
    ಆದರೆ ((pos = findmatch(ಖಾಲಿ, what)) != ಖಾಲಿ)
    {
	ಆಗ (ಎಣಿಕೆ_2-- > 0)
	{
	    ಆದರೆ ((pos = findmatch(ಖಾಲಿ, what)) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    curwin->w_cursor = *pos;
	    ಆರಂಭ_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (ಎಣಿಕೆ_2-- > 0)
	{
	    ಆದರೆ ((pos = findmatchlimit(ಖಾಲಿ, what, FM_FORWARD, 0)) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    curwin->w_cursor = *pos;
	    ಆರಂಭ_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ಆದರೆ (pos == ಖಾಲಿ || (ಕೊನೆ_pos = findmatch(ಖಾಲಿ, other)) == ಖಾಲಿ)
    {
	curwin->w_cursor = old_pos;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    curwin->w_cursor = *ಕೊನೆ_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ಆಗ (!include)
    {
	incl(&ಆರಂಭ_pos);
	sol = (curwin->w_cursor.ಲಂಬಸಾಲು == 0);
	decl(&curwin->w_cursor);
	ಆಗ (inindent(1))
	{
	    sol = TRUE;
	    ಆದರೆ (decl(&curwin->w_cursor) != 0)
		ನಿಲ್ಲಿಸು;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ಆದರೆ (EQUAL_POS(ಆರಂಭ_pos, *ಕೊನೆ_pos) && VIsual_ಸಕ್ರಿಯ)
	{
	    curwin->w_cursor = old_pos;
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ಆದರೆ (!LT_POS(ಆರಂಭ_pos, old_ಆರಂಭ) && !LT_POS(old_ಕೊನೆ, curwin->w_cursor)
		&& !EQUAL_POS(ಆರಂಭ_pos, curwin->w_cursor)
		&& VIsual_ಸಕ್ರಿಯ)
	{
	    curwin->w_cursor = old_ಆರಂಭ;
	    decl(&curwin->w_cursor);
	    ಆದರೆ ((pos = findmatch(ಖಾಲಿ, what)) == ಖಾಲಿ)
	    {
		curwin->w_cursor = old_pos;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆರಂಭ_pos = *pos;
	    curwin->w_cursor = *pos;
	    ಆದರೆ ((ಕೊನೆ_pos = findmatch(ಖಾಲಿ, other)) == ಖಾಲಿ)
	    {
		curwin->w_cursor = old_pos;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    curwin->w_cursor = *ಕೊನೆ_pos;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ಆದರೆ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = ಆರಂಭ_pos;
	VIsual_ಬಹುಲಕ = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	oap->ಆರಂಭ = ಆರಂಭ_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ಆದರೆ (sol)
	    incl(&curwin->w_cursor);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LTOREQ_POS(ಆರಂಭ_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = ಆರಂಭ_pos;
    }

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಒಳ_html_ಟ್ಯಾಗ್(
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಟ್ಯಾಗ್)
{
    char_u	*ಸಾಲು = ml_get_curline();
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		lc = NUL;
    pos_T	pos;

    ಆದರೆ (enc_dbcs)
    {
	char_u	*lp = ಖಾಲಿ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ಗಾಗಿ (p = ಸಾಲು; p < ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು; MB_PTR_ADV(p))
	    ಆದರೆ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ಆದರೆ (*p != '<')	    // check for '<' under cursor
	{
	    ಆದರೆ (lc != '<')
		ಹಿಂತಿರುಗಿ FALSE;
	    p = lp;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (p = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು; p > ಸಾಲು; )
	{
	    ಆದರೆ (*p == '<')	// find '<' under/before cursor
		ನಿಲ್ಲಿಸು;
	    MB_PTR_ಹಿಂಭಾಗ(ಸಾಲು, p);
	    ಆದರೆ (*p == '>')	// find '>' before cursor
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*p != '<')
	    ಹಿಂತಿರುಗಿ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.ಲಂಬಸಾಲು = (colnr_T)(p - ಸಾಲು);

    MB_PTR_ADV(p);
    ಆದರೆ (ಕೊನೆ_ಟ್ಯಾಗ್)
	// check that there is a '/' after the '<'
	ಹಿಂತಿರುಗಿ *p == '/';

    // check that there is no '/' after the '<'
    ಆದರೆ (*p == '/')
	ಹಿಂತಿರುಗಿ FALSE;

    // check that the matching '>' is not preceded by '/'
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (inc(&pos) < 0)
	    ಹಿಂತಿರುಗಿ FALSE;
	c = *ml_get_pos(&pos);
	ಆದರೆ (c == '>')
	    ನಿಲ್ಲಿಸು;
	lc = c;
    }
    ಹಿಂತಿರುಗಿ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ಪೂರ್ಣಾಂಕ
current_tagblock(
    oparg_T	*oap,
    ದೀರ್ಘ	ಎಣಿಕೆ_arg,
    ಪೂರ್ಣಾಂಕ		include)	// TRUE == include white space
{
    ದೀರ್ಘ	ಎಣಿಕೆ_2 = ಎಣಿಕೆ_arg;
    ದೀರ್ಘ	n;
    pos_T	old_pos;
    pos_T	ಆರಂಭ_pos;
    pos_T	ಕೊನೆ_pos;
    pos_T	old_ಆರಂಭ, old_ಕೊನೆ;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		do_include = include;
    ಪೂರ್ಣಾಂಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ಪೂರ್ಣಾಂಕ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_ಕೊನೆ = curwin->w_cursor;		    // remember where we started
    old_ಆರಂಭ = old_ಕೊನೆ;
    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ || *p_sel == 'e')
	decl(&old_ಕೊನೆ);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ಆಗ (inindent(1))
	    ಆದರೆ (inc_cursor() != 0)
		ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಒಳ_html_ಟ್ಯಾಗ್(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ಆಗ (*ml_get_cursor() != '>')
		ಆದರೆ (inc_cursor() < 0)
		    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_html_ಟ್ಯಾಗ್(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ಆಗ (*ml_get_cursor() != '<')
		ಆದರೆ (dec_cursor() < 0)
		    ನಿಲ್ಲಿಸು;
	    dec_cursor();
	    old_ಕೊನೆ = curwin->w_cursor;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ಆರಂಭ = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ಇಲ್ಲದಿದ್ದರೆ
	old_ಕೊನೆ = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ಗಾಗಿ (n = 0; n < ಎಣಿಕೆ_2; ++n)
    {
	ಆದರೆ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ಖಾಲಿ, 0,
						  ಖಾಲಿ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    ಹೋಗು theend;
	}
    }
    ಆರಂಭ_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    ಗಾಗಿ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ಪೂರ್ಣಾಂಕ)(cp - p);
    ಆದರೆ (len == 0)
    {
	curwin->w_cursor = old_pos;
	ಹೋಗು theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ಆದರೆ (spat == ಖಾಲಿ || epat == ಖಾಲಿ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	ಹೋಗು theend;
    }
    sprintf((ಅಕ್ಷರ *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((ಅಕ್ಷರ *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ಖಾಲಿ,
						    0, ಖಾಲಿ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ಆದರೆ (r < 1 || LT_POS(curwin->w_cursor, old_ಕೊನೆ))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	ಎಣಿಕೆ_2 = 1;
	curwin->w_cursor = ಆರಂಭ_pos;
	ಹೋಗು again;
    }

    ಆದರೆ (do_include)
    {
	// Include up to the '>'.
	ಆಗ (*ml_get_cursor() != '>')
	    ಆದರೆ (inc_cursor() < 0)
		ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ಆದರೆ (*c == '<' && !VIsual_ಸಕ್ರಿಯ && curwin->w_cursor.ಲಂಬಸಾಲು == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*c == '<')
	    dec_cursor();
    }
    ಕೊನೆ_pos = curwin->w_cursor;

    ಆದರೆ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ಪೂರ್ಣಾಂಕ ಒಳ_quotes = FALSE;
	curwin->w_cursor = ಆರಂಭ_pos;
	ಆಗ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ಆದರೆ (*p == '>' && !ಒಳ_quotes)
	    {
		inc_cursor();
		ಆರಂಭ_pos = curwin->w_cursor;
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"' || *p == '\'')
		ಒಳ_quotes = !ಒಳ_quotes;
	}
	curwin->w_cursor = ಕೊನೆ_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ && EQUAL_POS(ಆರಂಭ_pos, old_ಆರಂಭ)
						&& EQUAL_POS(ಕೊನೆ_pos, old_ಕೊನೆ))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_ಆರಂಭ;
	    ಎಣಿಕೆ_2 = ಎಣಿಕೆ_arg;
	    ಹೋಗು again;
	}
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ಆದರೆ (LT_POS(ಕೊನೆ_pos, ಆರಂಭ_pos))
	    curwin->w_cursor = ಆರಂಭ_pos;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_sel == 'e')
	    inc_cursor();
	VIsual = ಆರಂಭ_pos;
	VIsual_ಬಹುಲಕ = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	oap->ಆರಂಭ = ಆರಂಭ_pos;
	oap->motion_type = MCHAR;
	ಆದರೆ (LT_POS(ಕೊನೆ_pos, ಆರಂಭ_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = ಆರಂಭ_pos;
	    oap->inclusive = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
current_par(
    oparg_T	*oap,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		include,	// TRUE == include white space
    ಪೂರ್ಣಾಂಕ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	ಆರಂಭ_lnum;
    linenr_T	ಕೊನೆ_lnum;
    ಪೂರ್ಣಾಂಕ		white_ಒಳ_front;
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_is_white;
    ಪೂರ್ಣಾಂಕ		prev_ಆರಂಭ_is_white;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಪೂರ್ಣಾಂಕ		do_white = FALSE;
    ಪೂರ್ಣಾಂಕ		t;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (type == 'S')	    // not implemented yet
	ಹಿಂತಿರುಗಿ FAIL;

    ಆರಂಭ_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && ಆರಂಭ_lnum != VIsual.lnum)
    {
extend:
	ಆದರೆ (ಆರಂಭ_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ಇಲ್ಲದಿದ್ದರೆ
	    dir = FORWARD;
	ಗಾಗಿ (i = ಎಣಿಕೆ_2; --i >= 0; )
	{
	    ಆದರೆ (ಆರಂಭ_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
	    {
		retval = FAIL;
		ನಿಲ್ಲಿಸು;
	    }

	    prev_ಆರಂಭ_is_white = -1;
	    ಗಾಗಿ (t = 0; t < 2; ++t)
	    {
		ಆರಂಭ_lnum += dir;
		ಆರಂಭ_is_white = linewhite(ಆರಂಭ_lnum);
		ಆದರೆ (prev_ಆರಂಭ_is_white == ಆರಂಭ_is_white)
		{
		    ಆರಂಭ_lnum -= dir;
		    ನಿಲ್ಲಿಸು;
		}
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (ಆರಂಭ_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (ಆರಂಭ_is_white != linewhite(ಆರಂಭ_lnum + dir)
			    || (!ಆರಂಭ_is_white
				    && ಆರಂಭ_PS(ಆರಂಭ_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ನಿಲ್ಲಿಸು;
		    ಆರಂಭ_lnum += dir;
		}
		ಆದರೆ (!include)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (ಆರಂಭ_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
		    ನಿಲ್ಲಿಸು;
		prev_ಆರಂಭ_is_white = ಆರಂಭ_is_white;
	    }
	}
	curwin->w_cursor.lnum = ಆರಂಭ_lnum;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	ಹಿಂತಿರುಗಿ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_ಒಳ_front = linewhite(ಆರಂಭ_lnum);
    ಆಗ (ಆರಂಭ_lnum > 1)
    {
	ಆದರೆ (white_ಒಳ_front)	    // stop at first white line
	{
	    ಆದರೆ (!linewhite(ಆರಂಭ_lnum - 1))
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ		// stop at first non-white line of start of paragraph
	{
	    ಆದರೆ (linewhite(ಆರಂಭ_lnum - 1) || ಆರಂಭ_PS(ಆರಂಭ_lnum, 0, 0))
		ನಿಲ್ಲಿಸು;
	}
	--ಆರಂಭ_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    ಕೊನೆ_lnum = ಆರಂಭ_lnum;
    ಆಗ (ಕೊನೆ_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && linewhite(ಕೊನೆ_lnum))
	++ಕೊನೆ_lnum;

    --ಕೊನೆ_lnum;
    i = ಎಣಿಕೆ_2;
    ಆದರೆ (!include && white_ಒಳ_front)
	--i;
    ಆಗ (i--)
    {
	ಆದರೆ (ಕೊನೆ_lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (!include)
	    do_white = linewhite(ಕೊನೆ_lnum + 1);

	ಆದರೆ (include || !do_white)
	{
	    ++ಕೊನೆ_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ಆಗ (ಕೊನೆ_lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    && !linewhite(ಕೊನೆ_lnum + 1)
		    && !ಆರಂಭ_PS(ಕೊನೆ_lnum + 1, 0, 0))
		++ಕೊನೆ_lnum;
	}

	ಆದರೆ (i == 0 && white_ಒಳ_front && include)
	    ನಿಲ್ಲಿಸು;

	/*
	 * skip to end of white lines after paragraph
	 */
	ಆದರೆ (include || do_white)
	    ಆಗ (ಕೊನೆ_lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						   && linewhite(ಕೊನೆ_lnum + 1))
		++ಕೊನೆ_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ಆದರೆ (!white_ಒಳ_front && !linewhite(ಕೊನೆ_lnum) && include)
	ಆಗ (ಆರಂಭ_lnum > 1 && linewhite(ಆರಂಭ_lnum - 1))
	    --ಆರಂಭ_lnum;

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ಆದರೆ (VIsual_ಬಹುಲಕ == 'V' && ಆರಂಭ_lnum == curwin->w_cursor.lnum)
	    ಹೋಗು extend;
	ಆದರೆ (VIsual.lnum != ಆರಂಭ_lnum)
	{
	    VIsual.lnum = ಆರಂಭ_lnum;
	    VIsual.ಲಂಬಸಾಲು = 0;
	}
	VIsual_ಬಹುಲಕ = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	oap->ಆರಂಭ.lnum = ಆರಂಭ_lnum;
	oap->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = ಕೊನೆ_lnum;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ಪೂರ್ಣಾಂಕ		c;

    ಗಾಗಿ (;;)
    {
	c = ಸಾಲು[ಲಂಬಸಾಲು];
	ಆದರೆ (c == NUL)
	    ಹಿಂತಿರುಗಿ -1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (escape != ಖಾಲಿ && vim_strchr(escape, c))
	{
	    ++ಲಂಬಸಾಲು;
	    ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು] == NUL)
		ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == quotechar)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (has_mbyte)
	    ಲಂಬಸಾಲು += (*mb_ptr2len)(ಸಾಲು + ಲಂಬಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++ಲಂಬಸಾಲು;
    }
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_prev_quote(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ಪೂರ್ಣಾಂಕ		n;

    ಆಗ (ಲಂಬಸಾಲು_ಆರಂಭ > 0)
    {
	--ಲಂಬಸಾಲು_ಆರಂಭ;
	ಲಂಬಸಾಲು_ಆರಂಭ -= (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + ಲಂಬಸಾಲು_ಆರಂಭ);
	n = 0;
	ಆದರೆ (escape != ಖಾಲಿ)
	    ಆಗ (ಲಂಬಸಾಲು_ಆರಂಭ - n > 0 && vim_strchr(escape,
					     ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ - n - 1]) != ಖಾಲಿ)
	    ++n;
	ಆದರೆ (n & 1)
	    ಲಂಬಸಾಲು_ಆರಂಭ -= n;	// uneven number of escape chars, skip it
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ] == quotechar)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು_ಆರಂಭ;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ಪೂರ್ಣಾಂಕ
current_quote(
    oparg_T	*oap,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		include,	// TRUE == include quote char
    ಪೂರ್ಣಾಂಕ		quotechar)	// Quote character
{
    char_u	*ಸಾಲು = ml_get_curline();
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಆರಂಭ = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		inclusive = FALSE;
    ಪೂರ್ಣಾಂಕ		vis_ಖಾಲಿಗಳು = TRUE;	// Visual selection <= 1 char
    ಪೂರ್ಣಾಂಕ		vis_bef_curs = FALSE;	// Visual starts before cursor
    ಪೂರ್ಣಾಂಕ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ಪೂರ್ಣಾಂಕ		inside_quotes = FALSE;	// Looks like "i'" done before
    ಪೂರ್ಣಾಂಕ		selected_quote = FALSE;	// Has quote inside selection
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// this only works within one line
	ಆದರೆ (VIsual.lnum != curwin->w_cursor.lnum)
	    ಹಿಂತಿರುಗಿ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_ಖಾಲಿಗಳು = EQUAL_POS(VIsual, curwin->w_cursor);
	ಆದರೆ (*p_sel == 'e')
	{
	    ಆದರೆ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vis_ಖಾಲಿಗಳು)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_ಖಾಲಿಗಳು = EQUAL_POS(VIsual, curwin->w_cursor);
	    ಆದರೆ (!vis_bef_curs && !vis_ಖಾಲಿಗಳು)
	    {
		// VIsual needs to be the start of Visual selection.
		pos_T t = curwin->w_cursor;

		curwin->w_cursor = VIsual;
		VIsual = t;
		vis_bef_curs = TRUE;
		restore_vis_bef = TRUE;
	    }
	}
    }

    ಆದರೆ (!vis_ಖಾಲಿಗಳು)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ಆದರೆ (vis_bef_curs)
	{
	    inside_quotes = VIsual.ಲಂಬಸಾಲು > 0
			&& ಸಾಲು[VIsual.ಲಂಬಸಾಲು - 1] == quotechar
			&& ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು] != NUL
			&& ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು + 1] == quotechar;
	    i = VIsual.ಲಂಬಸಾಲು;
	    ಲಂಬಸಾಲು_ಕೊನೆ = curwin->w_cursor.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    inside_quotes = curwin->w_cursor.ಲಂಬಸಾಲು > 0
			&& ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು - 1] == quotechar
			&& ಸಾಲು[VIsual.ಲಂಬಸಾಲು] != NUL
			&& ಸಾಲು[VIsual.ಲಂಬಸಾಲು + 1] == quotechar;
	    i = curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಲಂಬಸಾಲು_ಕೊನೆ = VIsual.ಲಂಬಸಾಲು;
	}

	// Find out if we have a quote in the selection.
	ಆಗ (i <= ಲಂಬಸಾಲು_ಕೊನೆ)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ಆದರೆ (ಸಾಲು[i] == NUL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ಸಾಲು[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (!vis_ಖಾಲಿಗಳು && ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ಆದರೆ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    ಲಂಬಸಾಲು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ + 1, quotechar, ಖಾಲಿ);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಆರಂಭ < 0)
		ಹೋಗು abort_ಹುಡುಕು;
	    ಲಂಬಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ + 1, quotechar,
							      curbuf->b_p_qe);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಕೊನೆ < 0)
	    {
		// We were on a starting quote perhaps?
		ಲಂಬಸಾಲು_ಕೊನೆ = ಲಂಬಸಾಲು_ಆರಂಭ;
		ಲಂಬಸಾಲು_ಆರಂಭ = curwin->w_cursor.ಲಂಬಸಾಲು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಲಂಬಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_prev_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, ಖಾಲಿ);
	    ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು_ಕೊನೆ] != quotechar)
		ಹೋಗು abort_ಹುಡುಕು;
	    ಲಂಬಸಾಲು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_prev_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಕೊನೆ, quotechar,
							      curbuf->b_p_qe);
	    ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ] != quotechar)
	    {
		// We were on an ending quote perhaps?
		ಲಂಬಸಾಲು_ಆರಂಭ = ಲಂಬಸಾಲು_ಕೊನೆ;
		ಲಂಬಸಾಲು_ಕೊನೆ = curwin->w_cursor.ಲಂಬಸಾಲು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ

    ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ] == quotechar || !vis_ಖಾಲಿಗಳು)
    {
	ಪೂರ್ಣಾಂಕ	ಮೊದಲ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;

	ಆದರೆ (!vis_ಖಾಲಿಗಳು)
	{
	    ಆದರೆ (vis_bef_curs)
		ಮೊದಲ_ಲಂಬಸಾಲು = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಮೊದಲ_ಲಂಬಸಾಲು = ಕಂಡುಹಿಡಿ_prev_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, ಖಾಲಿ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	ಲಂಬಸಾಲು_ಆರಂಭ = 0;
	ಗಾಗಿ (;;)
	{
	    // Find open quote character.
	    ಲಂಬಸಾಲು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, ಖಾಲಿ);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಆರಂಭ < 0 || ಲಂಬಸಾಲು_ಆರಂಭ > ಮೊದಲ_ಲಂಬಸಾಲು)
		ಹೋಗು abort_ಹುಡುಕು;
	    // Find close quote character.
	    ಲಂಬಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ + 1, quotechar,
							      curbuf->b_p_qe);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಕೊನೆ < 0)
		ಹೋಗು abort_ಹುಡುಕು;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ಆದರೆ (ಲಂಬಸಾಲು_ಆರಂಭ <= ಮೊದಲ_ಲಂಬಸಾಲು && ಮೊದಲ_ಲಂಬಸಾಲು <= ಲಂಬಸಾಲು_ಕೊನೆ)
		ನಿಲ್ಲಿಸು;
	    ಲಂಬಸಾಲು_ಆರಂಭ = ಲಂಬಸಾಲು_ಕೊನೆ + 1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Search backward for a starting quote.
	ಲಂಬಸಾಲು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_prev_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, curbuf->b_p_qe);
	ಆದರೆ (ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    ಲಂಬಸಾಲು_ಆರಂಭ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ, quotechar, ಖಾಲಿ);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಆರಂಭ < 0)
		ಹೋಗು abort_ಹುಡುಕು;
	}

	// Find close quote character.
	ಲಂಬಸಾಲು_ಕೊನೆ = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_quote(ಸಾಲು, ಲಂಬಸಾಲು_ಆರಂಭ + 1, quotechar,
							      curbuf->b_p_qe);
	ಆದರೆ (ಲಂಬಸಾಲು_ಕೊನೆ < 0)
	    ಹೋಗು abort_ಹುಡುಕು;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ಆದರೆ (include)
    {
	ಆದರೆ (VIM_ISWHITE(ಸಾಲು[ಲಂಬಸಾಲು_ಕೊನೆ + 1]))
	    ಆಗ (VIM_ISWHITE(ಸಾಲು[ಲಂಬಸಾಲು_ಕೊನೆ + 1]))
		++ಲಂಬಸಾಲು_ಕೊನೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆಗ (ಲಂಬಸಾಲು_ಆರಂಭ > 0 && VIM_ISWHITE(ಸಾಲು[ಲಂಬಸಾಲು_ಆರಂಭ - 1]))
		--ಲಂಬಸಾಲು_ಆರಂಭ;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ಆದರೆ (!include && ಎಣಿಕೆ_2 < 2 && (vis_ಖಾಲಿಗಳು || !inside_quotes))
	++ಲಂಬಸಾಲು_ಆರಂಭ;
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ಆದರೆ (vis_ಖಾಲಿಗಳು
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (ಸಾಲು[VIsual.ಲಂಬಸಾಲು] != quotechar
			    && (VIsual.ಲಂಬಸಾಲು == 0
				|| ಸಾಲು[VIsual.ಲಂಬಸಾಲು - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	oap->ಆರಂಭ = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಕೊನೆ;
    ಆದರೆ ((include || ಎಣಿಕೆ_2 > 1 // After vi" another i" must include the ".
		|| (!vis_ಖಾಲಿಗಳು && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (vis_ಖಾಲಿಗಳು || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ಆದರೆ (*p_sel != 'e')
		dec_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ಆದರೆ (inside_quotes
		    || (!selected_quote
			&& ಸಾಲು[VIsual.ಲಂಬಸಾಲು] != quotechar
			&& (ಸಾಲು[VIsual.ಲಂಬಸಾಲು] == NUL
			    || ಸಾಲು[VIsual.ಲಂಬಸಾಲು + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಆರಂಭ;
	}
	ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')
	{
	    VIsual_ಬಹುಲಕ = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ಹಿಂತಿರುಗಿ OK;

abort_ಹುಡುಕು:
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && *p_sel == 'e')
    {
	ಆದರೆ (did_exclusive_adj)
	    inc_cursor();
	ಆದರೆ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
