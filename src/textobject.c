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
#شامل "vim.h"

ساکن صحیح cls(خالی);
ساکن صحیح skip_chars(صحیح, صحیح);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    صحیح
findsent(صحیح dir, طویل گنتی)
{
    pos_T	pos, tpos;
    pos_T	پچھلا_pos;
    صحیح		c;
    صحیح		(*func)(pos_T *);
    صحیح		startlnum;
    صحیح		noskip = FALSE;	    // do not skip blanks
    صحیح		cpo_J;
    صحیح		ملا_dot;

    pos = curwin->w_cursor;
    اگر (dir == FORWARD)
	func = incl;
    ورنہ
	func = decl;

    جب (گنتی--)
    {
	پچھلا_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	اگر (gchar_pos(&pos) == NUL)
	{
	    کرو
		اگر ((*func)(&pos) == -1)
		    توڑو;
	    جب (gchar_pos(&pos) == NUL);
	    اگر (dir == FORWARD)
		جاؤ ملا;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ورنہ اگر (dir == FORWARD && pos.ستون == 0 &&
						آغاز_PS(pos.lnum, NUL, FALSE))
	{
	    اگر (pos.lnum == curbuf->b_ml.ml_سطر_گنتی)
		واپس FAIL;
	    ++pos.lnum;
	    جاؤ ملا;
	}
	ورنہ اگر (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	ملا_dot = FALSE;
	جب (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != تہی)
	{
	    tpos = pos;
	    اگر (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		توڑو;

	    اگر (ملا_dot)
		توڑو;
	    اگر (vim_strchr((char_u *) ".!?", c) != تہی)
		ملا_dot = TRUE;

	    اگر (vim_strchr((char_u *) ")]\"'", c) != تہی
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == تہی)
		توڑو;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != تہی;

	برائے (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    اگر (c == NUL || (pos.ستون == 0 && آغاز_PS(pos.lnum, NUL, FALSE)))
	    {
		اگر (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		توڑو;
	    }
	    اگر (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		کرو
		    اگر ((c = inc(&tpos)) == -1)
			توڑو;
		جب (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= تہی);
		اگر (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    اگر (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    توڑو;
		}
	    }
	    اگر ((*func)(&pos) == -1)
	    {
		اگر (گنتی)
		    واپس FAIL;
		noskip = TRUE;
		توڑو;
	    }
	}
ملا:
	    // skip white space
	جب (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    اگر (incl(&pos) == -1)
		توڑو;

	اگر (EQUAL_POS(پچھلا_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    اگر ((*func)(&pos) == -1)
	    {
		اگر (گنتی)
		    واپس FAIL;
		توڑو;
	    }
	    ++گنتی;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    واپس OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    صحیح
findpar(
    صحیح		*pincl,	    // Return: TRUE if last char is to be included
    صحیح		dir,
    طویل	گنتی,
    صحیح		what,
    صحیح		both)
{
    linenr_T	curr;
    صحیح		did_skip;   // TRUE after separating lines have been skipped
    صحیح		پہلا;	    // TRUE on first line
    صحیح		posix = (vim_strchr(p_cpo, CPO_PARA) != تہی);
#اگر_تعریف_شدہ FEAT_FOLDING
    linenr_T	fold_پہلا;	// first line of a closed fold
    linenr_T	fold_آخری;	// last line of a closed fold
    صحیح		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#اگر_ختم

    curr = curwin->w_cursor.lnum;

    جب (گنتی--)
    {
	did_skip = FALSE;
	برائے (پہلا = TRUE; ; پہلا = FALSE)
	{
	    اگر (*ml_get(curr) != NUL)
		did_skip = TRUE;

#اگر_تعریف_شدہ FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    اگر (پہلا && hasFolding(curr, &fold_پہلا, &fold_آخری))
	    {
		curr = ((dir > 0) ? fold_آخری : fold_پہلا) + dir;
		fold_skipped = TRUE;
	    }
#اگر_ختم

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    اگر (!پہلا && did_skip && (آغاز_PS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		توڑو;

#اگر_تعریف_شدہ FEAT_FOLDING
	    اگر (fold_skipped)
		curr -= dir;
#اگر_ختم
	    اگر ((curr += dir) < 1 || curr > curbuf->b_ml.ml_سطر_گنتی)
	    {
		اگر (گنتی)
		    واپس FALSE;
		curr -= dir;
		توڑو;
	    }
	}
    }
    setpcmark();
    اگر (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    اگر (curr == curbuf->b_ml.ml_سطر_گنتی && what != '}' && dir == FORWARD)
    {
	char_u *سطر = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	اگر ((curwin->w_cursor.ستون = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.ستون;
	    curwin->w_cursor.ستون -=
			     (*mb_سر_off)(سطر, سطر + curwin->w_cursor.ستون);
	    *pincl = TRUE;
	}
    }
    ورنہ
	curwin->w_cursor.ستون = 0;
    واپس TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    ساکن صحیح
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    برائے (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	اگر (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    توڑو;
	++macro;
	اگر (macro[0] == NUL)
	    توڑو;
    }
    واپس (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    صحیح
آغاز_PS(linenr_T lnum, صحیح para, صحیح both)
{
    char_u	*s;

    s = ml_get(lnum);
    اگر (*s == para || *s == '\f' || (both && *s == '}'))
	واپس TRUE;
    اگر (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	واپس TRUE;
    واپس FALSE;
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

ساکن صحیح	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    ساکن صحیح
cls(خالی)
{
    صحیح	    c;

    c = gchar_cursor();
    اگر (c == ' ' || c == '\t' || c == NUL)
	واپس 0;
    اگر (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	اگر (enc_dbcs == DBCS_KOR && cls_bigword)
	    واپس 1;

	// process code leading/trailing bytes
	واپس dbcs_class(((غیرعلامتی)c >> 8), (c & 0xFF));
    }
    اگر (enc_utf8)
    {
	c = utf_class(c);
	اگر (c != 0 && cls_bigword)
	    واپس 1;
	واپس c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    اگر (cls_bigword)
	واپس 1;

    اگر (vim_iswordc(c))
	واپس 2;
    واپس 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    صحیح
fwd_لفظ(
    طویل	گنتی,
    صحیح		bigword,    // "W", "E" or "B"
    صحیح		eol)
{
    صحیح		sclass;	    // starting class
    صحیح		i;
    صحیح		آخری_سطر;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    جب (--گنتی >= 0)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	اگر (hasFolding(curwin->w_cursor.lnum, تہی, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#اگر_ختم
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	آخری_سطر = (curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_گنتی);
	i = inc_cursor();
	اگر (i == -1 || (i >= 1 && آخری_سطر)) // started at last char in file
	    واپس FAIL;
	اگر (i >= 1 && eol && گنتی == 0)      // started at last char in line
	    واپس OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	اگر (sclass != 0)
	    جب (cls() == sclass)
	    {
		i = inc_cursor();
		اگر (i == -1 || (i >= 1 && eol && گنتی == 0))
		    واپس OK;
	    }

	/*
	 * go to next non-white
	 */
	جب (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    اگر (curwin->w_cursor.ستون == 0 && *ml_get_curline() == NUL)
		توڑو;

	    i = inc_cursor();
	    اگر (i == -1 || (i >= 1 && eol && گنتی == 0))
		واپس OK;
	}
    }
    واپس OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    صحیح
bck_لفظ(طویل گنتی, صحیح bigword, صحیح stop)
{
    صحیح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    جب (--گنتی >= 0)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	اگر (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, تہی))
	    curwin->w_cursor.ستون = 0;
#اگر_ختم
	sclass = cls();
	اگر (dec_cursor() == -1)		// started at start of file
	    واپس FAIL;

	اگر (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    جب (cls() == 0)
	    {
		اگر (curwin->w_cursor.ستون == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    جاؤ ختم_شدہ;
		اگر (dec_cursor() == -1) // hit start of file, stop here
		    واپس OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    اگر (skip_chars(cls(), BACKWARD))
		واپس OK;
	}

	inc_cursor();			// overshot - forward one
ختم_شدہ:
	stop = FALSE;
    }
    adjust_skipcol();
    واپس OK;
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
    صحیح
انتہا_لفظ(
    طویل	گنتی,
    صحیح		bigword,
    صحیح		stop,
    صحیح		خالی_خانے_2)
{
    صحیح		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    اگر (*p_sel == 'e' && VIsual_فعال && VIsual_منوال == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    جب (--گنتی >= 0)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	اگر (hasFolding(curwin->w_cursor.lnum, تہی, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#اگر_ختم
	sclass = cls();
	اگر (inc_cursor() == -1)
	    واپس FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	اگر (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    اگر (skip_chars(sclass, FORWARD))
		واپس FAIL;
	}
	ورنہ اگر (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    جب (cls() == 0)
	    {
		اگر (خالی_خانے_2 && curwin->w_cursor.ستون == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    جاؤ ختم_شدہ;
		اگر (inc_cursor() == -1)	    // hit end of file, stop here
		    واپس FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    اگر (skip_chars(cls(), FORWARD))
		واپس FAIL;
	}
	dec_cursor();			// overshot - one char backward
ختم_شدہ:
	stop = FALSE;			// we move only one word less
    }
    واپس OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    صحیح
bckend_لفظ(
    طویل	گنتی,
    صحیح		bigword,    // TRUE for "B"
    صحیح		eol)	    // TRUE: stop at end of line.
{
    صحیح		sclass;	    // starting class
    صحیح		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    جب (--گنتی >= 0)
    {
	sclass = cls();
	اگر ((i = dec_cursor()) == -1)
	    واپس FAIL;
	اگر (eol && i == 1)
	    واپس OK;

	/*
	 * Move backward to before the start of this word.
	 */
	اگر (sclass != 0)
	{
	    جب (cls() == sclass)
		اگر ((i = dec_cursor()) == -1 || (eol && i == 1))
		    واپس OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	جب (cls() == 0)
	{
	    اگر (curwin->w_cursor.ستون == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		توڑو;
	    اگر ((i = dec_cursor()) == -1 || (eol && i == 1))
		واپس OK;
	}
    }
    adjust_skipcol();
    واپس OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    ساکن صحیح
skip_chars(صحیح cclass, صحیح dir)
{
    جب (cls() == cclass)
	اگر ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    ساکن خالی
عقب_اندر_سطر(خالی)
{
    صحیح		sclass;		    // starting class

    sclass = cls();
    برائے (;;)
    {
	اگر (curwin->w_cursor.ستون == 0)	    // stop at start of line
	    توڑو;
	dec_cursor();
	اگر (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    توڑو;
	}
    }
}

    ساکن خالی
ڈھونڈو_پہلا_blank(pos_T *posp)
{
    صحیح	    c;

    جب (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	اگر (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    توڑو;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    ساکن خالی
findsent_forward(
    طویل    گنتی,
    صحیح	    at_آغاز_بھیجا)	// cursor is at start of sentence
{
    جب (گنتی--)
    {
	findsent(FORWARD, 1L);
	اگر (at_آغاز_بھیجا)
	    ڈھونڈو_پہلا_blank(&curwin->w_cursor);
	اگر (گنتی == 0 || at_آغاز_بھیجا)
	    decl(&curwin->w_cursor);
	at_آغاز_بھیجا = !at_آغاز_بھیجا;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    صحیح
current_لفظ(
    oparg_T	*oap,
    طویل	گنتی,
    صحیح		include,	// TRUE: include word and white space
    صحیح		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	آغاز_pos;
    pos_T	pos;
    صحیح		inclusive = TRUE;
    صحیح		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&آغاز_pos);

    // Correct cursor when 'selection' is exclusive
    اگر (VIsual_فعال && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    اگر (!VIsual_فعال || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	عقب_اندر_سطر();
	آغاز_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	اگر ((cls() == 0) == include)
	{
	    اگر (انتہا_لفظ(1L, bigword, TRUE, TRUE) == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_لفظ(1L, bigword, TRUE);
	    اگر (curwin->w_cursor.ستون == 0)
		decl(&curwin->w_cursor);
	    ورنہ
		oneleft();

	    اگر (include)
		include_white = TRUE;
	}

	اگر (VIsual_فعال)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = آغاز_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ورنہ
	{
	    oap->آغاز = آغاز_pos;
	    oap->motion_type = MCHAR;
	}
	--گنتی;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    جب (گنتی > 0)
    {
	inclusive = TRUE;
	اگر (VIsual_فعال && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    اگر (decl(&curwin->w_cursor) == -1)
		واپس FAIL;
	    اگر (include != (cls() != 0))
	    {
		اگر (bck_لفظ(1L, bigword, TRUE) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ
	    {
		اگر (bckend_لفظ(1L, bigword, TRUE) == FAIL)
		    واپس FAIL;
		(خالی)incl(&curwin->w_cursor);
	    }
	}
	ورنہ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    اگر (incl(&curwin->w_cursor) == -1)
		واپس FAIL;
	    اگر (include != (cls() == 0))
	    {
		اگر (fwd_لفظ(1L, bigword, TRUE) == FAIL && گنتی > 1)
		    واپس FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		اگر (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ورنہ
	    {
		اگر (انتہا_لفظ(1L, bigword, TRUE, TRUE) == FAIL)
		    واپس FAIL;
	    }
	}
	--گنتی;
    }

    اگر (include_white && (cls() != 0
		 || (curwin->w_cursor.ستون == 0 && !inclusive)))
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
	curwin->w_cursor = آغاز_pos;
	اگر (oneleft() == OK)
	{
	    عقب_اندر_سطر();
	    اگر (cls() == 0 && curwin->w_cursor.ستون > 0)
	    {
		اگر (VIsual_فعال)
		    VIsual = curwin->w_cursor;
		ورنہ
		    oap->آغاز = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    اگر (VIsual_فعال)
    {
	اگر (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	اگر (VIsual_منوال == 'V')
	{
	    VIsual_منوال = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ورنہ
	oap->inclusive = inclusive;

    واپس OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    صحیح
current_بھیجا(oparg_T *oap, طویل گنتی, صحیح include)
{
    pos_T	آغاز_pos;
    pos_T	pos;
    صحیح		آغاز_blank;
    صحیح		c;
    صحیح		at_آغاز_بھیجا;
    طویل	ncount;

    آغاز_pos = curwin->w_cursor;
    pos = آغاز_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    اگر (VIsual_فعال && !EQUAL_POS(آغاز_pos, VIsual))
    {
extend:
	اگر (LT_POS(آغاز_pos, VIsual))
	{
	    /*
	     * Cursor at start of Visual area.
	     * Find out where we are:
	     * - in the white space before a sentence
	     * - in a sentence or just after it
	     * - at the start of a sentence
	     */
	    at_آغاز_بھیجا = TRUE;
	    decl(&pos);
	    جب (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		اگر (!VIM_ISWHITE(c))
		{
		    at_آغاز_بھیجا = FALSE;
		    توڑو;
		}
		incl(&pos);
	    }
	    اگر (!at_آغاز_بھیجا)
	    {
		findsent(BACKWARD, 1L);
		اگر (EQUAL_POS(curwin->w_cursor, آغاز_pos))
		    at_آغاز_بھیجا = TRUE;  // exactly at start of sentence
		ورنہ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    اگر (include)	// "as" gets twice as much as "is"
		گنتی *= 2;
	    جب (گنتی--)
	    {
		اگر (at_آغاز_بھیجا)
		    ڈھونڈو_پہلا_blank(&curwin->w_cursor);
		c = gchar_cursor();
		اگر (!at_آغاز_بھیجا || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_آغاز_بھیجا = !at_آغاز_بھیجا;
	    }
	}
	ورنہ
	{
	    /*
	     * Cursor at end of Visual area.
	     * Find out where we are:
	     * - just before a sentence
	     * - just before or in the white space before a sentence
	     * - in a sentence
	     */
	    incl(&pos);
	    at_آغاز_بھیجا = TRUE;
	    // not just before a sentence
	    اگر (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_آغاز_بھیجا = FALSE;
		جب (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    اگر (!VIM_ISWHITE(c))
		    {
			at_آغاز_بھیجا = TRUE;
			توڑو;
		    }
		    incl(&pos);
		}
		اگر (at_آغاز_بھیجا)	// in the sentence
		    findsent(BACKWARD, 1L);
		ورنہ		// in/before white before a sentence
		    curwin->w_cursor = آغاز_pos;
	    }

	    اگر (include)	// "as" gets twice as much as "is"
		گنتی *= 2;
	    findsent_forward(گنتی, at_آغاز_بھیجا);
	    اگر (*p_sel == 'e')
		++curwin->w_cursor.ستون;
	}
	واپس OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    جب (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    اگر (EQUAL_POS(pos, curwin->w_cursor))
    {
	آغاز_blank = TRUE;
	ڈھونڈو_پہلا_blank(&آغاز_pos);	// go back to first blank
    }
    ورنہ
    {
	آغاز_blank = FALSE;
	findsent(BACKWARD, 1L);
	آغاز_pos = curwin->w_cursor;
    }
    اگر (include)
	ncount = گنتی * 2;
    ورنہ
    {
	ncount = گنتی;
	اگر (آغاز_blank)
	    --ncount;
    }
    اگر (ncount > 0)
	findsent_forward(ncount, TRUE);
    ورنہ
	decl(&curwin->w_cursor);

    اگر (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	اگر (آغاز_blank)
	{
	    ڈھونڈو_پہلا_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    اگر (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ورنہ اگر (c = gchar_cursor(), !VIM_ISWHITE(c))
	    ڈھونڈو_پہلا_blank(&آغاز_pos);
    }

    اگر (VIsual_فعال)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	اگر (EQUAL_POS(آغاز_pos, curwin->w_cursor))
	    جاؤ extend;
	اگر (*p_sel == 'e')
	    ++curwin->w_cursor.ستون;
	VIsual = آغاز_pos;
	VIsual_منوال = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ورنہ
    {
	// include a newline after the sentence, if there is one
	اگر (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ورنہ
	    oap->inclusive = FALSE;
	oap->آغاز = آغاز_pos;
	oap->motion_type = MCHAR;
    }
    واپس OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    صحیح
current_بلاک(
    oparg_T	*oap,
    طویل	گنتی,
    صحیح		include,	// TRUE == include white space
    صحیح		what,		// '(', '{', etc.
    صحیح		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = تہی;
    pos_T	آغاز_pos;
    pos_T	*انتہا_pos;
    pos_T	old_آغاز, old_انتہا;
    char_u	*save_cpo;
    صحیح		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_انتہا = curwin->w_cursor;		// remember where we started
    old_آغاز = old_انتہا;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    اگر (!VIsual_فعال || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	اگر (what == '{')		// ignore indent
	    جب (inindent(1))
		اگر (inc_cursor() != 0)
		    توڑو;
	اگر (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.ستون;
    }
    ورنہ اگر (LT_POS(VIsual, curwin->w_cursor))
    {
	old_آغاز = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ورنہ
	old_انتہا = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != تہی ? "%M" : "%");
    اگر ((pos = findmatch(تہی, what)) != تہی)
    {
	جب (گنتی-- > 0)
	{
	    اگر ((pos = findmatch(تہی, what)) == تہی)
		توڑو;
	    curwin->w_cursor = *pos;
	    آغاز_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ورنہ
    {
	جب (گنتی-- > 0)
	{
	    اگر ((pos = findmatchlimit(تہی, what, FM_FORWARD, 0)) == تہی)
		توڑو;
	    curwin->w_cursor = *pos;
	    آغاز_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    اگر (pos == تہی || (انتہا_pos = findmatch(تہی, other)) == تہی)
    {
	curwin->w_cursor = old_pos;
	واپس FAIL;
    }
    curwin->w_cursor = *انتہا_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    جب (!include)
    {
	incl(&آغاز_pos);
	sol = (curwin->w_cursor.ستون == 0);
	decl(&curwin->w_cursor);
	جب (inindent(1))
	{
	    sol = TRUE;
	    اگر (decl(&curwin->w_cursor) != 0)
		توڑو;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	اگر (EQUAL_POS(آغاز_pos, *انتہا_pos) && VIsual_فعال)
	{
	    curwin->w_cursor = old_pos;
	    واپس FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	اگر (!LT_POS(آغاز_pos, old_آغاز) && !LT_POS(old_انتہا, curwin->w_cursor)
		&& !EQUAL_POS(آغاز_pos, curwin->w_cursor)
		&& VIsual_فعال)
	{
	    curwin->w_cursor = old_آغاز;
	    decl(&curwin->w_cursor);
	    اگر ((pos = findmatch(تہی, what)) == تہی)
	    {
		curwin->w_cursor = old_pos;
		واپس FAIL;
	    }
	    آغاز_pos = *pos;
	    curwin->w_cursor = *pos;
	    اگر ((انتہا_pos = findmatch(تہی, other)) == تہی)
	    {
		curwin->w_cursor = old_pos;
		واپس FAIL;
	    }
	    curwin->w_cursor = *انتہا_pos;
	}
	ورنہ
	    توڑو;
    }

    اگر (VIsual_فعال)
    {
	اگر (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	اگر (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = آغاز_pos;
	VIsual_منوال = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ورنہ
    {
	oap->آغاز = آغاز_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	اگر (sol)
	    incl(&curwin->w_cursor);
	ورنہ اگر (LTOREQ_POS(آغاز_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ورنہ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = آغاز_pos;
    }

    واپس OK;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    ساکن صحیح
اندر_html_لیبل(
    صحیح		انتہا_لیبل)
{
    char_u	*سطر = ml_get_curline();
    char_u	*p;
    صحیح		c;
    صحیح		lc = NUL;
    pos_T	pos;

    اگر (enc_dbcs)
    {
	char_u	*lp = تہی;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	برائے (p = سطر; p < سطر + curwin->w_cursor.ستون; MB_PTR_ADV(p))
	    اگر (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	اگر (*p != '<')	    // check for '<' under cursor
	{
	    اگر (lc != '<')
		واپس FALSE;
	    p = lp;
	}
    }
    ورنہ
    {
	برائے (p = سطر + curwin->w_cursor.ستون; p > سطر; )
	{
	    اگر (*p == '<')	// find '<' under/before cursor
		توڑو;
	    MB_PTR_عقب(سطر, p);
	    اگر (*p == '>')	// find '>' before cursor
		توڑو;
	}
	اگر (*p != '<')
	    واپس FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.ستون = (colnr_T)(p - سطر);

    MB_PTR_ADV(p);
    اگر (انتہا_لیبل)
	// check that there is a '/' after the '<'
	واپس *p == '/';

    // check that there is no '/' after the '<'
    اگر (*p == '/')
	واپس FALSE;

    // check that the matching '>' is not preceded by '/'
    برائے (;;)
    {
	اگر (inc(&pos) < 0)
	    واپس FALSE;
	c = *ml_get_pos(&pos);
	اگر (c == '>')
	    توڑو;
	lc = c;
    }
    واپس lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    صحیح
current_tagblock(
    oparg_T	*oap,
    طویل	گنتی_arg,
    صحیح		include)	// TRUE == include white space
{
    طویل	گنتی = گنتی_arg;
    طویل	n;
    pos_T	old_pos;
    pos_T	آغاز_pos;
    pos_T	انتہا_pos;
    pos_T	old_آغاز, old_انتہا;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    صحیح		len;
    صحیح		r;
    صحیح		do_include = include;
    صحیح		save_p_ws = p_ws;
    صحیح		retval = FAIL;
    صحیح		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_انتہا = curwin->w_cursor;		    // remember where we started
    old_آغاز = old_انتہا;
    اگر (!VIsual_فعال || *p_sel == 'e')
	decl(&old_انتہا);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    اگر (!VIsual_فعال || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	جب (inindent(1))
	    اگر (inc_cursor() != 0)
		توڑو;

	اگر (اندر_html_لیبل(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    جب (*ml_get_cursor() != '>')
		اگر (inc_cursor() < 0)
		    توڑو;
	}
	ورنہ اگر (اندر_html_لیبل(TRUE))
	{
	    // cursor on end tag, move to just before it
	    جب (*ml_get_cursor() != '<')
		اگر (dec_cursor() < 0)
		    توڑو;
	    dec_cursor();
	    old_انتہا = curwin->w_cursor;
	}
    }
    ورنہ اگر (LT_POS(VIsual, curwin->w_cursor))
    {
	old_آغاز = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ورنہ
	old_انتہا = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    برائے (n = 0; n < گنتی; ++n)
    {
	اگر (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, تہی, 0,
						  تہی, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    جاؤ theend;
	}
    }
    آغاز_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    برائے (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (صحیح)(cp - p);
    اگر (len == 0)
    {
	curwin->w_cursor = old_pos;
	جاؤ theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    اگر (spat == تہی || epat == تہی)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	جاؤ theend;
    }
    sprintf((حرف *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((حرف *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, تہی,
						    0, تہی, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    اگر (r < 1 || LT_POS(curwin->w_cursor, old_انتہا))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	گنتی = 1;
	curwin->w_cursor = آغاز_pos;
	جاؤ again;
    }

    اگر (do_include)
    {
	// Include up to the '>'.
	جب (*ml_get_cursor() != '>')
	    اگر (inc_cursor() < 0)
		توڑو;
    }
    ورنہ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	اگر (*c == '<' && !VIsual_فعال && curwin->w_cursor.ستون == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ورنہ اگر (*c == '<')
	    dec_cursor();
    }
    انتہا_pos = curwin->w_cursor;

    اگر (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	صحیح اندر_quotes = FALSE;
	curwin->w_cursor = آغاز_pos;
	جب (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    اگر (*p == '>' && !اندر_quotes)
	    {
		inc_cursor();
		آغاز_pos = curwin->w_cursor;
		توڑو;
	    }
	    ورنہ اگر (*p == '"' || *p == '\'')
		اندر_quotes = !اندر_quotes;
	}
	curwin->w_cursor = انتہا_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	اگر (VIsual_فعال && EQUAL_POS(آغاز_pos, old_آغاز)
						&& EQUAL_POS(انتہا_pos, old_انتہا))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_آغاز;
	    گنتی = گنتی_arg;
	    جاؤ again;
	}
    }

    اگر (VIsual_فعال)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	اگر (LT_POS(انتہا_pos, آغاز_pos))
	    curwin->w_cursor = آغاز_pos;
	ورنہ اگر (*p_sel == 'e')
	    inc_cursor();
	VIsual = آغاز_pos;
	VIsual_منوال = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ورنہ
    {
	oap->آغاز = آغاز_pos;
	oap->motion_type = MCHAR;
	اگر (LT_POS(انتہا_pos, آغاز_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = آغاز_pos;
	    oap->inclusive = FALSE;
	}
	ورنہ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    واپس retval;
}
#اگر_ختم

    صحیح
current_par(
    oparg_T	*oap,
    طویل	گنتی,
    صحیح		include,	// TRUE == include white space
    صحیح		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	آغاز_lnum;
    linenr_T	انتہا_lnum;
    صحیح		white_اندر_front;
    صحیح		dir;
    صحیح		آغاز_is_white;
    صحیح		پچھلا_آغاز_is_white;
    صحیح		retval = OK;
    صحیح		do_white = FALSE;
    صحیح		t;
    صحیح		i;

    اگر (type == 'S')	    // not implemented yet
	واپس FAIL;

    آغاز_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    اگر (VIsual_فعال && آغاز_lnum != VIsual.lnum)
    {
extend:
	اگر (آغاز_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ورنہ
	    dir = FORWARD;
	برائے (i = گنتی; --i >= 0; )
	{
	    اگر (آغاز_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_سطر_گنتی))
	    {
		retval = FAIL;
		توڑو;
	    }

	    پچھلا_آغاز_is_white = -1;
	    برائے (t = 0; t < 2; ++t)
	    {
		آغاز_lnum += dir;
		آغاز_is_white = linewhite(آغاز_lnum);
		اگر (پچھلا_آغاز_is_white == آغاز_is_white)
		{
		    آغاز_lnum -= dir;
		    توڑو;
		}
		برائے (;;)
		{
		    اگر (آغاز_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_گنتی))
			توڑو;
		    اگر (آغاز_is_white != linewhite(آغاز_lnum + dir)
			    || (!آغاز_is_white
				    && آغاز_PS(آغاز_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			توڑو;
		    آغاز_lnum += dir;
		}
		اگر (!include)
		    توڑو;
		اگر (آغاز_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_سطر_گنتی))
		    توڑو;
		پچھلا_آغاز_is_white = آغاز_is_white;
	    }
	}
	curwin->w_cursor.lnum = آغاز_lnum;
	curwin->w_cursor.ستون = 0;
	واپس retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_اندر_front = linewhite(آغاز_lnum);
    جب (آغاز_lnum > 1)
    {
	اگر (white_اندر_front)	    // stop at first white line
	{
	    اگر (!linewhite(آغاز_lnum - 1))
		توڑو;
	}
	ورنہ		// stop at first non-white line of start of paragraph
	{
	    اگر (linewhite(آغاز_lnum - 1) || آغاز_PS(آغاز_lnum, 0, 0))
		توڑو;
	}
	--آغاز_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    انتہا_lnum = آغاز_lnum;
    جب (انتہا_lnum <= curbuf->b_ml.ml_سطر_گنتی && linewhite(انتہا_lnum))
	++انتہا_lnum;

    --انتہا_lnum;
    i = گنتی;
    اگر (!include && white_اندر_front)
	--i;
    جب (i--)
    {
	اگر (انتہا_lnum == curbuf->b_ml.ml_سطر_گنتی)
	    واپس FAIL;

	اگر (!include)
	    do_white = linewhite(انتہا_lnum + 1);

	اگر (include || !do_white)
	{
	    ++انتہا_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    جب (انتہا_lnum < curbuf->b_ml.ml_سطر_گنتی
		    && !linewhite(انتہا_lnum + 1)
		    && !آغاز_PS(انتہا_lnum + 1, 0, 0))
		++انتہا_lnum;
	}

	اگر (i == 0 && white_اندر_front && include)
	    توڑو;

	/*
	 * skip to end of white lines after paragraph
	 */
	اگر (include || do_white)
	    جب (انتہا_lnum < curbuf->b_ml.ml_سطر_گنتی
						   && linewhite(انتہا_lnum + 1))
		++انتہا_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    اگر (!white_اندر_front && !linewhite(انتہا_lnum) && include)
	جب (آغاز_lnum > 1 && linewhite(آغاز_lnum - 1))
	    --آغاز_lnum;

    اگر (VIsual_فعال)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	اگر (VIsual_منوال == 'V' && آغاز_lnum == curwin->w_cursor.lnum)
	    جاؤ extend;
	اگر (VIsual.lnum != آغاز_lnum)
	{
	    VIsual.lnum = آغاز_lnum;
	    VIsual.ستون = 0;
	}
	VIsual_منوال = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ورنہ
    {
	oap->آغاز.lnum = آغاز_lnum;
	oap->آغاز.ستون = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = انتہا_lnum;
    curwin->w_cursor.ستون = 0;

    واپس OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    ساکن صحیح
ڈھونڈو_اگلا_quote(
    char_u	*سطر,
    صحیح		ستون,
    صحیح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحیح		c;

    برائے (;;)
    {
	c = سطر[ستون];
	اگر (c == NUL)
	    واپس -1;
	ورنہ اگر (escape != تہی && vim_strchr(escape, c))
	{
	    ++ستون;
	    اگر (سطر[ستون] == NUL)
		واپس -1;
	}
	ورنہ اگر (c == quotechar)
	    توڑو;
	اگر (has_mbyte)
	    ستون += (*mb_ptr2len)(سطر + ستون);
	ورنہ
	    ++ستون;
    }
    واپس ستون;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    ساکن صحیح
ڈھونڈو_پچھلا_quote(
    char_u	*سطر,
    صحیح		ستون_آغاز,
    صحیح		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    صحیح		n;

    جب (ستون_آغاز > 0)
    {
	--ستون_آغاز;
	ستون_آغاز -= (*mb_سر_off)(سطر, سطر + ستون_آغاز);
	n = 0;
	اگر (escape != تہی)
	    جب (ستون_آغاز - n > 0 && vim_strchr(escape,
					     سطر[ستون_آغاز - n - 1]) != تہی)
	    ++n;
	اگر (n & 1)
	    ستون_آغاز -= n;	// uneven number of escape chars, skip it
	ورنہ اگر (سطر[ستون_آغاز] == quotechar)
	    توڑو;
    }
    واپس ستون_آغاز;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    صحیح
current_quote(
    oparg_T	*oap,
    طویل	گنتی,
    صحیح		include,	// TRUE == include quote char
    صحیح		quotechar)	// Quote character
{
    char_u	*سطر = ml_get_curline();
    صحیح		ستون_انتہا;
    صحیح		ستون_آغاز = curwin->w_cursor.ستون;
    صحیح		inclusive = FALSE;
    صحیح		vis_خالی_خانے = TRUE;	// Visual selection <= 1 char
    صحیح		vis_bef_curs = FALSE;	// Visual starts before cursor
    صحیح		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    صحیح		inside_quotes = FALSE;	// Looks like "i'" done before
    صحیح		selected_quote = FALSE;	// Has quote inside selection
    صحیح		i;
    صحیح		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    اگر (VIsual_فعال)
    {
	// this only works within one line
	اگر (VIsual.lnum != curwin->w_cursor.lnum)
	    واپس FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_خالی_خانے = EQUAL_POS(VIsual, curwin->w_cursor);
	اگر (*p_sel == 'e')
	{
	    اگر (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ورنہ اگر (!vis_خالی_خانے)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_خالی_خانے = EQUAL_POS(VIsual, curwin->w_cursor);
	    اگر (!vis_bef_curs && !vis_خالی_خانے)
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

    اگر (!vis_خالی_خانے)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	اگر (vis_bef_curs)
	{
	    inside_quotes = VIsual.ستون > 0
			&& سطر[VIsual.ستون - 1] == quotechar
			&& سطر[curwin->w_cursor.ستون] != NUL
			&& سطر[curwin->w_cursor.ستون + 1] == quotechar;
	    i = VIsual.ستون;
	    ستون_انتہا = curwin->w_cursor.ستون;
	}
	ورنہ
	{
	    inside_quotes = curwin->w_cursor.ستون > 0
			&& سطر[curwin->w_cursor.ستون - 1] == quotechar
			&& سطر[VIsual.ستون] != NUL
			&& سطر[VIsual.ستون + 1] == quotechar;
	    i = curwin->w_cursor.ستون;
	    ستون_انتہا = VIsual.ستون;
	}

	// Find out if we have a quote in the selection.
	جب (i <= ستون_انتہا)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    اگر (سطر[i] == NUL)
		توڑو;
	    اگر (سطر[i++] == quotechar)
	    {
		selected_quote = TRUE;
		توڑو;
	    }
	}
    }

    اگر (!vis_خالی_خانے && سطر[ستون_آغاز] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	اگر (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    ستون_آغاز = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز + 1, quotechar, تہی);
	    اگر (ستون_آغاز < 0)
		جاؤ abort_تلاش;
	    ستون_انتہا = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز + 1, quotechar,
							      curbuf->b_p_qe);
	    اگر (ستون_انتہا < 0)
	    {
		// We were on a starting quote perhaps?
		ستون_انتہا = ستون_آغاز;
		ستون_آغاز = curwin->w_cursor.ستون;
	    }
	}
	ورنہ
	{
	    ستون_انتہا = ڈھونڈو_پچھلا_quote(سطر, ستون_آغاز, quotechar, تہی);
	    اگر (سطر[ستون_انتہا] != quotechar)
		جاؤ abort_تلاش;
	    ستون_آغاز = ڈھونڈو_پچھلا_quote(سطر, ستون_انتہا, quotechar,
							      curbuf->b_p_qe);
	    اگر (سطر[ستون_آغاز] != quotechar)
	    {
		// We were on an ending quote perhaps?
		ستون_آغاز = ستون_انتہا;
		ستون_انتہا = curwin->w_cursor.ستون;
	    }
	}
    }
    ورنہ

    اگر (سطر[ستون_آغاز] == quotechar || !vis_خالی_خانے)
    {
	صحیح	پہلا_ستون = ستون_آغاز;

	اگر (!vis_خالی_خانے)
	{
	    اگر (vis_bef_curs)
		پہلا_ستون = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز, quotechar, تہی);
	    ورنہ
		پہلا_ستون = ڈھونڈو_پچھلا_quote(سطر, ستون_آغاز, quotechar, تہی);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	ستون_آغاز = 0;
	برائے (;;)
	{
	    // Find open quote character.
	    ستون_آغاز = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز, quotechar, تہی);
	    اگر (ستون_آغاز < 0 || ستون_آغاز > پہلا_ستون)
		جاؤ abort_تلاش;
	    // Find close quote character.
	    ستون_انتہا = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز + 1, quotechar,
							      curbuf->b_p_qe);
	    اگر (ستون_انتہا < 0)
		جاؤ abort_تلاش;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    اگر (ستون_آغاز <= پہلا_ستون && پہلا_ستون <= ستون_انتہا)
		توڑو;
	    ستون_آغاز = ستون_انتہا + 1;
	}
    }
    ورنہ
    {
	// Search backward for a starting quote.
	ستون_آغاز = ڈھونڈو_پچھلا_quote(سطر, ستون_آغاز, quotechar, curbuf->b_p_qe);
	اگر (سطر[ستون_آغاز] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    ستون_آغاز = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز, quotechar, تہی);
	    اگر (ستون_آغاز < 0)
		جاؤ abort_تلاش;
	}

	// Find close quote character.
	ستون_انتہا = ڈھونڈو_اگلا_quote(سطر, ستون_آغاز + 1, quotechar,
							      curbuf->b_p_qe);
	اگر (ستون_انتہا < 0)
	    جاؤ abort_تلاش;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    اگر (include)
    {
	اگر (VIM_ISWHITE(سطر[ستون_انتہا + 1]))
	    جب (VIM_ISWHITE(سطر[ستون_انتہا + 1]))
		++ستون_انتہا;
	ورنہ
	    جب (ستون_آغاز > 0 && VIM_ISWHITE(سطر[ستون_آغاز - 1]))
		--ستون_آغاز;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    اگر (!include && گنتی < 2 && (vis_خالی_خانے || !inside_quotes))
	++ستون_آغاز;
    curwin->w_cursor.ستون = ستون_آغاز;
    اگر (VIsual_فعال)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	اگر (vis_خالی_خانے
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (سطر[VIsual.ستون] != quotechar
			    && (VIsual.ستون == 0
				|| سطر[VIsual.ستون - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ورنہ
    {
	oap->آغاز = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.ستون = ستون_انتہا;
    اگر ((include || گنتی > 1 // After vi" another i" must include the ".
		|| (!vis_خالی_خانے && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    اگر (VIsual_فعال)
    {
	اگر (vis_خالی_خانے || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    اگر (*p_sel != 'e')
		dec_cursor();
	}
	ورنہ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    اگر (inside_quotes
		    || (!selected_quote
			&& سطر[VIsual.ستون] != quotechar
			&& (سطر[VIsual.ستون] == NUL
			    || سطر[VIsual.ستون + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.ستون = ستون_آغاز;
	}
	اگر (VIsual_منوال == 'V')
	{
	    VIsual_منوال = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ورنہ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    واپس OK;

abort_تلاش:
    اگر (VIsual_فعال && *p_sel == 'e')
    {
	اگر (did_exclusive_adj)
	    inc_cursor();
	اگر (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    واپس FALSE;
}
