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

#અન્તર્ભાવ "vim.h"

સ્થિર histentry_T *(history[HIST_ગણતરી]) = {ખાલી, ખાલી, ખાલી, ખાલી, ખાલી};
સ્થિર પૂર્ણાંક	hisidx[HIST_ગણતરી] = {-1, -1, -1, -1, -1};  // lastused entry
સ્થિર પૂર્ણાંક	hisnum[HIST_ગણતરી] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
સ્થિર પૂર્ણાંક	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    પૂર્ણાંક
get_hislen(શૂન્ય)
{
    પાછા hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(પૂર્ણાંક hist_type)
{
    પાછા history[hist_type];
}

#જો defined(FEAT_VIMINFO)
    શૂન્ય
set_histentry(પૂર્ણાંક hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    પૂર્ણાંક *
get_hisidx(પૂર્ણાંક hist_type)
{
    પાછા &hisidx[hist_type];
}

#જો defined(FEAT_VIMINFO)
    પૂર્ણાંક *
get_hisnum(પૂર્ણાંક hist_type)
{
    પાછા &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    પૂર્ણાંક
hist_char2type(પૂર્ણાંક c)
{
    જો (c == ':')
	પાછા HIST_CMD;
    જો (c == '=')
	પાછા HIST_પદાવલિ;
    જો (c == '@')
	પાછા HIST_આગત;
    જો (c == '>')
	પાછા HIST_DEBUG;
    પાછા HIST_શોધ;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

સ્થિર અક્ષર *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ખાલી
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    અક્ષર    *short_names = ":=@>?/";
    પૂર્ણાંક	    short_names_ગણતરી = (પૂર્ણાંક)STRLEN(short_names);
    પૂર્ણાંક	    history_name_ગણતરી = ARRAY_LENGTH(history_names) - 1;

    જો (idx < short_names_ગણતરી)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	પાછા xp->xp_buf;
    }
    જો (idx < short_names_ગણતરી + history_name_ગણતરી)
	પાછા (char_u *)history_names[idx - short_names_ગણતરી];
    જો (idx == short_names_ગણતરી + history_name_ગણતરી)
	પાછા (char_u *)"all";
    પાછા ખાલી;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    શૂન્ય
init_history(શૂન્ય)
{
    પૂર્ણાંક		newlen;	    // new length of history table
    histentry_T	*temp;
    પૂર્ણાંક		type;

    // If size of history table changed, reallocate it
    newlen = (પૂર્ણાંક)p_hi;
    જો (newlen == hislen)		// history length didn't change
	પાછા;

    // history length changed
    માટે (type = 0; type < HIST_ગણતરી; ++type)   // adjust the tables
    {
	જો (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    જો (temp == ખાલી)   // out of memory!
	    {
		જો (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    તોડો;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ચાલુ;
	    }
	}
	નહીંતર
	    temp = ખાલી;

	જો (hisidx[type] < 0)		// there are no entries yet
	{
	    પૂર્ણાંક	i;

	    માટે (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	નહીંતર જો (newlen > hislen)	// array becomes bigger
	{
	    પૂર્ણાંક	i;
	    પૂર્ણાંક	j;

	    માટે (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    માટે ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    માટે ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	નહીંતર				// array becomes smaller or 0
	{
	    પૂર્ણાંક	i;
	    પૂર્ણાંક	j;

	    j = hisidx[type];
	    માટે (i = newlen - 1; ; --i)
	    {
		જો (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		નહીંતર			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		જો (--j < 0)
		    j = hislen - 1;
		જો (j == hisidx[type])
		    તોડો;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    શૂન્ય
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ખાલી;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    પૂર્ણાંક
in_history(
    પૂર્ણાંક	    type,
    char_u  *str,
    પૂર્ણાંક	    move_to_front,	// Move the entry to the front if it exists
    પૂર્ણાંક	    sep,
    પૂર્ણાંક	    writing)		// ignore entries read from viminfo
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    last_i = -1;
    char_u  *p;
    size_t  len;

    જો (hisidx[type] < 0)
	પાછા FALSE;
    i = hisidx[type];
    કરો
    {
	જો (history[type][i].hisstr == ખાલી)
	    પાછા FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	જો (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_શોધ || sep == p[history[type][i].hisstrlen + 1]))
	{
	    જો (!move_to_front)
		પાછા TRUE;
	    last_i = i;
	    તોડો;
	}
	જો (--i < 0)
	    i = hislen - 1;
    } જ્યારે (i != hisidx[type]);

    જો (last_i < 0)
	પાછા FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    જ્યારે (i != hisidx[type])
    {
	જો (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    પાછા TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    સ્થિર પૂર્ણાંક
get_histtype(char_u *name)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(name);

    // No argument: use current history.
    જો (len == 0)
	પાછા hist_char2type(get_cmdline_firstc());

    માટે (i = 0; history_names[i] != ખાલી; ++i)
	જો (STRNICMP(name, history_names[i], len) == 0)
	    પાછા i;

    જો (vim_strchr((char_u *)":=@>?/", name[0]) != ખાલી && name[1] == NUL)
	પાછા hist_char2type(name[0]);

    પાછા -1;
}

સ્થિર પૂર્ણાંક	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    શૂન્ય
add_to_history(
    પૂર્ણાંક		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    પૂર્ણાંક		in_map,		// consider maptick when inside a mapping
    પૂર્ણાંક		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    જો (hislen == 0)		// no history
	પાછા;

    જો ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_શોધ)
	પાછા;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    જો (histype == HIST_શોધ && in_map)
    {
	જો (maptick == last_maptick && hisidx[HIST_શોધ] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_શોધ][hisidx[HIST_શોધ]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    જો (--hisidx[HIST_શોધ] < 0)
		hisidx[HIST_શોધ] = hislen - 1;
	}
	last_maptick = -1;
    }

    જો (in_history(histype, new_entry, TRUE, sep, FALSE))
	પાછા;

    જો (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    જો (hisptr->hisstr == ખાલી)
	hisptr->hisstrlen = 0;
    નહીંતર
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    જો (histype == HIST_શોધ && in_map)
	last_maptick = maptick;
}

#જો defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    સ્થિર પૂર્ણાંક
get_history_idx(પૂર્ણાંક histype)
{
    જો (hislen == 0 || histype < 0 || histype >= HIST_ગણતરી
		    || hisidx[histype] < 0)
	પાછા -1;

    પાછા history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    સ્થિર પૂર્ણાંક
calc_hist_idx(પૂર્ણાંક histype, પૂર્ણાંક અંક)
{
    પૂર્ણાંક		i;
    histentry_T	*hist;
    પૂર્ણાંક		wrapped = FALSE;

    જો (hislen == 0 || histype < 0 || histype >= HIST_ગણતરી
		    || (i = hisidx[histype]) < 0 || અંક == 0)
	પાછા -1;

    hist = history[histype];
    જો (અંક > 0)
    {
	જ્યારે (hist[i].hisnum > અંક)
	    જો (--i < 0)
	    {
		જો (wrapped)
		    તોડો;
		i += hislen;
		wrapped = TRUE;
	    }
	જો (i >= 0 && hist[i].hisnum == અંક && hist[i].hisstr != ખાલી)
	    પાછા i;
    }
    નહીંતર જો (-અંક <= hislen)
    {
	i += અંક + 1;
	જો (i < 0)
	    i += hislen;
	જો (hist[i].hisstr != ખાલી)
	    પાછા i;
    }
    પાછા -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    સ્થિર પૂર્ણાંક
clr_history(પૂર્ણાંક histype)
{
    પૂર્ણાંક		i;
    histentry_T	*hisptr;

    જો (hislen != 0 && histype >= 0 && histype < HIST_ગણતરી)
    {
	hisptr = history[histype];
	માટે (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	પાછા OK;
    }
    પાછા FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    સ્થિર પૂર્ણાંક
del_history_entry(પૂર્ણાંક histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		i;
    પૂર્ણાંક		last;
    પૂર્ણાંક		found = FALSE;

    જો (hislen == 0 || histype < 0 || histype >= HIST_ગણતરી || *str == NUL
		|| hisidx[histype] < 0)
	પાછા FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    જો (regmatch.regprog == ખાલી)
	પાછા FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    કરો
    {
	hisptr = &history[histype][i];
	જો (hisptr->hisstr == ખાલી)
	    તોડો;
	જો (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	નહીંતર
	{
	    જો (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    જો (--last < 0)
		last += hislen;
	}
	જો (--i < 0)
	    i += hislen;
    } જ્યારે (i != idx);

    જો (history[histype][idx].hisstr == ખાલી)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    પાછા found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    સ્થિર પૂર્ણાંક
del_history_idx(પૂર્ણાંક histype, પૂર્ણાંક idx)
{
    પૂર્ણાંક	    i, j;

    i = calc_hist_idx(histype, idx);
    જો (i < 0)
	પાછા FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    જો (histype == HIST_શોધ && maptick == last_maptick && i == idx)
	last_maptick = -1;

    જ્યારે (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    જો (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    પાછા TRUE;
}

/*
 * "histadd()" function
 */
    શૂન્ય
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_સંખ્યા = FALSE;
    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ખાલી ? get_histtype(str) : -1;
    જો (histype < 0)
	પાછા;

    str = tv_get_string_buf(&argvars[1], buf);
    જો (*str == NUL)
	પાછા;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_સંખ્યા = TRUE;
}

/*
 * "histdel()" function
 */
    શૂન્ય
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    પૂર્ણાંક		n;
    char_u	*str;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    જો (str == ખાલી)
	n = 0;
    નહીંતર જો (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    નહીંતર જો (argvars[1].v_type == VAR_સંખ્યા)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]));
    નહીંતર
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_સંખ્યા = n;
}

/*
 * "histget()" function
 */
    શૂન્ય
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    જો (str == ખાલી)
	rettv->vval.v_string = ખાલી;
    નહીંતર
    {
	પૂર્ણાંક type;
	પૂર્ણાંક idx;

	type = get_histtype(str);
	જો (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	નહીંતર
	    idx = (પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[1], ખાલી);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	જો (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	નહીંતર
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    શૂન્ય
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		i;
    char_u	*histname;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ખાલી ? HIST_CMD - 1 : get_histtype(histname);
    જો (i >= HIST_CMD && i < HIST_ગણતરી)
	rettv->vval.v_સંખ્યા = get_history_idx(i);
    નહીંતર
	rettv->vval.v_સંખ્યા = -1;
}
#endif // FEAT_EVAL

#જો defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    શૂન્ય
remove_ચાવી_from_history(શૂન્ય)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    પૂર્ણાંક		i;

    i = hisidx[HIST_CMD];
    જો (i < 0)
	પાછા;
    p_start = history[HIST_CMD][i].hisstr;
    જો (p_start == ખાલી)
	પાછા;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    માટે (p = p_start; *p; ++p)
    {
	જો (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    જો (p == ખાલી)
		તોડો;
	    ++p;
	    માટે (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		જો (p[i] == '\\' && p[i + 1])
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
    શૂન્ય
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    પૂર્ણાંક		histype1 = HIST_CMD;
    પૂર્ણાંક		histype2 = HIST_CMD;
    પૂર્ણાંક		hisidx1 = 1;
    પૂર્ણાંક		hisidx2 = -1;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    જો (hislen == 0)
    {
	સંદેશ(_("'history' option is zero"));
	પાછા;
    }

    જો (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	જ્યારે (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != ખાલી)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	જો (histype1 == -1)
	{
	    જો (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_ગણતરી-1;
	    }
	    નહીંતર
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		પાછા;
	    }
	}
	નહીંતર
	    histype2 = histype1;
	*end = i;
    }
    નહીંતર
	end = arg;
    જો (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	જો (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	નહીંતર
	    semsg(_(e_val_too_large), arg);
	પાછા;
    }

    માટે (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((અક્ષર *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	સંદેશ_puts_title((અક્ષર *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	જો (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	જો (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	જો (idx >= 0 && j <= k)
	    માટે (i = idx + 1; !got_int; ++i)
	    {
		જો (i == hislen)
		    i = 0;
		જો (hist[i].hisstr != ખાલી
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    પૂર્ણાંક  len;

		    સંદેશ_putchar('\n');
		    len = vim_snprintf((અક્ષર *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    જો (vim_strsize(hist[i].hisstr) > (પૂર્ણાંક)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (પૂર્ણાંક)Columns - 10, IOSIZE - (પૂર્ણાંક)len);
		    નહીંતર
			STRCPY(IObuff + len, hist[i].hisstr);
		    સંદેશ_outtrans(IObuff);
		    out_flush();
		}
		જો (i == idx)
		    તોડો;
	    }
    }
}
