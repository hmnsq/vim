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

static පූර්ණ_සංඛ්‍යා cls(void);
static පූර්ණ_සංඛ්‍යා skip_chars(පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    පූර්ණ_සංඛ්‍යා
findsent(පූර්ණ_සංඛ්‍යා dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		(*func)(pos_T *);
    පූර්ණ_සංඛ්‍යා		startlnum;
    පූර්ණ_සංඛ්‍යා		noskip = FALSE;	    // do not skip blanks
    පූර්ණ_සංඛ්‍යා		cpo_J;
    පූර්ණ_සංඛ්‍යා		found_dot;

    pos = curwin->w_cursor;
    නම් (dir == FORWARD)
	func = incl;
    නැතහොත්
	func = decl;

    අතරතුර (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	නම් (gchar_pos(&pos) == NUL)
	{
	    do
		නම් ((*func)(&pos) == -1)
		    bha_nga;
	    අතරතුර (gchar_pos(&pos) == NUL);
	    නම් (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	නැතහොත් නම් (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    නම් (pos.lnum == curbuf->b_ml.ml_line_count)
		ආපසු FAIL;
	    ++pos.lnum;
	    goto found;
	}
	නැතහොත් නම් (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	අතරතුර (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    නම් (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    නම් (found_dot)
		bha_nga;
	    නම් (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    නම් (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	සඳහා (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    නම් (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		නම් (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    නම් (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    නම් ((c = inc(&tpos)) == -1)
			bha_nga;
		අතරතුර (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		නම් (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    නම් (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    නම් ((*func)(&pos) == -1)
	    {
		නම් (count)
		    ආපසු FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	අතරතුර (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    නම් (incl(&pos) == -1)
		bha_nga;

	නම් (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    නම් ((*func)(&pos) == -1)
	    {
		නම් (count)
		    ආපසු FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ආපසු OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    පූර්ණ_සංඛ්‍යා
findpar(
    පූර්ණ_සංඛ්‍යා		*pincl,	    // Return: TRUE if last char is to be included
    පූර්ණ_සංඛ්‍යා		dir,
    long	count,
    පූර්ණ_සංඛ්‍යා		what,
    පූර්ණ_සංඛ්‍යා		both)
{
    linenr_T	curr;
    පූර්ණ_සංඛ්‍යා		did_skip;   // TRUE after separating lines have been skipped
    පූර්ණ_සංඛ්‍යා		first;	    // TRUE on first line
    පූර්ණ_සංඛ්‍යා		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    පූර්ණ_සංඛ්‍යා		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    අතරතුර (count--)
    {
	did_skip = FALSE;
	සඳහා (first = TRUE; ; first = FALSE)
	{
	    නම් (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    නම් (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    නම් (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    නම් (fold_skipped)
		curr -= dir;
#endif
	    නම් ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		නම් (count)
		    ආපසු FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    නම් (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    නම් (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	නම් ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    නැතහොත්
	curwin->w_cursor.col = 0;
    ආපසු TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static පූර්ණ_සංඛ්‍යා
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    සඳහා (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	නම් (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	නම් (macro[0] == NUL)
	    bha_nga;
    }
    ආපසු (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    පූර්ණ_සංඛ්‍යා
startPS(linenr_T lnum, පූර්ණ_සංඛ්‍යා para, පූර්ණ_සංඛ්‍යා both)
{
    char_u	*s;

    s = ml_get(lnum);
    නම් (*s == para || *s == '\f' || (both && *s == '}'))
	ආපසු TRUE;
    නම් (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ආපසු TRUE;
    ආපසු FALSE;
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

static පූර්ණ_සංඛ්‍යා	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static පූර්ණ_සංඛ්‍යා
cls(void)
{
    පූර්ණ_සංඛ්‍යා	    c;

    c = gchar_cursor();
    නම් (c == ' ' || c == '\t' || c == NUL)
	ආපසු 0;
    නම් (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	නම් (enc_dbcs == DBCS_KOR && cls_bigword)
	    ආපසු 1;

	// process code leading/trailing bytes
	ආපසු dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    නම් (enc_utf8)
    {
	c = utf_class(c);
	නම් (c != 0 && cls_bigword)
	    ආපසු 1;
	ආපසු c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    නම් (cls_bigword)
	ආපසු 1;

    නම් (vim_iswordc(c))
	ආපසු 2;
    ආපසු 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    පූර්ණ_සංඛ්‍යා
fwd_word(
    long	count,
    පූර්ණ_සංඛ්‍යා		bigword,    // "W", "E" or "B"
    පූර්ණ_සංඛ්‍යා		eol)
{
    පූර්ණ_සංඛ්‍යා		sclass;	    // starting class
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    අතරතුර (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	නම් (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	නම් (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ආපසු FAIL;
	නම් (i >= 1 && eol && count == 0)      // started at last char in line
	    ආපසු OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	නම් (sclass != 0)
	    අතරතුර (cls() == sclass)
	    {
		i = inc_cursor();
		නම් (i == -1 || (i >= 1 && eol && count == 0))
		    ආපසු OK;
	    }

	/*
	 * go to next non-white
	 */
	අතරතුර (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    නම් (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    නම් (i == -1 || (i >= 1 && eol && count == 0))
		ආපසු OK;
	}
    }
    ආපසු OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    පූර්ණ_සංඛ්‍යා
bck_word(long count, පූර්ණ_සංඛ්‍යා bigword, පූර්ණ_සංඛ්‍යා stop)
{
    පූර්ණ_සංඛ්‍යා		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    අතරතුර (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	නම් (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	නම් (dec_cursor() == -1)		// started at start of file
	    ආපසු FAIL;

	නම් (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    අතරතුර (cls() == 0)
	    {
		නම් (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		නම් (dec_cursor() == -1) // hit start of file, stop here
		    ආපසු OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    නම් (skip_chars(cls(), BACKWARD))
		ආපසු OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ආපසු OK;
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
    පූර්ණ_සංඛ්‍යා
end_word(
    long	count,
    පූර්ණ_සංඛ්‍යා		bigword,
    පූර්ණ_සංඛ්‍යා		stop,
    පූර්ණ_සංඛ්‍යා		empty)
{
    පූර්ණ_සංඛ්‍යා		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    නම් (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    අතරතුර (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	නම් (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	නම් (inc_cursor() == -1)
	    ආපසු FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	නම් (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    නම් (skip_chars(sclass, FORWARD))
		ආපසු FAIL;
	}
	නැතහොත් නම් (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    අතරතුර (cls() == 0)
	    {
		නම් (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		නම් (inc_cursor() == -1)	    // hit end of file, stop here
		    ආපසු FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    නම් (skip_chars(cls(), FORWARD))
		ආපසු FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ආපසු OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    පූර්ණ_සංඛ්‍යා
bckend_word(
    long	count,
    පූර්ණ_සංඛ්‍යා		bigword,    // TRUE for "B"
    පූර්ණ_සංඛ්‍යා		eol)	    // TRUE: stop at end of line.
{
    පූර්ණ_සංඛ්‍යා		sclass;	    // starting class
    පූර්ණ_සංඛ්‍යා		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    අතරතුර (--count >= 0)
    {
	sclass = cls();
	නම් ((i = dec_cursor()) == -1)
	    ආපසු FAIL;
	නම් (eol && i == 1)
	    ආපසු OK;

	/*
	 * Move backward to before the start of this word.
	 */
	නම් (sclass != 0)
	{
	    අතරතුර (cls() == sclass)
		නම් ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ආපසු OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	අතරතුර (cls() == 0)
	{
	    නම් (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    නම් ((i = dec_cursor()) == -1 || (eol && i == 1))
		ආපසු OK;
	}
    }
    adjust_skipcol();
    ආපසු OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static පූර්ණ_සංඛ්‍යා
skip_chars(පූර්ණ_සංඛ්‍යා cclass, පූර්ණ_සංඛ්‍යා dir)
{
    අතරතුර (cls() == cclass)
	නම් ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ආපසු TRUE;
    ආපසු FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    පූර්ණ_සංඛ්‍යා		sclass;		    // starting class

    sclass = cls();
    සඳහා (;;)
    {
	නම් (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	නම් (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    පූර්ණ_සංඛ්‍යා	    c;

    අතරතුර (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	නම් (!VIM_ISWHITE(c))
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
    පූර්ණ_සංඛ්‍යා	    at_start_sent)	// cursor is at start of sentence
{
    අතරතුර (count--)
    {
	findsent(FORWARD, 1L);
	නම් (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	නම් (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    පූර්ණ_සංඛ්‍යා
current_word(
    oparg_T	*oap,
    long	count,
    පූර්ණ_සංඛ්‍යා		include,	// TRUE: include word and white space
    පූර්ණ_සංඛ්‍යා		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    පූර්ණ_සංඛ්‍යා		inclusive = TRUE;
    පූර්ණ_සංඛ්‍යා		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    නම් (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    නම් (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	නම් ((cls() == 0) == include)
	{
	    නම් (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ආපසු FAIL;
	}
	නැතහොත්
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    නම් (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    නැතහොත්
		oneleft();

	    නම් (include)
		include_white = TRUE;
	}

	නම් (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	නැතහොත්
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    අතරතුර (count > 0)
    {
	inclusive = TRUE;
	නම් (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    නම් (decl(&curwin->w_cursor) == -1)
		ආපසු FAIL;
	    නම් (include != (cls() != 0))
	    {
		නම් (bck_word(1L, bigword, TRUE) == FAIL)
		    ආපසු FAIL;
	    }
	    නැතහොත්
	    {
		නම් (bckend_word(1L, bigword, TRUE) == FAIL)
		    ආපසු FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	නැතහොත්
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    නම් (incl(&curwin->w_cursor) == -1)
		ආපසු FAIL;
	    නම් (include != (cls() == 0))
	    {
		නම් (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ආපසු FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		නම් (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    නැතහොත්
	    {
		නම් (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ආපසු FAIL;
	    }
	}
	--count;
    }

    නම් (include_white && (cls() != 0
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
	නම් (oneleft() == OK)
	{
	    back_in_line();
	    නම් (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		නම් (VIsual_active)
		    VIsual = curwin->w_cursor;
		නැතහොත්
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    නම් (VIsual_active)
    {
	නම් (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	නම් (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    නැතහොත්
	oap->inclusive = inclusive;

    ආපසු OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    පූර්ණ_සංඛ්‍යා
current_sent(oparg_T *oap, long count, පූර්ණ_සංඛ්‍යා include)
{
    pos_T	start_pos;
    pos_T	pos;
    පූර්ණ_සංඛ්‍යා		start_blank;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    නම් (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	නම් (LT_POS(start_pos, VIsual))
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
	    අතරතුර (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		නම් (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    නම් (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		නම් (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		නැතහොත්
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    නම් (include)	// "as" gets twice as much as "is"
		count *= 2;
	    අතරතුර (count--)
	    {
		නම් (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		නම් (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	නැතහොත්
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
	    නම් (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		අතරතුර (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    නම් (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		නම් (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		නැතහොත්		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    නම් (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    නම් (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ආපසු OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    අතරතුර (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    නම් (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    නැතහොත්
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    නම් (include)
	ncount = count * 2;
    නැතහොත්
    {
	ncount = count;
	නම් (start_blank)
	    --ncount;
    }
    නම් (ncount > 0)
	findsent_forward(ncount, TRUE);
    නැතහොත්
	decl(&curwin->w_cursor);

    නම් (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	නම් (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    නම් (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	නැතහොත් නම් (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    නම් (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	නම් (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	නම් (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    නැතහොත්
    {
	// include a newline after the sentence, if there is one
	නම් (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	නැතහොත්
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ආපසු OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    පූර්ණ_සංඛ්‍යා
current_block(
    oparg_T	*oap,
    long	count,
    පූර්ණ_සංඛ්‍යා		include,	// TRUE == include white space
    පූර්ණ_සංඛ්‍යා		what,		// '(', '{', etc.
    පූර්ණ_සංඛ්‍යා		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    පූර්ණ_සංඛ්‍යා		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    නම් (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	නම් (what == '{')		// ignore indent
	    අතරතුර (inindent(1))
		නම් (inc_cursor() != 0)
		    bha_nga;
	නම් (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    නැතහොත් නම් (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    නැතහොත්
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    නම් ((pos = findmatch(NULL, what)) != NULL)
    {
	අතරතුර (count-- > 0)
	{
	    නම් ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    නැතහොත්
    {
	අතරතුර (count-- > 0)
	{
	    නම් ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    නම් (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ආපසු FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    අතරතුර (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	අතරතුර (inindent(1))
	{
	    sol = TRUE;
	    නම් (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	නම් (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ආපසු FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	නම් (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    නම් ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ආපසු FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    නම් ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ආපසු FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	නැතහොත්
	    bha_nga;
    }

    නම් (VIsual_active)
    {
	නම් (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	නම් (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    නැතහොත්
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	නම් (sol)
	    incl(&curwin->w_cursor);
	නැතහොත් නම් (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	නැතහොත්
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ආපසු OK;
}

#නම් defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static පූර්ණ_සංඛ්‍යා
in_html_tag(
    පූර්ණ_සංඛ්‍යා		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		lc = NUL;
    pos_T	pos;

    නම් (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	සඳහා (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    නම් (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	නම් (*p != '<')	    // check for '<' under cursor
	{
	    නම් (lc != '<')
		ආපසු FALSE;
	    p = lp;
	}
    }
    නැතහොත්
    {
	සඳහා (p = line + curwin->w_cursor.col; p > line; )
	{
	    නම් (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    නම් (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	නම් (*p != '<')
	    ආපසු FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    නම් (end_tag)
	// check that there is a '/' after the '<'
	ආපසු *p == '/';

    // check that there is no '/' after the '<'
    නම් (*p == '/')
	ආපසු FALSE;

    // check that the matching '>' is not preceded by '/'
    සඳහා (;;)
    {
	නම් (inc(&pos) < 0)
	    ආපසු FALSE;
	c = *ml_get_pos(&pos);
	නම් (c == '>')
	    bha_nga;
	lc = c;
    }
    ආපසු lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    පූර්ණ_සංඛ්‍යා
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    පූර්ණ_සංඛ්‍යා		include)	// TRUE == include white space
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
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		r;
    පූර්ණ_සංඛ්‍යා		do_include = include;
    පූර්ණ_සංඛ්‍යා		save_p_ws = p_ws;
    පූර්ණ_සංඛ්‍යා		retval = FAIL;
    පූර්ණ_සංඛ්‍යා		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    නම් (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    නම් (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	අතරතුර (inindent(1))
	    නම් (inc_cursor() != 0)
		bha_nga;

	නම් (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    අතරතුර (*ml_get_cursor() != '>')
		නම් (inc_cursor() < 0)
		    bha_nga;
	}
	නැතහොත් නම් (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    අතරතුර (*ml_get_cursor() != '<')
		නම් (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    නැතහොත් නම් (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    නැතහොත්
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    සඳහා (n = 0; n < count; ++n)
    {
	නම් (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    සඳහා (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (පූර්ණ_සංඛ්‍යා)(cp - p);
    නම් (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    නම් (spat == NULL || epat == NULL)
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

    නම් (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    නම් (do_include)
    {
	// Include up to the '>'.
	අතරතුර (*ml_get_cursor() != '>')
	    නම් (inc_cursor() < 0)
		bha_nga;
    }
    නැතහොත්
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	නම් (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	නැතහොත් නම් (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    නම් (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	පූර්ණ_සංඛ්‍යා in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	අතරතුර (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    නම් (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    නැතහොත් නම් (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	නම් (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    නම් (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	නම් (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	නැතහොත් නම් (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    නැතහොත්
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	නම් (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	නැතහොත්
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ආපසු retval;
}
#endif

    පූර්ණ_සංඛ්‍යා
current_par(
    oparg_T	*oap,
    long	count,
    පූර්ණ_සංඛ්‍යා		include,	// TRUE == include white space
    පූර්ණ_සංඛ්‍යා		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    පූර්ණ_සංඛ්‍යා		white_in_front;
    පූර්ණ_සංඛ්‍යා		dir;
    පූර්ණ_සංඛ්‍යා		start_is_white;
    පූර්ණ_සංඛ්‍යා		prev_start_is_white;
    පූර්ණ_සංඛ්‍යා		retval = OK;
    පූර්ණ_සංඛ්‍යා		do_white = FALSE;
    පූර්ණ_සංඛ්‍යා		t;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (type == 'S')	    // not implemented yet
	ආපසු FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    නම් (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	නම් (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	නැතහොත්
	    dir = FORWARD;
	සඳහා (i = count; --i >= 0; )
	{
	    නම් (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    සඳහා (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		නම් (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		සඳහා (;;)
		{
		    නම් (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    නම් (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		නම් (!include)
		    bha_nga;
		නම් (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ආපසු retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    අතරතුර (start_lnum > 1)
    {
	නම් (white_in_front)	    // stop at first white line
	{
	    නම් (!linewhite(start_lnum - 1))
		bha_nga;
	}
	නැතහොත්		// stop at first non-white line of start of paragraph
	{
	    නම් (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    අතරතුර (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    නම් (!include && white_in_front)
	--i;
    අතරතුර (i--)
    {
	නම් (end_lnum == curbuf->b_ml.ml_line_count)
	    ආපසු FAIL;

	නම් (!include)
	    do_white = linewhite(end_lnum + 1);

	නම් (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    අතරතුර (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	නම් (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	නම් (include || do_white)
	    අතරතුර (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    නම් (!white_in_front && !linewhite(end_lnum) && include)
	අතරතුර (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    නම් (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	නම් (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	නම් (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    නැතහොත්
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ආපසු OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static පූර්ණ_සංඛ්‍යා
find_next_quote(
    char_u	*line,
    පූර්ණ_සංඛ්‍යා		col,
    පූර්ණ_සංඛ්‍යා		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    පූර්ණ_සංඛ්‍යා		c;

    සඳහා (;;)
    {
	c = line[col];
	නම් (c == NUL)
	    ආපසු -1;
	නැතහොත් නම් (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    නම් (line[col] == NUL)
		ආපසු -1;
	}
	නැතහොත් නම් (c == quotechar)
	    bha_nga;
	නම් (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	නැතහොත්
	    ++col;
    }
    ආපසු col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static පූර්ණ_සංඛ්‍යා
find_prev_quote(
    char_u	*line,
    පූර්ණ_සංඛ්‍යා		col_start,
    පූර්ණ_සංඛ්‍යා		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    පූර්ණ_සංඛ්‍යා		n;

    අතරතුර (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	නම් (escape != NULL)
	    අතරතුර (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	නම් (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	නැතහොත් නම් (line[col_start] == quotechar)
	    bha_nga;
    }
    ආපසු col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    පූර්ණ_සංඛ්‍යා
current_quote(
    oparg_T	*oap,
    long	count,
    පූර්ණ_සංඛ්‍යා		include,	// TRUE == include quote char
    පූර්ණ_සංඛ්‍යා		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    පූර්ණ_සංඛ්‍යා		col_end;
    පූර්ණ_සංඛ්‍යා		col_start = curwin->w_cursor.col;
    පූර්ණ_සංඛ්‍යා		inclusive = FALSE;
    පූර්ණ_සංඛ්‍යා		vis_empty = TRUE;	// Visual selection <= 1 char
    පූර්ණ_සංඛ්‍යා		vis_bef_curs = FALSE;	// Visual starts before cursor
    පූර්ණ_සංඛ්‍යා		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    පූර්ණ_සංඛ්‍යා		inside_quotes = FALSE;	// Looks like "i'" done before
    පූර්ණ_සංඛ්‍යා		selected_quote = FALSE;	// Has quote inside selection
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    නම් (VIsual_active)
    {
	// this only works within one line
	නම් (VIsual.lnum != curwin->w_cursor.lnum)
	    ආපසු FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	නම් (*p_sel == 'e')
	{
	    නම් (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    නැතහොත් නම් (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    නම් (!vis_bef_curs && !vis_empty)
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

    නම් (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	නම් (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	නැතහොත්
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	අතරතුර (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    නම් (line[i] == NUL)
		bha_nga;
	    නම් (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    නම් (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	නම් (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    නම් (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    නම් (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	නැතහොත්
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    නම් (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    නම් (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    නැතහොත්

    නම් (line[col_start] == quotechar || !vis_empty)
    {
	පූර්ණ_සංඛ්‍යා	first_col = col_start;

	නම් (!vis_empty)
	{
	    නම් (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    නැතහොත්
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	සඳහා (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    නම් (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    නම් (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    නම් (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    නැතහොත්
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	නම් (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    නම් (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	නම් (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    නම් (include)
    {
	නම් (VIM_ISWHITE(line[col_end + 1]))
	    අතරතුර (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	නැතහොත්
	    අතරතුර (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    නම් (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    නම් (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	නම් (vis_empty
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
    නැතහොත්
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    නම් ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    නම් (VIsual_active)
    {
	නම් (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    නම් (*p_sel != 'e')
		dec_cursor();
	}
	නැතහොත්
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    නම් (inside_quotes
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
	නම් (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    නැතහොත්
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ආපසු OK;

abort_search:
    නම් (VIsual_active && *p_sel == 'e')
    {
	නම් (did_exclusive_adj)
	    inc_cursor();
	නම් (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ආපසු FALSE;
}
