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
#include "vim.h"

static puur_naa_nka cls(void);
static puur_naa_nka skip_chars(puur_naa_nka, puur_naa_nka);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    puur_naa_nka
findsent(puur_naa_nka dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    puur_naa_nka		c;
    puur_naa_nka		(*func)(pos_T *);
    puur_naa_nka		startlnum;
    puur_naa_nka		noskip = FALSE;	    // do not skip blanks
    puur_naa_nka		cpo_J;
    puur_naa_nka		found_dot;

    pos = curwin->w_cursor;
    ᰀᰧ (dir == FORWARD)
	func = incl;
    ᰀᰧᰀ
	func = decl;

    ᰀᰬ (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ᰀᰧ (gchar_pos(&pos) == NUL)
	{
	    do
		ᰀᰧ ((*func)(&pos) == -1)
		    bha_nga;
	    ᰀᰬ (gchar_pos(&pos) == NUL);
	    ᰀᰧ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ᰀᰧᰀ ᰀᰧ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ᰀᰧ (pos.lnum == curbuf->b_ml.ml_line_count)
		ᰀᰧᰌ FAIL;
	    ++pos.lnum;
	    goto found;
	}
	ᰀᰧᰀ ᰀᰧ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ᰀᰬ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ᰀᰧ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    ᰀᰧ (found_dot)
		bha_nga;
	    ᰀᰧ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ᰀᰧ (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	for (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ᰀᰧ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ᰀᰧ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    ᰀᰧ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ᰀᰧ ((c = inc(&tpos)) == -1)
			bha_nga;
		ᰀᰬ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ᰀᰧ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ᰀᰧ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    ᰀᰧ ((*func)(&pos) == -1)
	    {
		ᰀᰧ (count)
		    ᰀᰧᰌ FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	ᰀᰬ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ᰀᰧ (incl(&pos) == -1)
		bha_nga;

	ᰀᰧ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ᰀᰧ ((*func)(&pos) == -1)
	    {
		ᰀᰧ (count)
		    ᰀᰧᰌ FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ᰀᰧᰌ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    puur_naa_nka
findpar(
    puur_naa_nka		*pincl,	    // Return: TRUE if last char is to be included
    puur_naa_nka		dir,
    long	count,
    puur_naa_nka		what,
    puur_naa_nka		both)
{
    linenr_T	curr;
    puur_naa_nka		did_skip;   // TRUE after separating lines have been skipped
    puur_naa_nka		first;	    // TRUE on first line
    puur_naa_nka		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    puur_naa_nka		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ᰀᰬ (count--)
    {
	did_skip = FALSE;
	for (first = TRUE; ; first = FALSE)
	{
	    ᰀᰧ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ᰀᰧ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ᰀᰧ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    ᰀᰧ (fold_skipped)
		curr -= dir;
#endif
	    ᰀᰧ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ᰀᰧ (count)
		    ᰀᰧᰌ FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    ᰀᰧ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ᰀᰧ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ᰀᰧ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ᰀᰧᰀ
	curwin->w_cursor.col = 0;
    ᰀᰧᰌ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static puur_naa_nka
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    for (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ᰀᰧ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	ᰀᰧ (macro[0] == NUL)
	    bha_nga;
    }
    ᰀᰧᰌ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    puur_naa_nka
startPS(linenr_T lnum, puur_naa_nka para, puur_naa_nka both)
{
    char_u	*s;

    s = ml_get(lnum);
    ᰀᰧ (*s == para || *s == '\f' || (both && *s == '}'))
	ᰀᰧᰌ TRUE;
    ᰀᰧ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ᰀᰧᰌ TRUE;
    ᰀᰧᰌ FALSE;
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

static puur_naa_nka	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static puur_naa_nka
cls(void)
{
    puur_naa_nka	    c;

    c = gchar_cursor();
    ᰀᰧ (c == ' ' || c == '\t' || c == NUL)
	ᰀᰧᰌ 0;
    ᰀᰧ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ᰀᰧ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ᰀᰧᰌ 1;

	// process code leading/trailing bytes
	ᰀᰧᰌ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ᰀᰧ (enc_utf8)
    {
	c = utf_class(c);
	ᰀᰧ (c != 0 && cls_bigword)
	    ᰀᰧᰌ 1;
	ᰀᰧᰌ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ᰀᰧ (cls_bigword)
	ᰀᰧᰌ 1;

    ᰀᰧ (vim_iswordc(c))
	ᰀᰧᰌ 2;
    ᰀᰧᰌ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    puur_naa_nka
fwd_word(
    long	count,
    puur_naa_nka		bigword,    // "W", "E" or "B"
    puur_naa_nka		eol)
{
    puur_naa_nka		sclass;	    // starting class
    puur_naa_nka		i;
    puur_naa_nka		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ᰀᰬ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ᰀᰧ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ᰀᰧ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ᰀᰧᰌ FAIL;
	ᰀᰧ (i >= 1 && eol && count == 0)      // started at last char in line
	    ᰀᰧᰌ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ᰀᰧ (sclass != 0)
	    ᰀᰬ (cls() == sclass)
	    {
		i = inc_cursor();
		ᰀᰧ (i == -1 || (i >= 1 && eol && count == 0))
		    ᰀᰧᰌ OK;
	    }

	/*
	 * go to next non-white
	 */
	ᰀᰬ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ᰀᰧ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    ᰀᰧ (i == -1 || (i >= 1 && eol && count == 0))
		ᰀᰧᰌ OK;
	}
    }
    ᰀᰧᰌ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    puur_naa_nka
bck_word(long count, puur_naa_nka bigword, puur_naa_nka stop)
{
    puur_naa_nka		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ᰀᰬ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ᰀᰧ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ᰀᰧ (dec_cursor() == -1)		// started at start of file
	    ᰀᰧᰌ FAIL;

	ᰀᰧ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ᰀᰬ (cls() == 0)
	    {
		ᰀᰧ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ᰀᰧ (dec_cursor() == -1) // hit start of file, stop here
		    ᰀᰧᰌ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ᰀᰧ (skip_chars(cls(), BACKWARD))
		ᰀᰧᰌ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ᰀᰧᰌ OK;
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
    puur_naa_nka
end_word(
    long	count,
    puur_naa_nka		bigword,
    puur_naa_nka		stop,
    puur_naa_nka		empty)
{
    puur_naa_nka		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ᰀᰧ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ᰀᰬ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ᰀᰧ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ᰀᰧ (inc_cursor() == -1)
	    ᰀᰧᰌ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ᰀᰧ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ᰀᰧ (skip_chars(sclass, FORWARD))
		ᰀᰧᰌ FAIL;
	}
	ᰀᰧᰀ ᰀᰧ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ᰀᰬ (cls() == 0)
	    {
		ᰀᰧ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ᰀᰧ (inc_cursor() == -1)	    // hit end of file, stop here
		    ᰀᰧᰌ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ᰀᰧ (skip_chars(cls(), FORWARD))
		ᰀᰧᰌ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ᰀᰧᰌ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    puur_naa_nka
bckend_word(
    long	count,
    puur_naa_nka		bigword,    // TRUE for "B"
    puur_naa_nka		eol)	    // TRUE: stop at end of line.
{
    puur_naa_nka		sclass;	    // starting class
    puur_naa_nka		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ᰀᰬ (--count >= 0)
    {
	sclass = cls();
	ᰀᰧ ((i = dec_cursor()) == -1)
	    ᰀᰧᰌ FAIL;
	ᰀᰧ (eol && i == 1)
	    ᰀᰧᰌ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ᰀᰧ (sclass != 0)
	{
	    ᰀᰬ (cls() == sclass)
		ᰀᰧ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ᰀᰧᰌ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ᰀᰬ (cls() == 0)
	{
	    ᰀᰧ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    ᰀᰧ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ᰀᰧᰌ OK;
	}
    }
    adjust_skipcol();
    ᰀᰧᰌ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static puur_naa_nka
skip_chars(puur_naa_nka cclass, puur_naa_nka dir)
{
    ᰀᰬ (cls() == cclass)
	ᰀᰧ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ᰀᰧᰌ TRUE;
    ᰀᰧᰌ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    puur_naa_nka		sclass;		    // starting class

    sclass = cls();
    for (;;)
    {
	ᰀᰧ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	ᰀᰧ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    puur_naa_nka	    c;

    ᰀᰬ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ᰀᰧ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    bha_nga;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    static void
findsent_forward(
    long    count,
    puur_naa_nka	    at_start_sent)	// cursor is at start of sentence
{
    ᰀᰬ (count--)
    {
	findsent(FORWARD, 1L);
	ᰀᰧ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ᰀᰧ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    puur_naa_nka
current_word(
    oparg_T	*oap,
    long	count,
    puur_naa_nka		include,	// TRUE: include word and white space
    puur_naa_nka		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    puur_naa_nka		inclusive = TRUE;
    puur_naa_nka		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ᰀᰧ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ᰀᰧ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_line();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ᰀᰧ ((cls() == 0) == include)
	{
	    ᰀᰧ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ᰀᰧᰌ FAIL;
	}
	ᰀᰧᰀ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ᰀᰧ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ᰀᰧᰀ
		oneleft();

	    ᰀᰧ (include)
		include_white = TRUE;
	}

	ᰀᰧ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ᰀᰧᰀ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ᰀᰬ (count > 0)
    {
	inclusive = TRUE;
	ᰀᰧ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ᰀᰧ (decl(&curwin->w_cursor) == -1)
		ᰀᰧᰌ FAIL;
	    ᰀᰧ (include != (cls() != 0))
	    {
		ᰀᰧ (bck_word(1L, bigword, TRUE) == FAIL)
		    ᰀᰧᰌ FAIL;
	    }
	    ᰀᰧᰀ
	    {
		ᰀᰧ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ᰀᰧᰌ FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	ᰀᰧᰀ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ᰀᰧ (incl(&curwin->w_cursor) == -1)
		ᰀᰧᰌ FAIL;
	    ᰀᰧ (include != (cls() == 0))
	    {
		ᰀᰧ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ᰀᰧᰌ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ᰀᰧ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ᰀᰧᰀ
	    {
		ᰀᰧ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ᰀᰧᰌ FAIL;
	    }
	}
	--count;
    }

    ᰀᰧ (include_white && (cls() != 0
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
	ᰀᰧ (oneleft() == OK)
	{
	    back_in_line();
	    ᰀᰧ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ᰀᰧ (VIsual_active)
		    VIsual = curwin->w_cursor;
		ᰀᰧᰀ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ᰀᰧ (VIsual_active)
    {
	ᰀᰧ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ᰀᰧ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ᰀᰧᰀ
	oap->inclusive = inclusive;

    ᰀᰧᰌ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    puur_naa_nka
current_sent(oparg_T *oap, long count, puur_naa_nka include)
{
    pos_T	start_pos;
    pos_T	pos;
    puur_naa_nka		start_blank;
    puur_naa_nka		c;
    puur_naa_nka		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ᰀᰧ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ᰀᰧ (LT_POS(start_pos, VIsual))
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
	    ᰀᰬ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ᰀᰧ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    ᰀᰧ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ᰀᰧ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ᰀᰧᰀ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ᰀᰧ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ᰀᰬ (count--)
	    {
		ᰀᰧ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ᰀᰧ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ᰀᰧᰀ
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
	    ᰀᰧ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ᰀᰬ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ᰀᰧ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		ᰀᰧ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ᰀᰧᰀ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ᰀᰧ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ᰀᰧ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ᰀᰧᰌ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ᰀᰬ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ᰀᰧ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ᰀᰧᰀ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ᰀᰧ (include)
	ncount = count * 2;
    ᰀᰧᰀ
    {
	ncount = count;
	ᰀᰧ (start_blank)
	    --ncount;
    }
    ᰀᰧ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ᰀᰧᰀ
	decl(&curwin->w_cursor);

    ᰀᰧ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ᰀᰧ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ᰀᰧ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ᰀᰧᰀ ᰀᰧ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ᰀᰧ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ᰀᰧ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ᰀᰧ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ᰀᰧᰀ
    {
	// include a newline after the sentence, if there is one
	ᰀᰧ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ᰀᰧᰀ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ᰀᰧᰌ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    puur_naa_nka
current_block(
    oparg_T	*oap,
    long	count,
    puur_naa_nka		include,	// TRUE == include white space
    puur_naa_nka		what,		// '(', '{', etc.
    puur_naa_nka		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    puur_naa_nka		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ᰀᰧ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ᰀᰧ (what == '{')		// ignore indent
	    ᰀᰬ (inindent(1))
		ᰀᰧ (inc_cursor() != 0)
		    bha_nga;
	ᰀᰧ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ᰀᰧᰀ ᰀᰧ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ᰀᰧᰀ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ᰀᰧ ((pos = findmatch(NULL, what)) != NULL)
    {
	ᰀᰬ (count-- > 0)
	{
	    ᰀᰧ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ᰀᰧᰀ
    {
	ᰀᰬ (count-- > 0)
	{
	    ᰀᰧ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ᰀᰧ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ᰀᰧᰌ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ᰀᰬ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ᰀᰬ (inindent(1))
	{
	    sol = TRUE;
	    ᰀᰧ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ᰀᰧ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ᰀᰧᰌ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ᰀᰧ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ᰀᰧ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ᰀᰧᰌ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ᰀᰧ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ᰀᰧᰌ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ᰀᰧᰀ
	    bha_nga;
    }

    ᰀᰧ (VIsual_active)
    {
	ᰀᰧ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ᰀᰧ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᰀᰧᰀ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ᰀᰧ (sol)
	    incl(&curwin->w_cursor);
	ᰀᰧᰀ ᰀᰧ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ᰀᰧᰀ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ᰀᰧᰌ OK;
}

#ᰀᰧ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static puur_naa_nka
in_html_tag(
    puur_naa_nka		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    puur_naa_nka		c;
    puur_naa_nka		lc = NUL;
    pos_T	pos;

    ᰀᰧ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	for (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ᰀᰧ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ᰀᰧ (*p != '<')	    // check for '<' under cursor
	{
	    ᰀᰧ (lc != '<')
		ᰀᰧᰌ FALSE;
	    p = lp;
	}
    }
    ᰀᰧᰀ
    {
	for (p = line + curwin->w_cursor.col; p > line; )
	{
	    ᰀᰧ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    ᰀᰧ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	ᰀᰧ (*p != '<')
	    ᰀᰧᰌ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ᰀᰧ (end_tag)
	// check that there is a '/' after the '<'
	ᰀᰧᰌ *p == '/';

    // check that there is no '/' after the '<'
    ᰀᰧ (*p == '/')
	ᰀᰧᰌ FALSE;

    // check that the matching '>' is not preceded by '/'
    for (;;)
    {
	ᰀᰧ (inc(&pos) < 0)
	    ᰀᰧᰌ FALSE;
	c = *ml_get_pos(&pos);
	ᰀᰧ (c == '>')
	    bha_nga;
	lc = c;
    }
    ᰀᰧᰌ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    puur_naa_nka
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    puur_naa_nka		include)	// TRUE == include white space
{
    long	count = count_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    puur_naa_nka		len;
    puur_naa_nka		r;
    puur_naa_nka		do_include = include;
    puur_naa_nka		save_p_ws = p_ws;
    puur_naa_nka		retval = FAIL;
    puur_naa_nka		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ᰀᰧ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ᰀᰧ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ᰀᰬ (inindent(1))
	    ᰀᰧ (inc_cursor() != 0)
		bha_nga;

	ᰀᰧ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ᰀᰬ (*ml_get_cursor() != '>')
		ᰀᰧ (inc_cursor() < 0)
		    bha_nga;
	}
	ᰀᰧᰀ ᰀᰧ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ᰀᰬ (*ml_get_cursor() != '<')
		ᰀᰧ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ᰀᰧᰀ ᰀᰧ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ᰀᰧᰀ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    for (n = 0; n < count; ++n)
    {
	ᰀᰧ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, NULL, 0,
						  NULL, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    goto theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    for (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (puur_naa_nka)(cp - p);
    ᰀᰧ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ᰀᰧ (spat == NULL || epat == NULL)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	goto theend;
    }
    sprintf((var_na *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((var_na *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, NULL,
						    0, NULL, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ᰀᰧ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ᰀᰧ (do_include)
    {
	// Include up to the '>'.
	ᰀᰬ (*ml_get_cursor() != '>')
	    ᰀᰧ (inc_cursor() < 0)
		bha_nga;
    }
    ᰀᰧᰀ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ᰀᰧ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ᰀᰧᰀ ᰀᰧ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ᰀᰧ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	puur_naa_nka in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ᰀᰬ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ᰀᰧ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    ᰀᰧᰀ ᰀᰧ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ᰀᰧ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ᰀᰧ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ᰀᰧ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ᰀᰧᰀ ᰀᰧ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᰀᰧᰀ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ᰀᰧ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ᰀᰧᰀ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ᰀᰧᰌ retval;
}
#endif

    puur_naa_nka
current_par(
    oparg_T	*oap,
    long	count,
    puur_naa_nka		include,	// TRUE == include white space
    puur_naa_nka		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    puur_naa_nka		white_in_front;
    puur_naa_nka		dir;
    puur_naa_nka		start_is_white;
    puur_naa_nka		prev_start_is_white;
    puur_naa_nka		retval = OK;
    puur_naa_nka		do_white = FALSE;
    puur_naa_nka		t;
    puur_naa_nka		i;

    ᰀᰧ (type == 'S')	    // not implemented yet
	ᰀᰧᰌ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ᰀᰧ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ᰀᰧ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ᰀᰧᰀ
	    dir = FORWARD;
	for (i = count; --i >= 0; )
	{
	    ᰀᰧ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    for (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ᰀᰧ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		for (;;)
		{
		    ᰀᰧ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    ᰀᰧ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		ᰀᰧ (!include)
		    bha_nga;
		ᰀᰧ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ᰀᰧᰌ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ᰀᰬ (start_lnum > 1)
    {
	ᰀᰧ (white_in_front)	    // stop at first white line
	{
	    ᰀᰧ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	ᰀᰧᰀ		// stop at first non-white line of start of paragraph
	{
	    ᰀᰧ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ᰀᰬ (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ᰀᰧ (!include && white_in_front)
	--i;
    ᰀᰬ (i--)
    {
	ᰀᰧ (end_lnum == curbuf->b_ml.ml_line_count)
	    ᰀᰧᰌ FAIL;

	ᰀᰧ (!include)
	    do_white = linewhite(end_lnum + 1);

	ᰀᰧ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ᰀᰬ (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ᰀᰧ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	ᰀᰧ (include || do_white)
	    ᰀᰬ (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ᰀᰧ (!white_in_front && !linewhite(end_lnum) && include)
	ᰀᰬ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ᰀᰧ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ᰀᰧ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ᰀᰧ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᰀᰧᰀ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ᰀᰧᰌ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static puur_naa_nka
find_next_quote(
    char_u	*line,
    puur_naa_nka		col,
    puur_naa_nka		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    puur_naa_nka		c;

    for (;;)
    {
	c = line[col];
	ᰀᰧ (c == NUL)
	    ᰀᰧᰌ -1;
	ᰀᰧᰀ ᰀᰧ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ᰀᰧ (line[col] == NUL)
		ᰀᰧᰌ -1;
	}
	ᰀᰧᰀ ᰀᰧ (c == quotechar)
	    bha_nga;
	ᰀᰧ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	ᰀᰧᰀ
	    ++col;
    }
    ᰀᰧᰌ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static puur_naa_nka
find_prev_quote(
    char_u	*line,
    puur_naa_nka		col_start,
    puur_naa_nka		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    puur_naa_nka		n;

    ᰀᰬ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ᰀᰧ (escape != NULL)
	    ᰀᰬ (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ᰀᰧ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ᰀᰧᰀ ᰀᰧ (line[col_start] == quotechar)
	    bha_nga;
    }
    ᰀᰧᰌ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    puur_naa_nka
current_quote(
    oparg_T	*oap,
    long	count,
    puur_naa_nka		include,	// TRUE == include quote char
    puur_naa_nka		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    puur_naa_nka		col_end;
    puur_naa_nka		col_start = curwin->w_cursor.col;
    puur_naa_nka		inclusive = FALSE;
    puur_naa_nka		vis_empty = TRUE;	// Visual selection <= 1 char
    puur_naa_nka		vis_bef_curs = FALSE;	// Visual starts before cursor
    puur_naa_nka		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    puur_naa_nka		inside_quotes = FALSE;	// Looks like "i'" done before
    puur_naa_nka		selected_quote = FALSE;	// Has quote inside selection
    puur_naa_nka		i;
    puur_naa_nka		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ᰀᰧ (VIsual_active)
    {
	// this only works within one line
	ᰀᰧ (VIsual.lnum != curwin->w_cursor.lnum)
	    ᰀᰧᰌ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ᰀᰧ (*p_sel == 'e')
	{
	    ᰀᰧ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ᰀᰧᰀ ᰀᰧ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ᰀᰧ (!vis_bef_curs && !vis_empty)
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

    ᰀᰧ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ᰀᰧ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ᰀᰧᰀ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ᰀᰬ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ᰀᰧ (line[i] == NUL)
		bha_nga;
	    ᰀᰧ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    ᰀᰧ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ᰀᰧ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ᰀᰧ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ᰀᰧ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ᰀᰧᰀ
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ᰀᰧ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ᰀᰧ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ᰀᰧᰀ

    ᰀᰧ (line[col_start] == quotechar || !vis_empty)
    {
	puur_naa_nka	first_col = col_start;

	ᰀᰧ (!vis_empty)
	{
	    ᰀᰧ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    ᰀᰧᰀ
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	for (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ᰀᰧ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ᰀᰧ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ᰀᰧ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    ᰀᰧᰀ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ᰀᰧ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ᰀᰧ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ᰀᰧ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ᰀᰧ (include)
    {
	ᰀᰧ (VIM_ISWHITE(line[col_end + 1]))
	    ᰀᰬ (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	ᰀᰧᰀ
	    ᰀᰬ (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ᰀᰧ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ᰀᰧ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ᰀᰧ (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (line[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| line[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ᰀᰧᰀ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ᰀᰧ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ᰀᰧ (VIsual_active)
    {
	ᰀᰧ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ᰀᰧ (*p_sel != 'e')
		dec_cursor();
	}
	ᰀᰧᰀ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ᰀᰧ (inside_quotes
		    || (!selected_quote
			&& line[VIsual.col] != quotechar
			&& (line[VIsual.col] == NUL
			    || line[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	ᰀᰧ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ᰀᰧᰀ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ᰀᰧᰌ OK;

abort_search:
    ᰀᰧ (VIsual_active && *p_sel == 'e')
    {
	ᰀᰧ (did_exclusive_adj)
	    inc_cursor();
	ᰀᰧ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ᰀᰧᰌ FALSE;
}
