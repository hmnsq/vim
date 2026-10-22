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

static 整数 cls(void);
static 整数 skip_chars(整数, 整数);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    整数
findsent(整数 dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    整数		c;
    整数		(*func)(pos_T *);
    整数		startlnum;
    整数		noskip = FALSE;	    // do not skip blanks
    整数		cpo_J;
    整数		found_dot;

    pos = curwin->w_cursor;
    もし (dir == FORWARD)
	func = incl;
    否则
	func = decl;

    ながら (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	もし (gchar_pos(&pos) == NUL)
	{
	    do
		もし ((*func)(&pos) == -1)
		    break;
	    ながら (gchar_pos(&pos) == NUL);
	    もし (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	否则 もし (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    もし (pos.lnum == curbuf->b_ml.ml_line_count)
		戻る FAIL;
	    ++pos.lnum;
	    goto found;
	}
	否则 もし (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ながら (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    もし (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    もし (found_dot)
		break;
	    もし (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    もし (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	ために (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    もし (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		もし (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    もし (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    もし ((c = inc(&tpos)) == -1)
			break;
		ながら (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		もし (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    もし (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    もし ((*func)(&pos) == -1)
	    {
		もし (count)
		    戻る FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	ながら (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    もし (incl(&pos) == -1)
		break;

	もし (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    もし ((*func)(&pos) == -1)
	    {
		もし (count)
		    戻る FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    戻る OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    整数
findpar(
    整数		*pincl,	    // Return: TRUE if last char is to be included
    整数		dir,
    long	count,
    整数		what,
    整数		both)
{
    linenr_T	curr;
    整数		did_skip;   // TRUE after separating lines have been skipped
    整数		first;	    // TRUE on first line
    整数		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    整数		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ながら (count--)
    {
	did_skip = FALSE;
	ために (first = TRUE; ; first = FALSE)
	{
	    もし (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    もし (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    もし (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    もし (fold_skipped)
		curr -= dir;
#endif
	    もし ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		もし (count)
		    戻る FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    もし (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    もし (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	もし ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    否则
	curwin->w_cursor.col = 0;
    戻る TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static 整数
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ために (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	もし (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    break;
	++macro;
	もし (macro[0] == NUL)
	    break;
    }
    戻る (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    整数
startPS(linenr_T lnum, 整数 para, 整数 both)
{
    char_u	*s;

    s = ml_get(lnum);
    もし (*s == para || *s == '\f' || (both && *s == '}'))
	戻る TRUE;
    もし (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	戻る TRUE;
    戻る FALSE;
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

static 整数	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static 整数
cls(void)
{
    整数	    c;

    c = gchar_cursor();
    もし (c == ' ' || c == '\t' || c == NUL)
	戻る 0;
    もし (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	もし (enc_dbcs == DBCS_KOR && cls_bigword)
	    戻る 1;

	// process code leading/trailing bytes
	戻る dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    もし (enc_utf8)
    {
	c = utf_class(c);
	もし (c != 0 && cls_bigword)
	    戻る 1;
	戻る c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    もし (cls_bigword)
	戻る 1;

    もし (vim_iswordc(c))
	戻る 2;
    戻る 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    整数
fwd_word(
    long	count,
    整数		bigword,    // "W", "E" or "B"
    整数		eol)
{
    整数		sclass;	    // starting class
    整数		i;
    整数		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ながら (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	もし (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	もし (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    戻る FAIL;
	もし (i >= 1 && eol && count == 0)      // started at last char in line
	    戻る OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	もし (sclass != 0)
	    ながら (cls() == sclass)
	    {
		i = inc_cursor();
		もし (i == -1 || (i >= 1 && eol && count == 0))
		    戻る OK;
	    }

	/*
	 * go to next non-white
	 */
	ながら (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    もし (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    もし (i == -1 || (i >= 1 && eol && count == 0))
		戻る OK;
	}
    }
    戻る OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    整数
bck_word(long count, 整数 bigword, 整数 stop)
{
    整数		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ながら (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	もし (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	もし (dec_cursor() == -1)		// started at start of file
	    戻る FAIL;

	もし (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ながら (cls() == 0)
	    {
		もし (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		もし (dec_cursor() == -1) // hit start of file, stop here
		    戻る OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    もし (skip_chars(cls(), BACKWARD))
		戻る OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    戻る OK;
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
    整数
end_word(
    long	count,
    整数		bigword,
    整数		stop,
    整数		empty)
{
    整数		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    もし (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ながら (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	もし (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	もし (inc_cursor() == -1)
	    戻る FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	もし (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    もし (skip_chars(sclass, FORWARD))
		戻る FAIL;
	}
	否则 もし (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ながら (cls() == 0)
	    {
		もし (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		もし (inc_cursor() == -1)	    // hit end of file, stop here
		    戻る FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    もし (skip_chars(cls(), FORWARD))
		戻る FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    戻る OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    整数
bckend_word(
    long	count,
    整数		bigword,    // TRUE for "B"
    整数		eol)	    // TRUE: stop at end of line.
{
    整数		sclass;	    // starting class
    整数		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ながら (--count >= 0)
    {
	sclass = cls();
	もし ((i = dec_cursor()) == -1)
	    戻る FAIL;
	もし (eol && i == 1)
	    戻る OK;

	/*
	 * Move backward to before the start of this word.
	 */
	もし (sclass != 0)
	{
	    ながら (cls() == sclass)
		もし ((i = dec_cursor()) == -1 || (eol && i == 1))
		    戻る OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ながら (cls() == 0)
	{
	    もし (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    もし ((i = dec_cursor()) == -1 || (eol && i == 1))
		戻る OK;
	}
    }
    adjust_skipcol();
    戻る OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static 整数
skip_chars(整数 cclass, 整数 dir)
{
    ながら (cls() == cclass)
	もし ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    戻る TRUE;
    戻る FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    整数		sclass;		    // starting class

    sclass = cls();
    ために (;;)
    {
	もし (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	もし (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    整数	    c;

    ながら (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	もし (!VIM_ISWHITE(c))
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
    整数	    at_start_sent)	// cursor is at start of sentence
{
    ながら (count--)
    {
	findsent(FORWARD, 1L);
	もし (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	もし (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    整数
current_word(
    oparg_T	*oap,
    long	count,
    整数		include,	// TRUE: include word and white space
    整数		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    整数		inclusive = TRUE;
    整数		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    もし (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    もし (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	もし ((cls() == 0) == include)
	{
	    もし (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		戻る FAIL;
	}
	否则
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    もし (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    否则
		oneleft();

	    もし (include)
		include_white = TRUE;
	}

	もし (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	否则
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ながら (count > 0)
    {
	inclusive = TRUE;
	もし (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    もし (decl(&curwin->w_cursor) == -1)
		戻る FAIL;
	    もし (include != (cls() != 0))
	    {
		もし (bck_word(1L, bigword, TRUE) == FAIL)
		    戻る FAIL;
	    }
	    否则
	    {
		もし (bckend_word(1L, bigword, TRUE) == FAIL)
		    戻る FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	否则
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    もし (incl(&curwin->w_cursor) == -1)
		戻る FAIL;
	    もし (include != (cls() == 0))
	    {
		もし (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    戻る FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		もし (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    否则
	    {
		もし (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    戻る FAIL;
	    }
	}
	--count;
    }

    もし (include_white && (cls() != 0
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
	もし (oneleft() == OK)
	{
	    back_in_line();
	    もし (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		もし (VIsual_active)
		    VIsual = curwin->w_cursor;
		否则
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    もし (VIsual_active)
    {
	もし (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	もし (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    否则
	oap->inclusive = inclusive;

    戻る OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    整数
current_sent(oparg_T *oap, long count, 整数 include)
{
    pos_T	start_pos;
    pos_T	pos;
    整数		start_blank;
    整数		c;
    整数		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    もし (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	もし (LT_POS(start_pos, VIsual))
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
	    ながら (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		もし (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    もし (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		もし (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		否则
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    もし (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ながら (count--)
	    {
		もし (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		もし (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	否则
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
	    もし (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ながら (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    もし (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		もし (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		否则		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    もし (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    もし (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	戻る OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ながら (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    もし (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    否则
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    もし (include)
	ncount = count * 2;
    否则
    {
	ncount = count;
	もし (start_blank)
	    --ncount;
    }
    もし (ncount > 0)
	findsent_forward(ncount, TRUE);
    否则
	decl(&curwin->w_cursor);

    もし (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	もし (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    もし (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	否则 もし (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    もし (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	もし (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	もし (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    否则
    {
	// include a newline after the sentence, if there is one
	もし (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	否则
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    戻る OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    整数
current_block(
    oparg_T	*oap,
    long	count,
    整数		include,	// TRUE == include white space
    整数		what,		// '(', '{', etc.
    整数		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    整数		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    もし (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	もし (what == '{')		// ignore indent
	    ながら (inindent(1))
		もし (inc_cursor() != 0)
		    break;
	もし (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    否则 もし (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    否则
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    もし ((pos = findmatch(NULL, what)) != NULL)
    {
	ながら (count-- > 0)
	{
	    もし ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    否则
    {
	ながら (count-- > 0)
	{
	    もし ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    もし (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	戻る FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ながら (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ながら (inindent(1))
	{
	    sol = TRUE;
	    もし (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	もし (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    戻る FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	もし (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    もし ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		戻る FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    もし ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		戻る FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	否则
	    break;
    }

    もし (VIsual_active)
    {
	もし (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	もし (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    否则
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	もし (sol)
	    incl(&curwin->w_cursor);
	否则 もし (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	否则
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    戻る OK;
}

#もし defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static 整数
in_html_tag(
    整数		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    整数		c;
    整数		lc = NUL;
    pos_T	pos;

    もし (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ために (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    もし (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	もし (*p != '<')	    // check for '<' under cursor
	{
	    もし (lc != '<')
		戻る FALSE;
	    p = lp;
	}
    }
    否则
    {
	ために (p = line + curwin->w_cursor.col; p > line; )
	{
	    もし (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    もし (*p == '>')	// find '>' before cursor
		break;
	}
	もし (*p != '<')
	    戻る FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    もし (end_tag)
	// check that there is a '/' after the '<'
	戻る *p == '/';

    // check that there is no '/' after the '<'
    もし (*p == '/')
	戻る FALSE;

    // check that the matching '>' is not preceded by '/'
    ために (;;)
    {
	もし (inc(&pos) < 0)
	    戻る FALSE;
	c = *ml_get_pos(&pos);
	もし (c == '>')
	    break;
	lc = c;
    }
    戻る lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    整数
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    整数		include)	// TRUE == include white space
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
    整数		len;
    整数		r;
    整数		do_include = include;
    整数		save_p_ws = p_ws;
    整数		retval = FAIL;
    整数		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    もし (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    もし (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ながら (inindent(1))
	    もし (inc_cursor() != 0)
		break;

	もし (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ながら (*ml_get_cursor() != '>')
		もし (inc_cursor() < 0)
		    break;
	}
	否则 もし (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ながら (*ml_get_cursor() != '<')
		もし (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    否则 もし (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    否则
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ために (n = 0; n < count; ++n)
    {
	もし (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    ために (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (整数)(cp - p);
    もし (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    もし (spat == NULL || epat == NULL)
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

    もし (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    もし (do_include)
    {
	// Include up to the '>'.
	ながら (*ml_get_cursor() != '>')
	    もし (inc_cursor() < 0)
		break;
    }
    否则
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	もし (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	否则 もし (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    もし (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	整数 in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ながら (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    もし (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	    否则 もし (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	もし (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    もし (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	もし (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	否则 もし (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    否则
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	もし (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	否则
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    戻る retval;
}
#endif

    整数
current_par(
    oparg_T	*oap,
    long	count,
    整数		include,	// TRUE == include white space
    整数		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    整数		white_in_front;
    整数		dir;
    整数		start_is_white;
    整数		prev_start_is_white;
    整数		retval = OK;
    整数		do_white = FALSE;
    整数		t;
    整数		i;

    もし (type == 'S')	    // not implemented yet
	戻る FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    もし (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	もし (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	否则
	    dir = FORWARD;
	ために (i = count; --i >= 0; )
	{
	    もし (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		break;
	    }

	    prev_start_is_white = -1;
	    ために (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		もし (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    break;
		}
		ために (;;)
		{
		    もし (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			break;
		    もし (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			break;
		    start_lnum += dir;
		}
		もし (!include)
		    break;
		もし (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    break;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	戻る retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ながら (start_lnum > 1)
    {
	もし (white_in_front)	    // stop at first white line
	{
	    もし (!linewhite(start_lnum - 1))
		break;
	}
	否则		// stop at first non-white line of start of paragraph
	{
	    もし (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		break;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ながら (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    もし (!include && white_in_front)
	--i;
    ながら (i--)
    {
	もし (end_lnum == curbuf->b_ml.ml_line_count)
	    戻る FAIL;

	もし (!include)
	    do_white = linewhite(end_lnum + 1);

	もし (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ながら (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	もし (i == 0 && white_in_front && include)
	    break;

	/*
	 * skip to end of white lines after paragraph
	 */
	もし (include || do_white)
	    ながら (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    もし (!white_in_front && !linewhite(end_lnum) && include)
	ながら (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    もし (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	もし (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	もし (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    否则
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    戻る OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static 整数
find_next_quote(
    char_u	*line,
    整数		col,
    整数		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    整数		c;

    ために (;;)
    {
	c = line[col];
	もし (c == NUL)
	    戻る -1;
	否则 もし (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    もし (line[col] == NUL)
		戻る -1;
	}
	否则 もし (c == quotechar)
	    break;
	もし (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	否则
	    ++col;
    }
    戻る col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static 整数
find_prev_quote(
    char_u	*line,
    整数		col_start,
    整数		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    整数		n;

    ながら (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	もし (escape != NULL)
	    ながら (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	もし (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	否则 もし (line[col_start] == quotechar)
	    break;
    }
    戻る col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    整数
current_quote(
    oparg_T	*oap,
    long	count,
    整数		include,	// TRUE == include quote char
    整数		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    整数		col_end;
    整数		col_start = curwin->w_cursor.col;
    整数		inclusive = FALSE;
    整数		vis_empty = TRUE;	// Visual selection <= 1 char
    整数		vis_bef_curs = FALSE;	// Visual starts before cursor
    整数		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    整数		inside_quotes = FALSE;	// Looks like "i'" done before
    整数		selected_quote = FALSE;	// Has quote inside selection
    整数		i;
    整数		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    もし (VIsual_active)
    {
	// this only works within one line
	もし (VIsual.lnum != curwin->w_cursor.lnum)
	    戻る FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	もし (*p_sel == 'e')
	{
	    もし (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    否则 もし (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    もし (!vis_bef_curs && !vis_empty)
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

    もし (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	もし (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	否则
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ながら (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    もし (line[i] == NUL)
		break;
	    もし (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		break;
	    }
	}
    }

    もし (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	もし (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    もし (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    もし (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	否则
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    もし (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    もし (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    否则

    もし (line[col_start] == quotechar || !vis_empty)
    {
	整数	first_col = col_start;

	もし (!vis_empty)
	{
	    もし (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    否则
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	ために (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    もし (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    もし (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    もし (col_start <= first_col && first_col <= col_end)
		break;
	    col_start = col_end + 1;
	}
    }
    否则
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	もし (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    もし (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	もし (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    もし (include)
    {
	もし (VIM_ISWHITE(line[col_end + 1]))
	    ながら (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	否则
	    ながら (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    もし (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    もし (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	もし (vis_empty
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
    否则
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    もし ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    もし (VIsual_active)
    {
	もし (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    もし (*p_sel != 'e')
		dec_cursor();
	}
	否则
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    もし (inside_quotes
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
	もし (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    否则
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    戻る OK;

abort_search:
    もし (VIsual_active && *p_sel == 'e')
    {
	もし (did_exclusive_adj)
	    inc_cursor();
	もし (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    戻る FALSE;
}
