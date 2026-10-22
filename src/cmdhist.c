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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ histentry_T *(history[HIST_ಎಣಿಕೆ]) = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	hisidx[HIST_ಎಣಿಕೆ] = {-1, -1, -1, -1, -1};  // lastused entry
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	hisnum[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ಪೂರ್ಣಾಙ್ಕ
get_hislen(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ಪೂರ್ಣಾಙ್ಕ hist_type)
{
    ಪ್ರತ್ಯಾಗಮ history[hist_type];
}

#ಯದಿ defined(FEAT_VIMINFO)
    ಶೂನ್ಯ
set_histentry(ಪೂರ್ಣಾಙ್ಕ hist_type, histentry_T *entry)
{
    history[hist_type] = entry;
}
#endif

    ಪೂರ್ಣಾಙ್ಕ *
get_hisidx(ಪೂರ್ಣಾಙ್ಕ hist_type)
{
    ಪ್ರತ್ಯಾಗಮ &hisidx[hist_type];
}

#ಯದಿ defined(FEAT_VIMINFO)
    ಪೂರ್ಣಾಙ್ಕ *
get_hisnum(ಪೂರ್ಣಾಙ್ಕ hist_type)
{
    ಪ್ರತ್ಯಾಗಮ &hisnum[hist_type];
}
#endif

/*
 * Translate a history character to the associated type number.
 */
    ಪೂರ್ಣಾಙ್ಕ
hist_char2type(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c == ':')
	ಪ್ರತ್ಯಾಗಮ HIST_CMD;
    ಯದಿ (c == '=')
	ಪ್ರತ್ಯಾಗಮ HIST_ಅಭಿವ್ಯಕ್ತಿ;
    ಯದಿ (c == '@')
	ಪ್ರತ್ಯಾಗಮ HIST_ಒಳಹರಿವು;
    ಯದಿ (c == '>')
	ಪ್ರತ್ಯಾಗಮ HIST_DEBUG;
    ಪ್ರತ್ಯಾಗಮ HIST_ಹುಡುಕು;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ಸ್ಥಿರ ವರ್ಣ *(history_names[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ರಿಕ್ತ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ    *short_names = ":=@>?/";
    ಪೂರ್ಣಾಙ್ಕ	    short_names_ಎಣಿಕೆ = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(short_names);
    ಪೂರ್ಣಾಙ್ಕ	    history_name_ಎಣಿಕೆ = ARRAY_LENGTH(history_names) - 1;

    ಯದಿ (idx < short_names_ಎಣಿಕೆ)
    {
	xp->xp_buf[0] = (char_u)short_names[idx];
	xp->xp_buf[1] = NUL;
	ಪ್ರತ್ಯಾಗಮ xp->xp_buf;
    }
    ಯದಿ (idx < short_names_ಎಣಿಕೆ + history_name_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ (char_u *)history_names[idx - short_names_ಎಣಿಕೆ];
    ಯದಿ (idx == short_names_ಎಣಿಕೆ + history_name_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"all";
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    ಶೂನ್ಯ
init_history(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		newlen;	    // new length of history table
    histentry_T	*temp;
    ಪೂರ್ಣಾಙ್ಕ		type;

    // If size of history table changed, reallocate it
    newlen = (ಪೂರ್ಣಾಙ್ಕ)p_hi;
    ಯದಿ (newlen == hislen)		// history length didn't change
	ಪ್ರತ್ಯಾಗಮ;

    // history length changed
    ಪ್ರತಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)   // adjust the tables
    {
	ಯದಿ (newlen > 0)
	{
	    temp = ALLOC_MULT(histentry_T, newlen);
	    ಯದಿ (temp == ರಿಕ್ತ)   // out of memory!
	    {
		ಯದಿ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ಭಙ್ಗ;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ಅನುವರ್ತ;
	    }
	}
	ಅನ್ಯಥಾ
	    temp = ರಿಕ್ತ;

	ಯದಿ (hisidx[type] < 0)		// there are no entries yet
	{
	    ಪೂರ್ಣಾಙ್ಕ	i;

	    ಪ್ರತಿ (i = 0; i < newlen; ++i)
		clear_hist_entry(&temp[i]);
	}
	ಅನ್ಯಥಾ ಯದಿ (newlen > hislen)	// array becomes bigger
	{
	    ಪೂರ್ಣಾಙ್ಕ	i;
	    ಪೂರ್ಣಾಙ್ಕ	j;

	    ಪ್ರತಿ (i = 0; i <= hisidx[type]; ++i)
		temp[i] = history[type][i];
	    j = i;
	    ಪ್ರತಿ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_entry(&temp[i]);
	    ಪ್ರತಿ ( ; j < hislen; ++i, ++j)
		temp[i] = history[type][j];
	}
	ಅನ್ಯಥಾ				// array becomes smaller or 0
	{
	    ಪೂರ್ಣಾಙ್ಕ	i;
	    ಪೂರ್ಣಾಙ್ಕ	j;

	    j = hisidx[type];
	    ಪ್ರತಿ (i = newlen - 1; ; --i)
	    {
		ಯದಿ (i >= 0)		// copy newest entries
		    temp[i] = history[type][j];
		ಅನ್ಯಥಾ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ಯದಿ (--j < 0)
		    j = hislen - 1;
		ಯದಿ (j == hisidx[type])
		    ಭಙ್ಗ;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = temp;
    }
    hislen = newlen;
}

    ಶೂನ್ಯ
clear_hist_entry(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ರಿಕ್ತ;
    hisptr->hisstrlen = 0;
    hisptr->time_set = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    ಪೂರ್ಣಾಙ್ಕ
in_history(
    ಪೂರ್ಣಾಙ್ಕ	    type,
    char_u  *str,
    ಪೂರ್ಣಾಙ್ಕ	    move_to_front,	// Move the entry to the front if it exists
    ಪೂರ್ಣಾಙ್ಕ	    sep,
    ಪೂರ್ಣಾಙ್ಕ	    writing)		// ignore entries read from viminfo
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    last_i = -1;
    char_u  *p;
    size_t  len;

    ಯದಿ (hisidx[type] < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    i = hisidx[type];
    ಕುರು
    {
	ಯದಿ (history[type][i].hisstr == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ಯದಿ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_ಹುಡುಕು || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ಯದಿ (!move_to_front)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    last_i = i;
	    ಭಙ್ಗ;
	}
	ಯದಿ (--i < 0)
	    i = hislen - 1;
    } ಯಾವತ್ (i != hisidx[type]);

    ಯದಿ (last_i < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ಯಾವತ್ (i != hisidx[type])
    {
	ಯದಿ (++i >= hislen)
	    i = 0;
	history[type][last_i] = history[type][i];
	last_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].time_set = vim_time();
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_histtype(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name);

    // No argument: use current history.
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ hist_char2type(get_cmdline_firstc());

    ಪ್ರತಿ (i = 0; history_names[i] != ರಿಕ್ತ; ++i)
	ಯದಿ (STRNICMP(name, history_names[i], len) == 0)
	    ಪ್ರತ್ಯಾಗಮ i;

    ಯದಿ (vim_strchr((char_u *)":=@>?/", name[0]) != ರಿಕ್ತ && name[1] == NUL)
	ಪ್ರತ್ಯಾಗಮ hist_char2type(name[0]);

    ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    ಶೂನ್ಯ
add_to_history(
    ಪೂರ್ಣಾಙ್ಕ		histype,
    char_u	*new_entry,
    size_t	new_entrylen,
    ಪೂರ್ಣಾಙ್ಕ		in_map,		// consider maptick when inside a mapping
    ಪೂರ್ಣಾಙ್ಕ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ಯದಿ (hislen == 0)		// no history
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_ಹುಡುಕು)
	ಪ್ರತ್ಯಾಗಮ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ಯದಿ (histype == HIST_ಹುಡುಕು && in_map)
    {
	ಯದಿ (maptick == last_maptick && hisidx[HIST_ಹುಡುಕು] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_ಹುಡುಕು][hisidx[HIST_ಹುಡುಕು]];
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    --hisnum[histype];
	    ಯದಿ (--hisidx[HIST_ಹುಡುಕು] < 0)
		hisidx[HIST_ಹುಡುಕು] = hislen - 1;
	}
	last_maptick = -1;
    }

    ಯದಿ (in_history(histype, new_entry, TRUE, sep, FALSE))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_entry, new_entrylen + 2);
    ಯದಿ (hisptr->hisstr == ರಿಕ್ತ)
	hisptr->hisstrlen = 0;
    ಅನ್ಯಥಾ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->time_set = vim_time();
    ಯದಿ (histype == HIST_ಹುಡುಕು && in_map)
	last_maptick = maptick;
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_history_idx(ಪೂರ್ಣಾಙ್ಕ histype)
{
    ಯದಿ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ
		    || hisidx[histype] < 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತ್ಯಾಗಮ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
calc_hist_idx(ಪೂರ್ಣಾಙ್ಕ histype, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    histentry_T	*hist;
    ಪೂರ್ಣಾಙ್ಕ		wrapped = FALSE;

    ಯದಿ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ
		    || (i = hisidx[histype]) < 0 || ಅಂಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    hist = history[histype];
    ಯದಿ (ಅಂಕೆ > 0)
    {
	ಯಾವತ್ (hist[i].hisnum > ಅಂಕೆ)
	    ಯದಿ (--i < 0)
	    {
		ಯದಿ (wrapped)
		    ಭಙ್ಗ;
		i += hislen;
		wrapped = TRUE;
	    }
	ಯದಿ (i >= 0 && hist[i].hisnum == ಅಂಕೆ && hist[i].hisstr != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ i;
    }
    ಅನ್ಯಥಾ ಯದಿ (-ಅಂಕೆ <= hislen)
    {
	i += ಅಂಕೆ + 1;
	ಯದಿ (i < 0)
	    i += hislen;
	ಯದಿ (hist[i].hisstr != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ i;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clr_history(ಪೂರ್ಣಾಙ್ಕ histype)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    histentry_T	*hisptr;

    ಯದಿ (hislen != 0 && histype >= 0 && histype < HIST_ಎಣಿಕೆ)
    {
	hisptr = history[histype];
	ಪ್ರತಿ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
del_history_entry(ಪೂರ್ಣಾಙ್ಕ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		last;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;

    ಯದಿ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ || *str == NUL
		|| hisidx[histype] < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = last = idx;
    ಕುರು
    {
	hisptr = &history[histype][i];
	ಯದಿ (hisptr->hisstr == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    found = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_entry(hisptr);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (i != last)
	    {
		history[histype][last] = *hisptr;
		clear_hist_entry(hisptr);
	    }
	    ಯದಿ (--last < 0)
		last += hislen;
	}
	ಯದಿ (--i < 0)
	    i += hislen;
    } ಯಾವತ್ (i != idx);

    ಯದಿ (history[histype][idx].hisstr == ರಿಕ್ತ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ಪ್ರತ್ಯಾಗಮ found;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
del_history_idx(ಪೂರ್ಣಾಙ್ಕ histype, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ	    i, j;

    i = calc_hist_idx(histype, idx);
    ಯದಿ (i < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ಯದಿ (histype == HIST_ಹುಡುಕು && maptick == last_maptick && i == idx)
	last_maptick = -1;

    ಯಾವತ್ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_entry(&history[histype][i]);
    ಯದಿ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * "histadd()" function
 */
    ಶೂನ್ಯ
f_histadd(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ರಿಕ್ತ ? get_histtype(str) : -1;
    ಯದಿ (histype < 0)
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_buf(&argvars[1], buf);
    ಯದಿ (*str == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    init_history();
    add_to_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "histdel()" function
 */
    ಶೂನ್ಯ
f_histdel(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    char_u	*str;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ಯದಿ (str == ರಿಕ್ತ)
	n = 0;
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]));
    ಅನ್ಯಥಾ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_entry(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "histget()" function
 */
    ಶೂನ್ಯ
f_histget(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u  *str;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ಯದಿ (str == ರಿಕ್ತ)
	rettv->vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ type;
	ಪೂರ್ಣಾಙ್ಕ idx;

	type = get_histtype(str);
	ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ಅನ್ಯಥಾ
	    idx = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ರಿಕ್ತ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ಯದಿ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    ಶೂನ್ಯ
f_histnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*histname;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ರಿಕ್ತ ? HIST_CMD - 1 : get_histtype(histname);
    ಯದಿ (i >= HIST_CMD && i < HIST_ಎಣಿಕೆ)
	rettv->vval.v_ಸಂಖ್ಯೆ = get_history_idx(i);
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#endif // FEAT_EVAL

#ಯದಿ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    ಶೂನ್ಯ
remove_ಕೀಲಿ_from_history(ಶೂನ್ಯ)
{
    char_u	*p_start;
    char_u	*p_end;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    i = hisidx[HIST_CMD];
    ಯದಿ (i < 0)
	ಪ್ರತ್ಯಾಗಮ;
    p_start = history[HIST_CMD][i].hisstr;
    ಯದಿ (p_start == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    p_end = p_start + history[HIST_CMD][i].hisstrlen;
    ಪ್ರತಿ (p = p_start; *p; ++p)
    {
	ಯದಿ (STRNCMP(p, "key", 3) == 0 && !SAFE_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ಯದಿ (p == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ++p;
	    ಪ್ರತಿ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ಯದಿ (p[i] == '\\' && p[i + 1])
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
    ಶೂನ್ಯ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    ಪೂರ್ಣಾಙ್ಕ		histype1 = HIST_CMD;
    ಪೂರ್ಣಾಙ್ಕ		histype2 = HIST_CMD;
    ಪೂರ್ಣಾಙ್ಕ		hisidx1 = 1;
    ಪೂರ್ಣಾಙ್ಕ		hisidx2 = -1;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		i, j, k;
    char_u	*end;
    char_u	*arg = eap->arg;

    ಯದಿ (hislen == 0)
    {
	ಸಂದೇಶ(_("'history' option is zero"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	end = arg;
	ಯಾವತ್ (ASCII_ISALPHA(*end)
		|| vim_strchr((char_u *)":=@>/?", *end) != ರಿಕ್ತ)
	    end++;
	i = *end;
	*end = NUL;
	histype1 = get_histtype(arg);
	ಯದಿ (histype1 == -1)
	{
	    ಯದಿ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_ಎಣಿಕೆ-1;
	    }
	    ಅನ್ಯಥಾ
	    {
		*end = i;
		semsg(_(e_trailing_characters_str), arg);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
	ಅನ್ಯಥಾ
	    histype2 = histype1;
	*end = i;
    }
    ಅನ್ಯಥಾ
	end = arg;
    ಯದಿ (!get_list_range(&end, &hisidx1, &hisidx2) || *end != NUL)
    {
	ಯದಿ (*end != NUL)
	    semsg(_(e_trailing_characters_str), end);
	ಅನ್ಯಥಾ
	    semsg(_(e_val_too_large), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "\n      #  %s history", history_names[histype1]);
	ಸಂದೇಶ_puts_title((ವರ್ಣ *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ಯದಿ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ಯದಿ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ಯದಿ (idx >= 0 && j <= k)
	    ಪ್ರತಿ (i = idx + 1; !got_int; ++i)
	    {
		ಯದಿ (i == hislen)
		    i = 0;
		ಯದಿ (hist[i].hisstr != ರಿಕ್ತ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !message_filtered(hist[i].hisstr))
		{
		    ಪೂರ್ಣಾಙ್ಕ  len;

		    ಸಂದೇಶ_putchar('\n');
		    len = vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ಯದಿ (vim_strsize(hist[i].hisstr) > (ಪೂರ್ಣಾಙ್ಕ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ಪೂರ್ಣಾಙ್ಕ)Columns - 10, IOSIZE - (ಪೂರ್ಣಾಙ್ಕ)len);
		    ಅನ್ಯಥಾ
			STRCPY(IObuff + len, hist[i].hisstr);
		    ಸಂದೇಶ_outtrans(IObuff);
		    out_flush();
		}
		ಯದಿ (i == idx)
		    ಭಙ್ಗ;
	    }
    }
}
