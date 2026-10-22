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
#ضمن "vim.h"

ساكن صحيح cls(فراغ);
ساكن صحيح skip_chars(صحيح, صحيح);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    صحيح
findsent(صحيح dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    صحيح		c;
    صحيح		(*func)(pos_T *);
    صحيح		startlnum;
    صحيح		noskip = FALSE;	    // do not skip blanks
    صحيح		cpo_J;
    صحيح		found_dot;

    pos = curwin->w_cursor;
    ܐܢ (dir == FORWARD)
	func = incl;
    ܐܚܪܢܐ
	func = decl;

    ܟܡܐ (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ܐܢ (gchar_pos(&pos) == NUL)
	{
	    ܥܒܕ
		ܐܢ ((*func)(&pos) == -1)
		    اكسر;
	    ܟܡܐ (gchar_pos(&pos) == NUL);
	    ܐܢ (dir == FORWARD)
		اذهب found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ܐܚܪܢܐ ܐܢ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ܐܢ (pos.lnum == curbuf->b_ml.ml_line_count)
		ܦܢܐ FAIL;
	    ++pos.lnum;
	    اذهب found;
	}
	ܐܚܪܢܐ ܐܢ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ܟܡܐ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != عدم)
	{
	    tpos = pos;
	    ܐܢ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		اكسر;

	    ܐܢ (found_dot)
		اكسر;
	    ܐܢ (vim_strchr((char_u *) ".!?", c) != عدم)
		found_dot = TRUE;

	    ܐܢ (vim_strchr((char_u *) ")]\"'", c) != عدم
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == عدم)
		اكسر;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != عدم;

	لكل (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ܐܢ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ܐܢ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		اكسر;
	    }
	    ܐܢ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		ܥܒܕ
		    ܐܢ ((c = inc(&tpos)) == -1)
			اكسر;
		ܟܡܐ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= عدم);
		ܐܢ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ܐܢ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    اكسر;
		}
	    }
	    ܐܢ ((*func)(&pos) == -1)
	    {
		ܐܢ (count)
		    ܦܢܐ FAIL;
		noskip = TRUE;
		اكسر;
	    }
	}
