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

static số_nguyên cls(void);
static số_nguyên skip_chars(số_nguyên, số_nguyên);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    số_nguyên
findsent(số_nguyên dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    số_nguyên		c;
    số_nguyên		(*func)(pos_T *);
    số_nguyên		startlnum;
    số_nguyên		noskip = FALSE;	    // do not skip blanks
    số_nguyên		cpo_J;
    số_nguyên		found_dot;

    pos = curwin->w_cursor;
    nếu (dir == FORWARD)
	func = incl;
    không_thì
	func = decl;

    trong_khi (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	nếu (gchar_pos(&pos) == NUL)
	{
	    do
		nếu ((*func)(&pos) == -1)
		    break;
	    trong_khi (gchar_pos(&pos) == NUL);
	    nếu (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	không_thì nếu (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    nếu (pos.lnum == curbuf->b_ml.ml_line_count)
		trả_về FAIL;
	    ++pos.lnum;
	    goto found;
	}
	không_thì nếu (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	trong_khi (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    nếu (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    nếu (found_dot)
		break;
	    nếu (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    nếu (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	cho (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    nếu (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		nếu (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    nếu (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    nếu ((c = inc(&tpos)) == -1)
			break;
		trong_khi (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		nếu (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    nếu (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    nếu ((*func)(&pos) == -1)
	    {
		nếu (count)
		    trả_về FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	trong_khi (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    nếu (incl(&pos) == -1)
		break;

	nếu (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    nếu ((*func)(&pos) == -1)
	    {
		nếu (count)
		    trả_về FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    trả_về OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    số_nguyên
findpar(
    số_nguyên		*pincl,	    // Return: TRUE if last char is to be included
    số_nguyên		dir,
    long	count,
    số_nguyên		what,
    số_nguyên		both)
{
    linenr_T	curr;
    số_nguyên		did_skip;   // TRUE after separating lines have been skipped
    số_nguyên		first;	    // TRUE on first line
    số_nguyên		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    số_nguyên		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    trong_khi (count--)
    {
	did_skip = FALSE;
	cho (first = TRUE; ; first = FALSE)
	{
	    nếu (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    nếu (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    nếu (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    nếu (fold_skipped)
		curr -= dir;
#endif
	    nếu ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		nếu (count)
		    trả_về FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    nếu (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    nếu (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	nếu ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    không_thì
	curwin->w_cursor.col = 0;
    trả_về TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static số_nguyên
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    cho (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	nếu (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    break;
	++macro;
	nếu (macro[0] == NUL)
	    break;
    }
    trả_về (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    số_nguyên
startPS(linenr_T lnum, số_nguyên para, số_nguyên both)
{
    char_u	*s;

    s = ml_get(lnum);
    nếu (*s == para || *s == '\f' || (both && *s == '}'))
	trả_về TRUE;
    nếu (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	trả_về TRUE;
    trả_về FALSE;
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

static số_nguyên	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static số_nguyên
cls(void)
{
    số_nguyên	    c;

    c = gchar_cursor();
    nếu (c == ' ' || c == '\t' || c == NUL)
	trả_về 0;
    nếu (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	nếu (enc_dbcs == DBCS_KOR && cls_bigword)
	    trả_về 1;

	// process code leading/trailing bytes
	trả_về dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    nếu (enc_utf8)
    {
	c = utf_class(c);
	nếu (c != 0 && cls_bigword)
	    trả_về 1;
	trả_về c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    nếu (cls_bigword)
	trả_về 1;

    nếu (vim_iswordc(c))
	trả_về 2;
    trả_về 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    số_nguyên
fwd_word(
    long	count,
    số_nguyên		bigword,    // "W", "E" or "B"
    số_nguyên		eol)
{
    số_nguyên		sclass;	    // starting class
    số_nguyên		i;
    số_nguyên		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    trong_khi (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	nếu (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	nếu (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    trả_về FAIL;
	nếu (i >= 1 && eol && count == 0)      // started at last char in line
	    trả_về OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	nếu (sclass != 0)
	    trong_khi (cls() == sclass)
	    {
		i = inc_cursor();
		nếu (i == -1 || (i >= 1 && eol && count == 0))
		    trả_về OK;
	    }

	/*
	 * go to next non-white
	 */
	trong_khi (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    nếu (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    nếu (i == -1 || (i >= 1 && eol && count == 0))
		trả_về OK;
	}
    }
    trả_về OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    số_nguyên
bck_word(long count, số_nguyên bigword, số_nguyên stop)
{
    số_nguyên		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    trong_khi (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	nếu (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	nếu (dec_cursor() == -1)		// started at start of file
	    trả_về FAIL;

	nếu (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    trong_khi (cls() == 0)
	    {
		nếu (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		nếu (dec_cursor() == -1) // hit start of file, stop here
		    trả_về OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    nếu (skip_chars(cls(), BACKWARD))
		trả_về OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    trả_về OK;
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
    số_nguyên
end_word(
    long	count,
    số_nguyên		bigword,
    số_nguyên		stop,
    số_nguyên		empty)
{
    số_nguyên		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    nếu (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    trong_khi (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	nếu (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	nếu (inc_cursor() == -1)
	    trả_về FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	nếu (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    nếu (skip_chars(sclass, FORWARD))
		trả_về FAIL;
	}
	không_thì nếu (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    trong_khi (cls() == 0)
	    {
		nếu (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		nếu (inc_cursor() == -1)	    // hit end of file, stop here
		    trả_về FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    nếu (skip_chars(cls(), FORWARD))
		trả_về FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    trả_về OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    số_nguyên
bckend_word(
    long	count,
    số_nguyên		bigword,    // TRUE for "B"
    số_nguyên		eol)	    // TRUE: stop at end of line.
{
    số_nguyên		sclass;	    // starting class
    số_nguyên		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    trong_khi (--count >= 0)
    {
	sclass = cls();
	nếu ((i = dec_cursor()) == -1)
	    trả_về FAIL;
	nếu (eol && i == 1)
	    trả_về OK;

	/*
	 * Move backward to before the start of this word.
	 */
	nếu (sclass != 0)
	{
	    trong_khi (cls() == sclass)
		nếu ((i = dec_cursor()) == -1 || (eol && i == 1))
		    trả_về OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	trong_khi (cls() == 0)
	{
	    nếu (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    nếu ((i = dec_cursor()) == -1 || (eol && i == 1))
		trả_về OK;
	}
    }
    adjust_skipcol();
    trả_về OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static số_nguyên
skip_chars(số_nguyên cclass, số_nguyên dir)
{
    trong_khi (cls() == cclass)
	nếu ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    trả_về TRUE;
    trả_về FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    số_nguyên		sclass;		    // starting class

    sclass = cls();
    cho (;;)
    {
	nếu (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	nếu (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    số_nguyên	    c;

    trong_khi (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	nếu (!VIM_ISWHITE(c))
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
    số_nguyên	    at_start_sent)	// cursor is at start of sentence
{
    trong_khi (count--)
    {
	findsent(FORWARD, 1L);
	nếu (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	nếu (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    số_nguyên
current_word(
    oparg_T	*oap,
    long	count,
    số_nguyên		include,	// TRUE: include word and white space
    số_nguyên		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    số_nguyên		inclusive = TRUE;
    số_nguyên		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    nếu (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    nếu (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	nếu ((cls() == 0) == include)
	{
	    nếu (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		trả_về FAIL;
	}
	không_thì
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    nếu (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    không_thì
		oneleft();

	    nếu (include)
		include_white = TRUE;
	}

	nếu (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	không_thì
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    trong_khi (count > 0)
    {
	inclusive = TRUE;
	nếu (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    nếu (decl(&curwin->w_cursor) == -1)
		trả_về FAIL;
	    nếu (include != (cls() != 0))
	    {
		nếu (bck_word(1L, bigword, TRUE) == FAIL)
		    trả_về FAIL;
	    }
	    không_thì
	    {
		nếu (bckend_word(1L, bigword, TRUE) == FAIL)
		    trả_về FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	không_thì
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    nếu (incl(&curwin->w_cursor) == -1)
		trả_về FAIL;
	    nếu (include != (cls() == 0))
	    {
		nếu (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    trả_về FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		nếu (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    không_thì
	    {
		nếu (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    trả_về FAIL;
	    }
	}
	--count;
    }

    nếu (include_white && (cls() != 0
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
	nếu (oneleft() == OK)
	{
	    back_in_line();
	    nếu (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		nếu (VIsual_active)
		    VIsual = curwin->w_cursor;
		không_thì
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    nếu (VIsual_active)
    {
	nếu (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	nếu (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    không_thì
	oap->inclusive = inclusive;

    trả_về OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    số_nguyên
current_sent(oparg_T *oap, long count, số_nguyên include)
{
    pos_T	start_pos;
    pos_T	pos;
    số_nguyên		start_blank;
    số_nguyên		c;
    số_nguyên		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    nếu (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	nếu (LT_POS(start_pos, VIsual))
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
	    trong_khi (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		nếu (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    nếu (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		nếu (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		không_thì
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    nếu (include)	// "as" gets twice as much as "is"
		count *= 2;
	    trong_khi (count--)
	    {
		nếu (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		nếu (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	không_thì
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
	    nếu (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		trong_khi (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    nếu (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		nếu (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		không_thì		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    nếu (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    nếu (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	trả_về OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    trong_khi (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    nếu (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    không_thì
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    nếu (include)
	ncount = count * 2;
    không_thì
    {
	ncount = count;
	nếu (start_blank)
	    --ncount;
    }
    nếu (ncount > 0)
	findsent_forward(ncount, TRUE);
    không_thì
	decl(&curwin->w_cursor);

    nếu (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	nếu (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    nếu (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	không_thì nếu (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    nếu (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	nếu (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	nếu (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    không_thì
    {
	// include a newline after the sentence, if there is one
	nếu (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	không_thì
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    trả_về OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    số_nguyên
current_block(
    oparg_T	*oap,
    long	count,
    số_nguyên		include,	// TRUE == include white space
    số_nguyên		what,		// '(', '{', etc.
    số_nguyên		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    số_nguyên		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    nếu (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	nếu (what == '{')		// ignore indent
	    trong_khi (inindent(1))
		nếu (inc_cursor() != 0)
		    break;
	nếu (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    không_thì nếu (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    không_thì
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    nếu ((pos = findmatch(NULL, what)) != NULL)
    {
	trong_khi (count-- > 0)
	{
	    nếu ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    không_thì
    {
	trong_khi (count-- > 0)
	{
	    nếu ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    nếu (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	trả_về FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    trong_khi (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	trong_khi (inindent(1))
	{
	    sol = TRUE;
	    nếu (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	nếu (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    trả_về FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	nếu (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    nếu ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		trả_về FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    nếu ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		trả_về FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	không_thì
	    break;
    }

    nếu (VIsual_active)
    {
	nếu (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	nếu (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    không_thì
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	nếu (sol)
	    incl(&curwin->w_cursor);
	không_thì nếu (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	không_thì
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    trả_về OK;
}

#nếu defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static số_nguyên
in_html_tag(
    số_nguyên		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    số_nguyên		c;
    số_nguyên		lc = NUL;
    pos_T	pos;

    nếu (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	cho (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    nếu (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	nếu (*p != '<')	    // check for '<' under cursor
	{
	    nếu (lc != '<')
		trả_về FALSE;
	    p = lp;
	}
    }
    không_thì
    {
	cho (p = line + curwin->w_cursor.col; p > line; )
	{
	    nếu (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    nếu (*p == '>')	// find '>' before cursor
		break;
	}
	nếu (*p != '<')
	    trả_về FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    nếu (end_tag)
	// check that there is a '/' after the '<'
	trả_về *p == '/';

    // check that there is no '/' after the '<'
    nếu (*p == '/')
	trả_về FALSE;

    // check that the matching '>' is not preceded by '/'
    cho (;;)
    {
	nếu (inc(&pos) < 0)
	    trả_về FALSE;
	c = *ml_get_pos(&pos);
	nếu (c == '>')
	    break;
	lc = c;
    }
    trả_về lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    số_nguyên
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    số_nguyên		include)	// TRUE == include white space
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
    số_nguyên		len;
    số_nguyên		r;
    số_nguyên		do_include = include;
    số_nguyên		save_p_ws = p_ws;
    số_nguyên		retval = FAIL;
    số_nguyên		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    nếu (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    nếu (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	trong_khi (inindent(1))
	    nếu (inc_cursor() != 0)
		break;

	nếu (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    trong_khi (*ml_get_cursor() != '>')
		nếu (inc_cursor() < 0)
		    break;
	}
	không_thì nếu (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    trong_khi (*ml_get_cursor() != '<')
		nếu (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    không_thì nếu (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    không_thì
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    cho (n = 0; n < count; ++n)
    {
	nếu (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    cho (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (số_nguyên)(cp - p);
    nếu (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    nếu (spat == NULL || epat == NULL)
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

    nếu (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    nếu (do_include)
    {
	// Include up to the '>'.
	trong_khi (*ml_get_cursor() != '>')
	    nếu (inc_cursor() < 0)
		break;
    }
    không_thì
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	nếu (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	không_thì nếu (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    nếu (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	số_nguyên in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	trong_khi (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    nếu (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	    không_thì nếu (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	nếu (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    nếu (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	nếu (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	không_thì nếu (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    không_thì
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	nếu (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	không_thì
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    trả_về retval;
}
#endif

    số_nguyên
current_par(
    oparg_T	*oap,
    long	count,
    số_nguyên		include,	// TRUE == include white space
    số_nguyên		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    số_nguyên		white_in_front;
    số_nguyên		dir;
    số_nguyên		start_is_white;
    số_nguyên		prev_start_is_white;
    số_nguyên		retval = OK;
    số_nguyên		do_white = FALSE;
    số_nguyên		t;
    số_nguyên		i;

    nếu (type == 'S')	    // not implemented yet
	trả_về FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    nếu (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	nếu (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	không_thì
	    dir = FORWARD;
	cho (i = count; --i >= 0; )
	{
	    nếu (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		break;
	    }

	    prev_start_is_white = -1;
	    cho (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		nếu (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    break;
		}
		cho (;;)
		{
		    nếu (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			break;
		    nếu (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			break;
		    start_lnum += dir;
		}
		nếu (!include)
		    break;
		nếu (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    break;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	trả_về retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    trong_khi (start_lnum > 1)
    {
	nếu (white_in_front)	    // stop at first white line
	{
	    nếu (!linewhite(start_lnum - 1))
		break;
	}
	không_thì		// stop at first non-white line of start of paragraph
	{
	    nếu (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		break;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    trong_khi (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    nếu (!include && white_in_front)
	--i;
    trong_khi (i--)
    {
	nếu (end_lnum == curbuf->b_ml.ml_line_count)
	    trả_về FAIL;

	nếu (!include)
	    do_white = linewhite(end_lnum + 1);

	nếu (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    trong_khi (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	nếu (i == 0 && white_in_front && include)
	    break;

	/*
	 * skip to end of white lines after paragraph
	 */
	nếu (include || do_white)
	    trong_khi (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    nếu (!white_in_front && !linewhite(end_lnum) && include)
	trong_khi (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    nếu (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	nếu (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	nếu (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    không_thì
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    trả_về OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static số_nguyên
find_next_quote(
    char_u	*line,
    số_nguyên		col,
    số_nguyên		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    số_nguyên		c;

    cho (;;)
    {
	c = line[col];
	nếu (c == NUL)
	    trả_về -1;
	không_thì nếu (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    nếu (line[col] == NUL)
		trả_về -1;
	}
	không_thì nếu (c == quotechar)
	    break;
	nếu (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	không_thì
	    ++col;
    }
    trả_về col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static số_nguyên
find_prev_quote(
    char_u	*line,
    số_nguyên		col_start,
    số_nguyên		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    số_nguyên		n;

    trong_khi (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	nếu (escape != NULL)
	    trong_khi (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	nếu (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	không_thì nếu (line[col_start] == quotechar)
	    break;
    }
    trả_về col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    số_nguyên
current_quote(
    oparg_T	*oap,
    long	count,
    số_nguyên		include,	// TRUE == include quote char
    số_nguyên		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    số_nguyên		col_end;
    số_nguyên		col_start = curwin->w_cursor.col;
    số_nguyên		inclusive = FALSE;
    số_nguyên		vis_empty = TRUE;	// Visual selection <= 1 char
    số_nguyên		vis_bef_curs = FALSE;	// Visual starts before cursor
    số_nguyên		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    số_nguyên		inside_quotes = FALSE;	// Looks like "i'" done before
    số_nguyên		selected_quote = FALSE;	// Has quote inside selection
    số_nguyên		i;
    số_nguyên		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    nếu (VIsual_active)
    {
	// this only works within one line
	nếu (VIsual.lnum != curwin->w_cursor.lnum)
	    trả_về FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	nếu (*p_sel == 'e')
	{
	    nếu (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    không_thì nếu (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    nếu (!vis_bef_curs && !vis_empty)
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

    nếu (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	nếu (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	không_thì
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	trong_khi (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    nếu (line[i] == NUL)
		break;
	    nếu (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		break;
	    }
	}
    }

    nếu (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	nếu (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    nếu (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    nếu (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	không_thì
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    nếu (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    nếu (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    không_thì

    nếu (line[col_start] == quotechar || !vis_empty)
    {
	số_nguyên	first_col = col_start;

	nếu (!vis_empty)
	{
	    nếu (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    không_thì
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	cho (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    nếu (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    nếu (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    nếu (col_start <= first_col && first_col <= col_end)
		break;
	    col_start = col_end + 1;
	}
    }
    không_thì
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	nếu (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    nếu (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	nếu (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    nếu (include)
    {
	nếu (VIM_ISWHITE(line[col_end + 1]))
	    trong_khi (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	không_thì
	    trong_khi (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    nếu (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    nếu (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	nếu (vis_empty
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
    không_thì
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    nếu ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    nếu (VIsual_active)
    {
	nếu (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    nếu (*p_sel != 'e')
		dec_cursor();
	}
	không_thì
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    nếu (inside_quotes
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
	nếu (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    không_thì
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    trả_về OK;

abort_search:
    nếu (VIsual_active && *p_sel == 'e')
    {
	nếu (did_exclusive_adj)
	    inc_cursor();
	nếu (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    trả_về FALSE;
}
