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
static цяло	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static цяло	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static цяло	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    цяло
get_hislen(void)
{
    върни hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(цяло hist_type)
{
    върни history[hist_type];
}

#ако defined(FEAT_VIMINFO)
    void
set_histentry(цяло hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    цяло *
get_hisidx(цяло hist_type)
{
    върни &hisidx[hist_type];
}

#ако defined(FEAT_VIMINFO)
    цяло *
get_hisnum(цяло hist_type)
{
    върни &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    цяло
hist_char2type(цяло c)
{
    ако (c == ':')
	върни HIST_CMD;
    ако (c == '=')
	върни HIST_EXPR;
    ако (c == '@')
	върни HIST_INPUT;
    ако (c == '>')
	върни HIST_DEBUG;
    върни HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, цяло idx)
{
    char    *short_names = ":=@>?/";
    цяло	    short_names_count = (цяло)STRLEN(short_names);
    цяло	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    ако (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	върни xp->xp_buf;
    }
    ако (idx < short_names_count + history_name_count)
	върни (char_u *)history_names[idx - short_names_count];
    ако (idx == short_names_count + history_name_count)
	върни (char_u *)"all";
    върни NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    цяло		newlen;	    // new length of history table
    histentry_T	*temp;
    цяло		type;

    // If size of history table changed, reallocate it
    newlen = (цяло)p_hi;
    ако (newlen == hislen)		// history length didn't change
	върни;

    // history length changed
    за (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	ако (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ако (temp == NULL)   // out of memory!
	    {
		ако (type == 0)  // first one: just keep the old length
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

	ако (hisidx[type] < 0)		// there are no entries yet
	{
	    цяло	i;

	    за (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	иначе ако (newlen > hislen)	// array becomes bigger
	{
	    цяло	i;
	    цяло	j;

	    за (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    за ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    за ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	иначе				// array becomes smaller or 0
	{
	    цяло	i;
	    цяло	j;

	    j = hisidx[type];
	    за (i = newlen - 1; ; --i)
	    {
		ако (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		иначе			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ако (--j < 0)
		    j = hislen - 1;
		ако (j == hisidx[type])
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
    цяло
in_history(
    цяло	    type,
    char_u  *str,
    цяло	    move_to_front,	// Move the entry to the front if it exists
    цяло	    sep,
    цяло	    writing)		// ignore entries read from viminfo
{
    цяло	    i;
    цяло	    last_i = -1;
    char_u  *p;
    size_t  len;

    ако (hisidx[type] < 0)
	върни FALSE;
    i = hisidx[type];
    do
    {
	ако (history[type][i].hisstr == NULL)
	    върни FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ако (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ако (!move_to_front)
		върни TRUE;
	    last_i = i;
	    break;
	}
	ако (--i < 0)
	    i = hislen - 1;
    } докато (i != hisidx[type]);

    ако (last_i < 0)
	върни FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    докато (i != hisidx[type])
    {
	ако (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    върни TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static цяло
get_histtype(char_u *name)
{
    цяло		i;
    цяло		len = (цяло)STRLEN(name);

    // No argument: use current history.
    ако (len == 0)
	върни hist_char2type(get_cmdline_firstc());

    за (i = 0; history_names[i] != NULL; ++i)
	ако (STRNICMP(name, history_names[i], len) == 0)
	    върни i;

    ако (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	върни hist_char2type(name[0]);

    върни -1;
}

static цяло	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    цяло		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    цяло		in_map,		// consider maptick when inside a mapping
    цяло		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ако (hislen == 0)		// no history
	върни;

    ако ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	върни;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ако (histype == HIST_SEARCH && in_map)
    {
	ако (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ако (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    ако (in_history(histype, new_entry, TRUE, sep, FALSE))
	върни;

    ако (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ако (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    иначе
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ако (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#ако defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static цяло
get_history_idx(цяло histype)
{
    ако (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	върни -1;

    върни history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static цяло
calc_hist_idx(цяло histype, цяло num)
{
    цяло		i;
    histentry_T	*hist;
    цяло		wrapped = FALSE;

    ако (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	върни -1;

    hist = history[histype];
    ако (num > 0)
    {
	докато (hist[i].hisnum > num)
	    ако (--i < 0)
	    {
		ако (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	ако (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    върни i;
    }
    иначе ако (-num <= hislen)
    {
	i += num + 1;
	ако (i < 0)
	    i += hislen;
	ако (hist[i].hisstr != NULL)
	    върни i;
    }
    върни -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static цяло
clr_history(цяло histype)
{
    цяло		i;
    histentry_T	*hisptr;

    ако (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	за (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	върни OK;
    }
    върни FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static цяло
del_history_entry(цяло histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    цяло		idx;
    цяло		i;
    цяло		last;
    цяло		found = FALSE;

    ако (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	върни FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ако (regmatch.regprog == NULL)
	върни FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	ако (hisptr->hisstr == NULL)
	    break;
	ако (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	иначе
	{
	    ако (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ако (--last < 0)
		last += hislen;
	}
	ако (--i < 0)
	    i += hislen;
    } докато (i != idx);

    ако (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    върни found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static цяло
del_history_idx(цяло histype, цяло idx)
{
    цяло	    i, j;

    i = calc_hist_idx(histype, idx);
    ако (i < 0)
	върни FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ако (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    докато (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ако (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    върни TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    цяло		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    ако (check_secure())
	върни;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	върни;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    ако (histype < 0)
	върни;

    str = tv_get_string_buf(&argvars[1], buf);
    ако (*str == NUL)
	върни;

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
    цяло		n;
    char_u	*str;

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	върни;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ако (str == NULL)
	n = 0;
    иначе ако (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    иначе ако (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (цяло)tv_get_number(&argvars[1]));
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

    ако (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	върни;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ако (str == NULL)
	rettv->vval.v_string = NULL;
    иначе
    {
	цяло type;
	цяло idx;

	type = get_histtype(str);
	ако (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	иначе
	    idx = (цяло)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ако (idx < 0)
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
    цяло		i;
    char_u	*histname;

    ако (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	върни;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    ако (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    иначе
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#ако defined(FEAT_CRYPT)
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
    цяло		i;

    i = hisidx[HIST_CMD];
    ако (i < 0)
	върни;
    p_start = history[HIST_CMD][i].hisstr;
    ако (p_start == NULL)
	върни;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    за (p = p_start; *p; ++p)
    {
	ако (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ако (p == NULL)
		break;
	    ++p;
	    за (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ако (p[i] == '\\' && p[i + 1])
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
    цяло		histype1 = HIST_CMD;
    цяло		histype2 = HIST_CMD;
    цяло		hisidx1 = 1;
    цяло		hisidx2 = -1;
    цяло		idx;
    цяло		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ако (hislen == 0)
    {
	msg(_("'history' option is zero"));
	върни;
    }

    ако (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	докато (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ако (histype1 == -1)
	{
	    ако (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    иначе
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		върни;
	    }
	}
	иначе
	    histype2 = histype1;
	*end = i;
    }
    иначе
	end = arg;
    ако (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ако (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	иначе
	    semsg(_(e_val_too_large), arg);
	върни;
    }

    за (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ако (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ако (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ако (idx >= 0 && j <= k)
	    за (i = idx + 1; !got_int; ++i)
	    {
		ако (i == hislen)
		    i = 0;
		ако (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    цяло  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ако (vim_strsize(hist[i].hisstr) > (цяло)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (цяло)Columns - 10, IOSIZE - (цяло)len);
		    иначе
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		ако (i == idx)
		    break;
	    }
    }
}
