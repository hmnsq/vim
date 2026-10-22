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

#অন্তর্ভাৱ "vim.h"

স্থির histentry_T *(history[HIST_গুনতি]) = {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};
স্থির পূর্ণাঙ্ক	hisidx[HIST_গুনতি] = {-1, -1, -1, -1, -1};  // lastused entry
স্থির পূর্ণাঙ্ক	hisnum[HIST_গুনতি] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
স্থির পূর্ণাঙ্ক	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    পূর্ণাঙ্ক
get_hislen(শূন্য)
{
    ঘূৰাই hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(পূর্ণাঙ্ক hist_type)
{
    ঘূৰাই history[hist_type];
}

#যদি defined(FEAT_VIMINFO)
    শূন্য
set_histentry(পূর্ণাঙ্ক hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    পূর্ণাঙ্ক *
get_hisidx(পূর্ণাঙ্ক hist_type)
{
    ঘূৰাই &hisidx[hist_type];
}

#যদি defined(FEAT_VIMINFO)
    পূর্ণাঙ্ক *
get_hisnum(পূর্ণাঙ্ক hist_type)
{
    ঘূৰাই &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    পূর্ণাঙ্ক
hist_char2type(পূর্ণাঙ্ক c)
{
    যদি (c == ':')
	ঘূৰাই HIST_CMD;
    যদি (c == '=')
	ঘূৰাই HIST_ৰাশিমালা;
    যদি (c == '@')
	ঘূৰাই HIST_প্ৰবেশ;
    যদি (c == '>')
	ঘূৰাই HIST_DEBUG;
    ঘূৰাই HIST_অনুসন্ধান;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

স্থির ৱর্ণ *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    রিক্ত
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    ৱর্ণ    *short_names = ":=@>?/";
    পূর্ণাঙ্ক	    short_names_গুনতি = (পূর্ণাঙ্ক)STRLEN(short_names);
    পূর্ণাঙ্ক	    history_name_গুনতি = ARRAY_LENGTH(history_names) - 1;

    যদি (idx < short_names_গুনতি)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ঘূৰাই xp->xp_buf;
    }
    যদি (idx < short_names_গুনতি + history_name_গুনতি)
	ঘূৰাই (char_u *)history_names[idx - short_names_গুনতি];
    যদি (idx == short_names_গুনতি + history_name_গুনতি)
	ঘূৰাই (char_u *)"all";
    ঘূৰাই রিক্ত;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    শূন্য
init_history(শূন্য)
{
    পূর্ণাঙ্ক		newlen;	    // new length of history table
    histentry_T	*temp;
    পূর্ণাঙ্ক		type;

    // If size of history table changed, reallocate it
    newlen = (পূর্ণাঙ্ক)p_hi;
    যদি (newlen == hislen)		// history length didn't change
	ঘূৰাই;

    // history length changed
    বাবে (type = 0; type < HIST_গুনতি; ++type)   // adjust the tables
    {
	যদি (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    যদি (temp == রিক্ত)   // out of memory!
	    {
		যদি (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ভঙ্গ;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		অনুৱর্ত;
	    }
	}
	অন্যথা
	    temp = রিক্ত;

	যদি (hisidx[type] < 0)		// there are no entries yet
	{
	    পূর্ণাঙ্ক	i;

	    বাবে (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	অন্যথা যদি (newlen > hislen)	// array becomes bigger
	{
	    পূর্ণাঙ্ক	i;
	    পূর্ণাঙ্ক	j;

	    বাবে (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    বাবে ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    বাবে ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	অন্যথা				// array becomes smaller or 0
	{
	    পূর্ণাঙ্ক	i;
	    পূর্ণাঙ্ক	j;

	    j = hisidx[type];
	    বাবে (i = newlen - 1; ; --i)
	    {
		যদি (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		অন্যথা			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		যদি (--j < 0)
		    j = hislen - 1;
		যদি (j == hisidx[type])
		    ভঙ্গ;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    শূন্য
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = রিক্ত;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    পূর্ণাঙ্ক
in_history(
    পূর্ণাঙ্ক	    type,
    char_u  *str,
    পূর্ণাঙ্ক	    move_to_front,	// Move the entry to the front if it exists
    পূর্ণাঙ্ক	    sep,
    পূর্ণাঙ্ক	    writing)		// ignore entries read from viminfo
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    last_i = -1;
    char_u  *p;
    size_t  len;

    যদি (hisidx[type] < 0)
	ঘূৰাই FALSE;
    i = hisidx[type];
    কৰা
    {
	যদি (history[type][i].hisstr == রিক্ত)
	    ঘূৰাই FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	যদি (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_অনুসন্ধান || sep == p[history[type][i].hisstrlen + 1]))
	{
	    যদি (!move_to_front)
		ঘূৰাই TRUE;
	    last_i = i;
	    ভঙ্গ;
	}
	যদি (--i < 0)
	    i = hislen - 1;
    } যেতিয়ালৈ (i != hisidx[type]);

    যদি (last_i < 0)
	ঘূৰাই FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    যেতিয়ালৈ (i != hisidx[type])
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
    ঘূৰাই TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    স্থির পূর্ণাঙ্ক
get_histtype(char_u *name)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(name);

    // No argument: use current history.
    যদি (len == 0)
	ঘূৰাই hist_char2type(get_cmdline_firstc());

    বাবে (i = 0; history_names[i] != রিক্ত; ++i)
	যদি (STRNICMP(name, history_names[i], len) == 0)
	    ঘূৰাই i;

    যদি (vim_strchr((char_u *)":=@>?/", name[0]) != রিক্ত && name[1] == NUL)
	ঘূৰাই hist_char2type(name[0]);

    ঘূৰাই -1;
}

স্থির পূর্ণাঙ্ক	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    শূন্য
add_to_history(
    পূর্ণাঙ্ক		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    পূর্ণাঙ্ক		in_map,		// consider maptick when inside a mapping
    পূর্ণাঙ্ক		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    যদি (hislen == 0)		// no history
	ঘূৰাই;

    যদি ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_অনুসন্ধান)
	ঘূৰাই;

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
	ঘূৰাই;

    যদি (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    যদি (hisptr->hisstr == রিক্ত)
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
    স্থির পূর্ণাঙ্ক
get_history_idx(পূর্ণাঙ্ক histype)
{
    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি
		    || hisidx[histype] < 0)
	ঘূৰাই -1;

    ঘূৰাই history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণাঙ্ক
calc_hist_idx(পূর্ণাঙ্ক histype, পূর্ণাঙ্ক অঙ্ক)
{
    পূর্ণাঙ্ক		i;
    histentry_T	*hist;
    পূর্ণাঙ্ক		wrapped = FALSE;

    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি
		    || (i = hisidx[histype]) < 0 || অঙ্ক == 0)
	ঘূৰাই -1;

    hist = history[histype];
    যদি (অঙ্ক > 0)
    {
	যেতিয়ালৈ (hist[i].hisnum > অঙ্ক)
	    যদি (--i < 0)
	    {
		যদি (wrapped)
		    ভঙ্গ;
		i += hislen;
		wrapped = TRUE;
	    }
	যদি (i >= 0 && hist[i].hisnum == অঙ্ক && hist[i].hisstr != রিক্ত)
	    ঘূৰাই i;
    }
    অন্যথা যদি (-অঙ্ক <= hislen)
    {
	i += অঙ্ক + 1;
	যদি (i < 0)
	    i += hislen;
	যদি (hist[i].hisstr != রিক্ত)
	    ঘূৰাই i;
    }
    ঘূৰাই -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণাঙ্ক
clr_history(পূর্ণাঙ্ক histype)
{
    পূর্ণাঙ্ক		i;
    histentry_T	*hisptr;

    যদি (hislen != 0 && histype >= 0 && histype < HIST_গুনতি)
    {
	hisptr = history[histype];
	বাবে (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ঘূৰাই OK;
    }
    ঘূৰাই FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণাঙ্ক
del_history_entry(পূর্ণাঙ্ক histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		last;
    পূর্ণাঙ্ক		found = FALSE;

    যদি (hislen == 0 || histype < 0 || histype >= HIST_গুনতি || *str == NUL
		|| hisidx[histype] < 0)
	ঘূৰাই FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    যদি (regmatch.regprog == রিক্ত)
	ঘূৰাই FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    কৰা
    {
	hisptr = &history[histype][i];
	যদি (hisptr->hisstr == রিক্ত)
	    ভঙ্গ;
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
    } যেতিয়ালৈ (i != idx);

    যদি (history[histype][idx].hisstr == রিক্ত)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ঘূৰাই found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    স্থির পূর্ণাঙ্ক
del_history_idx(পূর্ণাঙ্ক histype, পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক	    i, j;

    i = calc_hist_idx(histype, idx);
    যদি (i < 0)
	ঘূৰাই FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    যদি (histype == HIST_অনুসন্ধান && maptick == last_maptick && i == idx)
	last_maptick = -1;

    যেতিয়ালৈ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    যদি (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ঘূৰাই TRUE;
}

/*
 * "histadd()" function
 */
    শূন্য
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণাঙ্ক		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_সংখ্যা = FALSE;
    যদি (check_secure())
	ঘূৰাই;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != রিক্ত ? get_histtype(str) : -1;
    যদি (histype < 0)
	ঘূৰাই;

    str = tv_get_string_buf(&argvars[1], buf);
    যদি (*str == NUL)
	ঘূৰাই;

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
    পূর্ণাঙ্ক		n;
    char_u	*str;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_সংখ্যা_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    যদি (str == রিক্ত)
	n = 0;
    অন্যথা যদি (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    অন্যথা যদি (argvars[1].v_type == VAR_সংখ্যা)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]));
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
	ঘূৰাই;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    যদি (str == রিক্ত)
	rettv->vval.v_string = রিক্ত;
    অন্যথা
    {
	পূর্ণাঙ্ক type;
	পূর্ণাঙ্ক idx;

	type = get_histtype(str);
	যদি (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	অন্যথা
	    idx = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[1], রিক্ত);
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
    পূর্ণাঙ্ক		i;
    char_u	*histname;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == রিক্ত ? HIST_CMD - 1 : get_histtype(histname);
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
    পূর্ণাঙ্ক		i;

    i = hisidx[HIST_CMD];
    যদি (i < 0)
	ঘূৰাই;
    p_start = history[HIST_CMD][i].hisstr;
    যদি (p_start == রিক্ত)
	ঘূৰাই;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    বাবে (p = p_start; *p; ++p)
    {
	যদি (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    যদি (p == রিক্ত)
		ভঙ্গ;
	    ++p;
	    বাবে (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
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
    পূর্ণাঙ্ক		histype1 = HIST_CMD;
    পূর্ণাঙ্ক		histype2 = HIST_CMD;
    পূর্ণাঙ্ক		hisidx1 = 1;
    পূর্ণাঙ্ক		hisidx2 = -1;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    যদি (hislen == 0)
    {
	বাৰ্তা(_("'history' option is zero"));
	ঘূৰাই;
    }

    যদি (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	যেতিয়ালৈ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != রিক্ত)
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
		ঘূৰাই;
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
	ঘূৰাই;
    }

    বাবে (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((ৱর্ণ *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	বাৰ্তা_puts_title((ৱর্ণ *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	যদি (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	যদি (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	যদি (idx >= 0 && j <= k)
	    বাবে (i = idx + 1; !got_int; ++i)
	    {
		যদি (i == hislen)
		    i = 0;
		যদি (hist[i].hisstr != রিক্ত
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    পূর্ণাঙ্ক  len;

		    বাৰ্তা_putchar('\n');
		    len = vim_snprintf((ৱর্ণ *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    যদি (vim_strsize(hist[i].hisstr) > (পূর্ণাঙ্ক)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (পূর্ণাঙ্ক)Columns - 10, IOSIZE - (পূর্ণাঙ্ক)len);
		    অন্যথা
			STRCPY(IObuff + len, hist[i].hisstr);
		    বাৰ্তা_outtrans(IObuff);
		    out_flush();
		}
		যদি (i == idx)
		    ভঙ্গ;
	    }
    }
}
