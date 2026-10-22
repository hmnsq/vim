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
static පූර්ණ_සංඛ්‍යා	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static පූර්ණ_සංඛ්‍යා	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static පූර්ණ_සංඛ්‍යා	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    පූර්ණ_සංඛ්‍යා
get_hislen(void)
{
    ආපසු hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(පූර්ණ_සංඛ්‍යා hist_type)
{
    ආපසු history[hist_type];
}

#නම් defined(FEAT_VIMINFO)
    void
set_histentry(පූර්ණ_සංඛ්‍යා hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    පූර්ණ_සංඛ්‍යා *
get_hisidx(පූර්ණ_සංඛ්‍යා hist_type)
{
    ආපසු &hisidx[hist_type];
}

#නම් defined(FEAT_VIMINFO)
    පූර්ණ_සංඛ්‍යා *
get_hisnum(පූර්ණ_සංඛ්‍යා hist_type)
{
    ආපසු &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    පූර්ණ_සංඛ්‍යා
hist_char2type(පූර්ණ_සංඛ්‍යා c)
{
    නම් (c == ':')
	ආපසු HIST_CMD;
    නම් (c == '=')
	ආපසු HIST_EXPR;
    නම් (c == '@')
	ආපසු HIST_INPUT;
    නම් (c == '>')
	ආපසු HIST_DEBUG;
    ආපසු HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    var_na    *short_names = ":=@>?/";
    පූර්ණ_සංඛ්‍යා	    short_names_count = (පූර්ණ_සංඛ්‍යා)STRLEN(short_names);
    පූර්ණ_සංඛ්‍යා	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    නම් (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ආපසු xp->xp_buf;
    }
    නම් (idx < short_names_count + history_name_count)
	ආපසු (char_u *)history_names[idx - short_names_count];
    නම් (idx == short_names_count + history_name_count)
	ආපසු (char_u *)"all";
    ආපසු NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    පූර්ණ_සංඛ්‍යා		newlen;	    // new length of history table
    histentry_T	*temp;
    පූර්ණ_සංඛ්‍යා		type;

    // If size of history table changed, reallocate it
    newlen = (පූර්ණ_සංඛ්‍යා)p_hi;
    නම් (newlen == hislen)		// history length didn't change
	ආපසු;

    // history length changed
    සඳහා (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	නම් (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    නම් (temp == NULL)   // out of memory!
	    {
		නම් (type == 0)  // first one: just keep the old length
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
	නැතහොත්
	    temp = NULL;

	නම් (hisidx[type] < 0)		// there are no entries yet
	{
	    පූර්ණ_සංඛ්‍යා	i;

	    සඳහා (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	නැතහොත් නම් (newlen > hislen)	// array becomes bigger
	{
	    පූර්ණ_සංඛ්‍යා	i;
	    පූර්ණ_සංඛ්‍යා	j;

	    සඳහා (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    සඳහා ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    සඳහා ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	නැතහොත්				// array becomes smaller or 0
	{
	    පූර්ණ_සංඛ්‍යා	i;
	    පූර්ණ_සංඛ්‍යා	j;

	    j = hisidx[type];
	    සඳහා (i = newlen - 1; ; --i)
	    {
		නම් (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		නැතහොත්			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		නම් (--j < 0)
		    j = hislen - 1;
		නම් (j == hisidx[type])
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
    පූර්ණ_සංඛ්‍යා
in_history(
    පූර්ණ_සංඛ්‍යා	    type,
    char_u  *str,
    පූර්ණ_සංඛ්‍යා	    move_to_front,	// Move the entry to the front if it exists
    පූර්ණ_සංඛ්‍යා	    sep,
    පූර්ණ_සංඛ්‍යා	    writing)		// ignore entries read from viminfo
{
    පූර්ණ_සංඛ්‍යා	    i;
    පූර්ණ_සංඛ්‍යා	    last_i = -1;
    char_u  *p;
    size_t  len;

    නම් (hisidx[type] < 0)
	ආපසු FALSE;
    i = hisidx[type];
    do
    {
	නම් (history[type][i].hisstr == NULL)
	    ආපසු FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	නම් (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    නම් (!move_to_front)
		ආපසු TRUE;
	    last_i = i;
	    bha_nga;
	}
	නම් (--i < 0)
	    i = hislen - 1;
    } අතරතුර (i != hisidx[type]);

    නම් (last_i < 0)
	ආපසු FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    අතරතුර (i != hisidx[type])
    {
	නම් (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ආපසු TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static පූර්ණ_සංඛ්‍යා
get_histtype(char_u *name)
{
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		len = (පූර්ණ_සංඛ්‍යා)STRLEN(name);

    // No argument: use current history.
    නම් (len == 0)
	ආපසු hist_char2type(get_cmdline_firstc());

    සඳහා (i = 0; history_names[i] != NULL; ++i)
	නම් (STRNICMP(name, history_names[i], len) == 0)
	    ආපසු i;

    නම් (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ආපසු hist_char2type(name[0]);

    ආපසු -1;
}

static පූර්ණ_සංඛ්‍යා	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    පූර්ණ_සංඛ්‍යා		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    පූර්ණ_සංඛ්‍යා		in_map,		// consider maptick when inside a mapping
    පූර්ණ_සංඛ්‍යා		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    නම් (hislen == 0)		// no history
	ආපසු;

    නම් ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ආපසු;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    නම් (histype == HIST_SEARCH && in_map)
    {
	නම් (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    නම් (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    නම් (in_history(histype, new_entry, TRUE, sep, FALSE))
	ආපසු;

    නම් (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    නම් (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    නැතහොත්
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    නම් (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#නම් defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static පූර්ණ_සංඛ්‍යා
get_history_idx(පූර්ණ_සංඛ්‍යා histype)
{
    නම් (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ආපසු -1;

    ආපසු history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static පූර්ණ_සංඛ්‍යා
calc_hist_idx(පූර්ණ_සංඛ්‍යා histype, පූර්ණ_සංඛ්‍යා num)
{
    පූර්ණ_සංඛ්‍යා		i;
    histentry_T	*hist;
    පූර්ණ_සංඛ්‍යා		wrapped = FALSE;

    නම් (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ආපසු -1;

    hist = history[histype];
    නම් (num > 0)
    {
	අතරතුර (hist[i].hisnum > num)
	    නම් (--i < 0)
	    {
		නම් (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	නම් (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ආපසු i;
    }
    නැතහොත් නම් (-num <= hislen)
    {
	i += num + 1;
	නම් (i < 0)
	    i += hislen;
	නම් (hist[i].hisstr != NULL)
	    ආපසු i;
    }
    ආපසු -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static පූර්ණ_සංඛ්‍යා
clr_history(පූර්ණ_සංඛ්‍යා histype)
{
    පූර්ණ_සංඛ්‍යා		i;
    histentry_T	*hisptr;

    නම් (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	සඳහා (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ආපසු OK;
    }
    ආපසු FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static පූර්ණ_සංඛ්‍යා
del_history_entry(පූර්ණ_සංඛ්‍යා histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    පූර්ණ_සංඛ්‍යා		idx;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		last;
    පූර්ණ_සංඛ්‍යා		found = FALSE;

    නම් (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ආපසු FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    නම් (regmatch.regprog == NULL)
	ආපසු FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	නම් (hisptr->hisstr == NULL)
	    bha_nga;
	නම් (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	නැතහොත්
	{
	    නම් (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    නම් (--last < 0)
		last += hislen;
	}
	නම් (--i < 0)
	    i += hislen;
    } අතරතුර (i != idx);

    නම් (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ආපසු found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static පූර්ණ_සංඛ්‍යා
del_history_idx(පූර්ණ_සංඛ්‍යා histype, පූර්ණ_සංඛ්‍යා idx)
{
    පූර්ණ_සංඛ්‍යා	    i, j;

    i = calc_hist_idx(histype, idx);
    නම් (i < 0)
	ආපසු FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    නම් (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    අතරතුර (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    නම් (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ආපසු TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    පූර්ණ_සංඛ්‍යා		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    නම් (check_secure())
	ආපසු;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ආපසු;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    නම් (histype < 0)
	ආපසු;

    str = tv_get_string_buf(&argvars[1], buf);
    නම් (*str == NUL)
	ආපසු;

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
    පූර්ණ_සංඛ්‍යා		n;
    char_u	*str;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ආපසු;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    නම් (str == NULL)
	n = 0;
    නැතහොත් නම් (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    නැතහොත් නම් (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (පූර්ණ_සංඛ්‍යා)tv_get_number(&argvars[1]));
    නැතහොත්
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

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ආපසු;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    නම් (str == NULL)
	rettv->vval.v_string = NULL;
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා type;
	පූර්ණ_සංඛ්‍යා idx;

	type = get_histtype(str);
	නම් (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	නැතහොත්
	    idx = (පූර්ණ_සංඛ්‍යා)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	නම් (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	නැතහොත්
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
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*histname;

    නම් (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ආපසු;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    නම් (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    නැතහොත්
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#නම් defined(FEAT_CRYPT)
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
    පූර්ණ_සංඛ්‍යා		i;

    i = hisidx[HIST_CMD];
    නම් (i < 0)
	ආපසු;
    p_start = history[HIST_CMD][i].hisstr;
    නම් (p_start == NULL)
	ආපසු;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    සඳහා (p = p_start; *p; ++p)
    {
	නම් (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    නම් (p == NULL)
		bha_nga;
	    ++p;
	    සඳහා (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		නම් (p[i] == '\\' && p[i + 1])
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
    පූර්ණ_සංඛ්‍යා		histype1 = HIST_CMD;
    පූර්ණ_සංඛ්‍යා		histype2 = HIST_CMD;
    පූර්ණ_සංඛ්‍යා		hisidx1 = 1;
    පූර්ණ_සංඛ්‍යා		hisidx2 = -1;
    පූර්ණ_සංඛ්‍යා		idx;
    පූර්ණ_සංඛ්‍යා		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    නම් (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ආපසු;
    }

    නම් (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	අතරතුර (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	නම් (histype1 == -1)
	{
	    නම් (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    නැතහොත්
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ආපසු;
	    }
	}
	නැතහොත්
	    histype2 = histype1;
	*end = i;
    }
    නැතහොත්
	end = arg;
    නම් (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	නම් (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	නැතහොත්
	    semsg(_(e_val_too_large), arg);
	ආපසු;
    }

    සඳහා (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	නම් (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	නම් (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	නම් (idx >= 0 && j <= k)
	    සඳහා (i = idx + 1; !got_int; ++i)
	    {
		නම් (i == hislen)
		    i = 0;
		නම් (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    පූර්ණ_සංඛ්‍යා  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    නම් (vim_strsize(hist[i].hisstr) > (පූර්ණ_සංඛ්‍යා)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (පූර්ණ_සංඛ්‍යා)Columns - 10, IOSIZE - (පූර්ණ_සංඛ්‍යා)len);
		    නැතහොත්
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		නම් (i == idx)
		    bha_nga;
	    }
    }
}
