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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_QUICKFIX)

সংরচনা dir_স্তূপ_T
{
    সংরচনা dir_স্তূপ_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
প্রকারনির্দেশ সংরচনা qfline_S qfline_T;
সংরচনা qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    পূর্ণাঙ্ক		qf_fnum;	// file number for the line
    পূর্ণাঙ্ক		qf_col;		// column where the error occurred
    পূর্ণাঙ্ক		qf_end_col;	// column when the error has range or zero
    পূর্ণাঙ্ক		qf_nr;		// error number
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
প্রকারনির্দেশ গণনা
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
প্রকারনির্দেশ সংরচনা qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    পূর্ণাঙ্ক		qf_গুনতি;	// number of errors (0 means empty list)
    পূর্ণাঙ্ক		qf_index;	// current index in the error list
    পূর্ণাঙ্ক		qf_nonevalid;	// TRUE if not a single valid entry found
    পূর্ণাঙ্ক		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    সংরচনা dir_স্তূপ_T	*qf_dir_স্তূপ;
    char_u		*qf_directory;
    সংরচনা dir_স্তূপ_T	*qf_file_স্তূপ;
    char_u		*qf_currfile;
    পূর্ণাঙ্ক			qf_multiline;
    পূর্ণাঙ্ক			qf_multiignore;
    পূর্ণাঙ্ক			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
সংরচনা qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    পূর্ণাঙ্ক		qf_refcount;
    পূর্ণাঙ্ক		qf_listcount;	    // current number of lists
    পূর্ণাঙ্ক		qf_curlist;	    // current error list
    পূর্ণাঙ্ক         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    পূর্ণাঙ্ক		qf_bufnr;	    // quickfix window buffer number
};

স্থির qf_info_T ql_info_actual; // global quickfix list
স্থির qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
স্থির int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
প্রকারনির্দেশ সংরচনা efm_S efm_T;
সংরচনা efm_S
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
    পূর্ণাঙ্ক		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
প্রকারনির্দেশ সংরচনা qf_delq_S
{
    সংরচনা qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
স্থির qf_delq_T *qf_delq_head = রিক্ত;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
স্থির পূর্ণাঙ্ক	quickfix_busy = 0;

স্থির efm_T	*fmt_start = রিক্ত; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
স্থির callback_T qftf_cb;

স্থির শূন্য     qf_বের_করো_স্তূপ(qf_info_T *qi, পূর্ণাঙ্ক adjust);
স্থির শূন্য	qf_new_list(qf_info_T *qi, char_u *qf_title);
স্থির পূর্ণাঙ্ক	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, পূর্ণাঙ্ক bufnum, char_u *mesg, long lnum, long end_lnum, পূর্ণাঙ্ক col, পূর্ণাঙ্ক end_col, পূর্ণাঙ্ক vis_col, char_u *pattern, পূর্ণাঙ্ক nr, পূর্ণাঙ্ক type, typval_T *user_data, পূর্ণাঙ্ক valid);
স্থির পূর্ণাঙ্ক      qf_resize_স্তূপ_base(qf_info_T *qi, পূর্ণাঙ্ক n);
স্থির শূন্য     qf_sync_llw_to_win(win_T *llw);
স্থির শূন্য     qf_sync_win_to_llw(win_T *pwp);
স্থির qf_info_T *qf_alloc_স্তূপ(qfltype_T qfltype, পূর্ণাঙ্ক n);
স্থির qf_list_T *qf_alloc_list_স্তূপ(পূর্ণাঙ্ক n);
স্থির শূন্য	qf_free(qf_list_T *qfl);
স্থির char_u	*qf_types(পূর্ণাঙ্ক, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
স্থির char_u	*qf_ঠেলো_dir(char_u *, সংরচনা dir_স্তূপ_T **, পূর্ণাঙ্ক is_file_স্তূপ);
স্থির char_u	*qf_বের_করো_dir(সংরচনা dir_স্তূপ_T **);
স্থির char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
স্থির win_T    *qf_find_win_with_loclist(qf_info_T *ll);
স্থির শূন্য	qf_jump_newwin(qf_info_T *qi, পূর্ণাঙ্ক dir, পূর্ণাঙ্ক errornr, পূর্ণাঙ্ক forceit, পূর্ণাঙ্ক newwin);
স্থির শূন্য	qf_fmt_text(garray_T *gap, char_u *text);
স্থির শূন্য	qf_range_text(garray_T *gap, qfline_T *qfp);
স্থির পূর্ণাঙ্ক	qf_win_pos_update(qf_info_T *qi, পূর্ণাঙ্ক old_qf_index);
স্থির win_T	*qf_find_win(qf_info_T *qi);
স্থির buf_T	*qf_find_buf(qf_info_T *qi);
স্থির শূন্য	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
স্থির শূন্য	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, পূর্ণাঙ্ক qf_winid);
স্থির buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
স্থির শূন্য	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
স্থির শূন্য	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
স্থির qf_info_T *ll_get_or_alloc_list(win_T *);
স্থির পূর্ণাঙ্ক	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, পূর্ণাঙ্ক target_fnum, পূর্ণাঙ্ক target_lnum, পূর্ণাঙ্ক target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == রিক্ত)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != রিক্ত)

// Quickfix and location list stack check helper macros
# define IS_QF_স্তূপ(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_স্তূপ(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i) \
		    প্রতি ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_গুনতি && (qfp) != রিক্ত; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
স্থির char_u   *qf_last_bufname = রিক্ত;
স্থির bufref_T  qf_last_bufref = {রিক্ত, 0, 0};

