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

#ضمن "vim.h"

#אם defined(FEAT_QUICKFIX)

بنية dir_stack_T
{
    بنية dir_stack_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
تعريفنوع بنية qfline_S qfline_T;
بنية qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    صحيح		qf_fnum;	// file number for the line
    صحيح		qf_col;		// column where the error occurred
    صحيح		qf_end_col;	// column when the error has range or zero
    صحيح		qf_nr;		// error number
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
تعريفنوع تعداد
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
تعريفنوع بنية qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    صحيح		qf_count;	// number of errors (0 means empty list)
    صحيح		qf_index;	// current index in the error list
    صحيح		qf_nonevalid;	// TRUE if not a single valid entry found
    صحيح		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    بنية dir_stack_T	*qf_dir_stack;
    char_u		*qf_directory;
    بنية dir_stack_T	*qf_file_stack;
    char_u		*qf_currfile;
    صحيح			qf_multiline;
    صحيح			qf_multiignore;
    صحيح			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
بنية qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    صحيح		qf_refcount;
    صحيح		qf_listcount;	    // current number of lists
    صحيح		qf_curlist;	    // current error list
    صحيح         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    صحيح		qf_bufnr;	    // quickfix window buffer number
};

ساكن qf_info_T ql_info_actual; // global quickfix list
ساكن qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ساكن int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
تعريفنوع بنية efm_S efm_T;
بنية efm_S
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
    صحيح		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
تعريفنوع بنية qf_delq_S
{
    بنية qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
ساكن qf_delq_T *qf_delq_head = عدم;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ساكن صحيح	quickfix_busy = 0;

ساكن efm_T	*fmt_start = عدم; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ساكن callback_T qftf_cb;

ساكن فراغ     qf_pop_stack(qf_info_T *qi, صحيح adjust);
ساكن فراغ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ساكن صحيح	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, صحيح bufnum, char_u *mesg, long lnum, long end_lnum, صحيح col, صحيح end_col, صحيح vis_col, char_u *pattern, صحيح nr, صحيح type, typval_T *user_data, صحيح valid);
ساكن صحيح      qf_resize_stack_base(qf_info_T *qi, صحيح n);
ساكن فراغ     qf_sync_llw_to_win(win_T *llw);
ساكن فراغ     qf_sync_win_to_llw(win_T *pwp);
ساكن qf_info_T *qf_alloc_stack(qfltype_T qfltype, صحيح n);
ساكن qf_list_T *qf_alloc_list_stack(صحيح n);
ساكن فراغ	qf_free(qf_list_T *qfl);
ساكن char_u	*qf_types(صحيح, صحيح);
ساكن صحيح	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ساكن char_u	*qf_push_dir(char_u *, بنية dir_stack_T **, صحيح is_file_stack);
ساكن char_u	*qf_pop_dir(بنية dir_stack_T **);
ساكن char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ساكن win_T    *qf_find_win_with_loclist(qf_info_T *ll);
ساكن فراغ	qf_jump_newwin(qf_info_T *qi, صحيح dir, صحيح errornr, صحيح forceit, صحيح newwin);
ساكن فراغ	qf_fmt_text(garray_T *gap, char_u *text);
ساكن فراغ	qf_range_text(garray_T *gap, qfline_T *qfp);
ساكن صحيح	qf_win_pos_update(qf_info_T *qi, صحيح old_qf_index);
ساكن win_T	*qf_find_win(qf_info_T *qi);
ساكن buf_T	*qf_find_buf(qf_info_T *qi);
ساكن فراغ	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
ساكن فراغ	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, صحيح qf_winid);
ساكن buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
ساكن فراغ	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
ساكن فراغ	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
ساكن qf_info_T *ll_get_or_alloc_list(win_T *);
ساكن صحيح	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, صحيح target_fnum, صحيح target_lnum, صحيح target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == عدم)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != عدم)

// Quickfix and location list stack check helper macros
# define IS_QF_STACK(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_STACK(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ITEMS(qfl, qfp, i) \
		    עבור ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_count && (qfp) != عدم; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ساكن char_u   *qf_last_bufname = عدم;
ساكن bufref_T  qf_last_bufref = {عدم, 0, 0};

