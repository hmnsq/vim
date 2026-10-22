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
    ལོག hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ཧྲིལ hist_type)
{
    ལོག history[hist_type];
}

#གལ་སྲིད defined(FEAT_VIMINFO)
    void
set_histentry(ཧྲིལ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ཧྲིལ *
get_hisidx(ཧྲིལ hist_type)
{
    ལོག &hisidx[hist_type];
}

#གལ་སྲིད defined(FEAT_VIMINFO)
    ཧྲིལ *
get_hisnum(ཧྲིལ hist_type)
{
    ལོག &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ཧྲིལ
hist_char2type(ཧྲིལ c)
{
    གལ་སྲིད (c == ':')
	ལོག HIST_CMD;
    གལ་སྲིད (c == '=')
	ལོག HIST_EXPR;
    གལ་སྲིད (c == '@')
	ལོག HIST_INPUT;
    གལ་སྲིད (c == '>')
	ལོག HIST_DEBUG;
    ལོག HIST_SEARCH;	    // must be '?' or '/'
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

    གལ་སྲིད (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ལོག xp->xp_buf;
    }
    གལ་སྲིད (idx < short_names_count + history_name_count)
	ལོག (char_u *)history_names[idx - short_names_count];
    གལ་སྲིད (idx == short_names_count + history_name_count)
	ལོག (char_u *)"all";
    ལོག NULL;
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
    གལ་སྲིད (newlen == hislen)		// history length didn't change
	ལོག;

    // history length changed
    དོན་ལུ (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	གལ་སྲིད (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    གལ་སྲིད (temp == NULL)   // out of memory!
	    {
		གལ་སྲིད (type == 0)  // first one: just keep the old length
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

	གལ་སྲིད (hisidx[type] < 0)		// there are no entries yet
	{
	    ཧྲིལ	i;

	    དོན་ལུ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	དེ་མིན གལ་སྲིད (newlen > hislen)	// array becomes bigger
	{
	    ཧྲིལ	i;
	    ཧྲིལ	j;

	    དོན་ལུ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    དོན་ལུ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    དོན་ལུ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	དེ་མིན				// array becomes smaller or 0
	{
	    ཧྲིལ	i;
	    ཧྲིལ	j;

	    j = hisidx[type];
	    དོན་ལུ (i = newlen - 1; ; --i)
	    {
		གལ་སྲིད (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		དེ་མིན			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		གལ་སྲིད (--j < 0)
		    j = hislen - 1;
		གལ་སྲིད (j == hisidx[type])
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

    གལ་སྲིད (hisidx[type] < 0)
	ལོག FALSE;
    i = hisidx[type];
    do
    {
	གལ་སྲིད (history[type][i].hisstr == NULL)
	    ལོག FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	གལ་སྲིད (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    གལ་སྲིད (!move_to_front)
		ལོག TRUE;
	    last_i = i;
	    bha_nga;
	}
	གལ་སྲིད (--i < 0)
	    i = hislen - 1;
    } སྐབས (i != hisidx[type]);

    གལ་སྲིད (last_i < 0)
	ལོག FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    སྐབས (i != hisidx[type])
    {
	གལ་སྲིད (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ལོག TRUE;
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
    གལ་སྲིད (len == 0)
	ལོག hist_char2type(get_cmdline_firstc());

    དོན་ལུ (i = 0; history_names[i] != NULL; ++i)
	གལ་སྲིད (STRNICMP(name, history_names[i], len) == 0)
	    ལོག i;

    གལ་སྲིད (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	ལོག hist_char2type(name[0]);

    ལོག -1;
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

    གལ་སྲིད (hislen == 0)		// no history
	ལོག;

    གལ་སྲིད ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	ལོག;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    གལ་སྲིད (histype == HIST_SEARCH && in_map)
    {
	གལ་སྲིད (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    གལ་སྲིད (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    གལ་སྲིད (in_history(histype, new_entry, TRUE, sep, FALSE))
	ལོག;

    གལ་སྲིད (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    གལ་སྲིད (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    དེ་མིན
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    གལ་སྲིད (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#གལ་སྲིད defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ཧྲིལ
get_history_idx(ཧྲིལ histype)
{
    གལ་སྲིད (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	ལོག -1;

    ལོག history[histype][hisidx[histype]].hisnum;
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

    གལ་སྲིད (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	ལོག -1;

    hist = history[histype];
    གལ་སྲིད (num > 0)
    {
	སྐབས (hist[i].hisnum > num)
	    གལ་སྲིད (--i < 0)
	    {
		གལ་སྲིད (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	གལ་སྲིད (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    ལོག i;
    }
    དེ་མིན གལ་སྲིད (-num <= hislen)
    {
	i += num + 1;
	གལ་སྲིད (i < 0)
	    i += hislen;
	གལ་སྲིད (hist[i].hisstr != NULL)
	    ལོག i;
    }
    ལོག -1;
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

    གལ་སྲིད (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	དོན་ལུ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ལོག OK;
    }
    ལོག FAIL;
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

    གལ་སྲིད (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	ལོག FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    གལ་སྲིད (regmatch.regprog == NULL)
	ལོག FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	གལ་སྲིད (hisptr->hisstr == NULL)
	    bha_nga;
	གལ་སྲིད (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	དེ་མིན
	{
	    གལ་སྲིད (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    གལ་སྲིད (--last < 0)
		last += hislen;
	}
	གལ་སྲིད (--i < 0)
	    i += hislen;
    } སྐབས (i != idx);

    གལ་སྲིད (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ལོག found;
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
    གལ་སྲིད (i < 0)
	ལོག FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    གལ་སྲིད (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    སྐབས (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    གལ་སྲིད (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ལོག TRUE;
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
    གལ་སྲིད (check_secure())
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ལོག;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    གལ་སྲིད (histype < 0)
	ལོག;

    str = tv_get_string_buf(&argvars[1], buf);
    གལ་སྲིད (*str == NUL)
	ལོག;

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

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	ལོག;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    གལ་སྲིད (str == NULL)
	n = 0;
    དེ་མིན གལ་སྲིད (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    དེ་མིན གལ་སྲིད (argvars[1].v_type == VAR_NUMBER)
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

    གལ་སྲིད (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ལོག;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    གལ་སྲིད (str == NULL)
	rettv->vval.v_string = NULL;
    དེ་མིན
    {
	ཧྲིལ type;
	ཧྲིལ idx;

	type = get_histtype(str);
	གལ་སྲིད (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	དེ་མིན
	    idx = (ཧྲིལ)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	གལ་སྲིད (idx < 0)
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

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    གལ་སྲིད (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    དེ་མིན
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#གལ་སྲིད defined(FEAT_CRYPT)
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
    གལ་སྲིད (i < 0)
	ལོག;
    p_start = history[HIST_CMD][i].hisstr;
    གལ་སྲིད (p_start == NULL)
	ལོག;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    དོན་ལུ (p = p_start; *p; ++p)
    {
	གལ་སྲིད (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    གལ་སྲིད (p == NULL)
		bha_nga;
	    ++p;
	    དོན་ལུ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		གལ་སྲིད (p[i] == '\\' && p[i + 1])
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

    གལ་སྲིད (hislen == 0)
    {
	msg(_("'history' option is zero"));
	ལོག;
    }

    གལ་སྲིད (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	སྐབས (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	གལ་སྲིད (histype1 == -1)
	{
	    གལ་སྲིད (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    དེ་མིན
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ལོག;
	    }
	}
	དེ་མིན
	    histype2 = histype1;
	*end = i;
    }
    དེ་མིན
	end = arg;
    གལ་སྲིད (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	གལ་སྲིད (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	དེ་མིན
	    semsg(_(e_val_too_large), arg);
	ལོག;
    }

    དོན་ལུ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	གལ་སྲིད (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	གལ་སྲིད (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	གལ་སྲིད (idx >= 0 && j <= k)
	    དོན་ལུ (i = idx + 1; !got_int; ++i)
	    {
		གལ་སྲིད (i == hislen)
		    i = 0;
		གལ་སྲིད (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ཧྲིལ  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    གལ་སྲིད (vim_strsize(hist[i].hisstr) > (ཧྲིལ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ཧྲིལ)Columns - 10, IOSIZE - (ཧྲིལ)len);
		    དེ་མིན
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		གལ་སྲིད (i == idx)
		    bha_nga;
	    }
    }
}
