/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * quickfix.c: functions for quickfix mode, using a file with error messages
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_QUICKFIX)

నిర్మాణం dir_దొంతర_T
{
    నిర్మాణం dir_దొంతర_T	*తదుపరి;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
రకనిర్వచనం నిర్మాణం qfline_S qfline_T;
నిర్మాణం qfline_S
{
    qfline_T	*qf_తదుపరి;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_చివర_lnum;	// line number when the error has range or zero
    పూర్ణాంకం		qf_fnum;	// file number for the line
    పూర్ణాంకం		qf_నిలువు;		// column where the error occurred
    పూర్ణాంకం		qf_చివర_నిలువు;	// column when the error has range or zero
    పూర్ణాంకం		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_నమూనా;	// search pattern for the error
    char_u	*qf_పాఠ్యం;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_దత్తాంశం;	// custom user data associated with this item
    char_u	qf_valid;	// valid error message detected
};

/*
 * There is a stack of error lists.
 */
# define INVALID_QFIDX (-1)
# define INVALID_QFBUFNR (0)

/*
 * Quickfix list type.
 */
రకనిర్వచనం గణన
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_అంతర్గత  // Internal - Temporary list used by getqflist()/getloclist()
} qfltype_T;

/*
 * Quickfix/Location list definition
 * Contains a list of entries (qfline_T). qf_start points to the first entry
 * and qf_last points to the last entry. qf_count contains the list size.
 *
 * Usually the list contains one or more entries. But an empty list can be
 * created using setqflist()/setloclist() with a title and/or user context
 * information and entries can be added later using setqflist()/setloclist().
 */
రకనిర్వచనం నిర్మాణం qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_ప్రారంభం;	// pointer to the first error
    qfline_T	*qf_చివరి;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    పూర్ణాంకం		qf_లెక్క;	// number of errors (0 means empty list)
    పూర్ణాంకం		qf_సూచీ;	// current index in the error list
    పూర్ణాంకం		qf_nonevalid;	// TRUE if not a single valid entry found
    పూర్ణాంకం		qf_has_user_దత్తాంశం; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    నిర్మాణం dir_దొంతర_T	*qf_dir_దొంతర;
    char_u		*qf_directory;
    నిర్మాణం dir_దొంతర_T	*qf_file_దొంతర;
    char_u		*qf_currfile;
    పూర్ణాంకం			qf_multiline;
    పూర్ణాంకం			qf_multiignore;
    పూర్ణాంకం			qf_multiscan;
    దీర్ఘ		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
నిర్మాణం qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    పూర్ణాంకం		qf_refcount;
    పూర్ణాంకం		qf_listcount;	    // current number of lists
    పూర్ణాంకం		qf_curlist;	    // current error list
    పూర్ణాంకం         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    పూర్ణాంకం		qf_bufnr;	    // quickfix window buffer number
};

స్థిర qf_info_T ql_info_actual; // global quickfix list
స్థిర qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
స్థిర int_u చివరి_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
రకనిర్వచనం నిర్మాణం efm_S efm_T;
నిర్మాణం efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *తదుపరి;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    ఉపసర్గ;	// prefix of this format line:
				//   'D' enter directory
				//   'X' leave directory
				//   'A' start of multi-line message
				//   'E' error message
				//   'W' warning message
				//   'I' informational message
				//   'N' note message
				//   'C' continuation line
				//   'Z' end of multi-line message
				//   'G' general, unspecific message
				//   'P' push file (partial) message
				//   'Q' pop/quit file (partial) message
				//   'O' overread (partial) message
    char_u	    flags;	// additional flags given in prefix
				//   '-' do not include this line
				//   '+' include whole line in message
    పూర్ణాంకం		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
రకనిర్వచనం నిర్మాణం qf_delq_S
{
    నిర్మాణం qf_delq_S	*తదుపరి;
    qf_info_T		*qi;
} qf_delq_T;
స్థిర qf_delq_T *qf_delq_తల = ఖాళీ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
స్థిర పూర్ణాంకం	quickfix_busy = 0;

స్థిర efm_T	*fmt_ప్రారంభం = ఖాళీ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
స్థిర callback_T qftf_cb;

స్థిర శూన్య     qf_తీయి_దొంతర(qf_info_T *qi, పూర్ణాంకం adjust);
స్థిర శూన్య	qf_new_list(qf_info_T *qi, char_u *qf_title);
స్థిర పూర్ణాంకం	qf_కూడు_నమోదు(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, పూర్ణాంకం bufnum, char_u *mesg, దీర్ఘ lnum, దీర్ఘ చివర_lnum, పూర్ణాంకం నిలువు, పూర్ణాంకం చివర_నిలువు, పూర్ణాంకం vis_నిలువు, char_u *నమూనా, పూర్ణాంకం nr, పూర్ణాంకం type, typval_T *user_దత్తాంశం, పూర్ణాంకం valid);
స్థిర పూర్ణాంకం      qf_resize_దొంతర_పునాది(qf_info_T *qi, పూర్ణాంకం n);
స్థిర శూన్య     qf_sync_llw_గమ్యం_win(win_T *llw);
స్థిర శూన్య     qf_sync_win_గమ్యం_llw(win_T *pwp);
స్థిర qf_info_T *qf_alloc_దొంతర(qfltype_T qfltype, పూర్ణాంకం n);
స్థిర qf_list_T *qf_alloc_list_దొంతర(పూర్ణాంకం n);
స్థిర శూన్య	qf_free(qf_list_T *qfl);
స్థిర char_u	*qf_types(పూర్ణాంకం, పూర్ణాంకం);
స్థిర పూర్ణాంకం	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
స్థిర char_u	*qf_నెట్టు_dir(char_u *, నిర్మాణం dir_దొంతర_T **, పూర్ణాంకం is_file_దొంతర);
స్థిర char_u	*qf_తీయి_dir(నిర్మాణం dir_దొంతర_T **);
స్థిర char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
స్థిర win_T    *qf_కనుగొను_win_with_loclist(qf_info_T *ll);
స్థిర శూన్య	qf_jump_newwin(qf_info_T *qi, పూర్ణాంకం dir, పూర్ణాంకం errornr, పూర్ణాంకం forceit, పూర్ణాంకం newwin);
స్థిర శూన్య	qf_fmt_పాఠ్యం(garray_T *gap, char_u *పాఠ్యం);
స్థిర శూన్య	qf_range_పాఠ్యం(garray_T *gap, qfline_T *qfp);
స్థిర పూర్ణాంకం	qf_win_pos_update(qf_info_T *qi, పూర్ణాంకం old_qf_సూచీ);
స్థిర win_T	*qf_కనుగొను_win(qf_info_T *qi);
స్థిర buf_T	*qf_కనుగొను_buf(qf_info_T *qi);
స్థిర శూన్య	qf_update_బఫరు(qf_info_T *qi, qfline_T *old_చివరి);
స్థిర శూన్య	qf_fill_బఫరు(qf_list_T *qfl, buf_T *buf, qfline_T *old_చివరి, పూర్ణాంకం qf_winid);
స్థిర buf_T	*ఎక్కించు_dummy_బఫరు(char_u *fname, char_u *dirname_ప్రారంభం, char_u *resulting_dir);
స్థిర శూన్య	wipe_dummy_బఫరు(buf_T *buf, char_u *dirname_ప్రారంభం);
స్థిర శూన్య	unload_dummy_బఫరు(buf_T *buf, char_u *dirname_ప్రారంభం);
స్థిర qf_info_T *ll_get_or_alloc_list(win_T *);
స్థిర పూర్ణాంకం	నమోదు_is_closer_గమ్యం_లక్ష్యం(qfline_T *నమోదు_2, qfline_T *other_నమోదు, పూర్ణాంకం లక్ష్యం_fnum, పూర్ణాంకం లక్ష్యం_lnum, పూర్ణాంకం లక్ష్యం_నిలువు);

// Quickfix window check helper macro
# define IS_QF_కిటికీ(wp) (bt_quickfix((wp)->w_బఫరు) && (wp)->w_llist_ref == ఖాళీ)
// Location list window check helper macro
# define IS_LL_కిటికీ(wp) (bt_quickfix((wp)->w_బఫరు) && (wp)->w_llist_ref != ఖాళీ)

// Quickfix and location list stack check helper macros
# define IS_QF_దొంతర(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_దొంతర(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_కిటికీ(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_వస్తువులు(qfl, qfp, i) \
		    కొరకు ((i) = 1, (qfp) = (qfl)->qf_ప్రారంభం; \
			    !got_int && (i) <= (qfl)->qf_లెక్క && (qfp) != ఖాళీ; \
			    ++(i), (qfp) = (qfp)->qf_తదుపరి)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
స్థిర char_u   *qf_చివరి_bufname = ఖాళీ;
స్థిర bufref_T  qf_చివరి_bufref = {ఖాళీ, 0, 0};