ساكن garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ساكن garray_T *
qfga_get(فراغ)
{
    ساكن صحيح initialized = FALSE;

    אם (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    החזר &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ساكن فراغ
qfga_clear(فراغ)
{
    אם (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    אחרת
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define LINE_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ساكن بنية fmtpattern
{
    char_u	convchar;
    حرف	*pattern;
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
    ساكن char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	صحيح	idx,
	صحيح	round)
{
    char_u	*srcptr;

    אם (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	החזר عدم;
    }
    אם ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != عدم)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == عدم))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	החזר عدم;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    אם (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    אם (*efmpat == 'f' && efmpat[1] != NUL)
    {
	אם (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	אחרת
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    אחרת
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	כלעוד ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    החזר regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ساكن char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	صحيح	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    אם (*efmp == '[' || *efmp == '\\')
    {
	אם ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    אם (efmp[1] == '^')
		*regpat++ = *++efmp;
	    אם (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		כלעוד (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		אם (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    החזר عدم;
		}
	    }
	}
	אחרת אם (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    אחרת
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	החזר عدم;
    }

    *pefmp = efmp;

    החזר regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ساكن char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    אם (vim_strchr((char_u *)"+-", *efmp) != عدم)
	efminfo->flags = *efmp++;
    אם (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != عدم)
	efminfo->prefix = *efmp;
    אחרת
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	החזר عدم;
    }

    החזר efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ساكن صحيح
efm_to_regpat(
	char_u	*efm,
	صحيح	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    صحيح		round;
    صحيح		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    עבור (efmp = efm; efmp < efm + len; ++efmp)
    {
	אם (*efmp == '%')
	{
	    ++efmp;
	    עבור (idx = 0; idx < FMT_PATTERNS; ++idx)
		אם (fmt_pat[idx].convchar == *efmp)
		    اكسر;
	    אם (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		אם (ptr == عدم)
		    החזר FAIL;
		round++;
	    }
	    אחרת אם (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		אם (ptr == عدم)
		    החזר FAIL;
	    }
	    אחרת אם (vim_strchr((char_u *)"%\\.^$~[", *efmp) != عدم)
		*ptr++ = *efmp;		// regexp magic characters
	    אחרת אם (*efmp == '#')
		*ptr++ = '*';
	    אחרת אם (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    אחרת אם (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		אם (efmp == عدم)
		    החזר FAIL;
	    }
	    אחרת
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		החזר FAIL;
	    }
	}
	אחרת			// copy normal character
	{
	    אם (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    אחרת אם (vim_strchr((char_u *)".*^$~[", *efmp) != عدم)
		*ptr++ = '\\';	// escape regexp atoms
	    אם (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    החזר OK;
}

/*
 * Free the 'errorformat' information list
 */
    ساكن فراغ
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    עבור (efm_ptr = *efm_first; efm_ptr != عدم; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = عدم;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ساكن صحيح
efm_regpat_bufsz(char_u *efm)
{
    صحيح sz;
    صحيح i;

    sz = (FMT_PATTERNS * 3) + ((صحيح)STRLEN(efm) << 2);
    עבור (i = FMT_PATTERNS; i > 0; )
	sz += (صحيح)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# אחרת
    sz += 2; // "%f" can become two chars longer
# endif

    החזר sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ساكن صحيح
efm_option_part_len(char_u *efm)
{
    صحيح len;

    עבור (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	אם (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    החזר len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ساكن efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = عدم;
    efm_T	*fmt_first = عدم;
    efm_T	*fmt_last = عدم;
    char_u	*fmtstr = عدم;
    صحيح		len;
    صحيح		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    אם ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == عدم)
	اذهب parse_efm_error;

    כלעוד (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	אם (fmt_ptr == عدم)
	    اذهب parse_efm_error;
	אם (fmt_first == عدم)	    // first one
	    fmt_first = fmt_ptr;
	אחרת
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	אם (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    اذهب parse_efm_error;
	אם ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == عدم)
	    اذهب parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    אם (fmt_first == عدم)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    اذهب parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    החזר fmt_first;
}

تعداد {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_INPUT = 2,
    QF_NOMEM = 3,
    QF_IGNORE_LINE = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
تعريفنوع بنية {
    char_u	*linebuf;
    صحيح		linelen;
    char_u	*growbuf;
    صحيح		growbufsiz;
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
    ساكن char_u *
qf_grow_linebuf(qfstate_T *state, صحيح newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > LINE_MAXLEN ? LINE_MAXLEN - 1 : newsz;
    אם (state->growbuf == عدم)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	אם (state->growbuf == عدم)
	    החזר عدم;
	state->growbufsiz = state->linelen;
    }
    אחרת אם (state->linelen > state->growbufsiz)
    {
	אם ((p = vim_realloc(state->growbuf, state->linelen + 1)) == عدم)
	    החזר عدم;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    החזר state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ساكن صحيح
qf_get_next_str_line(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    صحيح		len;

    אם (*p_str == NUL) // Reached the end of the string
	החזר QF_END_OF_INPUT;

    p = vim_strchr(p_str, '\n');
    אם (p != عدم)
	len = (صحيح)(p - p_str) + 1;
    אחרת
	len = (صحيح)STRLEN(p_str);

    אם (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	אם (state->linebuf == عدم)
	    החזר QF_NOMEM;
    }
    אחרת
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    החזר QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ساكن صحيح
qf_get_next_list_line(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    صحيح		len;

    כלעוד (p_li != عدم
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == عدم))
	p_li = p_li->li_next;	// Skip non-string items

    אם (p_li == عدم)		// End of the list
    {
	state->p_li = عدم;
	החזר QF_END_OF_INPUT;
    }

    len = (صحيح)STRLEN(p_li->li_tv.vval.v_string);
    אם (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	אם (state->linebuf == عدم)
	    החזר QF_NOMEM;
    }
    אחרת
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    החזר QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ساكن صحيح
qf_get_next_buf_line(qfstate_T *state)
{
    char_u	*p_buf = عدم;
    صحيح		len;

    // Get the next line from the supplied buffer
    אם (state->buflnum > state->lnumlast)
	החזר QF_END_OF_INPUT;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    אם (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	אם (state->linebuf == عدم)
	    החזר QF_NOMEM;
    }
    אחרת
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    החזר QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ساكن صحيح
qf_get_next_file_line(qfstate_T *state)
{
    صحيح	    discard;
    صحيح	    growbuflen;

    אם (fgets((حرف *)IObuff, IOSIZE, state->fd) == عدم)
	החזר QF_END_OF_INPUT;

    discard = FALSE;
    state->linelen = (صحيح)STRLEN(IObuff);
    אם (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	אם (state->growbuf == عدم)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    אם (state->growbuf == عدم)
		החזר QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	עבור (;;)
	{
	    char_u	*p;

	    אם (fgets((حرف *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == عدم)
		اكسر;
	    state->linelen = (صحيح)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    אם ((state->growbuf)[growbuflen - 1] == '\n')
		اكسر;
	    אם (state->growbufsiz == LINE_MAXLEN)
	    {
		discard = TRUE;
		اكسر;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < LINE_MAXLEN
		? 2 * state->growbufsiz : LINE_MAXLEN;
	    אם ((p = vim_realloc(state->growbuf, state->growbufsiz)) == عدم)
		החזר QF_NOMEM;
	    state->growbuf = p;
	}

	כלעוד (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    אם (fgets((حرف *)IObuff, IOSIZE, state->fd) == عدم
		    || (صحيح)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		اكسر;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    אחרת
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    אם (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*line;

	line = string_convert(&state->vc, state->linebuf, &state->linelen);
	אם (line != عدم)
	{
	    אם (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, line);
		vim_free(line);
	    }
	    אחרת
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = line;
		state->growbufsiz = state->linelen < LINE_MAXLEN
						? state->linelen : LINE_MAXLEN;
	    }
	}
    }

    החזר QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ساكن صحيح
qf_get_nextline(qfstate_T *state)
{
    صحيح status = QF_FAIL;

    אם (state->fd == عدم)
    {
	אם (state->tv != عدم)
	{
	    אם (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_line(state);
	    אחרת אם (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_line(state);
	}
	אחרת
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_line(state);
    }
    אחרת
	// Get the next line from the supplied file
	status = qf_get_next_file_line(state);

    אם (status != QF_OK)
	החזר status;

    // remove newline/CR from the line
    אם (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	אם (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    החזר QF_OK;
}

تعريفنوع بنية {
    char_u	*namebuf;
    صحيح		bnr;
    char_u	*module;
    char_u	*errmsg;
    صحيح		errmsglen;
    long	lnum;
    long	end_lnum;
    صحيح		col;
    صحيح		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    صحيح		enr;
    صحيح		type;
    typval_T	*user_data;
    صحيح		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ساكن صحيح
qf_parse_fmt_f(regmatch_T *rmp, صحيح midx, qffields_T *fields, صحيح prefix)
{
    صحيح c;

    אם (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	החזר QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    אם (vim_strchr((char_u *)"OPQ", prefix) != عدم
	    && mch_getperm(fields->namebuf) == -1)
	החזר QF_FAIL;

    החזר QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ساكن صحيح
qf_parse_fmt_b(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    صحيح bnr = (صحيح)atol((حرف *)rmp->startp[midx]);
    אם (buflist_findnr(bnr) == عدم)
	החזר QF_FAIL;
    fields->bnr = bnr;
    החזר QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ساكن صحيح
qf_parse_fmt_n(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->enr = (صحيح)atol((حرف *)rmp->startp[midx]);
    החזר QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ساكن صحيح
qf_parse_fmt_l(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->lnum = atol((حرف *)rmp->startp[midx]);
    החזר QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ساكن صحيح
qf_parse_fmt_e(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->end_lnum = atol((حرف *)rmp->startp[midx]);
    החזר QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_c(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->col = (صحيح)atol((حرف *)rmp->startp[midx]);
    החזר QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ساكن صحيح
qf_parse_fmt_k(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->end_col = (صحيح)atol((حرف *)rmp->startp[midx]);
    החזר QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ساكن صحيح
qf_parse_fmt_t(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->type = *rmp->startp[midx];
    החזר QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ساكن صحيح
copy_nonerror_line(char_u *linebuf, صحيح linelen, qffields_T *fields)
{
    char_u	*p;

    אם (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	אם ((p = vim_realloc(fields->errmsg, linelen + 1)) == عدم)
	    החזר QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    החזר QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ساكن صحيح
qf_parse_fmt_m(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    char_u	*p;
    صحيح		len;

    אם (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	החזר QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    אם (len >= fields->errmsglen)
    {
	// len + null terminator
	אם ((p = vim_realloc(fields->errmsg, len + 1)) == عدم)
	    החזר QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    החזר QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ساكن صحيح
qf_parse_fmt_r(regmatch_T *rmp, صحيح midx, char_u **tail)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    *tail = rmp->startp[midx];
    החזר QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_p(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    char_u	*match_ptr;

    אם (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	החזר QF_FAIL;
    fields->col = 0;
    עבור (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	אם (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    החזר QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ساكن صحيح
qf_parse_fmt_v(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    אם (rmp->startp[midx] == عدم)
	החזר QF_FAIL;
    fields->col = (صحيح)atol((حرف *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    החזר QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ساكن صحيح
qf_parse_fmt_s(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    صحيح		len;

    אם (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	החזר QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    אם (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    החזר QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ساكن صحيح
qf_parse_fmt_o(regmatch_T *rmp, صحيح midx, qffields_T *fields)
{
    صحيح		len;

    אם (rmp->startp[midx] == عدم || rmp->endp[midx] == عدم)
	החזר QF_FAIL;
    len = (صحيح)(rmp->endp[midx] - rmp->startp[midx]);
    אם (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    החזר QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ساكن صحيح (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, صحيح, qffields_T *) =
{
    عدم, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    عدم, // %r
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
    ساكن صحيح
qf_parse_match(
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	صحيح		qf_multiline,
	صحيح		qf_multiscan,
	char_u		**tail)
{
    صحيح		idx = fmt_ptr->prefix;
    صحيح		i;
    صحيح		midx;
    صحيح		status;

    אם ((idx == 'C' || idx == 'Z') && !qf_multiline)
	החזר QF_FAIL;
    אם (vim_strchr((char_u *)"EWIN", idx) != عدم)
	fields->type = idx;
    אחרת
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    עבור (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (صحيح)fmt_ptr->addr[i];
	אם (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	אחרת אם (i == FMT_PATTERN_M)
	{
	    אם (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_line(linebuf, linelen, fields);
	    אחרת אם (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	אחרת אם (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	אחרת אם (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	אם (status != QF_OK)
	    החזר status;
    }

    החזר QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ساكن صحيح
qf_parse_get_fields(
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	صحيح		qf_multiline,
	صحيح		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    صحيح		status = QF_FAIL;
    صحيح		r;

    אם (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == عدم)
	החזר QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    אם (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = عدم;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    אם (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    החזר status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ساكن صحيح
qf_parse_dir_pfx(صحيح idx, qffields_T *fields, qf_list_T *qfl)
{
    אם (idx == 'D')				// enter directory
    {
	אם (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    החזר QF_FAIL;
	}
	qfl->qf_directory =
	    qf_push_dir(fields->namebuf, &qfl->qf_dir_stack, FALSE);
	אם (qfl->qf_directory == عدم)
	    החזר QF_FAIL;
    }
    אחרת אם (idx == 'X')			// leave directory
	qfl->qf_directory = qf_pop_dir(&qfl->qf_dir_stack);

    החזר QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ساكن صحيح
qf_parse_file_pfx(
	صحيح idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    אם (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	אם (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_push_dir(fields->namebuf, &qfl->qf_file_stack, TRUE);
	אחרת אם (idx == 'Q')
	    qfl->qf_currfile = qf_pop_dir(&qfl->qf_file_stack);
	*fields->namebuf = NUL;
	אם (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    החזר QF_MULTISCAN;
	}
    }

    החזר QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ساكن صحيح
qf_parse_line_nomatch(char_u *linebuf, صحيح linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    החזר copy_nonerror_line(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ساكن صحيح
qf_parse_multiline_pfx(
	صحيح idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    صحيح			len;

    אם (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	אם (qfprev == عدم)
	    החזר QF_FAIL;
	אם (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (صحيح)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    אם (ptr == عدم)
		החזר QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	אם (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	אם (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	אם (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	אם (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	אם (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	אם (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	אם (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != عدم
		    ? fields->namebuf
		    : qfl->qf_currfile != عدم && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    אם (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    line_breakcheck();

    החזר QF_IGNORE_LINE;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ساكن صحيح
qf_parse_line(
	qf_list_T	*qfl,
	char_u		*linebuf,
	صحيح		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    صحيح			idx = 0;
    char_u		*tail = عدم;
    صحيح			status;

restofline:
    // If there was no %> item start at the first pattern
    אם (fmt_start == عدم)
	fmt_ptr = fmt_first;
    אחרת
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = عدم;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    עבור ( ; fmt_ptr != عدم; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	אם (status == QF_NOMEM)
	    החזר status;
	אם (status == QF_OK)
	    اكسر;
    }
    qfl->qf_multiscan = FALSE;

    אם (fmt_ptr == عدم || idx == 'D' || idx == 'X')
    {
	אם (fmt_ptr != عدم)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    אם (status != QF_OK)
		החזר status;
	}

	status = qf_parse_line_nomatch(linebuf, linelen, fields);
	אם (status != QF_OK)
	    החזר status;

	אם (fmt_ptr == عدم)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    אחרת אם (fmt_ptr != عدم)
    {
	// honor %> item
	אם (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	אם (vim_strchr((char_u *)"AEWIN", idx) != عدم)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	אחרת אם (vim_strchr((char_u *)"CZ", idx) != عدم)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    אם (status != QF_OK)
		החזר status;
	}
	אחרת אם (vim_strchr((char_u *)"OPQ", idx) != عدم)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    אם (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		صحيح new_linelen = (صحيح)STRLEN(s);
		אם (new_linelen >= linelen)
		    החזר QF_IGNORE_LINE;
		linebuf = s;
		linelen = new_linelen;
		اذهب restofline;
	    }
	}
	אם (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    אם (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    החזר QF_IGNORE_LINE;
	}
    }

    החזר QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ساكن صحيح
qf_stack_empty(qf_info_T *qi)
{
    החזר qi == عدم || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ساكن صحيح
qf_list_empty(qf_list_T *qfl)
{
    החזר qfl == عدم || qfl->qf_count <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ساكن صحيح
qf_list_has_valid_entries(qf_list_T *qfl)
{
    החזר !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ساكن qf_list_T *
qf_get_list(qf_info_T *qi, صحيح idx)
{
    החזר &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ساكن صحيح
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    אם (pfields->namebuf == عدم || pfields->errmsg == عدم
		|| pfields->pattern == عدم || pfields->module == عدم)
	החזר FAIL;

    החזר OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ساكن فراغ
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
    ساكن صحيح
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
    אם (enc != عدم && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    אם (efile != عدم && (pstate->fd = mch_fopen((حرف *)efile, "r")) == عدم)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	החזר FAIL;
    }

    אם (tv != عدم)
    {
	אם (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	אחרת אם (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    החזר OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ساكن فراغ
qf_cleanup_state(qfstate_T *pstate)
{
    אם (pstate->fd != عدم)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    אם (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, عدم, عدم);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ساكن صحيح
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    صحيح		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    אם (status != QF_OK)
	החזר status;

    status = qf_parse_line(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    אם (status != QF_OK)
	החזר status;

    החזר qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != عدم)
		? fields->namebuf
		: ((qfl->qf_currfile != عدم && fields->valid)
		    ? qfl->qf_currfile : (char_u *)عدم),
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
    ساكن صحيح
qf_init_ext(
    qf_info_T	    *qi,
    صحيح		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    صحيح		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = عدم;
    صحيح		    adding = FALSE;
    ساكن efm_T    *fmt_first = عدم;
    char_u	    *efm;
    ساكن char_u   *last_efm = عدم;
    صحيح		    retval = -1;	// default: return error flag
    صحيح		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    אם ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	اذهب qf_init_end;

    אם (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    אחרת
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	אם (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    אם (errorformat == p_efm && tv == عدم && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    אחרת
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    אם (last_efm == عدم || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	אם (fmt_first != عدم)
	    last_efm = vim_strsave(efm);
    }

    אם (fmt_first == عدم)	// nothing found
	اذهب error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    כלעוד (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	אם (status == QF_NOMEM)		// memory alloc failure
	    اذهب qf_init_end;
	אם (status == QF_END_OF_INPUT)	// end of input
	    اكسر;
	אם (status == QF_FAIL)
	    اذهب error2;

	line_breakcheck();
    }
    אם (state.fd == عدم || !ferror(state.fd))
    {
	אם (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	אחרת
	{
	    qfl->qf_nonevalid = FALSE;
	    אם (qfl->qf_ptr == عدم)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_count;
	اذهب qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    אם (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	אם (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    אם (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    החזר retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    صحيح
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	صحيح	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    אם (wp != عدم)
	qi = ll_get_or_alloc_list(wp);
    אם (qi == عدم)
	החזר FAIL;

    החזר qf_init_ext(qi, qi->qf_curlist, efile, curbuf, عدم, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ساكن فراغ
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    אם (title == عدم)
	החזר;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    אם (p != عدم)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ساكن char_u *
qf_cmdtitle(char_u *cmd)
{
    ساكن char_u qftitle_str[IOSIZE];

    vim_snprintf((حرف *)qftitle_str, IOSIZE, ":%s", (حرف *)cmd);
    החזר qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ساكن qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    החזר qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ساكن فراغ
qf_pop_stack(qf_info_T *qi, صحيح adjust)
{
    صحيح i;
    qf_free(&qi->qf_lists[0]);
    עבור (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, حجم(*qi->qf_lists));

    אם (adjust)
    {
	qi->qf_listcount--;
	אם (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	אחרת
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ساكن فراغ
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    כלעוד (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    אם (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_pop_stack(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    אחרת
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
    ساكن فراغ
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    אם (q == عدم)
	החזר;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    صحيح
qf_stack_get_bufnr(فراغ)
{
    אם (ql_info == عدم)
	החזר INVALID_QFBUFNR;
    החזר ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ساكن فراغ
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    אם (qi->qf_bufnr == INVALID_QFBUFNR)
	החזר;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    אם (qfbuf != عدم && qfbuf->b_nwindows == 0)
    {
	صحيح buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	אם (curwin->w_buffer == عدم)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(عدم, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	אם (buf_was_null)
	    curwin->w_buffer = عدم;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ساكن فراغ
qf_free_list_stack_items(qf_info_T *qi)
{
    עבור (صحيح i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ساكن فراغ
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_stack_items(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ساكن فراغ
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    אם (qi == عدم)
	החזר;
    *pqi = عدم;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    אם (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	החזר;
    }

    qi->qf_refcount--;
    אם (qi->qf_refcount < 1)
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
    فراغ
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    אם (wp != عدم)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    אחרת אם (qi != عدم)
	qf_free_list_stack_items(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ساكن فراغ
incr_quickfix_busy(فراغ)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ساكن فراغ
decr_quickfix_busy(فراغ)
{
    אם (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	כלעוד (qf_delq_head != عدم)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    אם (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# אם defined(EXITFREE)
    فراغ
check_quickfix_busy(فراغ)
{
    אם (quickfix_busy != 0)
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
    ساكن صحيح
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    صحيح		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    صحيح		col,		// column
    صحيح		end_col,	// column for end
    صحيح		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    صحيح		nr,		// error number
    صحيح		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    صحيح		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = عدم;
    char_u	*p = عدم;

    אם ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == عدم)
	החזר QF_FAIL;
    אם (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	אם (buf != عدم)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    אחרת
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    אם (fname != عدم)
	fullname = fix_fname(fname);
    qfp->qf_fname = عدم;
    אם (buf != عدم &&
	buf->b_ffname != عدم && fullname != عدم)
    {
	אם (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    אם (p != عدم)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    אם ((qfp->qf_text = vim_strsave(mesg)) == عدم)
    {
	vim_free(qfp);
	החזר QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    אם (user_data == عدم || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    אחרת
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    אם (pattern == عدم || *pattern == NUL)
	qfp->qf_pattern = عدم;
    אחרת אם ((qfp->qf_pattern = vim_strsave(pattern)) == عدم)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	החזר QF_FAIL;
    }
    אם (module == عدم || *module == NUL)
	qfp->qf_module = عدم;
    אחרת אם ((qfp->qf_module = vim_strsave(module)) == عدم)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	החזר QF_FAIL;
    }
    qfp->qf_nr = nr;
    אם (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    אם (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = عدم;
    }
    אחרת
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = عدم;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_count;
    אם (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_count;
	qfl->qf_ptr = qfp;
    }

    החזר QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    صحيح
qf_resize_stack(صحيح n)
{
    אם (ql_info == عدم)
	החזר FAIL;

    אם (qf_resize_stack_base(ql_info, n) == FAIL)
	החזר FAIL;

    החזר OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    صحيح
ll_resize_stack(win_T *wp, صحيح n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    אם (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    אחרת
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    אם (qi == عدم)
	החזר FAIL;

    אם (qf_resize_stack_base(qi, n) == FAIL)
	החזר FAIL;

    החזר OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ساكن صحيح
qf_resize_stack_base(qf_info_T *qi, صحيح n)
{
    qf_list_T *new;
    صحيح amount_to_rm = 0, i;

    אם (qi == عدم)
	החזר FAIL;

    size_t lsz = حجم(*qi->qf_lists);

    אם (n == qi->qf_maxcount)
	החזר OK;
    אחרת אם (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	עבור (i = 0; i < amount_to_rm; i++)
	    qf_pop_stack(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    אם (new == عدم)
	החזר FAIL;

    // fill with zeroes any newly allocated memory
    אם (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, عدم);

    החזר OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   فراغ
qf_init_stack(فراغ)
{
    ql_info = qf_alloc_stack(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ساكن فراغ
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    אם (wp != عدم)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ساكن فراغ
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    אם (llw != عدم)
	FOR_ALL_WINDOWS(wp)
	    אם (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		החזר;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ساكن qf_info_T *
qf_alloc_stack(qfltype_T qfltype, صحيح n)
{
    qf_info_T *qi;

    אם (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    אחרת
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	אם (qi == عدم)
	    החזר عدم;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_stack(n);
    אם (qi->qf_lists == عدم)
    {
	אם (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	החזר عدم;
    }
    qi->qf_maxcount = n;

    החזר qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ساكن qf_list_T *
qf_alloc_list_stack(صحيح n)
{
    החזר ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ساكن qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    אם (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	החזר wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    אם (wp->w_llist == عدم)
	// new location list
	wp->w_llist = qf_alloc_stack(QFLT_LOCATION, wp->w_p_lhi);

    החזר wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ساكن qf_info_T *
qf_cmd_get_stack(exarg_T *eap, صحيح print_emsg)
{
    qf_info_T	*qi = ql_info;

    אם (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	אם (qi == عدم)
	{
	    אם (print_emsg)
		emsg(_(e_no_location_list));
	    החזר عدم;
	}
    }
    אם (qi == عدم && print_emsg)
	emsg(_(e_no_quickfix_stack));

    החזר qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ساكن qf_info_T *
qf_cmd_get_or_alloc_stack(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    אם (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	אם (qi == عدم)
	    החזר عدم;
	*pwinp = curwin;
    }

    החזר qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ساكن صحيح
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    صحيح		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ITEMS(from_qfl, from_qfp, i)
    {
	אם (qf_add_entry(to_qfl,
		    عدم,
		    عدم,
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
	    החזר FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	אם (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    החזר OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ساكن صحيح
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_count = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = عدم;
    to_qfl->qf_last = عدم;
    to_qfl->qf_ptr = عدم;
    אם (from_qfl->qf_title != عدم)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    אחרת
	to_qfl->qf_title = عدم;
    אם (from_qfl->qf_ctx != عدم)
    {
	to_qfl->qf_ctx = alloc_tv();
	אם (to_qfl->qf_ctx != عدم)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    אחרת
	to_qfl->qf_ctx = عدم;
    אם (from_qfl->qf_qftf_cb.cb_name != عدم)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    אחרת
	to_qfl->qf_qftf_cb.cb_name = عدم;

    אם (from_qfl->qf_count)
	אם (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    החזר FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    אם (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    החזר OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    فراغ
copy_loclist_stack(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    صحيح		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    אם (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    אחרת
	qi = from->w_llist;

    אם (qi == عدم)		    // no location list to copy
	החזר;

    // allocate a new location list, set size of stack to 'from' window value
    אם ((to->w_llist = qf_alloc_stack(QFLT_LOCATION, from->w_p_lhi)) == عدم)
	החזר;
    אחרת
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    עבור (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	אם (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    החזר;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ساكن صحيح
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {عدم, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    אם (fname == عدم || *fname == NUL)		// no file name
	החזר 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    אם (directory != عدم)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    אם (directory != عدم && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != عدم)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	אם (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    אם (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    אחרת
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    אם (ptr.string == عدم)
		החזר 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    אחרת
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    אם (qf_last_bufname != عدم && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    אחרת
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, عدم, (linenr_T)0, BLN_NOOPT);
	אם (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	אחרת
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    אם (buf == عدم)
	החזר 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    החזר buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ساكن char_u *
qf_push_dir(char_u *dirbuf, بنية dir_stack_T **stackptr, صحيح is_file_stack)
{
    بنية dir_stack_T  *ds_new;
    بنية dir_stack_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(بنية dir_stack_T, aid_qf_dirstack);
    אם (ds_new == عدم)
	החזר عدم;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    אם (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == عدم
	    || is_file_stack)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    אחרת
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = عدم;
	כלעוד (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    אם (dirname.string == عدم)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		החזר عدم;
	    }
	    אם (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		اكسر;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	כלעוד ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	אם (ds_new == عدم)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    אם ((*stackptr)->dirname != عدم)
	החזר (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    החזר عدم;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ساكن char_u *
qf_pop_dir(بنية dir_stack_T **stackptr)
{
    بنية dir_stack_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    אם (*stackptr != عدم)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    החזר *stackptr ? (*stackptr)->dirname : عدم;
}

/*
 * clean up directory stack
 */
    ساكن فراغ
qf_clean_dir_stack(بنية dir_stack_T **stackptr)
{
    بنية dir_stack_T  *ds_ptr;

    כלעוד ((ds_ptr = *stackptr) != عدم)
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
    ساكن char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    بنية dir_stack_T     *ds_ptr;
    بنية dir_stack_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    אם (qfl->qf_dir_stack == عدم)
	החזר عدم;

    ds_ptr = qfl->qf_dir_stack->next;
    fullname.string = عدم;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    כלעוד (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	אם ((fullname.string != عدم) && (mch_getperm(fullname.string) >= 0))
	    اكسر;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    כלעוד (qfl->qf_dir_stack->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_stack->next;
	qfl->qf_dir_stack->next = qfl->qf_dir_stack->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    החזר ds_ptr == عدم ? عدم : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ساكن صحيح
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    صحيح		i;

    אם (wp != عدم)
    {
	אם (!win_valid(wp))
	    החזר FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    אם (qi == عدم)
	החזר FALSE;

    עבור (i = 0; i < qi->qf_listcount; ++i)
	אם (qi->qf_lists[i].qf_id == qf_id)
	    החזר TRUE;

    החזר FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ساكن صحيح
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    صحيح		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
	אם (qfp == qf_ptr)
	    اكسر;

    אם (i > qfl->qf_count) // Entry is not found
	החזר FALSE;

    החזר TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ساكن qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحيح		*qf_index,
	صحيح		dir)
{
    صحيح			idx;
    صحيح			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    עשה
    {
	אם (idx == qfl->qf_count || qf_ptr->qf_next == عدم)
	    החזר عدم;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } כלעוד ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    החזר qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ساكن qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	صحيح		*qf_index,
	صحيح		dir)
{
    صحيح			idx;
    صحيح			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    עשה
    {
	אם (idx == 1 || qf_ptr->qf_prev == عدم)
	    החזר عدم;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } כלעוד ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    החזר qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ساكن qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	صحيح		errornr,
	صحيح		dir,
	صحيح		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    صحيح			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    صحيح			prev_index;
    حرف		*err = e_no_more_items;

    כלעוד (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	אם (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	אחרת
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	אם (qf_ptr == عدم)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    אם (err != عدم)
	    {
		emsg(_(err));
		החזר عدم;
	    }
	    اكسر;
	}

	err = عدم;
    }

    *new_qfidx = qf_idx;
    החזר qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ساكن qfline_T *
get_nth_entry(qf_list_T *qfl, صحيح errornr, صحيح *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحيح		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    כלעוד (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != عدم)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    כלעוד (errornr > qf_idx && qf_idx < qfl->qf_count &&
						qf_ptr->qf_next != عدم)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    החזר qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ساكن qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	صحيح		errornr,
	صحيح		dir,
	صحيح		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    صحيح		qfidx = qfl->qf_index;

    אם (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    אחרת אם (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    החזר qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ساكن win_T *
qf_find_help_win(فراغ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	אם (bt_help(wp->w_buffer))
	    החזר wp;

    החזר عدم;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ساكن فراغ
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ساكن صحيح
jump_to_help_window(qf_info_T *qi, صحيح newwin, صحيح *opened_window)
{
    win_T	*wp;
    صحيح		flags;

    אם (cmdmod.cmod_tab != 0 || newwin)
	wp = عدم;
    אחרת
	wp = qf_find_help_win();
    אם (wp != عدم && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    אחרת
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	אם (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_TOP;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	אם (IS_LL_STACK(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	אם (win_split(0, flags) == FAIL)
	    החזר FAIL;

	*opened_window = TRUE;

	אם (curwin->w_height < p_hh)
	    win_setheight((صحيح)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	אם (IS_LL_STACK(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    אם (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    החזר OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ساكن win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	אם (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    החזר wp;

    החזר عدم;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ساكن win_T *
qf_find_win_with_normal_buf(فراغ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	אם (bt_normal(wp->w_buffer))
	    החזר wp;

    החזר عدم;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ساكن صحيح
qf_goto_tabwin_with_file(صحيح fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	אם (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    החזר TRUE;
	}

    החזר FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ساكن صحيح
qf_open_new_file_win(qf_info_T *ll_ref)
{
    صحيح		flags;

    flags = WSP_ABOVE;
    אם (ll_ref != عدم)
	flags |= WSP_NEWLOC;
    אם (win_split(0, flags) == FAIL)
	החזר FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    אם (ll_ref != عدم)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    החזר OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ساكن فراغ
qf_goto_win_with_ll_file(win_T *use_win, صحيح qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    אם (win == عدم)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    אם (win->w_buffer->b_fnum == qf_fnum)
		اكسر;
	אם (win == عدم)
	{
	    // Find a previous usable window
	    win = curwin;
	    עשה
	    {
		אם (bt_normal(win->w_buffer))
		    اكسر;
		אם (win->w_prev == عدم)
		    win = lastwin;	// wrap around the top
		אחרת
		    win = win->w_prev; // go to previous window
	    } כלעוד (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    אם (win->w_llist == عدم && ll_ref != عدم)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ساكن فراغ
qf_goto_win_with_qfl_file(صحيح qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = عدم;
    עבור (;;)
    {
	אם (win->w_buffer->b_fnum == qf_fnum)
	    اكسر;
	אם (win->w_prev == عدم)
	    win = lastwin;	// wrap around the top
	אחרת
	    win = win->w_prev;	// go to previous window

	אם (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    אם ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    אחרת אם (altwin != عدم)
		win = altwin;
	    אחרת אם (curwin->w_prev != عدم)
		win = curwin->w_prev;
	    אחרת
		win = curwin->w_next;
	    اكسر;
	}

	// Remember a usable window.
	אם (altwin == عدم && !win->w_p_pvw && !win->w_p_wfb &&
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
    ساكن صحيح
qf_jump_to_usable_window(صحيح qf_fnum, صحيح newwin, صحيح *opened_window)
{
    win_T	*usable_wp = عدم;
    صحيح		usable_win = FALSE;
    qf_info_T	*ll_ref = عدم;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    אם (!newwin)
	ll_ref = curwin->w_llist_ref;

    אם (ll_ref != عدم)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	אם (usable_wp != عدم)
	    usable_win = TRUE;
    }

    אם (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	אם (win != عدم)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    אם (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    אם ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	אם (qf_open_new_file_win(ll_ref) != OK)
	    החזר FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    אחרת
    {
	אם (curwin->w_llist_ref != عدم)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	אחרת					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    החזר OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ساكن صحيح
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحيح		forceit,
	صحيح		prev_winid,
	صحيح		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحيح		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    صحيح		retval = OK;
    صحيح		old_qf_curlist = qi->qf_curlist;
    صحيح		save_qfid = qfl->qf_id;

    אם (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	אם (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    החזר FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, عدم, عدم, عدم, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : عدم);
    }
    אחרת
    {
	صحيح	fnum = qf_ptr->qf_fnum;

	אם (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    אם (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		החזר FAIL;
	    }

	    אם (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    אם (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		אם (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		אם (curwin->w_p_wfb)
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

	אם (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    אם (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	אם (wp == عدم && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    החזר QF_ABORT;
	}
    }

    אם (qfl_type == QFLT_QUICKFIX && !qflist_valid(عدم, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	החזר QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    אם (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	אם (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	אחרת
	    emsg(_(e_current_location_list_was_changed));
	החזר QF_ABORT;
    }

    החזר retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ساكن صحيح
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*line = ml_get_curline();
    char_u	*p = line;
    colnr_T	col = 0;

    כלעוד (*p != NUL && col < vcol)
    {
	אם (*p == TAB)
	    col += 8 - (col % 8);
	אחרת
	    col += ptr2cells(p);
	אם (col > vcol)
	    اكسر;
	MB_PTR_ADV(p);
    }
    החזר (صحيح)(p - line);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ساكن فراغ
qf_jump_goto_line(
	linenr_T	qf_lnum,
	صحيح		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    אם (qf_pattern == عدم)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	אם (i > 0)
	{
	    אם (i > curbuf->b_ml.ml_line_count)
		i = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = i;
	}
	אם (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    אם (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    אחרת
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	אחרת
	    beginline(BL_WHITE | BL_FIX);
    }
    אחרת
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	אם (!do_search(عدم, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, SEARCH_KEEP, عدم))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ساكن فراغ
qf_jump_print_msg(
	qf_info_T	*qi,
	صحيح		qf_index,
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
    אם (!msg_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((حرف *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_count,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (حرف *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = msg_scroll;
    אם (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	msg_scroll = TRUE;
    אחרת אם (!msg_scrolled && shortmess(SHM_OVERALL))
	msg_scroll = FALSE;
    msg_attr_keep((حرف *)gap->ga_data, 0, TRUE);
    msg_scroll = i;

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
    ساكن صحيح
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	صحيح		newwin,
	صحيح		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    صحيح		old_changedtick = qfl->qf_changedtick;
    صحيح		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    אם (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	אם (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    החזר FAIL;
    אם (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	אם (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	אחרת
	    emsg(_(e_current_location_list_was_changed));
	החזר QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    אם (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	אם (qf_ptr->qf_fnum == 0)
	    החזר NOTDONE;

	אם (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    החזר FAIL;
    }
    אם (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	אם (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	אחרת
	    emsg(_(e_current_location_list_was_changed));
	החזר QF_ABORT;
    }

    החזר OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ساكن صحيح
qf_jump_to_buffer(
	qf_info_T	*qi,
	صحيح		qf_index,
	qfline_T	*qf_ptr,
	صحيح		forceit,
	صحيح		prev_winid,
	صحيح		*opened_window,
	صحيح		openfold,
	صحيح		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    صحيح		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    אם (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	אם (retval != OK)
	    החזר retval;
    }

    // When not switched to another buffer, still need to set pc mark
    אם (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_line(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    אם ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    אם (print_message)
	qf_jump_print_msg(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    החזר retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    فراغ
qf_jump(qf_info_T	*qi,
	صحيح		dir,
	صحيح		errornr,
	صحيح		forceit)
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
    ساكن فراغ
qf_jump_newwin(qf_info_T	*qi,
	صحيح		dir,
	صحيح		errornr,
	صحيح		forceit,
	صحيح		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    صحيح			qf_index;
    صحيح			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    صحيح			prev_winid;
    صحيح			opened_window = FALSE;
    صحيح			print_message = TRUE;
    صحيح			old_KeyTyped = KeyTyped; // getting file may reset it
    صحيح			retval = OK;

    אם (qi == عدم)
    {
	אם (ql_info == عدم)
	{
	    emsg(_(e_no_quickfix_stack));
	    החזר;
	}
	qi = ql_info;
    }

    אם (qf_stack_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	החזר;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    אם (qf_ptr == عدم)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	اذهب theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    אם (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    אם (retval == FAIL)
	اذهب failed;
    אם (retval == QF_ABORT)
    {
	qi = عدم;
	qf_ptr = عدم;
	اذهب theend;
    }
    אם (retval == NOTDONE)
	اذهب theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_KeyTyped, print_message);
    אם (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = عدم;
	qf_ptr = عدم;
    }

    אם (retval != OK)
    {
	אם (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	אם (qf_ptr != عدم && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    אם (qi != عدم)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    אם (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ساكن صحيح	qfFileAttr;
ساكن صحيح	qfSepAttr;
ساكن صحيح	qfLineAttr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ساكن فراغ
qf_list_entry(qfline_T *qfp, صحيح qf_idx, صحيح cursel)
{
    char_u	*fname;
    buf_T	*buf;
    صحيح		filter_entry;
    garray_T	*gap;

    fname = عدم;
    אם (qfp->qf_module != عدم && *qfp->qf_module != NUL)
	vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(حرف *)qfp->qf_module);
    אחרת
    {
	אם (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != عدم)
	{
	    אם (qfp->qf_fname == عدم)
		fname = buf->b_fname;
	    אחרת
		fname = qfp->qf_fname;
	    אם (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	אם (fname == عدم)
	    sprintf((حرف *)IObuff, "%2d", qf_idx);
	אחרת
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (حرف *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    אם (qfp->qf_module != عدم && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    אם (filter_entry && fname != عدم)
	filter_entry &= message_filtered(fname);
    אם (filter_entry && qfp->qf_pattern != عدم)
	filter_entry &= message_filtered(qfp->qf_pattern);
    אם (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    אם (filter_entry)
	החזר;

    msg_putchar('\n');
    msg_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    אם (qfp->qf_lnum != 0)
	msg_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    אם (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    msg_puts_attr((حرف *)gap->ga_data, qfLineAttr);
    msg_puts_attr(":", qfSepAttr);
    אם (qfp->qf_pattern != عدم)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	msg_puts((حرف *)gap->ga_data);
	msg_puts_attr(":", qfSepAttr);
    }
    msg_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != عدم || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    msg_prt_line((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    فراغ
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i;
    صحيح		idx1 = 1;
    صحيح		idx2 = -1;
    char_u	*arg = eap->arg;
    صحيح		plus = FALSE;
    صحيح		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    אם (qf_stack_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	החזר;
    }
    אם (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    אם (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	החזר;
    }
    qfl = qf_get_curlist(qi);
    אם (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    אחרת
    {
	i = qfl->qf_count;
	אם (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	אם (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    אם (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    אם (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qfLineAttr = syn_name2attr((char_u *)"qfLineNr");
    אם (qfLineAttr == 0)
	qfLineAttr = HL_ATTR(HLF_N);

    אם (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	אם ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ساكن فراغ
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    כלעוד (*p != NUL)
    {
	אם (*p == '\n')
	{
	    ga_append(gap, ' ');
	    כלעוד (*++p != NUL)
		אם (!VIM_ISWHITE(*p) && *p != '\n')
		    اكسر;
	}
	אחרת
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ساكن فراغ
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((حرف *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    אם (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((حرف *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    אם (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((حرف *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	אם (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((حرف *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ساكن فراغ
qf_msg(qf_info_T *qi, صحيح which, حرف *lead)
{
    حرف   *title = (حرف *)qi->qf_lists[which].qf_title;
    صحيح    count = qi->qf_lists[which].qf_count;
    char_u buf[IOSIZE];

    vim_snprintf((حرف *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    count);

    אם (title != عدم)
    {
	size_t	len = STRLEN(buf);

	אם (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    msg((حرف *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    فراغ
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		count;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    אם (eap->addr_count != 0)
	count = eap->line2;
    אחרת
	count = 1;
    כלעוד (count--)
    {
	אם (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    אם (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_stack));
		اكسر;
	    }
	    --qi->qf_curlist;
	}
	אחרת
	{
	    אם (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_top_of_quickfix_stack));
		اكسر;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_msg(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, عدم);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    فراغ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_stack(eap, FALSE);
    صحيح		i;

    אם (eap->addr_count > 0)
    {
	אם (qi == عدم)
	{
	    emsg(_(e_no_location_list));
	    החזר;
	}

	// Jump to the specified quickfix list
	אם (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_msg(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, عدم);
	}
	אחרת
	    emsg(_(e_invalid_range));

	החזר;
    }

    אם (qf_stack_empty(qi))
	msg(_("No entries"));
    אחרת
	עבור (i = 0; i < qi->qf_listcount; ++i)
	    qf_msg(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ساكن فراغ
qf_free_items(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    صحيح		stop = FALSE;

    כלעוד (qfl->qf_count && qfl->qf_start != عدم)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	אם (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    אם (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_count = 1;
	    אחרת
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_count;
    }

    qfl->qf_index = 0;
    qfl->qf_start = عدم;
    qfl->qf_last = عدم;
    qfl->qf_ptr = عدم;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_stack(&qfl->qf_dir_stack);
    qfl->qf_directory = عدم;
    qf_clean_dir_stack(&qfl->qf_file_stack);
    qfl->qf_currfile = عدم;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ساكن فراغ
qf_free(qf_list_T *qfl)
{
    qf_free_items(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = عدم;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   فراغ
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    صحيح		i;
    qfline_T	*qfp;
    صحيح		idx;
    qf_info_T	*qi = ql_info;
    صحيح		found_one = FALSE;
    صحيح		buf_has_flag = wp == عدم ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    אם (!(curbuf->b_has_qf_entry & buf_has_flag))
	החזר;
    אם (wp != عدم)
    {
	אם (wp->w_llist == عدم)
	    החזר;
	qi = wp->w_llist;
    }
    אחרת אם (qi == عدم)
	החזר;

    עבור (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	אם (!qf_list_empty(qfl))
	    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
		אם (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    אם (qfp->qf_cleared)
			استمر;

		    אם (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			אם (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			אחרת
			    qfp->qf_lnum += amount;
		    }
		    אחרת אם (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    אם (!found_one)
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
    ساكن char_u *
qf_types(صحيح c, صحيح nr)
{
    ساكن char_u	buf[20];
    ساكن char_u	cc[3];
    char_u		*p;

    אם (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    אחרת אם (c == 'I' || c == 'i')
	p = (char_u *)" info";
    אחרת אם (c == 'N' || c == 'n')
	p = (char_u *)" note";
    אחרת אם (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    אחרת אם (c == 0 || c == 1)
	p = (char_u *)"";
    אחרת
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    אם (nr <= 0)
	החזר p;

    sprintf((حرف *)buf, "%s %3d", (حرف *)p, nr);
    החזר buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    فراغ
qf_view_result(صحيح split)
{
    qf_info_T   *qi = ql_info;

    אם (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    אחרת אם (qi == عدم)
    {
	emsg(_(e_no_quickfix_stack));
	החזר;
    }

    אם (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	החזר;
    }

    אם (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	החזר;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    فراغ
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    אם (qf_stack_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	אם (win != عدم)
	    ex_cclose(eap);
    }
    אחרת אם (win == عدم)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    فراغ
ex_cclose(exarg_T *eap)
{
    win_T	*win = عدم;
    qf_info_T	*qi;

    אם ((qi = qf_cmd_get_stack(eap, FALSE)) == عدم)
	החזר;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    אם (win != عدم)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ساكن فراغ
qf_set_title_var(qf_list_T *qfl)
{
    אם (qfl->qf_title != عدم)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ساكن صحيح
qf_goto_cwindow(qf_info_T *qi, صحيح resize, صحيح sz, صحيح vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    אם (win == عدم)
	החזר FAIL;

    win_goto(win);
    אם (resize)
    {
	אם (vertsplit)
	{
	    אם (sz != win->w_width)
		win_setwidth(sz);
	}
	אחרת אם (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    החזר OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ساكن فراغ
qf_set_cwindow_options(فراغ)
{
    // switch off 'swapfile'
    set_option_value_give_err((char_u *)"swf", 0L, عدم, OPT_LOCAL);
    set_option_value_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_value_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_value_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ساكن صحيح
qf_open_new_cwindow(qf_info_T *qi, صحيح height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    صحيح		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    אם (IS_QF_STACK(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    אם (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    אם (IS_QF_STACK(qi))
	flags |= WSP_QUICKFIX;

    אם (win_split(height, flags) == FAIL)
	החזר FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    אם (IS_LL_STACK(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    אם (oldwin != curwin)
	oldwin = عدم;  // don't store info when in another window
    אם (qf_buf != عدم)
    {
	// Use the existing quickfix buffer
	אם (do_ecmd(qf_buf->b_fnum, عدم, عدم, عدم, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    החזר FAIL;
    }
    אחרת
    {
	// Create a new quickfix buffer
	אם (do_ecmd(0, عدم, عدم, عدم, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    החזר FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    אם (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    אם (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    אם (win_valid(win))
	prevwin = win;

    החזר OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    فراغ
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحيح		height;
    صحيح		status = FAIL;
    صحيح		lnum;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    incr_quickfix_busy();

    אם (eap->addr_count != 0)
	height = eap->line2;
    אחרת
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    אם (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_count != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    אם (status == FAIL)
	אם (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    החזר;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, عدم, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ساكن فراغ
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
    فراغ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    win = qf_find_win(qi);
    אם (win != عدم && win->w_cursor.lnum != win->w_buffer->b_ml.ml_line_count)
	qf_win_goto(win, win->w_buffer->b_ml.ml_line_count);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    אם (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    אחרת אם (qi == عدم)
	החזר 0;

    החזר qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ساكن صحيح
qf_win_pos_update(
    qf_info_T	*qi,
    صحيح		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    صحيح		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    אם (win != عدم
	    && qf_index <= win->w_buffer->b_ml.ml_line_count
	    && old_qf_index != qf_index)
    {
	אם (qf_index > old_qf_index)
	{
	    win->w_redraw_top = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	אחרת
	{
	    win->w_redraw_top = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    החזר win != عدم;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ساكن صحيح
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    אם (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	אם ((IS_QF_STACK(qi) && win->w_llist_ref == عدم)
		|| (IS_LL_STACK(qi) && win->w_llist_ref == qi))
	    החזר TRUE;

    החזר FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ساكن win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	אם (is_qf_win(win, qi))
	    החזר win;
    החזר عدم;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ساكن buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    אם (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	אם (qfbuf != عدم)
	    החזר qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	אם (is_qf_win(win, qi))
	    החזר win->w_buffer;

    החזר عدم;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    حرف *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    אם (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	החזר e_invalid_argument;

    החזר عدم;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ساكن فراغ
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	אם (is_qf_win(win, qi))
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
    ساكن فراغ
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    אם (buf == عدم)
	החזר;

    linenr_T	old_line_count = buf->b_ml.ml_line_count;
    صحيح		qf_winid = 0;

    אם (IS_LL_STACK(qi))
    {
	אם (curwin->w_llist == qi)
	    win = curwin;
	אחרת
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    אם (win == عدم)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    אם (win == عدم)
		החזר;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    صحيح do_fill = TRUE;
    אם (old_last == عدم)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	אם (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    אם (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	אם (old_last == عدم)
	{
	    (فراغ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    אם ((win = qf_find_win(qi)) != عدم && old_line_count < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ساكن صحيح
qf_buf_add_line(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	صحيح		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    אם (qftf_str != عدم && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    אחרת
    {
	אם (qfp->qf_module != عدم)
	    ga_concat(gap, qfp->qf_module);
	אחרת אם (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != عدم
		&& errbuf->b_fname != عدم)
	{
	    אם (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    אחרת
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		אם (first_bufline && (errbuf->b_sfname == عدم
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    אם (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		אם (qfp->qf_fname == عدم)
		    ga_concat(gap, errbuf->b_fname);
		אחרת
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	אם (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	אחרת אם (qfp->qf_pattern != عدم)
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
    אם (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	החזר FAIL;

    החזר OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ساكن list_T *
call_qftf_func(qf_list_T *qfl, صحيح qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = عدم;
    ساكن صحيح	recursive = FALSE;

    אם (recursive)
	החזר عدم;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    אם (qfl->qf_qftf_cb.cb_name != عدم)
	cb = &qfl->qf_qftf_cb;
    אם (cb->cb_name != عدم)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	אם ((d = dict_alloc_lock(VAR_FIXED)) == عدم)
	{
	    recursive = FALSE;
	    החזר عدم;
	}
	dict_add_number(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_number(d, "winid", (long)qf_winid);
	dict_add_number(d, "id", (long)qfl->qf_id);
	dict_add_number(d, "start_idx", start_idx);
	dict_add_number(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = عدم;
	textlock++;
	אם (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    אם (rettv.v_type == VAR_LIST)
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
    החזר qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ساكن فراغ
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, صحيح qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    صحيح		old_KeyTyped = KeyTyped;
    list_T	*qftf_list = عدم;
    listitem_T	*qftf_li = عدم;

    אם (old_last == عدم)
    {
	win_T		*wp;
	tabpage_T	*tp;

	אם (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    החזר;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	כלעוד ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (فراغ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    אם (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    אם (qfl != عدم && qfl->qf_start != عدم)
    {
	char_u		dirname[MAXPATHL];
	صحيح		invalid_val = FALSE;
	صحيح		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	אם (old_last == عدم)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	אחרת
	{
	    אם (old_last->qf_next != عدم)
		qfp = old_last->qf_next;
	    אחרת
		qfp = old_last;
	    lnum = buf->b_ml.ml_line_count;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_count);
	אם (qftf_list != عدم)
	    qftf_li = qftf_list->lv_first;

	כלעוד (lnum < qfl->qf_count)
	{
	    char_u	*qftf_str = عدم;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    אם (qftf_li != عدم && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		אם (qftf_str == عدم)
		    invalid_val = TRUE;
	    }

	    אם (qf_buf_add_line(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		اكسر;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    אם (qfp == عدم)
		اكسر;

	    אם (qftf_li != عدم)
		qftf_li = qftf_li->li_next;
	}

	אם (old_last == عدم)
	    // Delete the empty line which is now at the end
	    (فراغ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    אם (old_last == عدم)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_value_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", عدم,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", عدم,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    KeyTyped = old_KeyTyped;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ساكن فراغ
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ساكن صحيح
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    صحيح		qf_idx;

    עבור (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	אם (qi->qf_lists[qf_idx].qf_id == qfid)
	    החזר qf_idx;
    החזר INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ساكن صحيح
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    صحيح curlist;

    אם (qf_get_curlist(qi)->qf_id == save_qfid)
	החזר OK;

    curlist = qf_id2nr(qi, save_qfid);
    אם (curlist < 0)
	// list is not present
	החזר FAIL;
    qi->qf_curlist = curlist;
    החזר OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ساكن فراغ
qf_jump_first(qf_info_T *qi, int_u save_qfid, صحيح forceit)
{
    אם (qf_restore_list(qi, save_qfid) == FAIL)
	החזר;


    אם (!check_can_set_curbuf_forceit(forceit))
	החזר;


    // Autocommands might have cleared the list, check for that.
    אם (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    صحيح
grep_internal(cmdidx_T cmdidx)
{
    החזר ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ساكن char_u *
make_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_make:	    החזר (char_u *)"make";
	حالة CMD_lmake:	    החזר (char_u *)"lmake";
	حالة CMD_grep:	    החזר (char_u *)"grep";
	حالة CMD_lgrep:	    החזר (char_u *)"lgrep";
	حالة CMD_grepadd:   החזר (char_u *)"grepadd";
	حالة CMD_lgrepadd:  החזר (char_u *)"lgrepadd";
	افتراضي: החזר عدم;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ساكن char_u *
get_mef_name(فراغ)
{
    char_u	*p;
    char_u	*name;
    ساكن صحيح	start = -1;
    ساكن صحيح	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    אם (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	אם (name == عدم)
	    emsg(_(e_cant_get_temp_file_name));
	החזר name;
    }

    עבור (p = p_mef; *p; ++p)
	אם (p[0] == '#' && p[1] == '#')
	    اكسر;

    אם (*p == NUL)
	החזר vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    עבור (;;)
    {
	אם (start == -1)
	    start = mch_get_pid();
	אחרת
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	אם (name == عدم)
	    اكسر;
	STRCPY(name, p_mef);
	sprintf((حرف *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	אם (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((حرف *)name, &sb) < 0
# endif
		)
	    اكسر;
	vim_free(name);
    }
    החזר name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ساكن char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    אם (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    אם (cmd == عدم)
	החזר عدم;
    sprintf((حرف *)cmd, "%s%s%s", (حرف *)p_shq, (حرف *)makecmd,
							       (حرف *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    אם (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    אם (msg_col == 0)
	msg_didout = FALSE;
    msg_start();
    msg_puts(":!");
    msg_outtrans(cmd);		// show what we are doing

    החזר cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    فراغ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = عدم;
    win_T	*wp = عدم;
    qf_info_T	*qi = ql_info;
    صحيح		res;
    char_u	*au_name = عدم;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    صحيح		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    אם (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	החזר;
    }

    au_name = make_get_auname(eap->cmdidx);
    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	אם (aborting())
	    החזר;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    אם (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    אם (fname == عدم)
	החזר;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    אם (cmd == عدم)
    {
	vim_free(fname);
	החזר;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (فراغ)char_avail();
# endif

    incr_quickfix_busy();

    אם (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    אם (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    אם (wp != عدم)
    {
	qi = GET_LOC_LIST(wp);
	אם (qi == عدم)
	    اذهب cleanup;
    }
    אחרת אם (qi == عدم)
	اذهب cleanup;

    אם (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    אם (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    אם (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    صحيح
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    אם ((qi = qf_cmd_get_stack(eap, FALSE)) == عدم)
	החזר 0;
    החזר qf_get_curlist(qi)->qf_count;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    صحيح
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i, sz = 0;
    صحيح		prev_fnum = 0;

    אם ((qi = qf_cmd_get_stack(eap, FALSE)) == عدم)
	החזר 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	אם (qfp->qf_valid)
	{
	    אם (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    אחרת אם (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    החזר sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    صحيح
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    אם ((qi = qf_cmd_get_stack(eap, FALSE)) == عدم)
	החזר 0;

    החזר qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    صحيح
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i, eidx = 0;
    صحيح		prev_fnum = 0;

    אם ((qi = qf_cmd_get_stack(eap, FALSE)) == عدم)
	החזר 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    אם (!qf_list_has_valid_entries(qfl))
	החזר 1;

    עבור (i = 1; i <= qfl->qf_index && qfp!= عدم; i++, qfp = qfp->qf_next)
    {
	אם (qfp->qf_valid)
	{
	    אם (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		אם (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    אחרת
		eidx++;
	}
    }

    החזר eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ساكن صحيح
qf_get_nth_valid_entry(qf_list_T *qfl, صحيح n, صحيح fdo)
{
    qfline_T	*qfp;
    صحيح		i, eidx;
    صحيح		prev_fnum = 0;

    // check if the list has valid errors
    אם (!qf_list_has_valid_entries(qfl))
	החזר 1;

    eidx = 0;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	אם (qfp->qf_valid)
	{
	    אם (fdo)
	    {
		אם (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    אחרת
		eidx++;
	}

	אם (eidx == n)
	    اكسر;
    }

    אם (i <= qfl->qf_count)
	החזר i;
    אחרת
	החזר 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    فراغ
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		errornr;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    אם (eap->addr_count > 0)
	errornr = (صحيح)eap->line2;
    אחרת
    {
	بدل (eap->cmdidx)
	{
	    حالة CMD_cc: حالة CMD_ll:
		errornr = 0;
		اكسر;
	    حالة CMD_crewind: حالة CMD_lrewind: حالة CMD_cfirst:
	    حالة CMD_lfirst:
		errornr = 1;
		اكسر;
	    افتراضي:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    אם (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_count > 0 ? (صحيح)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    فراغ
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    صحيح		errornr;
    صحيح		dir;

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    אם (eap->addr_count > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (صحيح)eap->line2;
    אחרת
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    بدل (eap->cmdidx)
    {
	حالة CMD_cnext: حالة CMD_lnext: حالة CMD_cdo: حالة CMD_ldo:
	    dir = FORWARD;
	    اكسر;
	حالة CMD_cprevious: حالة CMD_lprevious: حالة CMD_cNext:
	حالة CMD_lNext:
	    dir = BACKWARD;
	    اكسر;
	حالة CMD_cnfile: حالة CMD_lnfile: حالة CMD_cfdo: حالة CMD_lfdo:
	    dir = FORWARD_FILE;
	    اكسر;
	حالة CMD_cpfile: حالة CMD_lpfile: حالة CMD_cNfile: حالة CMD_lNfile:
	    dir = BACKWARD_FILE;
	    اكسر;
	افتراضي:
	    dir = FORWARD;
	    اكسر;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ساكن qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, صحيح bnr, صحيح *errornr)
{
    qfline_T	*qfp = عدم;
    صحيح		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ITEMS(qfl, qfp, idx)
	אם (qfp->qf_fnum == bnr)
	    اكسر;

    *errornr = idx;
    החזר qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساكن qfline_T *
qf_find_first_entry_on_line(qfline_T *entry, صحيح *errornr)
{
    כלעוד (!got_int
	    && entry->qf_prev != عدم
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    החזר entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ساكن qfline_T *
qf_find_last_entry_on_line(qfline_T *entry, صحيح *errornr)
{
    כלעוד (!got_int &&
	    entry->qf_next != عدم
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    החזר entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ساكن صحيح
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    אם (linewise)
	החזר qfp->qf_lnum > pos->lnum;
    אחרת
	החזר (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ساكن صحيح
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    אם (linewise)
	החזר qfp->qf_lnum < pos->lnum;
    אחרת
	החזר (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ساكن صحيح
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    אם (linewise)
	החזר qfp->qf_lnum >= pos->lnum;
    אחרת
	החזר (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ساكن صحيح
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, صحيح linewise)
{
    אם (linewise)
	החזר qfp->qf_lnum <= pos->lnum;
    אחרת
	החזר (qfp->qf_lnum < pos->lnum ||
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
    ساكن qfline_T *
qf_find_entry_after_pos(
	صحيح		bnr,
	pos_T		*pos,
	صحيح		linewise,
	qfline_T	*qfp,
	صحيح		*errornr)
{
    אם (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	החזר qfp;

    // Find the entry just before or at the position 'pos'
    כלעוד (qfp->qf_next != عدم
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    אם (qfp->qf_next == عدم || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	החזר عدم;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    החזר qfp;
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
    ساكن qfline_T *
qf_find_entry_before_pos(
	صحيح		bnr,
	pos_T		*pos,
	صحيح		linewise,
	qfline_T	*qfp,
	صحيح		*errornr)
{
    // Find the entry just before the position 'pos'
    כלעוד (qfp->qf_next != عدم
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    אם (qf_entry_on_or_after_pos(qfp, pos, linewise))
	החזר عدم;

    אם (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_line(qfp, errornr);

    החזר qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ساكن qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	صحيح		bnr,
	pos_T		*pos,
	صحيح		dir,
	صحيح		linewise,
	صحيح		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    אם (qfp == عدم)
	החזר عدم;		// no entry in this file

    אם (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    אחרת
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    החזר qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ساكن فراغ
qf_get_nth_below_entry(qfline_T *entry_arg, صحيح n, صحيح linewise, صحيح *errornr)
{
    qfline_T *entry = entry_arg;

    כלעוד (n-- > 0 && !got_int)
    {
	صحيح		first_errornr = *errornr;

	אם (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_line(entry, errornr);

	אם (entry->qf_next == عدم
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    אם (linewise)
		*errornr = first_errornr;
	    اكسر;
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
    ساكن فراغ
qf_get_nth_above_entry(qfline_T *entry, صحيح n, صحيح linewise, صحيح *errornr)
{
    כלעוד (n-- > 0 && !got_int)
    {
	אם (entry->qf_prev == عدم
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    اكسر;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	אם (linewise)
	    entry = qf_find_first_entry_on_line(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ساكن صحيح
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	صحيح		bnr,
	pos_T		*pos,
	صحيح		n,
	صحيح		dir,
	صحيح		linewise)
{
    qfline_T	*adj_entry;
    صحيح		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    אם (adj_entry == عدم)
	החזר 0;

    אם (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	אם (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	אחרת
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    החזר errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    فراغ
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    صحيح		dir;
    صحيح		buf_has_flag;
    صحيح		errornr = 0;
    pos_T	pos;

    אם (eap->addr_count > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	החזר;
    }

    // Check whether the current buffer has any quickfix entries
    אם (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    אחרת
	buf_has_flag = BUF_HAS_LL_ENTRY;
    אם (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	החזר;
    }

    אם ((qi = qf_cmd_get_stack(eap, TRUE)) == عدم)
	החזר;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    אם (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	החזר;
    }

    אם (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    אחרת
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_count > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    אם (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    אחרת
	emsg(_(e_no_more_items));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ساكن char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cfile:	    החזר (char_u *)"cfile";
	حالة CMD_cgetfile:  החזר (char_u *)"cgetfile";
	حالة CMD_caddfile:  החזר (char_u *)"caddfile";
	حالة CMD_lfile:	    החזר (char_u *)"lfile";
	حالة CMD_lgetfile:  החזר (char_u *)"lgetfile";
	حالة CMD_laddfile:  החזר (char_u *)"laddfile";
	افتراضي:	    החזר عدم;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    فراغ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = عدم;
    win_T	*wp = عدم;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = عدم;
    int_u	save_qfid = 0;		// init for gcc
    صحيح		res;

    au_name = cfile_get_auname(eap->cmdidx);
    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							عدم, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	אם (aborting())
	    החזר;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    אם (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   عدم, عدم,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), عدم);
	אם (browse_file == عدم)
	    החזר;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    אחרת
# endif
    אם (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    אם (is_loclist_cmd(eap->cmdidx))
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
    אם (wp != عدم)
    {
	qi = GET_LOC_LIST(wp);
	אם (qi == عدم)
	{
	    decr_quickfix_busy();
	    החזר;
	}
    }
    אחרת אם (qi == عدم)
    {
	decr_quickfix_busy();
	החזר;
    }
    אם (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    אם (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, عدم, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    אם (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ساكن char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_vimgrep:     החזר (char_u *)"vimgrep";
	حالة CMD_lvimgrep:    החזר (char_u *)"lvimgrep";
	حالة CMD_vimgrepadd:  החזר (char_u *)"vimgrepadd";
	حالة CMD_lvimgrepadd: החזר (char_u *)"lvimgrepadd";
	حالة CMD_grep:	      החזר (char_u *)"grep";
	حالة CMD_lgrep:	      החזר (char_u *)"lgrep";
	حالة CMD_grepadd:     החזר (char_u *)"grepadd";
	حالة CMD_lgrepadd:    החזר (char_u *)"lgrepadd";
	افتراضي: החזר عدم;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ساكن فراغ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = عدم;

    אם (s == عدم || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	אם (last_search_pat() == عدم)
	{
	    emsg(_(e_no_previous_regular_expression));
	    החזר;
	}
	regmatch->regprog = vim_regcomp(last_search_pat(), RE_MAGIC);
    }
    אחרת
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ساكن فراغ
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    msg_start();
    p = msg_strtrunc(fname, TRUE);
    אם (p == عدم)
	msg_outtrans(fname);
    אחרת
    {
	msg_outtrans(p);
	vim_free(p);
    }
    msg_clr_eos();
    msg_didout = FALSE;	    // overwrite this message
    msg_nowait = TRUE;	    // don't wait for this message
    msg_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ساكن buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    صحيح		save_mls;
# אם defined(FEAT_SYN_HL)
    char_u	*save_ei = عدم;
# endif
    buf_T	*buf;

# אם defined(FEAT_SYN_HL)
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
# אם defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    החזר buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ساكن صحيح
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    אם (!qflist_valid(wp, qfid))
    {
	אם (wp != عدم)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    החזר FALSE;
	}
	אחרת
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    החזר TRUE;
	}
    }

    אם (qf_restore_list(qi, qfid) == FAIL)
	החזר FALSE;

    החזר TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ساكن صحيح
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	صحيح	    duplicate_name,
	صحيح	    flags)
{
    صحيح		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    صحيح		pat_len = (صحيح)STRLEN(spat);
    אם (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    עבור (lnum = 1; lnum <= buf->b_ml.ml_line_count && *tomatch > 0; ++lnum)
    {
	col = 0;
	אם (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    כלעוד (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, عدم) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		אם (qf_add_entry(qfl,
			    عدم,	// dir
			    fname,
			    عدم,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    عدم,	// search pattern
			    0,		// nr
			    0,		// type
			    عدم,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    اكسر;
		}
		found_match = TRUE;
		אם (--*tomatch == 0)
		    اكسر;
		אם ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    اكسر;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		אם (col > ml_get_buf_len(buf, lnum))
		    اكسر;
	    }
	}
	אחרת
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    صحيح	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    כלעוד (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		אם (qf_add_entry(qfl,
			    عدم,	// dir
			    fname,
			    عدم,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    عدم,	// search pattern
			    0,		// nr
			    0,		// type
			    عدم,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    اكسر;
		}
		found_match = TRUE;
		אם (--*tomatch == 0)
		    اكسر;
		אם ((flags & VGR_GLOBAL) == 0)
		    اكسر;
		col = matches[pat_len - 1] + col + 1;
		אם (col > linelen)
		    اكسر;
	    }
	}
	line_breakcheck();
	אם (got_int)
	    اكسر;
    }

    החזר found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    ساكن فراغ
vgr_jump_to_match(
	qf_info_T   *qi,
	صحيح	    forceit,
	صحيح	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    אם (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    אם (curbuf == first_match_buf && target_dir != عدم)
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
تعريفنوع بنية
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    صحيح		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    صحيح		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ساكن صحيح
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = عدم;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    אם (eap->addr_count > 0)
	args->tomatch = eap->line2;
    אחרת
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    אם (p == عدم)
    {
	emsg(_(e_invalid_search_pattern_or_delimiter));
	החזר FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    אם (args->regmatch.regprog == عدم)
	החזר FAIL;

    p = skipwhite(p);
    אם (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	החזר FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    אם ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	החזר FAIL;
    }

    החזר OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ساكن صحيح
existing_swapfile(buf_T *buf)
{
    אם (buf->b_ml.ml_mfp != عدم && buf->b_ml.ml_mfp->mf_fname != عدم)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	החזר fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    החזר FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ساكن صحيح
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	صحيح		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    صحيح		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    صحيح		fi;
    buf_T	*buf;
    صحيح		duplicate_name = FALSE;
    صحيح		using_dummy;
    char_u	*dirname_start = عدم;
    char_u	*dirname_now = عدم;
    صحيح		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    אם (dirname_start == عدم || dirname_now == عدم)
	اذهب theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    עבור (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	אם (time(عدم) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(عدم);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	אם (buf == عدم || buf->b_ml.ml_mfp == عدم)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != عدم);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	אחרת
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	אם (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    اذهب theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	אם (buf == عدم)
	{
	    אם (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	אחרת
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    אם (using_dummy)
	    {
		אם (found_match && *first_match_buf == عدم)
		    *first_match_buf = buf;
		אם (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = عدم;
		}
		אחרת אם ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    אם (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = عدم;
		    }
		    אחרת אם (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = عدم;
		    }
		}

		אם (buf != عدم)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    אם (buf == *first_match_buf
			    && *target_dir == عدم
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    אם (curbuf == buf)
		    {
# אם defined(FEAT_SYN_HL)
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
    החזר status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    فراغ
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = عدم;
    صحيح		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = عدم;
    char_u	*target_dir = عدم;
    char_u	*au_name =  عدم;
    صحيح		status;

    אם (!check_can_set_curbuf_forceit(eap->forceit))
	החזר;

    au_name = vgr_get_auname(eap->cmdidx);
    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	אם (aborting())
	    החזר;
# endif
    }

    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    אם (qi == عدم)
	החזר;

    אם (vgr_process_args(eap, &args) == FAIL)
	اذهب theend;

    אם ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_stack_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    אם (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	اذهب theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, عدم);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    אם (au_name != عدم)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    אם (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	اذهب theend;
    }

    // Jump to first match.
    אם (!qf_list_empty(qf_get_curlist(qi)))
    {
	אם ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    אחרת
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    אם (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# אחרת
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
    ساكن فراغ
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    אם (dirname_now == عدم)
	החזר;

    mch_dirname(dirname_now, MAXPATHL);
    אם (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == عدم) ? CMD_cd : CMD_lcd;
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
    ساكن buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    صحيح		failed = TRUE;
    aco_save_T	aco;
    صحيح		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(عدم, عدم, (linenr_T)1, BLN_DUMMY);
    אם (newbuf == عدم)
	החזר عدم;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    אם (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	אם (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (فراغ)setfname(curbuf, fname, عدم, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = عدم;
	    readfile_result = readfile(fname, عدم,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			عدم, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    אם (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		אם (curbuf != newbuf)
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

	    אם (newbuf_to_wipe.br_buf != عدم && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, عدم);
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

    אם (!bufref_valid(&newbufref))
	החזר عدم;
    אם (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	החזר عدم;
    }
    החזר newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ساكن فراغ
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    כלעוד (buf->b_nwindows > 0)
    {
	صحيح	    did_one = FALSE;
	win_T	    *wp;

	אם (firstwin->w_next != عدم)
	    FOR_ALL_WINDOWS(wp)
		אם (wp->w_buffer == buf)
		{
		    אם (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    اكسر;
		}
	אם (!did_one)
	    اذهب fail;
    }

    אם (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# אם defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# אם defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	אם (dirname_start != عدم)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	החזר;
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
    ساكن فراغ
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    אם (curbuf == buf)		// safety check
	החזר;

    close_buffer(عدم, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# אם defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ساكن صحيح
get_qfline_items(qfline_T *qfp, list_T *list)
{
    صحيح		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    אם (bufnum != 0 && (buflist_findnr(bufnum) == عدم))
	bufnum = 0;

    אם ((dict = dict_alloc()) == عدم)
	החזר FAIL;
    אם (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	החזר FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    אם (dict_add_number(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_number(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_number(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_number(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_number(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_number(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_number(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (صحيح)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_number(dict, "valid", (long)qfp->qf_valid) == FAIL)
	החזר FAIL;

    החזר OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ساكن صحيح
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	صحيح		qf_idx,
	صحيح		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    صحيح		i;

    אם (qi == عدم)
    {
	qi = ql_info;
	אם (wp != عدم)
	    qi = GET_LOC_LIST(wp);
	אם (qi == عدم)
	    החזר FAIL;

    }

    אם (eidx < 0)
	החזר OK;

    אם (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    אם (qf_idx >= qi->qf_listcount)
	החזר FAIL;

    qfl = qf_get_list(qi, qf_idx);
    אם (qf_list_empty(qfl))
	החזר FAIL;

    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	אם (eidx > 0)
	{
	    אם (eidx == i)
		החזר get_qfline_items(qfp, list);
	}
	אחרת אם (get_qfline_items(qfp, list) == FAIL)
	    החזר FAIL;
    }

    החזר OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
تعداد {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ITEMS	= 0x2,
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
    ساكن صحيح
qf_get_list_from_lines(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    صحيح		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    אם (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == عدم)
	החזר FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    אם ((efm_di = dict_find(what, (char_u *)"efm", -1)) != عدم)
    {
	אם (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == عدم)
	    החזר FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    אם (l == عدم)
	החזר FAIL;

    qi = qf_alloc_stack(QFLT_INTERNAL, 1);
    אם (qi != عدم)
    {
	אם (qf_init_ext(qi, 0, عدم, عدم, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, عدم, عدم) > 0)
	{
	    (فراغ)get_errorlist(qi, عدم, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    החזר status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ساكن صحيح
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    אם (qi == عدم)
	החזר 0;
    win = qf_find_win(qi);
    אם (win != عدم)
	החזר win->w_id;
    החזר 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ساكن صحيح
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    صحيح	bufnum = 0;

    אם (qi != عدم && buflist_findnr(qi->qf_bufnr) != عدم)
	bufnum = qi->qf_bufnr;

    החזר dict_add_number(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ساكن صحيح
qf_getprop_keys2flags(dict_T *what, صحيح loclist)
{
    صحيح		flags = QF_GETLIST_NONE;

    אם (dict_has_key(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	אם (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    אם (dict_has_key(what, "title"))
	flags |= QF_GETLIST_TITLE;

    אם (dict_has_key(what, "nr"))
	flags |= QF_GETLIST_NR;

    אם (dict_has_key(what, "winid"))
	flags |= QF_GETLIST_WINID;

    אם (dict_has_key(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    אם (dict_has_key(what, "id"))
	flags |= QF_GETLIST_ID;

    אם (dict_has_key(what, "items"))
	flags |= QF_GETLIST_ITEMS;

    אם (dict_has_key(what, "idx"))
	flags |= QF_GETLIST_IDX;

    אם (dict_has_key(what, "size"))
	flags |= QF_GETLIST_SIZE;

    אם (dict_has_key(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    אם (loclist && dict_has_key(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    אם (dict_has_key(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    אם (dict_has_key(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    החזר flags;
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
    ساكن صحيح
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    صحيح		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    אם ((di = dict_find(what, (char_u *)"nr", -1)) != عدم)
    {
	// Use the specified quickfix/location list
	אם (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    אם (di->di_tv.vval.v_number != 0)
	    {
		qf_idx = di->di_tv.vval.v_number - 1;
		אם (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	אחרת אם (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != عدم
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	אחרת
	    qf_idx = INVALID_QFIDX;
    }

    אם ((di = dict_find(what, (char_u *)"id", -1)) != عدم)
    {
	// Look for a list with the specified id
	אם (di->di_tv.v_type == VAR_NUMBER)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    אם (di->di_tv.vval.v_number != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_number);
	}
	אחרת
	    qf_idx = INVALID_QFIDX;
    }

    החזר qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ساكن صحيح
qf_getprop_defaults(qf_info_T *qi, صحيح flags, صحيح locstack, dict_T *retdict)
{
    صحيح		status = OK;

    אם (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    אם ((status == OK) && (flags & QF_GETLIST_ITEMS))
    {
	list_T	*l = list_alloc();
	אם (l != عدم)
	    status = dict_add_list(retdict, "items", l);
	אחרת
	    status = FAIL;
    }
    אם ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", 0);
    אם ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    אם ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    אם ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", 0);
    אם ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_number(retdict, "idx", 0);
    אם ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", 0);
    אם ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", 0);
    אם ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_number(retdict, "filewinid", 0);
    אם ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    החזר status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ساكن صحيح
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    החזר dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ساكن صحيح
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    صحيح winid = 0;

    אם (wp != عدم && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	אם (ll_wp != عدم)
	    winid = ll_wp->w_id;
    }

    החזר dict_add_number(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ساكن صحيح
qf_getprop_items(qf_info_T *qi, صحيح qf_idx, صحيح eidx, dict_T *retdict)
{
    صحيح		status = OK;
    list_T	*l = list_alloc();
    אם (l != عدم)
    {
	(فراغ)get_errorlist(qi, عدم, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    אחרת
	status = FAIL;

    החזר status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ساكن صحيح
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    صحيح		status;
    dictitem_T	*di;

    אם (qfl->qf_ctx != عدم)
    {
	di = dictitem_alloc((char_u *)"context");
	אם (di != عدم)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    אם (status == FAIL)
		dictitem_free(di);
	}
	אחרת
	    status = FAIL;
    }
    אחרת
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    החזר status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ساكن صحيح
qf_getprop_idx(qf_list_T *qfl, صحيح eidx, dict_T *retdict)
{
    אם (eidx == 0)
    {
	eidx = qfl->qf_index;
	אם (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    החזר dict_add_number(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ساكن صحيح
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    صحيح		status;

    אם (qfl->qf_qftf_cb.cb_name != عدم)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    אחרת
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    החזר status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ساكن صحيح
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    صحيح		status = OK;
    صحيح		qf_idx = INVALID_QFIDX;
    صحيح		eidx = 0;
    dictitem_T	*di;
    صحيح		flags = QF_GETLIST_NONE;

    אם ((di = dict_find(what, (char_u *)"lines", -1)) != عدم)
	החזר qf_get_list_from_lines(what, di, retdict);

    אם (wp != عدم)
	qi = GET_LOC_LIST(wp);
    אחרת אם (qi == عدم)
	החזר FAIL;

    flags = qf_getprop_keys2flags(what, (wp != عدم));

    אם (!qf_stack_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    אם (qf_stack_empty(qi) || qf_idx == INVALID_QFIDX)
	החזר qf_getprop_defaults(qi, flags, wp != عدم, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    אם ((di = dict_find(what, (char_u *)"idx", -1)) != عدم)
    {
	אם (di->di_tv.v_type != VAR_NUMBER)
	    החזר FAIL;
	eidx = di->di_tv.vval.v_number;
    }

    אם (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_number(retdict, "nr", qf_idx + 1);
    אם ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_number(retdict, "winid", qf_winid(qi));
    אם ((status == OK) && (flags & QF_GETLIST_ITEMS))
	status = qf_getprop_items(qi, qf_idx, eidx, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_number(retdict, "id", qfl->qf_id);
    אם ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_number(retdict, "size", qfl->qf_count);
    אם ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_number(retdict, "changedtick", qfl->qf_changedtick);
    אם ((status == OK) && (wp != عدم) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    אם ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    החזר status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ساكن صحيح
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	صحيح		first_entry,
	صحيح		*valid_entry)
{
    ساكن صحيح	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    صحيح		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    אם (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (صحيح)dict_get_number(d, "bufnr");
    lnum = (صحيح)dict_get_number(d, "lnum");
    end_lnum = (صحيح)dict_get_number(d, "end_lnum");
    col = (صحيح)dict_get_number(d, "col");
    end_col = (صحيح)dict_get_number(d, "end_col");
    vcol = (صحيح)dict_get_number(d, "vcol");
    nr = (صحيح)dict_get_number(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    אם (text == عدم)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    אם ((filename == عدم && bufnum == 0) || (lnum == 0 && pattern == عدم))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    אם (bufnum != 0 && (buflist_findnr(bufnum) == عدم))
    {
	אם (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    אם (dict_has_key(d, "valid"))
	valid = (صحيح)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			عدم,		// dir
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
			type == عدم ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    אם (valid)
	*valid_entry = TRUE;

    החזר status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ساكن صحيح
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	صحيح		target_fnum,
	صحيح		target_lnum,
	صحيح		target_col)
{
    // First, compare entries to target file.
    אם (!target_fnum)
	// Without a target file, we can't know which is closer.
	החזר FALSE;

    صحيح is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    صحيح other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    אם (!is_target_file && other_is_target_file)
	החזר FALSE;
    אחרת אם (is_target_file && !other_is_target_file)
	החזר TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    אם (!target_lnum)
	// Without a target line number, we can't know which is closer.
	החזר FALSE;

    صحيح line_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    صحيح other_line_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    אם (line_distance > other_line_distance)
	החזר FALSE;
    אחרת אם (line_distance < other_line_distance)
	החזר TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    אם (!target_col)
	// Without a target column, we can't know which is closer.
	החזר FALSE;

    صحيح column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    صحيح other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    אם (column_distance > other_column_distance)
	החזר FALSE;
    אחרת אם (column_distance < other_column_distance)
	החזר TRUE;

    // It's a complete tie! The exact same file, line, and column.
    החזר FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ساكن صحيح
qf_add_entries(
	qf_info_T	*qi,
	صحيح		qf_idx,
	list_T		*list,
	char_u		*title,
	صحيح		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = عدم;
    صحيح		retval = OK;
    صحيح		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    صحيح prev_fnum = 0;
    صحيح prev_lnum = 0;
    صحيح prev_col = 0;
    אם (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    صحيح select_first_entry = FALSE;
    صحيح select_nearest_entry = FALSE;

    אם (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    אחרת אם (action == 'a')
    {
	אם (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	אחרת
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    אחרת אם (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_items(qfl);
	qf_store_title(qfl, title);
    }
    אחרת אם (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_items(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = عدم;
    صحيح entry_to_select_index = 0;

    FOR_ALL_LIST_ITEMS(list, li)
    {
	אם (li->li_tv.v_type != VAR_DICT)
	    استمر; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	אם (d == عدم)
	    استمر;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	אם (retval == QF_FAIL)
	    اكسر;

	qfline_T *entry = qfl->qf_last;
	אם (
	    (select_first_entry && entry_to_select == عدم) ||
	    (select_nearest_entry &&
		(entry_to_select == عدم ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_count;
	}
    }

    // Check if any valid error entries are added to the list.
    אם (valid_entry)
	qfl->qf_nonevalid = FALSE;
    אחרת אם (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    אם (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    החזר retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ساكن صحيح
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	صحيح		action,
	صحيح		*newlist)
{
    dictitem_T	*di;
    صحيح		qf_idx = qi->qf_curlist;    // default is the current list

    אם ((di = dict_find(what, (char_u *)"nr", -1)) != عدم)
    {
	// Use the specified quickfix/location list
	אם (di->di_tv.v_type == VAR_NUMBER)
	{
	    // for zero use the current list
	    אם (di->di_tv.vval.v_number != 0)
		qf_idx = di->di_tv.vval.v_number - 1;

	    אם ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_stack_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    אחרת אם (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		החזר INVALID_QFIDX;
	    אחרת אם (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	אחרת אם (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != عدم
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    אם (!qf_stack_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    אחרת אם (*newlist)
		qf_idx = 0;
	    אחרת
		החזר INVALID_QFIDX;
	}
	אחרת
	    החזר INVALID_QFIDX;
    }

    אם (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != عدم)
    {
	// Use the quickfix/location list with the specified id
	אם (di->di_tv.v_type != VAR_NUMBER)
	    החזר INVALID_QFIDX;

	החזר qf_id2nr(qi, di->di_tv.vval.v_number);
    }

    החזר qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ساكن صحيح
qf_setprop_title(qf_info_T *qi, صحيح qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    אם (di->di_tv.v_type != VAR_STRING)
	החזר FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    אם (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    החזר OK;
}

/*
 * Set quickfix list items/entries.
 */
    ساكن صحيح
qf_setprop_items(qf_info_T *qi, صحيح qf_idx, dictitem_T *di, صحيح action)
{
    صحيح		retval = FAIL;
    char_u	*title_save;

    אם (di->di_tv.v_type != VAR_LIST)
	החזר FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    החזר retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ساكن صحيح
qf_setprop_items_from_lines(
	qf_info_T	*qi,
	صحيح		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	صحيح		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    صحيح		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    אם ((efm_di = dict_find(what, (char_u *)"efm", -1)) != عدم)
    {
	אם (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == عدم)
	    החזר FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    אם (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == عدم)
	החזר FAIL;

    אם (action == 'r' || action == 'u')
	qf_free_items(&qi->qf_lists[qf_idx]);
    אם (qf_init_ext(qi, qf_idx, عدم, عدم, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, عدم, عدم) >= 0)
	retval = OK;

    החזר retval;
}

/*
 * Set quickfix list context.
 */
    ساكن صحيح
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    אם (ctx != عدم)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    החזר OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ساكن صحيح
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    صحيح		denote = FALSE;
    صحيح		newidx;
    صحيح		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    אם (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != عدم
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_count;
    אחרת
    {
	// Otherwise use the specified index
	newidx = tv_get_number_chk(&di->di_tv, &denote);
	אם (denote)
	    החזר FAIL;
    }

    אם (newidx < 1)		// sanity check
	החזר FAIL;
    אם (newidx > qfl->qf_count)
	newidx = qfl->qf_count;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    אם (qf_ptr == عدم)
	החזר FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    אם (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    החזר OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ساكن صحيح
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    אם (check_restricted() || check_secure())
	החזר FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    אם (cb.cb_name == عدم || *cb.cb_name == NUL)
	החזר OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    החזר OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ساكن صحيح
qf_set_properties(qf_info_T *qi, dict_T *what, صحيح action, char_u *title)
{
    dictitem_T	*di;
    صحيح		retval = FAIL;
    صحيح		qf_idx;
    صحيح		newlist = FALSE;
    qf_list_T	*qfl;

    אם (action == ' ' || qf_stack_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    אם (qf_idx == INVALID_QFIDX)	// List not found
	החזר FAIL;

    אם (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    אם ((di = dict_find(what, (char_u *)"title", -1)) != عدم)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    אם ((di = dict_find(what, (char_u *)"items", -1)) != عدم)
	retval = qf_setprop_items(qi, qf_idx, di, action);
    אם ((di = dict_find(what, (char_u *)"lines", -1)) != عدم)
	retval = qf_setprop_items_from_lines(qi, qf_idx, what, di, action);
    אם ((di = dict_find(what, (char_u *)"context", -1)) != عدم)
	retval = qf_setprop_context(qfl, di);
    אם ((di = dict_find(what, (char_u *)"idx", -1)) != عدم)
	retval = qf_setprop_curidx(qi, qfl, di);
    אם ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != عدم)
	retval = qf_setprop_qftf(qi, qfl, di);

    אם (newlist || retval == OK)
	qf_list_changed(qfl);
    אם (newlist)
	qf_update_buffer(qi, عدم);

    החזר retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ساكن فراغ
qf_free_stack(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = عدم;

    אם (qfwin != عدم)
    {
	// If the quickfix/location list window is open, then clear it
	אם (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, عدم);
    }

    אם (wp != عدم && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	אם (llwin != عدم)
	    wp = llwin;
    }

    qf_free_all(wp);
    אם (wp == عدم)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    אחרת אם (qfwin != عدم)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_stack(QFLT_LOCATION, wp->w_p_lhi);

	אם (new_ll != عدم)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    אם (wp != qfwin)
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
    صحيح
set_errorlist(
	win_T	*wp,
	list_T	*list,
	صحيح	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    صحيح		retval = OK;

    אם (wp != عدم)
	qi = ll_get_or_alloc_list(wp);
    אחרת
	qi = ql_info;
    אם (qi == عدم)
	החזר FAIL;

    אם (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_stack(wp, qi);
	החזר OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    אם (list->lv_len != 0 && what != عدم)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	החזר FAIL;
    }

    incr_quickfix_busy();

    אם (what != عدم)
	retval = qf_set_properties(qi, what, action, title);
    אחרת
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	אם (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    החזר retval;
}

ساكن صحيح mark_quickfix_user_data(qf_info_T *qi, صحيح copyID)
{
    صحيح abort = FALSE;
    עבור (صحيح i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	אם (!qfl->qf_has_user_data)
	    استمر;
	qfline_T *qfp;
	صحيح j;
	FOR_ALL_QFL_ITEMS(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    אם (user_data != عدم && user_data->v_type != VAR_NUMBER
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, عدم, عدم, عدم);
	}
    }
    החזר abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ساكن صحيح
mark_quickfix_ctx(qf_info_T *qi, صحيح copyID)
{
    صحيح		i;
    صحيح		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    עבור (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	אם (ctx != عدم && ctx->v_type != VAR_NUMBER
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, عدم, عدم, عدم);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    החזר abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    صحيح
set_ref_in_quickfix(صحيح copyID)
{
    صحيح		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    אם (ql_info == عدم)
	החזר TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    אם (abort)
	החזר abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    אם (abort)
	החזר abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    אם (abort)
	החזר abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	אם (win->w_llist != عدم)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    אם (abort)
		החזר abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    אם (abort)
		החזר abort;
	}
	אם (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    אם (abort)
		החזר abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    אם (abort)
		החזר abort;
	}
    }

    החזר abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ساكن char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cbuffer:	החזר (char_u *)"cbuffer";
	حالة CMD_cgetbuffer:	החזר (char_u *)"cgetbuffer";
	حالة CMD_caddbuffer:	החזר (char_u *)"caddbuffer";
	حالة CMD_lbuffer:	החזר (char_u *)"lbuffer";
	حالة CMD_lgetbuffer:	החזר (char_u *)"lgetbuffer";
	حالة CMD_laddbuffer:	החזר (char_u *)"laddbuffer";
	افتراضي:		החזר عدم;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ساكن صحيح
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = عدم;

    אם (*eap->arg == NUL)
	buf = curbuf;
    אחרת אם (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((حرف *)eap->arg));

    אם (buf == عدم)
    {
	emsg(_(e_invalid_argument));
	החזר FAIL;
    }

    אם (buf->b_ml.ml_mfp == عدم)
    {
	emsg(_(e_buffer_is_not_loaded));
	החזר FAIL;
    }

    אם (eap->addr_count == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_line_count;
    }

    אם (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_line_count
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	החזר FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    החזר OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    فراغ
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = عدم;
    qf_info_T	*qi;
    char_u	*au_name = عدم;
    صحيح		res;
    int_u	save_qfid;
    win_T	*wp = عدم;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	אם (aborting())
	    החזר;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    אם (qi == عدم)
	החזר;

    אם (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	החזר;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    אם (buf->b_sfname)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "%s (%s)",
		(حرف *)qf_title, (حرف *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, عدم, buf, عدم, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, عدم);
    אם (qf_stack_empty(qi))
    {
	decr_quickfix_busy();
	החזר;
    }
    אם (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    אם (au_name != عدم)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	אם (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    אם (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# אם defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    بدل (cmdidx)
    {
	حالة CMD_cexpr:	    החזר (char_u *)"cexpr";
	حالة CMD_cgetexpr:  החזר (char_u *)"cgetexpr";
	حالة CMD_caddexpr:  החזר (char_u *)"caddexpr";
	حالة CMD_lexpr:	    החזר (char_u *)"lexpr";
	حالة CMD_lgetexpr:  החזר (char_u *)"lgetexpr";
	حالة CMD_laddexpr:  החזר (char_u *)"laddexpr";
	افتراضي:	    החזר عدم;
    }
}

    صحيح
trigger_cexpr_autocmd(صحيح cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	אם (aborting())
	    החזר FAIL;
    }
    החזר OK;
}

    صحيح
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = عدم;

    qi = qf_cmd_get_or_alloc_stack(eap, &wp);
    אם (qi == عدم)
	החזר FAIL;

    אם ((tv->v_type == VAR_STRING && tv->vval.v_string != عدم)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != عدم))
    {
	صحيح	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, عدم, عدم, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), عدم);
	אם (qf_stack_empty(qi))
	{
	    decr_quickfix_busy();
	    החזר FAIL;
	}
	אם (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	אם (au_name != عدم)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	אם (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	החזר OK;
    }

    emsg(_(e_string_or_list_expected));
    החזר FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    فراغ
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    אם (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	החזר;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_expr(eap->arg, eap);
    אם (tv == عدم)
	החזר;

    (فراغ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ساكن qf_info_T *
hgr_get_ll(صحيح *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    אם (bt_help(curwin->w_buffer))
	wp = curwin;
    אחרת
	// Find an existing help window
	wp = qf_find_help_win();

    אם (wp == عدم)	    // Help window not found
	qi = عدم;
    אחרת
	qi = wp->w_llist;

    אם (qi == عدم)
    {
	// Allocate a new location list for help text matches
	אם ((qi = qf_alloc_stack(QFLT_LOCATION, 1)) == عدم)
	    החזר عدم;
	*new_ll = TRUE;
    }

    החזר qi;
}

/*
 * Search for a pattern in a help file.
 */
    ساكن فراغ
hgr_search_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((حرف *)fname, "r");
    אם (fd == عدم)
	החזר;

    lnum = 1;
    כלעוד (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *line = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	אם (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    line = string_convert(p_vc, IObuff, عدم);
	    אם (line == عدم)
		line = IObuff;
	}

	אם (vim_regexec(p_regmatch, line, (colnr_T)0))
	{
	    صحيح	l = (صحيح)STRLEN(line);

	    // remove trailing CR, LF, spaces, etc.
	    כלעוד (l > 0 && line[l - 1] <= ' ')
		line[--l] = NUL;

	    אם (qf_add_entry(qfl,
			عدم,	// dir
			fname,
			عدم,
			0,
			line,
			lnum,
			0,
			(صحيح)(p_regmatch->startp[0] - line)
			+ 1,	// col
			(صحيح)(p_regmatch->endp[0] - line)
			+ 1,	// end_col
			FALSE,	// vis_col
			عدم,	// search pattern
			0,	// nr
			1,	// type
			عدم,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		אם (line != IObuff)
		    vim_free(line);
		اكسر;
	    }
	}
	אם (line != IObuff)
	    vim_free(line);
	++lnum;
	line_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ساكن فراغ
hgr_search_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    صحيح		fcount;
    char_u	**fnames;
    صحيح		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    אם (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	עבור (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    אם (lang != عدم
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		استمر;
# endif

	    hgr_search_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ساكن فراغ
hgr_search_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    אם (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    כלעוד (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_search_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    אם (vc.vc_type != CONV_NONE)
	convert_setup(&vc, عدم, عدم);
}

/*
 * ":helpgrep {pattern}"
 */
    فراغ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    صحيح		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    صحيح		new_qi = FALSE;
    char_u	*au_name =  عدم;
    char_u	*lang = عدم;
    صحيح		updated = FALSE;

    بدل (eap->cmdidx)
    {
	حالة CMD_helpgrep:  au_name = (char_u *)"helpgrep"; اكسر;
	حالة CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; اكسر;
	افتراضي: اكسر;
    }
    אם (au_name != عدم && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	אם (aborting())
	    החזר;
# endif
    }

    אם (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	אם (qi == عدم)
	    החזר;
    }
    אחרת אם (qi == عدم)
    {
	emsg(_(e_no_quickfix_stack));
	החזר;
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
    אם (regmatch.regprog != عدم)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_search_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    אם (p_cpo == empty_option)
	p_cpo = save_cpo;
    אחרת
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	אם (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	אם (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    אם (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, عدم);

    אם (au_name != عدم)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	אם (!new_qi && IS_LL_STACK(qi) && qf_find_win_with_loclist(qi) == عدم)
	{
	    decr_quickfix_busy();
	    החזר;
	}
    }

    // Jump to first match.
    אם (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    אחרת
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    אם (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	אם (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    אם (new_qi)
		ll_free_all(&qi);
	}
	אחרת אם (curwin->w_llist == عدم && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# אם defined(EXITFREE)
    فراغ
free_quickfix(فراغ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(عدم);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#אם defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ساكن فراغ
get_qf_loc_list(صحيح is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    אם (what_arg->v_type == VAR_UNKNOWN)
    {
	אם (rettv_list_alloc(rettv) == OK)
	    אם (is_qf || wp != عدم)
		(فراغ)get_errorlist(عدم, wp, -1, 0, rettv->vval.v_list);
    }
    אחרת
    {
	אם (rettv_dict_alloc(rettv) == OK)
	    אם (is_qf || (wp != عدم))
	    {
		אם (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    אם (d != عدم)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		אחרת
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    فراغ
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	החזר;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    فراغ
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    אם (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	החזר;

    get_qf_loc_list(TRUE, عدم, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ساكن فراغ
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    صحيح		action = 0;
    ساكن صحيح	recursive = 0;
# endif

    rettv->vval.v_number = -1;

# ifdef FEAT_QUICKFIX
    אם (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    אחרת אם (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    אחרת
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = عدم;
	صحيح	valid_dict = TRUE;

	אם (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    אם (act == عدم)
		החזר;		// type error; errmsg already given
	    אם ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    אחרת
		semsg(_(e_invalid_action_str_1), act);
	}
	אחרת אם (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	אחרת
	    emsg(_(e_string_required));

	אם (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    אם (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != عدم)
		what = what_arg->vval.v_dict;
	    אחרת
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	אם (l != عدم && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == عدم ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_number = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    فراغ
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_number = -1;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	החזר;

    win = find_win_by_nr_or_id(&argvars[0]);
    אם (win != عدم)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    فراغ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	החזר;

    set_qf_ll_list(عدم, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
