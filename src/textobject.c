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

static ціле cls(void);
static ціле skip_chars(ціле, ціле);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ціле
findsent(ціле dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ціле		c;
    ціле		(*func)(pos_T *);
    ціле		startlnum;
    ціле		noskip = FALSE;	    // do not skip blanks
    ціле		cpo_J;
    ціле		found_dot;

    pos = curwin->w_cursor;
    якщо (dir == FORWARD)
	func = incl;
    інакше
	func = decl;

    поки (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	якщо (gchar_pos(&pos) == NUL)
	{
	    do
		якщо ((*func)(&pos) == -1)
		    break;
	    поки (gchar_pos(&pos) == NUL);
	    якщо (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	інакше якщо (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    якщо (pos.lnum == curbuf->b_ml.ml_line_count)
		повернути FAIL;
	    ++pos.lnum;
	    goto found;
	}
	інакше якщо (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	поки (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    якщо (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    якщо (found_dot)
		break;
	    якщо (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    якщо (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	для (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    якщо (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		якщо (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    якщо (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    якщо ((c = inc(&tpos)) == -1)
			break;
		поки (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		якщо (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    якщо (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    якщо ((*func)(&pos) == -1)
	    {
		якщо (count)
		    повернути FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	поки (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    якщо (incl(&pos) == -1)
		break;

	якщо (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    якщо ((*func)(&pos) == -1)
	    {
		якщо (count)
		    повернути FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    повернути OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ціле
findpar(
    ціле		*pincl,	    // Return: TRUE if last char is to be included
    ціле		dir,
    long	count,
    ціле		what,
    ціле		both)
{
    linenr_T	curr;
    ціле		did_skip;   // TRUE after separating lines have been skipped
    ціле		first;	    // TRUE on first line
    ціле		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ціле		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    поки (count--)
    {
	did_skip = FALSE;
	для (first = TRUE; ; first = FALSE)
	{
	    якщо (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    якщо (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    якщо (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    якщо (fold_skipped)
		curr -= dir;
#endif
	    якщо ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		якщо (count)
		    повернути FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    якщо (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    якщо (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	якщо ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    інакше
	curwin->w_cursor.col = 0;
    повернути TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ціле
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    для (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	якщо (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    break;
	++macro;
	якщо (macro[0] == NUL)
	    break;
    }
    повернути (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ціле
startPS(linenr_T lnum, ціле para, ціле both)
{
    char_u	*s;

    s = ml_get(lnum);
    якщо (*s == para || *s == '\f' || (both && *s == '}'))
	повернути TRUE;
    якщо (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	повернути TRUE;
    повернути FALSE;
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

static ціле	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static ціле
cls(void)
{
    ціле	    c;

    c = gchar_cursor();
    якщо (c == ' ' || c == '\t' || c == NUL)
	повернути 0;
    якщо (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	якщо (enc_dbcs == DBCS_KOR && cls_bigword)
	    повернути 1;

	// process code leading/trailing bytes
	повернути dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    якщо (enc_utf8)
    {
	c = utf_class(c);
	якщо (c != 0 && cls_bigword)
	    повернути 1;
	повернути c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    якщо (cls_bigword)
	повернути 1;

    якщо (vim_iswordc(c))
	повернути 2;
    повернути 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ціле
fwd_word(
    long	count,
    ціле		bigword,    // "W", "E" or "B"
    ціле		eol)
{
    ціле		sclass;	    // starting class
    ціле		i;
    ціле		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    поки (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	якщо (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	якщо (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    повернути FAIL;
	якщо (i >= 1 && eol && count == 0)      // started at last char in line
	    повернути OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	якщо (sclass != 0)
	    поки (cls() == sclass)
	    {
		i = inc_cursor();
		якщо (i == -1 || (i >= 1 && eol && count == 0))
		    повернути OK;
	    }

	/*
	 * go to next non-white
	 */
	поки (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    якщо (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    якщо (i == -1 || (i >= 1 && eol && count == 0))
		повернути OK;
	}
    }
    повернути OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ціле
bck_word(long count, ціле bigword, ціле stop)
{
    ціле		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    поки (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	якщо (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	якщо (dec_cursor() == -1)		// started at start of file
	    повернути FAIL;

	якщо (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    поки (cls() == 0)
	    {
		якщо (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		якщо (dec_cursor() == -1) // hit start of file, stop here
		    повернути OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    якщо (skip_chars(cls(), BACKWARD))
		повернути OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    повернути OK;
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
    ціле
end_word(
    long	count,
    ціле		bigword,
    ціле		stop,
    ціле		empty)
{
    ціле		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    якщо (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    поки (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	якщо (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	якщо (inc_cursor() == -1)
	    повернути FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	якщо (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    якщо (skip_chars(sclass, FORWARD))
		повернути FAIL;
	}
	інакше якщо (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    поки (cls() == 0)
	    {
		якщо (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		якщо (inc_cursor() == -1)	    // hit end of file, stop here
		    повернути FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    якщо (skip_chars(cls(), FORWARD))
		повернути FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    повернути OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ціле
bckend_word(
    long	count,
    ціле		bigword,    // TRUE for "B"
    ціле		eol)	    // TRUE: stop at end of line.
{
    ціле		sclass;	    // starting class
    ціле		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    поки (--count >= 0)
    {
	sclass = cls();
	якщо ((i = dec_cursor()) == -1)
	    повернути FAIL;
	якщо (eol && i == 1)
	    повернути OK;

	/*
	 * Move backward to before the start of this word.
	 */
	якщо (sclass != 0)
	{
	    поки (cls() == sclass)
		якщо ((i = dec_cursor()) == -1 || (eol && i == 1))
		    повернути OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	поки (cls() == 0)
	{
	    якщо (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    якщо ((i = dec_cursor()) == -1 || (eol && i == 1))
		повернути OK;
	}
    }
    adjust_skipcol();
    повернути OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ціле
skip_chars(ціле cclass, ціле dir)
{
    поки (cls() == cclass)
	якщо ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    повернути TRUE;
    повернути FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ціле		sclass;		    // starting class

    sclass = cls();
    для (;;)
    {
	якщо (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	якщо (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    ціле	    c;

    поки (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	якщо (!VIM_ISWHITE(c))
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
    ціле	    at_start_sent)	// cursor is at start of sentence
{
    поки (count--)
    {
	findsent(FORWARD, 1L);
	якщо (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	якщо (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ціле
current_word(
    oparg_T	*oap,
    long	count,
    ціле		include,	// TRUE: include word and white space
    ціле		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ціле		inclusive = TRUE;
    ціле		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    якщо (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    якщо (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	якщо ((cls() == 0) == include)
	{
	    якщо (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		повернути FAIL;
	}
	інакше
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    якщо (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    інакше
		oneleft();

	    якщо (include)
		include_white = TRUE;
	}

	якщо (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	інакше
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    поки (count > 0)
    {
	inclusive = TRUE;
	якщо (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    якщо (decl(&curwin->w_cursor) == -1)
		повернути FAIL;
	    якщо (include != (cls() != 0))
	    {
		якщо (bck_word(1L, bigword, TRUE) == FAIL)
		    повернути FAIL;
	    }
	    інакше
	    {
		якщо (bckend_word(1L, bigword, TRUE) == FAIL)
		    повернути FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	інакше
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    якщо (incl(&curwin->w_cursor) == -1)
		повернути FAIL;
	    якщо (include != (cls() == 0))
	    {
		якщо (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    повернути FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		якщо (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    інакше
	    {
		якщо (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    повернути FAIL;
	    }
	}
	--count;
    }

    якщо (include_white && (cls() != 0
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
	якщо (oneleft() == OK)
	{
	    back_in_line();
	    якщо (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		якщо (VIsual_active)
		    VIsual = curwin->w_cursor;
		інакше
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    якщо (VIsual_active)
    {
	якщо (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	якщо (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    інакше
	oap->inclusive = inclusive;

    повернути OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ціле
current_sent(oparg_T *oap, long count, ціле include)
{
    pos_T	start_pos;
    pos_T	pos;
    ціле		start_blank;
    ціле		c;
    ціле		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    якщо (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	якщо (LT_POS(start_pos, VIsual))
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
	    поки (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		якщо (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    якщо (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		якщо (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		інакше
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    якщо (include)	// "as" gets twice as much as "is"
		count *= 2;
	    поки (count--)
	    {
		якщо (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		якщо (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	інакше
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
	    якщо (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		поки (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    якщо (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		якщо (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		інакше		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    якщо (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    якщо (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	повернути OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    поки (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    якщо (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    інакше
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    якщо (include)
	ncount = count * 2;
    інакше
    {
	ncount = count;
	якщо (start_blank)
	    --ncount;
    }
    якщо (ncount > 0)
	findsent_forward(ncount, TRUE);
    інакше
	decl(&curwin->w_cursor);

    якщо (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	якщо (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    якщо (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	інакше якщо (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    якщо (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	якщо (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	якщо (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    інакше
    {
	// include a newline after the sentence, if there is one
	якщо (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	інакше
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    повернути OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ціле
current_block(
    oparg_T	*oap,
    long	count,
    ціле		include,	// TRUE == include white space
    ціле		what,		// '(', '{', etc.
    ціле		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ціле		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    якщо (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	якщо (what == '{')		// ignore indent
	    поки (inindent(1))
		якщо (inc_cursor() != 0)
		    break;
	якщо (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    інакше якщо (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    інакше
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    якщо ((pos = findmatch(NULL, what)) != NULL)
    {
	поки (count-- > 0)
	{
	    якщо ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    інакше
    {
	поки (count-- > 0)
	{
	    якщо ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    якщо (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	повернути FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    поки (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	поки (inindent(1))
	{
	    sol = TRUE;
	    якщо (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	якщо (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    повернути FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	якщо (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    якщо ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		повернути FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    якщо ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		повернути FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	інакше
	    break;
    }

    якщо (VIsual_active)
    {
	якщо (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	якщо (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    інакше
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	якщо (sol)
	    incl(&curwin->w_cursor);
	інакше якщо (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	інакше
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    повернути OK;
}

#якщо defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static ціле
in_html_tag(
    ціле		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    ціле		c;
    ціле		lc = NUL;
    pos_T	pos;

    якщо (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	для (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    якщо (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	якщо (*p != '<')	    // check for '<' under cursor
	{
	    якщо (lc != '<')
		повернути FALSE;
	    p = lp;
	}
    }
    інакше
    {
	для (p = line + curwin->w_cursor.col; p > line; )
	{
	    якщо (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    якщо (*p == '>')	// find '>' before cursor
		break;
	}
	якщо (*p != '<')
	    повернути FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    якщо (end_tag)
	// check that there is a '/' after the '<'
	повернути *p == '/';

    // check that there is no '/' after the '<'
    якщо (*p == '/')
	повернути FALSE;

    // check that the matching '>' is not preceded by '/'
    для (;;)
    {
	якщо (inc(&pos) < 0)
	    повернути FALSE;
	c = *ml_get_pos(&pos);
	якщо (c == '>')
	    break;
	lc = c;
    }
    повернути lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ціле
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    ціле		include)	// TRUE == include white space
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
    ціле		len;
    ціле		r;
    ціле		do_include = include;
    ціле		save_p_ws = p_ws;
    ціле		retval = FAIL;
    ціле		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    якщо (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    якщо (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	поки (inindent(1))
	    якщо (inc_cursor() != 0)
		break;

	якщо (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    поки (*ml_get_cursor() != '>')
		якщо (inc_cursor() < 0)
		    break;
	}
	інакше якщо (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    поки (*ml_get_cursor() != '<')
		якщо (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    інакше якщо (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    інакше
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    для (n = 0; n < count; ++n)
    {
	якщо (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    для (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ціле)(cp - p);
    якщо (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    якщо (spat == NULL || epat == NULL)
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

    якщо (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    якщо (do_include)
    {
	// Include up to the '>'.
	поки (*ml_get_cursor() != '>')
	    якщо (inc_cursor() < 0)
		break;
    }
    інакше
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	якщо (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	інакше якщо (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    якщо (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ціле in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	поки (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    якщо (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	    інакше якщо (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	якщо (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    якщо (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	якщо (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	інакше якщо (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    інакше
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	якщо (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	інакше
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    повернути retval;
}
#endif

    ціле
current_par(
    oparg_T	*oap,
    long	count,
    ціле		include,	// TRUE == include white space
    ціле		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ціле		white_in_front;
    ціле		dir;
    ціле		start_is_white;
    ціле		prev_start_is_white;
    ціле		retval = OK;
    ціле		do_white = FALSE;
    ціле		t;
    ціле		i;

    якщо (type == 'S')	    // not implemented yet
	повернути FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    якщо (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	якщо (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	інакше
	    dir = FORWARD;
	для (i = count; --i >= 0; )
	{
	    якщо (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		break;
	    }

	    prev_start_is_white = -1;
	    для (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		якщо (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    break;
		}
		для (;;)
		{
		    якщо (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			break;
		    якщо (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			break;
		    start_lnum += dir;
		}
		якщо (!include)
		    break;
		якщо (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    break;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	повернути retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    поки (start_lnum > 1)
    {
	якщо (white_in_front)	    // stop at first white line
	{
	    якщо (!linewhite(start_lnum - 1))
		break;
	}
	інакше		// stop at first non-white line of start of paragraph
	{
	    якщо (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		break;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    поки (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    якщо (!include && white_in_front)
	--i;
    поки (i--)
    {
	якщо (end_lnum == curbuf->b_ml.ml_line_count)
	    повернути FAIL;

	якщо (!include)
	    do_white = linewhite(end_lnum + 1);

	якщо (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    поки (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	якщо (i == 0 && white_in_front && include)
	    break;

	/*
	 * skip to end of white lines after paragraph
	 */
	якщо (include || do_white)
	    поки (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    якщо (!white_in_front && !linewhite(end_lnum) && include)
	поки (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    якщо (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	якщо (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	якщо (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    інакше
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    повернути OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static ціле
find_next_quote(
    char_u	*line,
    ціле		col,
    ціле		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ціле		c;

    для (;;)
    {
	c = line[col];
	якщо (c == NUL)
	    повернути -1;
	інакше якщо (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    якщо (line[col] == NUL)
		повернути -1;
	}
	інакше якщо (c == quotechar)
	    break;
	якщо (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	інакше
	    ++col;
    }
    повернути col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static ціле
find_prev_quote(
    char_u	*line,
    ціле		col_start,
    ціле		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ціле		n;

    поки (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	якщо (escape != NULL)
	    поки (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	якщо (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	інакше якщо (line[col_start] == quotechar)
	    break;
    }
    повернути col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ціле
current_quote(
    oparg_T	*oap,
    long	count,
    ціле		include,	// TRUE == include quote char
    ціле		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    ціле		col_end;
    ціле		col_start = curwin->w_cursor.col;
    ціле		inclusive = FALSE;
    ціле		vis_empty = TRUE;	// Visual selection <= 1 char
    ціле		vis_bef_curs = FALSE;	// Visual starts before cursor
    ціле		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ціле		inside_quotes = FALSE;	// Looks like "i'" done before
    ціле		selected_quote = FALSE;	// Has quote inside selection
    ціле		i;
    ціле		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    якщо (VIsual_active)
    {
	// this only works within one line
	якщо (VIsual.lnum != curwin->w_cursor.lnum)
	    повернути FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	якщо (*p_sel == 'e')
	{
	    якщо (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    інакше якщо (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    якщо (!vis_bef_curs && !vis_empty)
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

    якщо (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	якщо (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	інакше
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	поки (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    якщо (line[i] == NUL)
		break;
	    якщо (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		break;
	    }
	}
    }

    якщо (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	якщо (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    якщо (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    якщо (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	інакше
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    якщо (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    якщо (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    інакше

    якщо (line[col_start] == quotechar || !vis_empty)
    {
	ціле	first_col = col_start;

	якщо (!vis_empty)
	{
	    якщо (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    інакше
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	для (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    якщо (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    якщо (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    якщо (col_start <= first_col && first_col <= col_end)
		break;
	    col_start = col_end + 1;
	}
    }
    інакше
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	якщо (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    якщо (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	якщо (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    якщо (include)
    {
	якщо (VIM_ISWHITE(line[col_end + 1]))
	    поки (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	інакше
	    поки (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    якщо (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    якщо (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	якщо (vis_empty
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
    інакше
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    якщо ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    якщо (VIsual_active)
    {
	якщо (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    якщо (*p_sel != 'e')
		dec_cursor();
	}
	інакше
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    якщо (inside_quotes
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
	якщо (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    інакше
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    повернути OK;

abort_search:
    якщо (VIsual_active && *p_sel == 'e')
    {
	якщо (did_exclusive_adj)
	    inc_cursor();
	якщо (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    повернути FALSE;
}