found:
	    // skip white space
	ܟܡܐ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ܐܢ (incl(&pos) == -1)
		اكسر;

	ܐܢ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ܐܢ ((*func)(&pos) == -1)
	    {
		ܐܢ (count)
		    ܦܢܐ FAIL;
		اكسر;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ܦܢܐ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    صحيح
findpar(
    صحيح		*pincl,	    // Return: TRUE if last char is to be included
    صحيح		dir,
    long	count,
    صحيح		what,
    صحيح		both)
{
    linenr_T	curr;
    صحيح		did_skip;   // TRUE after separating lines have been skipped
    صحيح		first;	    // TRUE on first line
    صحيح		posix = (vim_strchr(p_cpo, CPO_PARA) != عدم);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    صحيح		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ܟܡܐ (count--)
    {
	did_skip = FALSE;
	لكل (first = TRUE; ; first = FALSE)
	{
	    ܐܢ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ܐܢ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ܐܢ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		اكسر;

#ifdef FEAT_FOLDING
	    ܐܢ (fold_skipped)
		curr -= dir;
#endif
	    ܐܢ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ܐܢ (count)
		    ܦܢܐ FALSE;
		curr -= dir;
		اكسر;
	    }
	}
    }
    setpcmark();
    ܐܢ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ܐܢ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ܐܢ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ܐܚܪܢܐ
	curwin->w_cursor.col = 0;
    ܦܢܐ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ساكن صحيح
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    لكل (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ܐܢ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    اكسر;
	++macro;
	ܐܢ (macro[0] == NUL)
	    اكسر;
    }
    ܦܢܐ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    صحيح
startPS(linenr_T lnum, صحيح para, صحيح both)
{
    char_u	*s;

    s = ml_get(lnum);
    ܐܢ (*s == para || *s == '\f' || (both && *s == '}'))
	ܦܢܐ TRUE;
    ܐܢ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ܦܢܐ TRUE;
    ܦܢܐ FALSE;
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

ساكن صحيح	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ساكن صحيح
cls(فراغ)
{
    صحيح	    c;

    c = gchar_cursor();
    ܐܢ (c == ' ' || c == '\t' || c == NUL)
	ܦܢܐ 0;
    ܐܢ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ܐܢ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ܦܢܐ 1;

	// process code leading/trailing bytes
	ܦܢܐ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ܐܢ (enc_utf8)
    {
	c = utf_class(c);
	ܐܢ (c != 0 && cls_bigword)
	    ܦܢܐ 1;
	ܦܢܐ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ܐܢ (cls_bigword)
	ܦܢܐ 1;

    ܐܢ (vim_iswordc(c))
	ܦܢܐ 2;
    ܦܢܐ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    صحيح
fwd_word(
    long	count,
    صحيح		bigword,    // "W", "E" or "B"
    صحيح		eol)
{
    صحيح		sclass;	    // starting class
    صحيح		i;
    صحيح		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ܟܡܐ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ܐܢ (hasFolding(curwin->w_cursor.lnum, عدم, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ܐܢ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ܦܢܐ FAIL;
	ܐܢ (i >= 1 && eol && count == 0)      // started at last char in line
	    ܦܢܐ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ܐܢ (sclass != 0)
	    ܟܡܐ (cls() == sclass)
	    {
		i = inc_cursor();
		ܐܢ (i == -1 || (i >= 1 && eol && count == 0))
		    ܦܢܐ OK;
	    }

	/*
	 * go to next non-white
	 */
	ܟܡܐ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ܐܢ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		اكسر;

	    i = inc_cursor();
	    ܐܢ (i == -1 || (i >= 1 && eol && count == 0))
		ܦܢܐ OK;
	}
    }
    ܦܢܐ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    صحيح
bck_word(long count, صحيح bigword, صحيح stop)
{
    صحيح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ܟܡܐ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ܐܢ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, عدم))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ܐܢ (dec_cursor() == -1)		// started at start of file
	    ܦܢܐ FAIL;

	ܐܢ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ܟܡܐ (cls() == 0)
	    {
		ܐܢ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    اذهب finished;
		ܐܢ (dec_cursor() == -1) // hit start of file, stop here
		    ܦܢܐ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ܐܢ (skip_chars(cls(), BACKWARD))
		ܦܢܐ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ܦܢܐ OK;
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
    صحيح
end_word(
    long	count,
    صحيح		bigword,
    صحيح		stop,
    صحيح		empty)
{
    صحيح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ܐܢ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ܟܡܐ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ܐܢ (hasFolding(curwin->w_cursor.lnum, عدم, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ܐܢ (inc_cursor() == -1)
	    ܦܢܐ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ܐܢ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ܐܢ (skip_chars(sclass, FORWARD))
		ܦܢܐ FAIL;
	}
	ܐܚܪܢܐ ܐܢ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ܟܡܐ (cls() == 0)
	    {
		ܐܢ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    اذهب finished;
		ܐܢ (inc_cursor() == -1)	    // hit end of file, stop here
		    ܦܢܐ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ܐܢ (skip_chars(cls(), FORWARD))
		ܦܢܐ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ܦܢܐ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    صحيح
bckend_word(
    long	count,
    صحيح		bigword,    // TRUE for "B"
    صحيح		eol)	    // TRUE: stop at end of line.
{
    صحيح		sclass;	    // starting class
    صحيح		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ܟܡܐ (--count >= 0)
    {
	sclass = cls();
	ܐܢ ((i = dec_cursor()) == -1)
	    ܦܢܐ FAIL;
	ܐܢ (eol && i == 1)
	    ܦܢܐ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ܐܢ (sclass != 0)
	{
	    ܟܡܐ (cls() == sclass)
		ܐܢ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ܦܢܐ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ܟܡܐ (cls() == 0)
	{
	    ܐܢ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		اكسر;
	    ܐܢ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ܦܢܐ OK;
	}
    }
    adjust_skipcol();
    ܦܢܐ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ساكن صحيح
skip_chars(صحيح cclass, صحيح dir)
{
    ܟܡܐ (cls() == cclass)
	ܐܢ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ܦܢܐ TRUE;
    ܦܢܐ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ساكن فراغ
back_in_line(فراغ)
{
    صحيح		sclass;		    // starting class

    sclass = cls();
    لكل (;;)
    {
	ܐܢ (curwin->w_cursor.col == 0)	    // stop at start of line
	    اكسر;
	dec_cursor();
	ܐܢ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    اكسر;
	}
    }
}

    ساكن فراغ
find_first_blank(pos_T *posp)
{
    صحيح	    c;

    ܟܡܐ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ܐܢ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    اكسر;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ساكن فراغ
findsent_forward(
    long    count,
    صحيح	    at_start_sent)	// cursor is at start of sentence
{
    ܟܡܐ (count--)
    {
	findsent(FORWARD, 1L);
	ܐܢ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ܐܢ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحيح
current_word(
    oparg_T	*oap,
    long	count,
    صحيح		include,	// TRUE: include word and white space
    صحيح		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    صحيح		inclusive = TRUE;
    صحيح		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ܐܢ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ܐܢ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	ܐܢ ((cls() == 0) == include)
	{
	    ܐܢ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ܦܢܐ FAIL;
	}
	ܐܚܪܢܐ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ܐܢ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ܐܚܪܢܐ
		oneleft();

	    ܐܢ (include)
		include_white = TRUE;
	}

	ܐܢ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ܐܚܪܢܐ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ܟܡܐ (count > 0)
    {
	inclusive = TRUE;
	ܐܢ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ܐܢ (decl(&curwin->w_cursor) == -1)
		ܦܢܐ FAIL;
	    ܐܢ (include != (cls() != 0))
	    {
		ܐܢ (bck_word(1L, bigword, TRUE) == FAIL)
		    ܦܢܐ FAIL;
	    }
	    ܐܚܪܢܐ
	    {
		ܐܢ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ܦܢܐ FAIL;
		(فراغ)incl(&curwin->w_cursor);
	    }
	}
	ܐܚܪܢܐ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ܐܢ (incl(&curwin->w_cursor) == -1)
		ܦܢܐ FAIL;
	    ܐܢ (include != (cls() == 0))
	    {
		ܐܢ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ܦܢܐ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ܐܢ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ܐܚܪܢܐ
	    {
		ܐܢ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ܦܢܐ FAIL;
	    }
	}
	--count;
    }

    ܐܢ (include_white && (cls() != 0
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
	ܐܢ (oneleft() == OK)
	{
	    back_in_line();
	    ܐܢ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ܐܢ (VIsual_active)
		    VIsual = curwin->w_cursor;
		ܐܚܪܢܐ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ܐܢ (VIsual_active)
    {
	ܐܢ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ܐܢ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ܐܚܪܢܐ
	oap->inclusive = inclusive;

    ܦܢܐ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    صحيح
current_sent(oparg_T *oap, long count, صحيح include)
{
    pos_T	start_pos;
    pos_T	pos;
    صحيح		start_blank;
    صحيح		c;
    صحيح		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ܐܢ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ܐܢ (LT_POS(start_pos, VIsual))
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
	    ܟܡܐ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ܐܢ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    اكسر;
		}
		incl(&pos);
	    }
	    ܐܢ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ܐܢ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ܐܚܪܢܐ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ܐܢ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ܟܡܐ (count--)
	    {
		ܐܢ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ܐܢ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ܐܚܪܢܐ
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
	    ܐܢ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ܟܡܐ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ܐܢ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			اكسر;
		    }
		    incl(&pos);
		}
		ܐܢ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ܐܚܪܢܐ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ܐܢ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ܐܢ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ܦܢܐ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ܟܡܐ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ܐܢ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ܐܚܪܢܐ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ܐܢ (include)
	ncount = count * 2;
    ܐܚܪܢܐ
    {
	ncount = count;
	ܐܢ (start_blank)
	    --ncount;
    }
    ܐܢ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ܐܚܪܢܐ
	decl(&curwin->w_cursor);

    ܐܢ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ܐܢ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ܐܢ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ܐܚܪܢܐ ܐܢ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ܐܢ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ܐܢ (EQUAL_POS(start_pos, curwin->w_cursor))
	    اذهب extend;
	ܐܢ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ܐܚܪܢܐ
    {
	// include a newline after the sentence, if there is one
	ܐܢ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ܐܚܪܢܐ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ܦܢܐ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    صحيح
current_block(
    oparg_T	*oap,
    long	count,
    صحيح		include,	// TRUE == include white space
    صحيح		what,		// '(', '{', etc.
    صحيح		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = عدم;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    صحيح		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ܐܢ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ܐܢ (what == '{')		// ignore indent
	    ܟܡܐ (inindent(1))
		ܐܢ (inc_cursor() != 0)
		    اكسر;
	ܐܢ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ܐܚܪܢܐ ܐܢ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ܐܚܪܢܐ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != عدم ? "%M" : "%");
    ܐܢ ((pos = findmatch(عدم, what)) != عدم)
    {
	ܟܡܐ (count-- > 0)
	{
	    ܐܢ ((pos = findmatch(عدم, what)) == عدم)
		اكسر;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ܐܚܪܢܐ
    {
	ܟܡܐ (count-- > 0)
	{
	    ܐܢ ((pos = findmatchlimit(عدم, what, FM_FORWARD, 0)) == عدم)
		اكسر;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ܐܢ (pos == عدم || (end_pos = findmatch(عدم, other)) == عدم)
    {
	curwin->w_cursor = old_pos;
	ܦܢܐ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ܟܡܐ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ܟܡܐ (inindent(1))
	{
	    sol = TRUE;
	    ܐܢ (decl(&curwin->w_cursor) != 0)
		اكسر;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ܐܢ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ܦܢܐ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ܐܢ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ܐܢ ((pos = findmatch(عدم, what)) == عدم)
	    {
		curwin->w_cursor = old_pos;
		ܦܢܐ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ܐܢ ((end_pos = findmatch(عدم, other)) == عدم)
	    {
		curwin->w_cursor = old_pos;
		ܦܢܐ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ܐܚܪܢܐ
	    اكسر;
    }

    ܐܢ (VIsual_active)
    {
	ܐܢ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ܐܢ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ܐܚܪܢܐ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ܐܢ (sol)
	    incl(&curwin->w_cursor);
	ܐܚܪܢܐ ܐܢ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ܐܚܪܢܐ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ܦܢܐ OK;
}

#ܐܢ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ساكن صحيح
in_html_tag(
    صحيح		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    صحيح		c;
    صحيح		lc = NUL;
    pos_T	pos;

    ܐܢ (enc_dbcs)
    {
	char_u	*lp = عدم;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	لكل (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ܐܢ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ܐܢ (*p != '<')	    // check for '<' under cursor
	{
	    ܐܢ (lc != '<')
		ܦܢܐ FALSE;
	    p = lp;
	}
    }
    ܐܚܪܢܐ
    {
	لكل (p = line + curwin->w_cursor.col; p > line; )
	{
	    ܐܢ (*p == '<')	// find '<' under/before cursor
		اكسر;
	    MB_PTR_BACK(line, p);
	    ܐܢ (*p == '>')	// find '>' before cursor
		اكسر;
	}
	ܐܢ (*p != '<')
	    ܦܢܐ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ܐܢ (end_tag)
	// check that there is a '/' after the '<'
	ܦܢܐ *p == '/';

    // check that there is no '/' after the '<'
    ܐܢ (*p == '/')
	ܦܢܐ FALSE;

    // check that the matching '>' is not preceded by '/'
    لكل (;;)
    {
	ܐܢ (inc(&pos) < 0)
	    ܦܢܐ FALSE;
	c = *ml_get_pos(&pos);
	ܐܢ (c == '>')
	    اكسر;
	lc = c;
    }
    ܦܢܐ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    صحيح
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    صحيح		include)	// TRUE == include white space
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
    صحيح		len;
    صحيح		r;
    صحيح		do_include = include;
    صحيح		save_p_ws = p_ws;
    صحيح		retval = FAIL;
    صحيح		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ܐܢ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ܐܢ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ܟܡܐ (inindent(1))
	    ܐܢ (inc_cursor() != 0)
		اكسر;

	ܐܢ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ܟܡܐ (*ml_get_cursor() != '>')
		ܐܢ (inc_cursor() < 0)
		    اكسر;
	}
	ܐܚܪܢܐ ܐܢ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ܟܡܐ (*ml_get_cursor() != '<')
		ܐܢ (dec_cursor() < 0)
		    اكسر;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ܐܚܪܢܐ ܐܢ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ܐܚܪܢܐ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    لكل (n = 0; n < count; ++n)
    {
	ܐܢ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, عدم, 0,
						  عدم, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    اذهب theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    لكل (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (صحيح)(cp - p);
    ܐܢ (len == 0)
    {
	curwin->w_cursor = old_pos;
	اذهب theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ܐܢ (spat == عدم || epat == عدم)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	اذهب theend;
    }
    sprintf((حرف *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((حرف *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, عدم,
						    0, عدم, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ܐܢ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	اذهب again;
    }

    ܐܢ (do_include)
    {
	// Include up to the '>'.
	ܟܡܐ (*ml_get_cursor() != '>')
	    ܐܢ (inc_cursor() < 0)
		اكسر;
    }
    ܐܚܪܢܐ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ܐܢ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ܐܚܪܢܐ ܐܢ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ܐܢ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	صحيح in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ܟܡܐ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ܐܢ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		اكسر;
	    }
	    ܐܚܪܢܐ ܐܢ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ܐܢ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    اذهب again;
	}
    }

    ܐܢ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ܐܢ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ܐܚܪܢܐ ܐܢ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ܐܚܪܢܐ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ܐܢ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ܐܚܪܢܐ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ܦܢܐ retval;
}
#endif

    صحيح
current_par(
    oparg_T	*oap,
    long	count,
    صحيح		include,	// TRUE == include white space
    صحيح		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    صحيح		white_in_front;
    صحيح		dir;
    صحيح		start_is_white;
    صحيح		prev_start_is_white;
    صحيح		retval = OK;
    صحيح		do_white = FALSE;
    صحيح		t;
    صحيح		i;

    ܐܢ (type == 'S')	    // not implemented yet
	ܦܢܐ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ܐܢ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ܐܢ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ܐܚܪܢܐ
	    dir = FORWARD;
	لكل (i = count; --i >= 0; )
	{
	    ܐܢ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		اكسر;
	    }

	    prev_start_is_white = -1;
	    لكل (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ܐܢ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    اكسر;
		}
		لكل (;;)
		{
		    ܐܢ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			اكسر;
		    ܐܢ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			اكسر;
		    start_lnum += dir;
		}
		ܐܢ (!include)
		    اكسر;
		ܐܢ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    اكسر;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ܦܢܐ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ܟܡܐ (start_lnum > 1)
    {
	ܐܢ (white_in_front)	    // stop at first white line
	{
	    ܐܢ (!linewhite(start_lnum - 1))
		اكسر;
	}
	ܐܚܪܢܐ		// stop at first non-white line of start of paragraph
	{
	    ܐܢ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		اكسر;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ܟܡܐ (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ܐܢ (!include && white_in_front)
	--i;
    ܟܡܐ (i--)
    {
	ܐܢ (end_lnum == curbuf->b_ml.ml_line_count)
	    ܦܢܐ FAIL;

	ܐܢ (!include)
	    do_white = linewhite(end_lnum + 1);

	ܐܢ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ܟܡܐ (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ܐܢ (i == 0 && white_in_front && include)
	    اكسر;

	/*
	 * skip to end of white lines after paragraph
	 */
	ܐܢ (include || do_white)
	    ܟܡܐ (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ܐܢ (!white_in_front && !linewhite(end_lnum) && include)
	ܟܡܐ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ܐܢ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ܐܢ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    اذهب extend;
	ܐܢ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ܐܚܪܢܐ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ܦܢܐ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ساكن صحيح
find_next_quote(
    char_u	*line,
    صحيح		col,
    صحيح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحيح		c;

    لكل (;;)
    {
	c = line[col];
	ܐܢ (c == NUL)
	    ܦܢܐ -1;
	ܐܚܪܢܐ ܐܢ (escape != عدم && vim_strchr(escape, c))
	{
	    ++col;
	    ܐܢ (line[col] == NUL)
		ܦܢܐ -1;
	}
	ܐܚܪܢܐ ܐܢ (c == quotechar)
	    اكسر;
	ܐܢ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	ܐܚܪܢܐ
	    ++col;
    }
    ܦܢܐ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ساكن صحيح
find_prev_quote(
    char_u	*line,
    صحيح		col_start,
    صحيح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحيح		n;

    ܟܡܐ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ܐܢ (escape != عدم)
	    ܟܡܐ (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != عدم)
	    ++n;
	ܐܢ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ܐܚܪܢܐ ܐܢ (line[col_start] == quotechar)
	    اكسر;
    }
    ܦܢܐ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    صحيح
current_quote(
    oparg_T	*oap,
    long	count,
    صحيح		include,	// TRUE == include quote char
    صحيح		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    صحيح		col_end;
    صحيح		col_start = curwin->w_cursor.col;
    صحيح		inclusive = FALSE;
    صحيح		vis_empty = TRUE;	// Visual selection <= 1 char
    صحيح		vis_bef_curs = FALSE;	// Visual starts before cursor
    صحيح		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    صحيح		inside_quotes = FALSE;	// Looks like "i'" done before
    صحيح		selected_quote = FALSE;	// Has quote inside selection
    صحيح		i;
    صحيح		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ܐܢ (VIsual_active)
    {
	// this only works within one line
	ܐܢ (VIsual.lnum != curwin->w_cursor.lnum)
	    ܦܢܐ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ܐܢ (*p_sel == 'e')
	{
	    ܐܢ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ܐܚܪܢܐ ܐܢ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ܐܢ (!vis_bef_curs && !vis_empty)
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

    ܐܢ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ܐܢ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ܐܚܪܢܐ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ܟܡܐ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ܐܢ (line[i] == NUL)
		اكسر;
	    ܐܢ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		اكسر;
	    }
	}
    }

    ܐܢ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ܐܢ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, عدم);
	    ܐܢ (col_start < 0)
		اذهب abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ܐܢ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ܐܚܪܢܐ
	{
	    col_end = find_prev_quote(line, col_start, quotechar, عدم);
	    ܐܢ (line[col_end] != quotechar)
		اذهب abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ܐܢ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ܐܚܪܢܐ

    ܐܢ (line[col_start] == quotechar || !vis_empty)
    {
	صحيح	first_col = col_start;

	ܐܢ (!vis_empty)
	{
	    ܐܢ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, عدم);
	    ܐܚܪܢܐ
		first_col = find_prev_quote(line, col_start, quotechar, عدم);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	لكل (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, عدم);
	    ܐܢ (col_start < 0 || col_start > first_col)
		اذهب abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ܐܢ (col_end < 0)
		اذهب abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ܐܢ (col_start <= first_col && first_col <= col_end)
		اكسر;
	    col_start = col_end + 1;
	}
    }
    ܐܚܪܢܐ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ܐܢ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, عدم);
	    ܐܢ (col_start < 0)
		اذهب abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ܐܢ (col_end < 0)
	    اذهب abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ܐܢ (include)
    {
	ܐܢ (VIM_ISWHITE(line[col_end + 1]))
	    ܟܡܐ (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	ܐܚܪܢܐ
	    ܟܡܐ (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ܐܢ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ܐܢ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ܐܢ (vis_empty
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
    ܐܚܪܢܐ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ܐܢ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ܐܢ (VIsual_active)
    {
	ܐܢ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ܐܢ (*p_sel != 'e')
		dec_cursor();
	}
	ܐܚܪܢܐ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ܐܢ (inside_quotes
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
	ܐܢ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ܐܚܪܢܐ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ܦܢܐ OK;

abort_search:
    ܐܢ (VIsual_active && *p_sel == 'e')
    {
	ܐܢ (did_exclusive_adj)
	    inc_cursor();
	ܐܢ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ܦܢܐ FALSE;
}
