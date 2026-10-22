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
static ціле	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static ціле	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static ціле	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ціле
get_hislen(void)
{
    повернути hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ціле hist_type)
{
    повернути history[hist_type];
}

#якщо defined(FEAT_VIMINFO)
    void
set_histentry(ціле hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ціле *
get_hisidx(ціле hist_type)
{
    повернути &hisidx[hist_type];
}

#якщо defined(FEAT_VIMINFO)
    ціле *
get_hisnum(ціле hist_type)
{
    повернути &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ціле
hist_char2type(ціле c)
{
    якщо (c == ':')
	повернути HIST_CMD;
    якщо (c == '=')
	повернути HIST_EXPR;
    якщо (c == '@')
	повернути HIST_INPUT;
    якщо (c == '>')
	повернути HIST_DEBUG;
    повернути HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, ціле idx)
{
    char    *short_names = ":=@>?/";
    ціле	    short_names_count = (ціле)STRLEN(short_names);
    ціле	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    якщо (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	повернути xp->xp_buf;
    }
    якщо (idx < short_names_count + history_name_count)
	повернути (char_u *)history_names[idx - short_names_count];
    якщо (idx == short_names_count + history_name_count)
	повернути (char_u *)"all";
    повернути NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    ціле		newlen;	    // new length of history table
    histentry_T	*temp;
    ціле		type;

    // If size of history table changed, reallocate it
    newlen = (ціле)p_hi;
    якщо (newlen == hislen)		// history length didn't change
	повернути;

    // history length changed
    для (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	якщо (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    якщо (temp == NULL)   // out of memory!
	    {
		якщо (type == 0)  // first one: just keep the old length
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
	інакше
	    temp = NULL;

	якщо (hisidx[type] < 0)		// there are no entries yet
	{
	    ціле	i;

	    для (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	інакше якщо (newlen > hislen)	// array becomes bigger
	{
	    ціле	i;
	    ціле	j;

	    для (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    для ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    для ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	інакше				// array becomes smaller or 0
	{
	    ціле	i;
	    ціле	j;

	    j = hisidx[type];
	    для (i = newlen - 1; ; --i)
	    {
		якщо (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		інакше			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		якщо (--j < 0)
		    j = hislen - 1;
		якщо (j == hisidx[type])
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
    ціле
in_history(
    ціле	    type,
    char_u  *str,
    ціле	    move_to_front,	// Move the entry to the front if it exists
    ціле	    sep,
    ціле	    writing)		// ignore entries read from viminfo
{
    ціле	    i;
    ціле	    last_i = -1;
    char_u  *p;
    size_t  len;

    якщо (hisidx[type] < 0)
	повернути FALSE;
    i = hisidx[type];
    do
    {
	якщо (history[type][i].hisstr == NULL)
	    повернути FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	якщо (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    якщо (!move_to_front)
		повернути TRUE;
	    last_i = i;
	    break;
	}
	якщо (--i < 0)
	    i = hislen - 1;
    } поки (i != hisidx[type]);

    якщо (last_i < 0)
	повернути FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    поки (i != hisidx[type])
    {
	якщо (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    повернути TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static ціле
get_histtype(char_u *name)
{
    ціле		i;
    ціле		len = (ціле)STRLEN(name);

    // No argument: use current history.
    якщо (len == 0)
	повернути hist_char2type(get_cmdline_firstc());

    для (i = 0; history_names[i] != NULL; ++i)
	якщо (STRNICMP(name, history_names[i], len) == 0)
	    повернути i;

    якщо (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	повернути hist_char2type(name[0]);

    повернути -1;
}

static ціле	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    ціле		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ціле		in_map,		// consider maptick when inside a mapping
    ціле		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    якщо (hislen == 0)		// no history
	повернути;

    якщо ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	повернути;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    якщо (histype == HIST_SEARCH && in_map)
    {
	якщо (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    якщо (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    якщо (in_history(histype, new_entry, TRUE, sep, FALSE))
	повернути;

    якщо (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    якщо (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    інакше
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    якщо (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#якщо defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static ціле
get_history_idx(ціле histype)
{
    якщо (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	повернути -1;

    повернути history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static ціле
calc_hist_idx(ціле histype, ціле num)
{
    ціле		i;
    histentry_T	*hist;
    ціле		wrapped = FALSE;

    якщо (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	повернути -1;

    hist = history[histype];
    якщо (num > 0)
    {
	поки (hist[i].hisnum > num)
	    якщо (--i < 0)
	    {
		якщо (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	якщо (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    повернути i;
    }
    інакше якщо (-num <= hislen)
    {
	i += num + 1;
	якщо (i < 0)
	    i += hislen;
	якщо (hist[i].hisstr != NULL)
	    повернути i;
    }
    повернути -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static ціле
clr_history(ціле histype)
{
    ціле		i;
    histentry_T	*hisptr;

    якщо (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	для (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	повернути OK;
    }
    повернути FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ціле
del_history_entry(ціле histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ціле		idx;
    ціле		i;
    ціле		last;
    ціле		found = FALSE;

    якщо (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	повернути FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    якщо (regmatch.regprog == NULL)
	повернути FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	якщо (hisptr->hisstr == NULL)
	    break;
	якщо (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	інакше
	{
	    якщо (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    якщо (--last < 0)
		last += hislen;
	}
	якщо (--i < 0)
	    i += hislen;
    } поки (i != idx);

    якщо (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    повернути found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static ціле
del_history_idx(ціле histype, ціле idx)
{
    ціле	    i, j;

    i = calc_hist_idx(histype, idx);
    якщо (i < 0)
	повернути FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    якщо (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    поки (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    якщо (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    повернути TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ціле		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    якщо (check_secure())
	повернути;

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	повернути;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    якщо (histype < 0)
	повернути;

    str = tv_get_string_buf(&argvars[1], buf);
    якщо (*str == NUL)
	повернути;

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
    ціле		n;
    char_u	*str;

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	повернути;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    якщо (str == NULL)
	n = 0;
    інакше якщо (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    інакше якщо (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ціле)tv_get_number(&argvars[1]));
    інакше
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

    якщо (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	повернути;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    якщо (str == NULL)
	rettv->vval.v_string = NULL;
    інакше
    {
	ціле type;
	ціле idx;

	type = get_histtype(str);
	якщо (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	інакше
	    idx = (ціле)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	якщо (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	інакше
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
    ціле		i;
    char_u	*histname;

    якщо (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	повернути;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    якщо (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    інакше
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#якщо defined(FEAT_CRYPT)
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
    ціле		i;

    i = hisidx[HIST_CMD];
    якщо (i < 0)
	повернути;
    p_start = history[HIST_CMD][i].hisstr;
    якщо (p_start == NULL)
	повернути;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    для (p = p_start; *p; ++p)
    {
	якщо (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    якщо (p == NULL)
		break;
	    ++p;
	    для (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		якщо (p[i] == '\\' && p[i + 1])
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
    ціле		histype1 = HIST_CMD;
    ціле		histype2 = HIST_CMD;
    ціле		hisidx1 = 1;
    ціле		hisidx2 = -1;
    ціле		idx;
    ціле		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    якщо (hislen == 0)
    {
	msg(_("'history' option is zero"));
	повернути;
    }

    якщо (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	поки (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	якщо (histype1 == -1)
	{
	    якщо (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    інакше
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		повернути;
	    }
	}
	інакше
	    histype2 = histype1;
	*end = i;
    }
    інакше
	end = arg;
    якщо (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	якщо (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	інакше
	    semsg(_(e_val_too_large), arg);
	повернути;
    }

    для (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	якщо (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	якщо (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	якщо (idx >= 0 && j <= k)
	    для (i = idx + 1; !got_int; ++i)
	    {
		якщо (i == hislen)
		    i = 0;
		якщо (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ціле  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    якщо (vim_strsize(hist[i].hisstr) > (ціле)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ціле)Columns - 10, IOSIZE - (ціле)len);
		    інакше
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		якщо (i == idx)
		    break;
	    }
    }
}
