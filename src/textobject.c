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

स्थिर पूर्णांक cls(शून्य);
स्थिर पूर्णांक skip_chars(पूर्णांक, पूर्णांक);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    पूर्णांक
findsent(पूर्णांक dir, दीर्घ मोजणी)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    पूर्णांक		c;
    पूर्णांक		(*func)(pos_T *);
    पूर्णांक		startlnum;
    पूर्णांक		noskip = FALSE;	    // do not skip blanks
    पूर्णांक		cpo_J;
    पूर्णांक		found_dot;

    pos = curwin->w_cursor;
    जर (dir == FORWARD)
	func = incl;
    अन्यथा
	func = decl;

    पर्यंत (मोजणी--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	जर (gchar_pos(&pos) == NUL)
	{
	    करा
		जर ((*func)(&pos) == -1)
		    थांबा;
	    पर्यंत (gchar_pos(&pos) == NUL);
	    जर (dir == FORWARD)
		जा found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	अन्यथा जर (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    जर (pos.lnum == curbuf->b_ml.ml_ओळ_मोजणी)
		परत FAIL;
	    ++pos.lnum;
	    जा found;
	}
	अन्यथा जर (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	पर्यंत (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != रिक्त)
	{
	    tpos = pos;
	    जर (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		थांबा;

	    जर (found_dot)
		थांबा;
	    जर (vim_strchr((char_u *) ".!?", c) != रिक्त)
		found_dot = TRUE;

	    जर (vim_strchr((char_u *) ")]\"'", c) != रिक्त
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == रिक्त)
		थांबा;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != रिक्त;

	साठी (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    जर (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		जर (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		थांबा;
	    }
	    जर (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		करा
		    जर ((c = inc(&tpos)) == -1)
			थांबा;
		पर्यंत (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= रिक्त);
		जर (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    जर (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    थांबा;
		}
	    }
	    जर ((*func)(&pos) == -1)
	    {
		जर (मोजणी)
		    परत FAIL;
		noskip = TRUE;
		थांबा;
	    }
	}
found:
	    // skip white space
	पर्यंत (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    जर (incl(&pos) == -1)
		थांबा;

	जर (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    जर ((*func)(&pos) == -1)
	    {
		जर (मोजणी)
		    परत FAIL;
		थांबा;
	    }
	    ++मोजणी;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    परत OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    पूर्णांक
findpar(
    पूर्णांक		*pincl,	    // Return: TRUE if last char is to be included
    पूर्णांक		dir,
    दीर्घ	मोजणी,
    पूर्णांक		what,
    पूर्णांक		both)
{
    linenr_T	curr;
    पूर्णांक		did_skip;   // TRUE after separating lines have been skipped
    पूर्णांक		first;	    // TRUE on first line
    पूर्णांक		posix = (vim_strchr(p_cpo, CPO_PARA) != रिक्त);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    पूर्णांक		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    पर्यंत (मोजणी--)
    {
	did_skip = FALSE;
	साठी (first = TRUE; ; first = FALSE)
	{
	    जर (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    जर (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    जर (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		थांबा;

#ifdef FEAT_FOLDING
	    जर (fold_skipped)
		curr -= dir;
#endif
	    जर ((curr += dir) < 1 || curr > curbuf->b_ml.ml_ओळ_मोजणी)
	    {
		जर (मोजणी)
		    परत FALSE;
		curr -= dir;
		थांबा;
	    }
	}
    }
    setpcmark();
    जर (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    जर (curr == curbuf->b_ml.ml_ओळ_मोजणी && what != '}' && dir == FORWARD)
    {
	char_u *ओळ = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	जर ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(ओळ, ओळ + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    अन्यथा
	curwin->w_cursor.col = 0;
    परत TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    स्थिर पूर्णांक
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    साठी (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	जर (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    थांबा;
	++macro;
	जर (macro[0] == NUL)
	    थांबा;
    }
    परत (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    पूर्णांक
startPS(linenr_T lnum, पूर्णांक para, पूर्णांक both)
{
    char_u	*s;

    s = ml_get(lnum);
    जर (*s == para || *s == '\f' || (both && *s == '}'))
	परत TRUE;
    जर (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	परत TRUE;
    परत FALSE;
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

स्थिर पूर्णांक	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    स्थिर पूर्णांक
cls(शून्य)
{
    पूर्णांक	    c;

    c = gchar_cursor();
    जर (c == ' ' || c == '\t' || c == NUL)
	परत 0;
    जर (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	जर (enc_dbcs == DBCS_KOR && cls_bigword)
	    परत 1;

	// process code leading/trailing bytes
	परत dbcs_class(((अचिन्हित)c >> 8), (c & 0xFF));
    }
    जर (enc_utf8)
    {
	c = utf_class(c);
	जर (c != 0 && cls_bigword)
	    परत 1;
	परत c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    जर (cls_bigword)
	परत 1;

    जर (vim_iswordc(c))
	परत 2;
    परत 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    पूर्णांक
fwd_word(
    दीर्घ	मोजणी,
    पूर्णांक		bigword,    // "W", "E" or "B"
    पूर्णांक		eol)
{
    पूर्णांक		sclass;	    // starting class
    पूर्णांक		i;
    पूर्णांक		last_ओळ;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    पर्यंत (--मोजणी >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	जर (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_ओळ = (curwin->w_cursor.lnum == curbuf->b_ml.ml_ओळ_मोजणी);
	i = inc_cursor();
	जर (i == -1 || (i >= 1 && last_ओळ)) // started at last char in file
	    परत FAIL;
	जर (i >= 1 && eol && मोजणी == 0)      // started at last char in line
	    परत OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	जर (sclass != 0)
	    पर्यंत (cls() == sclass)
	    {
		i = inc_cursor();
		जर (i == -1 || (i >= 1 && eol && मोजणी == 0))
		    परत OK;
	    }

	/*
	 * go to next non-white
	 */
	पर्यंत (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    जर (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		थांबा;

	    i = inc_cursor();
	    जर (i == -1 || (i >= 1 && eol && मोजणी == 0))
		परत OK;
	}
    }
    परत OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    पूर्णांक
bck_word(दीर्घ मोजणी, पूर्णांक bigword, पूर्णांक stop)
{
    पूर्णांक		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    पर्यंत (--मोजणी >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	जर (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, रिक्त))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	जर (dec_cursor() == -1)		// started at start of file
	    परत FAIL;

	जर (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    पर्यंत (cls() == 0)
	    {
		जर (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    जा finished;
		जर (dec_cursor() == -1) // hit start of file, stop here
		    परत OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    जर (skip_chars(cls(), BACKWARD))
		परत OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    परत OK;
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
    पूर्णांक
end_word(
    दीर्घ	मोजणी,
    पूर्णांक		bigword,
    पूर्णांक		stop,
    पूर्णांक		empty)
{
    पूर्णांक		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    जर (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    पर्यंत (--मोजणी >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	जर (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	जर (inc_cursor() == -1)
	    परत FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	जर (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    जर (skip_chars(sclass, FORWARD))
		परत FAIL;
	}
	अन्यथा जर (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    पर्यंत (cls() == 0)
	    {
		जर (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    जा finished;
		जर (inc_cursor() == -1)	    // hit end of file, stop here
		    परत FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    जर (skip_chars(cls(), FORWARD))
		परत FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    परत OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    पूर्णांक
bckend_word(
    दीर्घ	मोजणी,
    पूर्णांक		bigword,    // TRUE for "B"
    पूर्णांक		eol)	    // TRUE: stop at end of line.
{
    पूर्णांक		sclass;	    // starting class
    पूर्णांक		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    पर्यंत (--मोजणी >= 0)
    {
	sclass = cls();
	जर ((i = dec_cursor()) == -1)
	    परत FAIL;
	जर (eol && i == 1)
	    परत OK;

	/*
	 * Move backward to before the start of this word.
	 */
	जर (sclass != 0)
	{
	    पर्यंत (cls() == sclass)
		जर ((i = dec_cursor()) == -1 || (eol && i == 1))
		    परत OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	पर्यंत (cls() == 0)
	{
	    जर (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		थांबा;
	    जर ((i = dec_cursor()) == -1 || (eol && i == 1))
		परत OK;
	}
    }
    adjust_skipcol();
    परत OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    स्थिर पूर्णांक
skip_chars(पूर्णांक cclass, पूर्णांक dir)
{
    पर्यंत (cls() == cclass)
	जर ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    परत TRUE;
    परत FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    स्थिर शून्य
back_in_ओळ(शून्य)
{
    पूर्णांक		sclass;		    // starting class

    sclass = cls();
    साठी (;;)
    {
	जर (curwin->w_cursor.col == 0)	    // stop at start of line
	    थांबा;
	dec_cursor();
	जर (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    थांबा;
	}
    }
}

    स्थिर शून्य
find_first_blank(pos_T *posp)
{
    पूर्णांक	    c;

    पर्यंत (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	जर (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    थांबा;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    स्थिर शून्य
findsent_forward(
    दीर्घ    मोजणी,
    पूर्णांक	    at_start_sent)	// cursor is at start of sentence
{
    पर्यंत (मोजणी--)
    {
	findsent(FORWARD, 1L);
	जर (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	जर (मोजणी == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    पूर्णांक
current_word(
    oparg_T	*oap,
    दीर्घ	मोजणी,
    पूर्णांक		include,	// TRUE: include word and white space
    पूर्णांक		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    पूर्णांक		inclusive = TRUE;
    पूर्णांक		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    जर (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    जर (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_ओळ();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	जर ((cls() == 0) == include)
	{
	    जर (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    जर (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    अन्यथा
		oneleft();

	    जर (include)
		include_white = TRUE;
	}

	जर (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	अन्यथा
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--मोजणी;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    पर्यंत (मोजणी > 0)
    {
	inclusive = TRUE;
	जर (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    जर (decl(&curwin->w_cursor) == -1)
		परत FAIL;
	    जर (include != (cls() != 0))
	    {
		जर (bck_word(1L, bigword, TRUE) == FAIL)
		    परत FAIL;
	    }
	    अन्यथा
	    {
		जर (bckend_word(1L, bigword, TRUE) == FAIL)
		    परत FAIL;
		(शून्य)incl(&curwin->w_cursor);
	    }
	}
	अन्यथा
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    जर (incl(&curwin->w_cursor) == -1)
		परत FAIL;
	    जर (include != (cls() == 0))
	    {
		जर (fwd_word(1L, bigword, TRUE) == FAIL && मोजणी > 1)
		    परत FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		जर (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    अन्यथा
	    {
		जर (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    परत FAIL;
	    }
	}
	--मोजणी;
    }

    जर (include_white && (cls() != 0
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
	जर (oneleft() == OK)
	{
	    back_in_ओळ();
	    जर (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		जर (VIsual_active)
		    VIsual = curwin->w_cursor;
		अन्यथा
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    जर (VIsual_active)
    {
	जर (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	जर (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    अन्यथा
	oap->inclusive = inclusive;

    परत OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    पूर्णांक
current_sent(oparg_T *oap, दीर्घ मोजणी, पूर्णांक include)
{
    pos_T	start_pos;
    pos_T	pos;
    पूर्णांक		start_blank;
    पूर्णांक		c;
    पूर्णांक		at_start_sent;
    दीर्घ	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    जर (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	जर (LT_POS(start_pos, VIsual))
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
	    पर्यंत (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		जर (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    थांबा;
		}
		incl(&pos);
	    }
	    जर (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		जर (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		अन्यथा
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    जर (include)	// "as" gets twice as much as "is"
		मोजणी *= 2;
	    पर्यंत (मोजणी--)
	    {
		जर (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		जर (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	अन्यथा
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
	    जर (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		पर्यंत (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    जर (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			थांबा;
		    }
		    incl(&pos);
		}
		जर (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		अन्यथा		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    जर (include)	// "as" gets twice as much as "is"
		मोजणी *= 2;
	    findsent_forward(मोजणी, at_start_sent);
	    जर (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	परत OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    पर्यंत (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    जर (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    अन्यथा
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    जर (include)
	ncount = मोजणी * 2;
    अन्यथा
    {
	ncount = मोजणी;
	जर (start_blank)
	    --ncount;
    }
    जर (ncount > 0)
	findsent_forward(ncount, TRUE);
    अन्यथा
	decl(&curwin->w_cursor);

    जर (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	जर (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    जर (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	अन्यथा जर (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    जर (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	जर (EQUAL_POS(start_pos, curwin->w_cursor))
	    जा extend;
	जर (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    अन्यथा
    {
	// include a newline after the sentence, if there is one
	जर (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	अन्यथा
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    परत OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    पूर्णांक
current_block(
    oparg_T	*oap,
    दीर्घ	मोजणी,
    पूर्णांक		include,	// TRUE == include white space
    पूर्णांक		what,		// '(', '{', etc.
    पूर्णांक		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = रिक्त;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    पूर्णांक		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    जर (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	जर (what == '{')		// ignore indent
	    पर्यंत (inindent(1))
		जर (inc_cursor() != 0)
		    थांबा;
	जर (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    अन्यथा जर (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    अन्यथा
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != रिक्त ? "%M" : "%");
    जर ((pos = findmatch(रिक्त, what)) != रिक्त)
    {
	पर्यंत (मोजणी-- > 0)
	{
	    जर ((pos = findmatch(रिक्त, what)) == रिक्त)
		थांबा;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    अन्यथा
    {
	पर्यंत (मोजणी-- > 0)
	{
	    जर ((pos = findmatchlimit(रिक्त, what, FM_FORWARD, 0)) == रिक्त)
		थांबा;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    जर (pos == रिक्त || (end_pos = findmatch(रिक्त, other)) == रिक्त)
    {
	curwin->w_cursor = old_pos;
	परत FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    पर्यंत (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	पर्यंत (inindent(1))
	{
	    sol = TRUE;
	    जर (decl(&curwin->w_cursor) != 0)
		थांबा;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	जर (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    परत FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	जर (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    जर ((pos = findmatch(रिक्त, what)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		परत FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    जर ((end_pos = findmatch(रिक्त, other)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		परत FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	अन्यथा
	    थांबा;
    }

    जर (VIsual_active)
    {
	जर (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	जर (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अन्यथा
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	जर (sol)
	    incl(&curwin->w_cursor);
	अन्यथा जर (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	अन्यथा
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    परत OK;
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    स्थिर पूर्णांक
in_html_tag(
    पूर्णांक		end_tag)
{
    char_u	*ओळ = ml_get_curline();
    char_u	*p;
    पूर्णांक		c;
    पूर्णांक		lc = NUL;
    pos_T	pos;

    जर (enc_dbcs)
    {
	char_u	*lp = रिक्त;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	साठी (p = ओळ; p < ओळ + curwin->w_cursor.col; MB_PTR_ADV(p))
	    जर (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	जर (*p != '<')	    // check for '<' under cursor
	{
	    जर (lc != '<')
		परत FALSE;
	    p = lp;
	}
    }
    अन्यथा
    {
	साठी (p = ओळ + curwin->w_cursor.col; p > ओळ; )
	{
	    जर (*p == '<')	// find '<' under/before cursor
		थांबा;
	    MB_PTR_BACK(ओळ, p);
	    जर (*p == '>')	// find '>' before cursor
		थांबा;
	}
	जर (*p != '<')
	    परत FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - ओळ);

    MB_PTR_ADV(p);
    जर (end_tag)
	// check that there is a '/' after the '<'
	परत *p == '/';

    // check that there is no '/' after the '<'
    जर (*p == '/')
	परत FALSE;

    // check that the matching '>' is not preceded by '/'
    साठी (;;)
    {
	जर (inc(&pos) < 0)
	    परत FALSE;
	c = *ml_get_pos(&pos);
	जर (c == '>')
	    थांबा;
	lc = c;
    }
    परत lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    पूर्णांक
current_tagblock(
    oparg_T	*oap,
    दीर्घ	मोजणी_arg,
    पूर्णांक		include)	// TRUE == include white space
{
    दीर्घ	मोजणी = मोजणी_arg;
    दीर्घ	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    पूर्णांक		len;
    पूर्णांक		r;
    पूर्णांक		do_include = include;
    पूर्णांक		save_p_ws = p_ws;
    पूर्णांक		retval = FAIL;
    पूर्णांक		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    जर (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    जर (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	पर्यंत (inindent(1))
	    जर (inc_cursor() != 0)
		थांबा;

	जर (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    पर्यंत (*ml_get_cursor() != '>')
		जर (inc_cursor() < 0)
		    थांबा;
	}
	अन्यथा जर (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    पर्यंत (*ml_get_cursor() != '<')
		जर (dec_cursor() < 0)
		    थांबा;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    अन्यथा जर (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    अन्यथा
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    साठी (n = 0; n < मोजणी; ++n)
    {
	जर (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, रिक्त, 0,
						  रिक्त, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    जा theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    साठी (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (पूर्णांक)(cp - p);
    जर (len == 0)
    {
	curwin->w_cursor = old_pos;
	जा theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    जर (spat == रिक्त || epat == रिक्त)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	जा theend;
    }
    प्र_लिखो((अक्षर *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    प्र_लिखो((अक्षर *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, रिक्त,
						    0, रिक्त, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    जर (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	मोजणी = 1;
	curwin->w_cursor = start_pos;
	जा again;
    }

    जर (do_include)
    {
	// Include up to the '>'.
	पर्यंत (*ml_get_cursor() != '>')
	    जर (inc_cursor() < 0)
		थांबा;
    }
    अन्यथा
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	जर (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	अन्यथा जर (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    जर (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	पूर्णांक in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	पर्यंत (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    जर (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		थांबा;
	    }
	    अन्यथा जर (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	जर (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    मोजणी = मोजणी_arg;
	    जा again;
	}
    }

    जर (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	जर (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	अन्यथा जर (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अन्यथा
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	जर (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	अन्यथा
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    परत retval;
}
#endif

    पूर्णांक
current_par(
    oparg_T	*oap,
    दीर्घ	मोजणी,
    पूर्णांक		include,	// TRUE == include white space
    पूर्णांक		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    पूर्णांक		white_in_front;
    पूर्णांक		dir;
    पूर्णांक		start_is_white;
    पूर्णांक		prev_start_is_white;
    पूर्णांक		retval = OK;
    पूर्णांक		do_white = FALSE;
    पूर्णांक		t;
    पूर्णांक		i;

    जर (type == 'S')	    // not implemented yet
	परत FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    जर (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	जर (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	अन्यथा
	    dir = FORWARD;
	साठी (i = मोजणी; --i >= 0; )
	{
	    जर (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_ओळ_मोजणी))
	    {
		retval = FAIL;
		थांबा;
	    }

	    prev_start_is_white = -1;
	    साठी (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		जर (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    थांबा;
		}
		साठी (;;)
		{
		    जर (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ओळ_मोजणी))
			थांबा;
		    जर (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			थांबा;
		    start_lnum += dir;
		}
		जर (!include)
		    थांबा;
		जर (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ओळ_मोजणी))
		    थांबा;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	परत retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    पर्यंत (start_lnum > 1)
    {
	जर (white_in_front)	    // stop at first white line
	{
	    जर (!linewhite(start_lnum - 1))
		थांबा;
	}
	अन्यथा		// stop at first non-white line of start of paragraph
	{
	    जर (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		थांबा;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    पर्यंत (end_lnum <= curbuf->b_ml.ml_ओळ_मोजणी && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = मोजणी;
    जर (!include && white_in_front)
	--i;
    पर्यंत (i--)
    {
	जर (end_lnum == curbuf->b_ml.ml_ओळ_मोजणी)
	    परत FAIL;

	जर (!include)
	    do_white = linewhite(end_lnum + 1);

	जर (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    पर्यंत (end_lnum < curbuf->b_ml.ml_ओळ_मोजणी
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	जर (i == 0 && white_in_front && include)
	    थांबा;

	/*
	 * skip to end of white lines after paragraph
	 */
	जर (include || do_white)
	    पर्यंत (end_lnum < curbuf->b_ml.ml_ओळ_मोजणी
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    जर (!white_in_front && !linewhite(end_lnum) && include)
	पर्यंत (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    जर (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	जर (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    जा extend;
	जर (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    अन्यथा
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    परत OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    स्थिर पूर्णांक
find_next_quote(
    char_u	*ओळ,
    पूर्णांक		col,
    पूर्णांक		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णांक		c;

    साठी (;;)
    {
	c = ओळ[col];
	जर (c == NUL)
	    परत -1;
	अन्यथा जर (escape != रिक्त && vim_strchr(escape, c))
	{
	    ++col;
	    जर (ओळ[col] == NUL)
		परत -1;
	}
	अन्यथा जर (c == quotechar)
	    थांबा;
	जर (has_mbyte)
	    col += (*mb_ptr2len)(ओळ + col);
	अन्यथा
	    ++col;
    }
    परत col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    स्थिर पूर्णांक
find_prev_quote(
    char_u	*ओळ,
    पूर्णांक		col_start,
    पूर्णांक		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णांक		n;

    पर्यंत (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(ओळ, ओळ + col_start);
	n = 0;
	जर (escape != रिक्त)
	    पर्यंत (col_start - n > 0 && vim_strchr(escape,
					     ओळ[col_start - n - 1]) != रिक्त)
	    ++n;
	जर (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	अन्यथा जर (ओळ[col_start] == quotechar)
	    थांबा;
    }
    परत col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    पूर्णांक
current_quote(
    oparg_T	*oap,
    दीर्घ	मोजणी,
    पूर्णांक		include,	// TRUE == include quote char
    पूर्णांक		quotechar)	// Quote character
{
    char_u	*ओळ = ml_get_curline();
    पूर्णांक		col_end;
    पूर्णांक		col_start = curwin->w_cursor.col;
    पूर्णांक		inclusive = FALSE;
    पूर्णांक		vis_empty = TRUE;	// Visual selection <= 1 char
    पूर्णांक		vis_bef_curs = FALSE;	// Visual starts before cursor
    पूर्णांक		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    पूर्णांक		inside_quotes = FALSE;	// Looks like "i'" done before
    पूर्णांक		selected_quote = FALSE;	// Has quote inside selection
    पूर्णांक		i;
    पूर्णांक		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    जर (VIsual_active)
    {
	// this only works within one line
	जर (VIsual.lnum != curwin->w_cursor.lnum)
	    परत FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	जर (*p_sel == 'e')
	{
	    जर (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    अन्यथा जर (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    जर (!vis_bef_curs && !vis_empty)
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

    जर (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	जर (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& ओळ[VIsual.col - 1] == quotechar
			&& ओळ[curwin->w_cursor.col] != NUL
			&& ओळ[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	अन्यथा
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& ओळ[curwin->w_cursor.col - 1] == quotechar
			&& ओळ[VIsual.col] != NUL
			&& ओळ[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	पर्यंत (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    जर (ओळ[i] == NUL)
		थांबा;
	    जर (ओळ[i++] == quotechar)
	    {
		selected_quote = TRUE;
		थांबा;
	    }
	}
    }

    जर (!vis_empty && ओळ[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	जर (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(ओळ, col_start + 1, quotechar, रिक्त);
	    जर (col_start < 0)
		जा abort_शोध;
	    col_end = find_next_quote(ओळ, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    जर (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	अन्यथा
	{
	    col_end = find_prev_quote(ओळ, col_start, quotechar, रिक्त);
	    जर (ओळ[col_end] != quotechar)
		जा abort_शोध;
	    col_start = find_prev_quote(ओळ, col_end, quotechar,
							      curbuf->b_p_qe);
	    जर (ओळ[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    अन्यथा

    जर (ओळ[col_start] == quotechar || !vis_empty)
    {
	पूर्णांक	first_col = col_start;

	जर (!vis_empty)
	{
	    जर (vis_bef_curs)
		first_col = find_next_quote(ओळ, col_start, quotechar, रिक्त);
	    अन्यथा
		first_col = find_prev_quote(ओळ, col_start, quotechar, रिक्त);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	साठी (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(ओळ, col_start, quotechar, रिक्त);
	    जर (col_start < 0 || col_start > first_col)
		जा abort_शोध;
	    // Find close quote character.
	    col_end = find_next_quote(ओळ, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    जर (col_end < 0)
		जा abort_शोध;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    जर (col_start <= first_col && first_col <= col_end)
		थांबा;
	    col_start = col_end + 1;
	}
    }
    अन्यथा
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(ओळ, col_start, quotechar, curbuf->b_p_qe);
	जर (ओळ[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(ओळ, col_start, quotechar, रिक्त);
	    जर (col_start < 0)
		जा abort_शोध;
	}

	// Find close quote character.
	col_end = find_next_quote(ओळ, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	जर (col_end < 0)
	    जा abort_शोध;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    जर (include)
    {
	जर (VIM_ISWHITE(ओळ[col_end + 1]))
	    पर्यंत (VIM_ISWHITE(ओळ[col_end + 1]))
		++col_end;
	अन्यथा
	    पर्यंत (col_start > 0 && VIM_ISWHITE(ओळ[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    जर (!include && मोजणी < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    जर (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	जर (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (ओळ[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| ओळ[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    अन्यथा
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    जर ((include || मोजणी > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    जर (VIsual_active)
    {
	जर (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    जर (*p_sel != 'e')
		dec_cursor();
	}
	अन्यथा
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    जर (inside_quotes
		    || (!selected_quote
			&& ओळ[VIsual.col] != quotechar
			&& (ओळ[VIsual.col] == NUL
			    || ओळ[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	जर (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    अन्यथा
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    परत OK;

abort_शोध:
    जर (VIsual_active && *p_sel == 'e')
    {
	जर (did_exclusive_adj)
	    inc_cursor();
	जर (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    परत FALSE;
}
