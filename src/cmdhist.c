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
static ཧྲིལ	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static ཧྲིལ	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static ཧྲིལ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ཧྲིལ
get_hislen(void)
{
    ཕྱིར hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ཧྲིལ hist_type)
{
    ཕྱིར history[hist_type];
}

#གལ་ཏེ defined(FEAT_VIMINFO)
    void
set_histentry(ཧྲིལ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ཧྲིལ *
get_hisidx(ཧྲིལ hist_type)
{
    ཕྱིར &hisidx[hist_type];
}

#གལ་ཏེ defined(FEAT_VIMINFO)
    ཧྲིལ *
get_hisnum(ཧྲིལ hist_type)
{
    ཕྱིར &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ཧྲིལ
hist_char2type(ཧྲིལ c)
{
    གལ་ཏེ (c == ':')
	ཕྱིར HIST_CMD;
    གལ་ཏེ (c == '=')
	ཕྱིར HIST_EXPR;
    གལ་ཏེ (c == '@')
	ཕྱིར HIST_INPUT;
    གལ་ཏེ (c == '>')
	ཕྱིར HIST_DEBUG;
    ཕྱིར HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, ཧྲིལ idx)
{
    var_na    *short_names = ":=@>?/";
    ཧྲིལ	    short_names_count = (ཧྲིལ)STRLEN(short_names);
    ཧྲིལ	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    གལ་ཏེ (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ཕྱིར xp->xp_buf;
    }
    གལ་ཏེ (idx < short_names_count + history_name_count)
	ཕྱིར (char_u *)history_names[idx - short_names_count];
    གལ་ཏེ (idx == short_names_count + history_name_count)
	ཕྱིར (char_u *)"all";
    ཕྱིར NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    ཧྲིལ		newlen;	    // new length of history table
    histentry_T	*temp;
    ཧྲིལ		type;

    // If size of history table changed, reallocate it
    newlen = (ཧྲིལ)p_hi;
    གལ་ཏེ (newlen == hislen)		// history length didn't change
	ཕྱིར;

    // history length changed
    ཆེད་དུ (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	གལ་ཏེ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    གལ་ཏེ (temp == NULL)   // out of memory!
	    {
		གལ་ཏེ (type == 0)  // first one: just keep the old length
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
	དེ་མིན
	    temp = NULL;

	གལ་ཏེ (hisidx[type] < 0)		// there are no entries yet
	{
	    ཧྲིལ	i;

	    ཆེད་དུ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	དེ་མིན གལ་ཏེ (newlen > hislen)	// array becomes bigger
	{
	    ཧྲིལ	i;
	    ཧྲིལ	j;

	    ཆེད་དུ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    ཆེད་དུ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    ཆེད་དུ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	དེ་མིན				// array becomes smaller or 0
	{
	    ཧྲིལ	i;
	    ཧྲིལ	j;

	    j = hisidx[type];
	    ཆེད་དུ (i = newlen - 1; ; --i)
	    {
		གལ་ཏེ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		དེ་མིན			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		གལ་ཏེ (--j < 0)
		    j = hislen - 1;
		གལ་ཏེ (j == hisidx[type])
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
    ཧྲིལ
in_history(
    ཧྲིལ	    type,
    char_u  *str,
    ཧྲིལ	    move_to_front,	// Move the entry to the front if it exists
    ཧྲིལ	    sep,
    ཧྲིལ	    writing)		// ignore entries read from viminfo
{
    ཧྲིལ	    i;
    ཧྲིལ	    last_i = -1;
    char_u  *p;
    size_t  len;

    གལ་ཏེ (hisidx[type] < 0)
	ཕྱིར FALSE;
    i = hisidx[type];
    do
    {
	གལ་ཏེ (history[type][i].hisstr == NULL)
	    ཕྱིར FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	གལ་ཏེ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    གལ་ཏེ (!move_to_front)
		ཕྱིར TRUE;
	    last_i = i;
	    bha_nga;
	}
	གལ་ཏེ (--i < 0)
	    i = hislen - 1;
    } སྐབས (i != hisidx[type]);

    གལ་ཏེ (last_i < 0)
	ཕྱིར FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    སྐབས (i != hisidx[type])
    {
	གལ་ཏེ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ཕྱིར TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static ཧྲིལ
get_histtype(char_u *name)
{
    ཧྲིལ		i;
    ཧྲིལ		len = (ཧྲིལ)STRLEN(name);

    // No argument: use current history.
    གལ་ཏེ (len == 0)
	ཕྱིར hist_char2type(get_cmdline_firstc());

    ཆེད་དུ (i = 0; history_names[i] != NULL; ++i)
	གལ་ཏེ (STRNICMP(name, history_names[i], len) == 0)
	    ཕྱིར i;

    གལ་ཏེ (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ཕྱིར hist_char2type(name[0]);

    ཕྱིར -1;
}

static ཧྲིལ	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    ཧྲིལ		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ཧྲིལ		in_map,		// consider maptick when inside a mapping
    ཧྲིལ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    གལ་ཏེ (hislen == 0)		// no history
	ཕྱིར;

    གལ་ཏེ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ཕྱིར;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    གལ་ཏེ (histype == HIST_SEARCH && in_map)
    {
	གལ་ཏེ (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    གལ་ཏེ (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    གལ་ཏེ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ཕྱིར;

    གལ་ཏེ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    གལ་ཏེ (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    དེ་མིན
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    གལ་ཏེ (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#གལ་ཏེ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
get_history_idx(ཧྲིལ histype)
{
    གལ་ཏེ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ཕྱིར -1;

    ཕྱིར history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
calc_hist_idx(ཧྲིལ histype, ཧྲིལ num)
{
    ཧྲིལ		i;
    histentry_T	*hist;
    ཧྲིལ		wrapped = FALSE;

    གལ་ཏེ (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ཕྱིར -1;

    hist = history[histype];
    གལ་ཏེ (num > 0)
    {
	སྐབས (hist[i].hisnum > num)
	    གལ་ཏེ (--i < 0)
	    {
		གལ་ཏེ (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	གལ་ཏེ (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ཕྱིར i;
    }
    དེ་མིན གལ་ཏེ (-num <= hislen)
    {
	i += num + 1;
	གལ་ཏེ (i < 0)
	    i += hislen;
	གལ་ཏེ (hist[i].hisstr != NULL)
	    ཕྱིར i;
    }
    ཕྱིར -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
clr_history(ཧྲིལ histype)
{
    ཧྲིལ		i;
    histentry_T	*hisptr;

    གལ་ཏེ (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	ཆེད་དུ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ཕྱིར OK;
    }
    ཕྱིར FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
del_history_entry(ཧྲིལ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ཧྲིལ		idx;
    ཧྲིལ		i;
    ཧྲིལ		last;
    ཧྲིལ		found = FALSE;

    གལ་ཏེ (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ཕྱིར FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    གལ་ཏེ (regmatch.regprog == NULL)
	ཕྱིར FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	གལ་ཏེ (hisptr->hisstr == NULL)
	    bha_nga;
	གལ་ཏེ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	དེ་མིན
	{
	    གལ་ཏེ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    གལ་ཏེ (--last < 0)
		last += hislen;
	}
	གལ་ཏེ (--i < 0)
	    i += hislen;
    } སྐབས (i != idx);

    གལ་ཏེ (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ཕྱིར found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
del_history_idx(ཧྲིལ histype, ཧྲིལ idx)
{
    ཧྲིལ	    i, j;

    i = calc_hist_idx(histype, idx);
    གལ་ཏེ (i < 0)
	ཕྱིར FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    གལ་ཏེ (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    སྐབས (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    གལ་ཏེ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ཕྱིར TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ཧྲིལ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    གལ་ཏེ (check_secure())
	ཕྱིར;

    གལ་ཏེ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ཕྱིར;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    གལ་ཏེ (histype < 0)
	ཕྱིར;

    str = tv_get_string_buf(&argvars[1], buf);
    གལ་ཏེ (*str == NUL)
	ཕྱིར;

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
    ཧྲིལ		n;
    char_u	*str;

    གལ་ཏེ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ཕྱིར;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    གལ་ཏེ (str == NULL)
	n = 0;
    དེ་མིན གལ་ཏེ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    དེ་མིན གལ་ཏེ (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ཧྲིལ)tv_get_number(&argvars[1]));
    དེ་མིན
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

    གལ་ཏེ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ཕྱིར;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    གལ་ཏེ (str == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
    {
	ཧྲིལ type;
	ཧྲིལ idx;

	type = get_histtype(str);
	གལ་ཏེ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	དེ་མིན
	    idx = (ཧྲིལ)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	གལ་ཏེ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	དེ་མིན
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
    ཧྲིལ		i;
    char_u	*histname;

    གལ་ཏེ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ཕྱིར;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    གལ་ཏེ (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    དེ་མིན
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#གལ་ཏེ defined(FEAT_CRYPT)
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
    ཧྲིལ		i;

    i = hisidx[HIST_CMD];
    གལ་ཏེ (i < 0)
	ཕྱིར;
    p_start = history[HIST_CMD][i].hisstr;
    གལ་ཏེ (p_start == NULL)
	ཕྱིར;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    ཆེད་དུ (p = p_start; *p; ++p)
    {
	གལ་ཏེ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    གལ་ཏེ (p == NULL)
		bha_nga;
	    ++p;
	    ཆེད་དུ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		གལ་ཏེ (p[i] == '\\' && p[i + 1])
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
    ཧྲིལ		histype1 = HIST_CMD;
    ཧྲིལ		histype2 = HIST_CMD;
    ཧྲིལ		hisidx1 = 1;
    ཧྲིལ		hisidx2 = -1;
    ཧྲིལ		idx;
    ཧྲིལ		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    གལ་ཏེ (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ཕྱིར;
    }

    གལ་ཏེ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	སྐབས (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	གལ་ཏེ (histype1 == -1)
	{
	    གལ་ཏེ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    དེ་མིན
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ཕྱིར;
	    }
	}
	དེ་མིན
	    histype2 = histype1;
	*end = i;
    }
    དེ་མིན
	end = arg;
    གལ་ཏེ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	གལ་ཏེ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	དེ་མིན
	    semsg(_(e_val_too_large), arg);
	ཕྱིར;
    }

    ཆེད་དུ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	གལ་ཏེ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	གལ་ཏེ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	གལ་ཏེ (idx >= 0 && j <= k)
	    ཆེད་དུ (i = idx + 1; !got_int; ++i)
	    {
		གལ་ཏེ (i == hislen)
		    i = 0;
		གལ་ཏེ (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ཧྲིལ  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    གལ་ཏེ (vim_strsize(hist[i].hisstr) > (ཧྲིལ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ཧྲིལ)Columns - 10, IOSIZE - (ཧྲིལ)len);
		    དེ་མིན
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		གལ་ཏེ (i == idx)
		    bha_nga;
	    }
    }
}
