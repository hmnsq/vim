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
static 정수	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  // lastused entry
static 정수	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
static 정수	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    정수
get_hislen(void)
{
    반환 hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(정수 hist_type)
{
    반환 history[hist_type];
}

#만약 defined(FEAT_VIMINFO)
    void
set_histentry(정수 hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    정수 *
get_hisidx(정수 hist_type)
{
    반환 &hisidx[hist_type];
}

#만약 defined(FEAT_VIMINFO)
    정수 *
get_hisnum(정수 hist_type)
{
    반환 &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    정수
hist_char2type(정수 c)
{
    만약 (c == ':')
	반환 HIST_CMD;
    만약 (c == '=')
	반환 HIST_EXPR;
    만약 (c == '@')
	반환 HIST_INPUT;
    만약 (c == '>')
	반환 HIST_DEBUG;
    반환 HIST_SEARCH;	    // must be '?' or '/'
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
get_history_arg(expand_T *xp UNUSED, 정수 idx)
{
    char    *short_names = ":=@>?/";
    정수	    short_names_count = (정수)STRLEN(short_names);
    정수	    history_name_count = ARRAY_LENGTH(history_names) - 1;

    만약 (idx < short_names_count)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	반환 xp->xp_buf;
    }
    만약 (idx < short_names_count + history_name_count)
	반환 (char_u *)history_names[idx - short_names_count];
    만약 (idx == short_names_count + history_name_count)
	반환 (char_u *)"all";
    반환 NULL;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    void
init_history(void)
{
    정수		newlen;	    // new length of history table
    histentry_T	*temp;
    정수		type;

    // If size of history table changed, reallocate it
    newlen = (정수)p_hi;
    만약 (newlen == hislen)		// history length didn't change
	반환;

    // history length changed
    위해 (type = 0; type < HIST_COUNT; ++type)   // adjust the tables
    {
	만약 (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    만약 (temp == NULL)   // out of memory!
	    {
		만약 (type == 0)  // first one: just keep the old length
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
	아니면
	    temp = NULL;

	만약 (hisidx[type] < 0)		// there are no entries yet
	{
	    정수	i;

	    위해 (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	아니면 만약 (newlen > hislen)	// array becomes bigger
	{
	    정수	i;
	    정수	j;

	    위해 (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    위해 ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    위해 ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	아니면				// array becomes smaller or 0
	{
	    정수	i;
	    정수	j;

	    j = hisidx[type];
	    위해 (i = newlen - 1; ; --i)
	    {
		만약 (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		아니면			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		만약 (--j < 0)
		    j = hislen - 1;
		만약 (j == hisidx[type])
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
    정수
in_history(
    정수	    type,
    char_u  *str,
    정수	    move_to_front,	// Move the entry to the front if it exists
    정수	    sep,
    정수	    writing)		// ignore entries read from viminfo
{
    정수	    i;
    정수	    last_i = -1;
    char_u  *p;
    size_t  len;

    만약 (hisidx[type] < 0)
	반환 FALSE;
    i = hisidx[type];
    do
    {
	만약 (history[type][i].hisstr == NULL)
	    반환 FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	만약 (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_SEARCH || sep == p[history[type][i].hisstrlen + 1]))
	{
	    만약 (!move_to_front)
		반환 TRUE;
	    last_i = i;
	    break;
	}
	만약 (--i < 0)
	    i = hislen - 1;
    } 동안 (i != hisidx[type]);

    만약 (last_i < 0)
	반환 FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    동안 (i != hisidx[type])
    {
	만약 (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    반환 TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    static 정수
get_histtype(char_u *name)
{
    정수		i;
    정수		len = (정수)STRLEN(name);

    // No argument: use current history.
    만약 (len == 0)
	반환 hist_char2type(get_cmdline_firstc());

    위해 (i = 0; history_names[i] != NULL; ++i)
	만약 (STRNICMP(name, history_names[i], len) == 0)
	    반환 i;

    만약 (vim_strchr((char_u *)":=@>?/", name[0]) != NULL && name[1] == NUL)
	반환 hist_char2type(name[0]);

    반환 -1;
}

static 정수	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    void
add_to_history(
    정수		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    정수		in_map,		// consider maptick when inside a mapping
    정수		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    만약 (hislen == 0)		// no history
	반환;

    만약 ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_SEARCH)
	반환;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    만약 (histype == HIST_SEARCH && in_map)
    {
	만약 (maptick == last_maptick && hisidx[HIST_SEARCH] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_SEARCH][hisidx[HIST_SEARCH]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    만약 (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }

    만약 (in_history(histype, new_entry, TRUE, sep, FALSE))
	반환;

    만약 (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    만약 (hisptr->hisstr == NULL)
	hisptr->hisstrlen = 0;
    아니면
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    만약 (histype == HIST_SEARCH && in_map)
	last_maptick = maptick;
}

#만약 defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    static 정수
get_history_idx(정수 histype)
{
    만약 (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || hisidx[histype] < 0)
	반환 -1;

    반환 history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    static 정수
calc_hist_idx(정수 histype, 정수 num)
{
    정수		i;
    histentry_T	*hist;
    정수		wrapped = FALSE;

    만약 (hislen == 0 || histype < 0 || histype >= HIST_COUNT
		    || (i = hisidx[histype]) < 0 || num == 0)
	반환 -1;

    hist = history[histype];
    만약 (num > 0)
    {
	동안 (hist[i].hisnum > num)
	    만약 (--i < 0)
	    {
		만약 (wrapped)
		    break;
		i += hislen;
		wrapped = TRUE;
	    }
	만약 (i >= 0 && hist[i].hisnum == num && hist[i].hisstr != NULL)
	    반환 i;
    }
    아니면 만약 (-num <= hislen)
    {
	i += num + 1;
	만약 (i < 0)
	    i += hislen;
	만약 (hist[i].hisstr != NULL)
	    반환 i;
    }
    반환 -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    static 정수
clr_history(정수 histype)
{
    정수		i;
    histentry_T	*hisptr;

    만약 (hislen != 0 && histype >= 0 && histype < HIST_COUNT)
    {
	hisptr = history[histype];
	위해 (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	반환 OK;
    }
    반환 FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    static 정수
del_history_entry(정수 histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    정수		idx;
    정수		i;
    정수		last;
    정수		found = FALSE;

    만약 (hislen == 0 || histype < 0 || histype >= HIST_COUNT || *str == NUL
		|| hisidx[histype] < 0)
	반환 FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    만약 (regmatch.regprog == NULL)
	반환 FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    do
    {
	hisptr = &history[histype][i];
	만약 (hisptr->hisstr == NULL)
	    break;
	만약 (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	아니면
	{
	    만약 (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    만약 (--last < 0)
		last += hislen;
	}
	만약 (--i < 0)
	    i += hislen;
    } 동안 (i != idx);

    만약 (history[histype][idx].hisstr == NULL)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    반환 found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    static 정수
del_history_idx(정수 histype, 정수 idx)
{
    정수	    i, j;

    i = calc_hist_idx(histype, idx);
    만약 (i < 0)
	반환 FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    만약 (histype == HIST_SEARCH && maptick == last_maptick && i == idx)
	last_maptick = -1;

    동안 (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    만약 (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    반환 TRUE;
}

/*
 * "histadd()" function
 */
    void
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    정수		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_number = FALSE;
    만약 (check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != NULL ? get_histtype(str) : -1;
    만약 (histype < 0)
	반환;

    str = tv_get_string_buf(&argvars[1], buf);
    만약 (*str == NUL)
	반환;

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
    정수		n;
    char_u	*str;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_arg(argvars, 1) == FAIL))
	반환;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    만약 (str == NULL)
	n = 0;
    아니면 만약 (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    아니면 만약 (argvars[1].v_type == VAR_NUMBER)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (정수)tv_get_number(&argvars[1]));
    아니면
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

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	반환;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    만약 (str == NULL)
	rettv->vval.v_string = NULL;
    아니면
    {
	정수 type;
	정수 idx;

	type = get_histtype(str);
	만약 (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	아니면
	    idx = (정수)tv_get_number_chk(&argvars[1], NULL);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	만약 (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	아니면
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
    정수		i;
    char_u	*histname;

    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	반환;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == NULL ? HIST_CMD - 1 : get_histtype(histname);
    만약 (i >= HIST_CMD && i < HIST_COUNT)
	rettv->vval.v_number = get_history_idx(i);
    아니면
	rettv->vval.v_number = -1;
}
#endif // FEAT_EVAL

#만약 defined(FEAT_CRYPT)
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
    정수		i;

    i = hisidx[HIST_CMD];
    만약 (i < 0)
	반환;
    p_start = history[HIST_CMD][i].hisstr;
    만약 (p_start == NULL)
	반환;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    위해 (p = p_start; *p; ++p)
    {
	만약 (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    만약 (p == NULL)
		break;
	    ++p;
	    위해 (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		만약 (p[i] == '\\' && p[i + 1])
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
    정수		histype1 = HIST_CMD;
    정수		histype2 = HIST_CMD;
    정수		hisidx1 = 1;
    정수		hisidx2 = -1;
    정수		idx;
    정수		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    만약 (hislen == 0)
    {
	msg(_("'history' option is zero"));
	반환;
    }

    만약 (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	동안 (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != NULL)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	만약 (histype1 == -1)
	{
	    만약 (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_COUNT-1;
	    }
	    아니면
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		반환;
	    }
	}
	아니면
	    histype2 = histype1;
	*end = i;
    }
    아니면
	end = arg;
    만약 (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	만약 (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	아니면
	    semsg(_(e_val_too_large), arg);
	반환;
    }

    위해 (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	msg_puts_title((char *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	만약 (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	만약 (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	만약 (idx >= 0 && j <= k)
	    위해 (i = idx + 1; !got_int; ++i)
	    {
		만약 (i == hislen)
		    i = 0;
		만약 (hist[i].hisstr != NULL
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    정수  len;

		    msg_putchar('\n');
		    len = vim_snprintf((char *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    만약 (vim_strsize(hist[i].hisstr) > (정수)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (정수)Columns - 10, IOSIZE - (정수)len);
		    아니면
			STRCPY(IObuff + len, hist[i].hisstr);
		    msg_outtrans(IObuff);
		    out_flush();
		}
		만약 (i == idx)
		    break;
	    }
    }
}
