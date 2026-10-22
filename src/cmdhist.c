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

स्थिर histentry_T *(history[HIST_गन्ती]) = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर पूर्णाङ्क	hisidx[HIST_गन्ती] = {-1, -1, -1, -1, -1};  // lastused entry
स्थिर पूर्णाङ्क	hisnum[HIST_गन्ती] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
स्थिर पूर्णाङ्क	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    पूर्णाङ्क
get_hislen(शून्य)
{
    फर्काऊ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(पूर्णाङ्क hist_type)
{
    फर्काऊ history[hist_type];
}

#यदि defined(FEAT_VIMINFO)
    शून्य
set_histentry(पूर्णाङ्क hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    पूर्णाङ्क *
get_hisidx(पूर्णाङ्क hist_type)
{
    फर्काऊ &hisidx[hist_type];
}

#यदि defined(FEAT_VIMINFO)
    पूर्णाङ्क *
get_hisnum(पूर्णाङ्क hist_type)
{
    फर्काऊ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    पूर्णाङ्क
hist_char2type(पूर्णाङ्क c)
{
    यदि (c == ':')
	फर्काऊ HIST_CMD;
    यदि (c == '=')
	फर्काऊ HIST_अभिव्यक्ति;
    यदि (c == '@')
	फर्काऊ HIST_आगत;
    यदि (c == '>')
	फर्काऊ HIST_DEBUG;
    फर्काऊ HIST_खोज;	    // must be '?' or '/'
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
    पूर्णाङ्क	    short_names_गन्ती = (पूर्णाङ्क)STRLEN(short_names);
    पूर्णाङ्क	    history_name_गन्ती = ARRAY_LENGTH(history_names) - 1;

    यदि (idx < short_names_गन्ती)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	फर्काऊ xp->xp_buf;
    }
    यदि (idx < short_names_गन्ती + history_name_गन्ती)
	फर्काऊ (char_u *)history_names[idx - short_names_गन्ती];
    यदि (idx == short_names_गन्ती + history_name_गन्ती)
	फर्काऊ (char_u *)"all";
    फर्काऊ रिक्त;
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
    यदि (newlen == hislen)		// history length didn't change
	फर्काऊ;

    // history length changed
    लागि (type = 0; type < HIST_गन्ती; ++type)   // adjust the tables
    {
	यदि (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    यदि (temp == रिक्त)   // out of memory!
	    {
		यदि (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    रोक;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		जारी;
	    }
	}
	अन्यथा
	    temp = रिक्त;

	यदि (hisidx[type] < 0)		// there are no entries yet
	{
	    पूर्णाङ्क	i;

	    लागि (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	अन्यथा यदि (newlen > hislen)	// array becomes bigger
	{
	    पूर्णाङ्क	i;
	    पूर्णाङ्क	j;

	    लागि (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    लागि ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    लागि ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	अन्यथा				// array becomes smaller or 0
	{
	    पूर्णाङ्क	i;
	    पूर्णाङ्क	j;

	    j = hisidx[type];
	    लागि (i = newlen - 1; ; --i)
	    {
		यदि (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		अन्यथा			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		यदि (--j < 0)
		    j = hislen - 1;
		यदि (j == hisidx[type])
		    रोक;
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
    माप_प्रकार  len;

    यदि (hisidx[type] < 0)
	फर्काऊ FALSE;
    i = hisidx[type];
    गर
    {
	यदि (history[type][i].hisstr == रिक्त)
	    फर्काऊ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	यदि (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_खोज || sep == p[history[type][i].hisstrlen + 1]))
	{
	    यदि (!move_to_front)
		फर्काऊ TRUE;
	    last_i = i;
	    रोक;
	}
	यदि (--i < 0)
	    i = hislen - 1;
    } जबसम्म (i != hisidx[type]);

    यदि (last_i < 0)
	फर्काऊ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    जबसम्म (i != hisidx[type])
    {
	यदि (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    फर्काऊ TRUE;
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
    यदि (len == 0)
	फर्काऊ hist_char2type(get_cmdline_firstc());

    लागि (i = 0; history_names[i] != रिक्त; ++i)
	यदि (STRNICMP(name, history_names[i], len) == 0)
	    फर्काऊ i;

    यदि (vim_strchr((char_u *)":=@>?/", name[0]) != रिक्त && name[1] == NUL)
	फर्काऊ hist_char2type(name[0]);

    फर्काऊ -1;
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
    माप_प्रकार	new_entrylen,
    पूर्णाङ्क		in_map,		// consider maptick when inside a mapping
    पूर्णाङ्क		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    यदि (hislen == 0)		// no history
	फर्काऊ;

    यदि ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_खोज)
	फर्काऊ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    यदि (histype == HIST_खोज && in_map)
    {
	यदि (maptick == last_maptick && hisidx[HIST_खोज] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_खोज][hisidx[HIST_खोज]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    यदि (--hisidx[HIST_खोज] < 0)
		hisidx[HIST_खोज] = hislen - 1;
	}
	last_maptick = -1;
    }

    यदि (in_history(histype, new_entry, TRUE, sep, FALSE))
	फर्काऊ;

    यदि (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    यदि (hisptr->hisstr == रिक्त)
	hisptr->hisstrlen = 0;
    अन्यथा
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    यदि (histype == HIST_खोज && in_map)
	last_maptick = maptick;
}

#यदि defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
get_history_idx(पूर्णाङ्क histype)
{
    यदि (hislen == 0 || histype < 0 || histype >= HIST_गन्ती
		    || hisidx[histype] < 0)
	फर्काऊ -1;

    फर्काऊ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    स्थिर पूर्णाङ्क
calc_hist_idx(पूर्णाङ्क histype, पूर्णाङ्क अङ्क)
{
    पूर्णाङ्क		i;
    histentry_T	*hist;
    पूर्णाङ्क		wrapped = FALSE;

    यदि (hislen == 0 || histype < 0 || histype >= HIST_गन्ती
		    || (i = hisidx[histype]) < 0 || अङ्क == 0)
	फर्काऊ -1;

    hist = history[histype];
    यदि (अङ्क > 0)
    {
	जबसम्म (hist[i].hisnum > अङ्क)
	    यदि (--i < 0)
	    {
		यदि (wrapped)
		    रोक;
		i += hislen;
		wrapped = TRUE;
	    }
	यदि (i >= 0 && hist[i].hisnum == अङ्क && hist[i].hisstr != रिक्त)
	    फर्काऊ i;
    }
    अन्यथा यदि (-अङ्क <= hislen)
    {
	i += अङ्क + 1;
	यदि (i < 0)
	    i += hislen;
	यदि (hist[i].hisstr != रिक्त)
	    फर्काऊ i;
    }
    फर्काऊ -1;
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

    यदि (hislen != 0 && histype >= 0 && histype < HIST_गन्ती)
    {
	hisptr = history[histype];
	लागि (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	फर्काऊ OK;
    }
    फर्काऊ FAIL;
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

    यदि (hislen == 0 || histype < 0 || histype >= HIST_गन्ती || *str == NUL
		|| hisidx[histype] < 0)
	फर्काऊ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    यदि (regmatch.regprog == रिक्त)
	फर्काऊ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    गर
    {
	hisptr = &history[histype][i];
	यदि (hisptr->hisstr == रिक्त)
	    रोक;
	यदि (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	अन्यथा
	{
	    यदि (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    यदि (--last < 0)
		last += hislen;
	}
	यदि (--i < 0)
	    i += hislen;
    } जबसम्म (i != idx);

    यदि (history[histype][idx].hisstr == रिक्त)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    फर्काऊ found;
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
    यदि (i < 0)
	फर्काऊ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    यदि (histype == HIST_खोज && maptick == last_maptick && i == idx)
	last_maptick = -1;

    जबसम्म (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    यदि (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    फर्काऊ TRUE;
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
    यदि (check_secure())
	फर्काऊ;

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	फर्काऊ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != रिक्त ? get_histtype(str) : -1;
    यदि (histype < 0)
	फर्काऊ;

    str = tv_get_string_buf(&argvars[1], buf);
    यदि (*str == NUL)
	फर्काऊ;

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

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    यदि (str == रिक्त)
	n = 0;
    अन्यथा यदि (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    अन्यथा यदि (argvars[1].v_type == VAR_सङ्ख्या)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]));
    अन्यथा
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

    यदि (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    यदि (str == रिक्त)
	rettv->vval.v_string = रिक्त;
    अन्यथा
    {
	पूर्णाङ्क type;
	पूर्णाङ्क idx;

	type = get_histtype(str);
	यदि (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	अन्यथा
	    idx = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[1], रिक्त);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	यदि (idx < 0)
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
    पूर्णाङ्क		i;
    char_u	*histname;

    यदि (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	फर्काऊ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == रिक्त ? HIST_CMD - 1 : get_histtype(histname);
    यदि (i >= HIST_CMD && i < HIST_गन्ती)
	rettv->vval.v_सङ्ख्या = get_history_idx(i);
    अन्यथा
	rettv->vval.v_सङ्ख्या = -1;
}
#endif // FEAT_EVAL

#यदि defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    शून्य
remove_साँचो_from_history(शून्य)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    पूर्णाङ्क		i;

    i = hisidx[HIST_CMD];
    यदि (i < 0)
	फर्काऊ;
    p_start = history[HIST_CMD][i].hisstr;
    यदि (p_start == रिक्त)
	फर्काऊ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    लागि (p = p_start; *p; ++p)
    {
	यदि (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    यदि (p == रिक्त)
		रोक;
	    ++p;
	    लागि (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		यदि (p[i] == '\\' && p[i + 1])
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
    पूर्णाङ्क		histype1 = HIST_CMD;
    पूर्णाङ्क		histype2 = HIST_CMD;
    पूर्णाङ्क		hisidx1 = 1;
    पूर्णाङ्क		hisidx2 = -1;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    यदि (hislen == 0)
    {
	सन्देश(_("'history' option is zero"));
	फर्काऊ;
    }

    यदि (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	जबसम्म (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != रिक्त)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	यदि (histype1 == -1)
	{
	    यदि (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_गन्ती-1;
	    }
	    अन्यथा
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		फर्काऊ;
	    }
	}
	अन्यथा
	    histype2 = histype1;
	*end = i;
    }
    अन्यथा
	end = arg;
    यदि (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	यदि (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	अन्यथा
	    semsg(_(e_val_too_large), arg);
	फर्काऊ;
    }

    लागि (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	सन्देश_puts_title((वर्ण *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	यदि (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	यदि (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	यदि (idx >= 0 && j <= k)
	    लागि (i = idx + 1; !got_int; ++i)
	    {
		यदि (i == hislen)
		    i = 0;
		यदि (hist[i].hisstr != रिक्त
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    पूर्णाङ्क  len;

		    सन्देश_putchar('\n');
		    len = vim_snprintf((वर्ण *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    यदि (vim_strsize(hist[i].hisstr) > (पूर्णाङ्क)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (पूर्णाङ्क)Columns - 10, IOSIZE - (पूर्णाङ्क)len);
		    अन्यथा
			STRCPY(IObuff + len, hist[i].hisstr);
		    सन्देश_outtrans(IObuff);
		    out_flush();
		}
		यदि (i == idx)
		    रोक;
	    }
    }
}
