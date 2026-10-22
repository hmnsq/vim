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

static ကိန်းပြည့် cls(void);
static ကိန်းပြည့် skip_chars(ကိန်းပြည့်, ကိန်းပြည့်);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ကိန်းပြည့်
findsent(ကိန်းပြည့် dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		(*func)(pos_T *);
    ကိန်းပြည့်		startlnum;
    ကိန်းပြည့်		noskip = FALSE;	    // do not skip blanks
    ကိန်းပြည့်		cpo_J;
    ကိန်းပြည့်		found_dot;

    pos = curwin->w_cursor;
    အကယ်၍ (dir == FORWARD)
	func = incl;
    မဟုတ်ပါက
	func = decl;

    နေစဉ် (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	အကယ်၍ (gchar_pos(&pos) == NUL)
	{
	    do
		အကယ်၍ ((*func)(&pos) == -1)
		    bha_nga;
	    နေစဉ် (gchar_pos(&pos) == NUL);
	    အကယ်၍ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	မဟုတ်ပါက အကယ်၍ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    အကယ်၍ (pos.lnum == curbuf->b_ml.ml_line_count)
		ပြန်ပေး FAIL;
	    ++pos.lnum;
	    goto found;
	}
	မဟုတ်ပါက အကယ်၍ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	နေစဉ် (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    အကယ်၍ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    အကယ်၍ (found_dot)
		bha_nga;
	    အကယ်၍ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    အကယ်၍ (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	အတွက် (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    အကယ်၍ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		အကယ်၍ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    အကယ်၍ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    အကယ်၍ ((c = inc(&tpos)) == -1)
			bha_nga;
		နေစဉ် (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		အကယ်၍ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    အကယ်၍ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    အကယ်၍ ((*func)(&pos) == -1)
	    {
		အကယ်၍ (count)
		    ပြန်ပေး FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	နေစဉ် (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    အကယ်၍ (incl(&pos) == -1)
		bha_nga;

	အကယ်၍ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    အကယ်၍ ((*func)(&pos) == -1)
	    {
		အကယ်၍ (count)
		    ပြန်ပေး FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ပြန်ပေး OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ကိန်းပြည့်
findpar(
    ကိန်းပြည့်		*pincl,	    // Return: TRUE if last char is to be included
    ကိန်းပြည့်		dir,
    long	count,
    ကိန်းပြည့်		what,
    ကိန်းပြည့်		both)
{
    linenr_T	curr;
    ကိန်းပြည့်		did_skip;   // TRUE after separating lines have been skipped
    ကိန်းပြည့်		first;	    // TRUE on first line
    ကိန်းပြည့်		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ကိန်းပြည့်		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    နေစဉ် (count--)
    {
	did_skip = FALSE;
	အတွက် (first = TRUE; ; first = FALSE)
	{
	    အကယ်၍ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    အကယ်၍ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    အကယ်၍ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    အကယ်၍ (fold_skipped)
		curr -= dir;
#endif
	    အကယ်၍ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		အကယ်၍ (count)
		    ပြန်ပေး FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    အကယ်၍ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    အကယ်၍ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	အကယ်၍ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    မဟုတ်ပါက
	curwin->w_cursor.col = 0;
    ပြန်ပေး TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ကိန်းပြည့်
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    အတွက် (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	အကယ်၍ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	အကယ်၍ (macro[0] == NUL)
	    bha_nga;
    }
    ပြန်ပေး (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ကိန်းပြည့်
startPS(linenr_T lnum, ကိန်းပြည့် para, ကိန်းပြည့် both)
{
    char_u	*s;

    s = ml_get(lnum);
    အကယ်၍ (*s == para || *s == '\f' || (both && *s == '}'))
	ပြန်ပေး TRUE;
    အကယ်၍ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
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

static ကိန်းပြည့်	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static ကိန်းပြည့်
cls(void)
{
    ကိန်းပြည့်	    c;

    c = gchar_cursor();
    အကယ်၍ (c == ' ' || c == '\t' || c == NUL)
	ပြန်ပေး 0;
    အကယ်၍ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	အကယ်၍ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ပြန်ပေး 1;

	// process code leading/trailing bytes
	ပြန်ပေး dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    အကယ်၍ (enc_utf8)
    {
	c = utf_class(c);
	အကယ်၍ (c != 0 && cls_bigword)
	    ပြန်ပေး 1;
	ပြန်ပေး c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    အကယ်၍ (cls_bigword)
	ပြန်ပေး 1;

    အကယ်၍ (vim_iswordc(c))
	ပြန်ပေး 2;
    ပြန်ပေး 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ကိန်းပြည့်
fwd_word(
    long	count,
    ကိန်းပြည့်		bigword,    // "W", "E" or "B"
    ကိန်းပြည့်		eol)
{
    ကိန်းပြည့်		sclass;	    // starting class
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    နေစဉ် (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	အကယ်၍ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	အကယ်၍ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ပြန်ပေး FAIL;
	အကယ်၍ (i >= 1 && eol && count == 0)      // started at last char in line
	    ပြန်ပေး OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	အကယ်၍ (sclass != 0)
	    နေစဉ် (cls() == sclass)
	    {
		i = inc_cursor();
		အကယ်၍ (i == -1 || (i >= 1 && eol && count == 0))
		    ပြန်ပေး OK;
	    }

	/*
	 * go to next non-white
	 */
	နေစဉ် (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    အကယ်၍ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    အကယ်၍ (i == -1 || (i >= 1 && eol && count == 0))
		ပြန်ပေး OK;
	}
    }
    ပြန်ပေး OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ကိန်းပြည့်
bck_word(long count, ကိန်းပြည့် bigword, ကိန်းပြည့် stop)
{
    ကိန်းပြည့်		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    နေစဉ် (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	အကယ်၍ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	အကယ်၍ (dec_cursor() == -1)		// started at start of file
	    ပြန်ပေး FAIL;

	အကယ်၍ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    နေစဉ် (cls() == 0)
	    {
		အကယ်၍ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		အကယ်၍ (dec_cursor() == -1) // hit start of file, stop here
		    ပြန်ပေး OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    အကယ်၍ (skip_chars(cls(), BACKWARD))
		ပြန်ပေး OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ပြန်ပေး OK;
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
    ကိန်းပြည့်
end_word(
    long	count,
    ကိန်းပြည့်		bigword,
    ကိန်းပြည့်		stop,
    ကိန်းပြည့်		empty)
{
    ကိန်းပြည့်		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    အကယ်၍ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    နေစဉ် (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	အကယ်၍ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	အကယ်၍ (inc_cursor() == -1)
	    ပြန်ပေး FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	အကယ်၍ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    အကယ်၍ (skip_chars(sclass, FORWARD))
		ပြန်ပေး FAIL;
	}
	မဟုတ်ပါက အကယ်၍ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    နေစဉ် (cls() == 0)
	    {
		အကယ်၍ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		အကယ်၍ (inc_cursor() == -1)	    // hit end of file, stop here
		    ပြန်ပေး FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    အကယ်၍ (skip_chars(cls(), FORWARD))
		ပြန်ပေး FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ပြန်ပေး OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ကိန်းပြည့်
bckend_word(
    long	count,
    ကိန်းပြည့်		bigword,    // TRUE for "B"
    ကိန်းပြည့်		eol)	    // TRUE: stop at end of line.
{
    ကိန်းပြည့်		sclass;	    // starting class
    ကိန်းပြည့်		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    နေစဉ် (--count >= 0)
    {
	sclass = cls();
	အကယ်၍ ((i = dec_cursor()) == -1)
	    ပြန်ပေး FAIL;
	အကယ်၍ (eol && i == 1)
	    ပြန်ပေး OK;

	/*
	 * Move backward to before the start of this word.
	 */
	အကယ်၍ (sclass != 0)
	{
	    နေစဉ် (cls() == sclass)
		အကယ်၍ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ပြန်ပေး OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	နေစဉ် (cls() == 0)
	{
	    အကယ်၍ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    အကယ်၍ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ပြန်ပေး OK;
	}
    }
    adjust_skipcol();
    ပြန်ပေး OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ကိန်းပြည့်
skip_chars(ကိန်းပြည့် cclass, ကိန်းပြည့် dir)
{
    နေစဉ် (cls() == cclass)
	အကယ်၍ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ပြန်ပေး TRUE;
    ပြန်ပေး FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ကိန်းပြည့်		sclass;		    // starting class

    sclass = cls();
    အတွက် (;;)
    {
	အကယ်၍ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	အကယ်၍ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    ကိန်းပြည့်	    c;

    နေစဉ် (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	အကယ်၍ (!VIM_ISWHITE(c))
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
    ကိန်းပြည့်	    at_start_sent)	// cursor is at start of sentence
{
    နေစဉ် (count--)
    {
	findsent(FORWARD, 1L);
	အကယ်၍ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	အကယ်၍ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ကိန်းပြည့်
current_word(
    oparg_T	*oap,
    long	count,
    ကိန်းပြည့်		include,	// TRUE: include word and white space
    ကိန်းပြည့်		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ကိန်းပြည့်		inclusive = TRUE;
    ကိန်းပြည့်		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    အကယ်၍ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    အကယ်၍ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	အကယ်၍ ((cls() == 0) == include)
	{
	    အကယ်၍ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ပြန်ပေး FAIL;
	}
	မဟုတ်ပါက
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    အကယ်၍ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    မဟုတ်ပါက
		oneleft();

	    အကယ်၍ (include)
		include_white = TRUE;
	}

	အကယ်၍ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	မဟုတ်ပါက
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    နေစဉ် (count > 0)
    {
	inclusive = TRUE;
	အကယ်၍ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    အကယ်၍ (decl(&curwin->w_cursor) == -1)
		ပြန်ပေး FAIL;
	    အကယ်၍ (include != (cls() != 0))
	    {
		အကယ်၍ (bck_word(1L, bigword, TRUE) == FAIL)
		    ပြန်ပေး FAIL;
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ပြန်ပေး FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	မဟုတ်ပါက
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    အကယ်၍ (incl(&curwin->w_cursor) == -1)
		ပြန်ပေး FAIL;
	    အကယ်၍ (include != (cls() == 0))
	    {
		အကယ်၍ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ပြန်ပေး FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		အကယ်၍ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    မဟုတ်ပါက
	    {
		အကယ်၍ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ပြန်ပေး FAIL;
	    }
	}
	--count;
    }

    အကယ်၍ (include_white && (cls() != 0
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
	အကယ်၍ (oneleft() == OK)
	{
	    back_in_line();
	    အကယ်၍ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		အကယ်၍ (VIsual_active)
		    VIsual = curwin->w_cursor;
		မဟုတ်ပါက
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    အကယ်၍ (VIsual_active)
    {
	အကယ်၍ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	အကယ်၍ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    မဟုတ်ပါက
	oap->inclusive = inclusive;

    ပြန်ပေး OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ကိန်းပြည့်
current_sent(oparg_T *oap, long count, ကိန်းပြည့် include)
{
    pos_T	start_pos;
    pos_T	pos;
    ကိန်းပြည့်		start_blank;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    အကယ်၍ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	အကယ်၍ (LT_POS(start_pos, VIsual))
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
	    နေစဉ် (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		အကယ်၍ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    အကယ်၍ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		အကယ်၍ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		မဟုတ်ပါက
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    အကယ်၍ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    နေစဉ် (count--)
	    {
		အကယ်၍ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		အကယ်၍ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	မဟုတ်ပါက
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
	    အကယ်၍ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		နေစဉ် (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    အကယ်၍ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		အကယ်၍ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		မဟုတ်ပါက		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    အကယ်၍ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    အကယ်၍ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ပြန်ပေး OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    နေစဉ် (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    အကယ်၍ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    မဟုတ်ပါက
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    အကယ်၍ (include)
	ncount = count * 2;
    မဟုတ်ပါက
    {
	ncount = count;
	အကယ်၍ (start_blank)
	    --ncount;
    }
    အကယ်၍ (ncount > 0)
	findsent_forward(ncount, TRUE);
    မဟုတ်ပါက
	decl(&curwin->w_cursor);

    အကယ်၍ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	အကယ်၍ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    အကယ်၍ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	မဟုတ်ပါက အကယ်၍ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    အကယ်၍ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	အကယ်၍ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	အကယ်၍ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    မဟုတ်ပါက
    {
	// include a newline after the sentence, if there is one
	အကယ်၍ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	မဟုတ်ပါက
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ပြန်ပေး OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ကိန်းပြည့်
current_block(
    oparg_T	*oap,
    long	count,
    ကိန်းပြည့်		include,	// TRUE == include white space
    ကိန်းပြည့်		what,		// '(', '{', etc.
    ကိန်းပြည့်		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ကိန်းပြည့်		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    အကယ်၍ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	အကယ်၍ (what == '{')		// ignore indent
	    နေစဉ် (inindent(1))
		အကယ်၍ (inc_cursor() != 0)
		    bha_nga;
	အကယ်၍ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    မဟုတ်ပါက အကယ်၍ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    မဟုတ်ပါက
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    အကယ်၍ ((pos = findmatch(NULL, what)) != NULL)
    {
	နေစဉ် (count-- > 0)
	{
	    အကယ်၍ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    မဟုတ်ပါက
    {
	နေစဉ် (count-- > 0)
	{
	    အကယ်၍ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    အကယ်၍ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ပြန်ပေး FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    နေစဉ် (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	နေစဉ် (inindent(1))
	{
	    sol = TRUE;
	    အကယ်၍ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	အကယ်၍ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ပြန်ပေး FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	အကယ်၍ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    အကယ်၍ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ပြန်ပေး FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    အကယ်၍ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ပြန်ပေး FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	မဟုတ်ပါက
	    bha_nga;
    }

    အကယ်၍ (VIsual_active)
    {
	အကယ်၍ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	အကယ်၍ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    မဟုတ်ပါက
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	အကယ်၍ (sol)
	    incl(&curwin->w_cursor);
	မဟုတ်ပါက အကယ်၍ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	မဟုတ်ပါက
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ပြန်ပေး OK;
}

#အကယ်၍ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static ကိန်းပြည့်
in_html_tag(
    ကိန်းပြည့်		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    ကိန်းပြည့်		c;
    ကိန်းပြည့်		lc = NUL;
    pos_T	pos;

    အကယ်၍ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	အတွက် (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    အကယ်၍ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	အကယ်၍ (*p != '<')	    // check for '<' under cursor
	{
	    အကယ်၍ (lc != '<')
		ပြန်ပေး FALSE;
	    p = lp;
	}
    }
    မဟုတ်ပါက
    {
	အတွက် (p = line + curwin->w_cursor.col; p > line; )
	{
	    အကယ်၍ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    အကယ်၍ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	အကယ်၍ (*p != '<')
	    ပြန်ပေး FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    အကယ်၍ (end_tag)
	// check that there is a '/' after the '<'
	ပြန်ပေး *p == '/';

    // check that there is no '/' after the '<'
    အကယ်၍ (*p == '/')
	ပြန်ပေး FALSE;

    // check that the matching '>' is not preceded by '/'
    အတွက် (;;)
    {
	အကယ်၍ (inc(&pos) < 0)
	    ပြန်ပေး FALSE;
	c = *ml_get_pos(&pos);
	အကယ်၍ (c == '>')
	    bha_nga;
	lc = c;
    }
    ပြန်ပေး lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ကိန်းပြည့်
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    ကိန်းပြည့်		include)	// TRUE == include white space
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
    ကိန်းပြည့်		len;
    ကိန်းပြည့်		r;
    ကိန်းပြည့်		do_include = include;
    ကိန်းပြည့်		save_p_ws = p_ws;
    ကိန်းပြည့်		retval = FAIL;
    ကိန်းပြည့်		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    အကယ်၍ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    အကယ်၍ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	နေစဉ် (inindent(1))
	    အကယ်၍ (inc_cursor() != 0)
		bha_nga;

	အကယ်၍ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    နေစဉ် (*ml_get_cursor() != '>')
		အကယ်၍ (inc_cursor() < 0)
		    bha_nga;
	}
	မဟုတ်ပါက အကယ်၍ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    နေစဉ် (*ml_get_cursor() != '<')
		အကယ်၍ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    မဟုတ်ပါက အကယ်၍ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    မဟုတ်ပါက
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    အတွက် (n = 0; n < count; ++n)
    {
	အကယ်၍ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    အတွက် (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ကိန်းပြည့်)(cp - p);
    အကယ်၍ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    အကယ်၍ (spat == NULL || epat == NULL)
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

    အကယ်၍ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    အကယ်၍ (do_include)
    {
	// Include up to the '>'.
	နေစဉ် (*ml_get_cursor() != '>')
	    အကယ်၍ (inc_cursor() < 0)
		bha_nga;
    }
    မဟုတ်ပါက
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	အကယ်၍ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	မဟုတ်ပါက အကယ်၍ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    အကယ်၍ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ကိန်းပြည့် in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	နေစဉ် (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    အကယ်၍ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    မဟုတ်ပါက အကယ်၍ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	အကယ်၍ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    အကယ်၍ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	အကယ်၍ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	မဟုတ်ပါက အကယ်၍ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    မဟုတ်ပါက
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	အကယ်၍ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	မဟုတ်ပါက
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ပြန်ပေး retval;
}
#endif

    ကိန်းပြည့်
current_par(
    oparg_T	*oap,
    long	count,
    ကိန်းပြည့်		include,	// TRUE == include white space
    ကိန်းပြည့်		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ကိန်းပြည့်		white_in_front;
    ကိန်းပြည့်		dir;
    ကိန်းပြည့်		start_is_white;
    ကိန်းပြည့်		prev_start_is_white;
    ကိန်းပြည့်		retval = OK;
    ကိန်းပြည့်		do_white = FALSE;
    ကိန်းပြည့်		t;
    ကိန်းပြည့်		i;

    အကယ်၍ (type == 'S')	    // not implemented yet
	ပြန်ပေး FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    အကယ်၍ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	အကယ်၍ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	မဟုတ်ပါက
	    dir = FORWARD;
	အတွက် (i = count; --i >= 0; )
	{
	    အကယ်၍ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    အတွက် (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		အကယ်၍ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		အတွက် (;;)
		{
		    အကယ်၍ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    အကယ်၍ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		အကယ်၍ (!include)
		    bha_nga;
		အကယ်၍ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ပြန်ပေး retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    နေစဉ် (start_lnum > 1)
    {
	အကယ်၍ (white_in_front)	    // stop at first white line
	{
	    အကယ်၍ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	မဟုတ်ပါက		// stop at first non-white line of start of paragraph
	{
	    အကယ်၍ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    နေစဉ် (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    အကယ်၍ (!include && white_in_front)
	--i;
    နေစဉ် (i--)
    {
	အကယ်၍ (end_lnum == curbuf->b_ml.ml_line_count)
	    ပြန်ပေး FAIL;

	အကယ်၍ (!include)
	    do_white = linewhite(end_lnum + 1);

	အကယ်၍ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    နေစဉ် (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	အကယ်၍ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	အကယ်၍ (include || do_white)
	    နေစဉ် (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    အကယ်၍ (!white_in_front && !linewhite(end_lnum) && include)
	နေစဉ် (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    အကယ်၍ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	အကယ်၍ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	အကယ်၍ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    မဟုတ်ပါက
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ပြန်ပေး OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static ကိန်းပြည့်
find_next_quote(
    char_u	*line,
    ကိန်းပြည့်		col,
    ကိန်းပြည့်		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ကိန်းပြည့်		c;

    အတွက် (;;)
    {
	c = line[col];
	အကယ်၍ (c == NUL)
	    ပြန်ပေး -1;
	မဟုတ်ပါက အကယ်၍ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    အကယ်၍ (line[col] == NUL)
		ပြန်ပေး -1;
	}
	မဟုတ်ပါက အကယ်၍ (c == quotechar)
	    bha_nga;
	အကယ်၍ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	မဟုတ်ပါက
	    ++col;
    }
    ပြန်ပေး col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static ကိန်းပြည့်
find_prev_quote(
    char_u	*line,
    ကိန်းပြည့်		col_start,
    ကိန်းပြည့်		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ကိန်းပြည့်		n;

    နေစဉ် (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	အကယ်၍ (escape != NULL)
	    နေစဉ် (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	အကယ်၍ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	မဟုတ်ပါက အကယ်၍ (line[col_start] == quotechar)
	    bha_nga;
    }
    ပြန်ပေး col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ကိန်းပြည့်
current_quote(
    oparg_T	*oap,
    long	count,
    ကိန်းပြည့်		include,	// TRUE == include quote char
    ကိန်းပြည့်		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    ကိန်းပြည့်		col_end;
    ကိန်းပြည့်		col_start = curwin->w_cursor.col;
    ကိန်းပြည့်		inclusive = FALSE;
    ကိန်းပြည့်		vis_empty = TRUE;	// Visual selection <= 1 char
    ကိန်းပြည့်		vis_bef_curs = FALSE;	// Visual starts before cursor
    ကိန်းပြည့်		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ကိန်းပြည့်		inside_quotes = FALSE;	// Looks like "i'" done before
    ကိန်းပြည့်		selected_quote = FALSE;	// Has quote inside selection
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    အကယ်၍ (VIsual_active)
    {
	// this only works within one line
	အကယ်၍ (VIsual.lnum != curwin->w_cursor.lnum)
	    ပြန်ပေး FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	အကယ်၍ (*p_sel == 'e')
	{
	    အကယ်၍ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    မဟုတ်ပါက အကယ်၍ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    အကယ်၍ (!vis_bef_curs && !vis_empty)
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

    အကယ်၍ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	အကယ်၍ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	မဟုတ်ပါက
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	နေစဉ် (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    အကယ်၍ (line[i] == NUL)
		bha_nga;
	    အကယ်၍ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    အကယ်၍ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	အကယ်၍ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    အကယ်၍ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    အကယ်၍ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	မဟုတ်ပါက
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    အကယ်၍ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    အကယ်၍ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    မဟုတ်ပါက

    အကယ်၍ (line[col_start] == quotechar || !vis_empty)
    {
	ကိန်းပြည့်	first_col = col_start;

	အကယ်၍ (!vis_empty)
	{
	    အကယ်၍ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    မဟုတ်ပါက
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	အတွက် (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    အကယ်၍ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    အကယ်၍ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    အကယ်၍ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    မဟုတ်ပါက
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	အကယ်၍ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    အကယ်၍ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	အကယ်၍ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    အကယ်၍ (include)
    {
	အကယ်၍ (VIM_ISWHITE(line[col_end + 1]))
	    နေစဉ် (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	မဟုတ်ပါက
	    နေစဉ် (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    အကယ်၍ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    အကယ်၍ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	အကယ်၍ (vis_empty
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
    မဟုတ်ပါက
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    အကယ်၍ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    အကယ်၍ (VIsual_active)
    {
	အကယ်၍ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    အကယ်၍ (*p_sel != 'e')
		dec_cursor();
	}
	မဟုတ်ပါက
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    အကယ်၍ (inside_quotes
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
	အကယ်၍ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    မဟုတ်ပါက
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ပြန်ပေး OK;

abort_search:
    အကယ်၍ (VIsual_active && *p_sel == 'e')
    {
	အကယ်၍ (did_exclusive_adj)
	    inc_cursor();
	အကယ်၍ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ပြန်ပေး FALSE;
}
