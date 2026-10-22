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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_QUICKFIX)

ಸಂರಚನಾ dir_ರಾಶಿ_T
{
    ಸಂರಚನಾ dir_ರಾಶಿ_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ qfline_S qfline_T;
ಸಂರಚನಾ qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    ಪೂರ್ಣಾಙ್ಕ		qf_fnum;	// file number for the line
    ಪೂರ್ಣಾಙ್ಕ		qf_col;		// column where the error occurred
    ಪೂರ್ಣಾಙ್ಕ		qf_end_col;	// column when the error has range or zero
    ಪೂರ್ಣಾಙ್ಕ		qf_nr;		// error number
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    ಪೂರ್ಣಾಙ್ಕ		qf_ಎಣಿಕೆ;	// number of errors (0 means empty list)
    ಪೂರ್ಣಾಙ್ಕ		qf_index;	// current index in the error list
    ಪೂರ್ಣಾಙ್ಕ		qf_nonevalid;	// TRUE if not a single valid entry found
    ಪೂರ್ಣಾಙ್ಕ		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ಸಂರಚನಾ dir_ರಾಶಿ_T	*qf_dir_ರಾಶಿ;
    char_u		*qf_directory;
    ಸಂರಚನಾ dir_ರಾಶಿ_T	*qf_file_ರಾಶಿ;
    char_u		*qf_currfile;
    ಪೂರ್ಣಾಙ್ಕ			qf_multiline;
    ಪೂರ್ಣಾಙ್ಕ			qf_multiignore;
    ಪೂರ್ಣಾಙ್ಕ			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ಸಂರಚನಾ qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    ಪೂರ್ಣಾಙ್ಕ		qf_refcount;
    ಪೂರ್ಣಾಙ್ಕ		qf_listcount;	    // current number of lists
    ಪೂರ್ಣಾಙ್ಕ		qf_curlist;	    // current error list
    ಪೂರ್ಣಾಙ್ಕ         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    ಪೂರ್ಣಾಙ್ಕ		qf_bufnr;	    // quickfix window buffer number
};

