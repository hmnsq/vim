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
static จำนวนเต็ม	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static จำนวนเต็ม	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static จำนวนเต็ม	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    จำนวนเต็ม
get_hislen(void)
{
    คืน hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(จำนวนเต็ม hist_type)
{
    คืน history[hist_type];
}

#ถ้า defined(FEAT_VIMINFO)
    void
set_histentry(จำนวนเต็ม hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    จำนวนเต็ม *
get_hisidx(จำนวนเต็ม hist_type)
{
    คืน &hisidx[hist_type];
}

#ถ้า defined(FEAT_VIMINFO)
    จำนวนเต็ม *
get_hisnum(จำนวนเต็ม hist_type)
{
    คืน &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    จำนวนเต็ม
hist_char2type(จำนวนเต็ม c)
{
    ถ้า (c == ':')
	คืน HIST_CMD;
    ถ้า (c == '=')
	คืน HIST_EXPR;
    ถ้า (c == '@')
	คืน HIST_INPUT;
    ถ้า (c == '>')
	คืน HIST_DEBUG;
    คืน HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, จำนวนเต็ม idx)
{
    var_na    *short_names = ":=@>?/";
    จำนวนเต็ม	    short_names_count = (จำนวนเต็ม)STRLEN(short_names);
    จำนวนเต็ม	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ถ้า (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	คืน xp->xp_buf;
    }
    ถ้า (idx < short_names_count + history_name_count)
	คืน (char_u *)history_names[idx - short_names_count];
    ถ้า (idx == short_names_count + history_name_count)
	คืน (char_u *)"all";
    คืน NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    จำนวนเต็ม		newlen;	    // new length of history table
    histentry_T	*temp;
    จำนวนเต็ม		type;

    // If size of history table changed, reallocate it
    newlen = (จำนวนเต็ม)p_hi;
    ถ้า (newlen == hislen)		// history length didn't change
	คืน;

    // history length changed
    สำหรับ (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ถ้า (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ถ้า (temp == NULL)   // out of memory!
	    {
		ถ้า (type == 0)  // first one: just keep the old length
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
	มิฉะนั้น
	    temp = NULL;

	ถ้า (hisidx[type] < 0)		// there are no entries yet
	{
	    จำนวนเต็ม	i;

	    สำหรับ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	มิฉะนั้น ถ้า (newlen > hislen)	// array becomes bigger
	{
	    จำนวนเต็ม	i;
	    จำนวนเต็ม	j;

	    สำหรับ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    สำหรับ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    สำหรับ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	มิฉะนั้น				// array becomes smaller or 0
	{
	    จำนวนเต็ม	i;
	    จำนวนเต็ม	j;

	    j = hisidx[type];
	    สำหรับ (i = newlen - 1; ; --i)
	    {
		ถ้า (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		มิฉะนั้น			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ถ้า (--j < 0)
		    j = hislen - 1;
		ถ้า (j == hisidx[type])
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
    จำนวนเต็ม
in_history(
    จำนวนเต็ม	    type,
    char_u  *str,
    จำนวนเต็ม	    move_to_front,	// Move the entry to the front if it exists
    จำนวนเต็ม	    sep,
    จำนวนเต็ม	    writing)		// ignore entries read from viminfo
{
    จำนวนเต็ม	    i;
    จำนวนเต็ม	    last_i = -1;
    char_u  *p;
    size_t  len;

    ถ้า (hisidx[type] < 0)
	คืน FALSE;
    i = hisidx[type];
    do
    {
	ถ้า (history[type][i].hisstr == NULL)
	    คืน FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ถ้า (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ถ้า (!move_to_front)
		คืน TRUE;
	    last_i = i;
	    bha_nga;
	}
	ถ้า (--i < 0)
	    i = hislen - 1;
    } ขณะ (i != hisidx[type]);

    ถ้า (last_i < 0)
	คืน FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ขณะ (i != hisidx[type])
    {
	ถ้า (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    คืน TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static จำนวนเต็ม
get_histtype(char_u *name)
{
    จำนวนเต็ม		i;
    จำนวนเต็ม		len = (จำนวนเต็ม)STRLEN(name);

    // No argument: use current history.
    ถ้า (len == 0)
	คืน hist_char2type(get_cmdline_firstc());

    สำหรับ (i = 0; history_names[i] != NULL; ++i)
	ถ้า (STRNICMP(name, history_names[i], len) == 0)
	    คืน i;

    ถ้า (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	คืน hist_char2type(name[0]);

    คืน -1;
}

static จำนวนเต็ม	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    จำนวนเต็ม		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    จำนวนเต็ม		in_map,		// consider maptick when inside a mapping
    จำนวนเต็ม		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ถ้า (hislen == 0)		// no history
	คืน;

    ถ้า ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	คืน;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ถ้า (histype == HIST_SEARCH && in_map)
    {
	ถ้า (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ถ้า (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ถ้า (in_history(histype, new_entry, TRUE, sep, FALSE))
	คืน;

    ถ้า (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ถ้า (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    มิฉะนั้น
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ถ้า (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ถ้า defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static จำนวนเต็ม
get_history_idx(จำนวนเต็ม histype)
{
    ถ้า (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	คืน -1;

    คืน history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static จำนวนเต็ม
calc_hist_idx(จำนวนเต็ม histype, จำนวนเต็ม num)
{
    จำนวนเต็ม		i;
    histentry_T	*hist;
    จำนวนเต็ม		wrapped = FALSE;

    ถ้า (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	คืน -1;

    hist = history[histype];
    ถ้า (num > 0)
    {
	ขณะ (hist[i].hisnum > num)
	    ถ้า (--i < 0)
	    {
		ถ้า (wrapped)
		    bha_nga;
		i += hislen;
		wrapped = TRUE;
	    }
	ถ้า (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    คืน i;
    }
    มิฉะนั้น ถ้า (-num <= hislen)
    {
	i += num + 1;
	ถ้า (i < 0)
	    i += hislen;
	ถ้า (hist[i].hisstr != NULL)
	    คืน i;
    }
    คืน -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static จำนวนเต็ม
clr_history(จำนวนเต็ม histype)
{
    จำนวนเต็ม		i;
    histentry_T	*hisptr;

    ถ้า (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	สำหรับ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	คืน OK;
    }
    คืน FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static จำนวนเต็ม
del_history_entry(จำนวนเต็ม histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    จำนวนเต็ม		idx;
    จำนวนเต็ม		i;
    จำนวนเต็ม		last;
    จำนวนเต็ม		found = FALSE;

    ถ้า (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	คืน FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ถ้า (regmatch.regprog == NULL)
	คืน FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	ถ้า (hisptr->hisstr == NULL)
	    bha_nga;
	ถ้า (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	มิฉะนั้น
	{
	    ถ้า (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ถ้า (--last < 0)
		last += hislen;
	}
	ถ้า (--i < 0)
	    i += hislen;
    } ขณะ (i != idx);

    ถ้า (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    คืน found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static จำนวนเต็ม
del_history_idx(จำนวนเต็ม histype, จำนวนเต็ม idx)
{
    จำนวนเต็ม	    i, j;

    i = calc_hist_idx(histype, idx);
    ถ้า (i < 0)
	คืน FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ถ้า (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ขณะ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ถ้า (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    คืน TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    จำนวนเต็ม		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    ถ้า (histype < 0)
	คืน;

    str = tv_get_string_buf(&argvars[1], buf);
    ถ้า (*str == NUL)
	คืน;

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
    จำนวนเต็ม		n;
    char_u	*str;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	คืน;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ถ้า (str == NULL)
	n = 0;
    มิฉะนั้น ถ้า (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    มิฉะนั้น ถ้า (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (จำนวนเต็ม)tv_get_number(&argvars[1]));
    มิฉะนั้น
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

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	คืน;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ถ้า (str == NULL)
	rettv->vval.v_string = NULL;
    มิฉะนั้น
    {
	จำนวนเต็ม type;
	จำนวนเต็ม idx;

	type = get_histtype(str);
	ถ้า (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	มิฉะนั้น
	    idx = (จำนวนเต็ม)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ถ้า (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	มิฉะนั้น
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
    จำนวนเต็ม		i;
    char_u	*histname;

    ถ้า (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	คืน;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    ถ้า (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    มิฉะนั้น
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ถ้า defined(FEAT_CRYPT)
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
    จำนวนเต็ม		i;

    i = hisidx[HIST_CMD];
    ถ้า (i < 0)
	คืน;
    p_start = history[HIST_CMD][i].hisstr;
    ถ้า (p_start == NULL)
	คืน;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    สำหรับ (p = p_start; *p; ++p)
    {
	ถ้า (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ถ้า (p == NULL)
		bha_nga;
	    ++p;
	    สำหรับ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ถ้า (p[i] == '\\' && p[i + 1])
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
    จำนวนเต็ม		histype1 = HIST_CMD;
    จำนวนเต็ม		histype2 = HIST_CMD;
    จำนวนเต็ม		hisidx1 = 1;
    จำนวนเต็ม		hisidx2 = -1;
    จำนวนเต็ม		idx;
    จำนวนเต็ม		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ถ้า (hislen == 0)
    {
	msg(_("'history' option is zero"));
	คืน;
    }

    ถ้า (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ขณะ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ถ้า (histype1 == -1)
	{
	    ถ้า (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    มิฉะนั้น
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		คืน;
	    }
	}
	มิฉะนั้น
	    histype2 = histype1;
	*end = i;
    }
    มิฉะนั้น
	end = arg;
    ถ้า (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ถ้า (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	มิฉะนั้น
	    semsg(_(e_val_too_large), arg);
	คืน;
    }

    สำหรับ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((var_na *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((var_na *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ถ้า (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ถ้า (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ถ้า (idx >= 0 && j <= k)
	    สำหรับ (i = idx + 1; !got_int; ++i)
	    {
		ถ้า (i == hislen)
		    i = 0;
		ถ้า (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    จำนวนเต็ม  len;

		    msg_putchar('\n');
		    len = vim_snprintf((var_na *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ถ้า (vim_strsize(hist[i].hisstr) > (จำนวนเต็ม)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (จำนวนเต็ม)Columns - 10, IOSIZE - (จำนวนเต็ม)len);
		    มิฉะนั้น
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ถ้า (i == idx)
		    bha_nga;
	    }
    }
}
