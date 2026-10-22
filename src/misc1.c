/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * misc1.c: functions that didn't seem to fit elsewhere
 */

#شامل "vim.h"
#شامل "version.h"

#اگر defined(__HAIKU__)
# include <storage/FindDirectory.h>
#اگر_ختم

#اگر defined(MSWIN)
# include <lm.h>
#اگر_ختم

#تعریف_کرو URL_SLASH	1		// path_is_url() has found "://"
#تعریف_کرو URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ساکن garray_T	ga_users;

/*
 * get_leader_len() returns the length in bytes of the prefix of the given
 * string which introduces a comment.  If this string is not a comment then
 * 0 is returned.
 * When "flags" is not NULL, it is set to point to the flags of the recognized
 * comment leader.
 * "backward" must be true for the "O" command.
 * If "include_space" is set, include trailing whitespace while calculating the
 * length.
 */
    صحیح
get_leader_len(
    char_u	*سطر,
    char_u	**flags,
    صحیح		backward,
    صحیح		include_space)
{
    صحیح		i, j;
    صحیح		نتیجہ;
    صحیح		got_com = FALSE;
    صحیح		ملا_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    صحیح		درمیان_مطابقت_len = 0;
    char_u	*پچھلا_list;
    char_u	*saved_flags = تہی;

    نتیجہ = i = 0;
    جب (VIM_ISWHITE(سطر[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    جب (سطر[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ملا_one = FALSE;
	برائے (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    اگر (!got_com && flags != تہی)
		*flags = list;	    // remember where flags started
	    پچھلا_list = list;
	    (خالی)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    اگر (string == تہی)	    // missing ':', ignore this part
		جاری;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    اگر (درمیان_مطابقت_len != 0
		    && vim_strchr(part_buf, COM_درمیان) == تہی
		    && vim_strchr(part_buf, COM_انتہا) == تہی)
		توڑو;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    اگر (got_com && vim_strchr(part_buf, COM_NEST) == تہی)
		جاری;

	    // When 'O' flag present and using "O" command skip this one.
	    اگر (backward && vim_strchr(part_buf, COM_NOBACK) != تہی)
		جاری;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    اگر (VIM_ISWHITE(string[0]))
	    {
		اگر (i == 0 || !VIM_ISWHITE(سطر[i - 1]))
		    جاری;  // missing white space
		جب (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    برائے (j = 0; string[j] != NUL && string[j] == سطر[i + j]; ++j)
		;
	    اگر (string[j] != NUL)
		جاری;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    اگر (vim_strchr(part_buf, COM_BLANK) != تہی
			   && !VIM_ISWHITE(سطر[i + j]) && سطر[i + j] != NUL)
		جاری;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    اگر (vim_strchr(part_buf, COM_درمیان) != تہی)
	    {
		اگر (درمیان_مطابقت_len == 0)
		{
		    درمیان_مطابقت_len = j;
		    saved_flags = پچھلا_list;
		}
		جاری;
	    }
	    اگر (درمیان_مطابقت_len != 0 && j > درمیان_مطابقت_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		درمیان_مطابقت_len = 0;

	    اگر (درمیان_مطابقت_len == 0)
		i += j;
	    ملا_one = TRUE;
	    توڑو;
	}

	اگر (درمیان_مطابقت_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    اگر (!got_com && flags != تہی)
		*flags = saved_flags;
	    i += درمیان_مطابقت_len;
	    ملا_one = TRUE;
	}

	// No match found, stop scanning.
	اگر (!ملا_one)
	    توڑو;

	نتیجہ = i;

	// Include any trailing white space.
	جب (VIM_ISWHITE(سطر[i]))
	    ++i;

	اگر (include_space)
	    نتیجہ = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	اگر (vim_strchr(part_buf, COM_NEST) == تہی)
	    توڑو;
    }
    واپس نتیجہ;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    صحیح
get_آخری_leader_انحراف(char_u *سطر, char_u **flags)
{
    صحیح		نتیجہ = -1;
    صحیح		i, j;
    صحیح		زیریں_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    صحیح		ملا_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (صحیح)STRLEN(سطر);
    جب (--i >= زیریں_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ملا_one = FALSE;
	برائے (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (خالی)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    اگر (string == تہی)	// If everything is fine, this cannot actually
				// happen.
		جاری;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    اگر (VIM_ISWHITE(string[0]))
	    {
		اگر (i == 0 || !VIM_ISWHITE(سطر[i - 1]))
		    جاری;
		جب (VIM_ISWHITE(*string))
		    ++string;
	    }
	    برائے (j = 0; string[j] != NUL && string[j] == سطر[i + j]; ++j)
		/* do nothing */;
	    اگر (string[j] != NUL)
		جاری;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    اگر (vim_strchr(part_buf, COM_BLANK) != تہی
		    && !VIM_ISWHITE(سطر[i + j]) && سطر[i + j] != NUL)
		جاری;

	    اگر (vim_strchr(part_buf, COM_درمیان) != تہی)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		برائے (j = 0; VIM_ISWHITE(سطر[j]) && j <= i; j++)
		    ;
		اگر (j < i)
		    جاری;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    ملا_one = TRUE;

	    اگر (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    توڑو;
	}

	اگر (ملا_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    صحیح     len1, len2, off;

	    نتیجہ = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    اگر (vim_strchr(part_buf, COM_NEST) != تہی)
		جاری;

	    زیریں_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    جب (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (صحیح)STRLEN(com_leader);

	    برائے (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(خالی)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		اگر (flags_save == com_flags)
		    جاری;
		string = vim_strchr(part_buf2, ':');
		++string;
		جب (VIM_ISWHITE(*string))
		    ++string;
		len2 = (صحیح)STRLEN(string);
		اگر (len2 == 0)
		    جاری;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		برائے (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    اگر (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			اگر (i - off < زیریں_check_bound)
			    زیریں_check_bound = i - off;
		    }
		}
	    }
	}
    }
    واپس نتیجہ;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    صحیح
plines(linenr_T lnum)
{
    واپس plines_win(curwin, lnum, TRUE);
}

    صحیح
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		حد_بندی_winheight)	// when TRUE limit to window height
{
#اگر defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    واپس plines_win_nofill(wp, lnum, حد_بندی_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    صحیح
plines_nofill(linenr_T lnum)
{
    واپس plines_win_nofill(curwin, lnum, TRUE);
}

    صحیح
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		حد_بندی_winheight)	// when TRUE limit to window height
{
#اگر_ختم
    صحیح		سطریں;

    اگر (wp->w_چوڑائی == 0)
	واپس 1;

#اگر_تعریف_شدہ FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    اگر (سطر_Folded(wp, lnum) == TRUE)
	واپس 1;
#اگر_ختم

    اگر (!wp->w_p_wrap)
	سطریں = 1
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_گنتی_above_نیچے(wp->w_بفر, lnum)
#اگر_ختم
	;
    ورنہ
	سطریں = plines_win_nofold(wp, lnum);

    اگر (حد_بندی_winheight && سطریں > wp->w_اونچائی)
	واپس wp->w_اونچائی;
    واپس سطریں;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    صحیح
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    طویل	ستون;
    صحیح		چوڑائی;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_بفر, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    اگر (*s == NUL
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_متن
#اگر_ختم
	    )
	واپس 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ستون = (صحیح)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    اگر (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	ستون += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    چوڑائی = wp->w_چوڑائی - win_ستون_off(wp);
    اگر (چوڑائی <= 0)
	واپس 32000;
    اگر (ستون <= چوڑائی)
	واپس 1;
    ستون -= چوڑائی;
    چوڑائی += win_ستون_off2(wp);
    واپس (ستون + (چوڑائی - 1)) / چوڑائی + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    صحیح
plines_win_ستون(win_T *wp, linenr_T lnum, طویل عمود)
{
    طویل	ستون;
    صحیح		سطریں = 0;
    صحیح		چوڑائی;
    char_u	*سطر;
    chartabsize_T cts;

#اگر_تعریف_شدہ FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    سطریں = diff_check_fill(wp, lnum);
#اگر_ختم

    اگر (!wp->w_p_wrap)
	واپس سطریں + 1;

    اگر (wp->w_چوڑائی == 0)
	واپس سطریں + 1;

    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    // "column" is a byte index, advance the pointer until it is reached.
    جب (*cts.cts_ptr != NUL && cts.cts_ptr < سطر + عمود)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, تہی, تہی);
	MB_PTR_ADV(cts.cts_ptr);
    }

    /*
     * If *cts.cts_ptr is a TAB, and the TAB is not displayed as ^I, and we're
     * not in MODE_INSERT state, then col must be adjusted so that it
     * represents the last screen position of the TAB.  This only fixes an
     * error when the TAB wraps from one screen line to the next (when
     * 'columns' is not a multiple of 'ts') -- webb.
     */
    ستون = cts.cts_vcol;
    اگر (*cts.cts_ptr == TAB && (State & منوال_معمول)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	ستون += win_lbr_chartabsize(&cts, تہی, تہی) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    چوڑائی = wp->w_چوڑائی - win_ستون_off(wp);
    اگر (چوڑائی <= 0)
	واپس 9999;

    سطریں += 1;
    اگر (ستون > چوڑائی)
	سطریں += (ستون - چوڑائی) / (چوڑائی + win_ستون_off2(wp)) + 1;
    واپس سطریں;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    صحیح
plines_m_win(win_T *wp, linenr_T پہلا, linenr_T آخری, صحیح max)
{
    صحیح		گنتی = 0;

    جب (پہلا <= آخری && گنتی < max)
    {
#اگر_تعریف_شدہ FEAT_FOLDING
	صحیح	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_گنتی(wp, پہلا, تہی);
	اگر (x > 0)
	{
	    ++گنتی;	    // count 1 for "+-- folded" line
	    پہلا += x;
	}
	ورنہ
#اگر_ختم
	{
#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (پہلا == wp->w_topline)
		گنتی += plines_win_nofill(wp, پہلا, FALSE) + wp->w_topfill;
	    ورنہ
#اگر_ختم
		گنتی += plines_win(wp, پہلا, FALSE);
	    ++پہلا;
	}
    }
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (پہلا == wp->w_بفر->b_ml.ml_سطر_گنتی + 1)
	گنتی += diff_check_fill(wp, پہلا);
#اگر_ختم
    واپس MIN(max, گنتی);
}

    صحیح
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    صحیح		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    اگر (pos->ستون == MAXCOL)
	واپس NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    اگر (pos->ستون > ptrlen)
	واپس NUL;
    اگر (has_mbyte)
	واپس (*mb_ptr2char)(ptr);
    واپس (صحیح)*ptr;
}

    صحیح
gchar_cursor(خالی)
{
    اگر (has_mbyte)
	واپس (*mb_ptr2char)(ml_get_cursor());
    واپس (صحیح)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    صحیح
char_before_cursor(خالی)
{
    اگر (curwin->w_cursor.ستون == 0)
	واپس -1;

    char_u *سطر = ml_get_curline();

    اگر (has_mbyte)
    {
	char_u *p = سطر + curwin->w_cursor.ستون;
	صحیح پچھلا_len = (*mb_سر_off)(سطر, p - 1) + 1;
	واپس mb_ptr2char(p - پچھلا_len);
    }
    واپس سطر[curwin->w_cursor.ستون - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    خالی
pchar_cursor(صحیح c)
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE)
						  + curwin->w_cursor.ستون) = c;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_مقصد_option_part(char_u *p)
{
    اگر (*p == ',')
	++p;
    جب (*p == ' ')
	++p;
    واپس p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    خالی
check_حالت(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر == buf && wp->w_حالت_اونچائی)
	{
	    wp->w_redr_حالت = true;
	    سیٹ_must_redraw(UPD_VALID);
	}
}

/*
 * Ask for a reply from the user, a 'y' or a 'n', with prompt "str" (which
 * should have been translated already).
 * No other characters are accepted, the message is repeated until a valid
 * reply is entered or CTRL-C is hit.
 * If direct is TRUE, don't use vgetc() but ui_inchar(), don't get characters
 * from any buffers but directly from the user.
 *
 * return the 'y' or 'n'
 */
    صحیح
ask_yesno(char_u *str, صحیح براہ_راست)
{
    صحیح	    r = ' ';
    صحیح	    save_State = State;

    اگر (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم
    State = منوال_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_کلیدیں;		// no mapping here, but recognize keys

    جب (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	اگر (براہ_راست)
	    r = get_keystroke();
	ورنہ
	    r = plain_vgetc();
	اگر (r == Ctrl_C || r == ESC)
	    r = 'n';
	پیغام_putchar(r);	    // show what you typed
	برآمد_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_کلیدیں;

    واپس r;
}

#اگر defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    خالی
get_منوال(char_u *buf)
{
    صحیح		i = 0;

    اگر (وقت_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ورنہ اگر (رکن_use_loop())
    {
	اگر (State & منوال_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ورنہ اگر (State == منوال_HITRETURN || State == منوال_ASKMORE
						      || State == منوال_SETWSIZE
		|| State == منوال_CONFIRM)
    {
	buf[i++] = 'r';
	اگر (State == منوال_ASKMORE)
	    buf[i++] = 'm';
	ورنہ اگر (State == منوال_CONFIRM)
	    buf[i++] = '?';
    }
    ورنہ اگر (State == منوال_EXTERNCMD)
	buf[i++] = '!';
    ورنہ اگر (State & منوال_داخل_کرو)
    {
	اگر (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ورنہ
	{
	    اگر (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ورنہ
		buf[i++] = 'i';
	}

	اگر (ins_compl_فعال())
	    buf[i++] = 'c';
	ورنہ اگر (ctrl_x_منوال_not_defined_yet())
	    buf[i++] = 'x';
    }
    ورنہ اگر ((State & منوال_CMDLINE) || exmode_فعال)
    {
	buf[i++] = 'c';
	اگر (exmode_فعال == EXMODE_VIM)
	    buf[i++] = 'v';
	ورنہ اگر (exmode_فعال == EXMODE_معمول)
	    buf[i++] = 'e';
	اگر ((State & منوال_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ورنہ اگر (VIsual_فعال)
    {
	اگر (VIsual_select)
	    buf[i++] = VIsual_منوال + 's' - 'v';
	ورنہ
	{
	    buf[i++] = VIsual_منوال;
	    اگر (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ورنہ
    {
	buf[i++] = 'n';
	اگر (اختتام_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ورنہ اگر (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ورنہ اگر (رکن_اندر_معمول_منوال(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    خالی
f_منوال(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[منوال_MAX_لمبائی];

    اگر (اندر_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	واپس;

    get_منوال(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    اگر (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ساکن خالی
may_جوڑو_state_char(garray_T *gap, char_u *include, صحیح c)
{
    اگر (include == تہی || vim_strchr(include, c) != تہی)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    خالی
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = تہی;
    صحیح		i;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    ga_init2(&ga, 1, 20);
    اگر (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    اگر (!(stuff_خالی_خانے() && typebuf.tb_len == 0 && scriptin[curscript] == تہی))
	may_جوڑو_state_char(&ga, include, 'm');
    اگر (op_pending())
	may_جوڑو_state_char(&ga, include, 'o');
    اگر (autocmd_busy)
	may_جوڑو_state_char(&ga, include, 'x');
    اگر (ins_compl_فعال())
	may_جوڑو_state_char(&ga, include, 'a');

# ifdef FEAT_کام_CHANNEL
    اگر (channel_اندر_blocking_wait())
	may_جوڑو_state_char(&ga, include, 'w');
# endif
    اگر (!get_was_محفوظ_state())
	may_جوڑو_state_char(&ga, include, 'S');
    برائے (i = 0; i < get_callback_گہرائی() && i < 3; ++i)
	may_جوڑو_state_char(&ga, include, 'c');
    اگر (پیغام_scrolled > 0)
	may_جوڑو_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ڈیٹا;
}

#اگر_ختم // FEAT_EVAL

/*
 * Get a key stroke directly from the user.
 * Ignores mouse clicks and scrollbar events, except a click for the left
 * button (used at the more prompt).
 * Doesn't use vgetc(), because it syncs undo and eats mapped characters.
 * Disadvantage: typeahead is ignored.
 * Translates the interrupt character for unix to ESC.
 */
    صحیح
get_keystroke(خالی)
{
    char_u	*buf = تہی;
    صحیح		buflen = 150;
    صحیح		maxlen;
    صحیح		len = 0;
    صحیح		n;
    صحیح		save_mapped_ctrl_c = mapped_ctrl_c;
    صحیح		waited = 0;

    mod_نقاب = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    برائے (;;)
    {
	cursor_on();
	برآمد_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	اگر (buf == تہی)
	    buf = alloc(buflen);
	ورنہ اگر (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    اگر (buf == تہی)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	اگر (buf == تہی)
	{
	    do_outofmem_پیغام((long_u)buflen);
	    واپس ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	اگر (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_داخلہ_بفر(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ورنہ اگر (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	اگر ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    جاری;

	اگر (n == KEYLEN_REMOVED)  // key code removed
	{
	    اگر (must_redraw != 0 && !need_wait_return && (State
			& (منوال_CMDLINE | منوال_HITRETURN | منوال_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    جاری;
	}
	اگر (n > 0)		// found a termcode: adjust length
	    len = n;
	اگر (len == 0)		// nothing typed yet
	    جاری;

	// Handle modifier and/or special key code.
	n = buf[0];
	اگر (n == K_SPECIAL)
	{
	    n = مقصد_SPECIAL(buf[1], buf[2]);
	    اگر (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_کلید(n) && n != K_LEFTMOUSE)
#اگر_تعریف_شدہ FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#اگر_ختم
	       )
	    {
		اگر (buf[1] == KS_MODIFIER)
		    mod_نقاب = buf[2];
		len -= 3;
		اگر (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		جاری;
	    }
	    توڑو;
	}
	اگر (has_mbyte)
	{
	    اگر (MB_BYTE2LEN(n) > len)
		جاری;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#اگر_تعریف_شدہ UNIX
	اگر (n == intr_char)
	    n = ESC;
#اگر_ختم
	توڑو;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    واپس ملاؤ_modify_Other_کلیدیں(n, &mod_نقاب);
}

// For overflow detection, add a digit safely to an int value.
    ساکن صحیح
vim_append_digit_int(صحیح *قدر, صحیح digit)
{
    صحیح x = *قدر;
    اگر (x > ((زیادہ_سے_زیادہ_عدد - digit) / 10))
	واپس FAIL;
    *قدر = x * 10 + digit;
    واپس OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    صحیح
get_نمبر(
    صحیح	    colon,			// allow colon to abort
    صحیح	    *mouse_مستعمل)
{
    صحیح	n = 0;
    صحیح	c;
    صحیح typed = 0;

    اگر (mouse_مستعمل != تہی)
	*mouse_مستعمل = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    اگر (پیغام_silent != 0)
	واپس 0;

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم
    ++no_mapping;
    ++allow_کلیدیں;		// no mapping here, but recognize keys
    برائے (;;)
    {
	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
	c = محفوظ_vgetc();
	اگر (VIM_ISDIGIT(c))
	{
	    اگر (vim_append_digit_int(&n, c - '0') == FAIL)
		واپس 0;
	    پیغام_putchar(c);
	    ++typed;
	}
	ورنہ اگر (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    اگر (typed > 0)
	    {
		پیغام_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ورنہ اگر (mouse_مستعمل != تہی && c == K_LEFTMOUSE)
	{
	    *mouse_مستعمل = TRUE;
	    n = mouse_صف + 1;
	    توڑو;
	}
	ورنہ اگر (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    اگر (!exmode_فعال)
		cmdline_صف = پیغام_صف;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    توڑو;
	}
	ورنہ اگر (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    توڑو;
	}
	ورنہ اگر (c == CAR || c == NL )
	    توڑو;
    }
    --no_mapping;
    --allow_کلیدیں;
    واپس n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    صحیح
prompt_for_نمبر(صحیح *mouse_مستعمل)
{
    صحیح		i;
    صحیح		save_cmdline_صف;
    صحیح		save_State;

    // When using ":silent" assume that <CR> was entered.
    اگر (mouse_مستعمل != تہی)
	پیغام_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ورنہ
	پیغام_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_صف = cmdline_صف;
    cmdline_صف = 0;
    save_State = State;
    State = منوال_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_نمبر(TRUE, mouse_مستعمل);
    اگر (کلید_Typed)
    {
	// don't call wait_return() now
	اگر (پیغام_صف > 0)
	    cmdline_صف = پیغام_صف - 1;
	need_wait_return = FALSE;
	پیغام_didany = FALSE;
	پیغام_didout = FALSE;
    }
    ورنہ
	cmdline_صف = save_cmdline_صف;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    واپس i;
}

    خالی
msgmore(طویل n)
{
    طویل pn;

    اگر (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	واپس;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    اگر (keep_پیغام != تہی && !keep_پیغام_مزید)
	واپس;

    اگر (n > 0)
	pn = n;
    ورنہ
	pn = -n;

    اگر (pn > p_report)
    {
	اگر (n > 0)
	    vim_snprintf(پیغام_buf, پیغام_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ورنہ
	    vim_snprintf(پیغام_buf, پیغام_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	اگر (got_int)
	    vim_strcat((char_u *)پیغام_buf, (char_u *)_(" (Interrupted)"),
								  پیغام_BUF_LEN);
	اگر (پیغام(پیغام_buf))
	{
	    سیٹ_keep_پیغام((char_u *)پیغام_buf, 0);
	    keep_پیغام_مزید = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    خالی
beep_flush(خالی)
{
    اگر (emsg_silent == 0)
    {
	flush_buffers(FLUSH_اقل);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    خالی
vim_beep(غیرعلامتی val)
{
#اگر_تعریف_شدہ FEAT_EVAL
    called_vim_beep = TRUE;
#اگر_ختم

    اگر (emsg_silent != 0 || اندر_assert_fails)
	واپس;

    اگر (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#اگر_تعریف_شدہ ELAPSED_FUNC
	ساکن صحیح		did_init = FALSE;
	ساکن elapsed_T	آغاز_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	اگر (!did_init || ELAPSED_FUNC(آغاز_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(آغاز_tv);
#اگر_ختم
	    اگر (p_vb
#اگر_تعریف_شدہ FEAT_GUI
		    // While the GUI is starting up the termcap is set for
		    // the GUI but the output still goes to a terminal.
		    && !(gui.اندر_use && gui.starting)
#اگر_ختم
	       )
	    {
		برآمد_str_cf(T_VB);
#اگر_تعریف_شدہ FEAT_VTP
		// No restore color information, refresh the screen.
		اگر (has_vtp_working() != 0
# ifdef FEAT_TERMGUICOLORS
			&& (p_tgc || (!p_tgc && t_colors >= 256))
# endif
		   )
		{
		    redraw_later(UPD_CLEAR);
		    update_screen(0);
		    redrawcmd();
		}
#اگر_ختم
	    }
	    ورنہ
		برآمد_char(BELL);
#اگر_تعریف_شدہ ELAPSED_FUNC
	}
#اگر_ختم
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    اگر (vim_strchr(p_debug, 'e') != تہی)
    {
	پیغام_ماخذ(HL_ATTR(HLF_W));
	پیغام_attr(_("Beep!"), HL_ATTR(HLF_W));
    }
}

/*
 * To get the "real" home directory:
 * - get value of $HOME
 * For Unix:
 *  - go to that directory
 *  - do mch_dirname() to get the real name of that directory.
 *  This also works with mounts and links.
 *  Don't do this for MS-DOS, it will change the "current dir" for a drive.
 * For Windows:
 *  This code is duplicated in init_homedir() in dosinst.c.  Keep in sync!
 */
    خالی
init_homedir(خالی)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#اگر_تعریف_شدہ VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ورنہ
    var = mch_getenv((char_u *)"HOME");
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    اگر (var == تہی || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	اگر (homepath == تہی || *homepath == NUL)
	    homepath = (char_u *)"\\";
	اگر (homedrive != تہی
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((حرف *)NameBuff, "%s%s", homedrive, homepath);
	    اگر (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    اگر (var == تہی)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    اگر (var != تہی && *var == '%')
    {
	char_u	*p;
	char_u	*قوت_نما;

	p = vim_strchr(var + 1, '%');
	اگر (p != تہی)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    قوت_نما = mch_getenv(NameBuff);
	    اگر (قوت_نما != تہی && *قوت_نما != NUL
					&& STRLEN(قوت_نما) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s%s", قوت_نما, p + 1);
		var = NameBuff;
	    }
	}
    }

    اگر (var != تہی && *var == NUL)	// empty is same as not set
	var = تہی;

    اگر (enc_utf8 && var != تہی)
    {
	صحیح	len;
	char_u  *pp = تہی;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_مقصد_enc(var, (صحیح)STRLEN(var), &pp, &len);
	اگر (pp != تہی)
	{
	    homedir = pp;
	    واپس;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    اگر (var == تہی)
	var = (char_u *)"C:/";
#اگر_ختم

    اگر (var != تہی)
    {
#اگر_تعریف_شدہ UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	اگر (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((حرف *)NameBuff) == 0)
	{
	    اگر (!mch_chdir((حرف *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    اگر (mch_chdir((حرف *)NameBuff) != 0)
		emsg(_(e_cannot_go_عقب_مقصد_پچھلا_directory));
	}
#اگر_ختم
	homedir = vim_strsave(var);
    }
}

#اگر defined(EXITFREE)
    خالی
free_homedir(خالی)
{
    vim_free(homedir);
}

    خالی
free_users(خالی)
{
    ga_clear_لڑیاں(&ga_users);
}
#اگر_ختم

#اگر defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    خالی
init_vimdir(خالی)
{
    صحیح	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    اگر (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    اگر (mustfree)
	vim_free(p);
}
#اگر_ختم

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(char_u *src)
{
    واپس expand_env_save_opt(src, FALSE, تہی);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, صحیح one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    اگر (p != تہی)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, تہی);
    واپس p;
}

/*
 * Expand environment variable with path name.
 * "~/" is also expanded, using $HOME.	For Unix "~user/" is expanded.
 * Skips over "\ ", "\~" and "\$" (not for Win32 though).
 * If anything fails no expansion is done and dst equals src.
 */
    size_t
expand_env(
    char_u	*src,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    صحیح		dstlen)		// maximum length of the result
{
    واپس expand_env_esc(src, dst, dstlen, تہی, FALSE, تہی);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    صحیح		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    صحیح		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*دم;
    صحیح		c;
    char_u	*var;
    صحیح		copy_char;
    صحیح		mustfree;	// var was allocated, need to free it later
    صحیح		at_آغاز = TRUE; // at start of a name
    صحیح		startstr_len = 0;
    char_u	*dst_آغاز = dst;

    اگر (startstr != تہی)
	startstr_len = (صحیح)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    جب (*src && dstlen > 0)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	// Skip over `=expr`.
	اگر (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (خالی)skip_اظہاریہ(&src, تہی);
	    اگر (*src == '`')
		++src;
	    len = src - var;
	    اگر (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (صحیح)len;
	    جاری;
	}
#اگر_ختم
	copy_char = TRUE;
	اگر ((*src == '$'
#اگر_تعریف_شدہ VMS
		    && at_آغاز
#اگر_ختم
	   )
#اگر defined(MSWIN)
		|| *src == '%'
#اگر_ختم
		|| (*src == '~' && at_آغاز))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    اگر (*src != '~')				// environment var
	    {
		دم = src + 1;
		var = dst;
		c = dstlen - 1;

#اگر_تعریف_شدہ UNIX
		// Unix has ${var-name} type environment vars
		اگر (*دم == '{' && !vim_isIDc('{'))
		{
		    دم++;	// ignore '{'
		    جب (c-- > 0 && *دم && *دم != '}')
			*var++ = *دم++;
		}
		ورنہ
#اگر_ختم
		{
		    جب (c-- > 0 && *دم != NUL && ((vim_isIDc(*دم))
#اگر defined(MSWIN)
			    || (*src == '%' && *دم != '%')
#اگر_ختم
			    ))
			*var++ = *دم++;
		}

#اگر defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		اگر (src[1] == '{' && *دم != '}')
# ورنہ
		اگر (*src == '%' && *دم != '%')
# endif
		    var = تہی;
		ورنہ
		{
# ifdef UNIX
		    اگر (src[1] == '{')
# ورنہ
		    اگر (*src == '%')
# endif
			++دم;
#اگر_ختم
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#اگر defined(MSWIN) || defined(UNIX)
		}
#اگر_ختم
	    }
							// home directory
	    ورنہ اگر (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != تہی)
	    {
		var = homedir;
		دم = src + 1;
	    }
	    ورنہ					// user directory
	    {
#اگر defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		دم = src;
		var = dst;
		c = dstlen - 1;
		جب (	   c-- > 0
			&& *دم
			&& vim_isfilec(*دم)
			&& !vim_ispathsep(*دم))
		    *var++ = *دم++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  اگر defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ساخت passwd *pw = (*dst == NUL)
					? تہی : getpwnam((حرف *)dst + 1);

		    var = (pw == تہی) ? تہی : (char_u *)pw->pw_dir;
		}
		اگر (var == تہی)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, تہی,
				WILD_جوڑو_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ورنہ	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*راستہ, *اگلا_راستہ, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    اگلا_راستہ = paths;
		    جب (*اگلا_راستہ)
		    {
			برائے (راستہ = اگلا_راستہ; *اگلا_راستہ && *اگلا_راستہ != ',';
				اگلا_راستہ++);
			اگر (*اگلا_راستہ)
			    *اگلا_راستہ++ = NUL;
			testlen = vim_snprintf_safelen(
			    (حرف *)test,
			    حجم(test),
			    "%s/%s",
			    راستہ,
			    dst + 1);
			اگر (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    اگر (var != تہی)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    توڑو;
			}
		    }
		}
# endif // UNIX
#ورنہ
		// cannot expand user's home directory, so don't try
		var = تہی;
		دم = (char_u *)"";	// for gcc
#اگر_ختم // UNIX || VMS
	    }

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    اگر (p_ssl && var != تہی && vim_strchr(var, '\\') != تہی)
	    {
		char_u	*p = vim_strsave(var);

		اگر (p != تہی)
		{
		    اگر (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		    forward_slash(var);
		}
	    }
#اگر_ختم

	    // If "var" contains any character from "esc_chars", escape it
	    // with a backslash.  The historical use is escaping spaces so
	    // that ":e ~/tt" works when $HOME contains a space.
	    اگر (esc_chars != تہی && var != تہی
		    && vim_strpbrk(var, esc_chars) != تہی)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		اگر (p != تہی)
		{
		    اگر (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    اگر (var != تہی && *var != NUL)
	    {
		c = (صحیح)STRLEN(var);

		اگر (c + STRLEN(دم) + 1 < (غیرعلامتی)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    اگر (after_pathsep(dst, dst + c)
#اگر defined(BACKSLASH_اندر_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#اگر_ختم
			    && vim_ispathsep(*دم))
			++دم;
		    dst += c;
		    src = دم;
		    copy_char = FALSE;
		}
	    }
	    اگر (mustfree)
		vim_free(var);
	}

	اگر (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_آغاز = FALSE;
	    اگر (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ورنہ اگر ((src[0] == ' ' || src[0] == ',') && !one)
		at_آغاز = TRUE;
	    اگر (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		اگر (startstr != تہی && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_آغاز = TRUE;
	    }
	}

    }
    *dst = NUL;

    واپس (size_t)(dst - dst_آغاز);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ساکن char_u *
remove_دم(char_u *p, char_u *pend, char_u *name)
{
    صحیح		len = (صحیح)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    اگر (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	واپس newend;
    واپس pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ساکن char_u *
vim_نسخہ_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    اگر (vimdir == تہی || *vimdir == NUL)
	واپس تہی;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_نسخہ_NODOT, STRLEN_LITERAL(VIM_نسخہ_NODOT), TRUE, &p);
    اگر (p.string != تہی && mch_isdir(p.string))
	واپس p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    اگر (p.string != تہی && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.لمبائی,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	اگر (fname.string != تہی)
	{
	    صحیح exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    اگر (exists)
		واپس p.string;
	}
    }
    vim_free(p.string);
    واپس تہی;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, صحیح *mustfree)
{
    char_u	*p = تہی;
    char_u	*pend;
    صحیح		vimruntime;
#اگر_تعریف_شدہ MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    اگر (STRCMP(name, "HOME") == 0)
	واپس homedir;

    // Use Wide function
    wn = enc_مقصد_utf16(name, تہی);
    اگر (wn == تہی)
	واپس تہی;

    wp = _wgetenv(wn);
    vim_free(wn);

    اگر (wp != تہی && *wp == NUL)   // empty is the same as not set
	wp = تہی;

    اگر (wp != تہی)
    {
	p = utf16_مقصد_enc(wp, تہی);
	اگر (p == تہی)
	    واپس تہی;

	*mustfree = TRUE;
	واپس p;
    }
#ورنہ
    p = mch_getenv(name);
    اگر (p != تہی && *p == NUL)	    // empty is the same as not set
	p = تہی;

    اگر (p != تہی)
	واپس p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    اگر (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ساکن حرف user_Settings_راستہ[MAXPATHL];

	اگر (ڈھونڈو_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   user_Settings_راستہ, MAXPATHL) == B_OK)
	    واپس (char_u *)user_Settings_راستہ;
	ورنہ
	    واپس تہی;
    }
# endif
#اگر_ختم

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    اگر (!vimruntime && STRCMP(name, "VIM") != 0)
	واپس تہی;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    اگر (vimruntime
#اگر_تعریف_شدہ HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#اگر_ختم
       )
    {
#اگر_تعریف_شدہ MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	اگر (wp != تہی && *wp == NUL)	    // empty is the same as not set
	    wp = تہی;
	اگر (wp != تہی)
	{
	    char_u *q = utf16_مقصد_enc(wp, تہی);
	    اگر (q != تہی)
	    {
		p = vim_نسخہ_dir(q);
		*mustfree = TRUE;
		اگر (p == تہی)
		    p = q;
	    }
	}
#ورنہ
	p = mch_getenv((char_u *)"VIM");
	اگر (p != تہی && *p == NUL)	    // empty is the same as not set
	    p = تہی;
	اگر (p != تہی)
	{
	    p = vim_نسخہ_dir(p);
	    اگر (p != تہی)
		*mustfree = TRUE;
	    ورنہ
		p = mch_getenv((char_u *)"VIM");
	}
#اگر_ختم
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    اگر (p == تہی)
    {
	اگر (p_hf != تہی && vim_strchr(p_hf, '$') == تہی)
	    p = p_hf;
#اگر_تعریف_شدہ USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ورنہ
	    p = exe_name;
#اگر_ختم
	اگر (p != تہی)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    اگر (p == p_hf)
		pend = remove_دم(p, pend, (char_u *)"doc");

#اگر_تعریف_شدہ USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    اگر (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_دم(p, pend, (char_u *)"MacOS");
		اگر (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    اگر (pnew != تہی)
		    {
			STRNCPY(pnew, p, (pend1 - p));
			STRCPY(pnew + (pend1 - p), "Resources/vim");
			p = pnew;
			pend = p + STRLEN(p);
		    }
		}
	    }
# endif
	    // remove "src/" from exe_name, if present
	    اگر (p == exe_name)
		pend = remove_دم(p, pend, (char_u *)"src");
#اگر_ختم

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    اگر (!vimruntime)
	    {
		pend = remove_دم(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_دم(p, pend, (char_u *)VIM_نسخہ_NODOT);
	    }

	    // remove trailing path separator
	    اگر (pend > p && after_pathsep(p, pend))
		--pend;

#اگر_تعریف_شدہ MACOS_X
	    اگر (p == exe_name || p == p_hf)
#اگر_ختم
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    اگر (p != تہی && !mch_isdir(p))
		VIM_CLEAR(p);
	    ورنہ
	    {
#اگر_تعریف_شدہ USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		اگر (vimruntime && (pend = vim_نسخہ_dir(p)) != تہی)
		{
		    vim_free(p);
		    p = pend;
		}
#اگر_ختم
		*mustfree = TRUE;
	    }
	}
    }

#اگر_تعریف_شدہ HAVE_PATHDEF
    // When there is a pathdef.c file we can use default_vim_dir and
    // default_vimruntime_dir
    اگر (p == تہی)
    {
	// Only use default_vimruntime_dir when it is not empty
	اگر (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ورنہ اگر (*default_vim_dir != NUL)
	{
	    اگر (vimruntime && (p = vim_نسخہ_dir(default_vim_dir)) != تہی)
		*mustfree = TRUE;
	    ورنہ
	    {
		p = default_vim_dir;
		*mustfree = FALSE;
	    }
	}
    }
#اگر_ختم

    /*
     * Set the environment variable, so that the new value can be found fast
     * next time, and others can also use it (e.g. Perl).
     */
    اگر (p != تہی)
    {
	اگر (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ورنہ
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    واپس p;
}

    خالی
vim_unsetenv(char_u *var)
{
#اگر_تعریف_شدہ HAVE_UNSETENV
    unsetenv((حرف *)var);
#ورنہ
    vim_setenv(var, (char_u *)"");
#اگر_ختم
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    خالی
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    اگر (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ورنہ اگر (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#اگر defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    خالی
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    اگر (STRICMP(name, "HOME") == 0)
	init_homedir();
    ورنہ اگر (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ورنہ اگر (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#اگر_ختم

/*
 * Our portable version of setenv.
 */
    خالی
vim_setenv(char_u *name, char_u *val)
{
#اگر_تعریف_شدہ HAVE_SETENV
    mch_setenv((حرف *)name, (حرف *)val, 1);
#ورنہ
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    اگر (envbuf != تہی)
    {
	sprintf((حرف *)envbuf, "%s=%s", name, val);
	putenv((حرف *)envbuf);
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    اگر (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	اگر (buf != تہی)
	{
	    bindtextdomain(VIMPACKAGE, (حرف *)buf);
	    vim_free(buf);
	}
    }
#اگر_ختم
}

/*
 * Function given to ExpandGeneric() to obtain an environment variable name.
 */
    char_u *
get_env_name(
    expand_T	*xp غیر_مستعمل,
    صحیح		idx)
{
#اگر defined(AMIGA)
    // No environ[] on the Amiga.
    واپس تہی;
#ورنہ
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    خارجی حرف		**environ;
# endif
    char_u		*str;
    صحیح			n;

    str = (char_u *)environ[idx];
    اگر (str == تہی)
	واپس تہی;

    برائے (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	اگر (str[n] == '=' || str[n] == NUL)
	    توڑو;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    واپس xp->xp_buf;
#اگر_ختم
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ساکن خالی
جوڑو_user(char_u *user, صحیح need_copy)
{
    char_u	*user_copy = (user != تہی && need_copy)
						    ? vim_strsave(user) : user;

    اگر (user_copy == تہی || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	اگر (need_copy)
	    vim_free(user_copy);
	واپس;
    }
    ((char_u **)(ga_users.ga_ڈیٹا))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ساکن خالی
init_users(خالی)
{
    ساکن صحیح	lazy_init_مکمل = FALSE;

    اگر (lazy_init_مکمل)
	واپس;

    lazy_init_مکمل = TRUE;
    ga_init2(&ga_users, حجم(char_u *), 20);

#اگر defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ساخت passwd*	pw;

	setpwent();
	جب ((pw = getpwent()) != تہی)
	    جوڑو_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#وگرنہ_اگر defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	اگر (NetUserEnum(تہی, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_لمبائی,
				       &nusers, &ntotal, تہی) == NERR_Success)
	{
	    برائے (i = 0; i < nusers; i++)
		جوڑو_user(utf16_مقصد_enc(uinfo[i].usri0_name, تہی), FALSE);

	    Net_Api_بفر_Free(uinfo);
	}
    }
#اگر_ختم
#اگر defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	اگر (user_env != تہی && *user_env != NUL)
	{
	    صحیح	i;

	    برائے (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_ڈیٹا)[i];

		اگر (STRCMP(local_user, user_env) == 0)
		    توڑو;
	    }

	    اگر (i == ga_users.ga_len)
	    {
		ساخت passwd	*pw = getpwnam((حرف *)user_env);

		اگر (pw != تہی)
		    جوڑو_user((char_u *)pw->pw_name, TRUE);
	    }
	}
    }
#اگر_ختم
}

/*
 * Function given to ExpandGeneric() to obtain user names.
 */
    char_u*
get_users(expand_T *xp غیر_مستعمل, صحیح idx)
{
    init_users();
    اگر (idx < ga_users.ga_len)
	واپس ((char_u **)ga_users.ga_ڈیٹا)[idx];
    واپس تہی;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    صحیح
مطابقت_user(char_u *name)
{
    صحیح i;
    صحیح n = (صحیح)STRLEN(name);
    صحیح نتیجہ = 0;

    init_users();
    برائے (i = 0; i < ga_users.ga_len; i++)
    {
	اگر (STRCMP(((char_u **)ga_users.ga_ڈیٹا)[i], name) == 0)
	    واپس 2; // full match
	اگر (STRNCMP(((char_u **)ga_users.ga_ڈیٹا)[i], name, n) == 0)
	    نتیجہ = 1; // partial match
    }
    واپس نتیجہ;
}

    ساکن خالی
prepare_مقصد_exit(خالی)
{
#اگر defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	gui.dying = true;
	برآمد_trash();	// trash any pending output
    }
    ورنہ
#اگر_ختم
    {
	windgoto((صحیح)صفیں - 1, cmdline_ستون_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	اگر (!بھرا_screen)
	    mch_setmouse(FALSE);

	/*
	 * Switch terminal mode back now, so messages end up on the "normal"
	 * screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
	stoptermcap();
	برآمد_flush();
    }
}

/*
 * Preserve files and exit.
 * When called IObuff must contain a message.
 * NOTE: This may be called from deathtrap() in a signal handler, avoid unsafe
 * functions, such as allocating memory.
 */
    خالی
preserve_exit(خالی)
{
    buf_T	*buf;

    prepare_مقصد_exit();

    // Setting this will prevent free() calls.  That avoids calling free()
    // recursively when free() was invoked with a bad pointer.
    really_exiting = TRUE;

    برآمد_str(IObuff);
    screen_آغاز();		    // don't know where cursor is now
    برآمد_flush();

    ml_close_notmod();		    // close all not-modified buffers

    FOR_ALL_BUFFERS(buf)
    {
	اگر (buf->b_ml.ml_mfp != تہی && buf->b_ml.ml_mfp->mf_fname != تہی)
	{
	    برآمد_STR("Vim: preserving files...\r\n");
	    screen_آغاز();	    // don't know where cursor is now
	    برآمد_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    توڑو;
	}
    }

    ml_close_all(FALSE);	    // close all memfiles, without deleting

    برآمد_STR("Vim: Finished.\r\n");

    getout(1);
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#اگر_غیر_تعریف_شدہ BREAKCHECK_SKIP
# define BREAKCHECK_SKIP 1000
#اگر_ختم

ساکن صحیح	breakcheck_گنتی = 0;

    خالی
سطر_breakcheck(خالی)
{
    اگر (++breakcheck_گنتی >= BREAKCHECK_SKIP)
    {
	breakcheck_گنتی = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    خالی
fast_breakcheck(خالی)
{
    اگر (++breakcheck_گنتی >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_گنتی = 0;
	ui_breakcheck();
    }
}

#اگر defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    خالی
veryfast_breakcheck(خالی)
{
    اگر (++breakcheck_گنتی >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_گنتی = 0;
	ui_breakcheck();
    }
}
#اگر_ختم

#اگر defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

# ifndef SEEK_SET
#  define SEEK_SET 0
# endif
# ifndef SEEK_END
#  define SEEK_END 2
# endif

/*
 * Get the stdout of an external command.
 * If "ret_len" is NULL replace NUL characters with NL.  When "ret_len" is not
 * NULL store the length there.
 * Returns an allocated string, or NULL for error.
 */
    char_u *
get_cmd_output(
    char_u	*cmd,
    char_u	*infile,	// optional input file name
    صحیح		flags,		// can be SHELL_SILENT
    صحیح		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*بفر = تہی;
    صحیح		len;
    صحیح		i = 0;
    FILE	*fd;

    اگر (check_restricted() || check_secure())
	واپس تہی;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    اگر ((tempname = vim_tempname('o', TRUE)) == تہی)
    {
	emsg(_(e_cant_get_عارضی_file_name));
	واپس تہی;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    اگر (command == تہی)
	جاؤ مکمل;

    /*
     * Call the shell to execute the command (errors are ignored).
     * Don't check timestamps here.
     */
    ++no_check_timestamps;
    call_shell(command, SHELL_DOOUT | SHELL_EXPAND | flags);
    --no_check_timestamps;

    vim_free(command);

    /*
     * read the names from the file into memory
     */
# ifdef VMS
    // created temporary file is not always readable as binary
    fd = mch_fopen((حرف *)tempname, "r");
# ورنہ
    fd = mch_fopen((حرف *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    اگر (fd == تہی
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	اگر (fd != تہی)
	    fclose(fd);
	جاؤ مکمل;
    }

    بفر = alloc(len + 1);
    اگر (بفر != تہی)
	i = (صحیح)fread((حرف *)بفر, (size_t)1, (size_t)len, fd);
    fclose(fd);
    اگر (بفر == تہی)
	جاؤ مکمل;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    اگر (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(بفر);
    }
    ورنہ اگر (ret_len == تہی)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	برائے (i = 0; i < len; ++i)
	    اگر (بفر[i] == NUL)
		بفر[i] = 1;

	بفر[len] = NUL;	// make sure the buffer is terminated
    }
    ورنہ
	*ret_len = len;

مکمل:
    mch_remove(tempname);
    vim_free(tempname);
    واپس بفر;
}

# اگر defined(FEAT_EVAL)

    ساکن خالی
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		retlist)
{
    char_u	*res = تہی;
    char_u	*p;
    char_u	*infile = تہی;
    صحیح		err = FALSE;
    FILE	*fd;
    list_T	*list = تہی;
    صحیح		flags = SHELL_SILENT;
    صحیح		use_argv = FALSE;
    حرف	**argv = تہی;
    صحیح		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    اگر (check_restricted() || check_secure())
	جاؤ errret;

    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_نمبر_or_list_arg(argvars, 1)
								      == FAIL))
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	اگر ((infile = vim_tempname('i', TRUE)) == تہی)
	{
	    emsg(_(e_cant_get_عارضی_file_name));
	    جاؤ errret;
	}

	fd = mch_fopen((حرف *)infile, WRITEBIN);
	اگر (fd == تہی)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    جاؤ errret;
	}
	اگر (argvars[1].v_type == VAR_نمبر)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_نمبر);
	    اگر (buf == تہی)
	    {
		semsg(_(e_بفر_nr_does_not_exist), argvars[1].vval.v_نمبر);
		fclose(fd);
		جاؤ errret;
	    }

	    برائے (lnum = 1; lnum <= buf->b_ml.ml_سطر_گنتی; lnum++)
	    {
		برائے (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    اگر (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			توڑو;
		    }
		اگر (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    توڑو;
		}
	    }
	}
	ورنہ اگر (argvars[1].v_type == VAR_LIST)
	{
	    اگر (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ورنہ
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    اگر (p == تہی)
	    {
		fclose(fd);
		جاؤ errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    اگر (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	اگر (fclose(fd) != 0)
	    err = TRUE;
	اگر (err)
	{
	    emsg(_(e_error_writing_عارضی_file));
	    جاؤ errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    اگر (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	اگر (l == تہی || l->lv_len < 1)
	{
	    emsg(_(e_invalid_دلیل));
	    جاؤ errret;
	}
	اگر (build_argv_from_list(l, &argv, &argc) == FAIL)
	    جاؤ errret;
	اگر (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_دلیل));
	    جاؤ errret;
	}
	use_argv = TRUE;

	اگر (p_verbose > 3)
	{
	    صحیح		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    برائے (i = 0; i < argc; ++i)
	    {
		اگر (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (حرف *)ga.ga_ڈیٹا);
	    پیغام_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    اگر (!پیغام_silent)
	flags += SHELL_COOKED;

    اگر (retlist)
    {
	صحیح		len;
	listitem_T	*li;
	char_u		*s = تہی;
	char_u		*آغاز;
	char_u		*انتہا;
	صحیح		i;

	اگر (use_argv)
	    res = mch_get_cmd_output_براہ_راست(argv, infile, flags, &len);
	ورنہ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	اگر (res == تہی)
	    جاؤ errret;

	list = list_alloc();
	اگر (list == تہی)
	    جاؤ errret;

	برائے (i = 0; i < len; ++i)
	{
	    آغاز = res + i;
	    جب (i < len && res[i] != NL)
		++i;
	    انتہا = res + i;

	    s = alloc(انتہا - آغاز + 1);
	    اگر (s == تہی)
		جاؤ errret;

	    برائے (p = s; آغاز < انتہا; ++p, ++آغاز)
		*p = *آغاز == NUL ? NL : *آغاز;
	    *p = NUL;

	    li = listitem_alloc();
	    اگر (li == تہی)
	    {
		vim_free(s);
		جاؤ errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_تالا = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_سیٹ(rettv, list);
	list = تہی;
    }
    ورنہ
    {
	اگر (use_argv)
	    res = mch_get_cmd_output_براہ_راست(argv, infile, flags, تہی);
	ورنہ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, تہی);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	اگر (res != تہی)
	{
	    char_u	*s, *d;

	    d = res;
	    برائے (s = res; *s; ++s)
	    {
		اگر (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = تہی;
    }

errret:
    اگر (argv != تہی)
    {
	صحیح i;
	برائے (i = 0; argv[i] != تہی; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    اگر (infile != تہی)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    اگر (res != تہی)
	vim_free(res);
    اگر (list != تہی)
	list_free(list);
}

/*
 * "system()" function
 */
    خالی
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    خالی
f_systemlist(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, TRUE);
}
# endif // FEAT_EVAL

#اگر_ختم

/*
 * Return TRUE when need to go to Insert mode because of 'insertmode'.
 * Don't do this when still processing a command or a mapping.
 * Don't do this when inside a ":normal" command.
 */
    صحیح
goto_im(خالی)
{
    واپس (p_im && stuff_خالی_خانے() && typebuf_typed());
}

/*
 * Returns the isolated name of the shell in allocated memory:
 * - Skip beyond any path.  E.g., "/usr/bin/csh -f" -> "csh -f".
 * - Remove any argument.  E.g., "csh -f" -> "csh".
 * But don't allow a space in the path, so that this works:
 *   "/usr/bin/csh --rcfile ~/.cshrc"
 * But don't do that for Windows, it's common to have a space in the path.
 * Returns NULL when out of memory.
 */
    char_u *
get_isolated_shell_name(خالی)
{
    char_u *p;

#اگر_تعریف_شدہ MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ورنہ
    p = skiptowhite(p_sh);
    اگر (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ورنہ
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	برائے (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    اگر (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#اگر_ختم
    واپس p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    صحیح
راستہ_is_url(char_u *p)
{
    اگر (STRNCMP(p, "://", (size_t)3) == 0)
	واپس URL_SLASH;
    ورنہ اگر (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	واپس URL_BACKSLASH;
    واپس 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    صحیح
راستہ_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    اگر (!ASCII_ISALPHA(*fname))
	واپس 0;

    // check body: alpha or dash
    برائے (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    اگر (p[-1] == '-')
	واپس 0;

    // "://" or ":\\" must follow
    واپس راستہ_is_url(p);
}

#اگر defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    اگر (v_event->dv_hashtab.ht_مستعمل > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ورنہ
	sve->sve_did_save = FALSE;
    واپس v_event;
}

    خالی
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    اگر (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ورنہ
	hash_init(&v_event->dv_hashtab);
}
#اگر_ختم

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    خالی
may_trigger_modechanged(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_منوال[منوال_MAX_لمبائی];
    char_u	    نمونہ_buf[2 * منوال_MAX_لمبائی];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    اگر (!has_modechanged() || got_int)
	واپس;

    get_منوال(curr_منوال);
    اگر (STRCMP(curr_منوال, آخری_منوال) == 0)
	واپس;

    v_event = get_v_event(&save_v_event);
    (خالی)dict_جوڑو_string(v_event, "new_mode", curr_منوال);
    (خالی)dict_جوڑو_string(v_event, "old_mode", آخری_منوال);
    dict_سیٹ_اشیاء_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((حرف *)نمونہ_buf, حجم(نمونہ_buf), "%s:%s", آخری_منوال,
	    curr_منوال);

    apply_autocmds(EVENT_MODECHANGED, نمونہ_buf, تہی, FALSE, curbuf);
    STRCPY(آخری_منوال, curr_منوال);

    restore_v_event(v_event, &save_v_event);
#اگر_ختم
}

// For overflow detection, add a digit safely to a long value.
    صحیح
vim_append_digit_long(طویل *قدر, صحیح digit)
{
    طویل x = *قدر;
    اگر (x > ((LONG_MAX - (طویل)digit) / 10))
	واپس FAIL;
    *قدر = x * 10 + (طویل)digit;
    واپس OK;
}

// Return something that fits into an int.
    صحیح
trim_مقصد_int(vimlong_T x)
{
    واپس x > زیادہ_سے_زیادہ_عدد ? زیادہ_سے_زیادہ_عدد : x < INT_MIN ? INT_MIN : x;
}
