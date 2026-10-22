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

#அந்தர்பாவ "vim.h"

நிலையான histentry_T *(history[HIST_மொத்தம்]) = {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};
நிலையான முழு	hisidx[HIST_மொத்தம்] = {-1, -1, -1, -1, -1};  // lastused entry
நிலையான முழு	hisnum[HIST_மொத்தம்] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
நிலையான முழு	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    முழு
get_hislen(வெறுமை)
{
    திரும்பு hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(முழு hist_type)
{
    திரும்பு history[hist_type];
}

#எனில் defined(FEAT_VIMINFO)
    வெறுமை
set_histentry(முழு hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    முழு *
get_hisidx(முழு hist_type)
{
    திரும்பு &hisidx[hist_type];
}

#எனில் defined(FEAT_VIMINFO)
    முழு *
get_hisnum(முழு hist_type)
{
    திரும்பு &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    முழு
hist_char2type(முழு c)
{
    எனில் (c == ':')
	திரும்பு HIST_CMD;
    எனில் (c == '=')
	திரும்பு HIST_கோவை;
    எனில் (c == '@')
	திரும்பு HIST_உள்ளீடு;
    எனில் (c == '>')
	திரும்பு HIST_DEBUG;
    திரும்பு HIST_தேடல்;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

நிலையான எழுத்து *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    வெற்று
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, முழு idx)
{
    எழுத்து    *short_names = ":=@>?/";
    முழு	    short_names_மொத்தம் = (முழு)STRLEN(short_names);
    முழு	    history_name_மொத்தம் = ARRAY_LENGTH(history_names) - 1;

    எனில் (idx < short_names_மொத்தம்)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	திரும்பு xp->xp_buf;
    }
    எனில் (idx < short_names_மொத்தம் + history_name_மொத்தம்)
	திரும்பு (char_u *)history_names[idx - short_names_மொத்தம்];
    எனில் (idx == short_names_மொத்தம் + history_name_மொத்தம்)
	திரும்பு (char_u *)"all";
    திரும்பு வெற்று;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    வெறுமை
init_history(வெறுமை)
{
    முழு		newlen;	    // new length of history table
    histentry_T	*temp;
    முழு		type;

    // If size of history table changed, reallocate it
    newlen = (முழு)p_hi;
    எனில் (newlen == hislen)		// history length didn't change
	திரும்பு;

    // history length changed
    க்கு (type = 0; type < HIST_மொத்தம்; ++type)   // adjust the tables
    {
	எனில் (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    எனில் (temp == வெற்று)   // out of memory!
	    {
		எனில் (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    நிறுத்து;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		தொடர்;
	    }
	}
	இல்லையெனில்
	    temp = வெற்று;

	எனில் (hisidx[type] < 0)		// there are no entries yet
	{
	    முழு	i;

	    க்கு (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	இல்லையெனில் எனில் (newlen > hislen)	// array becomes bigger
	{
	    முழு	i;
	    முழு	j;

	    க்கு (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    க்கு ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    க்கு ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	இல்லையெனில்				// array becomes smaller or 0
	{
	    முழு	i;
	    முழு	j;

	    j = hisidx[type];
	    க்கு (i = newlen - 1; ; --i)
	    {
		எனில் (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		இல்லையெனில்			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		எனில் (--j < 0)
		    j = hislen - 1;
		எனில் (j == hisidx[type])
		    நிறுத்து;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    வெறுமை
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = வெற்று;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    முழு
in_history(
    முழு	    type,
    char_u  *str,
    முழு	    move_to_front,	// Move the entry to the front if it exists
    முழு	    sep,
    முழு	    writing)		// ignore entries read from viminfo
{
    முழு	    i;
    முழு	    last_i = -1;
    char_u  *p;
    size_t  len;

    எனில் (hisidx[type] < 0)
	திரும்பு FALSE;
    i = hisidx[type];
    செய்
    {
	எனில் (history[type][i].hisstr == வெற்று)
	    திரும்பு FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	எனில் (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_தேடல் || sep == p[history[type][i].hisstrlen + 1]))
	{
	    எனில் (!move_to_front)
		திரும்பு TRUE;
	    last_i = i;
	    நிறுத்து;
	}
	எனில் (--i < 0)
	    i = hislen - 1;
    } போது (i != hisidx[type]);

    எனில் (last_i < 0)
	திரும்பு FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    போது (i != hisidx[type])
    {
	எனில் (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    திரும்பு TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    நிலையான முழு
get_histtype(char_u *name)
{
    முழு		i;
    முழு		len = (முழு)STRLEN(name);

    // No argument: use current history.
    எனில் (len == 0)
	திரும்பு hist_char2type(get_cmdline_firstc());

    க்கு (i = 0; history_names[i] != வெற்று; ++i)
	எனில் (STRNICMP(name, history_names[i], len) == 0)
	    திரும்பு i;

    எனில் (vim_strchr((char_u *)":=@>?/", name[0]) != வெற்று && name[1] == NUL)
	திரும்பு hist_char2type(name[0]);

    திரும்பு -1;
}

நிலையான முழு	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    வெறுமை
add_to_history(
    முழு		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    முழு		in_map,		// consider maptick when inside a mapping
    முழு		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    எனில் (hislen == 0)		// no history
	திரும்பு;

    எனில் ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_தேடல்)
	திரும்பு;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    எனில் (histype == HIST_தேடல் && in_map)
    {
	எனில் (maptick == last_maptick && hisidx[HIST_தேடல்] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_தேடல்][hisidx[HIST_தேடல்]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    எனில் (--hisidx[HIST_தேடல்] < 0)
		hisidx[HIST_தேடல்] = hislen - 1;
	}
	last_maptick = -1;
    }

    எனில் (in_history(histype, new_entry, TRUE, sep, FALSE))
	திரும்பு;

    எனில் (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    எனில் (hisptr->hisstr == வெற்று)
	hisptr->hisstrlen = 0;
    இல்லையெனில்
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    எனில் (histype == HIST_தேடல் && in_map)
	last_maptick = maptick;
}

#எனில் defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    நிலையான முழு
get_history_idx(முழு histype)
{
    எனில் (hislen == 0 || histype < 0 || histype >= HIST_மொத்தம்
		    || hisidx[histype] < 0)
	திரும்பு -1;

    திரும்பு history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    நிலையான முழு
calc_hist_idx(முழு histype, முழு இலக்கம்)
{
    முழு		i;
    histentry_T	*hist;
    முழு		wrapped = FALSE;

    எனில் (hislen == 0 || histype < 0 || histype >= HIST_மொத்தம்
		    || (i = hisidx[histype]) < 0 || இலக்கம் == 0)
	திரும்பு -1;

    hist = history[histype];
    எனில் (இலக்கம் > 0)
    {
	போது (hist[i].hisnum > இலக்கம்)
	    எனில் (--i < 0)
	    {
		எனில் (wrapped)
		    நிறுத்து;
		i += hislen;
		wrapped = TRUE;
	    }
	எனில் (i >= 0 && hist[i].hisnum == இலக்கம் && hist[i].hisstr != வெற்று)
	    திரும்பு i;
    }
    இல்லையெனில் எனில் (-இலக்கம் <= hislen)
    {
	i += இலக்கம் + 1;
	எனில் (i < 0)
	    i += hislen;
	எனில் (hist[i].hisstr != வெற்று)
	    திரும்பு i;
    }
    திரும்பு -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    நிலையான முழு
clr_history(முழு histype)
{
    முழு		i;
    histentry_T	*hisptr;

    எனில் (hislen != 0 && histype >= 0 && histype < HIST_மொத்தம்)
    {
	hisptr = history[histype];
	க்கு (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	திரும்பு OK;
    }
    திரும்பு FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    நிலையான முழு
del_history_entry(முழு histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    முழு		idx;
    முழு		i;
    முழு		last;
    முழு		found = FALSE;

    எனில் (hislen == 0 || histype < 0 || histype >= HIST_மொத்தம் || *str == NUL
		|| hisidx[histype] < 0)
	திரும்பு FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    எனில் (regmatch.regprog == வெற்று)
	திரும்பு FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    செய்
    {
	hisptr = &history[histype][i];
	எனில் (hisptr->hisstr == வெற்று)
	    நிறுத்து;
	எனில் (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	இல்லையெனில்
	{
	    எனில் (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    எனில் (--last < 0)
		last += hislen;
	}
	எனில் (--i < 0)
	    i += hislen;
    } போது (i != idx);

    எனில் (history[histype][idx].hisstr == வெற்று)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    திரும்பு found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    நிலையான முழு
del_history_idx(முழு histype, முழு idx)
{
    முழு	    i, j;

    i = calc_hist_idx(histype, idx);
    எனில் (i < 0)
	திரும்பு FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    எனில் (histype == HIST_தேடல் && maptick == last_maptick && i == idx)
	last_maptick = -1;

    போது (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    எனில் (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    திரும்பு TRUE;
}

/*
 * "histadd()" function
 */
    வெறுமை
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_எண் = FALSE;
    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != வெற்று ? get_histtype(str) : -1;
    எனில் (histype < 0)
	திரும்பு;

    str = tv_get_string_buf(&argvars[1], buf);
    எனில் (*str == NUL)
	திரும்பு;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_எண் = TRUE;
}

/*
 * "histdel()" function
 */
    வெறுமை
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    முழு		n;
    char_u	*str;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    எனில் (str == வெற்று)
	n = 0;
    இல்லையெனில் எனில் (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    இல்லையெனில் எனில் (argvars[1].v_type == VAR_எண்)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (முழு)tv_get_எண்(&argvars[1]));
    இல்லையெனில்
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_எண் = n;
}

/*
 * "histget()" function
 */
    வெறுமை
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    எனில் (str == வெற்று)
	rettv->vval.v_string = வெற்று;
    இல்லையெனில்
    {
	முழு type;
	முழு idx;

	type = get_histtype(str);
	எனில் (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	இல்லையெனில்
	    idx = (முழு)tv_get_எண்_chk(&argvars[1], வெற்று);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	எனில் (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	இல்லையெனில்
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    வெறுமை
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		i;
    char_u	*histname;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == வெற்று ? HIST_CMD - 1 : get_histtype(histname);
    எனில் (i >= HIST_CMD && i < HIST_மொத்தம்)
	rettv->vval.v_எண் = get_history_idx(i);
    இல்லையெனில்
	rettv->vval.v_எண் = -1;
}
#endif // FEAT_EVAL

#எனில் defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    வெறுமை
remove_சாவி_from_history(வெறுமை)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    முழு		i;

    i = hisidx[HIST_CMD];
    எனில் (i < 0)
	திரும்பு;
    p_start = history[HIST_CMD][i].hisstr;
    எனில் (p_start == வெற்று)
	திரும்பு;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    க்கு (p = p_start; *p; ++p)
    {
	எனில் (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    எனில் (p == வெற்று)
		நிறுத்து;
	    ++p;
	    க்கு (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		எனில் (p[i] == '\\' && p[i + 1])
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
    வெறுமை
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    முழு		histype1 = HIST_CMD;
    முழு		histype2 = HIST_CMD;
    முழு		hisidx1 = 1;
    முழு		hisidx2 = -1;
    முழு		idx;
    முழு		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    எனில் (hislen == 0)
    {
	செய்தி(_("'history' option is zero"));
	திரும்பு;
    }

    எனில் (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	போது (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != வெற்று)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	எனில் (histype1 == -1)
	{
	    எனில் (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_மொத்தம்-1;
	    }
	    இல்லையெனில்
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		திரும்பு;
	    }
	}
	இல்லையெனில்
	    histype2 = histype1;
	*end = i;
    }
    இல்லையெனில்
	end = arg;
    எனில் (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	எனில் (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	இல்லையெனில்
	    semsg(_(e_val_too_large), arg);
	திரும்பு;
    }

    க்கு (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	செய்தி_puts_title((எழுத்து *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	எனில் (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	எனில் (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	எனில் (idx >= 0 && j <= k)
	    க்கு (i = idx + 1; !got_int; ++i)
	    {
		எனில் (i == hislen)
		    i = 0;
		எனில் (hist[i].hisstr != வெற்று
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    முழு  len;

		    செய்தி_putchar('\n');
		    len = vim_snprintf((எழுத்து *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    எனில் (vim_strsize(hist[i].hisstr) > (முழு)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (முழு)Columns - 10, IOSIZE - (முழு)len);
		    இல்லையெனில்
			STRCPY(IObuff + len, hist[i].hisstr);
		    செய்தி_outtrans(IObuff);
		    out_flush();
		}
		எனில் (i == idx)
		    நிறுத்து;
	    }
    }
}
