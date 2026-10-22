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

#अन्तर्भाव "vim.h"

स्थिर histentry_T *(history[HIST_मोजणी]) = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर पूर्णांक	hisidx[HIST_मोजणी] = {-1, -1, -1, -1, -1};  // lastused entry
स्थिर पूर्णांक	hisnum[HIST_मोजणी] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
स्थिर पूर्णांक	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    पूर्णांक
get_hislen(शून्य)
{
    परत hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(पूर्णांक hist_type)
{
    परत history[hist_type];
}

#जर defined(FEAT_VIMINFO)
    शून्य
set_histentry(पूर्णांक hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    पूर्णांक *
get_hisidx(पूर्णांक hist_type)
{
    परत &hisidx[hist_type];
}

#जर defined(FEAT_VIMINFO)
    पूर्णांक *
get_hisnum(पूर्णांक hist_type)
{
    परत &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    पूर्णांक
hist_char2type(पूर्णांक c)
{
    जर (c == ':')
	परत HIST_CMD;
    जर (c == '=')
	परत HIST_अभिव्यक्ती;
    जर (c == '@')
	परत HIST_आदान;
    जर (c == '>')
	परत HIST_DEBUG;
    परत HIST_शोध;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

स्थिर अक्षर *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    रिक्त
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, पूर्णांक idx)
{
    अक्षर    *short_names = ":=@>?/";
    पूर्णांक	    short_names_मोजणी = (पूर्णांक)STRLEN(short_names);
    पूर्णांक	    history_name_मोजणी = ARRAY_LENGTH(history_names) - 1;

    जर (idx < short_names_मोजणी)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	परत xp->xp_buf;
    }
    जर (idx < short_names_मोजणी + history_name_मोजणी)
	परत (char_u *)history_names[idx - short_names_मोजणी];
    जर (idx == short_names_मोजणी + history_name_मोजणी)
	परत (char_u *)"all";
    परत रिक्त;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    शून्य
init_history(शून्य)
{
    पूर्णांक		newlen;	    // new length of history table
    histentry_T	*temp;
    पूर्णांक		type;

    // If size of history table changed, reallocate it
    newlen = (पूर्णांक)p_hi;
    जर (newlen == hislen)		// history length didn't change
	परत;

    // history length changed
    साठी (type = 0; type < HIST_मोजणी; ++type)   // adjust the tables
    {
	जर (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    जर (temp == रिक्त)   // out of memory!
	    {
		जर (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    थांबा;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		सुरूठेवा;
	    }
	}
	अन्यथा
	    temp = रिक्त;

	जर (hisidx[type] < 0)		// there are no entries yet
	{
	    पूर्णांक	i;

	    साठी (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	अन्यथा जर (newlen > hislen)	// array becomes bigger
	{
	    पूर्णांक	i;
	    पूर्णांक	j;

	    साठी (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    साठी ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    साठी ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	अन्यथा				// array becomes smaller or 0
	{
	    पूर्णांक	i;
	    पूर्णांक	j;

	    j = hisidx[type];
	    साठी (i = newlen - 1; ; --i)
	    {
		जर (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		अन्यथा			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		जर (--j < 0)
		    j = hislen - 1;
		जर (j == hisidx[type])
		    थांबा;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    शून्य
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = रिक्त;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    पूर्णांक
in_history(
    पूर्णांक	    type,
    char_u  *str,
    पूर्णांक	    move_to_front,	// Move the entry to the front if it exists
    पूर्णांक	    sep,
    पूर्णांक	    writing)		// ignore entries read from viminfo
{
    पूर्णांक	    i;
    पूर्णांक	    last_i = -1;
    char_u  *p;
    माप_प्रकार  len;

    जर (hisidx[type] < 0)
	परत FALSE;
    i = hisidx[type];
    करा
    {
	जर (history[type][i].hisstr == रिक्त)
	    परत FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	जर (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_शोध || sep == p[history[type][i].hisstrlen + 1]))
	{
	    जर (!move_to_front)
		परत TRUE;
	    last_i = i;
	    थांबा;
	}
	जर (--i < 0)
	    i = hislen - 1;
    } पर्यंत (i != hisidx[type]);

    जर (last_i < 0)
	परत FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    पर्यंत (i != hisidx[type])
    {
	जर (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    परत TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    स्थिर पूर्णांक
get_histtype(char_u *name)
{
    पूर्णांक		i;
    पूर्णांक		len = (पूर्णांक)STRLEN(name);

    // No argument: use current history.
    जर (len == 0)
	परत hist_char2type(get_cmdline_firstc());

    साठी (i = 0; history_names[i] != रिक्त; ++i)
	जर (STRNICMP(name, history_names[i], len) == 0)
	    परत i;

    जर (vim_strchr((char_u *)":=@>?/", name[0]) != रिक्त && name[1] == NUL)
	परत hist_char2type(name[0]);

    परत -1;
}

स्थिर पूर्णांक	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    शून्य
add_to_history(
    पूर्णांक		histype,
    char_u	*new_entry,
    माप_प्रकार	new_entrylen,
    पूर्णांक		in_map,		// consider maptick when inside a mapping
    पूर्णांक		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    जर (hislen == 0)		// no history
	परत;

    जर ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_शोध)
	परत;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    जर (histype == HIST_शोध && in_map)
    {
	जर (maptick == last_maptick && hisidx[HIST_शोध] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_शोध][hisidx[HIST_शोध]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    जर (--hisidx[HIST_शोध] < 0)
		hisidx[HIST_शोध] = hislen - 1;
	}
	last_maptick = -1;
    }

    जर (in_history(histype, new_entry, TRUE, sep, FALSE))
	परत;

    जर (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    जर (hisptr->hisstr == रिक्त)
	hisptr->hisstrlen = 0;
    अन्यथा
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    जर (histype == HIST_शोध && in_map)
	last_maptick = maptick;
}

#जर defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णांक
get_history_idx(पूर्णांक histype)
{
    जर (hislen == 0 || histype < 0 || histype >= HIST_मोजणी
		    || hisidx[histype] < 0)
	परत -1;

    परत history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णांक
calc_hist_idx(पूर्णांक histype, पूर्णांक अंक)
{
    पूर्णांक		i;
    histentry_T	*hist;
    पूर्णांक		wrapped = FALSE;

    जर (hislen == 0 || histype < 0 || histype >= HIST_मोजणी
		    || (i = hisidx[histype]) < 0 || अंक == 0)
	परत -1;

    hist = history[histype];
    जर (अंक > 0)
    {
	पर्यंत (hist[i].hisnum > अंक)
	    जर (--i < 0)
	    {
		जर (wrapped)
		    थांबा;
		i += hislen;
		wrapped = TRUE;
	    }
	जर (i >= 0 && hist[i].hisnum == अंक && hist[i].hisstr != रिक्त)
	    परत i;
    }
    अन्यथा जर (-अंक <= hislen)
    {
	i += अंक + 1;
	जर (i < 0)
	    i += hislen;
	जर (hist[i].hisstr != रिक्त)
	    परत i;
    }
    परत -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णांक
clr_history(पूर्णांक histype)
{
    पूर्णांक		i;
    histentry_T	*hisptr;

    जर (hislen != 0 && histype >= 0 && histype < HIST_मोजणी)
    {
	hisptr = history[histype];
	साठी (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	परत OK;
    }
    परत FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णांक
del_history_entry(पूर्णांक histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    पूर्णांक		idx;
    पूर्णांक		i;
    पूर्णांक		last;
    पूर्णांक		found = FALSE;

    जर (hislen == 0 || histype < 0 || histype >= HIST_मोजणी || *str == NUL
		|| hisidx[histype] < 0)
	परत FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    जर (regmatch.regprog == रिक्त)
	परत FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    करा
    {
	hisptr = &history[histype][i];
	जर (hisptr->hisstr == रिक्त)
	    थांबा;
	जर (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	अन्यथा
	{
	    जर (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    जर (--last < 0)
		last += hislen;
	}
	जर (--i < 0)
	    i += hislen;
    } पर्यंत (i != idx);

    जर (history[histype][idx].hisstr == रिक्त)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    परत found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णांक
del_history_idx(पूर्णांक histype, पूर्णांक idx)
{
    पूर्णांक	    i, j;

    i = calc_hist_idx(histype, idx);
    जर (i < 0)
	परत FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    जर (histype == HIST_शोध && maptick == last_maptick && i == idx)
	last_maptick = -1;

    पर्यंत (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    जर (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    परत TRUE;
}

/*
 * "histadd()" function
 */
    शून्य
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णांक		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_संख्या = FALSE;
    जर (check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != रिक्त ? get_histtype(str) : -1;
    जर (histype < 0)
	परत;

    str = tv_get_string_buf(&argvars[1], buf);
    जर (*str == NUL)
	परत;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_संख्या = TRUE;
}

/*
 * "histdel()" function
 */
    शून्य
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    पूर्णांक		n;
    char_u	*str;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_संख्या_arg(argvars, 1) == FAIL))
	परत;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    जर (str == रिक्त)
	n = 0;
    अन्यथा जर (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    अन्यथा जर (argvars[1].v_type == VAR_संख्या)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (पूर्णांक)tv_get_संख्या(&argvars[1]));
    अन्यथा
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_संख्या = n;
}

/*
 * "histget()" function
 */
    शून्य
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	परत;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    जर (str == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
    {
	पूर्णांक type;
	पूर्णांक idx;

	type = get_histtype(str);
	जर (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	अन्यथा
	    idx = (पूर्णांक)tv_get_संख्या_chk(&argvars[1], रिक्त);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	जर (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	अन्यथा
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    शून्य
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णांक		i;
    char_u	*histname;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == रिक्त ? HIST_CMD - 1 : get_histtype(histname);
    जर (i >= HIST_CMD && i < HIST_मोजणी)
	rettv->vval.v_संख्या = get_history_idx(i);
    अन्यथा
	rettv->vval.v_संख्या = -1;
}
#endif // FEAT_EVAL

#जर defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    शून्य
remove_किल्ली_from_history(शून्य)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    पूर्णांक		i;

    i = hisidx[HIST_CMD];
    जर (i < 0)
	परत;
    p_start = history[HIST_CMD][i].hisstr;
    जर (p_start == रिक्त)
	परत;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    साठी (p = p_start; *p; ++p)
    {
	जर (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    जर (p == रिक्त)
		थांबा;
	    ++p;
	    साठी (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		जर (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_end - (p + i)) + 1);	    // +1 for the NUL
	    p_end -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (माप_प्रकार)(p_end - p_start);
}
#endif

/*
 * :history command - print a history
 */
    शून्य
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    पूर्णांक		histype1 = HIST_CMD;
    पूर्णांक		histype2 = HIST_CMD;
    पूर्णांक		hisidx1 = 1;
    पूर्णांक		hisidx2 = -1;
    पूर्णांक		idx;
    पूर्णांक		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    जर (hislen == 0)
    {
	संदेश(_("'history' option is zero"));
	परत;
    }

    जर (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	पर्यंत (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != रिक्त)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	जर (histype1 == -1)
	{
	    जर (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_मोजणी-1;
	    }
	    अन्यथा
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		परत;
	    }
	}
	अन्यथा
	    histype2 = histype1;
	*end = i;
    }
    अन्यथा
	end = arg;
    जर (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	जर (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	अन्यथा
	    semsg(_(e_val_too_large), arg);
	परत;
    }

    साठी (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((अक्षर *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	संदेश_puts_title((अक्षर *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	जर (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	जर (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	जर (idx >= 0 && j <= k)
	    साठी (i = idx + 1; !got_int; ++i)
	    {
		जर (i == hislen)
		    i = 0;
		जर (hist[i].hisstr != रिक्त
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    पूर्णांक  len;

		    संदेश_putchar('\n');
		    len = vim_snprintf((अक्षर *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    जर (vim_strsize(hist[i].hisstr) > (पूर्णांक)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (पूर्णांक)Columns - 10, IOSIZE - (पूर्णांक)len);
		    अन्यथा
			STRCPY(IObuff + len, hist[i].hisstr);
		    संदेश_outtrans(IObuff);
		    out_flush();
		}
		जर (i == idx)
		    थांबा;
	    }
    }
}
