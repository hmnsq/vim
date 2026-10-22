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

static ចំនួនគត់ cls(void);
static ចំនួនគត់ skip_chars(ចំនួនគត់, ចំនួនគត់);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ចំនួនគត់
findsent(ចំនួនគត់ dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ចំនួនគត់		c;
    ចំនួនគត់		(*func)(pos_T *);
    ចំនួនគត់		startlnum;
    ចំនួនគត់		noskip = FALSE;	    // do not skip blanks
    ចំនួនគត់		cpo_J;
    ចំនួនគត់		found_dot;

    pos = curwin->w_cursor;
    ប្រសិនបើ (dir == FORWARD)
	func = incl;
    បើមិនដូច្នេះ
	func = decl;

    ខណៈពេល (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ប្រសិនបើ (gchar_pos(&pos) == NUL)
	{
	    do
		ប្រសិនបើ ((*func)(&pos) == -1)
		    bha_nga;
	    ខណៈពេល (gchar_pos(&pos) == NUL);
	    ប្រសិនបើ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	បើមិនដូច្នេះ ប្រសិនបើ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ប្រសិនបើ (pos.lnum == curbuf->b_ml.ml_line_count)
		ត្រឡប់ FAIL;
	    ++pos.lnum;
	    goto found;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ខណៈពេល (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ប្រសិនបើ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    ប្រសិនបើ (found_dot)
		bha_nga;
	    ប្រសិនបើ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ប្រសិនបើ (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	សម្រាប់ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ប្រសិនបើ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ប្រសិនបើ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    ប្រសិនបើ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ប្រសិនបើ ((c = inc(&tpos)) == -1)
			bha_nga;
		ខណៈពេល (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ប្រសិនបើ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ប្រសិនបើ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    ប្រសិនបើ ((*func)(&pos) == -1)
	    {
		ប្រសិនបើ (count)
		    ត្រឡប់ FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	ខណៈពេល (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ប្រសិនបើ (incl(&pos) == -1)
		bha_nga;

	ប្រសិនបើ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ប្រសិនបើ ((*func)(&pos) == -1)
	    {
		ប្រសិនបើ (count)
		    ត្រឡប់ FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ត្រឡប់ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ចំនួនគត់
findpar(
    ចំនួនគត់		*pincl,	    // Return: TRUE if last char is to be included
    ចំនួនគត់		dir,
    long	count,
    ចំនួនគត់		what,
    ចំនួនគត់		both)
{
    linenr_T	curr;
    ចំនួនគត់		did_skip;   // TRUE after separating lines have been skipped
    ចំនួនគត់		first;	    // TRUE on first line
    ចំនួនគត់		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ចំនួនគត់		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ខណៈពេល (count--)
    {
	did_skip = FALSE;
	សម្រាប់ (first = TRUE; ; first = FALSE)
	{
	    ប្រសិនបើ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ប្រសិនបើ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ប្រសិនបើ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    ប្រសិនបើ (fold_skipped)
		curr -= dir;
#endif
	    ប្រសិនបើ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ប្រសិនបើ (count)
		    ត្រឡប់ FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    ប្រសិនបើ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ប្រសិនបើ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ប្រសិនបើ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    បើមិនដូច្នេះ
	curwin->w_cursor.col = 0;
    ត្រឡប់ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ចំនួនគត់
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    សម្រាប់ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ប្រសិនបើ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	ប្រសិនបើ (macro[0] == NUL)
	    bha_nga;
    }
    ត្រឡប់ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ចំនួនគត់
startPS(linenr_T lnum, ចំនួនគត់ para, ចំនួនគត់ both)
{
    char_u	*s;

    s = ml_get(lnum);
    ប្រសិនបើ (*s == para || *s == '\f' || (both && *s == '}'))
	ត្រឡប់ TRUE;
    ប្រសិនបើ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ត្រឡប់ TRUE;
    ត្រឡប់ FALSE;
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

static ចំនួនគត់	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static ចំនួនគត់
cls(void)
{
    ចំនួនគត់	    c;

    c = gchar_cursor();
    ប្រសិនបើ (c == ' ' || c == '\t' || c == NUL)
	ត្រឡប់ 0;
    ប្រសិនបើ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ប្រសិនបើ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ត្រឡប់ 1;

	// process code leading/trailing bytes
	ត្រឡប់ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ប្រសិនបើ (enc_utf8)
    {
	c = utf_class(c);
	ប្រសិនបើ (c != 0 && cls_bigword)
	    ត្រឡប់ 1;
	ត្រឡប់ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ប្រសិនបើ (cls_bigword)
	ត្រឡប់ 1;

    ប្រសិនបើ (vim_iswordc(c))
	ត្រឡប់ 2;
    ត្រឡប់ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ចំនួនគត់
fwd_word(
    long	count,
    ចំនួនគត់		bigword,    // "W", "E" or "B"
    ចំនួនគត់		eol)
{
    ចំនួនគត់		sclass;	    // starting class
    ចំនួនគត់		i;
    ចំនួនគត់		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ខណៈពេល (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ប្រសិនបើ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ប្រសិនបើ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (i >= 1 && eol && count == 0)      // started at last char in line
	    ត្រឡប់ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ប្រសិនបើ (sclass != 0)
	    ខណៈពេល (cls() == sclass)
	    {
		i = inc_cursor();
		ប្រសិនបើ (i == -1 || (i >= 1 && eol && count == 0))
		    ត្រឡប់ OK;
	    }

	/*
	 * go to next non-white
	 */
	ខណៈពេល (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ប្រសិនបើ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    ប្រសិនបើ (i == -1 || (i >= 1 && eol && count == 0))
		ត្រឡប់ OK;
	}
    }
    ត្រឡប់ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ចំនួនគត់
bck_word(long count, ចំនួនគត់ bigword, ចំនួនគត់ stop)
{
    ចំនួនគត់		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ខណៈពេល (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ប្រសិនបើ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ប្រសិនបើ (dec_cursor() == -1)		// started at start of file
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ខណៈពេល (cls() == 0)
	    {
		ប្រសិនបើ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ប្រសិនបើ (dec_cursor() == -1) // hit start of file, stop here
		    ត្រឡប់ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ប្រសិនបើ (skip_chars(cls(), BACKWARD))
		ត្រឡប់ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ត្រឡប់ OK;
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
    ចំនួនគត់
end_word(
    long	count,
    ចំនួនគត់		bigword,
    ចំនួនគត់		stop,
    ចំនួនគត់		empty)
{
    ចំនួនគត់		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ប្រសិនបើ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ខណៈពេល (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ប្រសិនបើ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ប្រសិនបើ (inc_cursor() == -1)
	    ត្រឡប់ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ប្រសិនបើ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ប្រសិនបើ (skip_chars(sclass, FORWARD))
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ខណៈពេល (cls() == 0)
	    {
		ប្រសិនបើ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ប្រសិនបើ (inc_cursor() == -1)	    // hit end of file, stop here
		    ត្រឡប់ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ប្រសិនបើ (skip_chars(cls(), FORWARD))
		ត្រឡប់ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ត្រឡប់ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ចំនួនគត់
bckend_word(
    long	count,
    ចំនួនគត់		bigword,    // TRUE for "B"
    ចំនួនគត់		eol)	    // TRUE: stop at end of line.
{
    ចំនួនគត់		sclass;	    // starting class
    ចំនួនគត់		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ខណៈពេល (--count >= 0)
    {
	sclass = cls();
	ប្រសិនបើ ((i = dec_cursor()) == -1)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (eol && i == 1)
	    ត្រឡប់ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ប្រសិនបើ (sclass != 0)
	{
	    ខណៈពេល (cls() == sclass)
		ប្រសិនបើ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ត្រឡប់ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ខណៈពេល (cls() == 0)
	{
	    ប្រសិនបើ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    ប្រសិនបើ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ត្រឡប់ OK;
	}
    }
    adjust_skipcol();
    ត្រឡប់ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ចំនួនគត់
skip_chars(ចំនួនគត់ cclass, ចំនួនគត់ dir)
{
    ខណៈពេល (cls() == cclass)
	ប្រសិនបើ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ត្រឡប់ TRUE;
    ត្រឡប់ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ចំនួនគត់		sclass;		    // starting class

    sclass = cls();
    សម្រាប់ (;;)
    {
	ប្រសិនបើ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	ប្រសិនបើ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    ចំនួនគត់	    c;

    ខណៈពេល (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ប្រសិនបើ (!VIM_ISWHITE(c))
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
    ចំនួនគត់	    at_start_sent)	// cursor is at start of sentence
{
    ខណៈពេល (count--)
    {
	findsent(FORWARD, 1L);
	ប្រសិនបើ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ប្រសិនបើ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ចំនួនគត់
current_word(
    oparg_T	*oap,
    long	count,
    ចំនួនគត់		include,	// TRUE: include word and white space
    ចំនួនគត់		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ចំនួនគត់		inclusive = TRUE;
    ចំនួនគត់		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ប្រសិនបើ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ប្រសិនបើ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	ប្រសិនបើ ((cls() == 0) == include)
	{
	    ប្រសិនបើ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ប្រសិនបើ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    បើមិនដូច្នេះ
		oneleft();

	    ប្រសិនបើ (include)
		include_white = TRUE;
	}

	ប្រសិនបើ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	បើមិនដូច្នេះ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ខណៈពេល (count > 0)
    {
	inclusive = TRUE;
	ប្រសិនបើ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ប្រសិនបើ (decl(&curwin->w_cursor) == -1)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (include != (cls() != 0))
	    {
		ប្រសិនបើ (bck_word(1L, bigword, TRUE) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ត្រឡប់ FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	បើមិនដូច្នេះ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ប្រសិនបើ (incl(&curwin->w_cursor) == -1)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (include != (cls() == 0))
	    {
		ប្រសិនបើ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ត្រឡប់ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ប្រសិនបើ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	}
	--count;
    }

    ប្រសិនបើ (include_white && (cls() != 0
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
	ប្រសិនបើ (oneleft() == OK)
	{
	    back_in_line();
	    ប្រសិនបើ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ប្រសិនបើ (VIsual_active)
		    VIsual = curwin->w_cursor;
		បើមិនដូច្នេះ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ប្រសិនបើ (VIsual_active)
    {
	ប្រសិនបើ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ប្រសិនបើ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    បើមិនដូច្នេះ
	oap->inclusive = inclusive;

    ត្រឡប់ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ចំនួនគត់
current_sent(oparg_T *oap, long count, ចំនួនគត់ include)
{
    pos_T	start_pos;
    pos_T	pos;
    ចំនួនគត់		start_blank;
    ចំនួនគត់		c;
    ចំនួនគត់		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ប្រសិនបើ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ប្រសិនបើ (LT_POS(start_pos, VIsual))
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
	    ខណៈពេល (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ប្រសិនបើ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    ប្រសិនបើ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ប្រសិនបើ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		បើមិនដូច្នេះ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ប្រសិនបើ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ខណៈពេល (count--)
	    {
		ប្រសិនបើ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ប្រសិនបើ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	បើមិនដូច្នេះ
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
	    ប្រសិនបើ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ខណៈពេល (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ប្រសិនបើ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		ប្រសិនបើ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		បើមិនដូច្នេះ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ប្រសិនបើ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ប្រសិនបើ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ត្រឡប់ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ខណៈពេល (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ប្រសិនបើ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    បើមិនដូច្នេះ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ប្រសិនបើ (include)
	ncount = count * 2;
    បើមិនដូច្នេះ
    {
	ncount = count;
	ប្រសិនបើ (start_blank)
	    --ncount;
    }
    ប្រសិនបើ (ncount > 0)
	findsent_forward(ncount, TRUE);
    បើមិនដូច្នេះ
	decl(&curwin->w_cursor);

    ប្រសិនបើ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ប្រសិនបើ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ប្រសិនបើ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ប្រសិនបើ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ប្រសិនបើ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ប្រសិនបើ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    បើមិនដូច្នេះ
    {
	// include a newline after the sentence, if there is one
	ប្រសិនបើ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	បើមិនដូច្នេះ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ត្រឡប់ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ចំនួនគត់
current_block(
    oparg_T	*oap,
    long	count,
    ចំនួនគត់		include,	// TRUE == include white space
    ចំនួនគត់		what,		// '(', '{', etc.
    ចំនួនគត់		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ចំនួនគត់		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ប្រសិនបើ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ប្រសិនបើ (what == '{')		// ignore indent
	    ខណៈពេល (inindent(1))
		ប្រសិនបើ (inc_cursor() != 0)
		    bha_nga;
	ប្រសិនបើ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    បើមិនដូច្នេះ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ប្រសិនបើ ((pos = findmatch(NULL, what)) != NULL)
    {
	ខណៈពេល (count-- > 0)
	{
	    ប្រសិនបើ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    បើមិនដូច្នេះ
    {
	ខណៈពេល (count-- > 0)
	{
	    ប្រសិនបើ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    ប្រសិនបើ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ត្រឡប់ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ខណៈពេល (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ខណៈពេល (inindent(1))
	{
	    sol = TRUE;
	    ប្រសិនបើ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ប្រសិនបើ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ត្រឡប់ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ប្រសិនបើ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ប្រសិនបើ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ត្រឡប់ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ប្រសិនបើ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ត្រឡប់ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	បើមិនដូច្នេះ
	    bha_nga;
    }

    ប្រសិនបើ (VIsual_active)
    {
	ប្រសិនបើ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ប្រសិនបើ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    បើមិនដូច្នេះ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ប្រសិនបើ (sol)
	    incl(&curwin->w_cursor);
	បើមិនដូច្នេះ ប្រសិនបើ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	បើមិនដូច្នេះ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ត្រឡប់ OK;
}

#ប្រសិនបើ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static ចំនួនគត់
in_html_tag(
    ចំនួនគត់		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    ចំនួនគត់		c;
    ចំនួនគត់		lc = NUL;
    pos_T	pos;

    ប្រសិនបើ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	សម្រាប់ (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ប្រសិនបើ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ប្រសិនបើ (*p != '<')	    // check for '<' under cursor
	{
	    ប្រសិនបើ (lc != '<')
		ត្រឡប់ FALSE;
	    p = lp;
	}
    }
    បើមិនដូច្នេះ
    {
	សម្រាប់ (p = line + curwin->w_cursor.col; p > line; )
	{
	    ប្រសិនបើ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    ប្រសិនបើ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	ប្រសិនបើ (*p != '<')
	    ត្រឡប់ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ប្រសិនបើ (end_tag)
	// check that there is a '/' after the '<'
	ត្រឡប់ *p == '/';

    // check that there is no '/' after the '<'
    ប្រសិនបើ (*p == '/')
	ត្រឡប់ FALSE;

    // check that the matching '>' is not preceded by '/'
    សម្រាប់ (;;)
    {
	ប្រសិនបើ (inc(&pos) < 0)
	    ត្រឡប់ FALSE;
	c = *ml_get_pos(&pos);
	ប្រសិនបើ (c == '>')
	    bha_nga;
	lc = c;
    }
    ត្រឡប់ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ចំនួនគត់
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    ចំនួនគត់		include)	// TRUE == include white space
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
    ចំនួនគត់		len;
    ចំនួនគត់		r;
    ចំនួនគត់		do_include = include;
    ចំនួនគត់		save_p_ws = p_ws;
    ចំនួនគត់		retval = FAIL;
    ចំនួនគត់		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ប្រសិនបើ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ប្រសិនបើ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ខណៈពេល (inindent(1))
	    ប្រសិនបើ (inc_cursor() != 0)
		bha_nga;

	ប្រសិនបើ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ខណៈពេល (*ml_get_cursor() != '>')
		ប្រសិនបើ (inc_cursor() < 0)
		    bha_nga;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ខណៈពេល (*ml_get_cursor() != '<')
		ប្រសិនបើ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    បើមិនដូច្នេះ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    សម្រាប់ (n = 0; n < count; ++n)
    {
	ប្រសិនបើ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    សម្រាប់ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ចំនួនគត់)(cp - p);
    ប្រសិនបើ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ប្រសិនបើ (spat == NULL || epat == NULL)
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

    ប្រសិនបើ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ប្រសិនបើ (do_include)
    {
	// Include up to the '>'.
	ខណៈពេល (*ml_get_cursor() != '>')
	    ប្រសិនបើ (inc_cursor() < 0)
		bha_nga;
    }
    បើមិនដូច្នេះ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ប្រសិនបើ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	បើមិនដូច្នេះ ប្រសិនបើ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ប្រសិនបើ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ចំនួនគត់ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ខណៈពេល (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ប្រសិនបើ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ប្រសិនបើ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ប្រសិនបើ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ប្រសិនបើ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	បើមិនដូច្នេះ ប្រសិនបើ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    បើមិនដូច្នេះ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ប្រសិនបើ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	បើមិនដូច្នេះ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ត្រឡប់ retval;
}
#endif

    ចំនួនគត់
current_par(
    oparg_T	*oap,
    long	count,
    ចំនួនគត់		include,	// TRUE == include white space
    ចំនួនគត់		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ចំនួនគត់		white_in_front;
    ចំនួនគត់		dir;
    ចំនួនគត់		start_is_white;
    ចំនួនគត់		prev_start_is_white;
    ចំនួនគត់		retval = OK;
    ចំនួនគត់		do_white = FALSE;
    ចំនួនគត់		t;
    ចំនួនគត់		i;

    ប្រសិនបើ (type == 'S')	    // not implemented yet
	ត្រឡប់ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ប្រសិនបើ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ប្រសិនបើ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	បើមិនដូច្នេះ
	    dir = FORWARD;
	សម្រាប់ (i = count; --i >= 0; )
	{
	    ប្រសិនបើ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    សម្រាប់ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ប្រសិនបើ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		សម្រាប់ (;;)
		{
		    ប្រសិនបើ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    ប្រសិនបើ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		ប្រសិនបើ (!include)
		    bha_nga;
		ប្រសិនបើ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ត្រឡប់ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ខណៈពេល (start_lnum > 1)
    {
	ប្រសិនបើ (white_in_front)	    // stop at first white line
	{
	    ប្រសិនបើ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	បើមិនដូច្នេះ		// stop at first non-white line of start of paragraph
	{
	    ប្រសិនបើ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ខណៈពេល (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ប្រសិនបើ (!include && white_in_front)
	--i;
    ខណៈពេល (i--)
    {
	ប្រសិនបើ (end_lnum == curbuf->b_ml.ml_line_count)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (!include)
	    do_white = linewhite(end_lnum + 1);

	ប្រសិនបើ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ខណៈពេល (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ប្រសិនបើ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	ប្រសិនបើ (include || do_white)
	    ខណៈពេល (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ប្រសិនបើ (!white_in_front && !linewhite(end_lnum) && include)
	ខណៈពេល (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ប្រសិនបើ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ប្រសិនបើ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ប្រសិនបើ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    បើមិនដូច្នេះ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ត្រឡប់ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static ចំនួនគត់
find_next_quote(
    char_u	*line,
    ចំនួនគត់		col,
    ចំនួនគត់		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ចំនួនគត់		c;

    សម្រាប់ (;;)
    {
	c = line[col];
	ប្រសិនបើ (c == NUL)
	    ត្រឡប់ -1;
	បើមិនដូច្នេះ ប្រសិនបើ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ប្រសិនបើ (line[col] == NUL)
		ត្រឡប់ -1;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (c == quotechar)
	    bha_nga;
	ប្រសិនបើ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	បើមិនដូច្នេះ
	    ++col;
    }
    ត្រឡប់ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static ចំនួនគត់
find_prev_quote(
    char_u	*line,
    ចំនួនគត់		col_start,
    ចំនួនគត់		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ចំនួនគត់		n;

    ខណៈពេល (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ប្រសិនបើ (escape != NULL)
	    ខណៈពេល (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ប្រសិនបើ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	បើមិនដូច្នេះ ប្រសិនបើ (line[col_start] == quotechar)
	    bha_nga;
    }
    ត្រឡប់ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ចំនួនគត់
current_quote(
    oparg_T	*oap,
    long	count,
    ចំនួនគត់		include,	// TRUE == include quote char
    ចំនួនគត់		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    ចំនួនគត់		col_end;
    ចំនួនគត់		col_start = curwin->w_cursor.col;
    ចំនួនគត់		inclusive = FALSE;
    ចំនួនគត់		vis_empty = TRUE;	// Visual selection <= 1 char
    ចំនួនគត់		vis_bef_curs = FALSE;	// Visual starts before cursor
    ចំនួនគត់		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ចំនួនគត់		inside_quotes = FALSE;	// Looks like "i'" done before
    ចំនួនគត់		selected_quote = FALSE;	// Has quote inside selection
    ចំនួនគត់		i;
    ចំនួនគត់		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ប្រសិនបើ (VIsual_active)
    {
	// this only works within one line
	ប្រសិនបើ (VIsual.lnum != curwin->w_cursor.lnum)
	    ត្រឡប់ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ប្រសិនបើ (*p_sel == 'e')
	{
	    ប្រសិនបើ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ប្រសិនបើ (!vis_bef_curs && !vis_empty)
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

    ប្រសិនបើ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ប្រសិនបើ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	បើមិនដូច្នេះ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ខណៈពេល (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ប្រសិនបើ (line[i] == NUL)
		bha_nga;
	    ប្រសិនបើ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    ប្រសិនបើ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ប្រសិនបើ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ប្រសិនបើ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ប្រសិនបើ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	បើមិនដូច្នេះ
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ប្រសិនបើ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ប្រសិនបើ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    បើមិនដូច្នេះ

    ប្រសិនបើ (line[col_start] == quotechar || !vis_empty)
    {
	ចំនួនគត់	first_col = col_start;

	ប្រសិនបើ (!vis_empty)
	{
	    ប្រសិនបើ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    បើមិនដូច្នេះ
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	សម្រាប់ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ប្រសិនបើ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ប្រសិនបើ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ប្រសិនបើ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    បើមិនដូច្នេះ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ប្រសិនបើ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ប្រសិនបើ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ប្រសិនបើ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ប្រសិនបើ (include)
    {
	ប្រសិនបើ (VIM_ISWHITE(line[col_end + 1]))
	    ខណៈពេល (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	បើមិនដូច្នេះ
	    ខណៈពេល (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ប្រសិនបើ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ប្រសិនបើ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ប្រសិនបើ (vis_empty
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
    បើមិនដូច្នេះ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ប្រសិនបើ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ប្រសិនបើ (VIsual_active)
    {
	ប្រសិនបើ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ប្រសិនបើ (*p_sel != 'e')
		dec_cursor();
	}
	បើមិនដូច្នេះ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ប្រសិនបើ (inside_quotes
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
	ប្រសិនបើ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    បើមិនដូច្នេះ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ត្រឡប់ OK;

abort_search:
    ប្រសិនបើ (VIsual_active && *p_sel == 'e')
    {
	ប្រសិនបើ (did_exclusive_adj)
	    inc_cursor();
	ប្រសិនបើ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ត្រឡប់ FALSE;
}
