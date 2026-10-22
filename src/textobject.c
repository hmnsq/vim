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
findsent(صحيح dir, long گنتی)
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
    إذا (dir == FORWARD)
	func = incl;
    وإلا
	func = decl;

    طالما (گنتی--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	إذا (gchar_pos(&pos) == NUL)
	{
	    افعل
		إذا ((*func)(&pos) == -1)
		    اكسر;
	    طالما (gchar_pos(&pos) == NUL);
	    إذا (dir == FORWARD)
		اذهب found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	وإلا إذا (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    إذا (pos.lnum == curbuf->b_ml.ml_سطر_گنتی)
		أرجع FAIL;
	    ++pos.lnum;
	    اذهب found;
	}
	وإلا إذا (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	طالما (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != عدم)
	{
	    tpos = pos;
	    إذا (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		اكسر;

	    إذا (found_dot)
		اكسر;
	    إذا (vim_strchr((char_u *) ".!?", c) != عدم)
		found_dot = TRUE;

	    إذا (vim_strchr((char_u *) ")]\"'", c) != عدم
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
	    إذا (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		إذا (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		اكسر;
	    }
	    إذا (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		افعل
		    إذا ((c = inc(&tpos)) == -1)
			اكسر;
		طالما (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= عدم);
		إذا (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    إذا (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    اكسر;
		}
	    }
	    إذا ((*func)(&pos) == -1)
	    {
		إذا (گنتی)
		    أرجع FAIL;
		noskip = TRUE;
		اكسر;
	    }
	}
found:
	    // skip white space
	طالما (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    إذا (incl(&pos) == -1)
		اكسر;

	إذا (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    إذا ((*func)(&pos) == -1)
	    {
		إذا (گنتی)
		    أرجع FAIL;
		اكسر;
	    }
	    ++گنتی;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    أرجع OK;
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
    long	گنتی,
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

    طالما (گنتی--)
    {
	did_skip = FALSE;
	لكل (first = TRUE; ; first = FALSE)
	{
	    إذا (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    إذا (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    إذا (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		اكسر;

#ifdef FEAT_FOLDING
	    إذا (fold_skipped)
		curr -= dir;
#endif
	    إذا ((curr += dir) < 1 || curr > curbuf->b_ml.ml_سطر_گنتی)
	    {
		إذا (گنتی)
		    أرجع FALSE;
		curr -= dir;
		اكسر;
	    }
	}
    }
    setpcmark();
    إذا (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    إذا (curr == curbuf->b_ml.ml_سطر_گنتی && what != '}' && dir == FORWARD)
    {
	char_u *سطر = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	إذا ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(سطر, سطر + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    وإلا
	curwin->w_cursor.col = 0;
    أرجع TRUE;
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
	إذا (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    اكسر;
	++macro;
	إذا (macro[0] == NUL)
	    اكسر;
    }
    أرجع (macro[0] != NUL);
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
    إذا (*s == para || *s == '\f' || (both && *s == '}'))
	أرجع TRUE;
    إذا (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	أرجع TRUE;
    أرجع FALSE;
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
    إذا (c == ' ' || c == '\t' || c == NUL)
	أرجع 0;
    إذا (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	إذا (enc_dbcs == DBCS_KOR && cls_bigword)
	    أرجع 1;

	// process code leading/trailing bytes
	أرجع dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    إذا (enc_utf8)
    {
	c = utf_class(c);
	إذا (c != 0 && cls_bigword)
	    أرجع 1;
	أرجع c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    إذا (cls_bigword)
	أرجع 1;

    إذا (vim_iswordc(c))
	أرجع 2;
    أرجع 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    صحيح
fwd_word(
    long	گنتی,
    صحيح		bigword,    // "W", "E" or "B"
    صحيح		eol)
{
    صحيح		sclass;	    // starting class
    صحيح		i;
    صحيح		last_سطر;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    طالما (--گنتی >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	إذا (hasFolding(curwin->w_cursor.lnum, عدم, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_سطر = (curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_گنتی);
	i = inc_cursor();
	إذا (i == -1 || (i >= 1 && last_سطر)) // started at last char in file
	    أرجع FAIL;
	إذا (i >= 1 && eol && گنتی == 0)      // started at last char in line
	    أرجع OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	إذا (sclass != 0)
	    طالما (cls() == sclass)
	    {
		i = inc_cursor();
		إذا (i == -1 || (i >= 1 && eol && گنتی == 0))
		    أرجع OK;
	    }

	/*
	 * go to next non-white
	 */
	طالما (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    إذا (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		اكسر;

	    i = inc_cursor();
	    إذا (i == -1 || (i >= 1 && eol && گنتی == 0))
		أرجع OK;
	}
    }
    أرجع OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    صحيح
bck_word(long گنتی, صحيح bigword, صحيح stop)
{
    صحيح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    طالما (--گنتی >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	إذا (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, عدم))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	إذا (dec_cursor() == -1)		// started at start of file
	    أرجع FAIL;

	إذا (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    طالما (cls() == 0)
	    {
		إذا (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    اذهب finished;
		إذا (dec_cursor() == -1) // hit start of file, stop here
		    أرجع OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    إذا (skip_chars(cls(), BACKWARD))
		أرجع OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    أرجع OK;
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
    long	گنتی,
    صحيح		bigword,
    صحيح		stop,
    صحيح		empty)
{
    صحيح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    إذا (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    طالما (--گنتی >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	إذا (hasFolding(curwin->w_cursor.lnum, عدم, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	إذا (inc_cursor() == -1)
	    أرجع FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	إذا (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    إذا (skip_chars(sclass, FORWARD))
		أرجع FAIL;
	}
	وإلا إذا (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    طالما (cls() == 0)
	    {
		إذا (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    اذهب finished;
		إذا (inc_cursor() == -1)	    // hit end of file, stop here
		    أرجع FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    إذا (skip_chars(cls(), FORWARD))
		أرجع FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    أرجع OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    صحيح
bckend_word(
    long	گنتی,
    صحيح		bigword,    // TRUE for "B"
    صحيح		eol)	    // TRUE: stop at end of line.
{
    صحيح		sclass;	    // starting class
    صحيح		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    طالما (--گنتی >= 0)
    {
	sclass = cls();
	إذا ((i = dec_cursor()) == -1)
	    أرجع FAIL;
	إذا (eol && i == 1)
	    أرجع OK;

	/*
	 * Move backward to before the start of this word.
	 */
	إذا (sclass != 0)
	{
	    طالما (cls() == sclass)
		إذا ((i = dec_cursor()) == -1 || (eol && i == 1))
		    أرجع OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	طالما (cls() == 0)
	{
	    إذا (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		اكسر;
	    إذا ((i = dec_cursor()) == -1 || (eol && i == 1))
		أرجع OK;
	}
    }
    adjust_skipcol();
    أرجع OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ساكن صحيح
skip_chars(صحيح cclass, صحيح dir)
{
    طالما (cls() == cclass)
	إذا ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ساكن فراغ
back_in_سطر(فراغ)
{
    صحيح		sclass;		    // starting class

    sclass = cls();
    لكل (;;)
    {
	إذا (curwin->w_cursor.col == 0)	    // stop at start of line
	    اكسر;
	dec_cursor();
	إذا (cls() != sclass)		    // stop at start of word
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

    طالما (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	إذا (!VIM_ISWHITE(c))
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
    long    گنتی,
    صحيح	    at_start_sent)	// cursor is at start of sentence
{
    طالما (گنتی--)
    {
	findsent(FORWARD, 1L);
	إذا (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	إذا (گنتی == 0 || at_start_sent)
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
    long	گنتی,
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
    إذا (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    إذا (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
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
	إذا ((cls() == 0) == include)
	{
	    إذا (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    إذا (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    وإلا
		oneleft();

	    إذا (include)
		include_white = TRUE;
	}

	إذا (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	وإلا
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--گنتی;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    طالما (گنتی > 0)
    {
	inclusive = TRUE;
	إذا (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    إذا (decl(&curwin->w_cursor) == -1)
		أرجع FAIL;
	    إذا (include != (cls() != 0))
	    {
		إذا (bck_word(1L, bigword, TRUE) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		إذا (bckend_word(1L, bigword, TRUE) == FAIL)
		    أرجع FAIL;
		(فراغ)incl(&curwin->w_cursor);
	    }
	}
	وإلا
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    إذا (incl(&curwin->w_cursor) == -1)
		أرجع FAIL;
	    إذا (include != (cls() == 0))
	    {
		إذا (fwd_word(1L, bigword, TRUE) == FAIL && گنتی > 1)
		    أرجع FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		إذا (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    وإلا
	    {
		إذا (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    أرجع FAIL;
	    }
	}
	--گنتی;
    }

    إذا (include_white && (cls() != 0
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
	إذا (oneleft() == OK)
	{
	    back_in_سطر();
	    إذا (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		إذا (VIsual_active)
		    VIsual = curwin->w_cursor;
		وإلا
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    إذا (VIsual_active)
    {
	إذا (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	إذا (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    وإلا
	oap->inclusive = inclusive;

    أرجع OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    صحيح
current_sent(oparg_T *oap, long گنتی, صحيح include)
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
    إذا (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	إذا (LT_POS(start_pos, VIsual))
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
	    طالما (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		إذا (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    اكسر;
		}
		incl(&pos);
	    }
	    إذا (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		إذا (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		وإلا
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    إذا (include)	// "as" gets twice as much as "is"
		گنتی *= 2;
	    طالما (گنتی--)
	    {
		إذا (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		إذا (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	وإلا
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
	    إذا (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		طالما (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    إذا (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			اكسر;
		    }
		    incl(&pos);
		}
		إذا (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		وإلا		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    إذا (include)	// "as" gets twice as much as "is"
		گنتی *= 2;
	    findsent_forward(گنتی, at_start_sent);
	    إذا (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	أرجع OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    طالما (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    إذا (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    وإلا
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    إذا (include)
	ncount = گنتی * 2;
    وإلا
    {
	ncount = گنتی;
	إذا (start_blank)
	    --ncount;
    }
    إذا (ncount > 0)
	findsent_forward(ncount, TRUE);
    وإلا
	decl(&curwin->w_cursor);

    إذا (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	إذا (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    إذا (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	وإلا إذا (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    إذا (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	إذا (EQUAL_POS(start_pos, curwin->w_cursor))
	    اذهب extend;
	إذا (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    وإلا
    {
	// include a newline after the sentence, if there is one
	إذا (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	وإلا
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    أرجع OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    صحيح
current_block(
    oparg_T	*oap,
    long	گنتی,
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
    إذا (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	إذا (what == '{')		// ignore indent
	    طالما (inindent(1))
		إذا (inc_cursor() != 0)
		    اكسر;
	إذا (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    وإلا إذا (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    وإلا
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != عدم ? "%M" : "%");
    إذا ((pos = findmatch(عدم, what)) != عدم)
    {
	طالما (گنتی-- > 0)
	{
	    إذا ((pos = findmatch(عدم, what)) == عدم)
		اكسر;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    وإلا
    {
	طالما (گنتی-- > 0)
	{
	    إذا ((pos = findmatchlimit(عدم, what, FM_FORWARD, 0)) == عدم)
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
    إذا (pos == عدم || (end_pos = findmatch(عدم, other)) == عدم)
    {
	curwin->w_cursor = old_pos;
	أرجع FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    طالما (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	طالما (inindent(1))
	{
	    sol = TRUE;
	    إذا (decl(&curwin->w_cursor) != 0)
		اكسر;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	إذا (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    أرجع FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	إذا (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    إذا ((pos = findmatch(عدم, what)) == عدم)
	    {
		curwin->w_cursor = old_pos;
		أرجع FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    إذا ((end_pos = findmatch(عدم, other)) == عدم)
	    {
		curwin->w_cursor = old_pos;
		أرجع FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	وإلا
	    اكسر;
    }

    إذا (VIsual_active)
    {
	إذا (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	إذا (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وإلا
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	إذا (sol)
	    incl(&curwin->w_cursor);
	وإلا إذا (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	وإلا
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    أرجع OK;
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ساكن صحيح
in_html_tag(
    صحيح		end_tag)
{
    char_u	*سطر = ml_get_curline();
    char_u	*p;
    صحيح		c;
    صحيح		lc = NUL;
    pos_T	pos;

    إذا (enc_dbcs)
    {
	char_u	*lp = عدم;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	لكل (p = سطر; p < سطر + curwin->w_cursor.col; MB_PTR_ADV(p))
	    إذا (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	إذا (*p != '<')	    // check for '<' under cursor
	{
	    إذا (lc != '<')
		أرجع FALSE;
	    p = lp;
	}
    }
    وإلا
    {
	لكل (p = سطر + curwin->w_cursor.col; p > سطر; )
	{
	    إذا (*p == '<')	// find '<' under/before cursor
		اكسر;
	    MB_PTR_BACK(سطر, p);
	    إذا (*p == '>')	// find '>' before cursor
		اكسر;
	}
	إذا (*p != '<')
	    أرجع FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - سطر);

    MB_PTR_ADV(p);
    إذا (end_tag)
	// check that there is a '/' after the '<'
	أرجع *p == '/';

    // check that there is no '/' after the '<'
    إذا (*p == '/')
	أرجع FALSE;

    // check that the matching '>' is not preceded by '/'
    لكل (;;)
    {
	إذا (inc(&pos) < 0)
	    أرجع FALSE;
	c = *ml_get_pos(&pos);
	إذا (c == '>')
	    اكسر;
	lc = c;
    }
    أرجع lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    صحيح
current_tagblock(
    oparg_T	*oap,
    long	گنتی_arg,
    صحيح		include)	// TRUE == include white space
{
    long	گنتی = گنتی_arg;
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
    إذا (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    إذا (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	طالما (inindent(1))
	    إذا (inc_cursor() != 0)
		اكسر;

	إذا (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    طالما (*ml_get_cursor() != '>')
		إذا (inc_cursor() < 0)
		    اكسر;
	}
	وإلا إذا (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    طالما (*ml_get_cursor() != '<')
		إذا (dec_cursor() < 0)
		    اكسر;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    وإلا إذا (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    وإلا
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    لكل (n = 0; n < گنتی; ++n)
    {
	إذا (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    إذا (len == 0)
    {
	curwin->w_cursor = old_pos;
	اذهب theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    إذا (spat == عدم || epat == عدم)
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

    إذا (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	گنتی = 1;
	curwin->w_cursor = start_pos;
	اذهب again;
    }

    إذا (do_include)
    {
	// Include up to the '>'.
	طالما (*ml_get_cursor() != '>')
	    إذا (inc_cursor() < 0)
		اكسر;
    }
    وإلا
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	إذا (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	وإلا إذا (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    إذا (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	صحيح in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	طالما (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    إذا (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		اكسر;
	    }
	    وإلا إذا (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	إذا (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    گنتی = گنتی_arg;
	    اذهب again;
	}
    }

    إذا (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	إذا (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	وإلا إذا (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وإلا
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	إذا (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	وإلا
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    أرجع retval;
}
#endif

    صحيح
current_par(
    oparg_T	*oap,
    long	گنتی,
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

    إذا (type == 'S')	    // not implemented yet
	أرجع FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    إذا (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	إذا (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	وإلا
	    dir = FORWARD;
	لكل (i = گنتی; --i >= 0; )
	{
	    إذا (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_سطر_گنتی))
	    {
		retval = FAIL;
		اكسر;
	    }

	    prev_start_is_white = -1;
	    لكل (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		إذا (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    اكسر;
		}
		لكل (;;)
		{
		    إذا (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_گنتی))
			اكسر;
		    إذا (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			اكسر;
		    start_lnum += dir;
		}
		إذا (!include)
		    اكسر;
		إذا (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_گنتی))
		    اكسر;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	أرجع retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    طالما (start_lnum > 1)
    {
	إذا (white_in_front)	    // stop at first white line
	{
	    إذا (!linewhite(start_lnum - 1))
		اكسر;
	}
	وإلا		// stop at first non-white line of start of paragraph
	{
	    إذا (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		اكسر;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    طالما (end_lnum <= curbuf->b_ml.ml_سطر_گنتی && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = گنتی;
    إذا (!include && white_in_front)
	--i;
    طالما (i--)
    {
	إذا (end_lnum == curbuf->b_ml.ml_سطر_گنتی)
	    أرجع FAIL;

	إذا (!include)
	    do_white = linewhite(end_lnum + 1);

	إذا (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    طالما (end_lnum < curbuf->b_ml.ml_سطر_گنتی
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	إذا (i == 0 && white_in_front && include)
	    اكسر;

	/*
	 * skip to end of white lines after paragraph
	 */
	إذا (include || do_white)
	    طالما (end_lnum < curbuf->b_ml.ml_سطر_گنتی
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    إذا (!white_in_front && !linewhite(end_lnum) && include)
	طالما (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    إذا (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	إذا (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    اذهب extend;
	إذا (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    وإلا
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    أرجع OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ساكن صحيح
find_next_quote(
    char_u	*سطر,
    صحيح		col,
    صحيح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحيح		c;

    لكل (;;)
    {
	c = سطر[col];
	إذا (c == NUL)
	    أرجع -1;
	وإلا إذا (escape != عدم && vim_strchr(escape, c))
	{
	    ++col;
	    إذا (سطر[col] == NUL)
		أرجع -1;
	}
	وإلا إذا (c == quotechar)
	    اكسر;
	إذا (has_mbyte)
	    col += (*mb_ptr2len)(سطر + col);
	وإلا
	    ++col;
    }
    أرجع col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ساكن صحيح
find_prev_quote(
    char_u	*سطر,
    صحيح		col_start,
    صحيح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحيح		n;

    طالما (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(سطر, سطر + col_start);
	n = 0;
	إذا (escape != عدم)
	    طالما (col_start - n > 0 && vim_strchr(escape,
					     سطر[col_start - n - 1]) != عدم)
	    ++n;
	إذا (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	وإلا إذا (سطر[col_start] == quotechar)
	    اكسر;
    }
    أرجع col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    صحيح
current_quote(
    oparg_T	*oap,
    long	گنتی,
    صحيح		include,	// TRUE == include quote char
    صحيح		quotechar)	// Quote character
{
    char_u	*سطر = ml_get_curline();
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
    إذا (VIsual_active)
    {
	// this only works within one line
	إذا (VIsual.lnum != curwin->w_cursor.lnum)
	    أرجع FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	إذا (*p_sel == 'e')
	{
	    إذا (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    وإلا إذا (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    إذا (!vis_bef_curs && !vis_empty)
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

    إذا (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	إذا (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& سطر[VIsual.col - 1] == quotechar
			&& سطر[curwin->w_cursor.col] != NUL
			&& سطر[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	وإلا
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& سطر[curwin->w_cursor.col - 1] == quotechar
			&& سطر[VIsual.col] != NUL
			&& سطر[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	طالما (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    إذا (سطر[i] == NUL)
		اكسر;
	    إذا (سطر[i++] == quotechar)
	    {
		selected_quote = TRUE;
		اكسر;
	    }
	}
    }

    إذا (!vis_empty && سطر[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	إذا (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(سطر, col_start + 1, quotechar, عدم);
	    إذا (col_start < 0)
		اذهب abort_تلاش;
	    col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    إذا (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	وإلا
	{
	    col_end = find_prev_quote(سطر, col_start, quotechar, عدم);
	    إذا (سطر[col_end] != quotechar)
		اذهب abort_تلاش;
	    col_start = find_prev_quote(سطر, col_end, quotechar,
							      curbuf->b_p_qe);
	    إذا (سطر[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    وإلا

    إذا (سطر[col_start] == quotechar || !vis_empty)
    {
	صحيح	first_col = col_start;

	إذا (!vis_empty)
	{
	    إذا (vis_bef_curs)
		first_col = find_next_quote(سطر, col_start, quotechar, عدم);
	    وإلا
		first_col = find_prev_quote(سطر, col_start, quotechar, عدم);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	لكل (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(سطر, col_start, quotechar, عدم);
	    إذا (col_start < 0 || col_start > first_col)
		اذهب abort_تلاش;
	    // Find close quote character.
	    col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    إذا (col_end < 0)
		اذهب abort_تلاش;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    إذا (col_start <= first_col && first_col <= col_end)
		اكسر;
	    col_start = col_end + 1;
	}
    }
    وإلا
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(سطر, col_start, quotechar, curbuf->b_p_qe);
	إذا (سطر[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(سطر, col_start, quotechar, عدم);
	    إذا (col_start < 0)
		اذهب abort_تلاش;
	}

	// Find close quote character.
	col_end = find_next_quote(سطر, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	إذا (col_end < 0)
	    اذهب abort_تلاش;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    إذا (include)
    {
	إذا (VIM_ISWHITE(سطر[col_end + 1]))
	    طالما (VIM_ISWHITE(سطر[col_end + 1]))
		++col_end;
	وإلا
	    طالما (col_start > 0 && VIM_ISWHITE(سطر[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    إذا (!include && گنتی < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    إذا (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	إذا (vis_empty
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
    وإلا
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    إذا ((include || گنتی > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    إذا (VIsual_active)
    {
	إذا (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    إذا (*p_sel != 'e')
		dec_cursor();
	}
	وإلا
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    إذا (inside_quotes
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
	إذا (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    وإلا
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    أرجع OK;

abort_تلاش:
    إذا (VIsual_active && *p_sel == 'e')
    {
	إذا (did_exclusive_adj)
	    inc_cursor();
	إذا (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    أرجع FALSE;
}
