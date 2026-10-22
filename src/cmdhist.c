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

#ضمن "vim.h"

ساكن histentry_T *(history[HIST_COUNT]) = {عدم, عدم, عدم, عدم, عدم};
ساكن صحيح	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
ساكن صحيح	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ساكن صحيح	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    صحيح
get_hislen(فراغ)
{
    ܦܢܐ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(صحيح hist_type)
{
    ܦܢܐ history[hist_type];
}

#ܐܢ defined(FEAT_VIMINFO)
    فراغ
set_histentry(صحيح hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    صحيح *
get_hisidx(صحيح hist_type)
{
    ܦܢܐ &hisidx[hist_type];
}

#ܐܢ defined(FEAT_VIMINFO)
    صحيح *
get_hisnum(صحيح hist_type)
{
    ܦܢܐ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    صحيح
hist_char2type(صحيح c)
{
    ܐܢ (c == ':')
	ܦܢܐ HIST_CMD;
    ܐܢ (c == '=')
	ܦܢܐ HIST_EXPR;
    ܐܢ (c == '@')
	ܦܢܐ HIST_INPUT;
    ܐܢ (c == '>')
	ܦܢܐ HIST_DEBUG;
    ܦܢܐ HIST_SEARCH;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ساكن حرف *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    عدم
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, صحيح idx)
{
    حرف    *short_names = ":=@>?/";
    صحيح	    short_names_count = (صحيح)STRLEN(short_names);
    صحيح	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ܐܢ (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ܦܢܐ xp->xp_buf;
    }
    ܐܢ (idx < short_names_count + history_name_count)
	ܦܢܐ (char_u *)history_names[idx - short_names_count];
    ܐܢ (idx == short_names_count + history_name_count)
	ܦܢܐ (char_u *)"all";
    ܦܢܐ عدم;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    فراغ
init_history(فراغ)
{
    صحيح		newlen;	    // new length of history table
    histentry_T	*temp;
    صحيح		type;

    // If size of history table changed, reallocate it
    newlen = (صحيح)p_hi;
    ܐܢ (newlen == hislen)		// history length didn't change
	ܦܢܐ;

    // history length changed
    لكل (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ܐܢ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ܐܢ (temp == عدم)   // out of memory!
	    {
		ܐܢ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    اكسر;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		استمر;
	    }
	}
	ܐܚܪܢܐ
	    temp = عدم;

	ܐܢ (hisidx[type] < 0)		// there are no entries yet
	{
	    صحيح	i;

	    لكل (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	ܐܚܪܢܐ ܐܢ (newlen > hislen)	// array becomes bigger
	{
	    صحيح	i;
	    صحيح	j;

	    لكل (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    لكل ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    لكل ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	ܐܚܪܢܐ				// array becomes smaller or 0
	{
	    صحيح	i;
	    صحيح	j;

	    j = hisidx[type];
	    لكل (i = newlen - 1; ; --i)
	    {
		ܐܢ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		ܐܚܪܢܐ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ܐܢ (--j < 0)
		    j = hislen - 1;
		ܐܢ (j == hisidx[type])
		    اكسر;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    فراغ
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = عدم;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    صحيح
in_history(
    صحيح	    type,
    char_u  *str,
    صحيح	    move_to_front,	// Move the entry to the front if it exists
    صحيح	    sep,
    صحيح	    writing)		// ignore entries read from viminfo
{
    صحيح	    i;
    صحيح	    last_i = -1;
    char_u  *p;
    size_t  len;

    ܐܢ (hisidx[type] < 0)
	ܦܢܐ FALSE;
    i = hisidx[type];
    ܥܒܕ
    {
	ܐܢ (history[type][i].hisstr == عدم)
	    ܦܢܐ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ܐܢ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ܐܢ (!move_to_front)
		ܦܢܐ TRUE;
	    last_i = i;
	    اكسر;
	}
	ܐܢ (--i < 0)
	    i = hislen - 1;
    } ܟܡܐ (i != hisidx[type]);

    ܐܢ (last_i < 0)
	ܦܢܐ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ܟܡܐ (i != hisidx[type])
    {
	ܐܢ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ܦܢܐ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ساكن صحيح
get_histtype(char_u *name)
{
    صحيح		i;
    صحيح		len = (صحيح)STRLEN(name);

    // No argument: use current history.
    ܐܢ (len == 0)
	ܦܢܐ hist_char2type(get_cmdline_firstc());

    لكل (i = 0; history_names[i] != عدم; ++i)
	ܐܢ (STRNICMP(name, history_names[i], len) == 0)
	    ܦܢܐ i;

    ܐܢ (vim_strchr((char_u *)":=@>?/", name[0]) != عدم && name[1] == NUL)
	ܦܢܐ hist_char2type(name[0]);

    ܦܢܐ -1;
}

ساكن صحيح	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    فراغ
add_to_history(
    صحيح		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    صحيح		in_map,		// consider maptick when inside a mapping
    صحيح		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ܐܢ (hislen == 0)		// no history
	ܦܢܐ;

    ܐܢ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ܦܢܐ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ܐܢ (histype == HIST_SEARCH && in_map)
    {
	ܐܢ (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ܐܢ (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ܐܢ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ܦܢܐ;

    ܐܢ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ܐܢ (hisptr->hisstr == عدم)
	hisptr->hisstrlen = 0;
    ܐܚܪܢܐ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ܐܢ (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ܐܢ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
get_history_idx(صحيح histype)
{
    ܐܢ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ܦܢܐ -1;

    ܦܢܐ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
calc_hist_idx(صحيح histype, صحيح num)
{
    صحيح		i;
    histentry_T	*hist;
    صحيح		wrapped = FALSE;

    ܐܢ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ܦܢܐ -1;

    hist = history[histype];
    ܐܢ (num > 0)
    {
	ܟܡܐ (hist[i].hisnum > num)
	    ܐܢ (--i < 0)
	    {
		ܐܢ (wrapped)
		    اكسر;
		i += hislen;
		wrapped = TRUE;
	    }
	ܐܢ (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != عدم)
	    ܦܢܐ i;
    }
    ܐܚܪܢܐ ܐܢ (-num <= hislen)
    {
	i += num + 1;
	ܐܢ (i < 0)
	    i += hislen;
	ܐܢ (hist[i].hisstr != عدم)
	    ܦܢܐ i;
    }
    ܦܢܐ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
clr_history(صحيح histype)
{
    صحيح		i;
    histentry_T	*hisptr;

    ܐܢ (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	لكل (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ܦܢܐ OK;
    }
    ܦܢܐ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
del_history_entry(صحيح histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    صحيح		idx;
    صحيح		i;
    صحيح		last;
    صحيح		found = FALSE;

    ܐܢ (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ܦܢܐ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ܐܢ (regmatch.regprog == عدم)
	ܦܢܐ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    ܥܒܕ
    {
	hisptr = &history[histype][i];
	ܐܢ (hisptr->hisstr == عدم)
	    اكسر;
	ܐܢ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ܐܢ (--last < 0)
		last += hislen;
	}
	ܐܢ (--i < 0)
	    i += hislen;
    } ܟܡܐ (i != idx);

    ܐܢ (history[histype][idx].hisstr == عدم)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ܦܢܐ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
del_history_idx(صحيح histype, صحيح idx)
{
    صحيح	    i, j;

    i = calc_hist_idx(histype, idx);
    ܐܢ (i < 0)
	ܦܢܐ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ܐܢ (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ܟܡܐ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ܐܢ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ܦܢܐ TRUE;
}

/*
 * "histadd()" function
 */
    فراغ
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ܐܢ (check_secure())
	ܦܢܐ;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != عدم ? get_histtype(str) : -1;
    ܐܢ (histype < 0)
	ܦܢܐ;

    str = tv_get_string_buf(&argvars[1], buf);
    ܐܢ (*str == NUL)
	ܦܢܐ;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_number = TRUE;
}

/*
 * "histdel()" function
 */
    فراغ
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    صحيح		n;
    char_u	*str;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ܐܢ (str == عدم)
	n = 0;
    ܐܚܪܢܐ ܐܢ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ܐܚܪܢܐ ܐܢ (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحيح)tv_get_number(&argvars[1]));
    ܐܚܪܢܐ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_number = n;
}

/*
 * "histget()" function
 */
    فراغ
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ܐܢ (str == عدم)
	rettv->vval.v_string = عدم;
    ܐܚܪܢܐ
    {
	صحيح type;
	صحيح idx;

	type = get_histtype(str);
	ܐܢ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ܐܚܪܢܐ
	    idx = (صحيح)tv_get_number_chk(&argvars[1], عدم);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ܐܢ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ܐܚܪܢܐ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    فراغ
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح		i;
    char_u	*histname;

    ܐܢ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == عدم ? HIST_CMD - 1 : get_histtype(histname);
    ܐܢ (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    ܐܚܪܢܐ
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ܐܢ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    فراغ
remove_key_from_history(فراغ)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    صحيح		i;

    i = hisidx[HIST_CMD];
    ܐܢ (i < 0)
	ܦܢܐ;
    p_start = history[HIST_CMD][i].hisstr;
    ܐܢ (p_start == عدم)
	ܦܢܐ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    لكل (p = p_start; *p; ++p)
    {
	ܐܢ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ܐܢ (p == عدم)
		اكسر;
	    ++p;
	    لكل (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ܐܢ (p[i] == '\\' && p[i + 1])
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
    فراغ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    صحيح		histype1 = HIST_CMD;
    صحيح		histype2 = HIST_CMD;
    صحيح		hisidx1 = 1;
    صحيح		hisidx2 = -1;
    صحيح		idx;
    صحيح		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ܐܢ (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ܦܢܐ;
    }

    ܐܢ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ܟܡܐ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != عدم)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ܐܢ (histype1 == -1)
	{
	    ܐܢ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    ܐܚܪܢܐ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ܦܢܐ;
	    }
	}
	ܐܚܪܢܐ
	    histype2 = histype1;
	*end = i;
    }
    ܐܚܪܢܐ
	end = arg;
    ܐܢ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ܐܢ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	ܐܚܪܢܐ
	    semsg(_(e_val_too_large), arg);
	ܦܢܐ;
    }

    لكل (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((حرف *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ܐܢ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ܐܢ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ܐܢ (idx >= 0 && j <= k)
	    لكل (i = idx + 1; !got_int; ++i)
	    {
		ܐܢ (i == hislen)
		    i = 0;
		ܐܢ (hist[i].hisstr != عدم
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    صحيح  len;

		    msg_putchar('\n');
		    len = vim_snprintf((حرف *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ܐܢ (vim_strsize(hist[i].hisstr) > (صحيح)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (صحيح)Columns - 10, IOSIZE - (صحيح)len);
		    ܐܚܪܢܐ
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ܐܢ (i == idx)
		    اكسر;
	    }
    }
}
