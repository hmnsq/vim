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
static ကိန်းပြည့်	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static ကိန်းပြည့်	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static ကိန်းပြည့်	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ကိန်းပြည့်
get_hislen(void)
{
    ပြန်ပေး hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ကိန်းပြည့် hist_type)
{
    ပြန်ပေး history[hist_type];
}

#အကယ်၍ defined(FEAT_VIMINFO)
    void
set_histentry(ကိန်းပြည့် hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ကိန်းပြည့် *
get_hisidx(ကိန်းပြည့် hist_type)
{
    ပြန်ပေး &hisidx[hist_type];
}

#အကယ်၍ defined(FEAT_VIMINFO)
    ကိန်းပြည့် *
get_hisnum(ကိန်းပြည့် hist_type)
{
    ပြန်ပေး &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ကိန်းပြည့်
hist_char2type(ကိန်းပြည့် c)
{
    အကယ်၍ (c == ':')
	ပြန်ပေး HIST_CMD;
    အကယ်၍ (c == '=')
	ပြန်ပေး HIST_EXPR;
    အကယ်၍ (c == '@')
	ပြန်ပေး HIST_INPUT;
    အကယ်၍ (c == '>')
	ပြန်ပေး HIST_DEBUG;
    ပြန်ပေး HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, ကိန်းပြည့် idx)
{
    var_na    *short_names = ":=@>?/";
    ကိန်းပြည့်	    short_names_count = (ကိန်းပြည့်)STRLEN(short_names);
    ကိန်းပြည့်	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    အကယ်၍ (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ပြန်ပေး xp->xp_buf;
    }
    အကယ်၍ (idx < short_names_count + history_name_count)
	ပြန်ပေး (char_u *)history_names[idx - short_names_count];
    အကယ်၍ (idx == short_names_count + history_name_count)
	ပြန်ပေး (char_u *)"all";
    ပြန်ပေး NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    ကိန်းပြည့်		newlen;	    // new length of history table
    histentry_T	*temp;
    ကိန်းပြည့်		type;

    // If size of history table changed, reallocate it
    newlen = (ကိန်းပြည့်)p_hi;
    အကယ်၍ (newlen == hislen)		// history length didn't change
	ပြန်ပေး;

    // history length changed
    အတွက် (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	အကယ်၍ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    အကယ်၍ (temp == NULL)   // out of memory!
	    {
		အကယ်၍ (type == 0)  // first one: just keep the old length
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
	မဟုတ်ပါက
	    temp = NULL;

	အကယ်၍ (hisidx[type] < 0)		// there are no entries yet
	{
	    ကိန်းပြည့်	i;

	    အတွက် (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	မဟုတ်ပါက အကယ်၍ (newlen > hislen)	// array becomes bigger
	{
	    ကိန်းပြည့်	i;
	    ကိန်းပြည့်	j;

	    အတွက် (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    အတွက် ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    အတွက် ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	မဟုတ်ပါက				// array becomes smaller or 0
	{
	    ကိန်းပြည့်	i;
	    ကိန်းပြည့်	j;

	    j = hisidx[type];
	    အတွက် (i = newlen - 1; ; --i)
	    {
		အကယ်၍ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		မဟုတ်ပါက			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		အကယ်၍ (--j < 0)
		    j = hislen - 1;
		အကယ်၍ (j == hisidx[type])
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
    ကိန်းပြည့်
in_history(
    ကိန်းပြည့်	    type,
    char_u  *str,
    ကိန်းပြည့်	    move_to_front,	// Move the entry to the front if it exists
    ကိန်းပြည့်	    sep,
    ကိန်းပြည့်	    writing)		// ignore entries read from viminfo
{
    ကိန်းပြည့်	    i;
    ကိန်းပြည့်	    last_i = -1;
    char_u  *p;
    size_t  len;

    အကယ်၍ (hisidx[type] < 0)
	ပြန်ပေး FALSE;
    i = hisidx[type];
    do
    {
	အကယ်၍ (history[type][i].hisstr == NULL)
	    ပြန်ပေး FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	အကယ်၍ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    အကယ်၍ (!move_to_front)
		ပြန်ပေး TRUE;
	    last_i = i;
	    bha_nga;
	}
	အကယ်၍ (--i < 0)
	    i = hislen - 1;
    } နေစဉ် (i != hisidx[type]);

    အကယ်၍ (last_i < 0)
	ပြန်ပေး FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    နေစဉ် (i != hisidx[type])
    {
	အကယ်၍ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ပြန်ပေး TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static ကိန်းပြည့်
get_histtype(char_u *name)
{
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		len = (ကိန်းပြည့်)STRLEN(name);

    // No argument: use current history.
    အကယ်၍ (len == 0)
	ပြန်ပေး hist_char2type(get_cmdline_firstc());

    အတွက် (i = 0; history_names[i] != NULL; ++i)
	အကယ်၍ (STRNICMP(name, history_names[i], len) == 0)
	    ပြန်ပေး i;

    အကယ်၍ (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ပြန်ပေး hist_char2type(name[0]);

    ပြန်ပေး -1;
}

static ကိန်းပြည့်	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    ကိန်းပြည့်		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ကိန်းပြည့်		in_map,		// consider maptick when inside a mapping
    ကိန်းပြည့်		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    အကယ်၍ (hislen == 0)		// no history
	ပြန်ပေး;

    အကယ်၍ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ပြန်ပေး;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    အကယ်၍ (histype == HIST_SEARCH && in_map)
    {
	အကယ်၍ (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    အကယ်၍ (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    အကယ်၍ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ပြန်ပေး;

    အကယ်၍ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    အကယ်၍ (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    မဟုတ်ပါက
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    အကယ်၍ (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#အကယ်၍ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ကိန်းပြည့်
get_history_idx(ကိန်းပြည့် histype)
{
    အကယ်၍ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ပြန်ပေး -1;

    ပြန်ပေး history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static ကိန်းပြည့်
calc_hist_idx(ကိန်းပြည့် histype, ကိန်းပြည့် num)
{
    ကိန်းပြည့်		i;
    histentry_T	*hist;
    ကိန်းပြည့်		wrapped = FALSE;

    အကယ်၍ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ပြန်ပေး -1;

    hist = history[histype];
    အကယ်၍ (num > 0)
    {
	နေစဉ် (hist[i].hisnum > num)
	    အကယ်၍ (--i < 0)
	    {
		အကယ်၍ (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	အကယ်၍ (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ပြန်ပေး i;
    }
    မဟုတ်ပါက အကယ်၍ (-num <= hislen)
    {
	i += num + 1;
	အကယ်၍ (i < 0)
	    i += hislen;
	အကယ်၍ (hist[i].hisstr != NULL)
	    ပြန်ပေး i;
    }
    ပြန်ပေး -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static ကိန်းပြည့်
clr_history(ကိန်းပြည့် histype)
{
    ကိန်းပြည့်		i;
    histentry_T	*hisptr;

    အကယ်၍ (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	အတွက် (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ပြန်ပေး OK;
    }
    ပြန်ပေး FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ကိန်းပြည့်
del_history_entry(ကိန်းပြည့် histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ကိန်းပြည့်		idx;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		last;
    ကိန်းပြည့်		found = FALSE;

    အကယ်၍ (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ပြန်ပေး FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    အကယ်၍ (regmatch.regprog == NULL)
	ပြန်ပေး FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	အကယ်၍ (hisptr->hisstr == NULL)
	    bha_nga;
	အကယ်၍ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	မဟုတ်ပါက
	{
	    အကယ်၍ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    အကယ်၍ (--last < 0)
		last += hislen;
	}
	အကယ်၍ (--i < 0)
	    i += hislen;
    } နေစဉ် (i != idx);

    အကယ်၍ (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ပြန်ပေး found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ကိန်းပြည့်
del_history_idx(ကိန်းပြည့် histype, ကိန်းပြည့် idx)
{
    ကိန်းပြည့်	    i, j;

    i = calc_hist_idx(histype, idx);
    အကယ်၍ (i < 0)
	ပြန်ပေး FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    အကယ်၍ (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    နေစဉ် (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    အကယ်၍ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ပြန်ပေး TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ကိန်းပြည့်		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    အကယ်၍ (check_secure())
	ပြန်ပေး;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    အကယ်၍ (histype < 0)
	ပြန်ပေး;

    str = tv_get_string_buf(&argvars[1], buf);
    အကယ်၍ (*str == NUL)
	ပြန်ပေး;

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
    ကိန်းပြည့်		n;
    char_u	*str;

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    အကယ်၍ (str == NULL)
	n = 0;
    မဟုတ်ပါက အကယ်၍ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    မဟုတ်ပါက အကယ်၍ (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ကိန်းပြည့်)tv_get_number(&argvars[1]));
    မဟုတ်ပါက
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

    အကယ်၍ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ပြန်ပေး;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    အကယ်၍ (str == NULL)
	rettv->vval.v_string = NULL;
    မဟုတ်ပါက
    {
	ကိန်းပြည့် type;
	ကိန်းပြည့် idx;

	type = get_histtype(str);
	အကယ်၍ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	မဟုတ်ပါက
	    idx = (ကိန်းပြည့်)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	အကယ်၍ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	မဟုတ်ပါက
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
    ကိန်းပြည့်		i;
    char_u	*histname;

    အကယ်၍ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ပြန်ပေး;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    အကယ်၍ (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    မဟုတ်ပါက
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#အကယ်၍ defined(FEAT_CRYPT)
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
    ကိန်းပြည့်		i;

    i = hisidx[HIST_CMD];
    အကယ်၍ (i < 0)
	ပြန်ပေး;
    p_start = history[HIST_CMD][i].hisstr;
    အကယ်၍ (p_start == NULL)
	ပြန်ပေး;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    အတွက် (p = p_start; *p; ++p)
    {
	အကယ်၍ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    အကယ်၍ (p == NULL)
		bha_nga;
	    ++p;
	    အတွက် (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		အကယ်၍ (p[i] == '\\' && p[i + 1])
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
    ကိန်းပြည့်		histype1 = HIST_CMD;
    ကိန်းပြည့်		histype2 = HIST_CMD;
    ကိန်းပြည့်		hisidx1 = 1;
    ကိန်းပြည့်		hisidx2 = -1;
    ကိန်းပြည့်		idx;
    ကိန်းပြည့်		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    အကယ်၍ (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ပြန်ပေး;
    }

    အကယ်၍ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	နေစဉ် (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	အကယ်၍ (histype1 == -1)
	{
	    အကယ်၍ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    မဟုတ်ပါက
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ပြန်ပေး;
	    }
	}
	မဟုတ်ပါက
	    histype2 = histype1;
	*end = i;
    }
    မဟုတ်ပါက
	end = arg;
    အကယ်၍ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	အကယ်၍ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	မဟုတ်ပါက
	    semsg(_(e_val_too_large), arg);
	ပြန်ပေး;
    }

    အတွက် (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	အကယ်၍ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	အကယ်၍ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	အကယ်၍ (idx >= 0 && j <= k)
	    အတွက် (i = idx + 1; !got_int; ++i)
	    {
		အကယ်၍ (i == hislen)
		    i = 0;
		အကယ်၍ (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ကိန်းပြည့်  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    အကယ်၍ (vim_strsize(hist[i].hisstr) > (ကိန်းပြည့်)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ကိန်းပြည့်)Columns - 10, IOSIZE - (ကိန်းပြည့်)len);
		    မဟုတ်ပါက
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		အကယ်၍ (i == idx)
		    bha_nga;
	    }
    }
}
