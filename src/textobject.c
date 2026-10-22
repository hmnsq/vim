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
#include "vim.h"

static ຈຳນວນເຕັມ cls(void);
static ຈຳນວນເຕັມ skip_chars(ຈຳນວນເຕັມ, ຈຳນວນເຕັມ);

/*
 * Find the start of the next sentence, searching in the direction specified
 * by the "dir" argument.  The cursor is positioned on the start of the next
 * sentence when found.  If the next sentence is found, return OK.  Return FAIL
 * otherwise.  See ":h sentence" for the precise definition of a "sentence"
 * text object.
 */
    ຈຳນວນເຕັມ
findsent(ຈຳນວນເຕັມ dir, long count)
{
    pos_T	pos, tpos;
    pos_T	prev_pos;
    ຈຳນວນເຕັມ		c;
    ຈຳນວນເຕັມ		(*func)(pos_T *);
    ຈຳນວນເຕັມ		startlnum;
    ຈຳນວນເຕັມ		noskip = FALSE;	    // do not skip blanks
    ຈຳນວນເຕັມ		cpo_J;
    ຈຳນວນເຕັມ		found_dot;

    pos = curwin->w_cursor;
    ຖ້າ (dir == FORWARD)
	func = incl;
    ບໍ່ດັ່ງນັ້ນ
	func = decl;

    ໃນຂະນະທີ່ (count--)
    {
	prev_pos = pos;

	/*
	 * if on an empty line, skip up to a non-empty line
	 */
	ຖ້າ (gchar_pos(&pos) == NUL)
	{
	    do
		ຖ້າ ((*func)(&pos) == -1)
		    bha_nga;
	    ໃນຂະນະທີ່ (gchar_pos(&pos) == NUL);
	    ຖ້າ (dir == FORWARD)
		goto found;
	}
	/*
	 * if on the start of a paragraph or a section and searching forward,
	 * go to the next line
	 */
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (dir == FORWARD && pos.col == 0 &&
						startPS(pos.lnum, NUL, FALSE))
	{
	    ຖ້າ (pos.lnum == curbuf->b_ml.ml_line_count)
		ສົ່ງຄືນ FAIL;
	    ++pos.lnum;
	    goto found;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (dir == BACKWARD)
	    decl(&pos);

	// go back to the previous non-white non-punctuation character
	found_dot = FALSE;
	ໃນຂະນະທີ່ (c = gchar_pos(&pos), VIM_ISWHITE(c)
				|| vim_strchr((char_u *)".!?)]\"'", c) != NULL)
	{
	    tpos = pos;
	    ຖ້າ (decl(&tpos) == -1 || (LINEEMPTY(tpos.lnum) && dir == FORWARD))
		bha_nga;

	    ຖ້າ (found_dot)
		bha_nga;
	    ຖ້າ (vim_strchr((char_u *) ".!?", c) != NULL)
		found_dot = TRUE;

	    ຖ້າ (vim_strchr((char_u *) ")]\"'", c) != NULL
		&& vim_strchr((char_u *) ".!?)]\"'", gchar_pos(&tpos)) == NULL)
		bha_nga;

	    decl(&pos);
	}

	// remember the line where the search started
	startlnum = pos.lnum;
	cpo_J = vim_strchr(p_cpo, CPO_ENDOFSENT) != NULL;

	ສຳລັບ (;;)		// find end of sentence
	{
	    c = gchar_pos(&pos);
	    ຖ້າ (c == NUL || (pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
	    {
		ຖ້າ (dir == BACKWARD && pos.lnum != startlnum)
		    ++pos.lnum;
		bha_nga;
	    }
	    ຖ້າ (c == '.' || c == '!' || c == '?')
	    {
		tpos = pos;
		do
		    ຖ້າ ((c = inc(&tpos)) == -1)
			bha_nga;
		ໃນຂະນະທີ່ (vim_strchr((char_u *)")]\"'", c = gchar_pos(&tpos))
			!= NULL);
		ຖ້າ (c == -1  || (!cpo_J && (c == ' ' || c == '\t')) || c == NUL
		    || (cpo_J && (c == ' ' && inc(&tpos) >= 0
			      && gchar_pos(&tpos) == ' ')))
		{
		    pos = tpos;
		    ຖ້າ (gchar_pos(&pos) == NUL) // skip NUL at EOL
			inc(&pos);
		    bha_nga;
		}
	    }
	    ຖ້າ ((*func)(&pos) == -1)
	    {
		ຖ້າ (count)
		    ສົ່ງຄືນ FAIL;
		noskip = TRUE;
		bha_nga;
	    }
	}
found:
	    // skip white space
	ໃນຂະນະທີ່ (!noskip && ((c = gchar_pos(&pos)) == ' ' || c == '\t'))
	    ຖ້າ (incl(&pos) == -1)
		bha_nga;

	ຖ້າ (EQUAL_POS(prev_pos, pos))
	{
	    // didn't actually move, advance one character and try again
	    ຖ້າ ((*func)(&pos) == -1)
	    {
		ຖ້າ (count)
		    ສົ່ງຄືນ FAIL;
		bha_nga;
	    }
	    ++count;
	}
    }

    setpcmark();
    curwin->w_cursor = pos;
    ສົ່ງຄືນ OK;
}

/*
 * Find the next paragraph or section in direction 'dir'.
 * Paragraphs are currently supposed to be separated by empty lines.
 * If 'what' is NUL we go to the next paragraph.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 * Return TRUE if the next paragraph or section was found.
 */
    ຈຳນວນເຕັມ
findpar(
    ຈຳນວນເຕັມ		*pincl,	    // Return: TRUE if last char is to be included
    ຈຳນວນເຕັມ		dir,
    long	count,
    ຈຳນວນເຕັມ		what,
    ຈຳນວນເຕັມ		both)
{
    linenr_T	curr;
    ຈຳນວນເຕັມ		did_skip;   // TRUE after separating lines have been skipped
    ຈຳນວນເຕັມ		first;	    // TRUE on first line
    ຈຳນວນເຕັມ		posix = (vim_strchr(p_cpo, CPO_PARA) != NULL);
#ifdef FEAT_FOLDING
    linenr_T	fold_first;	// first line of a closed fold
    linenr_T	fold_last;	// last line of a closed fold
    ຈຳນວນເຕັມ		fold_skipped;	// TRUE if a closed fold was skipped this
				// iteration
#endif

    curr = curwin->w_cursor.lnum;

    ໃນຂະນະທີ່ (count--)
    {
	did_skip = FALSE;
	ສຳລັບ (first = TRUE; ; first = FALSE)
	{
	    ຖ້າ (*ml_get(curr) != NUL)
		did_skip = TRUE;

#ifdef FEAT_FOLDING
	    // skip folded lines
	    fold_skipped = FALSE;
	    ຖ້າ (first && hasFolding(curr, &fold_first, &fold_last))
	    {
		curr = ((dir > 0) ? fold_last : fold_first) + dir;
		fold_skipped = TRUE;
	    }
#endif

	    // POSIX has its own ideas of what a paragraph boundary is and it
	    // doesn't match historical Vi: It also stops at a "{" in the
	    // first column and at an empty line.
	    ຖ້າ (!first && did_skip && (startPS(curr, what, both)
			   || (posix && what == NUL && *ml_get(curr) == '{')))
		bha_nga;

#ifdef FEAT_FOLDING
	    ຖ້າ (fold_skipped)
		curr -= dir;
#endif
	    ຖ້າ ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
	    {
		ຖ້າ (count)
		    ສົ່ງຄືນ FALSE;
		curr -= dir;
		bha_nga;
	    }
	}
    }
    setpcmark();
    ຖ້າ (both && *ml_get(curr) == '}')	// include line with '}'
	++curr;
    curwin->w_cursor.lnum = curr;
    ຖ້າ (curr == curbuf->b_ml.ml_line_count && what != '}' && dir == FORWARD)
    {
	char_u *line = ml_get(curr);

	// Put the cursor on the last character in the last line and make the
	// motion inclusive.
	ຖ້າ ((curwin->w_cursor.col = ml_get_len(curr)) != 0)
	{
	    --curwin->w_cursor.col;
	    curwin->w_cursor.col -=
			     (*mb_head_off)(line, line + curwin->w_cursor.col);
	    *pincl = TRUE;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	curwin->w_cursor.col = 0;
    ສົ່ງຄືນ TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
    static ຈຳນວນເຕັມ
inmacro(char_u *opt, char_u *s)
{
    char_u	*macro;

    ສຳລັບ (macro = opt; macro[0]; ++macro)
    {
	// Accept two characters in the option being equal to two characters
	// in the line.  A space in the option matches with a space in the
	// line or the line having ended.
	ຖ້າ (       (macro[0] == s[0]
		    || (macro[0] == ' '
			&& (s[0] == NUL || s[0] == ' ')))
		&& (macro[1] == s[1]
		    || ((macro[1] == NUL || macro[1] == ' ')
			&& (s[0] == NUL || s[1] == NUL || s[1] == ' '))))
	    bha_nga;
	++macro;
	ຖ້າ (macro[0] == NUL)
	    bha_nga;
    }
    ສົ່ງຄືນ (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 * If 'both' is TRUE also stop at '}'
 */
    ຈຳນວນເຕັມ
startPS(linenr_T lnum, ຈຳນວນເຕັມ para, ຈຳນວນເຕັມ both)
{
    char_u	*s;

    s = ml_get(lnum);
    ຖ້າ (*s == para || *s == '\f' || (both && *s == '}'))
	ສົ່ງຄືນ TRUE;
    ຖ້າ (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	ສົ່ງຄືນ TRUE;
    ສົ່ງຄືນ FALSE;
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

static ຈຳນວນເຕັມ	cls_bigword;	// TRUE for "W", "B" or "E"

/*
 * cls() - returns the class of character at curwin->w_cursor
 *
 * If a 'W', 'B', or 'E' motion is being done (cls_bigword == TRUE), chars
 * from class 2 and higher are reported as class 1 since only white space
 * boundaries are of interest.
 */
    static ຈຳນວນເຕັມ
cls(void)
{
    ຈຳນວນເຕັມ	    c;

    c = gchar_cursor();
    ຖ້າ (c == ' ' || c == '\t' || c == NUL)
	ສົ່ງຄືນ 0;
    ຖ້າ (enc_dbcs != 0 && c > 0xFF)
    {
	// If cls_bigword, report multi-byte chars as class 1.
	ຖ້າ (enc_dbcs == DBCS_KOR && cls_bigword)
	    ສົ່ງຄືນ 1;

	// process code leading/trailing bytes
	ສົ່ງຄືນ dbcs_class(((unsigned)c >> 8), (c & 0xFF));
    }
    ຖ້າ (enc_utf8)
    {
	c = utf_class(c);
	ຖ້າ (c != 0 && cls_bigword)
	    ສົ່ງຄືນ 1;
	ສົ່ງຄືນ c;
    }

    // If cls_bigword is TRUE, report all non-blanks as class 1.
    ຖ້າ (cls_bigword)
	ສົ່ງຄືນ 1;

    ຖ້າ (vim_iswordc(c))
	ສົ່ງຄືນ 2;
    ສົ່ງຄືນ 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns FAIL if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
    ຈຳນວນເຕັມ
fwd_word(
    long	count,
    ຈຳນວນເຕັມ		bigword,    // "W", "E" or "B"
    ຈຳນວນເຕັມ		eol)
{
    ຈຳນວນເຕັມ		sclass;	    // starting class
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		last_line;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ໃນຂະນະທີ່ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ຖ້າ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();

	/*
	 * We always move at least one character, unless on the last
	 * character in the buffer.
	 */
	last_line = (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count);
	i = inc_cursor();
	ຖ້າ (i == -1 || (i >= 1 && last_line)) // started at last char in file
	    ສົ່ງຄືນ FAIL;
	ຖ້າ (i >= 1 && eol && count == 0)      // started at last char in line
	    ສົ່ງຄືນ OK;

	/*
	 * Go one char past end of current word (if any)
	 */
	ຖ້າ (sclass != 0)
	    ໃນຂະນະທີ່ (cls() == sclass)
	    {
		i = inc_cursor();
		ຖ້າ (i == -1 || (i >= 1 && eol && count == 0))
		    ສົ່ງຄືນ OK;
	    }

	/*
	 * go to next non-white
	 */
	ໃນຂະນະທີ່ (cls() == 0)
	{
	    /*
	     * We'll stop if we land on a blank line
	     */
	    ຖ້າ (curwin->w_cursor.col == 0 && *ml_get_curline() == NUL)
		bha_nga;

	    i = inc_cursor();
	    ຖ້າ (i == -1 || (i >= 1 && eol && count == 0))
		ສົ່ງຄືນ OK;
	}
    }
    ສົ່ງຄືນ OK;
}

/*
 * bck_word() - move backward 'count' words
 *
 * If stop is TRUE and we are already on the start of a word, move one less.
 *
 * Returns FAIL if top of the file was reached.
 */
    ຈຳນວນເຕັມ
bck_word(long count, ຈຳນວນເຕັມ bigword, ຈຳນວນເຕັມ stop)
{
    ຈຳນວນເຕັມ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ໃນຂະນະທີ່ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the first char of the
	// first line.
	ຖ້າ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL))
	    curwin->w_cursor.col = 0;
#endif
	sclass = cls();
	ຖ້າ (dec_cursor() == -1)		// started at start of file
	    ສົ່ງຄືນ FAIL;

	ຖ້າ (!stop || sclass == cls() || sclass == 0)
	{
	    /*
	     * Skip white space before the word.
	     * Stop on an empty line.
	     */
	    ໃນຂະນະທີ່ (cls() == 0)
	    {
		ຖ້າ (curwin->w_cursor.col == 0
				      && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ຖ້າ (dec_cursor() == -1) // hit start of file, stop here
		    ສົ່ງຄືນ OK;
	    }

	    /*
	     * Move backward to start of this word.
	     */
	    ຖ້າ (skip_chars(cls(), BACKWARD))
		ສົ່ງຄືນ OK;
	}

	inc_cursor();			// overshot - forward one
finished:
	stop = FALSE;
    }
    adjust_skipcol();
    ສົ່ງຄືນ OK;
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
    ຈຳນວນເຕັມ
end_word(
    long	count,
    ຈຳນວນເຕັມ		bigword,
    ຈຳນວນເຕັມ		stop,
    ຈຳນວນເຕັມ		empty)
{
    ຈຳນວນເຕັມ		sclass;	    // starting class

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;

    // If adjusted cursor position previously, unadjust it.
    ຖ້າ (*p_sel == 'e' && VIsual_active && VIsual_mode == 'v'
		&& VIsual_select_exclu_adj)
	unadjust_for_sel();

    ໃນຂະນະທີ່ (--count >= 0)
    {
#ifdef FEAT_FOLDING
	// When inside a range of folded lines, move to the last char of the
	// last line.
	ຖ້າ (hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum))
	    coladvance((colnr_T)MAXCOL);
#endif
	sclass = cls();
	ຖ້າ (inc_cursor() == -1)
	    ສົ່ງຄືນ FAIL;

	/*
	 * If we're in the middle of a word, we just have to move to the end
	 * of it.
	 */
	ຖ້າ (cls() == sclass && sclass != 0)
	{
	    /*
	     * Move forward to end of the current word
	     */
	    ຖ້າ (skip_chars(sclass, FORWARD))
		ສົ່ງຄືນ FAIL;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (!stop || sclass == 0)
	{
	    /*
	     * We were at the end of a word. Go to the end of the next word.
	     * First skip white space, if 'empty' is TRUE, stop at empty line.
	     */
	    ໃນຂະນະທີ່ (cls() == 0)
	    {
		ຖ້າ (empty && curwin->w_cursor.col == 0
					  && LINEEMPTY(curwin->w_cursor.lnum))
		    goto finished;
		ຖ້າ (inc_cursor() == -1)	    // hit end of file, stop here
		    ສົ່ງຄືນ FAIL;
	    }

	    /*
	     * Move forward to the end of this word.
	     */
	    ຖ້າ (skip_chars(cls(), FORWARD))
		ສົ່ງຄືນ FAIL;
	}
	dec_cursor();			// overshot - one char backward
finished:
	stop = FALSE;			// we move only one word less
    }
    ສົ່ງຄືນ OK;
}

/*
 * Move back to the end of the word.
 *
 * Returns FAIL if start of the file was reached.
 */
    ຈຳນວນເຕັມ
bckend_word(
    long	count,
    ຈຳນວນເຕັມ		bigword,    // TRUE for "B"
    ຈຳນວນເຕັມ		eol)	    // TRUE: stop at end of line.
{
    ຈຳນວນເຕັມ		sclass;	    // starting class
    ຈຳນວນເຕັມ		i;

    curwin->w_cursor.coladd = 0;
    cls_bigword = bigword;
    ໃນຂະນະທີ່ (--count >= 0)
    {
	sclass = cls();
	ຖ້າ ((i = dec_cursor()) == -1)
	    ສົ່ງຄືນ FAIL;
	ຖ້າ (eol && i == 1)
	    ສົ່ງຄືນ OK;

	/*
	 * Move backward to before the start of this word.
	 */
	ຖ້າ (sclass != 0)
	{
	    ໃນຂະນະທີ່ (cls() == sclass)
		ຖ້າ ((i = dec_cursor()) == -1 || (eol && i == 1))
		    ສົ່ງຄືນ OK;
	}

	/*
	 * Move backward to end of the previous word
	 */
	ໃນຂະນະທີ່ (cls() == 0)
	{
	    ຖ້າ (curwin->w_cursor.col == 0 && LINEEMPTY(curwin->w_cursor.lnum))
		bha_nga;
	    ຖ້າ ((i = dec_cursor()) == -1 || (eol && i == 1))
		ສົ່ງຄືນ OK;
	}
    }
    adjust_skipcol();
    ສົ່ງຄືນ OK;
}

/*
 * Skip a row of characters of the same class.
 * Return TRUE when end-of-file reached, FALSE otherwise.
 */
    static ຈຳນວນເຕັມ
skip_chars(ຈຳນວນເຕັມ cclass, ຈຳນວນເຕັມ dir)
{
    ໃນຂະນະທີ່ (cls() == cclass)
	ຖ້າ ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
	    ສົ່ງຄືນ TRUE;
    ສົ່ງຄືນ FALSE;
}

/*
 * Go back to the start of the word or the start of white space
 */
    static void
back_in_line(void)
{
    ຈຳນວນເຕັມ		sclass;		    // starting class

    sclass = cls();
    ສຳລັບ (;;)
    {
	ຖ້າ (curwin->w_cursor.col == 0)	    // stop at start of line
	    bha_nga;
	dec_cursor();
	ຖ້າ (cls() != sclass)		    // stop at start of word
	{
	    inc_cursor();
	    bha_nga;
	}
    }
}

    static void
find_first_blank(pos_T *posp)
{
    ຈຳນວນເຕັມ	    c;

    ໃນຂະນະທີ່ (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	ຖ້າ (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    bha_nga;
	}
    }
}

/*
 * Skip count/2 sentences and count/2 separating white spaces.
 */
    static void
findsent_forward(
    long    count,
    ຈຳນວນເຕັມ	    at_start_sent)	// cursor is at start of sentence
{
    ໃນຂະນະທີ່ (count--)
    {
	findsent(FORWARD, 1L);
	ຖ້າ (at_start_sent)
	    find_first_blank(&curwin->w_cursor);
	ຖ້າ (count == 0 || at_start_sent)
	    decl(&curwin->w_cursor);
	at_start_sent = !at_start_sent;
    }
}

/*
 * Find word under cursor, cursor at end.
 * Used while an operator is pending, and in Visual mode.
 */
    ຈຳນວນເຕັມ
current_word(
    oparg_T	*oap,
    long	count,
    ຈຳນວນເຕັມ		include,	// TRUE: include word and white space
    ຈຳນວນເຕັມ		bigword)	// FALSE == word, TRUE == WORD
{
    pos_T	start_pos;
    pos_T	pos;
    ຈຳນວນເຕັມ		inclusive = TRUE;
    ຈຳນວນເຕັມ		include_white = FALSE;

    cls_bigword = bigword;
    CLEAR_POS(&start_pos);

    // Correct cursor when 'selection' is exclusive
    ຖ້າ (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    /*
     * When Visual mode is not active, or when the VIsual area is only one
     * character, select the word and/or white space under the cursor.
     */
    ຖ້າ (!VIsual_active || EQUAL_POS(curwin->w_cursor, VIsual))
    {
	/*
	 * Go to start of current word or white space.
	 */
	back_in_line();
	start_pos = curwin->w_cursor;

	/*
	 * If the start is on white space, and white space should be included
	 * ("	word"), or start is not on white space, and white space should
	 * not be included ("word"), find end of word.
	 */
	ຖ້າ ((cls() == 0) == include)
	{
	    ຖ້າ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		ສົ່ງຄືນ FAIL;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    /*
	     * If the start is not on white space, and white space should be
	     * included ("word	 "), or start is on white space and white
	     * space should not be included ("	 "), find start of word.
	     * If we end up in the first column of the next line (single char
	     * word) back up to end of the line.
	     */
	    fwd_word(1L, bigword, TRUE);
	    ຖ້າ (curwin->w_cursor.col == 0)
		decl(&curwin->w_cursor);
	    ບໍ່ດັ່ງນັ້ນ
		oneleft();

	    ຖ້າ (include)
		include_white = TRUE;
	}

	ຖ້າ (VIsual_active)
	{
	    // should do something when inclusive == FALSE !
	    VIsual = start_pos;
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    oap->start = start_pos;
	    oap->motion_type = MCHAR;
	}
	--count;
    }

    /*
     * When count is still > 0, extend with more objects.
     */
    ໃນຂະນະທີ່ (count > 0)
    {
	inclusive = TRUE;
	ຖ້າ (VIsual_active && LT_POS(curwin->w_cursor, VIsual))
	{
	    /*
	     * In Visual mode, with cursor at start: move cursor back.
	     */
	    ຖ້າ (decl(&curwin->w_cursor) == -1)
		ສົ່ງຄືນ FAIL;
	    ຖ້າ (include != (cls() != 0))
	    {
		ຖ້າ (bck_word(1L, bigword, TRUE) == FAIL)
		    ສົ່ງຄືນ FAIL;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຖ້າ (bckend_word(1L, bigword, TRUE) == FAIL)
		    ສົ່ງຄືນ FAIL;
		(void)incl(&curwin->w_cursor);
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    /*
	     * Move cursor forward one word and/or white area.
	     */
	    ຖ້າ (incl(&curwin->w_cursor) == -1)
		ສົ່ງຄືນ FAIL;
	    ຖ້າ (include != (cls() == 0))
	    {
		ຖ້າ (fwd_word(1L, bigword, TRUE) == FAIL && count > 1)
		    ສົ່ງຄືນ FAIL;
		/*
		 * If end is just past a new-line, we don't want to include
		 * the first character on the line.
		 * Put cursor on last char of white.
		 */
		ຖ້າ (oneleft() == FAIL)
		    inclusive = FALSE;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		ຖ້າ (end_word(1L, bigword, TRUE, TRUE) == FAIL)
		    ສົ່ງຄືນ FAIL;
	    }
	}
	--count;
    }

    ຖ້າ (include_white && (cls() != 0
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
	ຖ້າ (oneleft() == OK)
	{
	    back_in_line();
	    ຖ້າ (cls() == 0 && curwin->w_cursor.col > 0)
	    {
		ຖ້າ (VIsual_active)
		    VIsual = curwin->w_cursor;
		ບໍ່ດັ່ງນັ້ນ
		    oap->start = curwin->w_cursor;
	    }
	}
	curwin->w_cursor = pos;	// put cursor back at end
    }

    ຖ້າ (VIsual_active)
    {
	ຖ້າ (*p_sel == 'e' && inclusive && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	ຖ້າ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	oap->inclusive = inclusive;

    ສົ່ງຄືນ OK;
}

/*
 * Find sentence(s) under the cursor, cursor at end.
 * When Visual active, extend it by one or more sentences.
 */
    ຈຳນວນເຕັມ
current_sent(oparg_T *oap, long count, ຈຳນວນເຕັມ include)
{
    pos_T	start_pos;
    pos_T	pos;
    ຈຳນວນເຕັມ		start_blank;
    ຈຳນວນເຕັມ		c;
    ຈຳນວນເຕັມ		at_start_sent;
    long	ncount;

    start_pos = curwin->w_cursor;
    pos = start_pos;
    findsent(FORWARD, 1L);	// Find start of next sentence.

    /*
     * When the Visual area is bigger than one character: Extend it.
     */
    ຖ້າ (VIsual_active && !EQUAL_POS(start_pos, VIsual))
    {
extend:
	ຖ້າ (LT_POS(start_pos, VIsual))
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
	    ໃນຂະນະທີ່ (LT_POS(pos, curwin->w_cursor))
	    {
		c = gchar_pos(&pos);
		ຖ້າ (!VIM_ISWHITE(c))
		{
		    at_start_sent = FALSE;
		    bha_nga;
		}
		incl(&pos);
	    }
	    ຖ້າ (!at_start_sent)
	    {
		findsent(BACKWARD, 1L);
		ຖ້າ (EQUAL_POS(curwin->w_cursor, start_pos))
		    at_start_sent = TRUE;  // exactly at start of sentence
		ບໍ່ດັ່ງນັ້ນ
		    // inside a sentence, go to its end (start of next)
		    findsent(FORWARD, 1L);
	    }
	    ຖ້າ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    ໃນຂະນະທີ່ (count--)
	    {
		ຖ້າ (at_start_sent)
		    find_first_blank(&curwin->w_cursor);
		c = gchar_cursor();
		ຖ້າ (!at_start_sent || (!include && !VIM_ISWHITE(c)))
		    findsent(BACKWARD, 1L);
		at_start_sent = !at_start_sent;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
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
	    ຖ້າ (!EQUAL_POS(pos, curwin->w_cursor))
	    {
		at_start_sent = FALSE;
		ໃນຂະນະທີ່ (LT_POS(pos, curwin->w_cursor))
		{
		    c = gchar_pos(&pos);
		    ຖ້າ (!VIM_ISWHITE(c))
		    {
			at_start_sent = TRUE;
			bha_nga;
		    }
		    incl(&pos);
		}
		ຖ້າ (at_start_sent)	// in the sentence
		    findsent(BACKWARD, 1L);
		ບໍ່ດັ່ງນັ້ນ		// in/before white before a sentence
		    curwin->w_cursor = start_pos;
	    }

	    ຖ້າ (include)	// "as" gets twice as much as "is"
		count *= 2;
	    findsent_forward(count, at_start_sent);
	    ຖ້າ (*p_sel == 'e')
		++curwin->w_cursor.col;
	}
	ສົ່ງຄືນ OK;
    }

    /*
     * If the cursor started on a blank, check if it is just before the start
     * of the next sentence.
     */
    ໃນຂະນະທີ່ (c = gchar_pos(&pos), VIM_ISWHITE(c))	// VIM_ISWHITE() is a macro
	incl(&pos);
    ຖ້າ (EQUAL_POS(pos, curwin->w_cursor))
    {
	start_blank = TRUE;
	find_first_blank(&start_pos);	// go back to first blank
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	start_blank = FALSE;
	findsent(BACKWARD, 1L);
	start_pos = curwin->w_cursor;
    }
    ຖ້າ (include)
	ncount = count * 2;
    ບໍ່ດັ່ງນັ້ນ
    {
	ncount = count;
	ຖ້າ (start_blank)
	    --ncount;
    }
    ຖ້າ (ncount > 0)
	findsent_forward(ncount, TRUE);
    ບໍ່ດັ່ງນັ້ນ
	decl(&curwin->w_cursor);

    ຖ້າ (include)
    {
	/*
	 * If the blank in front of the sentence is included, exclude the
	 * blanks at the end of the sentence, go back to the first blank.
	 * If there are no trailing blanks, try to include leading blanks.
	 */
	ຖ້າ (start_blank)
	{
	    find_first_blank(&curwin->w_cursor);
	    c = gchar_pos(&curwin->w_cursor);	// VIM_ISWHITE() is a macro
	    ຖ້າ (VIM_ISWHITE(c))
		decl(&curwin->w_cursor);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c = gchar_cursor(), !VIM_ISWHITE(c))
	    find_first_blank(&start_pos);
    }

    ຖ້າ (VIsual_active)
    {
	// Avoid getting stuck with "is" on a single space before a sentence.
	ຖ້າ (EQUAL_POS(start_pos, curwin->w_cursor))
	    goto extend;
	ຖ້າ (*p_sel == 'e')
	    ++curwin->w_cursor.col;
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_cmdline = TRUE;		// show mode later
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// include a newline after the sentence, if there is one
	ຖ້າ (incl(&curwin->w_cursor) == -1)
	    oap->inclusive = TRUE;
	ບໍ່ດັ່ງນັ້ນ
	    oap->inclusive = FALSE;
	oap->start = start_pos;
	oap->motion_type = MCHAR;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Find block under the cursor, cursor at end.
 * "what" and "other" are two matching parenthesis/brace/etc.
 */
    ຈຳນວນເຕັມ
current_block(
    oparg_T	*oap,
    long	count,
    ຈຳນວນເຕັມ		include,	// TRUE == include white space
    ຈຳນວນເຕັມ		what,		// '(', '{', etc.
    ຈຳນວນເຕັມ		other)		// ')', '}', etc.
{
    pos_T	old_pos;
    pos_T	*pos = NULL;
    pos_T	start_pos;
    pos_T	*end_pos;
    pos_T	old_start, old_end;
    char_u	*save_cpo;
    ຈຳນວນເຕັມ		sol = FALSE;		// '{' at start of line

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		// remember where we started
    old_start = old_end;

    /*
     * If we start on '(', '{', ')', '}', etc., use the whole block inclusive.
     */
    ຖ້າ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();
	ຖ້າ (what == '{')		// ignore indent
	    ໃນຂະນະທີ່ (inindent(1))
		ຖ້າ (inc_cursor() != 0)
		    bha_nga;
	ຖ້າ (gchar_cursor() == what)
	    // cursor on '(' or '{', move cursor just after it
	    ++curwin->w_cursor.col;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ບໍ່ດັ່ງນັ້ນ
	old_end = VIsual;

    /*
     * Search backwards for unclosed '(', '{', etc..
     * Put this position in start_pos.
     * Ignore quotes here.  Keep the "M" flag in 'cpo', as that is what the
     * user wants.
     */
    save_cpo = p_cpo;
    p_cpo = (char_u *)(vim_strchr(p_cpo, CPO_MATCHBSL) != NULL ? "%M" : "%");
    ຖ້າ ((pos = findmatch(NULL, what)) != NULL)
    {
	ໃນຂະນະທີ່ (count-- > 0)
	{
	    ຖ້າ ((pos = findmatch(NULL, what)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ໃນຂະນະທີ່ (count-- > 0)
	{
	    ຖ້າ ((pos = findmatchlimit(NULL, what, FM_FORWARD, 0)) == NULL)
		bha_nga;
	    curwin->w_cursor = *pos;
	    start_pos = *pos;   // the findmatch for end_pos will overwrite *pos
	}
    }
    p_cpo = save_cpo;

    /*
     * Search for matching ')', '}', etc.
     * Put this position in curwin->w_cursor.
     */
    ຖ້າ (pos == NULL || (end_pos = findmatch(NULL, other)) == NULL)
    {
	curwin->w_cursor = old_pos;
	ສົ່ງຄືນ FAIL;
    }
    curwin->w_cursor = *end_pos;

    /*
     * Try to exclude the '(', '{', ')', '}', etc. when "include" is FALSE.
     * If the ending '}', ')' or ']' is only preceded by indent, skip that
     * indent.  But only if the resulting area is not smaller than what we
     * started with.
     */
    ໃນຂະນະທີ່ (!include)
    {
	incl(&start_pos);
	sol = (curwin->w_cursor.col == 0);
	decl(&curwin->w_cursor);
	ໃນຂະນະທີ່ (inindent(1))
	{
	    sol = TRUE;
	    ຖ້າ (decl(&curwin->w_cursor) != 0)
		bha_nga;
	}

	/*
	 * In Visual mode, when resulting area is empty
	 * i.e. there is no inner block to select, abort.
	 */
	ຖ້າ (EQUAL_POS(start_pos, *end_pos) && VIsual_active)
	{
	    curwin->w_cursor = old_pos;
	    ສົ່ງຄືນ FAIL;
	}

	/*
	 * In Visual mode, when the resulting area is not bigger than what we
	 * started with, extend it to the next block, and then exclude again.
	 * Don't try to expand the area if the area is empty.
	 */
	ຖ້າ (!LT_POS(start_pos, old_start) && !LT_POS(old_end, curwin->w_cursor)
		&& !EQUAL_POS(start_pos, curwin->w_cursor)
		&& VIsual_active)
	{
	    curwin->w_cursor = old_start;
	    decl(&curwin->w_cursor);
	    ຖ້າ ((pos = findmatch(NULL, what)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ສົ່ງຄືນ FAIL;
	    }
	    start_pos = *pos;
	    curwin->w_cursor = *pos;
	    ຖ້າ ((end_pos = findmatch(NULL, other)) == NULL)
	    {
		curwin->w_cursor = old_pos;
		ສົ່ງຄືນ FAIL;
	    }
	    curwin->w_cursor = *end_pos;
	}
	ບໍ່ດັ່ງນັ້ນ
	    bha_nga;
    }

    ຖ້າ (VIsual_active)
    {
	ຖ້າ (*p_sel == 'e')
	    inc(&curwin->w_cursor);
	ຖ້າ (sol && gchar_cursor() != NUL)
	    inc(&curwin->w_cursor);	// include the line break
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	oap->inclusive = FALSE;
	ຖ້າ (sol)
	    incl(&curwin->w_cursor);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (LTOREQ_POS(start_pos, curwin->w_cursor))
	    // Include the character under the cursor.
	    oap->inclusive = TRUE;
	ບໍ່ດັ່ງນັ້ນ
	    // End is before the start (no text in between <>, [], etc.): don't
	    // operate on any text.
	    curwin->w_cursor = start_pos;
    }

    ສົ່ງຄືນ OK;
}

#ຖ້າ defined(FEAT_EVAL)
/*
 * Return TRUE if the cursor is on a "<aaa>" tag.  Ignore "<aaa/>".
 * When "end_tag" is TRUE return TRUE if the cursor is on "</aaa>".
 */
    static ຈຳນວນເຕັມ
in_html_tag(
    ຈຳນວນເຕັມ		end_tag)
{
    char_u	*line = ml_get_curline();
    char_u	*p;
    ຈຳນວນເຕັມ		c;
    ຈຳນວນເຕັມ		lc = NUL;
    pos_T	pos;

    ຖ້າ (enc_dbcs)
    {
	char_u	*lp = NULL;

	// We search forward until the cursor, because searching backwards is
	// very slow for DBCS encodings.
	ສຳລັບ (p = line; p < line + curwin->w_cursor.col; MB_PTR_ADV(p))
	    ຖ້າ (*p == '>' || *p == '<')
	    {
		lc = *p;
		lp = p;
	    }
	ຖ້າ (*p != '<')	    // check for '<' under cursor
	{
	    ຖ້າ (lc != '<')
		ສົ່ງຄືນ FALSE;
	    p = lp;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ສຳລັບ (p = line + curwin->w_cursor.col; p > line; )
	{
	    ຖ້າ (*p == '<')	// find '<' under/before cursor
		bha_nga;
	    MB_PTR_BACK(line, p);
	    ຖ້າ (*p == '>')	// find '>' before cursor
		bha_nga;
	}
	ຖ້າ (*p != '<')
	    ສົ່ງຄືນ FALSE;
    }

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = (colnr_T)(p - line);

    MB_PTR_ADV(p);
    ຖ້າ (end_tag)
	// check that there is a '/' after the '<'
	ສົ່ງຄືນ *p == '/';

    // check that there is no '/' after the '<'
    ຖ້າ (*p == '/')
	ສົ່ງຄືນ FALSE;

    // check that the matching '>' is not preceded by '/'
    ສຳລັບ (;;)
    {
	ຖ້າ (inc(&pos) < 0)
	    ສົ່ງຄືນ FALSE;
	c = *ml_get_pos(&pos);
	ຖ້າ (c == '>')
	    bha_nga;
	lc = c;
    }
    ສົ່ງຄືນ lc != '/';
}

/*
 * Find tag block under the cursor, cursor at end.
 */
    ຈຳນວນເຕັມ
current_tagblock(
    oparg_T	*oap,
    long	count_arg,
    ຈຳນວນເຕັມ		include)	// TRUE == include white space
{
    long	count = count_arg;
    long	n;
    pos_T	old_pos;
    pos_T	start_pos;
    pos_T	end_pos;
    pos_T	old_start, old_end;
    char_u	*spat, *epat;
    char_u	*p;
    char_u	*cp;
    ຈຳນວນເຕັມ		len;
    ຈຳນວນເຕັມ		r;
    ຈຳນວນເຕັມ		do_include = include;
    ຈຳນວນເຕັມ		save_p_ws = p_ws;
    ຈຳນວນເຕັມ		retval = FAIL;
    ຈຳນວນເຕັມ		is_inclusive = TRUE;

    p_ws = FALSE;

    old_pos = curwin->w_cursor;
    old_end = curwin->w_cursor;		    // remember where we started
    old_start = old_end;
    ຖ້າ (!VIsual_active || *p_sel == 'e')
	decl(&old_end);			    // old_end is inclusive

    /*
     * If we start on "<aaa>" select that block.
     */
    ຖ້າ (!VIsual_active || EQUAL_POS(VIsual, curwin->w_cursor))
    {
	setpcmark();

	// ignore indent
	ໃນຂະນະທີ່ (inindent(1))
	    ຖ້າ (inc_cursor() != 0)
		bha_nga;

	ຖ້າ (in_html_tag(FALSE))
	{
	    // cursor on start tag, move to its '>'
	    ໃນຂະນະທີ່ (*ml_get_cursor() != '>')
		ຖ້າ (inc_cursor() < 0)
		    bha_nga;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_html_tag(TRUE))
	{
	    // cursor on end tag, move to just before it
	    ໃນຂະນະທີ່ (*ml_get_cursor() != '<')
		ຖ້າ (dec_cursor() < 0)
		    bha_nga;
	    dec_cursor();
	    old_end = curwin->w_cursor;
	}
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (LT_POS(VIsual, curwin->w_cursor))
    {
	old_start = VIsual;
	curwin->w_cursor = VIsual;	    // cursor at low end of Visual
    }
    ບໍ່ດັ່ງນັ້ນ
	old_end = VIsual;

again:
    /*
     * Search backwards for unclosed "<aaa>".
     * Put this position in start_pos.
     */
    ສຳລັບ (n = 0; n < count; ++n)
    {
	ຖ້າ (do_searchpair((char_u *)"<[^ \t>/!]\\+\\%(\\_s\\_[^>]\\{-}[^/]>\\|$\\|\\_s\\=>\\)",
		    (char_u *)"",
		    (char_u *)"</[^>]*>", BACKWARD, NULL, 0,
						  NULL, (linenr_T)0, 0L) <= 0)
	{
	    curwin->w_cursor = old_pos;
	    goto theend;
	}
    }
    start_pos = curwin->w_cursor;

    /*
     * Search for matching "</aaa>".  First isolate the "aaa".
     */
    inc_cursor();
    p = ml_get_cursor();
    ສຳລັບ (cp = p; *cp != NUL && *cp != '>' && !VIM_ISWHITE(*cp); MB_PTR_ADV(cp))
	;
    len = (ຈຳນວນເຕັມ)(cp - p);
    ຖ້າ (len == 0)
    {
	curwin->w_cursor = old_pos;
	goto theend;
    }
    spat = alloc(len + 39);
    epat = alloc(len + 9);
    ຖ້າ (spat == NULL || epat == NULL)
    {
	vim_free(spat);
	vim_free(epat);
	curwin->w_cursor = old_pos;
	goto theend;
    }
    sprintf((var_na *)spat, "<%.*s\\>\\%%(\\_s\\_[^>]\\{-}\\_[^/]>\\|\\_s\\?>\\)\\c", len, p);
    sprintf((var_na *)epat, "</%.*s>\\c", len, p);

    r = do_searchpair(spat, (char_u *)"", epat, FORWARD, NULL,
						    0, NULL, (linenr_T)0, 0L);

    vim_free(spat);
    vim_free(epat);

    ຖ້າ (r < 1 || LT_POS(curwin->w_cursor, old_end))
    {
	// Can't find other end or it's before the previous end.  Could be a
	// HTML tag that doesn't have a matching end.  Search backwards for
	// another starting tag.
	count = 1;
	curwin->w_cursor = start_pos;
	goto again;
    }

    ຖ້າ (do_include)
    {
	// Include up to the '>'.
	ໃນຂະນະທີ່ (*ml_get_cursor() != '>')
	    ຖ້າ (inc_cursor() < 0)
		bha_nga;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	char_u *c = ml_get_cursor();

	// Exclude the '<' of the end tag.
	// If the closing tag is on new line, do not decrement cursor, but
	// make operation exclusive, so that the linefeed will be selected
	ຖ້າ (*c == '<' && !VIsual_active && curwin->w_cursor.col == 0)
	    // do not decrement cursor
	    is_inclusive = FALSE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (*c == '<')
	    dec_cursor();
    }
    end_pos = curwin->w_cursor;

    ຖ້າ (!do_include)
    {
	// Exclude the start tag,
	// but skip over '>' if it appears in quotes
	ຈຳນວນເຕັມ in_quotes = FALSE;
	curwin->w_cursor = start_pos;
	ໃນຂະນະທີ່ (inc_cursor() >= 0)
	{
	    p = ml_get_cursor();
	    ຖ້າ (*p == '>' && !in_quotes)
	    {
		inc_cursor();
		start_pos = curwin->w_cursor;
		bha_nga;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p == '"' || *p == '\'')
		in_quotes = !in_quotes;
	}
	curwin->w_cursor = end_pos;

	// If we are in Visual mode and now have the same text as before set
	// "do_include" and try again.
	ຖ້າ (VIsual_active && EQUAL_POS(start_pos, old_start)
						&& EQUAL_POS(end_pos, old_end))
	{
	    do_include = TRUE;
	    curwin->w_cursor = old_start;
	    count = count_arg;
	    goto again;
	}
    }

    ຖ້າ (VIsual_active)
    {
	// If the end is before the start there is no text between tags, select
	// the char under the cursor.
	ຖ້າ (LT_POS(end_pos, start_pos))
	    curwin->w_cursor = start_pos;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (*p_sel == 'e')
	    inc_cursor();
	VIsual = start_pos;
	VIsual_mode = 'v';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	oap->start = start_pos;
	oap->motion_type = MCHAR;
	ຖ້າ (LT_POS(end_pos, start_pos))
	{
	    // End is before the start: there is no text between tags; operate
	    // on an empty area.
	    curwin->w_cursor = start_pos;
	    oap->inclusive = FALSE;
	}
	ບໍ່ດັ່ງນັ້ນ
	    oap->inclusive = is_inclusive;
    }
    retval = OK;

theend:
    p_ws = save_p_ws;
    ສົ່ງຄືນ retval;
}
#endif

    ຈຳນວນເຕັມ
current_par(
    oparg_T	*oap,
    long	count,
    ຈຳນວນເຕັມ		include,	// TRUE == include white space
    ຈຳນວນເຕັມ		type)		// 'p' for paragraph, 'S' for section
{
    linenr_T	start_lnum;
    linenr_T	end_lnum;
    ຈຳນວນເຕັມ		white_in_front;
    ຈຳນວນເຕັມ		dir;
    ຈຳນວນເຕັມ		start_is_white;
    ຈຳນວນເຕັມ		prev_start_is_white;
    ຈຳນວນເຕັມ		retval = OK;
    ຈຳນວນເຕັມ		do_white = FALSE;
    ຈຳນວນເຕັມ		t;
    ຈຳນວນເຕັມ		i;

    ຖ້າ (type == 'S')	    // not implemented yet
	ສົ່ງຄືນ FAIL;

    start_lnum = curwin->w_cursor.lnum;

    /*
     * When visual area is more than one line: extend it.
     */
    ຖ້າ (VIsual_active && start_lnum != VIsual.lnum)
    {
extend:
	ຖ້າ (start_lnum < VIsual.lnum)
	    dir = BACKWARD;
	ບໍ່ດັ່ງນັ້ນ
	    dir = FORWARD;
	ສຳລັບ (i = count; --i >= 0; )
	{
	    ຖ້າ (start_lnum ==
			   (dir == BACKWARD ? 1 : curbuf->b_ml.ml_line_count))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    prev_start_is_white = -1;
	    ສຳລັບ (t = 0; t < 2; ++t)
	    {
		start_lnum += dir;
		start_is_white = linewhite(start_lnum);
		ຖ້າ (prev_start_is_white == start_is_white)
		{
		    start_lnum -= dir;
		    bha_nga;
		}
		ສຳລັບ (;;)
		{
		    ຖ້າ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
			bha_nga;
		    ຖ້າ (start_is_white != linewhite(start_lnum + dir)
			    || (!start_is_white
				    && startPS(start_lnum + (dir > 0
							     ? 1 : 0), 0, 0)))
			bha_nga;
		    start_lnum += dir;
		}
		ຖ້າ (!include)
		    bha_nga;
		ຖ້າ (start_lnum == (dir == BACKWARD
					    ? 1 : curbuf->b_ml.ml_line_count))
		    bha_nga;
		prev_start_is_white = start_is_white;
	    }
	}
	curwin->w_cursor.lnum = start_lnum;
	curwin->w_cursor.col = 0;
	ສົ່ງຄືນ retval;
    }

    /*
     * First move back to the start_lnum of the paragraph or white lines
     */
    white_in_front = linewhite(start_lnum);
    ໃນຂະນະທີ່ (start_lnum > 1)
    {
	ຖ້າ (white_in_front)	    // stop at first white line
	{
	    ຖ້າ (!linewhite(start_lnum - 1))
		bha_nga;
	}
	ບໍ່ດັ່ງນັ້ນ		// stop at first non-white line of start of paragraph
	{
	    ຖ້າ (linewhite(start_lnum - 1) || startPS(start_lnum, 0, 0))
		bha_nga;
	}
	--start_lnum;
    }

    /*
     * Move past the end of any white lines.
     */
    end_lnum = start_lnum;
    ໃນຂະນະທີ່ (end_lnum <= curbuf->b_ml.ml_line_count && linewhite(end_lnum))
	++end_lnum;

    --end_lnum;
    i = count;
    ຖ້າ (!include && white_in_front)
	--i;
    ໃນຂະນະທີ່ (i--)
    {
	ຖ້າ (end_lnum == curbuf->b_ml.ml_line_count)
	    ສົ່ງຄືນ FAIL;

	ຖ້າ (!include)
	    do_white = linewhite(end_lnum + 1);

	ຖ້າ (include || !do_white)
	{
	    ++end_lnum;
	    /*
	     * skip to end of paragraph
	     */
	    ໃນຂະນະທີ່ (end_lnum < curbuf->b_ml.ml_line_count
		    && !linewhite(end_lnum + 1)
		    && !startPS(end_lnum + 1, 0, 0))
		++end_lnum;
	}

	ຖ້າ (i == 0 && white_in_front && include)
	    bha_nga;

	/*
	 * skip to end of white lines after paragraph
	 */
	ຖ້າ (include || do_white)
	    ໃນຂະນະທີ່ (end_lnum < curbuf->b_ml.ml_line_count
						   && linewhite(end_lnum + 1))
		++end_lnum;
    }

    /*
     * If there are no empty lines at the end, try to find some empty lines at
     * the start (unless that has been done already).
     */
    ຖ້າ (!white_in_front && !linewhite(end_lnum) && include)
	ໃນຂະນະທີ່ (start_lnum > 1 && linewhite(start_lnum - 1))
	    --start_lnum;

    ຖ້າ (VIsual_active)
    {
	// Problem: when doing "Vipipip" nothing happens in a single white
	// line, we get stuck there.  Trap this here.
	ຖ້າ (VIsual_mode == 'V' && start_lnum == curwin->w_cursor.lnum)
	    goto extend;
	ຖ້າ (VIsual.lnum != start_lnum)
	{
	    VIsual.lnum = start_lnum;
	    VIsual.col = 0;
	}
	VIsual_mode = 'V';
	redraw_curbuf_later(UPD_INVERTED);	// update the inversion
	showmode();
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	oap->start.lnum = start_lnum;
	oap->start.col = 0;
	oap->motion_type = MLINE;
    }
    curwin->w_cursor.lnum = end_lnum;
    curwin->w_cursor.col = 0;

    ສົ່ງຄືນ OK;
}

/*
 * Search quote char from string line[col].
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Returns column number of "quotechar" or -1 when not found.
 */
    static ຈຳນວນເຕັມ
find_next_quote(
    char_u	*line,
    ຈຳນວນເຕັມ		col,
    ຈຳນວນເຕັມ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ຈຳນວນເຕັມ		c;

    ສຳລັບ (;;)
    {
	c = line[col];
	ຖ້າ (c == NUL)
	    ສົ່ງຄືນ -1;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (escape != NULL && vim_strchr(escape, c))
	{
	    ++col;
	    ຖ້າ (line[col] == NUL)
		ສົ່ງຄືນ -1;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == quotechar)
	    bha_nga;
	ຖ້າ (has_mbyte)
	    col += (*mb_ptr2len)(line + col);
	ບໍ່ດັ່ງນັ້ນ
	    ++col;
    }
    ສົ່ງຄືນ col;
}

/*
 * Search backwards in "line" from column "col_start" to find "quotechar".
 * Quote character escaped by one of the characters in "escape" is not counted
 * as a quote.
 * Return the found column or zero.
 */
    static ຈຳນວນເຕັມ
find_prev_quote(
    char_u	*line,
    ຈຳນວນເຕັມ		col_start,
    ຈຳນວນເຕັມ		quotechar,
    char_u	*escape)	// escape characters, can be NULL
{
    ຈຳນວນເຕັມ		n;

    ໃນຂະນະທີ່ (col_start > 0)
    {
	--col_start;
	col_start -= (*mb_head_off)(line, line + col_start);
	n = 0;
	ຖ້າ (escape != NULL)
	    ໃນຂະນະທີ່ (col_start - n > 0 && vim_strchr(escape,
					     line[col_start - n - 1]) != NULL)
	    ++n;
	ຖ້າ (n & 1)
	    col_start -= n;	// uneven number of escape chars, skip it
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (line[col_start] == quotechar)
	    bha_nga;
    }
    ສົ່ງຄືນ col_start;
}

/*
 * Find quote under the cursor, cursor at end.
 * Returns TRUE if found, else FALSE.
 */
    ຈຳນວນເຕັມ
current_quote(
    oparg_T	*oap,
    long	count,
    ຈຳນວນເຕັມ		include,	// TRUE == include quote char
    ຈຳນວນເຕັມ		quotechar)	// Quote character
{
    char_u	*line = ml_get_curline();
    ຈຳນວນເຕັມ		col_end;
    ຈຳນວນເຕັມ		col_start = curwin->w_cursor.col;
    ຈຳນວນເຕັມ		inclusive = FALSE;
    ຈຳນວນເຕັມ		vis_empty = TRUE;	// Visual selection <= 1 char
    ຈຳນວນເຕັມ		vis_bef_curs = FALSE;	// Visual starts before cursor
    ຈຳນວນເຕັມ		did_exclusive_adj = FALSE;  // adjusted pos for 'selection'
    ຈຳນວນເຕັມ		inside_quotes = FALSE;	// Looks like "i'" done before
    ຈຳນວນເຕັມ		selected_quote = FALSE;	// Has quote inside selection
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		restore_vis_bef = FALSE; // restore VIsual on abort

    // When 'selection' is "exclusive" move the cursor to where it would be
    // with 'selection' "inclusive", so that the logic is the same for both.
    // The cursor then is moved forward after adjusting the area.
    ຖ້າ (VIsual_active)
    {
	// this only works within one line
	ຖ້າ (VIsual.lnum != curwin->w_cursor.lnum)
	    ສົ່ງຄືນ FALSE;

	vis_bef_curs = LT_POS(VIsual, curwin->w_cursor);
	vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	ຖ້າ (*p_sel == 'e')
	{
	    ຖ້າ (vis_bef_curs)
	    {
		dec_cursor();
		did_exclusive_adj = TRUE;
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!vis_empty)
	    {
		dec(&VIsual);
		did_exclusive_adj = TRUE;
	    }
	    vis_empty = EQUAL_POS(VIsual, curwin->w_cursor);
	    ຖ້າ (!vis_bef_curs && !vis_empty)
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

    ຖ້າ (!vis_empty)
    {
	// Check if the existing selection exactly spans the text inside
	// quotes.
	ຖ້າ (vis_bef_curs)
	{
	    inside_quotes = VIsual.col > 0
			&& line[VIsual.col - 1] == quotechar
			&& line[curwin->w_cursor.col] != NUL
			&& line[curwin->w_cursor.col + 1] == quotechar;
	    i = VIsual.col;
	    col_end = curwin->w_cursor.col;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    inside_quotes = curwin->w_cursor.col > 0
			&& line[curwin->w_cursor.col - 1] == quotechar
			&& line[VIsual.col] != NUL
			&& line[VIsual.col + 1] == quotechar;
	    i = curwin->w_cursor.col;
	    col_end = VIsual.col;
	}

	// Find out if we have a quote in the selection.
	ໃນຂະນະທີ່ (i <= col_end)
	{
	    // check for going over the end of the line, which can happen if
	    // the line was changed after the Visual area was selected.
	    ຖ້າ (line[i] == NUL)
		bha_nga;
	    ຖ້າ (line[i++] == quotechar)
	    {
		selected_quote = TRUE;
		bha_nga;
	    }
	}
    }

    ຖ້າ (!vis_empty && line[col_start] == quotechar)
    {
	// Already selecting something and on a quote character.  Find the
	// next quoted string.
	ຖ້າ (vis_bef_curs)
	{
	    // Assume we are on a closing quote: move to after the next
	    // opening quote.
	    col_start = find_next_quote(line, col_start + 1, quotechar, NULL);
	    ຖ້າ (col_start < 0)
		goto abort_search;
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ຖ້າ (col_end < 0)
	    {
		// We were on a starting quote perhaps?
		col_end = col_start;
		col_start = curwin->w_cursor.col;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    col_end = find_prev_quote(line, col_start, quotechar, NULL);
	    ຖ້າ (line[col_end] != quotechar)
		goto abort_search;
	    col_start = find_prev_quote(line, col_end, quotechar,
							      curbuf->b_p_qe);
	    ຖ້າ (line[col_start] != quotechar)
	    {
		// We were on an ending quote perhaps?
		col_start = col_end;
		col_end = curwin->w_cursor.col;
	    }
	}
    }
    ບໍ່ດັ່ງນັ້ນ

    ຖ້າ (line[col_start] == quotechar || !vis_empty)
    {
	ຈຳນວນເຕັມ	first_col = col_start;

	ຖ້າ (!vis_empty)
	{
	    ຖ້າ (vis_bef_curs)
		first_col = find_next_quote(line, col_start, quotechar, NULL);
	    ບໍ່ດັ່ງນັ້ນ
		first_col = find_prev_quote(line, col_start, quotechar, NULL);
	}

	// The cursor is on a quote, we don't know if it's the opening or
	// closing quote.  Search from the start of the line to find out.
	// Also do this when there is a Visual area, a' may leave the cursor
	// in between two strings.
	col_start = 0;
	ສຳລັບ (;;)
	{
	    // Find open quote character.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ຖ້າ (col_start < 0 || col_start > first_col)
		goto abort_search;
	    // Find close quote character.
	    col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	    ຖ້າ (col_end < 0)
		goto abort_search;
	    // If is cursor between start and end quote character, it is
	    // target text object.
	    ຖ້າ (col_start <= first_col && first_col <= col_end)
		bha_nga;
	    col_start = col_end + 1;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Search backward for a starting quote.
	col_start = find_prev_quote(line, col_start, quotechar, curbuf->b_p_qe);
	ຖ້າ (line[col_start] != quotechar)
	{
	    // No quote before the cursor, look after the cursor.
	    col_start = find_next_quote(line, col_start, quotechar, NULL);
	    ຖ້າ (col_start < 0)
		goto abort_search;
	}

	// Find close quote character.
	col_end = find_next_quote(line, col_start + 1, quotechar,
							      curbuf->b_p_qe);
	ຖ້າ (col_end < 0)
	    goto abort_search;
    }

    // When "include" is TRUE, include spaces after closing quote or before
    // the starting quote.
    ຖ້າ (include)
    {
	ຖ້າ (VIM_ISWHITE(line[col_end + 1]))
	    ໃນຂະນະທີ່ (VIM_ISWHITE(line[col_end + 1]))
		++col_end;
	ບໍ່ດັ່ງນັ້ນ
	    ໃນຂະນະທີ່ (col_start > 0 && VIM_ISWHITE(line[col_start - 1]))
		--col_start;
    }

    // Set start position.  After vi" another i" must include the ".
    // For v2i" include the quotes.
    ຖ້າ (!include && count < 2 && (vis_empty || !inside_quotes))
	++col_start;
    curwin->w_cursor.col = col_start;
    ຖ້າ (VIsual_active)
    {
	// Set the start of the Visual area when the Visual area was empty, we
	// were just inside quotes or the Visual area didn't start at a quote
	// and didn't include a quote.
	ຖ້າ (vis_empty
		|| (vis_bef_curs
		    && !selected_quote
		    && (inside_quotes
			|| (line[VIsual.col] != quotechar
			    && (VIsual.col == 0
				|| line[VIsual.col - 1] != quotechar)))))
	{
	    VIsual = curwin->w_cursor;
	    redraw_curbuf_later(UPD_INVERTED);
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	oap->start = curwin->w_cursor;
	oap->motion_type = MCHAR;
    }

    // Set end position.
    curwin->w_cursor.col = col_end;
    ຖ້າ ((include || count > 1 // After vi" another i" must include the ".
		|| (!vis_empty && inside_quotes)
	) && inc_cursor() == 2)
	inclusive = TRUE;
    ຖ້າ (VIsual_active)
    {
	ຖ້າ (vis_empty || vis_bef_curs)
	{
	    // decrement cursor when 'selection' is not exclusive
	    ຖ້າ (*p_sel != 'e')
		dec_cursor();
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    // Cursor is at start of Visual area.  Set the end of the Visual
	    // area when it was just inside quotes or it didn't end at a
	    // quote.
	    ຖ້າ (inside_quotes
		    || (!selected_quote
			&& line[VIsual.col] != quotechar
			&& (line[VIsual.col] == NUL
			    || line[VIsual.col + 1] != quotechar)))
	    {
		dec_cursor();
		VIsual = curwin->w_cursor;
	    }
	    curwin->w_cursor.col = col_start;
	}
	ຖ້າ (VIsual_mode == 'V')
	{
	    VIsual_mode = 'v';
	    redraw_cmdline = TRUE;		// show mode later
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// Set inclusive and other oap's flags.
	oap->inclusive = inclusive;
    }

    ສົ່ງຄືນ OK;

abort_search:
    ຖ້າ (VIsual_active && *p_sel == 'e')
    {
	ຖ້າ (did_exclusive_adj)
	    inc_cursor();
	ຖ້າ (restore_vis_bef)
	{
	    pos_T t = curwin->w_cursor;

	    curwin->w_cursor = VIsual;
	    VIsual = t;
	}
    }
    ສົ່ງຄືນ FALSE;
}
