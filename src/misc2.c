/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * misc2.c: Various functions.
 */
#include "vim.h"

static char_u	*username = NULL; // cached result of mch_get_user_name()

static целое coladvance2(pos_T *pos, целое addspaces, целое finetune, colnr_T wcol);

/*
 * Return TRUE if in the current mode we need to use virtual.
 */
    целое
virtual_active(void)
{
    unsigned целое cur_ve_flags = get_ve_flags();

    если (cur_ve_flags == VE_ALL
	    || ((cur_ve_flags & VE_INSERT) && (State & MODE_INSERT)))
	вернуть TRUE;

    // While an operator is being executed we return "virtual_op", because
    // VIsual_active has already been reset, thus we can't check for "block"
    // being used.
    если (virtual_op != MAYBE)
	вернуть virtual_op;
    вернуть (cur_ve_flags & VE_BLOCK) && VIsual_active && VIsual_mode == Ctrl_V;
}

/*
 * Get the screen position of the cursor.
 */
    целое
getviscol(void)
{
    colnr_T	x;

    getvvcol(curwin, &curwin->w_cursor, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    вернуть (целое)x;
}

/*
 * Go to column "wcol", and add/insert white space as necessary to get the
 * cursor in that column.
 * The caller must have saved the cursor line for undo!
 */
    целое
coladvance_force(colnr_T wcol)
{
    целое rc = coladvance2(&curwin->w_cursor, TRUE, FALSE, wcol);

    если (wcol == MAXCOL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    иначе
	// Virtcol is valid
	set_valid_virtcol(curwin, wcol);
    вернуть rc;
}

/*
 * Get the screen position of character col with a coladd in the cursor line.
 */
    целое
getviscol2(colnr_T col, colnr_T coladd)
{
    colnr_T	x;
    pos_T	pos;

    pos.lnum = curwin->w_cursor.lnum;
    pos.col = col;
    pos.coladd = coladd;
    getvvcol(curwin, &pos, &x, NULL, NULL, GETVCOL_NO_ABOVE);
    вернуть (целое)x;
}

/*
 * Try to advance the Cursor to the specified screen column "wantcol".
 * If virtual editing: fine tune the cursor position.
 * Note that all virtual positions off the end of a line should share
 * a curwin->w_cursor.col value (n.b. this is equal to STRLEN(line)),
 * beginning at coladd 0.
 *
 * return OK if desired column is reached, FAIL if not
 */
    целое
coladvance(colnr_T wantcol)
{
    целое rc = getvpos(&curwin->w_cursor, wantcol);

    если (wantcol == MAXCOL || rc == FAIL)
	curwin->w_valid &= ~VALID_VIRTCOL;
    иначе если (*ml_get_cursor() != TAB)
	// Virtcol is valid when not on a TAB
	set_valid_virtcol(curwin, wantcol);
    вернуть rc;
}

/*
 * Return in "pos" the position of the cursor advanced to screen column
 * "wantcol".
 * return OK if desired column is reached, FAIL if not
 */
    целое
getvpos(pos_T *pos, colnr_T wantcol)
{
    вернуть coladvance2(pos, FALSE, virtual_active(), wantcol);
}

    static целое
coladvance2(
    pos_T	*pos,
    целое		addspaces,	// change the text to achieve our goal?
    целое		finetune,	// change char offset for the exact column
    colnr_T	wcol_arg)	// column to move to (can be negative)
{
    colnr_T	wcol = wcol_arg;
    целое		idx;
    char_u	*line;
    целое		linelen;
    colnr_T	col = 0;
    целое		csize = 0;
    целое		one_more;
#ifdef FEAT_LINEBREAK
    целое		head = 0;
#endif

    one_more = (State & MODE_INSERT)
		    || restart_edit != NUL
		    || (VIsual_active && *p_sel != 'o')
		    || ((get_ve_flags() & VE_ONEMORE) && wcol < MAXCOL);
    line = ml_get_buf(curbuf, pos->lnum, FALSE);
    linelen = ml_get_buf_len(curbuf, pos->lnum);

    если (wcol >= MAXCOL)
    {
	    idx = linelen - 1 + one_more;
	    col = wcol;

	    если ((addspaces || finetune) && !VIsual_active)
	    {
		curwin->w_curswant = linetabsize_no_outer(curwin, pos->lnum)
								   + one_more;
		если (curwin->w_curswant > 0)
		    --curwin->w_curswant;
	    }
    }
    иначе
    {
	целое		width = curwin->w_width - win_col_off(curwin);
	chartabsize_T	cts;

	если (finetune
		&& curwin->w_p_wrap
		&& curwin->w_width != 0
		&& wcol >= (colnr_T)width
		&& width > 0)
	{
	    csize = linetabsize_no_outer_eol(curwin, pos->lnum);
	    если (csize > 0)
		csize--;

	    если (wcol / width > (colnr_T)csize / width
		    && ((State & MODE_INSERT) == 0 || (целое)wcol > csize + 1))
	    {
		// In case of line wrapping don't move the cursor beyond the
		// right screen edge.  In Insert mode allow going just beyond
		// the last character (like what happens when typing and
		// reaching the right window edge).
		wcol = (csize / width + 1) * width - 1;
	    }
	}

	init_chartabsize_arg(&cts, curwin, pos->lnum, 0, line, line);
#ifdef FEAT_PROP_POPUP
	cts.cts_no_above = true;
#endif
	пока (cts.cts_vcol <= wcol && *cts.cts_ptr != NUL)
	{
	    // Count a tab for what it's worth (if list mode not on)
#ifdef FEAT_LINEBREAK
	    csize = win_lbr_chartabsize(&cts, &head, NULL);
	    MB_PTR_ADV(cts.cts_ptr);
#иначе
	    csize = lbr_chartabsize_adv(&cts);
#endif
	    cts.cts_vcol += csize;
	}
	col = cts.cts_vcol;
	idx = (целое)(cts.cts_ptr - line);
	clear_chartabsize_arg(&cts);

	/*
	 * Handle all the special cases.  The virtual_active() check
	 * is needed to ensure that a virtual position off the end of
	 * a line has the correct indexing.  The one_more comparison
	 * replaces an explicit add of one_more later on.
	 */
	если (col > wcol || (!virtual_active() && one_more == 0))
	{
	    idx -= 1;
#ifdef FEAT_LINEBREAK
	    // Don't count the chars from 'showbreak'.
	    csize -= head;
#endif
	    col -= csize;
	}

	если (virtual_active()
		&& addspaces
		&& wcol >= 0
		&& ((col != wcol && col != wcol + 1) || csize > 1))
	{
	    // 'virtualedit' is set: The difference between wcol and col is
	    // filled with spaces.

	    если (line[idx] == NUL)
	    {
		// Append spaces
		целое	correct = wcol - col;
		char_u	*newline = alloc(idx + correct + 1);
		целое	t;

		если (newline == NULL)
		    вернуть FAIL;

		для (t = 0; t < idx; ++t)
		    newline[t] = line[t];

		для (t = 0; t < correct; ++t)
		    newline[t + idx] = ' ';

		newline[idx + correct] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, (colnr_T)idx);
		idx += correct;
		col = wcol;
	    }
	    иначе
	    {
		// Break a tab
		целое	correct = wcol - col - csize + 1; // negative!!
		char_u	*newline;
		целое	t, s = 0;
		целое	v;

		если (-correct > csize)
		    вернуть FAIL;

		newline = alloc(linelen + csize);
		если (newline == NULL)
		    вернуть FAIL;

		для (t = 0; t < linelen; t++)
		{
		    если (t != idx)
			newline[s++] = line[t];
		    иначе
			для (v = 0; v < csize; v++)
			    newline[s++] = ' ';
		}

		newline[linelen + csize - 1] = NUL;

		ml_replace(pos->lnum, newline, FALSE);
		changed_bytes(pos->lnum, idx);
		idx += (csize - 1 + correct);
		col += correct;
	    }
	}
    }

    если (idx < 0)
	pos->col = 0;
    иначе
	pos->col = idx;

    pos->coladd = 0;

    если (finetune)
    {
	если (wcol == MAXCOL)
	{
	    // The width of the last character is used to set coladd.
	    если (!one_more)
	    {
		colnr_T	    scol, ecol;

		getvcol(curwin, pos, &scol, NULL, &ecol, 0);
		pos->coladd = ecol - scol;
	    }
	}
	иначе
	{
	    целое b = (целое)wcol - (целое)col;

	    // The difference between wcol and col is used to set coladd.
	    если (b > 0 && b < (MAXCOL - 2 * curwin->w_width))
		pos->coladd = b;

	    col += b;
	}
    }

    // prevent from moving onto a trail byte
    если (has_mbyte)
	mb_adjustpos(curbuf, pos);

    если (wcol < 0 || col < wcol)
	вернуть FAIL;
    вернуть OK;
}

/*
 * Increment the cursor position.  See inc() for return values.
 */
    целое
inc_cursor(void)
{
    вернуть inc(&curwin->w_cursor);
}

/*
 * Increment the line pointer "lp" crossing line boundaries as necessary.
 * Return 1 when going to the next line.
 * Return 2 when moving forward onto a NUL at the end of the line).
 * Return -1 when at the end of file.
 * Return 0 otherwise.
 */
    целое
inc(pos_T *lp)
{
    char_u  *p;

    // when searching position may be set to end of a line
    если (lp->col != MAXCOL)
    {
	p = ml_get_pos(lp);
	если (*p != NUL)	// still within line, move to next char (may be NUL)
	{
	    если (has_mbyte)
	    {
		целое l = (*mb_ptr2len)(p);

		lp->col += l;
		вернуть ((p[l] != NUL) ? 0 : 2);
	    }
	    lp->col++;
	    lp->coladd = 0;
	    вернуть ((p[1] != NUL) ? 0 : 2);
	}
    }
    если (lp->lnum != curbuf->b_ml.ml_line_count)     // there is a next line
    {
	lp->col = 0;
	lp->lnum++;
	lp->coladd = 0;
	вернуть 1;
    }
    вернуть -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    целое
incl(pos_T *lp)
{
    целое	    r;

    если ((r = inc(lp)) >= 1 && lp->col)
	r = inc(lp);
    вернуть r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    целое
dec_cursor(void)
{
    вернуть dec(&curwin->w_cursor);
}

    целое
dec(pos_T *lp)
{
    char_u	*p;

    lp->coladd = 0;
    если (lp->col == MAXCOL)
    {
	// past end of line
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	если (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	вернуть 0;
    }

    если (lp->col > 0)
    {
	// still within line
	lp->col--;
	если (has_mbyte)
	{
	    p = ml_get(lp->lnum);
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	}
	вернуть 0;
    }

    если (lp->lnum > 1)
    {
	// there is a prior line
	lp->lnum--;
	p = ml_get(lp->lnum);
	lp->col = ml_get_len(lp->lnum);
	если (has_mbyte)
	    lp->col -= (*mb_head_off)(p, p + lp->col);
	вернуть 1;
    }

    // at start of file
    вернуть -1;
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    целое
decl(pos_T *lp)
{
    целое	    r;

    если ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    вернуть r;
}

/*
 * Get the line number relative to the current cursor position, i.e. the
 * difference between line number and cursor position. Only look for lines that
 * can be visible, folded lines don't count.
 */
    linenr_T
get_cursor_rel_lnum(
    win_T	*wp,
    linenr_T	lnum)		    // line number to get the result for
{
    linenr_T	cursor = wp->w_cursor.lnum;
    linenr_T	retval = 0;

#ifdef FEAT_FOLDING
    если (hasAnyFolding(wp))
    {
	если (lnum > cursor)
	{
	    пока (lnum > cursor)
	    {
		(void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum <= cursor
		если (lnum > cursor)
		    retval++;
		lnum--;
	    }
	}
	иначе если (lnum < cursor)
	{
	    пока (lnum < cursor)
	    {
		(void)hasFoldingWin(wp, lnum, NULL, &lnum, TRUE, NULL);
		// if lnum and cursor are in the same fold,
		// now lnum >= cursor
		если (lnum < cursor)
		    retval--;
		lnum++;
	    }
	}
	// else if (lnum == cursor)
	//     retval = 0;
    }
    иначе
#endif
	retval = lnum - cursor;

    вернуть retval;
}

/*
 * Make sure "pos.lnum" and "pos.col" are valid in "buf".
 * This allows for the col to be on the NUL byte.
 */
    void
check_pos(buf_T *buf, pos_T *pos)
{
    colnr_T len;

    если (pos->lnum > buf->b_ml.ml_line_count)
	pos->lnum = buf->b_ml.ml_line_count;

    если (pos->col > 0)
    {
	len = ml_get_buf_len(buf, pos->lnum);
	если (pos->col > len)
	    pos->col = len;
    }
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    void
check_cursor_lnum(void)
{
    если (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
#ifdef FEAT_FOLDING
	// If there is a closed fold at the end of the file, put the cursor in
	// its first line.  Otherwise in the last line.
	если (!hasFolding(curbuf->b_ml.ml_line_count,
						&curwin->w_cursor.lnum, NULL))
#endif
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    }
    если (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * Make sure curwin->w_cursor.col is valid.
 */
    void
check_cursor_col(void)
{
    check_cursor_col_win(curwin);
}

/*
 * Make sure win->w_cursor.col is valid.
 */
    void
check_cursor_col_win(win_T *win)
{
    colnr_T      len;
    colnr_T      oldcol = win->w_cursor.col;
    colnr_T      oldcoladd = win->w_cursor.col + win->w_cursor.coladd;
    unsigned целое cur_ve_flags = get_ve_flags();

    len = ml_get_buf_len(win->w_buffer, win->w_cursor.lnum);
    если (len == 0)
	win->w_cursor.col = 0;
    иначе если (win->w_cursor.col >= len)
    {
	// Allow cursor past end-of-line when:
	// - in Insert mode or restarting Insert mode
	// - in Visual mode and 'selection' isn't "old"
	// - 'virtualedit' is set
	если ((State & MODE_INSERT) || restart_edit
		|| (VIsual_active && *p_sel != 'o')
		|| (cur_ve_flags & VE_ONEMORE)
		|| virtual_active())
	    win->w_cursor.col = len;
	иначе
	{
	    win->w_cursor.col = len - 1;
	    // Move the cursor to the head byte.
	    если (has_mbyte)
		mb_adjustpos(win->w_buffer, &win->w_cursor);
	}
    }
    иначе если (win->w_cursor.col < 0)
	win->w_cursor.col = 0;

    // If virtual editing is on, we can leave the cursor on the old position,
    // only we must set it to virtual.  But don't do it when at the end of the
    // line.
    если (oldcol == MAXCOL)
	win->w_cursor.coladd = 0;
    иначе если (cur_ve_flags == VE_ALL)
    {
	если (oldcoladd > win->w_cursor.col)
	{
	    win->w_cursor.coladd = oldcoladd - win->w_cursor.col;

	    // Make sure that coladd is not more than the char width.
	    // Not for the last character, coladd is then used when the cursor
	    // is actually after the last character.
	    если (win->w_cursor.col + 1 < len)
	    {
		целое cs, ce;

		getvcol(win, &win->w_cursor, &cs, NULL, &ce, 0);
		если (win->w_cursor.coladd > ce - cs)
		    win->w_cursor.coladd = ce - cs;
	    }
	}
	иначе
	    // avoid weird number when there is a miscalculation or overflow
	    win->w_cursor.coladd = 0;
    }
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    void
check_cursor(void)
{
    check_cursor_lnum();
    check_cursor_col();
}

/*
 * Check if VIsual position is valid, correct it if not.
 * Can be called when in Visual mode and a change has been made.
 */
    void
check_visual_pos(void)
{
    если (VIsual.lnum > curbuf->b_ml.ml_line_count)
    {
	VIsual.lnum = curbuf->b_ml.ml_line_count;
	VIsual.col = 0;
	VIsual.coladd = 0;
    }
    иначе
    {
	целое len = ml_get_len(VIsual.lnum);

	если (VIsual.col > len)
	{
	    VIsual.col = len;
	    VIsual.coladd = 0;
	}
    }
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 * Allow it when in Visual mode and 'selection' is not "old".
 */
    void
adjust_cursor_col(void)
{
    если (curwin->w_cursor.col > 0
	    && (!VIsual_active || *p_sel == 'o')
	    && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * Set "curwin->w_leftcol" to "leftcol".
 * Adjust the cursor position if needed.
 * Return TRUE if the cursor was moved.
 */
    целое
set_leftcol(colnr_T leftcol)
{
    целое		retval = FALSE;

    // Return quickly when there is no change.
    если (curwin->w_leftcol == leftcol)
	вернуть FALSE;
    curwin->w_leftcol = leftcol;

    changed_cline_bef_curs();
    long lastcol = curwin->w_leftcol + curwin->w_width - curwin_col_off() - 1;
    validate_virtcol();

    // If the cursor is right or left of the screen, move it to last or first
    // visible character.
    long siso = get_sidescrolloff_value();
    если (curwin->w_virtcol > (colnr_T)(lastcol - siso))
    {
	retval = TRUE;
	coladvance((colnr_T)(lastcol - siso));
    }
    иначе если (curwin->w_virtcol < curwin->w_leftcol + siso)
    {
	retval = TRUE;
	(void)coladvance((colnr_T)(curwin->w_leftcol + siso));
    }

    // If the start of the character under the cursor is not on the screen,
    // advance the cursor one more char.  If this fails (last char of the
    // line) adjust the scrolling.
    colnr_T	s, e;
    getvvcol(curwin, &curwin->w_cursor, &s, NULL, &e, 0);
    если (e > (colnr_T)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    иначе если (s < curwin->w_leftcol)
    {
	retval = TRUE;
	если (coladvance(e + 1) == FAIL)	// there isn't another character
	{
	    curwin->w_leftcol = s;	// adjust w_leftcol instead
	    changed_cline_bef_curs();
	}
    }

    если (retval)
	curwin->w_set_curswant = true;
    redraw_later(UPD_NOT_VALID);
    вернуть retval;
}

/*
 * Isolate one part of a string option where parts are separated with
 * "sep_chars".
 * The part is copied into "buf[maxlen]".
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    целое
copy_option_part(
    char_u	**option,
    char_u	*buf,
    целое		maxlen,
    char	*sep_chars)
{
    целое	    len = 0;
    char_u  *p = *option;

    // skip '.' at start of option part, for 'suffixes'
    если (*p == '.')
	buf[len++] = *p++;
    пока (*p != NUL && vim_strchr((char_u *)sep_chars, *p) == NULL)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	если (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != NULL)
	    ++p;
	если (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    если (*p != NUL && *p != ',')	// skip non-standard separator
	++p;
    p = skip_to_option_part(p);	// p points to next file name

    *option = p;
    вернуть len;
}

#если !defined(HAVE_MEMSET) && !defined(PROTO)
    void *
vim_memset(void *ptr, целое c, size_t size)
{
    char *p = ptr;

    пока (size-- > 0)
	*p++ = c;
    вернуть ptr;
}
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    целое
vim_isspace(целое x)
{
    вернуть ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

static struct modmasktable
{
    short	mod_mask;	// Bit-mask for particular key modifier
    short	mod_flag;	// Bit(s) for particular key modifier
    char_u	name;		// Single letter name of modifier
} mod_mask_table[] =
{
    {MOD_MASK_ALT,		MOD_MASK_ALT,		(char_u)'M'},
    {MOD_MASK_META,		MOD_MASK_META,		(char_u)'T'},
    {MOD_MASK_CTRL,		MOD_MASK_CTRL,		(char_u)'C'},
    {MOD_MASK_SHIFT,		MOD_MASK_SHIFT,		(char_u)'S'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_2CLICK,	(char_u)'2'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_3CLICK,	(char_u)'3'},
    {MOD_MASK_MULTI_CLICK,	MOD_MASK_4CLICK,	(char_u)'4'},
#если defined(MACOS_X) || defined(FEAT_GUI_GTK)
    {MOD_MASK_CMD,		MOD_MASK_CMD,		(char_u)'D'},
#endif
    // 'A' must be the last one
    {MOD_MASK_ALT,		MOD_MASK_ALT,		(char_u)'A'},
    {0, 0, NUL}
    // NOTE: when adding an entry, update MAX_KEY_NAME_LEN!
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled separately!
 */
#define MOD_KEYS_ENTRY_SIZE 5

static char_u modifier_keys_table[] =
{
//  mod mask	    with modifier		without modifier
    MOD_MASK_SHIFT, '&', '9',			'@', '1',	// begin
    MOD_MASK_SHIFT, '&', '0',			'@', '2',	// cancel
    MOD_MASK_SHIFT, '*', '1',			'@', '4',	// command
    MOD_MASK_SHIFT, '*', '2',			'@', '5',	// copy
    MOD_MASK_SHIFT, '*', '3',			'@', '6',	// create
    MOD_MASK_SHIFT, '*', '4',			'k', 'D',	// delete char
    MOD_MASK_SHIFT, '*', '5',			'k', 'L',	// delete line
    MOD_MASK_SHIFT, '*', '7',			'@', '7',	// end
    MOD_MASK_CTRL,  KS_EXTRA, (целое)KE_C_END,	'@', '7',	// end
    MOD_MASK_SHIFT, '*', '9',			'@', '9',	// exit
    MOD_MASK_SHIFT, '*', '0',			'@', '0',	// find
    MOD_MASK_SHIFT, '#', '1',			'%', '1',	// help
    MOD_MASK_SHIFT, '#', '2',			'k', 'h',	// home
    MOD_MASK_CTRL,  KS_EXTRA, (целое)KE_C_HOME,	'k', 'h',	// home
    MOD_MASK_SHIFT, '#', '3',			'k', 'I',	// insert
    MOD_MASK_SHIFT, '#', '4',			'k', 'l',	// left arrow
    MOD_MASK_CTRL,  KS_EXTRA, (целое)KE_C_LEFT,	'k', 'l',	// left arrow
    MOD_MASK_SHIFT, '%', 'a',			'%', '3',	// message
    MOD_MASK_SHIFT, '%', 'b',			'%', '4',	// move
    MOD_MASK_SHIFT, '%', 'c',			'%', '5',	// next
    MOD_MASK_SHIFT, '%', 'd',			'%', '7',	// options
    MOD_MASK_SHIFT, '%', 'e',			'%', '8',	// previous
    MOD_MASK_SHIFT, '%', 'f',			'%', '9',	// print
    MOD_MASK_SHIFT, '%', 'g',			'%', '0',	// redo
    MOD_MASK_SHIFT, '%', 'h',			'&', '3',	// replace
    MOD_MASK_SHIFT, '%', 'i',			'k', 'r',	// right arr.
    MOD_MASK_CTRL,  KS_EXTRA, (целое)KE_C_RIGHT,	'k', 'r',	// right arr.
    MOD_MASK_SHIFT, '%', 'j',			'&', '5',	// resume
    MOD_MASK_SHIFT, '!', '1',			'&', '6',	// save
    MOD_MASK_SHIFT, '!', '2',			'&', '7',	// suspend
    MOD_MASK_SHIFT, '!', '3',			'&', '8',	// undo
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_UP,	'k', 'u',	// up arrow
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_DOWN,	'k', 'd',	// down arrow

								// vt100 F1
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_XF1,	KS_EXTRA, (целое)KE_XF1,
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_XF2,	KS_EXTRA, (целое)KE_XF2,
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_XF3,	KS_EXTRA, (целое)KE_XF3,
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_XF4,	KS_EXTRA, (целое)KE_XF4,

    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F1,	'k', '1',	// F1
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F2,	'k', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F3,	'k', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F4,	'k', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F5,	'k', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F6,	'k', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F7,	'k', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F8,	'k', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F9,	'k', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F10,	'k', ';',	// F10

    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F11,	'F', '1',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F12,	'F', '2',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F13,	'F', '3',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F14,	'F', '4',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F15,	'F', '5',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F16,	'F', '6',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F17,	'F', '7',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F18,	'F', '8',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F19,	'F', '9',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F20,	'F', 'A',

    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F21,	'F', 'B',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F22,	'F', 'C',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F23,	'F', 'D',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F24,	'F', 'E',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F25,	'F', 'F',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F26,	'F', 'G',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F27,	'F', 'H',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F28,	'F', 'I',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F29,	'F', 'J',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F30,	'F', 'K',

    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F31,	'F', 'L',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F32,	'F', 'M',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F33,	'F', 'N',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F34,	'F', 'O',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F35,	'F', 'P',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F36,	'F', 'Q',
    MOD_MASK_SHIFT, KS_EXTRA, (целое)KE_S_F37,	'F', 'R',

							    // TAB pseudo code
    MOD_MASK_SHIFT, 'k', 'B',			KS_EXTRA, (целое)KE_TAB,

    NUL
};

static struct key_name_entry
{
    целое		enabled;	    // is this entry available (TRUE/FALSE)?
    целое		key;		    // special key code or ascii value
    string_T	name;		    // name of key
    целое		is_alt;		    // is an alternative name
} key_names_table[] =
// Must be sorted by the 'name.string' field in ascending order because it is used by bsearch()!
{
    {TRUE, K_BS, STR_LITERAL_INIT("BackSpace"), TRUE},
    {TRUE, '|', STR_LITERAL_INIT("Bar"), FALSE},
    {TRUE, K_BS, STR_LITERAL_INIT("BS"), FALSE},
    {TRUE, '\\', STR_LITERAL_INIT("Bslash"), FALSE},
    {TRUE, K_COMMAND, STR_LITERAL_INIT("Cmd"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("CR"), FALSE},
    {TRUE, CSI, STR_LITERAL_INIT("CSI"), FALSE},
    {TRUE, K_CURSORHOLD, STR_LITERAL_INIT("CursorHold"), FALSE},
    {
#ifdef FEAT_MOUSE_DEC
    TRUE,
#иначе
    FALSE,
#endif
	K_DEC_MOUSE, STR_LITERAL_INIT("DecMouse"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Del"), FALSE},
    {TRUE, K_DEL, STR_LITERAL_INIT("Delete"), TRUE},
    {TRUE, K_DOWN, STR_LITERAL_INIT("Down"), FALSE},
    {TRUE, K_DROP, STR_LITERAL_INIT("Drop"), FALSE},
    {TRUE, K_END, STR_LITERAL_INIT("End"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Enter"), TRUE},
    {TRUE, ESC, STR_LITERAL_INIT("Esc"), FALSE},

    {TRUE, K_F1, STR_LITERAL_INIT("F1"), FALSE},
    {TRUE, K_F10, STR_LITERAL_INIT("F10"), FALSE},
    {TRUE, K_F11, STR_LITERAL_INIT("F11"), FALSE},
    {TRUE, K_F12, STR_LITERAL_INIT("F12"), FALSE},
    {TRUE, K_F13, STR_LITERAL_INIT("F13"), FALSE},
    {TRUE, K_F14, STR_LITERAL_INIT("F14"), FALSE},
    {TRUE, K_F15, STR_LITERAL_INIT("F15"), FALSE},
    {TRUE, K_F16, STR_LITERAL_INIT("F16"), FALSE},
    {TRUE, K_F17, STR_LITERAL_INIT("F17"), FALSE},
    {TRUE, K_F18, STR_LITERAL_INIT("F18"), FALSE},
    {TRUE, K_F19, STR_LITERAL_INIT("F19"), FALSE},

    {TRUE, K_F2, STR_LITERAL_INIT("F2"), FALSE},
    {TRUE, K_F20, STR_LITERAL_INIT("F20"), FALSE},
    {TRUE, K_F21, STR_LITERAL_INIT("F21"), FALSE},
    {TRUE, K_F22, STR_LITERAL_INIT("F22"), FALSE},
    {TRUE, K_F23, STR_LITERAL_INIT("F23"), FALSE},
    {TRUE, K_F24, STR_LITERAL_INIT("F24"), FALSE},
    {TRUE, K_F25, STR_LITERAL_INIT("F25"), FALSE},
    {TRUE, K_F26, STR_LITERAL_INIT("F26"), FALSE},
    {TRUE, K_F27, STR_LITERAL_INIT("F27"), FALSE},
    {TRUE, K_F28, STR_LITERAL_INIT("F28"), FALSE},
    {TRUE, K_F29, STR_LITERAL_INIT("F29"), FALSE},

    {TRUE, K_F3, STR_LITERAL_INIT("F3"), FALSE},
    {TRUE, K_F30, STR_LITERAL_INIT("F30"), FALSE},
    {TRUE, K_F31, STR_LITERAL_INIT("F31"), FALSE},
    {TRUE, K_F32, STR_LITERAL_INIT("F32"), FALSE},
    {TRUE, K_F33, STR_LITERAL_INIT("F33"), FALSE},
    {TRUE, K_F34, STR_LITERAL_INIT("F34"), FALSE},
    {TRUE, K_F35, STR_LITERAL_INIT("F35"), FALSE},
    {TRUE, K_F36, STR_LITERAL_INIT("F36"), FALSE},
    {TRUE, K_F37, STR_LITERAL_INIT("F37"), FALSE},

    {TRUE, K_F4, STR_LITERAL_INIT("F4"), FALSE},
    {TRUE, K_F5, STR_LITERAL_INIT("F5"), FALSE},
    {TRUE, K_F6, STR_LITERAL_INIT("F6"), FALSE},
    {TRUE, K_F7, STR_LITERAL_INIT("F7"), FALSE},
    {TRUE, K_F8, STR_LITERAL_INIT("F8"), FALSE},
    {TRUE, K_F9, STR_LITERAL_INIT("F9"), FALSE},

    {TRUE, K_FOCUSGAINED, STR_LITERAL_INIT("FocusGained"), FALSE},
    {TRUE, K_FOCUSLOST, STR_LITERAL_INIT("FocusLost"), FALSE},
    {TRUE, K_HELP, STR_LITERAL_INIT("Help"), FALSE},
    {TRUE, K_HOME, STR_LITERAL_INIT("Home"), FALSE},
    {TRUE, K_IGNORE, STR_LITERAL_INIT("Ignore"), FALSE},
    {TRUE, K_INS, STR_LITERAL_INIT("Ins"), TRUE},
    {TRUE, K_INS, STR_LITERAL_INIT("Insert"), FALSE},
    {
#ifdef FEAT_MOUSE_JSB
    TRUE,
#иначе
    FALSE,
#endif
	K_JSBTERM_MOUSE, STR_LITERAL_INIT("JsbMouse"), FALSE},
    {TRUE, K_K0, STR_LITERAL_INIT("k0"), FALSE},
    {TRUE, K_K1, STR_LITERAL_INIT("k1"), FALSE},
    {TRUE, K_K2, STR_LITERAL_INIT("k2"), FALSE},
    {TRUE, K_K3, STR_LITERAL_INIT("k3"), FALSE},
    {TRUE, K_K4, STR_LITERAL_INIT("k4"), FALSE},
    {TRUE, K_K5, STR_LITERAL_INIT("k5"), FALSE},
    {TRUE, K_K6, STR_LITERAL_INIT("k6"), FALSE},
    {TRUE, K_K7, STR_LITERAL_INIT("k7"), FALSE},
    {TRUE, K_K8, STR_LITERAL_INIT("k8"), FALSE},
    {TRUE, K_K9, STR_LITERAL_INIT("k9"), FALSE},

    {TRUE, K_KDEL, STR_LITERAL_INIT("kDel"), FALSE},
    {TRUE, K_KDIVIDE, STR_LITERAL_INIT("kDivide"), FALSE},
    {TRUE, K_KEND, STR_LITERAL_INIT("kEnd"), FALSE},
    {TRUE, K_KENTER, STR_LITERAL_INIT("kEnter"), FALSE},
    {TRUE, K_KHOME, STR_LITERAL_INIT("kHome"), FALSE},
    {TRUE, K_KINS, STR_LITERAL_INIT("kInsert"), FALSE},
    {TRUE, K_KMINUS, STR_LITERAL_INIT("kMinus"), FALSE},
    {TRUE, K_KMULTIPLY, STR_LITERAL_INIT("kMultiply"), FALSE},
    {TRUE, K_KPAGEDOWN, STR_LITERAL_INIT("kPageDown"), FALSE},
    {TRUE, K_KPAGEUP, STR_LITERAL_INIT("kPageUp"), FALSE},
    {TRUE, K_KPLUS, STR_LITERAL_INIT("kPlus"), FALSE},
    {TRUE, K_KPOINT, STR_LITERAL_INIT("kPoint"), FALSE},
    {TRUE, K_LEFT, STR_LITERAL_INIT("Left"), FALSE},
    {TRUE, K_LEFTDRAG, STR_LITERAL_INIT("LeftDrag"), FALSE},
    {TRUE, K_LEFTMOUSE, STR_LITERAL_INIT("LeftMouse"), FALSE},
    {TRUE, K_LEFTMOUSE_NM, STR_LITERAL_INIT("LeftMouseNM"), FALSE},
    {TRUE, K_LEFTRELEASE, STR_LITERAL_INIT("LeftRelease"), FALSE},
    {TRUE, K_LEFTRELEASE_NM, STR_LITERAL_INIT("LeftReleaseNM"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("LF"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("LineFeed"), TRUE},
    {TRUE, '<', STR_LITERAL_INIT("lt"), FALSE},
    {TRUE, K_MIDDLEDRAG, STR_LITERAL_INIT("MiddleDrag"), FALSE},
    {TRUE, K_MIDDLEMOUSE, STR_LITERAL_INIT("MiddleMouse"), FALSE},
    {TRUE, K_MIDDLERELEASE, STR_LITERAL_INIT("MiddleRelease"), FALSE},
    {TRUE, K_MOUSE, STR_LITERAL_INIT("Mouse"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("MouseDown"), TRUE},
    {TRUE, K_MOUSEMOVE, STR_LITERAL_INIT("MouseMove"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("MouseUp"), TRUE},
    {
#ifdef FEAT_MOUSE_NET
    TRUE,
#иначе
    FALSE,
#endif
	K_NETTERM_MOUSE, STR_LITERAL_INIT("NetMouse"), FALSE},
    {TRUE, NL, STR_LITERAL_INIT("NewLine"), TRUE},
    {TRUE, NL, STR_LITERAL_INIT("NL"), FALSE},
    {TRUE, K_ZERO, STR_LITERAL_INIT("Nul"), FALSE},
    {TRUE, OSC, STR_LITERAL_INIT("OSC"), FALSE},
    {TRUE, K_PAGEDOWN, STR_LITERAL_INIT("PageDown"), FALSE},
    {TRUE, K_PAGEUP, STR_LITERAL_INIT("PageUp"), FALSE},
    {TRUE, K_PE, STR_LITERAL_INIT("PasteEnd"), FALSE},
    {TRUE, K_PS, STR_LITERAL_INIT("PasteStart"), FALSE},
    {TRUE, K_PLUG, STR_LITERAL_INIT("Plug"), FALSE},
    {
#ifdef FEAT_MOUSE_PTERM
    TRUE,
#иначе
    FALSE,
#endif
	K_PTERM_MOUSE, STR_LITERAL_INIT("PtermMouse"), FALSE},
    {TRUE, CAR, STR_LITERAL_INIT("Return"), TRUE},
    {TRUE, K_RIGHT, STR_LITERAL_INIT("Right"), FALSE},
    {TRUE, K_RIGHTDRAG, STR_LITERAL_INIT("RightDrag"), FALSE},
    {TRUE, K_RIGHTMOUSE, STR_LITERAL_INIT("RightMouse"), FALSE},
    {TRUE, K_RIGHTRELEASE, STR_LITERAL_INIT("RightRelease"), FALSE},
    {TRUE, K_SCRIPT_COMMAND, STR_LITERAL_INIT("ScriptCmd"), FALSE},
    {TRUE, K_MOUSEUP, STR_LITERAL_INIT("ScrollWheelDown"), FALSE},
    {TRUE, K_MOUSERIGHT, STR_LITERAL_INIT("ScrollWheelLeft"), FALSE},
    {TRUE, K_MOUSELEFT, STR_LITERAL_INIT("ScrollWheelRight"), FALSE},
    {TRUE, K_MOUSEDOWN, STR_LITERAL_INIT("ScrollWheelUp"), FALSE},
    {TRUE, K_SGR_MOUSE, STR_LITERAL_INIT("SgrMouse"), FALSE},
    {TRUE, K_SGR_MOUSERELEASE, STR_LITERAL_INIT("SgrMouseRelease"), FALSE},
    {
#ifdef FEAT_EVAL
    TRUE,
#иначе
    FALSE,
#endif
	K_SNR, STR_LITERAL_INIT("SNR"), FALSE},
    {TRUE, ' ', STR_LITERAL_INIT("Space"), FALSE},
    {TRUE, TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_TAB, STR_LITERAL_INIT("Tab"), FALSE},
    {TRUE, K_UNDO, STR_LITERAL_INIT("Undo"), FALSE},
    {TRUE, K_UP, STR_LITERAL_INIT("Up"), FALSE},
    {
#ifdef FEAT_MOUSE_URXVT
    TRUE,
#иначе
    FALSE,
#endif
	K_URXVT_MOUSE, STR_LITERAL_INIT("UrxvtMouse"), FALSE},
    {TRUE, K_X1DRAG, STR_LITERAL_INIT("X1Drag"), FALSE},
    {TRUE, K_X1MOUSE, STR_LITERAL_INIT("X1Mouse"), FALSE},
    {TRUE, K_X1RELEASE, STR_LITERAL_INIT("X1Release"), FALSE},
    {TRUE, K_X2DRAG, STR_LITERAL_INIT("X2Drag"), FALSE},
    {TRUE, K_X2MOUSE, STR_LITERAL_INIT("X2Mouse"), FALSE},
    {TRUE, K_X2RELEASE, STR_LITERAL_INIT("X2Release"), FALSE},
    {TRUE, K_CSI, STR_LITERAL_INIT("xCSI"), FALSE},
    {TRUE, K_XDOWN, STR_LITERAL_INIT("xDown"), FALSE},
    {TRUE, K_XEND, STR_LITERAL_INIT("xEnd"), FALSE},
    {TRUE, K_XF1, STR_LITERAL_INIT("xF1"), FALSE},
    {TRUE, K_XF2, STR_LITERAL_INIT("xF2"), FALSE},
    {TRUE, K_XF3, STR_LITERAL_INIT("xF3"), FALSE},
    {TRUE, K_XF4, STR_LITERAL_INIT("xF4"), FALSE},
    {TRUE, K_XHOME, STR_LITERAL_INIT("xHome"), FALSE},
    {TRUE, K_XLEFT, STR_LITERAL_INIT("xLeft"), FALSE},
    {TRUE, K_OSC, STR_LITERAL_INIT("xOSC"), FALSE},
    {TRUE, K_XRIGHT, STR_LITERAL_INIT("xRight"), FALSE},
    {TRUE, K_XUP, STR_LITERAL_INIT("xUp"), FALSE},
    {TRUE, K_ZEND, STR_LITERAL_INIT("zEnd"), FALSE},
    {TRUE, K_ZHOME, STR_LITERAL_INIT("zHome"), FALSE}
    // NOTE: When adding a long name update MAX_KEY_NAME_LEN.
};

/*
 * Return the modifier mask bit (MOD_MASK_*) which corresponds to the given
 * modifier name ('S' for Shift, 'C' for Ctrl etc).
 */
    static целое
name_to_mod_mask(целое c)
{
    целое	    i;

    c = TOUPPER_ASC(c);
    для (i = 0; mod_mask_table[i].mod_mask != 0; i++)
	если (c == mod_mask_table[i].name)
	    вернуть mod_mask_table[i].mod_flag;
    вернуть 0;
}

/*
 * Check if if there is a special key code for "key" that includes the
 * modifiers specified.
 */
    целое
simplify_key(целое key, целое *modifiers)
{
    целое	    i;
    целое	    key0;
    целое	    key1;

    если (!(*modifiers & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	вернуть key;

    // TAB is a special case
    если (key == TAB && (*modifiers & MOD_MASK_SHIFT))
    {
	*modifiers &= ~MOD_MASK_SHIFT;
	вернуть K_S_TAB;
    }
    key0 = KEY2TERMCAP0(key);
    key1 = KEY2TERMCAP1(key);
    для (i = 0; modifier_keys_table[i] != NUL; i += MOD_KEYS_ENTRY_SIZE)
    {
	если (key0 == modifier_keys_table[i + 3]
		&& key1 == modifier_keys_table[i + 4]
		&& (*modifiers & modifier_keys_table[i]))
	{
	    *modifiers &= ~modifier_keys_table[i];
	    вернуть TERMCAP2KEY(modifier_keys_table[i + 1],
		    modifier_keys_table[i + 2]);
	}
    }
    вернуть key;
}

/*
 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
 */
    целое
handle_x_keys(целое key)
{
    switch (key)
    {
	case K_XUP:	вернуть K_UP;
	case K_XDOWN:	вернуть K_DOWN;
	case K_XLEFT:	вернуть K_LEFT;
	case K_XRIGHT:	вернуть K_RIGHT;
	case K_XHOME:	вернуть K_HOME;
	case K_ZHOME:	вернуть K_HOME;
	case K_XEND:	вернуть K_END;
	case K_ZEND:	вернуть K_END;
	case K_XF1:	вернуть K_F1;
	case K_XF2:	вернуть K_F2;
	case K_XF3:	вернуть K_F3;
	case K_XF4:	вернуть K_F4;
	case K_S_XF1:	вернуть K_S_F1;
	case K_S_XF2:	вернуть K_S_F2;
	case K_S_XF3:	вернуть K_S_F3;
	case K_S_XF4:	вернуть K_S_F4;
    }
    вернуть key;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_key_name(целое c, целое modifiers)
{
    static char_u string[MAX_KEY_NAME_LEN + 1];
    целое	    i, idx, len;
    целое	    table_idx;

    string[0] = '<';
    idx = 1;

    // Key that stands for a normal character.
    если (IS_SPECIAL(c) && KEY2TERMCAP0(c) == KS_KEY)
	c = KEY2TERMCAP1(c);

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     * Same for CTRL and ALT modifiers.
     */
    если (IS_SPECIAL(c))
    {
	для (i = 0; modifier_keys_table[i] != 0; i += MOD_KEYS_ENTRY_SIZE)
	    если (       KEY2TERMCAP0(c) == (целое)modifier_keys_table[i + 1]
		    && (целое)KEY2TERMCAP1(c) == (целое)modifier_keys_table[i + 2])
	    {
		modifiers |= modifier_keys_table[i];
		c = TERMCAP2KEY(modifier_keys_table[i + 3],
						   modifier_keys_table[i + 4]);
		break;
	    }
    }

    // try to find the key in the special key table
    table_idx = find_special_key_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    если (c > 0 && (*mb_char2len)(c) == 1)
    {
	если (table_idx < 0
		&& (!vim_isprintc(c) || (c & 0x7f) == ' ')
		&& (c & 0x80))
	{
	    c &= 0x7f;
	    modifiers |= MOD_MASK_ALT;
	    // try again, to find the un-alted key in the special key table
	    table_idx = find_special_key_in_table(c);
	}
	если (table_idx < 0 && !vim_isprintc(c) && c < ' ')
	{
	    c += '@';
	    modifiers |= MOD_MASK_CTRL;
	}
    }

    // translate the modifier into a string
    для (i = 0; mod_mask_table[i].name != 'A'; i++)
	если ((modifiers & mod_mask_table[i].mod_mask)
						== mod_mask_table[i].mod_flag)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    если (table_idx < 0)		// unknown special key, may output t_xx
    {
	если (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	// Not a special key, only modifiers, output directly
	иначе
	{
	    len = (*mb_char2len)(c);
	    если (len == 1 && vim_isprintc(c))
		string[idx++] = c;
	    иначе если (has_mbyte && len > 1)
		idx += (*mb_char2bytes)(c, string + idx);
	    иначе
	    {
		char_u	*s = transchar(c);
		пока (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    иначе		// use name of special key
    {
	string_T    *s;

	s = &key_names_table[table_idx].name;

	если (s->length + idx + 2 <= MAX_KEY_NAME_LEN)
	{
	    STRCPY(string + idx, s->string);
	    idx += (целое)s->length;
	}
    }
    string[idx++] = '>';
    string[idx] = NUL;

    вернуть string;
}

/*
 * Try translating a <> name at "(*srcp)[]" to "dst[]".
 * Return the number of characters added to "dst[]", zero for no match.
 * If there is a match, "srcp" is advanced to after the <> name.
 * "dst[]" must be big enough to hold the result (up to six characters)!
 */
    целое
trans_special(
    char_u	**srcp,
    char_u	*dst,
    целое		flags,		// FSK_ values
    целое		escape_ks,	// escape K_SPECIAL bytes in the character
    целое		*did_simplify)  // FSK_SIMPLIFY and found <C-H> or <A-x>
{
    целое		modifiers = 0;
    целое		key;

    key = find_special_key(srcp, &modifiers, flags, did_simplify);
    если (key == 0)
	вернуть 0;

    вернуть special_to_buf(key, modifiers, escape_ks, dst);
}

/*
 * Put the character sequence for "key" with "modifiers" into "dst" and return
 * the resulting length.
 * When "escape_ks" is TRUE escape K_SPECIAL bytes in the character.
 * The sequence is not NUL terminated.
 * This is how characters in a string are encoded.
 */
    целое
special_to_buf(целое key, целое modifiers, целое escape_ks, char_u *dst)
{
    целое		dlen = 0;

    // Put the appropriate modifier in a string
    если (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    если (IS_SPECIAL(key))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(key);
	dst[dlen++] = KEY2TERMCAP1(key);
    }
    иначе если (escape_ks)
	dlen = (целое)(add_char2buf(key, dst + dlen) - dst);
    иначе если (has_mbyte)
	dlen += (*mb_char2bytes)(key, dst + dlen);
    иначе
	dst[dlen++] = key;

    вернуть dlen;
}

/*
 * Try translating a <> name at "(*srcp)[]", return the key and put modifiers
 * in "modp".
 * "srcp" is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    целое
find_special_key(
    char_u	**srcp,
    целое		*modp,
    целое		flags,		// FSK_ values
    целое		*did_simplify)  // found <C-H> or <A-x>
{
    char_u	*last_dash;
    char_u	*end_of_name;
    char_u	*src;
    char_u	*bp;
    целое		in_string = flags & FSK_IN_STRING;
    целое		modifiers;
    целое		bit;
    целое		key;
    uvarnumber_T	n;
    целое		l;

    src = *srcp;
    если (src[0] != '<')
	вернуть 0;
    если (src[1] == '*')	    // <*xxx>: do not simplify
	++src;

    // Find end of modifier list
    last_dash = src;
    для (bp = src + 1; *bp == '-' || vim_isNormalIDc(*bp); bp++)
    {
	если (*bp == '-')
	{
	    last_dash = bp;
	    если (bp[1] != NUL)
	    {
		если (has_mbyte)
		    l = mb_ptr2len(bp + 1);
		иначе
		    l = 1;
		// Anything accepted, like <C-?>.
		// <C-"> or <M-"> are not special in strings as " is
		// the string delimiter. With a backslash it works: <M-\">
		если (!(in_string && bp[1] == '"') && bp[l + 1] == '>')
		    bp += l;
		иначе если (in_string && bp[1] == '\\' && bp[2] == '"'
							   && bp[3] == '>')
		    bp += 2;
	    }
	}
	если (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	// skip t_xx, xx may be '-' or '>'
	иначе если (STRNICMP(bp, "char-", 5) == 0)
	{
	    vim_str2nr(bp + 5, NULL, &l, STR2NR_ALL, NULL, NULL, 0, TRUE, NULL);
	    если (l == 0)
	    {
		emsg(_(e_invalid_argument));
		вернуть 0;
	    }
	    bp += l + 5;
	    break;
	}
    }

    если (*bp == '>')	// found matching '>'
    {
	end_of_name = bp + 1;

	// Which modifiers are given?
	modifiers = 0x0;
	для (bp = src + 1; bp < last_dash; bp++)
	{
	    если (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		если (bit == 0x0)
		    break;	// Illegal modifier name
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	если (bp >= last_dash)
	{
	    если (STRNICMP(last_dash + 1, "char-", 5) == 0
						 && VIM_ISDIGIT(last_dash[6]))
	    {
		// <Char-123> or <Char-033> or <Char-0x33>
		vim_str2nr(last_dash + 6, NULL, &l, STR2NR_ALL, NULL,
							    &n, 0, TRUE, NULL);
		если (l == 0)
		{
		    emsg(_(e_invalid_argument));
		    вернуть 0;
		}
		key = (целое)n;
	    }
	    иначе
	    {
		целое off = 1;

		// Modifier with single letter, or special key name.
		если (in_string && last_dash[1] == '\\' && last_dash[2] == '"')
		    off = 2;
		если (has_mbyte)
		    l = mb_ptr2len(last_dash + off);
		иначе
		    l = 1;
		если (modifiers != 0 && last_dash[l + off] == '>')
		    key = PTR2CHAR(last_dash + off);
		иначе
		{
		    key = get_special_key_code(last_dash + off);
		    если (!(flags & FSK_KEEP_X_KEY))
			key = handle_x_keys(key);
		}
	    }

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    если (key != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		key = simplify_key(key, &modifiers);

		если ((flags & FSK_KEYCODE) == 0)
		{
		    // don't want keycode, use single byte code
		    если (key == K_BS)
			key = BS;
		    иначе если (key == K_DEL || key == K_KDEL)
			key = DEL;
		}
		иначе если (key == 27
			&& (flags & FSK_FROM_PART) != 0
			&& (kitty_protocol_state == KKPS_ENABLED
			    || kitty_protocol_state == KKPS_DISABLED))
		{
		    // Using the Kitty key protocol, which uses K_ESC for an
		    // Esc character.  For the simplified keys use the Esc
		    // character and set did_simplify, then in the
		    // non-simplified keys use K_ESC.
		    если ((flags & FSK_SIMPLIFY) != 0)
		    {
			если (did_simplify != NULL)
			    *did_simplify = TRUE;
		    }
		    иначе
			key = K_ESC;
		}

		// Normal Key with modifier: Try to make a single byte code.
		если (!IS_SPECIAL(key))
		    key = extract_modifiers(key, &modifiers,
					   flags & FSK_SIMPLIFY, did_simplify);

		*modp = modifiers;
		*srcp = end_of_name;
		вернуть key;
	    }
	}
    }
    вернуть 0;
}


/*
 * Some keys are used with Ctrl without Shift and are still expected to be
 * mapped as if Shift was pressed:
 * CTRL-2 is CTRL-@
 * CTRL-6 is CTRL-^
 * CTRL-- is CTRL-_
 * Also, unless no_reduce_keys is set then <C-H> and <C-h> mean the same thing,
 * use "H".
 * Returns the possibly adjusted key.
 */
    целое
may_adjust_key_for_ctrl(целое modifiers, целое key)
{
    если ((modifiers & MOD_MASK_CTRL) == 0)
	вернуть key;

    если (ASCII_ISALPHA(key))
    {
#ifdef FEAT_TERMINAL
	check_no_reduce_keys();  // may update the no_reduce_keys flag
#endif
	вернуть no_reduce_keys == 0 ? TOUPPER_ASC(key) : key;
    }
    если (key == '2')
	вернуть '@';
    если (key == '6')
	вернуть '^';
    если (key == '-')
	вернуть '_';

    // On a Belgian keyboard AltGr $ is ']', on other keyboards '$' can only be
    // obtained with Shift.  Assume that '$' without shift implies a Belgian
    // keyboard, where CTRL-$ means CTRL-].
    если (key == '$' && (modifiers & MOD_MASK_SHIFT) == 0)
	вернуть ']';

    вернуть key;
}

/*
 * Some keys already have Shift included, pass them as normal keys.
 * When Ctrl is also used <C-H> and <C-S-H> are different, but <C-S-{> should
 * be <C-{>.  Same for <C-S-}> and <C-S-|>.
 * Also for <A-S-a> and <M-S-a>.
 * This includes all printable ASCII characters except a-z.
 * Digits are included because with AZERTY the Shift key is used to get them.
 */
    целое
may_remove_shift_modifier(целое modifiers, целое key)
{
    если ((modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
#ifdef FEAT_GUI_GTK
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_CMD)
#endif
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    && ((key >= '!' && key <= '/')
		|| (key >= ':' && key <= 'Z')
		|| vim_isdigit(key)
		|| (key >= '[' && key <= '`')
		|| (key >= '{' && key <= '~')))
	вернуть modifiers & ~MOD_MASK_SHIFT;

    если (modifiers == (MOD_MASK_SHIFT | MOD_MASK_CTRL)
		&& (key == '{' || key == '}' || key == '|'))
	вернуть modifiers & ~MOD_MASK_SHIFT;

    вернуть modifiers;
}

/*
 * Try to include modifiers in the key.
 * Changes "Shift-a" to 'A', "Alt-A" to 0xc0, etc.
 * When "simplify" is FALSE don't do Ctrl and Alt.
 * When "simplify" is TRUE and Ctrl or Alt is removed from modifiers set
 * "did_simplify" when it's not NULL.
 */
    целое
extract_modifiers(целое key, целое *modp, целое simplify, целое *did_simplify)
{
    целое	modifiers = *modp;

#ifdef MACOS_X
    // Command-key really special, no fancynest
    если (!(modifiers & MOD_MASK_CMD))
#endif
    если ((modifiers & MOD_MASK_SHIFT) && ASCII_ISALPHA(key))
    {
	key = TOUPPER_ASC(key);
	// With <C-S-a> we keep the shift modifier.
	// With <S-a>, <A-S-a> and <S-A> we don't keep the shift modifier.
	если (simplify || modifiers == MOD_MASK_SHIFT
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_ALT)
		|| modifiers == (MOD_MASK_SHIFT | MOD_MASK_META))
	    modifiers &= ~MOD_MASK_SHIFT;
    }

    // <C-H> and <C-h> mean the same thing, always use "H"
    если ((modifiers & MOD_MASK_CTRL) && ASCII_ISALPHA(key))
	key = TOUPPER_ASC(key);

    если (simplify && (modifiers & MOD_MASK_CTRL)
	    && ((key >= '?' && key <= '_') || ASCII_ISALPHA(key)))
    {
	key = Ctrl_chr(key);
	modifiers &= ~MOD_MASK_CTRL;
	// <C-@> is <Nul>
	если (key == NUL)
	    key = K_ZERO;
	если (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

#ifdef MACOS_X
    // Command-key really special, no fancynest
    если (!(modifiers & MOD_MASK_CMD))
#endif
    если (simplify && (modifiers & MOD_MASK_ALT) && key < 0x80
	    && !enc_dbcs)		// avoid creating a lead byte
    {
	key |= 0x80;
	modifiers &= ~MOD_MASK_ALT;	// remove the META modifier
	если (did_simplify != NULL)
	    *did_simplify = TRUE;
    }

    *modp = modifiers;
    вернуть key;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    целое
find_special_key_in_table(целое c)
{
    целое	    i;

    для (i = 0; i < (целое)ARRAY_LENGTH(key_names_table); i++)
	если (c == key_names_table[i].key && !key_names_table[i].is_alt)
	    вернуть key_names_table[i].enabled ? i : -1;

    вернуть -1;
}


/*
 * Compare two 'struct key_name_entry' structures.
 * Note that the target string (p1) may contain additional trailing characters
 * that should not factor into the comparison. Example:
 * 'LeftMouse>", "<LeftMouse>"] ...'
 * should match with
 * 'LeftMouse'.
 * These characters are identified by vim_isNormalIDc().
 */
    static целое
cmp_key_name_entry(const void *a, const void *b)
{
    char_u  *p1 = ((struct key_name_entry *)a)->name.string;
    char_u  *p2 = ((struct key_name_entry *)b)->name.string;
    целое	    result = 0;

    если (p1 == p2)
	вернуть 0;

    пока (vim_isNormalIDc(*p1) && *p2 != NUL)
    {
	если ((result = TOLOWER_ASC(*p1) - TOLOWER_ASC(*p2)) != 0)
	    break;
	++p1;
	++p2;
    }

    если (result == 0)
    {
	если (*p2 == NUL)
	{
	    если (vim_isNormalIDc(*p1))
		result = 1;
	}
	иначе
	{
	    result = -1;
	}
    }

    вернуть result;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    целое
get_special_key_code(char_u *name)
{
    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    если (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	char_u  string[3];

	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	если (add_termcap_entry(string, FALSE) == OK)
	    вернуть TERMCAP2KEY(name[2], name[3]);
    }
    иначе
    {
	struct key_name_entry	target;
	struct key_name_entry	*entry;

	target.enabled = TRUE;
	target.key = 0;
	target.name.string = name;
	target.name.length = 0;

	entry = (struct key_name_entry *)bsearch(
	    &target,
	    &key_names_table,
	    ARRAY_LENGTH(key_names_table),
	    sizeof(key_names_table[0]),
	    cmp_key_name_entry);
	если (entry != NULL && entry->enabled)
	{
	    целое key = entry->key;
	    // Both TAB and K_TAB have name "Tab", and it's unspecified which
	    // one bsearch() will return.  TAB is the expected one.
	    вернуть key == K_TAB ? TAB : key;
	}
    }

    вернуть 0;
}

    char_u *
get_key_name(целое i)
{
    если (i < 0 || i >= (целое)ARRAY_LENGTH(key_names_table))
	вернуть NULL;

    вернуть key_names_table[i].name.string;
}

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    целое
get_fileformat(buf_T *buf)
{
    целое		c = *buf->b_p_ff;

    если (buf->b_p_bin || c == 'u')
	вернуть EOL_UNIX;
    если (c == 'm')
	вернуть EOL_MAC;
    вернуть EOL_DOS;
}

/*
 * Like get_fileformat(), but override 'fileformat' with "p" for "++opt=val"
 * argument.
 */
    целое
get_fileformat_force(
    buf_T	*buf,
    exarg_T	*eap)	    // can be NULL!
{
    целое		c;

    если (eap != NULL && eap->force_ff != 0)
	c = eap->force_ff;
    иначе
    {
	если ((eap != NULL && eap->force_bin != 0)
			       ? (eap->force_bin == FORCE_BIN) : buf->b_p_bin)
	    вернуть EOL_UNIX;
	c = *buf->b_p_ff;
    }
    если (c == 'u')
	вернуть EOL_UNIX;
    если (c == 'm')
	вернуть EOL_MAC;
    вернуть EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 * Note: Does _not_ set global value of 'textmode'!
 */
    void
set_fileformat(
    целое		t,
    целое		opt_flags)	// OPT_LOCAL and/or OPT_GLOBAL
{
    char	*p = NULL;

    switch (t)
    {
    case EOL_DOS:
	p = FF_DOS;
	curbuf->b_p_tx = TRUE;
	break;
    case EOL_UNIX:
	p = FF_UNIX;
	curbuf->b_p_tx = FALSE;
	break;
    case EOL_MAC:
	p = FF_MAC;
	curbuf->b_p_tx = FALSE;
	break;
    }
    если (p != NULL)
	set_string_option_direct((char_u *)"ff", -1, (char_u *)p,
						     OPT_FREE | opt_flags, 0);

    // This may cause the buffer to become (un)modified.
    check_status(curbuf);
    redraw_tabline = TRUE;
#если defined(FEAT_TABPANEL)
    redraw_tabpanel = TRUE;
#endif
    need_maketitle = TRUE;	    // set window title later
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    целое
default_fileformat(void)
{
    switch (*p_ffs)
    {
	case 'm':   вернуть EOL_MAC;
	case 'd':   вернуть EOL_DOS;
    }
    вернуть EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    целое
call_shell(char_u *cmd, целое opt)
{
    целое		retval;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif

    если (p_verbose > 3)
    {
	verbose_enter();
	smsg(_("Calling shell to execute: \"%s\""), cmd == NULL ? p_sh : cmd);
	msg_putchar_attr('\n', 0);
	cursor_on();
	verbose_leave();
    }

#ifdef FEAT_PROFILE
    если (do_profiling == PROF_YES)
	prof_child_enter(&wait_time);
#endif

    если (*p_sh == NUL)
    {
	emsg(_(e_shell_option_is_empty));
	retval = -1;
    }
    иначе
    {
#ifdef FEAT_GUI_MSWIN
	// Don't hide the pointer while executing a shell command.
	gui_mch_mousehide(FALSE);
#endif
#ifdef FEAT_GUI
	++hold_gui_events;
#endif
	// The external command may update a tags file, clear cached tags.
	tag_freematch();

	если (cmd == NULL || *p_sxq == NUL)
	    retval = mch_call_shell(cmd, opt);
	иначе
	{
	    char_u  *ncmd;
	    size_t  ncmdsize;
	    char_u  *ecmd = cmd;

	    если (*p_sxe != NUL && *p_sxq == '(')
	    {
		ecmd = vim_strsave_escaped_ext(cmd, p_sxe, '^', FALSE);
		если (ecmd == NULL)
		    ecmd = cmd;
	    }
	    ncmdsize = STRLEN(ecmd) + STRLEN(p_sxq) * 2 + 1;
	    ncmd = alloc(ncmdsize);
	    если (ncmd != NULL)
	    {
		// When 'shellxquote' is ( append ).
		// When 'shellxquote' is "( append )".
		vim_snprintf((char *)ncmd, ncmdsize, "%s%s%s", p_sxq, ecmd, *p_sxq == '(' ? (char_u *)")"
		    : *p_sxq == '"' && *(p_sxq+1) == '(' ? (char_u *)")\""
		    : p_sxq);
		retval = mch_call_shell(ncmd, opt);
		vim_free(ncmd);
	    }
	    иначе
		retval = -1;
	    если (ecmd != cmd)
		vim_free(ecmd);
	}
#ifdef FEAT_GUI
	--hold_gui_events;
#endif
	/*
	 * Check the window size, in case it changed while executing the
	 * external command.
	 */
	shell_resized_check();
    }

#ifdef FEAT_EVAL
    set_vim_var_nr(VV_SHELL_ERROR, (long)retval);
# ifdef FEAT_PROFILE
    если (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif

    вернуть retval;
}

/*
 * MODE_VISUAL, MODE_SELECT and MODE_OP_PENDING State are never set, they are
 * equal to MODE_NORMAL State with a condition.  This function returns the real
 * State.
 */
    целое
get_real_state(void)
{
    если (State & MODE_NORMAL)
    {
	если (VIsual_active)
	{
	    если (VIsual_select)
		вернуть MODE_SELECT;
	    вернуть MODE_VISUAL;
	}
	иначе если (finish_op)
	    вернуть MODE_OP_PENDING;
    }
    вернуть State;
}

/*
 * Return TRUE if "p" points to just after a path separator.
 * Takes care of multi-byte characters.
 * "b" must point to the start of the file name
 */
    целое
after_pathsep(char_u *b, char_u *p)
{
    вернуть p > b && vim_ispathsep(p[-1])
			     && (!has_mbyte || (*mb_head_off)(b, p - 1) == 0);
}

/*
 * Return TRUE if file names "f1" and "f2" are in the same directory.
 * "f1" may be a short name, "f2" must be a full path.
 */
    целое
same_directory(char_u *f1, char_u *f2)
{
    char_u	ffname[MAXPATHL];
    char_u	*t1;
    char_u	*t2;

    // safety check
    если (f1 == NULL || f2 == NULL)
	вернуть FALSE;

    (void)vim_FullName(f1, ffname, MAXPATHL, FALSE);
    t1 = gettail_sep(ffname);
    t2 = gettail_sep(f2);
    вернуть (t1 - ffname == t2 - f2
	     && pathcmp((char *)ffname, (char *)f2, (целое)(t1 - ffname)) == 0);
}

#если defined(FEAT_SESSION) || defined(FEAT_AUTOCHDIR) \
	|| defined(MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_NETBEANS_INTG)
/*
 * Change to a file's directory.
 * Caller must call shorten_fnames()!
 * Return OK or FAIL.
 */
    целое
vim_chdirfile(char_u *fname, char *trigger_autocmd)
{
    char_u	old_dir[MAXPATHL];
    char_u	new_dir[MAXPATHL];

    если (mch_dirname(old_dir, MAXPATHL) != OK)
	*old_dir = NUL;

    vim_strncpy(new_dir, fname, MAXPATHL - 1);
    *gettail_sep(new_dir) = NUL;

    если (pathcmp((char *)old_dir, (char *)new_dir, -1) == 0)
	// nothing to do
	вернуть OK;

    если (trigger_autocmd != NULL)
	trigger_DirChangedPre((char_u *)trigger_autocmd, new_dir);

    если (mch_chdir((char *)new_dir) != 0)
	вернуть FAIL;

    если (trigger_autocmd != NULL)
	apply_autocmds(EVENT_DIRCHANGED, (char_u *)trigger_autocmd,
						       new_dir, FALSE, curbuf);
    вернуть OK;
}
#endif

#если defined(STAT_IGNORES_SLASH)
/*
 * Check if "name" ends in a slash and is not a directory.
 * Used for systems where stat() ignores a trailing slash on a file name.
 * The Vim code assumes a trailing slash is only ignored for a directory.
 */
    static целое
illegal_slash(const char *name)
{
    если (name[0] == NUL)
	вернуть FALSE;	    // no file name is not illegal
    если (name[strlen(name) - 1] != '/')
	вернуть FALSE;	    // no trailing slash
    если (mch_isdir((char_u *)name))
	вернуть FALSE;	    // trailing slash for a directory
    вернуть TRUE;
}

/*
 * Special implementation of mch_stat() for Solaris.
 */
    целое
vim_stat(const char *name, stat_T *stp)
{
    // On Solaris stat() accepts "file/" as if it was "file".  Return -1 if
    // the name ends in "/" and it's not a directory.
    вернуть illegal_slash(name) ? -1 : stat(name, stp);
}
#endif

#если defined(CURSOR_SHAPE)

/*
 * Handling of cursor and mouse pointer shapes in various modes.
 */

cursorentry_T shape_table[SHAPE_IDX_COUNT] =
{
    // The values will be filled in from the 'guicursor' and 'mouseshape'
    // defaults when Vim starts.
    // Adjust the SHAPE_IDX_ defines when making changes!
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "n", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "v", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "i", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "r", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "c", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ci", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "cr", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "o", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0, 700L, 400L, 250L, 0, 0, "ve", SHAPE_CURSOR+SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "e", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "s", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "sd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vs", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "vd", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "m", SHAPE_MOUSE},
    {0,	0, 0,   0L,   0L,   0L, 0, 0, "ml", SHAPE_MOUSE},
    {0,	0, 0, 100L, 100L, 100L, 0, 0, "sm", SHAPE_CURSOR},
};

# ifdef FEAT_MOUSESHAPE
/*
 * Table with names for mouse shapes.  Keep in sync with all the tables for
 * mch_set_mouse_shape()!.
 */
static string_T mshape_names[] =
{
    STR_LITERAL_INIT("arrow"),	// default, must be the first one
    STR_LITERAL_INIT("blank"),	// hidden
    STR_LITERAL_INIT("beam"),
    STR_LITERAL_INIT("updown"),
    STR_LITERAL_INIT("udsizing"),
    STR_LITERAL_INIT("leftright"),
    STR_LITERAL_INIT("lrsizing"),
    STR_LITERAL_INIT("busy"),
    STR_LITERAL_INIT("no"),
    STR_LITERAL_INIT("crosshair"),
    STR_LITERAL_INIT("hand1"),
    STR_LITERAL_INIT("hand2"),
    STR_LITERAL_INIT("pencil"),
    STR_LITERAL_INIT("question"),
    STR_LITERAL_INIT("rightup-arrow"),
    STR_LITERAL_INIT("up-arrow"),
    {NULL, 0}
};

#  define MSHAPE_NAMES_COUNT  (ARRAY_LENGTH(mshape_names) - 1)
# endif

/*
 * Parse the 'guicursor' option ("what" is SHAPE_CURSOR) or 'mouseshape'
 * ("what" is SHAPE_MOUSE).
 * Returns error message for an illegal option, NULL otherwise.
 */
    char *
parse_shape_opt(целое what)
{
    char_u	*modep;
    char_u	*colonp;
    char_u	*commap;
    char_u	*slashp;
    char_u	*p, *endp;
    целое		idx = 0;		// init for GCC
    целое		all_idx;
    целое		len;
    целое		i;
    long	n;
    целое		found_ve = FALSE;	// found "ve" flag
    целое		round;

    /*
     * First round: check for errors; second round: do it for real.
     */
    для (round = 1; round <= 2; ++round)
    {
	/*
	 * Repeat for all comma separated parts.
	 */
# ifdef FEAT_MOUSESHAPE
	если (what == SHAPE_MOUSE)
	    modep = p_mouseshape;
	иначе
# endif
	    modep = p_guicursor;
	пока (*modep != NUL)
	{
	    colonp = vim_strchr(modep, ':');
	    commap = vim_strchr(modep, ',');

	    если (colonp == NULL || (commap != NULL && commap < colonp))
		вернуть e_missing_colon_2;
	    если (colonp == modep)
		вернуть e_illegal_mode;

	    /*
	     * Repeat for all mode's before the colon.
	     * For the 'a' mode, we loop to handle all the modes.
	     */
	    all_idx = -1;
	    пока (modep < colonp || all_idx >= 0)
	    {
		если (all_idx < 0)
		{
		    // Find the mode.
		    если (modep[1] == '-' || modep[1] == ':')
			len = 1;
		    иначе
			len = 2;
		    если (len == 1 && TOLOWER_ASC(modep[0]) == 'a')
			all_idx = SHAPE_IDX_COUNT - 1;
		    иначе
		    {
			для (idx = 0; idx < SHAPE_IDX_COUNT; ++idx)
			    если (STRNICMP(modep, shape_table[idx].name, len)
									 == 0)
				break;
			если (idx == SHAPE_IDX_COUNT
				   || (shape_table[idx].used_for & what) == 0)
			    вернуть e_illegal_mode;
			если (len == 2 && modep[0] == 'v' && modep[1] == 'e')
			    found_ve = TRUE;
		    }
		    modep += len + 1;
		}

		если (all_idx >= 0)
		    idx = all_idx--;
		иначе если (round == 2)
		{
# ifdef FEAT_MOUSESHAPE
		    если (what == SHAPE_MOUSE)
		    {
			// Set the default, for the missing parts
			shape_table[idx].mshape = 0;
		    }
		    иначе
# endif
		    {
			// Set the defaults, for the missing parts
			shape_table[idx].shape = SHAPE_BLOCK;
			shape_table[idx].blinkwait = 700L;
			shape_table[idx].blinkon = 400L;
			shape_table[idx].blinkoff = 250L;
		    }
		}

		// Parse the part after the colon
		для (p = colonp + 1; *p && *p != ','; )
		{
# ifdef FEAT_MOUSESHAPE
		    если (what == SHAPE_MOUSE)
		    {
			для (i = 0; ; ++i)
			{
			    если (mshape_names[i].string == NULL)
			    {
				если (!VIM_ISDIGIT(*p))
				    вернуть e_illegal_mouseshape;
				если (round == 2)
				    shape_table[idx].mshape =
					      getdigits(&p) + MSHAPE_NUMBERED;
				иначе
				    (void)getdigits(&p);
				break;
			    }
			    если (STRNICMP(p, mshape_names[i].string, mshape_names[i].length) == 0)
			    {
				если (round == 2)
				    shape_table[idx].mshape = i;
				p += mshape_names[i].length;
				break;
			    }
			}
		    }
		    иначе // if (what == SHAPE_MOUSE)
# endif
		    {
			/*
			 * First handle the ones with a number argument.
			 */
			i = *p;
			len = 0;
			если (STRNICMP(p, "ver", 3) == 0)
			    len = 3;
			иначе если (STRNICMP(p, "hor", 3) == 0)
			    len = 3;
			иначе если (STRNICMP(p, "blinkwait", 9) == 0)
			    len = 9;
			иначе если (STRNICMP(p, "blinkon", 7) == 0)
			    len = 7;
			иначе если (STRNICMP(p, "blinkoff", 8) == 0)
			    len = 8;
			если (len != 0)
			{
			    p += len;
			    если (!VIM_ISDIGIT(*p))
				вернуть e_digit_expected;
			    n = getdigits(&p);
			    если (len == 3)   // "ver" or "hor"
			    {
				если (n == 0)
				    вернуть e_illegal_percentage;
				если (round == 2)
				{
				    если (TOLOWER_ASC(i) == 'v')
					shape_table[idx].shape = SHAPE_VER;
				    иначе
					shape_table[idx].shape = SHAPE_HOR;
				    shape_table[idx].percentage = n;
				}
			    }
			    иначе если (round == 2)
			    {
				если (len == 9)
				    shape_table[idx].blinkwait = n;
				иначе если (len == 7)
				    shape_table[idx].blinkon = n;
				иначе
				    shape_table[idx].blinkoff = n;
			    }
			}
			иначе если (STRNICMP(p, "block", 5) == 0)
			{
			    если (round == 2)
				shape_table[idx].shape = SHAPE_BLOCK;
			    p += 5;
			}
			иначе	// must be a highlight group name then
			{
			    endp = vim_strchr(p, '-');
			    если (commap == NULL)		    // last part
			    {
				если (endp == NULL)
				    endp = p + STRLEN(p);   // find end of part
			    }
			    иначе если (endp > commap || endp == NULL)
				endp = commap;
			    slashp = vim_strchr(p, '/');
			    если (slashp != NULL && slashp < endp)
			    {
				// "group/langmap_group"
				i = syn_check_group(p, (целое)(slashp - p));
				p = slashp + 1;
			    }
			    если (round == 2)
			    {
				shape_table[idx].id = syn_check_group(p,
							     (целое)(endp - p));
				shape_table[idx].id_lm = shape_table[idx].id;
				если (slashp != NULL && slashp < endp)
				    shape_table[idx].id = i;
			    }
			    p = endp;
			}
		    } // if (what != SHAPE_MOUSE)

		    если (*p == '-')
			++p;
		}
	    }
	    modep = p;
	    если (*modep == ',')
		++modep;
	}
    }

    // If the 's' flag is not given, use the 'v' cursor for 's'
    если (!found_ve)
    {
# ifdef FEAT_MOUSESHAPE
	если (what == SHAPE_MOUSE)
	{
	    shape_table[SHAPE_IDX_VE].mshape = shape_table[SHAPE_IDX_V].mshape;
	}
	иначе
# endif
	{
	    shape_table[SHAPE_IDX_VE].shape = shape_table[SHAPE_IDX_V].shape;
	    shape_table[SHAPE_IDX_VE].percentage =
					 shape_table[SHAPE_IDX_V].percentage;
	    shape_table[SHAPE_IDX_VE].blinkwait =
					  shape_table[SHAPE_IDX_V].blinkwait;
	    shape_table[SHAPE_IDX_VE].blinkon =
					    shape_table[SHAPE_IDX_V].blinkon;
	    shape_table[SHAPE_IDX_VE].blinkoff =
					   shape_table[SHAPE_IDX_V].blinkoff;
	    shape_table[SHAPE_IDX_VE].id = shape_table[SHAPE_IDX_V].id;
	    shape_table[SHAPE_IDX_VE].id_lm = shape_table[SHAPE_IDX_V].id_lm;
	}
    }

    вернуть NULL;
}

# если defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) \
	|| defined(FEAT_MOUSESHAPE)
/*
 * Return the index into shape_table[] for the current mode.
 * When "mouse" is TRUE, consider indexes valid for the mouse pointer.
 */
    целое
get_shape_idx(целое mouse)
{
#  ifdef FEAT_MOUSESHAPE
    если (mouse && (State == MODE_HITRETURN || State == MODE_ASKMORE))
    {
#   ifdef FEAT_GUI
	целое x, y;
	gui_mch_getmouse(&x, &y);
	если (Y_2_ROW(y) == Rows - 1)
	    вернуть SHAPE_IDX_MOREL;
#   endif
	вернуть SHAPE_IDX_MORE;
    }
    если (mouse && drag_status_line)
	вернуть SHAPE_IDX_SDRAG;
    если (mouse && drag_sep_line)
	вернуть SHAPE_IDX_VDRAG;
#  endif
    если (!mouse && State == MODE_SHOWMATCH)
	вернуть SHAPE_IDX_SM;
    если (State & VREPLACE_FLAG)
	вернуть SHAPE_IDX_R;
    если (State & REPLACE_FLAG)
	вернуть SHAPE_IDX_R;
    если (State & MODE_INSERT)
	вернуть SHAPE_IDX_I;
    если (State & MODE_CMDLINE)
    {
	если (cmdline_at_end())
	    вернуть SHAPE_IDX_C;
	если (cmdline_overstrike())
	    вернуть SHAPE_IDX_CR;
	вернуть SHAPE_IDX_CI;
    }
    если (finish_op)
	вернуть SHAPE_IDX_O;
    если (VIsual_active)
    {
	если (*p_sel == 'e')
	    вернуть SHAPE_IDX_VE;
	иначе
	    вернуть SHAPE_IDX_V;
    }
    вернуть SHAPE_IDX_N;
}
# endif

# если defined(FEAT_MOUSESHAPE)
static целое current_mouse_shape = 0;

/*
 * Set the mouse shape:
 * If "shape" is -1, use shape depending on the current mode,
 * depending on the current state.
 * If "shape" is -2, only update the shape when it's CLINE or STATUS (used
 * when the mouse moves off the status or command line).
 */
    void
update_mouseshape(целое shape_idx)
{
    целое new_mouse_shape;

    // Only works in GUI mode.
    если (!gui.in_use || gui.starting)
	вернуть;

    // Postpone the updating when more is to come.  Speeds up executing of
    // mappings.
    если (shape_idx == -1 && char_avail())
    {
	postponed_mouseshape = TRUE;
	вернуть;
    }

    // When ignoring the mouse don't change shape on the statusline.
    если (*p_mouse == NUL
	    && (shape_idx == SHAPE_IDX_CLINE
		|| shape_idx == SHAPE_IDX_STATUS
		|| shape_idx == SHAPE_IDX_VSEP))
	shape_idx = -2;

    если (shape_idx == -2
	    && current_mouse_shape != shape_table[SHAPE_IDX_CLINE].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_STATUS].mshape
	    && current_mouse_shape != shape_table[SHAPE_IDX_VSEP].mshape)
	вернуть;
    если (shape_idx < 0)
	new_mouse_shape = shape_table[get_shape_idx(TRUE)].mshape;
    иначе
	new_mouse_shape = shape_table[shape_idx].mshape;
    если (new_mouse_shape != current_mouse_shape)
    {
	mch_set_mouse_shape(new_mouse_shape);
	current_mouse_shape = new_mouse_shape;
    }
    postponed_mouseshape = FALSE;
}
# endif

#endif // CURSOR_SHAPE

#если defined(FEAT_EVAL)
/*
 * Mainly for tests: get the name of the current mouse shape.
 */
    void
f_getmouseshape(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
# если defined(FEAT_MOUSESHAPE)
    если (current_mouse_shape >= 0
			      && current_mouse_shape < (целое)MSHAPE_NAMES_COUNT)
	rettv->vval.v_string = vim_strnsave(
				  mshape_names[current_mouse_shape].string,
				  mshape_names[current_mouse_shape].length);
# endif
}
#endif



/*
 * Change directory to "new_dir".  Search 'cdpath' for relative directory
 * names.
 */
    целое
vim_chdir(char_u *new_dir)
{
    char_u	*dir_name;
    целое		r;
    char_u	*file_to_find = NULL;
    char	*search_ctx = NULL;

    dir_name = find_directory_in_path(new_dir, (целое)STRLEN(new_dir),
		     FNAME_MESS, curbuf->b_ffname, &file_to_find, &search_ctx);
    vim_free(file_to_find);
    vim_findfile_cleanup(search_ctx);
    если (dir_name == NULL)
	вернуть -1;
    r = mch_chdir((char *)dir_name);
    vim_free(dir_name);
    вернуть r;
}

/*
 * Get user name from machine-specific function.
 * Returns the user name in "buf[len]".
 * Some systems are quite slow in obtaining the user name (Windows NT), thus
 * cache the result.
 * Returns OK or FAIL.
 */
    целое
get_user_name(char_u *buf, целое len)
{
    если (username == NULL)
    {
	если (mch_get_user_name(buf, len) == FAIL)
	    вернуть FAIL;
	username = vim_strsave(buf);
    }
    иначе
	vim_strncpy(buf, username, len - 1);
    вернуть OK;
}

#если defined(EXITFREE)
/*
 * Free the memory allocated by get_user_name()
 */
    void
free_username(void)
{
    vim_free(username);
}
#endif

#если !defined(HAVE_QSORT) && !defined(PROTO)
/*
 * Our own qsort(), for systems that don't have it.
 * It's simple and slow.  From the K&R C book.
 */
    void
qsort(
    void	*base,
    size_t	elm_count,
    size_t	elm_size,
    целое (*cmp)(const void *, const void *))
{
    char_u	*buf;
    char_u	*p1;
    char_u	*p2;
    целое		i, j;
    целое		gap;

    buf = alloc(elm_size);
    если (buf == NULL)
	вернуть;

    для (gap = elm_count / 2; gap > 0; gap /= 2)
	для (i = gap; i < elm_count; ++i)
	    для (j = i - gap; j >= 0; j -= gap)
	    {
		// Compare the elements.
		p1 = (char_u *)base + j * elm_size;
		p2 = (char_u *)base + (j + gap) * elm_size;
		если ((*cmp)((void *)p1, (void *)p2) <= 0)
		    break;
		// Exchange the elements.
		mch_memmove(buf, p1, elm_size);
		mch_memmove(p1, p2, elm_size);
		mch_memmove(p2, buf, elm_size);
	    }

    vim_free(buf);
}
#endif

/*
 * The putenv() implementation below comes from the "screen" program.
 * Included with permission from Juergen Weigert.
 * See pty.c for the copyright notice.
 */

/*
 *  putenv  --	put value into environment
 *
 *  Usage:  i = putenv (string)
 *    int i;
 *    char  *string;
 *
 *  where string is of the form <name>=<value>.
 *  Putenv returns 0 normally, -1 on error (not enough core for malloc).
 *
 *  Putenv may need to add a new name into the environment, or to
 *  associate a value longer than the current value with a particular
 *  name.  So, to make life simpler, putenv() copies your entire
 *  environment into the heap (i.e. malloc()) from the stack
 *  (i.e. where it resides when your process is initiated) the first
 *  time you call it.
 *
 *  (history removed, not very interesting.  See the "screen" sources.)
 */

#если !defined(HAVE_SETENV) && !defined(HAVE_PUTENV) && !defined(PROTO)

# define EXTRASIZE 5		// increment to add to env. size

static целое  envsize = -1;	// current size of environment
extern char **environ;		// the global which is your env.

static целое  findenv(char *name); // look for a name in the env.
static целое  newenv(void);	// copy env. from stack to heap
static целое  moreenv(void);	// incr. size of env.

    целое
putenv(const char *string)
{
    целое	    i;
    char    *p;

    если (envsize < 0)
    {				// first time putenv called
	если (newenv() < 0)	// copy env. to heap
	    вернуть -1;
    }

    i = findenv((char *)string); // look for name in environment

    если (i < 0)
    {				// name must be added
	для (i = 0; environ[i]; i++);
	если (i >= (envsize - 1))
	{			// need new slot
	    если (moreenv() < 0)
		вернуть -1;
	}
	p = alloc(strlen(string) + 1);
	если (p == NULL)		// not enough core
	    вернуть -1;
	environ[i + 1] = 0;	// new end of env.
    }
    иначе
    {				// name already in env.
	p = vim_realloc(environ[i], strlen(string) + 1);
	если (p == NULL)
	    вернуть -1;
    }
    sprintf(p, "%s", string);	// copy into env.
    environ[i] = p;

    вернуть 0;
}

    static целое
findenv(char *name)
{
    char    *namechar, *envchar;
    целое	    i, found;

    found = 0;
    для (i = 0; environ[i] && !found; i++)
    {
	envchar = environ[i];
	namechar = name;
	пока (*namechar && *namechar != '=' && (*namechar == *envchar))
	{
	    namechar++;
	    envchar++;
	}
	found = ((*namechar == '\0' || *namechar == '=') && *envchar == '=');
    }
    вернуть found ? i - 1 : -1;
}

    static целое
newenv(void)
{
    char    **env, *elem;
    целое	    i, esize;

    для (i = 0; environ[i]; i++)
	;

    esize = i + EXTRASIZE + 1;
    env = ALLOC_MULT(char *, esize);
    если (env == NULL)
	вернуть -1;

    для (i = 0; environ[i]; i++)
    {
	elem = alloc(strlen(environ[i]) + 1);
	если (elem == NULL)
	    вернуть -1;
	env[i] = elem;
	strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;
    вернуть 0;
}

    static целое
moreenv(void)
{
    целое	    esize;
    char    **env;

    esize = envsize + EXTRASIZE;
    env = vim_realloc((char *)environ, esize * sizeof (*env));
    если (env == 0)
	вернуть -1;
    environ = env;
    envsize = esize;
    вернуть 0;
}

# ifdef USE_VIMPTY_GETENV
/*
 * Used for mch_getenv() for Mac.
 */
    char_u *
vimpty_getenv(const char_u *string)
{
    целое i;
    char_u *p;

    если (envsize < 0)
	вернуть NULL;

    i = findenv((char *)string);

    если (i < 0)
	вернуть NULL;

    p = vim_strchr((char_u *)environ[i], '=');
    вернуть (p + 1);
}
# endif

#endif // !defined(HAVE_SETENV) && !defined(HAVE_PUTENV)

#если defined(FEAT_EVAL) || defined(FEAT_SPELL)
/*
 * Return 0 for not writable, 1 for writable file, 2 for a dir which we have
 * rights to write into.
 */
    целое
filewritable(char_u *fname)
{
    целое		retval = 0;
# если defined(UNIX) || defined(VMS)
    целое		perm = 0;
# endif

# если defined(UNIX) || defined(VMS)
    perm = mch_getperm(fname);
# endif
    если (
# ifdef MSWIN
	    mch_writable(fname) &&
# иначе
#  если defined(UNIX) || defined(VMS)
	    (perm & 0222) &&
#  endif
# endif
	    mch_access((char *)fname, W_OK) == 0
       )
    {
	++retval;
	если (mch_isdir(fname))
	    ++retval;
    }
    вернуть retval;
}
#endif

#если defined(FEAT_SPELL) || defined(FEAT_PERSISTENT_UNDO)
/*
 * Read 2 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    целое
get2c(FILE *fd)
{
    целое		c, n;

    n = getc(fd);
    если (n == EOF) вернуть -1;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    вернуть (n << 8) + c;
}

/*
 * Read 3 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    целое
get3c(FILE *fd)
{
    целое		c, n;

    n = getc(fd);
    если (n == EOF) вернуть -1;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    n = (n << 8) + c;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    вернуть (n << 8) + c;
}

/*
 * Read 4 bytes from "fd" and turn them into an int, MSB first.
 * Returns -1 when encountering EOF.
 */
    целое
get4c(FILE *fd)
{
    целое		c;
    // Use unsigned rather than int otherwise result is undefined
    // when left-shift sets the MSB.
    unsigned	n;

    c = getc(fd);
    если (c == EOF) вернуть -1;
    n = (unsigned)c;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    n = (n << 8) + (unsigned)c;
    c = getc(fd);
    если (c == EOF) вернуть -1;
    n = (n << 8) + (unsigned)c;
    вернуть (целое)n;
}

/*
 * Read a string of length "cnt" from "fd" into allocated memory.
 * Returns NULL when out of memory or unable to read that many bytes.
 */
    char_u *
read_string(FILE *fd, целое cnt)
{
    char_u	*str;
    целое		i;
    целое		c;

    // allocate memory
    str = alloc(cnt + 1);
    если (str == NULL)
	вернуть NULL;

    // Read the string.  Quit when running into the EOF.
    для (i = 0; i < cnt; ++i)
    {
	c = getc(fd);
	если (c == EOF)
	{
	    vim_free(str);
	    вернуть NULL;
	}
	str[i] = c;
    }
    str[i] = NUL;
    вернуть str;
}

/*
 * Write a number to file "fd", MSB first, in "len" bytes.
 */
    целое
put_bytes(FILE *fd, long_u nr, целое len)
{
    целое	    i;

    для (i = len - 1; i >= 0; --i)
	если (putc((целое)(nr >> (i * 8)), fd) == EOF)
	    вернуть FAIL;
    вернуть OK;
}

#endif

#ifndef PROTO  // proto is defined in vim.h
# ifdef ELAPSED_TIMEVAL
/*
 * Return time in msec since "start_tv".
 */
    long
elapsed(struct timeval *start_tv)
{
    struct timeval  now_tv;

    gettimeofday(&now_tv, NULL);
    вернуть (now_tv.tv_sec - start_tv->tv_sec) * 1000L
	 + (now_tv.tv_usec - start_tv->tv_usec) / 1000L;
}
# endif

# ifdef ELAPSED_TICKCOUNT
/*
 * Return time in msec since "start_tick".
 */
    long
elapsed(DWORD start_tick)
{
    DWORD	now = GetTickCount();

    вернуть (long)now - (long)start_tick;
}
# endif
#endif

#если defined(FEAT_JOB_CHANNEL) \
	|| (defined(UNIX) && (!defined(USE_SYSTEM) \
	|| (defined(FEAT_GUI) && defined(FEAT_TERMINAL))))
/*
 * Parse "cmd" and put the white-separated parts in "argv".
 * "argv" is an allocated array with "argc" entries and room for 4 more.
 * Returns FAIL when out of memory.
 */
    целое
mch_parse_cmd(char_u *cmd, целое use_shcf, char ***argv, целое *argc)
{
    целое		i;
    char_u	*p, *d;
    целое		inquote;

    /*
     * Do this loop twice:
     * 1: find number of arguments
     * 2: separate them and build argv[]
     */
    для (i = 1; i <= 2; ++i)
    {
	p = skipwhite(cmd);
	inquote = FALSE;
	*argc = 0;
	пока (*p != NUL)
	{
	    если (i == 2)
		(*argv)[*argc] = (char *)p;
	    ++*argc;
	    d = p;
	    пока (*p != NUL && (inquote || (*p != ' ' && *p != TAB)))
	    {
		если (p[0] == '"')
		    // quotes surrounding an argument and are dropped
		    inquote = !inquote;
		иначе
		{
		    если (rem_backslash(p))
		    {
			// First pass: skip over "\ " and "\"".
			// Second pass: Remove the backslash.
			++p;
		    }
		    если (i == 2)
			*d++ = *p;
		}
		++p;
	    }
	    если (*p == NUL)
	    {
		если (i == 2)
		    *d++ = NUL;
		break;
	    }
	    если (i == 2)
		*d++ = NUL;
	    p = skipwhite(p + 1);
	}
	если (*argv == NULL)
	{
	    если (use_shcf)
	    {
		// Account for possible multiple args in p_shcf.
		p = p_shcf;
		для (;;)
		{
		    p = skiptowhite(p);
		    если (*p == NUL)
			break;
		    ++*argc;
		    p = skipwhite(p);
		}
	    }

	    *argv = ALLOC_MULT(char *, *argc + 4);
	    если (*argv == NULL)	    // out of memory
		вернуть FAIL;
	}
    }
    вернуть OK;
}

/*
 * Build "argv[argc]" from the string "cmd".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    целое
build_argv_from_string(char_u *cmd, char ***argv, целое *argc)
{
    char_u	*cmd_copy;
    целое		i;

    // Make a copy, parsing will modify "cmd".
    cmd_copy = vim_strsave(cmd);
    если (cmd_copy == NULL
	    || mch_parse_cmd(cmd_copy, FALSE, argv, argc) == FAIL)
    {
	vim_free(cmd_copy);
	вернуть FAIL;
    }
    для (i = 0; i < *argc; i++)
	(*argv)[i] = (char *)vim_strsave((char_u *)(*argv)[i]);
    (*argv)[*argc] = NULL;
    vim_free(cmd_copy);
    вернуть OK;
}

#endif

#если defined(FEAT_EVAL)
/*
 * Build "argv[argc]" from the list "l".
 * "argv[argc]" is set to NULL;
 * Return FAIL when out of memory.
 */
    целое
build_argv_from_list(list_T *l, char ***argv, целое *argc)
{
    listitem_T  *li;
    char_u	*s;

    // Pass argv[] to mch_call_shell().
    *argv = ALLOC_MULT(char *, l->lv_len + 1);
    если (*argv == NULL)
	вернуть FAIL;
    *argc = 0;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	s = tv_get_string_chk(&li->li_tv);
	если (s == NULL)
	{
	    целое i;

	    для (i = 0; i < *argc; ++i)
		VIM_CLEAR((*argv)[i]);
	    (*argv)[0] = NULL;
	    вернуть FAIL;
	}
	(*argv)[*argc] = (char *)vim_strsave(s);
	*argc += 1;
    }
    (*argv)[*argc] = NULL;
    вернуть OK;
}
#endif

/*
 * Change the behavior of vterm.
 * 0: As usual.
 * 1: Windows 10 version 1809
 *      The bug causes unstable handling of ambiguous width character.
 * 2: Windows 10 version 1903 & 1909
 *      Use the wrong result because each result is different.
 * 3: Windows 10 insider preview (current latest logic)
 */
    целое
get_special_pty_type(void)
{
#ifdef MSWIN
    вернуть get_conpty_type();
#иначе
    вернуть 0;
#endif
}

// compare two keyvalue_T structs by case sensitive value
    целое
cmp_keyvalue_value(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    вернуть STRCMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by value with length
    целое
cmp_keyvalue_value_n(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    вернуть STRNCMP(kv1->value.string, kv2->value.string, MAX(kv1->value.length,
		kv2->value.length));
}

// compare two keyvalue_T structs by case insensitive value
    целое
cmp_keyvalue_value_i(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    вернуть STRICMP(kv1->value.string, kv2->value.string);
}

// compare two keyvalue_T structs by case insensitive ASCII value
// with value.length
    целое
cmp_keyvalue_value_ni(const void *a, const void *b)
{
    keyvalue_T *kv1 = (keyvalue_T *)a;
    keyvalue_T *kv2 = (keyvalue_T *)b;

    вернуть vim_strnicmp_asc((char *)kv1->value.string,
	    (char *)kv2->value.string, MAX(kv1->value.length,
		    kv2->value.length));
}

/*
 * Iterative merge sort for doubly linked list.
 * O(NlogN) worst case, and stable.
 *  - The list is divided into blocks of increasing size (1, 2, 4, 8, ...).
 *  - Each pair of blocks is merged in sorted order.
 *  - Merged blocks are reconnected to build the sorted list.
 */
    void *
mergesort_list(
    void	*head,
    void	*(*get_next)(void *),
    void	(*set_next)(void *, void *),
    void	*(*get_prev)(void *),
    void	(*set_prev)(void *, void *),
    целое		(*compare)(const void *, const void *))
{
    если (!head || !get_next(head))
	вернуть head;

    // Count length
    целое	    n = 0;
    void*   curr = head;
    пока (curr)
    {
	n++;
	curr = get_next(curr);
    }

    целое	size;
    для (size = 1; size < n; size *= 2)
    {
	void*	new_head = NULL;
	void*	tail = NULL;
	curr = head;

	пока (curr)
	{
	    // Split two runs
	    void    *left = curr;
	    void    *right = left;
	    целое	    i;
	    для (i = 0; i < size && right; ++i)
		right = get_next(right);

	    void    *next = right;
	    для (i = 0; i < size && next; ++i)
		next = get_next(next);

	    // Break links
	    void    *l_end = right ? get_prev(right) : NULL;
	    если (l_end)
		set_next(l_end, NULL);
	    если (right)
		set_prev(right, NULL);

	    void    *r_end = next ? get_prev(next) : NULL;
	    если (r_end)
		set_next(r_end, NULL);
	    если (next)
		set_prev(next, NULL);

	    // Merge
	    void    *merged = NULL;
	    void    *merged_tail = NULL;

	    пока (left || right)
	    {
		void	*chosen = NULL;
		если (!left)
		{
		    chosen = right;
		    right = get_next(right);
		}
		иначе если (!right)
		{
		    chosen = left;
		    left = get_next(left);
		}
		иначе если (compare(left, right) <= 0)
		{
		    chosen = left;
		    left = get_next(left);
		}
		иначе
		{
		    chosen = right;
		    right = get_next(right);
		}

		если (merged_tail)
		{
		    set_next(merged_tail, chosen);
		    set_prev(chosen, merged_tail);
		    merged_tail = chosen;
		}
		иначе
		{
		    merged = merged_tail = chosen;
		    set_prev(chosen, NULL);
		}
	    }

	    // Connect to full list
	    если (!new_head)
		new_head = merged;
	    иначе
	    {
		set_next(tail, merged);
		set_prev(merged, tail);
	    }

	    // Move tail to end
	    пока (get_next(merged_tail))
		merged_tail = get_next(merged_tail);
	    tail = merged_tail;

	    curr = next;
	}

	head = new_head;
    }

    вернуть head;
}

static const char_u base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char_u base64_dec_table[256];

#define BASE64_ENCODED_LEN(len) ((((len) + 2) / 3) * 4 + 1)

/*
 * Initialize the base64 decoding table
 */
    static void
init_base64_dec_table(void)
{
    static целое base64_dec_tbl_initialized = FALSE;

    если (base64_dec_tbl_initialized)
	вернуть;

    vim_memset(base64_dec_table, 0xFF, sizeof(base64_dec_table));
    для (size_t i = 0; i < sizeof(base64_table) - 1; i++)
	base64_dec_table[(char_u)base64_table[i]] = (char_u)i;
    base64_dec_table['='] = 0;

    base64_dec_tbl_initialized = TRUE;
}

/*
 * Base64 encode "src[len]" into caller-supplied "dst", which must have room for
 * at least BASE64_ENCODED_LEN(len) bytes (NUL terminated). Returns the number
 * of bytes written, excluding the NUL.
 */
    long
base64_encode_buf(char_u *dst, const char_u *src, size_t len)
{
    size_t i, j;

    для (i = 0, j = 0; i < len;)
    {
	size_t remaining = len - i;

	int_u octet_a = src[i++];
	int_u octet_b = remaining > 1 ? src[i++] : 0;
	int_u octet_c = remaining > 2 ? src[i++] : 0;

	int_u triple = (octet_a << 16) | (octet_b << 8) | octet_c;

	dst[j++] = base64_table[(triple >> 18) & 0x3F];
	dst[j++] = base64_table[(triple >> 12) & 0x3F];
	dst[j++] = remaining > 1
	    ? base64_table[(triple >> 6) & 0x3F]
	    : '=';
	dst[j++] = remaining > 2
	    ? base64_table[triple & 0x3F]
	    : '=';
    }

    dst[j] = NUL;
    вернуть (long)j;
}

/*
 * Base64-encode "data[len]".  Returns an allocated NUL-terminated string, or
 * NULL on OOM.  Caller frees with vim_free().
 */
    char_u *
base64_encode(const char_u *data, size_t len)
{
    char_u *encoded = alloc(BASE64_ENCODED_LEN(len));

    если (encoded == NULL)
	вернуть NULL;
    base64_encode_buf(encoded, data, len);
    вернуть encoded;
}

/*
 * Decode base64 text "data[len]" (len must be a multiple of 4) into growarray
 * "out" (appended byte-by-byte, e.g. a blob's bv_ga). Returns OK on success and
 * FAIL on failure.
 */
    целое
base64_decode(const char_u *data, size_t len, garray_T *out)
{
    если (len == 0)
	вернуть OK;

    если (len % 4 != 0)
    {
	semsg(_(e_invalid_argument_str), data);
	вернуть FAIL;
    }

    init_base64_dec_table();

    size_t decoded_len = (len / 4) * 3;
    если (data[len - 1] == '=')
	decoded_len--;
    если (data[len - 2] == '=')
	decoded_len--;

    size_t i, j;
    для (i = 0, j = 0; i < len;)
    {
	int_u sextet_a = base64_dec_table[(char_u)data[i++]];
	int_u sextet_b = base64_dec_table[(char_u)data[i++]];
	int_u sextet_c = base64_dec_table[(char_u)data[i++]];
	int_u sextet_d = base64_dec_table[(char_u)data[i++]];

	если (sextet_a == 0xFF || sextet_b == 0xFF || sextet_c == 0xFF
							|| sextet_d == 0xFF)
	{
	    semsg(_(e_invalid_argument_str), data);
	    ga_clear(out);
	    вернуть FAIL;
	}

	int_u triple = (sextet_a << 18) | (sextet_b << 12)
						| (sextet_c << 6) | sextet_d;

	если (j < decoded_len)
	{
	    ga_append(out, (triple >> 16) & 0xFF);
	    j++;
	}
	если (j < decoded_len)
	{
	    ga_append(out, (triple >> 8) & 0xFF);
	    j++;
	}
	если (j < decoded_len)
	{
	    ga_append(out, triple & 0xFF);
	    j++;
	}

	если (j == decoded_len)
	{
	    если ((data[len - 2] == '=' && ((sextet_b & 0xF) != 0))
		|| ((data[len - 1] == '=') && ((sextet_c & 0x3) != 0)))
	    {
		semsg(_(e_invalid_argument_str), data);
		ga_clear(out);
		вернуть FAIL;
	    }
	}
    }
    вернуть OK;
}
