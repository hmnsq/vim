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
#અન્તર્ભાવ "vim.h"

સ્થિર પૂર્ણાંક cls(શૂન્ય);
સ્થિર પૂર્ણાંક skip_chars(પૂર્ણાંક, પૂર્ણાંક);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    પૂર્ણાંક
findsent(પૂર્ણાંક dir, long ગણતરી)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    પૂર્ણાંક		c;
    પૂર્ણાંક		(*func)(pos_T *);
    પૂર્ણાંક		startlnum;
    પૂર્ણાંક		noskip = FALSE;	    // do not skip blanks
    પૂર્ણાંક		cpo_J;
    પૂર્ણાંક		found_dot;

    pos = curwin->w_cursor;
    જો (dir == FORWARD)
	func = incl;
    નહીંતર
	func = decl;

    જ્યારે (ગણતરી--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	જો (gchar_pos(&pos) == NUL)
	{
	    કરો
		જો ((*func)(&pos) == -1)
		    તોડો;
	    જ્યારે (gchar_pos(&pos) == NUL);
	    જો (dir == FORWARD)
		જાઓ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	નહીંતર જો (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    જો (pos.lnum == curbuf->b_ml.ml_લીટી_ગણતરી)
		પાછા FAIL;
	    ++pos.lnum;
	    જાઓ found;
	}
	નહીંતર જો (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	જ્યારે (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ખાલી)
	{
	    tpos = pos;
	    જો (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		તોડો;

	    જો (found_dot)
		તોડો;
	    જો (vim_strchr((char_u *) ".!?", c) != ખાલી)
		found_dot = TRUE;

	    જો (vim_strchr((char_u *) ")]\"'", c) != ખાલી
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ખાલી)
		તોડો;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ખાલી;

	માટે (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    જો (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		જો (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		તોડો;
	    }
	    જો (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		કરો
		    જો ((c = inc(&tpos)) == -1)
			તોડો;
		જ્યારે (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ખાલી);
		જો (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    જો (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    તોડો;
		}
	    }
	    જો ((*func)(&pos) == -1)
	    {
		જો (ગણતરી)
		    પાછા FAIL;
		noskip = TRUE;
		તોડો;
	    }
	}
found:
	    // skip white space
	જ્યારે (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    જો (incl(&pos) == -1)
		તોડો;

	જો (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    જો ((*func)(&pos) == -1)
	    {
		જો (ગણતરી)
		    પાછા FAIL;
		તોડો;
	    }
	    ++ગણતરી;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    પાછા OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    પૂર્ણાંક
findpar(
    પૂર્ણાંક		*pincl,	    // Return: TRUE if last char is to be included
    પૂર્ણાંક		dir,
    long	ગણતરી,
    પૂર્ણાંક		what,
    પૂર્ણાંક		both)
{
    linenr_T	curr;
    પૂર્ણાંક		did_skip;   // TRUE after separating lines have been skipped
    પૂર્ણાંક		first;	    // TRUE on first line
    પૂર્ણાંક		posix = (vim_strchr(p_cpo, CPO_PARA) != ખાલી);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    પૂર્ણાંક		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    જ્યારે (ગણતરી--)
    {
	did_skip = FALSE;
	માટે (first = TRUE; ; first = FALSE)
	{
	    જો (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    જો (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    જો (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		તોડો;

#ifdef FEAT_FOLDING
	    જો (fold_skipped)
		curr -= dir;
#endif
	    જો ((curr += dir) < 1 || curr > curbuf->b_ml.ml_લીટી_ગણતરી)
	    {
		જો (ગણતરી)
		    પાછા FALSE;
		curr -= dir;
		તોડો;
	    }
	}
    }
    setpcmark();
    જો (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    જો (curr == curbuf->b_ml.ml_લીટી_ગણતરી && what != '}' && dir == FORWARD)
    {
	char_u *લીટી = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	જો ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(લીટી, લીટી + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    નહીંતર
	curwin->w_cursor.col = 0;
    પાછા TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    સ્થિર પૂર્ણાંક
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    માટે (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	જો (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    તોડો;
	++macro;
	જો (macro[0] == NUL)
	    તોડો;
    }
    પાછા (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    પૂર્ણાંક
startPS(linenr_T lnum, પૂર્ણાંક para, પૂર્ણાંક both)
{
    char_u	*s;

    s = ml_get(lnum);
    જો (*s == para || *s == '\f' || (both && *s == '}'))
	પાછા TRUE;
    જો (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	પાછા TRUE;
    પાછા FALSE;
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

સ્થિર પૂર્ણાંક	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    સ્થિર પૂર્ણાંક
cls(શૂન્ય)
{
    પૂર્ણાંક	    c;

    c = gchar_cursor();
    જો (c == ' ' || c == '\t' || c == NUL)
	પાછા 0;
    જો (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	જો (enc_dbcs == DBCS_KOR && cls_bigword)
	    પાછા 1;

	// process code leading/trailing bytes
	પાછા dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    જો (enc_utf8)
    {
	c = utf_class(c);
	જો (c != 0 && cls_bigword)
	    પાછા 1;
	પાછા c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    જો (cls_bigword)
	પાછા 1;

    જો (vim_iswordc(c))
	પાછા 2;
    પાછા 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    પૂર્ણાંક
fwd_word(
    long	ગણતરી,
    પૂર્ણાંક		bigword,    // "W", "E" or "B"
    પૂર્ણાંક		eol)
{
    પૂર્ણાંક		sclass;	    // starting class
    પૂર્ણાંક		i;
    પૂર્ણાંક		last_લીટી;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    જ્યારે (--ગણતરી >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	જો (hasFolding(curwin->w_cursor.lnum, ખાલી, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_લીટી = (curwin->w_cursor.lnum == curbuf->b_ml.ml_લીટી_ગણતરી);
	i = inc_cursor();
	જો (i == -1 || (i >= 1 && last_લીટી)) // started at last char in file
	    પાછા FAIL;
	જો (i >= 1 && eol && ગણતરી == 0)      // started at last char in line
	    પાછા OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	જો (sclass != 0)
	    જ્યારે (cls() == sclass)
	    {
		i = inc_cursor();
		જો (i == -1 || (i >= 1 && eol && ગણતરી == 0))
		    પાછા OK;
	    }

	/*
	 * go to next non-white
	 */
	જ્યારે (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    જો (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		તોડો;

	    i = inc_cursor();
	    જો (i == -1 || (i >= 1 && eol && ગણતરી == 0))
		પાછા OK;
	}
    }
    પાછા OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    પૂર્ણાંક
bck_word(long ગણતરી, પૂર્ણાંક bigword, પૂર્ણાંક stop)
{
    પૂર્ણાંક		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    જ્યારે (--ગણતરી >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	જો (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ખાલી))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	જો (dec_cursor() == -1)		// started at start of file
	    પાછા FAIL;

	જો (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    જ્યારે (cls() == 0)
	    {
		જો (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    જાઓ finished;
		જો (dec_cursor() == -1) // hit start of file, stop here
		    પાછા OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    જો (skip_chars(cls(), BACKWARD))
		પાછા OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    પાછા OK;
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
    પૂર્ણાંક
end_word(
    long	ગણતરી,
    પૂર્ણાંક		bigword,
    પૂર્ણાંક		stop,
    પૂર્ણાંક		empty)
{
    પૂર્ણાંક		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    જો (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    જ્યારે (--ગણતરી >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	જો (hasFolding(curwin->w_cursor.lnum, ખાલી, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	જો (inc_cursor() == -1)
	    પાછા FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	જો (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    જો (skip_chars(sclass, FORWARD))
		પાછા FAIL;
	}
	નહીંતર જો (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    જ્યારે (cls() == 0)
	    {
		જો (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    જાઓ finished;
		જો (inc_cursor() == -1)	    // hit end of file, stop here
		    પાછા FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    જો (skip_chars(cls(), FORWARD))
		પાછા FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    પાછા OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    પૂર્ણાંક
bckend_word(
    long	ગણતરી,
    પૂર્ણાંક		bigword,    // TRUE for "B"
    પૂર્ણાંક		eol)	    // TRUE: stop at end of line.
{
    પૂર્ણાંક		sclass;	    // starting class
    પૂર્ણાંક		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    જ્યારે (--ગણતરી >= 0)
    {
	sclass = cls();
	જો ((i = dec_cursor()) == -1)
	    પાછા FAIL;
	જો (eol && i == 1)
	    પાછા OK;

	/*
	 * Move backward to before the start of this word.
	 */
	જો (sclass != 0)
	{
	    જ્યારે (cls() == sclass)
		જો ((i = dec_cursor()) == -1 || (eol && i == 1))
		    પાછા OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	જ્યારે (cls() == 0)
	{
	    જો (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		તોડો;
	    જો ((i = dec_cursor()) == -1 || (eol && i == 1))
		પાછા OK;
	}
    }
    adjust_skipcol();
    પાછા OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    સ્થિર પૂર્ણાંક
skip_chars(પૂર્ણાંક cclass, પૂર્ણાંક dir)
{
    જ્યારે (cls() == cclass)
	જો ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    સ્થિર શૂન્ય
back_in_લીટી(શૂન્ય)
{
    પૂર્ણાંક		sclass;		    // starting class

    sclass = cls();
    માટે (;;)
    {
	જો (curwin->w_cursor.col == 0)	    // stop at start of line
	    તોડો;
	dec_cursor();
	જો (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    તોડો;
	}
    }
}

    સ્થિર શૂન્ય
find_first_blank(pos_T *posp)
{
    પૂર્ણાંક	    c;

    જ્યારે (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	જો (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    તોડો;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    સ્થિર શૂન્ય
findsent_forward(
    long    ગણતરી,
    પૂર્ણાંક	    at_start_sent)	// cursor is at start of sentence
{
    જ્યારે (ગણતરી--)
    {
	findsent(FORWARD, 1L);
	જો (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	જો (ગણતરી == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    પૂર્ણાંક
current_word(
    oparg_T	*oap,
    long	ગણતરી,
    પૂર્ણાંક		include,	// TRUE: include word and white space
    પૂર્ણાંક		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    પૂર્ણાંક		inclusive = TRUE;
    પૂર્ણાંક		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    જો (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    જો (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_લીટી();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	જો ((cls() == 0) == include)
	{
	    જો (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    જો (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    નહીંતર
		oneleft();

	    જો (include)
		include_white = TRUE;
	}

	જો (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	નહીંતર
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--ગણતરી;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    જ્યારે (ગણતરી > 0)
    {
	inclusive = TRUE;
	જો (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    જો (decl(&curwin->w_cursor) == -1)
		પાછા FAIL;
	    જો (include != (cls() != 0))
	    {
		જો (bck_word(1L, bigword, TRUE) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર
	    {
		જો (bckend_word(1L, bigword, TRUE) == FAIL)
		    પાછા FAIL;
		(શૂન્ય)incl(&curwin->w_cursor);
	    }
	}
	નહીંતર
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    જો (incl(&curwin->w_cursor) == -1)
		પાછા FAIL;
	    જો (include != (cls() == 0))
	    {
		જો (fwd_word(1L, bigword, TRUE) == FAIL && ગણતરી > 1)
		    પાછા FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		જો (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    નહીંતર
	    {
		જો (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    પાછા FAIL;
	    }
	}
	--ગણતરી;
    }

    જો (include_white && (cls() != 0
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
	જો (oneleft() == OK)
	{
	    back_in_લીટી();
	    જો (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		જો (VIsual_active)
		    VIsual = curwin->w_cursor;
		નહીંતર
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    જો (VIsual_active)
    {
	જો (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	જો (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    નહીંતર
	oap->inclusive = inclusive;

    પાછા OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    પૂર્ણાંક
current_sent(oparg_T *oap, long ગણતરી, પૂર્ણાંક include)
{
    pos_T	start_pos;
    pos_T	pos;
    પૂર્ણાંક		start_blank;
    પૂર્ણાંક		c;
    પૂર્ણાંક		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    જો (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	જો (LT_POS(start_pos, VIsual))
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
	    જ્યારે (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		જો (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    તોડો;
		}
		incl(&pos);
	    }
	    જો (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		જો (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		નહીંતર
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    જો (include)	// "as" gets twice as much as "is"
		ગણતરી *= 2;
	    જ્યારે (ગણતરી--)
	    {
		જો (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		જો (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	નહીંતર
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
	    જો (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		જ્યારે (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    જો (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			તોડો;
		    }
		    incl(&pos);
		}
		જો (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		નહીંતર		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    જો (include)	// "as" gets twice as much as "is"
		ગણતરી *= 2;
	    findsent_forward(ગણતરી, at_start_sent);
	    જો (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	પાછા OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    જ્યારે (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    જો (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    નહીંતર
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    જો (include)
	ncount = ગણતરી * 2;
    નહીંતર
    {
	ncount = ગણતરી;
	જો (start_blank)
	    --ncount;
    }
    જો (ncount > 0)
	findsent_forward(ncount, TRUE);
    નહીંતર
	decl(&curwin->w_cursor);

    જો (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	જો (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    જો (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	નહીંતર જો (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    જો (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	જો (EQUAL_POS(start_pos, curwin->w_cursor))
	    જાઓ extend;
	જો (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    નહીંતર
    {
	// include a newline after the sentence, if there is one
	જો (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	નહીંતર
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    પાછા OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    પૂર્ણાંક
current_block(
    oparg_T	*oap,
    long	ગણતરી,
    પૂર્ણાંક		include,	// TRUE == include white space
    પૂર્ણાંક		what,		// '(', '{', etc.
    પૂર્ણાંક		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ખાલી;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    પૂર્ણાંક		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    જો (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	જો (what == '{')		// ignore indent
	    જ્યારે (inindent(1))
		જો (inc_cursor() != 0)
		    તોડો;
	જો (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    નહીંતર જો (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    નહીંતર
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ખાલી ? "%M" : "%");
    જો ((pos = findmatch(ખાલી, what)) != ખાલી)
    {
	જ્યારે (ગણતરી-- > 0)
	{
	    જો ((pos = findmatch(ખાલી, what)) == ખાલી)
		તોડો;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    નહીંતર
    {
	જ્યારે (ગણતરી-- > 0)
	{
	    જો ((pos = findmatchlimit(ખાલી, what, FM_FORWARD, 0)) == ખાલી)
		તોડો;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    જો (pos == ખાલી || (end_pos = findmatch(ખાલી, other)) == ખાલી)
    {
	curwin->w_cursor = old_pos;
	પાછા FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    જ્યારે (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	જ્યારે (inindent(1))
	{
	    sol = TRUE;
	    જો (decl(&curwin->w_cursor) != 0)
		તોડો;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	જો (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    પાછા FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	જો (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    જો ((pos = findmatch(ખાલી, what)) == ખાલી)
	    {
		curwin->w_cursor = old_pos;
		પાછા FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    જો ((end_pos = findmatch(ખાલી, other)) == ખાલી)
	    {
		curwin->w_cursor = old_pos;
		પાછા FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	નહીંતર
	    તોડો;
    }

    જો (VIsual_active)
    {
	જો (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	જો (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    નહીંતર
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	જો (sol)
	    incl(&curwin->w_cursor);
	નહીંતર જો (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	નહીંતર
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    પાછા OK;
}

#જો defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    સ્થિર પૂર્ણાંક
in_html_tag(
    પૂર્ણાંક		end_tag)
{
    char_u	*લીટી = ml_get_curline();
    char_u	*p;
    પૂર્ણાંક		c;
    પૂર્ણાંક		lc = NUL;
    pos_T	pos;

    જો (enc_dbcs)
    {
	char_u	*lp = ખાલી;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	માટે (p = લીટી; p < લીટી + curwin->w_cursor.col; MB_PTR_ADV(p))
	    જો (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	જો (*p != '<')	    // check for '<' under cursor
	{
	    જો (lc != '<')
		પાછા FALSE;
	    p = lp;
	}
    }
    નહીંતર
    {
	માટે (p = લીટી + curwin->w_cursor.col; p > લીટી; )
	{
	    જો (*p == '<')	// find '<' under/before cursor
		તોડો;
	    MB_PTR_BACK(લીટી, p);
	    જો (*p == '>')	// find '>' before cursor
		તોડો;
	}
	જો (*p != '<')
	    પાછા FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - લીટી);

    MB_PTR_ADV(p);
    જો (end_tag)
	// check that there is a '/' after the '<'
	પાછા *p == '/';

    // check that there is no '/' after the '<'
    જો (*p == '/')
	પાછા FALSE;

    // check that the matching '>' is not preceded by '/'
    માટે (;;)
    {
	જો (inc(&pos) < 0)
	    પાછા FALSE;
	c = *ml_get_pos(&pos);
	જો (c == '>')
	    તોડો;
	lc = c;
    }
    પાછા lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    પૂર્ણાંક
current_tagblock(
    oparg_T	*oap,
    long	ગણતરી_arg,
    પૂર્ણાંક		include)	// TRUE == include white space
{
    long	ગણતરી = ગણતરી_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    પૂર્ણાંક		len;
    પૂર્ણાંક		r;
    પૂર્ણાંક		do_include = include;
    પૂર્ણાંક		save_p_ws = p_ws;
    પૂર્ણાંક		retval = FAIL;
    પૂર્ણાંક		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    જો (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    જો (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	જ્યારે (inindent(1))
	    જો (inc_cursor() != 0)
		તોડો;

	જો (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    જ્યારે (*ml_get_cursor() != '>')
		જો (inc_cursor() < 0)
		    તોડો;
	}
	નહીંતર જો (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    જ્યારે (*ml_get_cursor() != '<')
		જો (dec_cursor() < 0)
		    તોડો;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    નહીંતર જો (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    નહીંતર
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    માટે (n = 0; n < ગણતરી; ++n)
    {
	જો (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ખાલી, 0,
						  ખાલી, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    જાઓ theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    માટે (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (પૂર્ણાંક)(cp - p);
    જો (len == 0)
    {
	curwin->w_cursor = old_pos;
	જાઓ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    જો (spat == ખાલી || epat == ખાલી)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	જાઓ theend;
    }
    sprintf((અક્ષર *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((અક્ષર *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ખાલી,
						    0, ખાલી, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    જો (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	ગણતરી = 1;
	curwin->w_cursor = start_pos;
	જાઓ again;
    }

    જો (do_include)
    {
	// Include up to the '>'.
	જ્યારે (*ml_get_cursor() != '>')
	    જો (inc_cursor() < 0)
		તોડો;
    }
    નહીંતર
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	જો (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	નહીંતર જો (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    જો (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	પૂર્ણાંક in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	જ્યારે (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    જો (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		તોડો;
	    }
	    નહીંતર જો (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	જો (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    ગણતરી = ગણતરી_arg;
	    જાઓ again;
	}
    }

    જો (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	જો (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	નહીંતર જો (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    નહીંતર
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	જો (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	નહીંતર
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    પાછા retval;
}
#endif

    પૂર્ણાંક
current_par(
    oparg_T	*oap,
    long	ગણતરી,
    પૂર્ણાંક		include,	// TRUE == include white space
    પૂર્ણાંક		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    પૂર્ણાંક		white_in_front;
    પૂર્ણાંક		dir;
    પૂર્ણાંક		start_is_white;
    પૂર્ણાંક		prev_start_is_white;
    પૂર્ણાંક		retval = OK;
    પૂર્ણાંક		do_white = FALSE;
    પૂર્ણાંક		t;
    પૂર્ણાંક		i;

    જો (type == 'S')	    // not implemented yet
	પાછા FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    જો (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	જો (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	નહીંતર
	    dir = FORWARD;
	માટે (i = ગણતરી; --i >= 0; )
	{
	    જો (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_લીટી_ગણતરી))
	    {
		retval = FAIL;
		તોડો;
	    }

	    prev_start_is_white = -1;
	    માટે (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		જો (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    તોડો;
		}
		માટે (;;)
		{
		    જો (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_લીટી_ગણતરી))
			તોડો;
		    જો (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			તોડો;
		    start_lnum += dir;
		}
		જો (!include)
		    તોડો;
		જો (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_લીટી_ગણતરી))
		    તોડો;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	પાછા retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    જ્યારે (start_lnum > 1)
    {
	જો (white_in_front)	    // stop at first white line
	{
	    જો (!linewhite(start_lnum - 1))
		તોડો;
	}
	નહીંતર		// stop at first non-white line of start of paragraph
	{
	    જો (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		તોડો;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    જ્યારે (end_lnum <= curbuf->b_ml.ml_લીટી_ગણતરી && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = ગણતરી;
    જો (!include && white_in_front)
	--i;
    જ્યારે (i--)
    {
	જો (end_lnum == curbuf->b_ml.ml_લીટી_ગણતરી)
	    પાછા FAIL;

	જો (!include)
	    do_white = linewhite(end_lnum + 1);

	જો (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    જ્યારે (end_lnum < curbuf->b_ml.ml_લીટી_ગણતરી
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	જો (i == 0 && white_in_front && include)
	    તોડો;

	/*
	 * skip to end of white lines after paragraph
	 */
	જો (include || do_white)
	    જ્યારે (end_lnum < curbuf->b_ml.ml_લીટી_ગણતરી
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    જો (!white_in_front && !linewhite(end_lnum) && include)
	જ્યારે (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    જો (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	જો (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    જાઓ extend;
	જો (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    નહીંતર
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    પાછા OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    સ્થિર પૂર્ણાંક
find_next_quote(
    char_u	*લીટી,
    પૂર્ણાંક		col,
    પૂર્ણાંક		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    પૂર્ણાંક		c;

    માટે (;;)
    {
	c = લીટી[col];
	જો (c == NUL)
	    પાછા -1;
	નહીંતર જો (escape != ખાલી && vim_strchr(escape, c))
	{
	    ++col;
	    જો (લીટી[col] == NUL)
		પાછા -1;
	}
	નહીંતર જો (c == quotechar)
	    તોડો;
	જો (has_mbyte)
	    col += (*mb_ptr2len)(લીટી + col);
	નહીંતર
	    ++col;
    }
    પાછા col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    સ્થિર પૂર્ણાંક
find_prev_quote(
    char_u	*લીટી,
    પૂર્ણાંક		col_start,
    પૂર્ણાંક		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    પૂર્ણાંક		n;

    જ્યારે (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(લીટી, લીટી + col_start);
	n = 0;
	જો (escape != ખાલી)
	    જ્યારે (col_start - n > 0 && vim_strchr(escape,
					     લીટી[col_start - n - 1]) != ખાલી)
	    ++n;
	જો (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	નહીંતર જો (લીટી[col_start] == quotechar)
	    તોડો;
    }
    પાછા col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    પૂર્ણાંક
current_quote(
    oparg_T	*oap,
    long	ગણતરી,
    પૂર્ણાંક		include,	// TRUE == include quote char
    પૂર્ણાંક		quotechar)	// Quote character
{
    char_u	*લીટી = ml_get_curline();
    પૂર્ણાંક		col_end;
    પૂર્ણાંક		col_start = curwin->w_cursor.col;
    પૂર્ણાંક		inclusive = FALSE;
    પૂર્ણાંક		vis_empty = TRUE;	// Visual selection <= 1 char
    પૂર્ણાંક		vis_bef_curs = FALSE;	// Visual starts before cursor
    પૂર્ણાંક		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    પૂર્ણાંક		inside_quotes = FALSE;	// Looks like "i'" done before
    પૂર્ણાંક		selected_quote = FALSE;	// Has quote inside selection
    પૂર્ણાંક		i;
    પૂર્ણાંક		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    જો (VIsual_active)
    {
	// this only works within one line
	જો (VIsual.lnum != curwin->w_cursor.lnum)
	    પાછા FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	જો (*p_sel == 'e')
	{
	    જો (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    નહીંતર જો (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    જો (!vis_bef_curs && !vis_empty)
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

    જો (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	જો (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& લીટી[VIsual.col - 1] == quotechar
			&& લીટી[curwin->w_cursor.col] != NUL
			&& લીટી[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	નહીંતર
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& લીટી[curwin->w_cursor.col - 1] == quotechar
			&& લીટી[VIsual.col] != NUL
			&& લીટી[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	જ્યારે (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    જો (લીટી[i] == NUL)
		તોડો;
	    જો (લીટી[i++] == quotechar)
	    {
		selected_quote = TRUE;
		તોડો;
	    }
	}
    }

    જો (!vis_empty && લીટી[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	જો (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(લીટી, col_start + 1, quotechar, ખાલી);
	    જો (col_start < 0)
		જાઓ abort_શોધ;
	    col_end = find_next_quote(લીટી, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    જો (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	નહીંતર
	{
	    col_end = find_prev_quote(લીટી, col_start, quotechar, ખાલી);
	    જો (લીટી[col_end] != quotechar)
		જાઓ abort_શોધ;
	    col_start = find_prev_quote(લીટી, col_end, quotechar,
							      curbuf->b_p_qe);
	    જો (લીટી[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    નહીંતર

    જો (લીટી[col_start] == quotechar || !vis_empty)
    {
	પૂર્ણાંક	first_col = col_start;

	જો (!vis_empty)
	{
	    જો (vis_bef_curs)
		first_col = find_next_quote(લીટી, col_start, quotechar, ખાલી);
	    નહીંતર
		first_col = find_prev_quote(લીટી, col_start, quotechar, ખાલી);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	માટે (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(લીટી, col_start, quotechar, ખાલી);
	    જો (col_start < 0 || col_start > first_col)
		જાઓ abort_શોધ;
	    // Find close quote character.
	    col_end = find_next_quote(લીટી, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    જો (col_end < 0)
		જાઓ abort_શોધ;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    જો (col_start <= first_col && first_col <= col_end)
		તોડો;
	    col_start = col_end + 1;
	}
    }
    નહીંતર
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(લીટી, col_start, quotechar, curbuf->b_p_qe);
	જો (લીટી[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(લીટી, col_start, quotechar, ખાલી);
	    જો (col_start < 0)
		જાઓ abort_શોધ;
	}

	// Find close quote character.
	col_end = find_next_quote(લીટી, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	જો (col_end < 0)
	    જાઓ abort_શોધ;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    જો (include)
    {
	જો (VIM_ISWHITE(લીટી[col_end + 1]))
	    જ્યારે (VIM_ISWHITE(લીટી[col_end + 1]))
		++col_end;
	નહીંતર
	    જ્યારે (col_start > 0 && VIM_ISWHITE(લીટી[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    જો (!include && ગણતરી < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    જો (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	જો (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (લીટી[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| લીટી[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    નહીંતર
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    જો ((include || ગણતરી > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    જો (VIsual_active)
    {
	જો (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    જો (*p_sel != 'e')
		dec_cursor();
	}
	નહીંતર
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    જો (inside_quotes
		    || (!selected_quote
			&& લીટી[VIsual.col] != quotechar
			&& (લીટી[VIsual.col] == NUL
			    || લીટી[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	જો (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    નહીંતર
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    પાછા OK;

abort_શોધ:
    જો (VIsual_active && *p_sel == 'e')
    {
	જો (did_exclusive_adj)
	    inc_cursor();
	જો (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    પાછા FALSE;
}
