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
static số_nguyên	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static số_nguyên	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static số_nguyên	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    số_nguyên
get_hislen(void)
{
    trả_về hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(số_nguyên hist_type)
{
    trả_về history[hist_type];
}

#nếu defined(FEAT_VIMINFO)
    void
set_histentry(số_nguyên hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    số_nguyên *
get_hisidx(số_nguyên hist_type)
{
    trả_về &hisidx[hist_type];
}

#nếu defined(FEAT_VIMINFO)
    số_nguyên *
get_hisnum(số_nguyên hist_type)
{
    trả_về &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    số_nguyên
hist_char2type(số_nguyên c)
{
    nếu (c == ':')
	trả_về HIST_CMD;
    nếu (c == '=')
	trả_về HIST_EXPR;
    nếu (c == '@')
	trả_về HIST_INPUT;
    nếu (c == '>')
	trả_về HIST_DEBUG;
    trả_về HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, số_nguyên idx)
{
    char    *short_names = ":=@>?/";
    số_nguyên	    short_names_count = (số_nguyên)STRLEN(short_names);
    số_nguyên	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    nếu (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	trả_về xp->xp_buf;
    }
    nếu (idx < short_names_count + history_name_count)
	trả_về (char_u *)history_names[idx - short_names_count];
    nếu (idx == short_names_count + history_name_count)
	trả_về (char_u *)"all";
    trả_về NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    số_nguyên		newlen;	    // new length of history table
    histentry_T	*temp;
    số_nguyên		type;

    // If size of history table changed, reallocate it
    newlen = (số_nguyên)p_hi;
    nếu (newlen == hislen)		// history length didn't change
	trả_về;

    // history length changed
    cho (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	nếu (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    nếu (temp == NULL)   // out of memory!
	    {
		nếu (type == 0)  // first one: just keep the old length
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
	không_thì
	    temp = NULL;

	nếu (hisidx[type] < 0)		// there are no entries yet
	{
	    số_nguyên	i;

	    cho (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	không_thì nếu (newlen > hislen)	// array becomes bigger
	{
	    số_nguyên	i;
	    số_nguyên	j;

	    cho (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    cho ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    cho ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	không_thì				// array becomes smaller or 0
	{
	    số_nguyên	i;
	    số_nguyên	j;

	    j = hisidx[type];
	    cho (i = newlen - 1; ; --i)
	    {
		nếu (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		không_thì			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		nếu (--j < 0)
		    j = hislen - 1;
		nếu (j == hisidx[type])
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
    số_nguyên
in_history(
    số_nguyên	    type,
    char_u  *str,
    số_nguyên	    move_to_front,	// Move the entry to the front if it exists
    số_nguyên	    sep,
    số_nguyên	    writing)		// ignore entries read from viminfo
{
    số_nguyên	    i;
    số_nguyên	    last_i = -1;
    char_u  *p;
    size_t  len;

    nếu (hisidx[type] < 0)
	trả_về FALSE;
    i = hisidx[type];
    do
    {
	nếu (history[type][i].hisstr == NULL)
	    trả_về FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	nếu (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    nếu (!move_to_front)
		trả_về TRUE;
	    last_i = i;
	    break;
	}
	nếu (--i < 0)
	    i = hislen - 1;
    } trong_khi (i != hisidx[type]);

    nếu (last_i < 0)
	trả_về FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    trong_khi (i != hisidx[type])
    {
	nếu (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    trả_về TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static số_nguyên
get_histtype(char_u *name)
{
    số_nguyên		i;
    số_nguyên		len = (số_nguyên)STRLEN(name);

    // No argument: use current history.
    nếu (len == 0)
	trả_về hist_char2type(get_cmdline_firstc());

    cho (i = 0; history_names[i] != NULL; ++i)
	nếu (STRNICMP(name, history_names[i], len) == 0)
	    trả_về i;

    nếu (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	trả_về hist_char2type(name[0]);

    trả_về -1;
}

static số_nguyên	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    số_nguyên		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    số_nguyên		in_map,		// consider maptick when inside a mapping
    số_nguyên		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    nếu (hislen == 0)		// no history
	trả_về;

    nếu ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	trả_về;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    nếu (histype == HIST_SEARCH && in_map)
    {
	nếu (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    nếu (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    nếu (in_history(histype, new_entry, TRUE, sep, FALSE))
	trả_về;

    nếu (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    nếu (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    không_thì
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    nếu (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#nếu defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static số_nguyên
get_history_idx(số_nguyên histype)
{
    nếu (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	trả_về -1;

    trả_về history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static số_nguyên
calc_hist_idx(số_nguyên histype, số_nguyên num)
{
    số_nguyên		i;
    histentry_T	*hist;
    số_nguyên		wrapped = FALSE;

    nếu (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	trả_về -1;

    hist = history[histype];
    nếu (num > 0)
    {
	trong_khi (hist[i].hisnum > num)
	    nếu (--i < 0)
	    {
		nếu (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	nếu (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    trả_về i;
    }
    không_thì nếu (-num <= hislen)
    {
	i += num + 1;
	nếu (i < 0)
	    i += hislen;
	nếu (hist[i].hisstr != NULL)
	    trả_về i;
    }
    trả_về -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static số_nguyên
clr_history(số_nguyên histype)
{
    số_nguyên		i;
    histentry_T	*hisptr;

    nếu (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	cho (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	trả_về OK;
    }
    trả_về FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static số_nguyên
del_history_entry(số_nguyên histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    số_nguyên		idx;
    số_nguyên		i;
    số_nguyên		last;
    số_nguyên		found = FALSE;

    nếu (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	trả_về FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    nếu (regmatch.regprog == NULL)
	trả_về FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	nếu (hisptr->hisstr == NULL)
	    break;
	nếu (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	không_thì
	{
	    nếu (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    nếu (--last < 0)
		last += hislen;
	}
	nếu (--i < 0)
	    i += hislen;
    } trong_khi (i != idx);

    nếu (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    trả_về found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static số_nguyên
del_history_idx(số_nguyên histype, số_nguyên idx)
{
    số_nguyên	    i, j;

    i = calc_hist_idx(histype, idx);
    nếu (i < 0)
	trả_về FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    nếu (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    trong_khi (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    nếu (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    trả_về TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    số_nguyên		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    nếu (check_secure())
	trả_về;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	trả_về;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    nếu (histype < 0)
	trả_về;

    str = tv_get_string_buf(&argvars[1], buf);
    nếu (*str == NUL)
	trả_về;

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
    số_nguyên		n;
    char_u	*str;

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	trả_về;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    nếu (str == NULL)
	n = 0;
    không_thì nếu (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    không_thì nếu (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (số_nguyên)tv_get_number(&argvars[1]));
    không_thì
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

    nếu (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	trả_về;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    nếu (str == NULL)
	rettv->vval.v_string = NULL;
    không_thì
    {
	số_nguyên type;
	số_nguyên idx;

	type = get_histtype(str);
	nếu (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	không_thì
	    idx = (số_nguyên)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	nếu (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	không_thì
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
    số_nguyên		i;
    char_u	*histname;

    nếu (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	trả_về;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    nếu (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    không_thì
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#nếu defined(FEAT_CRYPT)
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
    số_nguyên		i;

    i = hisidx[HIST_CMD];
    nếu (i < 0)
	trả_về;
    p_start = history[HIST_CMD][i].hisstr;
    nếu (p_start == NULL)
	trả_về;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    cho (p = p_start; *p; ++p)
    {
	nếu (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    nếu (p == NULL)
		break;
	    ++p;
	    cho (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		nếu (p[i] == '\\' && p[i + 1])
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
    số_nguyên		histype1 = HIST_CMD;
    số_nguyên		histype2 = HIST_CMD;
    số_nguyên		hisidx1 = 1;
    số_nguyên		hisidx2 = -1;
    số_nguyên		idx;
    số_nguyên		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    nếu (hislen == 0)
    {
	msg(_("'history' option is zero"));
	trả_về;
    }

    nếu (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	trong_khi (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	nếu (histype1 == -1)
	{
	    nếu (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    không_thì
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		trả_về;
	    }
	}
	không_thì
	    histype2 = histype1;
	*end = i;
    }
    không_thì
	end = arg;
    nếu (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	nếu (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	không_thì
	    semsg(_(e_val_too_large), arg);
	trả_về;
    }

    cho (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	nếu (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	nếu (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	nếu (idx >= 0 && j <= k)
	    cho (i = idx + 1; !got_int; ++i)
	    {
		nếu (i == hislen)
		    i = 0;
		nếu (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    số_nguyên  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    nếu (vim_strsize(hist[i].hisstr) > (số_nguyên)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (số_nguyên)Columns - 10, IOSIZE - (số_nguyên)len);
		    không_thì
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		nếu (i == idx)
		    break;
	    }
    }
}
