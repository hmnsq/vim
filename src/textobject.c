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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਪੂਰਨ cls(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ skip_chars(ਪੂਰਨ, ਪੂਰਨ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ਪੂਰਨ
findsent(ਪੂਰਨ dir, ਲੰਮਾ ਗਿਣਤੀ_2)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ਪੂਰਨ		c;
    ਪੂਰਨ		(*func)(pos_T *);
    ਪੂਰਨ		startlnum;
    ਪੂਰਨ		noskip = FALSE;	    // do not skip blanks
    ਪੂਰਨ		cpo_J;
    ਪੂਰਨ		ਮਿਲਿਆ_dot;

    pos = curwin->w_cursor;
    ਜੇ (dir == FORWARD)
	func = incl;
    ਨਹੀਂਤਾਂ
	func = decl;

    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ਜੇ (gchar_pos(&pos) == NUL)
	{
	    ਕਰੋ
		ਜੇ ((*func)(&pos) == -1)
		    ਰੋਕੋ;
	    ਜਦੋਂ (gchar_pos(&pos) == NUL);
	    ਜੇ (dir == FORWARD)
		ਜਾਓ ਮਿਲਿਆ;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ਨਹੀਂਤਾਂ ਜੇ (dir == FORWARD && pos.ਖਾਨਾ == 0 &&
						ਸ਼ੁਰੂ_PS(pos.lnum, NUL, FALSE))
	{
	    ਜੇ (pos.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਵਾਪਸ FAIL;
	    ++pos.lnum;
	    ਜਾਓ ਮਿਲਿਆ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	ਮਿਲਿਆ_dot = FALSE;
	ਜਦੋਂ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ਨਿਰਾ)
	{
	    tpos = pos;
	    ਜੇ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ਰੋਕੋ;

	    ਜੇ (ਮਿਲਿਆ_dot)
		ਰੋਕੋ;
	    ਜੇ (vim_strchr((char_u *) ".!?", c) != ਨਿਰਾ)
		ਮਿਲਿਆ_dot = TRUE;

	    ਜੇ (vim_strchr((char_u *) ")]\"'", c) != ਨਿਰਾ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ਨਿਰਾ)
		ਰੋਕੋ;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ਨਿਰਾ;

	ਲਈ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ਜੇ (c == NUL || (pos.ਖਾਨਾ == 0 && ਸ਼ੁਰੂ_PS(pos.lnum, NUL, FALSE)))
	    {
		ਜੇ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ਰੋਕੋ;
	    }
	    ਜੇ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		ਕਰੋ
		    ਜੇ ((c = inc(&tpos)) == -1)
			ਰੋਕੋ;
		ਜਦੋਂ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ਨਿਰਾ);
		ਜੇ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ਜੇ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ਰੋਕੋ;
		}
	    }
	    ਜੇ ((*func)(&pos) == -1)
	    {
		ਜੇ (ਗਿਣਤੀ_2)
		    ਵਾਪਸ FAIL;
		noskip = TRUE;
		ਰੋਕੋ;
	    }
	}
ਮਿਲਿਆ:
	    // skip white space
	ਜਦੋਂ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ਜੇ (incl(&pos) == -1)
		ਰੋਕੋ;

	ਜੇ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ਜੇ ((*func)(&pos) == -1)
	    {
		ਜੇ (ਗਿਣਤੀ_2)
		    ਵਾਪਸ FAIL;
		ਰੋਕੋ;
	    }
	    ++ਗਿਣਤੀ_2;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ਵਾਪਸ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ਪੂਰਨ
findpar(
    ਪੂਰਨ		*pincl,	    // Return: TRUE if last char is to be included
    ਪੂਰਨ		dir,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		what,
    ਪੂਰਨ		both)
{
    linenr_T	curr;
    ਪੂਰਨ		did_skip;   // TRUE after separating lines have been skipped
    ਪੂਰਨ		ਪਹਿਲਾ;	    // TRUE on first line
    ਪੂਰਨ		posix = (vim_strchr(p_cpo, CPO_PARA) != ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    linenr_T	fold_ਪਹਿਲਾ;	// first line of a closed fold
    linenr_T	fold_ਆਖਰੀ;	// last line of a closed fold
    ਪੂਰਨ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#ਜੇ_ਅੰਤ

    curr = curwin->w_cursor.lnum;

    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	did_skip = FALSE;
	ਲਈ (ਪਹਿਲਾ = TRUE; ; ਪਹਿਲਾ = FALSE)
	{
	    ਜੇ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ਜੇ (ਪਹਿਲਾ && hasFolding(curr, &fold_ਪਹਿਲਾ, &fold_ਆਖਰੀ))
	    {
		curr = ((dir > 0) ? fold_ਆਖਰੀ : fold_ਪਹਿਲਾ) + dir;
		fold_skipped = TRUE;
	    }
#ਜੇ_ਅੰਤ

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ਜੇ (!ਪਹਿਲਾ && did_skip && (ਸ਼ੁਰੂ_PS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    ਜੇ (fold_skipped)
		curr -= dir;
#ਜੇ_ਅੰਤ
	    ਜੇ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਜੇ (ਗਿਣਤੀ_2)
		    ਵਾਪਸ FALSE;
		curr -= dir;
		ਰੋਕੋ;
	    }
	}
    }
    setpcmark();
    ਜੇ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ਜੇ (curr == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ && what != '}' && dir == FORWARD)
    {
	char_u *ਸਤਰ = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ਜੇ ((curwin->w_cursor.ਖਾਨਾ = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.ਖਾਨਾ;
	    curwin->w_cursor.ਖਾਨਾ -=
			     (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + curwin->w_cursor.ਖਾਨਾ);
	    *pincl = TRUE;
	}
    }
    ਨਹੀਂਤਾਂ
	curwin->w_cursor.ਖਾਨਾ = 0;
    ਵਾਪਸ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ਸਥਿਰ ਪੂਰਨ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ਲਈ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ਜੇ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    ਰੋਕੋ;
	++macro;
	ਜੇ (macro[0] == NUL)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ਪੂਰਨ
ਸ਼ੁਰੂ_PS(linenr_T lnum, ਪੂਰਨ para, ਪੂਰਨ both)
{
    char_u	*s;

    s = ml_get(lnum);
    ਜੇ (*s == para || *s == '\f' || (both && *s == '}'))
	ਵਾਪਸ TRUE;
    ਜੇ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
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

ਸਥਿਰ ਪੂਰਨ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ਸਥਿਰ ਪੂਰਨ
cls(ਖਾਲੀ)
{
    ਪੂਰਨ	    c;

    c = gchar_cursor();
    ਜੇ (c == ' ' || c == '\t' || c == NUL)
	ਵਾਪਸ 0;
    ਜੇ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ਜੇ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ਵਾਪਸ 1;

	// process code leading/trailing bytes
	ਵਾਪਸ dbcs_class(((ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 8), (c & 0xFF));
    }
    ਜੇ (enc_utf8)
    {
	c = utf_class(c);
	ਜੇ (c != 0 && cls_bigword)
	    ਵਾਪਸ 1;
	ਵਾਪਸ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ਜੇ (cls_bigword)
	ਵਾਪਸ 1;

    ਜੇ (vim_iswordc(c))
	ਵਾਪਸ 2;
    ਵਾਪਸ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ਪੂਰਨ
fwd_ਸ਼ਬਦ(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		bigword,    // "W", "E" or "B"
    ਪੂਰਨ		eol)
{
    ਪੂਰਨ		sclass;	    // starting class
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਆਖਰੀ_ਸਤਰ;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ਜੇ (hasFolding(curwin->w_cursor.lnum, ਨਿਰਾ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#ਜੇ_ਅੰਤ
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	ਆਖਰੀ_ਸਤਰ = (curwin->w_cursor.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
	i = inc_cursor();
	ਜੇ (i == -1 || (i >= 1 && ਆਖਰੀ_ਸਤਰ)) // started at last char in file
	    ਵਾਪਸ FAIL;
	ਜੇ (i >= 1 && eol && ਗਿਣਤੀ_2 == 0)      // started at last char in line
	    ਵਾਪਸ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ਜੇ (sclass != 0)
	    ਜਦੋਂ (cls() == sclass)
	    {
		i = inc_cursor();
		ਜੇ (i == -1 || (i >= 1 && eol && ਗਿਣਤੀ_2 == 0))
		    ਵਾਪਸ OK;
	    }

	/*
	 * go to next non-white
	 */
	ਜਦੋਂ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0 && *ml_get_curline() == NUL)
		ਰੋਕੋ;

	    i = inc_cursor();
	    ਜੇ (i == -1 || (i >= 1 && eol && ਗਿਣਤੀ_2 == 0))
		ਵਾਪਸ OK;
	}
    }
    ਵਾਪਸ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ਪੂਰਨ
bck_ਸ਼ਬਦ(ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ bigword, ਪੂਰਨ stop)
{
    ਪੂਰਨ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ਜੇ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ਨਿਰਾ))
	    curwin->w_cursor.ਖਾਨਾ = 0;
#ਜੇ_ਅੰਤ
	sclass = cls();
	ਜੇ (dec_cursor() == -1)		// started at start of file
	    ਵਾਪਸ FAIL;

	ਜੇ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ਜਦੋਂ (cls() == 0)
	    {
		ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    ਜਾਓ ਮੁਕੰਮਲ;
		ਜੇ (dec_cursor() == -1) // hit start of file, stop here
		    ਵਾਪਸ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ਜੇ (skip_chars(cls(), BACKWARD))
		ਵਾਪਸ OK;
	}

	inc_cursor();			// overshot - forward one
ਮੁਕੰਮਲ:
	stop = FALSE;
    }
    adjust_skipcol();
    ਵਾਪਸ OK;
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
    ਪੂਰਨ
ਸਿਰਾ_ਸ਼ਬਦ(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		bigword,
    ਪੂਰਨ		stop,
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2)
{
    ਪੂਰਨ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ਜੇ (*p_sel == 'e' && VIsual_ਸਰਗਰਮ && VIsual_ਬਹੁਲਕ == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ਜੇ (hasFolding(curwin->w_cursor.lnum, ਨਿਰਾ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#ਜੇ_ਅੰਤ
	sclass = cls();
	ਜੇ (inc_cursor() == -1)
	    ਵਾਪਸ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ਜੇ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ਜੇ (skip_chars(sclass, FORWARD))
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ਜਦੋਂ (cls() == 0)
	    {
		ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2 && curwin->w_cursor.ਖਾਨਾ == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    ਜਾਓ ਮੁਕੰਮਲ;
		ਜੇ (inc_cursor() == -1)	    // hit end of file, stop here
		    ਵਾਪਸ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ਜੇ (skip_chars(cls(), FORWARD))
		ਵਾਪਸ FAIL;
	}
	dec_cursor();			// overshot - one char backward
ਮੁਕੰਮਲ:
	stop = FALSE;			// we move only one word less
    }
    ਵਾਪਸ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ਪੂਰਨ
bckend_ਸ਼ਬਦ(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		bigword,    // TRUE for "B"
    ਪੂਰਨ		eol)	    // TRUE: stop at end of line.
{
    ਪੂਰਨ		sclass;	    // starting class
    ਪੂਰਨ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ਜਦੋਂ (--ਗਿਣਤੀ_2 >= 0)
    {
	sclass = cls();
	ਜੇ ((i = dec_cursor()) == -1)
	    ਵਾਪਸ FAIL;
	ਜੇ (eol && i == 1)
	    ਵਾਪਸ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ਜੇ (sclass != 0)
	{
	    ਜਦੋਂ (cls() == sclass)
		ਜੇ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ਵਾਪਸ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ਜਦੋਂ (cls() == 0)
	{
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ਰੋਕੋ;
	    ਜੇ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ਵਾਪਸ OK;
	}
    }
    adjust_skipcol();
    ਵਾਪਸ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
skip_chars(ਪੂਰਨ cclass, ਪੂਰਨ dir)
{
    ਜਦੋਂ (cls() == cclass)
	ਜੇ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਿੱਠ_ਵਿੱਚ_ਸਤਰ(ਖਾਲੀ)
{
    ਪੂਰਨ		sclass;		    // starting class

    sclass = cls();
    ਲਈ (;;)
    {
	ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)	    // stop at start of line
	    ਰੋਕੋ;
	dec_cursor();
	ਜੇ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ਰੋਕੋ;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਪਹਿਲਾ_blank(pos_T *posp)
{
    ਪੂਰਨ	    c;

    ਜਦੋਂ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ਜੇ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ਰੋਕੋ;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ਸਥਿਰ ਖਾਲੀ
findsent_forward(
    ਲੰਮਾ    ਗਿਣਤੀ_2,
    ਪੂਰਨ	    at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)	// cursor is at start of sentence
{
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	findsent(FORWARD, 1L);
	ਜੇ (at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)
	    ਲੱਭੋ_ਪਹਿਲਾ_blank(&curwin->w_cursor);
	ਜੇ (ਗਿਣਤੀ_2 == 0 || at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)
	    decl(&curwin->w_cursor);
	at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = !at_ਸ਼ੁਰੂ_ਭੇਜਿਆ;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ਪੂਰਨ
current_ਸ਼ਬਦ(
    oparg_T	*oap,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		include,	// TRUE: include word and white space
    ਪੂਰਨ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	ਸ਼ੁਰੂ_pos;
    pos_T	pos;
    ਪੂਰਨ		inclusive = TRUE;
    ਪੂਰਨ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&ਸ਼ੁਰੂ_pos);

    // Correct cursor when 'selection' is exclusive
    ਜੇ (VIsual_ਸਰਗਰਮ && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ਜੇ (!VIsual_ਸਰਗਰਮ || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	ਪਿੱਠ_ਵਿੱਚ_ਸਤਰ();
	ਸ਼ੁਰੂ_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ਜੇ ((cls() == 0) == include)
	{
	    ਜੇ (ਸਿਰਾ_ਸ਼ਬਦ(1L, bigword, TRUE, TRUE) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_ਸ਼ਬਦ(1L, bigword, TRUE);
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
		decl(&curwin->w_cursor);
	    ਨਹੀਂਤਾਂ
		oneleft();

	    ਜੇ (include)
		include_white = TRUE;
	}

	ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = ਸ਼ੁਰੂ_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ਨਹੀਂਤਾਂ
	{
	    oap->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_pos;
	    oap->motion_type = MCHAR;
	}
	--ਗਿਣਤੀ_2;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ਜਦੋਂ (ਗਿਣਤੀ_2 > 0)
    {
	inclusive = TRUE;
	ਜੇ (VIsual_ਸਰਗਰਮ && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ਜੇ (decl(&curwin->w_cursor) == -1)
		ਵਾਪਸ FAIL;
	    ਜੇ (include != (cls() != 0))
	    {
		ਜੇ (bck_ਸ਼ਬਦ(1L, bigword, TRUE) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (bckend_ਸ਼ਬਦ(1L, bigword, TRUE) == FAIL)
		    ਵਾਪਸ FAIL;
		(ਖਾਲੀ)incl(&curwin->w_cursor);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ਜੇ (incl(&curwin->w_cursor) == -1)
		ਵਾਪਸ FAIL;
	    ਜੇ (include != (cls() == 0))
	    {
		ਜੇ (fwd_ਸ਼ਬਦ(1L, bigword, TRUE) == FAIL && ਗਿਣਤੀ_2 > 1)
		    ਵਾਪਸ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ਜੇ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਸਿਰਾ_ਸ਼ਬਦ(1L, bigword, TRUE, TRUE) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
	--ਗਿਣਤੀ_2;
    }

    ਜੇ (include_white && (cls() != 0
		 || (curwin->w_cursor.ਖਾਨਾ == 0 && !inclusive)))
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
	curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	ਜੇ (oneleft() == OK)
	{
	    ਪਿੱਠ_ਵਿੱਚ_ਸਤਰ();
	    ਜੇ (cls() == 0 && curwin->w_cursor.ਖਾਨਾ > 0)
	    {
		ਜੇ (VIsual_ਸਰਗਰਮ)
		    VIsual = curwin->w_cursor;
		ਨਹੀਂਤਾਂ
		    oap->ਸ਼ੁਰੂ = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ਜੇ (VIsual_ਬਹੁਲਕ == 'V')
	{
	    VIsual_ਬਹੁਲਕ = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ਨਹੀਂਤਾਂ
	oap->inclusive = inclusive;

    ਵਾਪਸ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ਪੂਰਨ
current_ਭੇਜਿਆ(oparg_T *oap, ਲੰਮਾ ਗਿਣਤੀ_2, ਪੂਰਨ include)
{
    pos_T	ਸ਼ੁਰੂ_pos;
    pos_T	pos;
    ਪੂਰਨ		ਸ਼ੁਰੂ_blank;
    ਪੂਰਨ		c;
    ਪੂਰਨ		at_ਸ਼ੁਰੂ_ਭੇਜਿਆ;
    ਲੰਮਾ	ncount;

    ਸ਼ੁਰੂ_pos = curwin->w_cursor;
    pos = ਸ਼ੁਰੂ_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ਜੇ (VIsual_ਸਰਗਰਮ && !EQUAL_POS(ਸ਼ੁਰੂ_pos, VIsual))
    {
extend:
	ਜੇ (LT_POS(ਸ਼ੁਰੂ_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = TRUE;
	    decl(&pos);
	    ਜਦੋਂ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ਜੇ (!VIM_ISWHITE(c))
		{
		    at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = FALSE;
		    ਰੋਕੋ;
		}
		incl(&pos);
	    }
	    ਜੇ (!at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)
	    {
		findsent(BACKWARD, 1L);
		ਜੇ (EQUAL_POS(curwin->w_cursor, ਸ਼ੁਰੂ_pos))
		    at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = TRUE;  // exactly at start of sentence
		ਨਹੀਂਤਾਂ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ਜੇ (include)	// "as" gets twice as much as "is"
		ਗਿਣਤੀ_2 *= 2;
	    ਜਦੋਂ (ਗਿਣਤੀ_2--)
	    {
		ਜੇ (at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)
		    ਲੱਭੋ_ਪਹਿਲਾ_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ਜੇ (!at_ਸ਼ੁਰੂ_ਭੇਜਿਆ || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = !at_ਸ਼ੁਰੂ_ਭੇਜਿਆ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = TRUE;
	    // not just before a sentence
	    ਜੇ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = FALSE;
		ਜਦੋਂ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ਜੇ (!VIM_ISWHITE(c))
		    {
			at_ਸ਼ੁਰੂ_ਭੇਜਿਆ = TRUE;
			ਰੋਕੋ;
		    }
		    incl(&pos);
		}
		ਜੇ (at_ਸ਼ੁਰੂ_ਭੇਜਿਆ)	// in the sentence
		    findsent(BACKWARD, 1L);
		ਨਹੀਂਤਾਂ		// in/before white before a sentence
		    curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	    }

	    ਜੇ (include)	// "as" gets twice as much as "is"
		ਗਿਣਤੀ_2 *= 2;
	    findsent_forward(ਗਿਣਤੀ_2, at_ਸ਼ੁਰੂ_ਭੇਜਿਆ);
	    ਜੇ (*p_sel == 'e')
		++curwin->w_cursor.ਖਾਨਾ;
	}
	ਵਾਪਸ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ਜਦੋਂ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ਜੇ (EQUAL_POS(pos, curwin->w_cursor))
    {
	ਸ਼ੁਰੂ_blank = TRUE;
	ਲੱਭੋ_ਪਹਿਲਾ_blank(&ਸ਼ੁਰੂ_pos);	// go back to first blank
    }
    ਨਹੀਂਤਾਂ
    {
	ਸ਼ੁਰੂ_blank = FALSE;
	findsent(BACKWARD, 1L);
	ਸ਼ੁਰੂ_pos = curwin->w_cursor;
    }
    ਜੇ (include)
	ncount = ਗਿਣਤੀ_2 * 2;
    ਨਹੀਂਤਾਂ
    {
	ncount = ਗਿਣਤੀ_2;
	ਜੇ (ਸ਼ੁਰੂ_blank)
	    --ncount;
    }
    ਜੇ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ਨਹੀਂਤਾਂ
	decl(&curwin->w_cursor);

    ਜੇ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ਜੇ (ਸ਼ੁਰੂ_blank)
	{
	    ਲੱਭੋ_ਪਹਿਲਾ_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ਜੇ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    ਲੱਭੋ_ਪਹਿਲਾ_blank(&ਸ਼ੁਰੂ_pos);
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ਜੇ (EQUAL_POS(ਸ਼ੁਰੂ_pos, curwin->w_cursor))
	    ਜਾਓ extend;
	ਜੇ (*p_sel == 'e')
	    ++curwin->w_cursor.ਖਾਨਾ;
	VIsual = ਸ਼ੁਰੂ_pos;
	VIsual_ਬਹੁਲਕ = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ਨਹੀਂਤਾਂ
    {
	// include a newline after the sentence, if there is one
	ਜੇ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ਨਹੀਂਤਾਂ
	    oap->inclusive = FALSE;
	oap->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_pos;
	oap->motion_type = MCHAR;
    }
    ਵਾਪਸ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ਪੂਰਨ
current_ਖੰਡ(
    oparg_T	*oap,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		include,	// TRUE == include white space
    ਪੂਰਨ		what,		// '(', '{', etc.
    ਪੂਰਨ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ਨਿਰਾ;
    pos_T	ਸ਼ੁਰੂ_pos;
    pos_T	*ਸਿਰਾ_pos;
    pos_T	old_ਸ਼ੁਰੂ, old_ਸਿਰਾ;
    char_u	*save_cpo;
    ਪੂਰਨ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_ਸਿਰਾ = curwin->w_cursor;		// remember where we started
    old_ਸ਼ੁਰੂ = old_ਸਿਰਾ;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ਜੇ (!VIsual_ਸਰਗਰਮ || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ਜੇ (what == '{')		// ignore indent
	    ਜਦੋਂ (inindent(1))
		ਜੇ (inc_cursor() != 0)
		    ਰੋਕੋ;
	ਜੇ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ਸ਼ੁਰੂ = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ਨਹੀਂਤਾਂ
	old_ਸਿਰਾ = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ਨਿਰਾ ? "%M" : "%");
    ਜੇ ((pos = findmatch(ਨਿਰਾ, what)) != ਨਿਰਾ)
    {
	ਜਦੋਂ (ਗਿਣਤੀ_2-- > 0)
	{
	    ਜੇ ((pos = findmatch(ਨਿਰਾ, what)) == ਨਿਰਾ)
		ਰੋਕੋ;
	    curwin->w_cursor = *pos;
	    ਸ਼ੁਰੂ_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (ਗਿਣਤੀ_2-- > 0)
	{
	    ਜੇ ((pos = findmatchlimit(ਨਿਰਾ, what, FM_FORWARD, 0)) == ਨਿਰਾ)
		ਰੋਕੋ;
	    curwin->w_cursor = *pos;
	    ਸ਼ੁਰੂ_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ਜੇ (pos == ਨਿਰਾ || (ਸਿਰਾ_pos = findmatch(ਨਿਰਾ, other)) == ਨਿਰਾ)
    {
	curwin->w_cursor = old_pos;
	ਵਾਪਸ FAIL;
    }
    curwin->w_cursor = *ਸਿਰਾ_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ਜਦੋਂ (!include)
    {
	incl(&ਸ਼ੁਰੂ_pos);
	sol = (curwin->w_cursor.ਖਾਨਾ == 0);
	decl(&curwin->w_cursor);
	ਜਦੋਂ (inindent(1))
	{
	    sol = TRUE;
	    ਜੇ (decl(&curwin->w_cursor) != 0)
		ਰੋਕੋ;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ਜੇ (EQUAL_POS(ਸ਼ੁਰੂ_pos, *ਸਿਰਾ_pos) && VIsual_ਸਰਗਰਮ)
	{
	    curwin->w_cursor = old_pos;
	    ਵਾਪਸ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ਜੇ (!LT_POS(ਸ਼ੁਰੂ_pos, old_ਸ਼ੁਰੂ) && !LT_POS(old_ਸਿਰਾ, curwin->w_cursor)
		&& !EQUAL_POS(ਸ਼ੁਰੂ_pos, curwin->w_cursor)
		&& VIsual_ਸਰਗਰਮ)
	{
	    curwin->w_cursor = old_ਸ਼ੁਰੂ;
	    decl(&curwin->w_cursor);
	    ਜੇ ((pos = findmatch(ਨਿਰਾ, what)) == ਨਿਰਾ)
	    {
		curwin->w_cursor = old_pos;
		ਵਾਪਸ FAIL;
	    }
	    ਸ਼ੁਰੂ_pos = *pos;
	    curwin->w_cursor = *pos;
	    ਜੇ ((ਸਿਰਾ_pos = findmatch(ਨਿਰਾ, other)) == ਨਿਰਾ)
	    {
		curwin->w_cursor = old_pos;
		ਵਾਪਸ FAIL;
	    }
	    curwin->w_cursor = *ਸਿਰਾ_pos;
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ਜੇ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = ਸ਼ੁਰੂ_pos;
	VIsual_ਬਹੁਲਕ = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ਨਹੀਂਤਾਂ
    {
	oap->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ਜੇ (sol)
	    incl(&curwin->w_cursor);
	ਨਹੀਂਤਾਂ ਜੇ (LTOREQ_POS(ਸ਼ੁਰੂ_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ਨਹੀਂਤਾਂ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = ਸ਼ੁਰੂ_pos;
    }

    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ਸਥਿਰ ਪੂਰਨ
ਵਿੱਚ_html_ਟੈਗ(
    ਪੂਰਨ		ਸਿਰਾ_ਟੈਗ)
{
    char_u	*ਸਤਰ = ml_get_curline();
    char_u	*p;
    ਪੂਰਨ		c;
    ਪੂਰਨ		lc = NUL;
    pos_T	pos;

    ਜੇ (enc_dbcs)
    {
	char_u	*lp = ਨਿਰਾ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ਲਈ (p = ਸਤਰ; p < ਸਤਰ + curwin->w_cursor.ਖਾਨਾ; MB_PTR_ADV(p))
	    ਜੇ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ਜੇ (*p != '<')	    // check for '<' under cursor
	{
	    ਜੇ (lc != '<')
		ਵਾਪਸ FALSE;
	    p = lp;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (p = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ; p > ਸਤਰ; )
	{
	    ਜੇ (*p == '<')	// find '<' under/before cursor
		ਰੋਕੋ;
	    MB_PTR_ਪਿੱਠ(ਸਤਰ, p);
	    ਜੇ (*p == '>')	// find '>' before cursor
		ਰੋਕੋ;
	}
	ਜੇ (*p != '<')
	    ਵਾਪਸ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.ਖਾਨਾ = (colnr_T)(p - ਸਤਰ);

    MB_PTR_ADV(p);
    ਜੇ (ਸਿਰਾ_ਟੈਗ)
	// check that there is a '/' after the '<'
	ਵਾਪਸ *p == '/';

    // check that there is no '/' after the '<'
    ਜੇ (*p == '/')
	ਵਾਪਸ FALSE;

    // check that the matching '>' is not preceded by '/'
    ਲਈ (;;)
    {
	ਜੇ (inc(&pos) < 0)
	    ਵਾਪਸ FALSE;
	c = *ml_get_pos(&pos);
	ਜੇ (c == '>')
	    ਰੋਕੋ;
	lc = c;
    }
    ਵਾਪਸ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ਪੂਰਨ
current_tagblock(
    oparg_T	*oap,
    ਲੰਮਾ	ਗਿਣਤੀ_arg,
    ਪੂਰਨ		include)	// TRUE == include white space
{
    ਲੰਮਾ	ਗਿਣਤੀ_2 = ਗਿਣਤੀ_arg;
    ਲੰਮਾ	n;
    pos_T	old_pos;
    pos_T	ਸ਼ੁਰੂ_pos;
    pos_T	ਸਿਰਾ_pos;
    pos_T	old_ਸ਼ੁਰੂ, old_ਸਿਰਾ;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    ਪੂਰਨ		len;
    ਪੂਰਨ		r;
    ਪੂਰਨ		do_include = include;
    ਪੂਰਨ		save_p_ws = p_ws;
    ਪੂਰਨ		retval = FAIL;
    ਪੂਰਨ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_ਸਿਰਾ = curwin->w_cursor;		    // remember where we started
    old_ਸ਼ੁਰੂ = old_ਸਿਰਾ;
    ਜੇ (!VIsual_ਸਰਗਰਮ || *p_sel == 'e')
	decl(&old_ਸਿਰਾ);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ਜੇ (!VIsual_ਸਰਗਰਮ || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ਜਦੋਂ (inindent(1))
	    ਜੇ (inc_cursor() != 0)
		ਰੋਕੋ;

	ਜੇ (ਵਿੱਚ_html_ਟੈਗ(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ਜਦੋਂ (*ml_get_cursor() != '>')
		ਜੇ (inc_cursor() < 0)
		    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_html_ਟੈਗ(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ਜਦੋਂ (*ml_get_cursor() != '<')
		ਜੇ (dec_cursor() < 0)
		    ਰੋਕੋ;
	    dec_cursor();
	    old_ਸਿਰਾ = curwin->w_cursor;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ਸ਼ੁਰੂ = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ਨਹੀਂਤਾਂ
	old_ਸਿਰਾ = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ਲਈ (n = 0; n < ਗਿਣਤੀ_2; ++n)
    {
	ਜੇ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ਨਿਰਾ, 0,
						  ਨਿਰਾ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    ਜਾਓ theend;
	}
    }
    ਸ਼ੁਰੂ_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    ਲਈ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ਪੂਰਨ)(cp - p);
    ਜੇ (len == 0)
    {
	curwin->w_cursor = old_pos;
	ਜਾਓ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ਜੇ (spat == ਨਿਰਾ || epat == ਨਿਰਾ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	ਜਾਓ theend;
    }
    sprintf((ਅੱਖਰ *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((ਅੱਖਰ *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ਨਿਰਾ,
						    0, ਨਿਰਾ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ਜੇ (r < 1 || LT_POS(curwin->w_cursor, old_ਸਿਰਾ))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	ਗਿਣਤੀ_2 = 1;
	curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	ਜਾਓ again;
    }

    ਜੇ (do_include)
    {
	// Include up to the '>'.
	ਜਦੋਂ (*ml_get_cursor() != '>')
	    ਜੇ (inc_cursor() < 0)
		ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ਜੇ (*c == '<' && !VIsual_ਸਰਗਰਮ && curwin->w_cursor.ਖਾਨਾ == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ (*c == '<')
	    dec_cursor();
    }
    ਸਿਰਾ_pos = curwin->w_cursor;

    ਜੇ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ਪੂਰਨ ਵਿੱਚ_quotes = FALSE;
	curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	ਜਦੋਂ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ਜੇ (*p == '>' && !ਵਿੱਚ_quotes)
	    {
		inc_cursor();
		ਸ਼ੁਰੂ_pos = curwin->w_cursor;
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '"' || *p == '\'')
		ਵਿੱਚ_quotes = !ਵਿੱਚ_quotes;
	}
	curwin->w_cursor = ਸਿਰਾ_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ਜੇ (VIsual_ਸਰਗਰਮ && EQUAL_POS(ਸ਼ੁਰੂ_pos, old_ਸ਼ੁਰੂ)
						&& EQUAL_POS(ਸਿਰਾ_pos, old_ਸਿਰਾ))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_ਸ਼ੁਰੂ;
	    ਗਿਣਤੀ_2 = ਗਿਣਤੀ_arg;
	    ਜਾਓ again;
	}
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ਜੇ (LT_POS(ਸਿਰਾ_pos, ਸ਼ੁਰੂ_pos))
	    curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	ਨਹੀਂਤਾਂ ਜੇ (*p_sel == 'e')
	    inc_cursor();
	VIsual = ਸ਼ੁਰੂ_pos;
	VIsual_ਬਹੁਲਕ = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ਨਹੀਂਤਾਂ
    {
	oap->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_pos;
	oap->motion_type = MCHAR;
	ਜੇ (LT_POS(ਸਿਰਾ_pos, ਸ਼ੁਰੂ_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = ਸ਼ੁਰੂ_pos;
	    oap->inclusive = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
current_par(
    oparg_T	*oap,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		include,	// TRUE == include white space
    ਪੂਰਨ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	ਸ਼ੁਰੂ_lnum;
    linenr_T	ਸਿਰਾ_lnum;
    ਪੂਰਨ		white_ਵਿੱਚ_front;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		ਸ਼ੁਰੂ_is_white;
    ਪੂਰਨ		prev_ਸ਼ੁਰੂ_is_white;
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		do_white = FALSE;
    ਪੂਰਨ		t;
    ਪੂਰਨ		i;

    ਜੇ (type == 'S')	    // not implemented yet
	ਵਾਪਸ FAIL;

    ਸ਼ੁਰੂ_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ਜੇ (VIsual_ਸਰਗਰਮ && ਸ਼ੁਰੂ_lnum != VIsual.lnum)
    {
extend:
	ਜੇ (ਸ਼ੁਰੂ_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ਨਹੀਂਤਾਂ
	    dir = FORWARD;
	ਲਈ (i = ਗਿਣਤੀ_2; --i >= 0; )
	{
	    ਜੇ (ਸ਼ੁਰੂ_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
	    {
		retval = FAIL;
		ਰੋਕੋ;
	    }

	    prev_ਸ਼ੁਰੂ_is_white = -1;
	    ਲਈ (t = 0; t < 2; ++t)
	    {
		ਸ਼ੁਰੂ_lnum += dir;
		ਸ਼ੁਰੂ_is_white = linewhite(ਸ਼ੁਰੂ_lnum);
		ਜੇ (prev_ਸ਼ੁਰੂ_is_white == ਸ਼ੁਰੂ_is_white)
		{
		    ਸ਼ੁਰੂ_lnum -= dir;
		    ਰੋਕੋ;
		}
		ਲਈ (;;)
		{
		    ਜੇ (ਸ਼ੁਰੂ_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
			ਰੋਕੋ;
		    ਜੇ (ਸ਼ੁਰੂ_is_white != linewhite(ਸ਼ੁਰੂ_lnum + dir)
			    || (!ਸ਼ੁਰੂ_is_white
				    && ਸ਼ੁਰੂ_PS(ਸ਼ੁਰੂ_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ਰੋਕੋ;
		    ਸ਼ੁਰੂ_lnum += dir;
		}
		ਜੇ (!include)
		    ਰੋਕੋ;
		ਜੇ (ਸ਼ੁਰੂ_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ))
		    ਰੋਕੋ;
		prev_ਸ਼ੁਰੂ_is_white = ਸ਼ੁਰੂ_is_white;
	    }
	}
	curwin->w_cursor.lnum = ਸ਼ੁਰੂ_lnum;
	curwin->w_cursor.ਖਾਨਾ = 0;
	ਵਾਪਸ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_ਵਿੱਚ_front = linewhite(ਸ਼ੁਰੂ_lnum);
    ਜਦੋਂ (ਸ਼ੁਰੂ_lnum > 1)
    {
	ਜੇ (white_ਵਿੱਚ_front)	    // stop at first white line
	{
	    ਜੇ (!linewhite(ਸ਼ੁਰੂ_lnum - 1))
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ		// stop at first non-white line of start of paragraph
	{
	    ਜੇ (linewhite(ਸ਼ੁਰੂ_lnum - 1) || ਸ਼ੁਰੂ_PS(ਸ਼ੁਰੂ_lnum, 0, 0))
		ਰੋਕੋ;
	}
	--ਸ਼ੁਰੂ_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    ਸਿਰਾ_lnum = ਸ਼ੁਰੂ_lnum;
    ਜਦੋਂ (ਸਿਰਾ_lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ && linewhite(ਸਿਰਾ_lnum))
	++ਸਿਰਾ_lnum;

    --ਸਿਰਾ_lnum;
    i = ਗਿਣਤੀ_2;
    ਜੇ (!include && white_ਵਿੱਚ_front)
	--i;
    ਜਦੋਂ (i--)
    {
	ਜੇ (ਸਿਰਾ_lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਵਾਪਸ FAIL;

	ਜੇ (!include)
	    do_white = linewhite(ਸਿਰਾ_lnum + 1);

	ਜੇ (include || !do_white)
	{
	    ++ਸਿਰਾ_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ਜਦੋਂ (ਸਿਰਾ_lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		    && !linewhite(ਸਿਰਾ_lnum + 1)
		    && !ਸ਼ੁਰੂ_PS(ਸਿਰਾ_lnum + 1, 0, 0))
		++ਸਿਰਾ_lnum;
	}

	ਜੇ (i == 0 && white_ਵਿੱਚ_front && include)
	    ਰੋਕੋ;

	/*
	 * skip to end of white lines after paragraph
	 */
	ਜੇ (include || do_white)
	    ਜਦੋਂ (ਸਿਰਾ_lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
						   && linewhite(ਸਿਰਾ_lnum + 1))
		++ਸਿਰਾ_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ਜੇ (!white_ਵਿੱਚ_front && !linewhite(ਸਿਰਾ_lnum) && include)
	ਜਦੋਂ (ਸ਼ੁਰੂ_lnum > 1 && linewhite(ਸ਼ੁਰੂ_lnum - 1))
	    --ਸ਼ੁਰੂ_lnum;

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ਜੇ (VIsual_ਬਹੁਲਕ == 'V' && ਸ਼ੁਰੂ_lnum == curwin->w_cursor.lnum)
	    ਜਾਓ extend;
	ਜੇ (VIsual.lnum != ਸ਼ੁਰੂ_lnum)
	{
	    VIsual.lnum = ਸ਼ੁਰੂ_lnum;
	    VIsual.ਖਾਨਾ = 0;
	}
	VIsual_ਬਹੁਲਕ = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ਨਹੀਂਤਾਂ
    {
	oap->ਸ਼ੁਰੂ.lnum = ਸ਼ੁਰੂ_lnum;
	oap->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = ਸਿਰਾ_lnum;
    curwin->w_cursor.ਖਾਨਾ = 0;

    ਵਾਪਸ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਅਗਲਾ_quote(
    char_u	*ਸਤਰ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ਪੂਰਨ		c;

    ਲਈ (;;)
    {
	c = ਸਤਰ[ਖਾਨਾ];
	ਜੇ (c == NUL)
	    ਵਾਪਸ -1;
	ਨਹੀਂਤਾਂ ਜੇ (escape != ਨਿਰਾ && vim_strchr(escape, c))
	{
	    ++ਖਾਨਾ;
	    ਜੇ (ਸਤਰ[ਖਾਨਾ] == NUL)
		ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == quotechar)
	    ਰੋਕੋ;
	ਜੇ (has_mbyte)
	    ਖਾਨਾ += (*mb_ptr2len)(ਸਤਰ + ਖਾਨਾ);
	ਨਹੀਂਤਾਂ
	    ++ਖਾਨਾ;
    }
    ਵਾਪਸ ਖਾਨਾ;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_prev_quote(
    char_u	*ਸਤਰ,
    ਪੂਰਨ		ਖਾਨਾ_ਸ਼ੁਰੂ,
    ਪੂਰਨ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ਪੂਰਨ		n;

    ਜਦੋਂ (ਖਾਨਾ_ਸ਼ੁਰੂ > 0)
    {
	--ਖਾਨਾ_ਸ਼ੁਰੂ;
	ਖਾਨਾ_ਸ਼ੁਰੂ -= (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + ਖਾਨਾ_ਸ਼ੁਰੂ);
	n = 0;
	ਜੇ (escape != ਨਿਰਾ)
	    ਜਦੋਂ (ਖਾਨਾ_ਸ਼ੁਰੂ - n > 0 && vim_strchr(escape,
					     ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ - n - 1]) != ਨਿਰਾ)
	    ++n;
	ਜੇ (n & 1)
	    ਖਾਨਾ_ਸ਼ੁਰੂ -= n;	// uneven number of escape chars, skip it
	ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ] == quotechar)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਖਾਨਾ_ਸ਼ੁਰੂ;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ਪੂਰਨ
current_quote(
    oparg_T	*oap,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		include,	// TRUE == include quote char
    ਪੂਰਨ		quotechar)	// Quote character
{
    char_u	*ਸਤਰ = ml_get_curline();
    ਪੂਰਨ		ਖਾਨਾ_ਸਿਰਾ;
    ਪੂਰਨ		ਖਾਨਾ_ਸ਼ੁਰੂ = curwin->w_cursor.ਖਾਨਾ;
    ਪੂਰਨ		inclusive = FALSE;
    ਪੂਰਨ		vis_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;	// Visual selection <= 1 char
    ਪੂਰਨ		vis_bef_curs = FALSE;	// Visual starts before cursor
    ਪੂਰਨ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ਪੂਰਨ		inside_quotes = FALSE;	// Looks like "i'" done before
    ਪੂਰਨ		selected_quote = FALSE;	// Has quote inside selection
    ਪੂਰਨ		i;
    ਪੂਰਨ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// this only works within one line
	ਜੇ (VIsual.lnum != curwin->w_cursor.lnum)
	    ਵਾਪਸ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_ਖਾਲੀ_ਥਾਵਾਂ = EQUAL_POS(VIsual, curwin->w_cursor);
	ਜੇ (*p_sel == 'e')
	{
	    ਜੇ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!vis_ਖਾਲੀ_ਥਾਵਾਂ)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_ਖਾਲੀ_ਥਾਵਾਂ = EQUAL_POS(VIsual, curwin->w_cursor);
	    ਜੇ (!vis_bef_curs && !vis_ਖਾਲੀ_ਥਾਵਾਂ)
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

    ਜੇ (!vis_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ਜੇ (vis_bef_curs)
	{
	    inside_quotes = VIsual.ਖਾਨਾ > 0
			&& ਸਤਰ[VIsual.ਖਾਨਾ - 1] == quotechar
			&& ਸਤਰ[curwin->w_cursor.ਖਾਨਾ] != NUL
			&& ਸਤਰ[curwin->w_cursor.ਖਾਨਾ + 1] == quotechar;
	    i = VIsual.ਖਾਨਾ;
	    ਖਾਨਾ_ਸਿਰਾ = curwin->w_cursor.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    inside_quotes = curwin->w_cursor.ਖਾਨਾ > 0
			&& ਸਤਰ[curwin->w_cursor.ਖਾਨਾ - 1] == quotechar
			&& ਸਤਰ[VIsual.ਖਾਨਾ] != NUL
			&& ਸਤਰ[VIsual.ਖਾਨਾ + 1] == quotechar;
	    i = curwin->w_cursor.ਖਾਨਾ;
	    ਖਾਨਾ_ਸਿਰਾ = VIsual.ਖਾਨਾ;
	}

	// Find out if we have a quote in the selection.
	ਜਦੋਂ (i <= ਖਾਨਾ_ਸਿਰਾ)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ਜੇ (ਸਤਰ[i] == NUL)
		ਰੋਕੋ;
	    ਜੇ (ਸਤਰ[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (!vis_ਖਾਲੀ_ਥਾਵਾਂ && ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ਜੇ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    ਖਾਨਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ + 1, quotechar, ਨਿਰਾ);
	    ਜੇ (ਖਾਨਾ_ਸ਼ੁਰੂ < 0)
		ਜਾਓ abort_ਖੋਜ;
	    ਖਾਨਾ_ਸਿਰਾ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ + 1, quotechar,
							      curbuf->b_p_qe);
	    ਜੇ (ਖਾਨਾ_ਸਿਰਾ < 0)
	    {
		// We were on a starting quote perhaps?
		ਖਾਨਾ_ਸਿਰਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;
		ਖਾਨਾ_ਸ਼ੁਰੂ = curwin->w_cursor.ਖਾਨਾ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਖਾਨਾ_ਸਿਰਾ = ਲੱਭੋ_prev_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, ਨਿਰਾ);
	    ਜੇ (ਸਤਰ[ਖਾਨਾ_ਸਿਰਾ] != quotechar)
		ਜਾਓ abort_ਖੋਜ;
	    ਖਾਨਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_prev_quote(ਸਤਰ, ਖਾਨਾ_ਸਿਰਾ, quotechar,
							      curbuf->b_p_qe);
	    ਜੇ (ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ] != quotechar)
	    {
		// We were on an ending quote perhaps?
		ਖਾਨਾ_ਸ਼ੁਰੂ = ਖਾਨਾ_ਸਿਰਾ;
		ਖਾਨਾ_ਸਿਰਾ = curwin->w_cursor.ਖਾਨਾ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ

    ਜੇ (ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ] == quotechar || !vis_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	ਪੂਰਨ	ਪਹਿਲਾ_ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;

	ਜੇ (!vis_ਖਾਲੀ_ਥਾਵਾਂ)
	{
	    ਜੇ (vis_bef_curs)
		ਪਹਿਲਾ_ਖਾਨਾ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
		ਪਹਿਲਾ_ਖਾਨਾ = ਲੱਭੋ_prev_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, ਨਿਰਾ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	ਖਾਨਾ_ਸ਼ੁਰੂ = 0;
	ਲਈ (;;)
	{
	    // Find open quote character.
	    ਖਾਨਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, ਨਿਰਾ);
	    ਜੇ (ਖਾਨਾ_ਸ਼ੁਰੂ < 0 || ਖਾਨਾ_ਸ਼ੁਰੂ > ਪਹਿਲਾ_ਖਾਨਾ)
		ਜਾਓ abort_ਖੋਜ;
	    // Find close quote character.
	    ਖਾਨਾ_ਸਿਰਾ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ + 1, quotechar,
							      curbuf->b_p_qe);
	    ਜੇ (ਖਾਨਾ_ਸਿਰਾ < 0)
		ਜਾਓ abort_ਖੋਜ;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ਜੇ (ਖਾਨਾ_ਸ਼ੁਰੂ <= ਪਹਿਲਾ_ਖਾਨਾ && ਪਹਿਲਾ_ਖਾਨਾ <= ਖਾਨਾ_ਸਿਰਾ)
		ਰੋਕੋ;
	    ਖਾਨਾ_ਸ਼ੁਰੂ = ਖਾਨਾ_ਸਿਰਾ + 1;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Search backward for a starting quote.
	ਖਾਨਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_prev_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, curbuf->b_p_qe);
	ਜੇ (ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    ਖਾਨਾ_ਸ਼ੁਰੂ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ, quotechar, ਨਿਰਾ);
	    ਜੇ (ਖਾਨਾ_ਸ਼ੁਰੂ < 0)
		ਜਾਓ abort_ਖੋਜ;
	}

	// Find close quote character.
	ਖਾਨਾ_ਸਿਰਾ = ਲੱਭੋ_ਅਗਲਾ_quote(ਸਤਰ, ਖਾਨਾ_ਸ਼ੁਰੂ + 1, quotechar,
							      curbuf->b_p_qe);
	ਜੇ (ਖਾਨਾ_ਸਿਰਾ < 0)
	    ਜਾਓ abort_ਖੋਜ;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ਜੇ (include)
    {
	ਜੇ (VIM_ISWHITE(ਸਤਰ[ਖਾਨਾ_ਸਿਰਾ + 1]))
	    ਜਦੋਂ (VIM_ISWHITE(ਸਤਰ[ਖਾਨਾ_ਸਿਰਾ + 1]))
		++ਖਾਨਾ_ਸਿਰਾ;
	ਨਹੀਂਤਾਂ
	    ਜਦੋਂ (ਖਾਨਾ_ਸ਼ੁਰੂ > 0 && VIM_ISWHITE(ਸਤਰ[ਖਾਨਾ_ਸ਼ੁਰੂ - 1]))
		--ਖਾਨਾ_ਸ਼ੁਰੂ;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ਜੇ (!include && ਗਿਣਤੀ_2 < 2 && (vis_ਖਾਲੀ_ਥਾਵਾਂ || !inside_quotes))
	++ਖਾਨਾ_ਸ਼ੁਰੂ;
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ਜੇ (vis_ਖਾਲੀ_ਥਾਵਾਂ
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (ਸਤਰ[VIsual.ਖਾਨਾ] != quotechar
			    && (VIsual.ਖਾਨਾ == 0
				|| ਸਤਰ[VIsual.ਖਾਨਾ - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	oap->ਸ਼ੁਰੂ = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ_ਸਿਰਾ;
    ਜੇ ((include || ਗਿਣਤੀ_2 > 1 // After vi" another i" must include the ".
		|| (!vis_ਖਾਲੀ_ਥਾਵਾਂ && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (vis_ਖਾਲੀ_ਥਾਵਾਂ || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ਜੇ (*p_sel != 'e')
		dec_cursor();
	}
	ਨਹੀਂਤਾਂ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ਜੇ (inside_quotes
		    || (!selected_quote
			&& ਸਤਰ[VIsual.ਖਾਨਾ] != quotechar
			&& (ਸਤਰ[VIsual.ਖਾਨਾ] == NUL
			    || ਸਤਰ[VIsual.ਖਾਨਾ + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ_ਸ਼ੁਰੂ;
	}
	ਜੇ (VIsual_ਬਹੁਲਕ == 'V')
	{
	    VIsual_ਬਹੁਲਕ = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ਵਾਪਸ OK;

abort_ਖੋਜ:
    ਜੇ (VIsual_ਸਰਗਰਮ && *p_sel == 'e')
    {
	ਜੇ (did_exclusive_adj)
	    inc_cursor();
	ਜੇ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ਵਾਪਸ FALSE;
}
