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

ساكن histentry_T *(history[HIST_عدد]) = {عدم, عدم, عدم, عدم, عدم};
ساكن صحيح	hisidx[HIST_عدد] = {-1, -1, -1, -1, -1};  // lastused entry
ساكن صحيح	hisnum[HIST_عدد] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ساكن صحيح	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    صحيح
get_hislen(فراغ)
{
    أرجع hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(صحيح hist_type)
{
    أرجع history[hist_type];
}

#إذا defined(FEAT_VIMINFO)
    فراغ
set_histentry(صحيح hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    صحيح *
get_hisidx(صحيح hist_type)
{
    أرجع &hisidx[hist_type];
}

#إذا defined(FEAT_VIMINFO)
    صحيح *
get_hisnum(صحيح hist_type)
{
    أرجع &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    صحيح
hist_char2type(صحيح c)
{
    إذا (c == ':')
	أرجع HIST_CMD;
    إذا (c == '=')
	أرجع HIST_تعبير;
    إذا (c == '@')
	أرجع HIST_مدخل;
    إذا (c == '>')
	أرجع HIST_DEBUG;
    أرجع HIST_بحث;	    // must be '?' or '/'
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
    صحيح	    short_names_عدد = (صحيح)STRLEN(short_names);
    صحيح	    history_name_عدد = ARRAY_LENGTH(history_names) - 1;

    إذا (idx < short_names_عدد)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	أرجع xp->xp_buf;
    }
    إذا (idx < short_names_عدد + history_name_عدد)
	أرجع (char_u *)history_names[idx - short_names_عدد];
    إذا (idx == short_names_عدد + history_name_عدد)
	أرجع (char_u *)"all";
    أرجع عدم;
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
    إذا (newlen == hislen)		// history length didn't change
	أرجع;

    // history length changed
    لكل (type = 0; type < HIST_عدد; ++type)   // adjust the tables
    {
	إذا (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    إذا (temp == عدم)   // out of memory!
	    {
		إذا (type == 0)  // first one: just keep the old length
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
	وإلا
	    temp = عدم;

	إذا (hisidx[type] < 0)		// there are no entries yet
	{
	    صحيح	i;

	    لكل (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	وإلا إذا (newlen > hislen)	// array becomes bigger
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
	وإلا				// array becomes smaller or 0
	{
	    صحيح	i;
	    صحيح	j;

	    j = hisidx[type];
	    لكل (i = newlen - 1; ; --i)
	    {
		إذا (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		وإلا			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		إذا (--j < 0)
		    j = hislen - 1;
		إذا (j == hisidx[type])
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

    إذا (hisidx[type] < 0)
	أرجع FALSE;
    i = hisidx[type];
    افعل
    {
	إذا (history[type][i].hisstr == عدم)
	    أرجع FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	إذا (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_بحث || sep == p[history[type][i].hisstrlen + 1]))
	{
	    إذا (!move_to_front)
		أرجع TRUE;
	    last_i = i;
	    اكسر;
	}
	إذا (--i < 0)
	    i = hislen - 1;
    } طالما (i != hisidx[type]);

    إذا (last_i < 0)
	أرجع FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    طالما (i != hisidx[type])
    {
	إذا (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    أرجع TRUE;
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
    إذا (len == 0)
	أرجع hist_char2type(get_cmdline_firstc());

    لكل (i = 0; history_names[i] != عدم; ++i)
	إذا (STRNICMP(name, history_names[i], len) == 0)
	    أرجع i;

    إذا (vim_strchr((char_u *)":=@>?/", name[0]) != عدم && name[1] == NUL)
	أرجع hist_char2type(name[0]);

    أرجع -1;
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

    إذا (hislen == 0)		// no history
	أرجع;

    إذا ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_بحث)
	أرجع;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    إذا (histype == HIST_بحث && in_map)
    {
	إذا (maptick == last_maptick && hisidx[HIST_بحث] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_بحث][hisidx[HIST_بحث]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    إذا (--hisidx[HIST_بحث] < 0)
		hisidx[HIST_بحث] = hislen - 1;
	}
	last_maptick = -1;
    }

    إذا (in_history(histype, new_entry, TRUE, sep, FALSE))
	أرجع;

    إذا (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    إذا (hisptr->hisstr == عدم)
	hisptr->hisstrlen = 0;
    وإلا
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    إذا (histype == HIST_بحث && in_map)
	last_maptick = maptick;
}

#إذا defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
get_history_idx(صحيح histype)
{
    إذا (hislen == 0 || histype < 0 || histype >= HIST_عدد
		    || hisidx[histype] < 0)
	أرجع -1;

    أرجع history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ساكن صحيح
calc_hist_idx(صحيح histype, صحيح رقم)
{
    صحيح		i;
    histentry_T	*hist;
    صحيح		wrapped = FALSE;

    إذا (hislen == 0 || histype < 0 || histype >= HIST_عدد
		    || (i = hisidx[histype]) < 0 || رقم == 0)
	أرجع -1;

    hist = history[histype];
    إذا (رقم > 0)
    {
	طالما (hist[i].hisnum > رقم)
	    إذا (--i < 0)
	    {
		إذا (wrapped)
		    اكسر;
		i += hislen;
		wrapped = TRUE;
	    }
	إذا (i >= 0 && hist[i].hisnum == رقم && hist[i].hisstr != عدم)
	    أرجع i;
    }
    وإلا إذا (-رقم <= hislen)
    {
	i += رقم + 1;
	إذا (i < 0)
	    i += hislen;
	إذا (hist[i].hisstr != عدم)
	    أرجع i;
    }
    أرجع -1;
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

    إذا (hislen != 0 && histype >= 0 && histype < HIST_عدد)
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
	أرجع OK;
    }
    أرجع FAIL;
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

    إذا (hislen == 0 || histype < 0 || histype >= HIST_عدد || *str == NUL
		|| hisidx[histype] < 0)
	أرجع FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    إذا (regmatch.regprog == عدم)
	أرجع FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    افعل
    {
	hisptr = &history[histype][i];
	إذا (hisptr->hisstr == عدم)
	    اكسر;
	إذا (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	وإلا
	{
	    إذا (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    إذا (--last < 0)
		last += hislen;
	}
	إذا (--i < 0)
	    i += hislen;
    } طالما (i != idx);

    إذا (history[histype][idx].hisstr == عدم)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    أرجع found;
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
    إذا (i < 0)
	أرجع FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    إذا (histype == HIST_بحث && maptick == last_maptick && i == idx)
	last_maptick = -1;

    طالما (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    إذا (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    أرجع TRUE;
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

    rettv->vval.v_رقم = FALSE;
    إذا (check_secure())
	أرجع;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != عدم ? get_histtype(str) : -1;
    إذا (histype < 0)
	أرجع;

    str = tv_get_string_buf(&argvars[1], buf);
    إذا (*str == NUL)
	أرجع;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_رقم = TRUE;
}

/*
 * "histdel()" function
 */
    فراغ
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    صحيح		n;
    char_u	*str;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_رقم_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    إذا (str == عدم)
	n = 0;
    وإلا إذا (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    وإلا إذا (argvars[1].v_type == VAR_رقم)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (صحيح)tv_get_رقم(&argvars[1]));
    وإلا
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_رقم = n;
}

/*
 * "histget()" function
 */
    فراغ
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_رقم_arg(argvars, 1) == FAIL))
	أرجع;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    إذا (str == عدم)
	rettv->vval.v_string = عدم;
    وإلا
    {
	صحيح type;
	صحيح idx;

	type = get_histtype(str);
	إذا (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	وإلا
	    idx = (صحيح)tv_get_رقم_chk(&argvars[1], عدم);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	إذا (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	وإلا
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

    إذا (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == عدم ? HIST_CMD - 1 : get_histtype(histname);
    إذا (i >= HIST_CMD && i < HIST_عدد)
	rettv->vval.v_رقم = get_history_idx(i);
    وإلا
	rettv->vval.v_رقم = -1;
}
#endif // FEAT_EVAL

#إذا defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    فراغ
remove_مفتاح_from_history(فراغ)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    صحيح		i;

    i = hisidx[HIST_CMD];
    إذا (i < 0)
	أرجع;
    p_start = history[HIST_CMD][i].hisstr;
    إذا (p_start == عدم)
	أرجع;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    لكل (p = p_start; *p; ++p)
    {
	إذا (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    إذا (p == عدم)
		اكسر;
	    ++p;
	    لكل (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		إذا (p[i] == '\\' && p[i + 1])
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

    إذا (hislen == 0)
    {
	رسالة(_("'history' option is zero"));
	أرجع;
    }

    إذا (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	طالما (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != عدم)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	إذا (histype1 == -1)
	{
	    إذا (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_عدد-1;
	    }
	    وإلا
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		أرجع;
	    }
	}
	وإلا
	    histype2 = histype1;
	*end = i;
    }
    وإلا
	end = arg;
    إذا (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	إذا (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	وإلا
	    semsg(_(e_val_too_large), arg);
	أرجع;
    }

    لكل (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	رسالة_puts_title((حرف *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	إذا (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	إذا (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	إذا (idx >= 0 && j <= k)
	    لكل (i = idx + 1; !got_int; ++i)
	    {
		إذا (i == hislen)
		    i = 0;
		إذا (hist[i].hisstr != عدم
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    صحيح  len;

		    رسالة_putchar('\n');
		    len = vim_snprintf((حرف *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    إذا (vim_strsize(hist[i].hisstr) > (صحيح)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (صحيح)Columns - 10, IOSIZE - (صحيح)len);
		    وإلا
			STRCPY(IObuff + len, hist[i].hisstr);
		    رسالة_outtrans(IObuff);
		    out_flush();
		}
		إذا (i == idx)
		    اكسر;
	    }
    }
}
