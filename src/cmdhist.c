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
static 整数	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static 整数	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static 整数	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    整数
get_hislen(void)
{
    返回 hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(整数 hist_type)
{
    返回 history[hist_type];
}

#如果 defined(FEAT_VIMINFO)
    void
set_histentry(整数 hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    整数 *
get_hisidx(整数 hist_type)
{
    返回 &hisidx[hist_type];
}

#如果 defined(FEAT_VIMINFO)
    整数 *
get_hisnum(整数 hist_type)
{
    返回 &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    整数
hist_char2type(整数 c)
{
    如果 (c == ':')
	返回 HIST_CMD;
    如果 (c == '=')
	返回 HIST_EXPR;
    如果 (c == '@')
	返回 HIST_INPUT;
    如果 (c == '>')
	返回 HIST_DEBUG;
    返回 HIST_SEARCH;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

static char *(history_names[]) =
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
get_history_arg(expand_T *xp UNUSED, 整数 idx)
{
    char    *short_names = ":=@>?/";
    整数	    short_names_count = (整数)STRLEN(short_names);
    整数	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    如果 (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	返回 xp->xp_buf;
    }
    如果 (idx < short_names_count + history_name_count)
	返回 (char_u *)history_names[idx - short_names_count];
    如果 (idx == short_names_count + history_name_count)
	返回 (char_u *)"all";
    返回 NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    整数		newlen;	    // new length of history table
    histentry_T	*temp;
    整数		type;

    // If size of history table changed, reallocate it
    newlen = (整数)p_hi;
    如果 (newlen == hislen)		// history length didn't change
	返回;

    // history length changed
    为 (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	如果 (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    如果 (temp == NULL)   // out of memory!
	    {
		如果 (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    break;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		continue;
	    }
	}
	否则
	    temp = NULL;

	如果 (hisidx[type] < 0)		// there are no entries yet
	{
	    整数	i;

	    为 (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	否则 如果 (newlen > hislen)	// array becomes bigger
	{
	    整数	i;
	    整数	j;

	    为 (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    为 ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    为 ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	否则				// array becomes smaller or 0
	{
	    整数	i;
	    整数	j;

	    j = hisidx[type];
	    为 (i = newlen - 1; ; --i)
	    {
		如果 (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		否则			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		如果 (--j < 0)
		    j = hislen - 1;
		如果 (j == hisidx[type])
		    break;
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
    整数
in_history(
    整数	    type,
    char_u  *str,
    整数	    move_to_front,	// Move the entry to the front if it exists
    整数	    sep,
    整数	    writing)		// ignore entries read from viminfo
{
    整数	    i;
    整数	    last_i = -1;
    char_u  *p;
    size_t  len;

    如果 (hisidx[type] < 0)
	返回 FALSE;
    i = hisidx[type];
    do
    {
	如果 (history[type][i].hisstr == NULL)
	    返回 FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	如果 (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    如果 (!move_to_front)
		返回 TRUE;
	    last_i = i;
	    break;
	}
	如果 (--i < 0)
	    i = hislen - 1;
    } 当 (i != hisidx[type]);

    如果 (last_i < 0)
	返回 FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    当 (i != hisidx[type])
    {
	如果 (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    返回 TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static 整数
get_histtype(char_u *name)
{
    整数		i;
    整数		len = (整数)STRLEN(name);

    // No argument: use current history.
    如果 (len == 0)
	返回 hist_char2type(get_cmdline_firstc());

    为 (i = 0; history_names[i] != NULL; ++i)
	如果 (STRNICMP(name, history_names[i], len) == 0)
	    返回 i;

    如果 (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	返回 hist_char2type(name[0]);

    返回 -1;
}

static 整数	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    整数		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    整数		in_map,		// consider maptick when inside a mapping
    整数		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    如果 (hislen == 0)		// no history
	返回;

    如果 ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	返回;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    如果 (histype == HIST_SEARCH && in_map)
    {
	如果 (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    如果 (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    如果 (in_history(histype, new_entry, TRUE, sep, FALSE))
	返回;

    如果 (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    如果 (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    否则
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    如果 (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#如果 defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static 整数
get_history_idx(整数 histype)
{
    如果 (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	返回 -1;

    返回 history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static 整数
calc_hist_idx(整数 histype, 整数 num)
{
    整数		i;
    histentry_T	*hist;
    整数		wrapped = FALSE;

    如果 (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	返回 -1;

    hist = history[histype];
    如果 (num > 0)
    {
	当 (hist[i].hisnum > num)
	    如果 (--i < 0)
	    {
		如果 (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	如果 (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    返回 i;
    }
    否则 如果 (-num <= hislen)
    {
	i += num + 1;
	如果 (i < 0)
	    i += hislen;
	如果 (hist[i].hisstr != NULL)
	    返回 i;
    }
    返回 -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static 整数
clr_history(整数 histype)
{
    整数		i;
    histentry_T	*hisptr;

    如果 (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	为 (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	返回 OK;
    }
    返回 FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static 整数
del_history_entry(整数 histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    整数		idx;
    整数		i;
    整数		last;
    整数		found = FALSE;

    如果 (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	返回 FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    如果 (regmatch.regprog == NULL)
	返回 FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	如果 (hisptr->hisstr == NULL)
	    break;
	如果 (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	否则
	{
	    如果 (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    如果 (--last < 0)
		last += hislen;
	}
	如果 (--i < 0)
	    i += hislen;
    } 当 (i != idx);

    如果 (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    返回 found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static 整数
del_history_idx(整数 histype, 整数 idx)
{
    整数	    i, j;

    i = calc_hist_idx(histype, idx);
    如果 (i < 0)
	返回 FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    如果 (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    当 (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    如果 (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    返回 TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    整数		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    如果 (check_secure())
	返回;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	返回;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    如果 (histype < 0)
	返回;

    str = tv_get_string_buf(&argvars[1], buf);
    如果 (*str == NUL)
	返回;

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
    整数		n;
    char_u	*str;

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	返回;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    如果 (str == NULL)
	n = 0;
    否则 如果 (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    否则 如果 (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (整数)tv_get_number(&argvars[1]));
    否则
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

    如果 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	返回;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    如果 (str == NULL)
	rettv->vval.v_string = NULL;
    否则
    {
	整数 type;
	整数 idx;

	type = get_histtype(str);
	如果 (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	否则
	    idx = (整数)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	如果 (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	否则
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
    整数		i;
    char_u	*histname;

    如果 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	返回;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    如果 (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    否则
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#如果 defined(FEAT_CRYPT)
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
    整数		i;

    i = hisidx[HIST_CMD];
    如果 (i < 0)
	返回;
    p_start = history[HIST_CMD][i].hisstr;
    如果 (p_start == NULL)
	返回;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    为 (p = p_start; *p; ++p)
    {
	如果 (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    如果 (p == NULL)
		break;
	    ++p;
	    为 (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		如果 (p[i] == '\\' && p[i + 1])
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
    整数		histype1 = HIST_CMD;
    整数		histype2 = HIST_CMD;
    整数		hisidx1 = 1;
    整数		hisidx2 = -1;
    整数		idx;
    整数		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    如果 (hislen == 0)
    {
	msg(_("'history' option is zero"));
	返回;
    }

    如果 (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	当 (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	如果 (histype1 == -1)
	{
	    如果 (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    否则
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		返回;
	    }
	}
	否则
	    histype2 = histype1;
	*end = i;
    }
    否则
	end = arg;
    如果 (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	如果 (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	否则
	    semsg(_(e_val_too_large), arg);
	返回;
    }

    为 (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	如果 (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	如果 (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	如果 (idx >= 0 && j <= k)
	    为 (i = idx + 1; !got_int; ++i)
	    {
		如果 (i == hislen)
		    i = 0;
		如果 (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    整数  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    如果 (vim_strsize(hist[i].hisstr) > (整数)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (整数)Columns - 10, IOSIZE - (整数)len);
		    否则
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		如果 (i == idx)
		    break;
	    }
    }
}
