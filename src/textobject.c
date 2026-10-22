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
#شامل "vim.h"

ایستا صحیح cls(تهی);
ایستا صحیح skip_chars(صحیح, صحیح);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    صحیح
findsent(صحیح dir, long شمار)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    صحیح		c;
    صحیح		(*func)(pos_T *);
    صحیح		startlnum;
    صحیح		noskip = FALSE;	    // do not skip blanks
    صحیح		cpo_J;
    صحیح		found_dot;

    pos = curwin->w_cursor;
    اگر (dir == FORWARD)
	func = incl;
    وگرنه
	func = decl;

    وقتی (شمار--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	اگر (gchar_pos(&pos) == NUL)
	{
	    انجامبده
		اگر ((*func)(&pos) == -1)
		    بشکن;
	    وقتی (gchar_pos(&pos) == NUL);
	    اگر (dir == FORWARD)
		برو found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	وگرنه اگر (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    اگر (pos.lnum == curbuf->b_ml.ml_سطر_شمار)
		بازگشت FAIL;
	    ++pos.lnum;
	    برو found;
	}
	وگرنه اگر (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	وقتی (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != پوچ)
	{
	    tpos = pos;
	    اگر (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		بشکن;

	    اگر (found_dot)
		بشکن;
	    اگر (vim_strchr((char_u *) ".!?", c) != پوچ)
		found_dot = TRUE;

	    اگر (vim_strchr((char_u *) ")]\"'", c) != پوچ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == پوچ)
		بشکن;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != پوچ;

	برای (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    اگر (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		اگر (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		بشکن;
	    }
	    اگر (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		انجامبده
		    اگر ((c = inc(&tpos)) == -1)
			بشکن;
		وقتی (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= پوچ);
		اگر (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    اگر (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    بشکن;
		}
	    }
	    اگر ((*func)(&pos) == -1)
	    {
		اگر (شمار)
		    بازگشت FAIL;
		noskip = TRUE;
		بشکن;
	    }
	}
found:
	    // skip white space
	وقتی (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    اگر (incl(&pos) == -1)
		بشکن;

	اگر (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    اگر ((*func)(&pos) == -1)
	    {
		اگر (شمار)
		    بازگشت FAIL;
		بشکن;
	    }
	    ++شمار;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    بازگشت OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    صحیح
findpar(
    صحیح		*pincl,	    // Return: TRUE if last char is to be included
    صحیح		dir,
    long	شمار,
    صحیح		what,
    صحیح		both)
{
    linenr_T	curr;
    صحیح		did_skip;   // TRUE after separating lines have been skipped
    صحیح		first;	    // TRUE on first line
    صحیح		posix = (vim_strchr(p_cpo, CPO_PARA) != پوچ);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    صحیح		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    وقتی (شمار--)
    {
	did_skip = FALSE;
	برای (first = TRUE; ; first = FALSE)
	{
	    اگر (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    اگر (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    اگر (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		بشکن;

#ifdef FEAT_FOLDING
	    اگر (fold_skipped)
		curr -= dir;
#endif
	    اگر ((curr += dir) < 1 || curr > curbuf->b_ml.ml_سطر_شمار)
	    {
		اگر (شمار)
		    بازگشت FALSE;
		curr -= dir;
		بشکن;
	    }
	}
    }
    setpcmark();
    اگر (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    اگر (curr == curbuf->b_ml.ml_سطر_شمار && what != '}' && dir == FORWARD)
    {
	char_u *سطر = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	اگر ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(سطر, سطر + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    وگرنه
	curwin->w_cursor.col = 0;
    بازگشت TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ایستا صحیح
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    برای (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	اگر (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    بشکن;
	++macro;
	اگر (macro[0] == NUL)
	    بشکن;
    }
    بازگشت (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    صحیح
startPS(linenr_T lnum, صحیح para, صحیح both)
{
    char_u	*s;

    s = ml_get(lnum);
    اگر (*s == para || *s == '\f' || (both && *s == '}'))
	بازگشت TRUE;
    اگر (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	بازگشت TRUE;
    بازگشت FALSE;
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

ایستا صحیح	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ایستا صحیح
cls(تهی)
{
    صحیح	    c;

    c = gchar_cursor();
    اگر (c == ' ' || c == '\t' || c == NUL)
	بازگشت 0;
    اگر (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	اگر (enc_dbcs == DBCS_KOR && cls_bigword)
	    بازگشت 1;

	// process code leading/trailing bytes
	بازگشت dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    اگر (enc_utf8)
    {
	c = utf_class(c);
	اگر (c != 0 && cls_bigword)
	    بازگشت 1;
	بازگشت c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    اگر (cls_bigword)
	بازگشت 1;

    اگر (vim_iswordc(c))
	بازگشت 2;
    بازگشت 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    صحیح
fwd_word(
    long	شمار,
    صحیح		bigword,    // "W", "E" or "B"
    صحیح		eol)
{
    صحیح		sclass;	    // starting class
    صحیح		i;
    صحیح		last_سطر;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    وقتی (--شمار >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	اگر (hasFolding(curwin->w_cursor.lnum, پوچ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_سطر = (curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_شمار);
	i = inc_cursor();
	اگر (i == -1 || (i >= 1 && last_سطر)) // started at last char in file
	    بازگشت FAIL;
	اگر (i >= 1 && eol && شمار == 0)      // started at last char in line
	    بازگشت OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	اگر (sclass != 0)
	    وقتی (cls() == sclass)
	    {
		i = inc_cursor();
		اگر (i == -1 || (i >= 1 && eol && شمار == 0))
		    بازگشت OK;
	    }

	/*
	 * go to next non-white
	 */
	وقتی (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    اگر (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		بشکن;

	    i = inc_cursor();
	    اگر (i == -1 || (i >= 1 && eol && شمار == 0))
		بازگشت OK;
	}
    }
    بازگشت OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    صحیح
bck_word(long شمار, صحیح bigword, صحیح stop)
{
    صحیح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    وقتی (--شمار >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	اگر (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, پوچ))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	اگر (dec_cursor() == -1)		// started at start of file
	    بازگشت FAIL;

	اگر (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    وقتی (cls() == 0)
	    {
		اگر (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    برو finished;
		اگر (dec_cursor() == -1) // hit start of file, stop here
		    بازگشت OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    اگر (skip_chars(cls(), BACKWARD))
		بازگشت OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    بازگشت OK;
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
    صحیح
end_word(
    long	شمار,
    صحیح		bigword,
    صحیح		stop,
    صحیح		empty)
{
    صحیح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    اگر (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    وقتی (--شمار >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	اگر (hasFolding(curwin->w_cursor.lnum, پوچ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	اگر (inc_cursor() == -1)
	    بازگشت FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	اگر (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    اگر (skip_chars(sclass, FORWARD))
		بازگشت FAIL;
	}
	وگرنه اگر (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    وقتی (cls() == 0)
	    {
		اگر (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    برو finished;
		اگر (inc_cursor() == -1)	    // hit end of file, stop here
		    بازگشت FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    اگر (skip_chars(cls(), FORWARD))
		بازگشت FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    بازگشت OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    صحیح
bckend_word(
    long	شمار,
    صحیح		bigword,    // TRUE for "B"
    صحیح		eol)	    // TRUE: stop at end of line.
{
    صحیح		sclass;	    // starting class
    صحیح		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    وقتی (--شمار >= 0)
    {
	sclass = cls();
	اگر ((i = dec_cursor()) == -1)
	    بازگشت FAIL;
	اگر (eol && i == 1)
	    بازگشت OK;

	/*
	 * Move backward to before the start of this word.
	 */
	اگر (sclass != 0)
	{
	    وقتی (cls() == sclass)
		اگر ((i = dec_cursor()) == -1 || (eol && i == 1))
		    بازگشت OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	وقتی (cls() == 0)
	{
	    اگر (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		بشکن;
	    اگر ((i = dec_cursor()) == -1 || (eol && i == 1))
		بازگشت OK;
	}
    }
    adjust_skipcol();
    بازگشت OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ایستا صحیح
skip_chars(صحیح cclass, صحیح dir)
{
    وقتی (cls() == cclass)
	اگر ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ایستا تهی
back_in_سطر(تهی)
{
    صحیح		sclass;		    // starting class

    sclass = cls();
    برای (;;)
    {
	اگر (curwin->w_cursor.col == 0)	    // stop at start of line
	    بشکن;
	dec_cursor();
	اگر (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    بشکن;
	}
    }
}

    ایستا تهی
find_first_blank(pos_T *posp)
{
    صحیح	    c;

    وقتی (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	اگر (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    بشکن;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ایستا تهی
findsent_forward(
    long    شمار,
    صحیح	    at_start_sent)	// cursor is at start of sentence
{
    وقتی (شمار--)
    {
	findsent(FORWARD, 1L);
	اگر (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	اگر (شمار == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحیح
current_word(
    oparg_T	*oap,
    long	شمار,
    صحیح		include,	// TRUE: include word and white space
    صحیح		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    صحیح		inclusive = TRUE;
    صحیح		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    اگر (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    اگر (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_سطر();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	اگر ((cls() == 0) == include)
	{
	    اگر (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    اگر (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    وگرنه
		oneleft();

	    اگر (include)
		include_white = TRUE;
	}

	اگر (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	وگرنه
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--شمار;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    وقتی (شمار > 0)
    {
	inclusive = TRUE;
	اگر (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    اگر (decl(&curwin->w_cursor) == -1)
		بازگشت FAIL;
	    اگر (include != (cls() != 0))
	    {
		اگر (bck_word(1L, bigword, TRUE) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		اگر (bckend_word(1L, bigword, TRUE) == FAIL)
		    بازگشت FAIL;
		(تهی)incl(&curwin->w_cursor);
	    }
	}
	وگرنه
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    اگر (incl(&curwin->w_cursor) == -1)
		بازگشت FAIL;
	    اگر (include != (cls() == 0))
	    {
		اگر (fwd_word(1L, bigword, TRUE) == FAIL && شمار > 1)
		    بازگشت FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		اگر (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    وگرنه
	    {
		اگر (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    بازگشت FAIL;
	    }
	}
	--شمار;
    }

    اگر (include_white && (cls() != 0
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
	اگر (oneleft() == OK)
	{
	    back_in_سطر();
	    اگر (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		اگر (VIsual_active)
		    VIsual = curwin->w_cursor;
		وگرنه
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    اگر (VIsual_active)
    {
	اگر (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	اگر (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    وگرنه
	oap->inclusive = inclusive;

    بازگشت OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    صحیح
current_sent(oparg_T *oap, long شمار, صحیح include)
{
    pos_T	start_pos;
    pos_T	pos;
    صحیح		start_blank;
    صحیح		c;
    صحیح		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    اگر (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	اگر (LT_POS(start_pos, VIsual))
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
	    وقتی (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		اگر (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    بشکن;
		}
		incl(&pos);
	    }
	    اگر (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		اگر (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		وگرنه
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    اگر (include)	// "as" gets twice as much as "is"
		شمار *= 2;
	    وقتی (شمار--)
	    {
		اگر (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		اگر (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	وگرنه
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
	    اگر (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		وقتی (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    اگر (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			بشکن;
		    }
		    incl(&pos);
		}
		اگر (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		وگرنه		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    اگر (include)	// "as" gets twice as much as "is"
		شمار *= 2;
	    findsent_forward(شمار, at_start_sent);
	    اگر (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	بازگشت OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    وقتی (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    اگر (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    وگرنه
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    اگر (include)
	ncount = شمار * 2;
    وگرنه
    {
	ncount = شمار;
	اگر (start_blank)
	    --ncount;
    }
    اگر (ncount > 0)
	findsent_forward(ncount, TRUE);
    وگرنه
	decl(&curwin->w_cursor);

    اگر (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	اگر (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    اگر (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	وگرنه اگر (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    اگر (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	اگر (EQUAL_POS(start_pos, curwin->w_cursor))
	    برو extend;
	اگر (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    وگرنه
    {
	// include a newline after the sentence, if there is one
	اگر (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	وگرنه
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    بازگشت OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    صحیح
current_block(
    oparg_T	*oap,
    long	شمار,
    صحیح		include,	// TRUE == include white space
    صحیح		what,		// '(', '{', etc.
    صحیح		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = پوچ;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    صحیح		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    اگر (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	اگر (what == '{')		// ignore indent
	    وقتی (inindent(1))
		اگر (inc_cursor() != 0)
		    بشکن;
	اگر (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    وگرنه اگر (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    وگرنه
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != پوچ ? "%M" : "%");
    اگر ((pos = findmatch(پوچ, what)) != پوچ)
    {
	وقتی (شمار-- > 0)
	{
	    اگر ((pos = findmatch(پوچ, what)) == پوچ)
		بشکن;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    وگرنه
    {
	وقتی (شمار-- > 0)
	{
	    اگر ((pos = findmatchlimit(پوچ, what, FM_FORWARD, 0)) == پوچ)
		بشکن;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    اگر (pos == پوچ || (end_pos = findmatch(پوچ, other)) == پوچ)
    {
	curwin->w_cursor = old_pos;
	بازگشت FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    وقتی (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	وقتی (inindent(1))
	{
	    sol = TRUE;
	    اگر (decl(&curwin->w_cursor) != 0)
		بشکن;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	اگر (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    بازگشت FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	اگر (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    اگر ((pos = findmatch(پوچ, what)) == پوچ)
	    {
		curwin->w_cursor = old_pos;
		بازگشت FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    اگر ((end_pos = findmatch(پوچ, other)) == پوچ)
	    {
		curwin->w_cursor = old_pos;
		بازگشت FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	وگرنه
	    بشکن;
    }

    اگر (VIsual_active)
    {
	اگر (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	اگر (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وگرنه
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	اگر (sol)
	    incl(&curwin->w_cursor);
	وگرنه اگر (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	وگرنه
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    بازگشت OK;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ایستا صحیح
in_html_tag(
    صحیح		end_tag)
{
    char_u	*سطر = ml_get_curline();
    char_u	*p;
    صحیح		c;
    صحیح		lc = NUL;
    pos_T	pos;

    اگر (enc_dbcs)
    {
	char_u	*lp = پوچ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	برای (p = سطر; p < سطر + curwin->w_cursor.col; MB_PTR_ADV(p))
	    اگر (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	اگر (*p != '<')	    // check for '<' under cursor
	{
	    اگر (lc != '<')
		بازگشت FALSE;
	    p = lp;
	}
    }
    وگرنه
    {
	برای (p = سطر + curwin->w_cursor.col; p > سطر; )
	{
	    اگر (*p == '<')	// find '<' under/before cursor
		بشکن;
	    MB_PTR_BACK(سطر, p);
	    اگر (*p == '>')	// find '>' before cursor
		بشکن;
	}
	اگر (*p != '<')
	    بازگشت FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - سطر);

    MB_PTR_ADV(p);
    اگر (end_tag)
	// check that there is a '/' after the '<'
	بازگشت *p == '/';

    // check that there is no '/' after the '<'
    اگر (*p == '/')
	بازگشت FALSE;

    // check that the matching '>' is not preceded by '/'
    برای (;;)
    {
	اگر (inc(&pos) < 0)
	    بازگشت FALSE;
	c = *ml_get_pos(&pos);
	اگر (c == '>')
	    بشکن;
	lc = c;
    }
    بازگشت lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    صحیح
current_tagblock(
    oparg_T	*oap,
    long	شمار_arg,
    صحیح		include)	// TRUE == include white space
{
    long	شمار = شمار_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    صحیح		len;
    صحیح		r;
    صحیح		do_include = include;
    صحیح		save_p_ws = p_ws;
    صحیح		retval = FAIL;
    صحیح		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    اگر (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    اگر (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	وقتی (inindent(1))
	    اگر (inc_cursor() != 0)
		بشکن;

	اگر (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    وقتی (*ml_get_cursor() != '>')
		اگر (inc_cursor() < 0)
		    بشکن;
	}
	وگرنه اگر (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    وقتی (*ml_get_cursor() != '<')
		اگر (dec_cursor() < 0)
		    بشکن;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    وگرنه اگر (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    وگرنه
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    برای (n = 0; n < شمار; ++n)
    {
	اگر (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, پوچ, 0,
						  پوچ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    برو theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    برای (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (صحیح)(cp - p);
    اگر (len == 0)
    {
	curwin->w_cursor = old_pos;
	برو theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    اگر (spat == پوچ || epat == پوچ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	برو theend;
    }
    sprintf((نویسه *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((نویسه *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, پوچ,
						    0, پوچ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    اگر (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	شمار = 1;
	curwin->w_cursor = start_pos;
	برو again;
    }

    اگر (do_include)
    {
	// Include up to the '>'.
	وقتی (*ml_get_cursor() != '>')
	    اگر (inc_cursor() < 0)
		بشکن;
    }
    وگرنه
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	اگر (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	وگرنه اگر (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    اگر (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	صحیح in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	وقتی (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    اگر (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		بشکن;
	    }
	    وگرنه اگر (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	اگر (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    شمار = شمار_arg;
	    برو again;
	}
    }

    اگر (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	اگر (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	وگرنه اگر (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وگرنه
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	اگر (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	وگرنه
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    بازگشت retval;
}
#endif

    صحیح
current_par(
    oparg_T	*oap,
    long	شمار,
    صحیح		include,	// TRUE == include white space
    صحیح		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    صحیح		white_in_front;
    صحیح		dir;
    صحیح		start_is_white;
    صحیح		prev_start_is_white;
    صحیح		retval = OK;
    صحیح		do_white = FALSE;
    صحیح		t;
    صحیح		i;

    اگر (type == 'S')	    // not implemented yet
	بازگشت FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    اگر (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	اگر (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	وگرنه
	    dir = FORWARD;
	برای (i = شمار; --i >= 0; )
	{
	    اگر (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_سطر_شمار))
	    {
		retval = FAIL;
		بشکن;
	    }

	    prev_start_is_white = -1;
	    برای (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		اگر (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    بشکن;
		}
		برای (;;)
		{
		    اگر (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_شمار))
			بشکن;
		    اگر (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			بشکن;
		    start_lnum += dir;
		}
		اگر (!include)
		    بشکن;
		اگر (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_شمار))
		    بشکن;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	بازگشت retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    وقتی (start_lnum > 1)
    {
	اگر (white_in_front)	    // stop at first white line
	{
	    اگر (!linewhite(start_lnum - 1))
		بشکن;
	}
	وگرنه		// stop at first non-white line of start of paragraph
	{
	    اگر (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		بشکن;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    وقتی (end_lnum <= curbuf->b_ml.ml_سطر_شمار && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = شمار;
    اگر (!include && white_in_front)
	--i;
    وقتی (i--)
    {
	اگر (end_lnum == curbuf->b_ml.ml_سطر_شمار)
	    بازگشت FAIL;

	اگر (!include)
	    do_white = linewhite(end_lnum + 1);

	اگر (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    وقتی (end_lnum < curbuf->b_ml.ml_سطر_شمار
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	اگر (i == 0 && white_in_front && include)
	    بشکن;

	/*
	 * skip to end of white lines after paragraph
	 */
	اگر (include || do_white)
	    وقتی (end_lnum < curbuf->b_ml.ml_سطر_شمار
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    اگر (!white_in_front && !linewhite(end_lnum) && include)
	وقتی (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    اگر (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	اگر (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    برو extend;
	اگر (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وگرنه
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    بازگشت OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ایستا صحیح
find_next_quote(
    char_u	*سطر,
    صحیح		col,
    صحیح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحیح		c;

    برای (;;)
    {
	c = سطر[col];
	اگر (c == NUL)
	    بازگشت -1;
	وگرنه اگر (escape != پوچ && vim_strchr(escape, c))
	{
	    ++col;
	    اگر (سطر[col] == NUL)
		بازگشت -1;
	}
	وگرنه اگر (c == quotechar)
	    بشکن;
	اگر (has_mbyte)
	    col += (*mb_ptr2len)(سطر + col);
	وگرنه
	    ++col;
    }
    بازگشت col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ایستا صحیح
find_prev_quote(
    char_u	*سطر,
    صحیح		col_start,
    صحیح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحیح		n;

    وقتی (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(سطر, سطر + col_start);
	n = 0;
	اگر (escape != پوچ)
	    وقتی (col_start - n > 0 && vim_strchr(escape,
					     سطر[col_start - n - 1]) != پوچ)
	    ++n;
	اگر (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	وگرنه اگر (سطر[col_start] == quotechar)
	    بشکن;
    }
    بازگشت col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    صحیح
current_quote(
    oparg_T	*oap,
    long	شمار,
    صحیح		include,	// TRUE == include quote char
    صحیح		quotechar)	// Quote character
{
    char_u	*سطر = ml_get_curline();
    صحیح		col_end;
    صحیح		col_start = curwin->w_cursor.col;
    صحیح		inclusive = FALSE;
    صحیح		vis_empty = TRUE;	// Visual selection <= 1 char
    صحیح		vis_bef_curs = FALSE;	// Visual starts before cursor
    صحیح		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    صحیح		inside_quotes = FALSE;	// Looks like "i'" done before
    صحیح		selected_quote = FALSE;	// Has quote inside selection
    صحیح		i;
    صحیح		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    اگر (VIsual_active)
    {
	// this only works within one line
	اگر (VIsual.lnum != curwin->w_cursor.lnum)
	    بازگشت FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	اگر (*p_sel == 'e')
	{
	    اگر (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    وگرنه اگر (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    اگر (!vis_bef_curs && !vis_empty)
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

    اگر (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	اگر (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& سطر[VIsual.col - 1] == quotechar
			&& سطر[curwin->w_cursor.col] != NUL
			&& سطر[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	وگرنه
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& سطر[curwin->w_cursor.col - 1] == quotechar
			&& سطر[VIsual.col] != NUL
			&& سطر[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	وقتی (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    اگر (سطر[i] == NUL)
		بشکن;
	    اگر (سطر[i++] == quotechar)
	    {
		selected_quote = TRUE;
		بشکن;
	    }
	}
    }

    اگر (!vis_empty && سطر[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	اگر (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(سطر, col_start + 1, quotechar, پوچ);
	    اگر (col_start < 0)
		برو abort_جستجو;
	    col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    اگر (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	وگرنه
	{
	    col_end = find_prev_quote(سطر, col_start, quotechar, پوچ);
	    اگر (سطر[col_end] != quotechar)
		برو abort_جستجو;
	    col_start = find_prev_quote(سطر, col_end, quotechar,
							      curbuf->b_p_qe);
	    اگر (سطر[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    وگرنه

    اگر (سطر[col_start] == quotechar || !vis_empty)
    {
	صحیح	first_col = col_start;

	اگر (!vis_empty)
	{
	    اگر (vis_bef_curs)
		first_col = find_next_quote(سطر, col_start, quotechar, پوچ);
	    وگرنه
		first_col = find_prev_quote(سطر, col_start, quotechar, پوچ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	برای (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(سطر, col_start, quotechar, پوچ);
	    اگر (col_start < 0 || col_start > first_col)
		برو abort_جستجو;
	    // Find close quote character.
	    col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    اگر (col_end < 0)
		برو abort_جستجو;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    اگر (col_start <= first_col && first_col <= col_end)
		بشکن;
	    col_start = col_end + 1;
	}
    }
    وگرنه
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(سطر, col_start, quotechar, curbuf->b_p_qe);
	اگر (سطر[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(سطر, col_start, quotechar, پوچ);
	    اگر (col_start < 0)
		برو abort_جستجو;
	}

	// Find close quote character.
	col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	اگر (col_end < 0)
	    برو abort_جستجو;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    اگر (include)
    {
	اگر (VIM_ISWHITE(سطر[col_end + 1]))
	    وقتی (VIM_ISWHITE(سطر[col_end + 1]))
		++col_end;
	وگرنه
	    وقتی (col_start > 0 && VIM_ISWHITE(سطر[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    اگر (!include && شمار < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    اگر (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	اگر (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (سطر[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| سطر[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    وگرنه
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    اگر ((include || شمار > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    اگر (VIsual_active)
    {
	اگر (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    اگر (*p_sel != 'e')
		dec_cursor();
	}
	وگرنه
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    اگر (inside_quotes
		    || (!selected_quote
			&& سطر[VIsual.col] != quotechar
			&& (سطر[VIsual.col] == NUL
			    || سطر[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	اگر (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    وگرنه
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    بازگشت OK;

abort_جستجو:
    اگر (VIsual_active && *p_sel == 'e')
    {
	اگر (did_exclusive_adj)
	    inc_cursor();
	اگر (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    بازگشت FALSE;
}
