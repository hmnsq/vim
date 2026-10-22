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
    གལ་སྲིད (dir == FORWARD)
	func = incl;
    དེ་མིན
	func = decl;

    སྐབས (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	གལ་སྲིད (gchar_pos(&pos) == NUL)
	{
	    do
		གལ་སྲིད ((*func)(&pos) == -1)
		    bha_nga;
	    སྐབས (gchar_pos(&pos) == NUL);
	    གལ་སྲིད (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	དེ་མིན གལ་སྲིད (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    གལ་སྲིད (pos.lnum == curbuf->b_ml.ml_line_count)
		ལོག FAIL;
	    ++pos.lnum;
	    goto found;
	}
	དེ་མིན གལ་སྲིད (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	སྐབས (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    གལ་སྲིད (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    གལ་སྲིད (found_dot)
		bha_nga;
	    གལ་སྲིད (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    གལ་སྲིད (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	དོན་ལུ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    གལ་སྲིད (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		གལ་སྲིད (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    གལ་སྲིད (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    གལ་སྲིད ((c = inc(&tpos)) == -1)
			bha_nga;
		སྐབས (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		གལ་སྲིད (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    གལ་སྲིད (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    གལ་སྲིད ((*func)(&pos) == -1)
	    {
		གལ་སྲིད (count)
		    ལོག FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	སྐབས (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    གལ་སྲིད (incl(&pos) == -1)
		bha_nga;

	གལ་སྲིད (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    གལ་སྲིད ((*func)(&pos) == -1)
	    {
		གལ་སྲིད (count)
		    ལོག FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ལོག OK;
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
	དོན་ལུ (first = TRUE; ; first = FALSE)
	{
	    གལ་སྲིད (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    གལ་སྲིད (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    གལ་སྲིད (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    གལ་སྲིད (fold_skipped)
		curr -= dir;
#endif
	    གལ་སྲིད ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		གལ་སྲིད (count)
		    ལོག FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    གལ་སྲིད (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    གལ་སྲིད (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	གལ་སྲིད ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    དེ་མིན
	curwin->w_cursor.col = 0;
    ལོག TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ཧྲིལ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    དོན་ལུ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	གལ་སྲིད (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	གལ་སྲིད (macro[0] == NUL)
	    bha_nga;
    }
    ལོག (macro[0] != NUL);
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
    གལ་སྲིད (*s == para || *s == '\f' || (both && *s == '}'))
	ལོག TRUE;
    གལ་སྲིད (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ལོག TRUE;
    ལོག FALSE;
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
    གལ་སྲིད (c == ' ' || c == '\t' || c == NUL)
	ལོག 0;
    གལ་སྲིད (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	གལ་སྲིད (enc_dbcs == DBCS_KOR && cls_bigword)
	    ལོག 1;

	// process code leading/trailing bytes
	ལོག dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    གལ་སྲིད (enc_utf8)
    {
	c = utf_class(c);
	གལ་སྲིད (c != 0 && cls_bigword)
	    ལོག 1;
	ལོག c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    གལ་སྲིད (cls_bigword)
	ལོག 1;

    གལ་སྲིད (vim_iswordc(c))
	ལོག 2;
    ལོག 1;
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
	གལ་སྲིད (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	གལ་སྲིད (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ལོག FAIL;
	གལ་སྲིད (i >= 1 && eol && count == 0)      // started at last char in line
	    ལོག OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	གལ་སྲིད (sclass != 0)
	    སྐབས (cls() == sclass)
	    {
		i = inc_cursor();
		གལ་སྲིད (i == -1 || (i >= 1 && eol && count == 0))
		    ལོག OK;
	    }

	/*
	 * go to next non-white
	 */
	སྐབས (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    གལ་སྲིད (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    གལ་སྲིད (i == -1 || (i >= 1 && eol && count == 0))
		ལོག OK;
	}
    }
    ལོག OK;
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
	གལ་སྲིད (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	གལ་སྲིད (dec_cursor() == -1)		// started at start of file
	    ལོག FAIL;

	གལ་སྲིད (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    སྐབས (cls() == 0)
	    {
		གལ་སྲིད (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		གལ་སྲིད (dec_cursor() == -1) // hit start of file, stop here
		    ལོག OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    གལ་སྲིད (skip_chars(cls(), BACKWARD))
		ལོག OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ལོག OK;
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
    གལ་སྲིད (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    སྐབས (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	གལ་སྲིད (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	གལ་སྲིད (inc_cursor() == -1)
	    ལོག FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	གལ་སྲིད (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    གལ་སྲིད (skip_chars(sclass, FORWARD))
		ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    སྐབས (cls() == 0)
	    {
		གལ་སྲིད (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		གལ་སྲིད (inc_cursor() == -1)	    // hit end of file, stop here
		    ལོག FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    གལ་སྲིད (skip_chars(cls(), FORWARD))
		ལོག FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ལོག OK;
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
	གལ་སྲིད ((i = dec_cursor()) == -1)
	    ལོག FAIL;
	གལ་སྲིད (eol && i == 1)
	    ལོག OK;

	/*
	 * Move backward to before the start of this word.
	 */
	གལ་སྲིད (sclass != 0)
	{
	    སྐབས (cls() == sclass)
		གལ་སྲིད ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ལོག OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	སྐབས (cls() == 0)
	{
	    གལ་སྲིད (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    གལ་སྲིད ((i = dec_cursor()) == -1 || (eol && i == 1))
		ལོག OK;
	}
    }
    adjust_skipcol();
    ལོག OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ཧྲིལ
skip_chars(ཧྲིལ cclass, ཧྲིལ dir)
{
    སྐབས (cls() == cclass)
	གལ་སྲིད ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ལོག TRUE;
    ལོག FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ཧྲིལ		sclass;		    // starting class

    sclass = cls();
    དོན་ལུ (;;)
    {
	གལ་སྲིད (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	གལ་སྲིད (cls() != sclass)		    // stop at start of word
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
	གལ་སྲིད (!VIM_ISWHITE(c))
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
	གལ་སྲིད (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	གལ་སྲིད (count == 0 || at_start_sent)
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
    གལ་སྲིད (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    གལ་སྲིད (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	གལ་སྲིད ((cls() == 0) == include)
	{
	    གལ་སྲིད (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ལོག FAIL;
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
	    གལ་སྲིད (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    དེ་མིན
		oneleft();

	    གལ་སྲིད (include)
		include_white = TRUE;
	}

	གལ་སྲིད (VIsual_active)
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
	གལ་སྲིད (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    གལ་སྲིད (decl(&curwin->w_cursor) == -1)
		ལོག FAIL;
	    གལ་སྲིད (include != (cls() != 0))
	    {
		གལ་སྲིད (bck_word(1L, bigword, TRUE) == FAIL)
		    ལོག FAIL;
	    }
	    དེ་མིན
	    {
		གལ་སྲིད (bckend_word(1L, bigword, TRUE) == FAIL)
		    ལོག FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	དེ་མིན
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    གལ་སྲིད (incl(&curwin->w_cursor) == -1)
		ལོག FAIL;
	    གལ་སྲིད (include != (cls() == 0))
	    {
		གལ་སྲིད (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ལོག FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		གལ་སྲིད (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    དེ་མིན
	    {
		གལ་སྲིད (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ལོག FAIL;
	    }
	}
	--count;
    }

    གལ་སྲིད (include_white && (cls() != 0
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
	གལ་སྲིད (oneleft() == OK)
	{
	    back_in_line();
	    གལ་སྲིད (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		གལ་སྲིད (VIsual_active)
		    VIsual = curwin->w_cursor;
		དེ་མིན
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    གལ་སྲིད (VIsual_active)
    {
	གལ་སྲིད (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	གལ་སྲིད (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    དེ་མིན
	oap->inclusive = inclusive;

    ལོག OK;
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
    གལ་སྲིད (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	གལ་སྲིད (LT_POS(start_pos, VIsual))
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
		གལ་སྲིད (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    གལ་སྲིད (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		གལ་སྲིད (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		དེ་མིན
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    གལ་སྲིད (include)	// "as" gets twice as much as "is"
		count *= 2;
	    སྐབས (count--)
	    {
		གལ་སྲིད (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		གལ་སྲིད (!at_start_sent || (!include && !VIM_ISWHITE(c)))
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
	    གལ་སྲིད (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		སྐབས (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    གལ་སྲིད (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		གལ་སྲིད (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		དེ་མིན		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    གལ་སྲིད (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    གལ་སྲིད (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ལོག OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    སྐབས (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    གལ་སྲིད (EQUAL_POS(pos, curwin->w_cursor))
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
    གལ་སྲིད (include)
	ncount = count * 2;
    དེ་མིན
    {
	ncount = count;
	གལ་སྲིད (start_blank)
	    --ncount;
    }
    གལ་སྲིད (ncount > 0)
	findsent_forward(ncount, TRUE);
    དེ་མིན
	decl(&curwin->w_cursor);

    གལ་སྲིད (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	གལ་སྲིད (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    གལ་སྲིད (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	དེ་མིན གལ་སྲིད (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    གལ་སྲིད (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	གལ་སྲིད (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	གལ་སྲིད (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    དེ་མིན
    {
	// include a newline after the sentence, if there is one
	གལ་སྲིད (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	དེ་མིན
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ལོག OK;
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
    གལ་སྲིད (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	གལ་སྲིད (what == '{')		// ignore indent
	    སྐབས (inindent(1))
		གལ་སྲིད (inc_cursor() != 0)
		    bha_nga;
	གལ་སྲིད (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    དེ་མིན གལ་སྲིད (LT_POS(VIsual, curwin->w_cursor))
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
    གལ་སྲིད ((pos = findmatch(NULL, what)) != NULL)
    {
	སྐབས (count-- > 0)
	{
	    གལ་སྲིད ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    དེ་མིན
    {
	སྐབས (count-- > 0)
	{
	    གལ་སྲིད ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
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
    གལ་སྲིད (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ལོག FAIL;
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
	    གལ་སྲིད (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	གལ་སྲིད (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ལོག FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	གལ་སྲིད (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    གལ་སྲིད ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ལོག FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    གལ་སྲིད ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ལོག FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	དེ་མིན
	    bha_nga;
    }

    གལ་སྲིད (VIsual_active)
    {
	གལ་སྲིད (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	གལ་སྲིད (sol && gchar_cursor() != NUL)
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
	གལ་སྲིད (sol)
	    incl(&curwin->w_cursor);
	དེ་མིན གལ་སྲིད (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	དེ་མིན
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ལོག OK;
}

#གལ་སྲིད defined(FEAT_EVAL)
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

    གལ་སྲིད (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	དོན་ལུ (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    གལ་སྲིད (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	གལ་སྲིད (*p != '<')	    // check for '<' under cursor
	{
	    གལ་སྲིད (lc != '<')
		ལོག FALSE;
	    p = lp;
	}
    }
    དེ་མིན
    {
	དོན་ལུ (p = line + curwin->w_cursor.col; p > line; )
	{
	    གལ་སྲིད (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    གལ་སྲིད (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	གལ་སྲིད (*p != '<')
	    ལོག FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    གལ་སྲིད (end_tag)
	// check that there is a '/' after the '<'
	ལོག *p == '/';

    // check that there is no '/' after the '<'
    གལ་སྲིད (*p == '/')
	ལོག FALSE;

    // check that the matching '>' is not preceded by '/'
    དོན་ལུ (;;)
    {
	གལ་སྲིད (inc(&pos) < 0)
	    ལོག FALSE;
	c = *ml_get_pos(&pos);
	གལ་སྲིད (c == '>')
	    bha_nga;
	lc = c;
    }
    ལོག lc != '/';
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
    གལ་སྲིད (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    གལ་སྲིད (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	སྐབས (inindent(1))
	    གལ་སྲིད (inc_cursor() != 0)
		bha_nga;

	གལ་སྲིད (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    སྐབས (*ml_get_cursor() != '>')
		གལ་སྲིད (inc_cursor() < 0)
		    bha_nga;
	}
	དེ་མིན གལ་སྲིད (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    སྐབས (*ml_get_cursor() != '<')
		གལ་སྲིད (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    དེ་མིན གལ་སྲིད (LT_POS(VIsual, curwin->w_cursor))
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
    དོན་ལུ (n = 0; n < count; ++n)
    {
	གལ་སྲིད (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    དོན་ལུ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ཧྲིལ)(cp - p);
    གལ་སྲིད (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    གལ་སྲིད (spat == NULL || epat == NULL)
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

    གལ་སྲིད (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    གལ་སྲིད (do_include)
    {
	// Include up to the '>'.
	སྐབས (*ml_get_cursor() != '>')
	    གལ་སྲིད (inc_cursor() < 0)
		bha_nga;
    }
    དེ་མིན
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	གལ་སྲིད (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	དེ་མིན གལ་སྲིད (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    གལ་སྲིད (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ཧྲིལ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	སྐབས (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    གལ་སྲིད (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    དེ་མིན གལ་སྲིད (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	གལ་སྲིད (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    གལ་སྲིད (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	གལ་སྲིད (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	དེ་མིན གལ་སྲིད (*p_sel == 'e')
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
	གལ་སྲིད (LT_POS(end_pos, start_pos))
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
    ལོག retval;
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

    གལ་སྲིད (type == 'S')	    // not implemented yet
	ལོག FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    གལ་སྲིད (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	གལ་སྲིད (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	དེ་མིན
	    dir = FORWARD;
	དོན་ལུ (i = count; --i >= 0; )
	{
	    གལ་སྲིད (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    དོན་ལུ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		གལ་སྲིད (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		དོན་ལུ (;;)
		{
		    གལ་སྲིད (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    གལ་སྲིད (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		གལ་སྲིད (!include)
		    bha_nga;
		གལ་སྲིད (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ལོག retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    སྐབས (start_lnum > 1)
    {
	གལ་སྲིད (white_in_front)	    // stop at first white line
	{
	    གལ་སྲིད (!linewhite(start_lnum - 1))
		bha_nga;
	}
	དེ་མིན		// stop at first non-white line of start of paragraph
	{
	    གལ་སྲིད (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
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
    གལ་སྲིད (!include && white_in_front)
	--i;
    སྐབས (i--)
    {
	གལ་སྲིད (end_lnum == curbuf->b_ml.ml_line_count)
	    ལོག FAIL;

	གལ་སྲིད (!include)
	    do_white = linewhite(end_lnum + 1);

	གལ་སྲིད (include || !do_white)
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

	གལ་སྲིད (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	གལ་སྲིད (include || do_white)
	    སྐབས (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    གལ་སྲིད (!white_in_front && !linewhite(end_lnum) && include)
	སྐབས (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    གལ་སྲིད (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	གལ་སྲིད (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	གལ་སྲིད (VIsual.lnum != start_lnum)
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

    ལོག OK;
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

    དོན་ལུ (;;)
    {
	c = line[col];
	གལ་སྲིད (c == NUL)
	    ལོག -1;
	དེ་མིན གལ་སྲིད (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    གལ་སྲིད (line[col] == NUL)
		ལོག -1;
	}
	དེ་མིན གལ་སྲིད (c == quotechar)
	    bha_nga;
	གལ་སྲིད (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	དེ་མིན
	    ++col;
    }
    ལོག col;
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
	གལ་སྲིད (escape != NULL)
	    སྐབས (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	གལ་སྲིད (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	དེ་མིན གལ་སྲིད (line[col_start] == quotechar)
	    bha_nga;
    }
    ལོག col_start;
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
    གལ་སྲིད (VIsual_active)
    {
	// this only works within one line
	གལ་སྲིད (VIsual.lnum != curwin->w_cursor.lnum)
	    ལོག FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	གལ་སྲིད (*p_sel == 'e')
	{
	    གལ་སྲིད (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    དེ་མིན གལ་སྲིད (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    གལ་སྲིད (!vis_bef_curs && !vis_empty)
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

    གལ་སྲིད (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	གལ་སྲིད (vis_bef_curs)
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
	    གལ་སྲིད (line[i] == NUL)
		bha_nga;
	    གལ་སྲིད (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    གལ་སྲིད (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	གལ་སྲིད (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    གལ་སྲིད (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    གལ་སྲིད (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	དེ་མིན
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    གལ་སྲིད (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    གལ་སྲིད (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    དེ་མིན

    གལ་སྲིད (line[col_start] == quotechar || !vis_empty)
    {
	ཧྲིལ	first_col = col_start;

	གལ་སྲིད (!vis_empty)
	{
	    གལ་སྲིད (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    དེ་མིན
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	དོན་ལུ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    གལ་སྲིད (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    གལ་སྲིད (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    གལ་སྲིད (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    དེ་མིན
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	གལ་སྲིད (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    གལ་སྲིད (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	གལ་སྲིད (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    གལ་སྲིད (include)
    {
	གལ་སྲིད (VIM_ISWHITE(line[col_end + 1]))
	    སྐབས (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	དེ་མིན
	    སྐབས (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    གལ་སྲིད (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    གལ་སྲིད (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	གལ་སྲིད (vis_empty
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
    གལ་སྲིད ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    གལ་སྲིད (VIsual_active)
    {
	གལ་སྲིད (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    གལ་སྲིད (*p_sel != 'e')
		dec_cursor();
	}
	དེ་མིན
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    གལ་སྲིད (inside_quotes
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
	གལ་སྲིད (VIsual_mode == 'V')
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

    ལོག OK;

abort_search:
    གལ་སྲིད (VIsual_active && *p_sel == 'e')
    {
	གལ་སྲིད (did_exclusive_adj)
	    inc_cursor();
	གལ་སྲིད (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ལོག FALSE;
}
