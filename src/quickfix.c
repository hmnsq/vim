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

#شامل "vim.h"

#اگر defined(FEAT_QUICKFIX)

ساختار dir_پشته_T
{
    ساختار dir_پشته_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
تعریفنوع ساختار qfline_S qfline_T;
ساختار qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    صحیح		qf_fnum;	// file number for the line
    صحیح		qf_col;		// column where the error occurred
    صحیح		qf_end_col;	// column when the error has range or zero
    صحیح		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_pattern;	// search pattern for the error
    char_u	*qf_text;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_data;	// custom user data associated with this item
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
تعریفنوع شمارش
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_INTERNAL  // Internal - Temporary list used by getqflist()/getloclist()
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
تعریفنوع ساختار qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    صحیح		qf_شمار;	// number of errors (0 means empty list)
    صحیح		qf_index;	// current index in the error list
    صحیح		qf_nonevalid;	// TRUE if not a single valid entry found
    صحیح		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ساختار dir_پشته_T	*qf_dir_پشته;
    char_u		*qf_directory;
    ساختار dir_پشته_T	*qf_file_پشته;
    char_u		*qf_currfile;
    صحیح			qf_multiline;
    صحیح			qf_multiignore;
    صحیح			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ساختار qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    صحیح		qf_refcount;
    صحیح		qf_listcount;	    // current number of lists
    صحیح		qf_curlist;	    // current error list
    صحیح         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    صحیح		qf_bufnr;	    // quickfix window buffer number
};

ایستا qf_info_T ql_info_actual; // global quickfix list
ایستا qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ایستا int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
تعریفنوع ساختار efm_S efm_T;
ساختار efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *next;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    prefix;	// prefix of this format line:
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
    صحیح		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
تعریفنوع ساختار qf_delq_S
{
    ساختار qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
ایستا qf_delq_T *qf_delq_head = پوچ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ایستا صحیح	quickfix_busy = 0;

ایستا efm_T	*fmt_start = پوچ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ایستا callback_T qftf_cb;

ایستا تهی     qf_برداشت_پشته(qf_info_T *qi, صحیح adjust);
ایستا تهی	qf_new_list(qf_info_T *qi, char_u *qf_title);
ایستا صحیح	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, صحیح bufnum, char_u *mesg, long lnum, long end_lnum, صحیح col, صحیح end_col, صحیح vis_col, char_u *pattern, صحیح nr, صحیح type, typval_T *user_data, صحیح valid);
ایستا صحیح      qf_resize_پشته_base(qf_info_T *qi, صحیح n);
ایستا تهی     qf_sync_llw_to_win(win_T *llw);
ایستا تهی     qf_sync_win_to_llw(win_T *pwp);
ایستا qf_info_T *qf_alloc_پشته(qfltype_T qfltype, صحیح n);
ایستا qf_list_T *qf_alloc_list_پشته(صحیح n);
ایستا تهی	qf_free(qf_list_T *qfl);
ایستا char_u	*qf_types(صحیح, صحیح);
ایستا صحیح	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ایستا char_u	*qf_افزودن_dir(char_u *, ساختار dir_پشته_T **, صحیح is_file_پشته);
ایستا char_u	*qf_برداشت_dir(ساختار dir_پشته_T **);
ایستا char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ایستا win_T    *qf_find_win_with_loclist(qf_info_T *ll);
ایستا تهی	qf_jump_newwin(qf_info_T *qi, صحیح dir, صحیح errornr, صحیح forceit, صحیح newwin);
ایستا تهی	qf_fmt_text(garray_T *gap, char_u *text);
ایستا تهی	qf_range_text(garray_T *gap, qfline_T *qfp);
ایستا صحیح	qf_win_pos_update(qf_info_T *qi, صحیح old_qf_index);
ایستا win_T	*qf_find_win(qf_info_T *qi);
ایستا buf_T	*qf_find_buf(qf_info_T *qi);
ایستا تهی	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
ایستا تهی	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, صحیح qf_winid);
ایستا buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
ایستا تهی	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
ایستا تهی	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
ایستا qf_info_T *ll_get_or_alloc_list(win_T *);
ایستا صحیح	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, صحیح target_fnum, صحیح target_lnum, صحیح target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == پوچ)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != پوچ)

// Quickfix and location list stack check helper macros
# define IS_QF_پشته(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_پشته(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_اقلام(qfl, qfp, i) \
		    برای ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_شمار && (qfp) != پوچ; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ایستا char_u   *qf_last_bufname = پوچ;
ایستا bufref_T  qf_last_bufref = {پوچ, 0, 0};

