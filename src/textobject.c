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
#अन्तर्भाव "vim.h"

स्थिर पूर्णाङ्क cls(शून्य);
स्थिर पूर्णाङ्क skip_chars(पूर्णाङ्क, पूर्णाङ्क);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    पूर्णाङ्क
findsent(पूर्णाङ्क dir, long गणनम्)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    पूर्णाङ्क		c;
    पूर्णाङ्क		(*func)(pos_T *);
    पूर्णाङ्क		startlnum;
    पूर्णाङ्क		noskip = FALSE;	    // do not skip blanks
    पूर्णाङ्क		cpo_J;
    पूर्णाङ्क		found_dot;

    pos = curwin->w_cursor;
    जइ (dir == FORWARD)
	func = incl;
    अण्णहा
	func = decl;

    जाव (गणनम्--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	जइ (gchar_pos(&pos) == NUL)
	{
	    करेहि
		जइ ((*func)(&pos) == -1)
		    भङ्ग;
	    जाव (gchar_pos(&pos) == NUL);
	    जइ (dir == FORWARD)
		गच्छ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	अण्णहा जइ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    जइ (pos.lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		प्रत्यागम FAIL;
	    ++pos.lnum;
	    गच्छ found;
	}
	अण्णहा जइ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	जाव (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != रिक्त)
	{
	    tpos = pos;
	    जइ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		भङ्ग;

	    जइ (found_dot)
		भङ्ग;
	    जइ (vim_strchr((char_u *) ".!?", c) != रिक्त)
		found_dot = TRUE;

	    जइ (vim_strchr((char_u *) ")]\"'", c) != रिक्त
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == रिक्त)
		भङ्ग;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != रिक्त;

	प्रति (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    जइ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		जइ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		भङ्ग;
	    }
	    जइ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		करेहि
		    जइ ((c = inc(&tpos)) == -1)
			भङ्ग;
		जाव (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= रिक्त);
		जइ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    जइ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    भङ्ग;
		}
	    }
	    जइ ((*func)(&pos) == -1)
	    {
		जइ (गणनम्)
		    प्रत्यागम FAIL;
		noskip = TRUE;
		भङ्ग;
	    }
	}
