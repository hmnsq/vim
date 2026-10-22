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
#అన్తర్భావ "vim.h"

స్థిర పూర్ణాంకం cls(శూన్య);
స్థిర పూర్ణాంకం skip_chars(పూర్ణాంకం, పూర్ణాంకం);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    పూర్ణాంకం
findsent(పూర్ణాంకం dir, దీర్ఘ లెక్క)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    పూర్ణాంకం		c;
    పూర్ణాంకం		(*func)(pos_T *);
    పూర్ణాంకం		startlnum;
    పూర్ణాంకం		noskip = FALSE;	    // do not skip blanks
    పూర్ణాంకం		cpo_J;
    పూర్ణాంకం		దొరికింది_dot;

    pos = curwin->w_cursor;
    ఒకవేళ (dir == FORWARD)
	func = incl;
    లేకపోతే
	func = decl;

    అయితే (లెక్క--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ఒకవేళ (gchar_pos(&pos) == NUL)
	{
	    చేయి
		ఒకవేళ ((*func)(&pos) == -1)
		    ఆపు;
	    అయితే (gchar_pos(&pos) == NUL);
	    ఒకవేళ (dir == FORWARD)
		వెళ్ళు దొరికింది;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	లేకపోతే ఒకవేళ (dir == FORWARD && pos.నిలువు == 0 &&
						ప్రారంభం_PS(pos.lnum, NUL, FALSE))
	{
	    ఒకవేళ (pos.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
		తిరిగి FAIL;
	    ++pos.lnum;
	    వెళ్ళు దొరికింది;
	}
	లేకపోతే ఒకవేళ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	దొరికింది_dot = FALSE;
	అయితే (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != ఖాళీ)
	{
	    tpos = pos;
	    ఒకవేళ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		ఆపు;

	    ఒకవేళ (దొరికింది_dot)
		ఆపు;
	    ఒకవేళ (vim_strchr((char_u *) ".!?", c) != ఖాళీ)
		దొరికింది_dot = TRUE;

	    ఒకవేళ (vim_strchr((char_u *) ")]\"'", c) != ఖాళీ
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == ఖాళీ)
		ఆపు;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != ఖాళీ;

	కొరకు (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ఒకవేళ (c == NUL || (pos.నిలువు == 0 && ప్రారంభం_PS(pos.lnum, NUL, FALSE)))
	    {
		ఒకవేళ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		ఆపు;
	    }
	    ఒకవేళ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		చేయి
		    ఒకవేళ ((c = inc(&tpos)) == -1)
			ఆపు;
		అయితే (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= ఖాళీ);
		ఒకవేళ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ఒకవేళ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    ఆపు;
		}
	    }
	    ఒకవేళ ((*func)(&pos) == -1)
	    {
		ఒకవేళ (లెక్క)
		    తిరిగి FAIL;
		noskip = TRUE;
		ఆపు;
	    }
	}
దొరికింది:
	    // skip white space
	అయితే (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ఒకవేళ (incl(&pos) == -1)
		ఆపు;

	ఒకవేళ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ఒకవేళ ((*func)(&pos) == -1)
	    {
		ఒకవేళ (లెక్క)
		    తిరిగి FAIL;
		ఆపు;
	    }
	    ++లెక్క;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    తిరిగి OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    పూర్ణాంకం
findpar(
    పూర్ణాంకం		*pincl,	    // Return: TRUE if last char is to be included
    పూర్ణాంకం		dir,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		what,
    పూర్ణాంకం		both)
{
    linenr_T	curr;
    పూర్ణాంకం		did_skip;   // TRUE after separating lines have been skipped
    పూర్ణాంకం		మొదటి;	    // TRUE on first line
    పూర్ణాంకం		posix = (vim_strchr(p_cpo, CPO_PARA) != ఖాళీ);
#నిర్వచితమైతే FEAT_FOLDING
    linenr_T	fold_మొదటి;	// first line of a closed fold
    linenr_T	fold_చివరి;	// last line of a closed fold
    పూర్ణాంకం		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#అయితే_ముగింపు

    curr = curwin->w_cursor.lnum;

    అయితే (లెక్క--)
    {
	did_skip = FALSE;
	కొరకు (మొదటి = TRUE; ; మొదటి = FALSE)
	{
	    ఒకవేళ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#నిర్వచితమైతే FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ఒకవేళ (మొదటి && hasFolding(curr, &fold_మొదటి, &fold_చివరి))
	    {
		curr = ((dir > 0) ? fold_చివరి : fold_మొదటి) + dir;
		fold_skipped = TRUE;
	    }
#అయితే_ముగింపు

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ఒకవేళ (!మొదటి && did_skip && (ప్రారంభం_PS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		ఆపు;

#నిర్వచితమైతే FEAT_FOLDING
	    ఒకవేళ (fold_skipped)
		curr -= dir;
#అయితే_ముగింపు
	    ఒకవేళ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		ఒకవేళ (లెక్క)
		    తిరిగి FALSE;
		curr -= dir;
		ఆపు;
	    }
	}
    }
    setpcmark();
    ఒకవేళ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ఒకవేళ (curr == curbuf->b_ml.ml_పంక్తి_లెక్క && what != '}' && dir == FORWARD)
    {
	char_u *పంక్తి = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ఒకవేళ ((curwin->w_cursor.నిలువు = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.నిలువు;
	    curwin->w_cursor.నిలువు -=
			     (*mb_తల_off)(పంక్తి, పంక్తి + curwin->w_cursor.నిలువు);
	    *pincl = TRUE;
	}
    }
    లేకపోతే
	curwin->w_cursor.నిలువు = 0;
    తిరిగి TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    స్థిర పూర్ణాంకం
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    కొరకు (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ఒకవేళ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    ఆపు;
	++macro;
	ఒకవేళ (macro[0] == NUL)
	    ఆపు;
    }
    తిరిగి (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    పూర్ణాంకం
ప్రారంభం_PS(linenr_T lnum, పూర్ణాంకం para, పూర్ణాంకం both)
{
    char_u	*s;

    s = ml_get(lnum);
    ఒకవేళ (*s == para || *s == '\f' || (both && *s == '}'))
	తిరిగి TRUE;
    ఒకవేళ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	తిరిగి TRUE;
    తిరిగి FALSE;
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

స్థిర పూర్ణాంకం	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    స్థిర పూర్ణాంకం
cls(శూన్య)
{
    పూర్ణాంకం	    c;

    c = gchar_cursor();
    ఒకవేళ (c == ' ' || c == '\t' || c == NUL)
	తిరిగి 0;
    ఒకవేళ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ఒకవేళ (enc_dbcs == DBCS_KOR && cls_bigword)
	    తిరిగి 1;

	// process code leading/trailing bytes
	తిరిగి dbcs_class(((గుర్తులేని)c >> 8), (c & 0xFF));
    }
    ఒకవేళ (enc_utf8)
    {
	c = utf_class(c);
	ఒకవేళ (c != 0 && cls_bigword)
	    తిరిగి 1;
	తిరిగి c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ఒకవేళ (cls_bigword)
	తిరిగి 1;

    ఒకవేళ (vim_iswordc(c))
	తిరిగి 2;
    తిరిగి 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    పూర్ణాంకం
fwd_మాట(
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		bigword,    // "W", "E" or "B"
    పూర్ణాంకం		eol)
{
    పూర్ణాంకం		sclass;	    // starting class
    పూర్ణాంకం		i;
    పూర్ణాంకం		చివరి_పంక్తి;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    అయితే (--లెక్క >= 0)
    {
#నిర్వచితమైతే FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ఒకవేళ (hasFolding(curwin->w_cursor.lnum, ఖాళీ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#అయితే_ముగింపు
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	చివరి_పంక్తి = (curwin->w_cursor.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క);
	i = inc_cursor();
	ఒకవేళ (i == -1 || (i >= 1 && చివరి_పంక్తి)) // started at last char in file
	    తిరిగి FAIL;
	ఒకవేళ (i >= 1 && eol && లెక్క == 0)      // started at last char in line
	    తిరిగి OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ఒకవేళ (sclass != 0)
	    అయితే (cls() == sclass)
	    {
		i = inc_cursor();
		ఒకవేళ (i == -1 || (i >= 1 && eol && లెక్క == 0))
		    తిరిగి OK;
	    }

	/*
	 * go to next non-white
	 */
	అయితే (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ఒకవేళ (curwin->w_cursor.నిలువు == 0 && *ml_get_curline() == NUL)
		ఆపు;

	    i = inc_cursor();
	    ఒకవేళ (i == -1 || (i >= 1 && eol && లెక్క == 0))
		తిరిగి OK;
	}
    }
    తిరిగి OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    పూర్ణాంకం
bck_మాట(దీర్ఘ లెక్క, పూర్ణాంకం bigword, పూర్ణాంకం stop)
{
    పూర్ణాంకం		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    అయితే (--లెక్క >= 0)
    {
#నిర్వచితమైతే FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ఒకవేళ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, ఖాళీ))
	    curwin->w_cursor.నిలువు = 0;
#అయితే_ముగింపు
	sclass = cls();
	ఒకవేళ (dec_cursor() == -1)		// started at start of file
	    తిరిగి FAIL;

	ఒకవేళ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    అయితే (cls() == 0)
	    {
		ఒకవేళ (curwin->w_cursor.నిలువు == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    వెళ్ళు ముగిసిన;
		ఒకవేళ (dec_cursor() == -1) // hit start of file, stop here
		    తిరిగి OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ఒకవేళ (skip_chars(cls(), BACKWARD))
		తిరిగి OK;
	}

	inc_cursor();			// overshot - forward one
ముగిసిన:
	stop = FALSE;
    }
    adjust_skipcol();
    తిరిగి OK;
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
    పూర్ణాంకం
చివర_మాట(
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		bigword,
    పూర్ణాంకం		stop,
    పూర్ణాంకం		ఖాళీలు_2)
{
    పూర్ణాంకం		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ఒకవేళ (*p_sel == 'e' && VIsual_చురుకైన && VIsual_బహుళకం == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    అయితే (--లెక్క >= 0)
    {
#నిర్వచితమైతే FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ఒకవేళ (hasFolding(curwin->w_cursor.lnum, ఖాళీ, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#అయితే_ముగింపు
	sclass = cls();
	ఒకవేళ (inc_cursor() == -1)
	    తిరిగి FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ఒకవేళ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ఒకవేళ (skip_chars(sclass, FORWARD))
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    అయితే (cls() == 0)
	    {
		ఒకవేళ (ఖాళీలు_2 && curwin->w_cursor.నిలువు == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    వెళ్ళు ముగిసిన;
		ఒకవేళ (inc_cursor() == -1)	    // hit end of file, stop here
		    తిరిగి FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ఒకవేళ (skip_chars(cls(), FORWARD))
		తిరిగి FAIL;
	}
	dec_cursor();			// overshot - one char backward
ముగిసిన:
	stop = FALSE;			// we move only one word less
    }
    తిరిగి OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    పూర్ణాంకం
bckend_మాట(
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		bigword,    // TRUE for "B"
    పూర్ణాంకం		eol)	    // TRUE: stop at end of line.
{
    పూర్ణాంకం		sclass;	    // starting class
    పూర్ణాంకం		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    అయితే (--లెక్క >= 0)
    {
	sclass = cls();
	ఒకవేళ ((i = dec_cursor()) == -1)
	    తిరిగి FAIL;
	ఒకవేళ (eol && i == 1)
	    తిరిగి OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ఒకవేళ (sclass != 0)
	{
	    అయితే (cls() == sclass)
		ఒకవేళ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    తిరిగి OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	అయితే (cls() == 0)
	{
	    ఒకవేళ (curwin->w_cursor.నిలువు == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		ఆపు;
	    ఒకవేళ ((i = dec_cursor()) == -1 || (eol && i == 1))
		తిరిగి OK;
	}
    }
    adjust_skipcol();
    తిరిగి OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    స్థిర పూర్ణాంకం
skip_chars(పూర్ణాంకం cclass, పూర్ణాంకం dir)
{
    అయితే (cls() == cclass)
	ఒకవేళ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    స్థిర శూన్య
వెనుక_లోపల_పంక్తి(శూన్య)
{
    పూర్ణాంకం		sclass;		    // starting class

    sclass = cls();
    కొరకు (;;)
    {
	ఒకవేళ (curwin->w_cursor.నిలువు == 0)	    // stop at start of line
	    ఆపు;
	dec_cursor();
	ఒకవేళ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    ఆపు;
	}
    }
}

    స్థిర శూన్య
కనుగొను_మొదటి_blank(pos_T *posp)
{
    పూర్ణాంకం	    c;

    అయితే (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ఒకవేళ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    ఆపు;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    స్థిర శూన్య
findsent_forward(
    దీర్ఘ    లెక్క,
    పూర్ణాంకం	    at_ప్రారంభం_పంపిన)	// cursor is at start of sentence
{
    అయితే (లెక్క--)
    {
	findsent(FORWARD, 1L);
	ఒకవేళ (at_ప్రారంభం_పంపిన)
	    కనుగొను_మొదటి_blank(&curwin->w_cursor);
	ఒకవేళ (లెక్క == 0 || at_ప్రారంభం_పంపిన)
	    decl(&curwin->w_cursor);
	at_ప్రారంభం_పంపిన = !at_ప్రారంభం_పంపిన;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    పూర్ణాంకం
current_మాట(
    oparg_T	*oap,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		include,	// TRUE: include word and white space
    పూర్ణాంకం		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	ప్రారంభం_pos;
    pos_T	pos;
    పూర్ణాంకం		inclusive = TRUE;
    పూర్ణాంకం		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&ప్రారంభం_pos);

    // Correct cursor when 'selection' is exclusive
    ఒకవేళ (VIsual_చురుకైన && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ఒకవేళ (!VIsual_చురుకైన || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	వెనుక_లోపల_పంక్తి();
	ప్రారంభం_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ఒకవేళ ((cls() == 0) == include)
	{
	    ఒకవేళ (చివర_మాట(1L, bigword, TRUE, TRUE) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_మాట(1L, bigword, TRUE);
	    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
		decl(&curwin->w_cursor);
	    లేకపోతే
		oneleft();

	    ఒకవేళ (include)
		include_white = TRUE;
	}

	ఒకవేళ (VIsual_చురుకైన)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = ప్రారంభం_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	లేకపోతే
	{
	    oap->ప్రారంభం = ప్రారంభం_pos;
	    oap->motion_type = MCHAR;
	}
	--లెక్క;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    అయితే (లెక్క > 0)
    {
	inclusive = TRUE;
	ఒకవేళ (VIsual_చురుకైన && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ఒకవేళ (decl(&curwin->w_cursor) == -1)
		తిరిగి FAIL;
	    ఒకవేళ (include != (cls() != 0))
	    {
		ఒకవేళ (bck_మాట(1L, bigword, TRUE) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (bckend_మాట(1L, bigword, TRUE) == FAIL)
		    తిరిగి FAIL;
		(శూన్య)incl(&curwin->w_cursor);
	    }
	}
	లేకపోతే
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ఒకవేళ (incl(&curwin->w_cursor) == -1)
		తిరిగి FAIL;
	    ఒకవేళ (include != (cls() == 0))
	    {
		ఒకవేళ (fwd_మాట(1L, bigword, TRUE) == FAIL && లెక్క > 1)
		    తిరిగి FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ఒకవేళ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (చివర_మాట(1L, bigword, TRUE, TRUE) == FAIL)
		    తిరిగి FAIL;
	    }
	}
	--లెక్క;
    }

    ఒకవేళ (include_white && (cls() != 0
		 || (curwin->w_cursor.నిలువు == 0 && !inclusive)))
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
	curwin->w_cursor = ప్రారంభం_pos;
	ఒకవేళ (oneleft() == OK)
	{
	    వెనుక_లోపల_పంక్తి();
	    ఒకవేళ (cls() == 0 && curwin->w_cursor.నిలువు > 0)
	    {
		ఒకవేళ (VIsual_చురుకైన)
		    VIsual = curwin->w_cursor;
		లేకపోతే
		    oap->ప్రారంభం = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ఒకవేళ (VIsual_బహుళకం == 'V')
	{
	    VIsual_బహుళకం = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    లేకపోతే
	oap->inclusive = inclusive;

    తిరిగి OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    పూర్ణాంకం
current_పంపిన(oparg_T *oap, దీర్ఘ లెక్క, పూర్ణాంకం include)
{
    pos_T	ప్రారంభం_pos;
    pos_T	pos;
    పూర్ణాంకం		ప్రారంభం_blank;
    పూర్ణాంకం		c;
    పూర్ణాంకం		at_ప్రారంభం_పంపిన;
    దీర్ఘ	ncount;

    ప్రారంభం_pos = curwin->w_cursor;
    pos = ప్రారంభం_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ఒకవేళ (VIsual_చురుకైన && !EQUAL_POS(ప్రారంభం_pos, VIsual))
    {
extend:
	ఒకవేళ (LT_POS(ప్రారంభం_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_ప్రారంభం_పంపిన = TRUE;
	    decl(&pos);
	    అయితే (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ఒకవేళ (!VIM_ISWHITE(c))
		{
		    at_ప్రారంభం_పంపిన = FALSE;
		    ఆపు;
		}
		incl(&pos);
	    }
	    ఒకవేళ (!at_ప్రారంభం_పంపిన)
	    {
		findsent(BACKWARD, 1L);
		ఒకవేళ (EQUAL_POS(curwin->w_cursor, ప్రారంభం_pos))
		    at_ప్రారంభం_పంపిన = TRUE;  // exactly at start of sentence
		లేకపోతే
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ఒకవేళ (include)	// "as" gets twice as much as "is"
		లెక్క *= 2;
	    అయితే (లెక్క--)
	    {
		ఒకవేళ (at_ప్రారంభం_పంపిన)
		    కనుగొను_మొదటి_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ఒకవేళ (!at_ప్రారంభం_పంపిన || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_ప్రారంభం_పంపిన = !at_ప్రారంభం_పంపిన;
	    }
	}
	లేకపోతే
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_ప్రారంభం_పంపిన = TRUE;
	    // not just before a sentence
	    ఒకవేళ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_ప్రారంభం_పంపిన = FALSE;
		అయితే (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ఒకవేళ (!VIM_ISWHITE(c))
		    {
			at_ప్రారంభం_పంపిన = TRUE;
			ఆపు;
		    }
		    incl(&pos);
		}
		ఒకవేళ (at_ప్రారంభం_పంపిన)	// in the sentence
		    findsent(BACKWARD, 1L);
		లేకపోతే		// in/before white before a sentence
		    curwin->w_cursor = ప్రారంభం_pos;
	    }

	    ఒకవేళ (include)	// "as" gets twice as much as "is"
		లెక్క *= 2;
	    findsent_forward(లెక్క, at_ప్రారంభం_పంపిన);
	    ఒకవేళ (*p_sel == 'e')
		++curwin->w_cursor.నిలువు;
	}
	తిరిగి OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    అయితే (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ఒకవేళ (EQUAL_POS(pos, curwin->w_cursor))
    {
	ప్రారంభం_blank = TRUE;
	కనుగొను_మొదటి_blank(&ప్రారంభం_pos);	// go back to first blank
    }
    లేకపోతే
    {
	ప్రారంభం_blank = FALSE;
	findsent(BACKWARD, 1L);
	ప్రారంభం_pos = curwin->w_cursor;
    }
    ఒకవేళ (include)
	ncount = లెక్క * 2;
    లేకపోతే
    {
	ncount = లెక్క;
	ఒకవేళ (ప్రారంభం_blank)
	    --ncount;
    }
    ఒకవేళ (ncount > 0)
	findsent_forward(ncount, TRUE);
    లేకపోతే
	decl(&curwin->w_cursor);

    ఒకవేళ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ఒకవేళ (ప్రారంభం_blank)
	{
	    కనుగొను_మొదటి_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ఒకవేళ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	లేకపోతే ఒకవేళ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    కనుగొను_మొదటి_blank(&ప్రారంభం_pos);
    }

    ఒకవేళ (VIsual_చురుకైన)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ఒకవేళ (EQUAL_POS(ప్రారంభం_pos, curwin->w_cursor))
	    వెళ్ళు extend;
	ఒకవేళ (*p_sel == 'e')
	    ++curwin->w_cursor.నిలువు;
	VIsual = ప్రారంభం_pos;
	VIsual_బహుళకం = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    లేకపోతే
    {
	// include a newline after the sentence, if there is one
	ఒకవేళ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	లేకపోతే
	    oap->inclusive = FALSE;
	oap->ప్రారంభం = ప్రారంభం_pos;
	oap->motion_type = MCHAR;
    }
    తిరిగి OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    పూర్ణాంకం
current_ఖండం(
    oparg_T	*oap,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		include,	// TRUE == include white space
    పూర్ణాంకం		what,		// '(', '{', etc.
    పూర్ణాంకం		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = ఖాళీ;
    pos_T	ప్రారంభం_pos;
    pos_T	*చివర_pos;
    pos_T	old_ప్రారంభం, old_చివర;
    char_u	*save_cpo;
    పూర్ణాంకం		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_చివర = curwin->w_cursor;		// remember where we started
    old_ప్రారంభం = old_చివర;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ఒకవేళ (!VIsual_చురుకైన || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ఒకవేళ (what == '{')		// ignore indent
	    అయితే (inindent(1))
		ఒకవేళ (inc_cursor() != 0)
		    ఆపు;
	ఒకవేళ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.నిలువు;
    }
    లేకపోతే ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ప్రారంభం = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    లేకపోతే
	old_చివర = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != ఖాళీ ? "%M" : "%");
    ఒకవేళ ((pos = findmatch(ఖాళీ, what)) != ఖాళీ)
    {
	అయితే (లెక్క-- > 0)
	{
	    ఒకవేళ ((pos = findmatch(ఖాళీ, what)) == ఖాళీ)
		ఆపు;
	    curwin->w_cursor = *pos;
	    ప్రారంభం_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    లేకపోతే
    {
	అయితే (లెక్క-- > 0)
	{
	    ఒకవేళ ((pos = findmatchlimit(ఖాళీ, what, FM_FORWARD, 0)) == ఖాళీ)
		ఆపు;
	    curwin->w_cursor = *pos;
	    ప్రారంభం_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ఒకవేళ (pos == ఖాళీ || (చివర_pos = findmatch(ఖాళీ, other)) == ఖాళీ)
    {
	curwin->w_cursor = old_pos;
	తిరిగి FAIL;
    }
    curwin->w_cursor = *చివర_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    అయితే (!include)
    {
	incl(&ప్రారంభం_pos);
	sol = (curwin->w_cursor.నిలువు == 0);
	decl(&curwin->w_cursor);
	అయితే (inindent(1))
	{
	    sol = TRUE;
	    ఒకవేళ (decl(&curwin->w_cursor) != 0)
		ఆపు;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ఒకవేళ (EQUAL_POS(ప్రారంభం_pos, *చివర_pos) && VIsual_చురుకైన)
	{
	    curwin->w_cursor = old_pos;
	    తిరిగి FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ఒకవేళ (!LT_POS(ప్రారంభం_pos, old_ప్రారంభం) && !LT_POS(old_చివర, curwin->w_cursor)
		&& !EQUAL_POS(ప్రారంభం_pos, curwin->w_cursor)
		&& VIsual_చురుకైన)
	{
	    curwin->w_cursor = old_ప్రారంభం;
	    decl(&curwin->w_cursor);
	    ఒకవేళ ((pos = findmatch(ఖాళీ, what)) == ఖాళీ)
	    {
		curwin->w_cursor = old_pos;
		తిరిగి FAIL;
	    }
	    ప్రారంభం_pos = *pos;
	    curwin->w_cursor = *pos;
	    ఒకవేళ ((చివర_pos = findmatch(ఖాళీ, other)) == ఖాళీ)
	    {
		curwin->w_cursor = old_pos;
		తిరిగి FAIL;
	    }
	    curwin->w_cursor = *చివర_pos;
	}
	లేకపోతే
	    ఆపు;
    }

    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ఒకవేళ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = ప్రారంభం_pos;
	VIsual_బహుళకం = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    లేకపోతే
    {
	oap->ప్రారంభం = ప్రారంభం_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ఒకవేళ (sol)
	    incl(&curwin->w_cursor);
	లేకపోతే ఒకవేళ (LTOREQ_POS(ప్రారంభం_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	లేకపోతే
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = ప్రారంభం_pos;
    }

    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    స్థిర పూర్ణాంకం
లోపల_html_ట్యాగ్(
    పూర్ణాంకం		చివర_ట్యాగ్)
{
    char_u	*పంక్తి = ml_get_curline();
    char_u	*p;
    పూర్ణాంకం		c;
    పూర్ణాంకం		lc = NUL;
    pos_T	pos;

    ఒకవేళ (enc_dbcs)
    {
	char_u	*lp = ఖాళీ;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	కొరకు (p = పంక్తి; p < పంక్తి + curwin->w_cursor.నిలువు; MB_PTR_ADV(p))
	    ఒకవేళ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ఒకవేళ (*p != '<')	    // check for '<' under cursor
	{
	    ఒకవేళ (lc != '<')
		తిరిగి FALSE;
	    p = lp;
	}
    }
    లేకపోతే
    {
	కొరకు (p = పంక్తి + curwin->w_cursor.నిలువు; p > పంక్తి; )
	{
	    ఒకవేళ (*p == '<')	// find '<' under/before cursor
		ఆపు;
	    MB_PTR_వెనుక(పంక్తి, p);
	    ఒకవేళ (*p == '>')	// find '>' before cursor
		ఆపు;
	}
	ఒకవేళ (*p != '<')
	    తిరిగి FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.నిలువు = (colnr_T)(p - పంక్తి);

    MB_PTR_ADV(p);
    ఒకవేళ (చివర_ట్యాగ్)
	// check that there is a '/' after the '<'
	తిరిగి *p == '/';

    // check that there is no '/' after the '<'
    ఒకవేళ (*p == '/')
	తిరిగి FALSE;

    // check that the matching '>' is not preceded by '/'
    కొరకు (;;)
    {
	ఒకవేళ (inc(&pos) < 0)
	    తిరిగి FALSE;
	c = *ml_get_pos(&pos);
	ఒకవేళ (c == '>')
	    ఆపు;
	lc = c;
    }
    తిరిగి lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    పూర్ణాంకం
current_tagblock(
    oparg_T	*oap,
    దీర్ఘ	లెక్క_arg,
    పూర్ణాంకం		include)	// TRUE == include white space
{
    దీర్ఘ	లెక్క = లెక్క_arg;
    దీర్ఘ	n;
    pos_T	old_pos;
    pos_T	ప్రారంభం_pos;
    pos_T	చివర_pos;
    pos_T	old_ప్రారంభం, old_చివర;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    పూర్ణాంకం		len;
    పూర్ణాంకం		r;
    పూర్ణాంకం		do_include = include;
    పూర్ణాంకం		save_p_ws = p_ws;
    పూర్ణాంకం		retval = FAIL;
    పూర్ణాంకం		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_చివర = curwin->w_cursor;		    // remember where we started
    old_ప్రారంభం = old_చివర;
    ఒకవేళ (!VIsual_చురుకైన || *p_sel == 'e')
	decl(&old_చివర);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ఒకవేళ (!VIsual_చురుకైన || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	అయితే (inindent(1))
	    ఒకవేళ (inc_cursor() != 0)
		ఆపు;

	ఒకవేళ (లోపల_html_ట్యాగ్(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    అయితే (*ml_get_cursor() != '>')
		ఒకవేళ (inc_cursor() < 0)
		    ఆపు;
	}
	లేకపోతే ఒకవేళ (లోపల_html_ట్యాగ్(TRUE))
	{
	    // cursor on end tag, move to just before it
	    అయితే (*ml_get_cursor() != '<')
		ఒకవేళ (dec_cursor() < 0)
		    ఆపు;
	    dec_cursor();
	    old_చివర = curwin->w_cursor;
	}
    }
    లేకపోతే ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_ప్రారంభం = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    లేకపోతే
	old_చివర = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    కొరకు (n = 0; n < లెక్క; ++n)
    {
	ఒకవేళ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, ఖాళీ, 0,
						  ఖాళీ, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    వెళ్ళు theend;
	}
    }
    ప్రారంభం_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    కొరకు (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (పూర్ణాంకం)(cp - p);
    ఒకవేళ (len == 0)
    {
	curwin->w_cursor = old_pos;
	వెళ్ళు theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ఒకవేళ (spat == ఖాళీ || epat == ఖాళీ)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	వెళ్ళు theend;
    }
    sprintf((అక్షరం *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((అక్షరం *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, ఖాళీ,
						    0, ఖాళీ, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ఒకవేళ (r < 1 || LT_POS(curwin->w_cursor, old_చివర))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	లెక్క = 1;
	curwin->w_cursor = ప్రారంభం_pos;
	వెళ్ళు again;
    }

    ఒకవేళ (do_include)
    {
	// Include up to the '>'.
	అయితే (*ml_get_cursor() != '>')
	    ఒకవేళ (inc_cursor() < 0)
		ఆపు;
    }
    లేకపోతే
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ఒకవేళ (*c == '<' && !VIsual_చురుకైన && curwin->w_cursor.నిలువు == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	లేకపోతే ఒకవేళ (*c == '<')
	    dec_cursor();
    }
    చివర_pos = curwin->w_cursor;

    ఒకవేళ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	పూర్ణాంకం లోపల_quotes = FALSE;
	curwin->w_cursor = ప్రారంభం_pos;
	అయితే (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ఒకవేళ (*p == '>' && !లోపల_quotes)
	    {
		inc_cursor();
		ప్రారంభం_pos = curwin->w_cursor;
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (*p == '"' || *p == '\'')
		లోపల_quotes = !లోపల_quotes;
	}
	curwin->w_cursor = చివర_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ఒకవేళ (VIsual_చురుకైన && EQUAL_POS(ప్రారంభం_pos, old_ప్రారంభం)
						&& EQUAL_POS(చివర_pos, old_చివర))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_ప్రారంభం;
	    లెక్క = లెక్క_arg;
	    వెళ్ళు again;
	}
    }

    ఒకవేళ (VIsual_చురుకైన)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ఒకవేళ (LT_POS(చివర_pos, ప్రారంభం_pos))
	    curwin->w_cursor = ప్రారంభం_pos;
	లేకపోతే ఒకవేళ (*p_sel == 'e')
	    inc_cursor();
	VIsual = ప్రారంభం_pos;
	VIsual_బహుళకం = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    లేకపోతే
    {
	oap->ప్రారంభం = ప్రారంభం_pos;
	oap->motion_type = MCHAR;
	ఒకవేళ (LT_POS(చివర_pos, ప్రారంభం_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = ప్రారంభం_pos;
	    oap->inclusive = FALSE;
	}
	లేకపోతే
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    తిరిగి retval;
}
#అయితే_ముగింపు

    పూర్ణాంకం
current_par(
    oparg_T	*oap,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		include,	// TRUE == include white space
    పూర్ణాంకం		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	ప్రారంభం_lnum;
    linenr_T	చివర_lnum;
    పూర్ణాంకం		white_లోపల_front;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		ప్రారంభం_is_white;
    పూర్ణాంకం		prev_ప్రారంభం_is_white;
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		do_white = FALSE;
    పూర్ణాంకం		t;
    పూర్ణాంకం		i;

    ఒకవేళ (type == 'S')	    // not implemented yet
	తిరిగి FAIL;

    ప్రారంభం_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ఒకవేళ (VIsual_చురుకైన && ప్రారంభం_lnum != VIsual.lnum)
    {
extend:
	ఒకవేళ (ప్రారంభం_lnum < VIsual.lnum)
	    dir = BACKWARD;
	లేకపోతే
	    dir = FORWARD;
	కొరకు (i = లెక్క; --i >= 0; )
	{
	    ఒకవేళ (ప్రారంభం_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_పంక్తి_లెక్క))
	    {
		retval = FAIL;
		ఆపు;
	    }

	    prev_ప్రారంభం_is_white = -1;
	    కొరకు (t = 0; t < 2; ++t)
	    {
		ప్రారంభం_lnum += dir;
		ప్రారంభం_is_white = linewhite(ప్రారంభం_lnum);
		ఒకవేళ (prev_ప్రారంభం_is_white == ప్రారంభం_is_white)
		{
		    ప్రారంభం_lnum -= dir;
		    ఆపు;
		}
		కొరకు (;;)
		{
		    ఒకవేళ (ప్రారంభం_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_పంక్తి_లెక్క))
			ఆపు;
		    ఒకవేళ (ప్రారంభం_is_white != linewhite(ప్రారంభం_lnum + dir)
			    || (!ప్రారంభం_is_white
				    && ప్రారంభం_PS(ప్రారంభం_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			ఆపు;
		    ప్రారంభం_lnum += dir;
		}
		ఒకవేళ (!include)
		    ఆపు;
		ఒకవేళ (ప్రారంభం_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_పంక్తి_లెక్క))
		    ఆపు;
		prev_ప్రారంభం_is_white = ప్రారంభం_is_white;
	    }
	}
	curwin->w_cursor.lnum = ప్రారంభం_lnum;
	curwin->w_cursor.నిలువు = 0;
	తిరిగి retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_లోపల_front = linewhite(ప్రారంభం_lnum);
    అయితే (ప్రారంభం_lnum > 1)
    {
	ఒకవేళ (white_లోపల_front)	    // stop at first white line
	{
	    ఒకవేళ (!linewhite(ప్రారంభం_lnum - 1))
		ఆపు;
	}
	లేకపోతే		// stop at first non-white line of start of paragraph
	{
	    ఒకవేళ (linewhite(ప్రారంభం_lnum - 1) || ప్రారంభం_PS(ప్రారంభం_lnum, 0, 0))
		ఆపు;
	}
	--ప్రారంభం_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    చివర_lnum = ప్రారంభం_lnum;
    అయితే (చివర_lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క && linewhite(చివర_lnum))
	++చివర_lnum;

    --చివర_lnum;
    i = లెక్క;
    ఒకవేళ (!include && white_లోపల_front)
	--i;
    అయితే (i--)
    {
	ఒకవేళ (చివర_lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
	    తిరిగి FAIL;

	ఒకవేళ (!include)
	    do_white = linewhite(చివర_lnum + 1);

	ఒకవేళ (include || !do_white)
	{
	    ++చివర_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    అయితే (చివర_lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
		    && !linewhite(చివర_lnum + 1)
		    && !ప్రారంభం_PS(చివర_lnum + 1, 0, 0))
		++చివర_lnum;
	}

	ఒకవేళ (i == 0 && white_లోపల_front && include)
	    ఆపు;

	/*
	 * skip to end of white lines after paragraph
	 */
	ఒకవేళ (include || do_white)
	    అయితే (చివర_lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
						   && linewhite(చివర_lnum + 1))
		++చివర_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ఒకవేళ (!white_లోపల_front && !linewhite(చివర_lnum) && include)
	అయితే (ప్రారంభం_lnum > 1 && linewhite(ప్రారంభం_lnum - 1))
	    --ప్రారంభం_lnum;

    ఒకవేళ (VIsual_చురుకైన)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ఒకవేళ (VIsual_బహుళకం == 'V' && ప్రారంభం_lnum == curwin->w_cursor.lnum)
	    వెళ్ళు extend;
	ఒకవేళ (VIsual.lnum != ప్రారంభం_lnum)
	{
	    VIsual.lnum = ప్రారంభం_lnum;
	    VIsual.నిలువు = 0;
	}
	VIsual_బహుళకం = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    లేకపోతే
    {
	oap->ప్రారంభం.lnum = ప్రారంభం_lnum;
	oap->ప్రారంభం.నిలువు = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = చివర_lnum;
    curwin->w_cursor.నిలువు = 0;

    తిరిగి OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    స్థిర పూర్ణాంకం
కనుగొను_తదుపరి_quote(
    char_u	*పంక్తి,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    పూర్ణాంకం		c;

    కొరకు (;;)
    {
	c = పంక్తి[నిలువు];
	ఒకవేళ (c == NUL)
	    తిరిగి -1;
	లేకపోతే ఒకవేళ (escape != ఖాళీ && vim_strchr(escape, c))
	{
	    ++నిలువు;
	    ఒకవేళ (పంక్తి[నిలువు] == NUL)
		తిరిగి -1;
	}
	లేకపోతే ఒకవేళ (c == quotechar)
	    ఆపు;
	ఒకవేళ (has_mbyte)
	    నిలువు += (*mb_ptr2len)(పంక్తి + నిలువు);
	లేకపోతే
	    ++నిలువు;
    }
    తిరిగి నిలువు;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    స్థిర పూర్ణాంకం
కనుగొను_prev_quote(
    char_u	*పంక్తి,
    పూర్ణాంకం		నిలువు_ప్రారంభం,
    పూర్ణాంకం		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    పూర్ణాంకం		n;

    అయితే (నిలువు_ప్రారంభం > 0)
    {
	--నిలువు_ప్రారంభం;
	నిలువు_ప్రారంభం -= (*mb_తల_off)(పంక్తి, పంక్తి + నిలువు_ప్రారంభం);
	n = 0;
	ఒకవేళ (escape != ఖాళీ)
	    అయితే (నిలువు_ప్రారంభం - n > 0 && vim_strchr(escape,
					     పంక్తి[నిలువు_ప్రారంభం - n - 1]) != ఖాళీ)
	    ++n;
	ఒకవేళ (n & 1)
	    నిలువు_ప్రారంభం -= n;	// uneven number of escape chars, skip it
	లేకపోతే ఒకవేళ (పంక్తి[నిలువు_ప్రారంభం] == quotechar)
	    ఆపు;
    }
    తిరిగి నిలువు_ప్రారంభం;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    పూర్ణాంకం
current_quote(
    oparg_T	*oap,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		include,	// TRUE == include quote char
    పూర్ణాంకం		quotechar)	// Quote character
{
    char_u	*పంక్తి = ml_get_curline();
    పూర్ణాంకం		నిలువు_చివర;
    పూర్ణాంకం		నిలువు_ప్రారంభం = curwin->w_cursor.నిలువు;
    పూర్ణాంకం		inclusive = FALSE;
    పూర్ణాంకం		vis_ఖాళీలు = TRUE;	// Visual selection <= 1 char
    పూర్ణాంకం		vis_bef_curs = FALSE;	// Visual starts before cursor
    పూర్ణాంకం		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    పూర్ణాంకం		inside_quotes = FALSE;	// Looks like "i'" done before
    పూర్ణాంకం		selected_quote = FALSE;	// Has quote inside selection
    పూర్ణాంకం		i;
    పూర్ణాంకం		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ఒకవేళ (VIsual_చురుకైన)
    {
	// this only works within one line
	ఒకవేళ (VIsual.lnum != curwin->w_cursor.lnum)
	    తిరిగి FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_ఖాళీలు = EQUAL_POS(VIsual, curwin->w_cursor);
	ఒకవేళ (*p_sel == 'e')
	{
	    ఒకవేళ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    లేకపోతే ఒకవేళ (!vis_ఖాళీలు)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_ఖాళీలు = EQUAL_POS(VIsual, curwin->w_cursor);
	    ఒకవేళ (!vis_bef_curs && !vis_ఖాళీలు)
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

    ఒకవేళ (!vis_ఖాళీలు)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ఒకవేళ (vis_bef_curs)
	{
	    inside_quotes = VIsual.నిలువు > 0
			&& పంక్తి[VIsual.నిలువు - 1] == quotechar
			&& పంక్తి[curwin->w_cursor.నిలువు] != NUL
			&& పంక్తి[curwin->w_cursor.నిలువు + 1] == quotechar;
	    i = VIsual.నిలువు;
	    నిలువు_చివర = curwin->w_cursor.నిలువు;
	}
	లేకపోతే
	{
	    inside_quotes = curwin->w_cursor.నిలువు > 0
			&& పంక్తి[curwin->w_cursor.నిలువు - 1] == quotechar
			&& పంక్తి[VIsual.నిలువు] != NUL
			&& పంక్తి[VIsual.నిలువు + 1] == quotechar;
	    i = curwin->w_cursor.నిలువు;
	    నిలువు_చివర = VIsual.నిలువు;
	}

	// Find out if we have a quote in the selection.
	అయితే (i <= నిలువు_చివర)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ఒకవేళ (పంక్తి[i] == NUL)
		ఆపు;
	    ఒకవేళ (పంక్తి[i++] == quotechar)
	    {
		selected_quote = TRUE;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (!vis_ఖాళీలు && పంక్తి[నిలువు_ప్రారంభం] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ఒకవేళ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    నిలువు_ప్రారంభం = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం + 1, quotechar, ఖాళీ);
	    ఒకవేళ (నిలువు_ప్రారంభం < 0)
		వెళ్ళు abort_శోధన;
	    నిలువు_చివర = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం + 1, quotechar,
							      curbuf->b_p_qe);
	    ఒకవేళ (నిలువు_చివర < 0)
	    {
		// We were on a starting quote perhaps?
		నిలువు_చివర = నిలువు_ప్రారంభం;
		నిలువు_ప్రారంభం = curwin->w_cursor.నిలువు;
	    }
	}
	లేకపోతే
	{
	    నిలువు_చివర = కనుగొను_prev_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, ఖాళీ);
	    ఒకవేళ (పంక్తి[నిలువు_చివర] != quotechar)
		వెళ్ళు abort_శోధన;
	    నిలువు_ప్రారంభం = కనుగొను_prev_quote(పంక్తి, నిలువు_చివర, quotechar,
							      curbuf->b_p_qe);
	    ఒకవేళ (పంక్తి[నిలువు_ప్రారంభం] != quotechar)
	    {
		// We were on an ending quote perhaps?
		నిలువు_ప్రారంభం = నిలువు_చివర;
		నిలువు_చివర = curwin->w_cursor.నిలువు;
	    }
	}
    }
    లేకపోతే

    ఒకవేళ (పంక్తి[నిలువు_ప్రారంభం] == quotechar || !vis_ఖాళీలు)
    {
	పూర్ణాంకం	మొదటి_నిలువు = నిలువు_ప్రారంభం;

	ఒకవేళ (!vis_ఖాళీలు)
	{
	    ఒకవేళ (vis_bef_curs)
		మొదటి_నిలువు = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, ఖాళీ);
	    లేకపోతే
		మొదటి_నిలువు = కనుగొను_prev_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, ఖాళీ);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	నిలువు_ప్రారంభం = 0;
	కొరకు (;;)
	{
	    // Find open quote character.
	    నిలువు_ప్రారంభం = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, ఖాళీ);
	    ఒకవేళ (నిలువు_ప్రారంభం < 0 || నిలువు_ప్రారంభం > మొదటి_నిలువు)
		వెళ్ళు abort_శోధన;
	    // Find close quote character.
	    నిలువు_చివర = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం + 1, quotechar,
							      curbuf->b_p_qe);
	    ఒకవేళ (నిలువు_చివర < 0)
		వెళ్ళు abort_శోధన;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ఒకవేళ (నిలువు_ప్రారంభం <= మొదటి_నిలువు && మొదటి_నిలువు <= నిలువు_చివర)
		ఆపు;
	    నిలువు_ప్రారంభం = నిలువు_చివర + 1;
	}
    }
    లేకపోతే
    {
	// Search backward for a starting quote.
	నిలువు_ప్రారంభం = కనుగొను_prev_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, curbuf->b_p_qe);
	ఒకవేళ (పంక్తి[నిలువు_ప్రారంభం] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    నిలువు_ప్రారంభం = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం, quotechar, ఖాళీ);
	    ఒకవేళ (నిలువు_ప్రారంభం < 0)
		వెళ్ళు abort_శోధన;
	}

	// Find close quote character.
	నిలువు_చివర = కనుగొను_తదుపరి_quote(పంక్తి, నిలువు_ప్రారంభం + 1, quotechar,
							      curbuf->b_p_qe);
	ఒకవేళ (నిలువు_చివర < 0)
	    వెళ్ళు abort_శోధన;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ఒకవేళ (include)
    {
	ఒకవేళ (VIM_ISWHITE(పంక్తి[నిలువు_చివర + 1]))
	    అయితే (VIM_ISWHITE(పంక్తి[నిలువు_చివర + 1]))
		++నిలువు_చివర;
	లేకపోతే
	    అయితే (నిలువు_ప్రారంభం > 0 && VIM_ISWHITE(పంక్తి[నిలువు_ప్రారంభం - 1]))
		--నిలువు_ప్రారంభం;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ఒకవేళ (!include && లెక్క < 2 && (vis_ఖాళీలు || !inside_quotes))
	++నిలువు_ప్రారంభం;
    curwin->w_cursor.నిలువు = నిలువు_ప్రారంభం;
    ఒకవేళ (VIsual_చురుకైన)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ఒకవేళ (vis_ఖాళీలు
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (పంక్తి[VIsual.నిలువు] != quotechar
			    && (VIsual.నిలువు == 0
				|| పంక్తి[VIsual.నిలువు - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    లేకపోతే
    {
	oap->ప్రారంభం = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.నిలువు = నిలువు_చివర;
    ఒకవేళ ((include || లెక్క > 1 // After vi" another i" must include the ".
		|| (!vis_ఖాళీలు && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (vis_ఖాళీలు || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ఒకవేళ (*p_sel != 'e')
		dec_cursor();
	}
	లేకపోతే
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ఒకవేళ (inside_quotes
		    || (!selected_quote
			&& పంక్తి[VIsual.నిలువు] != quotechar
			&& (పంక్తి[VIsual.నిలువు] == NUL
			    || పంక్తి[VIsual.నిలువు + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.నిలువు = నిలువు_ప్రారంభం;
	}
	ఒకవేళ (VIsual_బహుళకం == 'V')
	{
	    VIsual_బహుళకం = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    లేకపోతే
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    తిరిగి OK;

abort_శోధన:
    ఒకవేళ (VIsual_చురుకైన && *p_sel == 'e')
    {
	ఒకవేళ (did_exclusive_adj)
	    inc_cursor();
	ఒకవేళ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    తిరిగి FALSE;
}