স্থির garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    স্থির garray_T *
qfga_get(শূন্য)
{
    স্থির পূর্ণাঙ্ক initialized = FALSE;

    যদি (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    প্রত্যাগম &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    স্থির শূন্য
qfga_clear(শূন্য)
{
    যদি (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    অন্যথা
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define সারি_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
স্থির সংরচনা fmtpattern
{
    char_u	convchar;
    বর্ণ	*pattern;
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
    স্থির char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	পূর্ণাঙ্ক	idx,
	পূর্ণাঙ্ক	round)
{
    char_u	*srcptr;

    যদি (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	প্রত্যাগম রিক্ত;
    }
    যদি ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != রিক্ত)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == রিক্ত))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	প্রত্যাগম রিক্ত;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    যদি (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    যদি (*efmpat == 'f' && efmpat[1] != NUL)
    {
	যদি (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	অন্যথা
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    অন্যথা
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	যাবত্ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    প্রত্যাগম regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    স্থির char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	পূর্ণাঙ্ক	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    যদি (*efmp == '[' || *efmp == '\\')
    {
	যদি ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    যদি (efmp[1] == '^')
		*regpat++ = *++efmp;
	    যদি (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		যাবত্ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		যদি (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    প্রত্যাগম রিক্ত;
		}
	    }
	}
	অন্যথা যদি (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    অন্যথা
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	প্রত্যাগম রিক্ত;
    }

    *pefmp = efmp;

    প্রত্যাগম regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    স্থির char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    যদি (vim_strchr((char_u *)"+-", *efmp) != রিক্ত)
	efminfo->flags = *efmp++;
    যদি (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != রিক্ত)
	efminfo->prefix = *efmp;
    অন্যথা
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
efm_to_regpat(
	char_u	*efm,
	পূর্ণাঙ্ক	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    প্রতি (efmp = efm; efmp < efm + len; ++efmp)
    {
	যদি (*efmp == '%')
	{
	    ++efmp;
	    প্রতি (idx = 0; idx < FMT_PATTERNS; ++idx)
		যদি (fmt_pat[idx].convchar == *efmp)
		    ভঙ্গ;
	    যদি (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		যদি (ptr == রিক্ত)
		    প্রত্যাগম FAIL;
		round++;
	    }
	    অন্যথা যদি (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		যদি (ptr == রিক্ত)
		    প্রত্যাগম FAIL;
	    }
	    অন্যথা যদি (vim_strchr((char_u *)"%\\.^$~[", *efmp) != রিক্ত)
		*ptr++ = *efmp;		// regexp magic characters
	    অন্যথা যদি (*efmp == '#')
		*ptr++ = '*';
	    অন্যথা যদি (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    অন্যথা যদি (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		যদি (efmp == রিক্ত)
		    প্রত্যাগম FAIL;
	    }
	    অন্যথা
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		প্রত্যাগম FAIL;
	    }
	}
	অন্যথা			// copy normal character
	{
	    যদি (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    অন্যথা যদি (vim_strchr((char_u *)".*^$~[", *efmp) != রিক্ত)
		*ptr++ = '\\';	// escape regexp atoms
	    যদি (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    প্রত্যাগম OK;
}

/*
 * Free the 'errorformat' information list
 */
    স্থির শূন্য
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    প্রতি (efm_ptr = *efm_first; efm_ptr != রিক্ত; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = রিক্ত;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    স্থির পূর্ণাঙ্ক
efm_regpat_bufsz(char_u *efm)
{
    পূর্ণাঙ্ক sz;
    পূর্ণাঙ্ক i;

    sz = (FMT_PATTERNS * 3) + ((পূর্ণাঙ্ক)STRLEN(efm) << 2);
    প্রতি (i = FMT_PATTERNS; i > 0; )
	sz += (পূর্ণাঙ্ক)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# অন্যথা
    sz += 2; // "%f" can become two chars longer
# endif

    প্রত্যাগম sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    স্থির পূর্ণাঙ্ক
efm_option_part_len(char_u *efm)
{
    পূর্ণাঙ্ক len;

    প্রতি (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	যদি (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    প্রত্যাগম len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    স্থির efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = রিক্ত;
    efm_T	*fmt_first = রিক্ত;
    efm_T	*fmt_last = রিক্ত;
    char_u	*fmtstr = রিক্ত;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    যদি ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == রিক্ত)
	গচ্ছ parse_efm_error;

    যাবত্ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	যদি (fmt_ptr == রিক্ত)
	    গচ্ছ parse_efm_error;
	যদি (fmt_first == রিক্ত)	    // first one
	    fmt_first = fmt_ptr;
	অন্যথা
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	যদি (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    গচ্ছ parse_efm_error;
	যদি ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == রিক্ত)
	    গচ্ছ parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    যদি (fmt_first == রিক্ত)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    গচ্ছ parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    প্রত্যাগম fmt_first;
}

গণনা {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_প্রবেশ = 2,
    QF_NOMEM = 3,
    QF_IGNORE_সারি = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
প্রকারনির্দেশ সংরচনা {
    char_u	*linebuf;
    পূর্ণাঙ্ক		linelen;
    char_u	*growbuf;
    পূর্ণাঙ্ক		growbufsiz;
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
    স্থির char_u *
qf_grow_linebuf(qfstate_T *state, পূর্ণাঙ্ক newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > সারি_MAXLEN ? সারি_MAXLEN - 1 : newsz;
    যদি (state->growbuf == রিক্ত)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	যদি (state->growbuf == রিক্ত)
	    প্রত্যাগম রিক্ত;
	state->growbufsiz = state->linelen;
    }
    অন্যথা যদি (state->linelen > state->growbufsiz)
    {
	যদি ((p = vim_realloc(state->growbuf, state->linelen + 1)) == রিক্ত)
	    প্রত্যাগম রিক্ত;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    প্রত্যাগম state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    স্থির পূর্ণাঙ্ক
qf_get_next_str_সারি(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    পূর্ণাঙ্ক		len;

    যদি (*p_str == NUL) // Reached the end of the string
	প্রত্যাগম QF_END_OF_প্রবেশ;

    p = vim_strchr(p_str, '\n');
    যদি (p != রিক্ত)
	len = (পূর্ণাঙ্ক)(p - p_str) + 1;
    অন্যথা
	len = (পূর্ণাঙ্ক)STRLEN(p_str);

    যদি (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	যদি (state->linebuf == রিক্ত)
	    প্রত্যাগম QF_NOMEM;
    }
    অন্যথা
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    প্রত্যাগম QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    স্থির পূর্ণাঙ্ক
qf_get_next_list_সারি(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    পূর্ণাঙ্ক		len;

    যাবত্ (p_li != রিক্ত
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == রিক্ত))
	p_li = p_li->li_next;	// Skip non-string items

    যদি (p_li == রিক্ত)		// End of the list
    {
	state->p_li = রিক্ত;
	প্রত্যাগম QF_END_OF_প্রবেশ;
    }

    len = (পূর্ণাঙ্ক)STRLEN(p_li->li_tv.vval.v_string);
    যদি (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	যদি (state->linebuf == রিক্ত)
	    প্রত্যাগম QF_NOMEM;
    }
    অন্যথা
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    প্রত্যাগম QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    স্থির পূর্ণাঙ্ক
qf_get_next_buf_সারি(qfstate_T *state)
{
    char_u	*p_buf = রিক্ত;
    পূর্ণাঙ্ক		len;

    // Get the next line from the supplied buffer
    যদি (state->buflnum > state->lnumlast)
	প্রত্যাগম QF_END_OF_প্রবেশ;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    যদি (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	যদি (state->linebuf == রিক্ত)
	    প্রত্যাগম QF_NOMEM;
    }
    অন্যথা
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    প্রত্যাগম QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    স্থির পূর্ণাঙ্ক
qf_get_next_file_সারি(qfstate_T *state)
{
    পূর্ণাঙ্ক	    discard;
    পূর্ণাঙ্ক	    growbuflen;

    যদি (fgets((বর্ণ *)IObuff, IOSIZE, state->fd) == রিক্ত)
	প্রত্যাগম QF_END_OF_প্রবেশ;

    discard = FALSE;
    state->linelen = (পূর্ণাঙ্ক)STRLEN(IObuff);
    যদি (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	যদি (state->growbuf == রিক্ত)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    যদি (state->growbuf == রিক্ত)
		প্রত্যাগম QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	প্রতি (;;)
	{
	    char_u	*p;

	    যদি (fgets((বর্ণ *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == রিক্ত)
		ভঙ্গ;
	    state->linelen = (পূর্ণাঙ্ক)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    যদি ((state->growbuf)[growbuflen - 1] == '\n')
		ভঙ্গ;
	    যদি (state->growbufsiz == সারি_MAXLEN)
	    {
		discard = TRUE;
		ভঙ্গ;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < সারি_MAXLEN
		? 2 * state->growbufsiz : সারি_MAXLEN;
	    যদি ((p = vim_realloc(state->growbuf, state->growbufsiz)) == রিক্ত)
		প্রত্যাগম QF_NOMEM;
	    state->growbuf = p;
	}

	যাবত্ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    যদি (fgets((বর্ণ *)IObuff, IOSIZE, state->fd) == রিক্ত
		    || (পূর্ণাঙ্ক)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ভঙ্গ;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    অন্যথা
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    যদি (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*সারি;

	সারি = string_convert(&state->vc, state->linebuf, &state->linelen);
	যদি (সারি != রিক্ত)
	{
	    যদি (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, সারি);
		vim_free(সারি);
	    }
	    অন্যথা
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = সারি;
		state->growbufsiz = state->linelen < সারি_MAXLEN
						? state->linelen : সারি_MAXLEN;
	    }
	}
    }

    প্রত্যাগম QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    স্থির পূর্ণাঙ্ক
qf_get_nextline(qfstate_T *state)
{
    পূর্ণাঙ্ক status = QF_FAIL;

    যদি (state->fd == রিক্ত)
    {
	যদি (state->tv != রিক্ত)
	{
	    যদি (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_সারি(state);
	    অন্যথা যদি (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_সারি(state);
	}
	অন্যথা
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_সারি(state);
    }
    অন্যথা
	// Get the next line from the supplied file
	status = qf_get_next_file_সারি(state);

    যদি (status != QF_OK)
	প্রত্যাগম status;

    // remove newline/CR from the line
    যদি (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	যদি (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    প্রত্যাগম QF_OK;
}

প্রকারনির্দেশ সংরচনা {
    char_u	*namebuf;
    পূর্ণাঙ্ক		bnr;
    char_u	*module;
    char_u	*errmsg;
    পূর্ণাঙ্ক		errmsglen;
    long	lnum;
    long	end_lnum;
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    পূর্ণাঙ্ক		enr;
    পূর্ণাঙ্ক		type;
    typval_T	*user_data;
    পূর্ণাঙ্ক		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_f(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields, পূর্ণাঙ্ক prefix)
{
    পূর্ণাঙ্ক c;

    যদি (rmp->startp[midx] == রিক্ত || rmp->endp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    যদি (vim_strchr((char_u *)"OPQ", prefix) != রিক্ত
	    && mch_getperm(fields->namebuf) == -1)
	প্রত্যাগম QF_FAIL;

    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_b(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    পূর্ণাঙ্ক bnr = (পূর্ণাঙ্ক)atol((বর্ণ *)rmp->startp[midx]);
    যদি (buflist_findnr(bnr) == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->bnr = bnr;
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_n(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->enr = (পূর্ণাঙ্ক)atol((বর্ণ *)rmp->startp[midx]);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_l(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->lnum = atol((বর্ণ *)rmp->startp[midx]);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_e(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->end_lnum = atol((বর্ণ *)rmp->startp[midx]);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_c(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->col = (পূর্ণাঙ্ক)atol((বর্ণ *)rmp->startp[midx]);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_k(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->end_col = (পূর্ণাঙ্ক)atol((বর্ণ *)rmp->startp[midx]);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_t(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->type = *rmp->startp[midx];
    প্রত্যাগম QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    স্থির পূর্ণাঙ্ক
copy_nonerror_সারি(char_u *linebuf, পূর্ণাঙ্ক linelen, qffields_T *fields)
{
    char_u	*p;

    যদি (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	যদি ((p = vim_realloc(fields->errmsg, linelen + 1)) == রিক্ত)
	    প্রত্যাগম QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_m(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    char_u	*p;
    পূর্ণাঙ্ক		len;

    যদি (rmp->startp[midx] == রিক্ত || rmp->endp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    len = (পূর্ণাঙ্ক)(rmp->endp[midx] - rmp->startp[midx]);
    যদি (len >= fields->errmsglen)
    {
	// len + null terminator
	যদি ((p = vim_realloc(fields->errmsg, len + 1)) == রিক্ত)
	    প্রত্যাগম QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_r(regmatch_T *rmp, পূর্ণাঙ্ক midx, char_u **tail)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    *tail = rmp->startp[midx];
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_p(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    char_u	*match_ptr;

    যদি (rmp->startp[midx] == রিক্ত || rmp->endp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->col = 0;
    প্রতি (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	যদি (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_v(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    যদি (rmp->startp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    fields->col = (পূর্ণাঙ্ক)atol((বর্ণ *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_s(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    পূর্ণাঙ্ক		len;

    যদি (rmp->startp[midx] == রিক্ত || rmp->endp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    len = (পূর্ণাঙ্ক)(rmp->endp[midx] - rmp->startp[midx]);
    যদি (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    প্রত্যাগম QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    স্থির পূর্ণাঙ্ক
qf_parse_fmt_o(regmatch_T *rmp, পূর্ণাঙ্ক midx, qffields_T *fields)
{
    পূর্ণাঙ্ক		len;

    যদি (rmp->startp[midx] == রিক্ত || rmp->endp[midx] == রিক্ত)
	প্রত্যাগম QF_FAIL;
    len = (পূর্ণাঙ্ক)(rmp->endp[midx] - rmp->startp[midx]);
    যদি (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    প্রত্যাগম QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
স্থির পূর্ণাঙ্ক (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, পূর্ণাঙ্ক, qffields_T *) =
{
    রিক্ত, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    রিক্ত, // %r
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
    স্থির পূর্ণাঙ্ক
qf_parse_match(
	char_u		*linebuf,
	পূর্ণাঙ্ক		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	পূর্ণাঙ্ক		qf_multiline,
	পূর্ণাঙ্ক		qf_multiscan,
	char_u		**tail)
{
    পূর্ণাঙ্ক		idx = fmt_ptr->prefix;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		midx;
    পূর্ণাঙ্ক		status;

    যদি ((idx == 'C' || idx == 'Z') && !qf_multiline)
	প্রত্যাগম QF_FAIL;
    যদি (vim_strchr((char_u *)"EWIN", idx) != রিক্ত)
	fields->type = idx;
    অন্যথা
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    প্রতি (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (পূর্ণাঙ্ক)fmt_ptr->addr[i];
	যদি (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	অন্যথা যদি (i == FMT_PATTERN_M)
	{
	    যদি (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_সারি(linebuf, linelen, fields);
	    অন্যথা যদি (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	অন্যথা যদি (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	অন্যথা যদি (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	যদি (status != QF_OK)
	    প্রত্যাগম status;
    }

    প্রত্যাগম QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    স্থির পূর্ণাঙ্ক
qf_parse_get_fields(
	char_u		*linebuf,
	পূর্ণাঙ্ক		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	পূর্ণাঙ্ক		qf_multiline,
	পূর্ণাঙ্ক		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		status = QF_FAIL;
    পূর্ণাঙ্ক		r;

    যদি (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == রিক্ত)
	প্রত্যাগম QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    যদি (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = রিক্ত;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    যদি (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    প্রত্যাগম status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    স্থির পূর্ণাঙ্ক
qf_parse_dir_pfx(পূর্ণাঙ্ক idx, qffields_T *fields, qf_list_T *qfl)
{
    যদি (idx == 'D')				// enter directory
    {
	যদি (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    প্রত্যাগম QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ঠেলো_dir(fields->namebuf, &qfl->qf_dir_স্তূপ, FALSE);
	যদি (qfl->qf_directory == রিক্ত)
	    প্রত্যাগম QF_FAIL;
    }
    অন্যথা যদি (idx == 'X')			// leave directory
	qfl->qf_directory = qf_বের_করো_dir(&qfl->qf_dir_স্তূপ);

    প্রত্যাগম QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    স্থির পূর্ণাঙ্ক
qf_parse_file_pfx(
	পূর্ণাঙ্ক idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    যদি (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	যদি (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ঠেলো_dir(fields->namebuf, &qfl->qf_file_স্তূপ, TRUE);
	অন্যথা যদি (idx == 'Q')
	    qfl->qf_currfile = qf_বের_করো_dir(&qfl->qf_file_স্তূপ);
	*fields->namebuf = NUL;
	যদি (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    প্রত্যাগম QF_MULTISCAN;
	}
    }

    প্রত্যাগম QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    স্থির পূর্ণাঙ্ক
qf_parse_সারি_nomatch(char_u *linebuf, পূর্ণাঙ্ক linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    প্রত্যাগম copy_nonerror_সারি(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    স্থির পূর্ণাঙ্ক
qf_parse_multiline_pfx(
	পূর্ণাঙ্ক idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    পূর্ণাঙ্ক			len;

    যদি (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	যদি (qfprev == রিক্ত)
	    প্রত্যাগম QF_FAIL;
	যদি (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (পূর্ণাঙ্ক)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    যদি (ptr == রিক্ত)
		প্রত্যাগম QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	যদি (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	যদি (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	যদি (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	যদি (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	যদি (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	যদি (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	যদি (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != রিক্ত
		    ? fields->namebuf
		    : qfl->qf_currfile != রিক্ত && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    যদি (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    সারি_breakcheck();

    প্রত্যাগম QF_IGNORE_সারি;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    স্থির পূর্ণাঙ্ক
qf_parse_সারি(
	qf_list_T	*qfl,
	char_u		*linebuf,
	পূর্ণাঙ্ক		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    পূর্ণাঙ্ক			idx = 0;
    char_u		*tail = রিক্ত;
    পূর্ণাঙ্ক			status;

restofline:
    // If there was no %> item start at the first pattern
    যদি (fmt_start == রিক্ত)
	fmt_ptr = fmt_first;
    অন্যথা
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = রিক্ত;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    প্রতি ( ; fmt_ptr != রিক্ত; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	যদি (status == QF_NOMEM)
	    প্রত্যাগম status;
	যদি (status == QF_OK)
	    ভঙ্গ;
    }
    qfl->qf_multiscan = FALSE;

    যদি (fmt_ptr == রিক্ত || idx == 'D' || idx == 'X')
    {
	যদি (fmt_ptr != রিক্ত)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    যদি (status != QF_OK)
		প্রত্যাগম status;
	}

	status = qf_parse_সারি_nomatch(linebuf, linelen, fields);
	যদি (status != QF_OK)
	    প্রত্যাগম status;

	যদি (fmt_ptr == রিক্ত)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    অন্যথা যদি (fmt_ptr != রিক্ত)
    {
	// honor %> item
	যদি (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	যদি (vim_strchr((char_u *)"AEWIN", idx) != রিক্ত)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	অন্যথা যদি (vim_strchr((char_u *)"CZ", idx) != রিক্ত)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    যদি (status != QF_OK)
		প্রত্যাগম status;
	}
	অন্যথা যদি (vim_strchr((char_u *)"OPQ", idx) != রিক্ত)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    যদি (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		পূর্ণাঙ্ক new_linelen = (পূর্ণাঙ্ক)STRLEN(s);
		যদি (new_linelen >= linelen)
		    প্রত্যাগম QF_IGNORE_সারি;
		linebuf = s;
		linelen = new_linelen;
		গচ্ছ restofline;
	    }
	}
	যদি (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    যদি (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    প্রত্যাগম QF_IGNORE_সারি;
	}
    }

    প্রত্যাগম QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    স্থির পূর্ণাঙ্ক
qf_স্তূপ_empty(qf_info_T *qi)
{
    প্রত্যাগম qi == রিক্ত || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    স্থির পূর্ণাঙ্ক
qf_list_empty(qf_list_T *qfl)
{
    প্রত্যাগম qfl == রিক্ত || qfl->qf_গুনতি <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    স্থির পূর্ণাঙ্ক
qf_list_has_valid_entries(qf_list_T *qfl)
{
    প্রত্যাগম !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    স্থির qf_list_T *
qf_get_list(qf_info_T *qi, পূর্ণাঙ্ক idx)
{
    প্রত্যাগম &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    স্থির পূর্ণাঙ্ক
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    যদি (pfields->namebuf == রিক্ত || pfields->errmsg == রিক্ত
		|| pfields->pattern == রিক্ত || pfields->module == রিক্ত)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    স্থির শূন্য
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
    স্থির পূর্ণাঙ্ক
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
    যদি (enc != রিক্ত && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    যদি (efile != রিক্ত && (pstate->fd = mch_fopen((বর্ণ *)efile, "r")) == রিক্ত)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	প্রত্যাগম FAIL;
    }

    যদি (tv != রিক্ত)
    {
	যদি (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	অন্যথা যদি (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    প্রত্যাগম OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    স্থির শূন্য
qf_cleanup_state(qfstate_T *pstate)
{
    যদি (pstate->fd != রিক্ত)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    যদি (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, রিক্ত, রিক্ত);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    স্থির পূর্ণাঙ্ক
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    পূর্ণাঙ্ক		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    যদি (status != QF_OK)
	প্রত্যাগম status;

    status = qf_parse_সারি(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    যদি (status != QF_OK)
	প্রত্যাগম status;

    প্রত্যাগম qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != রিক্ত)
		? fields->namebuf
		: ((qfl->qf_currfile != রিক্ত && fields->valid)
		    ? qfl->qf_currfile : (char_u *)রিক্ত),
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
    স্থির পূর্ণাঙ্ক
qf_init_ext(
    qf_info_T	    *qi,
    পূর্ণাঙ্ক		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    পূর্ণাঙ্ক		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = রিক্ত;
    পূর্ণাঙ্ক		    adding = FALSE;
    স্থির efm_T    *fmt_first = রিক্ত;
    char_u	    *efm;
    স্থির char_u   *last_efm = রিক্ত;
    পূর্ণাঙ্ক		    retval = -1;	// default: return error flag
    পূর্ণাঙ্ক		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    যদি ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	গচ্ছ qf_init_end;

    যদি (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    অন্যথা
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	যদি (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    যদি (errorformat == p_efm && tv == রিক্ত && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    অন্যথা
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    যদি (last_efm == রিক্ত || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	যদি (fmt_first != রিক্ত)
	    last_efm = vim_strsave(efm);
    }

    যদি (fmt_first == রিক্ত)	// nothing found
	গচ্ছ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    যাবত্ (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	যদি (status == QF_NOMEM)		// memory alloc failure
	    গচ্ছ qf_init_end;
	যদি (status == QF_END_OF_প্রবেশ)	// end of input
	    ভঙ্গ;
	যদি (status == QF_FAIL)
	    গচ্ছ error2;

	সারি_breakcheck();
    }
    যদি (state.fd == রিক্ত || !ferror(state.fd))
    {
	যদি (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	অন্যথা
	{
	    qfl->qf_nonevalid = FALSE;
	    যদি (qfl->qf_ptr == রিক্ত)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_গুনতি;
	গচ্ছ qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    যদি (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	যদি (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    যদি (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    প্রত্যাগম retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    পূর্ণাঙ্ক
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	পূর্ণাঙ্ক	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    যদি (wp != রিক্ত)
	qi = ll_get_or_alloc_list(wp);
    যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    প্রত্যাগম qf_init_ext(qi, qi->qf_curlist, efile, curbuf, রিক্ত, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    স্থির শূন্য
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    যদি (title == রিক্ত)
	প্রত্যাগম;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    যদি (p != রিক্ত)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    স্থির char_u *
qf_cmdtitle(char_u *cmd)
{
    স্থির char_u qftitle_str[IOSIZE];

    vim_snprintf((বর্ণ *)qftitle_str, IOSIZE, ":%s", (বর্ণ *)cmd);
    প্রত্যাগম qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    স্থির qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    প্রত্যাগম qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    স্থির শূন্য
qf_বের_করো_স্তূপ(qf_info_T *qi, পূর্ণাঙ্ক adjust)
{
    পূর্ণাঙ্ক i;
    qf_free(&qi->qf_lists[0]);
    প্রতি (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, পরিমাণ(*qi->qf_lists));

    যদি (adjust)
    {
	qi->qf_listcount--;
	যদি (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	অন্যথা
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    স্থির শূন্য
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    যাবত্ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    যদি (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_বের_করো_স্তূপ(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    অন্যথা
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
    স্থির শূন্য
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    যদি (q == রিক্ত)
	প্রত্যাগম;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    পূর্ণাঙ্ক
qf_স্তূপ_get_bufnr(শূন্য)
{
    যদি (ql_info == রিক্ত)
	প্রত্যাগম INVALID_QFBUFNR;
    প্রত্যাগম ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    স্থির শূন্য
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    যদি (qi->qf_bufnr == INVALID_QFBUFNR)
	প্রত্যাগম;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    যদি (qfbuf != রিক্ত && qfbuf->b_nwindows == 0)
    {
	পূর্ণাঙ্ক buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	যদি (curwin->w_buffer == রিক্ত)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(রিক্ত, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	যদি (buf_was_null)
	    curwin->w_buffer = রিক্ত;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    স্থির শূন্য
qf_free_list_স্তূপ_বস্তুসমূহ(qf_info_T *qi)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    স্থির শূন্য
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_স্তূপ_বস্তুসমূহ(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    স্থির শূন্য
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    যদি (qi == রিক্ত)
	প্রত্যাগম;
    *pqi = রিক্ত;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    যদি (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	প্রত্যাগম;
    }

    qi->qf_refcount--;
    যদি (qi->qf_refcount < 1)
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
    শূন্য
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    যদি (wp != রিক্ত)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    অন্যথা যদি (qi != রিক্ত)
	qf_free_list_স্তূপ_বস্তুসমূহ(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    স্থির শূন্য
incr_quickfix_busy(শূন্য)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    স্থির শূন্য
decr_quickfix_busy(শূন্য)
{
    যদি (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	যাবত্ (qf_delq_head != রিক্ত)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    যদি (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# যদি defined(EXITFREE)
    শূন্য
check_quickfix_busy(শূন্য)
{
    যদি (quickfix_busy != 0)
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
    স্থির পূর্ণাঙ্ক
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    পূর্ণাঙ্ক		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    পূর্ণাঙ্ক		col,		// column
    পূর্ণাঙ্ক		end_col,	// column for end
    পূর্ণাঙ্ক		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    পূর্ণাঙ্ক		nr,		// error number
    পূর্ণাঙ্ক		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    পূর্ণাঙ্ক		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = রিক্ত;
    char_u	*p = রিক্ত;

    যদি ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == রিক্ত)
	প্রত্যাগম QF_FAIL;
    যদি (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	যদি (buf != রিক্ত)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    অন্যথা
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    যদি (fname != রিক্ত)
	fullname = fix_fname(fname);
    qfp->qf_fname = রিক্ত;
    যদি (buf != রিক্ত &&
	buf->b_ffname != রিক্ত && fullname != রিক্ত)
    {
	যদি (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    যদি (p != রিক্ত)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    যদি ((qfp->qf_text = vim_strsave(mesg)) == রিক্ত)
    {
	vim_free(qfp);
	প্রত্যাগম QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    যদি (user_data == রিক্ত || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    অন্যথা
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    যদি (pattern == রিক্ত || *pattern == NUL)
	qfp->qf_pattern = রিক্ত;
    অন্যথা যদি ((qfp->qf_pattern = vim_strsave(pattern)) == রিক্ত)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	প্রত্যাগম QF_FAIL;
    }
    যদি (module == রিক্ত || *module == NUL)
	qfp->qf_module = রিক্ত;
    অন্যথা যদি ((qfp->qf_module = vim_strsave(module)) == রিক্ত)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	প্রত্যাগম QF_FAIL;
    }
    qfp->qf_nr = nr;
    যদি (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    যদি (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = রিক্ত;
    }
    অন্যথা
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = রিক্ত;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_গুনতি;
    যদি (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_গুনতি;
	qfl->qf_ptr = qfp;
    }

    প্রত্যাগম QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    পূর্ণাঙ্ক
qf_resize_স্তূপ(পূর্ণাঙ্ক n)
{
    যদি (ql_info == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (qf_resize_স্তূপ_base(ql_info, n) == FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    পূর্ণাঙ্ক
ll_resize_স্তূপ(win_T *wp, পূর্ণাঙ্ক n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    যদি (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    অন্যথা
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (qf_resize_স্তূপ_base(qi, n) == FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    স্থির পূর্ণাঙ্ক
qf_resize_স্তূপ_base(qf_info_T *qi, পূর্ণাঙ্ক n)
{
    qf_list_T *new;
    পূর্ণাঙ্ক amount_to_rm = 0, i;

    যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    size_t lsz = পরিমাণ(*qi->qf_lists);

    যদি (n == qi->qf_maxcount)
	প্রত্যাগম OK;
    অন্যথা যদি (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	প্রতি (i = 0; i < amount_to_rm; i++)
	    qf_বের_করো_স্তূপ(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    যদি (new == রিক্ত)
	প্রত্যাগম FAIL;

    // fill with zeroes any newly allocated memory
    যদি (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, রিক্ত);

    প্রত্যাগম OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   শূন্য
qf_init_স্তূপ(শূন্য)
{
    ql_info = qf_alloc_স্তূপ(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    স্থির শূন্য
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    যদি (wp != রিক্ত)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    স্থির শূন্য
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    যদি (llw != রিক্ত)
	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		প্রত্যাগম;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    স্থির qf_info_T *
qf_alloc_স্তূপ(qfltype_T qfltype, পূর্ণাঙ্ক n)
{
    qf_info_T *qi;

    যদি (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    অন্যথা
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	যদি (qi == রিক্ত)
	    প্রত্যাগম রিক্ত;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_স্তূপ(n);
    যদি (qi->qf_lists == রিক্ত)
    {
	যদি (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	প্রত্যাগম রিক্ত;
    }
    qi->qf_maxcount = n;

    প্রত্যাগম qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    স্থির qf_list_T *
qf_alloc_list_স্তূপ(পূর্ণাঙ্ক n)
{
    প্রত্যাগম ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    স্থির qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    যদি (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	প্রত্যাগম wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    যদি (wp->w_llist == রিক্ত)
	// new location list
	wp->w_llist = qf_alloc_স্তূপ(QFLT_LOCATION, wp->w_p_lhi);

    প্রত্যাগম wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    স্থির qf_info_T *
qf_cmd_get_স্তূপ(exarg_T *eap, পূর্ণাঙ্ক print_emsg)
{
    qf_info_T	*qi = ql_info;

    যদি (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	যদি (qi == রিক্ত)
	{
	    যদি (print_emsg)
		emsg(_(e_no_location_list));
	    প্রত্যাগম রিক্ত;
	}
    }
    যদি (qi == রিক্ত && print_emsg)
	emsg(_(e_no_quickfix_স্তূপ));

    প্রত্যাগম qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    স্থির qf_info_T *
qf_cmd_get_or_alloc_স্তূপ(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    যদি (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	যদি (qi == রিক্ত)
	    প্রত্যাগম রিক্ত;
	*pwinp = curwin;
    }

    প্রত্যাগম qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    স্থির পূর্ণাঙ্ক
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    পূর্ণাঙ্ক		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_বস্তুসমূহ(from_qfl, from_qfp, i)
    {
	যদি (qf_add_entry(to_qfl,
		    রিক্ত,
		    রিক্ত,
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
	    প্রত্যাগম FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	যদি (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    প্রত্যাগম OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    স্থির পূর্ণাঙ্ক
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_গুনতি = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = রিক্ত;
    to_qfl->qf_last = রিক্ত;
    to_qfl->qf_ptr = রিক্ত;
    যদি (from_qfl->qf_title != রিক্ত)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    অন্যথা
	to_qfl->qf_title = রিক্ত;
    যদি (from_qfl->qf_ctx != রিক্ত)
    {
	to_qfl->qf_ctx = alloc_tv();
	যদি (to_qfl->qf_ctx != রিক্ত)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    অন্যথা
	to_qfl->qf_ctx = রিক্ত;
    যদি (from_qfl->qf_qftf_cb.cb_name != রিক্ত)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    অন্যথা
	to_qfl->qf_qftf_cb.cb_name = রিক্ত;

    যদি (from_qfl->qf_গুনতি)
	যদি (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    প্রত্যাগম FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    যদি (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    প্রত্যাগম OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    শূন্য
copy_loclist_স্তূপ(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    পূর্ণাঙ্ক		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    যদি (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    অন্যথা
	qi = from->w_llist;

    যদি (qi == রিক্ত)		    // no location list to copy
	প্রত্যাগম;

    // allocate a new location list, set size of stack to 'from' window value
    যদি ((to->w_llist = qf_alloc_স্তূপ(QFLT_LOCATION, from->w_p_lhi)) == রিক্ত)
	প্রত্যাগম;
    অন্যথা
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    প্রতি (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	যদি (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    প্রত্যাগম;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    স্থির পূর্ণাঙ্ক
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {রিক্ত, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    যদি (fname == রিক্ত || *fname == NUL)		// no file name
	প্রত্যাগম 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    যদি (directory != রিক্ত)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    যদি (directory != রিক্ত && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != রিক্ত)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	যদি (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    যদি (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    অন্যথা
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    যদি (ptr.string == রিক্ত)
		প্রত্যাগম 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    অন্যথা
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    যদি (qf_last_bufname != রিক্ত && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    অন্যথা
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, রিক্ত, (linenr_T)0, BLN_NOOPT);
	যদি (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	অন্যথা
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    যদি (buf == রিক্ত)
	প্রত্যাগম 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    প্রত্যাগম buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    স্থির char_u *
qf_ঠেলো_dir(char_u *dirbuf, সংরচনা dir_স্তূপ_T **stackptr, পূর্ণাঙ্ক is_file_স্তূপ)
{
    সংরচনা dir_স্তূপ_T  *ds_new;
    সংরচনা dir_স্তূপ_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(সংরচনা dir_স্তূপ_T, aid_qf_dirstack);
    যদি (ds_new == রিক্ত)
	প্রত্যাগম রিক্ত;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    যদি (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == রিক্ত
	    || is_file_স্তূপ)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    অন্যথা
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = রিক্ত;
	যাবত্ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    যদি (dirname.string == রিক্ত)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		প্রত্যাগম রিক্ত;
	    }
	    যদি (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ভঙ্গ;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	যাবত্ ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	যদি (ds_new == রিক্ত)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    যদি ((*stackptr)->dirname != রিক্ত)
	প্রত্যাগম (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    প্রত্যাগম রিক্ত;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    স্থির char_u *
qf_বের_করো_dir(সংরচনা dir_স্তূপ_T **stackptr)
{
    সংরচনা dir_স্তূপ_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    যদি (*stackptr != রিক্ত)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    প্রত্যাগম *stackptr ? (*stackptr)->dirname : রিক্ত;
}

/*
 * clean up directory stack
 */
    স্থির শূন্য
qf_clean_dir_স্তূপ(সংরচনা dir_স্তূপ_T **stackptr)
{
    সংরচনা dir_স্তূপ_T  *ds_ptr;

    যাবত্ ((ds_ptr = *stackptr) != রিক্ত)
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
    স্থির char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    সংরচনা dir_স্তূপ_T     *ds_ptr;
    সংরচনা dir_স্তূপ_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    যদি (qfl->qf_dir_স্তূপ == রিক্ত)
	প্রত্যাগম রিক্ত;

    ds_ptr = qfl->qf_dir_স্তূপ->next;
    fullname.string = রিক্ত;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    যাবত্ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	যদি ((fullname.string != রিক্ত) && (mch_getperm(fullname.string) >= 0))
	    ভঙ্গ;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    যাবত্ (qfl->qf_dir_স্তূপ->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_স্তূপ->next;
	qfl->qf_dir_স্তূপ->next = qfl->qf_dir_স্তূপ->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    প্রত্যাগম ds_ptr == রিক্ত ? রিক্ত : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    স্থির পূর্ণাঙ্ক
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    পূর্ণাঙ্ক		i;

    যদি (wp != রিক্ত)
    {
	যদি (!win_valid(wp))
	    প্রত্যাগম FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    যদি (qi == রিক্ত)
	প্রত্যাগম FALSE;

    প্রতি (i = 0; i < qi->qf_listcount; ++i)
	যদি (qi->qf_lists[i].qf_id == qf_id)
	    প্রত্যাগম TRUE;

    প্রত্যাগম FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    স্থির পূর্ণাঙ্ক
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
	যদি (qfp == qf_ptr)
	    ভঙ্গ;

    যদি (i > qfl->qf_গুনতি) // Entry is not found
	প্রত্যাগম FALSE;

    প্রত্যাগম TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    স্থির qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	পূর্ণাঙ্ক		*qf_index,
	পূর্ণাঙ্ক		dir)
{
    পূর্ণাঙ্ক			idx;
    পূর্ণাঙ্ক			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    কুরু
    {
	যদি (idx == qfl->qf_গুনতি || qf_ptr->qf_next == রিক্ত)
	    প্রত্যাগম রিক্ত;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } যাবত্ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    প্রত্যাগম qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    স্থির qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	পূর্ণাঙ্ক		*qf_index,
	পূর্ণাঙ্ক		dir)
{
    পূর্ণাঙ্ক			idx;
    পূর্ণাঙ্ক			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    কুরু
    {
	যদি (idx == 1 || qf_ptr->qf_prev == রিক্ত)
	    প্রত্যাগম রিক্ত;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } যাবত্ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    প্রত্যাগম qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    স্থির qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	পূর্ণাঙ্ক		errornr,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    পূর্ণাঙ্ক			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    পূর্ণাঙ্ক			prev_index;
    বর্ণ		*err = e_no_more_বস্তুসমূহ;

    যাবত্ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	যদি (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	অন্যথা
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	যদি (qf_ptr == রিক্ত)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    যদি (err != রিক্ত)
	    {
		emsg(_(err));
		প্রত্যাগম রিক্ত;
	    }
	    ভঙ্গ;
	}

	err = রিক্ত;
    }

    *new_qfidx = qf_idx;
    প্রত্যাগম qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    স্থির qfline_T *
get_nth_entry(qf_list_T *qfl, পূর্ণাঙ্ক errornr, পূর্ণাঙ্ক *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    পূর্ণাঙ্ক		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    যাবত্ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != রিক্ত)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    যাবত্ (errornr > qf_idx && qf_idx < qfl->qf_গুনতি &&
						qf_ptr->qf_next != রিক্ত)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    প্রত্যাগম qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    স্থির qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	পূর্ণাঙ্ক		errornr,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    পূর্ণাঙ্ক		qfidx = qfl->qf_index;

    যদি (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    অন্যথা যদি (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    প্রত্যাগম qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    স্থির win_T *
qf_find_help_win(শূন্য)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	যদি (bt_help(wp->w_buffer))
	    প্রত্যাগম wp;

    প্রত্যাগম রিক্ত;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    স্থির শূন্য
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    স্থির পূর্ণাঙ্ক
jump_to_help_window(qf_info_T *qi, পূর্ণাঙ্ক newwin, পূর্ণাঙ্ক *opened_window)
{
    win_T	*wp;
    পূর্ণাঙ্ক		flags;

    যদি (cmdmod.cmod_tab != 0 || newwin)
	wp = রিক্ত;
    অন্যথা
	wp = qf_find_help_win();
    যদি (wp != রিক্ত && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    অন্যথা
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	যদি (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_শীর্ষ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	যদি (IS_LL_স্তূপ(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	যদি (win_split(0, flags) == FAIL)
	    প্রত্যাগম FAIL;

	*opened_window = TRUE;

	যদি (curwin->w_height < p_hh)
	    win_setheight((পূর্ণাঙ্ক)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	যদি (IS_LL_স্তূপ(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    যদি (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    প্রত্যাগম OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    স্থির win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    প্রত্যাগম wp;

    প্রত্যাগম রিক্ত;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    স্থির win_T *
qf_find_win_with_normal_buf(শূন্য)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (bt_normal(wp->w_buffer))
	    প্রত্যাগম wp;

    প্রত্যাগম রিক্ত;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    স্থির পূর্ণাঙ্ক
qf_goto_tabwin_with_file(পূর্ণাঙ্ক fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    প্রত্যাগম TRUE;
	}

    প্রত্যাগম FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    স্থির পূর্ণাঙ্ক
qf_open_new_file_win(qf_info_T *ll_ref)
{
    পূর্ণাঙ্ক		flags;

    flags = WSP_ABOVE;
    যদি (ll_ref != রিক্ত)
	flags |= WSP_NEWLOC;
    যদি (win_split(0, flags) == FAIL)
	প্রত্যাগম FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    যদি (ll_ref != রিক্ত)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    প্রত্যাগম OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    স্থির শূন্য
qf_goto_win_with_ll_file(win_T *use_win, পূর্ণাঙ্ক qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    যদি (win == রিক্ত)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    যদি (win->w_buffer->b_fnum == qf_fnum)
		ভঙ্গ;
	যদি (win == রিক্ত)
	{
	    // Find a previous usable window
	    win = curwin;
	    কুরু
	    {
		যদি (bt_normal(win->w_buffer))
		    ভঙ্গ;
		যদি (win->w_prev == রিক্ত)
		    win = lastwin;	// wrap around the top
		অন্যথা
		    win = win->w_prev; // go to previous window
	    } যাবত্ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    যদি (win->w_llist == রিক্ত && ll_ref != রিক্ত)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    স্থির শূন্য
qf_goto_win_with_qfl_file(পূর্ণাঙ্ক qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = রিক্ত;
    প্রতি (;;)
    {
	যদি (win->w_buffer->b_fnum == qf_fnum)
	    ভঙ্গ;
	যদি (win->w_prev == রিক্ত)
	    win = lastwin;	// wrap around the top
	অন্যথা
	    win = win->w_prev;	// go to previous window

	যদি (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    যদি ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    অন্যথা যদি (altwin != রিক্ত)
		win = altwin;
	    অন্যথা যদি (curwin->w_prev != রিক্ত)
		win = curwin->w_prev;
	    অন্যথা
		win = curwin->w_next;
	    ভঙ্গ;
	}

	// Remember a usable window.
	যদি (altwin == রিক্ত && !win->w_p_pvw && !win->w_p_wfb &&
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
    স্থির পূর্ণাঙ্ক
qf_jump_to_usable_window(পূর্ণাঙ্ক qf_fnum, পূর্ণাঙ্ক newwin, পূর্ণাঙ্ক *opened_window)
{
    win_T	*usable_wp = রিক্ত;
    পূর্ণাঙ্ক		usable_win = FALSE;
    qf_info_T	*ll_ref = রিক্ত;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    যদি (!newwin)
	ll_ref = curwin->w_llist_ref;

    যদি (ll_ref != রিক্ত)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	যদি (usable_wp != রিক্ত)
	    usable_win = TRUE;
    }

    যদি (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	যদি (win != রিক্ত)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    যদি (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    যদি ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	যদি (qf_open_new_file_win(ll_ref) != OK)
	    প্রত্যাগম FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    অন্যথা
    {
	যদি (curwin->w_llist_ref != রিক্ত)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	অন্যথা					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    প্রত্যাগম OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    স্থির পূর্ণাঙ্ক
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	পূর্ণাঙ্ক		forceit,
	পূর্ণাঙ্ক		prev_winid,
	পূর্ণাঙ্ক		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    পূর্ণাঙ্ক		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		old_qf_curlist = qi->qf_curlist;
    পূর্ণাঙ্ক		save_qfid = qfl->qf_id;

    যদি (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	যদি (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    প্রত্যাগম FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, রিক্ত, রিক্ত, রিক্ত, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : রিক্ত);
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	fnum = qf_ptr->qf_fnum;

	যদি (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    যদি (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		প্রত্যাগম FAIL;
	    }

	    যদি (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    যদি (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		যদি (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		যদি (curwin->w_p_wfb)
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

	যদি (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    যদি (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	যদি (wp == রিক্ত && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    প্রত্যাগম QF_ABORT;
	}
    }

    যদি (qfl_type == QFLT_QUICKFIX && !qflist_valid(রিক্ত, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	প্রত্যাগম QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    যদি (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	যদি (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	অন্যথা
	    emsg(_(e_current_location_list_was_changed));
	প্রত্যাগম QF_ABORT;
    }

    প্রত্যাগম retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    স্থির পূর্ণাঙ্ক
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*সারি = ml_get_curline();
    char_u	*p = সারি;
    colnr_T	col = 0;

    যাবত্ (*p != NUL && col < vcol)
    {
	যদি (*p == TAB)
	    col += 8 - (col % 8);
	অন্যথা
	    col += ptr2cells(p);
	যদি (col > vcol)
	    ভঙ্গ;
	MB_PTR_ADV(p);
    }
    প্রত্যাগম (পূর্ণাঙ্ক)(p - সারি);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    স্থির শূন্য
qf_jump_goto_সারি(
	linenr_T	qf_lnum,
	পূর্ণাঙ্ক		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    যদি (qf_pattern == রিক্ত)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	যদি (i > 0)
	{
	    যদি (i > curbuf->b_ml.ml_সারি_গুনতি)
		i = curbuf->b_ml.ml_সারি_গুনতি;
	    curwin->w_cursor.lnum = i;
	}
	যদি (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    যদি (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    অন্যথা
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	অন্যথা
	    beginline(BL_WHITE | BL_FIX);
    }
    অন্যথা
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	যদি (!do_অনুসন্ধান(রিক্ত, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, অনুসন্ধান_KEEP, রিক্ত))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    স্থির শূন্য
qf_jump_print_বার্তা(
	qf_info_T	*qi,
	পূর্ণাঙ্ক		qf_index,
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
    যদি (!বার্তা_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((বর্ণ *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_গুনতি,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (বর্ণ *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = বার্তা_scroll;
    যদি (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	বার্তা_scroll = TRUE;
    অন্যথা যদি (!বার্তা_scrolled && shortmess(SHM_OVERALL))
	বার্তা_scroll = FALSE;
    বার্তা_attr_keep((বর্ণ *)gap->ga_data, 0, TRUE);
    বার্তা_scroll = i;

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
    স্থির পূর্ণাঙ্ক
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	পূর্ণাঙ্ক		newwin,
	পূর্ণাঙ্ক		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    পূর্ণাঙ্ক		old_changedtick = qfl->qf_changedtick;
    পূর্ণাঙ্ক		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    যদি (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	যদি (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    প্রত্যাগম FAIL;
    যদি (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	যদি (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	অন্যথা
	    emsg(_(e_current_location_list_was_changed));
	প্রত্যাগম QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    যদি (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	যদি (qf_ptr->qf_fnum == 0)
	    প্রত্যাগম NOTDONE;

	যদি (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    প্রত্যাগম FAIL;
    }
    যদি (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	যদি (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	অন্যথা
	    emsg(_(e_current_location_list_was_changed));
	প্রত্যাগম QF_ABORT;
    }

    প্রত্যাগম OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    স্থির পূর্ণাঙ্ক
qf_jump_to_buffer(
	qf_info_T	*qi,
	পূর্ণাঙ্ক		qf_index,
	qfline_T	*qf_ptr,
	পূর্ণাঙ্ক		forceit,
	পূর্ণাঙ্ক		prev_winid,
	পূর্ণাঙ্ক		*opened_window,
	পূর্ণাঙ্ক		openfold,
	পূর্ণাঙ্ক		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    পূর্ণাঙ্ক		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    যদি (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	যদি (retval != OK)
	    প্রত্যাগম retval;
    }

    // When not switched to another buffer, still need to set pc mark
    যদি (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_সারি(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    যদি (print_message)
	qf_jump_print_বার্তা(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    প্রত্যাগম retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    শূন্য
qf_jump(qf_info_T	*qi,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		errornr,
	পূর্ণাঙ্ক		forceit)
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
    স্থির শূন্য
qf_jump_newwin(qf_info_T	*qi,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		errornr,
	পূর্ণাঙ্ক		forceit,
	পূর্ণাঙ্ক		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    পূর্ণাঙ্ক			qf_index;
    পূর্ণাঙ্ক			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    পূর্ণাঙ্ক			prev_winid;
    পূর্ণাঙ্ক			opened_window = FALSE;
    পূর্ণাঙ্ক			print_message = TRUE;
    পূর্ণাঙ্ক			old_চাবি_Typed = চাবি_Typed; // getting file may reset it
    পূর্ণাঙ্ক			retval = OK;

    যদি (qi == রিক্ত)
    {
	যদি (ql_info == রিক্ত)
	{
	    emsg(_(e_no_quickfix_স্তূপ));
	    প্রত্যাগম;
	}
	qi = ql_info;
    }

    যদি (qf_স্তূপ_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	প্রত্যাগম;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    যদি (qf_ptr == রিক্ত)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	গচ্ছ theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    যদি (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    যদি (retval == FAIL)
	গচ্ছ failed;
    যদি (retval == QF_ABORT)
    {
	qi = রিক্ত;
	qf_ptr = রিক্ত;
	গচ্ছ theend;
    }
    যদি (retval == NOTDONE)
	গচ্ছ theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_চাবি_Typed, print_message);
    যদি (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = রিক্ত;
	qf_ptr = রিক্ত;
    }

    যদি (retval != OK)
    {
	যদি (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	যদি (qf_ptr != রিক্ত && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    যদি (qi != রিক্ত)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    যদি (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
স্থির পূর্ণাঙ্ক	qfFileAttr;
স্থির পূর্ণাঙ্ক	qfSepAttr;
স্থির পূর্ণাঙ্ক	qf_সারি_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    স্থির শূন্য
qf_list_entry(qfline_T *qfp, পূর্ণাঙ্ক qf_idx, পূর্ণাঙ্ক cursel)
{
    char_u	*fname;
    buf_T	*buf;
    পূর্ণাঙ্ক		filter_entry;
    garray_T	*gap;

    fname = রিক্ত;
    যদি (qfp->qf_module != রিক্ত && *qfp->qf_module != NUL)
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(বর্ণ *)qfp->qf_module);
    অন্যথা
    {
	যদি (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != রিক্ত)
	{
	    যদি (qfp->qf_fname == রিক্ত)
		fname = buf->b_fname;
	    অন্যথা
		fname = qfp->qf_fname;
	    যদি (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	যদি (fname == রিক্ত)
	    sprintf((বর্ণ *)IObuff, "%2d", qf_idx);
	অন্যথা
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (বর্ণ *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    যদি (qfp->qf_module != রিক্ত && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    যদি (filter_entry && fname != রিক্ত)
	filter_entry &= message_filtered(fname);
    যদি (filter_entry && qfp->qf_pattern != রিক্ত)
	filter_entry &= message_filtered(qfp->qf_pattern);
    যদি (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    যদি (filter_entry)
	প্রত্যাগম;

    বার্তা_putchar('\n');
    বার্তা_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    যদি (qfp->qf_lnum != 0)
	বার্তা_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    যদি (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    বার্তা_puts_attr((বর্ণ *)gap->ga_data, qf_সারি_Attr);
    বার্তা_puts_attr(":", qfSepAttr);
    যদি (qfp->qf_pattern != রিক্ত)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	বার্তা_puts((বর্ণ *)gap->ga_data);
	বার্তা_puts_attr(":", qfSepAttr);
    }
    বার্তা_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != রিক্ত || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    বার্তা_prt_সারি((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    শূন্য
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		idx1 = 1;
    পূর্ণাঙ্ক		idx2 = -1;
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		plus = FALSE;
    পূর্ণাঙ্ক		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    যদি (qf_স্তূপ_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	প্রত্যাগম;
    }
    যদি (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    যদি (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	প্রত্যাগম;
    }
    qfl = qf_get_curlist(qi);
    যদি (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    অন্যথা
    {
	i = qfl->qf_গুনতি;
	যদি (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	যদি (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    যদি (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    যদি (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_সারি_Attr = syn_name2attr((char_u *)"qfLineNr");
    যদি (qf_সারি_Attr == 0)
	qf_সারি_Attr = HL_ATTR(HLF_N);

    যদি (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
    {
	যদি ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    স্থির শূন্য
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    যাবত্ (*p != NUL)
    {
	যদি (*p == '\n')
	{
	    ga_append(gap, ' ');
	    যাবত্ (*++p != NUL)
		যদি (!VIM_ISWHITE(*p) && *p != '\n')
		    ভঙ্গ;
	}
	অন্যথা
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    স্থির শূন্য
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((বর্ণ *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    যদি (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((বর্ণ *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    যদি (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((বর্ণ *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	যদি (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((বর্ণ *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    স্থির শূন্য
qf_বার্তা(qf_info_T *qi, পূর্ণাঙ্ক which, বর্ণ *lead)
{
    বর্ণ   *title = (বর্ণ *)qi->qf_lists[which].qf_title;
    পূর্ণাঙ্ক    গুনতি = qi->qf_lists[which].qf_গুনতি;
    char_u buf[IOSIZE];

    vim_snprintf((বর্ণ *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    গুনতি);

    যদি (title != রিক্ত)
    {
	size_t	len = STRLEN(buf);

	যদি (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    বার্তা((বর্ণ *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    শূন্য
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    পূর্ণাঙ্ক		গুনতি;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    যদি (eap->addr_গুনতি != 0)
	গুনতি = eap->line2;
    অন্যথা
	গুনতি = 1;
    যাবত্ (গুনতি--)
    {
	যদি (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    যদি (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_স্তূপ));
		ভঙ্গ;
	    }
	    --qi->qf_curlist;
	}
	অন্যথা
	{
	    যদি (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_শীর্ষ_of_quickfix_স্তূপ));
		ভঙ্গ;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_বার্তা(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, রিক্ত);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    শূন্য
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_স্তূপ(eap, FALSE);
    পূর্ণাঙ্ক		i;

    যদি (eap->addr_গুনতি > 0)
    {
	যদি (qi == রিক্ত)
	{
	    emsg(_(e_no_location_list));
	    প্রত্যাগম;
	}

	// Jump to the specified quickfix list
	যদি (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_বার্তা(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, রিক্ত);
	}
	অন্যথা
	    emsg(_(e_invalid_range));

	প্রত্যাগম;
    }

    যদি (qf_স্তূপ_empty(qi))
	বার্তা(_("No entries"));
    অন্যথা
	প্রতি (i = 0; i < qi->qf_listcount; ++i)
	    qf_বার্তা(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    স্থির শূন্য
qf_free_বস্তুসমূহ(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    পূর্ণাঙ্ক		stop = FALSE;

    যাবত্ (qfl->qf_গুনতি && qfl->qf_start != রিক্ত)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	যদি (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    যদি (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_গুনতি = 1;
	    অন্যথা
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_গুনতি;
    }

    qfl->qf_index = 0;
    qfl->qf_start = রিক্ত;
    qfl->qf_last = রিক্ত;
    qfl->qf_ptr = রিক্ত;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_স্তূপ(&qfl->qf_dir_স্তূপ);
    qfl->qf_directory = রিক্ত;
    qf_clean_dir_স্তূপ(&qfl->qf_file_স্তূপ);
    qfl->qf_currfile = রিক্ত;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    স্থির শূন্য
qf_free(qf_list_T *qfl)
{
    qf_free_বস্তুসমূহ(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = রিক্ত;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   শূন্য
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    পূর্ণাঙ্ক		i;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		idx;
    qf_info_T	*qi = ql_info;
    পূর্ণাঙ্ক		found_one = FALSE;
    পূর্ণাঙ্ক		buf_has_flag = wp == রিক্ত ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    যদি (!(curbuf->b_has_qf_entry & buf_has_flag))
	প্রত্যাগম;
    যদি (wp != রিক্ত)
    {
	যদি (wp->w_llist == রিক্ত)
	    প্রত্যাগম;
	qi = wp->w_llist;
    }
    অন্যথা যদি (qi == রিক্ত)
	প্রত্যাগম;

    প্রতি (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	যদি (!qf_list_empty(qfl))
	    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
		যদি (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    যদি (qfp->qf_cleared)
			অনুবর্ত;

		    যদি (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			যদি (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			অন্যথা
			    qfp->qf_lnum += amount;
		    }
		    অন্যথা যদি (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    যদি (!found_one)
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
    স্থির char_u *
qf_types(পূর্ণাঙ্ক c, পূর্ণাঙ্ক nr)
{
    স্থির char_u	buf[20];
    স্থির char_u	cc[3];
    char_u		*p;

    যদি (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    অন্যথা যদি (c == 'I' || c == 'i')
	p = (char_u *)" info";
    অন্যথা যদি (c == 'N' || c == 'n')
	p = (char_u *)" note";
    অন্যথা যদি (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    অন্যথা যদি (c == 0 || c == 1)
	p = (char_u *)"";
    অন্যথা
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    যদি (nr <= 0)
	প্রত্যাগম p;

    sprintf((বর্ণ *)buf, "%s %3d", (বর্ণ *)p, nr);
    প্রত্যাগম buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    শূন্য
qf_view_result(পূর্ণাঙ্ক split)
{
    qf_info_T   *qi = ql_info;

    যদি (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    অন্যথা যদি (qi == রিক্ত)
    {
	emsg(_(e_no_quickfix_স্তূপ));
	প্রত্যাগম;
    }

    যদি (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	প্রত্যাগম;
    }

    যদি (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	প্রত্যাগম;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    শূন্য
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    যদি (qf_স্তূপ_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	যদি (win != রিক্ত)
	    ex_cclose(eap);
    }
    অন্যথা যদি (win == রিক্ত)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    শূন্য
ex_cclose(exarg_T *eap)
{
    win_T	*win = রিক্ত;
    qf_info_T	*qi;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, FALSE)) == রিক্ত)
	প্রত্যাগম;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    যদি (win != রিক্ত)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    স্থির শূন্য
qf_set_title_var(qf_list_T *qfl)
{
    যদি (qfl->qf_title != রিক্ত)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
qf_goto_cwindow(qf_info_T *qi, পূর্ণাঙ্ক resize, পূর্ণাঙ্ক sz, পূর্ণাঙ্ক vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    যদি (win == রিক্ত)
	প্রত্যাগম FAIL;

    win_goto(win);
    যদি (resize)
    {
	যদি (vertsplit)
	{
	    যদি (sz != win->w_width)
		win_setwidth(sz);
	}
	অন্যথা যদি (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    প্রত্যাগম OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    স্থির শূন্য
qf_set_cwindow_options(শূন্য)
{
    // switch off 'swapfile'
    set_option_মান_give_err((char_u *)"swf", 0L, রিক্ত, OPT_LOCAL);
    set_option_মান_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_মান_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_মান_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    স্থির পূর্ণাঙ্ক
qf_open_new_cwindow(qf_info_T *qi, পূর্ণাঙ্ক height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    পূর্ণাঙ্ক		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    যদি (IS_QF_স্তূপ(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    যদি (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    যদি (IS_QF_স্তূপ(qi))
	flags |= WSP_QUICKFIX;

    যদি (win_split(height, flags) == FAIL)
	প্রত্যাগম FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    যদি (IS_LL_স্তূপ(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    যদি (oldwin != curwin)
	oldwin = রিক্ত;  // don't store info when in another window
    যদি (qf_buf != রিক্ত)
    {
	// Use the existing quickfix buffer
	যদি (do_ecmd(qf_buf->b_fnum, রিক্ত, রিক্ত, রিক্ত, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    প্রত্যাগম FAIL;
    }
    অন্যথা
    {
	// Create a new quickfix buffer
	যদি (do_ecmd(0, রিক্ত, রিক্ত, রিক্ত, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    প্রত্যাগম FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    যদি (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    যদি (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    যদি (win_valid(win))
	prevwin = win;

    প্রত্যাগম OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    শূন্য
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    পূর্ণাঙ্ক		height;
    পূর্ণাঙ্ক		status = FAIL;
    পূর্ণাঙ্ক		lnum;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    incr_quickfix_busy();

    যদি (eap->addr_গুনতি != 0)
	height = eap->line2;
    অন্যথা
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    যদি (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_গুনতি != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    যদি (status == FAIL)
	যদি (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    প্রত্যাগম;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, রিক্ত, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    স্থির শূন্য
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
    শূন্য
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    win = qf_find_win(qi);
    যদি (win != রিক্ত && win->w_cursor.lnum != win->w_buffer->b_ml.ml_সারি_গুনতি)
	qf_win_goto(win, win->w_buffer->b_ml.ml_সারি_গুনতি);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    যদি (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    অন্যথা যদি (qi == রিক্ত)
	প্রত্যাগম 0;

    প্রত্যাগম qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    স্থির পূর্ণাঙ্ক
qf_win_pos_update(
    qf_info_T	*qi,
    পূর্ণাঙ্ক		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    পূর্ণাঙ্ক		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    যদি (win != রিক্ত
	    && qf_index <= win->w_buffer->b_ml.ml_সারি_গুনতি
	    && old_qf_index != qf_index)
    {
	যদি (qf_index > old_qf_index)
	{
	    win->w_redraw_শীর্ষ = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	অন্যথা
	{
	    win->w_redraw_শীর্ষ = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    প্রত্যাগম win != রিক্ত;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    স্থির পূর্ণাঙ্ক
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    যদি (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	যদি ((IS_QF_স্তূপ(qi) && win->w_llist_ref == রিক্ত)
		|| (IS_LL_স্তূপ(qi) && win->w_llist_ref == qi))
	    প্রত্যাগম TRUE;

    প্রত্যাগম FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    স্থির win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	যদি (is_qf_win(win, qi))
	    প্রত্যাগম win;
    প্রত্যাগম রিক্ত;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    স্থির buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    যদি (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	যদি (qfbuf != রিক্ত)
	    প্রত্যাগম qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	যদি (is_qf_win(win, qi))
	    প্রত্যাগম win->w_buffer;

    প্রত্যাগম রিক্ত;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    বর্ণ *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    যদি (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	প্রত্যাগম e_invalid_argument;

    প্রত্যাগম রিক্ত;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    স্থির শূন্য
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	যদি (is_qf_win(win, qi))
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
    স্থির শূন্য
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    যদি (buf == রিক্ত)
	প্রত্যাগম;

    linenr_T	old_সারি_গুনতি = buf->b_ml.ml_সারি_গুনতি;
    পূর্ণাঙ্ক		qf_winid = 0;

    যদি (IS_LL_স্তূপ(qi))
    {
	যদি (curwin->w_llist == qi)
	    win = curwin;
	অন্যথা
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    যদি (win == রিক্ত)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    যদি (win == রিক্ত)
		প্রত্যাগম;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    পূর্ণাঙ্ক do_fill = TRUE;
    যদি (old_last == রিক্ত)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    যদি (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	যদি (old_last == রিক্ত)
	{
	    (শূন্য)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    যদি ((win = qf_find_win(qi)) != রিক্ত && old_সারি_গুনতি < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    স্থির পূর্ণাঙ্ক
qf_buf_add_সারি(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	পূর্ণাঙ্ক		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    যদি (qftf_str != রিক্ত && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    অন্যথা
    {
	যদি (qfp->qf_module != রিক্ত)
	    ga_concat(gap, qfp->qf_module);
	অন্যথা যদি (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != রিক্ত
		&& errbuf->b_fname != রিক্ত)
	{
	    যদি (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    অন্যথা
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		যদি (first_bufline && (errbuf->b_sfname == রিক্ত
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    যদি (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		যদি (qfp->qf_fname == রিক্ত)
		    ga_concat(gap, errbuf->b_fname);
		অন্যথা
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	যদি (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	অন্যথা যদি (qfp->qf_pattern != রিক্ত)
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
    যদি (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    স্থির list_T *
call_qftf_func(qf_list_T *qfl, পূর্ণাঙ্ক qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = রিক্ত;
    স্থির পূর্ণাঙ্ক	recursive = FALSE;

    যদি (recursive)
	প্রত্যাগম রিক্ত;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    যদি (qfl->qf_qftf_cb.cb_name != রিক্ত)
	cb = &qfl->qf_qftf_cb;
    যদি (cb->cb_name != রিক্ত)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	যদি ((d = dict_alloc_lock(VAR_FIXED)) == রিক্ত)
	{
	    recursive = FALSE;
	    প্রত্যাগম রিক্ত;
	}
	dict_add_সংখ্যা(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_সংখ্যা(d, "winid", (long)qf_winid);
	dict_add_সংখ্যা(d, "id", (long)qfl->qf_id);
	dict_add_সংখ্যা(d, "start_idx", start_idx);
	dict_add_সংখ্যা(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = রিক্ত;
	textlock++;
	যদি (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    যদি (rettv.v_type == VAR_LIST)
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
    প্রত্যাগম qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    স্থির শূন্য
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, পূর্ণাঙ্ক qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		old_চাবি_Typed = চাবি_Typed;
    list_T	*qftf_list = রিক্ত;
    listitem_T	*qftf_li = রিক্ত;

    যদি (old_last == রিক্ত)
    {
	win_T		*wp;
	tabpage_T	*tp;

	যদি (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    প্রত্যাগম;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	যাবত্ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (শূন্য)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    যদি (qfl != রিক্ত && qfl->qf_start != রিক্ত)
    {
	char_u		dirname[MAXPATHL];
	পূর্ণাঙ্ক		invalid_val = FALSE;
	পূর্ণাঙ্ক		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	যদি (old_last == রিক্ত)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	অন্যথা
	{
	    যদি (old_last->qf_next != রিক্ত)
		qfp = old_last->qf_next;
	    অন্যথা
		qfp = old_last;
	    lnum = buf->b_ml.ml_সারি_গুনতি;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_গুনতি);
	যদি (qftf_list != রিক্ত)
	    qftf_li = qftf_list->lv_first;

	যাবত্ (lnum < qfl->qf_গুনতি)
	{
	    char_u	*qftf_str = রিক্ত;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    যদি (qftf_li != রিক্ত && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		যদি (qftf_str == রিক্ত)
		    invalid_val = TRUE;
	    }

	    যদি (qf_buf_add_সারি(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ভঙ্গ;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    যদি (qfp == রিক্ত)
		ভঙ্গ;

	    যদি (qftf_li != রিক্ত)
		qftf_li = qftf_li->li_next;
	}

	যদি (old_last == রিক্ত)
	    // Delete the empty line which is now at the end
	    (শূন্য)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    যদি (old_last == রিক্ত)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_মান_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", রিক্ত,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", রিক্ত,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    চাবি_Typed = old_চাবি_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    স্থির শূন্য
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    স্থির পূর্ণাঙ্ক
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    পূর্ণাঙ্ক		qf_idx;

    প্রতি (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	যদি (qi->qf_lists[qf_idx].qf_id == qfid)
	    প্রত্যাগম qf_idx;
    প্রত্যাগম INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    স্থির পূর্ণাঙ্ক
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    পূর্ণাঙ্ক curlist;

    যদি (qf_get_curlist(qi)->qf_id == save_qfid)
	প্রত্যাগম OK;

    curlist = qf_id2nr(qi, save_qfid);
    যদি (curlist < 0)
	// list is not present
	প্রত্যাগম FAIL;
    qi->qf_curlist = curlist;
    প্রত্যাগম OK;
}

/*
 * Jump to the first entry if there is one.
 */
    স্থির শূন্য
qf_jump_first(qf_info_T *qi, int_u save_qfid, পূর্ণাঙ্ক forceit)
{
    যদি (qf_restore_list(qi, save_qfid) == FAIL)
	প্রত্যাগম;


    যদি (!check_can_set_curbuf_forceit(forceit))
	প্রত্যাগম;


    // Autocommands might have cleared the list, check for that.
    যদি (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    পূর্ণাঙ্ক
grep_internal(cmdidx_T cmdidx)
{
    প্রত্যাগম ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    স্থির char_u *
make_get_auname(cmdidx_T cmdidx)
{
    বিকল্প (cmdidx)
    {
	পক্ষ CMD_make:	    প্রত্যাগম (char_u *)"make";
	পক্ষ CMD_lmake:	    প্রত্যাগম (char_u *)"lmake";
	পক্ষ CMD_grep:	    প্রত্যাগম (char_u *)"grep";
	পক্ষ CMD_lgrep:	    প্রত্যাগম (char_u *)"lgrep";
	পক্ষ CMD_grepadd:   প্রত্যাগম (char_u *)"grepadd";
	পক্ষ CMD_lgrepadd:  প্রত্যাগম (char_u *)"lgrepadd";
	স্বভাব: প্রত্যাগম রিক্ত;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    স্থির char_u *
get_mef_name(শূন্য)
{
    char_u	*p;
    char_u	*name;
    স্থির পূর্ণাঙ্ক	start = -1;
    স্থির পূর্ণাঙ্ক	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    যদি (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	যদি (name == রিক্ত)
	    emsg(_(e_cant_get_temp_file_name));
	প্রত্যাগম name;
    }

    প্রতি (p = p_mef; *p; ++p)
	যদি (p[0] == '#' && p[1] == '#')
	    ভঙ্গ;

    যদি (*p == NUL)
	প্রত্যাগম vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    প্রতি (;;)
    {
	যদি (start == -1)
	    start = mch_get_pid();
	অন্যথা
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	যদি (name == রিক্ত)
	    ভঙ্গ;
	STRCPY(name, p_mef);
	sprintf((বর্ণ *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	যদি (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((বর্ণ *)name, &sb) < 0
# endif
		)
	    ভঙ্গ;
	vim_free(name);
    }
    প্রত্যাগম name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    স্থির char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    যদি (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    যদি (cmd == রিক্ত)
	প্রত্যাগম রিক্ত;
    sprintf((বর্ণ *)cmd, "%s%s%s", (বর্ণ *)p_shq, (বর্ণ *)makecmd,
							       (বর্ণ *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    যদি (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    যদি (বার্তা_col == 0)
	বার্তা_didout = FALSE;
    বার্তা_start();
    বার্তা_puts(":!");
    বার্তা_outtrans(cmd);		// show what we are doing

    প্রত্যাগম cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    শূন্য
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = রিক্ত;
    win_T	*wp = রিক্ত;
    qf_info_T	*qi = ql_info;
    পূর্ণাঙ্ক		res;
    char_u	*au_name = রিক্ত;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    পূর্ণাঙ্ক		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    যদি (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	প্রত্যাগম;
    }

    au_name = make_get_auname(eap->cmdidx);
    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	যদি (aborting())
	    প্রত্যাগম;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    যদি (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    যদি (fname == রিক্ত)
	প্রত্যাগম;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    যদি (cmd == রিক্ত)
    {
	vim_free(fname);
	প্রত্যাগম;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (শূন্য)char_avail();
# endif

    incr_quickfix_busy();

    যদি (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    যদি (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    যদি (wp != রিক্ত)
    {
	qi = GET_LOC_LIST(wp);
	যদি (qi == রিক্ত)
	    গচ্ছ cleanup;
    }
    অন্যথা যদি (qi == রিক্ত)
	গচ্ছ cleanup;

    যদি (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    যদি (au_name != রিক্ত)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    যদি (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    পূর্ণাঙ্ক
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, FALSE)) == রিক্ত)
	প্রত্যাগম 0;
    প্রত্যাগম qf_get_curlist(qi)->qf_গুনতি;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    পূর্ণাঙ্ক
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i, sz = 0;
    পূর্ণাঙ্ক		prev_fnum = 0;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, FALSE)) == রিক্ত)
	প্রত্যাগম 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
    {
	যদি (qfp->qf_valid)
	{
	    যদি (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    অন্যথা যদি (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    প্রত্যাগম sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    পূর্ণাঙ্ক
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, FALSE)) == রিক্ত)
	প্রত্যাগম 0;

    প্রত্যাগম qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    পূর্ণাঙ্ক
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i, eidx = 0;
    পূর্ণাঙ্ক		prev_fnum = 0;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, FALSE)) == রিক্ত)
	প্রত্যাগম 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    যদি (!qf_list_has_valid_entries(qfl))
	প্রত্যাগম 1;

    প্রতি (i = 1; i <= qfl->qf_index && qfp!= রিক্ত; i++, qfp = qfp->qf_next)
    {
	যদি (qfp->qf_valid)
	{
	    যদি (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		যদি (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    অন্যথা
		eidx++;
	}
    }

    প্রত্যাগম eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    স্থির পূর্ণাঙ্ক
qf_get_nth_valid_entry(qf_list_T *qfl, পূর্ণাঙ্ক n, পূর্ণাঙ্ক fdo)
{
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i, eidx;
    পূর্ণাঙ্ক		prev_fnum = 0;

    // check if the list has valid errors
    যদি (!qf_list_has_valid_entries(qfl))
	প্রত্যাগম 1;

    eidx = 0;
    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
    {
	যদি (qfp->qf_valid)
	{
	    যদি (fdo)
	    {
		যদি (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    অন্যথা
		eidx++;
	}

	যদি (eidx == n)
	    ভঙ্গ;
    }

    যদি (i <= qfl->qf_গুনতি)
	প্রত্যাগম i;
    অন্যথা
	প্রত্যাগম 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    শূন্য
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    পূর্ণাঙ্ক		errornr;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    যদি (eap->addr_গুনতি > 0)
	errornr = (পূর্ণাঙ্ক)eap->line2;
    অন্যথা
    {
	বিকল্প (eap->cmdidx)
	{
	    পক্ষ CMD_cc: পক্ষ CMD_ll:
		errornr = 0;
		ভঙ্গ;
	    পক্ষ CMD_crewind: পক্ষ CMD_lrewind: পক্ষ CMD_cfirst:
	    পক্ষ CMD_lfirst:
		errornr = 1;
		ভঙ্গ;
	    স্বভাব:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    যদি (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_গুনতি > 0 ? (পূর্ণাঙ্ক)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    শূন্য
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    পূর্ণাঙ্ক		errornr;
    পূর্ণাঙ্ক		dir;

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    যদি (eap->addr_গুনতি > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (পূর্ণাঙ্ক)eap->line2;
    অন্যথা
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    বিকল্প (eap->cmdidx)
    {
	পক্ষ CMD_cnext: পক্ষ CMD_lnext: পক্ষ CMD_cdo: পক্ষ CMD_ldo:
	    dir = FORWARD;
	    ভঙ্গ;
	পক্ষ CMD_cprevious: পক্ষ CMD_lprevious: পক্ষ CMD_cNext:
	পক্ষ CMD_lNext:
	    dir = BACKWARD;
	    ভঙ্গ;
	পক্ষ CMD_cnfile: পক্ষ CMD_lnfile: পক্ষ CMD_cfdo: পক্ষ CMD_lfdo:
	    dir = FORWARD_FILE;
	    ভঙ্গ;
	পক্ষ CMD_cpfile: পক্ষ CMD_lpfile: পক্ষ CMD_cNfile: পক্ষ CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ভঙ্গ;
	স্বভাব:
	    dir = FORWARD;
	    ভঙ্গ;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    স্থির qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, পূর্ণাঙ্ক bnr, পূর্ণাঙ্ক *errornr)
{
    qfline_T	*qfp = রিক্ত;
    পূর্ণাঙ্ক		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, idx)
	যদি (qfp->qf_fnum == bnr)
	    ভঙ্গ;

    *errornr = idx;
    প্রত্যাগম qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    স্থির qfline_T *
qf_find_first_entry_on_সারি(qfline_T *entry, পূর্ণাঙ্ক *errornr)
{
    যাবত্ (!got_int
	    && entry->qf_prev != রিক্ত
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    প্রত্যাগম entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    স্থির qfline_T *
qf_find_last_entry_on_সারি(qfline_T *entry, পূর্ণাঙ্ক *errornr)
{
    যাবত্ (!got_int &&
	    entry->qf_next != রিক্ত
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    প্রত্যাগম entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    স্থির পূর্ণাঙ্ক
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, পূর্ণাঙ্ক linewise)
{
    যদি (linewise)
	প্রত্যাগম qfp->qf_lnum > pos->lnum;
    অন্যথা
	প্রত্যাগম (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    স্থির পূর্ণাঙ্ক
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, পূর্ণাঙ্ক linewise)
{
    যদি (linewise)
	প্রত্যাগম qfp->qf_lnum < pos->lnum;
    অন্যথা
	প্রত্যাগম (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    স্থির পূর্ণাঙ্ক
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, পূর্ণাঙ্ক linewise)
{
    যদি (linewise)
	প্রত্যাগম qfp->qf_lnum >= pos->lnum;
    অন্যথা
	প্রত্যাগম (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    স্থির পূর্ণাঙ্ক
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, পূর্ণাঙ্ক linewise)
{
    যদি (linewise)
	প্রত্যাগম qfp->qf_lnum <= pos->lnum;
    অন্যথা
	প্রত্যাগম (qfp->qf_lnum < pos->lnum ||
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
    স্থির qfline_T *
qf_find_entry_after_pos(
	পূর্ণাঙ্ক		bnr,
	pos_T		*pos,
	পূর্ণাঙ্ক		linewise,
	qfline_T	*qfp,
	পূর্ণাঙ্ক		*errornr)
{
    যদি (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	প্রত্যাগম qfp;

    // Find the entry just before or at the position 'pos'
    যাবত্ (qfp->qf_next != রিক্ত
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    যদি (qfp->qf_next == রিক্ত || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	প্রত্যাগম রিক্ত;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    প্রত্যাগম qfp;
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
    স্থির qfline_T *
qf_find_entry_before_pos(
	পূর্ণাঙ্ক		bnr,
	pos_T		*pos,
	পূর্ণাঙ্ক		linewise,
	qfline_T	*qfp,
	পূর্ণাঙ্ক		*errornr)
{
    // Find the entry just before the position 'pos'
    যাবত্ (qfp->qf_next != রিক্ত
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    যদি (qf_entry_on_or_after_pos(qfp, pos, linewise))
	প্রত্যাগম রিক্ত;

    যদি (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_সারি(qfp, errornr);

    প্রত্যাগম qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    স্থির qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	পূর্ণাঙ্ক		bnr,
	pos_T		*pos,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		linewise,
	পূর্ণাঙ্ক		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    যদি (qfp == রিক্ত)
	প্রত্যাগম রিক্ত;		// no entry in this file

    যদি (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    অন্যথা
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    প্রত্যাগম qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    স্থির শূন্য
qf_get_nth_below_entry(qfline_T *entry_arg, পূর্ণাঙ্ক n, পূর্ণাঙ্ক linewise, পূর্ণাঙ্ক *errornr)
{
    qfline_T *entry = entry_arg;

    যাবত্ (n-- > 0 && !got_int)
    {
	পূর্ণাঙ্ক		first_errornr = *errornr;

	যদি (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_সারি(entry, errornr);

	যদি (entry->qf_next == রিক্ত
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    যদি (linewise)
		*errornr = first_errornr;
	    ভঙ্গ;
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
    স্থির শূন্য
qf_get_nth_above_entry(qfline_T *entry, পূর্ণাঙ্ক n, পূর্ণাঙ্ক linewise, পূর্ণাঙ্ক *errornr)
{
    যাবত্ (n-- > 0 && !got_int)
    {
	যদি (entry->qf_prev == রিক্ত
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    ভঙ্গ;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	যদি (linewise)
	    entry = qf_find_first_entry_on_সারি(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    স্থির পূর্ণাঙ্ক
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	পূর্ণাঙ্ক		bnr,
	pos_T		*pos,
	পূর্ণাঙ্ক		n,
	পূর্ণাঙ্ক		dir,
	পূর্ণাঙ্ক		linewise)
{
    qfline_T	*adj_entry;
    পূর্ণাঙ্ক		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    যদি (adj_entry == রিক্ত)
	প্রত্যাগম 0;

    যদি (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	যদি (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	অন্যথা
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    প্রত্যাগম errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    শূন্য
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    পূর্ণাঙ্ক		dir;
    পূর্ণাঙ্ক		buf_has_flag;
    পূর্ণাঙ্ক		errornr = 0;
    pos_T	pos;

    যদি (eap->addr_গুনতি > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	প্রত্যাগম;
    }

    // Check whether the current buffer has any quickfix entries
    যদি (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    অন্যথা
	buf_has_flag = BUF_HAS_LL_ENTRY;
    যদি (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	প্রত্যাগম;
    }

    যদি ((qi = qf_cmd_get_স্তূপ(eap, TRUE)) == রিক্ত)
	প্রত্যাগম;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    যদি (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	প্রত্যাগম;
    }

    যদি (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    অন্যথা
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_গুনতি > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    যদি (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    অন্যথা
	emsg(_(e_no_more_বস্তুসমূহ));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    স্থির char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    বিকল্প (cmdidx)
    {
	পক্ষ CMD_cfile:	    প্রত্যাগম (char_u *)"cfile";
	পক্ষ CMD_cgetfile:  প্রত্যাগম (char_u *)"cgetfile";
	পক্ষ CMD_caddfile:  প্রত্যাগম (char_u *)"caddfile";
	পক্ষ CMD_lfile:	    প্রত্যাগম (char_u *)"lfile";
	পক্ষ CMD_lgetfile:  প্রত্যাগম (char_u *)"lgetfile";
	পক্ষ CMD_laddfile:  প্রত্যাগম (char_u *)"laddfile";
	স্বভাব:	    প্রত্যাগম রিক্ত;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    শূন্য
ex_cfile(exarg_T *eap)
{
    char_u	*enc = রিক্ত;
    win_T	*wp = রিক্ত;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = রিক্ত;
    int_u	save_qfid = 0;		// init for gcc
    পূর্ণাঙ্ক		res;

    au_name = cfile_get_auname(eap->cmdidx);
    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							রিক্ত, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	যদি (aborting())
	    প্রত্যাগম;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   রিক্ত, রিক্ত,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), রিক্ত);
	যদি (browse_file == রিক্ত)
	    প্রত্যাগম;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    অন্যথা
# endif
    যদি (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    যদি (is_loclist_cmd(eap->cmdidx))
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
    যদি (wp != রিক্ত)
    {
	qi = GET_LOC_LIST(wp);
	যদি (qi == রিক্ত)
	{
	    decr_quickfix_busy();
	    প্রত্যাগম;
	}
    }
    অন্যথা যদি (qi == রিক্ত)
    {
	decr_quickfix_busy();
	প্রত্যাগম;
    }
    যদি (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    যদি (au_name != রিক্ত)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, রিক্ত, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    যদি (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    স্থির char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    বিকল্প (cmdidx)
    {
	পক্ষ CMD_vimgrep:     প্রত্যাগম (char_u *)"vimgrep";
	পক্ষ CMD_lvimgrep:    প্রত্যাগম (char_u *)"lvimgrep";
	পক্ষ CMD_vimgrepadd:  প্রত্যাগম (char_u *)"vimgrepadd";
	পক্ষ CMD_lvimgrepadd: প্রত্যাগম (char_u *)"lvimgrepadd";
	পক্ষ CMD_grep:	      প্রত্যাগম (char_u *)"grep";
	পক্ষ CMD_lgrep:	      প্রত্যাগম (char_u *)"lgrep";
	পক্ষ CMD_grepadd:     প্রত্যাগম (char_u *)"grepadd";
	পক্ষ CMD_lgrepadd:    প্রত্যাগম (char_u *)"lgrepadd";
	স্বভাব: প্রত্যাগম রিক্ত;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    স্থির শূন্য
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = রিক্ত;

    যদি (s == রিক্ত || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	যদি (last_অনুসন্ধান_pat() == রিক্ত)
	{
	    emsg(_(e_no_previous_regular_expression));
	    প্রত্যাগম;
	}
	regmatch->regprog = vim_regcomp(last_অনুসন্ধান_pat(), RE_MAGIC);
    }
    অন্যথা
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    স্থির শূন্য
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    বার্তা_start();
    p = বার্তা_strtrunc(fname, TRUE);
    যদি (p == রিক্ত)
	বার্তা_outtrans(fname);
    অন্যথা
    {
	বার্তা_outtrans(p);
	vim_free(p);
    }
    বার্তা_clr_eos();
    বার্তা_didout = FALSE;	    // overwrite this message
    বার্তা_nowait = TRUE;	    // don't wait for this message
    বার্তা_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    স্থির buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    পূর্ণাঙ্ক		save_mls;
# যদি defined(FEAT_SYN_HL)
    char_u	*save_ei = রিক্ত;
# endif
    buf_T	*buf;

# যদি defined(FEAT_SYN_HL)
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
# যদি defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    প্রত্যাগম buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    স্থির পূর্ণাঙ্ক
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    যদি (!qflist_valid(wp, qfid))
    {
	যদি (wp != রিক্ত)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    প্রত্যাগম FALSE;
	}
	অন্যথা
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    প্রত্যাগম TRUE;
	}
    }

    যদি (qf_restore_list(qi, qfid) == FAIL)
	প্রত্যাগম FALSE;

    প্রত্যাগম TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    স্থির পূর্ণাঙ্ক
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	পূর্ণাঙ্ক	    duplicate_name,
	পূর্ণাঙ্ক	    flags)
{
    পূর্ণাঙ্ক		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    পূর্ণাঙ্ক		pat_len = (পূর্ণাঙ্ক)STRLEN(spat);
    যদি (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    প্রতি (lnum = 1; lnum <= buf->b_ml.ml_সারি_গুনতি && *tomatch > 0; ++lnum)
    {
	col = 0;
	যদি (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    যাবত্ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, রিক্ত) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		যদি (qf_add_entry(qfl,
			    রিক্ত,	// dir
			    fname,
			    রিক্ত,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    রিক্ত,	// search pattern
			    0,		// nr
			    0,		// type
			    রিক্ত,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ভঙ্গ;
		}
		found_match = TRUE;
		যদি (--*tomatch == 0)
		    ভঙ্গ;
		যদি ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ভঙ্গ;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		যদি (col > ml_get_buf_len(buf, lnum))
		    ভঙ্গ;
	    }
	}
	অন্যথা
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    পূর্ণাঙ্ক	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    যাবত্ (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		যদি (qf_add_entry(qfl,
			    রিক্ত,	// dir
			    fname,
			    রিক্ত,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    রিক্ত,	// search pattern
			    0,		// nr
			    0,		// type
			    রিক্ত,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ভঙ্গ;
		}
		found_match = TRUE;
		যদি (--*tomatch == 0)
		    ভঙ্গ;
		যদি ((flags & VGR_GLOBAL) == 0)
		    ভঙ্গ;
		col = matches[pat_len - 1] + col + 1;
		যদি (col > linelen)
		    ভঙ্গ;
	    }
	}
	সারি_breakcheck();
	যদি (got_int)
	    ভঙ্গ;
    }

    প্রত্যাগম found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    স্থির শূন্য
vgr_jump_to_match(
	qf_info_T   *qi,
	পূর্ণাঙ্ক	    forceit,
	পূর্ণাঙ্ক	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    যদি (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    যদি (curbuf == first_match_buf && target_dir != রিক্ত)
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
প্রকারনির্দেশ সংরচনা
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    পূর্ণাঙ্ক		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    পূর্ণাঙ্ক		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    স্থির পূর্ণাঙ্ক
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = রিক্ত;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    যদি (eap->addr_গুনতি > 0)
	args->tomatch = eap->line2;
    অন্যথা
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    যদি (p == রিক্ত)
    {
	emsg(_(e_invalid_অনুসন্ধান_pattern_or_delimiter));
	প্রত্যাগম FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    যদি (args->regmatch.regprog == রিক্ত)
	প্রত্যাগম FAIL;

    p = skipwhite(p);
    যদি (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	প্রত্যাগম FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    যদি ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    স্থির পূর্ণাঙ্ক
existing_swapfile(buf_T *buf)
{
    যদি (buf->b_ml.ml_mfp != রিক্ত && buf->b_ml.ml_mfp->mf_fname != রিক্ত)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	প্রত্যাগম fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    প্রত্যাগম FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    স্থির পূর্ণাঙ্ক
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	পূর্ণাঙ্ক		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    পূর্ণাঙ্ক		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    পূর্ণাঙ্ক		fi;
    buf_T	*buf;
    পূর্ণাঙ্ক		duplicate_name = FALSE;
    পূর্ণাঙ্ক		using_dummy;
    char_u	*dirname_start = রিক্ত;
    char_u	*dirname_now = রিক্ত;
    পূর্ণাঙ্ক		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    যদি (dirname_start == রিক্ত || dirname_now == রিক্ত)
	গচ্ছ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    প্রতি (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	যদি (time(রিক্ত) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(রিক্ত);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != রিক্ত);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	অন্যথা
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	যদি (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    গচ্ছ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	যদি (buf == রিক্ত)
	{
	    যদি (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	অন্যথা
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    যদি (using_dummy)
	    {
		যদি (found_match && *first_match_buf == রিক্ত)
		    *first_match_buf = buf;
		যদি (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = রিক্ত;
		}
		অন্যথা যদি ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    যদি (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = রিক্ত;
		    }
		    অন্যথা যদি (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = রিক্ত;
		    }
		}

		যদি (buf != রিক্ত)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    যদি (buf == *first_match_buf
			    && *target_dir == রিক্ত
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    যদি (curbuf == buf)
		    {
# যদি defined(FEAT_SYN_HL)
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
    প্রত্যাগম status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    শূন্য
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = রিক্ত;
    পূর্ণাঙ্ক		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = রিক্ত;
    char_u	*target_dir = রিক্ত;
    char_u	*au_name =  রিক্ত;
    পূর্ণাঙ্ক		status;

    যদি (!check_can_set_curbuf_forceit(eap->forceit))
	প্রত্যাগম;

    au_name = vgr_get_auname(eap->cmdidx);
    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	যদি (aborting())
	    প্রত্যাগম;
# endif
    }

    qi = qf_cmd_get_or_alloc_স্তূপ(eap, &wp);
    যদি (qi == রিক্ত)
	প্রত্যাগম;

    যদি (vgr_process_args(eap, &args) == FAIL)
	গচ্ছ theend;

    যদি ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_স্তূপ_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    যদি (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	গচ্ছ theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, রিক্ত);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    যদি (au_name != রিক্ত)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    যদি (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	গচ্ছ theend;
    }

    // Jump to first match.
    যদি (!qf_list_empty(qf_get_curlist(qi)))
    {
	যদি ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    অন্যথা
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    যদি (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# অন্যথা
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
    স্থির শূন্য
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    যদি (dirname_now == রিক্ত)
	প্রত্যাগম;

    mch_dirname(dirname_now, MAXPATHL);
    যদি (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == রিক্ত) ? CMD_cd : CMD_lcd;
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
    স্থির buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    পূর্ণাঙ্ক		failed = TRUE;
    aco_save_T	aco;
    পূর্ণাঙ্ক		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(রিক্ত, রিক্ত, (linenr_T)1, BLN_DUMMY);
    যদি (newbuf == রিক্ত)
	প্রত্যাগম রিক্ত;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    যদি (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	যদি (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (শূন্য)setfname(curbuf, fname, রিক্ত, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = রিক্ত;
	    readfile_result = readfile(fname, রিক্ত,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			রিক্ত, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    যদি (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		যদি (curbuf != newbuf)
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

	    যদি (newbuf_to_wipe.br_buf != রিক্ত && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, রিক্ত);
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

    যদি (!bufref_valid(&newbufref))
	প্রত্যাগম রিক্ত;
    যদি (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    স্থির শূন্য
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    যাবত্ (buf->b_nwindows > 0)
    {
	পূর্ণাঙ্ক	    did_one = FALSE;
	win_T	    *wp;

	যদি (firstwin->w_next != রিক্ত)
	    FOR_ALL_WINDOWS(wp)
		যদি (wp->w_buffer == buf)
		{
		    যদি (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ভঙ্গ;
		}
	যদি (!did_one)
	    গচ্ছ fail;
    }

    যদি (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# যদি defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# যদি defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	যদি (dirname_start != রিক্ত)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	প্রত্যাগম;
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
    স্থির শূন্য
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    যদি (curbuf == buf)		// safety check
	প্রত্যাগম;

    close_buffer(রিক্ত, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# যদি defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    স্থির পূর্ণাঙ্ক
get_qfline_বস্তুসমূহ(qfline_T *qfp, list_T *list)
{
    পূর্ণাঙ্ক		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    যদি (bufnum != 0 && (buflist_findnr(bufnum) == রিক্ত))
	bufnum = 0;

    যদি ((dict = dict_alloc()) == রিক্ত)
	প্রত্যাগম FAIL;
    যদি (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	প্রত্যাগম FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    যদি (dict_add_সংখ্যা(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_সংখ্যা(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_সংখ্যা(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_সংখ্যা(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_সংখ্যা(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_সংখ্যা(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_সংখ্যা(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (পূর্ণাঙ্ক)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_সংখ্যা(dict, "valid", (long)qfp->qf_valid) == FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    স্থির পূর্ণাঙ্ক
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	পূর্ণাঙ্ক		qf_idx,
	পূর্ণাঙ্ক		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    পূর্ণাঙ্ক		i;

    যদি (qi == রিক্ত)
    {
	qi = ql_info;
	যদি (wp != রিক্ত)
	    qi = GET_LOC_LIST(wp);
	যদি (qi == রিক্ত)
	    প্রত্যাগম FAIL;

    }

    যদি (eidx < 0)
	প্রত্যাগম OK;

    যদি (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    যদি (qf_idx >= qi->qf_listcount)
	প্রত্যাগম FAIL;

    qfl = qf_get_list(qi, qf_idx);
    যদি (qf_list_empty(qfl))
	প্রত্যাগম FAIL;

    FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, i)
    {
	যদি (eidx > 0)
	{
	    যদি (eidx == i)
		প্রত্যাগম get_qfline_বস্তুসমূহ(qfp, list);
	}
	অন্যথা যদি (get_qfline_বস্তুসমূহ(qfp, list) == FAIL)
	    প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
গণনা {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_বস্তুসমূহ	= 0x2,
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
    স্থির পূর্ণাঙ্ক
qf_get_list_from_সারিগুলি(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    পূর্ণাঙ্ক		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    যদি (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == রিক্ত)
	প্রত্যাগম FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    যদি ((efm_di = dict_find(what, (char_u *)"efm", -1)) != রিক্ত)
    {
	যদি (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == রিক্ত)
	    প্রত্যাগম FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    যদি (l == রিক্ত)
	প্রত্যাগম FAIL;

    qi = qf_alloc_স্তূপ(QFLT_INTERNAL, 1);
    যদি (qi != রিক্ত)
    {
	যদি (qf_init_ext(qi, 0, রিক্ত, রিক্ত, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, রিক্ত, রিক্ত) > 0)
	{
	    (শূন্য)get_errorlist(qi, রিক্ত, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    প্রত্যাগম status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    স্থির পূর্ণাঙ্ক
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    যদি (qi == রিক্ত)
	প্রত্যাগম 0;
    win = qf_find_win(qi);
    যদি (win != রিক্ত)
	প্রত্যাগম win->w_id;
    প্রত্যাগম 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    পূর্ণাঙ্ক	bufnum = 0;

    যদি (qi != রিক্ত && buflist_findnr(qi->qf_bufnr) != রিক্ত)
	bufnum = qi->qf_bufnr;

    প্রত্যাগম dict_add_সংখ্যা(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_keys2flags(dict_T *what, পূর্ণাঙ্ক loclist)
{
    পূর্ণাঙ্ক		flags = QF_GETLIST_NONE;

    যদি (dict_has_চাবি(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	যদি (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    যদি (dict_has_চাবি(what, "title"))
	flags |= QF_GETLIST_TITLE;

    যদি (dict_has_চাবি(what, "nr"))
	flags |= QF_GETLIST_NR;

    যদি (dict_has_চাবি(what, "winid"))
	flags |= QF_GETLIST_WINID;

    যদি (dict_has_চাবি(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    যদি (dict_has_চাবি(what, "id"))
	flags |= QF_GETLIST_ID;

    যদি (dict_has_চাবি(what, "items"))
	flags |= QF_GETLIST_বস্তুসমূহ;

    যদি (dict_has_চাবি(what, "idx"))
	flags |= QF_GETLIST_IDX;

    যদি (dict_has_চাবি(what, "size"))
	flags |= QF_GETLIST_SIZE;

    যদি (dict_has_চাবি(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    যদি (loclist && dict_has_চাবি(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    যদি (dict_has_চাবি(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    যদি (dict_has_চাবি(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    প্রত্যাগম flags;
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
    স্থির পূর্ণাঙ্ক
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    পূর্ণাঙ্ক		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    যদি ((di = dict_find(what, (char_u *)"nr", -1)) != রিক্ত)
    {
	// Use the specified quickfix/location list
	যদি (di->di_tv.v_type == VAR_সংখ্যা)
	{
	    // for zero use the current list
	    যদি (di->di_tv.vval.v_সংখ্যা != 0)
	    {
		qf_idx = di->di_tv.vval.v_সংখ্যা - 1;
		যদি (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	অন্যথা যদি (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != রিক্ত
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	অন্যথা
	    qf_idx = INVALID_QFIDX;
    }

    যদি ((di = dict_find(what, (char_u *)"id", -1)) != রিক্ত)
    {
	// Look for a list with the specified id
	যদি (di->di_tv.v_type == VAR_সংখ্যা)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    যদি (di->di_tv.vval.v_সংখ্যা != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_সংখ্যা);
	}
	অন্যথা
	    qf_idx = INVALID_QFIDX;
    }

    প্রত্যাগম qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_defaults(qf_info_T *qi, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক locstack, dict_T *retdict)
{
    পূর্ণাঙ্ক		status = OK;

    যদি (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_বস্তুসমূহ))
    {
	list_T	*l = list_alloc();
	যদি (l != রিক্ত)
	    status = dict_add_list(retdict, "items", l);
	অন্যথা
	    status = FAIL;
    }
    যদি ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_সংখ্যা(retdict, "nr", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_সংখ্যা(retdict, "winid", qf_winid(qi));
    যদি ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_সংখ্যা(retdict, "id", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_সংখ্যা(retdict, "idx", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_সংখ্যা(retdict, "size", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_সংখ্যা(retdict, "changedtick", 0);
    যদি ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_সংখ্যা(retdict, "filewinid", 0);
    যদি ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    প্রত্যাগম status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    প্রত্যাগম dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    পূর্ণাঙ্ক winid = 0;

    যদি (wp != রিক্ত && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	যদি (ll_wp != রিক্ত)
	    winid = ll_wp->w_id;
    }

    প্রত্যাগম dict_add_সংখ্যা(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_বস্তুসমূহ(qf_info_T *qi, পূর্ণাঙ্ক qf_idx, পূর্ণাঙ্ক eidx, dict_T *retdict)
{
    পূর্ণাঙ্ক		status = OK;
    list_T	*l = list_alloc();
    যদি (l != রিক্ত)
    {
	(শূন্য)get_errorlist(qi, রিক্ত, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    অন্যথা
	status = FAIL;

    প্রত্যাগম status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    পূর্ণাঙ্ক		status;
    dictitem_T	*di;

    যদি (qfl->qf_ctx != রিক্ত)
    {
	di = dictitem_alloc((char_u *)"context");
	যদি (di != রিক্ত)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    যদি (status == FAIL)
		dictitem_free(di);
	}
	অন্যথা
	    status = FAIL;
    }
    অন্যথা
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    প্রত্যাগম status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_idx(qf_list_T *qfl, পূর্ণাঙ্ক eidx, dict_T *retdict)
{
    যদি (eidx == 0)
    {
	eidx = qfl->qf_index;
	যদি (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    প্রত্যাগম dict_add_সংখ্যা(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    স্থির পূর্ণাঙ্ক
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    পূর্ণাঙ্ক		status;

    যদি (qfl->qf_qftf_cb.cb_name != রিক্ত)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    অন্যথা
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    প্রত্যাগম status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    স্থির পূর্ণাঙ্ক
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    পূর্ণাঙ্ক		status = OK;
    পূর্ণাঙ্ক		qf_idx = INVALID_QFIDX;
    পূর্ণাঙ্ক		eidx = 0;
    dictitem_T	*di;
    পূর্ণাঙ্ক		flags = QF_GETLIST_NONE;

    যদি ((di = dict_find(what, (char_u *)"lines", -1)) != রিক্ত)
	প্রত্যাগম qf_get_list_from_সারিগুলি(what, di, retdict);

    যদি (wp != রিক্ত)
	qi = GET_LOC_LIST(wp);
    অন্যথা যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    flags = qf_getprop_keys2flags(what, (wp != রিক্ত));

    যদি (!qf_স্তূপ_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    যদি (qf_স্তূপ_empty(qi) || qf_idx == INVALID_QFIDX)
	প্রত্যাগম qf_getprop_defaults(qi, flags, wp != রিক্ত, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    যদি ((di = dict_find(what, (char_u *)"idx", -1)) != রিক্ত)
    {
	যদি (di->di_tv.v_type != VAR_সংখ্যা)
	    প্রত্যাগম FAIL;
	eidx = di->di_tv.vval.v_সংখ্যা;
    }

    যদি (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_সংখ্যা(retdict, "nr", qf_idx + 1);
    যদি ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_সংখ্যা(retdict, "winid", qf_winid(qi));
    যদি ((status == OK) && (flags & QF_GETLIST_বস্তুসমূহ))
	status = qf_getprop_বস্তুসমূহ(qi, qf_idx, eidx, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_সংখ্যা(retdict, "id", qfl->qf_id);
    যদি ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_সংখ্যা(retdict, "size", qfl->qf_গুনতি);
    যদি ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_সংখ্যা(retdict, "changedtick", qfl->qf_changedtick);
    যদি ((status == OK) && (wp != রিক্ত) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    যদি ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    প্রত্যাগম status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    স্থির পূর্ণাঙ্ক
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	পূর্ণাঙ্ক		first_entry,
	পূর্ণাঙ্ক		*valid_entry)
{
    স্থির পূর্ণাঙ্ক	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    পূর্ণাঙ্ক		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    যদি (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "bufnr");
    lnum = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "lnum");
    end_lnum = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "end_lnum");
    col = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "col");
    end_col = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "end_col");
    vcol = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "vcol");
    nr = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    যদি (text == রিক্ত)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    যদি ((filename == রিক্ত && bufnum == 0) || (lnum == 0 && pattern == রিক্ত))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    যদি (bufnum != 0 && (buflist_findnr(bufnum) == রিক্ত))
    {
	যদি (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    যদি (dict_has_চাবি(d, "valid"))
	valid = (পূর্ণাঙ্ক)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			রিক্ত,		// dir
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
			type == রিক্ত ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    যদি (valid)
	*valid_entry = TRUE;

    প্রত্যাগম status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    স্থির পূর্ণাঙ্ক
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	পূর্ণাঙ্ক		target_fnum,
	পূর্ণাঙ্ক		target_lnum,
	পূর্ণাঙ্ক		target_col)
{
    // First, compare entries to target file.
    যদি (!target_fnum)
	// Without a target file, we can't know which is closer.
	প্রত্যাগম FALSE;

    পূর্ণাঙ্ক is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    পূর্ণাঙ্ক other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    যদি (!is_target_file && other_is_target_file)
	প্রত্যাগম FALSE;
    অন্যথা যদি (is_target_file && !other_is_target_file)
	প্রত্যাগম TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    যদি (!target_lnum)
	// Without a target line number, we can't know which is closer.
	প্রত্যাগম FALSE;

    পূর্ণাঙ্ক সারি_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    পূর্ণাঙ্ক other_সারি_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    যদি (সারি_distance > other_সারি_distance)
	প্রত্যাগম FALSE;
    অন্যথা যদি (সারি_distance < other_সারি_distance)
	প্রত্যাগম TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    যদি (!target_col)
	// Without a target column, we can't know which is closer.
	প্রত্যাগম FALSE;

    পূর্ণাঙ্ক column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    পূর্ণাঙ্ক other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    যদি (column_distance > other_column_distance)
	প্রত্যাগম FALSE;
    অন্যথা যদি (column_distance < other_column_distance)
	প্রত্যাগম TRUE;

    // It's a complete tie! The exact same file, line, and column.
    প্রত্যাগম FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    স্থির পূর্ণাঙ্ক
qf_add_entries(
	qf_info_T	*qi,
	পূর্ণাঙ্ক		qf_idx,
	list_T		*list,
	char_u		*title,
	পূর্ণাঙ্ক		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = রিক্ত;
    পূর্ণাঙ্ক		retval = OK;
    পূর্ণাঙ্ক		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    পূর্ণাঙ্ক prev_fnum = 0;
    পূর্ণাঙ্ক prev_lnum = 0;
    পূর্ণাঙ্ক prev_col = 0;
    যদি (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    পূর্ণাঙ্ক select_first_entry = FALSE;
    পূর্ণাঙ্ক select_nearest_entry = FALSE;

    যদি (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    অন্যথা যদি (action == 'a')
    {
	যদি (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	অন্যথা
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    অন্যথা যদি (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_বস্তুসমূহ(qfl);
	qf_store_title(qfl, title);
    }
    অন্যথা যদি (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_বস্তুসমূহ(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = রিক্ত;
    পূর্ণাঙ্ক entry_to_select_index = 0;

    FOR_ALL_LIST_বস্তুসমূহ(list, li)
    {
	যদি (li->li_tv.v_type != VAR_DICT)
	    অনুবর্ত; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	যদি (d == রিক্ত)
	    অনুবর্ত;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	যদি (retval == QF_FAIL)
	    ভঙ্গ;

	qfline_T *entry = qfl->qf_last;
	যদি (
	    (select_first_entry && entry_to_select == রিক্ত) ||
	    (select_nearest_entry &&
		(entry_to_select == রিক্ত ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_গুনতি;
	}
    }

    // Check if any valid error entries are added to the list.
    যদি (valid_entry)
	qfl->qf_nonevalid = FALSE;
    অন্যথা যদি (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    যদি (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    প্রত্যাগম retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	পূর্ণাঙ্ক		action,
	পূর্ণাঙ্ক		*newlist)
{
    dictitem_T	*di;
    পূর্ণাঙ্ক		qf_idx = qi->qf_curlist;    // default is the current list

    যদি ((di = dict_find(what, (char_u *)"nr", -1)) != রিক্ত)
    {
	// Use the specified quickfix/location list
	যদি (di->di_tv.v_type == VAR_সংখ্যা)
	{
	    // for zero use the current list
	    যদি (di->di_tv.vval.v_সংখ্যা != 0)
		qf_idx = di->di_tv.vval.v_সংখ্যা - 1;

	    যদি ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_স্তূপ_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    অন্যথা যদি (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		প্রত্যাগম INVALID_QFIDX;
	    অন্যথা যদি (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	অন্যথা যদি (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != রিক্ত
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    যদি (!qf_স্তূপ_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    অন্যথা যদি (*newlist)
		qf_idx = 0;
	    অন্যথা
		প্রত্যাগম INVALID_QFIDX;
	}
	অন্যথা
	    প্রত্যাগম INVALID_QFIDX;
    }

    যদি (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != রিক্ত)
    {
	// Use the quickfix/location list with the specified id
	যদি (di->di_tv.v_type != VAR_সংখ্যা)
	    প্রত্যাগম INVALID_QFIDX;

	প্রত্যাগম qf_id2nr(qi, di->di_tv.vval.v_সংখ্যা);
    }

    প্রত্যাগম qf_idx;
}

/*
 * Set the quickfix list title.
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_title(qf_info_T *qi, পূর্ণাঙ্ক qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    যদি (di->di_tv.v_type != VAR_STRING)
	প্রত্যাগম FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    যদি (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    প্রত্যাগম OK;
}

/*
 * Set quickfix list items/entries.
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_বস্তুসমূহ(qf_info_T *qi, পূর্ণাঙ্ক qf_idx, dictitem_T *di, পূর্ণাঙ্ক action)
{
    পূর্ণাঙ্ক		retval = FAIL;
    char_u	*title_save;

    যদি (di->di_tv.v_type != VAR_LIST)
	প্রত্যাগম FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    প্রত্যাগম retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_বস্তুসমূহ_from_সারিগুলি(
	qf_info_T	*qi,
	পূর্ণাঙ্ক		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	পূর্ণাঙ্ক		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    পূর্ণাঙ্ক		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    যদি ((efm_di = dict_find(what, (char_u *)"efm", -1)) != রিক্ত)
    {
	যদি (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == রিক্ত)
	    প্রত্যাগম FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    যদি (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (action == 'r' || action == 'u')
	qf_free_বস্তুসমূহ(&qi->qf_lists[qf_idx]);
    যদি (qf_init_ext(qi, qf_idx, রিক্ত, রিক্ত, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, রিক্ত, রিক্ত) >= 0)
	retval = OK;

    প্রত্যাগম retval;
}

/*
 * Set quickfix list context.
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    যদি (ctx != রিক্ত)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    প্রত্যাগম OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    পূর্ণাঙ্ক		denote = FALSE;
    পূর্ণাঙ্ক		newidx;
    পূর্ণাঙ্ক		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    যদি (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != রিক্ত
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_গুনতি;
    অন্যথা
    {
	// Otherwise use the specified index
	newidx = tv_get_সংখ্যা_chk(&di->di_tv, &denote);
	যদি (denote)
	    প্রত্যাগম FAIL;
    }

    যদি (newidx < 1)		// sanity check
	প্রত্যাগম FAIL;
    যদি (newidx > qfl->qf_গুনতি)
	newidx = qfl->qf_গুনতি;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    যদি (qf_ptr == রিক্ত)
	প্রত্যাগম FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    যদি (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    প্রত্যাগম OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    স্থির পূর্ণাঙ্ক
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    যদি (check_restricted() || check_secure())
	প্রত্যাগম FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    যদি (cb.cb_name == রিক্ত || *cb.cb_name == NUL)
	প্রত্যাগম OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    প্রত্যাগম OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    স্থির পূর্ণাঙ্ক
qf_set_properties(qf_info_T *qi, dict_T *what, পূর্ণাঙ্ক action, char_u *title)
{
    dictitem_T	*di;
    পূর্ণাঙ্ক		retval = FAIL;
    পূর্ণাঙ্ক		qf_idx;
    পূর্ণাঙ্ক		newlist = FALSE;
    qf_list_T	*qfl;

    যদি (action == ' ' || qf_স্তূপ_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    যদি (qf_idx == INVALID_QFIDX)	// List not found
	প্রত্যাগম FAIL;

    যদি (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    যদি ((di = dict_find(what, (char_u *)"title", -1)) != রিক্ত)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    যদি ((di = dict_find(what, (char_u *)"items", -1)) != রিক্ত)
	retval = qf_setprop_বস্তুসমূহ(qi, qf_idx, di, action);
    যদি ((di = dict_find(what, (char_u *)"lines", -1)) != রিক্ত)
	retval = qf_setprop_বস্তুসমূহ_from_সারিগুলি(qi, qf_idx, what, di, action);
    যদি ((di = dict_find(what, (char_u *)"context", -1)) != রিক্ত)
	retval = qf_setprop_context(qfl, di);
    যদি ((di = dict_find(what, (char_u *)"idx", -1)) != রিক্ত)
	retval = qf_setprop_curidx(qi, qfl, di);
    যদি ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != রিক্ত)
	retval = qf_setprop_qftf(qi, qfl, di);

    যদি (newlist || retval == OK)
	qf_list_changed(qfl);
    যদি (newlist)
	qf_update_buffer(qi, রিক্ত);

    প্রত্যাগম retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    স্থির শূন্য
qf_free_স্তূপ(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = রিক্ত;

    যদি (qfwin != রিক্ত)
    {
	// If the quickfix/location list window is open, then clear it
	যদি (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, রিক্ত);
    }

    যদি (wp != রিক্ত && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	যদি (llwin != রিক্ত)
	    wp = llwin;
    }

    qf_free_all(wp);
    যদি (wp == রিক্ত)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    অন্যথা যদি (qfwin != রিক্ত)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_স্তূপ(QFLT_LOCATION, wp->w_p_lhi);

	যদি (new_ll != রিক্ত)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    যদি (wp != qfwin)
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
    পূর্ণাঙ্ক
set_errorlist(
	win_T	*wp,
	list_T	*list,
	পূর্ণাঙ্ক	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    পূর্ণাঙ্ক		retval = OK;

    যদি (wp != রিক্ত)
	qi = ll_get_or_alloc_list(wp);
    অন্যথা
	qi = ql_info;
    যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_স্তূপ(wp, qi);
	প্রত্যাগম OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    যদি (list->lv_len != 0 && what != রিক্ত)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	প্রত্যাগম FAIL;
    }

    incr_quickfix_busy();

    যদি (what != রিক্ত)
	retval = qf_set_properties(qi, what, action, title);
    অন্যথা
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	যদি (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    প্রত্যাগম retval;
}

স্থির পূর্ণাঙ্ক mark_quickfix_user_data(qf_info_T *qi, পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক abort = FALSE;
    প্রতি (পূর্ণাঙ্ক i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	যদি (!qfl->qf_has_user_data)
	    অনুবর্ত;
	qfline_T *qfp;
	পূর্ণাঙ্ক j;
	FOR_ALL_QFL_বস্তুসমূহ(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    যদি (user_data != রিক্ত && user_data->v_type != VAR_সংখ্যা
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, রিক্ত, রিক্ত, রিক্ত);
	}
    }
    প্রত্যাগম abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    স্থির পূর্ণাঙ্ক
mark_quickfix_ctx(qf_info_T *qi, পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    প্রতি (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	যদি (ctx != রিক্ত && ctx->v_type != VAR_সংখ্যা
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, রিক্ত, রিক্ত, রিক্ত);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    প্রত্যাগম abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    পূর্ণাঙ্ক
set_ref_in_quickfix(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    যদি (ql_info == রিক্ত)
	প্রত্যাগম TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    যদি (abort)
	প্রত্যাগম abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    যদি (abort)
	প্রত্যাগম abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    যদি (abort)
	প্রত্যাগম abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	যদি (win->w_llist != রিক্ত)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    যদি (abort)
		প্রত্যাগম abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    যদি (abort)
		প্রত্যাগম abort;
	}
	যদি (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    যদি (abort)
		প্রত্যাগম abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    যদি (abort)
		প্রত্যাগম abort;
	}
    }

    প্রত্যাগম abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    স্থির char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    বিকল্প (cmdidx)
    {
	পক্ষ CMD_cbuffer:	প্রত্যাগম (char_u *)"cbuffer";
	পক্ষ CMD_cgetbuffer:	প্রত্যাগম (char_u *)"cgetbuffer";
	পক্ষ CMD_caddbuffer:	প্রত্যাগম (char_u *)"caddbuffer";
	পক্ষ CMD_lbuffer:	প্রত্যাগম (char_u *)"lbuffer";
	পক্ষ CMD_lgetbuffer:	প্রত্যাগম (char_u *)"lgetbuffer";
	পক্ষ CMD_laddbuffer:	প্রত্যাগম (char_u *)"laddbuffer";
	স্বভাব:		প্রত্যাগম রিক্ত;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    স্থির পূর্ণাঙ্ক
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = রিক্ত;

    যদি (*eap->arg == NUL)
	buf = curbuf;
    অন্যথা যদি (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((বর্ণ *)eap->arg));

    যদি (buf == রিক্ত)
    {
	emsg(_(e_invalid_argument));
	প্রত্যাগম FAIL;
    }

    যদি (buf->b_ml.ml_mfp == রিক্ত)
    {
	emsg(_(e_buffer_is_not_loaded));
	প্রত্যাগম FAIL;
    }

    যদি (eap->addr_গুনতি == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_সারি_গুনতি;
    }

    যদি (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_সারি_গুনতি
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_সারি_গুনতি)
    {
	emsg(_(e_invalid_range));
	প্রত্যাগম FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    প্রত্যাগম OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    শূন্য
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = রিক্ত;
    qf_info_T	*qi;
    char_u	*au_name = রিক্ত;
    পূর্ণাঙ্ক		res;
    int_u	save_qfid;
    win_T	*wp = রিক্ত;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	যদি (aborting())
	    প্রত্যাগম;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_স্তূপ(eap, &wp);
    যদি (qi == রিক্ত)
	প্রত্যাগম;

    যদি (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	প্রত্যাগম;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    যদি (buf->b_sfname)
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%s (%s)",
		(বর্ণ *)qf_title, (বর্ণ *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, রিক্ত, buf, রিক্ত, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, রিক্ত);
    যদি (qf_স্তূপ_empty(qi))
    {
	decr_quickfix_busy();
	প্রত্যাগম;
    }
    যদি (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    যদি (au_name != রিক্ত)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	যদি (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    যদি (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# যদি defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    বিকল্প (cmdidx)
    {
	পক্ষ CMD_cexpr:	    প্রত্যাগম (char_u *)"cexpr";
	পক্ষ CMD_cgetexpr:  প্রত্যাগম (char_u *)"cgetexpr";
	পক্ষ CMD_caddexpr:  প্রত্যাগম (char_u *)"caddexpr";
	পক্ষ CMD_lexpr:	    প্রত্যাগম (char_u *)"lexpr";
	পক্ষ CMD_lgetexpr:  প্রত্যাগম (char_u *)"lgetexpr";
	পক্ষ CMD_laddexpr:  প্রত্যাগম (char_u *)"laddexpr";
	স্বভাব:	    প্রত্যাগম রিক্ত;
    }
}

    পূর্ণাঙ্ক
trigger_cexpr_autocmd(পূর্ণাঙ্ক cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	যদি (aborting())
	    প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

    পূর্ণাঙ্ক
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = রিক্ত;

    qi = qf_cmd_get_or_alloc_স্তূপ(eap, &wp);
    যদি (qi == রিক্ত)
	প্রত্যাগম FAIL;

    যদি ((tv->v_type == VAR_STRING && tv->vval.v_string != রিক্ত)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != রিক্ত))
    {
	পূর্ণাঙ্ক	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, রিক্ত, রিক্ত, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), রিক্ত);
	যদি (qf_স্তূপ_empty(qi))
	{
	    decr_quickfix_busy();
	    প্রত্যাগম FAIL;
	}
	যদি (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	যদি (au_name != রিক্ত)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	যদি (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	প্রত্যাগম OK;
    }

    emsg(_(e_string_or_list_expected));
    প্রত্যাগম FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    শূন্য
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    যদি (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	প্রত্যাগম;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_রাশিমালা(eap->arg, eap);
    যদি (tv == রিক্ত)
	প্রত্যাগম;

    (শূন্য)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    স্থির qf_info_T *
hgr_get_ll(পূর্ণাঙ্ক *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    যদি (bt_help(curwin->w_buffer))
	wp = curwin;
    অন্যথা
	// Find an existing help window
	wp = qf_find_help_win();

    যদি (wp == রিক্ত)	    // Help window not found
	qi = রিক্ত;
    অন্যথা
	qi = wp->w_llist;

    যদি (qi == রিক্ত)
    {
	// Allocate a new location list for help text matches
	যদি ((qi = qf_alloc_স্তূপ(QFLT_LOCATION, 1)) == রিক্ত)
	    প্রত্যাগম রিক্ত;
	*new_ll = TRUE;
    }

    প্রত্যাগম qi;
}

/*
 * Search for a pattern in a help file.
 */
    স্থির শূন্য
hgr_অনুসন্ধান_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((বর্ণ *)fname, "r");
    যদি (fd == রিক্ত)
	প্রত্যাগম;

    lnum = 1;
    যাবত্ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *সারি = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	যদি (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    সারি = string_convert(p_vc, IObuff, রিক্ত);
	    যদি (সারি == রিক্ত)
		সারি = IObuff;
	}

	যদি (vim_regexec(p_regmatch, সারি, (colnr_T)0))
	{
	    পূর্ণাঙ্ক	l = (পূর্ণাঙ্ক)STRLEN(সারি);

	    // remove trailing CR, LF, spaces, etc.
	    যাবত্ (l > 0 && সারি[l - 1] <= ' ')
		সারি[--l] = NUL;

	    যদি (qf_add_entry(qfl,
			রিক্ত,	// dir
			fname,
			রিক্ত,
			0,
			সারি,
			lnum,
			0,
			(পূর্ণাঙ্ক)(p_regmatch->startp[0] - সারি)
			+ 1,	// col
			(পূর্ণাঙ্ক)(p_regmatch->endp[0] - সারি)
			+ 1,	// end_col
			FALSE,	// vis_col
			রিক্ত,	// search pattern
			0,	// nr
			1,	// type
			রিক্ত,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		যদি (সারি != IObuff)
		    vim_free(সারি);
		ভঙ্গ;
	    }
	}
	যদি (সারি != IObuff)
	    vim_free(সারি);
	++lnum;
	সারি_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    স্থির শূন্য
hgr_অনুসন্ধান_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    পূর্ণাঙ্ক		fcount;
    char_u	**fnames;
    পূর্ণাঙ্ক		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    যদি (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	প্রতি (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    যদি (lang != রিক্ত
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		অনুবর্ত;
# endif

	    hgr_অনুসন্ধান_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    স্থির শূন্য
hgr_অনুসন্ধান_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    যদি (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    যাবত্ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_অনুসন্ধান_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    যদি (vc.vc_type != CONV_NONE)
	convert_setup(&vc, রিক্ত, রিক্ত);
}

/*
 * ":helpgrep {pattern}"
 */
    শূন্য
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    পূর্ণাঙ্ক		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    পূর্ণাঙ্ক		new_qi = FALSE;
    char_u	*au_name =  রিক্ত;
    char_u	*lang = রিক্ত;
    পূর্ণাঙ্ক		updated = FALSE;

    বিকল্প (eap->cmdidx)
    {
	পক্ষ CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ভঙ্গ;
	পক্ষ CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ভঙ্গ;
	স্বভাব: ভঙ্গ;
    }
    যদি (au_name != রিক্ত && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	যদি (aborting())
	    প্রত্যাগম;
# endif
    }

    যদি (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	যদি (qi == রিক্ত)
	    প্রত্যাগম;
    }
    অন্যথা যদি (qi == রিক্ত)
    {
	emsg(_(e_no_quickfix_স্তূপ));
	প্রত্যাগম;
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
    যদি (regmatch.regprog != রিক্ত)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_অনুসন্ধান_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    যদি (p_cpo == empty_option)
	p_cpo = save_cpo;
    অন্যথা
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	যদি (*p_cpo == NUL)
	    set_option_মান_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	যদি (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    যদি (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, রিক্ত);

    যদি (au_name != রিক্ত)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	যদি (!new_qi && IS_LL_স্তূপ(qi) && qf_find_win_with_loclist(qi) == রিক্ত)
	{
	    decr_quickfix_busy();
	    প্রত্যাগম;
	}
    }

    // Jump to first match.
    যদি (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    অন্যথা
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    যদি (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	যদি (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    যদি (new_qi)
		ll_free_all(&qi);
	}
	অন্যথা যদি (curwin->w_llist == রিক্ত && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# যদি defined(EXITFREE)
    শূন্য
free_quickfix(শূন্য)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(রিক্ত);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#যদি defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    স্থির শূন্য
get_qf_loc_list(পূর্ণাঙ্ক is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    যদি (what_arg->v_type == VAR_UNKNOWN)
    {
	যদি (rettv_list_alloc(rettv) == OK)
	    যদি (is_qf || wp != রিক্ত)
		(শূন্য)get_errorlist(রিক্ত, wp, -1, 0, rettv->vval.v_list);
    }
    অন্যথা
    {
	যদি (rettv_dict_alloc(rettv) == OK)
	    যদি (is_qf || (wp != রিক্ত))
	    {
		যদি (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    যদি (d != রিক্ত)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		অন্যথা
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    শূন্য
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    শূন্য
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    যদি (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    get_qf_loc_list(TRUE, রিক্ত, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    স্থির শূন্য
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    পূর্ণাঙ্ক		action = 0;
    স্থির পূর্ণাঙ্ক	recursive = 0;
# endif

    rettv->vval.v_সংখ্যা = -1;

# ifdef FEAT_QUICKFIX
    যদি (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    অন্যথা যদি (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    অন্যথা
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = রিক্ত;
	পূর্ণাঙ্ক	valid_dict = TRUE;

	যদি (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    যদি (act == রিক্ত)
		প্রত্যাগম;		// type error; errmsg already given
	    যদি ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    অন্যথা
		semsg(_(e_invalid_action_str_1), act);
	}
	অন্যথা যদি (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	অন্যথা
	    emsg(_(e_string_required));

	যদি (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    যদি (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != রিক্ত)
		what = what_arg->vval.v_dict;
	    অন্যথা
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	যদি (l != রিক্ত && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == রিক্ত ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_সংখ্যা = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    শূন্য
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	প্রত্যাগম;

    win = find_win_by_nr_or_id(&argvars[0]);
    যদি (win != রিক্ত)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    শূন্য
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	প্রত্যাগম;

    set_qf_ll_list(রিক্ত, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
