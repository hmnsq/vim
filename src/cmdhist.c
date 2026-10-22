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
static целое	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static целое	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static целое	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    целое
get_hislen(void)
{
    вернуть hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(целое hist_type)
{
    вернуть history[hist_type];
}

#если defined(FEAT_VIMINFO)
    void
set_histentry(целое hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    целое *
get_hisidx(целое hist_type)
{
    вернуть &hisidx[hist_type];
}

#если defined(FEAT_VIMINFO)
    целое *
get_hisnum(целое hist_type)
{
    вернуть &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    целое
hist_char2type(целое c)
{
    если (c == ':')
	вернуть HIST_CMD;
    если (c == '=')
	вернуть HIST_EXPR;
    если (c == '@')
	вернуть HIST_INPUT;
    если (c == '>')
	вернуть HIST_DEBUG;
    вернуть HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, целое idx)
{
    char    *short_names = ":=@>?/";
    целое	    short_names_count = (целое)STRLEN(short_names);
    целое	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    если (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	вернуть xp->xp_buf;
    }
    если (idx < short_names_count + history_name_count)
	вернуть (char_u *)history_names[idx - short_names_count];
    если (idx == short_names_count + history_name_count)
	вернуть (char_u *)"all";
    вернуть NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    целое		newlen;	    // new length of history table
    histentry_T	*temp;
    целое		type;

    // If size of history table changed, reallocate it
    newlen = (целое)p_hi;
    если (newlen == hislen)		// history length didn't change
	вернуть;

    // history length changed
    для (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	если (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    если (temp == NULL)   // out of memory!
	    {
		если (type == 0)  // first one: just keep the old length
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
	иначе
	    temp = NULL;

	если (hisidx[type] < 0)		// there are no entries yet
	{
	    целое	i;

	    для (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	иначе если (newlen > hislen)	// array becomes bigger
	{
	    целое	i;
	    целое	j;

	    для (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    для ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    для ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	иначе				// array becomes smaller or 0
	{
	    целое	i;
	    целое	j;

	    j = hisidx[type];
	    для (i = newlen - 1; ; --i)
	    {
		если (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		иначе			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		если (--j < 0)
		    j = hislen - 1;
		если (j == hisidx[type])
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
    целое
in_history(
    целое	    type,
    char_u  *str,
    целое	    move_to_front,	// Move the entry to the front if it exists
    целое	    sep,
    целое	    writing)		// ignore entries read from viminfo
{
    целое	    i;
    целое	    last_i = -1;
    char_u  *p;
    size_t  len;

    если (hisidx[type] < 0)
	вернуть FALSE;
    i = hisidx[type];
    do
    {
	если (history[type][i].hisstr == NULL)
	    вернуть FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	если (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    если (!move_to_front)
		вернуть TRUE;
	    last_i = i;
	    break;
	}
	если (--i < 0)
	    i = hislen - 1;
    } пока (i != hisidx[type]);

    если (last_i < 0)
	вернуть FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    пока (i != hisidx[type])
    {
	если (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    вернуть TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static целое
get_histtype(char_u *name)
{
    целое		i;
    целое		len = (целое)STRLEN(name);

    // No argument: use current history.
    если (len == 0)
	вернуть hist_char2type(get_cmdline_firstc());

    для (i = 0; history_names[i] != NULL; ++i)
	если (STRNICMP(name, history_names[i], len) == 0)
	    вернуть i;

    если (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	вернуть hist_char2type(name[0]);

    вернуть -1;
}

static целое	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    целое		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    целое		in_map,		// consider maptick when inside a mapping
    целое		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    если (hislen == 0)		// no history
	вернуть;

    если ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	вернуть;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    если (histype == HIST_SEARCH && in_map)
    {
	если (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    если (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    если (in_history(histype, new_entry, TRUE, sep, FALSE))
	вернуть;

    если (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    если (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    иначе
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    если (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#если defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static целое
get_history_idx(целое histype)
{
    если (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	вернуть -1;

    вернуть history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static целое
calc_hist_idx(целое histype, целое num)
{
    целое		i;
    histentry_T	*hist;
    целое		wrapped = FALSE;

    если (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	вернуть -1;

    hist = history[histype];
    если (num > 0)
    {
	пока (hist[i].hisnum > num)
	    если (--i < 0)
	    {
		если (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	если (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    вернуть i;
    }
    иначе если (-num <= hislen)
    {
	i += num + 1;
	если (i < 0)
	    i += hislen;
	если (hist[i].hisstr != NULL)
	    вернуть i;
    }
    вернуть -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static целое
clr_history(целое histype)
{
    целое		i;
    histentry_T	*hisptr;

    если (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
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
	вернуть OK;
    }
    вернуть FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static целое
del_history_entry(целое histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    целое		idx;
    целое		i;
    целое		last;
    целое		found = FALSE;

    если (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	вернуть FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    если (regmatch.regprog == NULL)
	вернуть FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	если (hisptr->hisstr == NULL)
	    break;
	если (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	иначе
	{
	    если (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    если (--last < 0)
		last += hislen;
	}
	если (--i < 0)
	    i += hislen;
    } пока (i != idx);

    если (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    вернуть found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static целое
del_history_idx(целое histype, целое idx)
{
    целое	    i, j;

    i = calc_hist_idx(histype, idx);
    если (i < 0)
	вернуть FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    если (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    пока (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    если (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    вернуть TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    целое		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    если (check_secure())
	вернуть;

    если (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	вернуть;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    если (histype < 0)
	вернуть;

    str = tv_get_string_buf(&argvars[1], buf);
    если (*str == NUL)
	вернуть;

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
    целое		n;
    char_u	*str;

    если (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	вернуть;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    если (str == NULL)
	n = 0;
    иначе если (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    иначе если (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (целое)tv_get_number(&argvars[1]));
    иначе
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

    если (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	вернуть;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    если (str == NULL)
	rettv->vval.v_string = NULL;
    иначе
    {
	целое type;
	целое idx;

	type = get_histtype(str);
	если (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	иначе
	    idx = (целое)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	если (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	иначе
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
    целое		i;
    char_u	*histname;

    если (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	вернуть;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    если (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    иначе
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#если defined(FEAT_CRYPT)
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
    целое		i;

    i = hisidx[HIST_CMD];
    если (i < 0)
	вернуть;
    p_start = history[HIST_CMD][i].hisstr;
    если (p_start == NULL)
	вернуть;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    для (p = p_start; *p; ++p)
    {
	если (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    если (p == NULL)
		break;
	    ++p;
	    для (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		если (p[i] == '\\' && p[i + 1])
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
    целое		histype1 = HIST_CMD;
    целое		histype2 = HIST_CMD;
    целое		hisidx1 = 1;
    целое		hisidx2 = -1;
    целое		idx;
    целое		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    если (hislen == 0)
    {
	msg(_("'history' option is zero"));
	вернуть;
    }

    если (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	пока (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	если (histype1 == -1)
	{
	    если (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    иначе
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		вернуть;
	    }
	}
	иначе
	    histype2 = histype1;
	*end = i;
    }
    иначе
	end = arg;
    если (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	если (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	иначе
	    semsg(_(e_val_too_large), arg);
	вернуть;
    }

    для (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	если (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	если (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	если (idx >= 0 && j <= k)
	    для (i = idx + 1; !got_int; ++i)
	    {
		если (i == hislen)
		    i = 0;
		если (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    целое  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    если (vim_strsize(hist[i].hisstr) > (целое)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (целое)Columns - 10, IOSIZE - (целое)len);
		    иначе
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		если (i == idx)
		    break;
	    }
    }
}
