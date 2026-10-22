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

static 정수 cls(void);
static 정수 skip_chars(정수, 정수);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    정수
findsent(정수 dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    정수		c;
    정수		(*func)(pos_T *);
    정수		startlnum;
    정수		noskip = FALSE;	    // do not skip blanks
    정수		cpo_J;
    정수		found_dot;

    pos = curwin->w_cursor;
    만약 (dir == FORWARD)
	func = incl;
    아니면
	func = decl;

    동안 (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	만약 (gchar_pos(&pos) == NUL)
	{
	    do
		만약 ((*func)(&pos) == -1)
		    break;
	    동안 (gchar_pos(&pos) == NUL);
	    만약 (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	아니면 만약 (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    만약 (pos.lnum == curbuf->b_ml.ml_line_count)
		반환 FAIL;
	    ++pos.lnum;
	    goto found;
	}
	아니면 만약 (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	동안 (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    만약 (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		break;

	    만약 (found_dot)
		break;
	    만약 (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    만약 (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		break;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	위해 (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    만약 (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		만약 (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		break;
	    }
	    만약 (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    만약 ((c = inc(&tpos)) == -1)
			break;
		동안 (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		만약 (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    만약 (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    break;
		}
	    }
	    만약 ((*func)(&pos) == -1)
	    {
		만약 (count)
		    반환 FAIL;
		noskip = TRUE;
		break;
	    }
	}
found:
	    // skip white space
	동안 (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    만약 (incl(&pos) == -1)
		break;

	만약 (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    만약 ((*func)(&pos) == -1)
	    {
		만약 (count)
		    반환 FAIL;
		break;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    반환 OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    정수
findpar(
    정수		*pincl,	    // Return: TRUE if last char is to be included
    정수		dir,
    long	count,
    정수		what,
    정수		both)
{
    linenr_T	curr;
    정수		did_skip;   // TRUE after separating lines have been skipped
    정수		first;	    // TRUE on first line
    정수		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    정수		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    동안 (count--)
    {
	did_skip = FALSE;
	위해 (first = TRUE; ; first = FALSE)
	{
	    만약 (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    만약 (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    만약 (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		break;

#ifdef FEAT_FOLDING
	    만약 (fold_skipped)
		curr -= dir;
#endif
	    만약 ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		만약 (count)
		    반환 FALSE;
		curr -= dir;
		break;
	    }
	}
    }
    setpcmark();
    만약 (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    만약 (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	만약 ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    아니면
	curwin->w_cursor.col = 0;
    반환 TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static 정수
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    위해 (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	만약 (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    break;
	++macro;
	만약 (macro[0] == NUL)
	    break;
    }
    반환 (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    정수
startPS(linenr_T lnum, 정수 para, 정수 both)
{
    char_u	*s;

    s = ml_get(lnum);
    만약 (*s == para || *s == '\f' || (both && *s == '}'))
	반환 TRUE;
    만약 (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	반환 TRUE;
    반환 FALSE;
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

static 정수	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static 정수
cls(void)
{
    정수	    c;

    c = gchar_cursor();
    만약 (c == ' ' || c == '\t' || c == NUL)
	반환 0;
    만약 (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	만약 (enc_dbcs == DBCS_KOR && cls_bigword)
	    반환 1;

	// process code leading/trailing bytes
	반환 dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    만약 (enc_utf8)
    {
	c = utf_class(c);
	만약 (c != 0 && cls_bigword)
	    반환 1;
	반환 c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    만약 (cls_bigword)
	반환 1;

    만약 (vim_iswordc(c))
	반환 2;
    반환 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    정수
fwd_word(
    long	count,
    정수		bigword,    // "W", "E" or "B"
    정수		eol)
{
    정수		sclass;	    // starting class
    정수		i;
    정수		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    동안 (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	만약 (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	만약 (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    반환 FAIL;
	만약 (i >= 1 && eol && count == 0)      // started at last char in line
	    반환 OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	만약 (sclass != 0)
	    동안 (cls() == sclass)
	    {
		i = inc_cursor();
		만약 (i == -1 || (i >= 1 && eol && count == 0))
		    반환 OK;
	    }

	/*
	 * go to next non-white
	 */
	동안 (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    만약 (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		break;

	    i = inc_cursor();
	    만약 (i == -1 || (i >= 1 && eol && count == 0))
		반환 OK;
	}
    }
    반환 OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    정수
bck_word(long count, 정수 bigword, 정수 stop)
{
    정수		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    동안 (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	만약 (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	만약 (dec_cursor() == -1)		// started at start of file
	    반환 FAIL;

	만약 (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    동안 (cls() == 0)
	    {
		만약 (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		만약 (dec_cursor() == -1) // hit start of file, stop here
		    반환 OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    만약 (skip_chars(cls(), BACKWARD))
		반환 OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    반환 OK;
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
    정수
end_word(
    long	count,
    정수		bigword,
    정수		stop,
    정수		empty)
{
    정수		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    만약 (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    동안 (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	만약 (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	만약 (inc_cursor() == -1)
	    반환 FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	만약 (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    만약 (skip_chars(sclass, FORWARD))
		반환 FAIL;
	}
	아니면 만약 (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    동안 (cls() == 0)
	    {
		만약 (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		만약 (inc_cursor() == -1)	    // hit end of file, stop here
		    반환 FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    만약 (skip_chars(cls(), FORWARD))
		반환 FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    반환 OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    정수
bckend_word(
    long	count,
    정수		bigword,    // TRUE for "B"
    정수		eol)	    // TRUE: stop at end of line.
{
    정수		sclass;	    // starting class
    정수		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    동안 (--count >= 0)
    {
	sclass = cls();
	만약 ((i = dec_cursor()) == -1)
	    반환 FAIL;
	만약 (eol && i == 1)
	    반환 OK;

	/*
	 * Move backward to before the start of this word.
	 */
	만약 (sclass != 0)
	{
	    동안 (cls() == sclass)
		만약 ((i = dec_cursor()) == -1 || (eol && i == 1))
		    반환 OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	동안 (cls() == 0)
	{
	    만약 (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		break;
	    만약 ((i = dec_cursor()) == -1 || (eol && i == 1))
		반환 OK;
	}
    }
    adjust_skipcol();
    반환 OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static 정수
skip_chars(정수 cclass, 정수 dir)
{
    동안 (cls() == cclass)
	만약 ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    반환 TRUE;
    반환 FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    정수		sclass;		    // starting class

    sclass = cls();
    위해 (;;)
    {
	만약 (curwin->w_cursor.col == 0)	    // stop at start of line
	    break;
	dec_cursor();
	만약 (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    break;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    정수	    c;

    동안 (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	만약 (!VIM_ISWHITE(c))
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
    정수	    at_start_sent)	// cursor is at start of sentence
{
    동안 (count--)
    {
	findsent(FORWARD, 1L);
	만약 (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	만약 (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    정수
current_word(
    oparg_T	*oap,
    long	count,
    정수		include,	// TRUE: include word and white space
    정수		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    정수		inclusive = TRUE;
    정수		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    만약 (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    만약 (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	만약 ((cls() == 0) == include)
	{
	    만약 (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		반환 FAIL;
	}
	아니면
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    만약 (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    아니면
		oneleft();

	    만약 (include)
		include_white = TRUE;
	}

	만약 (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	아니면
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    동안 (count > 0)
    {
	inclusive = TRUE;
	만약 (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    만약 (decl(&curwin->w_cursor) == -1)
		반환 FAIL;
	    만약 (include != (cls() != 0))
	    {
		만약 (bck_word(1L, bigword, TRUE) == FAIL)
		    반환 FAIL;
	    }
	    아니면
	    {
		만약 (bckend_word(1L, bigword, TRUE) == FAIL)
		    반환 FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	아니면
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    만약 (incl(&curwin->w_cursor) == -1)
		반환 FAIL;
	    만약 (include != (cls() == 0))
	    {
		만약 (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    반환 FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		만약 (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    아니면
	    {
		만약 (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    반환 FAIL;
	    }
	}
	--count;
    }

    만약 (include_white && (cls() != 0
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
	만약 (oneleft() == OK)
	{
	    back_in_line();
	    만약 (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		만약 (VIsual_active)
		    VIsual = curwin->w_cursor;
		아니면
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    만약 (VIsual_active)
    {
	만약 (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	만약 (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    아니면
	oap->inclusive = inclusive;

    반환 OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    정수
current_sent(oparg_T *oap, long count, 정수 include)
{
    pos_T	start_pos;
    pos_T	pos;
    정수		start_blank;
    정수		c;
    정수		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    만약 (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	만약 (LT_POS(start_pos, VIsual))
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
	    동안 (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		만약 (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    break;
		}
		incl(&pos);
	    }
	    만약 (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		만약 (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		아니면
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    만약 (include)	// "as" gets twice as much as "is"
		count *= 2;
	    동안 (count--)
	    {
		만약 (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		만약 (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	아니면
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
	    만약 (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		동안 (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    만약 (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			break;
		    }
		    incl(&pos);
		}
		만약 (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		아니면		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    만약 (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    만약 (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	반환 OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    동안 (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    만약 (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    아니면
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    만약 (include)
	ncount = count * 2;
    아니면
    {
	ncount = count;
	만약 (start_blank)
	    --ncount;
    }
    만약 (ncount > 0)
	findsent_forward(ncount, TRUE);
    아니면
	decl(&curwin->w_cursor);

    만약 (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	만약 (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    만약 (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	아니면 만약 (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    만약 (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	만약 (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	만약 (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    아니면
    {
	// include a newline after the sentence, if there is one
	만약 (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	아니면
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    반환 OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    정수
current_block(
    oparg_T	*oap,
    long	count,
    정수		include,	// TRUE == include white space
    정수		what,		// '(', '{', etc.
    정수		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    정수		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    만약 (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	만약 (what == '{')		// ignore indent
	    동안 (inindent(1))
		만약 (inc_cursor() != 0)
		    break;
	만약 (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    아니면 만약 (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    아니면
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    만약 ((pos = findmatch(NULL, what)) != NULL)
    {
	동안 (count-- > 0)
	{
	    만약 ((pos = findmatch(NULL, what)) == NULL)
		break;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    아니면
    {
	동안 (count-- > 0)
	{
	    만약 ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    만약 (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	반환 FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    동안 (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	동안 (inindent(1))
	{
	    sol = TRUE;
	    만약 (decl(&curwin->w_cursor) != 0)
		break;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	만약 (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    반환 FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	만약 (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    만약 ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		반환 FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    만약 ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		반환 FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	아니면
	    break;
    }

    만약 (VIsual_active)
    {
	만약 (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	만약 (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    아니면
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	만약 (sol)
	    incl(&curwin->w_cursor);
	아니면 만약 (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	아니면
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    반환 OK;
}

#만약 defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static 정수
in_html_tag(
    정수		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    정수		c;
    정수		lc = NUL;
    pos_T	pos;

    만약 (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	위해 (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    만약 (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	만약 (*p != '<')	    // check for '<' under cursor
	{
	    만약 (lc != '<')
		반환 FALSE;
	    p = lp;
	}
    }
    아니면
    {
	위해 (p = line + curwin->w_cursor.col; p > line; )
	{
	    만약 (*p == '<')	// find '<' under/before cursor
		break;
	    MB_PTR_BACK(line, p);
	    만약 (*p == '>')	// find '>' before cursor
		break;
	}
	만약 (*p != '<')
	    반환 FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    만약 (end_tag)
	// check that there is a '/' after the '<'
	반환 *p == '/';

    // check that there is no '/' after the '<'
    만약 (*p == '/')
	반환 FALSE;

    // check that the matching '>' is not preceded by '/'
    위해 (;;)
    {
	만약 (inc(&pos) < 0)
	    반환 FALSE;
	c = *ml_get_pos(&pos);
	만약 (c == '>')
	    break;
	lc = c;
    }
    반환 lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    정수
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    정수		include)	// TRUE == include white space
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
    정수		len;
    정수		r;
    정수		do_include = include;
    정수		save_p_ws = p_ws;
    정수		retval = FAIL;
    정수		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    만약 (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    만약 (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	동안 (inindent(1))
	    만약 (inc_cursor() != 0)
		break;

	만약 (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    동안 (*ml_get_cursor() != '>')
		만약 (inc_cursor() < 0)
		    break;
	}
	아니면 만약 (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    동안 (*ml_get_cursor() != '<')
		만약 (dec_cursor() < 0)
		    break;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    아니면 만약 (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    아니면
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    위해 (n = 0; n < count; ++n)
    {
	만약 (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    위해 (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (정수)(cp - p);
    만약 (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    만약 (spat == NULL || epat == NULL)
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

    만약 (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    만약 (do_include)
    {
	// Include up to the '>'.
	동안 (*ml_get_cursor() != '>')
	    만약 (inc_cursor() < 0)
		break;
    }
    아니면
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	만약 (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	아니면 만약 (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    만약 (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	정수 in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	동안 (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    만약 (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		break;
	    }
	    아니면 만약 (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	만약 (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    만약 (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	만약 (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	아니면 만약 (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    아니면
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	만약 (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	아니면
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    반환 retval;
}
#endif

    정수
current_par(
    oparg_T	*oap,
    long	count,
    정수		include,	// TRUE == include white space
    정수		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    정수		white_in_front;
    정수		dir;
    정수		start_is_white;
    정수		prev_start_is_white;
    정수		retval = OK;
    정수		do_white = FALSE;
    정수		t;
    정수		i;

    만약 (type == 'S')	    // not implemented yet
	반환 FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    만약 (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	만약 (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	아니면
	    dir = FORWARD;
	위해 (i = count; --i >= 0; )
	{
	    만약 (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		break;
	    }

	    prev_start_is_white = -1;
	    위해 (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		만약 (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    break;
		}
		위해 (;;)
		{
		    만약 (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			break;
		    만약 (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			break;
		    start_lnum += dir;
		}
		만약 (!include)
		    break;
		만약 (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    break;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	반환 retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    동안 (start_lnum > 1)
    {
	만약 (white_in_front)	    // stop at first white line
	{
	    만약 (!linewhite(start_lnum - 1))
		break;
	}
	아니면		// stop at first non-white line of start of paragraph
	{
	    만약 (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		break;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    동안 (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    만약 (!include && white_in_front)
	--i;
    동안 (i--)
    {
	만약 (end_lnum == curbuf->b_ml.ml_line_count)
	    반환 FAIL;

	만약 (!include)
	    do_white = linewhite(end_lnum + 1);

	만약 (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    동안 (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	만약 (i == 0 && white_in_front && include)
	    break;

	/*
	 * skip to end of white lines after paragraph
	 */
	만약 (include || do_white)
	    동안 (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    만약 (!white_in_front && !linewhite(end_lnum) && include)
	동안 (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    만약 (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	만약 (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	만약 (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    아니면
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    반환 OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static 정수
find_next_quote(
    char_u	*line,
    정수		col,
    정수		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    정수		c;

    위해 (;;)
    {
	c = line[col];
	만약 (c == NUL)
	    반환 -1;
	아니면 만약 (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    만약 (line[col] == NUL)
		반환 -1;
	}
	아니면 만약 (c == quotechar)
	    break;
	만약 (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	아니면
	    ++col;
    }
    반환 col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static 정수
find_prev_quote(
    char_u	*line,
    정수		col_start,
    정수		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    정수		n;

    동안 (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	만약 (escape != NULL)
	    동안 (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	만약 (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	아니면 만약 (line[col_start] == quotechar)
	    break;
    }
    반환 col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    정수
current_quote(
    oparg_T	*oap,
    long	count,
    정수		include,	// TRUE == include quote char
    정수		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    정수		col_end;
    정수		col_start = curwin->w_cursor.col;
    정수		inclusive = FALSE;
    정수		vis_empty = TRUE;	// Visual selection <= 1 char
    정수		vis_bef_curs = FALSE;	// Visual starts before cursor
    정수		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    정수		inside_quotes = FALSE;	// Looks like "i'" done before
    정수		selected_quote = FALSE;	// Has quote inside selection
    정수		i;
    정수		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    만약 (VIsual_active)
    {
	// this only works within one line
	만약 (VIsual.lnum != curwin->w_cursor.lnum)
	    반환 FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	만약 (*p_sel == 'e')
	{
	    만약 (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    아니면 만약 (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    만약 (!vis_bef_curs && !vis_empty)
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

    만약 (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	만약 (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	아니면
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	동안 (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    만약 (line[i] == NUL)
		break;
	    만약 (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		break;
	    }
	}
    }

    만약 (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	만약 (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    만약 (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    만약 (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	아니면
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    만약 (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    만약 (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    아니면

    만약 (line[col_start] == quotechar || !vis_empty)
    {
	정수	first_col = col_start;

	만약 (!vis_empty)
	{
	    만약 (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    아니면
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	위해 (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    만약 (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    만약 (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    만약 (col_start <= first_col && first_col <= col_end)
		break;
	    col_start = col_end + 1;
	}
    }
    아니면
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	만약 (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    만약 (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	만약 (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    만약 (include)
    {
	만약 (VIM_ISWHITE(line[col_end + 1]))
	    동안 (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	아니면
	    동안 (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    만약 (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    만약 (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	만약 (vis_empty
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
    아니면
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    만약 ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    만약 (VIsual_active)
    {
	만약 (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    만약 (*p_sel != 'e')
		dec_cursor();
	}
	아니면
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    만약 (inside_quotes
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
	만약 (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    아니면
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    반환 OK;

abort_search:
    만약 (VIsual_active && *p_sel == 'e')
    {
	만약 (did_exclusive_adj)
	    inc_cursor();
	만약 (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    반환 FALSE;
}
