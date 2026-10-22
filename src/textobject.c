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

static цяло cls(void);
static цяло skip_chars(цяло, цяло);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    цяло
findsent(цяло dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    цяло		c;
    цяло		(*func)(pos_T *);
    цяло		startlnum;
    цяло		noskip = FALSE;	    // do not skip blanks
    цяло		cpo_J;
    цяло		found_dot;

    pos = curwin->w_cursor;
    ако (dir == FORWARD)
	func = incl;
    иначе
	func = decl;

    докато (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ако (gchar_pos(&pos) == NUL)
	{
	    do
		ако ((*func)(&pos) == -1)
		    break;
	    докато (gchar_pos(&pos) == NUL);
	    ако (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	иначе ако (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ако (pos.lnum == curbuf->b_ml.ml_line_count)
		върни FAIL;
	    ++pos.lnum;
	    goto found;
	}
	иначе ако (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	докато (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ако (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    ако (found_dot)
		break;
	    ако (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ако (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	за (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ако (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ако (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    ако (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ако ((c = inc(&tpos)) == -1)
			break;
		докато (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ако (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ако (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    ако ((*func)(&pos) == -1)
	    {
		ако (count)
		    върни FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	докато (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ако (incl(&pos) == -1)
		break;

	ако (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ако ((*func)(&pos) == -1)
	    {
		ако (count)
		    върни FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    върни OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    цяло
findpar(
    цяло		*pincl,	    // Return: TRUE if last char is to be included
    цяло		dir,
    long	count,
    цяло		what,
    цяло		both)
{
    linenr_T	curr;
    цяло		did_skip;   // TRUE after separating lines have been skipped
    цяло		first;	    // TRUE on first line
    цяло		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    цяло		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    докато (count--)
    {
	did_skip = FALSE;
	за (first = TRUE; ; first = FALSE)
	{
	    ако (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ако (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ако (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    ако (fold_skipped)
		curr -= dir;
#endif
	    ако ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ако (count)
		    върни FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    ако (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ако (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ако ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    иначе
	curwin->w_cursor.col = 0;
    върни TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static цяло
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    за (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ако (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    break;
	++macro;
	ако (macro[0] == NUL)
	    break;
    }
    върни (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    цяло
startPS(linenr_T lnum, цяло para, цяло both)
{
    char_u	*s;

    s = ml_get(lnum);
    ако (*s == para || *s == '\f' || (both && *s == '}'))
	върни TRUE;
    ако (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	върни TRUE;
    върни FALSE;
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

static цяло	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static цяло
cls(void)
{
    цяло	    c;

    c = gchar_cursor();
    ако (c == ' ' || c == '\t' || c == NUL)
	върни 0;
    ако (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ако (enc_dbcs == DBCS_KOR && cls_bigword)
	    върни 1;

	// process code leading/trailing bytes
	върни dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ако (enc_utf8)
    {
	c = utf_class(c);
	ако (c != 0 && cls_bigword)
	    върни 1;
	върни c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ако (cls_bigword)
	върни 1;

    ако (vim_iswordc(c))
	върни 2;
    върни 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    цяло
fwd_word(
    long	count,
    цяло		bigword,    // "W", "E" or "B"
    цяло		eol)
{
    цяло		sclass;	    // starting class
    цяло		i;
    цяло		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    докато (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ако (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ако (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    върни FAIL;
	ако (i >= 1 && eol && count == 0)      // started at last char in line
	    върни OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ако (sclass != 0)
	    докато (cls() == sclass)
	    {
		i = inc_cursor();
		ако (i == -1 || (i >= 1 && eol && count == 0))
		    върни OK;
	    }

	/*
	 * go to next non-white
	 */
	докато (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ако (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    ако (i == -1 || (i >= 1 && eol && count == 0))
		върни OK;
	}
    }
    върни OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    цяло
bck_word(long count, цяло bigword, цяло stop)
{
    цяло		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    докато (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ако (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ако (dec_cursor() == -1)		// started at start of file
	    върни FAIL;

	ако (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    докато (cls() == 0)
	    {
		ако (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ако (dec_cursor() == -1) // hit start of file, stop here
		    върни OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ако (skip_chars(cls(), BACKWARD))
		върни OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    върни OK;
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
    цяло
end_word(
    long	count,
    цяло		bigword,
    цяло		stop,
    цяло		empty)
{
    цяло		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ако (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    докато (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ако (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ако (inc_cursor() == -1)
	    върни FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ако (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ако (skip_chars(sclass, FORWARD))
		върни FAIL;
	}
	иначе ако (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    докато (cls() == 0)
	    {
		ако (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ако (inc_cursor() == -1)	    // hit end of file, stop here
		    върни FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ако (skip_chars(cls(), FORWARD))
		върни FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    върни OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    цяло
bckend_word(
    long	count,
    цяло		bigword,    // TRUE for "B"
    цяло		eol)	    // TRUE: stop at end of line.
{
    цяло		sclass;	    // starting class
    цяло		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    докато (--count >= 0)
    {
	sclass = cls();
	ако ((i = dec_cursor()) == -1)
	    върни FAIL;
	ако (eol && i == 1)
	    върни OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ако (sclass != 0)
	{
	    докато (cls() == sclass)
		ако ((i = dec_cursor()) == -1 || (eol && i == 1))
		    върни OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	докато (cls() == 0)
	{
	    ако (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    ако ((i = dec_cursor()) == -1 || (eol && i == 1))
		върни OK;
	}
    }
    adjust_skipcol();
    върни OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static цяло
skip_chars(цяло cclass, цяло dir)
{
    докато (cls() == cclass)
	ако ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    върни TRUE;
    върни FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    цяло		sclass;		    // starting class

    sclass = cls();
    за (;;)
    {
	ако (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	ако (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    цяло	    c;

    докато (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ако (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    break;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    static void
findsent_forward(
    long    count,
    цяло	    at_start_sent)	// cursor is at start of sentence
{
    докато (count--)
    {
	findsent(FORWARD, 1L);
	ако (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ако (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    цяло
current_word(
    oparg_T	*oap,
    long	count,
    цяло		include,	// TRUE: include word and white space
    цяло		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    цяло		inclusive = TRUE;
    цяло		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ако (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ако (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	ако ((cls() == 0) == include)
	{
	    ако (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		върни FAIL;
	}
	иначе
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ако (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    иначе
		oneleft();

	    ако (include)
		include_white = TRUE;
	}

	ако (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	иначе
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    докато (count > 0)
    {
	inclusive = TRUE;
	ако (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ако (decl(&curwin->w_cursor) == -1)
		върни FAIL;
	    ако (include != (cls() != 0))
	    {
		ако (bck_word(1L, bigword, TRUE) == FAIL)
		    върни FAIL;
	    }
	    иначе
	    {
		ако (bckend_word(1L, bigword, TRUE) == FAIL)
		    върни FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	иначе
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ако (incl(&curwin->w_cursor) == -1)
		върни FAIL;
	    ако (include != (cls() == 0))
	    {
		ако (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    върни FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ако (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    иначе
	    {
		ако (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    върни FAIL;
	    }
	}
	--count;
    }

    ако (include_white && (cls() != 0
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
	ако (oneleft() == OK)
	{
	    back_in_line();
	    ако (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ако (VIsual_active)
		    VIsual = curwin->w_cursor;
		иначе
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ако (VIsual_active)
    {
	ако (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ако (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    иначе
	oap->inclusive = inclusive;

    върни OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    цяло
current_sent(oparg_T *oap, long count, цяло include)
{
    pos_T	start_pos;
    pos_T	pos;
    цяло		start_blank;
    цяло		c;
    цяло		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ако (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ако (LT_POS(start_pos, VIsual))
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
	    докато (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ако (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    ако (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ако (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		иначе
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ако (include)	// "as" gets twice as much as "is"
		count *= 2;
	    докато (count--)
	    {
		ако (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ако (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	иначе
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
	    ако (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		докато (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ако (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		ако (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		иначе		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ако (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ако (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	върни OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    докато (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ако (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    иначе
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ако (include)
	ncount = count * 2;
    иначе
    {
	ncount = count;
	ако (start_blank)
	    --ncount;
    }
    ако (ncount > 0)
	findsent_forward(ncount, TRUE);
    иначе
	decl(&curwin->w_cursor);

    ако (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ако (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ако (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	иначе ако (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ако (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ако (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ако (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    иначе
    {
	// include a newline after the sentence, if there is one
	ако (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	иначе
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    върни OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    цяло
current_block(
    oparg_T	*oap,
    long	count,
    цяло		include,	// TRUE == include white space
    цяло		what,		// '(', '{', etc.
    цяло		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    цяло		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ако (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ако (what == '{')		// ignore indent
	    докато (inindent(1))
		ако (inc_cursor() != 0)
		    break;
	ако (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    иначе ако (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    иначе
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ако ((pos = findmatch(NULL, what)) != NULL)
    {
	докато (count-- > 0)
	{
	    ако ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    иначе
    {
	докато (count-- > 0)
	{
	    ако ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ако (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	върни FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    докато (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	докато (inindent(1))
	{
	    sol = TRUE;
	    ако (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ако (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    върни FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ако (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ако ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		върни FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ако ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		върни FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	иначе
	    break;
    }

    ако (VIsual_active)
    {
	ако (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ако (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    иначе
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ако (sol)
	    incl(&curwin->w_cursor);
	иначе ако (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	иначе
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    върни OK;
}

#ако defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static цяло
in_html_tag(
    цяло		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    цяло		c;
    цяло		lc = NUL;
    pos_T	pos;

    ако (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	за (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ако (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ако (*p != '<')	    // check for '<' under cursor
	{
	    ако (lc != '<')
		върни FALSE;
	    p = lp;
	}
    }
    иначе
    {
	за (p = line + curwin->w_cursor.col; p > line; )
	{
	    ако (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    ако (*p == '>')	// find '>' before cursor
		break;
	}
	ако (*p != '<')
	    върни FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ако (end_tag)
	// check that there is a '/' after the '<'
	върни *p == '/';

    // check that there is no '/' after the '<'
    ако (*p == '/')
	върни FALSE;

    // check that the matching '>' is not preceded by '/'
    за (;;)
    {
	ако (inc(&pos) < 0)
	    върни FALSE;
	c = *ml_get_pos(&pos);
	ако (c == '>')
	    break;
	lc = c;
    }
    върни lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    цяло
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    цяло		include)	// TRUE == include white space
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
    цяло		len;
    цяло		r;
    цяло		do_include = include;
    цяло		save_p_ws = p_ws;
    цяло		retval = FAIL;
    цяло		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ако (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ако (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	докато (inindent(1))
	    ако (inc_cursor() != 0)
		break;

	ако (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    докато (*ml_get_cursor() != '>')
		ако (inc_cursor() < 0)
		    break;
	}
	иначе ако (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    докато (*ml_get_cursor() != '<')
		ако (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    иначе ако (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    иначе
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    за (n = 0; n < count; ++n)
    {
	ако (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    за (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (цяло)(cp - p);
    ако (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ако (spat == NULL || epat == NULL)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	goto theend;
    }
    sprintf((char *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((char *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, NULL,
						    0, NULL, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ако (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ако (do_include)
    {
	// Include up to the '>'.
	докато (*ml_get_cursor() != '>')
	    ако (inc_cursor() < 0)
		break;
    }
    иначе
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ако (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	иначе ако (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ако (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	цяло in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	докато (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ако (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	    иначе ако (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ако (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ако (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ако (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	иначе ако (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    иначе
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ако (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	иначе
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    върни retval;
}
#endif

    цяло
current_par(
    oparg_T	*oap,
    long	count,
    цяло		include,	// TRUE == include white space
    цяло		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    цяло		white_in_front;
    цяло		dir;
    цяло		start_is_white;
    цяло		prev_start_is_white;
    цяло		retval = OK;
    цяло		do_white = FALSE;
    цяло		t;
    цяло		i;

    ако (type == 'S')	    // not implemented yet
	върни FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ако (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ако (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	иначе
	    dir = FORWARD;
	за (i = count; --i >= 0; )
	{
	    ако (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		break;
	    }

	    prev_start_is_white = -1;
	    за (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ако (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    break;
		}
		за (;;)
		{
		    ако (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			break;
		    ако (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			break;
		    start_lnum += dir;
		}
		ако (!include)
		    break;
		ако (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    break;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	върни retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    докато (start_lnum > 1)
    {
	ако (white_in_front)	    // stop at first white line
	{
	    ако (!linewhite(start_lnum - 1))
		break;
	}
	иначе		// stop at first non-white line of start of paragraph
	{
	    ако (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		break;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    докато (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ако (!include && white_in_front)
	--i;
    докато (i--)
    {
	ако (end_lnum == curbuf->b_ml.ml_line_count)
	    върни FAIL;

	ако (!include)
	    do_white = linewhite(end_lnum + 1);

	ако (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    докато (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ако (i == 0 && white_in_front && include)
	    break;

	/*
	 * skip to end of white lines after paragraph
	 */
	ако (include || do_white)
	    докато (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ако (!white_in_front && !linewhite(end_lnum) && include)
	докато (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ако (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ако (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ако (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    иначе
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    върни OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static цяло
find_next_quote(
    char_u	*line,
    цяло		col,
    цяло		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    цяло		c;

    за (;;)
    {
	c = line[col];
	ако (c == NUL)
	    върни -1;
	иначе ако (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ако (line[col] == NUL)
		върни -1;
	}
	иначе ако (c == quotechar)
	    break;
	ако (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	иначе
	    ++col;
    }
    върни col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static цяло
find_prev_quote(
    char_u	*line,
    цяло		col_start,
    цяло		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    цяло		n;

    докато (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ако (escape != NULL)
	    докато (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ако (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	иначе ако (line[col_start] == quotechar)
	    break;
    }
    върни col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    цяло
current_quote(
    oparg_T	*oap,
    long	count,
    цяло		include,	// TRUE == include quote char
    цяло		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    цяло		col_end;
    цяло		col_start = curwin->w_cursor.col;
    цяло		inclusive = FALSE;
    цяло		vis_empty = TRUE;	// Visual selection <= 1 char
    цяло		vis_bef_curs = FALSE;	// Visual starts before cursor
    цяло		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    цяло		inside_quotes = FALSE;	// Looks like "i'" done before
    цяло		selected_quote = FALSE;	// Has quote inside selection
    цяло		i;
    цяло		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ако (VIsual_active)
    {
	// this only works within one line
	ако (VIsual.lnum != curwin->w_cursor.lnum)
	    върни FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ако (*p_sel == 'e')
	{
	    ако (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    иначе ако (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ако (!vis_bef_curs && !vis_empty)
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

    ако (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ако (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	иначе
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	докато (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ако (line[i] == NUL)
		break;
	    ако (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		break;
	    }
	}
    }

    ако (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ако (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ако (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ако (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	иначе
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ако (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ако (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    иначе

    ако (line[col_start] == quotechar || !vis_empty)
    {
	цяло	first_col = col_start;

	ако (!vis_empty)
	{
	    ако (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    иначе
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	за (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ако (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ако (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ако (col_start <= first_col && first_col <= col_end)
		break;
	    col_start = col_end + 1;
	}
    }
    иначе
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ако (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ако (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ако (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ако (include)
    {
	ако (VIM_ISWHITE(line[col_end + 1]))
	    докато (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	иначе
	    докато (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ако (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ако (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ако (vis_empty
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
    иначе
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ако ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ако (VIsual_active)
    {
	ако (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ако (*p_sel != 'e')
		dec_cursor();
	}
	иначе
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ако (inside_quotes
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
	ако (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    иначе
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    върни OK;

abort_search:
    ако (VIsual_active && *p_sel == 'e')
    {
	ако (did_exclusive_adj)
	    inc_cursor();
	ако (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    върни FALSE;
}
