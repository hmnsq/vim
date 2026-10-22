/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * cmdhist.c: Functions for the history of the command-line.
 */

#شامل "vim.h"

ایستا histentry_T *(history[HIST_شمار]) = {پوچ, پوچ, پوچ, پوچ, پوچ};
ایستا صحیح	hisidx[HIST_شمار] = {-1, -1, -1, -1, -1};  // lastused entry
ایستا صحیح	hisnum[HIST_شمار] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ایستا صحیح	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    صحیح
get_hislen(تهی)
{
    بازگشت hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(صحیح hist_type)
{
    بازگشت history[hist_type];
}

#اگر defined(FEAT_VIMINFO)
    تهی
set_histentry(صحیح hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    صحیح *
get_hisidx(صحیح hist_type)
{
    بازگشت &hisidx[hist_type];
}

#اگر defined(FEAT_VIMINFO)
    صحیح *
get_hisnum(صحیح hist_type)
{
    بازگشت &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    صحیح
hist_char2type(صحیح c)
{
    اگر (c == ':')
	بازگشت HIST_CMD;
    اگر (c == '=')
	بازگشت HIST_عبارت;
    اگر (c == '@')
	بازگشت HIST_درون_داد;
    اگر (c == '>')
	بازگشت HIST_DEBUG;
    بازگشت HIST_جستجو;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ایستا نویسه *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    پوچ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, صحیح idx)
{
    نویسه    *short_names = ":=@>?/";
    صحیح	    short_names_شمار = (صحیح)STRLEN(short_names);
    صحیح	    history_name_شمار = ARRAY_LENGTH(history_names) - 1;

    اگر (idx < short_names_شمار)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	بازگشت xp->xp_buf;
    }
    اگر (idx < short_names_شمار + history_name_شمار)
	بازگشت (char_u *)history_names[idx - short_names_شمار];
    اگر (idx == short_names_شمار + history_name_شمار)
	بازگشت (char_u *)"all";
    بازگشت پوچ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    تهی
init_history(تهی)
{
    صحیح		newlen;	    // new length of history table
    histentry_T	*temp;
    صحیح		type;

    // If size of history table changed, reallocate it
    newlen = (صحیح)p_hi;
    اگر (newlen == hislen)		// history length didn't change
	بازگشت;

    // history length changed
    برای (type = 0; type < HIST_شمار; ++type)   // adjust the tables
    {
	اگر (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    اگر (temp == پوچ)   // out of memory!
	    {
		اگر (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    بشکن;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ادامه;
	    }
	}
	وگرنه
	    temp = پوچ;

	اگر (hisidx[type] < 0)		// there are no entries yet
	{
	    صحیح	i;

	    برای (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	وگرنه اگر (newlen > hislen)	// array becomes bigger
	{
	    صحیح	i;
	    صحیح	j;

	    برای (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    برای ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    برای ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	وگرنه				// array becomes smaller or 0
	{
	    صحیح	i;
	    صحیح	j;

	    j = hisidx[type];
	    برای (i = newlen - 1; ; --i)
	    {
		اگر (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		وگرنه			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		اگر (--j < 0)
		    j = hislen - 1;
		اگر (j == hisidx[type])
		    بشکن;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    تهی
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = پوچ;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    صحیح
in_history(
    صحیح	    type,
    char_u  *str,
    صحیح	    move_to_front,	// Move the entry to the front if it exists
    صحیح	    sep,
    صحیح	    writing)		// ignore entries read from viminfo
{
    صحیح	    i;
    صحیح	    last_i = -1;
    char_u  *p;
    size_t  len;

    اگر (hisidx[type] < 0)
	بازگشت FALSE;
    i = hisidx[type];
    انجامبده
    {
	اگر (history[type][i].hisstr == پوچ)
	    بازگشت FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	اگر (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_جستجو || sep == p[history[type][i].hisstrlen + 1]))
	{
	    اگر (!move_to_front)
		بازگشت TRUE;
	    last_i = i;
	    بشکن;
	}
	اگر (--i < 0)
	    i = hislen - 1;
    } وقتی (i != hisidx[type]);

    اگر (last_i < 0)
	بازگشت FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    وقتی (i != hisidx[type])
    {
	اگر (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    بازگشت TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ایستا صحیح
get_histtype(char_u *name)
{
    صحیح		i;
    صحیح		len = (صحیح)STRLEN(name);

    // No argument: use current history.
    اگر (len == 0)
	بازگشت hist_char2type(get_cmdline_firstc());

    برای (i = 0; history_names[i] != پوچ; ++i)
	اگر (STRNICMP(name, history_names[i], len) == 0)
	    بازگشت i;

    اگر (vim_strchr((char_u *)":=@>?/", name[0]) != پوچ && name[1] == NUL)
	بازگشت hist_char2type(name[0]);

    بازگشت -1;
}

ایستا صحیح	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    تهی
add_to_history(
    صحیح		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    صحیح		in_map,		// consider maptick when inside a mapping
    صحیح		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    اگر (hislen == 0)		// no history
	بازگشت;

    اگر ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_جستجو)
	بازگشت;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    اگر (histype == HIST_جستجو && in_map)
    {
	اگر (maptick == last_maptick && hisidx[HIST_جستجو] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_جستجو][hisidx[HIST_جستجو]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    اگر (--hisidx[HIST_جستجو] < 0)
		hisidx[HIST_جستجو] = hislen - 1;
	}
	last_maptick = -1;
    }

    اگر (in_history(histype, new_entry, TRUE, sep, FALSE))
	بازگشت;

    اگر (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    اگر (hisptr->hisstr == پوچ)
	hisptr->hisstrlen = 0;
    وگرنه
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    اگر (histype == HIST_جستجو && in_map)
	last_maptick = maptick;
}

#اگر defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ایستا صحیح
get_history_idx(صحیح histype)
{
    اگر (hislen == 0 || histype < 0 || histype >= HIST_شمار
		    || hisidx[histype] < 0)
	بازگشت -1;

    بازگشت history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ایستا صحیح
calc_hist_idx(صحیح histype, صحیح رقم)
{
    صحیح		i;
    histentry_T	*hist;
    صحیح		wrapped = FALSE;

    اگر (hislen == 0 || histype < 0 || histype >= HIST_شمار
		    || (i = hisidx[histype]) < 0 || رقم == 0)
	بازگشت -1;

    hist = history[histype];
    اگر (رقم > 0)
    {
	وقتی (hist[i].hisnum > رقم)
	    اگر (--i < 0)
	    {
		اگر (wrapped)
		    بشکن;
		i += hislen;
		wrapped = TRUE;
	    }
	اگر (i >= 0 && hist[i].hisnum == رقم && hist[i].hisstr != پوچ)
	    بازگشت i;
    }
    وگرنه اگر (-رقم <= hislen)
    {
	i += رقم + 1;
	اگر (i < 0)
	    i += hislen;
	اگر (hist[i].hisstr != پوچ)
	    بازگشت i;
    }
    بازگشت -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ایستا صحیح
clr_history(صحیح histype)
{
    صحیح		i;
    histentry_T	*hisptr;

    اگر (hislen != 0 && histype >= 0 && histype < HIST_شمار)
    {
	hisptr = history[histype];
	برای (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	بازگشت OK;
    }
    بازگشت FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ایستا صحیح
del_history_entry(صحیح histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    صحیح		idx;
    صحیح		i;
    صحیح		last;
    صحیح		found = FALSE;

    اگر (hislen == 0 || histype < 0 || histype >= HIST_شمار || *str == NUL
		|| hisidx[histype] < 0)
	بازگشت FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    اگر (regmatch.regprog == پوچ)
	بازگشت FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    انجامبده
    {
	hisptr = &history[histype][i];
	اگر (hisptr->hisstr == پوچ)
	    بشکن;
	اگر (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	وگرنه
	{
	    اگر (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    اگر (--last < 0)
		last += hislen;
	}
	اگر (--i < 0)
	    i += hislen;
    } وقتی (i != idx);

    اگر (history[histype][idx].hisstr == پوچ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    بازگشت found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ایستا صحیح
del_history_idx(صحیح histype, صحیح idx)
{
    صحیح	    i, j;

    i = calc_hist_idx(histype, idx);
    اگر (i < 0)
	بازگشت FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    اگر (histype == HIST_جستجو && maptick == last_maptick && i == idx)
	last_maptick = -1;

    وقتی (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    اگر (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    بازگشت TRUE;
}

/*
 * "histadd()" function
 */
    تهی
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_عدد = FALSE;
    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != پوچ ? get_histtype(str) : -1;
    اگر (histype < 0)
	بازگشت;

    str = tv_get_string_buf(&argvars[1], buf);
    اگر (*str == NUL)
	بازگشت;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_عدد = TRUE;
}

/*
 * "histdel()" function
 */
    تهی
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    صحیح		n;
    char_u	*str;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    اگر (str == پوچ)
	n = 0;
    وگرنه اگر (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    وگرنه اگر (argvars[1].v_type == VAR_عدد)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحیح)tv_get_عدد(&argvars[1]));
    وگرنه
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_عدد = n;
}

/*
 * "histget()" function
 */
    تهی
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    اگر (str == پوچ)
	rettv->vval.v_string = پوچ;
    وگرنه
    {
	صحیح type;
	صحیح idx;

	type = get_histtype(str);
	اگر (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	وگرنه
	    idx = (صحیح)tv_get_عدد_chk(&argvars[1], پوچ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	اگر (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	وگرنه
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    تهی
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		i;
    char_u	*histname;

    اگر (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == پوچ ? HIST_CMD - 1 : get_histtype(histname);
    اگر (i >= HIST_CMD && i < HIST_شمار)
	rettv->vval.v_عدد = get_history_idx(i);
    وگرنه
	rettv->vval.v_عدد = -1;
}
#endif // FEAT_EVAL

#اگر defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    تهی
remove_کلید_from_history(تهی)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    صحیح		i;

    i = hisidx[HIST_CMD];
    اگر (i < 0)
	بازگشت;
    p_start = history[HIST_CMD][i].hisstr;
    اگر (p_start == پوچ)
	بازگشت;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    برای (p = p_start; *p; ++p)
    {
	اگر (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    اگر (p == پوچ)
		بشکن;
	    ++p;
	    برای (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		اگر (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_end - (p + i)) + 1);	    // +1 for the NUL
	    p_end -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_end - p_start);
}
#endif

/*
 * :history command - print a history
 */
    تهی
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    صحیح		histype1 = HIST_CMD;
    صحیح		histype2 = HIST_CMD;
    صحیح		hisidx1 = 1;
    صحیح		hisidx2 = -1;
    صحیح		idx;
    صحیح		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    اگر (hislen == 0)
    {
	پیام(_("'history' option is zero"));
	بازگشت;
    }

    اگر (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	وقتی (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != پوچ)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	اگر (histype1 == -1)
	{
	    اگر (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_شمار-1;
	    }
	    وگرنه
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		بازگشت;
	    }
	}
	وگرنه
	    histype2 = histype1;
	*end = i;
    }
    وگرنه
	end = arg;
    اگر (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	اگر (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	وگرنه
	    semsg(_(e_val_too_large), arg);
	بازگشت;
    }

    برای (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((نویسه *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	پیام_puts_title((نویسه *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	اگر (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	اگر (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	اگر (idx >= 0 && j <= k)
	    برای (i = idx + 1; !got_int; ++i)
	    {
		اگر (i == hislen)
		    i = 0;
		اگر (hist[i].hisstr != پوچ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    صحیح  len;

		    پیام_putchar('\n');
		    len = vim_snprintf((نویسه *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    اگر (vim_strsize(hist[i].hisstr) > (صحیح)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (صحیح)Columns - 10, IOSIZE - (صحیح)len);
		    وگرنه
			STRCPY(IObuff + len, hist[i].hisstr);
		    پیام_outtrans(IObuff);
		    out_flush();
		}
		اگر (i == idx)
		    بشکن;
	    }
    }
}
