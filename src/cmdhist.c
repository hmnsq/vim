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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ histentry_T *(history[HIST_ਗਿਣਤੀ]) = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
ਸਥਿਰ ਪੂਰਨ	hisidx[HIST_ਗਿਣਤੀ] = {-1, -1, -1, -1, -1};  // lastused entry
ਸਥਿਰ ਪੂਰਨ	hisnum[HIST_ਗਿਣਤੀ] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ਸਥਿਰ ਪੂਰਨ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ਪੂਰਨ
get_hislen(ਖਾਲੀ)
{
    ਵਾਪਸ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ਪੂਰਨ hist_type)
{
    ਵਾਪਸ history[hist_type];
}

#ਜੇ defined(FEAT_VIMINFO)
    ਖਾਲੀ
ਸੈੱਟ_histentry(ਪੂਰਨ hist_type, histentry_T *ਇੰਦਰਾਜ)
{
    history[hist_type] = ਇੰਦਰਾਜ;
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ *
get_hisidx(ਪੂਰਨ hist_type)
{
    ਵਾਪਸ &hisidx[hist_type];
}

#ਜੇ defined(FEAT_VIMINFO)
    ਪੂਰਨ *
get_hisnum(ਪੂਰਨ hist_type)
{
    ਵਾਪਸ &hisnum[hist_type];
}
#ਜੇ_ਅੰਤ

/*
 * Translate a history character to the associated type number.
 */
    ਪੂਰਨ
hist_char2type(ਪੂਰਨ c)
{
    ਜੇ (c == ':')
	ਵਾਪਸ HIST_CMD;
    ਜੇ (c == '=')
	ਵਾਪਸ HIST_ਵਿਅੰਜਕ;
    ਜੇ (c == '@')
	ਵਾਪਸ HIST_ਨਿਵੇਸ਼;
    ਜੇ (c == '>')
	ਵਾਪਸ HIST_DEBUG;
    ਵਾਪਸ HIST_ਖੋਜ;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ਸਥਿਰ ਅੱਖਰ *(history_ਨਾਂ[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ਨਿਰਾ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਅੱਖਰ    *short_ਨਾਂ = ":=@>?/";
    ਪੂਰਨ	    short_ਨਾਂ_ਗਿਣਤੀ = (ਪੂਰਨ)STRLEN(short_ਨਾਂ);
    ਪੂਰਨ	    history_name_ਗਿਣਤੀ = ARRAY_ਲੰਮਾਈ(history_ਨਾਂ) - 1;

    ਜੇ (idx < short_ਨਾਂ_ਗਿਣਤੀ)
    {
	xp->xp_buf[0] = (char_u)short_ਨਾਂ[idx];
	xp->xp_buf[1] = NUL;
	ਵਾਪਸ xp->xp_buf;
    }
    ਜੇ (idx < short_ਨਾਂ_ਗਿਣਤੀ + history_name_ਗਿਣਤੀ)
	ਵਾਪਸ (char_u *)history_ਨਾਂ[idx - short_ਨਾਂ_ਗਿਣਤੀ];
    ਜੇ (idx == short_ਨਾਂ_ਗਿਣਤੀ + history_name_ਗਿਣਤੀ)
	ਵਾਪਸ (char_u *)"all";
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    ਖਾਲੀ
init_history(ਖਾਲੀ)
{
    ਪੂਰਨ		newlen;	    // new length of history table
    histentry_T	*ਆਰਜ਼ੀ;
    ਪੂਰਨ		type;

    // If size of history table changed, reallocate it
    newlen = (ਪੂਰਨ)p_hi;
    ਜੇ (newlen == hislen)		// history length didn't change
	ਵਾਪਸ;

    // history length changed
    ਲਈ (type = 0; type < HIST_ਗਿਣਤੀ; ++type)   // adjust the tables
    {
	ਜੇ (newlen > 0)
	{
	    ਆਰਜ਼ੀ = ALLOC_MULT(histentry_T, newlen);
	    ਜੇ (ਆਰਜ਼ੀ == ਨਿਰਾ)   // out of memory!
	    {
		ਜੇ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ਰੋਕੋ;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ਜਾਰੀ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਆਰਜ਼ੀ = ਨਿਰਾ;

	ਜੇ (hisidx[type] < 0)		// there are no entries yet
	{
	    ਪੂਰਨ	i;

	    ਲਈ (i = 0; i < newlen; ++i)
		clear_hist_ਇੰਦਰਾਜ(&ਆਰਜ਼ੀ[i]);
	}
	ਨਹੀਂਤਾਂ ਜੇ (newlen > hislen)	// array becomes bigger
	{
	    ਪੂਰਨ	i;
	    ਪੂਰਨ	j;

	    ਲਈ (i = 0; i <= hisidx[type]; ++i)
		ਆਰਜ਼ੀ[i] = history[type][i];
	    j = i;
	    ਲਈ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_ਇੰਦਰਾਜ(&ਆਰਜ਼ੀ[i]);
	    ਲਈ ( ; j < hislen; ++i, ++j)
		ਆਰਜ਼ੀ[i] = history[type][j];
	}
	ਨਹੀਂਤਾਂ				// array becomes smaller or 0
	{
	    ਪੂਰਨ	i;
	    ਪੂਰਨ	j;

	    j = hisidx[type];
	    ਲਈ (i = newlen - 1; ; --i)
	    {
		ਜੇ (i >= 0)		// copy newest entries
		    ਆਰਜ਼ੀ[i] = history[type][j];
		ਨਹੀਂਤਾਂ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ਜੇ (--j < 0)
		    j = hislen - 1;
		ਜੇ (j == hisidx[type])
		    ਰੋਕੋ;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = ਆਰਜ਼ੀ;
    }
    hislen = newlen;
}

    ਖਾਲੀ
clear_hist_ਇੰਦਰਾਜ(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ਨਿਰਾ;
    hisptr->hisstrlen = 0;
    hisptr->ਸਮਾਂ_ਸੈੱਟ = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    ਪੂਰਨ
ਵਿੱਚ_history(
    ਪੂਰਨ	    type,
    char_u  *str,
    ਪੂਰਨ	    move_ਮੰਜ਼ਿਲ_front,	// Move the entry to the front if it exists
    ਪੂਰਨ	    sep,
    ਪੂਰਨ	    writing)		// ignore entries read from viminfo
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ਆਖਰੀ_i = -1;
    char_u  *p;
    size_t  len;

    ਜੇ (hisidx[type] < 0)
	ਵਾਪਸ FALSE;
    i = hisidx[type];
    ਕਰੋ
    {
	ਜੇ (history[type][i].hisstr == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ਜੇ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_ਖੋਜ || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ਜੇ (!move_ਮੰਜ਼ਿਲ_front)
		ਵਾਪਸ TRUE;
	    ਆਖਰੀ_i = i;
	    ਰੋਕੋ;
	}
	ਜੇ (--i < 0)
	    i = hislen - 1;
    } ਜਦੋਂ (i != hisidx[type]);

    ਜੇ (ਆਖਰੀ_i < 0)
	ਵਾਪਸ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ਜਦੋਂ (i != hisidx[type])
    {
	ਜੇ (++i >= hislen)
	    i = 0;
	history[type][ਆਖਰੀ_i] = history[type][i];
	ਆਖਰੀ_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
    ਵਾਪਸ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ਸਥਿਰ ਪੂਰਨ
get_histtype(char_u *name)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(name);

    // No argument: use current history.
    ਜੇ (len == 0)
	ਵਾਪਸ hist_char2type(get_cmdline_firstc());

    ਲਈ (i = 0; history_ਨਾਂ[i] != ਨਿਰਾ; ++i)
	ਜੇ (STRNICMP(name, history_ਨਾਂ[i], len) == 0)
	    ਵਾਪਸ i;

    ਜੇ (vim_strchr((char_u *)":=@>?/", name[0]) != ਨਿਰਾ && name[1] == NUL)
	ਵਾਪਸ hist_char2type(name[0]);

    ਵਾਪਸ -1;
}

ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    ਖਾਲੀ
ਜੋੜੋ_ਮੰਜ਼ਿਲ_history(
    ਪੂਰਨ		histype,
    char_u	*new_ਇੰਦਰਾਜ,
    size_t	new_entrylen,
    ਪੂਰਨ		ਵਿੱਚ_map,		// consider maptick when inside a mapping
    ਪੂਰਨ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ਜੇ (hislen == 0)		// no history
	ਵਾਪਸ;

    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_ਖੋਜ)
	ਵਾਪਸ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ਜੇ (histype == HIST_ਖੋਜ && ਵਿੱਚ_map)
    {
	ਜੇ (maptick == ਆਖਰੀ_maptick && hisidx[HIST_ਖੋਜ] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_ਖੋਜ][hisidx[HIST_ਖੋਜ]];
	    vim_free(hisptr->hisstr);
	    clear_hist_ਇੰਦਰਾਜ(hisptr);
	    --hisnum[histype];
	    ਜੇ (--hisidx[HIST_ਖੋਜ] < 0)
		hisidx[HIST_ਖੋਜ] = hislen - 1;
	}
	ਆਖਰੀ_maptick = -1;
    }

    ਜੇ (ਵਿੱਚ_history(histype, new_ਇੰਦਰਾਜ, TRUE, sep, FALSE))
	ਵਾਪਸ;

    ਜੇ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_ਇੰਦਰਾਜ, new_entrylen + 2);
    ਜੇ (hisptr->hisstr == ਨਿਰਾ)
	hisptr->hisstrlen = 0;
    ਨਹੀਂਤਾਂ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->ਸਮਾਂ_ਸੈੱਟ = vim_ਸਮਾਂ();
    ਜੇ (histype == HIST_ਖੋਜ && ਵਿੱਚ_map)
	ਆਖਰੀ_maptick = maptick;
}

#ਜੇ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ਸਥਿਰ ਪੂਰਨ
get_history_idx(ਪੂਰਨ histype)
{
    ਜੇ (hislen == 0 || histype < 0 || histype >= HIST_ਗਿਣਤੀ
		    || hisidx[histype] < 0)
	ਵਾਪਸ -1;

    ਵਾਪਸ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ਸਥਿਰ ਪੂਰਨ
calc_hist_idx(ਪੂਰਨ histype, ਪੂਰਨ ਅੰਕ)
{
    ਪੂਰਨ		i;
    histentry_T	*hist;
    ਪੂਰਨ		wrapped = FALSE;

    ਜੇ (hislen == 0 || histype < 0 || histype >= HIST_ਗਿਣਤੀ
		    || (i = hisidx[histype]) < 0 || ਅੰਕ == 0)
	ਵਾਪਸ -1;

    hist = history[histype];
    ਜੇ (ਅੰਕ > 0)
    {
	ਜਦੋਂ (hist[i].hisnum > ਅੰਕ)
	    ਜੇ (--i < 0)
	    {
		ਜੇ (wrapped)
		    ਰੋਕੋ;
		i += hislen;
		wrapped = TRUE;
	    }
	ਜੇ (i >= 0 && hist[i].hisnum == ਅੰਕ && hist[i].hisstr != ਨਿਰਾ)
	    ਵਾਪਸ i;
    }
    ਨਹੀਂਤਾਂ ਜੇ (-ਅੰਕ <= hislen)
    {
	i += ਅੰਕ + 1;
	ਜੇ (i < 0)
	    i += hislen;
	ਜੇ (hist[i].hisstr != ਨਿਰਾ)
	    ਵਾਪਸ i;
    }
    ਵਾਪਸ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ਸਥਿਰ ਪੂਰਨ
clr_history(ਪੂਰਨ histype)
{
    ਪੂਰਨ		i;
    histentry_T	*hisptr;

    ਜੇ (hislen != 0 && histype >= 0 && histype < HIST_ਗਿਣਤੀ)
    {
	hisptr = history[histype];
	ਲਈ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_ਇੰਦਰਾਜ(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ਸਥਿਰ ਪੂਰਨ
del_history_ਇੰਦਰਾਜ(ਪੂਰਨ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਆਖਰੀ;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;

    ਜੇ (hislen == 0 || histype < 0 || histype >= HIST_ਗਿਣਤੀ || *str == NUL
		|| hisidx[histype] < 0)
	ਵਾਪਸ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ਜੇ (regmatch.regprog == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = ਆਖਰੀ = idx;
    ਕਰੋ
    {
	hisptr = &history[histype][i];
	ਜੇ (hisptr->hisstr == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    ਮਿਲਿਆ = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_ਇੰਦਰਾਜ(hisptr);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (i != ਆਖਰੀ)
	    {
		history[histype][ਆਖਰੀ] = *hisptr;
		clear_hist_ਇੰਦਰਾਜ(hisptr);
	    }
	    ਜੇ (--ਆਖਰੀ < 0)
		ਆਖਰੀ += hislen;
	}
	ਜੇ (--i < 0)
	    i += hislen;
    } ਜਦੋਂ (i != idx);

    ਜੇ (history[histype][idx].hisstr == ਨਿਰਾ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ਵਾਪਸ ਮਿਲਿਆ;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ਸਥਿਰ ਪੂਰਨ
del_history_idx(ਪੂਰਨ histype, ਪੂਰਨ idx)
{
    ਪੂਰਨ	    i, j;

    i = calc_hist_idx(histype, idx);
    ਜੇ (i < 0)
	ਵਾਪਸ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ਜੇ (histype == HIST_ਖੋਜ && maptick == ਆਖਰੀ_maptick && i == idx)
	ਆਖਰੀ_maptick = -1;

    ਜਦੋਂ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_ਇੰਦਰਾਜ(&history[histype][i]);
    ਜੇ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ਵਾਪਸ TRUE;
}

/*
 * "histadd()" function
 */
    ਖਾਲੀ
f_histadd(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ਸੰਖਿਆ = FALSE;
    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ਨਿਰਾ ? get_histtype(str) : -1;
    ਜੇ (histype < 0)
	ਵਾਪਸ;

    str = tv_get_string_buf(&argvars[1], buf);
    ਜੇ (*str == NUL)
	ਵਾਪਸ;

    init_history();
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_ਸੰਖਿਆ = TRUE;
}

/*
 * "histdel()" function
 */
    ਖਾਲੀ
f_histdel(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		n;
    char_u	*str;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ਜੇ (str == ਨਿਰਾ)
	n = 0;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_ਸੰਖਿਆ)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]));
    ਨਹੀਂਤਾਂ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_ਇੰਦਰਾਜ(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * "histget()" function
 */
    ਖਾਲੀ
f_histget(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    char_u  *str;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ਜੇ (str == ਨਿਰਾ)
	rettv->vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ type;
	ਪੂਰਨ idx;

	type = get_histtype(str);
	ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ਨਹੀਂਤਾਂ
	    idx = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], ਨਿਰਾ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ਜੇ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    ਖਾਲੀ
f_histnr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		i;
    char_u	*histname;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ਨਿਰਾ ? HIST_CMD - 1 : get_histtype(histname);
    ਜੇ (i >= HIST_CMD && i < HIST_ਗਿਣਤੀ)
	rettv->vval.v_ਸੰਖਿਆ = get_history_idx(i);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = -1;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    ਖਾਲੀ
remove_ਕੁੰਜੀ_from_history(ਖਾਲੀ)
{
    char_u	*p_ਸ਼ੁਰੂ;
    char_u	*p_ਸਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		i;

    i = hisidx[HIST_CMD];
    ਜੇ (i < 0)
	ਵਾਪਸ;
    p_ਸ਼ੁਰੂ = history[HIST_CMD][i].hisstr;
    ਜੇ (p_ਸ਼ੁਰੂ == ਨਿਰਾ)
	ਵਾਪਸ;

    p_ਸਿਰਾ = p_ਸ਼ੁਰੂ + history[HIST_CMD][i].hisstrlen;
    ਲਈ (p = p_ਸ਼ੁਰੂ; *p; ++p)
    {
	ਜੇ (STRNCMP(p, "key", 3) == 0 && !ਸੁਰੱਖਿਅਤ_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ਜੇ (p == ਨਿਰਾ)
		ਰੋਕੋ;
	    ++p;
	    ਲਈ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ਜੇ (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_ਸਿਰਾ - (p + i)) + 1);	    // +1 for the NUL
	    p_ਸਿਰਾ -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_ਸਿਰਾ - p_ਸ਼ੁਰੂ);
}
#ਜੇ_ਅੰਤ

/*
 * :history command - print a history
 */
    ਖਾਲੀ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    ਪੂਰਨ		histype1 = HIST_CMD;
    ਪੂਰਨ		histype2 = HIST_CMD;
    ਪੂਰਨ		hisidx1 = 1;
    ਪੂਰਨ		hisidx2 = -1;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		i, j, k;
    char_u	*ਸਿਰਾ;
    char_u	*arg = eap->arg;

    ਜੇ (hislen == 0)
    {
	ਸੁਨੇਹਾ(_("'history' option is zero"));
	ਵਾਪਸ;
    }

    ਜੇ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	ਸਿਰਾ = arg;
	ਜਦੋਂ (ASCII_ISALPHA(*ਸਿਰਾ)
		|| vim_strchr((char_u *)":=@>/?", *ਸਿਰਾ) != ਨਿਰਾ)
	    ਸਿਰਾ++;
	i = *ਸਿਰਾ;
	*ਸਿਰਾ = NUL;
	histype1 = get_histtype(arg);
	ਜੇ (histype1 == -1)
	{
	    ਜੇ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_ਗਿਣਤੀ-1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		*ਸਿਰਾ = i;
		semsg(_(e_trailing_characters_str), arg);
		ਵਾਪਸ;
	    }
	}
	ਨਹੀਂਤਾਂ
	    histype2 = histype1;
	*ਸਿਰਾ = i;
    }
    ਨਹੀਂਤਾਂ
	ਸਿਰਾ = arg;
    ਜੇ (!get_list_range(&ਸਿਰਾ, &hisidx1, &hisidx2) || *ਸਿਰਾ != NUL)
    {
	ਜੇ (*ਸਿਰਾ != NUL)
	    semsg(_(e_trailing_characters_str), ਸਿਰਾ);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_val_too_large), arg);
	ਵਾਪਸ;
    }

    ਲਈ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "\n      #  %s history", history_ਨਾਂ[histype1]);
	ਸੁਨੇਹਾ_puts_title((ਅੱਖਰ *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ਜੇ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ਜੇ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ਜੇ (idx >= 0 && j <= k)
	    ਲਈ (i = idx + 1; !got_int; ++i)
	    {
		ਜੇ (i == hislen)
		    i = 0;
		ਜੇ (hist[i].hisstr != ਨਿਰਾ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !ਸੁਨੇਹਾ_filtered(hist[i].hisstr))
		{
		    ਪੂਰਨ  len;

		    ਸੁਨੇਹਾ_putchar('\n');
		    len = vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ਜੇ (vim_strsize(hist[i].hisstr) > (ਪੂਰਨ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ਪੂਰਨ)Columns - 10, IOSIZE - (ਪੂਰਨ)len);
		    ਨਹੀਂਤਾਂ
			STRCPY(IObuff + len, hist[i].hisstr);
		    ਸੁਨੇਹਾ_outtrans(IObuff);
		    ਬਾਹਰਲਾ_flush();
		}
		ਜੇ (i == idx)
		    ਰੋਕੋ;
	    }
    }
}
