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
#অন্তর্ভাব "vim.h"

স্থির পূর্ণসংখ্যা cls(শূন্য);
স্থির পূর্ণসংখ্যা skip_chars(পূর্ণসংখ্যা, পূর্ণসংখ্যা);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    পূর্ণসংখ্যা
findsent(পূর্ণসংখ্যা dir, long গুনতি)
{
    pos_T	pos, tpos;
    pos_T	পূর্ব_pos;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		(*func)(pos_T *);
    পূর্ণসংখ্যা		startlnum;
    পূর্ণসংখ্যা		noskip = FALSE;	    // do not skip blanks
    পূর্ণসংখ্যা		cpo_J;
    পূর্ণসংখ্যা		found_dot;

    pos = curwin->w_cursor;
    যদি (dir == FORWARD)
	func = incl;
    অন্যথা
	func = decl;

    যখন (গুনতি--)
    {
	পূর্ব_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	যদি (gchar_pos(&pos) == NUL)
	{
	    করো
		যদি ((*func)(&pos) == -1)
		    বিরতি;
	    যখন (gchar_pos(&pos) == NUL);
	    যদি (dir == FORWARD)
		যাও found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	অন্যথা যদি (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    যদি (pos.lnum == curbuf->b_ml.ml_সারি_গুনতি)
		ফেরত FAIL;
	    ++pos.lnum;
	    যাও found;
	}
	অন্যথা যদি (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	যখন (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ফাঁকা)
	{
	    tpos = pos;
	    যদি (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		বিরতি;

	    যদি (found_dot)
		বিরতি;
	    যদি (vim_strchr((char_u *) ".!?", c) != ফাঁকা)
		found_dot = TRUE;

	    যদি (vim_strchr((char_u *) ")]\"'", c) != ফাঁকা
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ফাঁকা)
		বিরতি;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ফাঁকা;

	জন্য (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    যদি (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		যদি (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		বিরতি;
	    }
	    যদি (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		করো
		    যদি ((c = inc(&tpos)) == -1)
			বিরতি;
		যখন (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ফাঁকা);
		যদি (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    যদি (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    বিরতি;
		}
	    }
	    যদি ((*func)(&pos) == -1)
	    {
		যদি (গুনতি)
		    ফেরত FAIL;
		noskip = TRUE;
		বিরতি;
	    }
	}
found:
	    // skip white space
	যখন (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    যদি (incl(&pos) == -1)
		বিরতি;

	যদি (EQUAL_POS(পূর্ব_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    যদি ((*func)(&pos) == -1)
	    {
		যদি (গুনতি)
		    ফেরত FAIL;
		বিরতি;
	    }
	    ++গুনতি;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ফেরত OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    পূর্ণসংখ্যা
findpar(
    পূর্ণসংখ্যা		*pincl,	    // Return: TRUE if last char is to be included
    পূর্ণসংখ্যা		dir,
    long	গুনতি,
    পূর্ণসংখ্যা		what,
    পূর্ণসংখ্যা		both)
{
    linenr_T	curr;
    পূর্ণসংখ্যা		did_skip;   // TRUE after separating lines have been skipped
    পূর্ণসংখ্যা		first;	    // TRUE on first line
    পূর্ণসংখ্যা		posix = (vim_strchr(p_cpo, CPO_PARA) != ফাঁকা);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    পূর্ণসংখ্যা		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    যখন (গুনতি--)
    {
	did_skip = FALSE;
	জন্য (first = TRUE; ; first = FALSE)
	{
	    যদি (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    যদি (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    যদি (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		বিরতি;

#ifdef FEAT_FOLDING
	    যদি (fold_skipped)
		curr -= dir;
#endif
	    যদি ((curr += dir) < 1 || curr > curbuf->b_ml.ml_সারি_গুনতি)
	    {
		যদি (গুনতি)
		    ফেরত FALSE;
		curr -= dir;
		বিরতি;
	    }
	}
    }
    setpcmark();
    যদি (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    যদি (curr == curbuf->b_ml.ml_সারি_গুনতি && what != '}' && dir == FORWARD)
    {
	char_u *সারি = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	যদি ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_শীর্ষ_off)(সারি, সারি + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    অন্যথা
	curwin->w_cursor.col = 0;
    ফেরত TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    স্থির পূর্ণসংখ্যা
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    জন্য (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	যদি (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    বিরতি;
	++macro;
	যদি (macro[0] == NUL)
	    বিরতি;
    }
    ফেরত (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    পূর্ণসংখ্যা
startPS(linenr_T lnum, পূর্ণসংখ্যা para, পূর্ণসংখ্যা both)
{
    char_u	*s;

    s = ml_get(lnum);
    যদি (*s == para || *s == '\f' || (both && *s == '}'))
	ফেরত TRUE;
    যদি (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ফেরত TRUE;
    ফেরত FALSE;
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

স্থির পূর্ণসংখ্যা	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    স্থির পূর্ণসংখ্যা
cls(শূন্য)
{
    পূর্ণসংখ্যা	    c;

    c = gchar_cursor();
    যদি (c == ' ' || c == '\t' || c == NUL)
	ফেরত 0;
    যদি (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	যদি (enc_dbcs == DBCS_KOR && cls_bigword)
	    ফেরত 1;

	// process code leading/trailing bytes
	ফেরত dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    যদি (enc_utf8)
    {
	c = utf_class(c);
	যদি (c != 0 && cls_bigword)
	    ফেরত 1;
	ফেরত c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    যদি (cls_bigword)
	ফেরত 1;

    যদি (vim_iswordc(c))
	ফেরত 2;
    ফেরত 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    পূর্ণসংখ্যা
fwd_word(
    long	গুনতি,
    পূর্ণসংখ্যা		bigword,    // "W", "E" or "B"
    পূর্ণসংখ্যা		eol)
{
    পূর্ণসংখ্যা		sclass;	    // starting class
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		last_সারি;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যখন (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	যদি (hasFolding(curwin->w_cursor.lnum, ফাঁকা, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_সারি = (curwin->w_cursor.lnum == curbuf->b_ml.ml_সারি_গুনতি);
	i = inc_cursor();
	যদি (i == -1 || (i >= 1 && last_সারি)) // started at last char in file
	    ফেরত FAIL;
	যদি (i >= 1 && eol && গুনতি == 0)      // started at last char in line
	    ফেরত OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	যদি (sclass != 0)
	    যখন (cls() == sclass)
	    {
		i = inc_cursor();
		যদি (i == -1 || (i >= 1 && eol && গুনতি == 0))
		    ফেরত OK;
	    }

	/*
	 * go to next non-white
	 */
	যখন (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    যদি (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		বিরতি;

	    i = inc_cursor();
	    যদি (i == -1 || (i >= 1 && eol && গুনতি == 0))
		ফেরত OK;
	}
    }
    ফেরত OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    পূর্ণসংখ্যা
bck_word(long গুনতি, পূর্ণসংখ্যা bigword, পূর্ণসংখ্যা stop)
{
    পূর্ণসংখ্যা		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যখন (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	যদি (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ফাঁকা))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	যদি (dec_cursor() == -1)		// started at start of file
	    ফেরত FAIL;

	যদি (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    যখন (cls() == 0)
	    {
		যদি (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    যাও finished;
		যদি (dec_cursor() == -1) // hit start of file, stop here
		    ফেরত OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    যদি (skip_chars(cls(), BACKWARD))
		ফেরত OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ফেরত OK;
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
    পূর্ণসংখ্যা
end_word(
    long	গুনতি,
    পূর্ণসংখ্যা		bigword,
    পূর্ণসংখ্যা		stop,
    পূর্ণসংখ্যা		empty)
{
    পূর্ণসংখ্যা		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    যদি (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    যখন (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	যদি (hasFolding(curwin->w_cursor.lnum, ফাঁকা, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	যদি (inc_cursor() == -1)
	    ফেরত FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	যদি (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    যদি (skip_chars(sclass, FORWARD))
		ফেরত FAIL;
	}
	অন্যথা যদি (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    যখন (cls() == 0)
	    {
		যদি (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    যাও finished;
		যদি (inc_cursor() == -1)	    // hit end of file, stop here
		    ফেরত FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    যদি (skip_chars(cls(), FORWARD))
		ফেরত FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ফেরত OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    পূর্ণসংখ্যা
bckend_word(
    long	গুনতি,
    পূর্ণসংখ্যা		bigword,    // TRUE for "B"
    পূর্ণসংখ্যা		eol)	    // TRUE: stop at end of line.
{
    পূর্ণসংখ্যা		sclass;	    // starting class
    পূর্ণসংখ্যা		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যখন (--গুনতি >= 0)
    {
	sclass = cls();
	যদি ((i = dec_cursor()) == -1)
	    ফেরত FAIL;
	যদি (eol && i == 1)
	    ফেরত OK;

	/*
	 * Move backward to before the start of this word.
	 */
	যদি (sclass != 0)
	{
	    যখন (cls() == sclass)
		যদি ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ফেরত OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	যখন (cls() == 0)
	{
	    যদি (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		বিরতি;
	    যদি ((i = dec_cursor()) == -1 || (eol && i == 1))
		ফেরত OK;
	}
    }
    adjust_skipcol();
    ফেরত OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    স্থির পূর্ণসংখ্যা
skip_chars(পূর্ণসংখ্যা cclass, পূর্ণসংখ্যা dir)
{
    যখন (cls() == cclass)
	যদি ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    স্থির শূন্য
back_in_সারি(শূন্য)
{
    পূর্ণসংখ্যা		sclass;		    // starting class

    sclass = cls();
    জন্য (;;)
    {
	যদি (curwin->w_cursor.col == 0)	    // stop at start of line
	    বিরতি;
	dec_cursor();
	যদি (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    বিরতি;
	}
    }
}

    স্থির শূন্য
find_first_blank(pos_T *posp)
{
    পূর্ণসংখ্যা	    c;

    যখন (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	যদি (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    বিরতি;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    স্থির শূন্য
findsent_forward(
    long    গুনতি,
    পূর্ণসংখ্যা	    at_start_sent)	// cursor is at start of sentence
{
    যখন (গুনতি--)
    {
	findsent(FORWARD, 1L);
	যদি (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	যদি (গুনতি == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    পূর্ণসংখ্যা
current_word(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণসংখ্যা		include,	// TRUE: include word and white space
    পূর্ণসংখ্যা		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    পূর্ণসংখ্যা		inclusive = TRUE;
    পূর্ণসংখ্যা		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    যদি (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    যদি (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_সারি();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	যদি ((cls() == 0) == include)
	{
	    যদি (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    যদি (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    অন্যথা
		oneleft();

	    যদি (include)
		include_white = TRUE;
	}

	যদি (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	অন্যথা
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--গুনতি;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    যখন (গুনতি > 0)
    {
	inclusive = TRUE;
	যদি (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    যদি (decl(&curwin->w_cursor) == -1)
		ফেরত FAIL;
	    যদি (include != (cls() != 0))
	    {
		যদি (bck_word(1L, bigword, TRUE) == FAIL)
		    ফেরত FAIL;
	    }
	    অন্যথা
	    {
		যদি (bckend_word(1L, bigword, TRUE) == FAIL)
		    ফেরত FAIL;
		(শূন্য)incl(&curwin->w_cursor);
	    }
	}
	অন্যথা
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    যদি (incl(&curwin->w_cursor) == -1)
		ফেরত FAIL;
	    যদি (include != (cls() == 0))
	    {
		যদি (fwd_word(1L, bigword, TRUE) == FAIL && গুনতি > 1)
		    ফেরত FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		যদি (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    অন্যথা
	    {
		যদি (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ফেরত FAIL;
	    }
	}
	--গুনতি;
    }

    যদি (include_white && (cls() != 0
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
	যদি (oneleft() == OK)
	{
	    back_in_সারি();
	    যদি (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		যদি (VIsual_active)
		    VIsual = curwin->w_cursor;
		অন্যথা
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    যদি (VIsual_active)
    {
	যদি (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	যদি (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    অন্যথা
	oap->inclusive = inclusive;

    ফেরত OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    পূর্ণসংখ্যা
current_sent(oparg_T *oap, long গুনতি, পূর্ণসংখ্যা include)
{
    pos_T	start_pos;
    pos_T	pos;
    পূর্ণসংখ্যা		start_blank;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    যদি (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	যদি (LT_POS(start_pos, VIsual))
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
	    যখন (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		যদি (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    বিরতি;
		}
		incl(&pos);
	    }
	    যদি (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		যদি (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		অন্যথা
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    যদি (include)	// "as" gets twice as much as "is"
		গুনতি *= 2;
	    যখন (গুনতি--)
	    {
		যদি (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		যদি (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	অন্যথা
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
	    যদি (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		যখন (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    যদি (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			বিরতি;
		    }
		    incl(&pos);
		}
		যদি (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		অন্যথা		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    যদি (include)	// "as" gets twice as much as "is"
		গুনতি *= 2;
	    findsent_forward(গুনতি, at_start_sent);
	    যদি (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ফেরত OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    যখন (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    যদি (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    অন্যথা
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    যদি (include)
	ncount = গুনতি * 2;
    অন্যথা
    {
	ncount = গুনতি;
	যদি (start_blank)
	    --ncount;
    }
    যদি (ncount > 0)
	findsent_forward(ncount, TRUE);
    অন্যথা
	decl(&curwin->w_cursor);

    যদি (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	যদি (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    যদি (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	অন্যথা যদি (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    যদি (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	যদি (EQUAL_POS(start_pos, curwin->w_cursor))
	    যাও extend;
	যদি (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    অন্যথা
    {
	// include a newline after the sentence, if there is one
	যদি (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	অন্যথা
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ফেরত OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    পূর্ণসংখ্যা
current_block(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণসংখ্যা		include,	// TRUE == include white space
    পূর্ণসংখ্যা		what,		// '(', '{', etc.
    পূর্ণসংখ্যা		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ফাঁকা;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    পূর্ণসংখ্যা		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    যদি (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	যদি (what == '{')		// ignore indent
	    যখন (inindent(1))
		যদি (inc_cursor() != 0)
		    বিরতি;
	যদি (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    অন্যথা যদি (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    অন্যথা
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ফাঁকা ? "%M" : "%");
    যদি ((pos = findmatch(ফাঁকা, what)) != ফাঁকা)
    {
	যখন (গুনতি-- > 0)
	{
	    যদি ((pos = findmatch(ফাঁকা, what)) == ফাঁকা)
		বিরতি;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    অন্যথা
    {
	যখন (গুনতি-- > 0)
	{
	    যদি ((pos = findmatchlimit(ফাঁকা, what, FM_FORWARD, 0)) == ফাঁকা)
		বিরতি;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    যদি (pos == ফাঁকা || (end_pos = findmatch(ফাঁকা, other)) == ফাঁকা)
    {
	curwin->w_cursor = old_pos;
	ফেরত FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    যখন (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	যখন (inindent(1))
	{
	    sol = TRUE;
	    যদি (decl(&curwin->w_cursor) != 0)
		বিরতি;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	যদি (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ফেরত FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	যদি (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    যদি ((pos = findmatch(ফাঁকা, what)) == ফাঁকা)
	    {
		curwin->w_cursor = old_pos;
		ফেরত FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    যদি ((end_pos = findmatch(ফাঁকা, other)) == ফাঁকা)
	    {
		curwin->w_cursor = old_pos;
		ফেরত FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	অন্যথা
	    বিরতি;
    }

    যদি (VIsual_active)
    {
	যদি (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	যদি (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    অন্যথা
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	যদি (sol)
	    incl(&curwin->w_cursor);
	অন্যথা যদি (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	অন্যথা
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ফেরত OK;
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    স্থির পূর্ণসংখ্যা
in_html_tag(
    পূর্ণসংখ্যা		end_tag)
{
    char_u	*সারি = ml_get_curline();
    char_u	*p;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		lc = NUL;
    pos_T	pos;

    যদি (enc_dbcs)
    {
	char_u	*lp = ফাঁকা;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	জন্য (p = সারি; p < সারি + curwin->w_cursor.col; MB_PTR_ADV(p))
	    যদি (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	যদি (*p != '<')	    // check for '<' under cursor
	{
	    যদি (lc != '<')
		ফেরত FALSE;
	    p = lp;
	}
    }
    অন্যথা
    {
	জন্য (p = সারি + curwin->w_cursor.col; p > সারি; )
	{
	    যদি (*p == '<')	// find '<' under/before cursor
		বিরতি;
	    MB_PTR_BACK(সারি, p);
	    যদি (*p == '>')	// find '>' before cursor
		বিরতি;
	}
	যদি (*p != '<')
	    ফেরত FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - সারি);

    MB_PTR_ADV(p);
    যদি (end_tag)
	// check that there is a '/' after the '<'
	ফেরত *p == '/';

    // check that there is no '/' after the '<'
    যদি (*p == '/')
	ফেরত FALSE;

    // check that the matching '>' is not preceded by '/'
    জন্য (;;)
    {
	যদি (inc(&pos) < 0)
	    ফেরত FALSE;
	c = *ml_get_pos(&pos);
	যদি (c == '>')
	    বিরতি;
	lc = c;
    }
    ফেরত lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    পূর্ণসংখ্যা
current_tagblock(
    oparg_T	*oap,
    long	গুনতি_arg,
    পূর্ণসংখ্যা		include)	// TRUE == include white space
{
    long	গুনতি = গুনতি_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		r;
    পূর্ণসংখ্যা		do_include = include;
    পূর্ণসংখ্যা		save_p_ws = p_ws;
    পূর্ণসংখ্যা		retval = FAIL;
    পূর্ণসংখ্যা		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    যদি (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    যদি (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	যখন (inindent(1))
	    যদি (inc_cursor() != 0)
		বিরতি;

	যদি (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    যখন (*ml_get_cursor() != '>')
		যদি (inc_cursor() < 0)
		    বিরতি;
	}
	অন্যথা যদি (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    যখন (*ml_get_cursor() != '<')
		যদি (dec_cursor() < 0)
		    বিরতি;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    অন্যথা যদি (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    অন্যথা
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    জন্য (n = 0; n < গুনতি; ++n)
    {
	যদি (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ফাঁকা, 0,
						  ফাঁকা, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    যাও theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    জন্য (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (পূর্ণসংখ্যা)(cp - p);
    যদি (len == 0)
    {
	curwin->w_cursor = old_pos;
	যাও theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    যদি (spat == ফাঁকা || epat == ফাঁকা)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	যাও theend;
    }
    sprintf((অক্ষর *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((অক্ষর *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ফাঁকা,
						    0, ফাঁকা, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    যদি (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	গুনতি = 1;
	curwin->w_cursor = start_pos;
	যাও again;
    }

    যদি (do_include)
    {
	// Include up to the '>'.
	যখন (*ml_get_cursor() != '>')
	    যদি (inc_cursor() < 0)
		বিরতি;
    }
    অন্যথা
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	যদি (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	অন্যথা যদি (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    যদি (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	পূর্ণসংখ্যা in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	যখন (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    যদি (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		বিরতি;
	    }
	    অন্যথা যদি (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	যদি (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    গুনতি = গুনতি_arg;
	    যাও again;
	}
    }

    যদি (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	যদি (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	অন্যথা যদি (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    অন্যথা
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	যদি (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	অন্যথা
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ফেরত retval;
}
#endif

    পূর্ণসংখ্যা
current_par(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণসংখ্যা		include,	// TRUE == include white space
    পূর্ণসংখ্যা		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    পূর্ণসংখ্যা		white_in_front;
    পূর্ণসংখ্যা		dir;
    পূর্ণসংখ্যা		start_is_white;
    পূর্ণসংখ্যা		পূর্ব_start_is_white;
    পূর্ণসংখ্যা		retval = OK;
    পূর্ণসংখ্যা		do_white = FALSE;
    পূর্ণসংখ্যা		t;
    পূর্ণসংখ্যা		i;

    যদি (type == 'S')	    // not implemented yet
	ফেরত FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    যদি (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	যদি (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	অন্যথা
	    dir = FORWARD;
	জন্য (i = গুনতি; --i >= 0; )
	{
	    যদি (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_সারি_গুনতি))
	    {
		retval = FAIL;
		বিরতি;
	    }

	    পূর্ব_start_is_white = -1;
	    জন্য (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		যদি (পূর্ব_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    বিরতি;
		}
		জন্য (;;)
		{
		    যদি (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_সারি_গুনতি))
			বিরতি;
		    যদি (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			বিরতি;
		    start_lnum += dir;
		}
		যদি (!include)
		    বিরতি;
		যদি (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_সারি_গুনতি))
		    বিরতি;
		পূর্ব_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ফেরত retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    যখন (start_lnum > 1)
    {
	যদি (white_in_front)	    // stop at first white line
	{
	    যদি (!linewhite(start_lnum - 1))
		বিরতি;
	}
	অন্যথা		// stop at first non-white line of start of paragraph
	{
	    যদি (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		বিরতি;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    যখন (end_lnum <= curbuf->b_ml.ml_সারি_গুনতি && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = গুনতি;
    যদি (!include && white_in_front)
	--i;
    যখন (i--)
    {
	যদি (end_lnum == curbuf->b_ml.ml_সারি_গুনতি)
	    ফেরত FAIL;

	যদি (!include)
	    do_white = linewhite(end_lnum + 1);

	যদি (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    যখন (end_lnum < curbuf->b_ml.ml_সারি_গুনতি
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	যদি (i == 0 && white_in_front && include)
	    বিরতি;

	/*
	 * skip to end of white lines after paragraph
	 */
	যদি (include || do_white)
	    যখন (end_lnum < curbuf->b_ml.ml_সারি_গুনতি
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    যদি (!white_in_front && !linewhite(end_lnum) && include)
	যখন (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    যদি (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	যদি (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    যাও extend;
	যদি (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    অন্যথা
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ফেরত OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    স্থির পূর্ণসংখ্যা
find_পরবর্তী_quote(
    char_u	*সারি,
    পূর্ণসংখ্যা		col,
    পূর্ণসংখ্যা		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    পূর্ণসংখ্যা		c;

    জন্য (;;)
    {
	c = সারি[col];
	যদি (c == NUL)
	    ফেরত -1;
	অন্যথা যদি (escape != ফাঁকা && vim_strchr(escape, c))
	{
	    ++col;
	    যদি (সারি[col] == NUL)
		ফেরত -1;
	}
	অন্যথা যদি (c == quotechar)
	    বিরতি;
	যদি (has_mbyte)
	    col += (*mb_ptr2len)(সারি + col);
	অন্যথা
	    ++col;
    }
    ফেরত col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    স্থির পূর্ণসংখ্যা
find_পূর্ব_quote(
    char_u	*সারি,
    পূর্ণসংখ্যা		col_start,
    পূর্ণসংখ্যা		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    পূর্ণসংখ্যা		n;

    যখন (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_শীর্ষ_off)(সারি, সারি + col_start);
	n = 0;
	যদি (escape != ফাঁকা)
	    যখন (col_start - n > 0 && vim_strchr(escape,
					     সারি[col_start - n - 1]) != ফাঁকা)
	    ++n;
	যদি (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	অন্যথা যদি (সারি[col_start] == quotechar)
	    বিরতি;
    }
    ফেরত col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    পূর্ণসংখ্যা
current_quote(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণসংখ্যা		include,	// TRUE == include quote char
    পূর্ণসংখ্যা		quotechar)	// Quote character
{
    char_u	*সারি = ml_get_curline();
    পূর্ণসংখ্যা		col_end;
    পূর্ণসংখ্যা		col_start = curwin->w_cursor.col;
    পূর্ণসংখ্যা		inclusive = FALSE;
    পূর্ণসংখ্যা		vis_empty = TRUE;	// Visual selection <= 1 char
    পূর্ণসংখ্যা		vis_bef_curs = FALSE;	// Visual starts before cursor
    পূর্ণসংখ্যা		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    পূর্ণসংখ্যা		inside_quotes = FALSE;	// Looks like "i'" done before
    পূর্ণসংখ্যা		selected_quote = FALSE;	// Has quote inside selection
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    যদি (VIsual_active)
    {
	// this only works within one line
	যদি (VIsual.lnum != curwin->w_cursor.lnum)
	    ফেরত FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	যদি (*p_sel == 'e')
	{
	    যদি (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    অন্যথা যদি (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    যদি (!vis_bef_curs && !vis_empty)
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

    যদি (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	যদি (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& সারি[VIsual.col - 1] == quotechar
			&& সারি[curwin->w_cursor.col] != NUL
			&& সারি[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	অন্যথা
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& সারি[curwin->w_cursor.col - 1] == quotechar
			&& সারি[VIsual.col] != NUL
			&& সারি[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	যখন (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    যদি (সারি[i] == NUL)
		বিরতি;
	    যদি (সারি[i++] == quotechar)
	    {
		selected_quote = TRUE;
		বিরতি;
	    }
	}
    }

    যদি (!vis_empty && সারি[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	যদি (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_পরবর্তী_quote(সারি, col_start + 1, quotechar, ফাঁকা);
	    যদি (col_start < 0)
		যাও abort_অনুসন্ধান;
	    col_end = find_পরবর্তী_quote(সারি, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    যদি (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	অন্যথা
	{
	    col_end = find_পূর্ব_quote(সারি, col_start, quotechar, ফাঁকা);
	    যদি (সারি[col_end] != quotechar)
		যাও abort_অনুসন্ধান;
	    col_start = find_পূর্ব_quote(সারি, col_end, quotechar,
							      curbuf->b_p_qe);
	    যদি (সারি[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    অন্যথা

    যদি (সারি[col_start] == quotechar || !vis_empty)
    {
	পূর্ণসংখ্যা	first_col = col_start;

	যদি (!vis_empty)
	{
	    যদি (vis_bef_curs)
		first_col = find_পরবর্তী_quote(সারি, col_start, quotechar, ফাঁকা);
	    অন্যথা
		first_col = find_পূর্ব_quote(সারি, col_start, quotechar, ফাঁকা);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	জন্য (;;)
	{
	    // Find open quote character.
	    col_start = find_পরবর্তী_quote(সারি, col_start, quotechar, ফাঁকা);
	    যদি (col_start < 0 || col_start > first_col)
		যাও abort_অনুসন্ধান;
	    // Find close quote character.
	    col_end = find_পরবর্তী_quote(সারি, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    যদি (col_end < 0)
		যাও abort_অনুসন্ধান;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    যদি (col_start <= first_col && first_col <= col_end)
		বিরতি;
	    col_start = col_end + 1;
	}
    }
    অন্যথা
    {
	// Search backward for a starting quote.
	col_start = find_পূর্ব_quote(সারি, col_start, quotechar, curbuf->b_p_qe);
	যদি (সারি[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_পরবর্তী_quote(সারি, col_start, quotechar, ফাঁকা);
	    যদি (col_start < 0)
		যাও abort_অনুসন্ধান;
	}

	// Find close quote character.
	col_end = find_পরবর্তী_quote(সারি, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	যদি (col_end < 0)
	    যাও abort_অনুসন্ধান;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    যদি (include)
    {
	যদি (VIM_ISWHITE(সারি[col_end + 1]))
	    যখন (VIM_ISWHITE(সারি[col_end + 1]))
		++col_end;
	অন্যথা
	    যখন (col_start > 0 && VIM_ISWHITE(সারি[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    যদি (!include && গুনতি < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    যদি (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	যদি (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (সারি[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| সারি[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    অন্যথা
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    যদি ((include || গুনতি > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    যদি (VIsual_active)
    {
	যদি (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    যদি (*p_sel != 'e')
		dec_cursor();
	}
	অন্যথা
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    যদি (inside_quotes
		    || (!selected_quote
			&& সারি[VIsual.col] != quotechar
			&& (সারি[VIsual.col] == NUL
			    || সারি[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	যদি (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    অন্যথা
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ফেরত OK;

abort_অনুসন্ধান:
    যদি (VIsual_active && *p_sel == 'e')
    {
	যদি (did_exclusive_adj)
	    inc_cursor();
	যদি (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ফেরত FALSE;
}