స్థిర garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    స్థిర garray_T *
qfga_get(శూన్య)
{
    స్థిర పూర్ణాంకం initialized = FALSE;

    ఒకవేళ (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    తిరిగి &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    స్థిర శూన్య
qfga_clear(శూన్య)
{
    ఒకవేళ (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    లేకపోతే
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define పంక్తి_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
స్థిర నిర్మాణం fmtpattern
{
    char_u	convchar;
    అక్షరం	*నమూనా;
} fmt_pat[FMT_PATTERNS] =
    {
	{'f', ".\\+"},	    // only used when at end
	{'b', "\\d\\+"},	// 1
	{'n', "\\d\\+"},	// 2
	{'l', "\\d\\+"},	// 3
	{'e', "\\d\\+"},	// 4
	{'c', "\\d\\+"},	// 5
	{'k', "\\d\\+"},	// 6
	{'t', "."},		// 7
# define FMT_నమూనా_M 8
	{'m', ".\\+"},		// 8
# define FMT_నమూనా_R 9
	{'r', ".*"},		// 9
	{'p', "[-	 .]*"},	// 10
	{'v', "\\d\\+"},	// 11
	{'s', ".\\+"},		// 12
	{'o', ".\\+"}		// 13
    };

/*
 * Convert an errorformat pattern to a regular expression pattern.
 * See fmt_pat definition above for the list of supported patterns.  The
 * pattern specifier is supplied in "efmpat".  The converted pattern is stored
 * in "regpat".  Returns a pointer to the location after the pattern.
 */
    స్థిర char_u *
efmpat_గమ్యం_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	పూర్ణాంకం	idx,
	పూర్ణాంకం	విడత)
{
    char_u	*srcptr;

    ఒకవేళ (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_లోపల_format_string), *efmpat);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ ((idx && idx < FMT_నమూనా_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->ఉపసర్గ) != ఖాళీ)
	    || (idx == FMT_నమూనా_R
		&& vim_strchr((char_u *)"OPQ", efminfo->ఉపసర్గ) == ఖాళీ))
    {
	semsg(_(e_unexpected_chr_లోపల_format_str), *efmpat);
	తిరిగి ఖాళీ;
    }
    efminfo->addr[idx] = (char_u)++విడత;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_లోపల_FILENAME
    ఒకవేళ (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    ఒకవేళ (*efmpat == 'f' && efmpat[1] != NUL)
    {
	ఒకవేళ (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	లేకపోతే
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    లేకపోతే
    {
	srcptr = (char_u *)fmt_pat[idx].నమూనా;
	అయితే ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    తిరిగి regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    స్థిర char_u *
scanf_fmt_గమ్యం_regpat(
	char_u	**pefmp,
	char_u	*efm,
	పూర్ణాంకం	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    ఒకవేళ (*efmp == '[' || *efmp == '\\')
    {
	ఒకవేళ ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    ఒకవేళ (efmp[1] == '^')
		*regpat++ = *++efmp;
	    ఒకవేళ (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		అయితే (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		ఒకవేళ (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_లోపల_format_string));
		    తిరిగి ఖాళీ;
		}
	    }
	}
	లేకపోతే ఒకవేళ (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    లేకపోతే
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_లోపల_format_string), *efmp);
	తిరిగి ఖాళీ;
    }

    *pefmp = efmp;

    తిరిగి regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    స్థిర char_u *
efm_analyze_ఉపసర్గ(char_u *efmp, efm_T *efminfo)
{
    ఒకవేళ (vim_strchr((char_u *)"+-", *efmp) != ఖాళీ)
	efminfo->flags = *efmp++;
    ఒకవేళ (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ఖాళీ)
	efminfo->ఉపసర్గ = *efmp;
    లేకపోతే
    {
	semsg(_(e_invalid_chr_లోపల_format_string_ఉపసర్గ), *efmp);
	తిరిగి ఖాళీ;
    }

    తిరిగి efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
efm_గమ్యం_regpat(
	char_u	*efm,
	పూర్ణాంకం	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    విడత = 0;
    కొరకు (efmp = efm; efmp < efm + len; ++efmp)
    {
	ఒకవేళ (*efmp == '%')
	{
	    ++efmp;
	    కొరకు (idx = 0; idx < FMT_PATTERNS; ++idx)
		ఒకవేళ (fmt_pat[idx].convchar == *efmp)
		    ఆపు;
	    ఒకవేళ (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_గమ్యం_regpat(efmp, ptr, fmt_ptr, idx, విడత);
		ఒకవేళ (ptr == ఖాళీ)
		    తిరిగి FAIL;
		విడత++;
	    }
	    లేకపోతే ఒకవేళ (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_గమ్యం_regpat(&efmp, efm, len, ptr);
		ఒకవేళ (ptr == ఖాళీ)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ఖాళీ)
		*ptr++ = *efmp;		// regexp magic characters
	    లేకపోతే ఒకవేళ (*efmp == '#')
		*ptr++ = '*';
	    లేకపోతే ఒకవేళ (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    లేకపోతే ఒకవేళ (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_ఉపసర్గ(efmp, fmt_ptr);
		ఒకవేళ (efmp == ఖాళీ)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		semsg(_(e_invalid_chr_లోపల_format_string), *efmp);
		తిరిగి FAIL;
	    }
	}
	లేకపోతే			// copy normal character
	{
	    ఒకవేళ (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    లేకపోతే ఒకవేళ (vim_strchr((char_u *)".*^$~[", *efmp) != ఖాళీ)
		*ptr++ = '\\';	// escape regexp atoms
	    ఒకవేళ (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    తిరిగి OK;
}

/*
 * Free the 'errorformat' information list
 */
    స్థిర శూన్య
free_efm_list(efm_T **efm_మొదటి)
{
    efm_T *efm_ptr;

    కొరకు (efm_ptr = *efm_మొదటి; efm_ptr != ఖాళీ; efm_ptr = *efm_మొదటి)
    {
	*efm_మొదటి = efm_ptr->తదుపరి;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_ప్రారంభం = ఖాళీ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    స్థిర పూర్ణాంకం
efm_regpat_bufsz(char_u *efm)
{
    పూర్ణాంకం sz;
    పూర్ణాంకం i;

    sz = (FMT_PATTERNS * 3) + ((పూర్ణాంకం)STRLEN(efm) << 2);
    కొరకు (i = FMT_PATTERNS; i > 0; )
	sz += (పూర్ణాంకం)STRLEN(fmt_pat[--i].నమూనా);
# ifdef BACKSLASH_లోపల_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# లేకపోతే
    sz += 2; // "%f" can become two chars longer
# endif

    తిరిగి sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    స్థిర పూర్ణాంకం
efm_option_part_len(char_u *efm)
{
    పూర్ణాంకం len;

    కొరకు (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	ఒకవేళ (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    తిరిగి len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    స్థిర efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ఖాళీ;
    efm_T	*fmt_మొదటి = ఖాళీ;
    efm_T	*fmt_చివరి = ఖాళీ;
    char_u	*fmtstr = ఖాళీ;
    పూర్ణాంకం		len;
    పూర్ణాంకం		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    ఒకవేళ ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ఖాళీ)
	వెళ్ళు parse_efm_error;

    అయితే (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	ఒకవేళ (fmt_ptr == ఖాళీ)
	    వెళ్ళు parse_efm_error;
	ఒకవేళ (fmt_మొదటి == ఖాళీ)	    // first one
	    fmt_మొదటి = fmt_ptr;
	లేకపోతే
	    fmt_చివరి->తదుపరి = fmt_ptr;
	fmt_చివరి = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	ఒకవేళ (efm_గమ్యం_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    వెళ్ళు parse_efm_error;
	ఒకవేళ ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ఖాళీ)
	    వెళ్ళు parse_efm_error;
	// Advance to next part
	efm = skip_గమ్యం_option_part(efm + len);	// skip comma and spaces
    }

    ఒకవేళ (fmt_మొదటి == ఖాళీ)	// nothing found
	emsg(_(e_errorformat_కలిగియుంది_no_నమూనా));

    వెళ్ళు parse_efm_చివర;

parse_efm_error:
    free_efm_list(&fmt_మొదటి);

parse_efm_చివర:
    vim_free(fmtstr);

    తిరిగి fmt_మొదటి;
}

గణన {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_చివర_యొక్క_ఆదానం = 2,
    QF_NOMEM = 3,
    QF_IGNORE_పంక్తి = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
రకనిర్వచనం నిర్మాణం {
    char_u	*linebuf;
    పూర్ణాంకం		linelen;
    char_u	*growbuf;
    పూర్ణాంకం		growbufsiz;
    FILE	*fd;
    typval_T	*tv;
    char_u	*p_str;
    listitem_T	*p_li;
    buf_T	*buf;
    linenr_T	buflnum;
    linenr_T	lnumlast;
    vimconv_T	vc;
} qfstate_T;

/*
 * Allocate more memory for the line buffer used for parsing lines.
 */
    స్థిర char_u *
qf_grow_linebuf(qfstate_T *state, పూర్ణాంకం newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > పంక్తి_MAXLEN ? పంక్తి_MAXLEN - 1 : newsz;
    ఒకవేళ (state->growbuf == ఖాళీ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	ఒకవేళ (state->growbuf == ఖాళీ)
	    తిరిగి ఖాళీ;
	state->growbufsiz = state->linelen;
    }
    లేకపోతే ఒకవేళ (state->linelen > state->growbufsiz)
    {
	ఒకవేళ ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ఖాళీ)
	    తిరిగి ఖాళీ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    తిరిగి state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    స్థిర పూర్ణాంకం
qf_get_తదుపరి_str_పంక్తి(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    పూర్ణాంకం		len;

    ఒకవేళ (*p_str == NUL) // Reached the end of the string
	తిరిగి QF_చివర_యొక్క_ఆదానం;

    p = vim_strchr(p_str, '\n');
    ఒకవేళ (p != ఖాళీ)
	len = (పూర్ణాంకం)(p - p_str) + 1;
    లేకపోతే
	len = (పూర్ణాంకం)STRLEN(p_str);

    ఒకవేళ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ఒకవేళ (state->linebuf == ఖాళీ)
	    తిరిగి QF_NOMEM;
    }
    లేకపోతే
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    తిరిగి QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    స్థిర పూర్ణాంకం
qf_get_తదుపరి_list_పంక్తి(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    పూర్ణాంకం		len;

    అయితే (p_li != ఖాళీ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ఖాళీ))
	p_li = p_li->li_తదుపరి;	// Skip non-string items

    ఒకవేళ (p_li == ఖాళీ)		// End of the list
    {
	state->p_li = ఖాళీ;
	తిరిగి QF_చివర_యొక్క_ఆదానం;
    }

    len = (పూర్ణాంకం)STRLEN(p_li->li_tv.vval.v_string);
    ఒకవేళ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ఒకవేళ (state->linebuf == ఖాళీ)
	    తిరిగి QF_NOMEM;
    }
    లేకపోతే
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_తదుపరి;	// next item
    తిరిగి QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    స్థిర పూర్ణాంకం
qf_get_తదుపరి_buf_పంక్తి(qfstate_T *state)
{
    char_u	*p_buf = ఖాళీ;
    పూర్ణాంకం		len;

    // Get the next line from the supplied buffer
    ఒకవేళ (state->buflnum > state->lnumlast)
	తిరిగి QF_చివర_యొక్క_ఆదానం;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    ఒకవేళ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ఒకవేళ (state->linebuf == ఖాళీ)
	    తిరిగి QF_NOMEM;
    }
    లేకపోతే
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    తిరిగి QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    స్థిర పూర్ణాంకం
qf_get_తదుపరి_file_పంక్తి(qfstate_T *state)
{
    పూర్ణాంకం	    discard;
    పూర్ణాంకం	    growbuflen;

    ఒకవేళ (fgets((అక్షరం *)IObuff, IOSIZE, state->fd) == ఖాళీ)
	తిరిగి QF_చివర_యొక్క_ఆదానం;

    discard = FALSE;
    state->linelen = (పూర్ణాంకం)STRLEN(IObuff);
    ఒకవేళ (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	ఒకవేళ (state->growbuf == ఖాళీ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    ఒకవేళ (state->growbuf == ఖాళీ)
		తిరిగి QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	జ్ఞాపకం_నకలు(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	కొరకు (;;)
	{
	    char_u	*p;

	    ఒకవేళ (fgets((అక్షరం *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ఖాళీ)
		ఆపు;
	    state->linelen = (పూర్ణాంకం)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    ఒకవేళ ((state->growbuf)[growbuflen - 1] == '\n')
		ఆపు;
	    ఒకవేళ (state->growbufsiz == పంక్తి_MAXLEN)
	    {
		discard = TRUE;
		ఆపు;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < పంక్తి_MAXLEN
		? 2 * state->growbufsiz : పంక్తి_MAXLEN;
	    ఒకవేళ ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ఖాళీ)
		తిరిగి QF_NOMEM;
	    state->growbuf = p;
	}

	అయితే (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    ఒకవేళ (fgets((అక్షరం *)IObuff, IOSIZE, state->fd) == ఖాళీ
		    || (పూర్ణాంకం)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ఆపు;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    లేకపోతే
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    ఒకవేళ (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*పంక్తి;

	పంక్తి = string_convert(&state->vc, state->linebuf, &state->linelen);
	ఒకవేళ (పంక్తి != ఖాళీ)
	{
	    ఒకవేళ (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, పంక్తి);
		vim_free(పంక్తి);
	    }
	    లేకపోతే
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = పంక్తి;
		state->growbufsiz = state->linelen < పంక్తి_MAXLEN
						? state->linelen : పంక్తి_MAXLEN;
	    }
	}
    }

    తిరిగి QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    స్థిర పూర్ణాంకం
qf_get_nextline(qfstate_T *state)
{
    పూర్ణాంకం స్థితి_2 = QF_FAIL;

    ఒకవేళ (state->fd == ఖాళీ)
    {
	ఒకవేళ (state->tv != ఖాళీ)
	{
	    ఒకవేళ (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		స్థితి_2 = qf_get_తదుపరి_str_పంక్తి(state);
	    లేకపోతే ఒకవేళ (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		స్థితి_2 = qf_get_తదుపరి_list_పంక్తి(state);
	}
	లేకపోతే
	    // Get the next line from the supplied buffer
	    స్థితి_2 = qf_get_తదుపరి_buf_పంక్తి(state);
    }
    లేకపోతే
	// Get the next line from the supplied file
	స్థితి_2 = qf_get_తదుపరి_file_పంక్తి(state);

    ఒకవేళ (స్థితి_2 != QF_OK)
	తిరిగి స్థితి_2;

    // remove newline/CR from the line
    ఒకవేళ (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	ఒకవేళ (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    తిరిగి QF_OK;
}

రకనిర్వచనం నిర్మాణం {
    char_u	*namebuf;
    పూర్ణాంకం		bnr;
    char_u	*module;
    char_u	*errmsg;
    పూర్ణాంకం		errmsglen;
    దీర్ఘ	lnum;
    దీర్ఘ	చివర_lnum;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		చివర_నిలువు;
    char_u	use_viscol;
    char_u	*నమూనా;
    పూర్ణాంకం		enr;
    పూర్ణాంకం		type;
    typval_T	*user_దత్తాంశం;
    పూర్ణాంకం		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_f(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields, పూర్ణాంకం ఉపసర్గ)
{
    పూర్ణాంకం c;

    ఒకవేళ (rmp->startp[midx] == ఖాళీ || rmp->endp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    ఒకవేళ (vim_strchr((char_u *)"OPQ", ఉపసర్గ) != ఖాళీ
	    && mch_getperm(fields->namebuf) == -1)
	తిరిగి QF_FAIL;

    తిరిగి QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_b(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    పూర్ణాంకం bnr = (పూర్ణాంకం)atol((అక్షరం *)rmp->startp[midx]);
    ఒకవేళ (buflist_findnr(bnr) == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->bnr = bnr;
    తిరిగి QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_n(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->enr = (పూర్ణాంకం)atol((అక్షరం *)rmp->startp[midx]);
    తిరిగి QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_l(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->lnum = atol((అక్షరం *)rmp->startp[midx]);
    తిరిగి QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_e(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->చివర_lnum = atol((అక్షరం *)rmp->startp[midx]);
    తిరిగి QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_c(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->నిలువు = (పూర్ణాంకం)atol((అక్షరం *)rmp->startp[midx]);
    తిరిగి QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_k(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->చివర_నిలువు = (పూర్ణాంకం)atol((అక్షరం *)rmp->startp[midx]);
    తిరిగి QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_t(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->type = *rmp->startp[midx];
    తిరిగి QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    స్థిర పూర్ణాంకం
copy_nonerror_పంక్తి(char_u *linebuf, పూర్ణాంకం linelen, qffields_T *fields)
{
    char_u	*p;

    ఒకవేళ (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	ఒకవేళ ((p = vim_realloc(fields->errmsg, linelen + 1)) == ఖాళీ)
	    తిరిగి QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    తిరిగి QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_m(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    char_u	*p;
    పూర్ణాంకం		len;

    ఒకవేళ (rmp->startp[midx] == ఖాళీ || rmp->endp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    len = (పూర్ణాంకం)(rmp->endp[midx] - rmp->startp[midx]);
    ఒకవేళ (len >= fields->errmsglen)
    {
	// len + null terminator
	ఒకవేళ ((p = vim_realloc(fields->errmsg, len + 1)) == ఖాళీ)
	    తిరిగి QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    తిరిగి QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_r(regmatch_T *rmp, పూర్ణాంకం midx, char_u **తోక)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    *తోక = rmp->startp[midx];
    తిరిగి QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_p(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    char_u	*సరిపోలిక_ptr;

    ఒకవేళ (rmp->startp[midx] == ఖాళీ || rmp->endp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->నిలువు = 0;
    కొరకు (సరిపోలిక_ptr = rmp->startp[midx]; సరిపోలిక_ptr != rmp->endp[midx];
								++సరిపోలిక_ptr)
    {
	++fields->నిలువు;
	ఒకవేళ (*సరిపోలిక_ptr == TAB)
	{
	    fields->నిలువు += 7;
	    fields->నిలువు -= fields->నిలువు % 8;
	}
    }
    ++fields->నిలువు;
    fields->use_viscol = TRUE;
    తిరిగి QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_v(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    ఒకవేళ (rmp->startp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    fields->నిలువు = (పూర్ణాంకం)atol((అక్షరం *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    తిరిగి QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_s(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    పూర్ణాంకం		len;

    ఒకవేళ (rmp->startp[midx] == ఖాళీ || rmp->endp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    len = (పూర్ణాంకం)(rmp->endp[midx] - rmp->startp[midx]);
    ఒకవేళ (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->నమూనా, "^\\V");
    STRNCAT(fields->నమూనా, rmp->startp[midx], len);
    fields->నమూనా[len + 3] = '\\';
    fields->నమూనా[len + 4] = '$';
    fields->నమూనా[len + 5] = NUL;
    తిరిగి QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    స్థిర పూర్ణాంకం
qf_parse_fmt_o(regmatch_T *rmp, పూర్ణాంకం midx, qffields_T *fields)
{
    పూర్ణాంకం		len;

    ఒకవేళ (rmp->startp[midx] == ఖాళీ || rmp->endp[midx] == ఖాళీ)
	తిరిగి QF_FAIL;
    len = (పూర్ణాంకం)(rmp->endp[midx] - rmp->startp[midx]);
    ఒకవేళ (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    తిరిగి QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
స్థిర పూర్ణాంకం (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, పూర్ణాంకం, qffields_T *) =
{
    ఖాళీ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ఖాళీ, // %r
    qf_parse_fmt_p,
    qf_parse_fmt_v,
    qf_parse_fmt_s,
    qf_parse_fmt_o
};

/*
 * Parse the error format pattern matches in "regmatch" and set the values in
 * "fields".  fmt_ptr contains the 'efm' format specifiers/prefixes that have a
 * match.  Returns QF_OK if all the matches are successfully parsed. On
 * failure, returns QF_FAIL or QF_NOMEM.
 */
    స్థిర పూర్ణాంకం
qf_parse_సరిపోలిక(
	char_u		*linebuf,
	పూర్ణాంకం		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	పూర్ణాంకం		qf_multiline,
	పూర్ణాంకం		qf_multiscan,
	char_u		**తోక)
{
    పూర్ణాంకం		idx = fmt_ptr->ఉపసర్గ;
    పూర్ణాంకం		i;
    పూర్ణాంకం		midx;
    పూర్ణాంకం		స్థితి_2;

    ఒకవేళ ((idx == 'C' || idx == 'Z') && !qf_multiline)
	తిరిగి QF_FAIL;
    ఒకవేళ (vim_strchr((char_u *)"EWIN", idx) != ఖాళీ)
	fields->type = idx;
    లేకపోతే
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    కొరకు (i = 0; i < FMT_PATTERNS; i++)
    {
	స్థితి_2 = QF_OK;
	midx = (పూర్ణాంకం)fmt_ptr->addr[i];
	ఒకవేళ (i == 0 && midx > 0)				// %f
	    స్థితి_2 = qf_parse_fmt_f(regmatch, midx, fields, idx);
	లేకపోతే ఒకవేళ (i == FMT_నమూనా_M)
	{
	    ఒకవేళ (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		స్థితి_2 = copy_nonerror_పంక్తి(linebuf, linelen, fields);
	    లేకపోతే ఒకవేళ (midx > 0)				// %m
		స్థితి_2 = qf_parse_fmt_m(regmatch, midx, fields);
	}
	లేకపోతే ఒకవేళ (i == FMT_నమూనా_R && midx > 0)	// %r
	    స్థితి_2 = qf_parse_fmt_r(regmatch, midx, తోక);
	లేకపోతే ఒకవేళ (midx > 0)				// others
	    స్థితి_2 = (qf_parse_fmt[i])(regmatch, midx, fields);

	ఒకవేళ (స్థితి_2 != QF_OK)
	    తిరిగి స్థితి_2;
    }

    తిరిగి QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    స్థిర పూర్ణాంకం
qf_parse_get_fields(
	char_u		*linebuf,
	పూర్ణాంకం		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	పూర్ణాంకం		qf_multiline,
	పూర్ణాంకం		qf_multiscan,
	char_u		**తోక)
{
    regmatch_T	regmatch;
    పూర్ణాంకం		స్థితి_2 = QF_FAIL;
    పూర్ణాంకం		r;

    ఒకవేళ (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->ఉపసర్గ) == ఖాళీ)
	తిరిగి QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->నమూనా[0] = NUL;
    ఒకవేళ (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->చివర_lnum = 0;
    fields->నిలువు = 0;
    fields->చివర_నిలువు = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *తోక = ఖాళీ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    ఒకవేళ (r)
	స్థితి_2 = qf_parse_సరిపోలిక(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, తోక);

    తిరిగి స్థితి_2;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    స్థిర పూర్ణాంకం
qf_parse_dir_pfx(పూర్ణాంకం idx, qffields_T *fields, qf_list_T *qfl)
{
    ఒకవేళ (idx == 'D')				// enter directory
    {
	ఒకవేళ (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_ఖాళీలు_directory_name));
	    తిరిగి QF_FAIL;
	}
	qfl->qf_directory =
	    qf_నెట్టు_dir(fields->namebuf, &qfl->qf_dir_దొంతర, FALSE);
	ఒకవేళ (qfl->qf_directory == ఖాళీ)
	    తిరిగి QF_FAIL;
    }
    లేకపోతే ఒకవేళ (idx == 'X')			// leave directory
	qfl->qf_directory = qf_తీయి_dir(&qfl->qf_dir_దొంతర);

    తిరిగి QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    స్థిర పూర్ణాంకం
qf_parse_file_pfx(
	పూర్ణాంకం idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *తోక)
{
    fields->valid = FALSE;
    ఒకవేళ (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	ఒకవేళ (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_నెట్టు_dir(fields->namebuf, &qfl->qf_file_దొంతర, TRUE);
	లేకపోతే ఒకవేళ (idx == 'Q')
	    qfl->qf_currfile = qf_తీయి_dir(&qfl->qf_file_దొంతర);
	*fields->namebuf = NUL;
	ఒకవేళ (తోక && *తోక)
	{
	    qfl->qf_multiscan = TRUE;
	    తిరిగి QF_MULTISCAN;
	}
    }

    తిరిగి QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    స్థిర పూర్ణాంకం
qf_parse_పంక్తి_nomatch(char_u *linebuf, పూర్ణాంకం linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    తిరిగి copy_nonerror_పంక్తి(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    స్థిర పూర్ణాంకం
qf_parse_multiline_pfx(
	పూర్ణాంకం idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    పూర్ణాంకం			len;

    ఒకవేళ (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_చివరి;

	ఒకవేళ (qfprev == ఖాళీ)
	    తిరిగి QF_FAIL;
	ఒకవేళ (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (పూర్ణాంకం)STRLEN(qfprev->qf_పాఠ్యం);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    ఒకవేళ (ptr == ఖాళీ)
		తిరిగి QF_FAIL;
	    STRCPY(ptr, qfprev->qf_పాఠ్యం);
	    vim_free(qfprev->qf_పాఠ్యం);
	    qfprev->qf_పాఠ్యం = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	ఒకవేళ (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	ఒకవేళ (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	ఒకవేళ (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	ఒకవేళ (!qfprev->qf_చివర_lnum)
	    qfprev->qf_చివర_lnum = fields->చివర_lnum;
	ఒకవేళ (!qfprev->qf_నిలువు)
	{
	    qfprev->qf_నిలువు = fields->నిలువు;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	ఒకవేళ (!qfprev->qf_చివర_నిలువు)
	    qfprev->qf_చివర_నిలువు = fields->చివర_నిలువు;
	ఒకవేళ (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ఖాళీ
		    ? fields->namebuf
		    : qfl->qf_currfile != ఖాళీ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    ఒకవేళ (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    పంక్తి_breakcheck();

    తిరిగి QF_IGNORE_పంక్తి;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    స్థిర పూర్ణాంకం
qf_parse_పంక్తి(
	qf_list_T	*qfl,
	char_u		*linebuf,
	పూర్ణాంకం		linelen,
	efm_T		*fmt_మొదటి,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    పూర్ణాంకం			idx = 0;
    char_u		*తోక = ఖాళీ;
    పూర్ణాంకం			స్థితి_2;

restofline:
    // If there was no %> item start at the first pattern
    ఒకవేళ (fmt_ప్రారంభం == ఖాళీ)
	fmt_ptr = fmt_మొదటి;
    లేకపోతే
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_ప్రారంభం;
	fmt_ప్రారంభం = ఖాళీ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    కొరకు ( ; fmt_ptr != ఖాళీ; fmt_ptr = fmt_ptr->తదుపరి)
    {
	idx = fmt_ptr->ఉపసర్గ;
	స్థితి_2 = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &తోక);
	ఒకవేళ (స్థితి_2 == QF_NOMEM)
	    తిరిగి స్థితి_2;
	ఒకవేళ (స్థితి_2 == QF_OK)
	    ఆపు;
    }
    qfl->qf_multiscan = FALSE;

    ఒకవేళ (fmt_ptr == ఖాళీ || idx == 'D' || idx == 'X')
    {
	ఒకవేళ (fmt_ptr != ఖాళీ)
	{
	    // 'D' and 'X' directory specifiers
	    స్థితి_2 = qf_parse_dir_pfx(idx, fields, qfl);
	    ఒకవేళ (స్థితి_2 != QF_OK)
		తిరిగి స్థితి_2;
	}

	స్థితి_2 = qf_parse_పంక్తి_nomatch(linebuf, linelen, fields);
	ఒకవేళ (స్థితి_2 != QF_OK)
	    తిరిగి స్థితి_2;

	ఒకవేళ (fmt_ptr == ఖాళీ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    లేకపోతే ఒకవేళ (fmt_ptr != ఖాళీ)
    {
	// honor %> item
	ఒకవేళ (fmt_ptr->conthere)
	    fmt_ప్రారంభం = fmt_ptr;

	ఒకవేళ (vim_strchr((char_u *)"AEWIN", idx) != ఖాళీ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)"CZ", idx) != ఖాళీ)
	{				// continuation of multi-line msg
	    స్థితి_2 = qf_parse_multiline_pfx(idx, qfl, fields);
	    ఒకవేళ (స్థితి_2 != QF_OK)
		తిరిగి స్థితి_2;
	}
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)"OPQ", idx) != ఖాళీ)
	{				// global file names
	    స్థితి_2 = qf_parse_file_pfx(idx, fields, qfl, తోక);
	    ఒకవేళ (స్థితి_2 == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(తోక);
		పూర్ణాంకం new_linelen = (పూర్ణాంకం)STRLEN(s);
		ఒకవేళ (new_linelen >= linelen)
		    తిరిగి QF_IGNORE_పంక్తి;
		linebuf = s;
		linelen = new_linelen;
		వెళ్ళు restofline;
	    }
	}
	ఒకవేళ (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    ఒకవేళ (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    తిరిగి QF_IGNORE_పంక్తి;
	}
    }

    తిరిగి QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    స్థిర పూర్ణాంకం
qf_దొంతర_ఖాళీలు(qf_info_T *qi)
{
    తిరిగి qi == ఖాళీ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    స్థిర పూర్ణాంకం
qf_list_ఖాళీలు(qf_list_T *qfl)
{
    తిరిగి qfl == ఖాళీ || qfl->qf_లెక్క <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    స్థిర పూర్ణాంకం
qf_list_has_valid_entries(qf_list_T *qfl)
{
    తిరిగి !qf_list_ఖాళీలు(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    స్థిర qf_list_T *
qf_get_list(qf_info_T *qi, పూర్ణాంకం idx)
{
    తిరిగి &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    స్థిర పూర్ణాంకం
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->నమూనా = alloc_id(CMDBUFFSIZE + 1, aid_qf_నమూనా);
    ఒకవేళ (pfields->namebuf == ఖాళీ || pfields->errmsg == ఖాళీ
		|| pfields->నమూనా == ఖాళీ || pfields->module == ఖాళీ)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    స్థిర శూన్య
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->నమూనా);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    స్థిర పూర్ణాంకం
qf_అమరిక_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    ఒకవేళ (enc != ఖాళీ && *enc != NUL)
	convert_అమరిక(&pstate->vc, enc, p_enc);

    ఒకవేళ (efile != ఖాళీ && (pstate->fd = mch_fopen((అక్షరం *)efile, "r")) == ఖాళీ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	తిరిగి FAIL;
    }

    ఒకవేళ (tv != ఖాళీ)
    {
	ఒకవేళ (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_మొదటి;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    తిరిగి OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    స్థిర శూన్య
qf_cleanup_state(qfstate_T *pstate)
{
    ఒకవేళ (pstate->fd != ఖాళీ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    ఒకవేళ (pstate->vc.vc_type != CONV_NONE)
	convert_అమరిక(&pstate->vc, ఖాళీ, ఖాళీ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    స్థిర పూర్ణాంకం
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_మొదటి,
	qfstate_T	*state,
	qffields_T	*fields)
{
    పూర్ణాంకం		    స్థితి_2;

    // Get the next line from a file/buffer/list/string
    స్థితి_2 = qf_get_nextline(state);
    ఒకవేళ (స్థితి_2 != QF_OK)
	తిరిగి స్థితి_2;

    స్థితి_2 = qf_parse_పంక్తి(qfl, state->linebuf, state->linelen,
	    fmt_మొదటి, fields);
    ఒకవేళ (స్థితి_2 != QF_OK)
	తిరిగి స్థితి_2;

    తిరిగి qf_కూడు_నమోదు(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ఖాళీ)
		? fields->namebuf
		: ((qfl->qf_currfile != ఖాళీ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ఖాళీ),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->చివర_lnum,
		fields->నిలువు,
		fields->చివర_నిలువు,
		fields->use_viscol,
		fields->నమూనా,
		fields->enr,
		fields->type,
		fields->user_దత్తాంశం,
		fields->valid);
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list.
 * Alternative: when "efile" is NULL read errors from buffer "buf".
 * Alternative: when "tv" is not NULL get errors from the string or list.
 * Always use 'errorformat' from "buf" if there is a local value.
 * Then "lnumfirst" and "lnumlast" specify the range of lines to use.
 * Set the title of the list to "qf_title".
 * Return -1 for error, number of errors for success.
 */
    స్థిర పూర్ణాంకం
qf_init_ext(
    qf_info_T	    *qi,
    పూర్ణాంకం		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    పూర్ణాంకం		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_చివరి = ఖాళీ;
    పూర్ణాంకం		    adding = FALSE;
    స్థిర efm_T    *fmt_మొదటి = ఖాళీ;
    char_u	    *efm;
    స్థిర char_u   *చివరి_efm = ఖాళీ;
    పూర్ణాంకం		    retval = -1;	// default: return error flag
    పూర్ణాంకం		    స్థితి_2;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_చివరి_bufname);

    CLEAR_క్షేత్రం(state);
    CLEAR_క్షేత్రం(fields);
    ఒకవేళ ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_అమరిక_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	వెళ్ళు qf_init_చివర;

    ఒకవేళ (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    లేకపోతే
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	ఒకవేళ (!qf_list_ఖాళీలు(qfl))
	    old_చివరి = qfl->qf_చివరి;
    }

    // Use the local value of 'errorformat' if it's set.
    ఒకవేళ (errorformat == p_efm && tv == ఖాళీ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    లేకపోతే
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    ఒకవేళ (చివరి_efm == ఖాళీ || (STRCMP(చివరి_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(చివరి_efm);
	free_efm_list(&fmt_మొదటి);

	// parse the current 'efm'
	fmt_మొదటి = parse_efm_option(efm);
	ఒకవేళ (fmt_మొదటి != ఖాళీ)
	    చివరి_efm = vim_strsave(efm);
    }

    ఒకవేళ (fmt_మొదటి == ఖాళీ)	// nothing found
	వెళ్ళు error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    అయితే (!got_int)
    {
	స్థితి_2 = qf_init_process_nextline(qfl, fmt_మొదటి, &state, &fields);
	ఒకవేళ (స్థితి_2 == QF_NOMEM)		// memory alloc failure
	    వెళ్ళు qf_init_చివర;
	ఒకవేళ (స్థితి_2 == QF_చివర_యొక్క_ఆదానం)	// end of input
	    ఆపు;
	ఒకవేళ (స్థితి_2 == QF_FAIL)
	    వెళ్ళు error2;

	పంక్తి_breakcheck();
    }
    ఒకవేళ (state.fd == ఖాళీ || !ferror(state.fd))
    {
	ఒకవేళ (qfl->qf_సూచీ == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_ప్రారంభం;
	    qfl->qf_సూచీ = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	లేకపోతే
	{
	    qfl->qf_nonevalid = FALSE;
	    ఒకవేళ (qfl->qf_ptr == ఖాళీ)
		qfl->qf_ptr = qfl->qf_ప్రారంభం;
	}
	// return number of matches
	retval = qfl->qf_లెక్క;
	వెళ్ళు qf_init_చివర;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    ఒకవేళ (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	ఒకవేళ (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_చివర:
    ఒకవేళ (qf_idx == qi->qf_curlist)
	qf_update_బఫరు(qi, old_చివరి);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    తిరిగి retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    పూర్ణాంకం
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	పూర్ణాంకం	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    ఒకవేళ (wp != ఖాళీ)
	qi = ll_get_or_alloc_list(wp);
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    తిరిగి qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ఖాళీ, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    స్థిర శూన్య
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    ఒకవేళ (title == ఖాళీ)
	తిరిగి;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    ఒకవేళ (p != ఖాళీ)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    స్థిర char_u *
qf_cmdtitle(char_u *cmd)
{
    స్థిర char_u qftitle_str[IOSIZE];

    vim_snprintf((అక్షరం *)qftitle_str, IOSIZE, ":%s", (అక్షరం *)cmd);
    తిరిగి qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    స్థిర qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    తిరిగి qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    స్థిర శూన్య
qf_తీయి_దొంతర(qf_info_T *qi, పూర్ణాంకం adjust)
{
    పూర్ణాంకం i;
    qf_free(&qi->qf_lists[0]);
    కొరకు (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, పరిమాణం(*qi->qf_lists));

    ఒకవేళ (adjust)
    {
	qi->qf_listcount--;
	ఒకవేళ (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	లేకపోతే
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    స్థిర శూన్య
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    అయితే (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    ఒకవేళ (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_తీయి_దొంతర(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    లేకపోతే
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_సూచిక(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++చివరి_qf_id;
    qfl->qf_has_user_దత్తాంశం = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    స్థిర శూన్య
locstack_వరుస_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    ఒకవేళ (q == ఖాళీ)
	తిరిగి;

    q->qi = qi;
    q->తదుపరి = qf_delq_తల;
    qf_delq_తల = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    పూర్ణాంకం
qf_దొంతర_get_bufnr(శూన్య)
{
    ఒకవేళ (ql_info == ఖాళీ)
	తిరిగి INVALID_QFBUFNR;
    తిరిగి ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    స్థిర శూన్య
wipe_qf_బఫరు(qf_info_T *qi)
{
    buf_T	*qfbuf;

    ఒకవేళ (qi->qf_bufnr == INVALID_QFBUFNR)
	తిరిగి;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    ఒకవేళ (qfbuf != ఖాళీ && qfbuf->b_nwindows == 0)
    {
	పూర్ణాంకం buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	ఒకవేళ (curwin->w_బఫరు == ఖాళీ)
	{
	    curwin->w_బఫరు = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_బఫరు(ఖాళీ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	ఒకవేళ (buf_was_null)
	    curwin->w_బఫరు = ఖాళీ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    స్థిర శూన్య
qf_free_list_దొంతర_వస్తువులు(qf_info_T *qi)
{
    కొరకు (పూర్ణాంకం i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    స్థిర శూన్య
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_దొంతర_వస్తువులు(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    స్థిర శూన్య
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి;
    *pqi = ఖాళీ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    ఒకవేళ (quickfix_busy > 0)
    {
	locstack_వరుస_delreq(qi);
	తిరిగి;
    }

    qi->qf_refcount--;
    ఒకవేళ (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_బఫరు(qi);

	qf_free_lists(qi);
    }
}

/*
 * Free all the quickfix/location lists in the stack.
 */
    శూన్య
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ఒకవేళ (wp != ఖాళీ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    లేకపోతే ఒకవేళ (qi != ఖాళీ)
	qf_free_list_దొంతర_వస్తువులు(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    స్థిర శూన్య
incr_quickfix_busy(శూన్య)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    స్థిర శూన్య
decr_quickfix_busy(శూన్య)
{
    ఒకవేళ (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	అయితే (qf_delq_తల != ఖాళీ)
	{
	    qf_delq_T	*q = qf_delq_తల;

	    qf_delq_తల = q->తదుపరి;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_అంతర్గత_ERROR
    ఒకవేళ (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# ఒకవేళ defined(EXITFREE)
    శూన్య
check_quickfix_busy(శూన్య)
{
    ఒకవేళ (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (దీర్ఘ)quickfix_busy);
#  ifdef ABORT_ON_అంతర్గత_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    స్థిర పూర్ణాంకం
qf_కూడు_నమోదు(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    పూర్ణాంకం		bufnum,		// buffer number or zero
    char_u	*mesg,		//సందేశం
    దీర్ఘ	lnum,		// line number
    దీర్ఘ	చివర_lnum,	// line number for end
    పూర్ణాంకం		నిలువు,		//స్తంభం
    పూర్ణాంకం		చివర_నిలువు,	// column for end
    పూర్ణాంకం		vis_నిలువు,	// using visual column
    char_u	*నమూనా,	// search pattern
    పూర్ణాంకం		nr,		// error number
    పూర్ణాంకం		type,		// type character
    typval_T	*user_దత్తాంశం,     // custom user data or NULL
    పూర్ణాంకం		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ఖాళీ;
    char_u	*p = ఖాళీ;

    ఒకవేళ ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ఖాళీ)
	తిరిగి QF_FAIL;
    ఒకవేళ (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	ఒకవేళ (buf != ఖాళీ)
	    buf->b_has_qf_నమోదు |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_నమోదు : BUF_HAS_LL_నమోదు;
    }
    లేకపోతే
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    ఒకవేళ (fname != ఖాళీ)
	fullname = fix_fname(fname);
    qfp->qf_fname = ఖాళీ;
    ఒకవేళ (buf != ఖాళీ &&
	buf->b_ffname != ఖాళీ && fullname != ఖాళీ)
    {
	ఒకవేళ (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    ఒకవేళ (p != ఖాళీ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    ఒకవేళ ((qfp->qf_పాఠ్యం = vim_strsave(mesg)) == ఖాళీ)
    {
	vim_free(qfp);
	తిరిగి QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_చివర_lnum = చివర_lnum;
    qfp->qf_నిలువు = నిలువు;
    qfp->qf_చివర_నిలువు = చివర_నిలువు;
    qfp->qf_viscol = vis_నిలువు;
    ఒకవేళ (user_దత్తాంశం == ఖాళీ || user_దత్తాంశం->v_type == VAR_UNKNOWN)
	qfp->qf_user_దత్తాంశం.v_type = VAR_UNKNOWN;
    లేకపోతే
    {
	copy_tv(user_దత్తాంశం, &qfp->qf_user_దత్తాంశం);
	qfl->qf_has_user_దత్తాంశం = TRUE;
    }
    ఒకవేళ (నమూనా == ఖాళీ || *నమూనా == NUL)
	qfp->qf_నమూనా = ఖాళీ;
    లేకపోతే ఒకవేళ ((qfp->qf_నమూనా = vim_strsave(నమూనా)) == ఖాళీ)
    {
	vim_free(qfp->qf_పాఠ్యం);
	vim_free(qfp);
	తిరిగి QF_FAIL;
    }
    ఒకవేళ (module == ఖాళీ || *module == NUL)
	qfp->qf_module = ఖాళీ;
    లేకపోతే ఒకవేళ ((qfp->qf_module = vim_strsave(module)) == ఖాళీ)
    {
	vim_free(qfp->qf_పాఠ్యం);
	vim_free(qfp->qf_నమూనా);
	vim_free(qfp);
	తిరిగి QF_FAIL;
    }
    qfp->qf_nr = nr;
    ఒకవేళ (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_చివరి;
    ఒకవేళ (qf_list_ఖాళీలు(qfl))		// first element in the list
    {
	qfl->qf_ప్రారంభం = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_సూచీ = 0;
	qfp->qf_prev = ఖాళీ;
    }
    లేకపోతే
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_తదుపరి = qfp;
    }
    qfp->qf_తదుపరి = ఖాళీ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_లెక్క;
    ఒకవేళ (qfl->qf_సూచీ == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_సూచీ = qfl->qf_లెక్క;
	qfl->qf_ptr = qfp;
    }

    తిరిగి QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    పూర్ణాంకం
qf_resize_దొంతర(పూర్ణాంకం n)
{
    ఒకవేళ (ql_info == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (qf_resize_దొంతర_పునాది(ql_info, n) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    పూర్ణాంకం
ll_resize_దొంతర(win_T *wp, పూర్ణాంకం n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    ఒకవేళ (IS_LL_కిటికీ(wp))
	qf_sync_llw_గమ్యం_win(wp);
    లేకపోతే
	qf_sync_win_గమ్యం_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (qf_resize_దొంతర_పునాది(qi, n) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    స్థిర పూర్ణాంకం
qf_resize_దొంతర_పునాది(qf_info_T *qi, పూర్ణాంకం n)
{
    qf_list_T *new;
    పూర్ణాంకం amount_గమ్యం_rm = 0, i;

    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    size_t lsz = పరిమాణం(*qi->qf_lists);

    ఒకవేళ (n == qi->qf_maxcount)
	తిరిగి OK;
    లేకపోతే ఒకవేళ (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_గమ్యం_rm = qi->qf_listcount - n;

	కొరకు (i = 0; i < amount_గమ్యం_rm; i++)
	    qf_తీయి_దొంతర(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    ఒకవేళ (new == ఖాళీ)
	తిరిగి FAIL;

    // fill with zeroes any newly allocated memory
    ఒకవేళ (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_బఫరు(qi, ఖాళీ);

    తిరిగి OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   శూన్య
qf_init_దొంతర(శూన్య)
{
    ql_info = qf_alloc_దొంతర(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    స్థిర శూన్య
qf_sync_llw_గమ్యం_win(win_T *llw)
{
    win_T *wp = qf_కనుగొను_win_with_loclist(llw->w_llist_ref);

    ఒకవేళ (wp != ఖాళీ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    స్థిర శూన్య
qf_sync_win_గమ్యం_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    ఒకవేళ (llw != ఖాళీ)
	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_llist_ref == llw && bt_quickfix(wp->w_బఫరు))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		తిరిగి;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    స్థిర qf_info_T *
qf_alloc_దొంతర(qfltype_T qfltype, పూర్ణాంకం n)
{
    qf_info_T *qi;

    ఒకవేళ (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    లేకపోతే
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	ఒకవేళ (qi == ఖాళీ)
	    తిరిగి ఖాళీ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_దొంతర(n);
    ఒకవేళ (qi->qf_lists == ఖాళీ)
    {
	ఒకవేళ (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	తిరిగి ఖాళీ;
    }
    qi->qf_maxcount = n;

    తిరిగి qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    స్థిర qf_list_T *
qf_alloc_list_దొంతర(పూర్ణాంకం n)
{
    తిరిగి ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    స్థిర qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    ఒకవేళ (IS_LL_కిటికీ(wp))
	// For a location list window, use the referenced location list
	తిరిగి wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    ఒకవేళ (wp->w_llist == ఖాళీ)
	// new location list
	wp->w_llist = qf_alloc_దొంతర(QFLT_LOCATION, wp->w_p_lhi);

    తిరిగి wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    స్థిర qf_info_T *
qf_cmd_get_దొంతర(exarg_T *eap, పూర్ణాంకం print_emsg)
{
    qf_info_T	*qi = ql_info;

    ఒకవేళ (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	ఒకవేళ (qi == ఖాళీ)
	{
	    ఒకవేళ (print_emsg)
		emsg(_(e_no_location_list));
	    తిరిగి ఖాళీ;
	}
    }
    ఒకవేళ (qi == ఖాళీ && print_emsg)
	emsg(_(e_no_quickfix_దొంతర));

    తిరిగి qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    స్థిర qf_info_T *
qf_cmd_get_or_alloc_దొంతర(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    ఒకవేళ (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	ఒకవేళ (qi == ఖాళీ)
	    తిరిగి ఖాళీ;
	*pwinp = curwin;
    }

    తిరిగి qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    స్థిర పూర్ణాంకం
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *గమ్యం_qfl)
{
    పూర్ణాంకం		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_వస్తువులు(from_qfl, from_qfp, i)
    {
	ఒకవేళ (qf_కూడు_నమోదు(గమ్యం_qfl,
		    ఖాళీ,
		    ఖాళీ,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_పాఠ్యం,
		    from_qfp->qf_lnum,
		    from_qfp->qf_చివర_lnum,
		    from_qfp->qf_నిలువు,
		    from_qfp->qf_చివర_నిలువు,
		    from_qfp->qf_viscol,
		    from_qfp->qf_నమూనా,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_దత్తాంశం,
		    from_qfp->qf_valid) == QF_FAIL)
	    తిరిగి FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = గమ్యం_qfl->qf_చివరి;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	ఒకవేళ (from_qfl->qf_ptr == from_qfp)
	    గమ్యం_qfl->qf_ptr = prevp;		// current location
    }

    తిరిగి OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    స్థిర పూర్ణాంకం
copy_loclist(qf_list_T *from_qfl, qf_list_T *గమ్యం_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    గమ్యం_qfl->qfl_type = from_qfl->qfl_type;
    గమ్యం_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    గమ్యం_qfl->qf_has_user_దత్తాంశం = from_qfl->qf_has_user_దత్తాంశం;
    గమ్యం_qfl->qf_లెక్క = 0;
    గమ్యం_qfl->qf_సూచీ = 0;
    గమ్యం_qfl->qf_ప్రారంభం = ఖాళీ;
    గమ్యం_qfl->qf_చివరి = ఖాళీ;
    గమ్యం_qfl->qf_ptr = ఖాళీ;
    ఒకవేళ (from_qfl->qf_title != ఖాళీ)
	గమ్యం_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    లేకపోతే
	గమ్యం_qfl->qf_title = ఖాళీ;
    ఒకవేళ (from_qfl->qf_ctx != ఖాళీ)
    {
	గమ్యం_qfl->qf_ctx = alloc_tv();
	ఒకవేళ (గమ్యం_qfl->qf_ctx != ఖాళీ)
	    copy_tv(from_qfl->qf_ctx, గమ్యం_qfl->qf_ctx);
    }
    లేకపోతే
	గమ్యం_qfl->qf_ctx = ఖాళీ;
    ఒకవేళ (from_qfl->qf_qftf_cb.cb_name != ఖాళీ)
	copy_callback(&గమ్యం_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    లేకపోతే
	గమ్యం_qfl->qf_qftf_cb.cb_name = ఖాళీ;

    ఒకవేళ (from_qfl->qf_లెక్క)
	ఒకవేళ (copy_loclist_entries(from_qfl, గమ్యం_qfl) == FAIL)
	    తిరిగి FAIL;

    గమ్యం_qfl->qf_సూచీ = from_qfl->qf_సూచీ;	// current index in the list

    // Assign a new ID for the location list
    గమ్యం_qfl->qf_id = ++చివరి_qf_id;
    గమ్యం_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    ఒకవేళ (గమ్యం_qfl->qf_nonevalid)
    {
	గమ్యం_qfl->qf_ptr = గమ్యం_qfl->qf_ప్రారంభం;
	గమ్యం_qfl->qf_సూచీ = 1;
    }

    తిరిగి OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    శూన్య
copy_loclist_దొంతర(win_T *from, win_T *గమ్యం)
{
    qf_info_T	*qi;
    పూర్ణాంకం		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    ఒకవేళ (IS_LL_కిటికీ(from))
	qi = from->w_llist_ref;
    లేకపోతే
	qi = from->w_llist;

    ఒకవేళ (qi == ఖాళీ)		    // no location list to copy
	తిరిగి;

    // allocate a new location list, set size of stack to 'from' window value
    ఒకవేళ ((గమ్యం->w_llist = qf_alloc_దొంతర(QFLT_LOCATION, from->w_p_lhi)) == ఖాళీ)
	తిరిగి;
    లేకపోతే
	// set 'to' lhi to reflect new value
	గమ్యం->w_p_lhi = గమ్యం->w_llist->qf_maxcount;

    గమ్యం->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    కొరకు (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	గమ్యం->w_llist->qf_curlist = idx;

	ఒకవేళ (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(గమ్యం->w_llist, idx)) == FAIL)
	{
	    qf_free_all(గమ్యం);
	    తిరిగి;
	}
    }

    గమ్యం->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    స్థిర పూర్ణాంకం
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ఖాళీ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    ఒకవేళ (fname == ఖాళీ || *fname == NUL)		// no file name
	తిరిగి 0;

# ifdef VMS
    vms_remove_సంస్కరణ(fname);
# endif
# ifdef BACKSLASH_లోపల_FILENAME
    ఒకవేళ (directory != ఖాళీ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    ఒకవేళ (directory != ఖాళీ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ఖాళీ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	ఒకవేళ (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    ఒకవేళ (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    లేకపోతే
	    {
		ptr.నిడివి = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    ఒకవేళ (ptr.string == ఖాళీ)
		తిరిగి 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.నిడివి = ptr.నిడివి;
    }
    లేకపోతే
    {
	bufname.string = fname;
	bufname.నిడివి = fname_len;
    }

    ఒకవేళ (qf_చివరి_bufname != ఖాళీ && STRCMP(bufname.string, qf_చివరి_bufname) == 0
	    && bufref_valid(&qf_చివరి_bufref))
    {
	buf = qf_చివరి_bufref.br_buf;
	vim_free(ptr.string);
    }
    లేకపోతే
    {
	vim_free(qf_చివరి_bufname);
	buf = buflist_new(bufname.string, ఖాళీ, (linenr_T)0, BLN_NOOPT);
	ఒకవేళ (bufname.string == ptr.string)
	    qf_చివరి_bufname = bufname.string;
	లేకపోతే
	    qf_చివరి_bufname = vim_strnsave(bufname.string, bufname.నిడివి);
	సమితి_bufref(&qf_చివరి_bufref, buf);
    }
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి 0;

    buf->b_has_qf_నమోదు =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_నమోదు : BUF_HAS_LL_నమోదు;
    తిరిగి buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    స్థిర char_u *
qf_నెట్టు_dir(char_u *dirbuf, నిర్మాణం dir_దొంతర_T **stackptr, పూర్ణాంకం is_file_దొంతర)
{
    నిర్మాణం dir_దొంతర_T  *ds_new;
    నిర్మాణం dir_దొంతర_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(నిర్మాణం dir_దొంతర_T, aid_qf_dirstack);
    ఒకవేళ (ds_new == ఖాళీ)
	తిరిగి ఖాళీ;

    // push the new element onto the stack
    ds_new->తదుపరి = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    ఒకవేళ (vim_isAbsName(dirbuf)
	    || (*stackptr)->తదుపరి == ఖాళీ
	    || is_file_దొంతర)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    లేకపోతే
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->తదుపరి;
	(*stackptr)->dirname = ఖాళీ;
	అయితే (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    ఒకవేళ (dirname.string == ఖాళీ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->తదుపరి;
		vim_free(ds_ptr);

		తిరిగి ఖాళీ;
	    }
	    ఒకవేళ (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ఆపు;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->తదుపరి;
	}

	// clean up all dirs we already left
	అయితే ((*stackptr)->తదుపరి != ds_new)
	{
	    ds_ptr = (*stackptr)->తదుపరి;
	    (*stackptr)->తదుపరి = (*stackptr)->తదుపరి->తదుపరి;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	ఒకవేళ (ds_new == ఖాళీ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    ఒకవేళ ((*stackptr)->dirname != ఖాళీ)
	తిరిగి (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->తదుపరి;
    vim_free(ds_ptr);

    తిరిగి ఖాళీ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    స్థిర char_u *
qf_తీయి_dir(నిర్మాణం dir_దొంతర_T **stackptr)
{
    నిర్మాణం dir_దొంతర_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    ఒకవేళ (*stackptr != ఖాళీ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->తదుపరి;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    తిరిగి *stackptr ? (*stackptr)->dirname : ఖాళీ;
}

/*
 * clean up directory stack
 */
    స్థిర శూన్య
qf_clean_dir_దొంతర(నిర్మాణం dir_దొంతర_T **stackptr)
{
    నిర్మాణం dir_దొంతర_T  *ds_ptr;

    అయితే ((ds_ptr = *stackptr) != ఖాళీ)
    {
	*stackptr = (*stackptr)->తదుపరి;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }
}

/*
 * Check in which directory of the directory stack the given file can be
 * found.
 * Returns a pointer to the directory name or NULL if not found.
 * Cleans up intermediate directory entries.
 *
 * TODO: How to solve the following problem?
 * If we have this directory tree:
 *     ./
 *     ./aa
 *     ./aa/bb
 *     ./bb
 *     ./bb/x.c
 * and make says:
 *     making all in aa
 *     making all in bb
 *     x.c:9: Error
 * Then qf_push_dir thinks we are in ./aa/bb, but we are in ./bb.
 * qf_guess_filepath will return NULL.
 */
    స్థిర char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    నిర్మాణం dir_దొంతర_T     *ds_ptr;
    నిర్మాణం dir_దొంతర_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    ఒకవేళ (qfl->qf_dir_దొంతర == ఖాళీ)
	తిరిగి ఖాళీ;

    ds_ptr = qfl->qf_dir_దొంతర->తదుపరి;
    fullname.string = ఖాళీ;
    fullname.నిడివి = 0;
    filename_len = STRLEN(filename);
    అయితే (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	ఒకవేళ ((fullname.string != ఖాళీ) && (mch_getperm(fullname.string) >= 0))
	    ఆపు;

	ds_ptr = ds_ptr->తదుపరి;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    అయితే (qfl->qf_dir_దొంతర->తదుపరి != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_దొంతర->తదుపరి;
	qfl->qf_dir_దొంతర->తదుపరి = qfl->qf_dir_దొంతర->తదుపరి->తదుపరి;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    తిరిగి ds_ptr == ఖాళీ ? ఖాళీ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    స్థిర పూర్ణాంకం
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    పూర్ణాంకం		i;

    ఒకవేళ (wp != ఖాళీ)
    {
	ఒకవేళ (!win_valid(wp))
	    తిరిగి FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (i = 0; i < qi->qf_listcount; ++i)
	ఒకవేళ (qi->qf_lists[i].qf_id == qf_id)
	    తిరిగి TRUE;

    తిరిగి FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    స్థిర పూర్ణాంకం
is_qf_నమోదు_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    పూర్ణాంకం		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
	ఒకవేళ (qfp == qf_ptr)
	    ఆపు;

    ఒకవేళ (i > qfl->qf_లెక్క) // Entry is not found
	తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    స్థిర qfline_T *
get_తదుపరి_valid_నమోదు(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	పూర్ణాంకం		*qf_సూచీ,
	పూర్ణాంకం		dir)
{
    పూర్ణాంకం			idx;
    పూర్ణాంకం			old_qf_fnum;

    idx = *qf_సూచీ;
    old_qf_fnum = qf_ptr->qf_fnum;

    చేయి
    {
	ఒకవేళ (idx == qfl->qf_లెక్క || qf_ptr->qf_తదుపరి == ఖాళీ)
	    తిరిగి ఖాళీ;
	++idx;
	qf_ptr = qf_ptr->qf_తదుపరి;
    } అయితే ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_సూచీ = idx;
    తిరిగి qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    స్థిర qfline_T *
get_prev_valid_నమోదు(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	పూర్ణాంకం		*qf_సూచీ,
	పూర్ణాంకం		dir)
{
    పూర్ణాంకం			idx;
    పూర్ణాంకం			old_qf_fnum;

    idx = *qf_సూచీ;
    old_qf_fnum = qf_ptr->qf_fnum;

    చేయి
    {
	ఒకవేళ (idx == 1 || qf_ptr->qf_prev == ఖాళీ)
	    తిరిగి ఖాళీ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } అయితే ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_సూచీ = idx;
    తిరిగి qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    స్థిర qfline_T *
get_nth_valid_నమోదు(
	qf_list_T	*qfl,
	పూర్ణాంకం		errornr,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    పూర్ణాంకం			qf_idx = qfl->qf_సూచీ;
    qfline_T		*prev_qf_ptr;
    పూర్ణాంకం			prev_సూచీ;
    అక్షరం		*err = e_no_మరిన్ని_వస్తువులు;

    అయితే (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_సూచీ = qf_idx;

	ఒకవేళ (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_తదుపరి_valid_నమోదు(qfl, qf_ptr, &qf_idx, dir);
	లేకపోతే
	    qf_ptr = get_prev_valid_నమోదు(qfl, qf_ptr, &qf_idx, dir);
	ఒకవేళ (qf_ptr == ఖాళీ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_సూచీ;
	    ఒకవేళ (err != ఖాళీ)
	    {
		emsg(_(err));
		తిరిగి ఖాళీ;
	    }
	    ఆపు;
	}

	err = ఖాళీ;
    }

    *new_qfidx = qf_idx;
    తిరిగి qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    స్థిర qfline_T *
get_nth_నమోదు(qf_list_T *qfl, పూర్ణాంకం errornr, పూర్ణాంకం *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    పూర్ణాంకం		qf_idx = qfl->qf_సూచీ;

    // New error number is less than the current error number
    అయితే (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ఖాళీ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    అయితే (errornr > qf_idx && qf_idx < qfl->qf_లెక్క &&
						qf_ptr->qf_తదుపరి != ఖాళీ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_తదుపరి;
    }

    *new_qfidx = qf_idx;
    తిరిగి qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    స్థిర qfline_T *
qf_get_నమోదు(
	qf_list_T	*qfl,
	పూర్ణాంకం		errornr,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    పూర్ణాంకం		qfidx = qfl->qf_సూచీ;

    ఒకవేళ (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_నమోదు(qfl, errornr, dir, &qfidx);
    లేకపోతే ఒకవేళ (errornr != 0)	// go to specified number
	qf_ptr = get_nth_నమోదు(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    తిరిగి qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    స్థిర win_T *
qf_కనుగొను_help_win(శూన్య)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (bt_help(wp->w_బఫరు))
	    తిరిగి wp;

    తిరిగి ఖాళీ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    స్థిర శూన్య
win_సమితి_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    స్థిర పూర్ణాంకం
jump_గమ్యం_help_కిటికీ(qf_info_T *qi, పూర్ణాంకం newwin, పూర్ణాంకం *opened_కిటికీ)
{
    win_T	*wp;
    పూర్ణాంకం		flags;

    ఒకవేళ (cmdmod.cmod_tab != 0 || newwin)
	wp = ఖాళీ;
    లేకపోతే
	wp = qf_కనుగొను_help_win();
    ఒకవేళ (wp != ఖాళీ && wp->w_బఫరు->b_nwindows > 0)
	win_enter(wp, TRUE);
    లేకపోతే
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	ఒకవేళ (cmdmod.cmod_విభజించు == 0 && curwin->w_వెడల్పు != Columns
		&& curwin->w_వెడల్పు < 80)
	    flags |= WSP_శిఖరం;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	ఒకవేళ (IS_LL_దొంతర(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	ఒకవేళ (win_విభజించు(0, flags) == FAIL)
	    తిరిగి FAIL;

	*opened_కిటికీ = TRUE;

	ఒకవేళ (curwin->w_ఎత్తు < p_hh)
	    win_setheight((పూర్ణాంకం)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	ఒకవేళ (IS_LL_దొంతర(qi) && !newwin)
	    win_సమితి_loclist(curwin, qi);
    }

    ఒకవేళ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    తిరిగి OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    స్థిర win_T *
qf_కనుగొను_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_llist == ll && !bt_quickfix(wp->w_బఫరు))
	    తిరిగి wp;

    తిరిగి ఖాళీ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    స్థిర win_T *
qf_కనుగొను_win_with_సాధారణ_buf(శూన్య)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (bt_సాధారణ(wp->w_బఫరు))
	    తిరిగి wp;

    తిరిగి ఖాళీ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    స్థిర పూర్ణాంకం
qf_goto_tabwin_with_file(పూర్ణాంకం fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_బఫరు->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    తిరిగి TRUE;
	}

    తిరిగి FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    స్థిర పూర్ణాంకం
qf_open_new_file_win(qf_info_T *ll_ref)
{
    పూర్ణాంకం		flags;

    flags = WSP_ABOVE;
    ఒకవేళ (ll_ref != ఖాళీ)
	flags |= WSP_NEWLOC;
    ఒకవేళ (win_విభజించు(0, flags) == FAIL)
	తిరిగి FAIL;		// not enough room for window
    p_swb = ఖాళీలు_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    ఒకవేళ (ll_ref != ఖాళీ)
	// The new window should use the location list from the
	// location list window
	win_సమితి_loclist(curwin, ll_ref);
    తిరిగి OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    స్థిర శూన్య
qf_goto_win_with_ll_file(win_T *use_win, పూర్ణాంకం qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    ఒకవేళ (win == ఖాళీ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    ఒకవేళ (win->w_బఫరు->b_fnum == qf_fnum)
		ఆపు;
	ఒకవేళ (win == ఖాళీ)
	{
	    // Find a previous usable window
	    win = curwin;
	    చేయి
	    {
		ఒకవేళ (bt_సాధారణ(win->w_బఫరు))
		    ఆపు;
		ఒకవేళ (win->w_prev == ఖాళీ)
		    win = lastwin;	// wrap around the top
		లేకపోతే
		    win = win->w_prev; // go to previous window
	    } అయితే (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    ఒకవేళ (win->w_llist == ఖాళీ && ll_ref != ఖాళీ)
	win_సమితి_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    స్థిర శూన్య
qf_goto_win_with_qfl_file(పూర్ణాంకం qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ఖాళీ;
    కొరకు (;;)
    {
	ఒకవేళ (win->w_బఫరు->b_fnum == qf_fnum)
	    ఆపు;
	ఒకవేళ (win->w_prev == ఖాళీ)
	    win = lastwin;	// wrap around the top
	లేకపోతే
	    win = win->w_prev;	// go to previous window

	ఒకవేళ (IS_QF_కిటికీ(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    ఒకవేళ ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    లేకపోతే ఒకవేళ (altwin != ఖాళీ)
		win = altwin;
	    లేకపోతే ఒకవేళ (curwin->w_prev != ఖాళీ)
		win = curwin->w_prev;
	    లేకపోతే
		win = curwin->w_తదుపరి;
	    ఆపు;
	}

	// Remember a usable window.
	ఒకవేళ (altwin == ఖాళీ && !win->w_p_pvw && !win->w_p_wfb &&
		bt_సాధారణ(win->w_బఫరు))
	    altwin = win;
    }

    win_goto(win);
}

/*
 * Find a suitable window for opening a file (qf_fnum) from the
 * quickfix/location list and jump to it.  If the file is already opened in a
 * window, jump to it. Otherwise open a new window to display the file. If
 * 'newwin' is TRUE, then always open a new window. This is called from either
 * a quickfix or a location list window.
 */
    స్థిర పూర్ణాంకం
qf_jump_గమ్యం_usable_కిటికీ(పూర్ణాంకం qf_fnum, పూర్ణాంకం newwin, పూర్ణాంకం *opened_కిటికీ)
{
    win_T	*usable_wp = ఖాళీ;
    పూర్ణాంకం		usable_win = FALSE;
    qf_info_T	*ll_ref = ఖాళీ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    ఒకవేళ (!newwin)
	ll_ref = curwin->w_llist_ref;

    ఒకవేళ (ll_ref != ఖాళీ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_కనుగొను_win_with_loclist(ll_ref);
	ఒకవేళ (usable_wp != ఖాళీ)
	    usable_win = TRUE;
    }

    ఒకవేళ (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_కనుగొను_win_with_సాధారణ_buf();
	ఒకవేళ (win != ఖాళీ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    ఒకవేళ (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    ఒకవేళ ((ONE_కిటికీ && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	ఒకవేళ (qf_open_new_file_win(ll_ref) != OK)
	    తిరిగి FAIL;
	*opened_కిటికీ = TRUE;	// close it when fail
    }
    లేకపోతే
    {
	ఒకవేళ (curwin->w_llist_ref != ఖాళీ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	లేకపోతే					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    తిరిగి OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    స్థిర పూర్ణాంకం
qf_jump_edit_బఫరు(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	పూర్ణాంకం		forceit,
	పూర్ణాంకం		prev_winid,
	పూర్ణాంకం		*opened_కిటికీ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    పూర్ణాంకం		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		old_qf_curlist = qi->qf_curlist;
    పూర్ణాంకం		save_qfid = qfl->qf_id;

    ఒకవేళ (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	ఒకవేళ (!can_abandon(curbuf, forceit))
	{
	    no_write_సందేశం();
	    తిరిగి FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ఖాళీ, ఖాళీ, ఖాళీ, (linenr_T)1,
		ECMD_HIDE + ECMD_సమితి_HELP,
		prev_winid == curwin->w_id ? curwin : ఖాళీ);
    }
    లేకపోతే
    {
	పూర్ణాంకం	fnum = qf_ptr->qf_fnum;

	ఒకవేళ (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    ఒకవేళ (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
		తిరిగి FAIL;
	    }

	    ఒకవేళ (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_బఫరు))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    ఒకవేళ (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		ఒకవేళ (win_విభజించు(0, 0) == OK)
		    *opened_కిటికీ = TRUE;

		ఒకవేళ (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
		    retval = FAIL;
		}
	    }
	}

	ఒకవేళ (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    ఒకవేళ (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	ఒకవేళ (wp == ఖాళీ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_కిటికీ_was_closed));
	    *opened_కిటికీ = FALSE;
	    తిరిగి QF_ABORT;
	}
    }

    ఒకవేళ (qfl_type == QFLT_QUICKFIX && !qflist_valid(ఖాళీ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_మారింది));
	తిరిగి QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    ఒకవేళ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_నమోదు_present(qfl, qf_ptr))
    {
	ఒకవేళ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_మారింది));
	లేకపోతే
	    emsg(_(e_current_location_list_was_మారింది));
	తిరిగి QF_ABORT;
    }

    తిరిగి retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    స్థిర పూర్ణాంకం
qf_screen_నిలువు_గమ్యం_idx(colnr_T vcol)
{
    char_u	*పంక్తి = ml_get_curline();
    char_u	*p = పంక్తి;
    colnr_T	నిలువు = 0;

    అయితే (*p != NUL && నిలువు < vcol)
    {
	ఒకవేళ (*p == TAB)
	    నిలువు += 8 - (నిలువు % 8);
	లేకపోతే
	    నిలువు += ptr2cells(p);
	ఒకవేళ (నిలువు > vcol)
	    ఆపు;
	MB_PTR_ADV(p);
    }
    తిరిగి (పూర్ణాంకం)(p - పంక్తి);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    స్థిర శూన్య
qf_jump_goto_పంక్తి(
	linenr_T	qf_lnum,
	పూర్ణాంకం		qf_నిలువు,
	char_u		qf_viscol,
	char_u		*qf_నమూనా)
{
    linenr_T		i;

    ఒకవేళ (qf_నమూనా == ఖాళీ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	ఒకవేళ (i > 0)
	{
	    ఒకవేళ (i > curbuf->b_ml.ml_పంక్తి_లెక్క)
		i = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    curwin->w_cursor.lnum = i;
	}
	ఒకవేళ (qf_నిలువు > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    ఒకవేళ (qf_viscol == TRUE)
		curwin->w_cursor.నిలువు = qf_screen_నిలువు_గమ్యం_idx(qf_నిలువు - 1);
	    లేకపోతే
		curwin->w_cursor.నిలువు = qf_నిలువు - 1;
	    curwin->w_సమితి_curswant = true;
	    check_cursor();
	}
	లేకపోతే
	    beginline(BL_WHITE | BL_FIX);
    }
    లేకపోతే
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	ఒకవేళ (!do_శోధన(ఖాళీ, '/', '/', qf_నమూనా, STRLEN(qf_నమూనా), (దీర్ఘ)1, శోధన_KEEP, ఖాళీ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    స్థిర శూన్య
qf_jump_print_సందేశం(
	qf_info_T	*qi,
	పూర్ణాంకం		qf_సూచీ,
	qfline_T	*qf_ptr,
	buf_T		*old_curbuf,
	linenr_T	old_lnum)
{
    linenr_T		i;
    garray_T		*gap;
    size_t		IObufflen;

    gap = qfga_get();

    // Update the screen before showing the message, unless the screen
    // scrolled up.
    ఒకవేళ (!సందేశం_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_సూచీ,
	    qf_get_curlist(qi)->qf_లెక్క,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (అక్షరం *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_పాఠ్యం(gap, skipwhite(qf_ptr->qf_పాఠ్యం));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = సందేశం_scroll;
    ఒకవేళ (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	సందేశం_scroll = TRUE;
    లేకపోతే ఒకవేళ (!సందేశం_scrolled && shortmess(SHM_OVERALL))
	సందేశం_scroll = FALSE;
    సందేశం_attr_keep((అక్షరం *)gap->ga_దత్తాంశం, 0, TRUE);
    సందేశం_scroll = i;

    qfga_clear();
}

/*
 * Find a usable window for opening a file from the quickfix/location list. If
 * a window is not found then open a new window. If 'newwin' is TRUE, then open
 * a new window.
 * Returns OK if successfully jumped or opened a window. Returns FAIL if not
 * able to jump/open a window.  Returns NOTDONE if a file is not associated
 * with the entry.  Returns QF_ABORT if the quickfix/location list was modified
 * by an autocmd.
 */
    స్థిర పూర్ణాంకం
qf_jump_open_కిటికీ(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	పూర్ణాంకం		newwin,
	పూర్ణాంకం		*opened_కిటికీ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    పూర్ణాంకం		old_changedtick = qfl->qf_changedtick;
    పూర్ణాంకం		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    ఒకవేళ (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_బఫరు)
						      || cmdmod.cmod_tab != 0))
	ఒకవేళ (jump_గమ్యం_help_కిటికీ(qi, newwin, opened_కిటికీ) == FAIL)
	    తిరిగి FAIL;
    ఒకవేళ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_నమోదు_present(qfl, qf_ptr))
    {
	ఒకవేళ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_మారింది));
	లేకపోతే
	    emsg(_(e_current_location_list_was_మారింది));
	తిరిగి QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    ఒకవేళ (bt_quickfix(curbuf) && !*opened_కిటికీ)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	ఒకవేళ (qf_ptr->qf_fnum == 0)
	    తిరిగి NOTDONE;

	ఒకవేళ (qf_jump_గమ్యం_usable_కిటికీ(qf_ptr->qf_fnum, newwin,
						opened_కిటికీ) == FAIL)
	    తిరిగి FAIL;
    }
    ఒకవేళ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_నమోదు_present(qfl, qf_ptr))
    {
	ఒకవేళ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_మారింది));
	లేకపోతే
	    emsg(_(e_current_location_list_was_మారింది));
	తిరిగి QF_ABORT;
    }

    తిరిగి OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    స్థిర పూర్ణాంకం
qf_jump_గమ్యం_బఫరు(
	qf_info_T	*qi,
	పూర్ణాంకం		qf_సూచీ,
	qfline_T	*qf_ptr,
	పూర్ణాంకం		forceit,
	పూర్ణాంకం		prev_winid,
	పూర్ణాంకం		*opened_కిటికీ,
	పూర్ణాంకం		openfold,
	పూర్ణాంకం		print_సందేశం)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    పూర్ణాంకం		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    ఒకవేళ (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_బఫరు(qi, qf_ptr, forceit, prev_winid,
						opened_కిటికీ);
	ఒకవేళ (retval != OK)
	    తిరిగి retval;
    }

    // When not switched to another buffer, still need to set pc mark
    ఒకవేళ (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_పంక్తి(qf_ptr->qf_lnum, qf_ptr->qf_నిలువు, qf_ptr->qf_viscol,
	    qf_ptr->qf_నమూనా);

# ifdef FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    ఒకవేళ (print_సందేశం)
	qf_jump_print_సందేశం(qi, qf_సూచీ, qf_ptr, old_curbuf, old_lnum);

    తిరిగి retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    శూన్య
qf_jump(qf_info_T	*qi,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		errornr,
	పూర్ణాంకం		forceit)
{
    qf_jump_newwin(qi, dir, errornr, forceit, FALSE);
}

/*
 * Jump to a quickfix line.
 * If dir == 0 go to entry "errornr".
 * If dir == FORWARD go "errornr" valid entries forward.
 * If dir == BACKWARD go "errornr" valid entries backward.
 * If dir == FORWARD_FILE go "errornr" valid entries files backward.
 * If dir == BACKWARD_FILE go "errornr" valid entries files backward
 * else if "errornr" is zero, redisplay the same line
 * If 'forceit' is TRUE, then can discard changes to the current buffer.
 * If 'newwin' is TRUE, then open the file in a new window.
 */
    స్థిర శూన్య
qf_jump_newwin(qf_info_T	*qi,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		errornr,
	పూర్ణాంకం		forceit,
	పూర్ణాంకం		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    పూర్ణాంకం			qf_సూచీ;
    పూర్ణాంకం			old_qf_సూచీ;
    char_u		*old_swb = p_swb;
    గుర్తులేని		old_swb_flags = swb_flags;
    పూర్ణాంకం			prev_winid;
    పూర్ణాంకం			opened_కిటికీ = FALSE;
    పూర్ణాంకం			print_సందేశం = TRUE;
    పూర్ణాంకం			old_కీలకం_Typed = కీలకం_Typed; // getting file may reset it
    పూర్ణాంకం			retval = OK;

    ఒకవేళ (qi == ఖాళీ)
    {
	ఒకవేళ (ql_info == ఖాళీ)
	{
	    emsg(_(e_no_quickfix_దొంతర));
	    తిరిగి;
	}
	qi = ql_info;
    }

    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi) || qf_list_ఖాళీలు(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	తిరిగి;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_సూచీ = qfl->qf_సూచీ;
    old_qf_సూచీ = qf_సూచీ;

    qf_ptr = qf_get_నమోదు(qfl, errornr, dir, &qf_సూచీ);
    ఒకవేళ (qf_ptr == ఖాళీ)
    {
	qf_ptr = old_qf_ptr;
	qf_సూచీ = old_qf_సూచీ;
	వెళ్ళు theend;
    }

    qfl->qf_సూచీ = qf_సూచీ;
    qfl->qf_ptr = qf_ptr;
    ఒకవేళ (qf_win_pos_update(qi, old_qf_సూచీ))
	// No need to print the error message if it's visible in the error
	//కిటికీ
	print_సందేశం = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_కిటికీ(qi, qf_ptr, newwin, &opened_కిటికీ);
    ఒకవేళ (retval == FAIL)
	వెళ్ళు failed;
    ఒకవేళ (retval == QF_ABORT)
    {
	qi = ఖాళీ;
	qf_ptr = ఖాళీ;
	వెళ్ళు theend;
    }
    ఒకవేళ (retval == NOTDONE)
	వెళ్ళు theend;

    retval = qf_jump_గమ్యం_బఫరు(qi, qf_సూచీ, qf_ptr, forceit, prev_winid,
				  &opened_కిటికీ, old_కీలకం_Typed, print_సందేశం);
    ఒకవేళ (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ఖాళీ;
	qf_ptr = ఖాళీ;
    }

    ఒకవేళ (retval != OK)
    {
	ఒకవేళ (opened_కిటికీ)
	    win_close(curwin, TRUE);    // Close opened window
	ఒకవేళ (qf_ptr != ఖాళీ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_సూచీ = old_qf_సూచీ;
	}
    }
theend:
    ఒకవేళ (qi != ఖాళీ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_సూచీ = qf_సూచీ;
    }
    ఒకవేళ (p_swb != old_swb && p_swb == ఖాళీలు_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
స్థిర పూర్ణాంకం	qfFileAttr;
స్థిర పూర్ణాంకం	qfSepAttr;
స్థిర పూర్ణాంకం	qf_పంక్తి_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    స్థిర శూన్య
qf_list_నమోదు(qfline_T *qfp, పూర్ణాంకం qf_idx, పూర్ణాంకం cursel)
{
    char_u	*fname;
    buf_T	*buf;
    పూర్ణాంకం		filter_నమోదు;
    garray_T	*gap;

    fname = ఖాళీ;
    ఒకవేళ (qfp->qf_module != ఖాళీ && *qfp->qf_module != NUL)
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(అక్షరం *)qfp->qf_module);
    లేకపోతే
    {
	ఒకవేళ (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ఖాళీ)
	{
	    ఒకవేళ (qfp->qf_fname == ఖాళీ)
		fname = buf->b_fname;
	    లేకపోతే
		fname = qfp->qf_fname;
	    ఒకవేళ (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	ఒకవేళ (fname == ఖాళీ)
	    sprintf((అక్షరం *)IObuff, "%2d", qf_idx);
	లేకపోతే
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (అక్షరం *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_నమోదు = TRUE;
    ఒకవేళ (qfp->qf_module != ఖాళీ && *qfp->qf_module != NUL)
	filter_నమోదు &= సందేశం_filtered(qfp->qf_module);
    ఒకవేళ (filter_నమోదు && fname != ఖాళీ)
	filter_నమోదు &= సందేశం_filtered(fname);
    ఒకవేళ (filter_నమోదు && qfp->qf_నమూనా != ఖాళీ)
	filter_నమోదు &= సందేశం_filtered(qfp->qf_నమూనా);
    ఒకవేళ (filter_నమోదు)
	filter_నమోదు &= సందేశం_filtered(qfp->qf_పాఠ్యం);
    ఒకవేళ (filter_నమోదు)
	తిరిగి;

    సందేశం_putchar('\n');
    సందేశం_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    ఒకవేళ (qfp->qf_lnum != 0)
	సందేశం_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    ఒకవేళ (qfp->qf_lnum != 0)
	qf_range_పాఠ్యం(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    సందేశం_puts_attr((అక్షరం *)gap->ga_దత్తాంశం, qf_పంక్తి_Attr);
    సందేశం_puts_attr(":", qfSepAttr);
    ఒకవేళ (qfp->qf_నమూనా != ఖాళీ)
    {
	gap = qfga_get();
	qf_fmt_పాఠ్యం(gap, qfp->qf_నమూనా);
	ga_append(gap, NUL);
	సందేశం_puts((అక్షరం *)gap->ga_దత్తాంశం);
	సందేశం_puts_attr(":", qfSepAttr);
    }
    సందేశం_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_పాఠ్యం(gap, (fname != ఖాళీ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_పాఠ్యం) : qfp->qf_పాఠ్యం);
    ga_append(gap, NUL);
    సందేశం_prt_పంక్తి((char_u *)gap->ga_దత్తాంశం, FALSE);
    బహిర్గతం_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    శూన్య
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		idx1 = 1;
    పూర్ణాంకం		idx2 = -1;
    char_u	*arg = eap->arg;
    పూర్ణాంకం		plus = FALSE;
    పూర్ణాంకం		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi) || qf_list_ఖాళీలు(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	తిరిగి;
    }
    ఒకవేళ (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    ఒకవేళ (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	తిరిగి;
    }
    qfl = qf_get_curlist(qi);
    ఒకవేళ (plus)
    {
	i = qfl->qf_సూచీ;
	idx2 = i + idx1;
	idx1 = i;
    }
    లేకపోతే
    {
	i = qfl->qf_లెక్క;
	ఒకవేళ (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	ఒకవేళ (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    ఒకవేళ (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    ఒకవేళ (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_పంక్తి_Attr = syn_name2attr((char_u *)"qfLineNr");
    ఒకవేళ (qf_పంక్తి_Attr == 0)
	qf_పంక్తి_Attr = HL_ATTR(HLF_N);

    ఒకవేళ (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
    {
	ఒకవేళ ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_నమోదు(qfp, i, i == qfl->qf_సూచీ);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    స్థిర శూన్య
qf_fmt_పాఠ్యం(garray_T *gap, char_u *పాఠ్యం)
{
    char_u	*p = పాఠ్యం;
    అయితే (*p != NUL)
    {
	ఒకవేళ (*p == '\n')
	{
	    ga_append(gap, ' ');
	    అయితే (*++p != NUL)
		ఒకవేళ (!VIM_ISWHITE(*p) && *p != '\n')
		    ఆపు;
	}
	లేకపోతే
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    స్థిర శూన్య
qf_range_పాఠ్యం(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.నిడివి = vim_snprintf_safelen((అక్షరం *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    ఒకవేళ (qfp->qf_చివర_lnum > 0 && qfp->qf_lnum != qfp->qf_చివర_lnum)
    {
	buf.నిడివి += vim_snprintf_safelen((అక్షరం *)buf.string + buf.నిడివి,
	    IOSIZE - buf.నిడివి, "-%ld", qfp->qf_చివర_lnum);
    }
    ఒకవేళ (qfp->qf_నిలువు > 0)
    {
	buf.నిడివి += vim_snprintf_safelen((అక్షరం *)buf.string + buf.నిడివి,
	    IOSIZE - buf.నిడివి, " col %d", qfp->qf_నిలువు);
	ఒకవేళ (qfp->qf_చివర_నిలువు > 0 && qfp->qf_నిలువు != qfp->qf_చివర_నిలువు)
	    buf.నిడివి += vim_snprintf_safelen((అక్షరం *)buf.string + buf.నిడివి,
		IOSIZE - buf.నిడివి, "-%d", qfp->qf_చివర_నిలువు);
    }

    ga_concat_len(gap, buf.string, buf.నిడివి);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    స్థిర శూన్య
qf_సందేశం(qf_info_T *qi, పూర్ణాంకం which, అక్షరం *lead)
{
    అక్షరం   *title = (అక్షరం *)qi->qf_lists[which].qf_title;
    పూర్ణాంకం    లెక్క = qi->qf_lists[which].qf_లెక్క;
    char_u buf[IOSIZE];

    vim_snprintf((అక్షరం *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    లెక్క);

    ఒకవేళ (title != ఖాళీ)
    {
	size_t	len = STRLEN(buf);

	ఒకవేళ (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    సందేశం((అక్షరం *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    శూన్య
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    పూర్ణాంకం		లెక్క;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    ఒకవేళ (eap->addr_లెక్క != 0)
	లెక్క = eap->line2;
    లేకపోతే
	లెక్క = 1;
    అయితే (లెక్క--)
    {
	ఒకవేళ (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    ఒకవేళ (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_యొక్క_quickfix_దొంతర));
		ఆపు;
	    }
	    --qi->qf_curlist;
	}
	లేకపోతే
	{
	    ఒకవేళ (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_శిఖరం_యొక్క_quickfix_దొంతర));
		ఆపు;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_సందేశం(qi, qi->qf_curlist, "");
    qf_update_బఫరు(qi, ఖాళీ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    శూన్య
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_దొంతర(eap, FALSE);
    పూర్ణాంకం		i;

    ఒకవేళ (eap->addr_లెక్క > 0)
    {
	ఒకవేళ (qi == ఖాళీ)
	{
	    emsg(_(e_no_location_list));
	    తిరిగి;
	}

	// Jump to the specified quickfix list
	ఒకవేళ (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_సందేశం(qi, qi->qf_curlist, "");
	    qf_update_బఫరు(qi, ఖాళీ);
	}
	లేకపోతే
	    emsg(_(e_invalid_range));

	తిరిగి;
    }

    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi))
	సందేశం(_("No entries"));
    లేకపోతే
	కొరకు (i = 0; i < qi->qf_listcount; ++i)
	    qf_సందేశం(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    స్థిర శూన్య
qf_free_వస్తువులు(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    పూర్ణాంకం		stop = FALSE;

    అయితే (qfl->qf_లెక్క && qfl->qf_ప్రారంభం != ఖాళీ)
    {
	qfp = qfl->qf_ప్రారంభం;
	qfpnext = qfp->qf_తదుపరి;
	ఒకవేళ (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_పాఠ్యం);
	    vim_free(qfp->qf_నమూనా);
	    clear_tv(&qfp->qf_user_దత్తాంశం);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    ఒకవేళ (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_లెక్క = 1;
	    లేకపోతే
		qfl->qf_ప్రారంభం = qfpnext;
	}
	--qfl->qf_లెక్క;
    }

    qfl->qf_సూచీ = 0;
    qfl->qf_ప్రారంభం = ఖాళీ;
    qfl->qf_చివరి = ఖాళీ;
    qfl->qf_ptr = ఖాళీ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_దొంతర(&qfl->qf_dir_దొంతర);
    qfl->qf_directory = ఖాళీ;
    qf_clean_dir_దొంతర(&qfl->qf_file_దొంతర);
    qfl->qf_currfile = ఖాళీ;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    స్థిర శూన్య
qf_free(qf_list_T *qfl)
{
    qf_free_వస్తువులు(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = ఖాళీ;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   శూన్య
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	దీర్ఘ	amount,
	దీర్ఘ	amount_after)
{
    పూర్ణాంకం		i;
    qfline_T	*qfp;
    పూర్ణాంకం		idx;
    qf_info_T	*qi = ql_info;
    పూర్ణాంకం		దొరికింది_one = FALSE;
    పూర్ణాంకం		buf_has_flag = wp == ఖాళీ ? BUF_HAS_QF_నమోదు : BUF_HAS_LL_నమోదు;

    ఒకవేళ (!(curbuf->b_has_qf_నమోదు & buf_has_flag))
	తిరిగి;
    ఒకవేళ (wp != ఖాళీ)
    {
	ఒకవేళ (wp->w_llist == ఖాళీ)
	    తిరిగి;
	qi = wp->w_llist;
    }
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
	తిరిగి;

    కొరకు (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	ఒకవేళ (!qf_list_ఖాళీలు(qfl))
	    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
		ఒకవేళ (qfp->qf_fnum == curbuf->b_fnum)
		{
		    దొరికింది_one = TRUE;
		    ఒకవేళ (qfp->qf_cleared)
			కొనసాగు;

		    ఒకవేళ (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			ఒకవేళ (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			లేకపోతే
			    qfp->qf_lnum += amount;
		    }
		    లేకపోతే ఒకవేళ (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    ఒకవేళ (!దొరికింది_one)
	curbuf->b_has_qf_నమోదు &= ~buf_has_flag;
}

/*
 * Make a nice message out of the error character and the error number:
 *  char    number	message
 *  e or E    0		" error"
 *  w or W    0		" warning"
 *  i or I    0		" info"
 *  n or N    0		" note"
 *  0	      0		""
 *  other     0		" c"
 *  e or E    n		" error n"
 *  w or W    n		" warning n"
 *  i or I    n		" info n"
 *  n or N    n		" note n"
 *  0	      n		" error n"
 *  other     n		" c n"
 *  1	      x		""	:helpgrep
 */
    స్థిర char_u *
qf_types(పూర్ణాంకం c, పూర్ణాంకం nr)
{
    స్థిర char_u	buf[20];
    స్థిర char_u	cc[3];
    char_u		*p;

    ఒకవేళ (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    లేకపోతే ఒకవేళ (c == 'I' || c == 'i')
	p = (char_u *)" info";
    లేకపోతే ఒకవేళ (c == 'N' || c == 'n')
	p = (char_u *)" note";
    లేకపోతే ఒకవేళ (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    లేకపోతే ఒకవేళ (c == 0 || c == 1)
	p = (char_u *)"";
    లేకపోతే
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    ఒకవేళ (nr <= 0)
	తిరిగి p;

    sprintf((అక్షరం *)buf, "%s %3d", (అక్షరం *)p, nr);
    తిరిగి buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    శూన్య
qf_view_ఫలితం(పూర్ణాంకం విభజించు)
{
    qf_info_T   *qi = ql_info;

    ఒకవేళ (IS_LL_కిటికీ(curwin))
	qi = GET_LOC_LIST(curwin);
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
    {
	emsg(_(e_no_quickfix_దొంతర));
	తిరిగి;
    }

    ఒకవేళ (qf_list_ఖాళీలు(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	తిరిగి;
    }

    ఒకవేళ (విభజించు)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (దీర్ఘ)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	తిరిగి;
    }

    do_cmdline_cmd((char_u *)(IS_LL_కిటికీ(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    శూన్య
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_కనుగొను_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi)
	    || qfl->qf_nonevalid
	    || qf_list_ఖాళీలు(qfl))
    {
	ఒకవేళ (win != ఖాళీ)
	    ex_cclose(eap);
    }
    లేకపోతే ఒకవేళ (win == ఖాళీ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    శూన్య
ex_cclose(exarg_T *eap)
{
    win_T	*win = ఖాళీ;
    qf_info_T	*qi;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, FALSE)) == ఖాళీ)
	తిరిగి;

    // Find existing quickfix window and close it.
    win = qf_కనుగొను_win(qi);
    ఒకవేళ (win != ఖాళీ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    స్థిర శూన్య
qf_సమితి_title_var(qf_list_T *qfl)
{
    ఒకవేళ (qfl->qf_title != ఖాళీ)
	సమితి_అంతర్గత_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
qf_goto_cwindow(qf_info_T *qi, పూర్ణాంకం resize, పూర్ణాంకం sz, పూర్ణాంకం vertsplit)
{
    win_T	*win;

    win = qf_కనుగొను_win(qi);
    ఒకవేళ (win == ఖాళీ)
	తిరిగి FAIL;

    win_goto(win);
    ఒకవేళ (resize)
    {
	ఒకవేళ (vertsplit)
	{
	    ఒకవేళ (sz != win->w_వెడల్పు)
		win_setwidth(sz);
	}
	లేకపోతే ఒకవేళ (sz != win->w_ఎత్తు && win->w_ఎత్తు
		       + win->w_స్థితి_ఎత్తు + tabline_ఎత్తు() < cmdline_అడ్డువరుస)
	    win_setheight(sz);
    }

    తిరిగి OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    స్థిర శూన్య
qf_సమితి_cwindow_options(శూన్య)
{
    // switch off 'swapfile'
    సమితి_option_విలువ_give_err((char_u *)"swf", 0L, ఖాళీ, OPT_LOCAL);
    సమితి_option_విలువ_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    సమితి_option_విలువ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    సమితి_option_విలువ_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    స్థిర పూర్ణాంకం
qf_open_new_cwindow(qf_info_T *qi, పూర్ణాంకం ఎత్తు)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    పూర్ణాంకం		flags = 0;
    win_T	*win;

    qf_buf = qf_కనుగొను_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    ఒకవేళ (IS_QF_దొంతర(qi) && cmdmod.cmod_విభజించు == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    ఒకవేళ (cmdmod.cmod_విభజించు == 0)
	flags = WSP_క్రింద;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    ఒకవేళ (IS_QF_దొంతర(qi))
	flags |= WSP_QUICKFIX;

    ఒకవేళ (win_విభజించు(ఎత్తు, flags) == FAIL)
	తిరిగి FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    ఒకవేళ (IS_LL_దొంతర(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    ఒకవేళ (oldwin != curwin)
	oldwin = ఖాళీ;  // don't store info when in another window
    ఒకవేళ (qf_buf != ఖాళీ)
    {
	// Use the existing quickfix buffer
	ఒకవేళ (do_ecmd(qf_buf->b_fnum, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	// Create a new quickfix buffer
	ఒకవేళ (do_ecmd(0, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    తిరిగి FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    ఒకవేళ (!bt_quickfix(curbuf))
	qf_సమితి_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    ఒకవేళ (curtab == prevtab && curwin->w_వెడల్పు == Columns)
	win_setheight(ఎత్తు);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    ఒకవేళ (win_valid(win))
	prevwin = win;

    తిరిగి OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    శూన్య
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    పూర్ణాంకం		ఎత్తు;
    పూర్ణాంకం		స్థితి_2 = FAIL;
    పూర్ణాంకం		lnum;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    incr_quickfix_busy();

    ఒకవేళ (eap->addr_లెక్క != 0)
	ఎత్తు = eap->line2;
    లేకపోతే
	ఎత్తు = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    ఒకవేళ (cmdmod.cmod_tab == 0)
	స్థితి_2 = qf_goto_cwindow(qi, eap->addr_లెక్క != 0, ఎత్తు,
						cmdmod.cmod_విభజించు & WSP_VERT);
    ఒకవేళ (స్థితి_2 == FAIL)
	ఒకవేళ (qf_open_new_cwindow(qi, ఎత్తు) == FAIL)
	{
	    decr_quickfix_busy();
	    తిరిగి;
	}

    qfl = qf_get_curlist(qi);
    qf_సమితి_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_సూచీ;

    // Fill the buffer with the quickfix list.
    qf_fill_బఫరు(qfl, curbuf, ఖాళీ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.నిలువు = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    స్థిర శూన్య
qf_win_goto(win_T *win, linenr_T lnum)
{
    win_T	*old_curwin = curwin;

    curwin = win;
    curbuf = win->w_బఫరు;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.నిలువు = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_స్థితి = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_బఫరు;
}

/*
 * :cbottom/:lbottom commands.
 */
    శూన్య
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    win = qf_కనుగొను_win(qi);
    ఒకవేళ (win != ఖాళీ && win->w_cursor.lnum != win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	qf_win_goto(win, win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_నమోదు(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ఒకవేళ (IS_LL_కిటికీ(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
	తిరిగి 0;

    తిరిగి qf_get_curlist(qi)->qf_సూచీ;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    స్థిర పూర్ణాంకం
qf_win_pos_update(
    qf_info_T	*qi,
    పూర్ణాంకం		old_qf_సూచీ)	// previous qf_index or zero
{
    win_T	*win;
    పూర్ణాంకం		qf_సూచీ = qf_get_curlist(qi)->qf_సూచీ;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_కనుగొను_win(qi);
    ఒకవేళ (win != ఖాళీ
	    && qf_సూచీ <= win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క
	    && old_qf_సూచీ != qf_సూచీ)
    {
	ఒకవేళ (qf_సూచీ > old_qf_సూచీ)
	{
	    win->w_redraw_శిఖరం = old_qf_సూచీ;
	    win->w_redraw_bot = qf_సూచీ;
	}
	లేకపోతే
	{
	    win->w_redraw_శిఖరం = qf_సూచీ;
	    win->w_redraw_bot = old_qf_సూచీ;
	}
	qf_win_goto(win, qf_సూచీ);
    }
    తిరిగి win != ఖాళీ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    స్థిర పూర్ణాంకం
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    ఒకవేళ (buf_valid(win->w_బఫరు) && bt_quickfix(win->w_బఫరు))
	ఒకవేళ ((IS_QF_దొంతర(qi) && win->w_llist_ref == ఖాళీ)
		|| (IS_LL_దొంతర(qi) && win->w_llist_ref == qi))
	    తిరిగి TRUE;

    తిరిగి FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    స్థిర win_T *
qf_కనుగొను_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ఒకవేళ (is_qf_win(win, qi))
	    తిరిగి win;
    తిరిగి ఖాళీ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    స్థిర buf_T *
qf_కనుగొను_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    ఒకవేళ (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	ఒకవేళ (qfbuf != ఖాళీ)
	    తిరిగి qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	ఒకవేళ (is_qf_win(win, qi))
	    తిరిగి win->w_బఫరు;

    తిరిగి ఖాళీ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    అక్షరం *
did_సమితి_quickfixtextfunc(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (option_సమితి_callback_func(p_qftf, &qftf_cb) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    స్థిర శూన్య
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ఒకవేళ (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_సమితి_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    స్థిర శూన్య
qf_update_బఫరు(qf_info_T *qi, qfline_T *old_చివరి)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_కనుగొను_buf(qi);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    linenr_T	old_పంక్తి_లెక్క = buf->b_ml.ml_పంక్తి_లెక్క;
    పూర్ణాంకం		qf_winid = 0;

    ఒకవేళ (IS_LL_దొంతర(qi))
    {
	ఒకవేళ (curwin->w_llist == qi)
	    win = curwin;
	లేకపోతే
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_కనుగొను_win_with_loclist(qi);
	    ఒకవేళ (win == ఖాళీ)
		// File window is not found. Find the location list window.
		win = qf_కనుగొను_win(qi);
	    ఒకవేళ (win == ఖాళీ)
		తిరిగి;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    పూర్ణాంకం do_fill = TRUE;
    ఒకవేళ (old_చివరి == ఖాళీ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    ఒకవేళ (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_బఫరు(qf_get_curlist(qi), buf, old_చివరి, qf_winid);
	++CHANGEDTICK(buf);

	ఒకవేళ (old_చివరి == ఖాళీ)
	{
	    (శూన్య)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    ఒకవేళ ((win = qf_కనుగొను_win(qi)) != ఖాళీ && old_పంక్తి_లెక్క < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    స్థిర పూర్ణాంకం
qf_buf_కూడు_పంక్తి(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	పూర్ణాంకం		మొదటి_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    ఒకవేళ (qftf_str != ఖాళీ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    లేకపోతే
    {
	ఒకవేళ (qfp->qf_module != ఖాళీ)
	    ga_concat(gap, qfp->qf_module);
	లేకపోతే ఒకవేళ (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ఖాళీ
		&& errbuf->b_fname != ఖాళీ)
	{
	    ఒకవేళ (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    లేకపోతే
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		ఒకవేళ (మొదటి_bufline && (errbuf->b_sfname == ఖాళీ
				|| mch_is_నిండిన_Name(errbuf->b_sfname)))
		{
		    ఒకవేళ (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		ఒకవేళ (qfp->qf_fname == ఖాళీ)
		    ga_concat(gap, errbuf->b_fname);
		లేకపోతే
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	ఒకవేళ (qfp->qf_lnum > 0)
	{
	    qf_range_పాఠ్యం(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	లేకపోతే ఒకవేళ (qfp->qf_నమూనా != ఖాళీ)
	    qf_fmt_పాఠ్యం(gap, qfp->qf_నమూనా);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_పాఠ్యం(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_పాఠ్యం)
							       : qfp->qf_పాఠ్యం);
    }

    ga_append(gap, NUL);
    ఒకవేళ (ml_append_buf(buf, lnum, gap->ga_దత్తాంశం, gap->ga_len, FALSE) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    స్థిర list_T *
call_qftf_func(qf_list_T *qfl, పూర్ణాంకం qf_winid, దీర్ఘ ప్రారంభం_idx, దీర్ఘ చివర_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ఖాళీ;
    స్థిర పూర్ణాంకం	recursive = FALSE;

    ఒకవేళ (recursive)
	తిరిగి ఖాళీ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    ఒకవేళ (qfl->qf_qftf_cb.cb_name != ఖాళీ)
	cb = &qfl->qf_qftf_cb;
    ఒకవేళ (cb->cb_name != ఖాళీ)
    {
	typval_T	వాదనలు[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	ఒకవేళ ((d = dict_alloc_తాళం(VAR_FIXED)) == ఖాళీ)
	{
	    recursive = FALSE;
	    తిరిగి ఖాళీ;
	}
	dict_కూడు_సంఖ్య(d, "quickfix", (దీర్ఘ)IS_QF_LIST(qfl));
	dict_కూడు_సంఖ్య(d, "winid", (దీర్ఘ)qf_winid);
	dict_కూడు_సంఖ్య(d, "id", (దీర్ఘ)qfl->qf_id);
	dict_కూడు_సంఖ్య(d, "start_idx", ప్రారంభం_idx);
	dict_కూడు_సంఖ్య(d, "end_idx", చివర_idx);
	++d->dv_refcount;
	వాదనలు[0].v_type = VAR_DICT;
	వాదనలు[0].vval.v_dict = d;

	qftf_list = ఖాళీ;
	textlock++;
	ఒకవేళ (call_callback(cb, 0, &rettv, 1, వాదనలు) != FAIL)
	{
	    ఒకవేళ (rettv.v_type == VAR_LIST)
	    {
		qftf_list = rettv.vval.v_list;
		qftf_list->lv_refcount++;
	    }
	    clear_tv(&rettv);
	}
	textlock--;
	dict_unref(d);
    }

    recursive = FALSE;
    తిరిగి qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    స్థిర శూన్య
qf_fill_బఫరు(qf_list_T *qfl, buf_T *buf, qfline_T *old_చివరి, పూర్ణాంకం qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    పూర్ణాంకం		old_కీలకం_Typed = కీలకం_Typed;
    list_T	*qftf_list = ఖాళీ;
    listitem_T	*qftf_li = ఖాళీ;

    ఒకవేళ (old_చివరి == ఖాళీ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	ఒకవేళ (buf != curbuf)
	{
	    అంతర్గత_error("qf_fill_buffer()");
	    తిరిగి;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	అయితే ((curbuf->b_ml.ml_flags & ML_ఖాళీలు) == 0)
	    (శూన్య)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    ఒకవేళ (qfl != ఖాళీ && qfl->qf_ప్రారంభం != ఖాళీ)
    {
	char_u		dirname[MAXPATHL];
	పూర్ణాంకం		invalid_val = FALSE;
	పూర్ణాంకం		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	ఒకవేళ (old_చివరి == ఖాళీ)
	{
	    qfp = qfl->qf_ప్రారంభం;
	    lnum = 0;
	}
	లేకపోతే
	{
	    ఒకవేళ (old_చివరి->qf_తదుపరి != ఖాళీ)
		qfp = old_చివరి->qf_తదుపరి;
	    లేకపోతే
		qfp = old_చివరి;
	    lnum = buf->b_ml.ml_పంక్తి_లెక్క;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (దీర్ఘ)(lnum + 1),
							(దీర్ఘ)qfl->qf_లెక్క);
	ఒకవేళ (qftf_list != ఖాళీ)
	    qftf_li = qftf_list->lv_మొదటి;

	అయితే (lnum < qfl->qf_లెక్క)
	{
	    char_u	*qftf_str = ఖాళీ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    ఒకవేళ (qftf_li != ఖాళీ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		ఒకవేళ (qftf_str == ఖాళీ)
		    invalid_val = TRUE;
	    }

	    ఒకవేళ (qf_buf_కూడు_పంక్తి(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ఆపు;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_తదుపరి;
	    ఒకవేళ (qfp == ఖాళీ)
		ఆపు;

	    ఒకవేళ (qftf_li != ఖాళీ)
		qftf_li = qftf_li->li_తదుపరి;
	}

	ఒకవేళ (old_చివరి == ఖాళీ)
	    // Delete the empty line which is now at the end
	    (శూన్య)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    ఒకవేళ (old_చివరి == ఖాళీ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_తాళం;
	సమితి_option_విలువ_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ఖాళీ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ఖాళీ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_తాళం;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    కీలకం_Typed = old_కీలకం_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    స్థిర శూన్య
qf_list_మారింది(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    స్థిర పూర్ణాంకం
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    పూర్ణాంకం		qf_idx;

    కొరకు (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	ఒకవేళ (qi->qf_lists[qf_idx].qf_id == qfid)
	    తిరిగి qf_idx;
    తిరిగి INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    స్థిర పూర్ణాంకం
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    పూర్ణాంకం curlist;

    ఒకవేళ (qf_get_curlist(qi)->qf_id == save_qfid)
	తిరిగి OK;

    curlist = qf_id2nr(qi, save_qfid);
    ఒకవేళ (curlist < 0)
	// list is not present
	తిరిగి FAIL;
    qi->qf_curlist = curlist;
    తిరిగి OK;
}

/*
 * Jump to the first entry if there is one.
 */
    స్థిర శూన్య
qf_jump_మొదటి(qf_info_T *qi, int_u save_qfid, పూర్ణాంకం forceit)
{
    ఒకవేళ (qf_restore_list(qi, save_qfid) == FAIL)
	తిరిగి;


    ఒకవేళ (!check_can_సమితి_curbuf_forceit(forceit))
	తిరిగి;


    // Autocommands might have cleared the list, check for that.
    ఒకవేళ (!qf_list_ఖాళీలు(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    పూర్ణాంకం
grep_అంతర్గత(cmdidx_T cmdidx)
{
    తిరిగి ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    స్థిర char_u *
make_get_auname(cmdidx_T cmdidx)
{
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_make:	    తిరిగి (char_u *)"make";
	స్థితి CMD_lmake:	    తిరిగి (char_u *)"lmake";
	స్థితి CMD_grep:	    తిరిగి (char_u *)"grep";
	స్థితి CMD_lgrep:	    తిరిగి (char_u *)"lgrep";
	స్థితి CMD_grepadd:   తిరిగి (char_u *)"grepadd";
	స్థితి CMD_lgrepadd:  తిరిగి (char_u *)"lgrepadd";
	అప్రమేయ: తిరిగి ఖాళీ;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    స్థిర char_u *
get_mef_name(శూన్య)
{
    char_u	*p;
    char_u	*name;
    స్థిర పూర్ణాంకం	ప్రారంభం = -1;
    స్థిర పూర్ణాంకం	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    ఒకవేళ (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_cant_get_తాత్కాలిక_file_name));
	తిరిగి name;
    }

    కొరకు (p = p_mef; *p; ++p)
	ఒకవేళ (p[0] == '#' && p[1] == '#')
	    ఆపు;

    ఒకవేళ (*p == NUL)
	తిరిగి vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    కొరకు (;;)
    {
	ఒకవేళ (ప్రారంభం == -1)
	    ప్రారంభం = mch_get_pid();
	లేకపోతే
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	ఒకవేళ (name == ఖాళీ)
	    ఆపు;
	STRCPY(name, p_mef);
	sprintf((అక్షరం *)name + (p - p_mef), "%d%d", ప్రారంభం, off);
	STRCAT(name, p + 2);
	ఒకవేళ (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((అక్షరం *)name, &sb) < 0
# endif
		)
	    ఆపు;
	vim_free(name);
    }
    తిరిగి name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    స్థిర char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    గుర్తులేని	len;

    len = (గుర్తులేని)STRLEN(p_shq) * 2 + (గుర్తులేని)STRLEN(makecmd) + 1;
    ఒకవేళ (*p_sp != NUL)
	len += (గుర్తులేని)STRLEN(p_sp) + (గుర్తులేని)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    ఒకవేళ (cmd == ఖాళీ)
	తిరిగి ఖాళీ;
    sprintf((అక్షరం *)cmd, "%s%s%s", (అక్షరం *)p_shq, (అక్షరం *)makecmd,
							       (అక్షరం *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    ఒకవేళ (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    ఒకవేళ (సందేశం_నిలువు == 0)
	సందేశం_didout = FALSE;
    సందేశం_ప్రారంభం();
    సందేశం_puts(":!");
    సందేశం_outtrans(cmd);		// show what we are doing

    తిరిగి cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    శూన్య
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ఖాళీ;
    win_T	*wp = ఖాళీ;
    qf_info_T	*qi = ql_info;
    పూర్ణాంకం		res;
    char_u	*au_name = ఖాళీ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    పూర్ణాంకం		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    ఒకవేళ (grep_అంతర్గత(eap->cmdidx))
    {
	ex_vimgrep(eap);
	తిరిగి;
    }

    au_name = make_get_auname(eap->cmdidx);
    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ఒకవేళ (aborting())
	    తిరిగి;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    ఒకవేళ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    ఒకవేళ (cmd == ఖాళీ)
    {
	vim_free(fname);
	తిరిగి;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    బహిర్గతం_flush();
		// read window status report and redraw before message
    (శూన్య)char_avail();
# endif

    incr_quickfix_busy();

    ఒకవేళ (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    ఒకవేళ (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    ఒకవేళ (wp != ఖాళీ)
    {
	qi = GET_LOC_LIST(wp);
	ఒకవేళ (qi == ఖాళీ)
	    వెళ్ళు cleanup;
    }
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
	వెళ్ళు cleanup;

    ఒకవేళ (res >= 0)
	qf_list_మారింది(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ఒకవేళ (au_name != ఖాళీ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    ఒకవేళ (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_మొదటి(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    పూర్ణాంకం
qf_get_కొలత(exarg_T *eap)
{
    qf_info_T	*qi;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, FALSE)) == ఖాళీ)
	తిరిగి 0;
    తిరిగి qf_get_curlist(qi)->qf_లెక్క;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    పూర్ణాంకం
qf_get_valid_కొలత(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    పూర్ణాంకం		i, sz = 0;
    పూర్ణాంకం		prev_fnum = 0;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, FALSE)) == ఖాళీ)
	తిరిగి 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
    {
	ఒకవేళ (qfp->qf_valid)
	{
	    ఒకవేళ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    లేకపోతే ఒకవేళ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    తిరిగి sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    పూర్ణాంకం
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, FALSE)) == ఖాళీ)
	తిరిగి 0;

    తిరిగి qf_get_curlist(qi)->qf_సూచీ;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    పూర్ణాంకం
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    పూర్ణాంకం		i, eidx = 0;
    పూర్ణాంకం		prev_fnum = 0;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, FALSE)) == ఖాళీ)
	తిరిగి 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_ప్రారంభం;

    // check if the list has valid errors
    ఒకవేళ (!qf_list_has_valid_entries(qfl))
	తిరిగి 1;

    కొరకు (i = 1; i <= qfl->qf_సూచీ && qfp!= ఖాళీ; i++, qfp = qfp->qf_తదుపరి)
    {
	ఒకవేళ (qfp->qf_valid)
	{
	    ఒకవేళ (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ఒకవేళ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    లేకపోతే
		eidx++;
	}
    }

    తిరిగి eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    స్థిర పూర్ణాంకం
qf_get_nth_valid_నమోదు(qf_list_T *qfl, పూర్ణాంకం n, పూర్ణాంకం fdo)
{
    qfline_T	*qfp;
    పూర్ణాంకం		i, eidx;
    పూర్ణాంకం		prev_fnum = 0;

    // check if the list has valid errors
    ఒకవేళ (!qf_list_has_valid_entries(qfl))
	తిరిగి 1;

    eidx = 0;
    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
    {
	ఒకవేళ (qfp->qf_valid)
	{
	    ఒకవేళ (fdo)
	    {
		ఒకవేళ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    లేకపోతే
		eidx++;
	}

	ఒకవేళ (eidx == n)
	    ఆపు;
    }

    ఒకవేళ (i <= qfl->qf_లెక్క)
	తిరిగి i;
    లేకపోతే
	తిరిగి 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    శూన్య
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    పూర్ణాంకం		errornr;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    ఒకవేళ (eap->addr_లెక్క > 0)
	errornr = (పూర్ణాంకం)eap->line2;
    లేకపోతే
    {
	ఎంపిక (eap->cmdidx)
	{
	    స్థితి CMD_cc: స్థితి CMD_ll:
		errornr = 0;
		ఆపు;
	    స్థితి CMD_crewind: స్థితి CMD_lrewind: స్థితి CMD_cfirst:
	    స్థితి CMD_lfirst:
		errornr = 1;
		ఆపు;
	    అప్రమేయ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    ఒకవేళ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_నమోదు(qf_get_curlist(qi),
		eap->addr_లెక్క > 0 ? (పూర్ణాంకం)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    శూన్య
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    పూర్ణాంకం		errornr;
    పూర్ణాంకం		dir;

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    ఒకవేళ (eap->addr_లెక్క > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (పూర్ణాంకం)eap->line2;
    లేకపోతే
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    ఎంపిక (eap->cmdidx)
    {
	స్థితి CMD_cnext: స్థితి CMD_lnext: స్థితి CMD_cdo: స్థితి CMD_ldo:
	    dir = FORWARD;
	    ఆపు;
	స్థితి CMD_cprevious: స్థితి CMD_lprevious: స్థితి CMD_c_తదుపరి:
	స్థితి CMD_l_తదుపరి:
	    dir = BACKWARD;
	    ఆపు;
	స్థితి CMD_cnfile: స్థితి CMD_lnfile: స్థితి CMD_cfdo: స్థితి CMD_lfdo:
	    dir = FORWARD_FILE;
	    ఆపు;
	స్థితి CMD_cpfile: స్థితి CMD_lpfile: స్థితి CMD_cNfile: స్థితి CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ఆపు;
	అప్రమేయ:
	    dir = FORWARD;
	    ఆపు;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    స్థిర qfline_T *
qf_కనుగొను_మొదటి_నమోదు_లోపల_buf(qf_list_T *qfl, పూర్ణాంకం bnr, పూర్ణాంకం *errornr)
{
    qfline_T	*qfp = ఖాళీ;
    పూర్ణాంకం		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_వస్తువులు(qfl, qfp, idx)
	ఒకవేళ (qfp->qf_fnum == bnr)
	    ఆపు;

    *errornr = idx;
    తిరిగి qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    స్థిర qfline_T *
qf_కనుగొను_మొదటి_నమోదు_on_పంక్తి(qfline_T *నమోదు_2, పూర్ణాంకం *errornr)
{
    అయితే (!got_int
	    && నమోదు_2->qf_prev != ఖాళీ
	    && నమోదు_2->qf_fnum == నమోదు_2->qf_prev->qf_fnum
	    && నమోదు_2->qf_lnum == నమోదు_2->qf_prev->qf_lnum)
    {
	నమోదు_2 = నమోదు_2->qf_prev;
	--*errornr;
    }

    తిరిగి నమోదు_2;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    స్థిర qfline_T *
qf_కనుగొను_చివరి_నమోదు_on_పంక్తి(qfline_T *నమోదు_2, పూర్ణాంకం *errornr)
{
    అయితే (!got_int &&
	    నమోదు_2->qf_తదుపరి != ఖాళీ
	    && నమోదు_2->qf_fnum == నమోదు_2->qf_తదుపరి->qf_fnum
	    && నమోదు_2->qf_lnum == నమోదు_2->qf_తదుపరి->qf_lnum)
    {
	నమోదు_2 = నమోదు_2->qf_తదుపరి;
	++*errornr;
    }

    తిరిగి నమోదు_2;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    స్థిర పూర్ణాంకం
qf_నమోదు_after_pos(qfline_T *qfp, pos_T *pos, పూర్ణాంకం linewise)
{
    ఒకవేళ (linewise)
	తిరిగి qfp->qf_lnum > pos->lnum;
    లేకపోతే
	తిరిగి (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_నిలువు > pos->నిలువు));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    స్థిర పూర్ణాంకం
qf_నమోదు_before_pos(qfline_T *qfp, pos_T *pos, పూర్ణాంకం linewise)
{
    ఒకవేళ (linewise)
	తిరిగి qfp->qf_lnum < pos->lnum;
    లేకపోతే
	తిరిగి (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_నిలువు < pos->నిలువు));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    స్థిర పూర్ణాంకం
qf_నమోదు_on_or_after_pos(qfline_T *qfp, pos_T *pos, పూర్ణాంకం linewise)
{
    ఒకవేళ (linewise)
	తిరిగి qfp->qf_lnum >= pos->lnum;
    లేకపోతే
	తిరిగి (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_నిలువు >= pos->నిలువు));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    స్థిర పూర్ణాంకం
qf_నమోదు_on_or_before_pos(qfline_T *qfp, pos_T *pos, పూర్ణాంకం linewise)
{
    ఒకవేళ (linewise)
	తిరిగి qfp->qf_lnum <= pos->lnum;
    లేకపోతే
	తిరిగి (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_నిలువు <= pos->నిలువు));
}

/*
 * Find the first quickfix entry after position 'pos' in buffer 'bnr'.
 * If 'linewise' is TRUE, returns the entry after the specified line and treats
 * multiple entries on a single line as one. Otherwise returns the entry after
 * the specified line and column.
 * 'qfp' points to the very first entry in the buffer and 'errornr' is the
 * index of the very first entry in the quickfix list.
 * Returns NULL if an entry is not found after 'pos'.
 */
    స్థిర qfline_T *
qf_కనుగొను_నమోదు_after_pos(
	పూర్ణాంకం		bnr,
	pos_T		*pos,
	పూర్ణాంకం		linewise,
	qfline_T	*qfp,
	పూర్ణాంకం		*errornr)
{
    ఒకవేళ (qf_నమోదు_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	తిరిగి qfp;

    // Find the entry just before or at the position 'pos'
    అయితే (qfp->qf_తదుపరి != ఖాళీ
	    && qfp->qf_తదుపరి->qf_fnum == bnr
	    && qf_నమోదు_on_or_before_pos(qfp->qf_తదుపరి, pos, linewise))
    {
	qfp = qfp->qf_తదుపరి;
	++*errornr;
    }

    ఒకవేళ (qfp->qf_తదుపరి == ఖాళీ || qfp->qf_తదుపరి->qf_fnum != bnr)
	// No entries found after position 'pos'
	తిరిగి ఖాళీ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_తదుపరి;
    ++*errornr;

    తిరిగి qfp;
}

/*
 * Find the first quickfix entry before position 'pos' in buffer 'bnr'.
 * If 'linewise' is TRUE, returns the entry before the specified line and
 * treats multiple entries on a single line as one. Otherwise returns the entry
 * before the specified line and column.
 * 'qfp' points to the very first entry in the buffer and 'errornr' is the
 * index of the very first entry in the quickfix list.
 * Returns NULL if an entry is not found before 'pos'.
 */
    స్థిర qfline_T *
qf_కనుగొను_నమోదు_before_pos(
	పూర్ణాంకం		bnr,
	pos_T		*pos,
	పూర్ణాంకం		linewise,
	qfline_T	*qfp,
	పూర్ణాంకం		*errornr)
{
    // Find the entry just before the position 'pos'
    అయితే (qfp->qf_తదుపరి != ఖాళీ
	    && qfp->qf_తదుపరి->qf_fnum == bnr
	    && qf_నమోదు_before_pos(qfp->qf_తదుపరి, pos, linewise))
    {
	qfp = qfp->qf_తదుపరి;
	++*errornr;
    }

    ఒకవేళ (qf_నమోదు_on_or_after_pos(qfp, pos, linewise))
	తిరిగి ఖాళీ;

    ఒకవేళ (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_కనుగొను_మొదటి_నమోదు_on_పంక్తి(qfp, errornr);

    తిరిగి qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    స్థిర qfline_T *
qf_కనుగొను_closest_నమోదు(
	qf_list_T	*qfl,
	పూర్ణాంకం		bnr,
	pos_T		*pos,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		linewise,
	పూర్ణాంకం		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_కనుగొను_మొదటి_నమోదు_లోపల_buf(qfl, bnr, errornr);
    ఒకవేళ (qfp == ఖాళీ)
	తిరిగి ఖాళీ;		// no entry in this file

    ఒకవేళ (dir == FORWARD)
	qfp = qf_కనుగొను_నమోదు_after_pos(bnr, pos, linewise, qfp, errornr);
    లేకపోతే
	qfp = qf_కనుగొను_నమోదు_before_pos(bnr, pos, linewise, qfp, errornr);

    తిరిగి qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    స్థిర శూన్య
qf_get_nth_క్రింద_నమోదు(qfline_T *నమోదు_arg, పూర్ణాంకం n, పూర్ణాంకం linewise, పూర్ణాంకం *errornr)
{
    qfline_T *నమోదు_2 = నమోదు_arg;

    అయితే (n-- > 0 && !got_int)
    {
	పూర్ణాంకం		మొదటి_errornr = *errornr;

	ఒకవేళ (linewise)
	    // Treat all the entries on the same line in this file as one
	    నమోదు_2 = qf_కనుగొను_చివరి_నమోదు_on_పంక్తి(నమోదు_2, errornr);

	ఒకవేళ (నమోదు_2->qf_తదుపరి == ఖాళీ
		|| నమోదు_2->qf_తదుపరి->qf_fnum != నమోదు_2->qf_fnum)
	{
	    ఒకవేళ (linewise)
		*errornr = మొదటి_errornr;
	    ఆపు;
	}

	నమోదు_2 = నమోదు_2->qf_తదుపరి;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    స్థిర శూన్య
qf_get_nth_above_నమోదు(qfline_T *నమోదు_2, పూర్ణాంకం n, పూర్ణాంకం linewise, పూర్ణాంకం *errornr)
{
    అయితే (n-- > 0 && !got_int)
    {
	ఒకవేళ (నమోదు_2->qf_prev == ఖాళీ
		|| నమోదు_2->qf_prev->qf_fnum != నమోదు_2->qf_fnum)
	    ఆపు;

	నమోదు_2 = నమోదు_2->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	ఒకవేళ (linewise)
	    నమోదు_2 = qf_కనుగొను_మొదటి_నమోదు_on_పంక్తి(నమోదు_2, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    స్థిర పూర్ణాంకం
qf_కనుగొను_nth_adj_నమోదు(
	qf_list_T	*qfl,
	పూర్ణాంకం		bnr,
	pos_T		*pos,
	పూర్ణాంకం		n,
	పూర్ణాంకం		dir,
	పూర్ణాంకం		linewise)
{
    qfline_T	*adj_నమోదు;
    పూర్ణాంకం		errornr;

    // Find an entry closest to the specified position
    adj_నమోదు = qf_కనుగొను_closest_నమోదు(qfl, bnr, pos, dir, linewise, &errornr);
    ఒకవేళ (adj_నమోదు == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	ఒకవేళ (dir == FORWARD)
	    qf_get_nth_క్రింద_నమోదు(adj_నమోదు, n, linewise, &errornr);
	లేకపోతే
	    qf_get_nth_above_నమోదు(adj_నమోదు, n, linewise, &errornr);
    }

    తిరిగి errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    శూన్య
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    పూర్ణాంకం		dir;
    పూర్ణాంకం		buf_has_flag;
    పూర్ణాంకం		errornr = 0;
    pos_T	pos;

    ఒకవేళ (eap->addr_లెక్క > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	తిరిగి;
    }

    // Check whether the current buffer has any quickfix entries
    ఒకవేళ (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_నమోదు;
    లేకపోతే
	buf_has_flag = BUF_HAS_LL_నమోదు;
    ఒకవేళ (!(curbuf->b_has_qf_నమోదు & buf_has_flag))
    {
	emsg(_(e_no_errors));
	తిరిగి;
    }

    ఒకవేళ ((qi = qf_cmd_get_దొంతర(eap, TRUE)) == ఖాళీ)
	తిరిగి;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    ఒకవేళ (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	తిరిగి;
    }

    ఒకవేళ (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    లేకపోతే
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.నిలువు++;
    errornr = qf_కనుగొను_nth_adj_నమోదు(qfl, curbuf->b_fnum, &pos,
				eap->addr_లెక్క > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    ఒకవేళ (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    లేకపోతే
	emsg(_(e_no_మరిన్ని_వస్తువులు));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    స్థిర char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_cfile:	    తిరిగి (char_u *)"cfile";
	స్థితి CMD_cgetfile:  తిరిగి (char_u *)"cgetfile";
	స్థితి CMD_caddfile:  తిరిగి (char_u *)"caddfile";
	స్థితి CMD_lfile:	    తిరిగి (char_u *)"lfile";
	స్థితి CMD_lgetfile:  తిరిగి (char_u *)"lgetfile";
	స్థితి CMD_laddfile:  తిరిగి (char_u *)"laddfile";
	అప్రమేయ:	    తిరిగి ఖాళీ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    శూన్య
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ఖాళీ;
    win_T	*wp = ఖాళీ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ఖాళీ;
    int_u	save_qfid = 0;		// init for gcc
    పూర్ణాంకం		res;

    au_name = cfile_get_auname(eap->cmdidx);
    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ఖాళీ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	ఒకవేళ (aborting())
	    తిరిగి;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ఖాళీ, ఖాళీ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ఖాళీ);
	ఒకవేళ (browse_file == ఖాళీ)
	    తిరిగి;
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    లేకపోతే
# endif
    ఒకవేళ (*eap->arg != NUL)
	సమితి_string_option_ప్రత్యక్ష((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    ఒకవేళ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    incr_quickfix_busy();

    // This function is used by the :cfile, :cgetfile and :caddfile
    // commands.
    // :cfile always creates a new quickfix list and may jump to the
    // first error.
    // :cgetfile creates a new quickfix list but doesn't jump to the
    // first error.
    // :caddfile adds to an existing quickfix list. If there is no
    // quickfix list then a new list is created.
    res = qf_init(wp, p_ef, p_efm, (eap->cmdidx != CMD_caddfile
			&& eap->cmdidx != CMD_laddfile),
			qf_cmdtitle(*eap->cmdlinep), enc);
    ఒకవేళ (wp != ఖాళీ)
    {
	qi = GET_LOC_LIST(wp);
	ఒకవేళ (qi == ఖాళీ)
	{
	    decr_quickfix_busy();
	    తిరిగి;
	}
    }
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
    {
	decr_quickfix_busy();
	తిరిగి;
    }
    ఒకవేళ (res >= 0)
	qf_list_మారింది(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    ఒకవేళ (au_name != ఖాళీ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ఖాళీ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ఒకవేళ (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_మొదటి(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    స్థిర char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_vimgrep:     తిరిగి (char_u *)"vimgrep";
	స్థితి CMD_lvimgrep:    తిరిగి (char_u *)"lvimgrep";
	స్థితి CMD_vimgrepadd:  తిరిగి (char_u *)"vimgrepadd";
	స్థితి CMD_lvimgrepadd: తిరిగి (char_u *)"lvimgrepadd";
	స్థితి CMD_grep:	      తిరిగి (char_u *)"grep";
	స్థితి CMD_lgrep:	      తిరిగి (char_u *)"lgrep";
	స్థితి CMD_grepadd:     తిరిగి (char_u *)"grepadd";
	స్థితి CMD_lgrepadd:    తిరిగి (char_u *)"lgrepadd";
	అప్రమేయ: తిరిగి ఖాళీ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    స్థిర శూన్య
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ఖాళీ;

    ఒకవేళ (s == ఖాళీ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	ఒకవేళ (చివరి_శోధన_pat() == ఖాళీ)
	{
	    emsg(_(e_no_మునుపటి_regular_వ్యక్తీకరణ));
	    తిరిగి;
	}
	regmatch->regprog = vim_regcomp(చివరి_శోధన_pat(), RE_MAGIC);
    }
    లేకపోతే
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    స్థిర శూన్య
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    సందేశం_ప్రారంభం();
    p = సందేశం_strtrunc(fname, TRUE);
    ఒకవేళ (p == ఖాళీ)
	సందేశం_outtrans(fname);
    లేకపోతే
    {
	సందేశం_outtrans(p);
	vim_free(p);
    }
    సందేశం_clr_eos();
    సందేశం_didout = FALSE;	    // overwrite this message
    సందేశం_nowait = TRUE;	    // don't wait for this message
    సందేశం_నిలువు = 0;
    బహిర్గతం_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    స్థిర buf_T *
vgr_ఎక్కించు_dummy_buf(
	char_u *fname,
	char_u *dirname_ప్రారంభం,
	char_u *dirname_now)
{
    పూర్ణాంకం		save_mls;
# ఒకవేళ defined(FEAT_SYN_HL)
    char_u	*save_ei = ఖాళీ;
# endif
    buf_T	*buf;

# ఒకవేళ defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = ఎక్కించు_dummy_బఫరు(fname, dirname_ప్రారంభం, dirname_now);

    p_mls = save_mls;
# ఒకవేళ defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    తిరిగి buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    స్థిర పూర్ణాంకం
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    ఒకవేళ (!qflist_valid(wp, qfid))
    {
	ఒకవేళ (wp != ఖాళీ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_మారింది));
	    తిరిగి FALSE;
	}
	లేకపోతే
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    తిరిగి TRUE;
	}
    }

    ఒకవేళ (qf_restore_list(qi, qfid) == FAIL)
	తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    స్థిర పూర్ణాంకం
vgr_సరిపోలిక_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	దీర్ఘ	    *tomatch,
	పూర్ణాంకం	    duplicate_name,
	పూర్ణాంకం	    flags)
{
    పూర్ణాంకం		దొరికింది_సరిపోలిక = FALSE;
    దీర్ఘ	lnum;
    colnr_T	నిలువు;
    పూర్ణాంకం		pat_len = (పూర్ణాంకం)STRLEN(spat);
    ఒకవేళ (pat_len > FUZZY_సరిపోలిక_MAX_LEN)
	pat_len = FUZZY_సరిపోలిక_MAX_LEN;

    కొరకు (lnum = 1; lnum <= buf->b_ml.ml_పంక్తి_లెక్క && *tomatch > 0; ++lnum)
    {
	నిలువు = 0;
	ఒకవేళ (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    అయితే (vim_regexec_multi(regmatch, curwin, buf, lnum,
								నిలువు, ఖాళీ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ఒకవేళ (qf_కూడు_నమోదు(qfl,
			    ఖాళీ,	// dir
			    fname,
			    ఖాళీ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].నిలువు + 1,
			    regmatch->endpos[0].నిలువు + 1,
			    FALSE,	// vis_col
			    ఖాళీ,	// search pattern
			    0,		// nr
			    0,		// type
			    ఖాళీ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ఆపు;
		}
		దొరికింది_సరిపోలిక = TRUE;
		ఒకవేళ (--*tomatch == 0)
		    ఆపు;
		ఒకవేళ ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ఆపు;
		నిలువు = regmatch->endpos[0].నిలువు
		    + (నిలువు == regmatch->endpos[0].నిలువు);
		ఒకవేళ (నిలువు > ml_get_buf_len(buf, lnum))
		    ఆపు;
	    }
	}
	లేకపోతే
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    పూర్ణాంకం	    score;
	    int_u   matches[FUZZY_సరిపోలిక_MAX_LEN];
	    int_u   sz = ARRAY_నిడివి(matches);

	    // Fuzzy string match
	    CLEAR_క్షేత్రం(matches);
	    అయితే (fuzzy_సరిపోలిక(str + నిలువు, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ఒకవేళ (qf_కూడు_నమోదు(qfl,
			    ఖాళీ,	// dir
			    fname,
			    ఖాళీ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + నిలువు + 1,
			    0,
			    FALSE,	// vis_col
			    ఖాళీ,	// search pattern
			    0,		// nr
			    0,		// type
			    ఖాళీ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ఆపు;
		}
		దొరికింది_సరిపోలిక = TRUE;
		ఒకవేళ (--*tomatch == 0)
		    ఆపు;
		ఒకవేళ ((flags & VGR_GLOBAL) == 0)
		    ఆపు;
		నిలువు = matches[pat_len - 1] + నిలువు + 1;
		ఒకవేళ (నిలువు > linelen)
		    ఆపు;
	    }
	}
	పంక్తి_breakcheck();
	ఒకవేళ (got_int)
	    ఆపు;
    }

    తిరిగి దొరికింది_సరిపోలిక;
}

/*
 * Jump to the first match and update the directory.
 */
    స్థిర శూన్య
vgr_jump_గమ్యం_సరిపోలిక(
	qf_info_T   *qi,
	పూర్ణాంకం	    forceit,
	పూర్ణాంకం	    *redraw_for_dummy,
	buf_T	    *మొదటి_సరిపోలిక_buf,
	char_u	    *లక్ష్యం_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    ఒకవేళ (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    ఒకవేళ (curbuf == మొదటి_సరిపోలిక_buf && లక్ష్యం_dir != ఖాళీ)
    {
	exarg_T ea;

	CLEAR_క్షేత్రం(ea);
	ea.arg = లక్ష్యం_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
రకనిర్వచనం నిర్మాణం
{
    దీర్ఘ	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    పూర్ణాంకం		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    పూర్ణాంకం		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_వాదనలు_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    స్థిర పూర్ణాంకం
vgr_process_వాదనలు(
	exarg_T		*eap,
	vgr_వాదనలు_T	*వాదనలు)
{
    char_u	*p;

    CLEAR_సూచిక(వాదనలు);

    వాదనలు->regmatch.regprog = ఖాళీ;
    వాదనలు->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    ఒకవేళ (eap->addr_లెక్క > 0)
	వాదనలు->tomatch = eap->line2;
    లేకపోతే
	వాదనలు->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &వాదనలు->spat, &వాదనలు->flags);
    ఒకవేళ (p == ఖాళీ)
    {
	emsg(_(e_invalid_శోధన_నమూనా_or_delimiter));
	తిరిగి FAIL;
    }

    vgr_init_regmatch(&వాదనలు->regmatch, వాదనలు->spat);
    ఒకవేళ (వాదనలు->regmatch.regprog == ఖాళీ)
	తిరిగి FAIL;

    p = skipwhite(p);
    ఒకవేళ (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_నమూనా));
	తిరిగి FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    ఒకవేళ ((get_arglist_exp(p, &వాదనలు->fcount, &వాదనలు->fnames, TRUE) == FAIL) ||
	వాదనలు->fcount == 0)
    {
	emsg(_(e_no_సరిపోలిక));
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    స్థిర పూర్ణాంకం
existing_swapfile(buf_T *buf)
{
    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ && buf->b_ml.ml_mfp->mf_fname != ఖాళీ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	తిరిగి fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    తిరిగి FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    స్థిర పూర్ణాంకం
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_వాదనలు_T	*cmd_వాదనలు,
	పూర్ణాంకం		*redraw_for_dummy,
	buf_T		**మొదటి_సరిపోలిక_buf,
	char_u		**లక్ష్యం_dir)
{
    పూర్ణాంకం		స్థితి_2 = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    పూర్ణాంకం		fi;
    buf_T	*buf;
    పూర్ణాంకం		duplicate_name = FALSE;
    పూర్ణాంకం		using_dummy;
    char_u	*dirname_ప్రారంభం = ఖాళీ;
    char_u	*dirname_now = ఖాళీ;
    పూర్ణాంకం		దొరికింది_సరిపోలిక;
    aco_save_T	aco;

    dirname_ప్రారంభం = alloc_id(MAXPATHL, aid_qf_dirname_ప్రారంభం);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    ఒకవేళ (dirname_ప్రారంభం == ఖాళీ || dirname_now == ఖాళీ)
	వెళ్ళు theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_ప్రారంభం, MAXPATHL);

    seconds = (time_t)0;
    కొరకు (fi = 0; fi < cmd_వాదనలు->fcount && !got_int && cmd_వాదనలు->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_వాదనలు->fnames[fi]);
	ఒకవేళ (సమయం(ఖాళీ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = సమయం(ఖాళీ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_వాదనలు->fnames[fi]);
	ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ఖాళీ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_ఎక్కించు_dummy_buf(fname, dirname_ప్రారంభం, dirname_now);
	}
	లేకపోతే
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	ఒకవేళ (!vgr_qflist_valid(wp, qi, save_qfid, cmd_వాదనలు->qf_title))
	    వెళ్ళు theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	ఒకవేళ (buf == ఖాళీ)
	{
	    ఒకవేళ (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	లేకపోతే
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    దొరికింది_సరిపోలిక = vgr_సరిపోలిక_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_వాదనలు->spat, &cmd_వాదనలు->regmatch,
		    &cmd_వాదనలు->tomatch, duplicate_name, cmd_వాదనలు->flags);

	    ఒకవేళ (using_dummy)
	    {
		ఒకవేళ (దొరికింది_సరిపోలిక && *మొదటి_సరిపోలిక_buf == ఖాళీ)
		    *మొదటి_సరిపోలిక_buf = buf;
		ఒకవేళ (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_బఫరు(buf, dirname_ప్రారంభం);
		    buf = ఖాళీ;
		}
		లేకపోతే ఒకవేళ ((cmdmod.cmod_flags & CMOD_HIDE) == 0
			    || buf->b_p_bh[0] == 'u'	// "unload"
			    || buf->b_p_bh[0] == 'w'	// "wipe"
			    || buf->b_p_bh[0] == 'd')	// "delete"
		{
		    // When no match was found we don't need to remember the
		    // buffer, wipe it out.  If there was a match and it
		    // wasn't the first one or we won't jump there: only
		    // unload the buffer.
		    // Ignore 'hidden' here, because it may lead to having too
		    // many swap files.
		    ఒకవేళ (!దొరికింది_సరిపోలిక)
		    {
			wipe_dummy_బఫరు(buf, dirname_ప్రారంభం);
			buf = ఖాళీ;
		    }
		    లేకపోతే ఒకవేళ (buf != *మొదటి_సరిపోలిక_buf
					|| (cmd_వాదనలు->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_బఫరు(buf, dirname_ప్రారంభం);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ఖాళీ;
		    }
		}

		ఒకవేళ (buf != ఖాళీ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    ఒకవేళ (buf == *మొదటి_సరిపోలిక_buf
			    && *లక్ష్యం_dir == ఖాళీ
			    && STRCMP(dirname_ప్రారంభం, dirname_now) != 0)
			*లక్ష్యం_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    ఒకవేళ (curbuf == buf)
		    {
# ఒకవేళ defined(FEAT_SYN_HL)
			apply_autocmds(EVENT_FILETYPE, buf->b_p_ft,
						     buf->b_fname, TRUE, buf);
# endif
			do_modelines(OPT_NOWIN);
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

    స్థితి_2 = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_ప్రారంభం);
    తిరిగి స్థితి_2;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    శూన్య
ex_vimgrep(exarg_T *eap)
{
    vgr_వాదనలు_T	వాదనలు;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ఖాళీ;
    పూర్ణాంకం		redraw_for_dummy = FALSE;
    buf_T	*మొదటి_సరిపోలిక_buf = ఖాళీ;
    char_u	*లక్ష్యం_dir = ఖాళీ;
    char_u	*au_name =  ఖాళీ;
    పూర్ణాంకం		స్థితి_2;

    ఒకవేళ (!check_can_సమితి_curbuf_forceit(eap->forceit))
	తిరిగి;

    au_name = vgr_get_auname(eap->cmdidx);
    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ఒకవేళ (aborting())
	    తిరిగి;
# endif
    }

    qi = qf_cmd_get_or_alloc_దొంతర(eap, &wp);
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి;

    ఒకవేళ (vgr_process_వాదనలు(eap, &వాదనలు) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_దొంతర_ఖాళీలు(qi))
	// make place for a new list
	qf_new_list(qi, వాదనలు.qf_title);

    incr_quickfix_busy();

    స్థితి_2 = vgr_process_files(wp, qi, &వాదనలు, &redraw_for_dummy,
						&మొదటి_సరిపోలిక_buf, &లక్ష్యం_dir);
    ఒకవేళ (స్థితి_2 != OK)
    {
	FreeWild(వాదనలు.fcount, వాదనలు.fnames);
	decr_quickfix_busy();
	వెళ్ళు theend;
    }

    FreeWild(వాదనలు.fcount, వాదనలు.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_ప్రారంభం;
    qfl->qf_సూచీ = 1;
    qf_list_మారింది(qfl);

    qf_update_బఫరు(qi, ఖాళీ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    ఒకవేళ (au_name != ఖాళీ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    ఒకవేళ (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	వెళ్ళు theend;
    }

    // Jump to first match.
    ఒకవేళ (!qf_list_ఖాళీలు(qf_get_curlist(qi)))
    {
	ఒకవేళ ((వాదనలు.flags & VGR_NOJUMP) == 0)
	    vgr_jump_గమ్యం_సరిపోలిక(qi, eap->forceit, &redraw_for_dummy,
		    మొదటి_సరిపోలిక_buf, లక్ష్యం_dir);
    }
    లేకపోతే
	semsg(_(e_no_సరిపోలిక_str_2), వాదనలు.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    ఒకవేళ (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# లేకపోతే
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(వాదనలు.qf_title);
    vim_free(లక్ష్యం_dir);
    vim_regfree(వాదనలు.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    స్థిర శూన్య
restore_ప్రారంభం_dir(char_u *dirname_ప్రారంభం)
{
    char_u *dirname_now = alloc(MAXPATHL);

    ఒకవేళ (dirname_now == ఖాళీ)
	తిరిగి;

    mch_dirname(dirname_now, MAXPATHL);
    ఒకవేళ (STRCMP(dirname_ప్రారంభం, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_క్షేత్రం(ea);
	ea.arg = dirname_ప్రారంభం;
	ea.cmdidx = (curwin->w_localdir == ఖాళీ) ? CMD_cd : CMD_lcd;
	ex_cd(&ea);
    }
    vim_free(dirname_now);
}

/*
 * Load file "fname" into a dummy buffer and return the buffer pointer,
 * placing the directory resulting from the buffer load into the
 * "resulting_dir" pointer. "resulting_dir" must be allocated by the caller
 * prior to calling this function. Restores directory to "dirname_start" prior
 * to returning, if autocmds or the 'autochdir' option have changed it.
 *
 * If creating the dummy buffer does not fail, must call unload_dummy_buffer()
 * or wipe_dummy_buffer() later!
 *
 * Returns NULL if it fails.
 */
    స్థిర buf_T *
ఎక్కించు_dummy_బఫరు(
    char_u	*fname,
    char_u	*dirname_ప్రారంభం,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_గమ్యం_wipe;
    పూర్ణాంకం		failed = TRUE;
    aco_save_T	aco;
    పూర్ణాంకం		readfile_ఫలితం;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ఖాళీ, ఖాళీ, (linenr_T)1, BLN_DUMMY);
    ఒకవేళ (newbuf == ఖాళీ)
	తిరిగి ఖాళీ;
    సమితి_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    ఒకవేళ (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	ఒకవేళ (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (శూన్య)setfname(curbuf, fname, ఖాళీ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_మార్పిడి(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_గమ్యం_wipe.br_buf = ఖాళీ;
	    readfile_ఫలితం = readfile(fname, ఖాళీ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ఖాళీ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    ఒకవేళ (readfile_ఫలితం == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		ఒకవేళ (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    సమితి_bufref(&newbuf_గమ్యం_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    ఒకవేళ (newbuf_గమ్యం_wipe.br_buf != ఖాళీ && bufref_valid(&newbuf_గమ్యం_wipe))
	    {
		ఖండం_autocmds();
		wipe_dummy_బఫరు(newbuf_గమ్యం_wipe.br_buf, ఖాళీ);
		unblock_autocmds();
	    }
	}

	// Add back the "dummy" flag, otherwise buflist_findname_stat() won't
	// skip it.
	newbuf->b_flags |= BF_DUMMY;
    }

    // When autocommands/'autochdir' option changed directory: go back.
    // Let the caller know what the resulting dir was first, in case it is
    // important.
    mch_dirname(resulting_dir, MAXPATHL);
    restore_ప్రారంభం_dir(dirname_ప్రారంభం);

    ఒకవేళ (!bufref_valid(&newbufref))
	తిరిగి ఖాళీ;
    ఒకవేళ (failed)
    {
	wipe_dummy_బఫరు(newbuf, dirname_ప్రారంభం);
	తిరిగి ఖాళీ;
    }
    తిరిగి newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    స్థిర శూన్య
wipe_dummy_బఫరు(buf_T *buf, char_u *dirname_ప్రారంభం)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    అయితే (buf->b_nwindows > 0)
    {
	పూర్ణాంకం	    did_one = FALSE;
	win_T	    *wp;

	ఒకవేళ (firstwin->w_తదుపరి != ఖాళీ)
	    FOR_ALL_WINDOWS(wp)
		ఒకవేళ (wp->w_బఫరు == buf)
		{
		    ఒకవేళ (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ఆపు;
		}
	ఒకవేళ (!did_one)
	    వెళ్ళు fail;
    }

    ఒకవేళ (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# ఒకవేళ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_బఫరు(buf, TRUE);

# ఒకవేళ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	ఒకవేళ (dirname_ప్రారంభం != ఖాళీ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_ప్రారంభం_dir(dirname_ప్రారంభం);

	తిరిగి;
    }

fail:
    // Keeping the buffer, remove the dummy flag.
    buf->b_flags &= ~BF_DUMMY;
}

/*
 * Unload the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" prior to returning, if autocmds or the
 * 'autochdir' option have changed it.
 */
    స్థిర శూన్య
unload_dummy_బఫరు(buf_T *buf, char_u *dirname_ప్రారంభం)
{
    ఒకవేళ (curbuf == buf)		// safety check
	తిరిగి;

    close_బఫరు(ఖాళీ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_ప్రారంభం_dir(dirname_ప్రారంభం);
}

# ఒకవేళ defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    స్థిర పూర్ణాంకం
get_qfline_వస్తువులు(qfline_T *qfp, list_T *list)
{
    పూర్ణాంకం		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    ఒకవేళ (bufnum != 0 && (buflist_findnr(bufnum) == ఖాళీ))
	bufnum = 0;

    ఒకవేళ ((dict = dict_alloc()) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	తిరిగి FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ఒకవేళ (dict_కూడు_సంఖ్య(dict, "bufnr", (దీర్ఘ)bufnum) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "lnum",     (దీర్ఘ)qfp->qf_lnum) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "end_lnum", (దీర్ఘ)qfp->qf_చివర_lnum) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "col",      (దీర్ఘ)qfp->qf_నిలువు) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "end_col",  (దీర్ఘ)qfp->qf_చివర_నిలువు) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "vcol",     (దీర్ఘ)qfp->qf_viscol) == FAIL
	    || dict_కూడు_సంఖ్య(dict, "nr",       (దీర్ఘ)qfp->qf_nr) == FAIL
	    || dict_కూడు_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_కూడు_string(dict, "pattern", qfp->qf_నమూనా) == FAIL
	    || dict_కూడు_string(dict, "text", qfp->qf_పాఠ్యం) == FAIL
	    || dict_కూడు_string_len(dict, "type", buf, (పూర్ణాంకం)buflen) == FAIL
	    || (qfp->qf_user_దత్తాంశం.v_type != VAR_UNKNOWN
		&& dict_కూడు_tv(dict, "user_data", &qfp->qf_user_దత్తాంశం) == FAIL )
	    || dict_కూడు_సంఖ్య(dict, "valid", (దీర్ఘ)qfp->qf_valid) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    స్థిర పూర్ణాంకం
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	పూర్ణాంకం		qf_idx,
	పూర్ణాంకం		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    పూర్ణాంకం		i;

    ఒకవేళ (qi == ఖాళీ)
    {
	qi = ql_info;
	ఒకవేళ (wp != ఖాళీ)
	    qi = GET_LOC_LIST(wp);
	ఒకవేళ (qi == ఖాళీ)
	    తిరిగి FAIL;

    }

    ఒకవేళ (eidx < 0)
	తిరిగి OK;

    ఒకవేళ (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    ఒకవేళ (qf_idx >= qi->qf_listcount)
	తిరిగి FAIL;

    qfl = qf_get_list(qi, qf_idx);
    ఒకవేళ (qf_list_ఖాళీలు(qfl))
	తిరిగి FAIL;

    FOR_ALL_QFL_వస్తువులు(qfl, qfp, i)
    {
	ఒకవేళ (eidx > 0)
	{
	    ఒకవేళ (eidx == i)
		తిరిగి get_qfline_వస్తువులు(qfp, list);
	}
	లేకపోతే ఒకవేళ (get_qfline_వస్తువులు(qfp, list) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
గణన {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_వస్తువులు	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_కొలత	= 0x80,
    QF_GETLIST_TICK	= 0x100,
    QF_GETLIST_FILEWINID	= 0x200,
    QF_GETLIST_QFBUFNR	= 0x400,
    QF_GETLIST_QFTF	= 0x800,
    QF_GETLIST_ALL	= 0xFFF,
};

/*
 * Parse text from 'di' and return the quickfix list items.
 * Existing quickfix lists are not modified.
 */
    స్థిర పూర్ణాంకం
qf_get_list_from_పంక్తులు(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    పూర్ణాంకం		స్థితి_2 = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    ఒకవేళ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ఖాళీ)
	తిరిగి FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    ఒకవేళ ((efm_di = dict_కనుగొను(what, (char_u *)"efm", -1)) != ఖాళీ)
    {
	ఒకవేళ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ఖాళీ)
	    తిరిగి FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి FAIL;

    qi = qf_alloc_దొంతర(QFLT_అంతర్గత, 1);
    ఒకవేళ (qi != ఖాళీ)
    {
	ఒకవేళ (qf_init_ext(qi, 0, ఖాళీ, ఖాళీ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ఖాళీ, ఖాళీ) > 0)
	{
	    (శూన్య)get_errorlist(qi, ఖాళీ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_కూడు_list(retdict, "items", l);
    స్థితి_2 = OK;

    తిరిగి స్థితి_2;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    స్థిర పూర్ణాంకం
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి 0;
    win = qf_కనుగొను_win(qi);
    ఒకవేళ (win != ఖాళీ)
	తిరిగి win->w_id;
    తిరిగి 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    స్థిర పూర్ణాంకం
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    పూర్ణాంకం	bufnum = 0;

    ఒకవేళ (qi != ఖాళీ && buflist_findnr(qi->qf_bufnr) != ఖాళీ)
	bufnum = qi->qf_bufnr;

    తిరిగి dict_కూడు_సంఖ్య(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    స్థిర పూర్ణాంకం
qf_getprop_keys2flags(dict_T *what, పూర్ణాంకం loclist)
{
    పూర్ణాంకం		flags = QF_GETLIST_NONE;

    ఒకవేళ (dict_has_కీలకం(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	ఒకవేళ (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    ఒకవేళ (dict_has_కీలకం(what, "title"))
	flags |= QF_GETLIST_TITLE;

    ఒకవేళ (dict_has_కీలకం(what, "nr"))
	flags |= QF_GETLIST_NR;

    ఒకవేళ (dict_has_కీలకం(what, "winid"))
	flags |= QF_GETLIST_WINID;

    ఒకవేళ (dict_has_కీలకం(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    ఒకవేళ (dict_has_కీలకం(what, "id"))
	flags |= QF_GETLIST_ID;

    ఒకవేళ (dict_has_కీలకం(what, "items"))
	flags |= QF_GETLIST_వస్తువులు;

    ఒకవేళ (dict_has_కీలకం(what, "idx"))
	flags |= QF_GETLIST_IDX;

    ఒకవేళ (dict_has_కీలకం(what, "size"))
	flags |= QF_GETLIST_కొలత;

    ఒకవేళ (dict_has_కీలకం(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    ఒకవేళ (loclist && dict_has_కీలకం(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    ఒకవేళ (dict_has_కీలకం(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    ఒకవేళ (dict_has_కీలకం(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    తిరిగి flags;
}

/*
 * Return the quickfix list index based on 'nr' or 'id' in 'what'.
 * If 'nr' and 'id' are not present in 'what' then return the current
 * quickfix list index.
 * If 'nr' is zero then return the current quickfix list index.
 * If 'nr' is '$' then return the last quickfix list index.
 * If 'id' is present then return the index of the quickfix list with that id.
 * If 'id' is zero then return the quickfix list index specified by 'nr'.
 * Return -1, if quickfix list is not present or if the stack is empty.
 */
    స్థిర పూర్ణాంకం
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    పూర్ణాంకం		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"nr", -1)) != ఖాళీ)
    {
	// Use the specified quickfix/location list
	ఒకవేళ (di->di_tv.v_type == VAR_సంఖ్య)
	{
	    // for zero use the current list
	    ఒకవేళ (di->di_tv.vval.v_సంఖ్య != 0)
	    {
		qf_idx = di->di_tv.vval.v_సంఖ్య - 1;
		ఒకవేళ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ఖాళీ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	లేకపోతే
	    qf_idx = INVALID_QFIDX;
    }

    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"id", -1)) != ఖాళీ)
    {
	// Look for a list with the specified id
	ఒకవేళ (di->di_tv.v_type == VAR_సంఖ్య)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    ఒకవేళ (di->di_tv.vval.v_సంఖ్య != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_సంఖ్య);
	}
	లేకపోతే
	    qf_idx = INVALID_QFIDX;
    }

    తిరిగి qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    స్థిర పూర్ణాంకం
qf_getprop_defaults(qf_info_T *qi, పూర్ణాంకం flags, పూర్ణాంకం locstack, dict_T *retdict)
{
    పూర్ణాంకం		స్థితి_2 = OK;

    ఒకవేళ (flags & QF_GETLIST_TITLE)
	స్థితి_2 = dict_కూడు_string_len(retdict, "title", (char_u *)"", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_వస్తువులు))
    {
	list_T	*l = list_alloc();
	ఒకవేళ (l != ఖాళీ)
	    స్థితి_2 = dict_కూడు_list(retdict, "items", l);
	లేకపోతే
	    స్థితి_2 = FAIL;
    }
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_NR))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "nr", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_WINID))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "winid", qf_winid(qi));
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	స్థితి_2 = dict_కూడు_string_len(retdict, "context", (char_u *)"", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_ID))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "id", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_IDX))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "idx", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_కొలత))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "size", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_TICK))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "changedtick", 0);
    ఒకవేళ ((స్థితి_2 == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "filewinid", 0);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	స్థితి_2 = qf_getprop_qfbufnr(qi, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_QFTF))
	స్థితి_2 = dict_కూడు_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    తిరిగి స్థితి_2;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    స్థిర పూర్ణాంకం
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    తిరిగి dict_కూడు_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    స్థిర పూర్ణాంకం
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    పూర్ణాంకం winid = 0;

    ఒకవేళ (wp != ఖాళీ && IS_LL_కిటికీ(wp))
    {
	win_T	*ll_wp = qf_కనుగొను_win_with_loclist(qi);
	ఒకవేళ (ll_wp != ఖాళీ)
	    winid = ll_wp->w_id;
    }

    తిరిగి dict_కూడు_సంఖ్య(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    స్థిర పూర్ణాంకం
qf_getprop_వస్తువులు(qf_info_T *qi, పూర్ణాంకం qf_idx, పూర్ణాంకం eidx, dict_T *retdict)
{
    పూర్ణాంకం		స్థితి_2 = OK;
    list_T	*l = list_alloc();
    ఒకవేళ (l != ఖాళీ)
    {
	(శూన్య)get_errorlist(qi, ఖాళీ, qf_idx, eidx, l);
	dict_కూడు_list(retdict, "items", l);
    }
    లేకపోతే
	స్థితి_2 = FAIL;

    తిరిగి స్థితి_2;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    స్థిర పూర్ణాంకం
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    పూర్ణాంకం		స్థితి_2;
    dictitem_T	*di;

    ఒకవేళ (qfl->qf_ctx != ఖాళీ)
    {
	di = dictitem_alloc((char_u *)"context");
	ఒకవేళ (di != ఖాళీ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    స్థితి_2 = dict_కూడు(retdict, di);
	    ఒకవేళ (స్థితి_2 == FAIL)
		dictitem_free(di);
	}
	లేకపోతే
	    స్థితి_2 = FAIL;
    }
    లేకపోతే
	స్థితి_2 = dict_కూడు_string_len(retdict, "context", (char_u *)"", 0);

    తిరిగి స్థితి_2;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    స్థిర పూర్ణాంకం
qf_getprop_idx(qf_list_T *qfl, పూర్ణాంకం eidx, dict_T *retdict)
{
    ఒకవేళ (eidx == 0)
    {
	eidx = qfl->qf_సూచీ;
	ఒకవేళ (qf_list_ఖాళీలు(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    తిరిగి dict_కూడు_సంఖ్య(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    స్థిర పూర్ణాంకం
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    పూర్ణాంకం		స్థితి_2;

    ఒకవేళ (qfl->qf_qftf_cb.cb_name != ఖాళీ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	స్థితి_2 = dict_కూడు_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    లేకపోతే
	స్థితి_2 = dict_కూడు_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    తిరిగి స్థితి_2;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    స్థిర పూర్ణాంకం
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    పూర్ణాంకం		స్థితి_2 = OK;
    పూర్ణాంకం		qf_idx = INVALID_QFIDX;
    పూర్ణాంకం		eidx = 0;
    dictitem_T	*di;
    పూర్ణాంకం		flags = QF_GETLIST_NONE;

    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"lines", -1)) != ఖాళీ)
	తిరిగి qf_get_list_from_పంక్తులు(what, di, retdict);

    ఒకవేళ (wp != ఖాళీ)
	qi = GET_LOC_LIST(wp);
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ఖాళీ));

    ఒకవేళ (!qf_దొంతర_ఖాళీలు(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi) || qf_idx == INVALID_QFIDX)
	తిరిగి qf_getprop_defaults(qi, flags, wp != ఖాళీ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"idx", -1)) != ఖాళీ)
    {
	ఒకవేళ (di->di_tv.v_type != VAR_సంఖ్య)
	    తిరిగి FAIL;
	eidx = di->di_tv.vval.v_సంఖ్య;
    }

    ఒకవేళ (flags & QF_GETLIST_TITLE)
	స్థితి_2 = qf_getprop_title(qfl, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_NR))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "nr", qf_idx + 1);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_WINID))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "winid", qf_winid(qi));
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_వస్తువులు))
	స్థితి_2 = qf_getprop_వస్తువులు(qi, qf_idx, eidx, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	స్థితి_2 = qf_getprop_ctx(qfl, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_ID))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "id", qfl->qf_id);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_IDX))
	స్థితి_2 = qf_getprop_idx(qfl, eidx, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_కొలత))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "size", qfl->qf_లెక్క);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_TICK))
	స్థితి_2 = dict_కూడు_సంఖ్య(retdict, "changedtick", qfl->qf_changedtick);
    ఒకవేళ ((స్థితి_2 == OK) && (wp != ఖాళీ) && (flags & QF_GETLIST_FILEWINID))
	స్థితి_2 = qf_getprop_filewinid(wp, qi, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	స్థితి_2 = qf_getprop_qfbufnr(qi, retdict);
    ఒకవేళ ((స్థితి_2 == OK) && (flags & QF_GETLIST_QFTF))
	స్థితి_2 = qf_getprop_qftf(qfl, retdict);

    తిరిగి స్థితి_2;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    స్థిర పూర్ణాంకం
qf_కూడు_నమోదు_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	పూర్ణాంకం		మొదటి_నమోదు,
	పూర్ణాంకం		*valid_నమోదు)
{
    స్థిర పూర్ణాంకం	did_bufnr_emsg;
    char_u	*filename, *module, *నమూనా, *పాఠ్యం, *type;
    పూర్ణాంకం		bufnum, valid, స్థితి_2, నిలువు, చివర_నిలువు, vcol, nr;
    దీర్ఘ	lnum, చివర_lnum;

    ఒకవేళ (మొదటి_నమోదు)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (పూర్ణాంకం)dict_get_సంఖ్య(d, "bufnr");
    lnum = (పూర్ణాంకం)dict_get_సంఖ్య(d, "lnum");
    చివర_lnum = (పూర్ణాంకం)dict_get_సంఖ్య(d, "end_lnum");
    నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(d, "col");
    చివర_నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(d, "end_col");
    vcol = (పూర్ణాంకం)dict_get_సంఖ్య(d, "vcol");
    nr = (పూర్ణాంకం)dict_get_సంఖ్య(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    నమూనా = dict_get_string(d, "pattern", TRUE);
    పాఠ్యం = dict_get_string(d, "text", TRUE);
    ఒకవేళ (పాఠ్యం == ఖాళీ)
	పాఠ్యం = vim_strsave((char_u *)"");
    typval_T user_దత్తాంశం;
    user_దత్తాంశం.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_దత్తాంశం);

    valid = TRUE;
    ఒకవేళ ((filename == ఖాళీ && bufnum == 0) || (lnum == 0 && నమూనా == ఖాళీ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    ఒకవేళ (bufnum != 0 && (buflist_findnr(bufnum) == ఖాళీ))
    {
	ఒకవేళ (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_బఫరు_nr_not_దొరికింది), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    ఒకవేళ (dict_has_కీలకం(d, "valid"))
	valid = (పూర్ణాంకం)dict_get_bool(d, "valid", FALSE);

    స్థితి_2 =  qf_కూడు_నమోదు(qfl,
			ఖాళీ,		// dir
			filename,
			module,
			bufnum,
			పాఠ్యం,
			lnum,
			చివర_lnum,
			నిలువు,
			చివర_నిలువు,
			vcol,		// vis_col
			నమూనా,	// search pattern
			nr,
			type == ఖాళీ ? NUL : *type,
			&user_దత్తాంశం,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(నమూనా);
    vim_free(పాఠ్యం);
    vim_free(type);
    clear_tv(&user_దత్తాంశం);

    ఒకవేళ (valid)
	*valid_నమోదు = TRUE;

    తిరిగి స్థితి_2;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    స్థిర పూర్ణాంకం
నమోదు_is_closer_గమ్యం_లక్ష్యం(
	qfline_T	*నమోదు_2,
	qfline_T	*other_నమోదు,
	పూర్ణాంకం		లక్ష్యం_fnum,
	పూర్ణాంకం		లక్ష్యం_lnum,
	పూర్ణాంకం		లక్ష్యం_నిలువు)
{
    // First, compare entries to target file.
    ఒకవేళ (!లక్ష్యం_fnum)
	// Without a target file, we can't know which is closer.
	తిరిగి FALSE;

    పూర్ణాంకం is_లక్ష్యం_file = నమోదు_2->qf_fnum && నమోదు_2->qf_fnum == లక్ష్యం_fnum;
    పూర్ణాంకం other_is_లక్ష్యం_file = other_నమోదు->qf_fnum && other_నమోదు->qf_fnum == లక్ష్యం_fnum;
    ఒకవేళ (!is_లక్ష్యం_file && other_is_లక్ష్యం_file)
	తిరిగి FALSE;
    లేకపోతే ఒకవేళ (is_లక్ష్యం_file && !other_is_లక్ష్యం_file)
	తిరిగి TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    ఒకవేళ (!లక్ష్యం_lnum)
	// Without a target line number, we can't know which is closer.
	తిరిగి FALSE;

    పూర్ణాంకం పంక్తి_దూరం = నమోదు_2->qf_lnum ? labs(నమోదు_2->qf_lnum - లక్ష్యం_lnum) : గరిష్ఠ_పూర్ణాంకం;
    పూర్ణాంకం other_పంక్తి_దూరం = other_నమోదు->qf_lnum ? labs(other_నమోదు->qf_lnum - లక్ష్యం_lnum) : గరిష్ఠ_పూర్ణాంకం;
    ఒకవేళ (పంక్తి_దూరం > other_పంక్తి_దూరం)
	తిరిగి FALSE;
    లేకపోతే ఒకవేళ (పంక్తి_దూరం < other_పంక్తి_దూరం)
	తిరిగి TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    ఒకవేళ (!లక్ష్యం_నిలువు)
	// Without a target column, we can't know which is closer.
	తిరిగి FALSE;

    పూర్ణాంకం స్తంభం_దూరం = నమోదు_2->qf_నిలువు ? నిరపేక్ష(నమోదు_2->qf_నిలువు - లక్ష్యం_నిలువు) : గరిష్ఠ_పూర్ణాంకం;
    పూర్ణాంకం other_స్తంభం_దూరం = other_నమోదు->qf_నిలువు ? నిరపేక్ష(other_నమోదు->qf_నిలువు - లక్ష్యం_నిలువు): గరిష్ఠ_పూర్ణాంకం;
    ఒకవేళ (స్తంభం_దూరం > other_స్తంభం_దూరం)
	తిరిగి FALSE;
    లేకపోతే ఒకవేళ (స్తంభం_దూరం < other_స్తంభం_దూరం)
	తిరిగి TRUE;

    // It's a complete tie! The exact same file, line, and column.
    తిరిగి FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    స్థిర పూర్ణాంకం
qf_కూడు_entries(
	qf_info_T	*qi,
	పూర్ణాంకం		qf_idx,
	list_T		*list,
	char_u		*title,
	పూర్ణాంకం		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_చివరి = ఖాళీ;
    పూర్ణాంకం		retval = OK;
    పూర్ణాంకం		valid_నమోదు = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    పూర్ణాంకం prev_fnum = 0;
    పూర్ణాంకం prev_lnum = 0;
    పూర్ణాంకం prev_నిలువు = 0;
    ఒకవేళ (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_నిలువు = qfl->qf_ptr->qf_నిలువు;
    }

    పూర్ణాంకం select_మొదటి_నమోదు = FALSE;
    పూర్ణాంకం select_సమీప_నమోదు = FALSE;

    ఒకవేళ (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_మొదటి_నమోదు = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    లేకపోతే ఒకవేళ (action == 'a')
    {
	ఒకవేళ (qf_list_ఖాళీలు(qfl))
	    // Appending to empty list, select first entry.
	    select_మొదటి_నమోదు = TRUE;
	లేకపోతే
	    // Adding to existing list, use last entry.
	    old_చివరి = qfl->qf_చివరి;
    }
    లేకపోతే ఒకవేళ (action == 'r')
    {
	select_మొదటి_నమోదు = TRUE;
	qf_free_వస్తువులు(qfl);
	qf_store_title(qfl, title);
    }
    లేకపోతే ఒకవేళ (action == 'u')
    {
	select_సమీప_నమోదు = TRUE;
	qf_free_వస్తువులు(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *నమోదు_గమ్యం_select = ఖాళీ;
    పూర్ణాంకం నమోదు_గమ్యం_select_సూచీ = 0;

    FOR_ALL_LIST_వస్తువులు(list, li)
    {
	ఒకవేళ (li->li_tv.v_type != VAR_DICT)
	    కొనసాగు; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	ఒకవేళ (d == ఖాళీ)
	    కొనసాగు;

	retval = qf_కూడు_నమోదు_from_dict(qfl, d, li == list->lv_మొదటి,
								&valid_నమోదు);
	ఒకవేళ (retval == QF_FAIL)
	    ఆపు;

	qfline_T *నమోదు_2 = qfl->qf_చివరి;
	ఒకవేళ (
	    (select_మొదటి_నమోదు && నమోదు_గమ్యం_select == ఖాళీ) ||
	    (select_సమీప_నమోదు &&
		(నమోదు_గమ్యం_select == ఖాళీ ||
		 నమోదు_is_closer_గమ్యం_లక్ష్యం(నమోదు_2, నమోదు_గమ్యం_select, prev_fnum,
					   prev_lnum, prev_నిలువు))))
	{
	    నమోదు_గమ్యం_select = నమోదు_2;
	    నమోదు_గమ్యం_select_సూచీ = qfl->qf_లెక్క;
	}
    }

    // Check if any valid error entries are added to the list.
    ఒకవేళ (valid_నమోదు)
	qfl->qf_nonevalid = FALSE;
    లేకపోతే ఒకవేళ (qfl->qf_సూచీ == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    ఒకవేళ (నమోదు_గమ్యం_select)
    {
	qfl->qf_ptr = నమోదు_గమ్యం_select;
	qfl->qf_సూచీ = నమోదు_గమ్యం_select_సూచీ;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_బఫరు(qi, old_చివరి);

    తిరిగి retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    స్థిర పూర్ణాంకం
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	పూర్ణాంకం		action,
	పూర్ణాంకం		*newlist)
{
    dictitem_T	*di;
    పూర్ణాంకం		qf_idx = qi->qf_curlist;    // default is the current list

    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"nr", -1)) != ఖాళీ)
    {
	// Use the specified quickfix/location list
	ఒకవేళ (di->di_tv.v_type == VAR_సంఖ్య)
	{
	    // for zero use the current list
	    ఒకవేళ (di->di_tv.vval.v_సంఖ్య != 0)
		qf_idx = di->di_tv.vval.v_సంఖ్య - 1;

	    ఒకవేళ ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_దొంతర_ఖాళీలు(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    లేకపోతే ఒకవేళ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		తిరిగి INVALID_QFIDX;
	    లేకపోతే ఒకవేళ (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ఖాళీ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    ఒకవేళ (!qf_దొంతర_ఖాళీలు(qi))
		qf_idx = qi->qf_listcount - 1;
	    లేకపోతే ఒకవేళ (*newlist)
		qf_idx = 0;
	    లేకపోతే
		తిరిగి INVALID_QFIDX;
	}
	లేకపోతే
	    తిరిగి INVALID_QFIDX;
    }

    ఒకవేళ (!*newlist && (di = dict_కనుగొను(what, (char_u *)"id", -1)) != ఖాళీ)
    {
	// Use the quickfix/location list with the specified id
	ఒకవేళ (di->di_tv.v_type != VAR_సంఖ్య)
	    తిరిగి INVALID_QFIDX;

	తిరిగి qf_id2nr(qi, di->di_tv.vval.v_సంఖ్య);
    }

    తిరిగి qf_idx;
}

/*
 * Set the quickfix list title.
 */
    స్థిర పూర్ణాంకం
qf_setprop_title(qf_info_T *qi, పూర్ణాంకం qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    ఒకవేళ (di->di_tv.v_type != VAR_STRING)
	తిరిగి FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    ఒకవేళ (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    తిరిగి OK;
}

/*
 * Set quickfix list items/entries.
 */
    స్థిర పూర్ణాంకం
qf_setprop_వస్తువులు(qf_info_T *qi, పూర్ణాంకం qf_idx, dictitem_T *di, పూర్ణాంకం action)
{
    పూర్ణాంకం		retval = FAIL;
    char_u	*title_save;

    ఒకవేళ (di->di_tv.v_type != VAR_LIST)
	తిరిగి FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_కూడు_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    తిరిగి retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    స్థిర పూర్ణాంకం
qf_setprop_వస్తువులు_from_పంక్తులు(
	qf_info_T	*qi,
	పూర్ణాంకం		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	పూర్ణాంకం		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    పూర్ణాంకం		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    ఒకవేళ ((efm_di = dict_కనుగొను(what, (char_u *)"efm", -1)) != ఖాళీ)
    {
	ఒకవేళ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ఖాళీ)
	    తిరిగి FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    ఒకవేళ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (action == 'r' || action == 'u')
	qf_free_వస్తువులు(&qi->qf_lists[qf_idx]);
    ఒకవేళ (qf_init_ext(qi, qf_idx, ఖాళీ, ఖాళీ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ఖాళీ, ఖాళీ) >= 0)
	retval = OK;

    తిరిగి retval;
}

/*
 * Set quickfix list context.
 */
    స్థిర పూర్ణాంకం
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    ఒకవేళ (ctx != ఖాళీ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    తిరిగి OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    స్థిర పూర్ణాంకం
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    పూర్ణాంకం		denote = FALSE;
    పూర్ణాంకం		newidx;
    పూర్ణాంకం		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    ఒకవేళ (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ఖాళీ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_లెక్క;
    లేకపోతే
    {
	// Otherwise use the specified index
	newidx = tv_get_సంఖ్య_chk(&di->di_tv, &denote);
	ఒకవేళ (denote)
	    తిరిగి FAIL;
    }

    ఒకవేళ (newidx < 1)		// sanity check
	తిరిగి FAIL;
    ఒకవేళ (newidx > qfl->qf_లెక్క)
	newidx = qfl->qf_లెక్క;

    old_qfidx = qfl->qf_సూచీ;
    qf_ptr = get_nth_నమోదు(qfl, newidx, &newidx);
    ఒకవేళ (qf_ptr == ఖాళీ)
	తిరిగి FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_సూచీ = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    ఒకవేళ (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    తిరిగి OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    స్థిర పూర్ణాంకం
qf_setprop_qftf(qf_info_T *qi వాడని, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    ఒకవేళ (cb.cb_name == ఖాళీ || *cb.cb_name == NUL)
	తిరిగి OK;
    సమితి_callback(&qfl->qf_qftf_cb, &cb);
    తిరిగి OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    స్థిర పూర్ణాంకం
qf_సమితి_properties(qf_info_T *qi, dict_T *what, పూర్ణాంకం action, char_u *title)
{
    dictitem_T	*di;
    పూర్ణాంకం		retval = FAIL;
    పూర్ణాంకం		qf_idx;
    పూర్ణాంకం		newlist = FALSE;
    qf_list_T	*qfl;

    ఒకవేళ (action == ' ' || qf_దొంతర_ఖాళీలు(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    ఒకవేళ (qf_idx == INVALID_QFIDX)	// List not found
	తిరిగి FAIL;

    ఒకవేళ (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"title", -1)) != ఖాళీ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"items", -1)) != ఖాళీ)
	retval = qf_setprop_వస్తువులు(qi, qf_idx, di, action);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"lines", -1)) != ఖాళీ)
	retval = qf_setprop_వస్తువులు_from_పంక్తులు(qi, qf_idx, what, di, action);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"context", -1)) != ఖాళీ)
	retval = qf_setprop_context(qfl, di);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"idx", -1)) != ఖాళీ)
	retval = qf_setprop_curidx(qi, qfl, di);
    ఒకవేళ ((di = dict_కనుగొను(what, (char_u *)"quickfixtextfunc", -1)) != ఖాళీ)
	retval = qf_setprop_qftf(qi, qfl, di);

    ఒకవేళ (newlist || retval == OK)
	qf_list_మారింది(qfl);
    ఒకవేళ (newlist)
	qf_update_బఫరు(qi, ఖాళీ);

    తిరిగి retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    స్థిర శూన్య
qf_free_దొంతర(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_కనుగొను_win(qi);
    win_T	*llwin = ఖాళీ;

    ఒకవేళ (qfwin != ఖాళీ)
    {
	// If the quickfix/location list window is open, then clear it
	ఒకవేళ (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_బఫరు(qi, ఖాళీ);
    }

    ఒకవేళ (wp != ఖాళీ && IS_LL_కిటికీ(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_కనుగొను_win_with_loclist(qi);
	ఒకవేళ (llwin != ఖాళీ)
	    wp = llwin;
    }

    qf_free_all(wp);
    ఒకవేళ (wp == ఖాళీ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    లేకపోతే ఒకవేళ (qfwin != ఖాళీ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_దొంతర(QFLT_LOCATION, wp->w_p_lhi);

	ఒకవేళ (new_ll != ఖాళీ)
	{
	    new_ll->qf_bufnr = qfwin->w_బఫరు->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    ఒకవేళ (wp != qfwin)
		win_సమితి_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    పూర్ణాంకం
సమితి_errorlist(
	win_T	*wp,
	list_T	*list,
	పూర్ణాంకం	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    పూర్ణాంకం		retval = OK;

    ఒకవేళ (wp != ఖాళీ)
	qi = ll_get_or_alloc_list(wp);
    లేకపోతే
	qi = ql_info;
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_దొంతర(wp, qi);
	తిరిగి OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    ఒకవేళ (list->lv_len != 0 && what != ఖాళీ)
    {
	semsg(_(e_invalid_వాదం_str),
			 _("cannot have both a list and a \"what\" argument"));
	తిరిగి FAIL;
    }

    incr_quickfix_busy();

    ఒకవేళ (what != ఖాళీ)
	retval = qf_సమితి_properties(qi, what, action, title);
    లేకపోతే
    {
	retval = qf_కూడు_entries(qi, qi->qf_curlist, list, title, action);
	ఒకవేళ (retval == OK)
	    qf_list_మారింది(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    తిరిగి retval;
}

స్థిర పూర్ణాంకం mark_quickfix_user_దత్తాంశం(qf_info_T *qi, పూర్ణాంకం copyID)
{
    పూర్ణాంకం abort = FALSE;
    కొరకు (పూర్ణాంకం i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	ఒకవేళ (!qfl->qf_has_user_దత్తాంశం)
	    కొనసాగు;
	qfline_T *qfp;
	పూర్ణాంకం j;
	FOR_ALL_QFL_వస్తువులు(qfl, qfp, j)
	{
	    typval_T* user_దత్తాంశం = &qfp->qf_user_దత్తాంశం;
	    ఒకవేళ (user_దత్తాంశం != ఖాళీ && user_దత్తాంశం->v_type != VAR_సంఖ్య
		&& user_దత్తాంశం->v_type != VAR_STRING && user_దత్తాంశం->v_type != VAR_FLOAT)
		abort = abort || సమితి_ref_లోపల_అంశం(user_దత్తాంశం, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    }
    తిరిగి abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    స్థిర పూర్ణాంకం
mark_quickfix_ctx(qf_info_T *qi, పూర్ణాంకం copyID)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    కొరకు (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	ఒకవేళ (ctx != ఖాళీ && ctx->v_type != VAR_సంఖ్య
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || సమితి_ref_లోపల_అంశం(ctx, copyID, ఖాళీ, ఖాళీ, ఖాళీ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || సమితి_ref_లోపల_callback(cb, copyID);
    }

    తిరిగి abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_quickfix(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    ఒకవేళ (ql_info == ఖాళీ)
	తిరిగి TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    ఒకవేళ (abort)
	తిరిగి abort;

    abort = mark_quickfix_user_దత్తాంశం(ql_info, copyID);
    ఒకవేళ (abort)
	తిరిగి abort;

    abort = సమితి_ref_లోపల_callback(&qftf_cb, copyID);
    ఒకవేళ (abort)
	తిరిగి abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ఒకవేళ (win->w_llist != ఖాళీ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    ఒకవేళ (abort)
		తిరిగి abort;

	    abort = mark_quickfix_user_దత్తాంశం(win->w_llist, copyID);
	    ఒకవేళ (abort)
		తిరిగి abort;
	}
	ఒకవేళ (IS_LL_కిటికీ(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    ఒకవేళ (abort)
		తిరిగి abort;

	    abort = mark_quickfix_user_దత్తాంశం(win->w_llist_ref, copyID);
	    ఒకవేళ (abort)
		తిరిగి abort;
	}
    }

    తిరిగి abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    స్థిర char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_cbuffer:	తిరిగి (char_u *)"cbuffer";
	స్థితి CMD_cgetbuffer:	తిరిగి (char_u *)"cgetbuffer";
	స్థితి CMD_caddbuffer:	తిరిగి (char_u *)"caddbuffer";
	స్థితి CMD_lbuffer:	తిరిగి (char_u *)"lbuffer";
	స్థితి CMD_lgetbuffer:	తిరిగి (char_u *)"lgetbuffer";
	స్థితి CMD_laddbuffer:	తిరిగి (char_u *)"laddbuffer";
	అప్రమేయ:		తిరిగి ఖాళీ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    స్థిర పూర్ణాంకం
cbuffer_process_వాదనలు(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ఖాళీ;

    ఒకవేళ (*eap->arg == NUL)
	buf = curbuf;
    లేకపోతే ఒకవేళ (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((అక్షరం *)eap->arg));

    ఒకవేళ (buf == ఖాళీ)
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి FAIL;
    }

    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
    {
	emsg(_(e_బఫరు_is_not_loaded));
	తిరిగి FAIL;
    }

    ఒకవేళ (eap->addr_లెక్క == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_పంక్తి_లెక్క;
    }

    ఒకవేళ (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_పంక్తి_లెక్క
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_పంక్తి_లెక్క)
    {
	emsg(_(e_invalid_range));
	తిరిగి FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    తిరిగి OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    శూన్య
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = ఖాళీ;
    qf_info_T	*qi;
    char_u	*au_name = ఖాళీ;
    పూర్ణాంకం		res;
    int_u	save_qfid;
    win_T	*wp = ఖాళీ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ఒకవేళ (aborting())
	    తిరిగి;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_దొంతర(eap, &wp);
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి;

    ఒకవేళ (cbuffer_process_వాదనలు(eap, &buf, &line1, &line2) == FAIL)
	తిరిగి;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    ఒకవేళ (buf->b_sfname)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%s (%s)",
		(అక్షరం *)qf_title, (అక్షరం *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ఖాళీ, buf, ఖాళీ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ఖాళీ);
    ఒకవేళ (qf_దొంతర_ఖాళీలు(qi))
    {
	decr_quickfix_busy();
	తిరిగి;
    }
    ఒకవేళ (res >= 0)
	qf_list_మారింది(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ఒకవేళ (au_name != ఖాళీ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	ఒకవేళ (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ఒకవేళ (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_మొదటి(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_cexpr:	    తిరిగి (char_u *)"cexpr";
	స్థితి CMD_cgetexpr:  తిరిగి (char_u *)"cgetexpr";
	స్థితి CMD_caddexpr:  తిరిగి (char_u *)"caddexpr";
	స్థితి CMD_lexpr:	    తిరిగి (char_u *)"lexpr";
	స్థితి CMD_lgetexpr:  తిరిగి (char_u *)"lgetexpr";
	స్థితి CMD_laddexpr:  తిరిగి (char_u *)"laddexpr";
	అప్రమేయ:	    తిరిగి ఖాళీ;
    }
}

    పూర్ణాంకం
trigger_cexpr_autocmd(పూర్ణాంకం cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	ఒకవేళ (aborting())
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

    పూర్ణాంకం
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ఖాళీ;

    qi = qf_cmd_get_or_alloc_దొంతర(eap, &wp);
    ఒకవేళ (qi == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ ((tv->v_type == VAR_STRING && tv->vval.v_string != ఖాళీ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ఖాళీ))
    {
	పూర్ణాంకం	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ఖాళీ, ఖాళీ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ఖాళీ);
	ఒకవేళ (qf_దొంతర_ఖాళీలు(qi))
	{
	    decr_quickfix_busy();
	    తిరిగి FAIL;
	}
	ఒకవేళ (res >= 0)
	    qf_list_మారింది(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	ఒకవేళ (au_name != ఖాళీ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	ఒకవేళ (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_మొదటి(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	తిరిగి OK;
    }

    emsg(_(e_string_or_list_expected));
    తిరిగి FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    శూన్య
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    ఒకవేళ (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	తిరిగి;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_సమాసం(eap->arg, eap);
    ఒకవేళ (tv == ఖాళీ)
	తిరిగి;

    (శూన్య)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    స్థిర qf_info_T *
hgr_get_ll(పూర్ణాంకం *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    ఒకవేళ (bt_help(curwin->w_బఫరు))
	wp = curwin;
    లేకపోతే
	// Find an existing help window
	wp = qf_కనుగొను_help_win();

    ఒకవేళ (wp == ఖాళీ)	    // Help window not found
	qi = ఖాళీ;
    లేకపోతే
	qi = wp->w_llist;

    ఒకవేళ (qi == ఖాళీ)
    {
	// Allocate a new location list for help text matches
	ఒకవేళ ((qi = qf_alloc_దొంతర(QFLT_LOCATION, 1)) == ఖాళీ)
	    తిరిగి ఖాళీ;
	*new_ll = TRUE;
    }

    తిరిగి qi;
}

/*
 * Search for a pattern in a help file.
 */
    స్థిర శూన్య
hgr_శోధన_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    దీర్ఘ	lnum;

    fd = mch_fopen((అక్షరం *)fname, "r");
    ఒకవేళ (fd == ఖాళీ)
	తిరిగి;

    lnum = 1;
    అయితే (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *పంక్తి = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	ఒకవేళ (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    పంక్తి = string_convert(p_vc, IObuff, ఖాళీ);
	    ఒకవేళ (పంక్తి == ఖాళీ)
		పంక్తి = IObuff;
	}

	ఒకవేళ (vim_regexec(p_regmatch, పంక్తి, (colnr_T)0))
	{
	    పూర్ణాంకం	l = (పూర్ణాంకం)STRLEN(పంక్తి);

	    // remove trailing CR, LF, spaces, etc.
	    అయితే (l > 0 && పంక్తి[l - 1] <= ' ')
		పంక్తి[--l] = NUL;

	    ఒకవేళ (qf_కూడు_నమోదు(qfl,
			ఖాళీ,	// dir
			fname,
			ఖాళీ,
			0,
			పంక్తి,
			lnum,
			0,
			(పూర్ణాంకం)(p_regmatch->startp[0] - పంక్తి)
			+ 1,	//నిలువు
			(పూర్ణాంకం)(p_regmatch->endp[0] - పంక్తి)
			+ 1,	// end_col
			FALSE,	// vis_col
			ఖాళీ,	// search pattern
			0,	// nr
			1,	// type
			ఖాళీ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		ఒకవేళ (పంక్తి != IObuff)
		    vim_free(పంక్తి);
		ఆపు;
	    }
	}
	ఒకవేళ (పంక్తి != IObuff)
	    vim_free(పంక్తి);
	++lnum;
	పంక్తి_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    స్థిర శూన్య
hgr_శోధన_files_లోపల_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    పూర్ణాంకం		fcount;
    char_u	**fnames;
    పూర్ణాంకం		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    కూడు_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    ఒకవేళ (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	కొరకు (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    ఒకవేళ (lang != ఖాళీ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		కొనసాగు;
# endif

	    hgr_శోధన_file(qfl, fnames[fi], p_vc, p_regmatch);
	}
	FreeWild(fcount, fnames);
    }
}

/*
 * Search for a pattern in all the help files in the 'runtimepath'
 * and add the matches to a quickfix list.
 * 'lang' is the language specifier.  If supplied, then only matches in the
 * specified language are found.
 */
    స్థిర శూన్య
hgr_శోధన_లోపల_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    ఒకవేళ (!enc_utf8)
	convert_అమరిక(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    అయితే (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_శోధన_files_లోపల_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    ఒకవేళ (vc.vc_type != CONV_NONE)
	convert_అమరిక(&vc, ఖాళీ, ఖాళీ);
}

/*
 * ":helpgrep {pattern}"
 */
    శూన్య
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    పూర్ణాంకం		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    పూర్ణాంకం		new_qi = FALSE;
    char_u	*au_name =  ఖాళీ;
    char_u	*lang = ఖాళీ;
    పూర్ణాంకం		updated = FALSE;

    ఎంపిక (eap->cmdidx)
    {
	స్థితి CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ఆపు;
	స్థితి CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ఆపు;
	అప్రమేయ: ఆపు;
    }
    ఒకవేళ (au_name != ఖాళీ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ఒకవేళ (aborting())
	    తిరిగి;
# endif
    }

    ఒకవేళ (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	ఒకవేళ (qi == ఖాళీ)
	    తిరిగి;
    }
    లేకపోతే ఒకవేళ (qi == ఖాళీ)
    {
	emsg(_(e_no_quickfix_దొంతర));
	తిరిగి;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = ఖాళీలు_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_శోధన_లోపల_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_ప్రారంభం;
	qfl->qf_సూచీ = 1;
	qf_list_మారింది(qfl);
	updated = TRUE;
    }

    ఒకవేళ (p_cpo == ఖాళీలు_option)
	p_cpo = save_cpo;
    లేకపోతే
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	ఒకవేళ (*p_cpo == NUL)
	    సమితి_option_విలువ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	ఒకవేళ (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    ఒకవేళ (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_బఫరు(qi, ఖాళీ);

    ఒకవేళ (au_name != ఖాళీ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	ఒకవేళ (!new_qi && IS_LL_దొంతర(qi) && qf_కనుగొను_win_with_loclist(qi) == ఖాళీ)
	{
	    decr_quickfix_busy();
	    తిరిగి;
	}
    }

    // Jump to first match.
    ఒకవేళ (!qf_list_ఖాళీలు(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    లేకపోతే
	semsg(_(e_no_సరిపోలిక_str_2), eap->arg);

    decr_quickfix_busy();

    ఒకవేళ (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	ఒకవేళ (!bt_help(curwin->w_బఫరు) || curwin->w_llist == qi)
	{
	    ఒకవేళ (new_qi)
		ll_free_all(&qi);
	}
	లేకపోతే ఒకవేళ (curwin->w_llist == ఖాళీ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_quickfix(శూన్య)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ఖాళీ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#అయితే_ముగింపు // FEAT_QUICKFIX

#ఒకవేళ defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    స్థిర శూన్య
get_qf_loc_list(పూర్ణాంకం is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    ఒకవేళ (what_arg->v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (rettv_list_alloc(rettv) == OK)
	    ఒకవేళ (is_qf || wp != ఖాళీ)
		(శూన్య)get_errorlist(ఖాళీ, wp, -1, 0, rettv->vval.v_list);
    }
    లేకపోతే
    {
	ఒకవేళ (rettv_dict_alloc(rettv) == OK)
	    ఒకవేళ (is_qf || (wp != ఖాళీ))
	    {
		ఒకవేళ (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    ఒకవేళ (d != ఖాళీ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		లేకపోతే
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    శూన్య
f_getloclist(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    శూన్య
f_getqflist(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_QUICKFIX
    ఒకవేళ (లోపల_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    get_qf_loc_list(TRUE, ఖాళీ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    స్థిర శూన్య
సమితి_qf_ll_list(
    win_T	*wp వాడని,
    typval_T	*list_arg వాడని,
    typval_T	*action_arg వాడని,
    typval_T	*what_arg వాడని,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    పూర్ణాంకం		action = 0;
    స్థిర పూర్ణాంకం	recursive = 0;
# endif

    rettv->vval.v_సంఖ్య = -1;

# ifdef FEAT_QUICKFIX
    ఒకవేళ (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    లేకపోతే ఒకవేళ (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    లేకపోతే
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ఖాళీ;
	పూర్ణాంకం	valid_dict = TRUE;

	ఒకవేళ (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    ఒకవేళ (act == ఖాళీ)
		తిరిగి;		// type error; errmsg already given
	    ఒకవేళ ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    లేకపోతే
		semsg(_(e_invalid_action_str_1), act);
	}
	లేకపోతే ఒకవేళ (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	లేకపోతే
	    emsg(_(e_string_required));

	ఒకవేళ (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    ఒకవేళ (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ఖాళీ)
		what = what_arg->vval.v_dict;
	    లేకపోతే
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	ఒకవేళ (l != ఖాళీ && action && valid_dict
		    && సమితి_errorlist(wp, l, action,
		     (char_u *)(wp == ఖాళీ ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_సంఖ్య = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    శూన్య
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	తిరిగి;

    win = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (win != ఖాళీ)
	సమితి_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    శూన్య
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	తిరిగి;

    సమితి_qf_ll_list(ఖాళీ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#అయితే_ముగింపు
