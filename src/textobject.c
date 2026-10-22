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
#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cls(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_chars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findsent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, long ଗଣନା_2)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*func)(pos_T *);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startlnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		noskip = FALSE;	    // do not skip blanks
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpo_J;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_dot;

    pos = curwin->w_cursor;
    ଯଦି (dir == FORWARD)
	func = incl;
    ଅନ୍ୟଥା
	func = decl;

    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ଯଦି (gchar_pos(&pos) == NUL)
	{
	    କର
		ଯଦି ((*func)(&pos) == -1)
		    ଭାଙ୍ଗ;
	    ଯେତେବେଳେ (gchar_pos(&pos) == NUL);
	    ଯଦି (dir == FORWARD)
		ଯାଅ found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ଅନ୍ୟଥା ଯଦି (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ଯଦି (pos.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ଫେରନ୍ତୁ FAIL;
	    ++pos.lnum;
	    ଯାଅ found;
	}
	ଅନ୍ୟଥା ଯଦି (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ଯେତେବେଳେ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ଖାଲି)
	{
	    tpos = pos;
	    ଯଦି (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ଭାଙ୍ଗ;

	    ଯଦି (found_dot)
		ଭାଙ୍ଗ;
	    ଯଦି (vim_strchr((char_u *) ".!?", c) != ଖାଲି)
		found_dot = TRUE;

	    ଯଦି (vim_strchr((char_u *) ")]\"'", c) != ଖାଲି
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ଖାଲି)
		ଭାଙ୍ଗ;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ଖାଲି;

	ପାଇଁ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ଯଦି (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ଯଦି (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		କର
		    ଯଦି ((c = inc(&tpos)) == -1)
			ଭାଙ୍ଗ;
		ଯେତେବେଳେ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ଖାଲି);
		ଯଦି (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ଯଦି (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ଭାଙ୍ଗ;
		}
	    }
	    ଯଦି ((*func)(&pos) == -1)
	    {
		ଯଦି (ଗଣନା_2)
		    ଫେରନ୍ତୁ FAIL;
		noskip = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
found:
	    // skip white space
	ଯେତେବେଳେ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ଯଦି (incl(&pos) == -1)
		ଭାଙ୍ଗ;

	ଯଦି (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ଯଦି ((*func)(&pos) == -1)
	    {
		ଯଦି (ଗଣନା_2)
		    ଫେରନ୍ତୁ FAIL;
		ଭାଙ୍ଗ;
	    }
	    ++ଗଣନା_2;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ଫେରନ୍ତୁ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
findpar(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*pincl,	    // Return: TRUE if last char is to be included
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both)
{
    linenr_T	curr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_skip;   // TRUE after separating lines have been skipped
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first;	    // TRUE on first line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		posix = (vim_strchr(p_cpo, CPO_PARA) != ଖାଲି);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	did_skip = FALSE;
	ପାଇଁ (first = TRUE; ; first = FALSE)
	{
	    ଯଦି (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ଯଦି (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ଯଦି (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		ଭାଙ୍ଗ;

#ifdef FEAT_FOLDING
	    ଯଦି (fold_skipped)
		curr -= dir;
#endif
	    ଯଦି ((curr += dir) < 1 || curr > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		ଯଦି (ଗଣନା_2)
		    ଫେରନ୍ତୁ FALSE;
		curr -= dir;
		ଭାଙ୍ଗ;
	    }
	}
    }
    setpcmark();
    ଯଦି (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ଯଦି (curr == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା && what != '}' && dir == FORWARD)
    {
	char_u *ଧାଡ଼ି = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ଯଦି ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ଅନ୍ୟଥା
	curwin->w_cursor.col = 0;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ପାଇଁ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ଯଦି (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    ଭାଙ୍ଗ;
	++macro;
	ଯଦି (macro[0] == NUL)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
startPS(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା para, ପୂର୍ଣ୍ଣସଂଖ୍ୟା both)
{
    char_u	*s;

    s = ml_get(lnum);
    ଯଦି (*s == para || *s == '\f' || (both && *s == '}'))
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cls(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    c = gchar_cursor();
    ଯଦି (c == ' ' || c == '\t' || c == NUL)
	ଫେରନ୍ତୁ 0;
    ଯଦି (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ଯଦି (enc_dbcs == DBCS_KOR && cls_bigword)
	    ଫେରନ୍ତୁ 1;

	// process code leading/trailing bytes
	ଫେରନ୍ତୁ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ଯଦି (enc_utf8)
    {
	c = utf_class(c);
	ଯଦି (c != 0 && cls_bigword)
	    ଫେରନ୍ତୁ 1;
	ଫେରନ୍ତୁ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ଯଦି (cls_bigword)
	ଫେରନ୍ତୁ 1;

    ଯଦି (vim_iswordc(c))
	ଫେରନ୍ତୁ 2;
    ଫେରନ୍ତୁ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fwd_word(
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bigword,    // "W", "E" or "B"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eol)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sclass;	    // starting class
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_ଧାଡ଼ି;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ଯଦି (hasFolding(curwin->w_cursor.lnum, ଖାଲି, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_ଧାଡ଼ି = (curwin->w_cursor.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
	i = inc_cursor();
	ଯଦି (i == -1 || (i >= 1 && last_ଧାଡ଼ି)) // started at last char in file
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (i >= 1 && eol && ଗଣନା_2 == 0)      // started at last char in line
	    ଫେରନ୍ତୁ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ଯଦି (sclass != 0)
	    ଯେତେବେଳେ (cls() == sclass)
	    {
		i = inc_cursor();
		ଯଦି (i == -1 || (i >= 1 && eol && ଗଣନା_2 == 0))
		    ଫେରନ୍ତୁ OK;
	    }

	/*
	 * go to next non-white
	 */
	ଯେତେବେଳେ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ଯଦି (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		ଭାଙ୍ଗ;

	    i = inc_cursor();
	    ଯଦି (i == -1 || (i >= 1 && eol && ଗଣନା_2 == 0))
		ଫେରନ୍ତୁ OK;
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bck_word(long ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bigword, ପୂର୍ଣ୍ଣସଂଖ୍ୟା stop)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ଯଦି (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ଖାଲି))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ଯଦି (dec_cursor() == -1)		// started at start of file
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ଯେତେବେଳେ (cls() == 0)
	    {
		ଯଦି (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    ଯାଅ finished;
		ଯଦି (dec_cursor() == -1) // hit start of file, stop here
		    ଫେରନ୍ତୁ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ଯଦି (skip_chars(cls(), BACKWARD))
		ଫେରନ୍ତୁ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
end_word(
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bigword,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stop,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ଯଦି (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ଯଦି (hasFolding(curwin->w_cursor.lnum, ଖାଲି, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ଯଦି (inc_cursor() == -1)
	    ଫେରନ୍ତୁ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ଯଦି (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ଯଦି (skip_chars(sclass, FORWARD))
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ଯେତେବେଳେ (cls() == 0)
	    {
		ଯଦି (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    ଯାଅ finished;
		ଯଦି (inc_cursor() == -1)	    // hit end of file, stop here
		    ଫେରନ୍ତୁ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ଯଦି (skip_chars(cls(), FORWARD))
		ଫେରନ୍ତୁ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bckend_word(
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bigword,    // TRUE for "B"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eol)	    // TRUE: stop at end of line.
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sclass;	    // starting class
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ଯେତେବେଳେ (--ଗଣନା_2 >= 0)
    {
	sclass = cls();
	ଯଦି ((i = dec_cursor()) == -1)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (eol && i == 1)
	    ଫେରନ୍ତୁ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ଯଦି (sclass != 0)
	{
	    ଯେତେବେଳେ (cls() == sclass)
		ଯଦି ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ଫେରନ୍ତୁ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ଯେତେବେଳେ (cls() == 0)
	{
	    ଯଦି (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ଭାଙ୍ଗ;
	    ଯଦି ((i = dec_cursor()) == -1 || (eol && i == 1))
		ଫେରନ୍ତୁ OK;
	}
    }
    adjust_skipcol();
    ଫେରନ୍ତୁ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_chars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cclass, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
    ଯେତେବେଳେ (cls() == cclass)
	ଯଦି ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ସ୍ଥିର ଶୂନ୍ୟ
back_in_ଧାଡ଼ି(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sclass;		    // starting class

    sclass = cls();
    ପାଇଁ (;;)
    {
	ଯଦି (curwin->w_cursor.col == 0)	    // stop at start of line
	    ଭାଙ୍ଗ;
	dec_cursor();
	ଯଦି (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ଭାଙ୍ଗ;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_first_blank(pos_T *posp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;

    ଯେତେବେଳେ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ଯଦି (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ଭାଙ୍ଗ;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
findsent_forward(
    long    ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    at_start_sent)	// cursor is at start of sentence
{
    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	findsent(FORWARD, 1L);
	ଯଦି (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ଯଦି (ଗଣନା_2 == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_word(
    oparg_T	*oap,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include,	// TRUE: include word and white space
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inclusive = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ଯଦି (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ଯଦି (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_ଧାଡ଼ି();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ଯଦି ((cls() == 0) == include)
	{
	    ଯଦି (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ଯଦି (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ଅନ୍ୟଥା
		oneleft();

	    ଯଦି (include)
		include_white = TRUE;
	}

	ଯଦି (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ଅନ୍ୟଥା
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--ଗଣନା_2;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ଯେତେବେଳେ (ଗଣନା_2 > 0)
    {
	inclusive = TRUE;
	ଯଦି (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ଯଦି (decl(&curwin->w_cursor) == -1)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (include != (cls() != 0))
	    {
		ଯଦି (bck_word(1L, bigword, TRUE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (bckend_word(1L, bigword, TRUE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		(ଶୂନ୍ୟ)incl(&curwin->w_cursor);
	    }
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ଯଦି (incl(&curwin->w_cursor) == -1)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (include != (cls() == 0))
	    {
		ଯଦି (fwd_word(1L, bigword, TRUE) == FAIL && ଗଣନା_2 > 1)
		    ଫେରନ୍ତୁ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ଯଦି (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	--ଗଣନା_2;
    }

    ଯଦି (include_white && (cls() != 0
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
	ଯଦି (oneleft() == OK)
	{
	    back_in_ଧାଡ଼ି();
	    ଯଦି (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ଯଦି (VIsual_active)
		    VIsual = curwin->w_cursor;
		ଅନ୍ୟଥା
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ଯଦି (VIsual_active)
    {
	ଯଦି (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ଯଦି (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ଅନ୍ୟଥା
	oap->inclusive = inclusive;

    ଫେରନ୍ତୁ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_sent(oparg_T *oap, long ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା include)
{
    pos_T	start_pos;
    pos_T	pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_blank;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ଯଦି (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ଯଦି (LT_POS(start_pos, VIsual))
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
	    ଯେତେବେଳେ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ଯଦି (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    ଭାଙ୍ଗ;
		}
		incl(&pos);
	    }
	    ଯଦି (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ଯଦି (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ଅନ୍ୟଥା
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ଯଦି (include)	// "as" gets twice as much as "is"
		ଗଣନା_2 *= 2;
	    ଯେତେବେଳେ (ଗଣନା_2--)
	    {
		ଯଦି (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ଯଦି (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ଅନ୍ୟଥା
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
	    ଯଦି (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ଯେତେବେଳେ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ଯଦି (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			ଭାଙ୍ଗ;
		    }
		    incl(&pos);
		}
		ଯଦି (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ଅନ୍ୟଥା		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ଯଦି (include)	// "as" gets twice as much as "is"
		ଗଣନା_2 *= 2;
	    findsent_forward(ଗଣନା_2, at_start_sent);
	    ଯଦି (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ଫେରନ୍ତୁ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ଯେତେବେଳେ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ଯଦି (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ଅନ୍ୟଥା
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ଯଦି (include)
	ncount = ଗଣନା_2 * 2;
    ଅନ୍ୟଥା
    {
	ncount = ଗଣନା_2;
	ଯଦି (start_blank)
	    --ncount;
    }
    ଯଦି (ncount > 0)
	findsent_forward(ncount, TRUE);
    ଅନ୍ୟଥା
	decl(&curwin->w_cursor);

    ଯଦି (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ଯଦି (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ଯଦି (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ଅନ୍ୟଥା ଯଦି (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ଯଦି (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ଯଦି (EQUAL_POS(start_pos, curwin->w_cursor))
	    ଯାଅ extend;
	ଯଦି (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ଅନ୍ୟଥା
    {
	// include a newline after the sentence, if there is one
	ଯଦି (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ଅନ୍ୟଥା
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_block(
    oparg_T	*oap,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include,	// TRUE == include white space
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		what,		// '(', '{', etc.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ଖାଲି;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ଯଦି (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ଯଦି (what == '{')		// ignore indent
	    ଯେତେବେଳେ (inindent(1))
		ଯଦି (inc_cursor() != 0)
		    ଭାଙ୍ଗ;
	ଯଦି (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ଅନ୍ୟଥା ଯଦି (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ଅନ୍ୟଥା
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ଖାଲି ? "%M" : "%");
    ଯଦି ((pos = findmatch(ଖାଲି, what)) != ଖାଲି)
    {
	ଯେତେବେଳେ (ଗଣନା_2-- > 0)
	{
	    ଯଦି ((pos = findmatch(ଖାଲି, what)) == ଖାଲି)
		ଭାଙ୍ଗ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (ଗଣନା_2-- > 0)
	{
	    ଯଦି ((pos = findmatchlimit(ଖାଲି, what, FM_FORWARD, 0)) == ଖାଲି)
		ଭାଙ୍ଗ;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ଯଦି (pos == ଖାଲି || (end_pos = findmatch(ଖାଲି, other)) == ଖାଲି)
    {
	curwin->w_cursor = old_pos;
	ଫେରନ୍ତୁ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ଯେତେବେଳେ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ଯେତେବେଳେ (inindent(1))
	{
	    sol = TRUE;
	    ଯଦି (decl(&curwin->w_cursor) != 0)
		ଭାଙ୍ଗ;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ଯଦି (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ଫେରନ୍ତୁ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ଯଦି (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ଯଦି ((pos = findmatch(ଖାଲି, what)) == ଖାଲି)
	    {
		curwin->w_cursor = old_pos;
		ଫେରନ୍ତୁ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ଯଦି ((end_pos = findmatch(ଖାଲି, other)) == ଖାଲି)
	    {
		curwin->w_cursor = old_pos;
		ଫେରନ୍ତୁ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    ଯଦି (VIsual_active)
    {
	ଯଦି (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ଯଦି (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ଅନ୍ୟଥା
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ଯଦି (sol)
	    incl(&curwin->w_cursor);
	ଅନ୍ୟଥା ଯଦି (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ଅନ୍ୟଥା
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_html_tag(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_tag)
{
    char_u	*ଧାଡ଼ି = ml_get_curline();
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lc = NUL;
    pos_T	pos;

    ଯଦି (enc_dbcs)
    {
	char_u	*lp = ଖାଲି;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ପାଇଁ (p = ଧାଡ଼ି; p < ଧାଡ଼ି + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ଯଦି (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ଯଦି (*p != '<')	    // check for '<' under cursor
	{
	    ଯଦି (lc != '<')
		ଫେରନ୍ତୁ FALSE;
	    p = lp;
	}
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (p = ଧାଡ଼ି + curwin->w_cursor.col; p > ଧାଡ଼ି; )
	{
	    ଯଦି (*p == '<')	// find '<' under/before cursor
		ଭାଙ୍ଗ;
	    MB_PTR_BACK(ଧାଡ଼ି, p);
	    ଯଦି (*p == '>')	// find '>' before cursor
		ଭାଙ୍ଗ;
	}
	ଯଦି (*p != '<')
	    ଫେରନ୍ତୁ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - ଧାଡ଼ି);

    MB_PTR_ADV(p);
    ଯଦି (end_tag)
	// check that there is a '/' after the '<'
	ଫେରନ୍ତୁ *p == '/';

    // check that there is no '/' after the '<'
    ଯଦି (*p == '/')
	ଫେରନ୍ତୁ FALSE;

    // check that the matching '>' is not preceded by '/'
    ପାଇଁ (;;)
    {
	ଯଦି (inc(&pos) < 0)
	    ଫେରନ୍ତୁ FALSE;
	c = *ml_get_pos(&pos);
	ଯଦି (c == '>')
	    ଭାଙ୍ଗ;
	lc = c;
    }
    ଫେରନ୍ତୁ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_tagblock(
    oparg_T	*oap,
    long	ଗଣନା_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include)	// TRUE == include white space
{
    long	ଗଣନା_2 = ଗଣନା_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_include = include;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ws = p_ws;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ଯଦି (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ଯଦି (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ଯେତେବେଳେ (inindent(1))
	    ଯଦି (inc_cursor() != 0)
		ଭାଙ୍ଗ;

	ଯଦି (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ଯେତେବେଳେ (*ml_get_cursor() != '>')
		ଯଦି (inc_cursor() < 0)
		    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ଯେତେବେଳେ (*ml_get_cursor() != '<')
		ଯଦି (dec_cursor() < 0)
		    ଭାଙ୍ଗ;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ଅନ୍ୟଥା ଯଦି (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ଅନ୍ୟଥା
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ପାଇଁ (n = 0; n < ଗଣନା_2; ++n)
    {
	ଯଦି (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ଖାଲି, 0,
						  ଖାଲି, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    ଯାଅ theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    ପାଇଁ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cp - p);
    ଯଦି (len == 0)
    {
	curwin->w_cursor = old_pos;
	ଯାଅ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ଯଦି (spat == ଖାଲି || epat == ଖାଲି)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	ଯାଅ theend;
    }
    sprintf((ଅକ୍ଷର *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((ଅକ୍ଷର *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ଖାଲି,
						    0, ଖାଲି, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ଯଦି (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	ଗଣନା_2 = 1;
	curwin->w_cursor = start_pos;
	ଯାଅ again;
    }

    ଯଦି (do_include)
    {
	// Include up to the '>'.
	ଯେତେବେଳେ (*ml_get_cursor() != '>')
	    ଯଦି (inc_cursor() < 0)
		ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ଯଦି (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ଅନ୍ୟଥା ଯଦି (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ଯଦି (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ଯେତେବେଳେ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ଯଦି (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ଯଦି (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    ଗଣନା_2 = ଗଣନା_arg;
	    ଯାଅ again;
	}
    }

    ଯଦି (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ଯଦି (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ଅନ୍ୟଥା ଯଦି (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ଅନ୍ୟଥା
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ଯଦି (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ଅନ୍ୟଥା
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ଫେରନ୍ତୁ retval;
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_par(
    oparg_T	*oap,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include,	// TRUE == include white space
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		white_in_front;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_is_white;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_start_is_white;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_white = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (type == 'S')	    // not implemented yet
	ଫେରନ୍ତୁ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ଯଦି (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ଯଦି (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ଅନ୍ୟଥା
	    dir = FORWARD;
	ପାଇଁ (i = ଗଣନା_2; --i >= 0; )
	{
	    ଯଦି (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
	    {
		retval = FAIL;
		ଭାଙ୍ଗ;
	    }

	    prev_start_is_white = -1;
	    ପାଇଁ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ଯଦି (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    ଭାଙ୍ଗ;
		}
		ପାଇଁ (;;)
		{
		    ଯଦି (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
			ଭାଙ୍ଗ;
		    ଯଦି (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ଭାଙ୍ଗ;
		    start_lnum += dir;
		}
		ଯଦି (!include)
		    ଭାଙ୍ଗ;
		ଯଦି (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା))
		    ଭାଙ୍ଗ;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ଫେରନ୍ତୁ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ଯେତେବେଳେ (start_lnum > 1)
    {
	ଯଦି (white_in_front)	    // stop at first white line
	{
	    ଯଦି (!linewhite(start_lnum - 1))
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା		// stop at first non-white line of start of paragraph
	{
	    ଯଦି (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		ଭାଙ୍ଗ;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ଯେତେବେଳେ (end_lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = ଗଣନା_2;
    ଯଦି (!include && white_in_front)
	--i;
    ଯେତେବେଳେ (i--)
    {
	ଯଦି (end_lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (!include)
	    do_white = linewhite(end_lnum + 1);

	ଯଦି (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ଯେତେବେଳେ (end_lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ଯଦି (i == 0 && white_in_front && include)
	    ଭାଙ୍ଗ;

	/*
	 * skip to end of white lines after paragraph
	 */
	ଯଦି (include || do_white)
	    ଯେତେବେଳେ (end_lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ଯଦି (!white_in_front && !linewhite(end_lnum) && include)
	ଯେତେବେଳେ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ଯଦି (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ଯଦି (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    ଯାଅ extend;
	ଯଦି (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ଅନ୍ୟଥା
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ଫେରନ୍ତୁ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_next_quote(
    char_u	*ଧାଡ଼ି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ପାଇଁ (;;)
    {
	c = ଧାଡ଼ି[col];
	ଯଦି (c == NUL)
	    ଫେରନ୍ତୁ -1;
	ଅନ୍ୟଥା ଯଦି (escape != ଖାଲି && vim_strchr(escape, c))
	{
	    ++col;
	    ଯଦି (ଧାଡ଼ି[col] == NUL)
		ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା ଯଦି (c == quotechar)
	    ଭାଙ୍ଗ;
	ଯଦି (has_mbyte)
	    col += (*mb_ptr2len)(ଧାଡ଼ି + col);
	ଅନ୍ୟଥା
	    ++col;
    }
    ଫେରନ୍ତୁ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_prev_quote(
    char_u	*ଧାଡ଼ି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯେତେବେଳେ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + col_start);
	n = 0;
	ଯଦି (escape != ଖାଲି)
	    ଯେତେବେଳେ (col_start - n > 0 && vim_strchr(escape,
					     ଧାଡ଼ି[col_start - n - 1]) != ଖାଲି)
	    ++n;
	ଯଦି (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି[col_start] == quotechar)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
current_quote(
    oparg_T	*oap,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include,	// TRUE == include quote char
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quotechar)	// Quote character
{
    char_u	*ଧାଡ଼ି = ml_get_curline();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_start = curwin->w_cursor.col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inclusive = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vis_empty = TRUE;	// Visual selection <= 1 char
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vis_bef_curs = FALSE;	// Visual starts before cursor
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inside_quotes = FALSE;	// Looks like "i'" done before
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		selected_quote = FALSE;	// Has quote inside selection
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ଯଦି (VIsual_active)
    {
	// this only works within one line
	ଯଦି (VIsual.lnum != curwin->w_cursor.lnum)
	    ଫେରନ୍ତୁ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ଯଦି (*p_sel == 'e')
	{
	    ଯଦି (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ଯଦି (!vis_bef_curs && !vis_empty)
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

    ଯଦି (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ଯଦି (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& ଧାଡ଼ି[VIsual.col - 1] == quotechar
			&& ଧାଡ଼ି[curwin->w_cursor.col] != NUL
			&& ଧାଡ଼ି[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ଅନ୍ୟଥା
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& ଧାଡ଼ି[curwin->w_cursor.col - 1] == quotechar
			&& ଧାଡ଼ି[VIsual.col] != NUL
			&& ଧାଡ଼ି[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ଯେତେବେଳେ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ଯଦି (ଧାଡ଼ି[i] == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (ଧାଡ଼ି[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (!vis_empty && ଧାଡ଼ି[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ଯଦି (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(ଧାଡ଼ି, col_start + 1, quotechar, ଖାଲି);
	    ଯଦି (col_start < 0)
		ଯାଅ abort_ସନ୍ଧାନ;
	    col_end = find_next_quote(ଧାଡ଼ି, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ଯଦି (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ଅନ୍ୟଥା
	{
	    col_end = find_prev_quote(ଧାଡ଼ି, col_start, quotechar, ଖାଲି);
	    ଯଦି (ଧାଡ଼ି[col_end] != quotechar)
		ଯାଅ abort_ସନ୍ଧାନ;
	    col_start = find_prev_quote(ଧାଡ଼ି, col_end, quotechar,
							      curbuf->b_p_qe);
	    ଯଦି (ଧାଡ଼ି[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ଅନ୍ୟଥା

    ଯଦି (ଧାଡ଼ି[col_start] == quotechar || !vis_empty)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_col = col_start;

	ଯଦି (!vis_empty)
	{
	    ଯଦି (vis_bef_curs)
		first_col = find_next_quote(ଧାଡ଼ି, col_start, quotechar, ଖାଲି);
	    ଅନ୍ୟଥା
		first_col = find_prev_quote(ଧାଡ଼ି, col_start, quotechar, ଖାଲି);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	ପାଇଁ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(ଧାଡ଼ି, col_start, quotechar, ଖାଲି);
	    ଯଦି (col_start < 0 || col_start > first_col)
		ଯାଅ abort_ସନ୍ଧାନ;
	    // Find close quote character.
	    col_end = find_next_quote(ଧାଡ଼ି, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ଯଦି (col_end < 0)
		ଯାଅ abort_ସନ୍ଧାନ;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ଯଦି (col_start <= first_col && first_col <= col_end)
		ଭାଙ୍ଗ;
	    col_start = col_end + 1;
	}
    }
    ଅନ୍ୟଥା
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(ଧାଡ଼ି, col_start, quotechar, curbuf->b_p_qe);
	ଯଦି (ଧାଡ଼ି[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(ଧାଡ଼ି, col_start, quotechar, ଖାଲି);
	    ଯଦି (col_start < 0)
		ଯାଅ abort_ସନ୍ଧାନ;
	}

	// Find close quote character.
	col_end = find_next_quote(ଧାଡ଼ି, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ଯଦି (col_end < 0)
	    ଯାଅ abort_ସନ୍ଧାନ;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ଯଦି (include)
    {
	ଯଦି (VIM_ISWHITE(ଧାଡ଼ି[col_end + 1]))
	    ଯେତେବେଳେ (VIM_ISWHITE(ଧାଡ଼ି[col_end + 1]))
		++col_end;
	ଅନ୍ୟଥା
	    ଯେତେବେଳେ (col_start > 0 && VIM_ISWHITE(ଧାଡ଼ି[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ଯଦି (!include && ଗଣନା_2 < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ଯଦି (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ଯଦି (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (ଧାଡ଼ି[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| ଧାଡ଼ି[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ଅନ୍ୟଥା
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ଯଦି ((include || ଗଣନା_2 > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ଯଦି (VIsual_active)
    {
	ଯଦି (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ଯଦି (*p_sel != 'e')
		dec_cursor();
	}
	ଅନ୍ୟଥା
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ଯଦି (inside_quotes
		    || (!selected_quote
			&& ଧାଡ଼ି[VIsual.col] != quotechar
			&& (ଧାଡ଼ି[VIsual.col] == NUL
			    || ଧାଡ଼ି[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	ଯଦି (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ଅନ୍ୟଥା
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ଫେରନ୍ତୁ OK;

abort_ସନ୍ଧାନ:
    ଯଦି (VIsual_active && *p_sel == 'e')
    {
	ଯଦି (did_exclusive_adj)
	    inc_cursor();
	ଯଦି (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}
