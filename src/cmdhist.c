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

#include "vim.h"

static histentry_T *(history[HIST_COUNT]) = {NULL, NULL, NULL, NULL, NULL};
static ចំនួនគត់	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static ចំនួនគត់	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static ចំនួនគត់	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ចំនួនគត់
get_hislen(void)
{
    ត្រឡប់ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ចំនួនគត់ hist_type)
{
    ត្រឡប់ history[hist_type];
}

#ប្រសិនបើ defined(FEAT_VIMINFO)
    void
set_histentry(ចំនួនគត់ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ចំនួនគត់ *
get_hisidx(ចំនួនគត់ hist_type)
{
    ត្រឡប់ &hisidx[hist_type];
}

#ប្រសិនបើ defined(FEAT_VIMINFO)
    ចំនួនគត់ *
get_hisnum(ចំនួនគត់ hist_type)
{
    ត្រឡប់ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ចំនួនគត់
hist_char2type(ចំនួនគត់ c)
{
    ប្រសិនបើ (c == ':')
	ត្រឡប់ HIST_CMD;
    ប្រសិនបើ (c == '=')
	ត្រឡប់ HIST_EXPR;
    ប្រសិនបើ (c == '@')
	ត្រឡប់ HIST_INPUT;
    ប្រសិនបើ (c == '>')
	ត្រឡប់ HIST_DEBUG;
    ត្រឡប់ HIST_SEARCH;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

static var_na *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    NULL
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, ចំនួនគត់ idx)
{
    var_na    *short_names = ":=@>?/";
    ចំនួនគត់	    short_names_count = (ចំនួនគត់)STRLEN(short_names);
    ចំនួនគត់	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ប្រសិនបើ (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ត្រឡប់ xp->xp_buf;
    }
    ប្រសិនបើ (idx < short_names_count + history_name_count)
	ត្រឡប់ (char_u *)history_names[idx - short_names_count];
    ប្រសិនបើ (idx == short_names_count + history_name_count)
	ត្រឡប់ (char_u *)"all";
    ត្រឡប់ NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    ចំនួនគត់		newlen;	    // new length of history table
    histentry_T	*temp;
    ចំនួនគត់		type;

    // If size of history table changed, reallocate it
    newlen = (ចំនួនគត់)p_hi;
    ប្រសិនបើ (newlen == hislen)		// history length didn't change
	ត្រឡប់;

    // history length changed
    សម្រាប់ (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ប្រសិនបើ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ប្រសិនបើ (temp == NULL)   // out of memory!
	    {
		ប្រសិនបើ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    bha_nga;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		continue;
	    }
	}
	បើមិនដូច្នេះ
	    temp = NULL;

	ប្រសិនបើ (hisidx[type] < 0)		// there are no entries yet
	{
	    ចំនួនគត់	i;

	    សម្រាប់ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (newlen > hislen)	// array becomes bigger
	{
	    ចំនួនគត់	i;
	    ចំនួនគត់	j;

	    សម្រាប់ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    សម្រាប់ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    សម្រាប់ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	បើមិនដូច្នេះ				// array becomes smaller or 0
	{
	    ចំនួនគត់	i;
	    ចំនួនគត់	j;

	    j = hisidx[type];
	    សម្រាប់ (i = newlen - 1; ; --i)
	    {
		ប្រសិនបើ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		បើមិនដូច្នេះ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ប្រសិនបើ (--j < 0)
		    j = hislen - 1;
		ប្រសិនបើ (j == hisidx[type])
		    bha_nga;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    void
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = NULL;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    ចំនួនគត់
in_history(
    ចំនួនគត់	    type,
    char_u  *str,
    ចំនួនគត់	    move_to_front,	// Move the entry to the front if it exists
    ចំនួនគត់	    sep,
    ចំនួនគត់	    writing)		// ignore entries read from viminfo
{
    ចំនួនគត់	    i;
    ចំនួនគត់	    last_i = -1;
    char_u  *p;
    size_t  len;

    ប្រសិនបើ (hisidx[type] < 0)
	ត្រឡប់ FALSE;
    i = hisidx[type];
    do
    {
	ប្រសិនបើ (history[type][i].hisstr == NULL)
	    ត្រឡប់ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ប្រសិនបើ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ប្រសិនបើ (!move_to_front)
		ត្រឡប់ TRUE;
	    last_i = i;
	    bha_nga;
	}
	ប្រសិនបើ (--i < 0)
	    i = hislen - 1;
    } ខណៈពេល (i != hisidx[type]);

    ប្រសិនបើ (last_i < 0)
	ត្រឡប់ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ខណៈពេល (i != hisidx[type])
    {
	ប្រសិនបើ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ត្រឡប់ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static ចំនួនគត់
get_histtype(char_u *name)
{
    ចំនួនគត់		i;
    ចំនួនគត់		len = (ចំនួនគត់)STRLEN(name);

    // No argument: use current history.
    ប្រសិនបើ (len == 0)
	ត្រឡប់ hist_char2type(get_cmdline_firstc());

    សម្រាប់ (i = 0; history_names[i] != NULL; ++i)
	ប្រសិនបើ (STRNICMP(name, history_names[i], len) == 0)
	    ត្រឡប់ i;

    ប្រសិនបើ (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ត្រឡប់ hist_char2type(name[0]);

    ត្រឡប់ -1;
}

static ចំនួនគត់	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    ចំនួនគត់		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ចំនួនគត់		in_map,		// consider maptick when inside a mapping
    ចំនួនគត់		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ប្រសិនបើ (hislen == 0)		// no history
	ត្រឡប់;

    ប្រសិនបើ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ត្រឡប់;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ប្រសិនបើ (histype == HIST_SEARCH && in_map)
    {
	ប្រសិនបើ (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ប្រសិនបើ (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ប្រសិនបើ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ត្រឡប់;

    ប្រសិនបើ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ប្រសិនបើ (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    បើមិនដូច្នេះ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ប្រសិនបើ (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ប្រសិនបើ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ចំនួនគត់
get_history_idx(ចំនួនគត់ histype)
{
    ប្រសិនបើ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ត្រឡប់ -1;

    ត្រឡប់ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static ចំនួនគត់
calc_hist_idx(ចំនួនគត់ histype, ចំនួនគត់ num)
{
    ចំនួនគត់		i;
    histentry_T	*hist;
    ចំនួនគត់		wrapped = FALSE;

    ប្រសិនបើ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ត្រឡប់ -1;

    hist = history[histype];
    ប្រសិនបើ (num > 0)
    {
	ខណៈពេល (hist[i].hisnum > num)
	    ប្រសិនបើ (--i < 0)
	    {
		ប្រសិនបើ (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	ប្រសិនបើ (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ត្រឡប់ i;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (-num <= hislen)
    {
	i += num + 1;
	ប្រសិនបើ (i < 0)
	    i += hislen;
	ប្រសិនបើ (hist[i].hisstr != NULL)
	    ត្រឡប់ i;
    }
    ត្រឡប់ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static ចំនួនគត់
clr_history(ចំនួនគត់ histype)
{
    ចំនួនគត់		i;
    histentry_T	*hisptr;

    ប្រសិនបើ (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	សម្រាប់ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ត្រឡប់ OK;
    }
    ត្រឡប់ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ចំនួនគត់
del_history_entry(ចំនួនគត់ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ចំនួនគត់		idx;
    ចំនួនគត់		i;
    ចំនួនគត់		last;
    ចំនួនគត់		found = FALSE;

    ប្រសិនបើ (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ត្រឡប់ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ប្រសិនបើ (regmatch.regprog == NULL)
	ត្រឡប់ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	ប្រសិនបើ (hisptr->hisstr == NULL)
	    bha_nga;
	ប្រសិនបើ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ប្រសិនបើ (--last < 0)
		last += hislen;
	}
	ប្រសិនបើ (--i < 0)
	    i += hislen;
    } ខណៈពេល (i != idx);

    ប្រសិនបើ (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ត្រឡប់ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ចំនួនគត់
del_history_idx(ចំនួនគត់ histype, ចំនួនគត់ idx)
{
    ចំនួនគត់	    i, j;

    i = calc_hist_idx(histype, idx);
    ប្រសិនបើ (i < 0)
	ត្រឡប់ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ប្រសិនបើ (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ខណៈពេល (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ប្រសិនបើ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ត្រឡប់ TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ចំនួនគត់		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ប្រសិនបើ (check_secure())
	ត្រឡប់;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    ប្រសិនបើ (histype < 0)
	ត្រឡប់;

    str = tv_get_string_buf(&argvars[1], buf);
    ប្រសិនបើ (*str == NUL)
	ត្រឡប់;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_number = TRUE;
}

/*
 * "histdel()" function
 */
    void
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ចំនួនគត់		n;
    char_u	*str;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ប្រសិនបើ (str == NULL)
	n = 0;
    បើមិនដូច្នេះ ប្រសិនបើ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    បើមិនដូច្នេះ ប្រសិនបើ (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ចំនួនគត់)tv_get_number(&argvars[1]));
    បើមិនដូច្នេះ
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
    void
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    ប្រសិនបើ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ត្រឡប់;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ប្រសិនបើ (str == NULL)
	rettv->vval.v_string = NULL;
    បើមិនដូច្នេះ
    {
	ចំនួនគត់ type;
	ចំនួនគត់ idx;

	type = get_histtype(str);
	ប្រសិនបើ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	បើមិនដូច្នេះ
	    idx = (ចំនួនគត់)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ប្រសិនបើ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	បើមិនដូច្នេះ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    void
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ចំនួនគត់		i;
    char_u	*histname;

    ប្រសិនបើ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    ប្រសិនបើ (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    បើមិនដូច្នេះ
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ប្រសិនបើ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    void
remove_key_from_history(void)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    ចំនួនគត់		i;

    i = hisidx[HIST_CMD];
    ប្រសិនបើ (i < 0)
	ត្រឡប់;
    p_start = history[HIST_CMD][i].hisstr;
    ប្រសិនបើ (p_start == NULL)
	ត្រឡប់;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    សម្រាប់ (p = p_start; *p; ++p)
    {
	ប្រសិនបើ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ប្រសិនបើ (p == NULL)
		bha_nga;
	    ++p;
	    សម្រាប់ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ប្រសិនបើ (p[i] == '\\' && p[i + 1])
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
    void
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    ចំនួនគត់		histype1 = HIST_CMD;
    ចំនួនគត់		histype2 = HIST_CMD;
    ចំនួនគត់		hisidx1 = 1;
    ចំនួនគត់		hisidx2 = -1;
    ចំនួនគត់		idx;
    ចំនួនគត់		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ប្រសិនបើ (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ត្រឡប់;
    }

    ប្រសិនបើ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ខណៈពេល (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ប្រសិនបើ (histype1 == -1)
	{
	    ប្រសិនបើ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    បើមិនដូច្នេះ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ត្រឡប់;
	    }
	}
	បើមិនដូច្នេះ
	    histype2 = histype1;
	*end = i;
    }
    បើមិនដូច្នេះ
	end = arg;
    ប្រសិនបើ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ប្រសិនបើ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	បើមិនដូច្នេះ
	    semsg(_(e_val_too_large), arg);
	ត្រឡប់;
    }

    សម្រាប់ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ប្រសិនបើ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ប្រសិនបើ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ប្រសិនបើ (idx >= 0 && j <= k)
	    សម្រាប់ (i = idx + 1; !got_int; ++i)
	    {
		ប្រសិនបើ (i == hislen)
		    i = 0;
		ប្រសិនបើ (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ចំនួនគត់  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ប្រសិនបើ (vim_strsize(hist[i].hisstr) > (ចំនួនគត់)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ចំនួនគត់)Columns - 10, IOSIZE - (ចំនួនគត់)len);
		    បើមិនដូច្នេះ
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ប្រសិនបើ (i == idx)
		    bha_nga;
	    }
    }
}