ಸ್ಥಿರ qf_info_T ql_info_actual; // global quickfix list
ಸ್ಥಿರ qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ಸ್ಥಿರ int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ efm_S efm_T;
ಸಂರಚನಾ efm_S
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
    ಪೂರ್ಣಾಙ್ಕ		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ qf_delq_S
{
    ಸಂರಚನಾ qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
ಸ್ಥಿರ qf_delq_T *qf_delq_head = ರಿಕ್ತ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	quickfix_busy = 0;

ಸ್ಥಿರ efm_T	*fmt_start = ರಿಕ್ತ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ಸ್ಥಿರ callback_T qftf_cb;

ಸ್ಥಿರ ಶೂನ್ಯ     qf_ತೆಗೆ_ರಾಶಿ(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ adjust);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, ಪೂರ್ಣಾಙ್ಕ bufnum, char_u *mesg, long lnum, long end_lnum, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ end_col, ಪೂರ್ಣಾಙ್ಕ vis_col, char_u *pattern, ಪೂರ್ಣಾಙ್ಕ nr, ಪೂರ್ಣಾಙ್ಕ type, typval_T *user_data, ಪೂರ್ಣಾಙ್ಕ valid);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      qf_resize_ರಾಶಿ_base(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ     qf_sync_llw_to_win(win_T *llw);
ಸ್ಥಿರ ಶೂನ್ಯ     qf_sync_win_to_llw(win_T *pwp);
ಸ್ಥಿರ qf_info_T *qf_alloc_ರಾಶಿ(qfltype_T qfltype, ಪೂರ್ಣಾಙ್ಕ n);
ಸ್ಥಿರ qf_list_T *qf_alloc_list_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_free(qf_list_T *qfl);
ಸ್ಥಿರ char_u	*qf_types(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ಸ್ಥಿರ char_u	*qf_ತಳ್ಳು_dir(char_u *, ಸಂರಚನಾ dir_ರಾಶಿ_T **, ಪೂರ್ಣಾಙ್ಕ is_file_ರಾಶಿ);
ಸ್ಥಿರ char_u	*qf_ತೆಗೆ_dir(ಸಂರಚನಾ dir_ರಾಶಿ_T **);
ಸ್ಥಿರ char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ಸ್ಥಿರ win_T    *qf_find_win_with_loclist(qf_info_T *ll);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_jump_newwin(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ dir, ಪೂರ್ಣಾಙ್ಕ errornr, ಪೂರ್ಣಾಙ್ಕ forceit, ಪೂರ್ಣಾಙ್ಕ newwin);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_fmt_text(garray_T *gap, char_u *text);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_range_text(garray_T *gap, qfline_T *qfp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qf_win_pos_update(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ old_qf_index);
ಸ್ಥಿರ win_T	*qf_find_win(qf_info_T *qi);
ಸ್ಥಿರ buf_T	*qf_find_buf(qf_info_T *qi);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, ಪೂರ್ಣಾಙ್ಕ qf_winid);
ಸ್ಥಿರ buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
ಸ್ಥಿರ ಶೂನ್ಯ	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
ಸ್ಥಿರ ಶೂನ್ಯ	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
ಸ್ಥಿರ qf_info_T *ll_get_or_alloc_list(win_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, ಪೂರ್ಣಾಙ್ಕ target_fnum, ಪೂರ್ಣಾಙ್ಕ target_lnum, ಪೂರ್ಣಾಙ್ಕ target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == ರಿಕ್ತ)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != ರಿಕ್ತ)

// Quickfix and location list stack check helper macros
# define IS_QF_ರಾಶಿ(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ರಾಶಿ(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i) \
		    ಪ್ರತಿ ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_ಎಣಿಕೆ && (qfp) != ರಿಕ್ತ; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ಸ್ಥಿರ char_u   *qf_last_bufname = ರಿಕ್ತ;
ಸ್ಥಿರ bufref_T  qf_last_bufref = {ರಿಕ್ತ, 0, 0};

ಸ್ಥಿರ garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ಸ್ಥಿರ garray_T *
qfga_get(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ initialized = FALSE;

    ಯದಿ (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    ಪ್ರತ್ಯಾಗಮ &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qfga_clear(ಶೂನ್ಯ)
{
    ಯದಿ (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    ಅನ್ಯಥಾ
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define ಸಾಲು_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ಸ್ಥಿರ ಸಂರಚನಾ fmtpattern
{
    char_u	convchar;
    ವರ್ಣ	*pattern;
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
    ಸ್ಥಿರ char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	ಪೂರ್ಣಾಙ್ಕ	idx,
	ಪೂರ್ಣಾಙ್ಕ	round)
{
    char_u	*srcptr;

    ಯದಿ (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != ರಿಕ್ತ)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == ರಿಕ್ತ))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    ಯದಿ (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    ಯದಿ (*efmpat == 'f' && efmpat[1] != NUL)
    {
	ಯದಿ (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	ಅನ್ಯಥಾ
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    ಅನ್ಯಥಾ
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	ಯಾವತ್ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    ಪ್ರತ್ಯಾಗಮ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ಸ್ಥಿರ char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	ಪೂರ್ಣಾಙ್ಕ	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    ಯದಿ (*efmp == '[' || *efmp == '\\')
    {
	ಯದಿ ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    ಯದಿ (efmp[1] == '^')
		*regpat++ = *++efmp;
	    ಯದಿ (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		ಯಾವತ್ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		ಯದಿ (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    ಅನ್ಯಥಾ
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    *pefmp = efmp;

    ಪ್ರತ್ಯಾಗಮ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ಸ್ಥಿರ char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    ಯದಿ (vim_strchr((char_u *)"+-", *efmp) != ರಿಕ್ತ)
	efminfo->flags = *efmp++;
    ಯದಿ (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ರಿಕ್ತ)
	efminfo->prefix = *efmp;
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
efm_to_regpat(
	char_u	*efm,
	ಪೂರ್ಣಾಙ್ಕ	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    ಪೂರ್ಣಾಙ್ಕ		round;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    ಪ್ರತಿ (efmp = efm; efmp < efm + len; ++efmp)
    {
	ಯದಿ (*efmp == '%')
	{
	    ++efmp;
	    ಪ್ರತಿ (idx = 0; idx < FMT_PATTERNS; ++idx)
		ಯದಿ (fmt_pat[idx].convchar == *efmp)
		    ಭಙ್ಗ;
	    ಯದಿ (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		ಯದಿ (ptr == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		round++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		ಯದಿ (ptr == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ರಿಕ್ತ)
		*ptr++ = *efmp;		// regexp magic characters
	    ಅನ್ಯಥಾ ಯದಿ (*efmp == '#')
		*ptr++ = '*';
	    ಅನ್ಯಥಾ ಯದಿ (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		ಯದಿ (efmp == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ			// copy normal character
	{
	    ಯದಿ (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)".*^$~[", *efmp) != ರಿಕ್ತ)
		*ptr++ = '\\';	// escape regexp atoms
	    ಯದಿ (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Free the 'errorformat' information list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    ಪ್ರತಿ (efm_ptr = *efm_first; efm_ptr != ರಿಕ್ತ; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = ರಿಕ್ತ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
efm_regpat_bufsz(char_u *efm)
{
    ಪೂರ್ಣಾಙ್ಕ sz;
    ಪೂರ್ಣಾಙ್ಕ i;

    sz = (FMT_PATTERNS * 3) + ((ಪೂರ್ಣಾಙ್ಕ)STRLEN(efm) << 2);
    ಪ್ರತಿ (i = FMT_PATTERNS; i > 0; )
	sz += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# ಅನ್ಯಥಾ
    sz += 2; // "%f" can become two chars longer
# endif

    ಪ್ರತ್ಯಾಗಮ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
efm_option_part_len(char_u *efm)
{
    ಪೂರ್ಣಾಙ್ಕ len;

    ಪ್ರತಿ (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	ಯದಿ (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ಸ್ಥಿರ efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ರಿಕ್ತ;
    efm_T	*fmt_first = ರಿಕ್ತ;
    efm_T	*fmt_last = ರಿಕ್ತ;
    char_u	*fmtstr = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    ಯದಿ ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ರಿಕ್ತ)
	ಗಚ್ಛ parse_efm_error;

    ಯಾವತ್ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	ಯದಿ (fmt_ptr == ರಿಕ್ತ)
	    ಗಚ್ಛ parse_efm_error;
	ಯದಿ (fmt_first == ರಿಕ್ತ)	    // first one
	    fmt_first = fmt_ptr;
	ಅನ್ಯಥಾ
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	ಯದಿ (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    ಗಚ್ಛ parse_efm_error;
	ಯದಿ ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ರಿಕ್ತ)
	    ಗಚ್ಛ parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    ಯದಿ (fmt_first == ರಿಕ್ತ)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    ಗಚ್ಛ parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    ಪ್ರತ್ಯಾಗಮ fmt_first;
}

ಗಣನಾ {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_ಒಳಹರಿವು = 2,
    QF_NOMEM = 3,
    QF_IGNORE_ಸಾಲು = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*linebuf;
    ಪೂರ್ಣಾಙ್ಕ		linelen;
    char_u	*growbuf;
    ಪೂರ್ಣಾಙ್ಕ		growbufsiz;
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
    ಸ್ಥಿರ char_u *
qf_grow_linebuf(qfstate_T *state, ಪೂರ್ಣಾಙ್ಕ newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > ಸಾಲು_MAXLEN ? ಸಾಲು_MAXLEN - 1 : newsz;
    ಯದಿ (state->growbuf == ರಿಕ್ತ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	ಯದಿ (state->growbuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	state->growbufsiz = state->linelen;
    }
    ಅನ್ಯಥಾ ಯದಿ (state->linelen > state->growbufsiz)
    {
	ಯದಿ ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    ಪ್ರತ್ಯಾಗಮ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_next_str_ಸಾಲು(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (*p_str == NUL) // Reached the end of the string
	ಪ್ರತ್ಯಾಗಮ QF_END_OF_ಒಳಹರಿವು;

    p = vim_strchr(p_str, '\n');
    ಯದಿ (p != ರಿಕ್ತ)
	len = (ಪೂರ್ಣಾಙ್ಕ)(p - p_str) + 1;
    ಅನ್ಯಥಾ
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_str);

    ಯದಿ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಯದಿ (state->linebuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
    }
    ಅನ್ಯಥಾ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_next_list_ಸಾಲು(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯಾವತ್ (p_li != ರಿಕ್ತ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ರಿಕ್ತ))
	p_li = p_li->li_next;	// Skip non-string items

    ಯದಿ (p_li == ರಿಕ್ತ)		// End of the list
    {
	state->p_li = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ QF_END_OF_ಒಳಹರಿವು;
    }

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_li->li_tv.vval.v_string);
    ಯದಿ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಯದಿ (state->linebuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
    }
    ಅನ್ಯಥಾ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_next_buf_ಸಾಲು(qfstate_T *state)
{
    char_u	*p_buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Get the next line from the supplied buffer
    ಯದಿ (state->buflnum > state->lnumlast)
	ಪ್ರತ್ಯಾಗಮ QF_END_OF_ಒಳಹರಿವು;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    ಯದಿ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಯದಿ (state->linebuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
    }
    ಅನ್ಯಥಾ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_next_file_ಸಾಲು(qfstate_T *state)
{
    ಪೂರ್ಣಾಙ್ಕ	    discard;
    ಪೂರ್ಣಾಙ್ಕ	    growbuflen;

    ಯದಿ (fgets((ವರ್ಣ *)IObuff, IOSIZE, state->fd) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_END_OF_ಒಳಹರಿವು;

    discard = FALSE;
    state->linelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(IObuff);
    ಯದಿ (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	ಯದಿ (state->growbuf == ರಿಕ್ತ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    ಯದಿ (state->growbuf == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	ಪ್ರತಿ (;;)
	{
	    char_u	*p;

	    ಯದಿ (fgets((ವರ್ಣ *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    state->linelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    ಯದಿ ((state->growbuf)[growbuflen - 1] == '\n')
		ಭಙ್ಗ;
	    ಯದಿ (state->growbufsiz == ಸಾಲು_MAXLEN)
	    {
		discard = TRUE;
		ಭಙ್ಗ;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < ಸಾಲು_MAXLEN
		? 2 * state->growbufsiz : ಸಾಲು_MAXLEN;
	    ಯದಿ ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
	    state->growbuf = p;
	}

	ಯಾವತ್ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    ಯದಿ (fgets((ವರ್ಣ *)IObuff, IOSIZE, state->fd) == ರಿಕ್ತ
		    || (ಪೂರ್ಣಾಙ್ಕ)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ಭಙ್ಗ;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    ಅನ್ಯಥಾ
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    ಯದಿ (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*ಸಾಲು;

	ಸಾಲು = string_convert(&state->vc, state->linebuf, &state->linelen);
	ಯದಿ (ಸಾಲು != ರಿಕ್ತ)
	{
	    ಯದಿ (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, ಸಾಲು);
		vim_free(ಸಾಲು);
	    }
	    ಅನ್ಯಥಾ
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = ಸಾಲು;
		state->growbufsiz = state->linelen < ಸಾಲು_MAXLEN
						? state->linelen : ಸಾಲು_MAXLEN;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_nextline(qfstate_T *state)
{
    ಪೂರ್ಣಾಙ್ಕ status = QF_FAIL;

    ಯದಿ (state->fd == ರಿಕ್ತ)
    {
	ಯದಿ (state->tv != ರಿಕ್ತ)
	{
	    ಯದಿ (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_ಸಾಲು(state);
	    ಅನ್ಯಥಾ ಯದಿ (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_ಸಾಲು(state);
	}
	ಅನ್ಯಥಾ
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_ಸಾಲು(state);
    }
    ಅನ್ಯಥಾ
	// Get the next line from the supplied file
	status = qf_get_next_file_ಸಾಲು(state);

    ಯದಿ (status != QF_OK)
	ಪ್ರತ್ಯಾಗಮ status;

    // remove newline/CR from the line
    ಯದಿ (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	ಯದಿ (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*namebuf;
    ಪೂರ್ಣಾಙ್ಕ		bnr;
    char_u	*module;
    char_u	*errmsg;
    ಪೂರ್ಣಾಙ್ಕ		errmsglen;
    long	lnum;
    long	end_lnum;
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    ಪೂರ್ಣಾಙ್ಕ		enr;
    ಪೂರ್ಣಾಙ್ಕ		type;
    typval_T	*user_data;
    ಪೂರ್ಣಾಙ್ಕ		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_f(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields, ಪೂರ್ಣಾಙ್ಕ prefix)
{
    ಪೂರ್ಣಾಙ್ಕ c;

    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ || rmp->endp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    ಯದಿ (vim_strchr((char_u *)"OPQ", prefix) != ರಿಕ್ತ
	    && mch_getperm(fields->namebuf) == -1)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_b(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    ಪೂರ್ಣಾಙ್ಕ bnr = (ಪೂರ್ಣಾಙ್ಕ)atol((ವರ್ಣ *)rmp->startp[midx]);
    ಯದಿ (buflist_findnr(bnr) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->bnr = bnr;
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_n(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->enr = (ಪೂರ್ಣಾಙ್ಕ)atol((ವರ್ಣ *)rmp->startp[midx]);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_l(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->lnum = atol((ವರ್ಣ *)rmp->startp[midx]);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_e(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->end_lnum = atol((ವರ್ಣ *)rmp->startp[midx]);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_c(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->col = (ಪೂರ್ಣಾಙ್ಕ)atol((ವರ್ಣ *)rmp->startp[midx]);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_k(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->end_col = (ಪೂರ್ಣಾಙ್ಕ)atol((ವರ್ಣ *)rmp->startp[midx]);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_t(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->type = *rmp->startp[midx];
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_nonerror_ಸಾಲು(char_u *linebuf, ಪೂರ್ಣಾಙ್ಕ linelen, qffields_T *fields)
{
    char_u	*p;

    ಯದಿ (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	ಯದಿ ((p = vim_realloc(fields->errmsg, linelen + 1)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_m(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ || rmp->endp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    len = (ಪೂರ್ಣಾಙ್ಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಯದಿ (len >= fields->errmsglen)
    {
	// len + null terminator
	ಯದಿ ((p = vim_realloc(fields->errmsg, len + 1)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_r(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, char_u **tail)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    *tail = rmp->startp[midx];
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_p(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    char_u	*match_ptr;

    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ || rmp->endp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->col = 0;
    ಪ್ರತಿ (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	ಯದಿ (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_v(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    fields->col = (ಪೂರ್ಣಾಙ್ಕ)atol((ವರ್ಣ *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_s(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ || rmp->endp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    len = (ಪೂರ್ಣಾಙ್ಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಯದಿ (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_fmt_o(regmatch_T *rmp, ಪೂರ್ಣಾಙ್ಕ midx, qffields_T *fields)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (rmp->startp[midx] == ರಿಕ್ತ || rmp->endp[midx] == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    len = (ಪೂರ್ಣಾಙ್ಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಯದಿ (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, ಪೂರ್ಣಾಙ್ಕ, qffields_T *) =
{
    ರಿಕ್ತ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ರಿಕ್ತ, // %r
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_match(
	char_u		*linebuf,
	ಪೂರ್ಣಾಙ್ಕ		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	ಪೂರ್ಣಾಙ್ಕ		qf_multiline,
	ಪೂರ್ಣಾಙ್ಕ		qf_multiscan,
	char_u		**tail)
{
    ಪೂರ್ಣಾಙ್ಕ		idx = fmt_ptr->prefix;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		midx;
    ಪೂರ್ಣಾಙ್ಕ		status;

    ಯದಿ ((idx == 'C' || idx == 'Z') && !qf_multiline)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    ಯದಿ (vim_strchr((char_u *)"EWIN", idx) != ರಿಕ್ತ)
	fields->type = idx;
    ಅನ್ಯಥಾ
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    ಪ್ರತಿ (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (ಪೂರ್ಣಾಙ್ಕ)fmt_ptr->addr[i];
	ಯದಿ (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	ಅನ್ಯಥಾ ಯದಿ (i == FMT_PATTERN_M)
	{
	    ಯದಿ (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_ಸಾಲು(linebuf, linelen, fields);
	    ಅನ್ಯಥಾ ಯದಿ (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	ಅನ್ಯಥಾ ಯದಿ (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	ಅನ್ಯಥಾ ಯದಿ (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	ಯದಿ (status != QF_OK)
	    ಪ್ರತ್ಯಾಗಮ status;
    }

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_get_fields(
	char_u		*linebuf,
	ಪೂರ್ಣಾಙ್ಕ		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	ಪೂರ್ಣಾಙ್ಕ		qf_multiline,
	ಪೂರ್ಣಾಙ್ಕ		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		status = QF_FAIL;
    ಪೂರ್ಣಾಙ್ಕ		r;

    ಯದಿ (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    ಯದಿ (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = ರಿಕ್ತ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    ಯದಿ (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_dir_pfx(ಪೂರ್ಣಾಙ್ಕ idx, qffields_T *fields, qf_list_T *qfl)
{
    ಯದಿ (idx == 'D')				// enter directory
    {
	ಯದಿ (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    ಪ್ರತ್ಯಾಗಮ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ತಳ್ಳು_dir(fields->namebuf, &qfl->qf_dir_ರಾಶಿ, FALSE);
	ಯದಿ (qfl->qf_directory == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (idx == 'X')			// leave directory
	qfl->qf_directory = qf_ತೆಗೆ_dir(&qfl->qf_dir_ರಾಶಿ);

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_file_pfx(
	ಪೂರ್ಣಾಙ್ಕ idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    ಯದಿ (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	ಯದಿ (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ತಳ್ಳು_dir(fields->namebuf, &qfl->qf_file_ರಾಶಿ, TRUE);
	ಅನ್ಯಥಾ ಯದಿ (idx == 'Q')
	    qfl->qf_currfile = qf_ತೆಗೆ_dir(&qfl->qf_file_ರಾಶಿ);
	*fields->namebuf = NUL;
	ಯದಿ (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    ಪ್ರತ್ಯಾಗಮ QF_MULTISCAN;
	}
    }

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_ಸಾಲು_nomatch(char_u *linebuf, ಪೂರ್ಣಾಙ್ಕ linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    ಪ್ರತ್ಯಾಗಮ copy_nonerror_ಸಾಲು(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_multiline_pfx(
	ಪೂರ್ಣಾಙ್ಕ idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    ಪೂರ್ಣಾಙ್ಕ			len;

    ಯದಿ (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	ಯದಿ (qfprev == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ QF_FAIL;
	ಯದಿ (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    ಯದಿ (ptr == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	ಯದಿ (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	ಯದಿ (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	ಯದಿ (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	ಯದಿ (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	ಯದಿ (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	ಯದಿ (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	ಯದಿ (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ರಿಕ್ತ
		    ? fields->namebuf
		    : qfl->qf_currfile != ರಿಕ್ತ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    ಯದಿ (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    ಸಾಲು_breakcheck();

    ಪ್ರತ್ಯಾಗಮ QF_IGNORE_ಸಾಲು;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_parse_ಸಾಲು(
	qf_list_T	*qfl,
	char_u		*linebuf,
	ಪೂರ್ಣಾಙ್ಕ		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    ಪೂರ್ಣಾಙ್ಕ			idx = 0;
    char_u		*tail = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			status;

restofline:
    // If there was no %> item start at the first pattern
    ಯದಿ (fmt_start == ರಿಕ್ತ)
	fmt_ptr = fmt_first;
    ಅನ್ಯಥಾ
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = ರಿಕ್ತ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    ಪ್ರತಿ ( ; fmt_ptr != ರಿಕ್ತ; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	ಯದಿ (status == QF_NOMEM)
	    ಪ್ರತ್ಯಾಗಮ status;
	ಯದಿ (status == QF_OK)
	    ಭಙ್ಗ;
    }
    qfl->qf_multiscan = FALSE;

    ಯದಿ (fmt_ptr == ರಿಕ್ತ || idx == 'D' || idx == 'X')
    {
	ಯದಿ (fmt_ptr != ರಿಕ್ತ)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    ಯದಿ (status != QF_OK)
		ಪ್ರತ್ಯಾಗಮ status;
	}

	status = qf_parse_ಸಾಲು_nomatch(linebuf, linelen, fields);
	ಯದಿ (status != QF_OK)
	    ಪ್ರತ್ಯಾಗಮ status;

	ಯದಿ (fmt_ptr == ರಿಕ್ತ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (fmt_ptr != ರಿಕ್ತ)
    {
	// honor %> item
	ಯದಿ (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	ಯದಿ (vim_strchr((char_u *)"AEWIN", idx) != ರಿಕ್ತ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"CZ", idx) != ರಿಕ್ತ)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    ಯದಿ (status != QF_OK)
		ಪ್ರತ್ಯಾಗಮ status;
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"OPQ", idx) != ರಿಕ್ತ)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    ಯದಿ (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		ಪೂರ್ಣಾಙ್ಕ new_linelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);
		ಯದಿ (new_linelen >= linelen)
		    ಪ್ರತ್ಯಾಗಮ QF_IGNORE_ಸಾಲು;
		linebuf = s;
		linelen = new_linelen;
		ಗಚ್ಛ restofline;
	    }
	}
	ಯದಿ (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    ಯದಿ (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    ಪ್ರತ್ಯಾಗಮ QF_IGNORE_ಸಾಲು;
	}
    }

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_ರಾಶಿ_empty(qf_info_T *qi)
{
    ಪ್ರತ್ಯಾಗಮ qi == ರಿಕ್ತ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_list_empty(qf_list_T *qfl)
{
    ಪ್ರತ್ಯಾಗಮ qfl == ರಿಕ್ತ || qfl->qf_ಎಣಿಕೆ <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_list_has_valid_entries(qf_list_T *qfl)
{
    ಪ್ರತ್ಯಾಗಮ !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ಸ್ಥಿರ qf_list_T *
qf_get_list(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    ಯದಿ (pfields->namebuf == ರಿಕ್ತ || pfields->errmsg == ರಿಕ್ತ
		|| pfields->pattern == ರಿಕ್ತ || pfields->module == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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
    ಯದಿ (enc != ರಿಕ್ತ && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    ಯದಿ (efile != ರಿಕ್ತ && (pstate->fd = mch_fopen((ವರ್ಣ *)efile, "r")) == ರಿಕ್ತ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (tv != ರಿಕ್ತ)
    {
	ಯದಿ (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_cleanup_state(qfstate_T *pstate)
{
    ಯದಿ (pstate->fd != ರಿಕ್ತ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    ಯದಿ (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    ಪೂರ್ಣಾಙ್ಕ		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    ಯದಿ (status != QF_OK)
	ಪ್ರತ್ಯಾಗಮ status;

    status = qf_parse_ಸಾಲು(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    ಯದಿ (status != QF_OK)
	ಪ್ರತ್ಯಾಗಮ status;

    ಪ್ರತ್ಯಾಗಮ qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ರಿಕ್ತ)
		? fields->namebuf
		: ((qfl->qf_currfile != ರಿಕ್ತ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ರಿಕ್ತ),
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_init_ext(
    qf_info_T	    *qi,
    ಪೂರ್ಣಾಙ್ಕ		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    ಪೂರ್ಣಾಙ್ಕ		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    adding = FALSE;
    ಸ್ಥಿರ efm_T    *fmt_first = ರಿಕ್ತ;
    char_u	    *efm;
    ಸ್ಥಿರ char_u   *last_efm = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    retval = -1;	// default: return error flag
    ಪೂರ್ಣಾಙ್ಕ		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    ಯದಿ ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	ಗಚ್ಛ qf_init_end;

    ಯದಿ (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ಅನ್ಯಥಾ
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	ಯದಿ (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    ಯದಿ (errorformat == p_efm && tv == ರಿಕ್ತ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    ಅನ್ಯಥಾ
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    ಯದಿ (last_efm == ರಿಕ್ತ || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	ಯದಿ (fmt_first != ರಿಕ್ತ)
	    last_efm = vim_strsave(efm);
    }

    ಯದಿ (fmt_first == ರಿಕ್ತ)	// nothing found
	ಗಚ್ಛ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    ಯಾವತ್ (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	ಯದಿ (status == QF_NOMEM)		// memory alloc failure
	    ಗಚ್ಛ qf_init_end;
	ಯದಿ (status == QF_END_OF_ಒಳಹರಿವು)	// end of input
	    ಭಙ್ಗ;
	ಯದಿ (status == QF_FAIL)
	    ಗಚ್ಛ error2;

	ಸಾಲು_breakcheck();
    }
    ಯದಿ (state.fd == ರಿಕ್ತ || !ferror(state.fd))
    {
	ಯದಿ (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    qfl->qf_nonevalid = FALSE;
	    ಯದಿ (qfl->qf_ptr == ರಿಕ್ತ)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_ಎಣಿಕೆ;
	ಗಚ್ಛ qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    ಯದಿ (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	ಯದಿ (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    ಯದಿ (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	ಪೂರ್ಣಾಙ್ಕ	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    ಯದಿ (wp != ರಿಕ್ತ)
	qi = ll_get_or_alloc_list(wp);
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ರಿಕ್ತ, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    ಯದಿ (title == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    ಯದಿ (p != ರಿಕ್ತ)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ಸ್ಥಿರ char_u *
qf_cmdtitle(char_u *cmd)
{
    ಸ್ಥಿರ char_u qftitle_str[IOSIZE];

    vim_snprintf((ವರ್ಣ *)qftitle_str, IOSIZE, ":%s", (ವರ್ಣ *)cmd);
    ಪ್ರತ್ಯಾಗಮ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ಸ್ಥಿರ qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    ಪ್ರತ್ಯಾಗಮ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ತೆಗೆ_ರಾಶಿ(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ adjust)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    qf_free(&qi->qf_lists[0]);
    ಪ್ರತಿ (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, ಪರಿಮಾಣ(*qi->qf_lists));

    ಯದಿ (adjust)
    {
	qi->qf_listcount--;
	ಯದಿ (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	ಅನ್ಯಥಾ
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    ಯಾವತ್ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    ಯದಿ (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_ತೆಗೆ_ರಾಶಿ(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    ಯದಿ (q == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_ರಾಶಿ_get_bufnr(ಶೂನ್ಯ)
{
    ಯದಿ (ql_info == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ INVALID_QFBUFNR;
    ಪ್ರತ್ಯಾಗಮ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    ಯದಿ (qi->qf_bufnr == INVALID_QFBUFNR)
	ಪ್ರತ್ಯಾಗಮ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    ಯದಿ (qfbuf != ರಿಕ್ತ && qfbuf->b_nwindows == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	ಯದಿ (curwin->w_buffer == ರಿಕ್ತ)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(ರಿಕ್ತ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	ಯದಿ (buf_was_null)
	    curwin->w_buffer = ರಿಕ್ತ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_list_ರಾಶಿ_ವಸ್ತುಗಳು(qf_info_T *qi)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_ರಾಶಿ_ವಸ್ತುಗಳು(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    *pqi = ರಿಕ್ತ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    ಯದಿ (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	ಪ್ರತ್ಯಾಗಮ;
    }

    qi->qf_refcount--;
    ಯದಿ (qi->qf_refcount < 1)
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
    ಶೂನ್ಯ
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ಯದಿ (wp != ರಿಕ್ತ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    ಅನ್ಯಥಾ ಯದಿ (qi != ರಿಕ್ತ)
	qf_free_list_ರಾಶಿ_ವಸ್ತುಗಳು(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
incr_quickfix_busy(ಶೂನ್ಯ)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
decr_quickfix_busy(ಶೂನ್ಯ)
{
    ಯದಿ (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	ಯಾವತ್ (qf_delq_head != ರಿಕ್ತ)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    ಯದಿ (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
check_quickfix_busy(ಶೂನ್ಯ)
{
    ಯದಿ (quickfix_busy != 0)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    ಪೂರ್ಣಾಙ್ಕ		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    ಪೂರ್ಣಾಙ್ಕ		col,		// column
    ಪೂರ್ಣಾಙ್ಕ		end_col,	// column for end
    ಪೂರ್ಣಾಙ್ಕ		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    ಪೂರ್ಣಾಙ್ಕ		nr,		// error number
    ಪೂರ್ಣಾಙ್ಕ		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    ಪೂರ್ಣಾಙ್ಕ		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ರಿಕ್ತ;
    char_u	*p = ರಿಕ್ತ;

    ಯದಿ ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    ಯದಿ (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	ಯದಿ (buf != ರಿಕ್ತ)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    ಅನ್ಯಥಾ
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    ಯದಿ (fname != ರಿಕ್ತ)
	fullname = fix_fname(fname);
    qfp->qf_fname = ರಿಕ್ತ;
    ಯದಿ (buf != ರಿಕ್ತ &&
	buf->b_ffname != ರಿಕ್ತ && fullname != ರಿಕ್ತ)
    {
	ಯದಿ (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    ಯದಿ (p != ರಿಕ್ತ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    ಯದಿ ((qfp->qf_text = vim_strsave(mesg)) == ರಿಕ್ತ)
    {
	vim_free(qfp);
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    ಯದಿ (user_data == ರಿಕ್ತ || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    ಅನ್ಯಥಾ
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    ಯದಿ (pattern == ರಿಕ್ತ || *pattern == NUL)
	qfp->qf_pattern = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ ((qfp->qf_pattern = vim_strsave(pattern)) == ರಿಕ್ತ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    }
    ಯದಿ (module == ರಿಕ್ತ || *module == NUL)
	qfp->qf_module = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ ((qfp->qf_module = vim_strsave(module)) == ರಿಕ್ತ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	ಪ್ರತ್ಯಾಗಮ QF_FAIL;
    }
    qfp->qf_nr = nr;
    ಯದಿ (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    ಯದಿ (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = ರಿಕ್ತ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_ಎಣಿಕೆ;
    ಯದಿ (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_ಎಣಿಕೆ;
	qfl->qf_ptr = qfp;
    }

    ಪ್ರತ್ಯಾಗಮ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_resize_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಯದಿ (ql_info == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (qf_resize_ರಾಶಿ_base(ql_info, n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    ಪೂರ್ಣಾಙ್ಕ
ll_resize_ರಾಶಿ(win_T *wp, ಪೂರ್ಣಾಙ್ಕ n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    ಯದಿ (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    ಅನ್ಯಥಾ
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (qf_resize_ರಾಶಿ_base(qi, n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_resize_ರಾಶಿ_base(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ n)
{
    qf_list_T *new;
    ಪೂರ್ಣಾಙ್ಕ amount_to_rm = 0, i;

    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    size_t lsz = ಪರಿಮಾಣ(*qi->qf_lists);

    ಯದಿ (n == qi->qf_maxcount)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಅನ್ಯಥಾ ಯದಿ (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	ಪ್ರತಿ (i = 0; i < amount_to_rm; i++)
	    qf_ತೆಗೆ_ರಾಶಿ(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    ಯದಿ (new == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // fill with zeroes any newly allocated memory
    ಯದಿ (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   ಶೂನ್ಯ
qf_init_ರಾಶಿ(ಶೂನ್ಯ)
{
    ql_info = qf_alloc_ರಾಶಿ(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    ಯದಿ (wp != ರಿಕ್ತ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    ಯದಿ (llw != ರಿಕ್ತ)
	FOR_ALL_WINDOWS(wp)
	    ಯದಿ (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		ಪ್ರತ್ಯಾಗಮ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ಸ್ಥಿರ qf_info_T *
qf_alloc_ರಾಶಿ(qfltype_T qfltype, ಪೂರ್ಣಾಙ್ಕ n)
{
    qf_info_T *qi;

    ಯದಿ (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    ಅನ್ಯಥಾ
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	ಯದಿ (qi == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ರಾಶಿ(n);
    ಯದಿ (qi->qf_lists == ರಿಕ್ತ)
    {
	ಯದಿ (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    qi->qf_maxcount = n;

    ಪ್ರತ್ಯಾಗಮ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ಸ್ಥಿರ qf_list_T *
qf_alloc_list_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಪ್ರತ್ಯಾಗಮ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ಸ್ಥಿರ qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    ಯದಿ (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	ಪ್ರತ್ಯಾಗಮ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    ಯದಿ (wp->w_llist == ರಿಕ್ತ)
	// new location list
	wp->w_llist = qf_alloc_ರಾಶಿ(QFLT_LOCATION, wp->w_p_lhi);

    ಪ್ರತ್ಯಾಗಮ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ಸ್ಥಿರ qf_info_T *
qf_cmd_get_ರಾಶಿ(exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ print_emsg)
{
    qf_info_T	*qi = ql_info;

    ಯದಿ (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	ಯದಿ (qi == ರಿಕ್ತ)
	{
	    ಯದಿ (print_emsg)
		emsg(_(e_no_location_list));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಯದಿ (qi == ರಿಕ್ತ && print_emsg)
	emsg(_(e_no_quickfix_ರಾಶಿ));

    ಪ್ರತ್ಯಾಗಮ qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ಸ್ಥಿರ qf_info_T *
qf_cmd_get_or_alloc_ರಾಶಿ(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    ಯದಿ (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	ಯದಿ (qi == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*pwinp = curwin;
    }

    ಪ್ರತ್ಯಾಗಮ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ವಸ್ತುಗಳು(from_qfl, from_qfp, i)
    {
	ಯದಿ (qf_add_entry(to_qfl,
		    ರಿಕ್ತ,
		    ರಿಕ್ತ,
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
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	ಯದಿ (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_ಎಣಿಕೆ = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = ರಿಕ್ತ;
    to_qfl->qf_last = ರಿಕ್ತ;
    to_qfl->qf_ptr = ರಿಕ್ತ;
    ಯದಿ (from_qfl->qf_title != ರಿಕ್ತ)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    ಅನ್ಯಥಾ
	to_qfl->qf_title = ರಿಕ್ತ;
    ಯದಿ (from_qfl->qf_ctx != ರಿಕ್ತ)
    {
	to_qfl->qf_ctx = alloc_tv();
	ಯದಿ (to_qfl->qf_ctx != ರಿಕ್ತ)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    ಅನ್ಯಥಾ
	to_qfl->qf_ctx = ರಿಕ್ತ;
    ಯದಿ (from_qfl->qf_qftf_cb.cb_name != ರಿಕ್ತ)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    ಅನ್ಯಥಾ
	to_qfl->qf_qftf_cb.cb_name = ರಿಕ್ತ;

    ಯದಿ (from_qfl->qf_ಎಣಿಕೆ)
	ಯದಿ (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    ಯದಿ (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    ಶೂನ್ಯ
copy_loclist_ರಾಶಿ(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    ಯದಿ (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    ಅನ್ಯಥಾ
	qi = from->w_llist;

    ಯದಿ (qi == ರಿಕ್ತ)		    // no location list to copy
	ಪ್ರತ್ಯಾಗಮ;

    // allocate a new location list, set size of stack to 'from' window value
    ಯದಿ ((to->w_llist = qf_alloc_ರಾಶಿ(QFLT_LOCATION, from->w_p_lhi)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    ಪ್ರತಿ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	ಯದಿ (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ರಿಕ್ತ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    ಯದಿ (fname == ರಿಕ್ತ || *fname == NUL)		// no file name
	ಪ್ರತ್ಯಾಗಮ 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    ಯದಿ (directory != ರಿಕ್ತ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    ಯದಿ (directory != ರಿಕ್ತ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ರಿಕ್ತ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	ಯದಿ (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    ಯದಿ (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    ಅನ್ಯಥಾ
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    ಯದಿ (ptr.string == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    ಅನ್ಯಥಾ
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    ಯದಿ (qf_last_bufname != ರಿಕ್ತ && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    ಅನ್ಯಥಾ
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, ರಿಕ್ತ, (linenr_T)0, BLN_NOOPT);
	ಯದಿ (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	ಅನ್ಯಥಾ
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    ಪ್ರತ್ಯಾಗಮ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ಸ್ಥಿರ char_u *
qf_ತಳ್ಳು_dir(char_u *dirbuf, ಸಂರಚನಾ dir_ರಾಶಿ_T **stackptr, ಪೂರ್ಣಾಙ್ಕ is_file_ರಾಶಿ)
{
    ಸಂರಚನಾ dir_ರಾಶಿ_T  *ds_new;
    ಸಂರಚನಾ dir_ರಾಶಿ_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ಸಂರಚನಾ dir_ರಾಶಿ_T, aid_qf_dirstack);
    ಯದಿ (ds_new == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    ಯದಿ (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == ರಿಕ್ತ
	    || is_file_ರಾಶಿ)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    ಅನ್ಯಥಾ
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = ರಿಕ್ತ;
	ಯಾವತ್ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    ಯದಿ (dirname.string == ರಿಕ್ತ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಯದಿ (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ಭಙ್ಗ;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	ಯಾವತ್ ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	ಯದಿ (ds_new == ರಿಕ್ತ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    ಯದಿ ((*stackptr)->dirname != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ಸ್ಥಿರ char_u *
qf_ತೆಗೆ_dir(ಸಂರಚನಾ dir_ರಾಶಿ_T **stackptr)
{
    ಸಂರಚನಾ dir_ರಾಶಿ_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    ಯದಿ (*stackptr != ರಿಕ್ತ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    ಪ್ರತ್ಯಾಗಮ *stackptr ? (*stackptr)->dirname : ರಿಕ್ತ;
}

/*
 * clean up directory stack
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_clean_dir_ರಾಶಿ(ಸಂರಚನಾ dir_ರಾಶಿ_T **stackptr)
{
    ಸಂರಚನಾ dir_ರಾಶಿ_T  *ds_ptr;

    ಯಾವತ್ ((ds_ptr = *stackptr) != ರಿಕ್ತ)
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
    ಸ್ಥಿರ char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ಸಂರಚನಾ dir_ರಾಶಿ_T     *ds_ptr;
    ಸಂರಚನಾ dir_ರಾಶಿ_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    ಯದಿ (qfl->qf_dir_ರಾಶಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ds_ptr = qfl->qf_dir_ರಾಶಿ->next;
    fullname.string = ರಿಕ್ತ;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    ಯಾವತ್ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	ಯದಿ ((fullname.string != ರಿಕ್ತ) && (mch_getperm(fullname.string) >= 0))
	    ಭಙ್ಗ;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    ಯಾವತ್ (qfl->qf_dir_ರಾಶಿ->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ರಾಶಿ->next;
	qfl->qf_dir_ರಾಶಿ->next = qfl->qf_dir_ರಾಶಿ->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    ಪ್ರತ್ಯಾಗಮ ds_ptr == ರಿಕ್ತ ? ರಿಕ್ತ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (wp != ರಿಕ್ತ)
    {
	ಯದಿ (!win_valid(wp))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (i = 0; i < qi->qf_listcount; ++i)
	ಯದಿ (qi->qf_lists[i].qf_id == qf_id)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
	ಯದಿ (qfp == qf_ptr)
	    ಭಙ್ಗ;

    ಯದಿ (i > qfl->qf_ಎಣಿಕೆ) // Entry is not found
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ಸ್ಥಿರ qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಙ್ಕ		*qf_index,
	ಪೂರ್ಣಾಙ್ಕ		dir)
{
    ಪೂರ್ಣಾಙ್ಕ			idx;
    ಪೂರ್ಣಾಙ್ಕ			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    ಕುರು
    {
	ಯದಿ (idx == qfl->qf_ಎಣಿಕೆ || qf_ptr->qf_next == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } ಯಾವತ್ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ಪ್ರತ್ಯಾಗಮ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ಸ್ಥಿರ qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಙ್ಕ		*qf_index,
	ಪೂರ್ಣಾಙ್ಕ		dir)
{
    ಪೂರ್ಣಾಙ್ಕ			idx;
    ಪೂರ್ಣಾಙ್ಕ			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    ಕುರು
    {
	ಯದಿ (idx == 1 || qf_ptr->qf_prev == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } ಯಾವತ್ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    ಪ್ರತ್ಯಾಗಮ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ಸ್ಥಿರ qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಙ್ಕ		errornr,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಙ್ಕ			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    ಪೂರ್ಣಾಙ್ಕ			prev_index;
    ವರ್ಣ		*err = e_no_more_ವಸ್ತುಗಳು;

    ಯಾವತ್ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	ಯದಿ (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	ಅನ್ಯಥಾ
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	ಯದಿ (qf_ptr == ರಿಕ್ತ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    ಯದಿ (err != ರಿಕ್ತ)
	    {
		emsg(_(err));
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ಭಙ್ಗ;
	}

	err = ರಿಕ್ತ;
    }

    *new_qfidx = qf_idx;
    ಪ್ರತ್ಯಾಗಮ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ಸ್ಥಿರ qfline_T *
get_nth_entry(qf_list_T *qfl, ಪೂರ್ಣಾಙ್ಕ errornr, ಪೂರ್ಣಾಙ್ಕ *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಙ್ಕ		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    ಯಾವತ್ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ರಿಕ್ತ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    ಯಾವತ್ (errornr > qf_idx && qf_idx < qfl->qf_ಎಣಿಕೆ &&
						qf_ptr->qf_next != ರಿಕ್ತ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    ಪ್ರತ್ಯಾಗಮ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ಸ್ಥಿರ qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಙ್ಕ		errornr,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಙ್ಕ		qfidx = qfl->qf_index;

    ಯದಿ (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    ಅನ್ಯಥಾ ಯದಿ (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    ಪ್ರತ್ಯಾಗಮ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ಸ್ಥಿರ win_T *
qf_find_help_win(ಶೂನ್ಯ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (bt_help(wp->w_buffer))
	    ಪ್ರತ್ಯಾಗಮ wp;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
jump_to_help_window(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ newwin, ಪೂರ್ಣಾಙ್ಕ *opened_window)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		flags;

    ಯದಿ (cmdmod.cmod_tab != 0 || newwin)
	wp = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	wp = qf_find_help_win();
    ಯದಿ (wp != ರಿಕ್ತ && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    ಅನ್ಯಥಾ
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	ಯದಿ (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_ತುದಿ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	ಯದಿ (IS_LL_ರಾಶಿ(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	ಯದಿ (win_split(0, flags) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	*opened_window = TRUE;

	ಯದಿ (curwin->w_height < p_hh)
	    win_setheight((ಪೂರ್ಣಾಙ್ಕ)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	ಯದಿ (IS_LL_ರಾಶಿ(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    ಯದಿ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ಸ್ಥಿರ win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    ಪ್ರತ್ಯಾಗಮ wp;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ಸ್ಥಿರ win_T *
qf_find_win_with_normal_buf(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (bt_normal(wp->w_buffer))
	    ಪ್ರತ್ಯಾಗಮ wp;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_goto_tabwin_with_file(ಪೂರ್ಣಾಙ್ಕ fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_open_new_file_win(qf_info_T *ll_ref)
{
    ಪೂರ್ಣಾಙ್ಕ		flags;

    flags = WSP_ABOVE;
    ಯದಿ (ll_ref != ರಿಕ್ತ)
	flags |= WSP_NEWLOC;
    ಯದಿ (win_split(0, flags) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    ಯದಿ (ll_ref != ರಿಕ್ತ)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_goto_win_with_ll_file(win_T *use_win, ಪೂರ್ಣಾಙ್ಕ qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    ಯದಿ (win == ರಿಕ್ತ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    ಯದಿ (win->w_buffer->b_fnum == qf_fnum)
		ಭಙ್ಗ;
	ಯದಿ (win == ರಿಕ್ತ)
	{
	    // Find a previous usable window
	    win = curwin;
	    ಕುರು
	    {
		ಯದಿ (bt_normal(win->w_buffer))
		    ಭಙ್ಗ;
		ಯದಿ (win->w_prev == ರಿಕ್ತ)
		    win = lastwin;	// wrap around the top
		ಅನ್ಯಥಾ
		    win = win->w_prev; // go to previous window
	    } ಯಾವತ್ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    ಯದಿ (win->w_llist == ರಿಕ್ತ && ll_ref != ರಿಕ್ತ)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_goto_win_with_qfl_file(ಪೂರ್ಣಾಙ್ಕ qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ರಿಕ್ತ;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (win->w_buffer->b_fnum == qf_fnum)
	    ಭಙ್ಗ;
	ಯದಿ (win->w_prev == ರಿಕ್ತ)
	    win = lastwin;	// wrap around the top
	ಅನ್ಯಥಾ
	    win = win->w_prev;	// go to previous window

	ಯದಿ (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    ಯದಿ ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    ಅನ್ಯಥಾ ಯದಿ (altwin != ರಿಕ್ತ)
		win = altwin;
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_prev != ರಿಕ್ತ)
		win = curwin->w_prev;
	    ಅನ್ಯಥಾ
		win = curwin->w_next;
	    ಭಙ್ಗ;
	}

	// Remember a usable window.
	ಯದಿ (altwin == ರಿಕ್ತ && !win->w_p_pvw && !win->w_p_wfb &&
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_jump_to_usable_window(ಪೂರ್ಣಾಙ್ಕ qf_fnum, ಪೂರ್ಣಾಙ್ಕ newwin, ಪೂರ್ಣಾಙ್ಕ *opened_window)
{
    win_T	*usable_wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		usable_win = FALSE;
    qf_info_T	*ll_ref = ರಿಕ್ತ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    ಯದಿ (!newwin)
	ll_ref = curwin->w_llist_ref;

    ಯದಿ (ll_ref != ರಿಕ್ತ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	ಯದಿ (usable_wp != ರಿಕ್ತ)
	    usable_win = TRUE;
    }

    ಯದಿ (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	ಯದಿ (win != ರಿಕ್ತ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    ಯದಿ (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    ಯದಿ ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	ಯದಿ (qf_open_new_file_win(ll_ref) != OK)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (curwin->w_llist_ref != ರಿಕ್ತ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	ಅನ್ಯಥಾ					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಙ್ಕ		forceit,
	ಪೂರ್ಣಾಙ್ಕ		prev_winid,
	ಪೂರ್ಣಾಙ್ಕ		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ಪೂರ್ಣಾಙ್ಕ		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		old_qf_curlist = qi->qf_curlist;
    ಪೂರ್ಣಾಙ್ಕ		save_qfid = qfl->qf_id;

    ಯದಿ (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	ಯದಿ (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	fnum = qf_ptr->qf_fnum;

	ಯದಿ (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    ಯದಿ (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    ಯದಿ (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		ಯದಿ (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		ಯದಿ (curwin->w_p_wfb)
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

	ಯದಿ (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    ಯದಿ (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	ಯದಿ (wp == ರಿಕ್ತ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    ಪ್ರತ್ಯಾಗಮ QF_ABORT;
	}
    }

    ಯದಿ (qfl_type == QFLT_QUICKFIX && !qflist_valid(ರಿಕ್ತ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	ಪ್ರತ್ಯಾಗಮ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    ಯದಿ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ಯದಿ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ಅನ್ಯಥಾ
	    emsg(_(e_current_location_list_was_changed));
	ಪ್ರತ್ಯಾಗಮ QF_ABORT;
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*ಸಾಲು = ml_get_curline();
    char_u	*p = ಸಾಲು;
    colnr_T	col = 0;

    ಯಾವತ್ (*p != NUL && col < vcol)
    {
	ಯದಿ (*p == TAB)
	    col += 8 - (col % 8);
	ಅನ್ಯಥಾ
	    col += ptr2cells(p);
	ಯದಿ (col > vcol)
	    ಭಙ್ಗ;
	MB_PTR_ADV(p);
    }
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_goto_ಸಾಲು(
	linenr_T	qf_lnum,
	ಪೂರ್ಣಾಙ್ಕ		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    ಯದಿ (qf_pattern == ರಿಕ್ತ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	ಯದಿ (i > 0)
	{
	    ಯದಿ (i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		i = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    curwin->w_cursor.lnum = i;
	}
	ಯದಿ (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    ಯದಿ (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    ಅನ್ಯಥಾ
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	ಅನ್ಯಥಾ
	    beginline(BL_WHITE | BL_FIX);
    }
    ಅನ್ಯಥಾ
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	ಯದಿ (!do_ಹುಡುಕು(ರಿಕ್ತ, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, ಹುಡುಕು_KEEP, ರಿಕ್ತ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_print_ಸಂದೇಶ(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		qf_index,
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
    ಯದಿ (!ಸಂದೇಶ_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_ಎಣಿಕೆ,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (ವರ್ಣ *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = ಸಂದೇಶ_scroll;
    ಯದಿ (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	ಸಂದೇಶ_scroll = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (!ಸಂದೇಶ_scrolled && shortmess(SHM_OVERALL))
	ಸಂದೇಶ_scroll = FALSE;
    ಸಂದೇಶ_attr_keep((ವರ್ಣ *)gap->ga_data, 0, TRUE);
    ಸಂದೇಶ_scroll = i;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಙ್ಕ		newwin,
	ಪೂರ್ಣಾಙ್ಕ		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ಪೂರ್ಣಾಙ್ಕ		old_changedtick = qfl->qf_changedtick;
    ಪೂರ್ಣಾಙ್ಕ		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    ಯದಿ (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	ಯದಿ (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ಯದಿ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ಅನ್ಯಥಾ
	    emsg(_(e_current_location_list_was_changed));
	ಪ್ರತ್ಯಾಗಮ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    ಯದಿ (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	ಯದಿ (qf_ptr->qf_fnum == 0)
	    ಪ್ರತ್ಯಾಗಮ NOTDONE;

	ಯದಿ (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	ಯದಿ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	ಅನ್ಯಥಾ
	    emsg(_(e_current_location_list_was_changed));
	ಪ್ರತ್ಯಾಗಮ QF_ABORT;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_jump_to_buffer(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		qf_index,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಙ್ಕ		forceit,
	ಪೂರ್ಣಾಙ್ಕ		prev_winid,
	ಪೂರ್ಣಾಙ್ಕ		*opened_window,
	ಪೂರ್ಣಾಙ್ಕ		openfold,
	ಪೂರ್ಣಾಙ್ಕ		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    ಯದಿ (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	ಯದಿ (retval != OK)
	    ಪ್ರತ್ಯಾಗಮ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    ಯದಿ (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_ಸಾಲು(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    ಯದಿ (print_message)
	qf_jump_print_ಸಂದೇಶ(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    ಶೂನ್ಯ
qf_jump(qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		errornr,
	ಪೂರ್ಣಾಙ್ಕ		forceit)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_newwin(qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		errornr,
	ಪೂರ್ಣಾಙ್ಕ		forceit,
	ಪೂರ್ಣಾಙ್ಕ		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    ಪೂರ್ಣಾಙ್ಕ			qf_index;
    ಪೂರ್ಣಾಙ್ಕ			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    ಪೂರ್ಣಾಙ್ಕ			prev_winid;
    ಪೂರ್ಣಾಙ್ಕ			opened_window = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			print_message = TRUE;
    ಪೂರ್ಣಾಙ್ಕ			old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed; // getting file may reset it
    ಪೂರ್ಣಾಙ್ಕ			retval = OK;

    ಯದಿ (qi == ರಿಕ್ತ)
    {
	ಯದಿ (ql_info == ರಿಕ್ತ)
	{
	    emsg(_(e_no_quickfix_ರಾಶಿ));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	qi = ql_info;
    }

    ಯದಿ (qf_ರಾಶಿ_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಪ್ರತ್ಯಾಗಮ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    ಯದಿ (qf_ptr == ರಿಕ್ತ)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	ಗಚ್ಛ theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    ಯದಿ (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    ಯದಿ (retval == FAIL)
	ಗಚ್ಛ failed;
    ಯದಿ (retval == QF_ABORT)
    {
	qi = ರಿಕ್ತ;
	qf_ptr = ರಿಕ್ತ;
	ಗಚ್ಛ theend;
    }
    ಯದಿ (retval == NOTDONE)
	ಗಚ್ಛ theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_ಕೀಲಿ_Typed, print_message);
    ಯದಿ (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ರಿಕ್ತ;
	qf_ptr = ರಿಕ್ತ;
    }

    ಯದಿ (retval != OK)
    {
	ಯದಿ (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	ಯದಿ (qf_ptr != ರಿಕ್ತ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    ಯದಿ (qi != ರಿಕ್ತ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    ಯದಿ (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qfFileAttr;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qfSepAttr;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	qf_ಸಾಲು_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_list_entry(qfline_T *qfp, ಪೂರ್ಣಾಙ್ಕ qf_idx, ಪೂರ್ಣಾಙ್ಕ cursel)
{
    char_u	*fname;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		filter_entry;
    garray_T	*gap;

    fname = ರಿಕ್ತ;
    ಯದಿ (qfp->qf_module != ರಿಕ್ತ && *qfp->qf_module != NUL)
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(ವರ್ಣ *)qfp->qf_module);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ರಿಕ್ತ)
	{
	    ಯದಿ (qfp->qf_fname == ರಿಕ್ತ)
		fname = buf->b_fname;
	    ಅನ್ಯಥಾ
		fname = qfp->qf_fname;
	    ಯದಿ (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	ಯದಿ (fname == ರಿಕ್ತ)
	    sprintf((ವರ್ಣ *)IObuff, "%2d", qf_idx);
	ಅನ್ಯಥಾ
	    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (ವರ್ಣ *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    ಯದಿ (qfp->qf_module != ರಿಕ್ತ && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    ಯದಿ (filter_entry && fname != ರಿಕ್ತ)
	filter_entry &= message_filtered(fname);
    ಯದಿ (filter_entry && qfp->qf_pattern != ರಿಕ್ತ)
	filter_entry &= message_filtered(qfp->qf_pattern);
    ಯದಿ (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    ಯದಿ (filter_entry)
	ಪ್ರತ್ಯಾಗಮ;

    ಸಂದೇಶ_putchar('\n');
    ಸಂದೇಶ_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    ಯದಿ (qfp->qf_lnum != 0)
	ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    ಯದಿ (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    ಸಂದೇಶ_puts_attr((ವರ್ಣ *)gap->ga_data, qf_ಸಾಲು_Attr);
    ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    ಯದಿ (qfp->qf_pattern != ರಿಕ್ತ)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	ಸಂದೇಶ_puts((ವರ್ಣ *)gap->ga_data);
	ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    }
    ಸಂದೇಶ_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != ರಿಕ್ತ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    ಸಂದೇಶ_prt_ಸಾಲು((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    ಶೂನ್ಯ
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		idx1 = 1;
    ಪೂರ್ಣಾಙ್ಕ		idx2 = -1;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		plus = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (qf_ರಾಶಿ_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    ಯದಿ (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    qfl = qf_get_curlist(qi);
    ಯದಿ (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    ಅನ್ಯಥಾ
    {
	i = qfl->qf_ಎಣಿಕೆ;
	ಯದಿ (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	ಯದಿ (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    ಯದಿ (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    ಯದಿ (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_ಸಾಲು_Attr = syn_name2attr((char_u *)"qfLineNr");
    ಯದಿ (qf_ಸಾಲು_Attr == 0)
	qf_ಸಾಲು_Attr = HL_ATTR(HLF_N);

    ಯದಿ (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಯದಿ ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (*p == '\n')
	{
	    ga_append(gap, ' ');
	    ಯಾವತ್ (*++p != NUL)
		ಯದಿ (!VIM_ISWHITE(*p) && *p != '\n')
		    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((ವರ್ಣ *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    ಯದಿ (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((ವರ್ಣ *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    ಯದಿ (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((ವರ್ಣ *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	ಯದಿ (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((ವರ್ಣ *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ಸಂದೇಶ(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ which, ವರ್ಣ *lead)
{
    ವರ್ಣ   *title = (ವರ್ಣ *)qi->qf_lists[which].qf_title;
    ಪೂರ್ಣಾಙ್ಕ    ಎಣಿಕೆ = qi->qf_lists[which].qf_ಎಣಿಕೆ;
    char_u buf[IOSIZE];

    vim_snprintf((ವರ್ಣ *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    ಎಣಿಕೆ);

    ಯದಿ (title != ರಿಕ್ತ)
    {
	size_t	len = STRLEN(buf);

	ಯದಿ (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    ಸಂದೇಶ((ವರ್ಣ *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    ಶೂನ್ಯ
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->addr_ಎಣಿಕೆ != 0)
	ಎಣಿಕೆ = eap->line2;
    ಅನ್ಯಥಾ
	ಎಣಿಕೆ = 1;
    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	ಯದಿ (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    ಯದಿ (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_ರಾಶಿ));
		ಭಙ್ಗ;
	    }
	    --qi->qf_curlist;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_ತುದಿ_of_quickfix_ರಾಶಿ));
		ಭಙ್ಗ;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_ಸಂದೇಶ(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, ರಿಕ್ತ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    ಶೂನ್ಯ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ರಾಶಿ(eap, FALSE);
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಯದಿ (qi == ರಿಕ್ತ)
	{
	    emsg(_(e_no_location_list));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// Jump to the specified quickfix list
	ಯದಿ (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_ಸಂದೇಶ(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	    emsg(_(e_invalid_range));

	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (qf_ರಾಶಿ_empty(qi))
	ಸಂದೇಶ(_("No entries"));
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (i = 0; i < qi->qf_listcount; ++i)
	    qf_ಸಂದೇಶ(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_ವಸ್ತುಗಳು(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    ಪೂರ್ಣಾಙ್ಕ		stop = FALSE;

    ಯಾವತ್ (qfl->qf_ಎಣಿಕೆ && qfl->qf_start != ರಿಕ್ತ)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	ಯದಿ (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    ಯದಿ (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_ಎಣಿಕೆ = 1;
	    ಅನ್ಯಥಾ
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_ಎಣಿಕೆ;
    }

    qfl->qf_index = 0;
    qfl->qf_start = ರಿಕ್ತ;
    qfl->qf_last = ರಿಕ್ತ;
    qfl->qf_ptr = ರಿಕ್ತ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ರಾಶಿ(&qfl->qf_dir_ರಾಶಿ);
    qfl->qf_directory = ರಿಕ್ತ;
    qf_clean_dir_ರಾಶಿ(&qfl->qf_file_ರಾಶಿ);
    qfl->qf_currfile = ರಿಕ್ತ;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free(qf_list_T *qfl)
{
    qf_free_ವಸ್ತುಗಳು(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = ರಿಕ್ತ;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   ಶೂನ್ಯ
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಙ್ಕ		found_one = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		buf_has_flag = wp == ರಿಕ್ತ ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    ಯದಿ (!(curbuf->b_has_qf_entry & buf_has_flag))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	ಯದಿ (wp->w_llist == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	qi = wp->w_llist;
    }
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	ಯದಿ (!qf_list_empty(qfl))
	    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
		ಯದಿ (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    ಯದಿ (qfp->qf_cleared)
			ಅನುವರ್ತ;

		    ಯದಿ (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			ಯದಿ (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			ಅನ್ಯಥಾ
			    qfp->qf_lnum += amount;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    ಯದಿ (!found_one)
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
    ಸ್ಥಿರ char_u *
qf_types(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಸ್ಥಿರ char_u	buf[20];
    ಸ್ಥಿರ char_u	cc[3];
    char_u		*p;

    ಯದಿ (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    ಅನ್ಯಥಾ ಯದಿ (c == 'I' || c == 'i')
	p = (char_u *)" info";
    ಅನ್ಯಥಾ ಯದಿ (c == 'N' || c == 'n')
	p = (char_u *)" note";
    ಅನ್ಯಥಾ ಯದಿ (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    ಅನ್ಯಥಾ ಯದಿ (c == 0 || c == 1)
	p = (char_u *)"";
    ಅನ್ಯಥಾ
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    ಯದಿ (nr <= 0)
	ಪ್ರತ್ಯಾಗಮ p;

    sprintf((ವರ್ಣ *)buf, "%s %3d", (ವರ್ಣ *)p, nr);
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    ಶೂನ್ಯ
qf_view_result(ಪೂರ್ಣಾಙ್ಕ split)
{
    qf_info_T   *qi = ql_info;

    ಯದಿ (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
    {
	emsg(_(e_no_quickfix_ರಾಶಿ));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	ಪ್ರತ್ಯಾಗಮ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    ಶೂನ್ಯ
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    ಯದಿ (qf_ರಾಶಿ_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	ಯದಿ (win != ರಿಕ್ತ)
	    ex_cclose(eap);
    }
    ಅನ್ಯಥಾ ಯದಿ (win == ರಿಕ್ತ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    ಶೂನ್ಯ
ex_cclose(exarg_T *eap)
{
    win_T	*win = ರಿಕ್ತ;
    qf_info_T	*qi;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    ಯದಿ (win != ರಿಕ್ತ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_set_title_var(qf_list_T *qfl)
{
    ಯದಿ (qfl->qf_title != ರಿಕ್ತ)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_goto_cwindow(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ resize, ಪೂರ್ಣಾಙ್ಕ sz, ಪೂರ್ಣಾಙ್ಕ vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    ಯದಿ (win == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    win_goto(win);
    ಯದಿ (resize)
    {
	ಯದಿ (vertsplit)
	{
	    ಯದಿ (sz != win->w_width)
		win_setwidth(sz);
	}
	ಅನ್ಯಥಾ ಯದಿ (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_set_cwindow_options(ಶೂನ್ಯ)
{
    // switch off 'swapfile'
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"swf", 0L, ರಿಕ್ತ, OPT_LOCAL);
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_open_new_cwindow(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    ಯದಿ (IS_QF_ರಾಶಿ(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    ಯದಿ (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    ಯದಿ (IS_QF_ರಾಶಿ(qi))
	flags |= WSP_QUICKFIX;

    ಯದಿ (win_split(height, flags) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    ಯದಿ (IS_LL_ರಾಶಿ(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    ಯದಿ (oldwin != curwin)
	oldwin = ರಿಕ್ತ;  // don't store info when in another window
    ಯದಿ (qf_buf != ರಿಕ್ತ)
    {
	// Use the existing quickfix buffer
	ಯದಿ (do_ecmd(qf_buf->b_fnum, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	// Create a new quickfix buffer
	ಯದಿ (do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    ಯದಿ (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    ಯದಿ (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    ಯದಿ (win_valid(win))
	prevwin = win;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    ಶೂನ್ಯ
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ಪೂರ್ಣಾಙ್ಕ		height;
    ಪೂರ್ಣಾಙ್ಕ		status = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		lnum;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    incr_quickfix_busy();

    ಯದಿ (eap->addr_ಎಣಿಕೆ != 0)
	height = eap->line2;
    ಅನ್ಯಥಾ
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    ಯದಿ (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_ಎಣಿಕೆ != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    ಯದಿ (status == FAIL)
	ಯದಿ (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    ಪ್ರತ್ಯಾಗಮ;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, ರಿಕ್ತ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಶೂನ್ಯ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    win = qf_find_win(qi);
    ಯದಿ (win != ರಿಕ್ತ && win->w_cursor.lnum != win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	qf_win_goto(win, win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ಯದಿ (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_win_pos_update(
    qf_info_T	*qi,
    ಪೂರ್ಣಾಙ್ಕ		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    ಪೂರ್ಣಾಙ್ಕ		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    ಯದಿ (win != ರಿಕ್ತ
	    && qf_index <= win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && old_qf_index != qf_index)
    {
	ಯದಿ (qf_index > old_qf_index)
	{
	    win->w_redraw_ತುದಿ = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	ಅನ್ಯಥಾ
	{
	    win->w_redraw_ತುದಿ = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    ಪ್ರತ್ಯಾಗಮ win != ರಿಕ್ತ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    ಯದಿ (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	ಯದಿ ((IS_QF_ರಾಶಿ(qi) && win->w_llist_ref == ರಿಕ್ತ)
		|| (IS_LL_ರಾಶಿ(qi) && win->w_llist_ref == qi))
	    ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ಸ್ಥಿರ win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ಯದಿ (is_qf_win(win, qi))
	    ಪ್ರತ್ಯಾಗಮ win;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ಸ್ಥಿರ buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    ಯದಿ (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	ಯದಿ (qfbuf != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	ಯದಿ (is_qf_win(win, qi))
	    ಪ್ರತ್ಯಾಗಮ win->w_buffer;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    ವರ್ಣ *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    ಯದಿ (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ಯದಿ (is_qf_win(win, qi))
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
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    linenr_T	old_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		qf_winid = 0;

    ಯದಿ (IS_LL_ರಾಶಿ(qi))
    {
	ಯದಿ (curwin->w_llist == qi)
	    win = curwin;
	ಅನ್ಯಥಾ
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    ಯದಿ (win == ರಿಕ್ತ)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    ಯದಿ (win == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    ಪೂರ್ಣಾಙ್ಕ do_fill = TRUE;
    ಯದಿ (old_last == ರಿಕ್ತ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    ಯದಿ (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	ಯದಿ (old_last == ರಿಕ್ತ)
	{
	    (ಶೂನ್ಯ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    ಯದಿ ((win = qf_find_win(qi)) != ರಿಕ್ತ && old_ಸಾಲು_ಎಣಿಕೆ < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_buf_add_ಸಾಲು(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	ಪೂರ್ಣಾಙ್ಕ		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    ಯದಿ (qftf_str != ರಿಕ್ತ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (qfp->qf_module != ರಿಕ್ತ)
	    ga_concat(gap, qfp->qf_module);
	ಅನ್ಯಥಾ ಯದಿ (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ರಿಕ್ತ
		&& errbuf->b_fname != ರಿಕ್ತ)
	{
	    ಯದಿ (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    ಅನ್ಯಥಾ
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		ಯದಿ (first_bufline && (errbuf->b_sfname == ರಿಕ್ತ
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    ಯದಿ (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		ಯದಿ (qfp->qf_fname == ರಿಕ್ತ)
		    ga_concat(gap, errbuf->b_fname);
		ಅನ್ಯಥಾ
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	ಯದಿ (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	ಅನ್ಯಥಾ ಯದಿ (qfp->qf_pattern != ರಿಕ್ತ)
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
    ಯದಿ (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ಸ್ಥಿರ list_T *
call_qftf_func(qf_list_T *qfl, ಪೂರ್ಣಾಙ್ಕ qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = FALSE;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    ಯದಿ (qfl->qf_qftf_cb.cb_name != ರಿಕ್ತ)
	cb = &qfl->qf_qftf_cb;
    ಯದಿ (cb->cb_name != ರಿಕ್ತ)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	ಯದಿ ((d = dict_alloc_lock(VAR_FIXED)) == ರಿಕ್ತ)
	{
	    recursive = FALSE;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	dict_add_ಸಂಖ್ಯೆ(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_ಸಂಖ್ಯೆ(d, "winid", (long)qf_winid);
	dict_add_ಸಂಖ್ಯೆ(d, "id", (long)qfl->qf_id);
	dict_add_ಸಂಖ್ಯೆ(d, "start_idx", start_idx);
	dict_add_ಸಂಖ್ಯೆ(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = ರಿಕ್ತ;
	textlock++;
	ಯದಿ (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    ಯದಿ (rettv.v_type == VAR_LIST)
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
    ಪ್ರತ್ಯಾಗಮ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, ಪೂರ್ಣಾಙ್ಕ qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    list_T	*qftf_list = ರಿಕ್ತ;
    listitem_T	*qftf_li = ರಿಕ್ತ;

    ಯದಿ (old_last == ರಿಕ್ತ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	ಯದಿ (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	ಯಾವತ್ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (ಶೂನ್ಯ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    ಯದಿ (qfl != ರಿಕ್ತ && qfl->qf_start != ರಿಕ್ತ)
    {
	char_u		dirname[MAXPATHL];
	ಪೂರ್ಣಾಙ್ಕ		invalid_val = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	ಯದಿ (old_last == ರಿಕ್ತ)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (old_last->qf_next != ರಿಕ್ತ)
		qfp = old_last->qf_next;
	    ಅನ್ಯಥಾ
		qfp = old_last;
	    lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_ಎಣಿಕೆ);
	ಯದಿ (qftf_list != ರಿಕ್ತ)
	    qftf_li = qftf_list->lv_first;

	ಯಾವತ್ (lnum < qfl->qf_ಎಣಿಕೆ)
	{
	    char_u	*qftf_str = ರಿಕ್ತ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    ಯದಿ (qftf_li != ರಿಕ್ತ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		ಯದಿ (qftf_str == ರಿಕ್ತ)
		    invalid_val = TRUE;
	    }

	    ಯದಿ (qf_buf_add_ಸಾಲು(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ಭಙ್ಗ;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    ಯದಿ (qfp == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    ಯದಿ (qftf_li != ರಿಕ್ತ)
		qftf_li = qftf_li->li_next;
	}

	ಯದಿ (old_last == ರಿಕ್ತ)
	    // Delete the empty line which is now at the end
	    (ಶೂನ್ಯ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    ಯದಿ (old_last == ರಿಕ್ತ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ರಿಕ್ತ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ರಿಕ್ತ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    ಕೀಲಿ_Typed = old_ಕೀಲಿ_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    ಪೂರ್ಣಾಙ್ಕ		qf_idx;

    ಪ್ರತಿ (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	ಯದಿ (qi->qf_lists[qf_idx].qf_id == qfid)
	    ಪ್ರತ್ಯಾಗಮ qf_idx;
    ಪ್ರತ್ಯಾಗಮ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    ಪೂರ್ಣಾಙ್ಕ curlist;

    ಯದಿ (qf_get_curlist(qi)->qf_id == save_qfid)
	ಪ್ರತ್ಯಾಗಮ OK;

    curlist = qf_id2nr(qi, save_qfid);
    ಯದಿ (curlist < 0)
	// list is not present
	ಪ್ರತ್ಯಾಗಮ FAIL;
    qi->qf_curlist = curlist;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_first(qf_info_T *qi, int_u save_qfid, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಯದಿ (qf_restore_list(qi, save_qfid) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;


    ಯದಿ (!check_can_set_curbuf_forceit(forceit))
	ಪ್ರತ್ಯಾಗಮ;


    // Autocommands might have cleared the list, check for that.
    ಯದಿ (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    ಪೂರ್ಣಾಙ್ಕ
grep_internal(cmdidx_T cmdidx)
{
    ಪ್ರತ್ಯಾಗಮ ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ಸ್ಥಿರ char_u *
make_get_auname(cmdidx_T cmdidx)
{
    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_make:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"make";
	ಪಕ್ಷ CMD_lmake:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"lmake";
	ಪಕ್ಷ CMD_grep:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"grep";
	ಪಕ್ಷ CMD_lgrep:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"lgrep";
	ಪಕ್ಷ CMD_grepadd:   ಪ್ರತ್ಯಾಗಮ (char_u *)"grepadd";
	ಪಕ್ಷ CMD_lgrepadd:  ಪ್ರತ್ಯಾಗಮ (char_u *)"lgrepadd";
	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ಸ್ಥಿರ char_u *
get_mef_name(ಶೂನ್ಯ)
{
    char_u	*p;
    char_u	*name;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	start = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    ಯದಿ (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_cant_get_temp_file_name));
	ಪ್ರತ್ಯಾಗಮ name;
    }

    ಪ್ರತಿ (p = p_mef; *p; ++p)
	ಯದಿ (p[0] == '#' && p[1] == '#')
	    ಭಙ್ಗ;

    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (start == -1)
	    start = mch_get_pid();
	ಅನ್ಯಥಾ
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	STRCPY(name, p_mef);
	sprintf((ವರ್ಣ *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	ಯದಿ (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((ವರ್ಣ *)name, &sb) < 0
# endif
		)
	    ಭಙ್ಗ;
	vim_free(name);
    }
    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ಸ್ಥಿರ char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    ಯದಿ (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    ಯದಿ (cmd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    sprintf((ವರ್ಣ *)cmd, "%s%s%s", (ವರ್ಣ *)p_shq, (ವರ್ಣ *)makecmd,
							       (ವರ್ಣ *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    ಯದಿ (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    ಯದಿ (ಸಂದೇಶ_col == 0)
	ಸಂದೇಶ_didout = FALSE;
    ಸಂದೇಶ_start();
    ಸಂದೇಶ_puts(":!");
    ಸಂದೇಶ_outtrans(cmd);		// show what we are doing

    ಪ್ರತ್ಯಾಗಮ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    ಶೂನ್ಯ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ರಿಕ್ತ;
    win_T	*wp = ರಿಕ್ತ;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಙ್ಕ		res;
    char_u	*au_name = ರಿಕ್ತ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    ಪೂರ್ಣಾಙ್ಕ		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    ಯದಿ (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }

    au_name = make_get_auname(eap->cmdidx);
    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    ಯದಿ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    ಯದಿ (cmd == ರಿಕ್ತ)
    {
	vim_free(fname);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (ಶೂನ್ಯ)char_avail();
# endif

    incr_quickfix_busy();

    ಯದಿ (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    ಯದಿ (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	qi = GET_LOC_LIST(wp);
	ಯದಿ (qi == ರಿಕ್ತ)
	    ಗಚ್ಛ cleanup;
    }
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
	ಗಚ್ಛ cleanup;

    ಯದಿ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಯದಿ (au_name != ರಿಕ್ತ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    ಯದಿ (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    ಪೂರ್ಣಾಙ್ಕ
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ qf_get_curlist(qi)->qf_ಎಣಿಕೆ;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i, sz = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_fnum = 0;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಯದಿ (qfp->qf_valid)
	{
	    ಯದಿ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    ಅನ್ಯಥಾ ಯದಿ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತ್ಯಾಗಮ qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    ಪೂರ್ಣಾಙ್ಕ
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i, eidx = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_fnum = 0;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    ಯದಿ (!qf_list_has_valid_entries(qfl))
	ಪ್ರತ್ಯಾಗಮ 1;

    ಪ್ರತಿ (i = 1; i <= qfl->qf_index && qfp!= ರಿಕ್ತ; i++, qfp = qfp->qf_next)
    {
	ಯದಿ (qfp->qf_valid)
	{
	    ಯದಿ (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ಯದಿ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ಅನ್ಯಥಾ
		eidx++;
	}
    }

    ಪ್ರತ್ಯಾಗಮ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_nth_valid_entry(qf_list_T *qfl, ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ fdo)
{
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i, eidx;
    ಪೂರ್ಣಾಙ್ಕ		prev_fnum = 0;

    // check if the list has valid errors
    ಯದಿ (!qf_list_has_valid_entries(qfl))
	ಪ್ರತ್ಯಾಗಮ 1;

    eidx = 0;
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಯದಿ (qfp->qf_valid)
	{
	    ಯದಿ (fdo)
	    {
		ಯದಿ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ಅನ್ಯಥಾ
		eidx++;
	}

	ಯದಿ (eidx == n)
	    ಭಙ್ಗ;
    }

    ಯದಿ (i <= qfl->qf_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ i;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ಶೂನ್ಯ
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಙ್ಕ		errornr;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	errornr = (ಪೂರ್ಣಾಙ್ಕ)eap->line2;
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (eap->cmdidx)
	{
	    ಪಕ್ಷ CMD_cc: ಪಕ್ಷ CMD_ll:
		errornr = 0;
		ಭಙ್ಗ;
	    ಪಕ್ಷ CMD_crewind: ಪಕ್ಷ CMD_lrewind: ಪಕ್ಷ CMD_cfirst:
	    ಪಕ್ಷ CMD_lfirst:
		errornr = 1;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    ಯದಿ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಙ್ಕ)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    ಶೂನ್ಯ
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಙ್ಕ		errornr;
    ಪೂರ್ಣಾಙ್ಕ		dir;

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (ಪೂರ್ಣಾಙ್ಕ)eap->line2;
    ಅನ್ಯಥಾ
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    ವಿಕಲ್ಪ (eap->cmdidx)
    {
	ಪಕ್ಷ CMD_cnext: ಪಕ್ಷ CMD_lnext: ಪಕ್ಷ CMD_cdo: ಪಕ್ಷ CMD_ldo:
	    dir = FORWARD;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_cprevious: ಪಕ್ಷ CMD_lprevious: ಪಕ್ಷ CMD_cNext:
	ಪಕ್ಷ CMD_lNext:
	    dir = BACKWARD;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_cnfile: ಪಕ್ಷ CMD_lnfile: ಪಕ್ಷ CMD_cfdo: ಪಕ್ಷ CMD_lfdo:
	    dir = FORWARD_FILE;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CMD_cpfile: ಪಕ್ಷ CMD_lpfile: ಪಕ್ಷ CMD_cNfile: ಪಕ್ಷ CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    dir = FORWARD;
	    ಭಙ್ಗ;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ಸ್ಥಿರ qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, ಪೂರ್ಣಾಙ್ಕ bnr, ಪೂರ್ಣಾಙ್ಕ *errornr)
{
    qfline_T	*qfp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, idx)
	ಯದಿ (qfp->qf_fnum == bnr)
	    ಭಙ್ಗ;

    *errornr = idx;
    ಪ್ರತ್ಯಾಗಮ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ಸ್ಥಿರ qfline_T *
qf_find_first_entry_on_ಸಾಲು(qfline_T *entry, ಪೂರ್ಣಾಙ್ಕ *errornr)
{
    ಯಾವತ್ (!got_int
	    && entry->qf_prev != ರಿಕ್ತ
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    ಪ್ರತ್ಯಾಗಮ entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ಸ್ಥಿರ qfline_T *
qf_find_last_entry_on_ಸಾಲು(qfline_T *entry, ಪೂರ್ಣಾಙ್ಕ *errornr)
{
    ಯಾವತ್ (!got_int &&
	    entry->qf_next != ರಿಕ್ತ
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    ಪ್ರತ್ಯಾಗಮ entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ linewise)
{
    ಯದಿ (linewise)
	ಪ್ರತ್ಯಾಗಮ qfp->qf_lnum > pos->lnum;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ linewise)
{
    ಯದಿ (linewise)
	ಪ್ರತ್ಯಾಗಮ qfp->qf_lnum < pos->lnum;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ linewise)
{
    ಯದಿ (linewise)
	ಪ್ರತ್ಯಾಗಮ qfp->qf_lnum >= pos->lnum;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ linewise)
{
    ಯದಿ (linewise)
	ಪ್ರತ್ಯಾಗಮ qfp->qf_lnum <= pos->lnum;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ (qfp->qf_lnum < pos->lnum ||
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
    ಸ್ಥಿರ qfline_T *
qf_find_entry_after_pos(
	ಪೂರ್ಣಾಙ್ಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಙ್ಕ		linewise,
	qfline_T	*qfp,
	ಪೂರ್ಣಾಙ್ಕ		*errornr)
{
    ಯದಿ (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	ಪ್ರತ್ಯಾಗಮ qfp;

    // Find the entry just before or at the position 'pos'
    ಯಾವತ್ (qfp->qf_next != ರಿಕ್ತ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    ಯದಿ (qfp->qf_next == ರಿಕ್ತ || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    ಪ್ರತ್ಯಾಗಮ qfp;
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
    ಸ್ಥಿರ qfline_T *
qf_find_entry_before_pos(
	ಪೂರ್ಣಾಙ್ಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಙ್ಕ		linewise,
	qfline_T	*qfp,
	ಪೂರ್ಣಾಙ್ಕ		*errornr)
{
    // Find the entry just before the position 'pos'
    ಯಾವತ್ (qfp->qf_next != ರಿಕ್ತ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    ಯದಿ (qf_entry_on_or_after_pos(qfp, pos, linewise))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_ಸಾಲು(qfp, errornr);

    ಪ್ರತ್ಯಾಗಮ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ಸ್ಥಿರ qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಙ್ಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		linewise,
	ಪೂರ್ಣಾಙ್ಕ		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    ಯದಿ (qfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;		// no entry in this file

    ಯದಿ (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    ಅನ್ಯಥಾ
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    ಪ್ರತ್ಯಾಗಮ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_get_nth_below_entry(qfline_T *entry_arg, ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ linewise, ಪೂರ್ಣಾಙ್ಕ *errornr)
{
    qfline_T *entry = entry_arg;

    ಯಾವತ್ (n-- > 0 && !got_int)
    {
	ಪೂರ್ಣಾಙ್ಕ		first_errornr = *errornr;

	ಯದಿ (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_ಸಾಲು(entry, errornr);

	ಯದಿ (entry->qf_next == ರಿಕ್ತ
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    ಯದಿ (linewise)
		*errornr = first_errornr;
	    ಭಙ್ಗ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_get_nth_above_entry(qfline_T *entry, ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ linewise, ಪೂರ್ಣಾಙ್ಕ *errornr)
{
    ಯಾವತ್ (n-- > 0 && !got_int)
    {
	ಯದಿ (entry->qf_prev == ರಿಕ್ತ
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    ಭಙ್ಗ;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	ಯದಿ (linewise)
	    entry = qf_find_first_entry_on_ಸಾಲು(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಙ್ಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಙ್ಕ		n,
	ಪೂರ್ಣಾಙ್ಕ		dir,
	ಪೂರ್ಣಾಙ್ಕ		linewise)
{
    qfline_T	*adj_entry;
    ಪೂರ್ಣಾಙ್ಕ		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    ಯದಿ (adj_entry == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	ಯದಿ (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	ಅನ್ಯಥಾ
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    ಪ್ರತ್ಯಾಗಮ errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    ಶೂನ್ಯ
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ಪೂರ್ಣಾಙ್ಕ		dir;
    ಪೂರ್ಣಾಙ್ಕ		buf_has_flag;
    ಪೂರ್ಣಾಙ್ಕ		errornr = 0;
    pos_T	pos;

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Check whether the current buffer has any quickfix entries
    ಯದಿ (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    ಅನ್ಯಥಾ
	buf_has_flag = BUF_HAS_LL_ENTRY;
    ಯದಿ (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    ಯದಿ (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    ಅನ್ಯಥಾ
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    ಯದಿ (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    ಅನ್ಯಥಾ
	emsg(_(e_no_more_ವಸ್ತುಗಳು));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ಸ್ಥಿರ char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_cfile:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"cfile";
	ಪಕ್ಷ CMD_cgetfile:  ಪ್ರತ್ಯಾಗಮ (char_u *)"cgetfile";
	ಪಕ್ಷ CMD_caddfile:  ಪ್ರತ್ಯಾಗಮ (char_u *)"caddfile";
	ಪಕ್ಷ CMD_lfile:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"lfile";
	ಪಕ್ಷ CMD_lgetfile:  ಪ್ರತ್ಯಾಗಮ (char_u *)"lgetfile";
	ಪಕ್ಷ CMD_laddfile:  ಪ್ರತ್ಯಾಗಮ (char_u *)"laddfile";
	ಸ್ವಭಾವ:	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    ಶೂನ್ಯ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ರಿಕ್ತ;
    win_T	*wp = ರಿಕ್ತ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ರಿಕ್ತ;
    int_u	save_qfid = 0;		// init for gcc
    ಪೂರ್ಣಾಙ್ಕ		res;

    au_name = cfile_get_auname(eap->cmdidx);
    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ರಿಕ್ತ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    ಯದಿ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ರಿಕ್ತ, ರಿಕ್ತ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ರಿಕ್ತ);
	ಯದಿ (browse_file == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    ಅನ್ಯಥಾ
# endif
    ಯದಿ (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    ಯದಿ (is_loclist_cmd(eap->cmdidx))
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
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	qi = GET_LOC_LIST(wp);
	ಯದಿ (qi == ರಿಕ್ತ)
	{
	    decr_quickfix_busy();
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
    {
	decr_quickfix_busy();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಯದಿ (au_name != ರಿಕ್ತ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ರಿಕ್ತ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ಯದಿ (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ಸ್ಥಿರ char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_vimgrep:     ಪ್ರತ್ಯಾಗಮ (char_u *)"vimgrep";
	ಪಕ್ಷ CMD_lvimgrep:    ಪ್ರತ್ಯಾಗಮ (char_u *)"lvimgrep";
	ಪಕ್ಷ CMD_vimgrepadd:  ಪ್ರತ್ಯಾಗಮ (char_u *)"vimgrepadd";
	ಪಕ್ಷ CMD_lvimgrepadd: ಪ್ರತ್ಯಾಗಮ (char_u *)"lvimgrepadd";
	ಪಕ್ಷ CMD_grep:	      ಪ್ರತ್ಯಾಗಮ (char_u *)"grep";
	ಪಕ್ಷ CMD_lgrep:	      ಪ್ರತ್ಯಾಗಮ (char_u *)"lgrep";
	ಪಕ್ಷ CMD_grepadd:     ಪ್ರತ್ಯಾಗಮ (char_u *)"grepadd";
	ಪಕ್ಷ CMD_lgrepadd:    ಪ್ರತ್ಯಾಗಮ (char_u *)"lgrepadd";
	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ರಿಕ್ತ;

    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	ಯದಿ (last_ಹುಡುಕು_pat() == ರಿಕ್ತ)
	{
	    emsg(_(e_no_previous_regular_expression));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	regmatch->regprog = vim_regcomp(last_ಹುಡುಕು_pat(), RE_MAGIC);
    }
    ಅನ್ಯಥಾ
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    ಸಂದೇಶ_start();
    p = ಸಂದೇಶ_strtrunc(fname, TRUE);
    ಯದಿ (p == ರಿಕ್ತ)
	ಸಂದೇಶ_outtrans(fname);
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_outtrans(p);
	vim_free(p);
    }
    ಸಂದೇಶ_clr_eos();
    ಸಂದೇಶ_didout = FALSE;	    // overwrite this message
    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
    ಸಂದೇಶ_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ಸ್ಥಿರ buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    ಪೂರ್ಣಾಙ್ಕ		save_mls;
# ಯದಿ defined(FEAT_SYN_HL)
    char_u	*save_ei = ರಿಕ್ತ;
# endif
    buf_T	*buf;

# ಯದಿ defined(FEAT_SYN_HL)
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
# ಯದಿ defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    ಯದಿ (!qflist_valid(wp, qfid))
    {
	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }

    ಯದಿ (qf_restore_list(qi, qfid) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	ಪೂರ್ಣಾಙ್ಕ	    duplicate_name,
	ಪೂರ್ಣಾಙ್ಕ	    flags)
{
    ಪೂರ್ಣಾಙ್ಕ		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    ಪೂರ್ಣಾಙ್ಕ		pat_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(spat);
    ಯದಿ (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    ಪ್ರತಿ (lnum = 1; lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && *tomatch > 0; ++lnum)
    {
	col = 0;
	ಯದಿ (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    ಯಾವತ್ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, ರಿಕ್ತ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ಯದಿ (qf_add_entry(qfl,
			    ರಿಕ್ತ,	// dir
			    fname,
			    ರಿಕ್ತ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    ರಿಕ್ತ,	// search pattern
			    0,		// nr
			    0,		// type
			    ರಿಕ್ತ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ಭಙ್ಗ;
		}
		found_match = TRUE;
		ಯದಿ (--*tomatch == 0)
		    ಭಙ್ಗ;
		ಯದಿ ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ಭಙ್ಗ;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		ಯದಿ (col > ml_get_buf_len(buf, lnum))
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    ಪೂರ್ಣಾಙ್ಕ	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    ಯಾವತ್ (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ಯದಿ (qf_add_entry(qfl,
			    ರಿಕ್ತ,	// dir
			    fname,
			    ರಿಕ್ತ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    ರಿಕ್ತ,	// search pattern
			    0,		// nr
			    0,		// type
			    ರಿಕ್ತ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ಭಙ್ಗ;
		}
		found_match = TRUE;
		ಯದಿ (--*tomatch == 0)
		    ಭಙ್ಗ;
		ಯದಿ ((flags & VGR_GLOBAL) == 0)
		    ಭಙ್ಗ;
		col = matches[pat_len - 1] + col + 1;
		ಯದಿ (col > linelen)
		    ಭಙ್ಗ;
	    }
	}
	ಸಾಲು_breakcheck();
	ಯದಿ (got_int)
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vgr_jump_to_match(
	qf_info_T   *qi,
	ಪೂರ್ಣಾಙ್ಕ	    forceit,
	ಪೂರ್ಣಾಙ್ಕ	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    ಯದಿ (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    ಯದಿ (curbuf == first_match_buf && target_dir != ರಿಕ್ತ)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    ಪೂರ್ಣಾಙ್ಕ		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    ಪೂರ್ಣಾಙ್ಕ		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = ರಿಕ್ತ;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    ಯದಿ (eap->addr_ಎಣಿಕೆ > 0)
	args->tomatch = eap->line2;
    ಅನ್ಯಥಾ
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    ಯದಿ (p == ರಿಕ್ತ)
    {
	emsg(_(e_invalid_ಹುಡುಕು_pattern_or_delimiter));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    ಯದಿ (args->regmatch.regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = skipwhite(p);
    ಯದಿ (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    ಯದಿ ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
existing_swapfile(buf_T *buf)
{
    ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ && buf->b_ml.ml_mfp->mf_fname != ರಿಕ್ತ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	ಪ್ರತ್ಯಾಗಮ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	ಪೂರ್ಣಾಙ್ಕ		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    ಪೂರ್ಣಾಙ್ಕ		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		fi;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		duplicate_name = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		using_dummy;
    char_u	*dirname_start = ರಿಕ್ತ;
    char_u	*dirname_now = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    ಯದಿ (dirname_start == ರಿಕ್ತ || dirname_now == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    ಪ್ರತಿ (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	ಯದಿ (time(ರಿಕ್ತ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(ರಿಕ್ತ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ರಿಕ್ತ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	ಅನ್ಯಥಾ
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	ಯದಿ (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    ಗಚ್ಛ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    ಯದಿ (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	ಅನ್ಯಥಾ
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    ಯದಿ (using_dummy)
	    {
		ಯದಿ (found_match && *first_match_buf == ರಿಕ್ತ)
		    *first_match_buf = buf;
		ಯದಿ (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ ಯದಿ ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    ಯದಿ (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = ರಿಕ್ತ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ರಿಕ್ತ;
		    }
		}

		ಯದಿ (buf != ರಿಕ್ತ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    ಯದಿ (buf == *first_match_buf
			    && *target_dir == ರಿಕ್ತ
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    ಯದಿ (curbuf == buf)
		    {
# ಯದಿ defined(FEAT_SYN_HL)
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
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    ಶೂನ್ಯ
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = ರಿಕ್ತ;
    char_u	*target_dir = ರಿಕ್ತ;
    char_u	*au_name =  ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		status;

    ಯದಿ (!check_can_set_curbuf_forceit(eap->forceit))
	ಪ್ರತ್ಯಾಗಮ;

    au_name = vgr_get_auname(eap->cmdidx);
    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ;
# endif
    }

    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (vgr_process_args(eap, &args) == FAIL)
	ಗಚ್ಛ theend;

    ಯದಿ ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ರಾಶಿ_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    ಯದಿ (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	ಗಚ್ಛ theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, ರಿಕ್ತ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    ಯದಿ (au_name != ರಿಕ್ತ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    ಯದಿ (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	ಗಚ್ಛ theend;
    }

    // Jump to first match.
    ಯದಿ (!qf_list_empty(qf_get_curlist(qi)))
    {
	ಯದಿ ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    ಯದಿ (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    ಯದಿ (dirname_now == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    mch_dirname(dirname_now, MAXPATHL);
    ಯದಿ (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == ರಿಕ್ತ) ? CMD_cd : CMD_lcd;
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
    ಸ್ಥಿರ buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    ಪೂರ್ಣಾಙ್ಕ		failed = TRUE;
    aco_save_T	aco;
    ಪೂರ್ಣಾಙ್ಕ		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ರಿಕ್ತ, ರಿಕ್ತ, (linenr_T)1, BLN_DUMMY);
    ಯದಿ (newbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    ಯದಿ (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	ಯದಿ (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (ಶೂನ್ಯ)setfname(curbuf, fname, ರಿಕ್ತ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = ರಿಕ್ತ;
	    readfile_result = readfile(fname, ರಿಕ್ತ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ರಿಕ್ತ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    ಯದಿ (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		ಯದಿ (curbuf != newbuf)
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

	    ಯದಿ (newbuf_to_wipe.br_buf != ರಿಕ್ತ && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, ರಿಕ್ತ);
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

    ಯದಿ (!bufref_valid(&newbufref))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    ಯಾವತ್ (buf->b_nwindows > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	    did_one = FALSE;
	win_T	    *wp;

	ಯದಿ (firstwin->w_next != ರಿಕ್ತ)
	    FOR_ALL_WINDOWS(wp)
		ಯದಿ (wp->w_buffer == buf)
		{
		    ಯದಿ (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ಭಙ್ಗ;
		}
	ಯದಿ (!did_one)
	    ಗಚ್ಛ fail;
    }

    ಯದಿ (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# ಯದಿ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# ಯದಿ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	ಯದಿ (dirname_start != ರಿಕ್ತ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	ಪ್ರತ್ಯಾಗಮ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    ಯದಿ (curbuf == buf)		// safety check
	ಪ್ರತ್ಯಾಗಮ;

    close_buffer(ರಿಕ್ತ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# ಯದಿ defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_qfline_ವಸ್ತುಗಳು(qfline_T *qfp, list_T *list)
{
    ಪೂರ್ಣಾಙ್ಕ		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    ಯದಿ (bufnum != 0 && (buflist_findnr(bufnum) == ರಿಕ್ತ))
	bufnum = 0;

    ಯದಿ ((dict = dict_alloc()) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ಯದಿ (dict_add_ಸಂಖ್ಯೆ(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_ಸಂಖ್ಯೆ(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (ಪೂರ್ಣಾಙ್ಕ)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_ಸಂಖ್ಯೆ(dict, "valid", (long)qfp->qf_valid) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	ಪೂರ್ಣಾಙ್ಕ		qf_idx,
	ಪೂರ್ಣಾಙ್ಕ		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (qi == ರಿಕ್ತ)
    {
	qi = ql_info;
	ಯದಿ (wp != ರಿಕ್ತ)
	    qi = GET_LOC_LIST(wp);
	ಯದಿ (qi == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

    }

    ಯದಿ (eidx < 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    ಯದಿ (qf_idx >= qi->qf_listcount)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    ಯದಿ (qf_list_empty(qfl))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಯದಿ (eidx > 0)
	{
	    ಯದಿ (eidx == i)
		ಪ್ರತ್ಯಾಗಮ get_qfline_ವಸ್ತುಗಳು(qfp, list);
	}
	ಅನ್ಯಥಾ ಯದಿ (get_qfline_ವಸ್ತುಗಳು(qfp, list) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
ಗಣನಾ {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ವಸ್ತುಗಳು	= 0x2,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_list_from_ಸಾಲುಗಳು(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    ಯದಿ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    ಯದಿ ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ರಿಕ್ತ)
    {
	ಯದಿ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    qi = qf_alloc_ರಾಶಿ(QFLT_INTERNAL, 1);
    ಯದಿ (qi != ರಿಕ್ತ)
    {
	ಯದಿ (qf_init_ext(qi, 0, ರಿಕ್ತ, ರಿಕ್ತ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ರಿಕ್ತ, ರಿಕ್ತ) > 0)
	{
	    (ಶೂನ್ಯ)get_errorlist(qi, ರಿಕ್ತ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    win = qf_find_win(qi);
    ಯದಿ (win != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ win->w_id;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ	bufnum = 0;

    ಯದಿ (qi != ರಿಕ್ತ && buflist_findnr(qi->qf_bufnr) != ರಿಕ್ತ)
	bufnum = qi->qf_bufnr;

    ಪ್ರತ್ಯಾಗಮ dict_add_ಸಂಖ್ಯೆ(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_keys2flags(dict_T *what, ಪೂರ್ಣಾಙ್ಕ loclist)
{
    ಪೂರ್ಣಾಙ್ಕ		flags = QF_GETLIST_NONE;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	ಯದಿ (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    ಯದಿ (dict_has_ಕೀಲಿ(what, "title"))
	flags |= QF_GETLIST_TITLE;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "nr"))
	flags |= QF_GETLIST_NR;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "winid"))
	flags |= QF_GETLIST_WINID;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "id"))
	flags |= QF_GETLIST_ID;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "items"))
	flags |= QF_GETLIST_ವಸ್ತುಗಳು;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "idx"))
	flags |= QF_GETLIST_IDX;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "size"))
	flags |= QF_GETLIST_SIZE;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    ಯದಿ (loclist && dict_has_ಕೀಲಿ(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    ಯದಿ (dict_has_ಕೀಲಿ(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    ಪ್ರತ್ಯಾಗಮ flags;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    ಪೂರ್ಣಾಙ್ಕ		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    ಯದಿ ((di = dict_find(what, (char_u *)"nr", -1)) != ರಿಕ್ತ)
    {
	// Use the specified quickfix/location list
	ಯದಿ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // for zero use the current list
	    ಯದಿ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
	    {
		qf_idx = di->di_tv.vval.v_ಸಂಖ್ಯೆ - 1;
		ಯದಿ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ರಿಕ್ತ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	ಅನ್ಯಥಾ
	    qf_idx = INVALID_QFIDX;
    }

    ಯದಿ ((di = dict_find(what, (char_u *)"id", -1)) != ರಿಕ್ತ)
    {
	// Look for a list with the specified id
	ಯದಿ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    ಯದಿ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_ಸಂಖ್ಯೆ);
	}
	ಅನ್ಯಥಾ
	    qf_idx = INVALID_QFIDX;
    }

    ಪ್ರತ್ಯಾಗಮ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_defaults(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ locstack, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		status = OK;

    ಯದಿ (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_ವಸ್ತುಗಳು))
    {
	list_T	*l = list_alloc();
	ಯದಿ (l != ರಿಕ್ತ)
	    status = dict_add_list(retdict, "items", l);
	ಅನ್ಯಥಾ
	    status = FAIL;
    }
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "nr", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "winid", qf_winid(qi));
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "id", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "idx", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "size", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "changedtick", 0);
    ಯದಿ ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "filewinid", 0);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    ಪ್ರತ್ಯಾಗಮ dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ winid = 0;

    ಯದಿ (wp != ರಿಕ್ತ && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	ಯದಿ (ll_wp != ರಿಕ್ತ)
	    winid = ll_wp->w_id;
    }

    ಪ್ರತ್ಯಾಗಮ dict_add_ಸಂಖ್ಯೆ(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_ವಸ್ತುಗಳು(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ qf_idx, ಪೂರ್ಣಾಙ್ಕ eidx, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    list_T	*l = list_alloc();
    ಯದಿ (l != ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)get_errorlist(qi, ರಿಕ್ತ, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    ಅನ್ಯಥಾ
	status = FAIL;

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		status;
    dictitem_T	*di;

    ಯದಿ (qfl->qf_ctx != ರಿಕ್ತ)
    {
	di = dictitem_alloc((char_u *)"context");
	ಯದಿ (di != ರಿಕ್ತ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    ಯದಿ (status == FAIL)
		dictitem_free(di);
	}
	ಅನ್ಯಥಾ
	    status = FAIL;
    }
    ಅನ್ಯಥಾ
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_idx(qf_list_T *qfl, ಪೂರ್ಣಾಙ್ಕ eidx, dict_T *retdict)
{
    ಯದಿ (eidx == 0)
    {
	eidx = qfl->qf_index;
	ಯದಿ (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    ಪ್ರತ್ಯಾಗಮ dict_add_ಸಂಖ್ಯೆ(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    ಪೂರ್ಣಾಙ್ಕ		status;

    ಯದಿ (qfl->qf_qftf_cb.cb_name != ರಿಕ್ತ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    ಅನ್ಯಥಾ
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    ಪೂರ್ಣಾಙ್ಕ		qf_idx = INVALID_QFIDX;
    ಪೂರ್ಣಾಙ್ಕ		eidx = 0;
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		flags = QF_GETLIST_NONE;

    ಯದಿ ((di = dict_find(what, (char_u *)"lines", -1)) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ qf_get_list_from_ಸಾಲುಗಳು(what, di, retdict);

    ಯದಿ (wp != ರಿಕ್ತ)
	qi = GET_LOC_LIST(wp);
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ರಿಕ್ತ));

    ಯದಿ (!qf_ರಾಶಿ_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    ಯದಿ (qf_ರಾಶಿ_empty(qi) || qf_idx == INVALID_QFIDX)
	ಪ್ರತ್ಯಾಗಮ qf_getprop_defaults(qi, flags, wp != ರಿಕ್ತ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    ಯದಿ ((di = dict_find(what, (char_u *)"idx", -1)) != ರಿಕ್ತ)
    {
	ಯದಿ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	eidx = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
    }

    ಯದಿ (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "nr", qf_idx + 1);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "winid", qf_winid(qi));
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_ವಸ್ತುಗಳು))
	status = qf_getprop_ವಸ್ತುಗಳು(qi, qf_idx, eidx, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "id", qfl->qf_id);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "size", qfl->qf_ಎಣಿಕೆ);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_ಸಂಖ್ಯೆ(retdict, "changedtick", qfl->qf_changedtick);
    ಯದಿ ((status == OK) && (wp != ರಿಕ್ತ) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    ಯದಿ ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	ಪೂರ್ಣಾಙ್ಕ		first_entry,
	ಪೂರ್ಣಾಙ್ಕ		*valid_entry)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    ಪೂರ್ಣಾಙ್ಕ		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    ಯದಿ (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "bufnr");
    lnum = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "lnum");
    end_lnum = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "end_lnum");
    col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "col");
    end_col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "end_col");
    vcol = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "vcol");
    nr = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    ಯದಿ (text == ರಿಕ್ತ)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    ಯದಿ ((filename == ರಿಕ್ತ && bufnum == 0) || (lnum == 0 && pattern == ರಿಕ್ತ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    ಯದಿ (bufnum != 0 && (buflist_findnr(bufnum) == ರಿಕ್ತ))
    {
	ಯದಿ (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    ಯದಿ (dict_has_ಕೀಲಿ(d, "valid"))
	valid = (ಪೂರ್ಣಾಙ್ಕ)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			ರಿಕ್ತ,		// dir
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
			type == ರಿಕ್ತ ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    ಯದಿ (valid)
	*valid_entry = TRUE;

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	ಪೂರ್ಣಾಙ್ಕ		target_fnum,
	ಪೂರ್ಣಾಙ್ಕ		target_lnum,
	ಪೂರ್ಣಾಙ್ಕ		target_col)
{
    // First, compare entries to target file.
    ಯದಿ (!target_fnum)
	// Without a target file, we can't know which is closer.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    ಪೂರ್ಣಾಙ್ಕ other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    ಯದಿ (!is_target_file && other_is_target_file)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಅನ್ಯಥಾ ಯದಿ (is_target_file && !other_is_target_file)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    ಯದಿ (!target_lnum)
	// Without a target line number, we can't know which is closer.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    ಪೂರ್ಣಾಙ್ಕ other_ಸಾಲು_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    ಯದಿ (ಸಾಲು_distance > other_ಸಾಲು_distance)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಅನ್ಯಥಾ ಯದಿ (ಸಾಲು_distance < other_ಸಾಲು_distance)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    ಯದಿ (!target_col)
	// Without a target column, we can't know which is closer.
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    ಪೂರ್ಣಾಙ್ಕ other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    ಯದಿ (column_distance > other_column_distance)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಅನ್ಯಥಾ ಯದಿ (column_distance < other_column_distance)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_add_entries(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		qf_idx,
	list_T		*list,
	char_u		*title,
	ಪೂರ್ಣಾಙ್ಕ		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ಪೂರ್ಣಾಙ್ಕ		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    ಪೂರ್ಣಾಙ್ಕ prev_fnum = 0;
    ಪೂರ್ಣಾಙ್ಕ prev_lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ prev_col = 0;
    ಯದಿ (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    ಪೂರ್ಣಾಙ್ಕ select_first_entry = FALSE;
    ಪೂರ್ಣಾಙ್ಕ select_nearest_entry = FALSE;

    ಯದಿ (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ಅನ್ಯಥಾ ಯದಿ (action == 'a')
    {
	ಯದಿ (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	ಅನ್ಯಥಾ
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    ಅನ್ಯಥಾ ಯದಿ (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_ವಸ್ತುಗಳು(qfl);
	qf_store_title(qfl, title);
    }
    ಅನ್ಯಥಾ ಯದಿ (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_ವಸ್ತುಗಳು(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ entry_to_select_index = 0;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಯದಿ (li->li_tv.v_type != VAR_DICT)
	    ಅನುವರ್ತ; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	ಯದಿ (d == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	ಯದಿ (retval == QF_FAIL)
	    ಭಙ್ಗ;

	qfline_T *entry = qfl->qf_last;
	ಯದಿ (
	    (select_first_entry && entry_to_select == ರಿಕ್ತ) ||
	    (select_nearest_entry &&
		(entry_to_select == ರಿಕ್ತ ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_ಎಣಿಕೆ;
	}
    }

    // Check if any valid error entries are added to the list.
    ಯದಿ (valid_entry)
	qfl->qf_nonevalid = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    ಯದಿ (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	ಪೂರ್ಣಾಙ್ಕ		action,
	ಪೂರ್ಣಾಙ್ಕ		*newlist)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		qf_idx = qi->qf_curlist;    // default is the current list

    ಯದಿ ((di = dict_find(what, (char_u *)"nr", -1)) != ರಿಕ್ತ)
    {
	// Use the specified quickfix/location list
	ಯದಿ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // for zero use the current list
	    ಯದಿ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
		qf_idx = di->di_tv.vval.v_ಸಂಖ್ಯೆ - 1;

	    ಯದಿ ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ರಾಶಿ_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		ಪ್ರತ್ಯಾಗಮ INVALID_QFIDX;
	    ಅನ್ಯಥಾ ಯದಿ (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ರಿಕ್ತ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    ಯದಿ (!qf_ರಾಶಿ_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    ಅನ್ಯಥಾ ಯದಿ (*newlist)
		qf_idx = 0;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ INVALID_QFIDX;
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ INVALID_QFIDX;
    }

    ಯದಿ (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != ರಿಕ್ತ)
    {
	// Use the quickfix/location list with the specified id
	ಯದಿ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಪ್ರತ್ಯಾಗಮ INVALID_QFIDX;

	ಪ್ರತ್ಯಾಗಮ qf_id2nr(qi, di->di_tv.vval.v_ಸಂಖ್ಯೆ);
    }

    ಪ್ರತ್ಯಾಗಮ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_title(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    ಯದಿ (di->di_tv.v_type != VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    ಯದಿ (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set quickfix list items/entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_ವಸ್ತುಗಳು(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ qf_idx, dictitem_T *di, ಪೂರ್ಣಾಙ್ಕ action)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    char_u	*title_save;

    ಯದಿ (di->di_tv.v_type != VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_ವಸ್ತುಗಳು_from_ಸಾಲುಗಳು(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಙ್ಕ		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	ಪೂರ್ಣಾಙ್ಕ		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    ಯದಿ ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ರಿಕ್ತ)
    {
	ಯದಿ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    ಯದಿ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (action == 'r' || action == 'u')
	qf_free_ವಸ್ತುಗಳು(&qi->qf_lists[qf_idx]);
    ಯದಿ (qf_init_ext(qi, qf_idx, ರಿಕ್ತ, ರಿಕ್ತ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ರಿಕ್ತ, ರಿಕ್ತ) >= 0)
	retval = OK;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Set quickfix list context.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    ಯದಿ (ctx != ರಿಕ್ತ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    ಪೂರ್ಣಾಙ್ಕ		denote = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		newidx;
    ಪೂರ್ಣಾಙ್ಕ		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    ಯದಿ (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ರಿಕ್ತ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_ಎಣಿಕೆ;
    ಅನ್ಯಥಾ
    {
	// Otherwise use the specified index
	newidx = tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &denote);
	ಯದಿ (denote)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (newidx < 1)		// sanity check
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (newidx > qfl->qf_ಎಣಿಕೆ)
	newidx = qfl->qf_ಎಣಿಕೆ;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    ಯದಿ (qf_ptr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    ಯದಿ (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    ಯದಿ (cb.cb_name == ರಿಕ್ತ || *cb.cb_name == NUL)
	ಪ್ರತ್ಯಾಗಮ OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
qf_set_properties(qf_info_T *qi, dict_T *what, ಪೂರ್ಣಾಙ್ಕ action, char_u *title)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		qf_idx;
    ಪೂರ್ಣಾಙ್ಕ		newlist = FALSE;
    qf_list_T	*qfl;

    ಯದಿ (action == ' ' || qf_ರಾಶಿ_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    ಯದಿ (qf_idx == INVALID_QFIDX)	// List not found
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    ಯದಿ ((di = dict_find(what, (char_u *)"title", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    ಯದಿ ((di = dict_find(what, (char_u *)"items", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_ವಸ್ತುಗಳು(qi, qf_idx, di, action);
    ಯದಿ ((di = dict_find(what, (char_u *)"lines", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_ವಸ್ತುಗಳು_from_ಸಾಲುಗಳು(qi, qf_idx, what, di, action);
    ಯದಿ ((di = dict_find(what, (char_u *)"context", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_context(qfl, di);
    ಯದಿ ((di = dict_find(what, (char_u *)"idx", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_curidx(qi, qfl, di);
    ಯದಿ ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != ರಿಕ್ತ)
	retval = qf_setprop_qftf(qi, qfl, di);

    ಯದಿ (newlist || retval == OK)
	qf_list_changed(qfl);
    ಯದಿ (newlist)
	qf_update_buffer(qi, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_ರಾಶಿ(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = ರಿಕ್ತ;

    ಯದಿ (qfwin != ರಿಕ್ತ)
    {
	// If the quickfix/location list window is open, then clear it
	ಯದಿ (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, ರಿಕ್ತ);
    }

    ಯದಿ (wp != ರಿಕ್ತ && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	ಯದಿ (llwin != ರಿಕ್ತ)
	    wp = llwin;
    }

    qf_free_all(wp);
    ಯದಿ (wp == ರಿಕ್ತ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (qfwin != ರಿಕ್ತ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ರಾಶಿ(QFLT_LOCATION, wp->w_p_lhi);

	ಯದಿ (new_ll != ರಿಕ್ತ)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    ಯದಿ (wp != qfwin)
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
    ಪೂರ್ಣಾಙ್ಕ
set_errorlist(
	win_T	*wp,
	list_T	*list,
	ಪೂರ್ಣಾಙ್ಕ	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;

    ಯದಿ (wp != ರಿಕ್ತ)
	qi = ll_get_or_alloc_list(wp);
    ಅನ್ಯಥಾ
	qi = ql_info;
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ರಾಶಿ(wp, qi);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    ಯದಿ (list->lv_len != 0 && what != ರಿಕ್ತ)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    incr_quickfix_busy();

    ಯದಿ (what != ರಿಕ್ತ)
	retval = qf_set_properties(qi, what, action, title);
    ಅನ್ಯಥಾ
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	ಯದಿ (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    ಪ್ರತ್ಯಾಗಮ retval;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mark_quickfix_user_data(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ abort = FALSE;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	ಯದಿ (!qfl->qf_has_user_data)
	    ಅನುವರ್ತ;
	qfline_T *qfp;
	ಪೂರ್ಣಾಙ್ಕ j;
	FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    ಯದಿ (user_data != ರಿಕ್ತ && user_data->v_type != VAR_ಸಂಖ್ಯೆ
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    }
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mark_quickfix_ctx(qf_info_T *qi, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    ಪ್ರತಿ (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	ಯದಿ (ctx != ರಿಕ್ತ && ctx->v_type != VAR_ಸಂಖ್ಯೆ
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_quickfix(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    ಯದಿ (ql_info == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    ಯದಿ (abort)
	ಪ್ರತ್ಯಾಗಮ abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    ಯದಿ (abort)
	ಪ್ರತ್ಯಾಗಮ abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    ಯದಿ (abort)
	ಪ್ರತ್ಯಾಗಮ abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ಯದಿ (win->w_llist != ರಿಕ್ತ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    ಯದಿ (abort)
		ಪ್ರತ್ಯಾಗಮ abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    ಯದಿ (abort)
		ಪ್ರತ್ಯಾಗಮ abort;
	}
	ಯದಿ (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    ಯದಿ (abort)
		ಪ್ರತ್ಯಾಗಮ abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    ಯದಿ (abort)
		ಪ್ರತ್ಯಾಗಮ abort;
	}
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ಸ್ಥಿರ char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_cbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"cbuffer";
	ಪಕ್ಷ CMD_cgetbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"cgetbuffer";
	ಪಕ್ಷ CMD_caddbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"caddbuffer";
	ಪಕ್ಷ CMD_lbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"lbuffer";
	ಪಕ್ಷ CMD_lgetbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"lgetbuffer";
	ಪಕ್ಷ CMD_laddbuffer:	ಪ್ರತ್ಯಾಗಮ (char_u *)"laddbuffer";
	ಸ್ವಭಾವ:		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ರಿಕ್ತ;

    ಯದಿ (*eap->arg == NUL)
	buf = curbuf;
    ಅನ್ಯಥಾ ಯದಿ (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((ವರ್ಣ *)eap->arg));

    ಯದಿ (buf == ರಿಕ್ತ)
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	emsg(_(e_buffer_is_not_loaded));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }

    ಯದಿ (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    ಶೂನ್ಯ
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = ರಿಕ್ತ;
    qf_info_T	*qi;
    char_u	*au_name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res;
    int_u	save_qfid;
    win_T	*wp = ರಿಕ್ತ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    ಯದಿ (buf->b_sfname)
    {
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%s (%s)",
		(ವರ್ಣ *)qf_title, (ವರ್ಣ *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ರಿಕ್ತ, buf, ರಿಕ್ತ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ರಿಕ್ತ);
    ಯದಿ (qf_ರಾಶಿ_empty(qi))
    {
	decr_quickfix_busy();
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಯದಿ (au_name != ರಿಕ್ತ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	ಯದಿ (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ಯದಿ (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# ಯದಿ defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    ವಿಕಲ್ಪ (cmdidx)
    {
	ಪಕ್ಷ CMD_cexpr:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"cexpr";
	ಪಕ್ಷ CMD_cgetexpr:  ಪ್ರತ್ಯಾಗಮ (char_u *)"cgetexpr";
	ಪಕ್ಷ CMD_caddexpr:  ಪ್ರತ್ಯಾಗಮ (char_u *)"caddexpr";
	ಪಕ್ಷ CMD_lexpr:	    ಪ್ರತ್ಯಾಗಮ (char_u *)"lexpr";
	ಪಕ್ಷ CMD_lgetexpr:  ಪ್ರತ್ಯಾಗಮ (char_u *)"lgetexpr";
	ಪಕ್ಷ CMD_laddexpr:  ಪ್ರತ್ಯಾಗಮ (char_u *)"laddexpr";
	ಸ್ವಭಾವ:	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಪೂರ್ಣಾಙ್ಕ
trigger_cexpr_autocmd(ಪೂರ್ಣಾಙ್ಕ cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ರಿಕ್ತ;

    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಯದಿ (qi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ ((tv->v_type == VAR_STRING && tv->vval.v_string != ರಿಕ್ತ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ರಿಕ್ತ))
    {
	ಪೂರ್ಣಾಙ್ಕ	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ರಿಕ್ತ, ರಿಕ್ತ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ರಿಕ್ತ);
	ಯದಿ (qf_ರಾಶಿ_empty(qi))
	{
	    decr_quickfix_busy();
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	ಯದಿ (au_name != ರಿಕ್ತ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	ಯದಿ (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    emsg(_(e_string_or_list_expected));
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    ಶೂನ್ಯ
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    ಯದಿ (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ(eap->arg, eap);
    ಯದಿ (tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    (ಶೂನ್ಯ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ಸ್ಥಿರ qf_info_T *
hgr_get_ll(ಪೂರ್ಣಾಙ್ಕ *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    ಯದಿ (bt_help(curwin->w_buffer))
	wp = curwin;
    ಅನ್ಯಥಾ
	// Find an existing help window
	wp = qf_find_help_win();

    ಯದಿ (wp == ರಿಕ್ತ)	    // Help window not found
	qi = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	qi = wp->w_llist;

    ಯದಿ (qi == ರಿಕ್ತ)
    {
	// Allocate a new location list for help text matches
	ಯದಿ ((qi = qf_alloc_ರಾಶಿ(QFLT_LOCATION, 1)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*new_ll = TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ qi;
}

/*
 * Search for a pattern in a help file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hgr_ಹುಡುಕು_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((ವರ್ಣ *)fname, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = 1;
    ಯಾವತ್ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *ಸಾಲು = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	ಯದಿ (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    ಸಾಲು = string_convert(p_vc, IObuff, ರಿಕ್ತ);
	    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
		ಸಾಲು = IObuff;
	}

	ಯದಿ (vim_regexec(p_regmatch, ಸಾಲು, (colnr_T)0))
	{
	    ಪೂರ್ಣಾಙ್ಕ	l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು);

	    // remove trailing CR, LF, spaces, etc.
	    ಯಾವತ್ (l > 0 && ಸಾಲು[l - 1] <= ' ')
		ಸಾಲು[--l] = NUL;

	    ಯದಿ (qf_add_entry(qfl,
			ರಿಕ್ತ,	// dir
			fname,
			ರಿಕ್ತ,
			0,
			ಸಾಲು,
			lnum,
			0,
			(ಪೂರ್ಣಾಙ್ಕ)(p_regmatch->startp[0] - ಸಾಲು)
			+ 1,	// col
			(ಪೂರ್ಣಾಙ್ಕ)(p_regmatch->endp[0] - ಸಾಲು)
			+ 1,	// end_col
			FALSE,	// vis_col
			ರಿಕ್ತ,	// search pattern
			0,	// nr
			1,	// type
			ರಿಕ್ತ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		ಯದಿ (ಸಾಲು != IObuff)
		    vim_free(ಸಾಲು);
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (ಸಾಲು != IObuff)
	    vim_free(ಸಾಲು);
	++lnum;
	ಸಾಲು_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hgr_ಹುಡುಕು_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    ಪೂರ್ಣಾಙ್ಕ		fcount;
    char_u	**fnames;
    ಪೂರ್ಣಾಙ್ಕ		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    ಯದಿ (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	ಪ್ರತಿ (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    ಯದಿ (lang != ರಿಕ್ತ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		ಅನುವರ್ತ;
# endif

	    hgr_ಹುಡುಕು_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
hgr_ಹುಡುಕು_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    ಯದಿ (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    ಯಾವತ್ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_ಹುಡುಕು_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    ಯದಿ (vc.vc_type != CONV_NONE)
	convert_setup(&vc, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * ":helpgrep {pattern}"
 */
    ಶೂನ್ಯ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    ಪೂರ್ಣಾಙ್ಕ		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಙ್ಕ		new_qi = FALSE;
    char_u	*au_name =  ರಿಕ್ತ;
    char_u	*lang = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		updated = FALSE;

    ವಿಕಲ್ಪ (eap->cmdidx)
    {
	ಪಕ್ಷ CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ಭಙ್ಗ;
	ಪಕ್ಷ CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ಭಙ್ಗ;
	ಸ್ವಭಾವ: ಭಙ್ಗ;
    }
    ಯದಿ (au_name != ರಿಕ್ತ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಯದಿ (aborting())
	    ಪ್ರತ್ಯಾಗಮ;
# endif
    }

    ಯದಿ (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	ಯದಿ (qi == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (qi == ರಿಕ್ತ)
    {
	emsg(_(e_no_quickfix_ರಾಶಿ));
	ಪ್ರತ್ಯಾಗಮ;
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
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_ಹುಡುಕು_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    ಯದಿ (p_cpo == empty_option)
	p_cpo = save_cpo;
    ಅನ್ಯಥಾ
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	ಯದಿ (*p_cpo == NUL)
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	ಯದಿ (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    ಯದಿ (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, ರಿಕ್ತ);

    ಯದಿ (au_name != ರಿಕ್ತ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	ಯದಿ (!new_qi && IS_LL_ರಾಶಿ(qi) && qf_find_win_with_loclist(qi) == ರಿಕ್ತ)
	{
	    decr_quickfix_busy();
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    // Jump to first match.
    ಯದಿ (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    ಅನ್ಯಥಾ
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    ಯದಿ (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	ಯದಿ (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    ಯದಿ (new_qi)
		ll_free_all(&qi);
	}
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_llist == ರಿಕ್ತ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_quickfix(ಶೂನ್ಯ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ರಿಕ್ತ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#ಯದಿ defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ಸ್ಥಿರ ಶೂನ್ಯ
get_qf_loc_list(ಪೂರ್ಣಾಙ್ಕ is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    ಯದಿ (what_arg->v_type == VAR_UNKNOWN)
    {
	ಯದಿ (rettv_list_alloc(rettv) == OK)
	    ಯದಿ (is_qf || wp != ರಿಕ್ತ)
		(ಶೂನ್ಯ)get_errorlist(ರಿಕ್ತ, wp, -1, 0, rettv->vval.v_list);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (rettv_dict_alloc(rettv) == OK)
	    ಯದಿ (is_qf || (wp != ರಿಕ್ತ))
	    {
		ಯದಿ (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    ಯದಿ (d != ರಿಕ್ತ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		ಅನ್ಯಥಾ
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    ಶೂನ್ಯ
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    ಶೂನ್ಯ
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    ಯದಿ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    get_qf_loc_list(TRUE, ರಿಕ್ತ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    ಪೂರ್ಣಾಙ್ಕ		action = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;
# endif

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

# ifdef FEAT_QUICKFIX
    ಯದಿ (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    ಅನ್ಯಥಾ ಯದಿ (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    ಅನ್ಯಥಾ
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	valid_dict = TRUE;

	ಯದಿ (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    ಯದಿ (act == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given
	    ಯದಿ ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    ಅನ್ಯಥಾ
		semsg(_(e_invalid_action_str_1), act);
	}
	ಅನ್ಯಥಾ ಯದಿ (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	ಅನ್ಯಥಾ
	    emsg(_(e_string_required));

	ಯದಿ (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    ಯದಿ (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ರಿಕ್ತ)
		what = what_arg->vval.v_dict;
	    ಅನ್ಯಥಾ
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	ಯದಿ (l != ರಿಕ್ತ && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == ರಿಕ್ತ ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    ಶೂನ್ಯ
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    win = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (win != ರಿಕ್ತ)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    ಶೂನ್ಯ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    set_qf_ll_list(ರಿಕ್ತ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
