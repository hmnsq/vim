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
#அந்தர்பாவ "vim.h"

நிலையான முழு cls(வெறுமை);
நிலையான முழு skip_chars(முழு, முழு);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    முழு
findsent(முழு dir, long மொத்தம்)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    முழு		c;
    முழு		(*func)(pos_T *);
    முழு		startlnum;
    முழு		noskip = FALSE;	    // do not skip blanks
    முழு		cpo_J;
    முழு		found_dot;

    pos = curwin->w_cursor;
    எனில் (dir == FORWARD)
	func = incl;
    இல்லையெனில்
	func = decl;

    போது (மொத்தம்--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	எனில் (gchar_pos(&pos) == NUL)
	{
	    செய்
		எனில் ((*func)(&pos) == -1)
		    நிறுத்து;
	    போது (gchar_pos(&pos) == NUL);
	    எனில் (dir == FORWARD)
		செல் found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	இல்லையெனில் எனில் (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    எனில் (pos.lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
		திரும்பு FAIL;
	    ++pos.lnum;
	    செல் found;
	}
	இல்லையெனில் எனில் (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	போது (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != வெற்று)
	{
	    tpos = pos;
	    எனில் (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		நிறுத்து;

	    எனில் (found_dot)
		நிறுத்து;
	    எனில் (vim_strchr((char_u *) ".!?", c) != வெற்று)
		found_dot = TRUE;

	    எனில் (vim_strchr((char_u *) ")]\"'", c) != வெற்று
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == வெற்று)
		நிறுத்து;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != வெற்று;

	க்கு (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    எனில் (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		எனில் (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		நிறுத்து;
	    }
	    எனில் (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		செய்
		    எனில் ((c = inc(&tpos)) == -1)
			நிறுத்து;
		போது (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= வெற்று);
		எனில் (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    எனில் (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    நிறுத்து;
		}
	    }
	    எனில் ((*func)(&pos) == -1)
	    {
		எனில் (மொத்தம்)
		    திரும்பு FAIL;
		noskip = TRUE;
		நிறுத்து;
	    }
	}
found:
	    // skip white space
	போது (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    எனில் (incl(&pos) == -1)
		நிறுத்து;

	எனில் (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    எனில் ((*func)(&pos) == -1)
	    {
		எனில் (மொத்தம்)
		    திரும்பு FAIL;
		நிறுத்து;
	    }
	    ++மொத்தம்;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    திரும்பு OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    முழு
findpar(
    முழு		*pincl,	    // Return: TRUE if last char is to be included
    முழு		dir,
    long	மொத்தம்,
    முழு		what,
    முழு		both)
{
    linenr_T	curr;
    முழு		did_skip;   // TRUE after separating lines have been skipped
    முழு		first;	    // TRUE on first line
    முழு		posix = (vim_strchr(p_cpo, CPO_PARA) != வெற்று);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    முழு		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    போது (மொத்தம்--)
    {
	did_skip = FALSE;
	க்கு (first = TRUE; ; first = FALSE)
	{
	    எனில் (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    எனில் (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    எனில் (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		நிறுத்து;

#ifdef FEAT_FOLDING
	    எனில் (fold_skipped)
		curr -= dir;
#endif
	    எனில் ((curr += dir) < 1 || curr > curbuf->b_ml.ml_வரி_மொத்தம்)
	    {
		எனில் (மொத்தம்)
		    திரும்பு FALSE;
		curr -= dir;
		நிறுத்து;
	    }
	}
    }
    setpcmark();
    எனில் (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    எனில் (curr == curbuf->b_ml.ml_வரி_மொத்தம் && what != '}' && dir == FORWARD)
    {
	char_u *வரி = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	எனில் ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(வரி, வரி + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    இல்லையெனில்
	curwin->w_cursor.col = 0;
    திரும்பு TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    நிலையான முழு
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    க்கு (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	எனில் (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    நிறுத்து;
	++macro;
	எனில் (macro[0] == NUL)
	    நிறுத்து;
    }
    திரும்பு (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    முழு
startPS(linenr_T lnum, முழு para, முழு both)
{
    char_u	*s;

    s = ml_get(lnum);
    எனில் (*s == para || *s == '\f' || (both && *s == '}'))
	திரும்பு TRUE;
    எனில் (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	திரும்பு TRUE;
    திரும்பு FALSE;
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

நிலையான முழு	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    நிலையான முழு
cls(வெறுமை)
{
    முழு	    c;

    c = gchar_cursor();
    எனில் (c == ' ' || c == '\t' || c == NUL)
	திரும்பு 0;
    எனில் (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	எனில் (enc_dbcs == DBCS_KOR && cls_bigword)
	    திரும்பு 1;

	// process code leading/trailing bytes
	திரும்பு dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    எனில் (enc_utf8)
    {
	c = utf_class(c);
	எனில் (c != 0 && cls_bigword)
	    திரும்பு 1;
	திரும்பு c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    எனில் (cls_bigword)
	திரும்பு 1;

    எனில் (vim_iswordc(c))
	திரும்பு 2;
    திரும்பு 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    முழு
fwd_word(
    long	மொத்தம்,
    முழு		bigword,    // "W", "E" or "B"
    முழு		eol)
{
    முழு		sclass;	    // starting class
    முழு		i;
    முழு		last_வரி;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    போது (--மொத்தம் >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	எனில் (hasFolding(curwin->w_cursor.lnum, வெற்று, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_வரி = (curwin->w_cursor.lnum == curbuf->b_ml.ml_வரி_மொத்தம்);
	i = inc_cursor();
	எனில் (i == -1 || (i >= 1 && last_வரி)) // started at last char in file
	    திரும்பு FAIL;
	எனில் (i >= 1 && eol && மொத்தம் == 0)      // started at last char in line
	    திரும்பு OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	எனில் (sclass != 0)
	    போது (cls() == sclass)
	    {
		i = inc_cursor();
		எனில் (i == -1 || (i >= 1 && eol && மொத்தம் == 0))
		    திரும்பு OK;
	    }

	/*
	 * go to next non-white
	 */
	போது (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    எனில் (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		நிறுத்து;

	    i = inc_cursor();
	    எனில் (i == -1 || (i >= 1 && eol && மொத்தம் == 0))
		திரும்பு OK;
	}
    }
    திரும்பு OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    முழு
bck_word(long மொத்தம், முழு bigword, முழு stop)
{
    முழு		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    போது (--மொத்தம் >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	எனில் (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, வெற்று))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	எனில் (dec_cursor() == -1)		// started at start of file
	    திரும்பு FAIL;

	எனில் (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    போது (cls() == 0)
	    {
		எனில் (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    செல் finished;
		எனில் (dec_cursor() == -1) // hit start of file, stop here
		    திரும்பு OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    எனில் (skip_chars(cls(), BACKWARD))
		திரும்பு OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    திரும்பு OK;
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
    முழு
end_word(
    long	மொத்தம்,
    முழு		bigword,
    முழு		stop,
    முழு		empty)
{
    முழு		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    எனில் (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    போது (--மொத்தம் >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	எனில் (hasFolding(curwin->w_cursor.lnum, வெற்று, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	எனில் (inc_cursor() == -1)
	    திரும்பு FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	எனில் (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    எனில் (skip_chars(sclass, FORWARD))
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    போது (cls() == 0)
	    {
		எனில் (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    செல் finished;
		எனில் (inc_cursor() == -1)	    // hit end of file, stop here
		    திரும்பு FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    எனில் (skip_chars(cls(), FORWARD))
		திரும்பு FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    திரும்பு OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    முழு
bckend_word(
    long	மொத்தம்,
    முழு		bigword,    // TRUE for "B"
    முழு		eol)	    // TRUE: stop at end of line.
{
    முழு		sclass;	    // starting class
    முழு		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    போது (--மொத்தம் >= 0)
    {
	sclass = cls();
	எனில் ((i = dec_cursor()) == -1)
	    திரும்பு FAIL;
	எனில் (eol && i == 1)
	    திரும்பு OK;

	/*
	 * Move backward to before the start of this word.
	 */
	எனில் (sclass != 0)
	{
	    போது (cls() == sclass)
		எனில் ((i = dec_cursor()) == -1 || (eol && i == 1))
		    திரும்பு OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	போது (cls() == 0)
	{
	    எனில் (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		நிறுத்து;
	    எனில் ((i = dec_cursor()) == -1 || (eol && i == 1))
		திரும்பு OK;
	}
    }
    adjust_skipcol();
    திரும்பு OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    நிலையான முழு
skip_chars(முழு cclass, முழு dir)
{
    போது (cls() == cclass)
	எனில் ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    நிலையான வெறுமை
back_in_வரி(வெறுமை)
{
    முழு		sclass;		    // starting class

    sclass = cls();
    க்கு (;;)
    {
	எனில் (curwin->w_cursor.col == 0)	    // stop at start of line
	    நிறுத்து;
	dec_cursor();
	எனில் (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    நிறுத்து;
	}
    }
}

    நிலையான வெறுமை
find_first_blank(pos_T *posp)
{
    முழு	    c;

    போது (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	எனில் (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    நிறுத்து;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    நிலையான வெறுமை
findsent_forward(
    long    மொத்தம்,
    முழு	    at_start_sent)	// cursor is at start of sentence
{
    போது (மொத்தம்--)
    {
	findsent(FORWARD, 1L);
	எனில் (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	எனில் (மொத்தம் == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    முழு
current_word(
    oparg_T	*oap,
    long	மொத்தம்,
    முழு		include,	// TRUE: include word and white space
    முழு		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    முழு		inclusive = TRUE;
    முழு		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    எனில் (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    எனில் (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_வரி();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	எனில் ((cls() == 0) == include)
	{
	    எனில் (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    எனில் (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    இல்லையெனில்
		oneleft();

	    எனில் (include)
		include_white = TRUE;
	}

	எனில் (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	இல்லையெனில்
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--மொத்தம்;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    போது (மொத்தம் > 0)
    {
	inclusive = TRUE;
	எனில் (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    எனில் (decl(&curwin->w_cursor) == -1)
		திரும்பு FAIL;
	    எனில் (include != (cls() != 0))
	    {
		எனில் (bck_word(1L, bigword, TRUE) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		எனில் (bckend_word(1L, bigword, TRUE) == FAIL)
		    திரும்பு FAIL;
		(வெறுமை)incl(&curwin->w_cursor);
	    }
	}
	இல்லையெனில்
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    எனில் (incl(&curwin->w_cursor) == -1)
		திரும்பு FAIL;
	    எனில் (include != (cls() == 0))
	    {
		எனில் (fwd_word(1L, bigword, TRUE) == FAIL && மொத்தம் > 1)
		    திரும்பு FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		எனில் (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    இல்லையெனில்
	    {
		எனில் (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    திரும்பு FAIL;
	    }
	}
	--மொத்தம்;
    }

    எனில் (include_white && (cls() != 0
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
	எனில் (oneleft() == OK)
	{
	    back_in_வரி();
	    எனில் (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		எனில் (VIsual_active)
		    VIsual = curwin->w_cursor;
		இல்லையெனில்
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    எனில் (VIsual_active)
    {
	எனில் (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	எனில் (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    இல்லையெனில்
	oap->inclusive = inclusive;

    திரும்பு OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    முழு
current_sent(oparg_T *oap, long மொத்தம், முழு include)
{
    pos_T	start_pos;
    pos_T	pos;
    முழு		start_blank;
    முழு		c;
    முழு		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    எனில் (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	எனில் (LT_POS(start_pos, VIsual))
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
	    போது (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		எனில் (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    நிறுத்து;
		}
		incl(&pos);
	    }
	    எனில் (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		எனில் (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		இல்லையெனில்
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    எனில் (include)	// "as" gets twice as much as "is"
		மொத்தம் *= 2;
	    போது (மொத்தம்--)
	    {
		எனில் (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		எனில் (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	இல்லையெனில்
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
	    எனில் (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		போது (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    எனில் (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			நிறுத்து;
		    }
		    incl(&pos);
		}
		எனில் (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		இல்லையெனில்		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    எனில் (include)	// "as" gets twice as much as "is"
		மொத்தம் *= 2;
	    findsent_forward(மொத்தம், at_start_sent);
	    எனில் (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	திரும்பு OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    போது (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    எனில் (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    இல்லையெனில்
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    எனில் (include)
	ncount = மொத்தம் * 2;
    இல்லையெனில்
    {
	ncount = மொத்தம்;
	எனில் (start_blank)
	    --ncount;
    }
    எனில் (ncount > 0)
	findsent_forward(ncount, TRUE);
    இல்லையெனில்
	decl(&curwin->w_cursor);

    எனில் (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	எனில் (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    எனில் (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	இல்லையெனில் எனில் (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    எனில் (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	எனில் (EQUAL_POS(start_pos, curwin->w_cursor))
	    செல் extend;
	எனில் (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    இல்லையெனில்
    {
	// include a newline after the sentence, if there is one
	எனில் (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	இல்லையெனில்
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    திரும்பு OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    முழு
current_block(
    oparg_T	*oap,
    long	மொத்தம்,
    முழு		include,	// TRUE == include white space
    முழு		what,		// '(', '{', etc.
    முழு		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = வெற்று;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    முழு		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    எனில் (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	எனில் (what == '{')		// ignore indent
	    போது (inindent(1))
		எனில் (inc_cursor() != 0)
		    நிறுத்து;
	எனில் (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    இல்லையெனில் எனில் (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    இல்லையெனில்
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != வெற்று ? "%M" : "%");
    எனில் ((pos = findmatch(வெற்று, what)) != வெற்று)
    {
	போது (மொத்தம்-- > 0)
	{
	    எனில் ((pos = findmatch(வெற்று, what)) == வெற்று)
		நிறுத்து;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    இல்லையெனில்
    {
	போது (மொத்தம்-- > 0)
	{
	    எனில் ((pos = findmatchlimit(வெற்று, what, FM_FORWARD, 0)) == வெற்று)
		நிறுத்து;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    எனில் (pos == வெற்று || (end_pos = findmatch(வெற்று, other)) == வெற்று)
    {
	curwin->w_cursor = old_pos;
	திரும்பு FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    போது (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	போது (inindent(1))
	{
	    sol = TRUE;
	    எனில் (decl(&curwin->w_cursor) != 0)
		நிறுத்து;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	எனில் (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    திரும்பு FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	எனில் (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    எனில் ((pos = findmatch(வெற்று, what)) == வெற்று)
	    {
		curwin->w_cursor = old_pos;
		திரும்பு FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    எனில் ((end_pos = findmatch(வெற்று, other)) == வெற்று)
	    {
		curwin->w_cursor = old_pos;
		திரும்பு FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	இல்லையெனில்
	    நிறுத்து;
    }

    எனில் (VIsual_active)
    {
	எனில் (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	எனில் (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    இல்லையெனில்
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	எனில் (sol)
	    incl(&curwin->w_cursor);
	இல்லையெனில் எனில் (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	இல்லையெனில்
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    திரும்பு OK;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    நிலையான முழு
in_html_tag(
    முழு		end_tag)
{
    char_u	*வரி = ml_get_curline();
    char_u	*p;
    முழு		c;
    முழு		lc = NUL;
    pos_T	pos;

    எனில் (enc_dbcs)
    {
	char_u	*lp = வெற்று;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	க்கு (p = வரி; p < வரி + curwin->w_cursor.col; MB_PTR_ADV(p))
	    எனில் (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	எனில் (*p != '<')	    // check for '<' under cursor
	{
	    எனில் (lc != '<')
		திரும்பு FALSE;
	    p = lp;
	}
    }
    இல்லையெனில்
    {
	க்கு (p = வரி + curwin->w_cursor.col; p > வரி; )
	{
	    எனில் (*p == '<')	// find '<' under/before cursor
		நிறுத்து;
	    MB_PTR_BACK(வரி, p);
	    எனில் (*p == '>')	// find '>' before cursor
		நிறுத்து;
	}
	எனில் (*p != '<')
	    திரும்பு FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - வரி);

    MB_PTR_ADV(p);
    எனில் (end_tag)
	// check that there is a '/' after the '<'
	திரும்பு *p == '/';

    // check that there is no '/' after the '<'
    எனில் (*p == '/')
	திரும்பு FALSE;

    // check that the matching '>' is not preceded by '/'
    க்கு (;;)
    {
	எனில் (inc(&pos) < 0)
	    திரும்பு FALSE;
	c = *ml_get_pos(&pos);
	எனில் (c == '>')
	    நிறுத்து;
	lc = c;
    }
    திரும்பு lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    முழு
current_tagblock(
    oparg_T	*oap,
    long	மொத்தம்_arg,
    முழு		include)	// TRUE == include white space
{
    long	மொத்தம் = மொத்தம்_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    முழு		len;
    முழு		r;
    முழு		do_include = include;
    முழு		save_p_ws = p_ws;
    முழு		retval = FAIL;
    முழு		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    எனில் (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    எனில் (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	போது (inindent(1))
	    எனில் (inc_cursor() != 0)
		நிறுத்து;

	எனில் (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    போது (*ml_get_cursor() != '>')
		எனில் (inc_cursor() < 0)
		    நிறுத்து;
	}
	இல்லையெனில் எனில் (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    போது (*ml_get_cursor() != '<')
		எனில் (dec_cursor() < 0)
		    நிறுத்து;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    இல்லையெனில் எனில் (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    இல்லையெனில்
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    க்கு (n = 0; n < மொத்தம்; ++n)
    {
	எனில் (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, வெற்று, 0,
						  வெற்று, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    செல் theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    க்கு (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (முழு)(cp - p);
    எனில் (len == 0)
    {
	curwin->w_cursor = old_pos;
	செல் theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    எனில் (spat == வெற்று || epat == வெற்று)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	செல் theend;
    }
    sprintf((எழுத்து *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((எழுத்து *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, வெற்று,
						    0, வெற்று, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    எனில் (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	மொத்தம் = 1;
	curwin->w_cursor = start_pos;
	செல் again;
    }

    எனில் (do_include)
    {
	// Include up to the '>'.
	போது (*ml_get_cursor() != '>')
	    எனில் (inc_cursor() < 0)
		நிறுத்து;
    }
    இல்லையெனில்
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	எனில் (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	இல்லையெனில் எனில் (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    எனில் (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	முழு in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	போது (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    எனில் (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	எனில் (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    மொத்தம் = மொத்தம்_arg;
	    செல் again;
	}
    }

    எனில் (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	எனில் (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	இல்லையெனில் எனில் (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    இல்லையெனில்
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	எனில் (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	இல்லையெனில்
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    திரும்பு retval;
}
#endif

    முழு
current_par(
    oparg_T	*oap,
    long	மொத்தம்,
    முழு		include,	// TRUE == include white space
    முழு		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    முழு		white_in_front;
    முழு		dir;
    முழு		start_is_white;
    முழு		prev_start_is_white;
    முழு		retval = OK;
    முழு		do_white = FALSE;
    முழு		t;
    முழு		i;

    எனில் (type == 'S')	    // not implemented yet
	திரும்பு FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    எனில் (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	எனில் (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	இல்லையெனில்
	    dir = FORWARD;
	க்கு (i = மொத்தம்; --i >= 0; )
	{
	    எனில் (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_வரி_மொத்தம்))
	    {
		retval = FAIL;
		நிறுத்து;
	    }

	    prev_start_is_white = -1;
	    க்கு (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		எனில் (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    நிறுத்து;
		}
		க்கு (;;)
		{
		    எனில் (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_வரி_மொத்தம்))
			நிறுத்து;
		    எனில் (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			நிறுத்து;
		    start_lnum += dir;
		}
		எனில் (!include)
		    நிறுத்து;
		எனில் (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_வரி_மொத்தம்))
		    நிறுத்து;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	திரும்பு retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    போது (start_lnum > 1)
    {
	எனில் (white_in_front)	    // stop at first white line
	{
	    எனில் (!linewhite(start_lnum - 1))
		நிறுத்து;
	}
	இல்லையெனில்		// stop at first non-white line of start of paragraph
	{
	    எனில் (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		நிறுத்து;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    போது (end_lnum <= curbuf->b_ml.ml_வரி_மொத்தம் && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = மொத்தம்;
    எனில் (!include && white_in_front)
	--i;
    போது (i--)
    {
	எனில் (end_lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
	    திரும்பு FAIL;

	எனில் (!include)
	    do_white = linewhite(end_lnum + 1);

	எனில் (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    போது (end_lnum < curbuf->b_ml.ml_வரி_மொத்தம்
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	எனில் (i == 0 && white_in_front && include)
	    நிறுத்து;

	/*
	 * skip to end of white lines after paragraph
	 */
	எனில் (include || do_white)
	    போது (end_lnum < curbuf->b_ml.ml_வரி_மொத்தம்
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    எனில் (!white_in_front && !linewhite(end_lnum) && include)
	போது (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    எனில் (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	எனில் (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    செல் extend;
	எனில் (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    இல்லையெனில்
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    திரும்பு OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    நிலையான முழு
find_next_quote(
    char_u	*வரி,
    முழு		col,
    முழு		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    முழு		c;

    க்கு (;;)
    {
	c = வரி[col];
	எனில் (c == NUL)
	    திரும்பு -1;
	இல்லையெனில் எனில் (escape != வெற்று && vim_strchr(escape, c))
	{
	    ++col;
	    எனில் (வரி[col] == NUL)
		திரும்பு -1;
	}
	இல்லையெனில் எனில் (c == quotechar)
	    நிறுத்து;
	எனில் (has_mbyte)
	    col += (*mb_ptr2len)(வரி + col);
	இல்லையெனில்
	    ++col;
    }
    திரும்பு col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    நிலையான முழு
find_prev_quote(
    char_u	*வரி,
    முழு		col_start,
    முழு		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    முழு		n;

    போது (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(வரி, வரி + col_start);
	n = 0;
	எனில் (escape != வெற்று)
	    போது (col_start - n > 0 && vim_strchr(escape,
					     வரி[col_start - n - 1]) != வெற்று)
	    ++n;
	எனில் (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	இல்லையெனில் எனில் (வரி[col_start] == quotechar)
	    நிறுத்து;
    }
    திரும்பு col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    முழு
current_quote(
    oparg_T	*oap,
    long	மொத்தம்,
    முழு		include,	// TRUE == include quote char
    முழு		quotechar)	// Quote character
{
    char_u	*வரி = ml_get_curline();
    முழு		col_end;
    முழு		col_start = curwin->w_cursor.col;
    முழு		inclusive = FALSE;
    முழு		vis_empty = TRUE;	// Visual selection <= 1 char
    முழு		vis_bef_curs = FALSE;	// Visual starts before cursor
    முழு		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    முழு		inside_quotes = FALSE;	// Looks like "i'" done before
    முழு		selected_quote = FALSE;	// Has quote inside selection
    முழு		i;
    முழு		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    எனில் (VIsual_active)
    {
	// this only works within one line
	எனில் (VIsual.lnum != curwin->w_cursor.lnum)
	    திரும்பு FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	எனில் (*p_sel == 'e')
	{
	    எனில் (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    இல்லையெனில் எனில் (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    எனில் (!vis_bef_curs && !vis_empty)
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

    எனில் (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	எனில் (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& வரி[VIsual.col - 1] == quotechar
			&& வரி[curwin->w_cursor.col] != NUL
			&& வரி[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	இல்லையெனில்
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& வரி[curwin->w_cursor.col - 1] == quotechar
			&& வரி[VIsual.col] != NUL
			&& வரி[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	போது (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    எனில் (வரி[i] == NUL)
		நிறுத்து;
	    எனில் (வரி[i++] == quotechar)
	    {
		selected_quote = TRUE;
		நிறுத்து;
	    }
	}
    }

    எனில் (!vis_empty && வரி[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	எனில் (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(வரி, col_start + 1, quotechar, வெற்று);
	    எனில் (col_start < 0)
		செல் abort_தேடல்;
	    col_end = find_next_quote(வரி, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    எனில் (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	இல்லையெனில்
	{
	    col_end = find_prev_quote(வரி, col_start, quotechar, வெற்று);
	    எனில் (வரி[col_end] != quotechar)
		செல் abort_தேடல்;
	    col_start = find_prev_quote(வரி, col_end, quotechar,
							      curbuf->b_p_qe);
	    எனில் (வரி[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    இல்லையெனில்

    எனில் (வரி[col_start] == quotechar || !vis_empty)
    {
	முழு	first_col = col_start;

	எனில் (!vis_empty)
	{
	    எனில் (vis_bef_curs)
		first_col = find_next_quote(வரி, col_start, quotechar, வெற்று);
	    இல்லையெனில்
		first_col = find_prev_quote(வரி, col_start, quotechar, வெற்று);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	க்கு (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(வரி, col_start, quotechar, வெற்று);
	    எனில் (col_start < 0 || col_start > first_col)
		செல் abort_தேடல்;
	    // Find close quote character.
	    col_end = find_next_quote(வரி, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    எனில் (col_end < 0)
		செல் abort_தேடல்;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    எனில் (col_start <= first_col && first_col <= col_end)
		நிறுத்து;
	    col_start = col_end + 1;
	}
    }
    இல்லையெனில்
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(வரி, col_start, quotechar, curbuf->b_p_qe);
	எனில் (வரி[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(வரி, col_start, quotechar, வெற்று);
	    எனில் (col_start < 0)
		செல் abort_தேடல்;
	}

	// Find close quote character.
	col_end = find_next_quote(வரி, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	எனில் (col_end < 0)
	    செல் abort_தேடல்;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    எனில் (include)
    {
	எனில் (VIM_ISWHITE(வரி[col_end + 1]))
	    போது (VIM_ISWHITE(வரி[col_end + 1]))
		++col_end;
	இல்லையெனில்
	    போது (col_start > 0 && VIM_ISWHITE(வரி[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    எனில் (!include && மொத்தம் < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    எனில் (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	எனில் (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (வரி[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| வரி[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    இல்லையெனில்
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    எனில் ((include || மொத்தம் > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    எனில் (VIsual_active)
    {
	எனில் (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    எனில் (*p_sel != 'e')
		dec_cursor();
	}
	இல்லையெனில்
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    எனில் (inside_quotes
		    || (!selected_quote
			&& வரி[VIsual.col] != quotechar
			&& (வரி[VIsual.col] == NUL
			    || வரி[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	எனில் (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    இல்லையெனில்
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    திரும்பு OK;

abort_தேடல்:
    எனில் (VIsual_active && *p_sel == 'e')
    {
	எனில் (did_exclusive_adj)
	    inc_cursor();
	எனில் (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    திரும்பு FALSE;
}
