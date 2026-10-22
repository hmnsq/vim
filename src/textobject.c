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
    ᬬᬾᬦ᭄ (dir == FORWARD)
	func = incl;
    ᬮᬶᬬ
	func = decl;

    ᬲᭂᬤᭂᬮ (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ᬬᬾᬦ᭄ (gchar_pos(&pos) == NUL)
	{
	    do
		ᬬᬾᬦ᭄ ((*func)(&pos) == -1)
		    bha_nga;
	    ᬲᭂᬤᭂᬮ (gchar_pos(&pos) == NUL);
	    ᬬᬾᬦ᭄ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ᬮᬶᬬ ᬬᬾᬦ᭄ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ᬬᬾᬦ᭄ (pos.lnum == curbuf->b_ml.ml_line_count)
		ᬯᬮᬶ FAIL;
	    ++pos.lnum;
	    goto found;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ᬲᭂᬤᭂᬮ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ᬬᬾᬦ᭄ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    ᬬᬾᬦ᭄ (found_dot)
		bha_nga;
	    ᬬᬾᬦ᭄ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ᬬᬾᬦ᭄ (vim_strchr((char_u *) ")]\"'", c) != NULL
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
	    ᬬᬾᬦ᭄ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ᬬᬾᬦ᭄ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    ᬬᬾᬦ᭄ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ᬬᬾᬦ᭄ ((c = inc(&tpos)) == -1)
			bha_nga;
		ᬲᭂᬤᭂᬮ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ᬬᬾᬦ᭄ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ᬬᬾᬦ᭄ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    ᬬᬾᬦ᭄ ((*func)(&pos) == -1)
	    {
		ᬬᬾᬦ᭄ (count)
		    ᬯᬮᬶ FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	ᬲᭂᬤᭂᬮ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ᬬᬾᬦ᭄ (incl(&pos) == -1)
		bha_nga;

	ᬬᬾᬦ᭄ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ᬬᬾᬦ᭄ ((*func)(&pos) == -1)
	    {
		ᬬᬾᬦ᭄ (count)
		    ᬯᬮᬶ FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ᬯᬮᬶ OK;
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

    ᬲᭂᬤᭂᬮ (count--)
    {
	did_skip = FALSE;
	for (first = TRUE; ; first = FALSE)
	{
	    ᬬᬾᬦ᭄ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ᬬᬾᬦ᭄ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ᬬᬾᬦ᭄ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    ᬬᬾᬦ᭄ (fold_skipped)
		curr -= dir;
#endif
	    ᬬᬾᬦ᭄ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ᬬᬾᬦ᭄ (count)
		    ᬯᬮᬶ FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    ᬬᬾᬦ᭄ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ᬬᬾᬦ᭄ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ᬬᬾᬦ᭄ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ᬮᬶᬬ
	curwin->w_cursor.col = 0;
    ᬯᬮᬶ TRUE;
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
	ᬬᬾᬦ᭄ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	ᬬᬾᬦ᭄ (macro[0] == NUL)
	    bha_nga;
    }
    ᬯᬮᬶ (macro[0] != NUL);
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
    ᬬᬾᬦ᭄ (*s == para || *s == '\f' || (both && *s == '}'))
	ᬯᬮᬶ TRUE;
    ᬬᬾᬦ᭄ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
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
    ᬬᬾᬦ᭄ (c == ' ' || c == '\t' || c == NUL)
	ᬯᬮᬶ 0;
    ᬬᬾᬦ᭄ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ᬬᬾᬦ᭄ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ᬯᬮᬶ 1;

	// process code leading/trailing bytes
	ᬯᬮᬶ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ᬬᬾᬦ᭄ (enc_utf8)
    {
	c = utf_class(c);
	ᬬᬾᬦ᭄ (c != 0 && cls_bigword)
	    ᬯᬮᬶ 1;
	ᬯᬮᬶ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ᬬᬾᬦ᭄ (cls_bigword)
	ᬯᬮᬶ 1;

    ᬬᬾᬦ᭄ (vim_iswordc(c))
	ᬯᬮᬶ 2;
    ᬯᬮᬶ 1;
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
    ᬲᭂᬤᭂᬮ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ᬬᬾᬦ᭄ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ᬬᬾᬦ᭄ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ᬯᬮᬶ FAIL;
	ᬬᬾᬦ᭄ (i >= 1 && eol && count == 0)      // started at last char in line
	    ᬯᬮᬶ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ᬬᬾᬦ᭄ (sclass != 0)
	    ᬲᭂᬤᭂᬮ (cls() == sclass)
	    {
		i = inc_cursor();
		ᬬᬾᬦ᭄ (i == -1 || (i >= 1 && eol && count == 0))
		    ᬯᬮᬶ OK;
	    }

	/*
	 * go to next non-white
	 */
	ᬲᭂᬤᭂᬮ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ᬬᬾᬦ᭄ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    ᬬᬾᬦ᭄ (i == -1 || (i >= 1 && eol && count == 0))
		ᬯᬮᬶ OK;
	}
    }
    ᬯᬮᬶ OK;
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
    ᬲᭂᬤᭂᬮ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ᬬᬾᬦ᭄ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ᬬᬾᬦ᭄ (dec_cursor() == -1)		// started at start of file
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ᬲᭂᬤᭂᬮ (cls() == 0)
	    {
		ᬬᬾᬦ᭄ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ᬬᬾᬦ᭄ (dec_cursor() == -1) // hit start of file, stop here
		    ᬯᬮᬶ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ᬬᬾᬦ᭄ (skip_chars(cls(), BACKWARD))
		ᬯᬮᬶ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ᬯᬮᬶ OK;
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
    ᬬᬾᬦ᭄ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ᬲᭂᬤᭂᬮ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ᬬᬾᬦ᭄ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ᬬᬾᬦ᭄ (inc_cursor() == -1)
	    ᬯᬮᬶ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ᬬᬾᬦ᭄ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ᬬᬾᬦ᭄ (skip_chars(sclass, FORWARD))
		ᬯᬮᬶ FAIL;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ᬲᭂᬤᭂᬮ (cls() == 0)
	    {
		ᬬᬾᬦ᭄ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ᬬᬾᬦ᭄ (inc_cursor() == -1)	    // hit end of file, stop here
		    ᬯᬮᬶ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ᬬᬾᬦ᭄ (skip_chars(cls(), FORWARD))
		ᬯᬮᬶ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ᬯᬮᬶ OK;
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
    ᬲᭂᬤᭂᬮ (--count >= 0)
    {
	sclass = cls();
	ᬬᬾᬦ᭄ ((i = dec_cursor()) == -1)
	    ᬯᬮᬶ FAIL;
	ᬬᬾᬦ᭄ (eol && i == 1)
	    ᬯᬮᬶ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ᬬᬾᬦ᭄ (sclass != 0)
	{
	    ᬲᭂᬤᭂᬮ (cls() == sclass)
		ᬬᬾᬦ᭄ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ᬯᬮᬶ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ᬲᭂᬤᭂᬮ (cls() == 0)
	{
	    ᬬᬾᬦ᭄ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    ᬬᬾᬦ᭄ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ᬯᬮᬶ OK;
	}
    }
    adjust_skipcol();
    ᬯᬮᬶ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static puur_naa_nka
skip_chars(puur_naa_nka cclass, puur_naa_nka dir)
{
    ᬲᭂᬤᭂᬮ (cls() == cclass)
	ᬬᬾᬦ᭄ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
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
	ᬬᬾᬦ᭄ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	ᬬᬾᬦ᭄ (cls() != sclass)		    // stop at start of word
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

    ᬲᭂᬤᭂᬮ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ᬬᬾᬦ᭄ (!VIM_ISWHITE(c))
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
    ᬲᭂᬤᭂᬮ (count--)
    {
	findsent(FORWARD, 1L);
	ᬬᬾᬦ᭄ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ᬬᬾᬦ᭄ (count == 0 || at_start_sent)
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
    ᬬᬾᬦ᭄ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ᬬᬾᬦ᭄ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	ᬬᬾᬦ᭄ ((cls() == 0) == include)
	{
	    ᬬᬾᬦ᭄ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ᬯᬮᬶ FAIL;
	}
	ᬮᬶᬬ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ᬬᬾᬦ᭄ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ᬮᬶᬬ
		oneleft();

	    ᬬᬾᬦ᭄ (include)
		include_white = TRUE;
	}

	ᬬᬾᬦ᭄ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ᬮᬶᬬ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ᬲᭂᬤᭂᬮ (count > 0)
    {
	inclusive = TRUE;
	ᬬᬾᬦ᭄ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ᬬᬾᬦ᭄ (decl(&curwin->w_cursor) == -1)
		ᬯᬮᬶ FAIL;
	    ᬬᬾᬦ᭄ (include != (cls() != 0))
	    {
		ᬬᬾᬦ᭄ (bck_word(1L, bigword, TRUE) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	    ᬮᬶᬬ
	    {
		ᬬᬾᬦ᭄ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ᬯᬮᬶ FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	ᬮᬶᬬ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ᬬᬾᬦ᭄ (incl(&curwin->w_cursor) == -1)
		ᬯᬮᬶ FAIL;
	    ᬬᬾᬦ᭄ (include != (cls() == 0))
	    {
		ᬬᬾᬦ᭄ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ᬯᬮᬶ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ᬬᬾᬦ᭄ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ᬮᬶᬬ
	    {
		ᬬᬾᬦ᭄ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ᬯᬮᬶ FAIL;
	    }
	}
	--count;
    }

    ᬬᬾᬦ᭄ (include_white && (cls() != 0
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
	ᬬᬾᬦ᭄ (oneleft() == OK)
	{
	    back_in_line();
	    ᬬᬾᬦ᭄ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ᬬᬾᬦ᭄ (VIsual_active)
		    VIsual = curwin->w_cursor;
		ᬮᬶᬬ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ᬬᬾᬦ᭄ (VIsual_active)
    {
	ᬬᬾᬦ᭄ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ᬬᬾᬦ᭄ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ᬮᬶᬬ
	oap->inclusive = inclusive;

    ᬯᬮᬶ OK;
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
    ᬬᬾᬦ᭄ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ᬬᬾᬦ᭄ (LT_POS(start_pos, VIsual))
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
	    ᬲᭂᬤᭂᬮ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ᬬᬾᬦ᭄ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    ᬬᬾᬦ᭄ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ᬬᬾᬦ᭄ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ᬮᬶᬬ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ᬬᬾᬦ᭄ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ᬲᭂᬤᭂᬮ (count--)
	    {
		ᬬᬾᬦ᭄ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ᬬᬾᬦ᭄ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ᬮᬶᬬ
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
	    ᬬᬾᬦ᭄ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ᬲᭂᬤᭂᬮ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ᬬᬾᬦ᭄ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		ᬬᬾᬦ᭄ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ᬮᬶᬬ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ᬬᬾᬦ᭄ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ᬬᬾᬦ᭄ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ᬯᬮᬶ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ᬲᭂᬤᭂᬮ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ᬬᬾᬦ᭄ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ᬮᬶᬬ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ᬬᬾᬦ᭄ (include)
	ncount = count * 2;
    ᬮᬶᬬ
    {
	ncount = count;
	ᬬᬾᬦ᭄ (start_blank)
	    --ncount;
    }
    ᬬᬾᬦ᭄ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ᬮᬶᬬ
	decl(&curwin->w_cursor);

    ᬬᬾᬦ᭄ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ᬬᬾᬦ᭄ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ᬬᬾᬦ᭄ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ᬬᬾᬦ᭄ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ᬬᬾᬦ᭄ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ᬬᬾᬦ᭄ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ᬮᬶᬬ
    {
	// include a newline after the sentence, if there is one
	ᬬᬾᬦ᭄ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ᬮᬶᬬ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ᬯᬮᬶ OK;
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
    ᬬᬾᬦ᭄ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ᬬᬾᬦ᭄ (what == '{')		// ignore indent
	    ᬲᭂᬤᭂᬮ (inindent(1))
		ᬬᬾᬦ᭄ (inc_cursor() != 0)
		    bha_nga;
	ᬬᬾᬦ᭄ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ᬮᬶᬬ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ᬬᬾᬦ᭄ ((pos = findmatch(NULL, what)) != NULL)
    {
	ᬲᭂᬤᭂᬮ (count-- > 0)
	{
	    ᬬᬾᬦ᭄ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ᬮᬶᬬ
    {
	ᬲᭂᬤᭂᬮ (count-- > 0)
	{
	    ᬬᬾᬦ᭄ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    ᬬᬾᬦ᭄ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ᬯᬮᬶ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ᬲᭂᬤᭂᬮ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ᬲᭂᬤᭂᬮ (inindent(1))
	{
	    sol = TRUE;
	    ᬬᬾᬦ᭄ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ᬬᬾᬦ᭄ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ᬯᬮᬶ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ᬬᬾᬦ᭄ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ᬬᬾᬦ᭄ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ᬯᬮᬶ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ᬬᬾᬦ᭄ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ᬯᬮᬶ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ᬮᬶᬬ
	    bha_nga;
    }

    ᬬᬾᬦ᭄ (VIsual_active)
    {
	ᬬᬾᬦ᭄ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ᬬᬾᬦ᭄ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᬮᬶᬬ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ᬬᬾᬦ᭄ (sol)
	    incl(&curwin->w_cursor);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ᬮᬶᬬ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ᬯᬮᬶ OK;
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
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

    ᬬᬾᬦ᭄ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	for (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ᬬᬾᬦ᭄ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ᬬᬾᬦ᭄ (*p != '<')	    // check for '<' under cursor
	{
	    ᬬᬾᬦ᭄ (lc != '<')
		ᬯᬮᬶ FALSE;
	    p = lp;
	}
    }
    ᬮᬶᬬ
    {
	for (p = line + curwin->w_cursor.col; p > line; )
	{
	    ᬬᬾᬦ᭄ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    ᬬᬾᬦ᭄ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	ᬬᬾᬦ᭄ (*p != '<')
	    ᬯᬮᬶ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ᬬᬾᬦ᭄ (end_tag)
	// check that there is a '/' after the '<'
	ᬯᬮᬶ *p == '/';

    // check that there is no '/' after the '<'
    ᬬᬾᬦ᭄ (*p == '/')
	ᬯᬮᬶ FALSE;

    // check that the matching '>' is not preceded by '/'
    for (;;)
    {
	ᬬᬾᬦ᭄ (inc(&pos) < 0)
	    ᬯᬮᬶ FALSE;
	c = *ml_get_pos(&pos);
	ᬬᬾᬦ᭄ (c == '>')
	    bha_nga;
	lc = c;
    }
    ᬯᬮᬶ lc != '/';
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
    ᬬᬾᬦ᭄ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ᬬᬾᬦ᭄ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ᬲᭂᬤᭂᬮ (inindent(1))
	    ᬬᬾᬦ᭄ (inc_cursor() != 0)
		bha_nga;

	ᬬᬾᬦ᭄ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ᬲᭂᬤᭂᬮ (*ml_get_cursor() != '>')
		ᬬᬾᬦ᭄ (inc_cursor() < 0)
		    bha_nga;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ᬲᭂᬤᭂᬮ (*ml_get_cursor() != '<')
		ᬬᬾᬦ᭄ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ᬮᬶᬬ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    for (n = 0; n < count; ++n)
    {
	ᬬᬾᬦ᭄ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    ᬬᬾᬦ᭄ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ᬬᬾᬦ᭄ (spat == NULL || epat == NULL)
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

    ᬬᬾᬦ᭄ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ᬬᬾᬦ᭄ (do_include)
    {
	// Include up to the '>'.
	ᬲᭂᬤᭂᬮ (*ml_get_cursor() != '>')
	    ᬬᬾᬦ᭄ (inc_cursor() < 0)
		bha_nga;
    }
    ᬮᬶᬬ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ᬬᬾᬦ᭄ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ᬬᬾᬦ᭄ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	puur_naa_nka in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ᬲᭂᬤᭂᬮ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ᬬᬾᬦ᭄ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ᬬᬾᬦ᭄ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ᬬᬾᬦ᭄ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ᬬᬾᬦ᭄ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᬮᬶᬬ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ᬬᬾᬦ᭄ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ᬮᬶᬬ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ᬯᬮᬶ retval;
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

    ᬬᬾᬦ᭄ (type == 'S')	    // not implemented yet
	ᬯᬮᬶ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ᬬᬾᬦ᭄ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ᬬᬾᬦ᭄ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ᬮᬶᬬ
	    dir = FORWARD;
	for (i = count; --i >= 0; )
	{
	    ᬬᬾᬦ᭄ (start_lnum ==
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
		ᬬᬾᬦ᭄ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		for (;;)
		{
		    ᬬᬾᬦ᭄ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    ᬬᬾᬦ᭄ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		ᬬᬾᬦ᭄ (!include)
		    bha_nga;
		ᬬᬾᬦ᭄ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ᬯᬮᬶ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ᬲᭂᬤᭂᬮ (start_lnum > 1)
    {
	ᬬᬾᬦ᭄ (white_in_front)	    // stop at first white line
	{
	    ᬬᬾᬦ᭄ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	ᬮᬶᬬ		// stop at first non-white line of start of paragraph
	{
	    ᬬᬾᬦ᭄ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ᬲᭂᬤᭂᬮ (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ᬬᬾᬦ᭄ (!include && white_in_front)
	--i;
    ᬲᭂᬤᭂᬮ (i--)
    {
	ᬬᬾᬦ᭄ (end_lnum == curbuf->b_ml.ml_line_count)
	    ᬯᬮᬶ FAIL;

	ᬬᬾᬦ᭄ (!include)
	    do_white = linewhite(end_lnum + 1);

	ᬬᬾᬦ᭄ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ᬲᭂᬤᭂᬮ (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ᬬᬾᬦ᭄ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	ᬬᬾᬦ᭄ (include || do_white)
	    ᬲᭂᬤᭂᬮ (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ᬬᬾᬦ᭄ (!white_in_front && !linewhite(end_lnum) && include)
	ᬲᭂᬤᭂᬮ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ᬬᬾᬦ᭄ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ᬬᬾᬦ᭄ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ᬬᬾᬦ᭄ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ᬮᬶᬬ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ᬯᬮᬶ OK;
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
	ᬬᬾᬦ᭄ (c == NUL)
	    ᬯᬮᬶ -1;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ᬬᬾᬦ᭄ (line[col] == NUL)
		ᬯᬮᬶ -1;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (c == quotechar)
	    bha_nga;
	ᬬᬾᬦ᭄ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	ᬮᬶᬬ
	    ++col;
    }
    ᬯᬮᬶ col;
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

    ᬲᭂᬤᭂᬮ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ᬬᬾᬦ᭄ (escape != NULL)
	    ᬲᭂᬤᭂᬮ (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ᬬᬾᬦ᭄ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ᬮᬶᬬ ᬬᬾᬦ᭄ (line[col_start] == quotechar)
	    bha_nga;
    }
    ᬯᬮᬶ col_start;
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
    ᬬᬾᬦ᭄ (VIsual_active)
    {
	// this only works within one line
	ᬬᬾᬦ᭄ (VIsual.lnum != curwin->w_cursor.lnum)
	    ᬯᬮᬶ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ᬬᬾᬦ᭄ (*p_sel == 'e')
	{
	    ᬬᬾᬦ᭄ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ᬬᬾᬦ᭄ (!vis_bef_curs && !vis_empty)
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

    ᬬᬾᬦ᭄ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ᬬᬾᬦ᭄ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ᬮᬶᬬ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ᬲᭂᬤᭂᬮ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ᬬᬾᬦ᭄ (line[i] == NUL)
		bha_nga;
	    ᬬᬾᬦ᭄ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    ᬬᬾᬦ᭄ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ᬬᬾᬦ᭄ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ᬬᬾᬦ᭄ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ᬬᬾᬦ᭄ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ᬮᬶᬬ
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ᬬᬾᬦ᭄ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ᬬᬾᬦ᭄ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ᬮᬶᬬ

    ᬬᬾᬦ᭄ (line[col_start] == quotechar || !vis_empty)
    {
	puur_naa_nka	first_col = col_start;

	ᬬᬾᬦ᭄ (!vis_empty)
	{
	    ᬬᬾᬦ᭄ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    ᬮᬶᬬ
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
	    ᬬᬾᬦ᭄ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ᬬᬾᬦ᭄ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ᬬᬾᬦ᭄ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    ᬮᬶᬬ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ᬬᬾᬦ᭄ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ᬬᬾᬦ᭄ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ᬬᬾᬦ᭄ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ᬬᬾᬦ᭄ (include)
    {
	ᬬᬾᬦ᭄ (VIM_ISWHITE(line[col_end + 1]))
	    ᬲᭂᬤᭂᬮ (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	ᬮᬶᬬ
	    ᬲᭂᬤᭂᬮ (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ᬬᬾᬦ᭄ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ᬬᬾᬦ᭄ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ᬬᬾᬦ᭄ (vis_empty
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
    ᬮᬶᬬ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ᬬᬾᬦ᭄ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ᬬᬾᬦ᭄ (VIsual_active)
    {
	ᬬᬾᬦ᭄ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ᬬᬾᬦ᭄ (*p_sel != 'e')
		dec_cursor();
	}
	ᬮᬶᬬ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ᬬᬾᬦ᭄ (inside_quotes
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
	ᬬᬾᬦ᭄ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ᬮᬶᬬ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ᬯᬮᬶ OK;

abort_search:
    ᬬᬾᬦ᭄ (VIsual_active && *p_sel == 'e')
    {
	ᬬᬾᬦ᭄ (did_exclusive_adj)
	    inc_cursor();
	ᬬᬾᬦ᭄ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ᬯᬮᬶ FALSE;
}
