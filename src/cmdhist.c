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
static ຈຳນວນເຕັມ	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static ຈຳນວນເຕັມ	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static ຈຳນວນເຕັມ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ຈຳນວນເຕັມ
get_hislen(void)
{
    ສົ່ງຄືນ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ຈຳນວນເຕັມ hist_type)
{
    ສົ່ງຄືນ history[hist_type];
}

#ຖ້າ defined(FEAT_VIMINFO)
    void
set_histentry(ຈຳນວນເຕັມ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ຈຳນວນເຕັມ *
get_hisidx(ຈຳນວນເຕັມ hist_type)
{
    ສົ່ງຄືນ &hisidx[hist_type];
}

#ຖ້າ defined(FEAT_VIMINFO)
    ຈຳນວນເຕັມ *
get_hisnum(ຈຳນວນເຕັມ hist_type)
{
    ສົ່ງຄືນ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ຈຳນວນເຕັມ
hist_char2type(ຈຳນວນເຕັມ c)
{
    ຖ້າ (c == ':')
	ສົ່ງຄືນ HIST_CMD;
    ຖ້າ (c == '=')
	ສົ່ງຄືນ HIST_EXPR;
    ຖ້າ (c == '@')
	ສົ່ງຄືນ HIST_INPUT;
    ຖ້າ (c == '>')
	ສົ່ງຄືນ HIST_DEBUG;
    ສົ່ງຄືນ HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, ຈຳນວນເຕັມ idx)
{
    var_na    *short_names = ":=@>?/";
    ຈຳນວນເຕັມ	    short_names_count = (ຈຳນວນເຕັມ)STRLEN(short_names);
    ຈຳນວນເຕັມ	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ຖ້າ (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ສົ່ງຄືນ xp->xp_buf;
    }
    ຖ້າ (idx < short_names_count + history_name_count)
	ສົ່ງຄືນ (char_u *)history_names[idx - short_names_count];
    ຖ້າ (idx == short_names_count + history_name_count)
	ສົ່ງຄືນ (char_u *)"all";
    ສົ່ງຄືນ NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    ຈຳນວນເຕັມ		newlen;	    // new length of history table
    histentry_T	*temp;
    ຈຳນວນເຕັມ		type;

    // If size of history table changed, reallocate it
    newlen = (ຈຳນວນເຕັມ)p_hi;
    ຖ້າ (newlen == hislen)		// history length didn't change
	ສົ່ງຄືນ;

    // history length changed
    ສຳລັບ (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ຖ້າ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ຖ້າ (temp == NULL)   // out of memory!
	    {
		ຖ້າ (type == 0)  // first one: just keep the old length
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
	ບໍ່ດັ່ງນັ້ນ
	    temp = NULL;

	ຖ້າ (hisidx[type] < 0)		// there are no entries yet
	{
	    ຈຳນວນເຕັມ	i;

	    ສຳລັບ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (newlen > hislen)	// array becomes bigger
	{
	    ຈຳນວນເຕັມ	i;
	    ຈຳນວນເຕັມ	j;

	    ສຳລັບ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    ສຳລັບ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    ສຳລັບ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	ບໍ່ດັ່ງນັ້ນ				// array becomes smaller or 0
	{
	    ຈຳນວນເຕັມ	i;
	    ຈຳນວນເຕັມ	j;

	    j = hisidx[type];
	    ສຳລັບ (i = newlen - 1; ; --i)
	    {
		ຖ້າ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		ບໍ່ດັ່ງນັ້ນ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ຖ້າ (--j < 0)
		    j = hislen - 1;
		ຖ້າ (j == hisidx[type])
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
    ຈຳນວນເຕັມ
in_history(
    ຈຳນວນເຕັມ	    type,
    char_u  *str,
    ຈຳນວນເຕັມ	    move_to_front,	// Move the entry to the front if it exists
    ຈຳນວນເຕັມ	    sep,
    ຈຳນວນເຕັມ	    writing)		// ignore entries read from viminfo
{
    ຈຳນວນເຕັມ	    i;
    ຈຳນວນເຕັມ	    last_i = -1;
    char_u  *p;
    size_t  len;

    ຖ້າ (hisidx[type] < 0)
	ສົ່ງຄືນ FALSE;
    i = hisidx[type];
    do
    {
	ຖ້າ (history[type][i].hisstr == NULL)
	    ສົ່ງຄືນ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ຖ້າ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ຖ້າ (!move_to_front)
		ສົ່ງຄືນ TRUE;
	    last_i = i;
	    bha_nga;
	}
	ຖ້າ (--i < 0)
	    i = hislen - 1;
    } ໃນຂະນະທີ່ (i != hisidx[type]);

    ຖ້າ (last_i < 0)
	ສົ່ງຄືນ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ໃນຂະນະທີ່ (i != hisidx[type])
    {
	ຖ້າ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ສົ່ງຄືນ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static ຈຳນວນເຕັມ
get_histtype(char_u *name)
{
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		len = (ຈຳນວນເຕັມ)STRLEN(name);

    // No argument: use current history.
    ຖ້າ (len == 0)
	ສົ່ງຄືນ hist_char2type(get_cmdline_firstc());

    ສຳລັບ (i = 0; history_names[i] != NULL; ++i)
	ຖ້າ (STRNICMP(name, history_names[i], len) == 0)
	    ສົ່ງຄືນ i;

    ຖ້າ (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ສົ່ງຄືນ hist_char2type(name[0]);

    ສົ່ງຄືນ -1;
}

static ຈຳນວນເຕັມ	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    ຈຳນວນເຕັມ		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ຈຳນວນເຕັມ		in_map,		// consider maptick when inside a mapping
    ຈຳນວນເຕັມ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ຖ້າ (hislen == 0)		// no history
	ສົ່ງຄືນ;

    ຖ້າ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ສົ່ງຄືນ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ຖ້າ (histype == HIST_SEARCH && in_map)
    {
	ຖ້າ (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ຖ້າ (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ຖ້າ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ສົ່ງຄືນ;

    ຖ້າ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ຖ້າ (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    ບໍ່ດັ່ງນັ້ນ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ຖ້າ (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ຖ້າ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ຈຳນວນເຕັມ
get_history_idx(ຈຳນວນເຕັມ histype)
{
    ຖ້າ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ສົ່ງຄືນ -1;

    ສົ່ງຄືນ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static ຈຳນວນເຕັມ
calc_hist_idx(ຈຳນວນເຕັມ histype, ຈຳນວນເຕັມ num)
{
    ຈຳນວນເຕັມ		i;
    histentry_T	*hist;
    ຈຳນວນເຕັມ		wrapped = FALSE;

    ຖ້າ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ສົ່ງຄືນ -1;

    hist = history[histype];
    ຖ້າ (num > 0)
    {
	ໃນຂະນະທີ່ (hist[i].hisnum > num)
	    ຖ້າ (--i < 0)
	    {
		ຖ້າ (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	ຖ້າ (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ສົ່ງຄືນ i;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (-num <= hislen)
    {
	i += num + 1;
	ຖ້າ (i < 0)
	    i += hislen;
	ຖ້າ (hist[i].hisstr != NULL)
	    ສົ່ງຄືນ i;
    }
    ສົ່ງຄືນ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static ຈຳນວນເຕັມ
clr_history(ຈຳນວນເຕັມ histype)
{
    ຈຳນວນເຕັມ		i;
    histentry_T	*hisptr;

    ຖ້າ (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	ສຳລັບ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ສົ່ງຄືນ OK;
    }
    ສົ່ງຄືນ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ຈຳນວນເຕັມ
del_history_entry(ຈຳນວນເຕັມ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ຈຳນວນເຕັມ		idx;
    ຈຳນວນເຕັມ		i;
    ຈຳນວນເຕັມ		last;
    ຈຳນວນເຕັມ		found = FALSE;

    ຖ້າ (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ສົ່ງຄືນ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ຖ້າ (regmatch.regprog == NULL)
	ສົ່ງຄືນ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	ຖ້າ (hisptr->hisstr == NULL)
	    bha_nga;
	ຖ້າ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຖ້າ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ຖ້າ (--last < 0)
		last += hislen;
	}
	ຖ້າ (--i < 0)
	    i += hislen;
    } ໃນຂະນະທີ່ (i != idx);

    ຖ້າ (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ສົ່ງຄືນ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ຈຳນວນເຕັມ
del_history_idx(ຈຳນວນເຕັມ histype, ຈຳນວນເຕັມ idx)
{
    ຈຳນວນເຕັມ	    i, j;

    i = calc_hist_idx(histype, idx);
    ຖ້າ (i < 0)
	ສົ່ງຄືນ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ຖ້າ (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ໃນຂະນະທີ່ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ຖ້າ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ສົ່ງຄືນ TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ຈຳນວນເຕັມ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ຖ້າ (check_secure())
	ສົ່ງຄືນ;

    ຖ້າ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ສົ່ງຄືນ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    ຖ້າ (histype < 0)
	ສົ່ງຄືນ;

    str = tv_get_string_buf(&argvars[1], buf);
    ຖ້າ (*str == NUL)
	ສົ່ງຄືນ;

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
    ຈຳນວນເຕັມ		n;
    char_u	*str;

    ຖ້າ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ສົ່ງຄືນ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ຖ້າ (str == NULL)
	n = 0;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ຈຳນວນເຕັມ)tv_get_number(&argvars[1]));
    ບໍ່ດັ່ງນັ້ນ
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

    ຖ້າ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ສົ່ງຄືນ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ຖ້າ (str == NULL)
	rettv->vval.v_string = NULL;
    ບໍ່ດັ່ງນັ້ນ
    {
	ຈຳນວນເຕັມ type;
	ຈຳນວນເຕັມ idx;

	type = get_histtype(str);
	ຖ້າ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ບໍ່ດັ່ງນັ້ນ
	    idx = (ຈຳນວນເຕັມ)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ຖ້າ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ບໍ່ດັ່ງນັ້ນ
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
    ຈຳນວນເຕັມ		i;
    char_u	*histname;

    ຖ້າ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ສົ່ງຄືນ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    ຖ້າ (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    ບໍ່ດັ່ງນັ້ນ
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ຖ້າ defined(FEAT_CRYPT)
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
    ຈຳນວນເຕັມ		i;

    i = hisidx[HIST_CMD];
    ຖ້າ (i < 0)
	ສົ່ງຄືນ;
    p_start = history[HIST_CMD][i].hisstr;
    ຖ້າ (p_start == NULL)
	ສົ່ງຄືນ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    ສຳລັບ (p = p_start; *p; ++p)
    {
	ຖ້າ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ຖ້າ (p == NULL)
		bha_nga;
	    ++p;
	    ສຳລັບ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ຖ້າ (p[i] == '\\' && p[i + 1])
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
    ຈຳນວນເຕັມ		histype1 = HIST_CMD;
    ຈຳນວນເຕັມ		histype2 = HIST_CMD;
    ຈຳນວນເຕັມ		hisidx1 = 1;
    ຈຳນວນເຕັມ		hisidx2 = -1;
    ຈຳນວນເຕັມ		idx;
    ຈຳນວນເຕັມ		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ຖ້າ (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ສົ່ງຄືນ;
    }

    ຖ້າ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ໃນຂະນະທີ່ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ຖ້າ (histype1 == -1)
	{
	    ຖ້າ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    ບໍ່ດັ່ງນັ້ນ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ສົ່ງຄືນ;
	    }
	}
	ບໍ່ດັ່ງນັ້ນ
	    histype2 = histype1;
	*end = i;
    }
    ບໍ່ດັ່ງນັ້ນ
	end = arg;
    ຖ້າ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ຖ້າ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	ບໍ່ດັ່ງນັ້ນ
	    semsg(_(e_val_too_large), arg);
	ສົ່ງຄືນ;
    }

    ສຳລັບ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ຖ້າ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ຖ້າ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ຖ້າ (idx >= 0 && j <= k)
	    ສຳລັບ (i = idx + 1; !got_int; ++i)
	    {
		ຖ້າ (i == hislen)
		    i = 0;
		ຖ້າ (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ຈຳນວນເຕັມ  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ຖ້າ (vim_strsize(hist[i].hisstr) > (ຈຳນວນເຕັມ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ຈຳນວນເຕັມ)Columns - 10, IOSIZE - (ຈຳນວນເຕັມ)len);
		    ບໍ່ດັ່ງນັ້ນ
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ຖ້າ (i == idx)
		    bha_nga;
	    }
    }
}
