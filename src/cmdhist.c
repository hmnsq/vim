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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം histentry_T *(history[HIST_എണ്ണം]) = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
സ്ഥിതം പൂർണ്ണസംഖ്യ	hisidx[HIST_എണ്ണം] = {-1, -1, -1, -1, -1};  // lastused entry
സ്ഥിതം പൂർണ്ണസംഖ്യ	hisnum[HIST_എണ്ണം] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
സ്ഥിതം പൂർണ്ണസംഖ്യ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    പൂർണ്ണസംഖ്യ
get_hislen(ശൂന്യം)
{
    തിരികെ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(പൂർണ്ണസംഖ്യ hist_type)
{
    തിരികെ history[hist_type];
}

#എങ്കിൽ defined(FEAT_VIMINFO)
    ശൂന്യം
set_histentry(പൂർണ്ണസംഖ്യ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    പൂർണ്ണസംഖ്യ *
get_hisidx(പൂർണ്ണസംഖ്യ hist_type)
{
    തിരികെ &hisidx[hist_type];
}

#എങ്കിൽ defined(FEAT_VIMINFO)
    പൂർണ്ണസംഖ്യ *
get_hisnum(പൂർണ്ണസംഖ്യ hist_type)
{
    തിരികെ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    പൂർണ്ണസംഖ്യ
hist_char2type(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c == ':')
	തിരികെ HIST_CMD;
    എങ്കിൽ (c == '=')
	തിരികെ HIST_വ്യഞ്ജകം;
    എങ്കിൽ (c == '@')
	തിരികെ HIST_നിവേശം;
    എങ്കിൽ (c == '>')
	തിരികെ HIST_DEBUG;
    തിരികെ HIST_തിരയൽ;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

സ്ഥിതം അക്ഷരം *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ശൂന്യ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    അക്ഷരം    *short_names = ":=@>?/";
    പൂർണ്ണസംഖ്യ	    short_names_എണ്ണം = (പൂർണ്ണസംഖ്യ)STRLEN(short_names);
    പൂർണ്ണസംഖ്യ	    history_name_എണ്ണം = ARRAY_LENGTH(history_names) - 1;

    എങ്കിൽ (idx < short_names_എണ്ണം)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	തിരികെ xp->xp_buf;
    }
    എങ്കിൽ (idx < short_names_എണ്ണം + history_name_എണ്ണം)
	തിരികെ (char_u *)history_names[idx - short_names_എണ്ണം];
    എങ്കിൽ (idx == short_names_എണ്ണം + history_name_എണ്ണം)
	തിരികെ (char_u *)"all";
    തിരികെ ശൂന്യ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    ശൂന്യം
init_history(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		newlen;	    // new length of history table
    histentry_T	*temp;
    പൂർണ്ണസംഖ്യ		type;

    // If size of history table changed, reallocate it
    newlen = (പൂർണ്ണസംഖ്യ)p_hi;
    എങ്കിൽ (newlen == hislen)		// history length didn't change
	തിരികെ;

    // history length changed
    വേണ്ടി (type = 0; type < HIST_എണ്ണം; ++type)   // adjust the tables
    {
	എങ്കിൽ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    എങ്കിൽ (temp == ശൂന്യ)   // out of memory!
	    {
		എങ്കിൽ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    നിർത്തുക;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		തുടരുക;
	    }
	}
	അല്ലെങ്കിൽ
	    temp = ശൂന്യ;

	എങ്കിൽ (hisidx[type] < 0)		// there are no entries yet
	{
	    പൂർണ്ണസംഖ്യ	i;

	    വേണ്ടി (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (newlen > hislen)	// array becomes bigger
	{
	    പൂർണ്ണസംഖ്യ	i;
	    പൂർണ്ണസംഖ്യ	j;

	    വേണ്ടി (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    വേണ്ടി ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    വേണ്ടി ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	അല്ലെങ്കിൽ				// array becomes smaller or 0
	{
	    പൂർണ്ണസംഖ്യ	i;
	    പൂർണ്ണസംഖ്യ	j;

	    j = hisidx[type];
	    വേണ്ടി (i = newlen - 1; ; --i)
	    {
		എങ്കിൽ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		അല്ലെങ്കിൽ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		എങ്കിൽ (--j < 0)
		    j = hislen - 1;
		എങ്കിൽ (j == hisidx[type])
		    നിർത്തുക;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    ശൂന്യം
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ശൂന്യ;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    പൂർണ്ണസംഖ്യ
in_history(
    പൂർണ്ണസംഖ്യ	    type,
    char_u  *str,
    പൂർണ്ണസംഖ്യ	    move_to_front,	// Move the entry to the front if it exists
    പൂർണ്ണസംഖ്യ	    sep,
    പൂർണ്ണസംഖ്യ	    writing)		// ignore entries read from viminfo
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    last_i = -1;
    char_u  *p;
    size_t  len;

    എങ്കിൽ (hisidx[type] < 0)
	തിരികെ FALSE;
    i = hisidx[type];
    ചെയ്യുക
    {
	എങ്കിൽ (history[type][i].hisstr == ശൂന്യ)
	    തിരികെ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	എങ്കിൽ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_തിരയൽ || sep == p[history[type][i].hisstrlen + 1]))
	{
	    എങ്കിൽ (!move_to_front)
		തിരികെ TRUE;
	    last_i = i;
	    നിർത്തുക;
	}
	എങ്കിൽ (--i < 0)
	    i = hislen - 1;
    } ആകുമ്പോൾ (i != hisidx[type]);

    എങ്കിൽ (last_i < 0)
	തിരികെ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ആകുമ്പോൾ (i != hisidx[type])
    {
	എങ്കിൽ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    തിരികെ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_histtype(char_u *name)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len = (പൂർണ്ണസംഖ്യ)STRLEN(name);

    // No argument: use current history.
    എങ്കിൽ (len == 0)
	തിരികെ hist_char2type(get_cmdline_firstc());

    വേണ്ടി (i = 0; history_names[i] != ശൂന്യ; ++i)
	എങ്കിൽ (STRNICMP(name, history_names[i], len) == 0)
	    തിരികെ i;

    എങ്കിൽ (vim_strchr((char_u *)":=@>?/", name[0]) != ശൂന്യ && name[1] == NUL)
	തിരികെ hist_char2type(name[0]);

    തിരികെ -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    ശൂന്യം
add_to_history(
    പൂർണ്ണസംഖ്യ		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    പൂർണ്ണസംഖ്യ		in_map,		// consider maptick when inside a mapping
    പൂർണ്ണസംഖ്യ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    എങ്കിൽ (hislen == 0)		// no history
	തിരികെ;

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_തിരയൽ)
	തിരികെ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    എങ്കിൽ (histype == HIST_തിരയൽ && in_map)
    {
	എങ്കിൽ (maptick == last_maptick && hisidx[HIST_തിരയൽ] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_തിരയൽ][hisidx[HIST_തിരയൽ]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    എങ്കിൽ (--hisidx[HIST_തിരയൽ] < 0)
		hisidx[HIST_തിരയൽ] = hislen - 1;
	}
	last_maptick = -1;
    }

    എങ്കിൽ (in_history(histype, new_entry, TRUE, sep, FALSE))
	തിരികെ;

    എങ്കിൽ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    എങ്കിൽ (hisptr->hisstr == ശൂന്യ)
	hisptr->hisstrlen = 0;
    അല്ലെങ്കിൽ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    എങ്കിൽ (histype == HIST_തിരയൽ && in_map)
	last_maptick = maptick;
}

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_history_idx(പൂർണ്ണസംഖ്യ histype)
{
    എങ്കിൽ (hislen == 0 || histype < 0 || histype >= HIST_എണ്ണം
		    || hisidx[histype] < 0)
	തിരികെ -1;

    തിരികെ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
calc_hist_idx(പൂർണ്ണസംഖ്യ histype, പൂർണ്ണസംഖ്യ അക്കം)
{
    പൂർണ്ണസംഖ്യ		i;
    histentry_T	*hist;
    പൂർണ്ണസംഖ്യ		wrapped = FALSE;

    എങ്കിൽ (hislen == 0 || histype < 0 || histype >= HIST_എണ്ണം
		    || (i = hisidx[histype]) < 0 || അക്കം == 0)
	തിരികെ -1;

    hist = history[histype];
    എങ്കിൽ (അക്കം > 0)
    {
	ആകുമ്പോൾ (hist[i].hisnum > അക്കം)
	    എങ്കിൽ (--i < 0)
	    {
		എങ്കിൽ (wrapped)
		    നിർത്തുക;
		i += hislen;
		wrapped = TRUE;
	    }
	എങ്കിൽ (i >= 0 && hist[i].hisnum == അക്കം && hist[i].hisstr != ശൂന്യ)
	    തിരികെ i;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (-അക്കം <= hislen)
    {
	i += അക്കം + 1;
	എങ്കിൽ (i < 0)
	    i += hislen;
	എങ്കിൽ (hist[i].hisstr != ശൂന്യ)
	    തിരികെ i;
    }
    തിരികെ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clr_history(പൂർണ്ണസംഖ്യ histype)
{
    പൂർണ്ണസംഖ്യ		i;
    histentry_T	*hisptr;

    എങ്കിൽ (hislen != 0 && histype >= 0 && histype < HIST_എണ്ണം)
    {
	hisptr = history[histype];
	വേണ്ടി (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	തിരികെ OK;
    }
    തിരികെ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
del_history_entry(പൂർണ്ണസംഖ്യ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		last;
    പൂർണ്ണസംഖ്യ		found = FALSE;

    എങ്കിൽ (hislen == 0 || histype < 0 || histype >= HIST_എണ്ണം || *str == NUL
		|| hisidx[histype] < 0)
	തിരികെ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	തിരികെ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    ചെയ്യുക
    {
	hisptr = &history[histype][i];
	എങ്കിൽ (hisptr->hisstr == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    എങ്കിൽ (--last < 0)
		last += hislen;
	}
	എങ്കിൽ (--i < 0)
	    i += hislen;
    } ആകുമ്പോൾ (i != idx);

    എങ്കിൽ (history[histype][idx].hisstr == ശൂന്യ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    തിരികെ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
del_history_idx(പൂർണ്ണസംഖ്യ histype, പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ	    i, j;

    i = calc_hist_idx(histype, idx);
    എങ്കിൽ (i < 0)
	തിരികെ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    എങ്കിൽ (histype == HIST_തിരയൽ && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ആകുമ്പോൾ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    എങ്കിൽ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    തിരികെ TRUE;
}

/*
 * "histadd()" function
 */
    ശൂന്യം
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_സംഖ്യ = FALSE;
    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ശൂന്യ ? get_histtype(str) : -1;
    എങ്കിൽ (histype < 0)
	തിരികെ;

    str = tv_get_string_buf(&argvars[1], buf);
    എങ്കിൽ (*str == NUL)
	തിരികെ;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_സംഖ്യ = TRUE;
}

/*
 * "histdel()" function
 */
    ശൂന്യം
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    പൂർണ്ണസംഖ്യ		n;
    char_u	*str;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    എങ്കിൽ (str == ശൂന്യ)
	n = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_സംഖ്യ)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]));
    അല്ലെങ്കിൽ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_സംഖ്യ = n;
}

/*
 * "histget()" function
 */
    ശൂന്യം
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    എങ്കിൽ (str == ശൂന്യ)
	rettv->vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ type;
	പൂർണ്ണസംഖ്യ idx;

	type = get_histtype(str);
	എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	അല്ലെങ്കിൽ
	    idx = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[1], ശൂന്യ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	എങ്കിൽ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    ശൂന്യം
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*histname;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ശൂന്യ ? HIST_CMD - 1 : get_histtype(histname);
    എങ്കിൽ (i >= HIST_CMD && i < HIST_എണ്ണം)
	rettv->vval.v_സംഖ്യ = get_history_idx(i);
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = -1;
}
#endif // FEAT_EVAL

#എങ്കിൽ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    ശൂന്യം
remove_താക്കോൽ_from_history(ശൂന്യം)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;

    i = hisidx[HIST_CMD];
    എങ്കിൽ (i < 0)
	തിരികെ;
    p_start = history[HIST_CMD][i].hisstr;
    എങ്കിൽ (p_start == ശൂന്യ)
	തിരികെ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    വേണ്ടി (p = p_start; *p; ++p)
    {
	എങ്കിൽ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    എങ്കിൽ (p == ശൂന്യ)
		നിർത്തുക;
	    ++p;
	    വേണ്ടി (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		എങ്കിൽ (p[i] == '\\' && p[i + 1])
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
    ശൂന്യം
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    പൂർണ്ണസംഖ്യ		histype1 = HIST_CMD;
    പൂർണ്ണസംഖ്യ		histype2 = HIST_CMD;
    പൂർണ്ണസംഖ്യ		hisidx1 = 1;
    പൂർണ്ണസംഖ്യ		hisidx2 = -1;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    എങ്കിൽ (hislen == 0)
    {
	സന്ദേശം(_("'history' option is zero"));
	തിരികെ;
    }

    എങ്കിൽ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ആകുമ്പോൾ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != ശൂന്യ)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	എങ്കിൽ (histype1 == -1)
	{
	    എങ്കിൽ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_എണ്ണം-1;
	    }
	    അല്ലെങ്കിൽ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		തിരികെ;
	    }
	}
	അല്ലെങ്കിൽ
	    histype2 = histype1;
	*end = i;
    }
    അല്ലെങ്കിൽ
	end = arg;
    എങ്കിൽ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	എങ്കിൽ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	അല്ലെങ്കിൽ
	    semsg(_(e_val_too_large), arg);
	തിരികെ;
    }

    വേണ്ടി (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	സന്ദേശം_puts_title((അക്ഷരം *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	എങ്കിൽ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	എങ്കിൽ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	എങ്കിൽ (idx >= 0 && j <= k)
	    വേണ്ടി (i = idx + 1; !got_int; ++i)
	    {
		എങ്കിൽ (i == hislen)
		    i = 0;
		എങ്കിൽ (hist[i].hisstr != ശൂന്യ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    പൂർണ്ണസംഖ്യ  len;

		    സന്ദേശം_putchar('\n');
		    len = vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    എങ്കിൽ (vim_strsize(hist[i].hisstr) > (പൂർണ്ണസംഖ്യ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (പൂർണ്ണസംഖ്യ)Columns - 10, IOSIZE - (പൂർണ്ണസംഖ്യ)len);
		    അല്ലെങ്കിൽ
			STRCPY(IObuff + len, hist[i].hisstr);
		    സന്ദേശം_outtrans(IObuff);
		    out_flush();
		}
		എങ്കിൽ (i == idx)
		    നിർത്തുക;
	    }
    }
}
