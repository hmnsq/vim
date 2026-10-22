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
#അന്തര്ഭാവ "vim.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ cls(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ skip_chars(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    പൂർണ്ണസംഖ്യ
findsent(പൂർണ്ണസംഖ്യ dir, long എണ്ണം)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		(*func)(pos_T *);
    പൂർണ്ണസംഖ്യ		startlnum;
    പൂർണ്ണസംഖ്യ		noskip = FALSE;	    // do not skip blanks
    പൂർണ്ണസംഖ്യ		cpo_J;
    പൂർണ്ണസംഖ്യ		found_dot;

    pos = curwin->w_cursor;
    എങ്കിൽ (dir == FORWARD)
	func = incl;
    അല്ലെങ്കിൽ
	func = decl;

    ആകുമ്പോൾ (എണ്ണം--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	എങ്കിൽ (gchar_pos(&pos) == NUL)
	{
	    ചെയ്യുക
		എങ്കിൽ ((*func)(&pos) == -1)
		    നിർത്തുക;
	    ആകുമ്പോൾ (gchar_pos(&pos) == NUL);
	    എങ്കിൽ (dir == FORWARD)
		പോകുക found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	അല്ലെങ്കിൽ എങ്കിൽ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    എങ്കിൽ (pos.lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
		തിരികെ FAIL;
	    ++pos.lnum;
	    പോകുക found;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ആകുമ്പോൾ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ശൂന്യ)
	{
	    tpos = pos;
	    എങ്കിൽ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		നിർത്തുക;

	    എങ്കിൽ (found_dot)
		നിർത്തുക;
	    എങ്കിൽ (vim_strchr((char_u *) ".!?", c) != ശൂന്യ)
		found_dot = TRUE;

	    എങ്കിൽ (vim_strchr((char_u *) ")]\"'", c) != ശൂന്യ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ശൂന്യ)
		നിർത്തുക;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ശൂന്യ;

	വേണ്ടി (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    എങ്കിൽ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		എങ്കിൽ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		നിർത്തുക;
	    }
	    എങ്കിൽ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		ചെയ്യുക
		    എങ്കിൽ ((c = inc(&tpos)) == -1)
			നിർത്തുക;
		ആകുമ്പോൾ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ശൂന്യ);
		എങ്കിൽ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    എങ്കിൽ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    നിർത്തുക;
		}
	    }
	    എങ്കിൽ ((*func)(&pos) == -1)
	    {
		എങ്കിൽ (എണ്ണം)
		    തിരികെ FAIL;
		noskip = TRUE;
		നിർത്തുക;
	    }
	}
found:
	    // skip white space
	ആകുമ്പോൾ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    എങ്കിൽ (incl(&pos) == -1)
		നിർത്തുക;

	എങ്കിൽ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    എങ്കിൽ ((*func)(&pos) == -1)
	    {
		എങ്കിൽ (എണ്ണം)
		    തിരികെ FAIL;
		നിർത്തുക;
	    }
	    ++എണ്ണം;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    തിരികെ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    പൂർണ്ണസംഖ്യ