ایستا garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ایستا garray_T *
qfga_get(تهی)
{
    ایستا صحیح initialized = FALSE;

    اگر (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    بازگشت &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ایستا تهی
qfga_clear(تهی)
{
    اگر (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    وگرنه
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define سطر_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ایستا ساختار fmtpattern
{
    char_u	convchar;
    نویسه	*pattern;
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
# define FMT_PATTERN_M 8
	{'m', ".\\+"},		// 8
# define FMT_PATTERN_R 9
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
    ایستا char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	صحیح	idx,
	صحیح	round)
{
    char_u	*srcptr;

    اگر (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	بازگشت پوچ;
    }
    اگر ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != پوچ)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == پوچ))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	بازگشت پوچ;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    اگر (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    اگر (*efmpat == 'f' && efmpat[1] != NUL)
    {
	اگر (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	وگرنه
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    وگرنه
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	وقتی ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    بازگشت regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ایستا char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	صحیح	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    اگر (*efmp == '[' || *efmp == '\\')
    {
	اگر ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    اگر (efmp[1] == '^')
		*regpat++ = *++efmp;
	    اگر (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		وقتی (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		اگر (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    بازگشت پوچ;
		}
	    }
	}
	وگرنه اگر (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    وگرنه
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	بازگشت پوچ;
    }

    *pefmp = efmp;

    بازگشت regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ایستا char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    اگر (vim_strchr((char_u *)"+-", *efmp) != پوچ)
	efminfo->flags = *efmp++;
    اگر (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != پوچ)
	efminfo->prefix = *efmp;
    وگرنه
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	بازگشت پوچ;
    }

    بازگشت efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ایستا صحیح
efm_to_regpat(
	char_u	*efm,
	صحیح	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    صحیح		round;
    صحیح		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    برای (efmp = efm; efmp < efm + len; ++efmp)
    {
	اگر (*efmp == '%')
	{
	    ++efmp;
	    برای (idx = 0; idx < FMT_PATTERNS; ++idx)
		اگر (fmt_pat[idx].convchar == *efmp)
		    بشکن;
	    اگر (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		اگر (ptr == پوچ)
		    بازگشت FAIL;
		round++;
	    }
	    وگرنه اگر (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		اگر (ptr == پوچ)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (vim_strchr((char_u *)"%\\.^$~[", *efmp) != پوچ)
		*ptr++ = *efmp;		// regexp magic characters
	    وگرنه اگر (*efmp == '#')
		*ptr++ = '*';
	    وگرنه اگر (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    وگرنه اگر (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		اگر (efmp == پوچ)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		بازگشت FAIL;
	    }
	}
	وگرنه			// copy normal character
	{
	    اگر (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    وگرنه اگر (vim_strchr((char_u *)".*^$~[", *efmp) != پوچ)
		*ptr++ = '\\';	// escape regexp atoms
	    اگر (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    بازگشت OK;
}

/*
 * Free the 'errorformat' information list
 */
    ایستا تهی
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    برای (efm_ptr = *efm_first; efm_ptr != پوچ; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = پوچ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ایستا صحیح
efm_regpat_bufsz(char_u *efm)
{
    صحیح sz;
    صحیح i;

    sz = (FMT_PATTERNS * 3) + ((صحیح)STRLEN(efm) << 2);
    برای (i = FMT_PATTERNS; i > 0; )
	sz += (صحیح)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# وگرنه
    sz += 2; // "%f" can become two chars longer
# endif

    بازگشت sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ایستا صحیح
efm_option_part_len(char_u *efm)
{
    صحیح len;

    برای (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	اگر (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    بازگشت len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ایستا efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = پوچ;
    efm_T	*fmt_first = پوچ;
    efm_T	*fmt_last = پوچ;
    char_u	*fmtstr = پوچ;
    صحیح		len;
    صحیح		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    اگر ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == پوچ)
	برو parse_efm_error;

    وقتی (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	اگر (fmt_ptr == پوچ)
	    برو parse_efm_error;
	اگر (fmt_first == پوچ)	    // first one
	    fmt_first = fmt_ptr;
	وگرنه
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	اگر (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    برو parse_efm_error;
	اگر ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == پوچ)
	    برو parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    اگر (fmt_first == پوچ)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    برو parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    بازگشت fmt_first;
}

شمارش {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_درون_داد = 2,
    QF_NOMEM = 3,
    QF_IGNORE_سطر = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
تعریفنوع ساختار {
    char_u	*linebuf;
    صحیح		linelen;
    char_u	*growbuf;
    صحیح		growbufsiz;
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
    ایستا char_u *
qf_grow_linebuf(qfstate_T *state, صحیح newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > سطر_MAXLEN ? سطر_MAXLEN - 1 : newsz;
    اگر (state->growbuf == پوچ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	اگر (state->growbuf == پوچ)
	    بازگشت پوچ;
	state->growbufsiz = state->linelen;
    }
    وگرنه اگر (state->linelen > state->growbufsiz)
    {
	اگر ((p = vim_realloc(state->growbuf, state->linelen + 1)) == پوچ)
	    بازگشت پوچ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    بازگشت state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ایستا صحیح
qf_get_next_str_سطر(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    صحیح		len;

    اگر (*p_str == NUL) // Reached the end of the string
	بازگشت QF_END_OF_درون_داد;

    p = vim_strchr(p_str, '\n');
    اگر (p != پوچ)
	len = (صحیح)(p - p_str) + 1;
    وگرنه
	len = (صحیح)STRLEN(p_str);

    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == پوچ)
	    بازگشت QF_NOMEM;
    }
    وگرنه
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    بازگشت QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ایستا صحیح
qf_get_next_list_سطر(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    صحیح		len;

    وقتی (p_li != پوچ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == پوچ))
	p_li = p_li->li_next;	// Skip non-string items

    اگر (p_li == پوچ)		// End of the list
    {
	state->p_li = پوچ;
	بازگشت QF_END_OF_درون_داد;
    }

    len = (صحیح)STRLEN(p_li->li_tv.vval.v_string);
    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == پوچ)
	    بازگشت QF_NOMEM;
    }
    وگرنه
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    بازگشت QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ایستا صحیح
qf_get_next_buf_سطر(qfstate_T *state)
{
    char_u	*p_buf = پوچ;
    صحیح		len;

    // Get the next line from the supplied buffer
    اگر (state->buflnum > state->lnumlast)
	بازگشت QF_END_OF_درون_داد;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    اگر (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	اگر (state->linebuf == پوچ)
	    بازگشت QF_NOMEM;
    }
    وگرنه
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    بازگشت QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ایستا صحیح
qf_get_next_file_سطر(qfstate_T *state)
{
    صحیح	    discard;
    صحیح	    growbuflen;

    اگر (fgets((نویسه *)IObuff, IOSIZE, state->fd) == پوچ)
	بازگشت QF_END_OF_درون_داد;

    discard = FALSE;
    state->linelen = (صحیح)STRLEN(IObuff);
    اگر (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	اگر (state->growbuf == پوچ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    اگر (state->growbuf == پوچ)
		بازگشت QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	برای (;;)
	{
	    char_u	*p;

	    اگر (fgets((نویسه *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == پوچ)
		بشکن;
	    state->linelen = (صحیح)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    اگر ((state->growbuf)[growbuflen - 1] == '\n')
		بشکن;
	    اگر (state->growbufsiz == سطر_MAXLEN)
	    {
		discard = TRUE;
		بشکن;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < سطر_MAXLEN
		? 2 * state->growbufsiz : سطر_MAXLEN;
	    اگر ((p = vim_realloc(state->growbuf, state->growbufsiz)) == پوچ)
		بازگشت QF_NOMEM;
	    state->growbuf = p;
	}

	وقتی (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    اگر (fgets((نویسه *)IObuff, IOSIZE, state->fd) == پوچ
		    || (صحیح)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		بشکن;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    وگرنه
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    اگر (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*سطر;

	سطر = string_convert(&state->vc, state->linebuf, &state->linelen);
	اگر (سطر != پوچ)
	{
	    اگر (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, سطر);
		vim_free(سطر);
	    }
	    وگرنه
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = سطر;
		state->growbufsiz = state->linelen < سطر_MAXLEN
						? state->linelen : سطر_MAXLEN;
	    }
	}
    }

    بازگشت QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ایستا صحیح
qf_get_nextline(qfstate_T *state)
{
    صحیح status = QF_FAIL;

    اگر (state->fd == پوچ)
    {
	اگر (state->tv != پوچ)
	{
	    اگر (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_سطر(state);
	    وگرنه اگر (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_سطر(state);
	}
	وگرنه
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_سطر(state);
    }
    وگرنه
	// Get the next line from the supplied file
	status = qf_get_next_file_سطر(state);

    اگر (status != QF_OK)
	بازگشت status;

    // remove newline/CR from the line
    اگر (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	اگر (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    بازگشت QF_OK;
}

تعریفنوع ساختار {
    char_u	*namebuf;
    صحیح		bnr;
    char_u	*module;
    char_u	*errmsg;
    صحیح		errmsglen;
    long	lnum;
    long	end_lnum;
    صحیح		col;
    صحیح		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    صحیح		enr;
    صحیح		type;
    typval_T	*user_data;
    صحیح		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ایستا صحیح
qf_parse_fmt_f(regmatch_T *rmp, صحیح midx, qffields_T *fields, صحیح prefix)
{
    صحیح c;

    اگر (rmp->startp[midx] == پوچ || rmp->endp[midx] == پوچ)
	بازگشت QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    اگر (vim_strchr((char_u *)"OPQ", prefix) != پوچ
	    && mch_getperm(fields->namebuf) == -1)
	بازگشت QF_FAIL;

    بازگشت QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ایستا صحیح
qf_parse_fmt_b(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    صحیح bnr = (صحیح)atol((نویسه *)rmp->startp[midx]);
    اگر (buflist_findnr(bnr) == پوچ)
	بازگشت QF_FAIL;
    fields->bnr = bnr;
    بازگشت QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ایستا صحیح
qf_parse_fmt_n(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->enr = (صحیح)atol((نویسه *)rmp->startp[midx]);
    بازگشت QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ایستا صحیح
qf_parse_fmt_l(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->lnum = atol((نویسه *)rmp->startp[midx]);
    بازگشت QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ایستا صحیح
qf_parse_fmt_e(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->end_lnum = atol((نویسه *)rmp->startp[midx]);
    بازگشت QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ایستا صحیح
qf_parse_fmt_c(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->col = (صحیح)atol((نویسه *)rmp->startp[midx]);
    بازگشت QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ایستا صحیح
qf_parse_fmt_k(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->end_col = (صحیح)atol((نویسه *)rmp->startp[midx]);
    بازگشت QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ایستا صحیح
qf_parse_fmt_t(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->type = *rmp->startp[midx];
    بازگشت QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ایستا صحیح
copy_nonerror_سطر(char_u *linebuf, صحیح linelen, qffields_T *fields)
{
    char_u	*p;

    اگر (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	اگر ((p = vim_realloc(fields->errmsg, linelen + 1)) == پوچ)
	    بازگشت QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    بازگشت QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ایستا صحیح
qf_parse_fmt_m(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    char_u	*p;
    صحیح		len;

    اگر (rmp->startp[midx] == پوچ || rmp->endp[midx] == پوچ)
	بازگشت QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len >= fields->errmsglen)
    {
	// len + null terminator
	اگر ((p = vim_realloc(fields->errmsg, len + 1)) == پوچ)
	    بازگشت QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    بازگشت QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ایستا صحیح
qf_parse_fmt_r(regmatch_T *rmp, صحیح midx, char_u **tail)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    *tail = rmp->startp[midx];
    بازگشت QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ایستا صحیح
qf_parse_fmt_p(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    char_u	*match_ptr;

    اگر (rmp->startp[midx] == پوچ || rmp->endp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->col = 0;
    برای (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	اگر (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    بازگشت QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ایستا صحیح
qf_parse_fmt_v(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    اگر (rmp->startp[midx] == پوچ)
	بازگشت QF_FAIL;
    fields->col = (صحیح)atol((نویسه *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    بازگشت QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ایستا صحیح
qf_parse_fmt_s(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    صحیح		len;

    اگر (rmp->startp[midx] == پوچ || rmp->endp[midx] == پوچ)
	بازگشت QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    بازگشت QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ایستا صحیح
qf_parse_fmt_o(regmatch_T *rmp, صحیح midx, qffields_T *fields)
{
    صحیح		len;

    اگر (rmp->startp[midx] == پوچ || rmp->endp[midx] == پوچ)
	بازگشت QF_FAIL;
    len = (صحیح)(rmp->endp[midx] - rmp->startp[midx]);
    اگر (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    بازگشت QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ایستا صحیح (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, صحیح, qffields_T *) =
{
    پوچ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    پوچ, // %r
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
    ایستا صحیح
qf_parse_match(
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	صحیح		qf_multiline,
	صحیح		qf_multiscan,
	char_u		**tail)
{
    صحیح		idx = fmt_ptr->prefix;
    صحیح		i;
    صحیح		midx;
    صحیح		status;

    اگر ((idx == 'C' || idx == 'Z') && !qf_multiline)
	بازگشت QF_FAIL;
    اگر (vim_strchr((char_u *)"EWIN", idx) != پوچ)
	fields->type = idx;
    وگرنه
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    برای (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (صحیح)fmt_ptr->addr[i];
	اگر (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	وگرنه اگر (i == FMT_PATTERN_M)
	{
	    اگر (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_سطر(linebuf, linelen, fields);
	    وگرنه اگر (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	وگرنه اگر (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	وگرنه اگر (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	اگر (status != QF_OK)
	    بازگشت status;
    }

    بازگشت QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ایستا صحیح
qf_parse_get_fields(
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	صحیح		qf_multiline,
	صحیح		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    صحیح		status = QF_FAIL;
    صحیح		r;

    اگر (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == پوچ)
	بازگشت QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    اگر (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = پوچ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    اگر (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    بازگشت status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ایستا صحیح
qf_parse_dir_pfx(صحیح idx, qffields_T *fields, qf_list_T *qfl)
{
    اگر (idx == 'D')				// enter directory
    {
	اگر (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    بازگشت QF_FAIL;
	}
	qfl->qf_directory =
	    qf_افزودن_dir(fields->namebuf, &qfl->qf_dir_پشته, FALSE);
	اگر (qfl->qf_directory == پوچ)
	    بازگشت QF_FAIL;
    }
    وگرنه اگر (idx == 'X')			// leave directory
	qfl->qf_directory = qf_برداشت_dir(&qfl->qf_dir_پشته);

    بازگشت QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ایستا صحیح
qf_parse_file_pfx(
	صحیح idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    اگر (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	اگر (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_افزودن_dir(fields->namebuf, &qfl->qf_file_پشته, TRUE);
	وگرنه اگر (idx == 'Q')
	    qfl->qf_currfile = qf_برداشت_dir(&qfl->qf_file_پشته);
	*fields->namebuf = NUL;
	اگر (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    بازگشت QF_MULTISCAN;
	}
    }

    بازگشت QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ایستا صحیح
qf_parse_سطر_nomatch(char_u *linebuf, صحیح linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    بازگشت copy_nonerror_سطر(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ایستا صحیح
qf_parse_multiline_pfx(
	صحیح idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    صحیح			len;

    اگر (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	اگر (qfprev == پوچ)
	    بازگشت QF_FAIL;
	اگر (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (صحیح)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    اگر (ptr == پوچ)
		بازگشت QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	اگر (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	اگر (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	اگر (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	اگر (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	اگر (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	اگر (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	اگر (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != پوچ
		    ? fields->namebuf
		    : qfl->qf_currfile != پوچ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    اگر (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    سطر_breakcheck();

    بازگشت QF_IGNORE_سطر;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ایستا صحیح
qf_parse_سطر(
	qf_list_T	*qfl,
	char_u		*linebuf,
	صحیح		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    صحیح			idx = 0;
    char_u		*tail = پوچ;
    صحیح			status;

restofline:
    // If there was no %> item start at the first pattern
    اگر (fmt_start == پوچ)
	fmt_ptr = fmt_first;
    وگرنه
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = پوچ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    برای ( ; fmt_ptr != پوچ; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	اگر (status == QF_NOMEM)
	    بازگشت status;
	اگر (status == QF_OK)
	    بشکن;
    }
    qfl->qf_multiscan = FALSE;

    اگر (fmt_ptr == پوچ || idx == 'D' || idx == 'X')
    {
	اگر (fmt_ptr != پوچ)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    اگر (status != QF_OK)
		بازگشت status;
	}

	status = qf_parse_سطر_nomatch(linebuf, linelen, fields);
	اگر (status != QF_OK)
	    بازگشت status;

	اگر (fmt_ptr == پوچ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    وگرنه اگر (fmt_ptr != پوچ)
    {
	// honor %> item
	اگر (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	اگر (vim_strchr((char_u *)"AEWIN", idx) != پوچ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	وگرنه اگر (vim_strchr((char_u *)"CZ", idx) != پوچ)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    اگر (status != QF_OK)
		بازگشت status;
	}
	وگرنه اگر (vim_strchr((char_u *)"OPQ", idx) != پوچ)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    اگر (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		صحیح new_linelen = (صحیح)STRLEN(s);
		اگر (new_linelen >= linelen)
		    بازگشت QF_IGNORE_سطر;
		linebuf = s;
		linelen = new_linelen;
		برو restofline;
	    }
	}
	اگر (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    اگر (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    بازگشت QF_IGNORE_سطر;
	}
    }

    بازگشت QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ایستا صحیح
qf_پشته_empty(qf_info_T *qi)
{
    بازگشت qi == پوچ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ایستا صحیح
qf_list_empty(qf_list_T *qfl)
{
    بازگشت qfl == پوچ || qfl->qf_شمار <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ایستا صحیح
qf_list_has_valid_entries(qf_list_T *qfl)
{
    بازگشت !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ایستا qf_list_T *
qf_get_list(qf_info_T *qi, صحیح idx)
{
    بازگشت &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ایستا صحیح
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    اگر (pfields->namebuf == پوچ || pfields->errmsg == پوچ
		|| pfields->pattern == پوچ || pfields->module == پوچ)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ایستا تهی
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->pattern);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ایستا صحیح
qf_setup_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    اگر (enc != پوچ && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    اگر (efile != پوچ && (pstate->fd = mch_fopen((نویسه *)efile, "r")) == پوچ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	بازگشت FAIL;
    }

    اگر (tv != پوچ)
    {
	اگر (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	وگرنه اگر (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    بازگشت OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ایستا تهی
qf_cleanup_state(qfstate_T *pstate)
{
    اگر (pstate->fd != پوچ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    اگر (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, پوچ, پوچ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ایستا صحیح
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    صحیح		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    اگر (status != QF_OK)
	بازگشت status;

    status = qf_parse_سطر(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    اگر (status != QF_OK)
	بازگشت status;

    بازگشت qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != پوچ)
		? fields->namebuf
		: ((qfl->qf_currfile != پوچ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)پوچ),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->end_lnum,
		fields->col,
		fields->end_col,
		fields->use_viscol,
		fields->pattern,
		fields->enr,
		fields->type,
		fields->user_data,
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
    ایستا صحیح
qf_init_ext(
    qf_info_T	    *qi,
    صحیح		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    صحیح		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = پوچ;
    صحیح		    adding = FALSE;
    ایستا efm_T    *fmt_first = پوچ;
    char_u	    *efm;
    ایستا char_u   *last_efm = پوچ;
    صحیح		    retval = -1;	// default: return error flag
    صحیح		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    اگر ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	برو qf_init_end;

    اگر (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    وگرنه
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	اگر (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    اگر (errorformat == p_efm && tv == پوچ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    وگرنه
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    اگر (last_efm == پوچ || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	اگر (fmt_first != پوچ)
	    last_efm = vim_strsave(efm);
    }

    اگر (fmt_first == پوچ)	// nothing found
	برو error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    وقتی (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	اگر (status == QF_NOMEM)		// memory alloc failure
	    برو qf_init_end;
	اگر (status == QF_END_OF_درون_داد)	// end of input
	    بشکن;
	اگر (status == QF_FAIL)
	    برو error2;

	سطر_breakcheck();
    }
    اگر (state.fd == پوچ || !ferror(state.fd))
    {
	اگر (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	وگرنه
	{
	    qfl->qf_nonevalid = FALSE;
	    اگر (qfl->qf_ptr == پوچ)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_شمار;
	برو qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    اگر (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	اگر (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    اگر (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    بازگشت retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    صحیح
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	صحیح	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    اگر (wp != پوچ)
	qi = ll_get_or_alloc_list(wp);
    اگر (qi == پوچ)
	بازگشت FAIL;

    بازگشت qf_init_ext(qi, qi->qf_curlist, efile, curbuf, پوچ, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ایستا تهی
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    اگر (title == پوچ)
	بازگشت;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    اگر (p != پوچ)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ایستا char_u *
qf_cmdtitle(char_u *cmd)
{
    ایستا char_u qftitle_str[IOSIZE];

    vim_snprintf((نویسه *)qftitle_str, IOSIZE, ":%s", (نویسه *)cmd);
    بازگشت qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ایستا qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    بازگشت qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ایستا تهی
qf_برداشت_پشته(qf_info_T *qi, صحیح adjust)
{
    صحیح i;
    qf_free(&qi->qf_lists[0]);
    برای (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, اندازه(*qi->qf_lists));

    اگر (adjust)
    {
	qi->qf_listcount--;
	اگر (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	وگرنه
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ایستا تهی
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    وقتی (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    اگر (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_برداشت_پشته(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    وگرنه
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_POINTER(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++last_qf_id;
    qfl->qf_has_user_data = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    ایستا تهی
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    اگر (q == پوچ)
	بازگشت;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    صحیح
qf_پشته_get_bufnr(تهی)
{
    اگر (ql_info == پوچ)
	بازگشت INVALID_QFBUFNR;
    بازگشت ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ایستا تهی
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    اگر (qi->qf_bufnr == INVALID_QFBUFNR)
	بازگشت;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    اگر (qfbuf != پوچ && qfbuf->b_nwindows == 0)
    {
	صحیح buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	اگر (curwin->w_buffer == پوچ)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(پوچ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	اگر (buf_was_null)
	    curwin->w_buffer = پوچ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ایستا تهی
qf_free_list_پشته_اقلام(qf_info_T *qi)
{
    برای (صحیح i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ایستا تهی
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_پشته_اقلام(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ایستا تهی
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    اگر (qi == پوچ)
	بازگشت;
    *pqi = پوچ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    اگر (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	بازگشت;
    }

    qi->qf_refcount--;
    اگر (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_buffer(qi);

	qf_free_lists(qi);
    }
}

/*
 * Free all the quickfix/location lists in the stack.
 */
    تهی
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    اگر (wp != پوچ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    وگرنه اگر (qi != پوچ)
	qf_free_list_پشته_اقلام(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ایستا تهی
incr_quickfix_busy(تهی)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ایستا تهی
decr_quickfix_busy(تهی)
{
    اگر (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	وقتی (qf_delq_head != پوچ)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    اگر (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# اگر defined(EXITFREE)
    تهی
check_quickfix_busy(تهی)
{
    اگر (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (long)quickfix_busy);
#  ifdef ABORT_ON_INTERNAL_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    ایستا صحیح
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    صحیح		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    صحیح		col,		// column
    صحیح		end_col,	// column for end
    صحیح		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    صحیح		nr,		// error number
    صحیح		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    صحیح		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = پوچ;
    char_u	*p = پوچ;

    اگر ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == پوچ)
	بازگشت QF_FAIL;
    اگر (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	اگر (buf != پوچ)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    وگرنه
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    اگر (fname != پوچ)
	fullname = fix_fname(fname);
    qfp->qf_fname = پوچ;
    اگر (buf != پوچ &&
	buf->b_ffname != پوچ && fullname != پوچ)
    {
	اگر (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    اگر (p != پوچ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    اگر ((qfp->qf_text = vim_strsave(mesg)) == پوچ)
    {
	vim_free(qfp);
	بازگشت QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    اگر (user_data == پوچ || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    وگرنه
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    اگر (pattern == پوچ || *pattern == NUL)
	qfp->qf_pattern = پوچ;
    وگرنه اگر ((qfp->qf_pattern = vim_strsave(pattern)) == پوچ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	بازگشت QF_FAIL;
    }
    اگر (module == پوچ || *module == NUL)
	qfp->qf_module = پوچ;
    وگرنه اگر ((qfp->qf_module = vim_strsave(module)) == پوچ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	بازگشت QF_FAIL;
    }
    qfp->qf_nr = nr;
    اگر (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    اگر (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = پوچ;
    }
    وگرنه
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = پوچ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_شمار;
    اگر (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_شمار;
	qfl->qf_ptr = qfp;
    }

    بازگشت QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    صحیح
qf_resize_پشته(صحیح n)
{
    اگر (ql_info == پوچ)
	بازگشت FAIL;

    اگر (qf_resize_پشته_base(ql_info, n) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    صحیح
ll_resize_پشته(win_T *wp, صحیح n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    اگر (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    وگرنه
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    اگر (qi == پوچ)
	بازگشت FAIL;

    اگر (qf_resize_پشته_base(qi, n) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ایستا صحیح
qf_resize_پشته_base(qf_info_T *qi, صحیح n)
{
    qf_list_T *new;
    صحیح amount_to_rm = 0, i;

    اگر (qi == پوچ)
	بازگشت FAIL;

    size_t lsz = اندازه(*qi->qf_lists);

    اگر (n == qi->qf_maxcount)
	بازگشت OK;
    وگرنه اگر (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	برای (i = 0; i < amount_to_rm; i++)
	    qf_برداشت_پشته(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    اگر (new == پوچ)
	بازگشت FAIL;

    // fill with zeroes any newly allocated memory
    اگر (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, پوچ);

    بازگشت OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   تهی
qf_init_پشته(تهی)
{
    ql_info = qf_alloc_پشته(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ایستا تهی
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    اگر (wp != پوچ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ایستا تهی
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    اگر (llw != پوچ)
	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		بازگشت;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ایستا qf_info_T *
qf_alloc_پشته(qfltype_T qfltype, صحیح n)
{
    qf_info_T *qi;

    اگر (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    وگرنه
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	اگر (qi == پوچ)
	    بازگشت پوچ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_پشته(n);
    اگر (qi->qf_lists == پوچ)
    {
	اگر (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	بازگشت پوچ;
    }
    qi->qf_maxcount = n;

    بازگشت qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ایستا qf_list_T *
qf_alloc_list_پشته(صحیح n)
{
    بازگشت ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ایستا qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    اگر (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	بازگشت wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    اگر (wp->w_llist == پوچ)
	// new location list
	wp->w_llist = qf_alloc_پشته(QFLT_LOCATION, wp->w_p_lhi);

    بازگشت wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ایستا qf_info_T *
qf_cmd_get_پشته(exarg_T *eap, صحیح print_emsg)
{
    qf_info_T	*qi = ql_info;

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	اگر (qi == پوچ)
	{
	    اگر (print_emsg)
		emsg(_(e_no_location_list));
	    بازگشت پوچ;
	}
    }
    اگر (qi == پوچ && print_emsg)
	emsg(_(e_no_quickfix_پشته));

    بازگشت qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ایستا qf_info_T *
qf_cmd_get_or_alloc_پشته(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	اگر (qi == پوچ)
	    بازگشت پوچ;
	*pwinp = curwin;
    }

    بازگشت qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ایستا صحیح
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    صحیح		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_اقلام(from_qfl, from_qfp, i)
    {
	اگر (qf_add_entry(to_qfl,
		    پوچ,
		    پوچ,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_text,
		    from_qfp->qf_lnum,
		    from_qfp->qf_end_lnum,
		    from_qfp->qf_col,
		    from_qfp->qf_end_col,
		    from_qfp->qf_viscol,
		    from_qfp->qf_pattern,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_data,
		    from_qfp->qf_valid) == QF_FAIL)
	    بازگشت FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	اگر (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    بازگشت OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ایستا صحیح
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_شمار = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = پوچ;
    to_qfl->qf_last = پوچ;
    to_qfl->qf_ptr = پوچ;
    اگر (from_qfl->qf_title != پوچ)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    وگرنه
	to_qfl->qf_title = پوچ;
    اگر (from_qfl->qf_ctx != پوچ)
    {
	to_qfl->qf_ctx = alloc_tv();
	اگر (to_qfl->qf_ctx != پوچ)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    وگرنه
	to_qfl->qf_ctx = پوچ;
    اگر (from_qfl->qf_qftf_cb.cb_name != پوچ)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    وگرنه
	to_qfl->qf_qftf_cb.cb_name = پوچ;

    اگر (from_qfl->qf_شمار)
	اگر (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    بازگشت FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    اگر (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    بازگشت OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    تهی
copy_loclist_پشته(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    صحیح		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    اگر (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    وگرنه
	qi = from->w_llist;

    اگر (qi == پوچ)		    // no location list to copy
	بازگشت;

    // allocate a new location list, set size of stack to 'from' window value
    اگر ((to->w_llist = qf_alloc_پشته(QFLT_LOCATION, from->w_p_lhi)) == پوچ)
	بازگشت;
    وگرنه
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    برای (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	اگر (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    بازگشت;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ایستا صحیح
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {پوچ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    اگر (fname == پوچ || *fname == NUL)		// no file name
	بازگشت 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    اگر (directory != پوچ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    اگر (directory != پوچ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != پوچ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	اگر (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    اگر (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    وگرنه
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    اگر (ptr.string == پوچ)
		بازگشت 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    وگرنه
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    اگر (qf_last_bufname != پوچ && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    وگرنه
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, پوچ, (linenr_T)0, BLN_NOOPT);
	اگر (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	وگرنه
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    اگر (buf == پوچ)
	بازگشت 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    بازگشت buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ایستا char_u *
qf_افزودن_dir(char_u *dirbuf, ساختار dir_پشته_T **stackptr, صحیح is_file_پشته)
{
    ساختار dir_پشته_T  *ds_new;
    ساختار dir_پشته_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ساختار dir_پشته_T, aid_qf_dirstack);
    اگر (ds_new == پوچ)
	بازگشت پوچ;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    اگر (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == پوچ
	    || is_file_پشته)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    وگرنه
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = پوچ;
	وقتی (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    اگر (dirname.string == پوچ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		بازگشت پوچ;
	    }
	    اگر (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		بشکن;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	وقتی ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	اگر (ds_new == پوچ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    اگر ((*stackptr)->dirname != پوچ)
	بازگشت (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    بازگشت پوچ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ایستا char_u *
qf_برداشت_dir(ساختار dir_پشته_T **stackptr)
{
    ساختار dir_پشته_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    اگر (*stackptr != پوچ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    بازگشت *stackptr ? (*stackptr)->dirname : پوچ;
}

/*
 * clean up directory stack
 */
    ایستا تهی
qf_clean_dir_پشته(ساختار dir_پشته_T **stackptr)
{
    ساختار dir_پشته_T  *ds_ptr;

    وقتی ((ds_ptr = *stackptr) != پوچ)
    {
	*stackptr = (*stackptr)->next;
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
    ایستا char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ساختار dir_پشته_T     *ds_ptr;
    ساختار dir_پشته_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    اگر (qfl->qf_dir_پشته == پوچ)
	بازگشت پوچ;

    ds_ptr = qfl->qf_dir_پشته->next;
    fullname.string = پوچ;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    وقتی (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	اگر ((fullname.string != پوچ) && (mch_getperm(fullname.string) >= 0))
	    بشکن;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    وقتی (qfl->qf_dir_پشته->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_پشته->next;
	qfl->qf_dir_پشته->next = qfl->qf_dir_پشته->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    بازگشت ds_ptr == پوچ ? پوچ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ایستا صحیح
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    صحیح		i;

    اگر (wp != پوچ)
    {
	اگر (!win_valid(wp))
	    بازگشت FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    اگر (qi == پوچ)
	بازگشت FALSE;

    برای (i = 0; i < qi->qf_listcount; ++i)
	اگر (qi->qf_lists[i].qf_id == qf_id)
	    بازگشت TRUE;

    بازگشت FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ایستا صحیح
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    صحیح		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_اقلام(qfl, qfp, i)
	اگر (qfp == qf_ptr)
	    بشکن;

    اگر (i > qfl->qf_شمار) // Entry is not found
	بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ایستا qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحیح		*qf_index,
	صحیح		dir)
{
    صحیح			idx;
    صحیح			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    انجامبده
    {
	اگر (idx == qfl->qf_شمار || qf_ptr->qf_next == پوچ)
	    بازگشت پوچ;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } وقتی ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    بازگشت qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ایستا qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحیح		*qf_index,
	صحیح		dir)
{
    صحیح			idx;
    صحیح			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    انجامبده
    {
	اگر (idx == 1 || qf_ptr->qf_prev == پوچ)
	    بازگشت پوچ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } وقتی ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    بازگشت qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ایستا qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	صحیح		errornr,
	صحیح		dir,
	صحیح		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    صحیح			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    صحیح			prev_index;
    نویسه		*err = e_no_more_اقلام;

    وقتی (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	اگر (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	وگرنه
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	اگر (qf_ptr == پوچ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    اگر (err != پوچ)
	    {
		emsg(_(err));
		بازگشت پوچ;
	    }
	    بشکن;
	}

	err = پوچ;
    }

    *new_qfidx = qf_idx;
    بازگشت qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ایستا qfline_T *
get_nth_entry(qf_list_T *qfl, صحیح errornr, صحیح *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحیح		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    وقتی (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != پوچ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    وقتی (errornr > qf_idx && qf_idx < qfl->qf_شمار &&
						qf_ptr->qf_next != پوچ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    بازگشت qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ایستا qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	صحیح		errornr,
	صحیح		dir,
	صحیح		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحیح		qfidx = qfl->qf_index;

    اگر (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    وگرنه اگر (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    بازگشت qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ایستا win_T *
qf_find_help_win(تهی)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	اگر (bt_help(wp->w_buffer))
	    بازگشت wp;

    بازگشت پوچ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ایستا تهی
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ایستا صحیح
jump_to_help_window(qf_info_T *qi, صحیح newwin, صحیح *opened_window)
{
    win_T	*wp;
    صحیح		flags;

    اگر (cmdmod.cmod_tab != 0 || newwin)
	wp = پوچ;
    وگرنه
	wp = qf_find_help_win();
    اگر (wp != پوچ && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    وگرنه
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	اگر (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_بالاترین;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	اگر (IS_LL_پشته(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	اگر (win_split(0, flags) == FAIL)
	    بازگشت FAIL;

	*opened_window = TRUE;

	اگر (curwin->w_height < p_hh)
	    win_setheight((صحیح)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	اگر (IS_LL_پشته(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    اگر (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    بازگشت OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ایستا win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    بازگشت wp;

    بازگشت پوچ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ایستا win_T *
qf_find_win_with_normal_buf(تهی)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (bt_normal(wp->w_buffer))
	    بازگشت wp;

    بازگشت پوچ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ایستا صحیح
qf_goto_tabwin_with_file(صحیح fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    بازگشت TRUE;
	}

    بازگشت FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ایستا صحیح
qf_open_new_file_win(qf_info_T *ll_ref)
{
    صحیح		flags;

    flags = WSP_ABOVE;
    اگر (ll_ref != پوچ)
	flags |= WSP_NEWLOC;
    اگر (win_split(0, flags) == FAIL)
	بازگشت FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    اگر (ll_ref != پوچ)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    بازگشت OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ایستا تهی
qf_goto_win_with_ll_file(win_T *use_win, صحیح qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    اگر (win == پوچ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    اگر (win->w_buffer->b_fnum == qf_fnum)
		بشکن;
	اگر (win == پوچ)
	{
	    // Find a previous usable window
	    win = curwin;
	    انجامبده
	    {
		اگر (bt_normal(win->w_buffer))
		    بشکن;
		اگر (win->w_prev == پوچ)
		    win = lastwin;	// wrap around the top
		وگرنه
		    win = win->w_prev; // go to previous window
	    } وقتی (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    اگر (win->w_llist == پوچ && ll_ref != پوچ)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ایستا تهی
qf_goto_win_with_qfl_file(صحیح qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = پوچ;
    برای (;;)
    {
	اگر (win->w_buffer->b_fnum == qf_fnum)
	    بشکن;
	اگر (win->w_prev == پوچ)
	    win = lastwin;	// wrap around the top
	وگرنه
	    win = win->w_prev;	// go to previous window

	اگر (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    اگر ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    وگرنه اگر (altwin != پوچ)
		win = altwin;
	    وگرنه اگر (curwin->w_prev != پوچ)
		win = curwin->w_prev;
	    وگرنه
		win = curwin->w_next;
	    بشکن;
	}

	// Remember a usable window.
	اگر (altwin == پوچ && !win->w_p_pvw && !win->w_p_wfb &&
		bt_normal(win->w_buffer))
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
    ایستا صحیح
qf_jump_to_usable_window(صحیح qf_fnum, صحیح newwin, صحیح *opened_window)
{
    win_T	*usable_wp = پوچ;
    صحیح		usable_win = FALSE;
    qf_info_T	*ll_ref = پوچ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    اگر (!newwin)
	ll_ref = curwin->w_llist_ref;

    اگر (ll_ref != پوچ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	اگر (usable_wp != پوچ)
	    usable_win = TRUE;
    }

    اگر (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	اگر (win != پوچ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    اگر (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    اگر ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	اگر (qf_open_new_file_win(ll_ref) != OK)
	    بازگشت FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    وگرنه
    {
	اگر (curwin->w_llist_ref != پوچ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	وگرنه					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    بازگشت OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ایستا صحیح
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحیح		forceit,
	صحیح		prev_winid,
	صحیح		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحیح		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    صحیح		retval = OK;
    صحیح		old_qf_curlist = qi->qf_curlist;
    صحیح		save_qfid = qfl->qf_id;

    اگر (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	اگر (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    بازگشت FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, پوچ, پوچ, پوچ, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : پوچ);
    }
    وگرنه
    {
	صحیح	fnum = qf_ptr->qf_fnum;

	اگر (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    اگر (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		بازگشت FAIL;
	    }

	    اگر (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    اگر (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		اگر (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		اگر (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_to_buffer));
		    retval = FAIL;
		}
	    }
	}

	اگر (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    اگر (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	اگر (wp == پوچ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    بازگشت QF_ABORT;
	}
    }

    اگر (qfl_type == QFLT_QUICKFIX && !qflist_valid(پوچ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	بازگشت QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	وگرنه
	    emsg(_(e_current_location_list_was_changed));
	بازگشت QF_ABORT;
    }

    بازگشت retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ایستا صحیح
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*سطر = ml_get_curline();
    char_u	*p = سطر;
    colnr_T	col = 0;

    وقتی (*p != NUL && col < vcol)
    {
	اگر (*p == TAB)
	    col += 8 - (col % 8);
	وگرنه
	    col += ptr2cells(p);
	اگر (col > vcol)
	    بشکن;
	MB_PTR_ADV(p);
    }
    بازگشت (صحیح)(p - سطر);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ایستا تهی
qf_jump_goto_سطر(
	linenr_T	qf_lnum,
	صحیح		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    اگر (qf_pattern == پوچ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	اگر (i > 0)
	{
	    اگر (i > curbuf->b_ml.ml_سطر_شمار)
		i = curbuf->b_ml.ml_سطر_شمار;
	    curwin->w_cursor.lnum = i;
	}
	اگر (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    اگر (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    وگرنه
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	وگرنه
	    beginline(BL_WHITE | BL_FIX);
    }
    وگرنه
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	اگر (!do_جستجو(پوچ, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, جستجو_KEEP, پوچ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ایستا تهی
qf_jump_print_پیام(
	qf_info_T	*qi,
	صحیح		qf_index,
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
    اگر (!پیام_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((نویسه *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_شمار,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (نویسه *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = پیام_scroll;
    اگر (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	پیام_scroll = TRUE;
    وگرنه اگر (!پیام_scrolled && shortmess(SHM_OVERALL))
	پیام_scroll = FALSE;
    پیام_attr_keep((نویسه *)gap->ga_data, 0, TRUE);
    پیام_scroll = i;

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
    ایستا صحیح
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحیح		newwin,
	صحیح		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحیح		old_changedtick = qfl->qf_changedtick;
    صحیح		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    اگر (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	اگر (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    بازگشت FAIL;
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	وگرنه
	    emsg(_(e_current_location_list_was_changed));
	بازگشت QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    اگر (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	اگر (qf_ptr->qf_fnum == 0)
	    بازگشت NOTDONE;

	اگر (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    بازگشت FAIL;
    }
    اگر (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	اگر (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	وگرنه
	    emsg(_(e_current_location_list_was_changed));
	بازگشت QF_ABORT;
    }

    بازگشت OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ایستا صحیح
qf_jump_to_buffer(
	qf_info_T	*qi,
	صحیح		qf_index,
	qfline_T	*qf_ptr,
	صحیح		forceit,
	صحیح		prev_winid,
	صحیح		*opened_window,
	صحیح		openfold,
	صحیح		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    صحیح		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    اگر (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	اگر (retval != OK)
	    بازگشت retval;
    }

    // When not switched to another buffer, still need to set pc mark
    اگر (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_سطر(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    اگر ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    اگر (print_message)
	qf_jump_print_پیام(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    بازگشت retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    تهی
qf_jump(qf_info_T	*qi,
	صحیح		dir,
	صحیح		errornr,
	صحیح		forceit)
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
    ایستا تهی
qf_jump_newwin(qf_info_T	*qi,
	صحیح		dir,
	صحیح		errornr,
	صحیح		forceit,
	صحیح		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    صحیح			qf_index;
    صحیح			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    صحیح			prev_winid;
    صحیح			opened_window = FALSE;
    صحیح			print_message = TRUE;
    صحیح			old_کلید_Typed = کلید_Typed; // getting file may reset it
    صحیح			retval = OK;

    اگر (qi == پوچ)
    {
	اگر (ql_info == پوچ)
	{
	    emsg(_(e_no_quickfix_پشته));
	    بازگشت;
	}
	qi = ql_info;
    }

    اگر (qf_پشته_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	بازگشت;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    اگر (qf_ptr == پوچ)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	برو theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    اگر (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    اگر (retval == FAIL)
	برو failed;
    اگر (retval == QF_ABORT)
    {
	qi = پوچ;
	qf_ptr = پوچ;
	برو theend;
    }
    اگر (retval == NOTDONE)
	برو theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_کلید_Typed, print_message);
    اگر (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = پوچ;
	qf_ptr = پوچ;
    }

    اگر (retval != OK)
    {
	اگر (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	اگر (qf_ptr != پوچ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    اگر (qi != پوچ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    اگر (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ایستا صحیح	qfFileAttr;
ایستا صحیح	qfSepAttr;
ایستا صحیح	qf_سطر_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ایستا تهی
qf_list_entry(qfline_T *qfp, صحیح qf_idx, صحیح cursel)
{
    char_u	*fname;
    buf_T	*buf;
    صحیح		filter_entry;
    garray_T	*gap;

    fname = پوچ;
    اگر (qfp->qf_module != پوچ && *qfp->qf_module != NUL)
	vim_snprintf((نویسه *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(نویسه *)qfp->qf_module);
    وگرنه
    {
	اگر (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != پوچ)
	{
	    اگر (qfp->qf_fname == پوچ)
		fname = buf->b_fname;
	    وگرنه
		fname = qfp->qf_fname;
	    اگر (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	اگر (fname == پوچ)
	    sprintf((نویسه *)IObuff, "%2d", qf_idx);
	وگرنه
	    vim_snprintf((نویسه *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (نویسه *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    اگر (qfp->qf_module != پوچ && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    اگر (filter_entry && fname != پوچ)
	filter_entry &= message_filtered(fname);
    اگر (filter_entry && qfp->qf_pattern != پوچ)
	filter_entry &= message_filtered(qfp->qf_pattern);
    اگر (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    اگر (filter_entry)
	بازگشت;

    پیام_putchar('\n');
    پیام_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    اگر (qfp->qf_lnum != 0)
	پیام_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    اگر (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    پیام_puts_attr((نویسه *)gap->ga_data, qf_سطر_Attr);
    پیام_puts_attr(":", qfSepAttr);
    اگر (qfp->qf_pattern != پوچ)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	پیام_puts((نویسه *)gap->ga_data);
	پیام_puts_attr(":", qfSepAttr);
    }
    پیام_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != پوچ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    پیام_prt_سطر((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    تهی
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i;
    صحیح		idx1 = 1;
    صحیح		idx2 = -1;
    char_u	*arg = eap->arg;
    صحیح		plus = FALSE;
    صحیح		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    اگر (qf_پشته_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	بازگشت;
    }
    اگر (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    اگر (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	بازگشت;
    }
    qfl = qf_get_curlist(qi);
    اگر (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    وگرنه
    {
	i = qfl->qf_شمار;
	اگر (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	اگر (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    اگر (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    اگر (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_سطر_Attr = syn_name2attr((char_u *)"qfLineNr");
    اگر (qf_سطر_Attr == 0)
	qf_سطر_Attr = HL_ATTR(HLF_N);

    اگر (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_اقلام(qfl, qfp, i)
    {
	اگر ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ایستا تهی
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    وقتی (*p != NUL)
    {
	اگر (*p == '\n')
	{
	    ga_append(gap, ' ');
	    وقتی (*++p != NUL)
		اگر (!VIM_ISWHITE(*p) && *p != '\n')
		    بشکن;
	}
	وگرنه
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ایستا تهی
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((نویسه *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    اگر (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((نویسه *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    اگر (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((نویسه *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	اگر (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((نویسه *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ایستا تهی
qf_پیام(qf_info_T *qi, صحیح which, نویسه *lead)
{
    نویسه   *title = (نویسه *)qi->qf_lists[which].qf_title;
    صحیح    شمار = qi->qf_lists[which].qf_شمار;
    char_u buf[IOSIZE];

    vim_snprintf((نویسه *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    شمار);

    اگر (title != پوچ)
    {
	size_t	len = STRLEN(buf);

	اگر (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    پیام((نویسه *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    تهی
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		شمار;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    اگر (eap->addr_شمار != 0)
	شمار = eap->line2;
    وگرنه
	شمار = 1;
    وقتی (شمار--)
    {
	اگر (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    اگر (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_پشته));
		بشکن;
	    }
	    --qi->qf_curlist;
	}
	وگرنه
	{
	    اگر (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_بالاترین_of_quickfix_پشته));
		بشکن;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_پیام(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, پوچ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    تهی
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_پشته(eap, FALSE);
    صحیح		i;

    اگر (eap->addr_شمار > 0)
    {
	اگر (qi == پوچ)
	{
	    emsg(_(e_no_location_list));
	    بازگشت;
	}

	// Jump to the specified quickfix list
	اگر (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_پیام(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, پوچ);
	}
	وگرنه
	    emsg(_(e_invalid_range));

	بازگشت;
    }

    اگر (qf_پشته_empty(qi))
	پیام(_("No entries"));
    وگرنه
	برای (i = 0; i < qi->qf_listcount; ++i)
	    qf_پیام(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ایستا تهی
qf_free_اقلام(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    صحیح		stop = FALSE;

    وقتی (qfl->qf_شمار && qfl->qf_start != پوچ)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	اگر (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    اگر (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_شمار = 1;
	    وگرنه
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_شمار;
    }

    qfl->qf_index = 0;
    qfl->qf_start = پوچ;
    qfl->qf_last = پوچ;
    qfl->qf_ptr = پوچ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_پشته(&qfl->qf_dir_پشته);
    qfl->qf_directory = پوچ;
    qf_clean_dir_پشته(&qfl->qf_file_پشته);
    qfl->qf_currfile = پوچ;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ایستا تهی
qf_free(qf_list_T *qfl)
{
    qf_free_اقلام(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = پوچ;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   تهی
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    صحیح		i;
    qfline_T	*qfp;
    صحیح		idx;
    qf_info_T	*qi = ql_info;
    صحیح		found_one = FALSE;
    صحیح		buf_has_flag = wp == پوچ ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    اگر (!(curbuf->b_has_qf_entry & buf_has_flag))
	بازگشت;
    اگر (wp != پوچ)
    {
	اگر (wp->w_llist == پوچ)
	    بازگشت;
	qi = wp->w_llist;
    }
    وگرنه اگر (qi == پوچ)
	بازگشت;

    برای (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	اگر (!qf_list_empty(qfl))
	    FOR_ALL_QFL_اقلام(qfl, qfp, i)
		اگر (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    اگر (qfp->qf_cleared)
			ادامه;

		    اگر (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			اگر (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			وگرنه
			    qfp->qf_lnum += amount;
		    }
		    وگرنه اگر (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    اگر (!found_one)
	curbuf->b_has_qf_entry &= ~buf_has_flag;
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
    ایستا char_u *
qf_types(صحیح c, صحیح nr)
{
    ایستا char_u	buf[20];
    ایستا char_u	cc[3];
    char_u		*p;

    اگر (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    وگرنه اگر (c == 'I' || c == 'i')
	p = (char_u *)" info";
    وگرنه اگر (c == 'N' || c == 'n')
	p = (char_u *)" note";
    وگرنه اگر (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    وگرنه اگر (c == 0 || c == 1)
	p = (char_u *)"";
    وگرنه
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    اگر (nr <= 0)
	بازگشت p;

    sprintf((نویسه *)buf, "%s %3d", (نویسه *)p, nr);
    بازگشت buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    تهی
qf_view_result(صحیح split)
{
    qf_info_T   *qi = ql_info;

    اگر (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    وگرنه اگر (qi == پوچ)
    {
	emsg(_(e_no_quickfix_پشته));
	بازگشت;
    }

    اگر (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	بازگشت;
    }

    اگر (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	بازگشت;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    تهی
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    اگر (qf_پشته_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	اگر (win != پوچ)
	    ex_cclose(eap);
    }
    وگرنه اگر (win == پوچ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    تهی
ex_cclose(exarg_T *eap)
{
    win_T	*win = پوچ;
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_پشته(eap, FALSE)) == پوچ)
	بازگشت;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    اگر (win != پوچ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ایستا تهی
qf_set_title_var(qf_list_T *qfl)
{
    اگر (qfl->qf_title != پوچ)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ایستا صحیح
qf_goto_cwindow(qf_info_T *qi, صحیح resize, صحیح sz, صحیح vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    اگر (win == پوچ)
	بازگشت FAIL;

    win_goto(win);
    اگر (resize)
    {
	اگر (vertsplit)
	{
	    اگر (sz != win->w_width)
		win_setwidth(sz);
	}
	وگرنه اگر (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    بازگشت OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ایستا تهی
qf_set_cwindow_options(تهی)
{
    // switch off 'swapfile'
    set_option_مقدار_give_err((char_u *)"swf", 0L, پوچ, OPT_LOCAL);
    set_option_مقدار_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_مقدار_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_مقدار_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ایستا صحیح
qf_open_new_cwindow(qf_info_T *qi, صحیح height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    صحیح		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    اگر (IS_QF_پشته(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    اگر (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    اگر (IS_QF_پشته(qi))
	flags |= WSP_QUICKFIX;

    اگر (win_split(height, flags) == FAIL)
	بازگشت FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    اگر (IS_LL_پشته(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    اگر (oldwin != curwin)
	oldwin = پوچ;  // don't store info when in another window
    اگر (qf_buf != پوچ)
    {
	// Use the existing quickfix buffer
	اگر (do_ecmd(qf_buf->b_fnum, پوچ, پوچ, پوچ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	// Create a new quickfix buffer
	اگر (do_ecmd(0, پوچ, پوچ, پوچ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    بازگشت FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    اگر (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    اگر (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    اگر (win_valid(win))
	prevwin = win;

    بازگشت OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    تهی
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحیح		height;
    صحیح		status = FAIL;
    صحیح		lnum;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    incr_quickfix_busy();

    اگر (eap->addr_شمار != 0)
	height = eap->line2;
    وگرنه
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    اگر (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_شمار != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    اگر (status == FAIL)
	اگر (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    بازگشت;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, پوچ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ایستا تهی
qf_win_goto(win_T *win, linenr_T lnum)
{
    win_T	*old_curwin = curwin;

    curwin = win;
    curbuf = win->w_buffer;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_status = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * :cbottom/:lbottom commands.
 */
    تهی
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    win = qf_find_win(qi);
    اگر (win != پوچ && win->w_cursor.lnum != win->w_buffer->b_ml.ml_سطر_شمار)
	qf_win_goto(win, win->w_buffer->b_ml.ml_سطر_شمار);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    اگر (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    وگرنه اگر (qi == پوچ)
	بازگشت 0;

    بازگشت qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ایستا صحیح
qf_win_pos_update(
    qf_info_T	*qi,
    صحیح		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    صحیح		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    اگر (win != پوچ
	    && qf_index <= win->w_buffer->b_ml.ml_سطر_شمار
	    && old_qf_index != qf_index)
    {
	اگر (qf_index > old_qf_index)
	{
	    win->w_redraw_بالاترین = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	وگرنه
	{
	    win->w_redraw_بالاترین = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    بازگشت win != پوچ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ایستا صحیح
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    اگر (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	اگر ((IS_QF_پشته(qi) && win->w_llist_ref == پوچ)
		|| (IS_LL_پشته(qi) && win->w_llist_ref == qi))
	    بازگشت TRUE;

    بازگشت FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ایستا win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	اگر (is_qf_win(win, qi))
	    بازگشت win;
    بازگشت پوچ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ایستا buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    اگر (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	اگر (qfbuf != پوچ)
	    بازگشت qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	اگر (is_qf_win(win, qi))
	    بازگشت win->w_buffer;

    بازگشت پوچ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    نویسه *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    اگر (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ایستا تهی
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	اگر (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_set_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    ایستا تهی
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    اگر (buf == پوچ)
	بازگشت;

    linenr_T	old_سطر_شمار = buf->b_ml.ml_سطر_شمار;
    صحیح		qf_winid = 0;

    اگر (IS_LL_پشته(qi))
    {
	اگر (curwin->w_llist == qi)
	    win = curwin;
	وگرنه
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    اگر (win == پوچ)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    اگر (win == پوچ)
		بازگشت;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    صحیح do_fill = TRUE;
    اگر (old_last == پوچ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    اگر (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	اگر (old_last == پوچ)
	{
	    (تهی)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    اگر ((win = qf_find_win(qi)) != پوچ && old_سطر_شمار < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ایستا صحیح
qf_buf_add_سطر(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	صحیح		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    اگر (qftf_str != پوچ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    وگرنه
    {
	اگر (qfp->qf_module != پوچ)
	    ga_concat(gap, qfp->qf_module);
	وگرنه اگر (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != پوچ
		&& errbuf->b_fname != پوچ)
	{
	    اگر (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    وگرنه
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		اگر (first_bufline && (errbuf->b_sfname == پوچ
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    اگر (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		اگر (qfp->qf_fname == پوچ)
		    ga_concat(gap, errbuf->b_fname);
		وگرنه
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	اگر (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	وگرنه اگر (qfp->qf_pattern != پوچ)
	    qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_text(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_text)
							       : qfp->qf_text);
    }

    ga_append(gap, NUL);
    اگر (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ایستا list_T *
call_qftf_func(qf_list_T *qfl, صحیح qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = پوچ;
    ایستا صحیح	recursive = FALSE;

    اگر (recursive)
	بازگشت پوچ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    اگر (qfl->qf_qftf_cb.cb_name != پوچ)
	cb = &qfl->qf_qftf_cb;
    اگر (cb->cb_name != پوچ)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	اگر ((d = dict_alloc_lock(VAR_FIXED)) == پوچ)
	{
	    recursive = FALSE;
	    بازگشت پوچ;
	}
	dict_add_عدد(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_عدد(d, "winid", (long)qf_winid);
	dict_add_عدد(d, "id", (long)qfl->qf_id);
	dict_add_عدد(d, "start_idx", start_idx);
	dict_add_عدد(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = پوچ;
	textlock++;
	اگر (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    اگر (rettv.v_type == VAR_LIST)
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
    بازگشت qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ایستا تهی
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, صحیح qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    صحیح		old_کلید_Typed = کلید_Typed;
    list_T	*qftf_list = پوچ;
    listitem_T	*qftf_li = پوچ;

    اگر (old_last == پوچ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	اگر (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    بازگشت;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	وقتی ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (تهی)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    اگر (qfl != پوچ && qfl->qf_start != پوچ)
    {
	char_u		dirname[MAXPATHL];
	صحیح		invalid_val = FALSE;
	صحیح		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	اگر (old_last == پوچ)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	وگرنه
	{
	    اگر (old_last->qf_next != پوچ)
		qfp = old_last->qf_next;
	    وگرنه
		qfp = old_last;
	    lnum = buf->b_ml.ml_سطر_شمار;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_شمار);
	اگر (qftf_list != پوچ)
	    qftf_li = qftf_list->lv_first;

	وقتی (lnum < qfl->qf_شمار)
	{
	    char_u	*qftf_str = پوچ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    اگر (qftf_li != پوچ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		اگر (qftf_str == پوچ)
		    invalid_val = TRUE;
	    }

	    اگر (qf_buf_add_سطر(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		بشکن;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    اگر (qfp == پوچ)
		بشکن;

	    اگر (qftf_li != پوچ)
		qftf_li = qftf_li->li_next;
	}

	اگر (old_last == پوچ)
	    // Delete the empty line which is now at the end
	    (تهی)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    اگر (old_last == پوچ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_مقدار_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", پوچ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", پوچ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    کلید_Typed = old_کلید_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ایستا تهی
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ایستا صحیح
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    صحیح		qf_idx;

    برای (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	اگر (qi->qf_lists[qf_idx].qf_id == qfid)
	    بازگشت qf_idx;
    بازگشت INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ایستا صحیح
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    صحیح curlist;

    اگر (qf_get_curlist(qi)->qf_id == save_qfid)
	بازگشت OK;

    curlist = qf_id2nr(qi, save_qfid);
    اگر (curlist < 0)
	// list is not present
	بازگشت FAIL;
    qi->qf_curlist = curlist;
    بازگشت OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ایستا تهی
qf_jump_first(qf_info_T *qi, int_u save_qfid, صحیح forceit)
{
    اگر (qf_restore_list(qi, save_qfid) == FAIL)
	بازگشت;


    اگر (!check_can_set_curbuf_forceit(forceit))
	بازگشت;


    // Autocommands might have cleared the list, check for that.
    اگر (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    صحیح
grep_internal(cmdidx_T cmdidx)
{
    بازگشت ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ایستا char_u *
make_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_make:	    بازگشت (char_u *)"make";
	حالت CMD_lmake:	    بازگشت (char_u *)"lmake";
	حالت CMD_grep:	    بازگشت (char_u *)"grep";
	حالت CMD_lgrep:	    بازگشت (char_u *)"lgrep";
	حالت CMD_grepadd:   بازگشت (char_u *)"grepadd";
	حالت CMD_lgrepadd:  بازگشت (char_u *)"lgrepadd";
	پیشفرض: بازگشت پوچ;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ایستا char_u *
get_mef_name(تهی)
{
    char_u	*p;
    char_u	*name;
    ایستا صحیح	start = -1;
    ایستا صحیح	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    اگر (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	اگر (name == پوچ)
	    emsg(_(e_cant_get_temp_file_name));
	بازگشت name;
    }

    برای (p = p_mef; *p; ++p)
	اگر (p[0] == '#' && p[1] == '#')
	    بشکن;

    اگر (*p == NUL)
	بازگشت vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    برای (;;)
    {
	اگر (start == -1)
	    start = mch_get_pid();
	وگرنه
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	اگر (name == پوچ)
	    بشکن;
	STRCPY(name, p_mef);
	sprintf((نویسه *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	اگر (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((نویسه *)name, &sb) < 0
# endif
		)
	    بشکن;
	vim_free(name);
    }
    بازگشت name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ایستا char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    اگر (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    اگر (cmd == پوچ)
	بازگشت پوچ;
    sprintf((نویسه *)cmd, "%s%s%s", (نویسه *)p_shq, (نویسه *)makecmd,
							       (نویسه *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    اگر (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    اگر (پیام_col == 0)
	پیام_didout = FALSE;
    پیام_start();
    پیام_puts(":!");
    پیام_outtrans(cmd);		// show what we are doing

    بازگشت cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    تهی
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = پوچ;
    win_T	*wp = پوچ;
    qf_info_T	*qi = ql_info;
    صحیح		res;
    char_u	*au_name = پوچ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    صحیح		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    اگر (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	بازگشت;
    }

    au_name = make_get_auname(eap->cmdidx);
    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    بازگشت;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    اگر (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    اگر (fname == پوچ)
	بازگشت;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    اگر (cmd == پوچ)
    {
	vim_free(fname);
	بازگشت;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (تهی)char_avail();
# endif

    incr_quickfix_busy();

    اگر (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    اگر (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    اگر (wp != پوچ)
    {
	qi = GET_LOC_LIST(wp);
	اگر (qi == پوچ)
	    برو cleanup;
    }
    وگرنه اگر (qi == پوچ)
	برو cleanup;

    اگر (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != پوچ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    اگر (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    صحیح
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_پشته(eap, FALSE)) == پوچ)
	بازگشت 0;
    بازگشت qf_get_curlist(qi)->qf_شمار;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    صحیح
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i, sz = 0;
    صحیح		prev_fnum = 0;

    اگر ((qi = qf_cmd_get_پشته(eap, FALSE)) == پوچ)
	بازگشت 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_اقلام(qfl, qfp, i)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    وگرنه اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    بازگشت sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    صحیح
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    اگر ((qi = qf_cmd_get_پشته(eap, FALSE)) == پوچ)
	بازگشت 0;

    بازگشت qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    صحیح
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i, eidx = 0;
    صحیح		prev_fnum = 0;

    اگر ((qi = qf_cmd_get_پشته(eap, FALSE)) == پوچ)
	بازگشت 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
	بازگشت 1;

    برای (i = 1; i <= qfl->qf_index && qfp!= پوچ; i++, qfp = qfp->qf_next)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    وگرنه
		eidx++;
	}
    }

    بازگشت eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ایستا صحیح
qf_get_nth_valid_entry(qf_list_T *qfl, صحیح n, صحیح fdo)
{
    qfline_T	*qfp;
    صحیح		i, eidx;
    صحیح		prev_fnum = 0;

    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
	بازگشت 1;

    eidx = 0;
    FOR_ALL_QFL_اقلام(qfl, qfp, i)
    {
	اگر (qfp->qf_valid)
	{
	    اگر (fdo)
	    {
		اگر (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    وگرنه
		eidx++;
	}

	اگر (eidx == n)
	    بشکن;
    }

    اگر (i <= qfl->qf_شمار)
	بازگشت i;
    وگرنه
	بازگشت 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    تهی
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		errornr;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    اگر (eap->addr_شمار > 0)
	errornr = (صحیح)eap->line2;
    وگرنه
    {
	انتخاب (eap->cmdidx)
	{
	    حالت CMD_cc: حالت CMD_ll:
		errornr = 0;
		بشکن;
	    حالت CMD_crewind: حالت CMD_lrewind: حالت CMD_cfirst:
	    حالت CMD_lfirst:
		errornr = 1;
		بشکن;
	    پیشفرض:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_شمار > 0 ? (صحیح)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    تهی
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    صحیح		errornr;
    صحیح		dir;

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    اگر (eap->addr_شمار > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (صحیح)eap->line2;
    وگرنه
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    انتخاب (eap->cmdidx)
    {
	حالت CMD_cnext: حالت CMD_lnext: حالت CMD_cdo: حالت CMD_ldo:
	    dir = FORWARD;
	    بشکن;
	حالت CMD_cprevious: حالت CMD_lprevious: حالت CMD_cNext:
	حالت CMD_lNext:
	    dir = BACKWARD;
	    بشکن;
	حالت CMD_cnfile: حالت CMD_lnfile: حالت CMD_cfdo: حالت CMD_lfdo:
	    dir = FORWARD_FILE;
	    بشکن;
	حالت CMD_cpfile: حالت CMD_lpfile: حالت CMD_cNfile: حالت CMD_lNfile:
	    dir = BACKWARD_FILE;
	    بشکن;
	پیشفرض:
	    dir = FORWARD;
	    بشکن;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ایستا qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, صحیح bnr, صحیح *errornr)
{
    qfline_T	*qfp = پوچ;
    صحیح		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_اقلام(qfl, qfp, idx)
	اگر (qfp->qf_fnum == bnr)
	    بشکن;

    *errornr = idx;
    بازگشت qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ایستا qfline_T *
qf_find_first_entry_on_سطر(qfline_T *entry, صحیح *errornr)
{
    وقتی (!got_int
	    && entry->qf_prev != پوچ
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    بازگشت entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ایستا qfline_T *
qf_find_last_entry_on_سطر(qfline_T *entry, صحیح *errornr)
{
    وقتی (!got_int &&
	    entry->qf_next != پوچ
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    بازگشت entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ایستا صحیح
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	بازگشت qfp->qf_lnum > pos->lnum;
    وگرنه
	بازگشت (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ایستا صحیح
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	بازگشت qfp->qf_lnum < pos->lnum;
    وگرنه
	بازگشت (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ایستا صحیح
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	بازگشت qfp->qf_lnum >= pos->lnum;
    وگرنه
	بازگشت (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ایستا صحیح
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, صحیح linewise)
{
    اگر (linewise)
	بازگشت qfp->qf_lnum <= pos->lnum;
    وگرنه
	بازگشت (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col <= pos->col));
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
    ایستا qfline_T *
qf_find_entry_after_pos(
	صحیح		bnr,
	pos_T		*pos,
	صحیح		linewise,
	qfline_T	*qfp,
	صحیح		*errornr)
{
    اگر (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	بازگشت qfp;

    // Find the entry just before or at the position 'pos'
    وقتی (qfp->qf_next != پوچ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    اگر (qfp->qf_next == پوچ || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	بازگشت پوچ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    بازگشت qfp;
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
    ایستا qfline_T *
qf_find_entry_before_pos(
	صحیح		bnr,
	pos_T		*pos,
	صحیح		linewise,
	qfline_T	*qfp,
	صحیح		*errornr)
{
    // Find the entry just before the position 'pos'
    وقتی (qfp->qf_next != پوچ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    اگر (qf_entry_on_or_after_pos(qfp, pos, linewise))
	بازگشت پوچ;

    اگر (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_سطر(qfp, errornr);

    بازگشت qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ایستا qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	صحیح		bnr,
	pos_T		*pos,
	صحیح		dir,
	صحیح		linewise,
	صحیح		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    اگر (qfp == پوچ)
	بازگشت پوچ;		// no entry in this file

    اگر (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    وگرنه
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    بازگشت qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ایستا تهی
qf_get_nth_below_entry(qfline_T *entry_arg, صحیح n, صحیح linewise, صحیح *errornr)
{
    qfline_T *entry = entry_arg;

    وقتی (n-- > 0 && !got_int)
    {
	صحیح		first_errornr = *errornr;

	اگر (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_سطر(entry, errornr);

	اگر (entry->qf_next == پوچ
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    اگر (linewise)
		*errornr = first_errornr;
	    بشکن;
	}

	entry = entry->qf_next;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ایستا تهی
qf_get_nth_above_entry(qfline_T *entry, صحیح n, صحیح linewise, صحیح *errornr)
{
    وقتی (n-- > 0 && !got_int)
    {
	اگر (entry->qf_prev == پوچ
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    بشکن;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	اگر (linewise)
	    entry = qf_find_first_entry_on_سطر(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ایستا صحیح
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	صحیح		bnr,
	pos_T		*pos,
	صحیح		n,
	صحیح		dir,
	صحیح		linewise)
{
    qfline_T	*adj_entry;
    صحیح		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    اگر (adj_entry == پوچ)
	بازگشت 0;

    اگر (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	اگر (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	وگرنه
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    بازگشت errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    تهی
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحیح		dir;
    صحیح		buf_has_flag;
    صحیح		errornr = 0;
    pos_T	pos;

    اگر (eap->addr_شمار > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	بازگشت;
    }

    // Check whether the current buffer has any quickfix entries
    اگر (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    وگرنه
	buf_has_flag = BUF_HAS_LL_ENTRY;
    اگر (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	بازگشت;
    }

    اگر ((qi = qf_cmd_get_پشته(eap, TRUE)) == پوچ)
	بازگشت;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    اگر (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	بازگشت;
    }

    اگر (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    وگرنه
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_شمار > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    اگر (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    وگرنه
	emsg(_(e_no_more_اقلام));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ایستا char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cfile:	    بازگشت (char_u *)"cfile";
	حالت CMD_cgetfile:  بازگشت (char_u *)"cgetfile";
	حالت CMD_caddfile:  بازگشت (char_u *)"caddfile";
	حالت CMD_lfile:	    بازگشت (char_u *)"lfile";
	حالت CMD_lgetfile:  بازگشت (char_u *)"lgetfile";
	حالت CMD_laddfile:  بازگشت (char_u *)"laddfile";
	پیشفرض:	    بازگشت پوچ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    تهی
ex_cfile(exarg_T *eap)
{
    char_u	*enc = پوچ;
    win_T	*wp = پوچ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = پوچ;
    int_u	save_qfid = 0;		// init for gcc
    صحیح		res;

    au_name = cfile_get_auname(eap->cmdidx);
    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							پوچ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    بازگشت;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   پوچ, پوچ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), پوچ);
	اگر (browse_file == پوچ)
	    بازگشت;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    وگرنه
# endif
    اگر (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    اگر (is_loclist_cmd(eap->cmdidx))
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
    اگر (wp != پوچ)
    {
	qi = GET_LOC_LIST(wp);
	اگر (qi == پوچ)
	{
	    decr_quickfix_busy();
	    بازگشت;
	}
    }
    وگرنه اگر (qi == پوچ)
    {
	decr_quickfix_busy();
	بازگشت;
    }
    اگر (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != پوچ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, پوچ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    اگر (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ایستا char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_vimgrep:     بازگشت (char_u *)"vimgrep";
	حالت CMD_lvimgrep:    بازگشت (char_u *)"lvimgrep";
	حالت CMD_vimgrepadd:  بازگشت (char_u *)"vimgrepadd";
	حالت CMD_lvimgrepadd: بازگشت (char_u *)"lvimgrepadd";
	حالت CMD_grep:	      بازگشت (char_u *)"grep";
	حالت CMD_lgrep:	      بازگشت (char_u *)"lgrep";
	حالت CMD_grepadd:     بازگشت (char_u *)"grepadd";
	حالت CMD_lgrepadd:    بازگشت (char_u *)"lgrepadd";
	پیشفرض: بازگشت پوچ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ایستا تهی
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = پوچ;

    اگر (s == پوچ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	اگر (last_جستجو_pat() == پوچ)
	{
	    emsg(_(e_no_previous_regular_expression));
	    بازگشت;
	}
	regmatch->regprog = vim_regcomp(last_جستجو_pat(), RE_MAGIC);
    }
    وگرنه
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ایستا تهی
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    پیام_start();
    p = پیام_strtrunc(fname, TRUE);
    اگر (p == پوچ)
	پیام_outtrans(fname);
    وگرنه
    {
	پیام_outtrans(p);
	vim_free(p);
    }
    پیام_clr_eos();
    پیام_didout = FALSE;	    // overwrite this message
    پیام_nowait = TRUE;	    // don't wait for this message
    پیام_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ایستا buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    صحیح		save_mls;
# اگر defined(FEAT_SYN_HL)
    char_u	*save_ei = پوچ;
# endif
    buf_T	*buf;

# اگر defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = load_dummy_buffer(fname, dirname_start, dirname_now);

    p_mls = save_mls;
# اگر defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    بازگشت buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ایستا صحیح
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    اگر (!qflist_valid(wp, qfid))
    {
	اگر (wp != پوچ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    بازگشت FALSE;
	}
	وگرنه
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    بازگشت TRUE;
	}
    }

    اگر (qf_restore_list(qi, qfid) == FAIL)
	بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ایستا صحیح
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	صحیح	    duplicate_name,
	صحیح	    flags)
{
    صحیح		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    صحیح		pat_len = (صحیح)STRLEN(spat);
    اگر (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    برای (lnum = 1; lnum <= buf->b_ml.ml_سطر_شمار && *tomatch > 0; ++lnum)
    {
	col = 0;
	اگر (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    وقتی (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, پوچ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		اگر (qf_add_entry(qfl,
			    پوچ,	// dir
			    fname,
			    پوچ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    پوچ,	// search pattern
			    0,		// nr
			    0,		// type
			    پوچ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    بشکن;
		}
		found_match = TRUE;
		اگر (--*tomatch == 0)
		    بشکن;
		اگر ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    بشکن;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		اگر (col > ml_get_buf_len(buf, lnum))
		    بشکن;
	    }
	}
	وگرنه
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    صحیح	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    وقتی (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		اگر (qf_add_entry(qfl,
			    پوچ,	// dir
			    fname,
			    پوچ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    پوچ,	// search pattern
			    0,		// nr
			    0,		// type
			    پوچ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    بشکن;
		}
		found_match = TRUE;
		اگر (--*tomatch == 0)
		    بشکن;
		اگر ((flags & VGR_GLOBAL) == 0)
		    بشکن;
		col = matches[pat_len - 1] + col + 1;
		اگر (col > linelen)
		    بشکن;
	    }
	}
	سطر_breakcheck();
	اگر (got_int)
	    بشکن;
    }

    بازگشت found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    ایستا تهی
vgr_jump_to_match(
	qf_info_T   *qi,
	صحیح	    forceit,
	صحیح	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    اگر (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    اگر (curbuf == first_match_buf && target_dir != پوچ)
    {
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = target_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
تعریفنوع ساختار
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    صحیح		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    صحیح		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ایستا صحیح
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = پوچ;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    اگر (eap->addr_شمار > 0)
	args->tomatch = eap->line2;
    وگرنه
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    اگر (p == پوچ)
    {
	emsg(_(e_invalid_جستجو_pattern_or_delimiter));
	بازگشت FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    اگر (args->regmatch.regprog == پوچ)
	بازگشت FAIL;

    p = skipwhite(p);
    اگر (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	بازگشت FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    اگر ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ایستا صحیح
existing_swapfile(buf_T *buf)
{
    اگر (buf->b_ml.ml_mfp != پوچ && buf->b_ml.ml_mfp->mf_fname != پوچ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	بازگشت fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    بازگشت FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ایستا صحیح
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	صحیح		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    صحیح		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    صحیح		fi;
    buf_T	*buf;
    صحیح		duplicate_name = FALSE;
    صحیح		using_dummy;
    char_u	*dirname_start = پوچ;
    char_u	*dirname_now = پوچ;
    صحیح		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    اگر (dirname_start == پوچ || dirname_now == پوچ)
	برو theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    برای (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	اگر (time(پوچ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(پوچ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != پوچ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	وگرنه
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	اگر (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    برو theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	اگر (buf == پوچ)
	{
	    اگر (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	وگرنه
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    اگر (using_dummy)
	    {
		اگر (found_match && *first_match_buf == پوچ)
		    *first_match_buf = buf;
		اگر (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = پوچ;
		}
		وگرنه اگر ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    اگر (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = پوچ;
		    }
		    وگرنه اگر (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = پوچ;
		    }
		}

		اگر (buf != پوچ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    اگر (buf == *first_match_buf
			    && *target_dir == پوچ
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    اگر (curbuf == buf)
		    {
# اگر defined(FEAT_SYN_HL)
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

    status = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_start);
    بازگشت status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    تهی
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = پوچ;
    صحیح		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = پوچ;
    char_u	*target_dir = پوچ;
    char_u	*au_name =  پوچ;
    صحیح		status;

    اگر (!check_can_set_curbuf_forceit(eap->forceit))
	بازگشت;

    au_name = vgr_get_auname(eap->cmdidx);
    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    بازگشت;
# endif
    }

    qi = qf_cmd_get_or_alloc_پشته(eap, &wp);
    اگر (qi == پوچ)
	بازگشت;

    اگر (vgr_process_args(eap, &args) == FAIL)
	برو theend;

    اگر ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_پشته_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    اگر (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	برو theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, پوچ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    اگر (au_name != پوچ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    اگر (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	برو theend;
    }

    // Jump to first match.
    اگر (!qf_list_empty(qf_get_curlist(qi)))
    {
	اگر ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    وگرنه
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    اگر (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# وگرنه
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(args.qf_title);
    vim_free(target_dir);
    vim_regfree(args.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    ایستا تهی
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    اگر (dirname_now == پوچ)
	بازگشت;

    mch_dirname(dirname_now, MAXPATHL);
    اگر (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == پوچ) ? CMD_cd : CMD_lcd;
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
    ایستا buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    صحیح		failed = TRUE;
    aco_save_T	aco;
    صحیح		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(پوچ, پوچ, (linenr_T)1, BLN_DUMMY);
    اگر (newbuf == پوچ)
	بازگشت پوچ;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    اگر (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	اگر (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (تهی)setfname(curbuf, fname, پوچ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = پوچ;
	    readfile_result = readfile(fname, پوچ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			پوچ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    اگر (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		اگر (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    set_bufref(&newbuf_to_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    اگر (newbuf_to_wipe.br_buf != پوچ && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, پوچ);
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
    restore_start_dir(dirname_start);

    اگر (!bufref_valid(&newbufref))
	بازگشت پوچ;
    اگر (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	بازگشت پوچ;
    }
    بازگشت newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ایستا تهی
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    وقتی (buf->b_nwindows > 0)
    {
	صحیح	    did_one = FALSE;
	win_T	    *wp;

	اگر (firstwin->w_next != پوچ)
	    FOR_ALL_WINDOWS(wp)
		اگر (wp->w_buffer == buf)
		{
		    اگر (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    بشکن;
		}
	اگر (!did_one)
	    برو fail;
    }

    اگر (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# اگر defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	اگر (dirname_start != پوچ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	بازگشت;
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
    ایستا تهی
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    اگر (curbuf == buf)		// safety check
	بازگشت;

    close_buffer(پوچ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# اگر defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ایستا صحیح
get_qfline_اقلام(qfline_T *qfp, list_T *list)
{
    صحیح		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    اگر (bufnum != 0 && (buflist_findnr(bufnum) == پوچ))
	bufnum = 0;

    اگر ((dict = dict_alloc()) == پوچ)
	بازگشت FAIL;
    اگر (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	بازگشت FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    اگر (dict_add_عدد(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_عدد(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_عدد(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_عدد(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_عدد(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_عدد(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_عدد(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (صحیح)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_عدد(dict, "valid", (long)qfp->qf_valid) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ایستا صحیح
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	صحیح		qf_idx,
	صحیح		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحیح		i;

    اگر (qi == پوچ)
    {
	qi = ql_info;
	اگر (wp != پوچ)
	    qi = GET_LOC_LIST(wp);
	اگر (qi == پوچ)
	    بازگشت FAIL;

    }

    اگر (eidx < 0)
	بازگشت OK;

    اگر (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    اگر (qf_idx >= qi->qf_listcount)
	بازگشت FAIL;

    qfl = qf_get_list(qi, qf_idx);
    اگر (qf_list_empty(qfl))
	بازگشت FAIL;

    FOR_ALL_QFL_اقلام(qfl, qfp, i)
    {
	اگر (eidx > 0)
	{
	    اگر (eidx == i)
		بازگشت get_qfline_اقلام(qfp, list);
	}
	وگرنه اگر (get_qfline_اقلام(qfp, list) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
شمارش {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_اقلام	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_SIZE	= 0x80,
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
    ایستا صحیح
qf_get_list_from_سطرها(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    صحیح		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == پوچ)
	بازگشت FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    اگر ((efm_di = dict_find(what, (char_u *)"efm", -1)) != پوچ)
    {
	اگر (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == پوچ)
	    بازگشت FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    اگر (l == پوچ)
	بازگشت FAIL;

    qi = qf_alloc_پشته(QFLT_INTERNAL, 1);
    اگر (qi != پوچ)
    {
	اگر (qf_init_ext(qi, 0, پوچ, پوچ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, پوچ, پوچ) > 0)
	{
	    (تهی)get_errorlist(qi, پوچ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    بازگشت status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ایستا صحیح
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    اگر (qi == پوچ)
	بازگشت 0;
    win = qf_find_win(qi);
    اگر (win != پوچ)
	بازگشت win->w_id;
    بازگشت 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ایستا صحیح
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    صحیح	bufnum = 0;

    اگر (qi != پوچ && buflist_findnr(qi->qf_bufnr) != پوچ)
	bufnum = qi->qf_bufnr;

    بازگشت dict_add_عدد(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ایستا صحیح
qf_getprop_keys2flags(dict_T *what, صحیح loclist)
{
    صحیح		flags = QF_GETLIST_NONE;

    اگر (dict_has_کلید(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	اگر (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    اگر (dict_has_کلید(what, "title"))
	flags |= QF_GETLIST_TITLE;

    اگر (dict_has_کلید(what, "nr"))
	flags |= QF_GETLIST_NR;

    اگر (dict_has_کلید(what, "winid"))
	flags |= QF_GETLIST_WINID;

    اگر (dict_has_کلید(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    اگر (dict_has_کلید(what, "id"))
	flags |= QF_GETLIST_ID;

    اگر (dict_has_کلید(what, "items"))
	flags |= QF_GETLIST_اقلام;

    اگر (dict_has_کلید(what, "idx"))
	flags |= QF_GETLIST_IDX;

    اگر (dict_has_کلید(what, "size"))
	flags |= QF_GETLIST_SIZE;

    اگر (dict_has_کلید(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    اگر (loclist && dict_has_کلید(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    اگر (dict_has_کلید(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    اگر (dict_has_کلید(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    بازگشت flags;
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
    ایستا صحیح
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    صحیح		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    اگر ((di = dict_find(what, (char_u *)"nr", -1)) != پوچ)
    {
	// Use the specified quickfix/location list
	اگر (di->di_tv.v_type == VAR_عدد)
	{
	    // for zero use the current list
	    اگر (di->di_tv.vval.v_عدد != 0)
	    {
		qf_idx = di->di_tv.vval.v_عدد - 1;
		اگر (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	وگرنه اگر (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != پوچ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	وگرنه
	    qf_idx = INVALID_QFIDX;
    }

    اگر ((di = dict_find(what, (char_u *)"id", -1)) != پوچ)
    {
	// Look for a list with the specified id
	اگر (di->di_tv.v_type == VAR_عدد)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    اگر (di->di_tv.vval.v_عدد != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_عدد);
	}
	وگرنه
	    qf_idx = INVALID_QFIDX;
    }

    بازگشت qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ایستا صحیح
qf_getprop_defaults(qf_info_T *qi, صحیح flags, صحیح locstack, dict_T *retdict)
{
    صحیح		status = OK;

    اگر (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_اقلام))
    {
	list_T	*l = list_alloc();
	اگر (l != پوچ)
	    status = dict_add_list(retdict, "items", l);
	وگرنه
	    status = FAIL;
    }
    اگر ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_عدد(retdict, "nr", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_عدد(retdict, "winid", qf_winid(qi));
    اگر ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_عدد(retdict, "id", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_عدد(retdict, "idx", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_عدد(retdict, "size", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_عدد(retdict, "changedtick", 0);
    اگر ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_عدد(retdict, "filewinid", 0);
    اگر ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    بازگشت status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ایستا صحیح
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    بازگشت dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ایستا صحیح
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    صحیح winid = 0;

    اگر (wp != پوچ && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	اگر (ll_wp != پوچ)
	    winid = ll_wp->w_id;
    }

    بازگشت dict_add_عدد(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ایستا صحیح
qf_getprop_اقلام(qf_info_T *qi, صحیح qf_idx, صحیح eidx, dict_T *retdict)
{
    صحیح		status = OK;
    list_T	*l = list_alloc();
    اگر (l != پوچ)
    {
	(تهی)get_errorlist(qi, پوچ, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    وگرنه
	status = FAIL;

    بازگشت status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ایستا صحیح
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    صحیح		status;
    dictitem_T	*di;

    اگر (qfl->qf_ctx != پوچ)
    {
	di = dictitem_alloc((char_u *)"context");
	اگر (di != پوچ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    اگر (status == FAIL)
		dictitem_free(di);
	}
	وگرنه
	    status = FAIL;
    }
    وگرنه
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    بازگشت status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ایستا صحیح
qf_getprop_idx(qf_list_T *qfl, صحیح eidx, dict_T *retdict)
{
    اگر (eidx == 0)
    {
	eidx = qfl->qf_index;
	اگر (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    بازگشت dict_add_عدد(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ایستا صحیح
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    صحیح		status;

    اگر (qfl->qf_qftf_cb.cb_name != پوچ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    وگرنه
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    بازگشت status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ایستا صحیح
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    صحیح		status = OK;
    صحیح		qf_idx = INVALID_QFIDX;
    صحیح		eidx = 0;
    dictitem_T	*di;
    صحیح		flags = QF_GETLIST_NONE;

    اگر ((di = dict_find(what, (char_u *)"lines", -1)) != پوچ)
	بازگشت qf_get_list_from_سطرها(what, di, retdict);

    اگر (wp != پوچ)
	qi = GET_LOC_LIST(wp);
    وگرنه اگر (qi == پوچ)
	بازگشت FAIL;

    flags = qf_getprop_keys2flags(what, (wp != پوچ));

    اگر (!qf_پشته_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    اگر (qf_پشته_empty(qi) || qf_idx == INVALID_QFIDX)
	بازگشت qf_getprop_defaults(qi, flags, wp != پوچ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    اگر ((di = dict_find(what, (char_u *)"idx", -1)) != پوچ)
    {
	اگر (di->di_tv.v_type != VAR_عدد)
	    بازگشت FAIL;
	eidx = di->di_tv.vval.v_عدد;
    }

    اگر (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_عدد(retdict, "nr", qf_idx + 1);
    اگر ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_عدد(retdict, "winid", qf_winid(qi));
    اگر ((status == OK) && (flags & QF_GETLIST_اقلام))
	status = qf_getprop_اقلام(qi, qf_idx, eidx, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_عدد(retdict, "id", qfl->qf_id);
    اگر ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_عدد(retdict, "size", qfl->qf_شمار);
    اگر ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_عدد(retdict, "changedtick", qfl->qf_changedtick);
    اگر ((status == OK) && (wp != پوچ) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    اگر ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    بازگشت status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ایستا صحیح
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	صحیح		first_entry,
	صحیح		*valid_entry)
{
    ایستا صحیح	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    صحیح		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    اگر (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (صحیح)dict_get_عدد(d, "bufnr");
    lnum = (صحیح)dict_get_عدد(d, "lnum");
    end_lnum = (صحیح)dict_get_عدد(d, "end_lnum");
    col = (صحیح)dict_get_عدد(d, "col");
    end_col = (صحیح)dict_get_عدد(d, "end_col");
    vcol = (صحیح)dict_get_عدد(d, "vcol");
    nr = (صحیح)dict_get_عدد(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    اگر (text == پوچ)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    اگر ((filename == پوچ && bufnum == 0) || (lnum == 0 && pattern == پوچ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    اگر (bufnum != 0 && (buflist_findnr(bufnum) == پوچ))
    {
	اگر (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    اگر (dict_has_کلید(d, "valid"))
	valid = (صحیح)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			پوچ,		// dir
			filename,
			module,
			bufnum,
			text,
			lnum,
			end_lnum,
			col,
			end_col,
			vcol,		// vis_col
			pattern,	// search pattern
			nr,
			type == پوچ ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    اگر (valid)
	*valid_entry = TRUE;

    بازگشت status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ایستا صحیح
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	صحیح		target_fnum,
	صحیح		target_lnum,
	صحیح		target_col)
{
    // First, compare entries to target file.
    اگر (!target_fnum)
	// Without a target file, we can't know which is closer.
	بازگشت FALSE;

    صحیح is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    صحیح other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    اگر (!is_target_file && other_is_target_file)
	بازگشت FALSE;
    وگرنه اگر (is_target_file && !other_is_target_file)
	بازگشت TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    اگر (!target_lnum)
	// Without a target line number, we can't know which is closer.
	بازگشت FALSE;

    صحیح سطر_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    صحیح other_سطر_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    اگر (سطر_distance > other_سطر_distance)
	بازگشت FALSE;
    وگرنه اگر (سطر_distance < other_سطر_distance)
	بازگشت TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    اگر (!target_col)
	// Without a target column, we can't know which is closer.
	بازگشت FALSE;

    صحیح column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    صحیح other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    اگر (column_distance > other_column_distance)
	بازگشت FALSE;
    وگرنه اگر (column_distance < other_column_distance)
	بازگشت TRUE;

    // It's a complete tie! The exact same file, line, and column.
    بازگشت FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ایستا صحیح
qf_add_entries(
	qf_info_T	*qi,
	صحیح		qf_idx,
	list_T		*list,
	char_u		*title,
	صحیح		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = پوچ;
    صحیح		retval = OK;
    صحیح		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    صحیح prev_fnum = 0;
    صحیح prev_lnum = 0;
    صحیح prev_col = 0;
    اگر (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    صحیح select_first_entry = FALSE;
    صحیح select_nearest_entry = FALSE;

    اگر (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    وگرنه اگر (action == 'a')
    {
	اگر (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	وگرنه
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    وگرنه اگر (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_اقلام(qfl);
	qf_store_title(qfl, title);
    }
    وگرنه اگر (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_اقلام(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = پوچ;
    صحیح entry_to_select_index = 0;

    FOR_ALL_LIST_اقلام(list, li)
    {
	اگر (li->li_tv.v_type != VAR_DICT)
	    ادامه; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	اگر (d == پوچ)
	    ادامه;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	اگر (retval == QF_FAIL)
	    بشکن;

	qfline_T *entry = qfl->qf_last;
	اگر (
	    (select_first_entry && entry_to_select == پوچ) ||
	    (select_nearest_entry &&
		(entry_to_select == پوچ ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_شمار;
	}
    }

    // Check if any valid error entries are added to the list.
    اگر (valid_entry)
	qfl->qf_nonevalid = FALSE;
    وگرنه اگر (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    اگر (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    بازگشت retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ایستا صحیح
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	صحیح		action,
	صحیح		*newlist)
{
    dictitem_T	*di;
    صحیح		qf_idx = qi->qf_curlist;    // default is the current list

    اگر ((di = dict_find(what, (char_u *)"nr", -1)) != پوچ)
    {
	// Use the specified quickfix/location list
	اگر (di->di_tv.v_type == VAR_عدد)
	{
	    // for zero use the current list
	    اگر (di->di_tv.vval.v_عدد != 0)
		qf_idx = di->di_tv.vval.v_عدد - 1;

	    اگر ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_پشته_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    وگرنه اگر (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		بازگشت INVALID_QFIDX;
	    وگرنه اگر (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	وگرنه اگر (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != پوچ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    اگر (!qf_پشته_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    وگرنه اگر (*newlist)
		qf_idx = 0;
	    وگرنه
		بازگشت INVALID_QFIDX;
	}
	وگرنه
	    بازگشت INVALID_QFIDX;
    }

    اگر (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != پوچ)
    {
	// Use the quickfix/location list with the specified id
	اگر (di->di_tv.v_type != VAR_عدد)
	    بازگشت INVALID_QFIDX;

	بازگشت qf_id2nr(qi, di->di_tv.vval.v_عدد);
    }

    بازگشت qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ایستا صحیح
qf_setprop_title(qf_info_T *qi, صحیح qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    اگر (di->di_tv.v_type != VAR_STRING)
	بازگشت FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    اگر (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    بازگشت OK;
}

/*
 * Set quickfix list items/entries.
 */
    ایستا صحیح
qf_setprop_اقلام(qf_info_T *qi, صحیح qf_idx, dictitem_T *di, صحیح action)
{
    صحیح		retval = FAIL;
    char_u	*title_save;

    اگر (di->di_tv.v_type != VAR_LIST)
	بازگشت FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    بازگشت retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ایستا صحیح
qf_setprop_اقلام_from_سطرها(
	qf_info_T	*qi,
	صحیح		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	صحیح		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    صحیح		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    اگر ((efm_di = dict_find(what, (char_u *)"efm", -1)) != پوچ)
    {
	اگر (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == پوچ)
	    بازگشت FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    اگر (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == پوچ)
	بازگشت FAIL;

    اگر (action == 'r' || action == 'u')
	qf_free_اقلام(&qi->qf_lists[qf_idx]);
    اگر (qf_init_ext(qi, qf_idx, پوچ, پوچ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, پوچ, پوچ) >= 0)
	retval = OK;

    بازگشت retval;
}

/*
 * Set quickfix list context.
 */
    ایستا صحیح
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    اگر (ctx != پوچ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    بازگشت OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ایستا صحیح
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    صحیح		denote = FALSE;
    صحیح		newidx;
    صحیح		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    اگر (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != پوچ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_شمار;
    وگرنه
    {
	// Otherwise use the specified index
	newidx = tv_get_عدد_chk(&di->di_tv, &denote);
	اگر (denote)
	    بازگشت FAIL;
    }

    اگر (newidx < 1)		// sanity check
	بازگشت FAIL;
    اگر (newidx > qfl->qf_شمار)
	newidx = qfl->qf_شمار;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    اگر (qf_ptr == پوچ)
	بازگشت FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    اگر (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    بازگشت OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ایستا صحیح
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    اگر (check_restricted() || check_secure())
	بازگشت FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    اگر (cb.cb_name == پوچ || *cb.cb_name == NUL)
	بازگشت OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    بازگشت OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ایستا صحیح
qf_set_properties(qf_info_T *qi, dict_T *what, صحیح action, char_u *title)
{
    dictitem_T	*di;
    صحیح		retval = FAIL;
    صحیح		qf_idx;
    صحیح		newlist = FALSE;
    qf_list_T	*qfl;

    اگر (action == ' ' || qf_پشته_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    اگر (qf_idx == INVALID_QFIDX)	// List not found
	بازگشت FAIL;

    اگر (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    اگر ((di = dict_find(what, (char_u *)"title", -1)) != پوچ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    اگر ((di = dict_find(what, (char_u *)"items", -1)) != پوچ)
	retval = qf_setprop_اقلام(qi, qf_idx, di, action);
    اگر ((di = dict_find(what, (char_u *)"lines", -1)) != پوچ)
	retval = qf_setprop_اقلام_from_سطرها(qi, qf_idx, what, di, action);
    اگر ((di = dict_find(what, (char_u *)"context", -1)) != پوچ)
	retval = qf_setprop_context(qfl, di);
    اگر ((di = dict_find(what, (char_u *)"idx", -1)) != پوچ)
	retval = qf_setprop_curidx(qi, qfl, di);
    اگر ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != پوچ)
	retval = qf_setprop_qftf(qi, qfl, di);

    اگر (newlist || retval == OK)
	qf_list_changed(qfl);
    اگر (newlist)
	qf_update_buffer(qi, پوچ);

    بازگشت retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ایستا تهی
qf_free_پشته(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = پوچ;

    اگر (qfwin != پوچ)
    {
	// If the quickfix/location list window is open, then clear it
	اگر (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, پوچ);
    }

    اگر (wp != پوچ && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	اگر (llwin != پوچ)
	    wp = llwin;
    }

    qf_free_all(wp);
    اگر (wp == پوچ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    وگرنه اگر (qfwin != پوچ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_پشته(QFLT_LOCATION, wp->w_p_lhi);

	اگر (new_ll != پوچ)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    اگر (wp != qfwin)
		win_set_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    صحیح
set_errorlist(
	win_T	*wp,
	list_T	*list,
	صحیح	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    صحیح		retval = OK;

    اگر (wp != پوچ)
	qi = ll_get_or_alloc_list(wp);
    وگرنه
	qi = ql_info;
    اگر (qi == پوچ)
	بازگشت FAIL;

    اگر (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_پشته(wp, qi);
	بازگشت OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    اگر (list->lv_len != 0 && what != پوچ)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	بازگشت FAIL;
    }

    incr_quickfix_busy();

    اگر (what != پوچ)
	retval = qf_set_properties(qi, what, action, title);
    وگرنه
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	اگر (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    بازگشت retval;
}

ایستا صحیح mark_quickfix_user_data(qf_info_T *qi, صحیح copyID)
{
    صحیح abort = FALSE;
    برای (صحیح i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	اگر (!qfl->qf_has_user_data)
	    ادامه;
	qfline_T *qfp;
	صحیح j;
	FOR_ALL_QFL_اقلام(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    اگر (user_data != پوچ && user_data->v_type != VAR_عدد
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, پوچ, پوچ, پوچ);
	}
    }
    بازگشت abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ایستا صحیح
mark_quickfix_ctx(qf_info_T *qi, صحیح copyID)
{
    صحیح		i;
    صحیح		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    برای (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	اگر (ctx != پوچ && ctx->v_type != VAR_عدد
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, پوچ, پوچ, پوچ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    بازگشت abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    صحیح
set_ref_in_quickfix(صحیح copyID)
{
    صحیح		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    اگر (ql_info == پوچ)
	بازگشت TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    اگر (abort)
	بازگشت abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    اگر (abort)
	بازگشت abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    اگر (abort)
	بازگشت abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	اگر (win->w_llist != پوچ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    اگر (abort)
		بازگشت abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    اگر (abort)
		بازگشت abort;
	}
	اگر (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    اگر (abort)
		بازگشت abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    اگر (abort)
		بازگشت abort;
	}
    }

    بازگشت abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ایستا char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cbuffer:	بازگشت (char_u *)"cbuffer";
	حالت CMD_cgetbuffer:	بازگشت (char_u *)"cgetbuffer";
	حالت CMD_caddbuffer:	بازگشت (char_u *)"caddbuffer";
	حالت CMD_lbuffer:	بازگشت (char_u *)"lbuffer";
	حالت CMD_lgetbuffer:	بازگشت (char_u *)"lgetbuffer";
	حالت CMD_laddbuffer:	بازگشت (char_u *)"laddbuffer";
	پیشفرض:		بازگشت پوچ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ایستا صحیح
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = پوچ;

    اگر (*eap->arg == NUL)
	buf = curbuf;
    وگرنه اگر (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((نویسه *)eap->arg));

    اگر (buf == پوچ)
    {
	emsg(_(e_invalid_argument));
	بازگشت FAIL;
    }

    اگر (buf->b_ml.ml_mfp == پوچ)
    {
	emsg(_(e_buffer_is_not_loaded));
	بازگشت FAIL;
    }

    اگر (eap->addr_شمار == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_سطر_شمار;
    }

    اگر (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_سطر_شمار
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_سطر_شمار)
    {
	emsg(_(e_invalid_range));
	بازگشت FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    بازگشت OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    تهی
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = پوچ;
    qf_info_T	*qi;
    char_u	*au_name = پوچ;
    صحیح		res;
    int_u	save_qfid;
    win_T	*wp = پوچ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    بازگشت;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_پشته(eap, &wp);
    اگر (qi == پوچ)
	بازگشت;

    اگر (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	بازگشت;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    اگر (buf->b_sfname)
    {
	vim_snprintf((نویسه *)IObuff, IOSIZE, "%s (%s)",
		(نویسه *)qf_title, (نویسه *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, پوچ, buf, پوچ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, پوچ);
    اگر (qf_پشته_empty(qi))
    {
	decr_quickfix_busy();
	بازگشت;
    }
    اگر (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    اگر (au_name != پوچ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	اگر (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    اگر (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# اگر defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    انتخاب (cmdidx)
    {
	حالت CMD_cexpr:	    بازگشت (char_u *)"cexpr";
	حالت CMD_cgetexpr:  بازگشت (char_u *)"cgetexpr";
	حالت CMD_caddexpr:  بازگشت (char_u *)"caddexpr";
	حالت CMD_lexpr:	    بازگشت (char_u *)"lexpr";
	حالت CMD_lgetexpr:  بازگشت (char_u *)"lgetexpr";
	حالت CMD_laddexpr:  بازگشت (char_u *)"laddexpr";
	پیشفرض:	    بازگشت پوچ;
    }
}

    صحیح
trigger_cexpr_autocmd(صحیح cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	اگر (aborting())
	    بازگشت FAIL;
    }
    بازگشت OK;
}

    صحیح
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = پوچ;

    qi = qf_cmd_get_or_alloc_پشته(eap, &wp);
    اگر (qi == پوچ)
	بازگشت FAIL;

    اگر ((tv->v_type == VAR_STRING && tv->vval.v_string != پوچ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != پوچ))
    {
	صحیح	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, پوچ, پوچ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), پوچ);
	اگر (qf_پشته_empty(qi))
	{
	    decr_quickfix_busy();
	    بازگشت FAIL;
	}
	اگر (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	اگر (au_name != پوچ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	اگر (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	بازگشت OK;
    }

    emsg(_(e_string_or_list_expected));
    بازگشت FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    تهی
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    اگر (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	بازگشت;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_عبارت(eap->arg, eap);
    اگر (tv == پوچ)
	بازگشت;

    (تهی)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ایستا qf_info_T *
hgr_get_ll(صحیح *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    اگر (bt_help(curwin->w_buffer))
	wp = curwin;
    وگرنه
	// Find an existing help window
	wp = qf_find_help_win();

    اگر (wp == پوچ)	    // Help window not found
	qi = پوچ;
    وگرنه
	qi = wp->w_llist;

    اگر (qi == پوچ)
    {
	// Allocate a new location list for help text matches
	اگر ((qi = qf_alloc_پشته(QFLT_LOCATION, 1)) == پوچ)
	    بازگشت پوچ;
	*new_ll = TRUE;
    }

    بازگشت qi;
}

/*
 * Search for a pattern in a help file.
 */
    ایستا تهی
hgr_جستجو_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((نویسه *)fname, "r");
    اگر (fd == پوچ)
	بازگشت;

    lnum = 1;
    وقتی (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *سطر = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	اگر (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    سطر = string_convert(p_vc, IObuff, پوچ);
	    اگر (سطر == پوچ)
		سطر = IObuff;
	}

	اگر (vim_regexec(p_regmatch, سطر, (colnr_T)0))
	{
	    صحیح	l = (صحیح)STRLEN(سطر);

	    // remove trailing CR, LF, spaces, etc.
	    وقتی (l > 0 && سطر[l - 1] <= ' ')
		سطر[--l] = NUL;

	    اگر (qf_add_entry(qfl,
			پوچ,	// dir
			fname,
			پوچ,
			0,
			سطر,
			lnum,
			0,
			(صحیح)(p_regmatch->startp[0] - سطر)
			+ 1,	// col
			(صحیح)(p_regmatch->endp[0] - سطر)
			+ 1,	// end_col
			FALSE,	// vis_col
			پوچ,	// search pattern
			0,	// nr
			1,	// type
			پوچ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		اگر (سطر != IObuff)
		    vim_free(سطر);
		بشکن;
	    }
	}
	اگر (سطر != IObuff)
	    vim_free(سطر);
	++lnum;
	سطر_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ایستا تهی
hgr_جستجو_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    صحیح		fcount;
    char_u	**fnames;
    صحیح		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    اگر (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	برای (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    اگر (lang != پوچ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		ادامه;
# endif

	    hgr_جستجو_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ایستا تهی
hgr_جستجو_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    اگر (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    وقتی (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_جستجو_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    اگر (vc.vc_type != CONV_NONE)
	convert_setup(&vc, پوچ, پوچ);
}

/*
 * ":helpgrep {pattern}"
 */
    تهی
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    صحیح		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    صحیح		new_qi = FALSE;
    char_u	*au_name =  پوچ;
    char_u	*lang = پوچ;
    صحیح		updated = FALSE;

    انتخاب (eap->cmdidx)
    {
	حالت CMD_helpgrep:  au_name = (char_u *)"helpgrep"; بشکن;
	حالت CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; بشکن;
	پیشفرض: بشکن;
    }
    اگر (au_name != پوچ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	اگر (aborting())
	    بازگشت;
# endif
    }

    اگر (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	اگر (qi == پوچ)
	    بازگشت;
    }
    وگرنه اگر (qi == پوچ)
    {
	emsg(_(e_no_quickfix_پشته));
	بازگشت;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = empty_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    اگر (regmatch.regprog != پوچ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_جستجو_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    اگر (p_cpo == empty_option)
	p_cpo = save_cpo;
    وگرنه
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	اگر (*p_cpo == NUL)
	    set_option_مقدار_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	اگر (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    اگر (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, پوچ);

    اگر (au_name != پوچ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	اگر (!new_qi && IS_LL_پشته(qi) && qf_find_win_with_loclist(qi) == پوچ)
	{
	    decr_quickfix_busy();
	    بازگشت;
	}
    }

    // Jump to first match.
    اگر (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    وگرنه
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    اگر (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	اگر (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    اگر (new_qi)
		ll_free_all(&qi);
	}
	وگرنه اگر (curwin->w_llist == پوچ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# اگر defined(EXITFREE)
    تهی
free_quickfix(تهی)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(پوچ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#اگر defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ایستا تهی
get_qf_loc_list(صحیح is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    اگر (what_arg->v_type == VAR_UNKNOWN)
    {
	اگر (rettv_list_alloc(rettv) == OK)
	    اگر (is_qf || wp != پوچ)
		(تهی)get_errorlist(پوچ, wp, -1, 0, rettv->vval.v_list);
    }
    وگرنه
    {
	اگر (rettv_dict_alloc(rettv) == OK)
	    اگر (is_qf || (wp != پوچ))
	    {
		اگر (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    اگر (d != پوچ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		وگرنه
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    تهی
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    تهی
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    اگر (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    get_qf_loc_list(TRUE, پوچ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ایستا تهی
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    صحیح		action = 0;
    ایستا صحیح	recursive = 0;
# endif

    rettv->vval.v_عدد = -1;

# ifdef FEAT_QUICKFIX
    اگر (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    وگرنه اگر (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    وگرنه
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = پوچ;
	صحیح	valid_dict = TRUE;

	اگر (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    اگر (act == پوچ)
		بازگشت;		// type error; errmsg already given
	    اگر ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    وگرنه
		semsg(_(e_invalid_action_str_1), act);
	}
	وگرنه اگر (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	وگرنه
	    emsg(_(e_string_required));

	اگر (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    اگر (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != پوچ)
		what = what_arg->vval.v_dict;
	    وگرنه
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	اگر (l != پوچ && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == پوچ ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_عدد = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    تهی
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	بازگشت;

    win = find_win_by_nr_or_id(&argvars[0]);
    اگر (win != پوچ)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    تهی
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	بازگشت;

    set_qf_ll_list(پوچ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
