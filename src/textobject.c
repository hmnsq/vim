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

static ཧྲིལ cls(void);
static ཧྲིལ skip_chars(ཧྲིལ, ཧྲིལ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ཧྲིལ
findsent(ཧྲིལ dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ཧྲིལ		c;
    ཧྲིལ		(*func)(pos_T *);
    ཧྲིལ		startlnum;
    ཧྲིལ		noskip = FALSE;	    // do not skip blanks
    ཧྲིལ		cpo_J;
    ཧྲིལ		found_dot;

    pos = curwin->w_cursor;
    གལ་ཏེ (dir == FORWARD)
	func = incl;
    དེ་མིན
	func = decl;

    སྐབས (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	གལ་ཏེ (gchar_pos(&pos) == NUL)
	{
	    do
		གལ་ཏེ ((*func)(&pos) == -1)
		    bha_nga;
	    སྐབས (gchar_pos(&pos) == NUL);
	    གལ་ཏེ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	དེ་མིན གལ་ཏེ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    གལ་ཏེ (pos.lnum == curbuf->b_ml.ml_line_count)
		ཕྱིར FAIL;
	    ++pos.lnum;
	    goto found;
	}
	དེ་མིན གལ་ཏེ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	སྐབས (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    གལ་ཏེ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    གལ་ཏེ (found_dot)
		bha_nga;
	    གལ་ཏེ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    གལ་ཏེ (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	ཆེད་དུ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    གལ་ཏེ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		གལ་ཏེ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    གལ་ཏེ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    གལ་ཏེ ((c = inc(&tpos)) == -1)
			bha_nga;
		སྐབས (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		གལ་ཏེ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    གལ་ཏེ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    གལ་ཏེ ((*func)(&pos) == -1)
	    {
		གལ་ཏེ (count)
		    ཕྱིར FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	སྐབས (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    གལ་ཏེ (incl(&pos) == -1)
		bha_nga;

	གལ་ཏེ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    གལ་ཏེ ((*func)(&pos) == -1)
	    {
		གལ་ཏེ (count)
		    ཕྱིར FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ཕྱིར OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ཧྲིལ
findpar(
    ཧྲིལ		*pincl,	    // Return: TRUE if last char is to be included
    ཧྲིལ		dir,
    long	count,
    ཧྲིལ		what,
    ཧྲིལ		both)
{
    linenr_T	curr;
    ཧྲིལ		did_skip;   // TRUE after separating lines have been skipped
    ཧྲིལ		first;	    // TRUE on first line
    ཧྲིལ		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ཧྲིལ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    སྐབས (count--)
    {
	did_skip = FALSE;
	ཆེད་དུ (first = TRUE; ; first = FALSE)
	{
	    གལ་ཏེ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    གལ་ཏེ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    གལ་ཏེ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    གལ་ཏེ (fold_skipped)
		curr -= dir;
#endif
	    གལ་ཏེ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		གལ་ཏེ (count)
		    ཕྱིར FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    གལ་ཏེ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    གལ་ཏེ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	གལ་ཏེ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    དེ་མིན
	curwin->w_cursor.col = 0;
    ཕྱིར TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ཧྲིལ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ཆེད་དུ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	གལ་ཏེ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	གལ་ཏེ (macro[0] == NUL)
	    bha_nga;
    }
    ཕྱིར (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ཧྲིལ
startPS(linenr_T lnum, ཧྲིལ para, ཧྲིལ both)
{
    char_u	*s;

    s = ml_get(lnum);
    གལ་ཏེ (*s == para || *s == '\f' || (both && *s == '}'))
	ཕྱིར TRUE;
    གལ་ཏེ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ཕྱིར TRUE;
    ཕྱིར FALSE;
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

static ཧྲིལ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static ཧྲིལ
cls(void)
{
    ཧྲིལ	    c;

    c = gchar_cursor();
    གལ་ཏེ (c == ' ' || c == '\t' || c == NUL)
	ཕྱིར 0;
    གལ་ཏེ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	གལ་ཏེ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ཕྱིར 1;

	// process code leading/trailing bytes
	ཕྱིར dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    གལ་ཏེ (enc_utf8)
    {
	c = utf_class(c);
	གལ་ཏེ (c != 0 && cls_bigword)
	    ཕྱིར 1;
	ཕྱིར c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    གལ་ཏེ (cls_bigword)
	ཕྱིར 1;

    གལ་ཏེ (vim_iswordc(c))
	ཕྱིར 2;
    ཕྱིར 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ཧྲིལ
fwd_word(
    long	count,
    ཧྲིལ		bigword,    // "W", "E" or "B"
    ཧྲིལ		eol)
{
    ཧྲིལ		sclass;	    // starting class
    ཧྲིལ		i;
    ཧྲིལ		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    སྐབས (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	གལ་ཏེ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	གལ་ཏེ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ཕྱིར FAIL;
	གལ་ཏེ (i >= 1 && eol && count == 0)      // started at last char in line
	    ཕྱིར OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	གལ་ཏེ (sclass != 0)
	    སྐབས (cls() == sclass)
	    {
		i = inc_cursor();
		གལ་ཏེ (i == -1 || (i >= 1 && eol && count == 0))
		    ཕྱིར OK;
	    }

	/*
	 * go to next non-white
	 */
	སྐབས (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    གལ་ཏེ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    གལ་ཏེ (i == -1 || (i >= 1 && eol && count == 0))
		ཕྱིར OK;
	}
    }
    ཕྱིར OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ཧྲིལ
bck_word(long count, ཧྲིལ bigword, ཧྲིལ stop)
{
    ཧྲིལ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    སྐབས (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	གལ་ཏེ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	གལ་ཏེ (dec_cursor() == -1)		// started at start of file
	    ཕྱིར FAIL;

	གལ་ཏེ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    སྐབས (cls() == 0)
	    {
		གལ་ཏེ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		གལ་ཏེ (dec_cursor() == -1) // hit start of file, stop here
		    ཕྱིར OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    གལ་ཏེ (skip_chars(cls(), BACKWARD))
		ཕྱིར OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ཕྱིར OK;
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
    ཧྲིལ
end_word(
    long	count,
    ཧྲིལ		bigword,
    ཧྲིལ		stop,
    ཧྲིལ		empty)
{
    ཧྲིལ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    གལ་ཏེ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    སྐབས (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	གལ་ཏེ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	གལ་ཏེ (inc_cursor() == -1)
	    ཕྱིར FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	གལ་ཏེ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    གལ་ཏེ (skip_chars(sclass, FORWARD))
		ཕྱིར FAIL;
	}
	དེ་མིན གལ་ཏེ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    སྐབས (cls() == 0)
	    {
		གལ་ཏེ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		གལ་ཏེ (inc_cursor() == -1)	    // hit end of file, stop here
		    ཕྱིར FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    གལ་ཏེ (skip_chars(cls(), FORWARD))
		ཕྱིར FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ཕྱིར OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ཧྲིལ
bckend_word(
    long	count,
    ཧྲིལ		bigword,    // TRUE for "B"
    ཧྲིལ		eol)	    // TRUE: stop at end of line.
{
    ཧྲིལ		sclass;	    // starting class
    ཧྲིལ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    སྐབས (--count >= 0)
    {
	sclass = cls();
	གལ་ཏེ ((i = dec_cursor()) == -1)
	    ཕྱིར FAIL;
	གལ་ཏེ (eol && i == 1)
	    ཕྱིར OK;

	/*
	 * Move backward to before the start of this word.
	 */
	གལ་ཏེ (sclass != 0)
	{
	    སྐབས (cls() == sclass)
		གལ་ཏེ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ཕྱིར OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	སྐབས (cls() == 0)
	{
	    གལ་ཏེ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    གལ་ཏེ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ཕྱིར OK;
	}
    }
    adjust_skipcol();
    ཕྱིར OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ཧྲིལ
skip_chars(ཧྲིལ cclass, ཧྲིལ dir)
{
    སྐབས (cls() == cclass)
	གལ་ཏེ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ཕྱིར TRUE;
    ཕྱིར FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ཧྲིལ		sclass;		    // starting class

    sclass = cls();
    ཆེད་དུ (;;)
    {
	གལ་ཏེ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	གལ་ཏེ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    ཧྲིལ	    c;

    སྐབས (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	གལ་ཏེ (!VIM_ISWHITE(c))
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
    ཧྲིལ	    at_start_sent)	// cursor is at start of sentence
{
    སྐབས (count--)
    {
	findsent(FORWARD, 1L);
	གལ་ཏེ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	གལ་ཏེ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ཧྲིལ
current_word(
    oparg_T	*oap,
    long	count,
    ཧྲིལ		include,	// TRUE: include word and white space
    ཧྲིལ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ཧྲིལ		inclusive = TRUE;
    ཧྲིལ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    གལ་ཏེ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    གལ་ཏེ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	གལ་ཏེ ((cls() == 0) == include)
	{
	    གལ་ཏེ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ཕྱིར FAIL;
	}
	དེ་མིན
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    གལ་ཏེ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    དེ་མིན
		oneleft();

	    གལ་ཏེ (include)
		include_white = TRUE;
	}

	གལ་ཏེ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	དེ་མིན
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    སྐབས (count > 0)
    {
	inclusive = TRUE;
	གལ་ཏེ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    གལ་ཏེ (decl(&curwin->w_cursor) == -1)
		ཕྱིར FAIL;
	    གལ་ཏེ (include != (cls() != 0))
	    {
		གལ་ཏེ (bck_word(1L, bigword, TRUE) == FAIL)
		    ཕྱིར FAIL;
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ཕྱིར FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	དེ་མིན
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    གལ་ཏེ (incl(&curwin->w_cursor) == -1)
		ཕྱིར FAIL;
	    གལ་ཏེ (include != (cls() == 0))
	    {
		གལ་ཏེ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ཕྱིར FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		གལ་ཏེ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ཕྱིར FAIL;
	    }
	}
	--count;
    }

    གལ་ཏེ (include_white && (cls() != 0
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
	གལ་ཏེ (oneleft() == OK)
	{
	    back_in_line();
	    གལ་ཏེ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		གལ་ཏེ (VIsual_active)
		    VIsual = curwin->w_cursor;
		དེ་མིན
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    གལ་ཏེ (VIsual_active)
    {
	གལ་ཏེ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	གལ་ཏེ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    དེ་མིན
	oap->inclusive = inclusive;

    ཕྱིར OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ཧྲིལ
current_sent(oparg_T *oap, long count, ཧྲིལ include)
{
    pos_T	start_pos;
    pos_T	pos;
    ཧྲིལ		start_blank;
    ཧྲིལ		c;
    ཧྲིལ		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    གལ་ཏེ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	གལ་ཏེ (LT_POS(start_pos, VIsual))
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
	    སྐབས (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		གལ་ཏེ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    གལ་ཏེ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		གལ་ཏེ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		དེ་མིན
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    གལ་ཏེ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    སྐབས (count--)
	    {
		གལ་ཏེ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		གལ་ཏེ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	དེ་མིན
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
	    གལ་ཏེ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		སྐབས (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    གལ་ཏེ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		གལ་ཏེ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		དེ་མིན		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    གལ་ཏེ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    གལ་ཏེ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ཕྱིར OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    སྐབས (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    གལ་ཏེ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    དེ་མིན
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    གལ་ཏེ (include)
	ncount = count * 2;
    དེ་མིན
    {
	ncount = count;
	གལ་ཏེ (start_blank)
	    --ncount;
    }
    གལ་ཏེ (ncount > 0)
	findsent_forward(ncount, TRUE);
    དེ་མིན
	decl(&curwin->w_cursor);

    གལ་ཏེ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	གལ་ཏེ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    གལ་ཏེ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	དེ་མིན གལ་ཏེ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    གལ་ཏེ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	གལ་ཏེ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	གལ་ཏེ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    དེ་མིན
    {
	// include a newline after the sentence, if there is one
	གལ་ཏེ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	དེ་མིན
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ཕྱིར OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ཧྲིལ
current_block(
    oparg_T	*oap,
    long	count,
    ཧྲིལ		include,	// TRUE == include white space
    ཧྲིལ		what,		// '(', '{', etc.
    ཧྲིལ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ཧྲིལ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    གལ་ཏེ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	གལ་ཏེ (what == '{')		// ignore indent
	    སྐབས (inindent(1))
		གལ་ཏེ (inc_cursor() != 0)
		    bha_nga;
	གལ་ཏེ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    དེ་མིན གལ་ཏེ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    དེ་མིན
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    གལ་ཏེ ((pos = findmatch(NULL, what)) != NULL)
    {
	སྐབས (count-- > 0)
	{
	    གལ་ཏེ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    དེ་མིན
    {
	སྐབས (count-- > 0)
	{
	    གལ་ཏེ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    གལ་ཏེ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ཕྱིར FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    སྐབས (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	སྐབས (inindent(1))
	{
	    sol = TRUE;
	    གལ་ཏེ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	གལ་ཏེ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ཕྱིར FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	གལ་ཏེ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    གལ་ཏེ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ཕྱིར FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    གལ་ཏེ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ཕྱིར FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	དེ་མིན
	    bha_nga;
    }

    གལ་ཏེ (VIsual_active)
    {
	གལ་ཏེ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	གལ་ཏེ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    དེ་མིན
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	གལ་ཏེ (sol)
	    incl(&curwin->w_cursor);
	དེ་མིན གལ་ཏེ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	དེ་མིན
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ཕྱིར OK;
}

#གལ་ཏེ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static ཧྲིལ
in_html_tag(
    ཧྲིལ		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    ཧྲིལ		c;
    ཧྲིལ		lc = NUL;
    pos_T	pos;

    གལ་ཏེ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ཆེད་དུ (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    གལ་ཏེ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	གལ་ཏེ (*p != '<')	    // check for '<' under cursor
	{
	    གལ་ཏེ (lc != '<')
		ཕྱིར FALSE;
	    p = lp;
	}
    }
    དེ་མིན
    {
	ཆེད་དུ (p = line + curwin->w_cursor.col; p > line; )
	{
	    གལ་ཏེ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    གལ་ཏེ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	གལ་ཏེ (*p != '<')
	    ཕྱིར FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    གལ་ཏེ (end_tag)
	// check that there is a '/' after the '<'
	ཕྱིར *p == '/';

    // check that there is no '/' after the '<'
    གལ་ཏེ (*p == '/')
	ཕྱིར FALSE;

    // check that the matching '>' is not preceded by '/'
    ཆེད་དུ (;;)
    {
	གལ་ཏེ (inc(&pos) < 0)
	    ཕྱིར FALSE;
	c = *ml_get_pos(&pos);
	གལ་ཏེ (c == '>')
	    bha_nga;
	lc = c;
    }
    ཕྱིར lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ཧྲིལ
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    ཧྲིལ		include)	// TRUE == include white space
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
    ཧྲིལ		len;
    ཧྲིལ		r;
    ཧྲིལ		do_include = include;
    ཧྲིལ		save_p_ws = p_ws;
    ཧྲིལ		retval = FAIL;
    ཧྲིལ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    གལ་ཏེ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    གལ་ཏེ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	སྐབས (inindent(1))
	    གལ་ཏེ (inc_cursor() != 0)
		bha_nga;

	གལ་ཏེ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    སྐབས (*ml_get_cursor() != '>')
		གལ་ཏེ (inc_cursor() < 0)
		    bha_nga;
	}
	དེ་མིན གལ་ཏེ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    སྐབས (*ml_get_cursor() != '<')
		གལ་ཏེ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    དེ་མིན གལ་ཏེ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    དེ་མིན
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ཆེད་དུ (n = 0; n < count; ++n)
    {
	གལ་ཏེ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    ཆེད་དུ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ཧྲིལ)(cp - p);
    གལ་ཏེ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    གལ་ཏེ (spat == NULL || epat == NULL)
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

    གལ་ཏེ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    གལ་ཏེ (do_include)
    {
	// Include up to the '>'.
	སྐབས (*ml_get_cursor() != '>')
	    གལ་ཏེ (inc_cursor() < 0)
		bha_nga;
    }
    དེ་མིན
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	གལ་ཏེ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	དེ་མིན གལ་ཏེ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    གལ་ཏེ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ཧྲིལ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	སྐབས (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    གལ་ཏེ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    དེ་མིན གལ་ཏེ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	གལ་ཏེ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    གལ་ཏེ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	གལ་ཏེ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	དེ་མིན གལ་ཏེ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    དེ་མིན
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	གལ་ཏེ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	དེ་མིན
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ཕྱིར retval;
}
#endif

    ཧྲིལ
current_par(
    oparg_T	*oap,
    long	count,
    ཧྲིལ		include,	// TRUE == include white space
    ཧྲིལ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ཧྲིལ		white_in_front;
    ཧྲིལ		dir;
    ཧྲིལ		start_is_white;
    ཧྲིལ		prev_start_is_white;
    ཧྲིལ		retval = OK;
    ཧྲིལ		do_white = FALSE;
    ཧྲིལ		t;
    ཧྲིལ		i;

    གལ་ཏེ (type == 'S')	    // not implemented yet
	ཕྱིར FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    གལ་ཏེ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	གལ་ཏེ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	དེ་མིན
	    dir = FORWARD;
	ཆེད་དུ (i = count; --i >= 0; )
	{
	    གལ་ཏེ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    ཆེད་དུ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		གལ་ཏེ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		ཆེད་དུ (;;)
		{
		    གལ་ཏེ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    གལ་ཏེ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		གལ་ཏེ (!include)
		    bha_nga;
		གལ་ཏེ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ཕྱིར retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    སྐབས (start_lnum > 1)
    {
	གལ་ཏེ (white_in_front)	    // stop at first white line
	{
	    གལ་ཏེ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	དེ་མིན		// stop at first non-white line of start of paragraph
	{
	    གལ་ཏེ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    སྐབས (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    གལ་ཏེ (!include && white_in_front)
	--i;
    སྐབས (i--)
    {
	གལ་ཏེ (end_lnum == curbuf->b_ml.ml_line_count)
	    ཕྱིར FAIL;

	གལ་ཏེ (!include)
	    do_white = linewhite(end_lnum + 1);

	གལ་ཏེ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    སྐབས (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	གལ་ཏེ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	གལ་ཏེ (include || do_white)
	    སྐབས (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    གལ་ཏེ (!white_in_front && !linewhite(end_lnum) && include)
	སྐབས (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    གལ་ཏེ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	གལ་ཏེ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	གལ་ཏེ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    དེ་མིན
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ཕྱིར OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static ཧྲིལ
find_next_quote(
    char_u	*line,
    ཧྲིལ		col,
    ཧྲིལ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ཧྲིལ		c;

    ཆེད་དུ (;;)
    {
	c = line[col];
	གལ་ཏེ (c == NUL)
	    ཕྱིར -1;
	དེ་མིན གལ་ཏེ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    གལ་ཏེ (line[col] == NUL)
		ཕྱིར -1;
	}
	དེ་མིན གལ་ཏེ (c == quotechar)
	    bha_nga;
	གལ་ཏེ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	དེ་མིན
	    ++col;
    }
    ཕྱིར col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static ཧྲིལ
find_prev_quote(
    char_u	*line,
    ཧྲིལ		col_start,
    ཧྲིལ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ཧྲིལ		n;

    སྐབས (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	གལ་ཏེ (escape != NULL)
	    སྐབས (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	གལ་ཏེ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	དེ་མིན གལ་ཏེ (line[col_start] == quotechar)
	    bha_nga;
    }
    ཕྱིར col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ཧྲིལ
current_quote(
    oparg_T	*oap,
    long	count,
    ཧྲིལ		include,	// TRUE == include quote char
    ཧྲིལ		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    ཧྲིལ		col_end;
    ཧྲིལ		col_start = curwin->w_cursor.col;
    ཧྲིལ		inclusive = FALSE;
    ཧྲིལ		vis_empty = TRUE;	// Visual selection <= 1 char
    ཧྲིལ		vis_bef_curs = FALSE;	// Visual starts before cursor
    ཧྲིལ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ཧྲིལ		inside_quotes = FALSE;	// Looks like "i'" done before
    ཧྲིལ		selected_quote = FALSE;	// Has quote inside selection
    ཧྲིལ		i;
    ཧྲིལ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    གལ་ཏེ (VIsual_active)
    {
	// this only works within one line
	གལ་ཏེ (VIsual.lnum != curwin->w_cursor.lnum)
	    ཕྱིར FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	གལ་ཏེ (*p_sel == 'e')
	{
	    གལ་ཏེ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    དེ་མིན གལ་ཏེ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    གལ་ཏེ (!vis_bef_curs && !vis_empty)
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

    གལ་ཏེ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	གལ་ཏེ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	དེ་མིན
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	སྐབས (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    གལ་ཏེ (line[i] == NUL)
		bha_nga;
	    གལ་ཏེ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    གལ་ཏེ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	གལ་ཏེ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    གལ་ཏེ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    གལ་ཏེ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	དེ་མིན
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    གལ་ཏེ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    གལ་ཏེ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    དེ་མིན

    གལ་ཏེ (line[col_start] == quotechar || !vis_empty)
    {
	ཧྲིལ	first_col = col_start;

	གལ་ཏེ (!vis_empty)
	{
	    གལ་ཏེ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    དེ་མིན
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	ཆེད་དུ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    གལ་ཏེ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    གལ་ཏེ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    གལ་ཏེ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    དེ་མིན
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	གལ་ཏེ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    གལ་ཏེ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	གལ་ཏེ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    གལ་ཏེ (include)
    {
	གལ་ཏེ (VIM_ISWHITE(line[col_end + 1]))
	    སྐབས (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	དེ་མིན
	    སྐབས (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    གལ་ཏེ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    གལ་ཏེ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	གལ་ཏེ (vis_empty
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
    དེ་མིན
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    གལ་ཏེ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    གལ་ཏེ (VIsual_active)
    {
	གལ་ཏེ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    གལ་ཏེ (*p_sel != 'e')
		dec_cursor();
	}
	དེ་མིན
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    གལ་ཏེ (inside_quotes
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
	གལ་ཏེ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    དེ་མིན
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ཕྱིར OK;

abort_search:
    གལ་ཏེ (VIsual_active && *p_sel == 'e')
    {
	གལ་ཏེ (did_exclusive_adj)
	    inc_cursor();
	གལ་ཏེ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ཕྱིར FALSE;
}
