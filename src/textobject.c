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
#অন্তর্ভাৱ "vim.h"

স্থির পূর্ণাঙ্ক cls(শূন্য);
স্থির পূর্ণাঙ্ক skip_chars(পূর্ণাঙ্ক, পূর্ণাঙ্ক);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    পূর্ণাঙ্ক
findsent(পূর্ণাঙ্ক dir, long গুনতি)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		(*func)(pos_T *);
    পূর্ণাঙ্ক		startlnum;
    পূর্ণাঙ্ক		noskip = FALSE;	    // do not skip blanks
    পূর্ণাঙ্ক		cpo_J;
    পূর্ণাঙ্ক		found_dot;

    pos = curwin->w_cursor;
    যদি (dir == FORWARD)
	func = incl;
    অন্যথা
	func = decl;

    যেতিয়ালৈ (গুনতি--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	যদি (gchar_pos(&pos) == NUL)
	{
	    কৰা
		যদি ((*func)(&pos) == -1)
		    ভঙ্গ;
	    যেতিয়ালৈ (gchar_pos(&pos) == NUL);
	    যদি (dir == FORWARD)
		গচ্ছ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	অন্যথা যদি (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    যদি (pos.lnum == curbuf->b_ml.ml_সাৰি_গুনতি)
		ঘূৰাই FAIL;
	    ++pos.lnum;
	    গচ্ছ found;
	}
	অন্যথা যদি (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	যেতিয়ালৈ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != রিক্ত)
	{
	    tpos = pos;
	    যদি (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ভঙ্গ;

	    যদি (found_dot)
		ভঙ্গ;
	    যদি (vim_strchr((char_u *) ".!?", c) != রিক্ত)
		found_dot = TRUE;

	    যদি (vim_strchr((char_u *) ")]\"'", c) != রিক্ত
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == রিক্ত)
		ভঙ্গ;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != রিক্ত;

	বাবে (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    যদি (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		যদি (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ভঙ্গ;
	    }
	    যদি (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		কৰা
		    যদি ((c = inc(&tpos)) == -1)
			ভঙ্গ;
		যেতিয়ালৈ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= রিক্ত);
		যদি (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    যদি (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ভঙ্গ;
		}
	    }
	    যদি ((*func)(&pos) == -1)
	    {
		যদি (গুনতি)
		    ঘূৰাই FAIL;
		noskip = TRUE;
		ভঙ্গ;
	    }
	}
found:
	    // skip white space
	যেতিয়ালৈ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    যদি (incl(&pos) == -1)
		ভঙ্গ;

	যদি (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    যদি ((*func)(&pos) == -1)
	    {
		যদি (গুনতি)
		    ঘূৰাই FAIL;
		ভঙ্গ;
	    }
	    ++গুনতি;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ঘূৰাই OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    পূর্ণাঙ্ক
findpar(
    পূর্ণাঙ্ক		*pincl,	    // Return: TRUE if last char is to be included
    পূর্ণাঙ্ক		dir,
    long	গুনতি,
    পূর্ণাঙ্ক		what,
    পূর্ণাঙ্ক		both)
{
    linenr_T	curr;
    পূর্ণাঙ্ক		did_skip;   // TRUE after separating lines have been skipped
    পূর্ণাঙ্ক		first;	    // TRUE on first line
    পূর্ণাঙ্ক		posix = (vim_strchr(p_cpo, CPO_PARA) != রিক্ত);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    পূর্ণাঙ্ক		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    যেতিয়ালৈ (গুনতি--)
    {
	did_skip = FALSE;
	বাবে (first = TRUE; ; first = FALSE)
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
		ভঙ্গ;

#ifdef FEAT_FOLDING
	    যদি (fold_skipped)
		curr -= dir;
#endif
	    যদি ((curr += dir) < 1 || curr > curbuf->b_ml.ml_সাৰি_গুনতি)
	    {
		যদি (গুনতি)
		    ঘূৰাই FALSE;
		curr -= dir;
		ভঙ্গ;
	    }
	}
    }
    setpcmark();
    যদি (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    যদি (curr == curbuf->b_ml.ml_সাৰি_গুনতি && what != '}' && dir == FORWARD)
    {
	char_u *সাৰি = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	যদি ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(সাৰি, সাৰি + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    অন্যথা
	curwin->w_cursor.col = 0;
    ঘূৰাই TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    স্থির পূর্ণাঙ্ক
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    বাবে (macro = opt; macro[0]; ++macro)
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
	    ভঙ্গ;
	++macro;
	যদি (macro[0] == NUL)
	    ভঙ্গ;
    }
    ঘূৰাই (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    পূর্ণাঙ্ক
startPS(linenr_T lnum, পূর্ণাঙ্ক para, পূর্ণাঙ্ক both)
{
    char_u	*s;

    s = ml_get(lnum);
    যদি (*s == para || *s == '\f' || (both && *s == '}'))
	ঘূৰাই TRUE;
    যদি (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ঘূৰাই TRUE;
    ঘূৰাই FALSE;
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

স্থির পূর্ণাঙ্ক	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    স্থির পূর্ণাঙ্ক
cls(শূন্য)
{
    পূর্ণাঙ্ক	    c;

    c = gchar_cursor();
    যদি (c == ' ' || c == '\t' || c == NUL)
	ঘূৰাই 0;
    যদি (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	যদি (enc_dbcs == DBCS_KOR && cls_bigword)
	    ঘূৰাই 1;

	// process code leading/trailing bytes
	ঘূৰাই dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    যদি (enc_utf8)
    {
	c = utf_class(c);
	যদি (c != 0 && cls_bigword)
	    ঘূৰাই 1;
	ঘূৰাই c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    যদি (cls_bigword)
	ঘূৰাই 1;

    যদি (vim_iswordc(c))
	ঘূৰাই 2;
    ঘূৰাই 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    পূর্ণাঙ্ক
fwd_word(
    long	গুনতি,
    পূর্ণাঙ্ক		bigword,    // "W", "E" or "B"
    পূর্ণাঙ্ক		eol)
{
    পূর্ণাঙ্ক		sclass;	    // starting class
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		last_সাৰি;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যেতিয়ালৈ (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	যদি (hasFolding(curwin->w_cursor.lnum, রিক্ত, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_সাৰি = (curwin->w_cursor.lnum == curbuf->b_ml.ml_সাৰি_গুনতি);
	i = inc_cursor();
	যদি (i == -1 || (i >= 1 && last_সাৰি)) // started at last char in file
	    ঘূৰাই FAIL;
	যদি (i >= 1 && eol && গুনতি == 0)      // started at last char in line
	    ঘূৰাই OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	যদি (sclass != 0)
	    যেতিয়ালৈ (cls() == sclass)
	    {
		i = inc_cursor();
		যদি (i == -1 || (i >= 1 && eol && গুনতি == 0))
		    ঘূৰাই OK;
	    }

	/*
	 * go to next non-white
	 */
	যেতিয়ালৈ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    যদি (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		ভঙ্গ;

	    i = inc_cursor();
	    যদি (i == -1 || (i >= 1 && eol && গুনতি == 0))
		ঘূৰাই OK;
	}
    }
    ঘূৰাই OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    পূর্ণাঙ্ক
bck_word(long গুনতি, পূর্ণাঙ্ক bigword, পূর্ণাঙ্ক stop)
{
    পূর্ণাঙ্ক		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যেতিয়ালৈ (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	যদি (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, রিক্ত))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	যদি (dec_cursor() == -1)		// started at start of file
	    ঘূৰাই FAIL;

	যদি (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    যেতিয়ালৈ (cls() == 0)
	    {
		যদি (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    গচ্ছ finished;
		যদি (dec_cursor() == -1) // hit start of file, stop here
		    ঘূৰাই OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    যদি (skip_chars(cls(), BACKWARD))
		ঘূৰাই OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ঘূৰাই OK;
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
    পূর্ণাঙ্ক
end_word(
    long	গুনতি,
    পূর্ণাঙ্ক		bigword,
    পূর্ণাঙ্ক		stop,
    পূর্ণাঙ্ক		empty)
{
    পূর্ণাঙ্ক		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    যদি (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    যেতিয়ালৈ (--গুনতি >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	যদি (hasFolding(curwin->w_cursor.lnum, রিক্ত, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	যদি (inc_cursor() == -1)
	    ঘূৰাই FAIL;

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
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    যেতিয়ালৈ (cls() == 0)
	    {
		যদি (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    গচ্ছ finished;
		যদি (inc_cursor() == -1)	    // hit end of file, stop here
		    ঘূৰাই FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    যদি (skip_chars(cls(), FORWARD))
		ঘূৰাই FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ঘূৰাই OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    পূর্ণাঙ্ক
bckend_word(
    long	গুনতি,
    পূর্ণাঙ্ক		bigword,    // TRUE for "B"
    পূর্ণাঙ্ক		eol)	    // TRUE: stop at end of line.
{
    পূর্ণাঙ্ক		sclass;	    // starting class
    পূর্ণাঙ্ক		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    যেতিয়ালৈ (--গুনতি >= 0)
    {
	sclass = cls();
	যদি ((i = dec_cursor()) == -1)
	    ঘূৰাই FAIL;
	যদি (eol && i == 1)
	    ঘূৰাই OK;

	/*
	 * Move backward to before the start of this word.
	 */
	যদি (sclass != 0)
	{
	    যেতিয়ালৈ (cls() == sclass)
		যদি ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ঘূৰাই OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	যেতিয়ালৈ (cls() == 0)
	{
	    যদি (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ভঙ্গ;
	    যদি ((i = dec_cursor()) == -1 || (eol && i == 1))
		ঘূৰাই OK;
	}
    }
    adjust_skipcol();
    ঘূৰাই OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    স্থির পূর্ণাঙ্ক
skip_chars(পূর্ণাঙ্ক cclass, পূর্ণাঙ্ক dir)
{
    যেতিয়ালৈ (cls() == cclass)
	যদি ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    স্থির শূন্য
back_in_সাৰি(শূন্য)
{
    পূর্ণাঙ্ক		sclass;		    // starting class

    sclass = cls();
    বাবে (;;)
    {
	যদি (curwin->w_cursor.col == 0)	    // stop at start of line
	    ভঙ্গ;
	dec_cursor();
	যদি (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ভঙ্গ;
	}
    }
}

    স্থির শূন্য
find_first_blank(pos_T *posp)
{
    পূর্ণাঙ্ক	    c;

    যেতিয়ালৈ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	যদি (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ভঙ্গ;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    স্থির শূন্য
findsent_forward(
    long    গুনতি,
    পূর্ণাঙ্ক	    at_start_sent)	// cursor is at start of sentence
{
    যেতিয়ালৈ (গুনতি--)
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
    পূর্ণাঙ্ক
current_word(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণাঙ্ক		include,	// TRUE: include word and white space
    পূর্ণাঙ্ক		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    পূর্ণাঙ্ক		inclusive = TRUE;
    পূর্ণাঙ্ক		include_white = FALSE;

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
	back_in_সাৰি();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	যদি ((cls() == 0) == include)
	{
	    যদি (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ঘূৰাই FAIL;
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
    যেতিয়ালৈ (গুনতি > 0)
    {
	inclusive = TRUE;
	যদি (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    যদি (decl(&curwin->w_cursor) == -1)
		ঘূৰাই FAIL;
	    যদি (include != (cls() != 0))
	    {
		যদি (bck_word(1L, bigword, TRUE) == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা
	    {
		যদি (bckend_word(1L, bigword, TRUE) == FAIL)
		    ঘূৰাই FAIL;
		(শূন্য)incl(&curwin->w_cursor);
	    }
	}
	অন্যথা
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    যদি (incl(&curwin->w_cursor) == -1)
		ঘূৰাই FAIL;
	    যদি (include != (cls() == 0))
	    {
		যদি (fwd_word(1L, bigword, TRUE) == FAIL && গুনতি > 1)
		    ঘূৰাই FAIL;
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
		    ঘূৰাই FAIL;
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
	    back_in_সাৰি();
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

    ঘূৰাই OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    পূর্ণাঙ্ক
current_sent(oparg_T *oap, long গুনতি, পূর্ণাঙ্ক include)
{
    pos_T	start_pos;
    pos_T	pos;
    পূর্ণাঙ্ক		start_blank;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		at_start_sent;
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
	    যেতিয়ালৈ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		যদি (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    ভঙ্গ;
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
	    যেতিয়ালৈ (গুনতি--)
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
		যেতিয়ালৈ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    যদি (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			ভঙ্গ;
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
	ঘূৰাই OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    যেতিয়ালৈ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
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
	    গচ্ছ extend;
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
    ঘূৰাই OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    পূর্ণাঙ্ক
current_block(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণাঙ্ক		include,	// TRUE == include white space
    পূর্ণাঙ্ক		what,		// '(', '{', etc.
    পূর্ণাঙ্ক		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = রিক্ত;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    পূর্ণাঙ্ক		sol = FALSE;		// '{' at start of line

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
	    যেতিয়ালৈ (inindent(1))
		যদি (inc_cursor() != 0)
		    ভঙ্গ;
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
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != রিক্ত ? "%M" : "%");
    যদি ((pos = findmatch(রিক্ত, what)) != রিক্ত)
    {
	যেতিয়ালৈ (গুনতি-- > 0)
	{
	    যদি ((pos = findmatch(রিক্ত, what)) == রিক্ত)
		ভঙ্গ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    অন্যথা
    {
	যেতিয়ালৈ (গুনতি-- > 0)
	{
	    যদি ((pos = findmatchlimit(রিক্ত, what, FM_FORWARD, 0)) == রিক্ত)
		ভঙ্গ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    যদি (pos == রিক্ত || (end_pos = findmatch(রিক্ত, other)) == রিক্ত)
    {
	curwin->w_cursor = old_pos;
	ঘূৰাই FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    যেতিয়ালৈ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	যেতিয়ালৈ (inindent(1))
	{
	    sol = TRUE;
	    যদি (decl(&curwin->w_cursor) != 0)
		ভঙ্গ;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	যদি (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ঘূৰাই FAIL;
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
	    যদি ((pos = findmatch(রিক্ত, what)) == রিক্ত)
	    {
		curwin->w_cursor = old_pos;
		ঘূৰাই FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    যদি ((end_pos = findmatch(রিক্ত, other)) == রিক্ত)
	    {
		curwin->w_cursor = old_pos;
		ঘূৰাই FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	অন্যথা
	    ভঙ্গ;
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

    ঘূৰাই OK;
}

#যদি defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    স্থির পূর্ণাঙ্ক
in_html_tag(
    পূর্ণাঙ্ক		end_tag)
{
    char_u	*সাৰি = ml_get_curline();
    char_u	*p;
    পূর্ণাঙ্ক		c;
    পূর্ণাঙ্ক		lc = NUL;
    pos_T	pos;

    যদি (enc_dbcs)
    {
	char_u	*lp = রিক্ত;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	বাবে (p = সাৰি; p < সাৰি + curwin->w_cursor.col; MB_PTR_ADV(p))
	    যদি (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	যদি (*p != '<')	    // check for '<' under cursor
	{
	    যদি (lc != '<')
		ঘূৰাই FALSE;
	    p = lp;
	}
    }
    অন্যথা
    {
	বাবে (p = সাৰি + curwin->w_cursor.col; p > সাৰি; )
	{
	    যদি (*p == '<')	// find '<' under/before cursor
		ভঙ্গ;
	    MB_PTR_BACK(সাৰি, p);
	    যদি (*p == '>')	// find '>' before cursor
		ভঙ্গ;
	}
	যদি (*p != '<')
	    ঘূৰাই FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - সাৰি);

    MB_PTR_ADV(p);
    যদি (end_tag)
	// check that there is a '/' after the '<'
	ঘূৰাই *p == '/';

    // check that there is no '/' after the '<'
    যদি (*p == '/')
	ঘূৰাই FALSE;

    // check that the matching '>' is not preceded by '/'
    বাবে (;;)
    {
	যদি (inc(&pos) < 0)
	    ঘূৰাই FALSE;
	c = *ml_get_pos(&pos);
	যদি (c == '>')
	    ভঙ্গ;
	lc = c;
    }
    ঘূৰাই lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    পূর্ণাঙ্ক
current_tagblock(
    oparg_T	*oap,
    long	গুনতি_arg,
    পূর্ণাঙ্ক		include)	// TRUE == include white space
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
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		do_include = include;
    পূর্ণাঙ্ক		save_p_ws = p_ws;
    পূর্ণাঙ্ক		retval = FAIL;
    পূর্ণাঙ্ক		is_inclusive = TRUE;

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
	যেতিয়ালৈ (inindent(1))
	    যদি (inc_cursor() != 0)
		ভঙ্গ;

	যদি (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    যেতিয়ালৈ (*ml_get_cursor() != '>')
		যদি (inc_cursor() < 0)
		    ভঙ্গ;
	}
	অন্যথা যদি (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    যেতিয়ালৈ (*ml_get_cursor() != '<')
		যদি (dec_cursor() < 0)
		    ভঙ্গ;
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
    বাবে (n = 0; n < গুনতি; ++n)
    {
	যদি (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, রিক্ত, 0,
						  রিক্ত, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    গচ্ছ theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    বাবে (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (পূর্ণাঙ্ক)(cp - p);
    যদি (len == 0)
    {
	curwin->w_cursor = old_pos;
	গচ্ছ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    যদি (spat == রিক্ত || epat == রিক্ত)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	গচ্ছ theend;
    }
    sprintf((ৱর্ণ *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((ৱর্ণ *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, রিক্ত,
						    0, রিক্ত, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    যদি (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	গুনতি = 1;
	curwin->w_cursor = start_pos;
	গচ্ছ again;
    }

    যদি (do_include)
    {
	// Include up to the '>'.
	যেতিয়ালৈ (*ml_get_cursor() != '>')
	    যদি (inc_cursor() < 0)
		ভঙ্গ;
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
	পূর্ণাঙ্ক in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	যেতিয়ালৈ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    যদি (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		ভঙ্গ;
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
	    গচ্ছ again;
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
    ঘূৰাই retval;
}
#endif

    পূর্ণাঙ্ক
current_par(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণাঙ্ক		include,	// TRUE == include white space
    পূর্ণাঙ্ক		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    পূর্ণাঙ্ক		white_in_front;
    পূর্ণাঙ্ক		dir;
    পূর্ণাঙ্ক		start_is_white;
    পূর্ণাঙ্ক		prev_start_is_white;
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		do_white = FALSE;
    পূর্ণাঙ্ক		t;
    পূর্ণাঙ্ক		i;

    যদি (type == 'S')	    // not implemented yet
	ঘূৰাই FAIL;

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
	বাবে (i = গুনতি; --i >= 0; )
	{
	    যদি (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_সাৰি_গুনতি))
	    {
		retval = FAIL;
		ভঙ্গ;
	    }

	    prev_start_is_white = -1;
	    বাবে (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		যদি (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    ভঙ্গ;
		}
		বাবে (;;)
		{
		    যদি (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_সাৰি_গুনতি))
			ভঙ্গ;
		    যদি (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ভঙ্গ;
		    start_lnum += dir;
		}
		যদি (!include)
		    ভঙ্গ;
		যদি (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_সাৰি_গুনতি))
		    ভঙ্গ;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ঘূৰাই retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    যেতিয়ালৈ (start_lnum > 1)
    {
	যদি (white_in_front)	    // stop at first white line
	{
	    যদি (!linewhite(start_lnum - 1))
		ভঙ্গ;
	}
	অন্যথা		// stop at first non-white line of start of paragraph
	{
	    যদি (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		ভঙ্গ;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    যেতিয়ালৈ (end_lnum <= curbuf->b_ml.ml_সাৰি_গুনতি && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = গুনতি;
    যদি (!include && white_in_front)
	--i;
    যেতিয়ালৈ (i--)
    {
	যদি (end_lnum == curbuf->b_ml.ml_সাৰি_গুনতি)
	    ঘূৰাই FAIL;

	যদি (!include)
	    do_white = linewhite(end_lnum + 1);

	যদি (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    যেতিয়ালৈ (end_lnum < curbuf->b_ml.ml_সাৰি_গুনতি
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	যদি (i == 0 && white_in_front && include)
	    ভঙ্গ;

	/*
	 * skip to end of white lines after paragraph
	 */
	যদি (include || do_white)
	    যেতিয়ালৈ (end_lnum < curbuf->b_ml.ml_সাৰি_গুনতি
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    যদি (!white_in_front && !linewhite(end_lnum) && include)
	যেতিয়ালৈ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    যদি (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	যদি (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    গচ্ছ extend;
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

    ঘূৰাই OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    স্থির পূর্ণাঙ্ক
find_next_quote(
    char_u	*সাৰি,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    পূর্ণাঙ্ক		c;

    বাবে (;;)
    {
	c = সাৰি[col];
	যদি (c == NUL)
	    ঘূৰাই -1;
	অন্যথা যদি (escape != রিক্ত && vim_strchr(escape, c))
	{
	    ++col;
	    যদি (সাৰি[col] == NUL)
		ঘূৰাই -1;
	}
	অন্যথা যদি (c == quotechar)
	    ভঙ্গ;
	যদি (has_mbyte)
	    col += (*mb_ptr2len)(সাৰি + col);
	অন্যথা
	    ++col;
    }
    ঘূৰাই col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    স্থির পূর্ণাঙ্ক
find_prev_quote(
    char_u	*সাৰি,
    পূর্ণাঙ্ক		col_start,
    পূর্ণাঙ্ক		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    পূর্ণাঙ্ক		n;

    যেতিয়ালৈ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(সাৰি, সাৰি + col_start);
	n = 0;
	যদি (escape != রিক্ত)
	    যেতিয়ালৈ (col_start - n > 0 && vim_strchr(escape,
					     সাৰি[col_start - n - 1]) != রিক্ত)
	    ++n;
	যদি (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	অন্যথা যদি (সাৰি[col_start] == quotechar)
	    ভঙ্গ;
    }
    ঘূৰাই col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    পূর্ণাঙ্ক
current_quote(
    oparg_T	*oap,
    long	গুনতি,
    পূর্ণাঙ্ক		include,	// TRUE == include quote char
    পূর্ণাঙ্ক		quotechar)	// Quote character
{
    char_u	*সাৰি = ml_get_curline();
    পূর্ণাঙ্ক		col_end;
    পূর্ণাঙ্ক		col_start = curwin->w_cursor.col;
    পূর্ণাঙ্ক		inclusive = FALSE;
    পূর্ণাঙ্ক		vis_empty = TRUE;	// Visual selection <= 1 char
    পূর্ণাঙ্ক		vis_bef_curs = FALSE;	// Visual starts before cursor
    পূর্ণাঙ্ক		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    পূর্ণাঙ্ক		inside_quotes = FALSE;	// Looks like "i'" done before
    পূর্ণাঙ্ক		selected_quote = FALSE;	// Has quote inside selection
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    যদি (VIsual_active)
    {
	// this only works within one line
	যদি (VIsual.lnum != curwin->w_cursor.lnum)
	    ঘূৰাই FALSE;

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
			&& সাৰি[VIsual.col - 1] == quotechar
			&& সাৰি[curwin->w_cursor.col] != NUL
			&& সাৰি[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	অন্যথা
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& সাৰি[curwin->w_cursor.col - 1] == quotechar
			&& সাৰি[VIsual.col] != NUL
			&& সাৰি[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	যেতিয়ালৈ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    যদি (সাৰি[i] == NUL)
		ভঙ্গ;
	    যদি (সাৰি[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ভঙ্গ;
	    }
	}
    }

    যদি (!vis_empty && সাৰি[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	যদি (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(সাৰি, col_start + 1, quotechar, রিক্ত);
	    যদি (col_start < 0)
		গচ্ছ abort_অনুসন্ধান;
	    col_end = find_next_quote(সাৰি, col_start + 1, quotechar,
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
	    col_end = find_prev_quote(সাৰি, col_start, quotechar, রিক্ত);
	    যদি (সাৰি[col_end] != quotechar)
		গচ্ছ abort_অনুসন্ধান;
	    col_start = find_prev_quote(সাৰি, col_end, quotechar,
							      curbuf->b_p_qe);
	    যদি (সাৰি[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    অন্যথা

    যদি (সাৰি[col_start] == quotechar || !vis_empty)
    {
	পূর্ণাঙ্ক	first_col = col_start;

	যদি (!vis_empty)
	{
	    যদি (vis_bef_curs)
		first_col = find_next_quote(সাৰি, col_start, quotechar, রিক্ত);
	    অন্যথা
		first_col = find_prev_quote(সাৰি, col_start, quotechar, রিক্ত);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	বাবে (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(সাৰি, col_start, quotechar, রিক্ত);
	    যদি (col_start < 0 || col_start > first_col)
		গচ্ছ abort_অনুসন্ধান;
	    // Find close quote character.
	    col_end = find_next_quote(সাৰি, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    যদি (col_end < 0)
		গচ্ছ abort_অনুসন্ধান;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    যদি (col_start <= first_col && first_col <= col_end)
		ভঙ্গ;
	    col_start = col_end + 1;
	}
    }
    অন্যথা
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(সাৰি, col_start, quotechar, curbuf->b_p_qe);
	যদি (সাৰি[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(সাৰি, col_start, quotechar, রিক্ত);
	    যদি (col_start < 0)
		গচ্ছ abort_অনুসন্ধান;
	}

	// Find close quote character.
	col_end = find_next_quote(সাৰি, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	যদি (col_end < 0)
	    গচ্ছ abort_অনুসন্ধান;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    যদি (include)
    {
	যদি (VIM_ISWHITE(সাৰি[col_end + 1]))
	    যেতিয়ালৈ (VIM_ISWHITE(সাৰি[col_end + 1]))
		++col_end;
	অন্যথা
	    যেতিয়ালৈ (col_start > 0 && VIM_ISWHITE(সাৰি[col_start - 1]))
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
			|| (সাৰি[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| সাৰি[VIsual.col - 1] != quotechar)))))
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
			&& সাৰি[VIsual.col] != quotechar
			&& (সাৰি[VIsual.col] == NUL
			    || সাৰি[VIsual.col + 1] != quotechar)))
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

    ঘূৰাই OK;

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
    ঘূৰাই FALSE;
}
