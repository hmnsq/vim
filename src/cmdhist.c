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
static puur_naa_nka	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static puur_naa_nka	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static puur_naa_nka	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    puur_naa_nka
get_hislen(void)
{
    ပွၵ်ႈ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(puur_naa_nka hist_type)
{
    ပွၵ်ႈ history[hist_type];
}

#ပေႃး defined(FEAT_VIMINFO)
    void
set_histentry(puur_naa_nka hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    puur_naa_nka *
get_hisidx(puur_naa_nka hist_type)
{
    ပွၵ်ႈ &hisidx[hist_type];
}

#ပေႃး defined(FEAT_VIMINFO)
    puur_naa_nka *
get_hisnum(puur_naa_nka hist_type)
{
    ပွၵ်ႈ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    puur_naa_nka
hist_char2type(puur_naa_nka c)
{
    ပေႃး (c == ':')
	ပွၵ်ႈ HIST_CMD;
    ပေႃး (c == '=')
	ပွၵ်ႈ HIST_EXPR;
    ပေႃး (c == '@')
	ပွၵ်ႈ HIST_INPUT;
    ပေႃး (c == '>')
	ပွၵ်ႈ HIST_DEBUG;
    ပွၵ်ႈ HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, puur_naa_nka idx)
{
    var_na    *short_names = ":=@>?/";
    puur_naa_nka	    short_names_count = (puur_naa_nka)STRLEN(short_names);
    puur_naa_nka	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ပေႃး (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ပွၵ်ႈ xp->xp_buf;
    }
    ပေႃး (idx < short_names_count + history_name_count)
	ပွၵ်ႈ (char_u *)history_names[idx - short_names_count];
    ပေႃး (idx == short_names_count + history_name_count)
	ပွၵ်ႈ (char_u *)"all";
    ပွၵ်ႈ NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    puur_naa_nka		newlen;	    // new length of history table
    histentry_T	*temp;
    puur_naa_nka		type;

    // If size of history table changed, reallocate it
    newlen = (puur_naa_nka)p_hi;
    ပေႃး (newlen == hislen)		// history length didn't change
	ပွၵ်ႈ;

    // history length changed
    for (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ပေႃး (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ပေႃး (temp == NULL)   // out of memory!
	    {
		ပေႃး (type == 0)  // first one: just keep the old length
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
	ပေႃးဢမ်ႇ
	    temp = NULL;

	ပေႃး (hisidx[type] < 0)		// there are no entries yet
	{
	    puur_naa_nka	i;

	    for (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	ပေႃးဢမ်ႇ ပေႃး (newlen > hislen)	// array becomes bigger
	{
	    puur_naa_nka	i;
	    puur_naa_nka	j;

	    for (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    for ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    for ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	ပေႃးဢမ်ႇ				// array becomes smaller or 0
	{
	    puur_naa_nka	i;
	    puur_naa_nka	j;

	    j = hisidx[type];
	    for (i = newlen - 1; ; --i)
	    {
		ပေႃး (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		ပေႃးဢမ်ႇ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ပေႃး (--j < 0)
		    j = hislen - 1;
		ပေႃး (j == hisidx[type])
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
    puur_naa_nka
in_history(
    puur_naa_nka	    type,
    char_u  *str,
    puur_naa_nka	    move_to_front,	// Move the entry to the front if it exists
    puur_naa_nka	    sep,
    puur_naa_nka	    writing)		// ignore entries read from viminfo
{
    puur_naa_nka	    i;
    puur_naa_nka	    last_i = -1;
    char_u  *p;
    size_t  len;

    ပေႃး (hisidx[type] < 0)
	ပွၵ်ႈ FALSE;
    i = hisidx[type];
    do
    {
	ပေႃး (history[type][i].hisstr == NULL)
	    ပွၵ်ႈ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ပေႃး (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ပေႃး (!move_to_front)
		ပွၵ်ႈ TRUE;
	    last_i = i;
	    bha_nga;
	}
	ပေႃး (--i < 0)
	    i = hislen - 1;
    } ၶၢဝ်း (i != hisidx[type]);

    ပေႃး (last_i < 0)
	ပွၵ်ႈ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ၶၢဝ်း (i != hisidx[type])
    {
	ပေႃး (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ပွၵ်ႈ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static puur_naa_nka
get_histtype(char_u *name)
{
    puur_naa_nka		i;
    puur_naa_nka		len = (puur_naa_nka)STRLEN(name);

    // No argument: use current history.
    ပေႃး (len == 0)
	ပွၵ်ႈ hist_char2type(get_cmdline_firstc());

    for (i = 0; history_names[i] != NULL; ++i)
	ပေႃး (STRNICMP(name, history_names[i], len) == 0)
	    ပွၵ်ႈ i;

    ပေႃး (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ပွၵ်ႈ hist_char2type(name[0]);

    ပွၵ်ႈ -1;
}

static puur_naa_nka	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    puur_naa_nka		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    puur_naa_nka		in_map,		// consider maptick when inside a mapping
    puur_naa_nka		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ပေႃး (hislen == 0)		// no history
	ပွၵ်ႈ;

    ပေႃး ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ပွၵ်ႈ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ပေႃး (histype == HIST_SEARCH && in_map)
    {
	ပေႃး (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ပေႃး (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ပေႃး (in_history(histype, new_entry, TRUE, sep, FALSE))
	ပွၵ်ႈ;

    ပေႃး (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ပေႃး (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    ပေႃးဢမ်ႇ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ပေႃး (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ပေႃး defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static puur_naa_nka
get_history_idx(puur_naa_nka histype)
{
    ပေႃး (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ပွၵ်ႈ -1;

    ပွၵ်ႈ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static puur_naa_nka
calc_hist_idx(puur_naa_nka histype, puur_naa_nka num)
{
    puur_naa_nka		i;
    histentry_T	*hist;
    puur_naa_nka		wrapped = FALSE;

    ပေႃး (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ပွၵ်ႈ -1;

    hist = history[histype];
    ပေႃး (num > 0)
    {
	ၶၢဝ်း (hist[i].hisnum > num)
	    ပေႃး (--i < 0)
	    {
		ပေႃး (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	ပေႃး (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ပွၵ်ႈ i;
    }
    ပေႃးဢမ်ႇ ပေႃး (-num <= hislen)
    {
	i += num + 1;
	ပေႃး (i < 0)
	    i += hislen;
	ပေႃး (hist[i].hisstr != NULL)
	    ပွၵ်ႈ i;
    }
    ပွၵ်ႈ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static puur_naa_nka
clr_history(puur_naa_nka histype)
{
    puur_naa_nka		i;
    histentry_T	*hisptr;

    ပေႃး (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	for (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ပွၵ်ႈ OK;
    }
    ပွၵ်ႈ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static puur_naa_nka
del_history_entry(puur_naa_nka histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    puur_naa_nka		idx;
    puur_naa_nka		i;
    puur_naa_nka		last;
    puur_naa_nka		found = FALSE;

    ပေႃး (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ပွၵ်ႈ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ပေႃး (regmatch.regprog == NULL)
	ပွၵ်ႈ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	ပေႃး (hisptr->hisstr == NULL)
	    bha_nga;
	ပေႃး (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	ပေႃးဢမ်ႇ
	{
	    ပေႃး (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ပေႃး (--last < 0)
		last += hislen;
	}
	ပေႃး (--i < 0)
	    i += hislen;
    } ၶၢဝ်း (i != idx);

    ပေႃး (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ပွၵ်ႈ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static puur_naa_nka
del_history_idx(puur_naa_nka histype, puur_naa_nka idx)
{
    puur_naa_nka	    i, j;

    i = calc_hist_idx(histype, idx);
    ပေႃး (i < 0)
	ပွၵ်ႈ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ပေႃး (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ၶၢဝ်း (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ပေႃး (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ပွၵ်ႈ TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    puur_naa_nka		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ပေႃး (check_secure())
	ပွၵ်ႈ;

    ပေႃး (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ပွၵ်ႈ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    ပေႃး (histype < 0)
	ပွၵ်ႈ;

    str = tv_get_string_buf(&argvars[1], buf);
    ပေႃး (*str == NUL)
	ပွၵ်ႈ;

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
    puur_naa_nka		n;
    char_u	*str;

    ပေႃး (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ပွၵ်ႈ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ပေႃး (str == NULL)
	n = 0;
    ပေႃးဢမ်ႇ ပေႃး (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ပေႃးဢမ်ႇ ပေႃး (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (puur_naa_nka)tv_get_number(&argvars[1]));
    ပေႃးဢမ်ႇ
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

    ပေႃး (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ပွၵ်ႈ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ပေႃး (str == NULL)
	rettv->vval.v_string = NULL;
    ပေႃးဢမ်ႇ
    {
	puur_naa_nka type;
	puur_naa_nka idx;

	type = get_histtype(str);
	ပေႃး (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ပေႃးဢမ်ႇ
	    idx = (puur_naa_nka)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ပေႃး (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ပေႃးဢမ်ႇ
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
    puur_naa_nka		i;
    char_u	*histname;

    ပေႃး (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပွၵ်ႈ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    ပေႃး (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    ပေႃးဢမ်ႇ
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ပေႃး defined(FEAT_CRYPT)
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
    puur_naa_nka		i;

    i = hisidx[HIST_CMD];
    ပေႃး (i < 0)
	ပွၵ်ႈ;
    p_start = history[HIST_CMD][i].hisstr;
    ပေႃး (p_start == NULL)
	ပွၵ်ႈ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    for (p = p_start; *p; ++p)
    {
	ပေႃး (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ပေႃး (p == NULL)
		bha_nga;
	    ++p;
	    for (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ပေႃး (p[i] == '\\' && p[i + 1])
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
    puur_naa_nka		histype1 = HIST_CMD;
    puur_naa_nka		histype2 = HIST_CMD;
    puur_naa_nka		hisidx1 = 1;
    puur_naa_nka		hisidx2 = -1;
    puur_naa_nka		idx;
    puur_naa_nka		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ပေႃး (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ပွၵ်ႈ;
    }

    ပေႃး (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ၶၢဝ်း (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ပေႃး (histype1 == -1)
	{
	    ပေႃး (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    ပေႃးဢမ်ႇ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ပွၵ်ႈ;
	    }
	}
	ပေႃးဢမ်ႇ
	    histype2 = histype1;
	*end = i;
    }
    ပေႃးဢမ်ႇ
	end = arg;
    ပေႃး (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ပေႃး (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	ပေႃးဢမ်ႇ
	    semsg(_(e_val_too_large), arg);
	ပွၵ်ႈ;
    }

    for (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ပေႃး (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ပေႃး (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ပေႃး (idx >= 0 && j <= k)
	    for (i = idx + 1; !got_int; ++i)
	    {
		ပေႃး (i == hislen)
		    i = 0;
		ပေႃး (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    puur_naa_nka  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ပေႃး (vim_strsize(hist[i].hisstr) > (puur_naa_nka)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (puur_naa_nka)Columns - 10, IOSIZE - (puur_naa_nka)len);
		    ပေႃးဢမ်ႇ
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ပေႃး (i == idx)
		    bha_nga;
	    }
    }
}
