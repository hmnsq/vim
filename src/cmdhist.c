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

स्थिर histentry_T *(history[HIST_गणनम्]) = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर पूर्णाङ्क	hisidx[HIST_गणनम्] = {-1, -1, -1, -1, -1};  // lastused entry
स्थिर पूर्णाङ्क	hisnum[HIST_गणनम्] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
स्थिर पूर्णाङ्क	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    पूर्णाङ्क
get_hislen(शून्य)
{
    पटिनिवत्त hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(पूर्णाङ्क hist_type)
{
    पटिनिवत्त history[hist_type];
}

#सचे defined(FEAT_VIMINFO)
    शून्य
set_histentry(पूर्णाङ्क hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    पूर्णाङ्क *
get_hisidx(पूर्णाङ्क hist_type)
{
    पटिनिवत्त &hisidx[hist_type];
}

#सचे defined(FEAT_VIMINFO)
    पूर्णाङ्क *
get_hisnum(पूर्णाङ्क hist_type)
{
    पटिनिवत्त &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    पूर्णाङ्क
hist_char2type(पूर्णाङ्क c)
{
    सचे (c == ':')
	पटिनिवत्त HIST_CMD;
    सचे (c == '=')
	पटिनिवत्त HIST_व्यञ्जकम्;
    सचे (c == '@')
	पटिनिवत्त HIST_निवेशः;
    सचे (c == '>')
	पटिनिवत्त HIST_DEBUG;
    पटिनिवत्त HIST_अन्वेषणम्;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

स्थिर वर्ण *(history_names[]) =
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
get_history_arg(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    वर्ण    *short_names = ":=@>?/";
    पूर्णाङ्क	    short_names_गणनम् = (पूर्णाङ्क)STRLEN(short_names);
    पूर्णाङ्क	    history_name_गणनम् = ARRAY_LENGTH(history_names) - 1;

    सचे (idx < short_names_गणनम्)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	पटिनिवत्त xp->xp_buf;
    }
    सचे (idx < short_names_गणनम् + history_name_गणनम्)
	पटिनिवत्त (char_u *)history_names[idx - short_names_गणनम्];
    सचे (idx == short_names_गणनम् + history_name_गणनम्)
	पटिनिवत्त (char_u *)"all";
    पटिनिवत्त रिक्त;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    शून्य
init_history(शून्य)
{
    पूर्णाङ्क		newlen;	    // new length of history table
    histentry_T	*temp;
    पूर्णाङ्क		type;

    // If size of history table changed, reallocate it
    newlen = (पूर्णाङ्क)p_hi;
    सचे (newlen == hislen)		// history length didn't change
	पटिनिवत्त;

    // history length changed
    अत्थाय (type = 0; type < HIST_गणनम्; ++type)   // adjust the tables
    {
	सचे (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    सचे (temp == रिक्त)   // out of memory!
	    {
		सचे (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    भङ्ग;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		अनुवर्त;
	    }
	}
	अञ्ञथा
	    temp = रिक्त;

	सचे (hisidx[type] < 0)		// there are no entries yet
	{
	    पूर्णाङ्क	i;

	    अत्थाय (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	अञ्ञथा सचे (newlen > hislen)	// array becomes bigger
	{
	    पूर्णाङ्क	i;
	    पूर्णाङ्क	j;

	    अत्थाय (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    अत्थाय ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    अत्थाय ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	अञ्ञथा				// array becomes smaller or 0
	{
	    पूर्णाङ्क	i;
	    पूर्णाङ्क	j;

	    j = hisidx[type];
	    अत्थाय (i = newlen - 1; ; --i)
	    {
		सचे (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		अञ्ञथा			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		सचे (--j < 0)
		    j = hislen - 1;
		सचे (j == hisidx[type])
		    भङ्ग;
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
    पूर्णाङ्क
in_history(
    पूर्णाङ्क	    type,
    char_u  *str,
    पूर्णाङ्क	    move_to_front,	// Move the entry to the front if it exists
    पूर्णाङ्क	    sep,
    पूर्णाङ्क	    writing)		// ignore entries read from viminfo
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    last_i = -1;
    char_u  *p;
    size_t  len;

    सचे (hisidx[type] < 0)
	पटिनिवत्त FALSE;
    i = hisidx[type];
    करोहि
    {
	सचे (history[type][i].hisstr == रिक्त)
	    पटिनिवत्त FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	सचे (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_अन्वेषणम् || sep == p[history[type][i].hisstrlen + 1]))
	{
	    सचे (!move_to_front)
		पटिनिवत्त TRUE;
	    last_i = i;
	    भङ्ग;
	}
	सचे (--i < 0)
	    i = hislen - 1;
    } याव (i != hisidx[type]);

    सचे (last_i < 0)
	पटिनिवत्त FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    याव (i != hisidx[type])
    {
	सचे (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    पटिनिवत्त TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    स्थिर पूर्णाङ्क
get_histtype(char_u *name)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		len = (पूर्णाङ्क)STRLEN(name);

    // No argument: use current history.
    सचे (len == 0)
	पटिनिवत्त hist_char2type(get_cmdline_firstc());

    अत्थाय (i = 0; history_names[i] != रिक्त; ++i)
	सचे (STRNICMP(name, history_names[i], len) == 0)
	    पटिनिवत्त i;

    सचे (vim_strchr((char_u *)":=@>?/", name[0]) != रिक्त && name[1] == NUL)
	पटिनिवत्त hist_char2type(name[0]);

    पटिनिवत्त -1;
}

स्थिर पूर्णाङ्क	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    शून्य
add_to_history(
    पूर्णाङ्क		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    पूर्णाङ्क		in_map,		// consider maptick when inside a mapping
    पूर्णाङ्क		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    सचे (hislen == 0)		// no history
	पटिनिवत्त;

    सचे ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_अन्वेषणम्)
	पटिनिवत्त;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    सचे (histype == HIST_अन्वेषणम् && in_map)
    {
	सचे (maptick == last_maptick && hisidx[HIST_अन्वेषणम्] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_अन्वेषणम्][hisidx[HIST_अन्वेषणम्]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    सचे (--hisidx[HIST_अन्वेषणम्] < 0)
		hisidx[HIST_अन्वेषणम्] = hislen - 1;
	}
	last_maptick = -1;
    }

    सचे (in_history(histype, new_entry, TRUE, sep, FALSE))
	पटिनिवत्त;

    सचे (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    सचे (hisptr->hisstr == रिक्त)
	hisptr->hisstrlen = 0;
    अञ्ञथा
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    सचे (histype == HIST_अन्वेषणम् && in_map)
	last_maptick = maptick;
}

#सचे defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
get_history_idx(पूर्णाङ्क histype)
{
    सचे (hislen == 0 || histype < 0 || histype >= HIST_गणनम्
		    || hisidx[histype] < 0)
	पटिनिवत्त -1;

    पटिनिवत्त history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
calc_hist_idx(पूर्णाङ्क histype, पूर्णाङ्क अङ्कः)
{
    पूर्णाङ्क		i;
    histentry_T	*hist;
    पूर्णाङ्क		wrapped = FALSE;

    सचे (hislen == 0 || histype < 0 || histype >= HIST_गणनम्
		    || (i = hisidx[histype]) < 0 || अङ्कः == 0)
	पटिनिवत्त -1;

    hist = history[histype];
    सचे (अङ्कः > 0)
    {
	याव (hist[i].hisnum > अङ्कः)
	    सचे (--i < 0)
	    {
		सचे (wrapped)
		    भङ्ग;
		i += hislen;
		wrapped = TRUE;
	    }
	सचे (i >= 0 && hist[i].hisnum == अङ्कः && hist[i].hisstr != रिक्त)
	    पटिनिवत्त i;
    }
    अञ्ञथा सचे (-अङ्कः <= hislen)
    {
	i += अङ्कः + 1;
	सचे (i < 0)
	    i += hislen;
	सचे (hist[i].hisstr != रिक्त)
	    पटिनिवत्त i;
    }
    पटिनिवत्त -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
clr_history(पूर्णाङ्क histype)
{
    पूर्णाङ्क		i;
    histentry_T	*hisptr;

    सचे (hislen != 0 && histype >= 0 && histype < HIST_गणनम्)
    {
	hisptr = history[histype];
	अत्थाय (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	पटिनिवत्त OK;
    }
    पटिनिवत्त FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
del_history_entry(पूर्णाङ्क histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		i;
    पूर्णाङ्क		last;
    पूर्णाङ्क		found = FALSE;

    सचे (hislen == 0 || histype < 0 || histype >= HIST_गणनम् || *str == NUL
		|| hisidx[histype] < 0)
	पटिनिवत्त FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    सचे (regmatch.regprog == रिक्त)
	पटिनिवत्त FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    करोहि
    {
	hisptr = &history[histype][i];
	सचे (hisptr->hisstr == रिक्त)
	    भङ्ग;
	सचे (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	अञ्ञथा
	{
	    सचे (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    सचे (--last < 0)
		last += hislen;
	}
	सचे (--i < 0)
	    i += hislen;
    } याव (i != idx);

    सचे (history[histype][idx].hisstr == रिक्त)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    पटिनिवत्त found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
del_history_idx(पूर्णाङ्क histype, पूर्णाङ्क idx)
{
    पूर्णाङ्क	    i, j;

    i = calc_hist_idx(histype, idx);
    सचे (i < 0)
	पटिनिवत्त FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    सचे (histype == HIST_अन्वेषणम् && maptick == last_maptick && i == idx)
	last_maptick = -1;

    याव (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    सचे (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    पटिनिवत्त TRUE;
}

/*
 * "histadd()" function
 */
    शून्य
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_सङ्ख्या = FALSE;
    सचे (check_secure())
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != रिक्त ? get_histtype(str) : -1;
    सचे (histype < 0)
	पटिनिवत्त;

    str = tv_get_string_buf(&argvars[1], buf);
    सचे (*str == NUL)
	पटिनिवत्त;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_सङ्ख्या = TRUE;
}

/*
 * "histdel()" function
 */
    शून्य
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    पूर्णाङ्क		n;
    char_u	*str;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    सचे (str == रिक्त)
	n = 0;
    अञ्ञथा सचे (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    अञ्ञथा सचे (argvars[1].v_type == VAR_सङ्ख्या)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]));
    अञ्ञथा
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_सङ्ख्या = n;
}

/*
 * "histget()" function
 */
    शून्य
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    सचे (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    सचे (str == रिक्त)
	rettv->vval.v_string = रिक्त;
    अञ्ञथा
    {
	पूर्णाङ्क type;
	पूर्णाङ्क idx;

	type = get_histtype(str);
	सचे (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	अञ्ञथा
	    idx = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[1], रिक्त);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	सचे (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	अञ्ञथा
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
    पूर्णाङ्क		i;
    char_u	*histname;

    सचे (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == रिक्त ? HIST_CMD - 1 : get_histtype(histname);
    सचे (i >= HIST_CMD && i < HIST_गणनम्)
	rettv->vval.v_सङ्ख्या = get_history_idx(i);
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = -1;
}
#endif // FEAT_EVAL

#सचे defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    शून्य
remove_कुञ्चिका_from_history(शून्य)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    पूर्णाङ्क		i;

    i = hisidx[HIST_CMD];
    सचे (i < 0)
	पटिनिवत्त;
    p_start = history[HIST_CMD][i].hisstr;
    सचे (p_start == रिक्त)
	पटिनिवत्त;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    अत्थाय (p = p_start; *p; ++p)
    {
	सचे (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    सचे (p == रिक्त)
		भङ्ग;
	    ++p;
	    अत्थाय (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		सचे (p[i] == '\\' && p[i + 1])
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
    शून्य
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    पूर्णाङ्क		histype1 = HIST_CMD;
    पूर्णाङ्क		histype2 = HIST_CMD;
    पूर्णाङ्क		hisidx1 = 1;
    पूर्णाङ्क		hisidx2 = -1;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    सचे (hislen == 0)
    {
	सन्देशः(_("'history' option is zero"));
	पटिनिवत्त;
    }

    सचे (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	याव (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != रिक्त)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	सचे (histype1 == -1)
	{
	    सचे (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_गणनम्-1;
	    }
	    अञ्ञथा
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		पटिनिवत्त;
	    }
	}
	अञ्ञथा
	    histype2 = histype1;
	*end = i;
    }
    अञ्ञथा
	end = arg;
    सचे (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	सचे (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	अञ्ञथा
	    semsg(_(e_val_too_large), arg);
	पटिनिवत्त;
    }

    अत्थाय (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	सन्देशः_puts_title((वर्ण *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	सचे (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	सचे (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	सचे (idx >= 0 && j <= k)
	    अत्थाय (i = idx + 1; !got_int; ++i)
	    {
		सचे (i == hislen)
		    i = 0;
		सचे (hist[i].hisstr != रिक्त
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    पूर्णाङ्क  len;

		    सन्देशः_putchar('\n');
		    len = vim_snprintf((वर्ण *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    सचे (vim_strsize(hist[i].hisstr) > (पूर्णाङ्क)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (पूर्णाङ्क)Columns - 10, IOSIZE - (पूर्णाङ्क)len);
		    अञ्ञथा
			STRCPY(IObuff + len, hist[i].hisstr);
		    सन्देशः_outtrans(IObuff);
		    out_flush();
		}
		सचे (i == idx)
		    भङ्ग;
	    }
    }
}
