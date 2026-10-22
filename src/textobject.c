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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cls(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ skip_chars(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ಪೂರ್ಣಾಙ್ಕ
findsent(ಪೂರ್ಣಾಙ್ಕ dir, long ಎಣಿಕೆ)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		(*func)(pos_T *);
    ಪೂರ್ಣಾಙ್ಕ		startlnum;
    ಪೂರ್ಣಾಙ್ಕ		noskip = FALSE;	    // do not skip blanks
    ಪೂರ್ಣಾಙ್ಕ		cpo_J;
    ಪೂರ್ಣಾಙ್ಕ		found_dot;

    pos = curwin->w_cursor;
    ಯದಿ (dir == FORWARD)
	func = incl;
    ಅನ್ಯಥಾ
	func = decl;

    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ಯದಿ (gchar_pos(&pos) == NUL)
	{
	    ಕುರು
		ಯದಿ ((*func)(&pos) == -1)
		    ಭಙ್ಗ;
	    ಯಾವತ್ (gchar_pos(&pos) == NUL);
	    ಯದಿ (dir == FORWARD)
		ಗಚ್ಛ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ಅನ್ಯಥಾ ಯದಿ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ಯದಿ (pos.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ++pos.lnum;
	    ಗಚ್ಛ found;
	}
	ಅನ್ಯಥಾ ಯದಿ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ಯಾವತ್ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ರಿಕ್ತ)
	{
	    tpos = pos;
	    ಯದಿ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ಭಙ್ಗ;

	    ಯದಿ (found_dot)
		ಭಙ್ಗ;
	    ಯದಿ (vim_strchr((char_u *) ".!?", c) != ರಿಕ್ತ)
		found_dot = TRUE;

	    ಯದಿ (vim_strchr((char_u *) ")]\"'", c) != ರಿಕ್ತ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ರಿಕ್ತ;

	ಪ್ರತಿ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ಯದಿ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ಯದಿ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		ಕುರು
		    ಯದಿ ((c = inc(&tpos)) == -1)
			ಭಙ್ಗ;
		ಯಾವತ್ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ರಿಕ್ತ);
		ಯದಿ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ಯದಿ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ಭಙ್ಗ;
		}
	    }
	    ಯದಿ ((*func)(&pos) == -1)
	    {
		ಯದಿ (ಎಣಿಕೆ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		noskip = TRUE;
		ಭಙ್ಗ;
	    }
	}
found:
	    // skip white space
	ಯಾವತ್ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ಯದಿ (incl(&pos) == -1)
		ಭಙ್ಗ;

	ಯದಿ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ಯದಿ ((*func)(&pos) == -1)
	    {
		ಯದಿ (ಎಣಿಕೆ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಭಙ್ಗ;
	    }
	    ++ಎಣಿಕೆ;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ಪೂರ್ಣಾಙ್ಕ
findpar(
    ಪೂರ್ಣಾಙ್ಕ		*pincl,	    // Return: TRUE if last char is to be included
    ಪೂರ್ಣಾಙ್ಕ		dir,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		what,
    ಪೂರ್ಣಾಙ್ಕ		both)
{
    linenr_T	curr;
    ಪೂರ್ಣಾಙ್ಕ		did_skip;   // TRUE after separating lines have been skipped
    ಪೂರ್ಣಾಙ್ಕ		first;	    // TRUE on first line
    ಪೂರ್ಣಾಙ್ಕ		posix = (vim_strchr(p_cpo, CPO_PARA) != ರಿಕ್ತ);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ಪೂರ್ಣಾಙ್ಕ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	did_skip = FALSE;
	ಪ್ರತಿ (first = TRUE; ; first = FALSE)
	{
	    ಯದಿ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ಯದಿ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ಯದಿ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		ಭಙ್ಗ;

#ifdef FEAT_FOLDING
	    ಯದಿ (fold_skipped)
		curr -= dir;
#endif
	    ಯದಿ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಯದಿ (ಎಣಿಕೆ)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		curr -= dir;
		ಭಙ್ಗ;
	    }
	}
    }
    setpcmark();
    ಯದಿ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ಯದಿ (curr == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && what != '}' && dir == FORWARD)
    {
	char_u *ಸಾಲು = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ಯದಿ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(ಸಾಲು, ಸಾಲು + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ಅನ್ಯಥಾ
	curwin->w_cursor.col = 0;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ಪ್ರತಿ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ಯದಿ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    ಭಙ್ಗ;
	++macro;
	ಯದಿ (macro[0] == NUL)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ಪೂರ್ಣಾಙ್ಕ
startPS(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ para, ಪೂರ್ಣಾಙ್ಕ both)
{
    char_u	*s;

    s = ml_get(lnum);
    ಯದಿ (*s == para || *s == '\f' || (both && *s == '}'))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cls(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    c;

    c = gchar_cursor();
    ಯದಿ (c == ' ' || c == '\t' || c == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ಯದಿ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ಪ್ರತ್ಯಾಗಮ 1;

	// process code leading/trailing bytes
	ಪ್ರತ್ಯಾಗಮ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ಯದಿ (enc_utf8)
    {
	c = utf_class(c);
	ಯದಿ (c != 0 && cls_bigword)
	    ಪ್ರತ್ಯಾಗಮ 1;
	ಪ್ರತ್ಯಾಗಮ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ಯದಿ (cls_bigword)
	ಪ್ರತ್ಯಾಗಮ 1;

    ಯದಿ (vim_iswordc(c))
	ಪ್ರತ್ಯಾಗಮ 2;
    ಪ್ರತ್ಯಾಗಮ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ಪೂರ್ಣಾಙ್ಕ
fwd_word(
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		bigword,    // "W", "E" or "B"
    ಪೂರ್ಣಾಙ್ಕ		eol)
{
    ಪೂರ್ಣಾಙ್ಕ		sclass;	    // starting class
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		last_ಸಾಲು;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ಯದಿ (hasFolding(curwin->w_cursor.lnum, ರಿಕ್ತ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_ಸಾಲು = (curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	i = inc_cursor();
	ಯದಿ (i == -1 || (i >= 1 && last_ಸಾಲು)) // started at last char in file
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (i >= 1 && eol && ಎಣಿಕೆ == 0)      // started at last char in line
	    ಪ್ರತ್ಯಾಗಮ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ಯದಿ (sclass != 0)
	    ಯಾವತ್ (cls() == sclass)
	    {
		i = inc_cursor();
		ಯದಿ (i == -1 || (i >= 1 && eol && ಎಣಿಕೆ == 0))
		    ಪ್ರತ್ಯಾಗಮ OK;
	    }

	/*
	 * go to next non-white
	 */
	ಯಾವತ್ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ಯದಿ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		ಭಙ್ಗ;

	    i = inc_cursor();
	    ಯದಿ (i == -1 || (i >= 1 && eol && ಎಣಿಕೆ == 0))
		ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ಪೂರ್ಣಾಙ್ಕ
bck_word(long ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ bigword, ಪೂರ್ಣಾಙ್ಕ stop)
{
    ಪೂರ್ಣಾಙ್ಕ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ಯದಿ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ರಿಕ್ತ))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ಯದಿ (dec_cursor() == -1)		// started at start of file
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ಯಾವತ್ (cls() == 0)
	    {
		ಯದಿ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    ಗಚ್ಛ finished;
		ಯದಿ (dec_cursor() == -1) // hit start of file, stop here
		    ಪ್ರತ್ಯಾಗಮ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ಯದಿ (skip_chars(cls(), BACKWARD))
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ
end_word(
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		bigword,
    ಪೂರ್ಣಾಙ್ಕ		stop,
    ಪೂರ್ಣಾಙ್ಕ		empty)
{
    ಪೂರ್ಣಾಙ್ಕ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ಯದಿ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ಯದಿ (hasFolding(curwin->w_cursor.lnum, ರಿಕ್ತ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ಯದಿ (inc_cursor() == -1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ಯದಿ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ಯದಿ (skip_chars(sclass, FORWARD))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ಯಾವತ್ (cls() == 0)
	    {
		ಯದಿ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    ಗಚ್ಛ finished;
		ಯದಿ (inc_cursor() == -1)	    // hit end of file, stop here
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ಯದಿ (skip_chars(cls(), FORWARD))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ಪೂರ್ಣಾಙ್ಕ
bckend_word(
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		bigword,    // TRUE for "B"
    ಪೂರ್ಣಾಙ್ಕ		eol)	    // TRUE: stop at end of line.
{
    ಪೂರ್ಣಾಙ್ಕ		sclass;	    // starting class
    ಪೂರ್ಣಾಙ್ಕ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ಯಾವತ್ (--ಎಣಿಕೆ >= 0)
    {
	sclass = cls();
	ಯದಿ ((i = dec_cursor()) == -1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (eol && i == 1)
	    ಪ್ರತ್ಯಾಗಮ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ಯದಿ (sclass != 0)
	{
	    ಯಾವತ್ (cls() == sclass)
		ಯದಿ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ಪ್ರತ್ಯಾಗಮ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ಯಾವತ್ (cls() == 0)
	{
	    ಯದಿ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ಭಙ್ಗ;
	    ಯದಿ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    adjust_skipcol();
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_chars(ಪೂರ್ಣಾಙ್ಕ cclass, ಪೂರ್ಣಾಙ್ಕ dir)
{
    ಯಾವತ್ (cls() == cclass)
	ಯದಿ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
back_in_ಸಾಲು(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		sclass;		    // starting class

    sclass = cls();
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (curwin->w_cursor.col == 0)	    // stop at start of line
	    ಭಙ್ಗ;
	dec_cursor();
	ಯದಿ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ಭಙ್ಗ;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_first_blank(pos_T *posp)
{
    ಪೂರ್ಣಾಙ್ಕ	    c;

    ಯಾವತ್ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ಯದಿ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ಭಙ್ಗ;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findsent_forward(
    long    ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ	    at_start_sent)	// cursor is at start of sentence
{
    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	findsent(FORWARD, 1L);
	ಯದಿ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ಯದಿ (ಎಣಿಕೆ == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_word(
    oparg_T	*oap,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		include,	// TRUE: include word and white space
    ಪೂರ್ಣಾಙ್ಕ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		inclusive = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ಯದಿ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ಯದಿ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_ಸಾಲು();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ಯದಿ ((cls() == 0) == include)
	{
	    ಯದಿ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ಯದಿ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ಅನ್ಯಥಾ
		oneleft();

	    ಯದಿ (include)
		include_white = TRUE;
	}

	ಯದಿ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ಅನ್ಯಥಾ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--ಎಣಿಕೆ;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ಯಾವತ್ (ಎಣಿಕೆ > 0)
    {
	inclusive = TRUE;
	ಯದಿ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ಯದಿ (decl(&curwin->w_cursor) == -1)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (include != (cls() != 0))
	    {
		ಯದಿ (bck_word(1L, bigword, TRUE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		(ಶೂನ್ಯ)incl(&curwin->w_cursor);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ಯದಿ (incl(&curwin->w_cursor) == -1)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (include != (cls() == 0))
	    {
		ಯದಿ (fwd_word(1L, bigword, TRUE) == FAIL && ಎಣಿಕೆ > 1)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ಯದಿ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	--ಎಣಿಕೆ;
    }

    ಯದಿ (include_white && (cls() != 0
		 || (curwin->w_cursor.col == 0 && !inclusive)))
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
	curwin->w_cursor = start_pos;
	ಯದಿ (oneleft() == OK)
	{
	    back_in_ಸಾಲು();
	    ಯದಿ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ಯದಿ (VIsual_active)
		    VIsual = curwin->w_cursor;
		ಅನ್ಯಥಾ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ಯದಿ (VIsual_active)
    {
	ಯದಿ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ಯದಿ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ಅನ್ಯಥಾ
	oap->inclusive = inclusive;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_sent(oparg_T *oap, long ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ include)
{
    pos_T	start_pos;
    pos_T	pos;
    ಪೂರ್ಣಾಙ್ಕ		start_blank;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ಯದಿ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ಯದಿ (LT_POS(start_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_start_sent = TRUE;
	    decl(&pos);
	    ಯಾವತ್ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ಯದಿ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    ಭಙ್ಗ;
		}
		incl(&pos);
	    }
	    ಯದಿ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ಯದಿ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ಅನ್ಯಥಾ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ಯದಿ (include)	// "as" gets twice as much as "is"
		ಎಣಿಕೆ *= 2;
	    ಯಾವತ್ (ಎಣಿಕೆ--)
	    {
		ಯದಿ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ಯದಿ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_start_sent = TRUE;
	    // not just before a sentence
	    ಯದಿ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ಯಾವತ್ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ಯದಿ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			ಭಙ್ಗ;
		    }
		    incl(&pos);
		}
		ಯದಿ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ಅನ್ಯಥಾ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ಯದಿ (include)	// "as" gets twice as much as "is"
		ಎಣಿಕೆ *= 2;
	    findsent_forward(ಎಣಿಕೆ, at_start_sent);
	    ಯದಿ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ಯಾವತ್ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ಯದಿ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ಅನ್ಯಥಾ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ಯದಿ (include)
	ncount = ಎಣಿಕೆ * 2;
    ಅನ್ಯಥಾ
    {
	ncount = ಎಣಿಕೆ;
	ಯದಿ (start_blank)
	    --ncount;
    }
    ಯದಿ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ಅನ್ಯಥಾ
	decl(&curwin->w_cursor);

    ಯದಿ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ಯದಿ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ಯದಿ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ಅನ್ಯಥಾ ಯದಿ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ಯದಿ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ಯದಿ (EQUAL_POS(start_pos, curwin->w_cursor))
	    ಗಚ್ಛ extend;
	ಯದಿ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ಅನ್ಯಥಾ
    {
	// include a newline after the sentence, if there is one
	ಯದಿ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ಅನ್ಯಥಾ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_block(
    oparg_T	*oap,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		include,	// TRUE == include white space
    ಪೂರ್ಣಾಙ್ಕ		what,		// '(', '{', etc.
    ಪೂರ್ಣಾಙ್ಕ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ರಿಕ್ತ;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ಪೂರ್ಣಾಙ್ಕ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ಯದಿ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ಯದಿ (what == '{')		// ignore indent
	    ಯಾವತ್ (inindent(1))
		ಯದಿ (inc_cursor() != 0)
		    ಭಙ್ಗ;
	ಯದಿ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ಅನ್ಯಥಾ ಯದಿ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ಅನ್ಯಥಾ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ರಿಕ್ತ ? "%M" : "%");
    ಯದಿ ((pos = findmatch(ರಿಕ್ತ, what)) != ರಿಕ್ತ)
    {
	ಯಾವತ್ (ಎಣಿಕೆ-- > 0)
	{
	    ಯದಿ ((pos = findmatch(ರಿಕ್ತ, what)) == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (ಎಣಿಕೆ-- > 0)
	{
	    ಯದಿ ((pos = findmatchlimit(ರಿಕ್ತ, what, FM_FORWARD, 0)) == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ಯದಿ (pos == ರಿಕ್ತ || (end_pos = findmatch(ರಿಕ್ತ, other)) == ರಿಕ್ತ)
    {
	curwin->w_cursor = old_pos;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ಯಾವತ್ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ಯಾವತ್ (inindent(1))
	{
	    sol = TRUE;
	    ಯದಿ (decl(&curwin->w_cursor) != 0)
		ಭಙ್ಗ;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ಯದಿ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ಯದಿ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ಯದಿ ((pos = findmatch(ರಿಕ್ತ, what)) == ರಿಕ್ತ)
	    {
		curwin->w_cursor = old_pos;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ಯದಿ ((end_pos = findmatch(ರಿಕ್ತ, other)) == ರಿಕ್ತ)
	    {
		curwin->w_cursor = old_pos;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಯದಿ (VIsual_active)
    {
	ಯದಿ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ಯದಿ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಅನ್ಯಥಾ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ಯದಿ (sol)
	    incl(&curwin->w_cursor);
	ಅನ್ಯಥಾ ಯದಿ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ಅನ್ಯಥಾ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
in_html_tag(
    ಪೂರ್ಣಾಙ್ಕ		end_tag)
{
    char_u	*ಸಾಲು = ml_get_curline();
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		lc = NUL;
    pos_T	pos;

    ಯದಿ (enc_dbcs)
    {
	char_u	*lp = ರಿಕ್ತ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ಪ್ರತಿ (p = ಸಾಲು; p < ಸಾಲು + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ಯದಿ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ಯದಿ (*p != '<')	    // check for '<' under cursor
	{
	    ಯದಿ (lc != '<')
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    p = lp;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (p = ಸಾಲು + curwin->w_cursor.col; p > ಸಾಲು; )
	{
	    ಯದಿ (*p == '<')	// find '<' under/before cursor
		ಭಙ್ಗ;
	    MB_PTR_BACK(ಸಾಲು, p);
	    ಯದಿ (*p == '>')	// find '>' before cursor
		ಭಙ್ಗ;
	}
	ಯದಿ (*p != '<')
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - ಸಾಲು);

    MB_PTR_ADV(p);
    ಯದಿ (end_tag)
	// check that there is a '/' after the '<'
	ಪ್ರತ್ಯಾಗಮ *p == '/';

    // check that there is no '/' after the '<'
    ಯದಿ (*p == '/')
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // check that the matching '>' is not preceded by '/'
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (inc(&pos) < 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	c = *ml_get_pos(&pos);
	ಯದಿ (c == '>')
	    ಭಙ್ಗ;
	lc = c;
    }
    ಪ್ರತ್ಯಾಗಮ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_tagblock(
    oparg_T	*oap,
    long	ಎಣಿಕೆ_arg,
    ಪೂರ್ಣಾಙ್ಕ		include)	// TRUE == include white space
{
    long	ಎಣಿಕೆ = ಎಣಿಕೆ_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		do_include = include;
    ಪೂರ್ಣಾಙ್ಕ		save_p_ws = p_ws;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ಯದಿ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ಯದಿ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ಯಾವತ್ (inindent(1))
	    ಯದಿ (inc_cursor() != 0)
		ಭಙ್ಗ;

	ಯದಿ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ಯಾವತ್ (*ml_get_cursor() != '>')
		ಯದಿ (inc_cursor() < 0)
		    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ಯಾವತ್ (*ml_get_cursor() != '<')
		ಯದಿ (dec_cursor() < 0)
		    ಭಙ್ಗ;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ಅನ್ಯಥಾ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ಪ್ರತಿ (n = 0; n < ಎಣಿಕೆ; ++n)
    {
	ಯದಿ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ರಿಕ್ತ, 0,
						  ರಿಕ್ತ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    ಗಚ್ಛ theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    ಪ್ರತಿ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ಪೂರ್ಣಾಙ್ಕ)(cp - p);
    ಯದಿ (len == 0)
    {
	curwin->w_cursor = old_pos;
	ಗಚ್ಛ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ಯದಿ (spat == ರಿಕ್ತ || epat == ರಿಕ್ತ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	ಗಚ್ಛ theend;
    }
    sprintf((ವರ್ಣ *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((ವರ್ಣ *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ರಿಕ್ತ,
						    0, ರಿಕ್ತ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ಯದಿ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	ಎಣಿಕೆ = 1;
	curwin->w_cursor = start_pos;
	ಗಚ್ಛ again;
    }

    ಯದಿ (do_include)
    {
	// Include up to the '>'.
	ಯಾವತ್ (*ml_get_cursor() != '>')
	    ಯದಿ (inc_cursor() < 0)
		ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ಯದಿ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ಅನ್ಯಥಾ ಯದಿ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ಯದಿ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ಪೂರ್ಣಾಙ್ಕ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ಯಾವತ್ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ಯದಿ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ಯದಿ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    ಎಣಿಕೆ = ಎಣಿಕೆ_arg;
	    ಗಚ್ಛ again;
	}
    }

    ಯದಿ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ಯದಿ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ಅನ್ಯಥಾ ಯದಿ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಅನ್ಯಥಾ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ಯದಿ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ಅನ್ಯಥಾ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
current_par(
    oparg_T	*oap,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		include,	// TRUE == include white space
    ಪೂರ್ಣಾಙ್ಕ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ಪೂರ್ಣಾಙ್ಕ		white_in_front;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		start_is_white;
    ಪೂರ್ಣಾಙ್ಕ		prev_start_is_white;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		do_white = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		t;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (type == 'S')	    // not implemented yet
	ಪ್ರತ್ಯಾಗಮ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ಯದಿ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ಯದಿ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ಅನ್ಯಥಾ
	    dir = FORWARD;
	ಪ್ರತಿ (i = ಎಣಿಕೆ; --i >= 0; )
	{
	    ಯದಿ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
	    {
		retval = FAIL;
		ಭಙ್ಗ;
	    }

	    prev_start_is_white = -1;
	    ಪ್ರತಿ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ಯದಿ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    ಭಙ್ಗ;
		}
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
			ಭಙ್ಗ;
		    ಯದಿ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ಭಙ್ಗ;
		    start_lnum += dir;
		}
		ಯದಿ (!include)
		    ಭಙ್ಗ;
		ಯದಿ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ))
		    ಭಙ್ಗ;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ಪ್ರತ್ಯಾಗಮ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ಯಾವತ್ (start_lnum > 1)
    {
	ಯದಿ (white_in_front)	    // stop at first white line
	{
	    ಯದಿ (!linewhite(start_lnum - 1))
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ		// stop at first non-white line of start of paragraph
	{
	    ಯದಿ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		ಭಙ್ಗ;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ಯಾವತ್ (end_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = ಎಣಿಕೆ;
    ಯದಿ (!include && white_in_front)
	--i;
    ಯಾವತ್ (i--)
    {
	ಯದಿ (end_lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (!include)
	    do_white = linewhite(end_lnum + 1);

	ಯದಿ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ಯಾವತ್ (end_lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ಯದಿ (i == 0 && white_in_front && include)
	    ಭಙ್ಗ;

	/*
	 * skip to end of white lines after paragraph
	 */
	ಯದಿ (include || do_white)
	    ಯಾವತ್ (end_lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ಯದಿ (!white_in_front && !linewhite(end_lnum) && include)
	ಯಾವತ್ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ಯದಿ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ಯದಿ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    ಗಚ್ಛ extend;
	ಯದಿ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ಅನ್ಯಥಾ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_next_quote(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಪ್ರತಿ (;;)
    {
	c = ಸಾಲು[col];
	ಯದಿ (c == NUL)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಅನ್ಯಥಾ ಯದಿ (escape != ರಿಕ್ತ && vim_strchr(escape, c))
	{
	    ++col;
	    ಯದಿ (ಸಾಲು[col] == NUL)
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == quotechar)
	    ಭಙ್ಗ;
	ಯದಿ (has_mbyte)
	    col += (*mb_ptr2len)(ಸಾಲು + col);
	ಅನ್ಯಥಾ
	    ++col;
    }
    ಪ್ರತ್ಯಾಗಮ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_prev_quote(
    char_u	*ಸಾಲು,
    ಪೂರ್ಣಾಙ್ಕ		col_start,
    ಪೂರ್ಣಾಙ್ಕ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯಾವತ್ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(ಸಾಲು, ಸಾಲು + col_start);
	n = 0;
	ಯದಿ (escape != ರಿಕ್ತ)
	    ಯಾವತ್ (col_start - n > 0 && vim_strchr(escape,
					     ಸಾಲು[col_start - n - 1]) != ರಿಕ್ತ)
	    ++n;
	ಯದಿ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು[col_start] == quotechar)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
current_quote(
    oparg_T	*oap,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		include,	// TRUE == include quote char
    ಪೂರ್ಣಾಙ್ಕ		quotechar)	// Quote character
{
    char_u	*ಸಾಲು = ml_get_curline();
    ಪೂರ್ಣಾಙ್ಕ		col_end;
    ಪೂರ್ಣಾಙ್ಕ		col_start = curwin->w_cursor.col;
    ಪೂರ್ಣಾಙ್ಕ		inclusive = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		vis_empty = TRUE;	// Visual selection <= 1 char
    ಪೂರ್ಣಾಙ್ಕ		vis_bef_curs = FALSE;	// Visual starts before cursor
    ಪೂರ್ಣಾಙ್ಕ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ಪೂರ್ಣಾಙ್ಕ		inside_quotes = FALSE;	// Looks like "i'" done before
    ಪೂರ್ಣಾಙ್ಕ		selected_quote = FALSE;	// Has quote inside selection
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ಯದಿ (VIsual_active)
    {
	// this only works within one line
	ಯದಿ (VIsual.lnum != curwin->w_cursor.lnum)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ಯದಿ (*p_sel == 'e')
	{
	    ಯದಿ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ಯದಿ (!vis_bef_curs && !vis_empty)
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

    ಯದಿ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ಯದಿ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& ಸಾಲು[VIsual.col - 1] == quotechar
			&& ಸಾಲು[curwin->w_cursor.col] != NUL
			&& ಸಾಲು[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ಅನ್ಯಥಾ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& ಸಾಲು[curwin->w_cursor.col - 1] == quotechar
			&& ಸಾಲು[VIsual.col] != NUL
			&& ಸಾಲು[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ಯಾವತ್ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ಯದಿ (ಸಾಲು[i] == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (ಸಾಲು[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (!vis_empty && ಸಾಲು[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ಯದಿ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(ಸಾಲು, col_start + 1, quotechar, ರಿಕ್ತ);
	    ಯದಿ (col_start < 0)
		ಗಚ್ಛ abort_ಹುಡುಕು;
	    col_end = find_next_quote(ಸಾಲು, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ಯದಿ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    col_end = find_prev_quote(ಸಾಲು, col_start, quotechar, ರಿಕ್ತ);
	    ಯದಿ (ಸಾಲು[col_end] != quotechar)
		ಗಚ್ಛ abort_ಹುಡುಕು;
	    col_start = find_prev_quote(ಸಾಲು, col_end, quotechar,
							      curbuf->b_p_qe);
	    ಯದಿ (ಸಾಲು[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ಅನ್ಯಥಾ

    ಯದಿ (ಸಾಲು[col_start] == quotechar || !vis_empty)
    {
	ಪೂರ್ಣಾಙ್ಕ	first_col = col_start;

	ಯದಿ (!vis_empty)
	{
	    ಯದಿ (vis_bef_curs)
		first_col = find_next_quote(ಸಾಲು, col_start, quotechar, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
		first_col = find_prev_quote(ಸಾಲು, col_start, quotechar, ರಿಕ್ತ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	ಪ್ರತಿ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(ಸಾಲು, col_start, quotechar, ರಿಕ್ತ);
	    ಯದಿ (col_start < 0 || col_start > first_col)
		ಗಚ್ಛ abort_ಹುಡುಕು;
	    // Find close quote character.
	    col_end = find_next_quote(ಸಾಲು, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ಯದಿ (col_end < 0)
		ಗಚ್ಛ abort_ಹುಡುಕು;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ಯದಿ (col_start <= first_col && first_col <= col_end)
		ಭಙ್ಗ;
	    col_start = col_end + 1;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(ಸಾಲು, col_start, quotechar, curbuf->b_p_qe);
	ಯದಿ (ಸಾಲು[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(ಸಾಲು, col_start, quotechar, ರಿಕ್ತ);
	    ಯದಿ (col_start < 0)
		ಗಚ್ಛ abort_ಹುಡುಕು;
	}

	// Find close quote character.
	col_end = find_next_quote(ಸಾಲು, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ಯದಿ (col_end < 0)
	    ಗಚ್ಛ abort_ಹುಡುಕು;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ಯದಿ (include)
    {
	ಯದಿ (VIM_ISWHITE(ಸಾಲು[col_end + 1]))
	    ಯಾವತ್ (VIM_ISWHITE(ಸಾಲು[col_end + 1]))
		++col_end;
	ಅನ್ಯಥಾ
	    ಯಾವತ್ (col_start > 0 && VIM_ISWHITE(ಸಾಲು[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ಯದಿ (!include && ಎಣಿಕೆ < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ಯದಿ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ಯದಿ (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (ಸಾಲು[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| ಸಾಲು[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ಅನ್ಯಥಾ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ಯದಿ ((include || ಎಣಿಕೆ > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ಯದಿ (VIsual_active)
    {
	ಯದಿ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ಯದಿ (*p_sel != 'e')
		dec_cursor();
	}
	ಅನ್ಯಥಾ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ಯದಿ (inside_quotes
		    || (!selected_quote
			&& ಸಾಲು[VIsual.col] != quotechar
			&& (ಸಾಲು[VIsual.col] == NUL
			    || ಸಾಲು[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	ಯದಿ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ಅನ್ಯಥಾ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ಪ್ರತ್ಯಾಗಮ OK;

abort_ಹುಡುಕು:
    ಯದಿ (VIsual_active && *p_sel == 'e')
    {
	ಯದಿ (did_exclusive_adj)
	    inc_cursor();
	ಯದಿ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
