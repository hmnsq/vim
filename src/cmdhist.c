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

ಸ್ಥಿರ histentry_T *(history[HIST_ಎಣಿಕೆ]) = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	hisidx[HIST_ಎಣಿಕೆ] = {-1, -1, -1, -1, -1};  // lastused entry
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	hisnum[HIST_ಎಣಿಕೆ] = {0, 0, 0, 0, 0};
		    // identifying (unique) number of newest history entry
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	hislen = 0;		// actual length of history tables

/*
 * Return the length of the history tables
 */
    ಪೂರ್ಣಾಂಕ
get_hislen(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ hislen;
}

/*
 * Return a pointer to a specified history table
 */
    histentry_T *
get_histentry(ಪೂರ್ಣಾಂಕ hist_type)
{
    ಹಿಂತಿರುಗಿ history[hist_type];
}

#ಆದರೆ defined(FEAT_VIMINFO)
    ಶೂನ್ಯ
ಗಣ_histentry(ಪೂರ್ಣಾಂಕ hist_type, histentry_T *ನಮೂದು)
{
    history[hist_type] = ನಮೂದು;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ *
get_hisidx(ಪೂರ್ಣಾಂಕ hist_type)
{
    ಹಿಂತಿರುಗಿ &hisidx[hist_type];
}

#ಆದರೆ defined(FEAT_VIMINFO)
    ಪೂರ್ಣಾಂಕ *
get_hisnum(ಪೂರ್ಣಾಂಕ hist_type)
{
    ಹಿಂತಿರುಗಿ &hisnum[hist_type];
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Translate a history character to the associated type number.
 */
    ಪೂರ್ಣಾಂಕ
hist_char2type(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c == ':')
	ಹಿಂತಿರುಗಿ HIST_CMD;
    ಆದರೆ (c == '=')
	ಹಿಂತಿರುಗಿ HIST_ಅಭಿವ್ಯಕ್ತಿ;
    ಆದರೆ (c == '@')
	ಹಿಂತಿರುಗಿ HIST_ಒಳಹರಿವು;
    ಆದರೆ (c == '>')
	ಹಿಂತಿರುಗಿ HIST_DEBUG;
    ಹಿಂತಿರುಗಿ HIST_ಹುಡುಕು;	    // must be '?' or '/'
}

/*
 * Table of history names.
 * These names are used in :history and various hist...() functions.
 * It is sufficient to give the significant prefix of a history name.
 */

ಸ್ಥಿರ ಅಕ್ಷರ *(history_ಹೆಸರುಗಳು[]) =
{
    "cmd",
    "search",
    "expr",
    "input",
    "debug",
    ಖಾಲಿ
};

/*
 * Function given to ExpandGeneric() to obtain the possible first
 * arguments of the ":history command.
 */
    char_u *
get_history_arg(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ    *short_ಹೆಸರುಗಳು = ":=@>?/";
    ಪೂರ್ಣಾಂಕ	    short_ಹೆಸರುಗಳು_ಎಣಿಕೆ = (ಪೂರ್ಣಾಂಕ)STRLEN(short_ಹೆಸರುಗಳು);
    ಪೂರ್ಣಾಂಕ	    history_name_ಎಣಿಕೆ = ARRAY_ಉದ್ದಳತೆ(history_ಹೆಸರುಗಳು) - 1;

    ಆದರೆ (idx < short_ಹೆಸರುಗಳು_ಎಣಿಕೆ)
    {
	xp->xp_buf[0] = (char_u)short_ಹೆಸರುಗಳು[idx];
	xp->xp_buf[1] = NUL;
	ಹಿಂತಿರುಗಿ xp->xp_buf;
    }
    ಆದರೆ (idx < short_ಹೆಸರುಗಳು_ಎಣಿಕೆ + history_name_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ (char_u *)history_ಹೆಸರುಗಳು[idx - short_ಹೆಸರುಗಳು_ಎಣಿಕೆ];
    ಆದರೆ (idx == short_ಹೆಸರುಗಳು_ಎಣಿಕೆ + history_name_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ (char_u *)"all";
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    ಶೂನ್ಯ
init_history(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		newlen;	    // new length of history table
    histentry_T	*ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ		type;

    // If size of history table changed, reallocate it
    newlen = (ಪೂರ್ಣಾಂಕ)p_hi;
    ಆದರೆ (newlen == hislen)		// history length didn't change
	ಹಿಂತಿರುಗಿ;

    // history length changed
    ಗಾಗಿ (type = 0; type < HIST_ಎಣಿಕೆ; ++type)   // adjust the tables
    {
	ಆದರೆ (newlen > 0)
	{
	    ತಾತ್ಕಾಲಿಕ = ALLOC_MULT(histentry_T, newlen);
	    ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಖಾಲಿ)   // out of memory!
	    {
		ಆದರೆ (type == 0)  // first one: just keep the old length
		{
		    newlen = hislen;
		    ನಿಲ್ಲಿಸು;
		}
		// Already changed one table, now we can only have zero
		// length for all tables.
		newlen = 0;
		type = -1;
		ಮುಂದುವರಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ತಾತ್ಕಾಲಿಕ = ಖಾಲಿ;

	ಆದರೆ (hisidx[type] < 0)		// there are no entries yet
	{
	    ಪೂರ್ಣಾಂಕ	i;

	    ಗಾಗಿ (i = 0; i < newlen; ++i)
		clear_hist_ನಮೂದು(&ತಾತ್ಕಾಲಿಕ[i]);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newlen > hislen)	// array becomes bigger
	{
	    ಪೂರ್ಣಾಂಕ	i;
	    ಪೂರ್ಣಾಂಕ	j;

	    ಗಾಗಿ (i = 0; i <= hisidx[type]; ++i)
		ತಾತ್ಕಾಲಿಕ[i] = history[type][i];
	    j = i;
	    ಗಾಗಿ ( ; i <= newlen - (hislen - hisidx[type]); ++i)
		clear_hist_ನಮೂದು(&ತಾತ್ಕಾಲಿಕ[i]);
	    ಗಾಗಿ ( ; j < hislen; ++i, ++j)
		ತಾತ್ಕಾಲಿಕ[i] = history[type][j];
	}
	ಇಲ್ಲದಿದ್ದರೆ				// array becomes smaller or 0
	{
	    ಪೂರ್ಣಾಂಕ	i;
	    ಪೂರ್ಣಾಂಕ	j;

	    j = hisidx[type];
	    ಗಾಗಿ (i = newlen - 1; ; --i)
	    {
		ಆದರೆ (i >= 0)		// copy newest entries
		    ತಾತ್ಕಾಲಿಕ[i] = history[type][j];
		ಇಲ್ಲದಿದ್ದರೆ			// remove older entries
		{
		    vim_free(history[type][j].hisstr);
		    history[type][j].hisstrlen = 0;
		}
		ಆದರೆ (--j < 0)
		    j = hislen - 1;
		ಆದರೆ (j == hisidx[type])
		    ನಿಲ್ಲಿಸು;
	    }
	    hisidx[type] = newlen - 1;
	}
	vim_free(history[type]);
	history[type] = ತಾತ್ಕಾಲಿಕ;
    }
    hislen = newlen;
}

    ಶೂನ್ಯ
clear_hist_ನಮೂದು(histentry_T *hisptr)
{
    hisptr->hisnum = 0;
    hisptr->viminfo = FALSE;
    hisptr->hisstr = ಖಾಲಿ;
    hisptr->hisstrlen = 0;
    hisptr->ಸಮಯ_ಗಣ = 0;
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    ಪೂರ್ಣಾಂಕ
ಒಳ_history(
    ಪೂರ್ಣಾಂಕ	    type,
    char_u  *str,
    ಪೂರ್ಣಾಂಕ	    move_ಗಮ್ಯ_front,	// Move the entry to the front if it exists
    ಪೂರ್ಣಾಂಕ	    sep,
    ಪೂರ್ಣಾಂಕ	    writing)		// ignore entries read from viminfo
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ಕೊನೆಯ_i = -1;
    char_u  *p;
    size_t  len;

    ಆದರೆ (hisidx[type] < 0)
	ಹಿಂತಿರುಗಿ FALSE;
    i = hisidx[type];
    ಮಾಡು
    {
	ಆದರೆ (history[type][i].hisstr == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;

	// For search history, check that the separator character matches as
	// well.
	p = history[type][i].hisstr;
	ಆದರೆ (STRCMP(str, p) == 0
		&& !(writing && history[type][i].viminfo)
		&& (type != HIST_ಹುಡುಕು || sep == p[history[type][i].hisstrlen + 1]))
	{
	    ಆದರೆ (!move_ಗಮ್ಯ_front)
		ಹಿಂತಿರುಗಿ TRUE;
	    ಕೊನೆಯ_i = i;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (--i < 0)
	    i = hislen - 1;
    } ಆಗ (i != hisidx[type]);

    ಆದರೆ (ಕೊನೆಯ_i < 0)
	ಹಿಂತಿರುಗಿ FALSE;

    str = history[type][i].hisstr;
    len = history[type][i].hisstrlen;
    ಆಗ (i != hisidx[type])
    {
	ಆದರೆ (++i >= hislen)
	    i = 0;
	history[type][ಕೊನೆಯ_i] = history[type][i];
	ಕೊನೆಯ_i = i;
    }
    history[type][i].hisnum = ++hisnum[type];
    history[type][i].viminfo = FALSE;
    history[type][i].hisstr = str;
    history[type][i].hisstrlen = len;
    history[type][i].ಸಮಯ_ಗಣ = vim_ಸಮಯ();
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Convert history name (from table above) to its HIST_ equivalent.
 * When "name" is empty, return "cmd" history.
 * Returns -1 for unknown history name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_histtype(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(name);

    // No argument: use current history.
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ hist_char2type(get_cmdline_firstc());

    ಗಾಗಿ (i = 0; history_ಹೆಸರುಗಳು[i] != ಖಾಲಿ; ++i)
	ಆದರೆ (STRNICMP(name, history_ಹೆಸರುಗಳು[i], len) == 0)
	    ಹಿಂತಿರುಗಿ i;

    ಆದರೆ (vim_strchr((char_u *)":=@>?/", name[0]) != ಖಾಲಿ && name[1] == NUL)
	ಹಿಂತಿರುಗಿ hist_char2type(name[0]);

    ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_maptick = -1;	// last seen maptick

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be one of he HIST_
 * values.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_ಗಮ್ಯ_history(
    ಪೂರ್ಣಾಂಕ		histype,
    char_u	*new_ನಮೂದು,
    size_t	new_entrylen,
    ಪೂರ್ಣಾಂಕ		ಒಳ_map,		// consider maptick when inside a mapping
    ಪೂರ್ಣಾಂಕ		sep)		// separator character used (search hist)
{
    histentry_T	*hisptr;

    ಆದರೆ (hislen == 0)		// no history
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPPATTERNS) && histype == HIST_ಹುಡುಕು)
	ಹಿಂತಿರುಗಿ;

    // Searches inside the same mapping overwrite each other, so that only
    // the last line is kept.  Be careful not to remove a line that was moved
    // down, only lines that were added.
    ಆದರೆ (histype == HIST_ಹುಡುಕು && ಒಳ_map)
    {
	ಆದರೆ (maptick == ಕೊನೆಯ_maptick && hisidx[HIST_ಹುಡುಕು] >= 0)
	{
	    // Current line is from the same mapping, remove it
	    hisptr = &history[HIST_ಹುಡುಕು][hisidx[HIST_ಹುಡುಕು]];
	    vim_free(hisptr->hisstr);
	    clear_hist_ನಮೂದು(hisptr);
	    --hisnum[histype];
	    ಆದರೆ (--hisidx[HIST_ಹುಡುಕು] < 0)
		hisidx[HIST_ಹುಡುಕು] = hislen - 1;
	}
	ಕೊನೆಯ_maptick = -1;
    }

    ಆದರೆ (ಒಳ_history(histype, new_ನಮೂದು, TRUE, sep, FALSE))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (++hisidx[histype] == hislen)
	hisidx[histype] = 0;
    hisptr = &history[histype][hisidx[histype]];
    vim_free(hisptr->hisstr);

    // Store the separator after the NUL of the string.
    hisptr->hisstr = vim_strnsave(new_ನಮೂದು, new_entrylen + 2);
    ಆದರೆ (hisptr->hisstr == ಖಾಲಿ)
	hisptr->hisstrlen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	hisptr->hisstr[new_entrylen + 1] = sep;
	hisptr->hisstrlen = new_entrylen;
    }

    hisptr->hisnum = ++hisnum[histype];
    hisptr->viminfo = FALSE;
    hisptr->ಸಮಯ_ಗಣ = vim_ಸಮಯ();
    ಆದರೆ (histype == HIST_ಹುಡುಕು && ಒಳ_map)
	ಕೊನೆಯ_maptick = maptick;
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Get identifier of newest history entry.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_history_idx(ಪೂರ್ಣಾಂಕ histype)
{
    ಆದರೆ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ
		    || hisidx[histype] < 0)
	ಹಿಂತಿರುಗಿ -1;

    ಹಿಂತಿರುಗಿ history[histype][hisidx[histype]].hisnum;
}

/*
 * Calculate history index from a number:
 *   num > 0: seen as identifying number of a history entry
 *   num < 0: relative position in history wrt newest entry
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
calc_hist_idx(ಪೂರ್ಣಾಂಕ histype, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
    ಪೂರ್ಣಾಂಕ		i;
    histentry_T	*hist;
    ಪೂರ್ಣಾಂಕ		wrapped = FALSE;

    ಆದರೆ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ
		    || (i = hisidx[histype]) < 0 || ಅಂಕೆ == 0)
	ಹಿಂತಿರುಗಿ -1;

    hist = history[histype];
    ಆದರೆ (ಅಂಕೆ > 0)
    {
	ಆಗ (hist[i].hisnum > ಅಂಕೆ)
	    ಆದರೆ (--i < 0)
	    {
		ಆದರೆ (wrapped)
		    ನಿಲ್ಲಿಸು;
		i += hislen;
		wrapped = TRUE;
	    }
	ಆದರೆ (i >= 0 && hist[i].hisnum == ಅಂಕೆ && hist[i].hisstr != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ i;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (-ಅಂಕೆ <= hislen)
    {
	i += ಅಂಕೆ + 1;
	ಆದರೆ (i < 0)
	    i += hislen;
	ಆದರೆ (hist[i].hisstr != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ i;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Clear all entries of a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clr_history(ಪೂರ್ಣಾಂಕ histype)
{
    ಪೂರ್ಣಾಂಕ		i;
    histentry_T	*hisptr;

    ಆದರೆ (hislen != 0 && histype >= 0 && histype < HIST_ಎಣಿಕೆ)
    {
	hisptr = history[histype];
	ಗಾಗಿ (i = hislen; i--;)
	{
	    vim_free(hisptr->hisstr);
	    clear_hist_ನಮೂದು(hisptr);
	    hisptr++;
	}
	hisidx[histype] = -1;	// mark history as cleared
	hisnum[histype] = 0;	// reset identifier counter
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Remove all entries matching {str} from a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
del_history_ನಮೂದು(ಪೂರ್ಣಾಂಕ histype, char_u *str)
{
    regmatch_T	regmatch;
    histentry_T	*hisptr;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;

    ಆದರೆ (hislen == 0 || histype < 0 || histype >= HIST_ಎಣಿಕೆ || *str == NUL
		|| hisidx[histype] < 0)
	ಹಿಂತಿರುಗಿ FALSE;

    idx = hisidx[histype];
    regmatch.regprog = vim_regcomp(str, RE_MAGIC + RE_STRING);
    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    regmatch.rm_ic = FALSE;	// always match case

    i = ಕೊನೆಯ = idx;
    ಮಾಡು
    {
	hisptr = &history[histype][i];
	ಆದರೆ (hisptr->hisstr == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (vim_regexec(&regmatch, hisptr->hisstr, (colnr_T)0))
	{
	    ಸಿಕ್ಕಿತು = TRUE;
	    vim_free(hisptr->hisstr);
	    clear_hist_ನಮೂದು(hisptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (i != ಕೊನೆಯ)
	    {
		history[histype][ಕೊನೆಯ] = *hisptr;
		clear_hist_ನಮೂದು(hisptr);
	    }
	    ಆದರೆ (--ಕೊನೆಯ < 0)
		ಕೊನೆಯ += hislen;
	}
	ಆದರೆ (--i < 0)
	    i += hislen;
    } ಆಗ (i != idx);

    ಆದರೆ (history[histype][idx].hisstr == ಖಾಲಿ)
	hisidx[histype] = -1;

    vim_regfree(regmatch.regprog);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

/*
 * Remove an indexed entry from a history.
 * "histype" may be one of the HIST_ values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
del_history_idx(ಪೂರ್ಣಾಂಕ histype, ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ	    i, j;

    i = calc_hist_idx(histype, idx);
    ಆದರೆ (i < 0)
	ಹಿಂತಿರುಗಿ FALSE;
    idx = hisidx[histype];
    vim_free(history[histype][i].hisstr);
    history[histype][i].hisstrlen = 0;

    // When deleting the last added search string in a mapping, reset
    // last_maptick, so that the last added search string isn't deleted again.
    ಆದರೆ (histype == HIST_ಹುಡುಕು && maptick == ಕೊನೆಯ_maptick && i == idx)
	ಕೊನೆಯ_maptick = -1;

    ಆಗ (i != idx)
    {
	j = (i + 1) % hislen;
	history[histype][i] = history[histype][j];
	i = j;
    }
    clear_hist_ನಮೂದು(&history[histype][i]);
    ಆದರೆ (--i < 0)
	i += hislen;
    hisidx[histype] = i;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * "histadd()" function
 */
    ಶೂನ್ಯ
f_histadd(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		histype;
    char_u	*str;
    char_u	buf[NUMBUFLEN];

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;
    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    histype = str != ಖಾಲಿ ? get_histtype(str) : -1;
    ಆದರೆ (histype < 0)
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_buf(&argvars[1], buf);
    ಆದರೆ (*str == NUL)
	ಹಿಂತಿರುಗಿ;

    init_history();
    ಕೂಡಿಸು_ಗಮ್ಯ_history(histype, str, STRLEN(str), FALSE, NUL);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "histdel()" function
 */
    ಶೂನ್ಯ
f_histdel(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*str;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ಆದರೆ (str == ಖಾಲಿ)
	n = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	// only one argument: clear entire history
	n = clr_history(get_histtype(str));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	// index given: remove that entry
	n = del_history_idx(get_histtype(str),
					  (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	buf[NUMBUFLEN];

	// string given: remove all matching entries
	n = del_history_ನಮೂದು(get_histtype(str),
				      tv_get_string_buf(&argvars[1], buf));
    }

    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * "histget()" function
 */
    ಶೂನ್ಯ
f_histget(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    char_u  *str;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    str = tv_get_string_chk(&argvars[0]);	// NULL on type error
    ಆದರೆ (str == ಖಾಲಿ)
	rettv->vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ type;
	ಪೂರ್ಣಾಂಕ idx;

	type = get_histtype(str);
	ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	    idx = get_history_idx(type);
	ಇಲ್ಲದಿದ್ದರೆ
	    idx = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], ಖಾಲಿ);
						    // -1 on type error

	idx = calc_hist_idx(type, idx);
	ಆದರೆ (idx < 0)
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = vim_strnsave(history[type][idx].hisstr, history[type][idx].hisstrlen);
    }
    rettv->v_type = VAR_STRING;
}

/*
 * "histnr()" function
 */
    ಶೂನ್ಯ
f_histnr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*histname;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    histname = tv_get_string_chk(&argvars[0]);
    i = histname == ಖಾಲಿ ? HIST_CMD - 1 : get_histtype(histname);
    ಆದರೆ (i >= HIST_CMD && i < HIST_ಎಣಿಕೆ)
	rettv->vval.v_ಸಂಖ್ಯೆ = get_history_idx(i);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * Very specific function to remove the value in ":set key=val" from the
 * history.
 */
    ಶೂನ್ಯ
remove_ಕೀಲಿ_from_history(ಶೂನ್ಯ)
{
    char_u	*p_ಆರಂಭ;
    char_u	*p_ಕೊನೆ;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;

    i = hisidx[HIST_CMD];
    ಆದರೆ (i < 0)
	ಹಿಂತಿರುಗಿ;
    p_ಆರಂಭ = history[HIST_CMD][i].hisstr;
    ಆದರೆ (p_ಆರಂಭ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p_ಕೊನೆ = p_ಆರಂಭ + history[HIST_CMD][i].hisstrlen;
    ಗಾಗಿ (p = p_ಆರಂಭ; *p; ++p)
    {
	ಆದರೆ (STRNCMP(p, "key", 3) == 0 && !ಸುರಕ್ಷಿತ_isalpha(p[3]))
	{
	    p = vim_strchr(p + 3, '=');
	    ಆದರೆ (p == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ++p;
	    ಗಾಗಿ (i = 0; p[i] && !VIM_ISWHITE(p[i]); ++i)
		ಆದರೆ (p[i] == '\\' && p[i + 1])
		    ++i;

	    mch_memmove(p, p + i, (p_ಕೊನೆ - (p + i)) + 1);	    // +1 for the NUL
	    p_ಕೊನೆ -= i;						    // adjust p_end for shortened string
	    --p;
	}
    }

    history[HIST_CMD][i].hisstrlen = (size_t)(p_ಕೊನೆ - p_ಆರಂಭ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * :history command - print a history
 */
    ಶೂನ್ಯ
ex_history(exarg_T *eap)
{
    histentry_T	*hist;
    ಪೂರ್ಣಾಂಕ		histype1 = HIST_CMD;
    ಪೂರ್ಣಾಂಕ		histype2 = HIST_CMD;
    ಪೂರ್ಣಾಂಕ		hisidx1 = 1;
    ಪೂರ್ಣಾಂಕ		hisidx2 = -1;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		i, j, k;
    char_u	*ಕೊನೆ;
    char_u	*arg = eap->arg;

    ಆದರೆ (hislen == 0)
    {
	ಸಂದೇಶ(_("'history' option is zero"));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!(VIM_ISDIGIT(*arg) || *arg == '-' || *arg == ','))
    {
	ಕೊನೆ = arg;
	ಆಗ (ASCII_ISALPHA(*ಕೊನೆ)
		|| vim_strchr((char_u *)":=@>/?", *ಕೊನೆ) != ಖಾಲಿ)
	    ಕೊನೆ++;
	i = *ಕೊನೆ;
	*ಕೊನೆ = NUL;
	histype1 = get_histtype(arg);
	ಆದರೆ (histype1 == -1)
	{
	    ಆದರೆ (STRNICMP(arg, "all", STRLEN(arg)) == 0)
	    {
		histype1 = 0;
		histype2 = HIST_ಎಣಿಕೆ-1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*ಕೊನೆ = i;
		semsg(_(e_trailing_characters_str), arg);
		ಹಿಂತಿರುಗಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    histype2 = histype1;
	*ಕೊನೆ = i;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆ = arg;
    ಆದರೆ (!get_list_range(&ಕೊನೆ, &hisidx1, &hisidx2) || *ಕೊನೆ != NUL)
    {
	ಆದರೆ (*ಕೊನೆ != NUL)
	    semsg(_(e_trailing_characters_str), ಕೊನೆ);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_val_too_large), arg);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (; !got_int && histype1 <= histype2; ++histype1)
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "\n      #  %s history", history_ಹೆಸರುಗಳು[histype1]);
	ಸಂದೇಶ_puts_title((ಅಕ್ಷರ *)IObuff);
	idx = hisidx[histype1];
	hist = history[histype1];
	j = hisidx1;
	k = hisidx2;
	ಆದರೆ (j < 0)
	    j = (-j > hislen) ? 0 : hist[(hislen + j + idx + 1) % hislen].hisnum;
	ಆದರೆ (k < 0)
	    k = (-k > hislen) ? 0 : hist[(hislen + k + idx + 1) % hislen].hisnum;
	ಆದರೆ (idx >= 0 && j <= k)
	    ಗಾಗಿ (i = idx + 1; !got_int; ++i)
	    {
		ಆದರೆ (i == hislen)
		    i = 0;
		ಆದರೆ (hist[i].hisstr != ಖಾಲಿ
			&& hist[i].hisnum >= j && hist[i].hisnum <= k
			&& !ಸಂದೇಶ_filtered(hist[i].hisstr))
		{
		    ಪೂರ್ಣಾಂಕ  len;

		    ಸಂದೇಶ_putchar('\n');
		    len = vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
				"%c%6d  ", i == idx ? '>' : ' ', hist[i].hisnum);
		    ಆದರೆ (vim_strsize(hist[i].hisstr) > (ಪೂರ್ಣಾಂಕ)Columns - 10)
			trunc_string(hist[i].hisstr, IObuff + len,
			     (ಪೂರ್ಣಾಂಕ)Columns - 10, IOSIZE - (ಪೂರ್ಣಾಂಕ)len);
		    ಇಲ್ಲದಿದ್ದರೆ
			STRCPY(IObuff + len, hist[i].hisstr);
		    ಸಂದೇಶ_outtrans(IObuff);
		    ಹೊರಹೋಗು_flush();
		}
		ಆದರೆ (i == idx)
		    ನಿಲ್ಲಿಸು;
	    }
    }
}