found:
	    // skip white space
	जाव (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    जइ (incl(&pos) == -1)
		भङ्ग;

	जइ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    जइ ((*func)(&pos) == -1)
	    {
		जइ (गणनम्)
		    प्रत्यागम FAIL;
		भङ्ग;
	    }
	    ++गणनम्;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    प्रत्यागम OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    पूर्णाङ्क
findpar(
    पूर्णाङ्क		*pincl,	    // Return: TRUE if last char is to be included
    पूर्णाङ्क		dir,
    long	गणनम्,
    पूर्णाङ्क		what,
    पूर्णाङ्क		both)
{
    linenr_T	curr;
    पूर्णाङ्क		did_skip;   // TRUE after separating lines have been skipped
    पूर्णाङ्क		first;	    // TRUE on first line
    पूर्णाङ्क		posix = (vim_strchr(p_cpo, CPO_PARA) != रिक्त);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    पूर्णाङ्क		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    जाव (गणनम्--)
    {
	did_skip = FALSE;
	प्रति (first = TRUE; ; first = FALSE)
	{
	    जइ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    जइ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    जइ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		भङ्ग;

#ifdef FEAT_FOLDING
	    जइ (fold_skipped)
		curr -= dir;
#endif
	    जइ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    {
		जइ (गणनम्)
		    प्रत्यागम FALSE;
		curr -= dir;
		भङ्ग;
	    }
	}
    }
    setpcmark();
    जइ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    जइ (curr == curbuf->b_ml.ml_पङ्क्तिः_गणनम् && what != '}' && dir == FORWARD)
    {
	char_u *पङ्क्तिः = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	जइ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(पङ्क्तिः, पङ्क्तिः + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    अण्णहा
	curwin->w_cursor.col = 0;
    प्रत्यागम TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    स्थिर पूर्णाङ्क
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    प्रति (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	जइ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    भङ्ग;
	++macro;
	जइ (macro[0] == NUL)
	    भङ्ग;
    }
    प्रत्यागम (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    पूर्णाङ्क
startPS(linenr_T lnum, पूर्णाङ्क para, पूर्णाङ्क both)
{
    char_u	*s;

    s = ml_get(lnum);
    जइ (*s == para || *s == '\f' || (both && *s == '}'))
	प्रत्यागम TRUE;
    जइ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
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

स्थिर पूर्णाङ्क	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    स्थिर पूर्णाङ्क
cls(शून्य)
{
    पूर्णाङ्क	    c;

    c = gchar_cursor();
    जइ (c == ' ' || c == '\t' || c == NUL)
	प्रत्यागम 0;
    जइ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	जइ (enc_dbcs == DBCS_KOR && cls_bigword)
	    प्रत्यागम 1;

	// process code leading/trailing bytes
	प्रत्यागम dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    जइ (enc_utf8)
    {
	c = utf_class(c);
	जइ (c != 0 && cls_bigword)
	    प्रत्यागम 1;
	प्रत्यागम c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    जइ (cls_bigword)
	प्रत्यागम 1;

    जइ (vim_iswordc(c))
	प्रत्यागम 2;
    प्रत्यागम 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    पूर्णाङ्क
fwd_word(
    long	गणनम्,
    पूर्णाङ्क		bigword,    // "W", "E" or "B"
    पूर्णाङ्क		eol)
{
    पूर्णाङ्क		sclass;	    // starting class
    पूर्णाङ्क		i;
    पूर्णाङ्क		last_पङ्क्तिः;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जाव (--गणनम् >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	जइ (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_पङ्क्तिः = (curwin->w_cursor.lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्);
	i = inc_cursor();
	जइ (i == -1 || (i >= 1 && last_पङ्क्तिः)) // started at last char in file
	    प्रत्यागम FAIL;
	जइ (i >= 1 && eol && गणनम् == 0)      // started at last char in line
	    प्रत्यागम OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	जइ (sclass != 0)
	    जाव (cls() == sclass)
	    {
		i = inc_cursor();
		जइ (i == -1 || (i >= 1 && eol && गणनम् == 0))
		    प्रत्यागम OK;
	    }

	/*
	 * go to next non-white
	 */
	जाव (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    जइ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		भङ्ग;

	    i = inc_cursor();
	    जइ (i == -1 || (i >= 1 && eol && गणनम् == 0))
		प्रत्यागम OK;
	}
    }
    प्रत्यागम OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    पूर्णाङ्क
bck_word(long गणनम्, पूर्णाङ्क bigword, पूर्णाङ्क stop)
{
    पूर्णाङ्क		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जाव (--गणनम् >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	जइ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, रिक्त))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	जइ (dec_cursor() == -1)		// started at start of file
	    प्रत्यागम FAIL;

	जइ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    जाव (cls() == 0)
	    {
		जइ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    गच्छ finished;
		जइ (dec_cursor() == -1) // hit start of file, stop here
		    प्रत्यागम OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    जइ (skip_chars(cls(), BACKWARD))
		प्रत्यागम OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    प्रत्यागम OK;
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
    पूर्णाङ्क
end_word(
    long	गणनम्,
    पूर्णाङ्क		bigword,
    पूर्णाङ्क		stop,
    पूर्णाङ्क		empty)
{
    पूर्णाङ्क		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    जइ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    जाव (--गणनम् >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	जइ (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	जइ (inc_cursor() == -1)
	    प्रत्यागम FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	जइ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    जइ (skip_chars(sclass, FORWARD))
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    जाव (cls() == 0)
	    {
		जइ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    गच्छ finished;
		जइ (inc_cursor() == -1)	    // hit end of file, stop here
		    प्रत्यागम FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    जइ (skip_chars(cls(), FORWARD))
		प्रत्यागम FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    प्रत्यागम OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    पूर्णाङ्क
bckend_word(
    long	गणनम्,
    पूर्णाङ्क		bigword,    // TRUE for "B"
    पूर्णाङ्क		eol)	    // TRUE: stop at end of line.
{
    पूर्णाङ्क		sclass;	    // starting class
    पूर्णाङ्क		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जाव (--गणनम् >= 0)
    {
	sclass = cls();
	जइ ((i = dec_cursor()) == -1)
	    प्रत्यागम FAIL;
	जइ (eol && i == 1)
	    प्रत्यागम OK;

	/*
	 * Move backward to before the start of this word.
	 */
	जइ (sclass != 0)
	{
	    जाव (cls() == sclass)
		जइ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    प्रत्यागम OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	जाव (cls() == 0)
	{
	    जइ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		भङ्ग;
	    जइ ((i = dec_cursor()) == -1 || (eol && i == 1))
		प्रत्यागम OK;
	}
    }
    adjust_skipcol();
    प्रत्यागम OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    स्थिर पूर्णाङ्क
skip_chars(पूर्णाङ्क cclass, पूर्णाङ्क dir)
{
    जाव (cls() == cclass)
	जइ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    स्थिर शून्य
back_in_पङ्क्तिः(शून्य)
{
    पूर्णाङ्क		sclass;		    // starting class

    sclass = cls();
    प्रति (;;)
    {
	जइ (curwin->w_cursor.col == 0)	    // stop at start of line
	    भङ्ग;
	dec_cursor();
	जइ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    भङ्ग;
	}
    }
}

    स्थिर शून्य
find_first_blank(pos_T *posp)
{
    पूर्णाङ्क	    c;

    जाव (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	जइ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    भङ्ग;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    स्थिर शून्य
findsent_forward(
    long    गणनम्,
    पूर्णाङ्क	    at_start_sent)	// cursor is at start of sentence
{
    जाव (गणनम्--)
    {
	findsent(FORWARD, 1L);
	जइ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	जइ (गणनम् == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    पूर्णाङ्क
current_word(
    oparg_T	*oap,
    long	गणनम्,
    पूर्णाङ्क		include,	// TRUE: include word and white space
    पूर्णाङ्क		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    पूर्णाङ्क		inclusive = TRUE;
    पूर्णाङ्क		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    जइ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    जइ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_पङ्क्तिः();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	जइ ((cls() == 0) == include)
	{
	    जइ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    जइ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    अण्णहा
		oneleft();

	    जइ (include)
		include_white = TRUE;
	}

	जइ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	अण्णहा
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--गणनम्;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    जाव (गणनम् > 0)
    {
	inclusive = TRUE;
	जइ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    जइ (decl(&curwin->w_cursor) == -1)
		प्रत्यागम FAIL;
	    जइ (include != (cls() != 0))
	    {
		जइ (bck_word(1L, bigword, TRUE) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा
	    {
		जइ (bckend_word(1L, bigword, TRUE) == FAIL)
		    प्रत्यागम FAIL;
		(शून्य)incl(&curwin->w_cursor);
	    }
	}
	अण्णहा
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    जइ (incl(&curwin->w_cursor) == -1)
		प्रत्यागम FAIL;
	    जइ (include != (cls() == 0))
	    {
		जइ (fwd_word(1L, bigword, TRUE) == FAIL && गणनम् > 1)
		    प्रत्यागम FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		जइ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    अण्णहा
	    {
		जइ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}
	--गणनम्;
    }

    जइ (include_white && (cls() != 0
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
	जइ (oneleft() == OK)
	{
	    back_in_पङ्क्तिः();
	    जइ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		जइ (VIsual_active)
		    VIsual = curwin->w_cursor;
		अण्णहा
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    जइ (VIsual_active)
    {
	जइ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	जइ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    अण्णहा
	oap->inclusive = inclusive;

    प्रत्यागम OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    पूर्णाङ्क
current_sent(oparg_T *oap, long गणनम्, पूर्णाङ्क include)
{
    pos_T	start_pos;
    pos_T	pos;
    पूर्णाङ्क		start_blank;
    पूर्णाङ्क		c;
    पूर्णाङ्क		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    जइ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	जइ (LT_POS(start_pos, VIsual))
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
	    जाव (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		जइ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    भङ्ग;
		}
		incl(&pos);
	    }
	    जइ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		जइ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		अण्णहा
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    जइ (include)	// "as" gets twice as much as "is"
		गणनम् *= 2;
	    जाव (गणनम्--)
	    {
		जइ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		जइ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	अण्णहा
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
	    जइ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		जाव (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    जइ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			भङ्ग;
		    }
		    incl(&pos);
		}
		जइ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		अण्णहा		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    जइ (include)	// "as" gets twice as much as "is"
		गणनम् *= 2;
	    findsent_forward(गणनम्, at_start_sent);
	    जइ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	प्रत्यागम OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    जाव (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    जइ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    अण्णहा
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    जइ (include)
	ncount = गणनम् * 2;
    अण्णहा
    {
	ncount = गणनम्;
	जइ (start_blank)
	    --ncount;
    }
    जइ (ncount > 0)
	findsent_forward(ncount, TRUE);
    अण्णहा
	decl(&curwin->w_cursor);

    जइ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	जइ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    जइ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	अण्णहा जइ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    जइ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	जइ (EQUAL_POS(start_pos, curwin->w_cursor))
	    गच्छ extend;
	जइ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    अण्णहा
    {
	// include a newline after the sentence, if there is one
	जइ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	अण्णहा
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    प्रत्यागम OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    पूर्णाङ्क
current_block(
    oparg_T	*oap,
    long	गणनम्,
    पूर्णाङ्क		include,	// TRUE == include white space
    पूर्णाङ्क		what,		// '(', '{', etc.
    पूर्णाङ्क		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = रिक्त;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    पूर्णाङ्क		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    जइ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	जइ (what == '{')		// ignore indent
	    जाव (inindent(1))
		जइ (inc_cursor() != 0)
		    भङ्ग;
	जइ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    अण्णहा जइ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    अण्णहा
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != रिक्त ? "%M" : "%");
    जइ ((pos = findmatch(रिक्त, what)) != रिक्त)
    {
	जाव (गणनम्-- > 0)
	{
	    जइ ((pos = findmatch(रिक्त, what)) == रिक्त)
		भङ्ग;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    अण्णहा
    {
	जाव (गणनम्-- > 0)
	{
	    जइ ((pos = findmatchlimit(रिक्त, what, FM_FORWARD, 0)) == रिक्त)
		भङ्ग;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    जइ (pos == रिक्त || (end_pos = findmatch(रिक्त, other)) == रिक्त)
    {
	curwin->w_cursor = old_pos;
	प्रत्यागम FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    जाव (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	जाव (inindent(1))
	{
	    sol = TRUE;
	    जइ (decl(&curwin->w_cursor) != 0)
		भङ्ग;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	जइ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    प्रत्यागम FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	जइ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    जइ ((pos = findmatch(रिक्त, what)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		प्रत्यागम FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    जइ ((end_pos = findmatch(रिक्त, other)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		प्रत्यागम FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	अण्णहा
	    भङ्ग;
    }

    जइ (VIsual_active)
    {
	जइ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	जइ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अण्णहा
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	जइ (sol)
	    incl(&curwin->w_cursor);
	अण्णहा जइ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	अण्णहा
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    प्रत्यागम OK;
}

#जइ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    स्थिर पूर्णाङ्क
in_html_tag(
    पूर्णाङ्क		end_tag)
{
    char_u	*पङ्क्तिः = ml_get_curline();
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		lc = NUL;
    pos_T	pos;

    जइ (enc_dbcs)
    {
	char_u	*lp = रिक्त;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	प्रति (p = पङ्क्तिः; p < पङ्क्तिः + curwin->w_cursor.col; MB_PTR_ADV(p))
	    जइ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	जइ (*p != '<')	    // check for '<' under cursor
	{
	    जइ (lc != '<')
		प्रत्यागम FALSE;
	    p = lp;
	}
    }
    अण्णहा
    {
	प्रति (p = पङ्क्तिः + curwin->w_cursor.col; p > पङ्क्तिः; )
	{
	    जइ (*p == '<')	// find '<' under/before cursor
		भङ्ग;
	    MB_PTR_BACK(पङ्क्तिः, p);
	    जइ (*p == '>')	// find '>' before cursor
		भङ्ग;
	}
	जइ (*p != '<')
	    प्रत्यागम FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - पङ्क्तिः);

    MB_PTR_ADV(p);
    जइ (end_tag)
	// check that there is a '/' after the '<'
	प्रत्यागम *p == '/';

    // check that there is no '/' after the '<'
    जइ (*p == '/')
	प्रत्यागम FALSE;

    // check that the matching '>' is not preceded by '/'
    प्रति (;;)
    {
	जइ (inc(&pos) < 0)
	    प्रत्यागम FALSE;
	c = *ml_get_pos(&pos);
	जइ (c == '>')
	    भङ्ग;
	lc = c;
    }
    प्रत्यागम lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    पूर्णाङ्क
current_tagblock(
    oparg_T	*oap,
    long	गणनम्_arg,
    पूर्णाङ्क		include)	// TRUE == include white space
{
    long	गणनम् = गणनम्_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    पूर्णाङ्क		len;
    पूर्णाङ्क		r;
    पूर्णाङ्क		do_include = include;
    पूर्णाङ्क		save_p_ws = p_ws;
    पूर्णाङ्क		retval = FAIL;
    पूर्णाङ्क		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    जइ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    जइ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	जाव (inindent(1))
	    जइ (inc_cursor() != 0)
		भङ्ग;

	जइ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    जाव (*ml_get_cursor() != '>')
		जइ (inc_cursor() < 0)
		    भङ्ग;
	}
	अण्णहा जइ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    जाव (*ml_get_cursor() != '<')
		जइ (dec_cursor() < 0)
		    भङ्ग;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    अण्णहा जइ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    अण्णहा
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    प्रति (n = 0; n < गणनम्; ++n)
    {
	जइ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, रिक्त, 0,
						  रिक्त, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    गच्छ theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    प्रति (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (पूर्णाङ्क)(cp - p);
    जइ (len == 0)
    {
	curwin->w_cursor = old_pos;
	गच्छ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    जइ (spat == रिक्त || epat == रिक्त)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	गच्छ theend;
    }
    sprintf((वर्ण *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((वर्ण *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, रिक्त,
						    0, रिक्त, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    जइ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	गणनम् = 1;
	curwin->w_cursor = start_pos;
	गच्छ again;
    }

    जइ (do_include)
    {
	// Include up to the '>'.
	जाव (*ml_get_cursor() != '>')
	    जइ (inc_cursor() < 0)
		भङ्ग;
    }
    अण्णहा
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	जइ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	अण्णहा जइ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    जइ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	पूर्णाङ्क in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	जाव (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    जइ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		भङ्ग;
	    }
	    अण्णहा जइ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	जइ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    गणनम् = गणनम्_arg;
	    गच्छ again;
	}
    }

    जइ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	जइ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	अण्णहा जइ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अण्णहा
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	जइ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	अण्णहा
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    प्रत्यागम retval;
}
#endif

    पूर्णाङ्क
current_par(
    oparg_T	*oap,
    long	गणनम्,
    पूर्णाङ्क		include,	// TRUE == include white space
    पूर्णाङ्क		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    पूर्णाङ्क		white_in_front;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		start_is_white;
    पूर्णाङ्क		prev_start_is_white;
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		do_white = FALSE;
    पूर्णाङ्क		t;
    पूर्णाङ्क		i;

    जइ (type == 'S')	    // not implemented yet
	प्रत्यागम FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    जइ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	जइ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	अण्णहा
	    dir = FORWARD;
	प्रति (i = गणनम्; --i >= 0; )
	{
	    जइ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_पङ्क्तिः_गणनम्))
	    {
		retval = FAIL;
		भङ्ग;
	    }

	    prev_start_is_white = -1;
	    प्रति (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		जइ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    भङ्ग;
		}
		प्रति (;;)
		{
		    जइ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_पङ्क्तिः_गणनम्))
			भङ्ग;
		    जइ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			भङ्ग;
		    start_lnum += dir;
		}
		जइ (!include)
		    भङ्ग;
		जइ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_पङ्क्तिः_गणनम्))
		    भङ्ग;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	प्रत्यागम retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    जाव (start_lnum > 1)
    {
	जइ (white_in_front)	    // stop at first white line
	{
	    जइ (!linewhite(start_lnum - 1))
		भङ्ग;
	}
	अण्णहा		// stop at first non-white line of start of paragraph
	{
	    जइ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		भङ्ग;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    जाव (end_lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम् && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = गणनम्;
    जइ (!include && white_in_front)
	--i;
    जाव (i--)
    {
	जइ (end_lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    प्रत्यागम FAIL;

	जइ (!include)
	    do_white = linewhite(end_lnum + 1);

	जइ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    जाव (end_lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	जइ (i == 0 && white_in_front && include)
	    भङ्ग;

	/*
	 * skip to end of white lines after paragraph
	 */
	जइ (include || do_white)
	    जाव (end_lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    जइ (!white_in_front && !linewhite(end_lnum) && include)
	जाव (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    जइ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	जइ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    गच्छ extend;
	जइ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अण्णहा
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    प्रत्यागम OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    स्थिर पूर्णाङ्क
find_next_quote(
    char_u	*पङ्क्तिः,
    पूर्णाङ्क		col,
    पूर्णाङ्क		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णाङ्क		c;

    प्रति (;;)
    {
	c = पङ्क्तिः[col];
	जइ (c == NUL)
	    प्रत्यागम -1;
	अण्णहा जइ (escape != रिक्त && vim_strchr(escape, c))
	{
	    ++col;
	    जइ (पङ्क्तिः[col] == NUL)
		प्रत्यागम -1;
	}
	अण्णहा जइ (c == quotechar)
	    भङ्ग;
	जइ (has_mbyte)
	    col += (*mb_ptr2len)(पङ्क्तिः + col);
	अण्णहा
	    ++col;
    }
    प्रत्यागम col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    स्थिर पूर्णाङ्क
find_prev_quote(
    char_u	*पङ्क्तिः,
    पूर्णाङ्क		col_start,
    पूर्णाङ्क		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णाङ्क		n;

    जाव (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(पङ्क्तिः, पङ्क्तिः + col_start);
	n = 0;
	जइ (escape != रिक्त)
	    जाव (col_start - n > 0 && vim_strchr(escape,
					     पङ्क्तिः[col_start - n - 1]) != रिक्त)
	    ++n;
	जइ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	अण्णहा जइ (पङ्क्तिः[col_start] == quotechar)
	    भङ्ग;
    }
    प्रत्यागम col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    पूर्णाङ्क
current_quote(
    oparg_T	*oap,
    long	गणनम्,
    पूर्णाङ्क		include,	// TRUE == include quote char
    पूर्णाङ्क		quotechar)	// Quote character
{
    char_u	*पङ्क्तिः = ml_get_curline();
    पूर्णाङ्क		col_end;
    पूर्णाङ्क		col_start = curwin->w_cursor.col;
    पूर्णाङ्क		inclusive = FALSE;
    पूर्णाङ्क		vis_empty = TRUE;	// Visual selection <= 1 char
    पूर्णाङ्क		vis_bef_curs = FALSE;	// Visual starts before cursor
    पूर्णाङ्क		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    पूर्णाङ्क		inside_quotes = FALSE;	// Looks like "i'" done before
    पूर्णाङ्क		selected_quote = FALSE;	// Has quote inside selection
    पूर्णाङ्क		i;
    पूर्णाङ्क		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    जइ (VIsual_active)
    {
	// this only works within one line
	जइ (VIsual.lnum != curwin->w_cursor.lnum)
	    प्रत्यागम FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	जइ (*p_sel == 'e')
	{
	    जइ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    अण्णहा जइ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    जइ (!vis_bef_curs && !vis_empty)
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

    जइ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	जइ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& पङ्क्तिः[VIsual.col - 1] == quotechar
			&& पङ्क्तिः[curwin->w_cursor.col] != NUL
			&& पङ्क्तिः[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	अण्णहा
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& पङ्क्तिः[curwin->w_cursor.col - 1] == quotechar
			&& पङ्क्तिः[VIsual.col] != NUL
			&& पङ्क्तिः[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	जाव (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    जइ (पङ्क्तिः[i] == NUL)
		भङ्ग;
	    जइ (पङ्क्तिः[i++] == quotechar)
	    {
		selected_quote = TRUE;
		भङ्ग;
	    }
	}
    }

    जइ (!vis_empty && पङ्क्तिः[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	जइ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(पङ्क्तिः, col_start + 1, quotechar, रिक्त);
	    जइ (col_start < 0)
		गच्छ abort_अन्वेषणम्;
	    col_end = find_next_quote(पङ्क्तिः, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    जइ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	अण्णहा
	{
	    col_end = find_prev_quote(पङ्क्तिः, col_start, quotechar, रिक्त);
	    जइ (पङ्क्तिः[col_end] != quotechar)
		गच्छ abort_अन्वेषणम्;
	    col_start = find_prev_quote(पङ्क्तिः, col_end, quotechar,
							      curbuf->b_p_qe);
	    जइ (पङ्क्तिः[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    अण्णहा

    जइ (पङ्क्तिः[col_start] == quotechar || !vis_empty)
    {
	पूर्णाङ्क	first_col = col_start;

	जइ (!vis_empty)
	{
	    जइ (vis_bef_curs)
		first_col = find_next_quote(पङ्क्तिः, col_start, quotechar, रिक्त);
	    अण्णहा
		first_col = find_prev_quote(पङ्क्तिः, col_start, quotechar, रिक्त);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	प्रति (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(पङ्क्तिः, col_start, quotechar, रिक्त);
	    जइ (col_start < 0 || col_start > first_col)
		गच्छ abort_अन्वेषणम्;
	    // Find close quote character.
	    col_end = find_next_quote(पङ्क्तिः, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    जइ (col_end < 0)
		गच्छ abort_अन्वेषणम्;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    जइ (col_start <= first_col && first_col <= col_end)
		भङ्ग;
	    col_start = col_end + 1;
	}
    }
    अण्णहा
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(पङ्क्तिः, col_start, quotechar, curbuf->b_p_qe);
	जइ (पङ्क्तिः[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(पङ्क्तिः, col_start, quotechar, रिक्त);
	    जइ (col_start < 0)
		गच्छ abort_अन्वेषणम्;
	}

	// Find close quote character.
	col_end = find_next_quote(पङ्क्तिः, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	जइ (col_end < 0)
	    गच्छ abort_अन्वेषणम्;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    जइ (include)
    {
	जइ (VIM_ISWHITE(पङ्क्तिः[col_end + 1]))
	    जाव (VIM_ISWHITE(पङ्क्तिः[col_end + 1]))
		++col_end;
	अण्णहा
	    जाव (col_start > 0 && VIM_ISWHITE(पङ्क्तिः[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    जइ (!include && गणनम् < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    जइ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	जइ (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (पङ्क्तिः[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| पङ्क्तिः[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    अण्णहा
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    जइ ((include || गणनम् > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    जइ (VIsual_active)
    {
	जइ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    जइ (*p_sel != 'e')
		dec_cursor();
	}
	अण्णहा
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    जइ (inside_quotes
		    || (!selected_quote
			&& पङ्क्तिः[VIsual.col] != quotechar
			&& (पङ्क्तिः[VIsual.col] == NUL
			    || पङ्क्तिः[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	जइ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    अण्णहा
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    प्रत्यागम OK;

abort_अन्वेषणम्:
    जइ (VIsual_active && *p_sel == 'e')
    {
	जइ (did_exclusive_adj)
	    inc_cursor();
	जइ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    प्रत्यागम FALSE;
}
