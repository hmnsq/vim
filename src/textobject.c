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
findsent(पूर्णाङ्क dir, दीर्घ गन्ती)
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
    यदि (dir == FORWARD)
	func = incl;
    अन्यथा
	func = decl;

    जबसम्म (गन्ती--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	यदि (gchar_pos(&pos) == NUL)
	{
	    गर
		यदि ((*func)(&pos) == -1)
		    रोक;
	    जबसम्म (gchar_pos(&pos) == NUL);
	    यदि (dir == FORWARD)
		गच्छ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	अन्यथा यदि (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    यदि (pos.lnum == curbuf->b_ml.ml_पङ्क्ति_गन्ती)
		फर्काऊ FAIL;
	    ++pos.lnum;
	    गच्छ found;
	}
	अन्यथा यदि (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	जबसम्म (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != रिक्त)
	{
	    tpos = pos;
	    यदि (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		रोक;

	    यदि (found_dot)
		रोक;
	    यदि (vim_strchr((char_u *) ".!?", c) != रिक्त)
		found_dot = TRUE;

	    यदि (vim_strchr((char_u *) ")]\"'", c) != रिक्त
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == रिक्त)
		रोक;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != रिक्त;

	लागि (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    यदि (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		यदि (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		रोक;
	    }
	    यदि (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		गर
		    यदि ((c = inc(&tpos)) == -1)
			रोक;
		जबसम्म (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= रिक्त);
		यदि (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    यदि (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    रोक;
		}
	    }
	    यदि ((*func)(&pos) == -1)
	    {
		यदि (गन्ती)
		    फर्काऊ FAIL;
		noskip = TRUE;
		रोक;
	    }
	}
found:
	    // skip white space
	जबसम्म (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    यदि (incl(&pos) == -1)
		रोक;

	यदि (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    यदि ((*func)(&pos) == -1)
	    {
		यदि (गन्ती)
		    फर्काऊ FAIL;
		रोक;
	    }
	    ++गन्ती;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    फर्काऊ OK;
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
    दीर्घ	गन्ती,
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

    जबसम्म (गन्ती--)
    {
	did_skip = FALSE;
	लागि (first = TRUE; ; first = FALSE)
	{
	    यदि (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    यदि (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    यदि (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		रोक;

#ifdef FEAT_FOLDING
	    यदि (fold_skipped)
		curr -= dir;
#endif
	    यदि ((curr += dir) < 1 || curr > curbuf->b_ml.ml_पङ्क्ति_गन्ती)
	    {
		यदि (गन्ती)
		    फर्काऊ FALSE;
		curr -= dir;
		रोक;
	    }
	}
    }
    setpcmark();
    यदि (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    यदि (curr == curbuf->b_ml.ml_पङ्क्ति_गन्ती && what != '}' && dir == FORWARD)
    {
	char_u *पङ्क्ति = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	यदि ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(पङ्क्ति, पङ्क्ति + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    अन्यथा
	curwin->w_cursor.col = 0;
    फर्काऊ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    स्थिर पूर्णाङ्क
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    लागि (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	यदि (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    रोक;
	++macro;
	यदि (macro[0] == NUL)
	    रोक;
    }
    फर्काऊ (macro[0] != NUL);
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
    यदि (*s == para || *s == '\f' || (both && *s == '}'))
	फर्काऊ TRUE;
    यदि (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	फर्काऊ TRUE;
    फर्काऊ FALSE;
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
    यदि (c == ' ' || c == '\t' || c == NUL)
	फर्काऊ 0;
    यदि (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	यदि (enc_dbcs == DBCS_KOR && cls_bigword)
	    फर्काऊ 1;

	// process code leading/trailing bytes
	फर्काऊ dbcs_class(((अचिन्हित)c >> 8), (c & 0xFF));
    }
    यदि (enc_utf8)
    {
	c = utf_class(c);
	यदि (c != 0 && cls_bigword)
	    फर्काऊ 1;
	फर्काऊ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    यदि (cls_bigword)
	फर्काऊ 1;

    यदि (vim_iswordc(c))
	फर्काऊ 2;
    फर्काऊ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    पूर्णाङ्क
fwd_word(
    दीर्घ	गन्ती,
    पूर्णाङ्क		bigword,    // "W", "E" or "B"
    पूर्णाङ्क		eol)
{
    पूर्णाङ्क		sclass;	    // starting class
    पूर्णाङ्क		i;
    पूर्णाङ्क		last_पङ्क्ति;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जबसम्म (--गन्ती >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	यदि (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_पङ्क्ति = (curwin->w_cursor.lnum == curbuf->b_ml.ml_पङ्क्ति_गन्ती);
	i = inc_cursor();
	यदि (i == -1 || (i >= 1 && last_पङ्क्ति)) // started at last char in file
	    फर्काऊ FAIL;
	यदि (i >= 1 && eol && गन्ती == 0)      // started at last char in line
	    फर्काऊ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	यदि (sclass != 0)
	    जबसम्म (cls() == sclass)
	    {
		i = inc_cursor();
		यदि (i == -1 || (i >= 1 && eol && गन्ती == 0))
		    फर्काऊ OK;
	    }

	/*
	 * go to next non-white
	 */
	जबसम्म (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    यदि (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		रोक;

	    i = inc_cursor();
	    यदि (i == -1 || (i >= 1 && eol && गन्ती == 0))
		फर्काऊ OK;
	}
    }
    फर्काऊ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    पूर्णाङ्क
bck_word(दीर्घ गन्ती, पूर्णाङ्क bigword, पूर्णाङ्क stop)
{
    पूर्णाङ्क		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जबसम्म (--गन्ती >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	यदि (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, रिक्त))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	यदि (dec_cursor() == -1)		// started at start of file
	    फर्काऊ FAIL;

	यदि (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    जबसम्म (cls() == 0)
	    {
		यदि (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    गच्छ finished;
		यदि (dec_cursor() == -1) // hit start of file, stop here
		    फर्काऊ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    यदि (skip_chars(cls(), BACKWARD))
		फर्काऊ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    फर्काऊ OK;
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
    दीर्घ	गन्ती,
    पूर्णाङ्क		bigword,
    पूर्णाङ्क		stop,
    पूर्णाङ्क		empty)
{
    पूर्णाङ्क		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    यदि (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    जबसम्म (--गन्ती >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	यदि (hasFolding(curwin->w_cursor.lnum, रिक्त, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	यदि (inc_cursor() == -1)
	    फर्काऊ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	यदि (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    यदि (skip_chars(sclass, FORWARD))
		फर्काऊ FAIL;
	}
	अन्यथा यदि (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    जबसम्म (cls() == 0)
	    {
		यदि (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    गच्छ finished;
		यदि (inc_cursor() == -1)	    // hit end of file, stop here
		    फर्काऊ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    यदि (skip_chars(cls(), FORWARD))
		फर्काऊ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    फर्काऊ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    पूर्णाङ्क
bckend_word(
    दीर्घ	गन्ती,
    पूर्णाङ्क		bigword,    // TRUE for "B"
    पूर्णाङ्क		eol)	    // TRUE: stop at end of line.
{
    पूर्णाङ्क		sclass;	    // starting class
    पूर्णाङ्क		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    जबसम्म (--गन्ती >= 0)
    {
	sclass = cls();
	यदि ((i = dec_cursor()) == -1)
	    फर्काऊ FAIL;
	यदि (eol && i == 1)
	    फर्काऊ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	यदि (sclass != 0)
	{
	    जबसम्म (cls() == sclass)
		यदि ((i = dec_cursor()) == -1 || (eol && i == 1))
		    फर्काऊ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	जबसम्म (cls() == 0)
	{
	    यदि (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		रोक;
	    यदि ((i = dec_cursor()) == -1 || (eol && i == 1))
		फर्काऊ OK;
	}
    }
    adjust_skipcol();
    फर्काऊ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    स्थिर पूर्णाङ्क
skip_chars(पूर्णाङ्क cclass, पूर्णाङ्क dir)
{
    जबसम्म (cls() == cclass)
	यदि ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    फर्काऊ TRUE;
    फर्काऊ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    स्थिर शून्य
back_in_पङ्क्ति(शून्य)
{
    पूर्णाङ्क		sclass;		    // starting class

    sclass = cls();
    लागि (;;)
    {
	यदि (curwin->w_cursor.col == 0)	    // stop at start of line
	    रोक;
	dec_cursor();
	यदि (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    रोक;
	}
    }
}

    स्थिर शून्य
find_first_blank(pos_T *posp)
{
    पूर्णाङ्क	    c;

    जबसम्म (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	यदि (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    रोक;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    स्थिर शून्य
findsent_forward(
    दीर्घ    गन्ती,
    पूर्णाङ्क	    at_start_sent)	// cursor is at start of sentence
{
    जबसम्म (गन्ती--)
    {
	findsent(FORWARD, 1L);
	यदि (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	यदि (गन्ती == 0 || at_start_sent)
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
    दीर्घ	गन्ती,
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
    यदि (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    यदि (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_पङ्क्ति();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	यदि ((cls() == 0) == include)
	{
	    यदि (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		फर्काऊ FAIL;
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
	    यदि (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    अन्यथा
		oneleft();

	    यदि (include)
		include_white = TRUE;
	}

	यदि (VIsual_active)
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
	--गन्ती;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    जबसम्म (गन्ती > 0)
    {
	inclusive = TRUE;
	यदि (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    यदि (decl(&curwin->w_cursor) == -1)
		फर्काऊ FAIL;
	    यदि (include != (cls() != 0))
	    {
		यदि (bck_word(1L, bigword, TRUE) == FAIL)
		    फर्काऊ FAIL;
	    }
	    अन्यथा
	    {
		यदि (bckend_word(1L, bigword, TRUE) == FAIL)
		    फर्काऊ FAIL;
		(शून्य)incl(&curwin->w_cursor);
	    }
	}
	अन्यथा
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    यदि (incl(&curwin->w_cursor) == -1)
		फर्काऊ FAIL;
	    यदि (include != (cls() == 0))
	    {
		यदि (fwd_word(1L, bigword, TRUE) == FAIL && गन्ती > 1)
		    फर्काऊ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		यदि (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    अन्यथा
	    {
		यदि (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    फर्काऊ FAIL;
	    }
	}
	--गन्ती;
    }

    यदि (include_white && (cls() != 0
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
	यदि (oneleft() == OK)
	{
	    back_in_पङ्क्ति();
	    यदि (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		यदि (VIsual_active)
		    VIsual = curwin->w_cursor;
		अन्यथा
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    यदि (VIsual_active)
    {
	यदि (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	यदि (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    अन्यथा
	oap->inclusive = inclusive;

    फर्काऊ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    पूर्णाङ्क
current_sent(oparg_T *oap, दीर्घ गन्ती, पूर्णाङ्क include)
{
    pos_T	start_pos;
    pos_T	pos;
    पूर्णाङ्क		start_blank;
    पूर्णाङ्क		c;
    पूर्णाङ्क		at_start_sent;
    दीर्घ	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    यदि (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	यदि (LT_POS(start_pos, VIsual))
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
	    जबसम्म (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		यदि (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    रोक;
		}
		incl(&pos);
	    }
	    यदि (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		यदि (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		अन्यथा
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    यदि (include)	// "as" gets twice as much as "is"
		गन्ती *= 2;
	    जबसम्म (गन्ती--)
	    {
		यदि (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		यदि (!at_start_sent || (!include && !VIM_ISWHITE(c)))
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
	    यदि (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		जबसम्म (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    यदि (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			रोक;
		    }
		    incl(&pos);
		}
		यदि (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		अन्यथा		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    यदि (include)	// "as" gets twice as much as "is"
		गन्ती *= 2;
	    findsent_forward(गन्ती, at_start_sent);
	    यदि (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	फर्काऊ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    जबसम्म (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    यदि (EQUAL_POS(pos, curwin->w_cursor))
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
    यदि (include)
	ncount = गन्ती * 2;
    अन्यथा
    {
	ncount = गन्ती;
	यदि (start_blank)
	    --ncount;
    }
    यदि (ncount > 0)
	findsent_forward(ncount, TRUE);
    अन्यथा
	decl(&curwin->w_cursor);

    यदि (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	यदि (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    यदि (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	अन्यथा यदि (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    यदि (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	यदि (EQUAL_POS(start_pos, curwin->w_cursor))
	    गच्छ extend;
	यदि (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    अन्यथा
    {
	// include a newline after the sentence, if there is one
	यदि (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	अन्यथा
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    फर्काऊ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    पूर्णाङ्क
current_block(
    oparg_T	*oap,
    दीर्घ	गन्ती,
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
    यदि (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	यदि (what == '{')		// ignore indent
	    जबसम्म (inindent(1))
		यदि (inc_cursor() != 0)
		    रोक;
	यदि (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    अन्यथा यदि (LT_POS(VIsual, curwin->w_cursor))
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
    यदि ((pos = findmatch(रिक्त, what)) != रिक्त)
    {
	जबसम्म (गन्ती-- > 0)
	{
	    यदि ((pos = findmatch(रिक्त, what)) == रिक्त)
		रोक;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    अन्यथा
    {
	जबसम्म (गन्ती-- > 0)
	{
	    यदि ((pos = findmatchlimit(रिक्त, what, FM_FORWARD, 0)) == रिक्त)
		रोक;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    यदि (pos == रिक्त || (end_pos = findmatch(रिक्त, other)) == रिक्त)
    {
	curwin->w_cursor = old_pos;
	फर्काऊ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    जबसम्म (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	जबसम्म (inindent(1))
	{
	    sol = TRUE;
	    यदि (decl(&curwin->w_cursor) != 0)
		रोक;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	यदि (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    फर्काऊ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	यदि (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    यदि ((pos = findmatch(रिक्त, what)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		फर्काऊ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    यदि ((end_pos = findmatch(रिक्त, other)) == रिक्त)
	    {
		curwin->w_cursor = old_pos;
		फर्काऊ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	अन्यथा
	    रोक;
    }

    यदि (VIsual_active)
    {
	यदि (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	यदि (sol && gchar_cursor() != NUL)
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
	यदि (sol)
	    incl(&curwin->w_cursor);
	अन्यथा यदि (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	अन्यथा
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    फर्काऊ OK;
}

#यदि defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    स्थिर पूर्णाङ्क
in_html_tag(
    पूर्णाङ्क		end_tag)
{
    char_u	*पङ्क्ति = ml_get_curline();
    char_u	*p;
    पूर्णाङ्क		c;
    पूर्णाङ्क		lc = NUL;
    pos_T	pos;

    यदि (enc_dbcs)
    {
	char_u	*lp = रिक्त;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	लागि (p = पङ्क्ति; p < पङ्क्ति + curwin->w_cursor.col; MB_PTR_ADV(p))
	    यदि (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	यदि (*p != '<')	    // check for '<' under cursor
	{
	    यदि (lc != '<')
		फर्काऊ FALSE;
	    p = lp;
	}
    }
    अन्यथा
    {
	लागि (p = पङ्क्ति + curwin->w_cursor.col; p > पङ्क्ति; )
	{
	    यदि (*p == '<')	// find '<' under/before cursor
		रोक;
	    MB_PTR_BACK(पङ्क्ति, p);
	    यदि (*p == '>')	// find '>' before cursor
		रोक;
	}
	यदि (*p != '<')
	    फर्काऊ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - पङ्क्ति);

    MB_PTR_ADV(p);
    यदि (end_tag)
	// check that there is a '/' after the '<'
	फर्काऊ *p == '/';

    // check that there is no '/' after the '<'
    यदि (*p == '/')
	फर्काऊ FALSE;

    // check that the matching '>' is not preceded by '/'
    लागि (;;)
    {
	यदि (inc(&pos) < 0)
	    फर्काऊ FALSE;
	c = *ml_get_pos(&pos);
	यदि (c == '>')
	    रोक;
	lc = c;
    }
    फर्काऊ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    पूर्णाङ्क
current_tagblock(
    oparg_T	*oap,
    दीर्घ	गन्ती_arg,
    पूर्णाङ्क		include)	// TRUE == include white space
{
    दीर्घ	गन्ती = गन्ती_arg;
    दीर्घ	n;
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
    यदि (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    यदि (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	जबसम्म (inindent(1))
	    यदि (inc_cursor() != 0)
		रोक;

	यदि (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    जबसम्म (*ml_get_cursor() != '>')
		यदि (inc_cursor() < 0)
		    रोक;
	}
	अन्यथा यदि (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    जबसम्म (*ml_get_cursor() != '<')
		यदि (dec_cursor() < 0)
		    रोक;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    अन्यथा यदि (LT_POS(VIsual, curwin->w_cursor))
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
    लागि (n = 0; n < गन्ती; ++n)
    {
	यदि (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
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
    लागि (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (पूर्णाङ्क)(cp - p);
    यदि (len == 0)
    {
	curwin->w_cursor = old_pos;
	गच्छ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    यदि (spat == रिक्त || epat == रिक्त)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	गच्छ theend;
    }
    प्र_लिखो((वर्ण *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    प्र_लिखो((वर्ण *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, रिक्त,
						    0, रिक्त, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    यदि (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	गन्ती = 1;
	curwin->w_cursor = start_pos;
	गच्छ again;
    }

    यदि (do_include)
    {
	// Include up to the '>'.
	जबसम्म (*ml_get_cursor() != '>')
	    यदि (inc_cursor() < 0)
		रोक;
    }
    अन्यथा
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	यदि (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	अन्यथा यदि (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    यदि (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	पूर्णाङ्क in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	जबसम्म (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    यदि (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		रोक;
	    }
	    अन्यथा यदि (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	यदि (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    गन्ती = गन्ती_arg;
	    गच्छ again;
	}
    }

    यदि (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	यदि (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	अन्यथा यदि (*p_sel == 'e')
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
	यदि (LT_POS(end_pos, start_pos))
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
    फर्काऊ retval;
}
#endif

    पूर्णाङ्क
current_par(
    oparg_T	*oap,
    दीर्घ	गन्ती,
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

    यदि (type == 'S')	    // not implemented yet
	फर्काऊ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    यदि (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	यदि (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	अन्यथा
	    dir = FORWARD;
	लागि (i = गन्ती; --i >= 0; )
	{
	    यदि (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_पङ्क्ति_गन्ती))
	    {
		retval = FAIL;
		रोक;
	    }

	    prev_start_is_white = -1;
	    लागि (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		यदि (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    रोक;
		}
		लागि (;;)
		{
		    यदि (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_पङ्क्ति_गन्ती))
			रोक;
		    यदि (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			रोक;
		    start_lnum += dir;
		}
		यदि (!include)
		    रोक;
		यदि (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_पङ्क्ति_गन्ती))
		    रोक;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	फर्काऊ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    जबसम्म (start_lnum > 1)
    {
	यदि (white_in_front)	    // stop at first white line
	{
	    यदि (!linewhite(start_lnum - 1))
		रोक;
	}
	अन्यथा		// stop at first non-white line of start of paragraph
	{
	    यदि (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		रोक;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    जबसम्म (end_lnum <= curbuf->b_ml.ml_पङ्क्ति_गन्ती && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = गन्ती;
    यदि (!include && white_in_front)
	--i;
    जबसम्म (i--)
    {
	यदि (end_lnum == curbuf->b_ml.ml_पङ्क्ति_गन्ती)
	    फर्काऊ FAIL;

	यदि (!include)
	    do_white = linewhite(end_lnum + 1);

	यदि (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    जबसम्म (end_lnum < curbuf->b_ml.ml_पङ्क्ति_गन्ती
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	यदि (i == 0 && white_in_front && include)
	    रोक;

	/*
	 * skip to end of white lines after paragraph
	 */
	यदि (include || do_white)
	    जबसम्म (end_lnum < curbuf->b_ml.ml_पङ्क्ति_गन्ती
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    यदि (!white_in_front && !linewhite(end_lnum) && include)
	जबसम्म (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    यदि (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	यदि (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    गच्छ extend;
	यदि (VIsual.lnum != start_lnum)
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

    फर्काऊ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    स्थिर पूर्णाङ्क
find_next_quote(
    char_u	*पङ्क्ति,
    पूर्णाङ्क		col,
    पूर्णाङ्क		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णाङ्क		c;

    लागि (;;)
    {
	c = पङ्क्ति[col];
	यदि (c == NUL)
	    फर्काऊ -1;
	अन्यथा यदि (escape != रिक्त && vim_strchr(escape, c))
	{
	    ++col;
	    यदि (पङ्क्ति[col] == NUL)
		फर्काऊ -1;
	}
	अन्यथा यदि (c == quotechar)
	    रोक;
	यदि (has_mbyte)
	    col += (*mb_ptr2len)(पङ्क्ति + col);
	अन्यथा
	    ++col;
    }
    फर्काऊ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    स्थिर पूर्णाङ्क
find_prev_quote(
    char_u	*पङ्क्ति,
    पूर्णाङ्क		col_start,
    पूर्णाङ्क		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    पूर्णाङ्क		n;

    जबसम्म (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(पङ्क्ति, पङ्क्ति + col_start);
	n = 0;
	यदि (escape != रिक्त)
	    जबसम्म (col_start - n > 0 && vim_strchr(escape,
					     पङ्क्ति[col_start - n - 1]) != रिक्त)
	    ++n;
	यदि (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	अन्यथा यदि (पङ्क्ति[col_start] == quotechar)
	    रोक;
    }
    फर्काऊ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    पूर्णाङ्क
current_quote(
    oparg_T	*oap,
    दीर्घ	गन्ती,
    पूर्णाङ्क		include,	// TRUE == include quote char
    पूर्णाङ्क		quotechar)	// Quote character
{
    char_u	*पङ्क्ति = ml_get_curline();
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
    यदि (VIsual_active)
    {
	// this only works within one line
	यदि (VIsual.lnum != curwin->w_cursor.lnum)
	    फर्काऊ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	यदि (*p_sel == 'e')
	{
	    यदि (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    अन्यथा यदि (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    यदि (!vis_bef_curs && !vis_empty)
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

    यदि (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	यदि (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& पङ्क्ति[VIsual.col - 1] == quotechar
			&& पङ्क्ति[curwin->w_cursor.col] != NUL
			&& पङ्क्ति[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	अन्यथा
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& पङ्क्ति[curwin->w_cursor.col - 1] == quotechar
			&& पङ्क्ति[VIsual.col] != NUL
			&& पङ्क्ति[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	जबसम्म (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    यदि (पङ्क्ति[i] == NUL)
		रोक;
	    यदि (पङ्क्ति[i++] == quotechar)
	    {
		selected_quote = TRUE;
		रोक;
	    }
	}
    }

    यदि (!vis_empty && पङ्क्ति[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	यदि (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(पङ्क्ति, col_start + 1, quotechar, रिक्त);
	    यदि (col_start < 0)
		गच्छ abort_खोज;
	    col_end = find_next_quote(पङ्क्ति, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    यदि (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	अन्यथा
	{
	    col_end = find_prev_quote(पङ्क्ति, col_start, quotechar, रिक्त);
	    यदि (पङ्क्ति[col_end] != quotechar)
		गच्छ abort_खोज;
	    col_start = find_prev_quote(पङ्क्ति, col_end, quotechar,
							      curbuf->b_p_qe);
	    यदि (पङ्क्ति[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    अन्यथा

    यदि (पङ्क्ति[col_start] == quotechar || !vis_empty)
    {
	पूर्णाङ्क	first_col = col_start;

	यदि (!vis_empty)
	{
	    यदि (vis_bef_curs)
		first_col = find_next_quote(पङ्क्ति, col_start, quotechar, रिक्त);
	    अन्यथा
		first_col = find_prev_quote(पङ्क्ति, col_start, quotechar, रिक्त);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	लागि (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(पङ्क्ति, col_start, quotechar, रिक्त);
	    यदि (col_start < 0 || col_start > first_col)
		गच्छ abort_खोज;
	    // Find close quote character.
	    col_end = find_next_quote(पङ्क्ति, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    यदि (col_end < 0)
		गच्छ abort_खोज;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    यदि (col_start <= first_col && first_col <= col_end)
		रोक;
	    col_start = col_end + 1;
	}
    }
    अन्यथा
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(पङ्क्ति, col_start, quotechar, curbuf->b_p_qe);
	यदि (पङ्क्ति[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(पङ्क्ति, col_start, quotechar, रिक्त);
	    यदि (col_start < 0)
		गच्छ abort_खोज;
	}

	// Find close quote character.
	col_end = find_next_quote(पङ्क्ति, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	यदि (col_end < 0)
	    गच्छ abort_खोज;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    यदि (include)
    {
	यदि (VIM_ISWHITE(पङ्क्ति[col_end + 1]))
	    जबसम्म (VIM_ISWHITE(पङ्क्ति[col_end + 1]))
		++col_end;
	अन्यथा
	    जबसम्म (col_start > 0 && VIM_ISWHITE(पङ्क्ति[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    यदि (!include && गन्ती < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    यदि (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	यदि (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (पङ्क्ति[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| पङ्क्ति[VIsual.col - 1] != quotechar)))))
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
    यदि ((include || गन्ती > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    यदि (VIsual_active)
    {
	यदि (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    यदि (*p_sel != 'e')
		dec_cursor();
	}
	अन्यथा
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    यदि (inside_quotes
		    || (!selected_quote
			&& पङ्क्ति[VIsual.col] != quotechar
			&& (पङ्क्ति[VIsual.col] == NUL
			    || पङ्क्ति[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	यदि (VIsual_mode == 'V')
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

    फर्काऊ OK;

abort_खोज:
    यदि (VIsual_active && *p_sel == 'e')
    {
	यदि (did_exclusive_adj)
	    inc_cursor();
	यदि (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    फर्काऊ FALSE;
}
