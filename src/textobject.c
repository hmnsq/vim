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

static จำนวนเต็ม cls(void);
static จำนวนเต็ม skip_chars(จำนวนเต็ม, จำนวนเต็ม);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    จำนวนเต็ม
findsent(จำนวนเต็ม dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    จำนวนเต็ม		c;
    จำนวนเต็ม		(*func)(pos_T *);
    จำนวนเต็ม		startlnum;
    จำนวนเต็ม		noskip = FALSE;	    // do not skip blanks
    จำนวนเต็ม		cpo_J;
    จำนวนเต็ม		found_dot;

    pos = curwin->w_cursor;
    ถ้า (dir == FORWARD)
	func = incl;
    มิฉะนั้น
	func = decl;

    ขณะ (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ถ้า (gchar_pos(&pos) == NUL)
	{
	    do
		ถ้า ((*func)(&pos) == -1)
		    bha_nga;
	    ขณะ (gchar_pos(&pos) == NUL);
	    ถ้า (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	มิฉะนั้น ถ้า (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ถ้า (pos.lnum == curbuf->b_ml.ml_line_count)
		คืน FAIL;
	    ++pos.lnum;
	    goto found;
	}
	มิฉะนั้น ถ้า (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ขณะ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ถ้า (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    ถ้า (found_dot)
		bha_nga;
	    ถ้า (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ถ้า (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	สำหรับ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ถ้า (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ถ้า (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    ถ้า (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ถ้า ((c = inc(&tpos)) == -1)
			bha_nga;
		ขณะ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ถ้า (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ถ้า (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    ถ้า ((*func)(&pos) == -1)
	    {
		ถ้า (count)
		    คืน FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	ขณะ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ถ้า (incl(&pos) == -1)
		bha_nga;

	ถ้า (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ถ้า ((*func)(&pos) == -1)
	    {
		ถ้า (count)
		    คืน FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    คืน OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    จำนวนเต็ม
findpar(
    จำนวนเต็ม		*pincl,	    // Return: TRUE if last char is to be included
    จำนวนเต็ม		dir,
    long	count,
    จำนวนเต็ม		what,
    จำนวนเต็ม		both)
{
    linenr_T	curr;
    จำนวนเต็ม		did_skip;   // TRUE after separating lines have been skipped
    จำนวนเต็ม		first;	    // TRUE on first line
    จำนวนเต็ม		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    จำนวนเต็ม		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ขณะ (count--)
    {
	did_skip = FALSE;
	สำหรับ (first = TRUE; ; first = FALSE)
	{
	    ถ้า (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ถ้า (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ถ้า (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    ถ้า (fold_skipped)
		curr -= dir;
#endif
	    ถ้า ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ถ้า (count)
		    คืน FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    ถ้า (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ถ้า (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ถ้า ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    มิฉะนั้น
	curwin->w_cursor.col = 0;
    คืน TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static จำนวนเต็ม
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    สำหรับ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ถ้า (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	ถ้า (macro[0] == NUL)
	    bha_nga;
    }
    คืน (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    จำนวนเต็ม
startPS(linenr_T lnum, จำนวนเต็ม para, จำนวนเต็ม both)
{
    char_u	*s;

    s = ml_get(lnum);
    ถ้า (*s == para || *s == '\f' || (both && *s == '}'))
	คืน TRUE;
    ถ้า (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	คืน TRUE;
    คืน FALSE;
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

static จำนวนเต็ม	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static จำนวนเต็ม
cls(void)
{
    จำนวนเต็ม	    c;

    c = gchar_cursor();
    ถ้า (c == ' ' || c == '\t' || c == NUL)
	คืน 0;
    ถ้า (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ถ้า (enc_dbcs == DBCS_KOR && cls_bigword)
	    คืน 1;

	// process code leading/trailing bytes
	คืน dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ถ้า (enc_utf8)
    {
	c = utf_class(c);
	ถ้า (c != 0 && cls_bigword)
	    คืน 1;
	คืน c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ถ้า (cls_bigword)
	คืน 1;

    ถ้า (vim_iswordc(c))
	คืน 2;
    คืน 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    จำนวนเต็ม
fwd_word(
    long	count,
    จำนวนเต็ม		bigword,    // "W", "E" or "B"
    จำนวนเต็ม		eol)
{
    จำนวนเต็ม		sclass;	    // starting class
    จำนวนเต็ม		i;
    จำนวนเต็ม		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ขณะ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ถ้า (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ถ้า (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    คืน FAIL;
	ถ้า (i >= 1 && eol && count == 0)      // started at last char in line
	    คืน OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ถ้า (sclass != 0)
	    ขณะ (cls() == sclass)
	    {
		i = inc_cursor();
		ถ้า (i == -1 || (i >= 1 && eol && count == 0))
		    คืน OK;
	    }

	/*
	 * go to next non-white
	 */
	ขณะ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ถ้า (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    ถ้า (i == -1 || (i >= 1 && eol && count == 0))
		คืน OK;
	}
    }
    คืน OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    จำนวนเต็ม
bck_word(long count, จำนวนเต็ม bigword, จำนวนเต็ม stop)
{
    จำนวนเต็ม		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ขณะ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ถ้า (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ถ้า (dec_cursor() == -1)		// started at start of file
	    คืน FAIL;

	ถ้า (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ขณะ (cls() == 0)
	    {
		ถ้า (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ถ้า (dec_cursor() == -1) // hit start of file, stop here
		    คืน OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ถ้า (skip_chars(cls(), BACKWARD))
		คืน OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    คืน OK;
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
    จำนวนเต็ม
end_word(
    long	count,
    จำนวนเต็ม		bigword,
    จำนวนเต็ม		stop,
    จำนวนเต็ม		empty)
{
    จำนวนเต็ม		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ถ้า (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ขณะ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ถ้า (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ถ้า (inc_cursor() == -1)
	    คืน FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ถ้า (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ถ้า (skip_chars(sclass, FORWARD))
		คืน FAIL;
	}
	มิฉะนั้น ถ้า (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ขณะ (cls() == 0)
	    {
		ถ้า (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ถ้า (inc_cursor() == -1)	    // hit end of file, stop here
		    คืน FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ถ้า (skip_chars(cls(), FORWARD))
		คืน FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    คืน OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    จำนวนเต็ม
bckend_word(
    long	count,
    จำนวนเต็ม		bigword,    // TRUE for "B"
    จำนวนเต็ม		eol)	    // TRUE: stop at end of line.
{
    จำนวนเต็ม		sclass;	    // starting class
    จำนวนเต็ม		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ขณะ (--count >= 0)
    {
	sclass = cls();
	ถ้า ((i = dec_cursor()) == -1)
	    คืน FAIL;
	ถ้า (eol && i == 1)
	    คืน OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ถ้า (sclass != 0)
	{
	    ขณะ (cls() == sclass)
		ถ้า ((i = dec_cursor()) == -1 || (eol && i == 1))
		    คืน OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ขณะ (cls() == 0)
	{
	    ถ้า (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    ถ้า ((i = dec_cursor()) == -1 || (eol && i == 1))
		คืน OK;
	}
    }
    adjust_skipcol();
    คืน OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static จำนวนเต็ม
skip_chars(จำนวนเต็ม cclass, จำนวนเต็ม dir)
{
    ขณะ (cls() == cclass)
	ถ้า ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    คืน TRUE;
    คืน FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    จำนวนเต็ม		sclass;		    // starting class

    sclass = cls();
    สำหรับ (;;)
    {
	ถ้า (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	ถ้า (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    จำนวนเต็ม	    c;

    ขณะ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ถ้า (!VIM_ISWHITE(c))
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
    จำนวนเต็ม	    at_start_sent)	// cursor is at start of sentence
{
    ขณะ (count--)
    {
	findsent(FORWARD, 1L);
	ถ้า (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ถ้า (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    จำนวนเต็ม
current_word(
    oparg_T	*oap,
    long	count,
    จำนวนเต็ม		include,	// TRUE: include word and white space
    จำนวนเต็ม		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    จำนวนเต็ม		inclusive = TRUE;
    จำนวนเต็ม		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ถ้า (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ถ้า (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	ถ้า ((cls() == 0) == include)
	{
	    ถ้า (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		คืน FAIL;
	}
	มิฉะนั้น
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ถ้า (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    มิฉะนั้น
		oneleft();

	    ถ้า (include)
		include_white = TRUE;
	}

	ถ้า (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	มิฉะนั้น
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ขณะ (count > 0)
    {
	inclusive = TRUE;
	ถ้า (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ถ้า (decl(&curwin->w_cursor) == -1)
		คืน FAIL;
	    ถ้า (include != (cls() != 0))
	    {
		ถ้า (bck_word(1L, bigword, TRUE) == FAIL)
		    คืน FAIL;
	    }
	    มิฉะนั้น
	    {
		ถ้า (bckend_word(1L, bigword, TRUE) == FAIL)
		    คืน FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	มิฉะนั้น
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ถ้า (incl(&curwin->w_cursor) == -1)
		คืน FAIL;
	    ถ้า (include != (cls() == 0))
	    {
		ถ้า (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    คืน FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ถ้า (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    มิฉะนั้น
	    {
		ถ้า (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    คืน FAIL;
	    }
	}
	--count;
    }

    ถ้า (include_white && (cls() != 0
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
	ถ้า (oneleft() == OK)
	{
	    back_in_line();
	    ถ้า (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ถ้า (VIsual_active)
		    VIsual = curwin->w_cursor;
		มิฉะนั้น
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ถ้า (VIsual_active)
    {
	ถ้า (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ถ้า (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    มิฉะนั้น
	oap->inclusive = inclusive;

    คืน OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    จำนวนเต็ม
current_sent(oparg_T *oap, long count, จำนวนเต็ม include)
{
    pos_T	start_pos;
    pos_T	pos;
    จำนวนเต็ม		start_blank;
    จำนวนเต็ม		c;
    จำนวนเต็ม		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ถ้า (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ถ้า (LT_POS(start_pos, VIsual))
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
	    ขณะ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ถ้า (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    ถ้า (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ถ้า (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		มิฉะนั้น
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ถ้า (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ขณะ (count--)
	    {
		ถ้า (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ถ้า (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	มิฉะนั้น
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
	    ถ้า (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ขณะ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ถ้า (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		ถ้า (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		มิฉะนั้น		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ถ้า (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ถ้า (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	คืน OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ขณะ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ถ้า (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    มิฉะนั้น
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ถ้า (include)
	ncount = count * 2;
    มิฉะนั้น
    {
	ncount = count;
	ถ้า (start_blank)
	    --ncount;
    }
    ถ้า (ncount > 0)
	findsent_forward(ncount, TRUE);
    มิฉะนั้น
	decl(&curwin->w_cursor);

    ถ้า (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ถ้า (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ถ้า (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	มิฉะนั้น ถ้า (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ถ้า (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ถ้า (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ถ้า (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    มิฉะนั้น
    {
	// include a newline after the sentence, if there is one
	ถ้า (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	มิฉะนั้น
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    คืน OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    จำนวนเต็ม
current_block(
    oparg_T	*oap,
    long	count,
    จำนวนเต็ม		include,	// TRUE == include white space
    จำนวนเต็ม		what,		// '(', '{', etc.
    จำนวนเต็ม		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    จำนวนเต็ม		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ถ้า (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ถ้า (what == '{')		// ignore indent
	    ขณะ (inindent(1))
		ถ้า (inc_cursor() != 0)
		    bha_nga;
	ถ้า (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    มิฉะนั้น ถ้า (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    มิฉะนั้น
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ถ้า ((pos = findmatch(NULL, what)) != NULL)
    {
	ขณะ (count-- > 0)
	{
	    ถ้า ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    มิฉะนั้น
    {
	ขณะ (count-- > 0)
	{
	    ถ้า ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    ถ้า (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	คืน FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ขณะ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ขณะ (inindent(1))
	{
	    sol = TRUE;
	    ถ้า (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ถ้า (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    คืน FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ถ้า (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ถ้า ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		คืน FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ถ้า ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		คืน FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	มิฉะนั้น
	    bha_nga;
    }

    ถ้า (VIsual_active)
    {
	ถ้า (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ถ้า (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    มิฉะนั้น
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ถ้า (sol)
	    incl(&curwin->w_cursor);
	มิฉะนั้น ถ้า (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	มิฉะนั้น
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    คืน OK;
}

#ถ้า defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static จำนวนเต็ม
in_html_tag(
    จำนวนเต็ม		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    จำนวนเต็ม		c;
    จำนวนเต็ม		lc = NUL;
    pos_T	pos;

    ถ้า (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	สำหรับ (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ถ้า (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ถ้า (*p != '<')	    // check for '<' under cursor
	{
	    ถ้า (lc != '<')
		คืน FALSE;
	    p = lp;
	}
    }
    มิฉะนั้น
    {
	สำหรับ (p = line + curwin->w_cursor.col; p > line; )
	{
	    ถ้า (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    ถ้า (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	ถ้า (*p != '<')
	    คืน FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ถ้า (end_tag)
	// check that there is a '/' after the '<'
	คืน *p == '/';

    // check that there is no '/' after the '<'
    ถ้า (*p == '/')
	คืน FALSE;

    // check that the matching '>' is not preceded by '/'
    สำหรับ (;;)
    {
	ถ้า (inc(&pos) < 0)
	    คืน FALSE;
	c = *ml_get_pos(&pos);
	ถ้า (c == '>')
	    bha_nga;
	lc = c;
    }
    คืน lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    จำนวนเต็ม
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    จำนวนเต็ม		include)	// TRUE == include white space
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
    จำนวนเต็ม		len;
    จำนวนเต็ม		r;
    จำนวนเต็ม		do_include = include;
    จำนวนเต็ม		save_p_ws = p_ws;
    จำนวนเต็ม		retval = FAIL;
    จำนวนเต็ม		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ถ้า (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ถ้า (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ขณะ (inindent(1))
	    ถ้า (inc_cursor() != 0)
		bha_nga;

	ถ้า (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ขณะ (*ml_get_cursor() != '>')
		ถ้า (inc_cursor() < 0)
		    bha_nga;
	}
	มิฉะนั้น ถ้า (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ขณะ (*ml_get_cursor() != '<')
		ถ้า (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    มิฉะนั้น ถ้า (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    มิฉะนั้น
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    สำหรับ (n = 0; n < count; ++n)
    {
	ถ้า (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    สำหรับ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (จำนวนเต็ม)(cp - p);
    ถ้า (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ถ้า (spat == NULL || epat == NULL)
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

    ถ้า (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ถ้า (do_include)
    {
	// Include up to the '>'.
	ขณะ (*ml_get_cursor() != '>')
	    ถ้า (inc_cursor() < 0)
		bha_nga;
    }
    มิฉะนั้น
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ถ้า (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	มิฉะนั้น ถ้า (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ถ้า (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	จำนวนเต็ม in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ขณะ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ถ้า (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    มิฉะนั้น ถ้า (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ถ้า (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ถ้า (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ถ้า (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	มิฉะนั้น ถ้า (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    มิฉะนั้น
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ถ้า (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	มิฉะนั้น
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    คืน retval;
}
#endif

    จำนวนเต็ม
current_par(
    oparg_T	*oap,
    long	count,
    จำนวนเต็ม		include,	// TRUE == include white space
    จำนวนเต็ม		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    จำนวนเต็ม		white_in_front;
    จำนวนเต็ม		dir;
    จำนวนเต็ม		start_is_white;
    จำนวนเต็ม		prev_start_is_white;
    จำนวนเต็ม		retval = OK;
    จำนวนเต็ม		do_white = FALSE;
    จำนวนเต็ม		t;
    จำนวนเต็ม		i;

    ถ้า (type == 'S')	    // not implemented yet
	คืน FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ถ้า (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ถ้า (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	มิฉะนั้น
	    dir = FORWARD;
	สำหรับ (i = count; --i >= 0; )
	{
	    ถ้า (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    สำหรับ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ถ้า (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		สำหรับ (;;)
		{
		    ถ้า (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    ถ้า (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		ถ้า (!include)
		    bha_nga;
		ถ้า (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	คืน retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ขณะ (start_lnum > 1)
    {
	ถ้า (white_in_front)	    // stop at first white line
	{
	    ถ้า (!linewhite(start_lnum - 1))
		bha_nga;
	}
	มิฉะนั้น		// stop at first non-white line of start of paragraph
	{
	    ถ้า (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ขณะ (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ถ้า (!include && white_in_front)
	--i;
    ขณะ (i--)
    {
	ถ้า (end_lnum == curbuf->b_ml.ml_line_count)
	    คืน FAIL;

	ถ้า (!include)
	    do_white = linewhite(end_lnum + 1);

	ถ้า (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ขณะ (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ถ้า (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	ถ้า (include || do_white)
	    ขณะ (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ถ้า (!white_in_front && !linewhite(end_lnum) && include)
	ขณะ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ถ้า (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ถ้า (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ถ้า (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    มิฉะนั้น
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    คืน OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static จำนวนเต็ม
find_next_quote(
    char_u	*line,
    จำนวนเต็ม		col,
    จำนวนเต็ม		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    จำนวนเต็ม		c;

    สำหรับ (;;)
    {
	c = line[col];
	ถ้า (c == NUL)
	    คืน -1;
	มิฉะนั้น ถ้า (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ถ้า (line[col] == NUL)
		คืน -1;
	}
	มิฉะนั้น ถ้า (c == quotechar)
	    bha_nga;
	ถ้า (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	มิฉะนั้น
	    ++col;
    }
    คืน col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static จำนวนเต็ม
find_prev_quote(
    char_u	*line,
    จำนวนเต็ม		col_start,
    จำนวนเต็ม		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    จำนวนเต็ม		n;

    ขณะ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ถ้า (escape != NULL)
	    ขณะ (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ถ้า (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	มิฉะนั้น ถ้า (line[col_start] == quotechar)
	    bha_nga;
    }
    คืน col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    จำนวนเต็ม
current_quote(
    oparg_T	*oap,
    long	count,
    จำนวนเต็ม		include,	// TRUE == include quote char
    จำนวนเต็ม		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    จำนวนเต็ม		col_end;
    จำนวนเต็ม		col_start = curwin->w_cursor.col;
    จำนวนเต็ม		inclusive = FALSE;
    จำนวนเต็ม		vis_empty = TRUE;	// Visual selection <= 1 char
    จำนวนเต็ม		vis_bef_curs = FALSE;	// Visual starts before cursor
    จำนวนเต็ม		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    จำนวนเต็ม		inside_quotes = FALSE;	// Looks like "i'" done before
    จำนวนเต็ม		selected_quote = FALSE;	// Has quote inside selection
    จำนวนเต็ม		i;
    จำนวนเต็ม		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ถ้า (VIsual_active)
    {
	// this only works within one line
	ถ้า (VIsual.lnum != curwin->w_cursor.lnum)
	    คืน FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ถ้า (*p_sel == 'e')
	{
	    ถ้า (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    มิฉะนั้น ถ้า (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ถ้า (!vis_bef_curs && !vis_empty)
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

    ถ้า (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ถ้า (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	มิฉะนั้น
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ขณะ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ถ้า (line[i] == NUL)
		bha_nga;
	    ถ้า (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    ถ้า (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ถ้า (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ถ้า (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ถ้า (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	มิฉะนั้น
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ถ้า (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ถ้า (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    มิฉะนั้น

    ถ้า (line[col_start] == quotechar || !vis_empty)
    {
	จำนวนเต็ม	first_col = col_start;

	ถ้า (!vis_empty)
	{
	    ถ้า (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    มิฉะนั้น
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	สำหรับ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ถ้า (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ถ้า (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ถ้า (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    มิฉะนั้น
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ถ้า (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ถ้า (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ถ้า (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ถ้า (include)
    {
	ถ้า (VIM_ISWHITE(line[col_end + 1]))
	    ขณะ (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	มิฉะนั้น
	    ขณะ (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ถ้า (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ถ้า (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ถ้า (vis_empty
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
    มิฉะนั้น
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ถ้า ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ถ้า (VIsual_active)
    {
	ถ้า (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ถ้า (*p_sel != 'e')
		dec_cursor();
	}
	มิฉะนั้น
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ถ้า (inside_quotes
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
	ถ้า (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    มิฉะนั้น
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    คืน OK;

abort_search:
    ถ้า (VIsual_active && *p_sel == 'e')
    {
	ถ้า (did_exclusive_adj)
	    inc_cursor();
	ถ้า (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    คืน FALSE;
}