findpar(
    പൂർണ്ണസംഖ്യ		*pincl,	    // Return: TRUE if last char is to be included
    പൂർണ്ണസംഖ്യ		dir,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		what,
    പൂർണ്ണസംഖ്യ		both)
{
    linenr_T	curr;
    പൂർണ്ണസംഖ്യ		did_skip;   // TRUE after separating lines have been skipped
    പൂർണ്ണസംഖ്യ		first;	    // TRUE on first line
    പൂർണ്ണസംഖ്യ		posix = (vim_strchr(p_cpo, CPO_PARA) != ശൂന്യ);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    പൂർണ്ണസംഖ്യ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ആകുമ്പോൾ (എണ്ണം--)
    {
	did_skip = FALSE;
	വേണ്ടി (first = TRUE; ; first = FALSE)
	{
	    എങ്കിൽ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    എങ്കിൽ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    എങ്കിൽ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		നിർത്തുക;

#ifdef FEAT_FOLDING
	    എങ്കിൽ (fold_skipped)
		curr -= dir;
#endif
	    എങ്കിൽ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_വരി_എണ്ണം)
	    {
		എങ്കിൽ (എണ്ണം)
		    തിരികെ FALSE;
		curr -= dir;
		നിർത്തുക;
	    }
	}
    }
    setpcmark();
    എങ്കിൽ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    എങ്കിൽ (curr == curbuf->b_ml.ml_വരി_എണ്ണം && what != '}' && dir == FORWARD)
    {
	char_u *വരി = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	എങ്കിൽ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(വരി, വരി + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    അല്ലെങ്കിൽ
	curwin->w_cursor.col = 0;
    തിരികെ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    വേണ്ടി (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	എങ്കിൽ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    നിർത്തുക;
	++macro;
	എങ്കിൽ (macro[0] == NUL)
	    നിർത്തുക;
    }
    തിരികെ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    പൂർണ്ണസംഖ്യ
startPS(linenr_T lnum, പൂർണ്ണസംഖ്യ para, പൂർണ്ണസംഖ്യ both)
{
    char_u	*s;

    s = ml_get(lnum);
    എങ്കിൽ (*s == para || *s == '\f' || (both && *s == '}'))
	തിരികെ TRUE;
    എങ്കിൽ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	തിരികെ TRUE;
    തിരികെ FALSE;
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

സ്ഥിതം പൂർണ്ണസംഖ്യ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cls(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    c;

    c = gchar_cursor();
    എങ്കിൽ (c == ' ' || c == '\t' || c == NUL)
	തിരികെ 0;
    എങ്കിൽ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	എങ്കിൽ (enc_dbcs == DBCS_KOR && cls_bigword)
	    തിരികെ 1;

	// process code leading/trailing bytes
	തിരികെ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    എങ്കിൽ (enc_utf8)
    {
	c = utf_class(c);
	എങ്കിൽ (c != 0 && cls_bigword)
	    തിരികെ 1;
	തിരികെ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    എങ്കിൽ (cls_bigword)
	തിരികെ 1;

    എങ്കിൽ (vim_iswordc(c))
	തിരികെ 2;
    തിരികെ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    പൂർണ്ണസംഖ്യ
fwd_word(
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		bigword,    // "W", "E" or "B"
    പൂർണ്ണസംഖ്യ		eol)
{
    പൂർണ്ണസംഖ്യ		sclass;	    // starting class
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		last_വരി;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ആകുമ്പോൾ (--എണ്ണം >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	എങ്കിൽ (hasFolding(curwin->w_cursor.lnum, ശൂന്യ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_വരി = (curwin->w_cursor.lnum == curbuf->b_ml.ml_വരി_എണ്ണം);
	i = inc_cursor();
	എങ്കിൽ (i == -1 || (i >= 1 && last_വരി)) // started at last char in file
	    തിരികെ FAIL;
	എങ്കിൽ (i >= 1 && eol && എണ്ണം == 0)      // started at last char in line
	    തിരികെ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	എങ്കിൽ (sclass != 0)
	    ആകുമ്പോൾ (cls() == sclass)
	    {
		i = inc_cursor();
		എങ്കിൽ (i == -1 || (i >= 1 && eol && എണ്ണം == 0))
		    തിരികെ OK;
	    }

	/*
	 * go to next non-white
	 */
	ആകുമ്പോൾ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    എങ്കിൽ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		നിർത്തുക;

	    i = inc_cursor();
	    എങ്കിൽ (i == -1 || (i >= 1 && eol && എണ്ണം == 0))
		തിരികെ OK;
	}
    }
    തിരികെ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    പൂർണ്ണസംഖ്യ
bck_word(long എണ്ണം, പൂർണ്ണസംഖ്യ bigword, പൂർണ്ണസംഖ്യ stop)
{
    പൂർണ്ണസംഖ്യ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ആകുമ്പോൾ (--എണ്ണം >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	എങ്കിൽ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ശൂന്യ))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	എങ്കിൽ (dec_cursor() == -1)		// started at start of file
	    തിരികെ FAIL;

	എങ്കിൽ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ആകുമ്പോൾ (cls() == 0)
	    {
		എങ്കിൽ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    പോകുക finished;
		എങ്കിൽ (dec_cursor() == -1) // hit start of file, stop here
		    തിരികെ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    എങ്കിൽ (skip_chars(cls(), BACKWARD))
		തിരികെ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ
end_word(
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		bigword,
    പൂർണ്ണസംഖ്യ		stop,
    പൂർണ്ണസംഖ്യ		empty)
{
    പൂർണ്ണസംഖ്യ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    എങ്കിൽ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ആകുമ്പോൾ (--എണ്ണം >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	എങ്കിൽ (hasFolding(curwin->w_cursor.lnum, ശൂന്യ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	എങ്കിൽ (inc_cursor() == -1)
	    തിരികെ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	എങ്കിൽ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    എങ്കിൽ (skip_chars(sclass, FORWARD))
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ആകുമ്പോൾ (cls() == 0)
	    {
		എങ്കിൽ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    പോകുക finished;
		എങ്കിൽ (inc_cursor() == -1)	    // hit end of file, stop here
		    തിരികെ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    എങ്കിൽ (skip_chars(cls(), FORWARD))
		തിരികെ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    തിരികെ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    പൂർണ്ണസംഖ്യ
bckend_word(
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		bigword,    // TRUE for "B"
    പൂർണ്ണസംഖ്യ		eol)	    // TRUE: stop at end of line.
{
    പൂർണ്ണസംഖ്യ		sclass;	    // starting class
    പൂർണ്ണസംഖ്യ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ആകുമ്പോൾ (--എണ്ണം >= 0)
    {
	sclass = cls();
	എങ്കിൽ ((i = dec_cursor()) == -1)
	    തിരികെ FAIL;
	എങ്കിൽ (eol && i == 1)
	    തിരികെ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	എങ്കിൽ (sclass != 0)
	{
	    ആകുമ്പോൾ (cls() == sclass)
		എങ്കിൽ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    തിരികെ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ആകുമ്പോൾ (cls() == 0)
	{
	    എങ്കിൽ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		നിർത്തുക;
	    എങ്കിൽ ((i = dec_cursor()) == -1 || (eol && i == 1))
		തിരികെ OK;
	}
    }
    adjust_skipcol();
    തിരികെ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_chars(പൂർണ്ണസംഖ്യ cclass, പൂർണ്ണസംഖ്യ dir)
{
    ആകുമ്പോൾ (cls() == cclass)
	എങ്കിൽ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    സ്ഥിതം ശൂന്യം
back_in_വരി(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		sclass;		    // starting class

    sclass = cls();
    വേണ്ടി (;;)
    {
	എങ്കിൽ (curwin->w_cursor.col == 0)	    // stop at start of line
	    നിർത്തുക;
	dec_cursor();
	എങ്കിൽ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    നിർത്തുക;
	}
    }
}

    സ്ഥിതം ശൂന്യം
find_first_blank(pos_T *posp)
{
    പൂർണ്ണസംഖ്യ	    c;

    ആകുമ്പോൾ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	എങ്കിൽ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    നിർത്തുക;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    സ്ഥിതം ശൂന്യം
findsent_forward(
    long    എണ്ണം,
    പൂർണ്ണസംഖ്യ	    at_start_sent)	// cursor is at start of sentence
{
    ആകുമ്പോൾ (എണ്ണം--)
    {
	findsent(FORWARD, 1L);
	എങ്കിൽ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	എങ്കിൽ (എണ്ണം == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    പൂർണ്ണസംഖ്യ
current_word(
    oparg_T	*oap,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		include,	// TRUE: include word and white space
    പൂർണ്ണസംഖ്യ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    പൂർണ്ണസംഖ്യ		inclusive = TRUE;
    പൂർണ്ണസംഖ്യ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    എങ്കിൽ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    എങ്കിൽ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_വരി();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	എങ്കിൽ ((cls() == 0) == include)
	{
	    എങ്കിൽ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    എങ്കിൽ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    അല്ലെങ്കിൽ
		oneleft();

	    എങ്കിൽ (include)
		include_white = TRUE;
	}

	എങ്കിൽ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	അല്ലെങ്കിൽ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--എണ്ണം;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ആകുമ്പോൾ (എണ്ണം > 0)
    {
	inclusive = TRUE;
	എങ്കിൽ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    എങ്കിൽ (decl(&curwin->w_cursor) == -1)
		തിരികെ FAIL;
	    എങ്കിൽ (include != (cls() != 0))
	    {
		എങ്കിൽ (bck_word(1L, bigword, TRUE) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (bckend_word(1L, bigword, TRUE) == FAIL)
		    തിരികെ FAIL;
		(ശൂന്യം)incl(&curwin->w_cursor);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    എങ്കിൽ (incl(&curwin->w_cursor) == -1)
		തിരികെ FAIL;
	    എങ്കിൽ (include != (cls() == 0))
	    {
		എങ്കിൽ (fwd_word(1L, bigword, TRUE) == FAIL && എണ്ണം > 1)
		    തിരികെ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		എങ്കിൽ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    തിരികെ FAIL;
	    }
	}
	--എണ്ണം;
    }

    എങ്കിൽ (include_white && (cls() != 0
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
	എങ്കിൽ (oneleft() == OK)
	{
	    back_in_വരി();
	    എങ്കിൽ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		എങ്കിൽ (VIsual_active)
		    VIsual = curwin->w_cursor;
		അല്ലെങ്കിൽ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	എങ്കിൽ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    അല്ലെങ്കിൽ
	oap->inclusive = inclusive;

    തിരികെ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    പൂർണ്ണസംഖ്യ
current_sent(oparg_T *oap, long എണ്ണം, പൂർണ്ണസംഖ്യ include)
{
    pos_T	start_pos;
    pos_T	pos;
    പൂർണ്ണസംഖ്യ		start_blank;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    എങ്കിൽ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	എങ്കിൽ (LT_POS(start_pos, VIsual))
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
	    ആകുമ്പോൾ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		എങ്കിൽ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    നിർത്തുക;
		}
		incl(&pos);
	    }
	    എങ്കിൽ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		എങ്കിൽ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		അല്ലെങ്കിൽ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    എങ്കിൽ (include)	// "as" gets twice as much as "is"
		എണ്ണം *= 2;
	    ആകുമ്പോൾ (എണ്ണം--)
	    {
		എങ്കിൽ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		എങ്കിൽ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	അല്ലെങ്കിൽ
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
	    എങ്കിൽ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ആകുമ്പോൾ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    എങ്കിൽ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			നിർത്തുക;
		    }
		    incl(&pos);
		}
		എങ്കിൽ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		അല്ലെങ്കിൽ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    എങ്കിൽ (include)	// "as" gets twice as much as "is"
		എണ്ണം *= 2;
	    findsent_forward(എണ്ണം, at_start_sent);
	    എങ്കിൽ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	തിരികെ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ആകുമ്പോൾ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    എങ്കിൽ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    അല്ലെങ്കിൽ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    എങ്കിൽ (include)
	ncount = എണ്ണം * 2;
    അല്ലെങ്കിൽ
    {
	ncount = എണ്ണം;
	എങ്കിൽ (start_blank)
	    --ncount;
    }
    എങ്കിൽ (ncount > 0)
	findsent_forward(ncount, TRUE);
    അല്ലെങ്കിൽ
	decl(&curwin->w_cursor);

    എങ്കിൽ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	എങ്കിൽ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    എങ്കിൽ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    എങ്കിൽ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	എങ്കിൽ (EQUAL_POS(start_pos, curwin->w_cursor))
	    പോകുക extend;
	എങ്കിൽ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    അല്ലെങ്കിൽ
    {
	// include a newline after the sentence, if there is one
	എങ്കിൽ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	അല്ലെങ്കിൽ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    തിരികെ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    പൂർണ്ണസംഖ്യ
current_block(
    oparg_T	*oap,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		include,	// TRUE == include white space
    പൂർണ്ണസംഖ്യ		what,		// '(', '{', etc.
    പൂർണ്ണസംഖ്യ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ശൂന്യ;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    പൂർണ്ണസംഖ്യ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    എങ്കിൽ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	എങ്കിൽ (what == '{')		// ignore indent
	    ആകുമ്പോൾ (inindent(1))
		എങ്കിൽ (inc_cursor() != 0)
		    നിർത്തുക;
	എങ്കിൽ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    അല്ലെങ്കിൽ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ശൂന്യ ? "%M" : "%");
    എങ്കിൽ ((pos = findmatch(ശൂന്യ, what)) != ശൂന്യ)
    {
	ആകുമ്പോൾ (എണ്ണം-- > 0)
	{
	    എങ്കിൽ ((pos = findmatch(ശൂന്യ, what)) == ശൂന്യ)
		നിർത്തുക;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (എണ്ണം-- > 0)
	{
	    എങ്കിൽ ((pos = findmatchlimit(ശൂന്യ, what, FM_FORWARD, 0)) == ശൂന്യ)
		നിർത്തുക;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    എങ്കിൽ (pos == ശൂന്യ || (end_pos = findmatch(ശൂന്യ, other)) == ശൂന്യ)
    {
	curwin->w_cursor = old_pos;
	തിരികെ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ആകുമ്പോൾ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ആകുമ്പോൾ (inindent(1))
	{
	    sol = TRUE;
	    എങ്കിൽ (decl(&curwin->w_cursor) != 0)
		നിർത്തുക;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	എങ്കിൽ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    തിരികെ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	എങ്കിൽ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    എങ്കിൽ ((pos = findmatch(ശൂന്യ, what)) == ശൂന്യ)
	    {
		curwin->w_cursor = old_pos;
		തിരികെ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    എങ്കിൽ ((end_pos = findmatch(ശൂന്യ, other)) == ശൂന്യ)
	    {
		curwin->w_cursor = old_pos;
		തിരികെ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	എങ്കിൽ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    അല്ലെങ്കിൽ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	എങ്കിൽ (sol)
	    incl(&curwin->w_cursor);
	അല്ലെങ്കിൽ എങ്കിൽ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	അല്ലെങ്കിൽ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
in_html_tag(
    പൂർണ്ണസംഖ്യ		end_tag)
{
    char_u	*വരി = ml_get_curline();
    char_u	*p;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		lc = NUL;
    pos_T	pos;

    എങ്കിൽ (enc_dbcs)
    {
	char_u	*lp = ശൂന്യ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	വേണ്ടി (p = വരി; p < വരി + curwin->w_cursor.col; MB_PTR_ADV(p))
	    എങ്കിൽ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	എങ്കിൽ (*p != '<')	    // check for '<' under cursor
	{
	    എങ്കിൽ (lc != '<')
		തിരികെ FALSE;
	    p = lp;
	}
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (p = വരി + curwin->w_cursor.col; p > വരി; )
	{
	    എങ്കിൽ (*p == '<')	// find '<' under/before cursor
		നിർത്തുക;
	    MB_PTR_BACK(വരി, p);
	    എങ്കിൽ (*p == '>')	// find '>' before cursor
		നിർത്തുക;
	}
	എങ്കിൽ (*p != '<')
	    തിരികെ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - വരി);

    MB_PTR_ADV(p);
    എങ്കിൽ (end_tag)
	// check that there is a '/' after the '<'
	തിരികെ *p == '/';

    // check that there is no '/' after the '<'
    എങ്കിൽ (*p == '/')
	തിരികെ FALSE;

    // check that the matching '>' is not preceded by '/'
    വേണ്ടി (;;)
    {
	എങ്കിൽ (inc(&pos) < 0)
	    തിരികെ FALSE;
	c = *ml_get_pos(&pos);
	എങ്കിൽ (c == '>')
	    നിർത്തുക;
	lc = c;
    }
    തിരികെ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    പൂർണ്ണസംഖ്യ
current_tagblock(
    oparg_T	*oap,
    long	എണ്ണം_arg,
    പൂർണ്ണസംഖ്യ		include)	// TRUE == include white space
{
    long	എണ്ണം = എണ്ണം_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		do_include = include;
    പൂർണ്ണസംഖ്യ		save_p_ws = p_ws;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    പൂർണ്ണസംഖ്യ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    എങ്കിൽ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    എങ്കിൽ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ആകുമ്പോൾ (inindent(1))
	    എങ്കിൽ (inc_cursor() != 0)
		നിർത്തുക;

	എങ്കിൽ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ആകുമ്പോൾ (*ml_get_cursor() != '>')
		എങ്കിൽ (inc_cursor() < 0)
		    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ആകുമ്പോൾ (*ml_get_cursor() != '<')
		എങ്കിൽ (dec_cursor() < 0)
		    നിർത്തുക;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    അല്ലെങ്കിൽ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    വേണ്ടി (n = 0; n < എണ്ണം; ++n)
    {
	എങ്കിൽ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ശൂന്യ, 0,
						  ശൂന്യ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    പോകുക theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    വേണ്ടി (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (പൂർണ്ണസംഖ്യ)(cp - p);
    എങ്കിൽ (len == 0)
    {
	curwin->w_cursor = old_pos;
	പോകുക theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    എങ്കിൽ (spat == ശൂന്യ || epat == ശൂന്യ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	പോകുക theend;
    }
    sprintf((അക്ഷരം *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((അക്ഷരം *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ശൂന്യ,
						    0, ശൂന്യ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    എങ്കിൽ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	എണ്ണം = 1;
	curwin->w_cursor = start_pos;
	പോകുക again;
    }

    എങ്കിൽ (do_include)
    {
	// Include up to the '>'.
	ആകുമ്പോൾ (*ml_get_cursor() != '>')
	    എങ്കിൽ (inc_cursor() < 0)
		നിർത്തുക;
    }
    അല്ലെങ്കിൽ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	എങ്കിൽ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	അല്ലെങ്കിൽ എങ്കിൽ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    എങ്കിൽ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	പൂർണ്ണസംഖ്യ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ആകുമ്പോൾ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    എങ്കിൽ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	എങ്കിൽ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    എണ്ണം = എണ്ണം_arg;
	    പോകുക again;
	}
    }

    എങ്കിൽ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	എങ്കിൽ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	അല്ലെങ്കിൽ എങ്കിൽ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    അല്ലെങ്കിൽ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	എങ്കിൽ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	അല്ലെങ്കിൽ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    തിരികെ retval;
}
#endif

    പൂർണ്ണസംഖ്യ
current_par(
    oparg_T	*oap,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		include,	// TRUE == include white space
    പൂർണ്ണസംഖ്യ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    പൂർണ്ണസംഖ്യ		white_in_front;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		start_is_white;
    പൂർണ്ണസംഖ്യ		prev_start_is_white;
    പൂർണ്ണസംഖ്യ		retval = OK;
    പൂർണ്ണസംഖ്യ		do_white = FALSE;
    പൂർണ്ണസംഖ്യ		t;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (type == 'S')	    // not implemented yet
	തിരികെ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    എങ്കിൽ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	എങ്കിൽ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	അല്ലെങ്കിൽ
	    dir = FORWARD;
	വേണ്ടി (i = എണ്ണം; --i >= 0; )
	{
	    എങ്കിൽ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_വരി_എണ്ണം))
	    {
		retval = FAIL;
		നിർത്തുക;
	    }

	    prev_start_is_white = -1;
	    വേണ്ടി (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		എങ്കിൽ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    നിർത്തുക;
		}
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_വരി_എണ്ണം))
			നിർത്തുക;
		    എങ്കിൽ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			നിർത്തുക;
		    start_lnum += dir;
		}
		എങ്കിൽ (!include)
		    നിർത്തുക;
		എങ്കിൽ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_വരി_എണ്ണം))
		    നിർത്തുക;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	തിരികെ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ആകുമ്പോൾ (start_lnum > 1)
    {
	എങ്കിൽ (white_in_front)	    // stop at first white line
	{
	    എങ്കിൽ (!linewhite(start_lnum - 1))
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ		// stop at first non-white line of start of paragraph
	{
	    എങ്കിൽ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		നിർത്തുക;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ആകുമ്പോൾ (end_lnum <= curbuf->b_ml.ml_വരി_എണ്ണം && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = എണ്ണം;
    എങ്കിൽ (!include && white_in_front)
	--i;
    ആകുമ്പോൾ (i--)
    {
	എങ്കിൽ (end_lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
	    തിരികെ FAIL;

	എങ്കിൽ (!include)
	    do_white = linewhite(end_lnum + 1);

	എങ്കിൽ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ആകുമ്പോൾ (end_lnum < curbuf->b_ml.ml_വരി_എണ്ണം
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	എങ്കിൽ (i == 0 && white_in_front && include)
	    നിർത്തുക;

	/*
	 * skip to end of white lines after paragraph
	 */
	എങ്കിൽ (include || do_white)
	    ആകുമ്പോൾ (end_lnum < curbuf->b_ml.ml_വരി_എണ്ണം
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    എങ്കിൽ (!white_in_front && !linewhite(end_lnum) && include)
	ആകുമ്പോൾ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    എങ്കിൽ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	എങ്കിൽ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    പോകുക extend;
	എങ്കിൽ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    അല്ലെങ്കിൽ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    തിരികെ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_next_quote(
    char_u	*വരി,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    പൂർണ്ണസംഖ്യ		c;

    വേണ്ടി (;;)
    {
	c = വരി[col];
	എങ്കിൽ (c == NUL)
	    തിരികെ -1;
	അല്ലെങ്കിൽ എങ്കിൽ (escape != ശൂന്യ && vim_strchr(escape, c))
	{
	    ++col;
	    എങ്കിൽ (വരി[col] == NUL)
		തിരികെ -1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == quotechar)
	    നിർത്തുക;
	എങ്കിൽ (has_mbyte)
	    col += (*mb_ptr2len)(വരി + col);
	അല്ലെങ്കിൽ
	    ++col;
    }
    തിരികെ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_prev_quote(
    char_u	*വരി,
    പൂർണ്ണസംഖ്യ		col_start,
    പൂർണ്ണസംഖ്യ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    പൂർണ്ണസംഖ്യ		n;

    ആകുമ്പോൾ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(വരി, വരി + col_start);
	n = 0;
	എങ്കിൽ (escape != ശൂന്യ)
	    ആകുമ്പോൾ (col_start - n > 0 && vim_strchr(escape,
					     വരി[col_start - n - 1]) != ശൂന്യ)
	    ++n;
	എങ്കിൽ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	അല്ലെങ്കിൽ എങ്കിൽ (വരി[col_start] == quotechar)
	    നിർത്തുക;
    }
    തിരികെ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    പൂർണ്ണസംഖ്യ
current_quote(
    oparg_T	*oap,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		include,	// TRUE == include quote char
    പൂർണ്ണസംഖ്യ		quotechar)	// Quote character
{
    char_u	*വരി = ml_get_curline();
    പൂർണ്ണസംഖ്യ		col_end;
    പൂർണ്ണസംഖ്യ		col_start = curwin->w_cursor.col;
    പൂർണ്ണസംഖ്യ		inclusive = FALSE;
    പൂർണ്ണസംഖ്യ		vis_empty = TRUE;	// Visual selection <= 1 char
    പൂർണ്ണസംഖ്യ		vis_bef_curs = FALSE;	// Visual starts before cursor
    പൂർണ്ണസംഖ്യ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    പൂർണ്ണസംഖ്യ		inside_quotes = FALSE;	// Looks like "i'" done before
    പൂർണ്ണസംഖ്യ		selected_quote = FALSE;	// Has quote inside selection
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    എങ്കിൽ (VIsual_active)
    {
	// this only works within one line
	എങ്കിൽ (VIsual.lnum != curwin->w_cursor.lnum)
	    തിരികെ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	എങ്കിൽ (*p_sel == 'e')
	{
	    എങ്കിൽ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    എങ്കിൽ (!vis_bef_curs && !vis_empty)
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

    എങ്കിൽ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	എങ്കിൽ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& വരി[VIsual.col - 1] == quotechar
			&& വരി[curwin->w_cursor.col] != NUL
			&& വരി[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	അല്ലെങ്കിൽ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& വരി[curwin->w_cursor.col - 1] == quotechar
			&& വരി[VIsual.col] != NUL
			&& വരി[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ആകുമ്പോൾ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    എങ്കിൽ (വരി[i] == NUL)
		നിർത്തുക;
	    എങ്കിൽ (വരി[i++] == quotechar)
	    {
		selected_quote = TRUE;
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (!vis_empty && വരി[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	എങ്കിൽ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(വരി, col_start + 1, quotechar, ശൂന്യ);
	    എങ്കിൽ (col_start < 0)
		പോകുക abort_തിരയൽ;
	    col_end = find_next_quote(വരി, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    എങ്കിൽ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    col_end = find_prev_quote(വരി, col_start, quotechar, ശൂന്യ);
	    എങ്കിൽ (വരി[col_end] != quotechar)
		പോകുക abort_തിരയൽ;
	    col_start = find_prev_quote(വരി, col_end, quotechar,
							      curbuf->b_p_qe);
	    എങ്കിൽ (വരി[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    അല്ലെങ്കിൽ

    എങ്കിൽ (വരി[col_start] == quotechar || !vis_empty)
    {
	പൂർണ്ണസംഖ്യ	first_col = col_start;

	എങ്കിൽ (!vis_empty)
	{
	    എങ്കിൽ (vis_bef_curs)
		first_col = find_next_quote(വരി, col_start, quotechar, ശൂന്യ);
	    അല്ലെങ്കിൽ
		first_col = find_prev_quote(വരി, col_start, quotechar, ശൂന്യ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	വേണ്ടി (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(വരി, col_start, quotechar, ശൂന്യ);
	    എങ്കിൽ (col_start < 0 || col_start > first_col)
		പോകുക abort_തിരയൽ;
	    // Find close quote character.
	    col_end = find_next_quote(വരി, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    എങ്കിൽ (col_end < 0)
		പോകുക abort_തിരയൽ;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    എങ്കിൽ (col_start <= first_col && first_col <= col_end)
		നിർത്തുക;
	    col_start = col_end + 1;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(വരി, col_start, quotechar, curbuf->b_p_qe);
	എങ്കിൽ (വരി[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(വരി, col_start, quotechar, ശൂന്യ);
	    എങ്കിൽ (col_start < 0)
		പോകുക abort_തിരയൽ;
	}

	// Find close quote character.
	col_end = find_next_quote(വരി, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	എങ്കിൽ (col_end < 0)
	    പോകുക abort_തിരയൽ;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    എങ്കിൽ (include)
    {
	എങ്കിൽ (VIM_ISWHITE(വരി[col_end + 1]))
	    ആകുമ്പോൾ (VIM_ISWHITE(വരി[col_end + 1]))
		++col_end;
	അല്ലെങ്കിൽ
	    ആകുമ്പോൾ (col_start > 0 && VIM_ISWHITE(വരി[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    എങ്കിൽ (!include && എണ്ണം < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    എങ്കിൽ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	എങ്കിൽ (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (വരി[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| വരി[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    അല്ലെങ്കിൽ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    എങ്കിൽ ((include || എണ്ണം > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    എങ്കിൽ (VIsual_active)
    {
	എങ്കിൽ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    എങ്കിൽ (*p_sel != 'e')
		dec_cursor();
	}
	അല്ലെങ്കിൽ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    എങ്കിൽ (inside_quotes
		    || (!selected_quote
			&& വരി[VIsual.col] != quotechar
			&& (വരി[VIsual.col] == NUL
			    || വരി[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	എങ്കിൽ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    അല്ലെങ്കിൽ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    തിരികെ OK;

abort_തിരയൽ:
    എങ്കിൽ (VIsual_active && *p_sel == 'e')
    {
	എങ്കിൽ (did_exclusive_adj)
	    inc_cursor();
	എങ്കിൽ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    തിരികെ FALSE;
}
