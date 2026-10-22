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

#অন্তর্ভাব "vim.h"

স্থির histentry_T *(history[HIST_গুনতি]) = {ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা};
স্থির পূর্ণসংখ্যা	hisidx[HIST_গুনতি] = {-1, -1, -1, -1, -1};  // lastused entry
স্থির পূর্ণসংখ্যা	hisnum[HIST_গুনতি] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
স্থির পূর্ণসংখ্যা	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    পূর্ণসংখ্যা
get_hislen(শূন্য)
{
    ফেরত hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(পূর্ণসংখ্যা hist_type)
{
    ফেরত history[hist_type];
}

#যদি defined(FEAT_VIMINFO)
    শূন্য
set_histentry(পূর্ণসংখ্যা hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    পূর্ণসংখ্যা *
get_hisidx(পূর্ণসংখ্যা hist_type)
{
    ফেরত &hisidx[hist_type];
}

#যদি defined(FEAT_VIMINFO)
    পূর্ণসংখ্যা *
get_hisnum(পূর্ণসংখ্যা hist_type)
{
    ফেরত &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    পূর্ণসংখ্যা
hist_char2type(পূর্ণসংখ্যা c)
{
    যদি (c == ':')
	ফেরত HIST_CMD;
    যদি (c == '=')
	ফেরত HIST_রাশিমালা;
    যদি (c == '@')
	ফেরত HIST_প্রবেশ;
    যদি (c == '>')
	ফেরত HIST_DEBUG;
    ফেরত HIST_অনুসন্ধান;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

স্থির অক্ষর *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ফাঁকা
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    অক্ষর    *short_names = ":=@>?/";
    পূর্ণসংখ্যা	    short_names_গুনতি = (পূর্ণসংখ্যা)STRLEN(short_names);
    পূর্ণসংখ্যা	    history_name_গুনতি = ARRAY_দৈর্ঘ্য(history_names) - 1;

    যদি (idx < short_names_গুনতি)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ফেরত xp->xp_buf;
    }
    যদি (idx < short_names_গুনতি + history_name_গুনতি)
	ফেরত (char_u *)history_names[idx - short_names_গুনতি];
    যদি (idx == short_names_গুনতি + history_name_গুনতি)
	ফেরত (char_u *)"all";
    ফেরত ফাঁকা;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    শূন্য
init_history(শূন্য)
{
    পূর্ণসংখ্যা		newlen;	    // new length of history table
    histentry_T	*অস্থায়ী;
    পূর্ণসংখ্যা		type;

    // If size of history table changed, reallocate it
    newlen = (পূর্ণসংখ্যা)p_hi;
    যদি (newlen == hislen)		// history length didn't change
	ফেরত;

    // history length changed
    জন্য (type = 0; type < HIST_গুনতি; ++type)   // adjust the tables
    {
	যদি (newlen > 0)
	{
	    অস্থায়ী = ALLOC_MULT(histentry_T, newlen);
	    যদি (অস্থায়ী == ফাঁকা)   // out of memory!
	    {
		যদি (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    বিরতি;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		চালু;
	    }
	}
	অন্যথা
	    অস্থায়ী = ফাঁকা;

	যদি (hisidx[type] < 0)		// there are no entries yet
	{
	    পূর্ণসংখ্যা	i;

	    জন্য (i = 0; i < newlen; ++i)
		clear_hist_entry(&অস্থায়ী[i]);
	}
	অন্যথা যদি (newlen > hislen)	// array becomes bigger
	{
	    পূর্ণসংখ্যা	i;
	    পূর্ণসংখ্যা	j;

	    জন্য (i = 0; i <= hisidx[type]; ++i)
		অস্থায়ী[i] = history[type][i];
	    j = i;
	    জন্য ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&অস্থায়ী[i]);
	    জন্য ( ; j < hislen; ++i, ++j)
		অস্থায়ী[i] = history[type][j];
	}
	অন্যথা				// array becomes smaller or 0
	{
	    পূর্ণসংখ্যা	i;
	    পূর্ণসংখ্যা	j;

	    j = hisidx[type];
	    জন্য (i = newlen - 1; ; --i)
	    {
		যদি (i >= 0)		// copy newest entries
		    অস্থায়ী[i] = history[type][j];
		অন্যথা			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		যদি (--j < 0)
		    j = hislen - 1;
		যদি (j == hisidx[type])
		    বিরতি;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = অস্থায়ী;
    }
    hislen = newlen;
}

    শূন্য
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ফাঁকা;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    পূর্ণসংখ্যা
in_history(
    পূর্ণসংখ্যা	    type,
    char_u  *str,
    পূর্ণসংখ্যা	    move_to_front,	// Move the entry to the front if it exists
    পূর্ণসংখ্যা	    sep,
    পূর্ণসংখ্যা	    writing)		// ignore entries read from viminfo
{
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    last_i = -1;
    char_u  *p;
    size_t  len;

    যদি (hisidx[type] < 0)
	ফেরত FALSE;
    i = hisidx[type];
    করো
    {
	যদি (history[type][i].hisstr == ফাঁকা)
	    ফেরত FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	যদি (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_অনুসন্ধান || sep == p[history[type][i].hisstrlen + 1]))
	{
	    যদি (!move_to_front)
		ফেরত TRUE;
	    last_i = i;
	    বিরতি;
	}
	যদি (--i < 0)
	    i = hislen - 1;
    } যখন (i != hisidx[type]);

    যদি (last_i < 0)
	ফেরত FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    যখন (i != hisidx[type])
    {
	যদি (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ফেরত TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    স্থির পূর্ণসংখ্যা
get_histtype(char_u *name)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		len = (পূর্ণসংখ্যা)STRLEN(name);

    // No argument: use current history.
    যদি (len == 0)
	ফেরত hist_char2type(get_cmdline_firstc());

    জন্য (i = 0; history_names[i] != ফাঁকা; ++i)
	যদি (STRNICMP(name, history_names[i], len) == 0)
	    ফেরত i;

    যদি (vim_strchr((char_u *)":=@>?/", name[0]) != ফাঁকা && name[1] == NUL)
	ফেরত hist_char2type(name[0]);

    ফেরত -1;
}

স্থির পূর্ণসংখ্যা	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    শূন্য
add_to_history(
    পূর্ণসংখ্যা		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    পূর্ণসংখ্যা		in_map,		// consider maptick when inside a mapping
    পূর্ণসংখ্যা		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    যদি (hislen == 0)		// no history
	ফেরত;

    যদি ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_অনুসন্ধান)
	ফেরত;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    যদি (histype == HIST_অনুসন্ধান && in_map)
    {
	যদি (maptick == last_maptick && hisidx[HIST_অনুসন্ধান] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_অনুসন্ধান][hisidx[HIST_অনুসন্ধান]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    যদি (--hisidx[HIST_অনুসন্ধান] < 0)
		hisidx[HIST_অনুসন্ধান] = hislen - 1;
	}
	last_maptick = -1;
    }

    যদি (in_history(histype, new_entry, TRUE, sep, FALSE))
	ফেরত;

    যদি (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    যদি (hisptr->hisstr == ফাঁকা)
	hisptr->hisstrlen = 0;
    অন্যথা
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    যদি (histype == HIST_অনুসন্ধান && in_map)
	last_maptick = maptick;
}

#যদি defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণসংখ্যা
get_history_idx(পূর্ণসংখ্যা histype)
{
    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি
		    || hisidx[histype] < 0)
	ফেরত -1;

    ফেরত history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণসংখ্যা
calc_hist_idx(পূর্ণসংখ্যা histype, পূর্ণসংখ্যা অঙ্ক)
{
    পূর্ণসংখ্যা		i;
    histentry_T	*hist;
    পূর্ণসংখ্যা		wrapped = FALSE;

    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি
		    || (i = hisidx[histype]) < 0 || অঙ্ক == 0)
	ফেরত -1;

    hist = history[histype];
    যদি (অঙ্ক > 0)
    {
	যখন (hist[i].hisnum > অঙ্ক)
	    যদি (--i < 0)
	    {
		যদি (wrapped)
		    বিরতি;
		i += hislen;
		wrapped = TRUE;
	    }
	যদি (i >= 0 && hist[i].hisnum == অঙ্ক && hist[i].hisstr != ফাঁকা)
	    ফেরত i;
    }
    অন্যথা যদি (-অঙ্ক <= hislen)
    {
	i += অঙ্ক + 1;
	যদি (i < 0)
	    i += hislen;
	যদি (hist[i].hisstr != ফাঁকা)
	    ফেরত i;
    }
    ফেরত -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণসংখ্যা
clr_history(পূর্ণসংখ্যা histype)
{
    পূর্ণসংখ্যা		i;
    histentry_T	*hisptr;

    যদি (hislen != 0 && histype >= 0 && histype < HIST_গুনতি)
    {
	hisptr = history[histype];
	জন্য (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ফেরত OK;
    }
    ফেরত FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণসংখ্যা
del_history_entry(পূর্ণসংখ্যা histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    পূর্ণসংখ্যা		idx;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		last;
    পূর্ণসংখ্যা		found = FALSE;

    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি || *str == NUL
		|| hisidx[histype] < 0)
	ফেরত FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    যদি (regmatch.regprog == ফাঁকা)
	ফেরত FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    করো
    {
	hisptr = &history[histype][i];
	যদি (hisptr->hisstr == ফাঁকা)
	    বিরতি;
	যদি (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	অন্যথা
	{
	    যদি (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    যদি (--last < 0)
		last += hislen;
	}
	যদি (--i < 0)
	    i += hislen;
    } যখন (i != idx);

    যদি (history[histype][idx].hisstr == ফাঁকা)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ফেরত found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণসংখ্যা
del_history_idx(পূর্ণসংখ্যা histype, পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা	    i, j;

    i = calc_hist_idx(histype, idx);
    যদি (i < 0)
	ফেরত FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    যদি (histype == HIST_অনুসন্ধান && maptick == last_maptick && i == idx)
	last_maptick = -1;

    যখন (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    যদি (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ফেরত TRUE;
}

/*
 * "histadd()" function
 */
    শূন্য
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_সংখ্যা = FALSE;
    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ফাঁকা ? get_histtype(str) : -1;
    যদি (histype < 0)
	ফেরত;

    str = tv_get_string_buf(&argvars[1], buf);
    যদি (*str == NUL)
	ফেরত;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_সংখ্যা = TRUE;
}

/*
 * "histdel()" function
 */
    শূন্য
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    পূর্ণসংখ্যা		n;
    char_u	*str;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    যদি (str == ফাঁকা)
	n = 0;
    অন্যথা যদি (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    অন্যথা যদি (argvars[1].v_type == VAR_সংখ্যা)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]));
    অন্যথা
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_সংখ্যা = n;
}

/*
 * "histget()" function
 */
    শূন্য
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    যদি (str == ফাঁকা)
	rettv->vval.v_string = ফাঁকা;
    অন্যথা
    {
	পূর্ণসংখ্যা type;
	পূর্ণসংখ্যা idx;

	type = get_histtype(str);
	যদি (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	অন্যথা
	    idx = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[1], ফাঁকা);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	যদি (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	অন্যথা
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    শূন্য
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		i;
    char_u	*histname;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ফাঁকা ? HIST_CMD - 1 : get_histtype(histname);
    যদি (i >= HIST_CMD && i < HIST_গুনতি)
	rettv->vval.v_সংখ্যা = get_history_idx(i);
    অন্যথা
	rettv->vval.v_সংখ্যা = -1;
}
#endif // FEAT_EVAL

#যদি defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    শূন্য
remove_চাবি_from_history(শূন্য)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    পূর্ণসংখ্যা		i;

    i = hisidx[HIST_CMD];
    যদি (i < 0)
	ফেরত;
    p_start = history[HIST_CMD][i].hisstr;
    যদি (p_start == ফাঁকা)
	ফেরত;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    জন্য (p = p_start; *p; ++p)
    {
	যদি (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    যদি (p == ফাঁকা)
		বিরতি;
	    ++p;
	    জন্য (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		যদি (p[i] == '\\' && p[i + 1])
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
    শূন্য
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    পূর্ণসংখ্যা		histype1 = HIST_CMD;
    পূর্ণসংখ্যা		histype2 = HIST_CMD;
    পূর্ণসংখ্যা		hisidx1 = 1;
    পূর্ণসংখ্যা		hisidx2 = -1;
    পূর্ণসংখ্যা		idx;
    পূর্ণসংখ্যা		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    যদি (hislen == 0)
    {
	বার্তা(_("'history' option is zero"));
	ফেরত;
    }

    যদি (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	যখন (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != ফাঁকা)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	যদি (histype1 == -1)
	{
	    যদি (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_গুনতি-1;
	    }
	    অন্যথা
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ফেরত;
	    }
	}
	অন্যথা
	    histype2 = histype1;
	*end = i;
    }
    অন্যথা
	end = arg;
    যদি (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	যদি (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	অন্যথা
	    semsg(_(e_val_too_large), arg);
	ফেরত;
    }

    জন্য (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((অক্ষর *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	বার্তা_puts_title((অক্ষর *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	যদি (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	যদি (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	যদি (idx >= 0 && j <= k)
	    জন্য (i = idx + 1; !got_int; ++i)
	    {
		যদি (i == hislen)
		    i = 0;
		যদি (hist[i].hisstr != ফাঁকা
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    পূর্ণসংখ্যা  len;

		    বার্তা_putchar('\n');
		    len = vim_snprintf((অক্ষর *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    যদি (vim_strsize(hist[i].hisstr) > (পূর্ণসংখ্যা)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (পূর্ণসংখ্যা)Columns - 10, IOSIZE - (পূর্ণসংখ্যা)len);
		    অন্যথা
			STRCPY(IObuff + len, hist[i].hisstr);
		    বার্তা_outtrans(IObuff);
		    out_flush();
		}
		যদি (i == idx)
		    বিরতি;
	    }
    }
}
