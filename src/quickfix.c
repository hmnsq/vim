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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_QUICKFIX)

ਬਣਤਰ dir_ਢੇਰ_T
{
    ਬਣਤਰ dir_ਢੇਰ_T	*ਅਗਲਾ;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ qfline_S qfline_T;
ਬਣਤਰ qfline_S
{
    qfline_T	*qf_ਅਗਲਾ;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_ਸਿਰਾ_lnum;	// line number when the error has range or zero
    ਪੂਰਨ		qf_fnum;	// file number for the line
    ਪੂਰਨ		qf_ਖਾਨਾ;		// column where the error occurred
    ਪੂਰਨ		qf_ਸਿਰਾ_ਖਾਨਾ;	// column when the error has range or zero
    ਪੂਰਨ		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_ਨਮੂਨਾ;	// search pattern for the error
    char_u	*qf_ਪਾਠ;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_ਅੰਕੜੇ;	// custom user data associated with this item
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_ਅੰਦਰੂਨੀ  // Internal - Temporary list used by getqflist()/getloclist()
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_ਸ਼ੁਰੂ;	// pointer to the first error
    qfline_T	*qf_ਆਖਰੀ;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    ਪੂਰਨ		qf_ਗਿਣਤੀ;	// number of errors (0 means empty list)
    ਪੂਰਨ		qf_ਸੂਚਕਾਂਕ;	// current index in the error list
    ਪੂਰਨ		qf_nonevalid;	// TRUE if not a single valid entry found
    ਪੂਰਨ		qf_has_user_ਅੰਕੜੇ; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ਬਣਤਰ dir_ਢੇਰ_T	*qf_dir_ਢੇਰ;
    char_u		*qf_directory;
    ਬਣਤਰ dir_ਢੇਰ_T	*qf_file_ਢੇਰ;
    char_u		*qf_currfile;
    ਪੂਰਨ			qf_multiline;
    ਪੂਰਨ			qf_multiignore;
    ਪੂਰਨ			qf_multiscan;
    ਲੰਮਾ		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ਬਣਤਰ qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    ਪੂਰਨ		qf_refcount;
    ਪੂਰਨ		qf_listcount;	    // current number of lists
    ਪੂਰਨ		qf_curlist;	    // current error list
    ਪੂਰਨ         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    ਪੂਰਨ		qf_bufnr;	    // quickfix window buffer number
};

ਸਥਿਰ qf_info_T ql_info_actual; // global quickfix list
ਸਥਿਰ qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ਸਥਿਰ int_u ਆਖਰੀ_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ efm_S efm_T;
ਬਣਤਰ efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *ਅਗਲਾ;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    ਅਗੇਤਰ;	// prefix of this format line:
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
    ਪੂਰਨ		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ qf_delq_S
{
    ਬਣਤਰ qf_delq_S	*ਅਗਲਾ;
    qf_info_T		*qi;
} qf_delq_T;
ਸਥਿਰ qf_delq_T *qf_delq_ਸਿਰ = ਨਿਰਾ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ਸਥਿਰ ਪੂਰਨ	quickfix_busy = 0;

ਸਥਿਰ efm_T	*fmt_ਸ਼ੁਰੂ = ਨਿਰਾ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ਸਥਿਰ callback_T qftf_cb;

ਸਥਿਰ ਖਾਲੀ     qf_ਕੱਢੋ_ਢੇਰ(qf_info_T *qi, ਪੂਰਨ adjust);
ਸਥਿਰ ਖਾਲੀ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ਸਥਿਰ ਪੂਰਨ	qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, ਪੂਰਨ bufnum, char_u *mesg, ਲੰਮਾ lnum, ਲੰਮਾ ਸਿਰਾ_lnum, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ, ਪੂਰਨ vis_ਖਾਨਾ, char_u *ਨਮੂਨਾ, ਪੂਰਨ nr, ਪੂਰਨ type, typval_T *user_ਅੰਕੜੇ, ਪੂਰਨ valid);
ਸਥਿਰ ਪੂਰਨ      qf_resize_ਢੇਰ_ਅਧਾਰ(qf_info_T *qi, ਪੂਰਨ n);
ਸਥਿਰ ਖਾਲੀ     qf_sync_llw_ਮੰਜ਼ਿਲ_win(win_T *llw);
ਸਥਿਰ ਖਾਲੀ     qf_sync_win_ਮੰਜ਼ਿਲ_llw(win_T *pwp);
ਸਥਿਰ qf_info_T *qf_alloc_ਢੇਰ(qfltype_T qfltype, ਪੂਰਨ n);
ਸਥਿਰ qf_list_T *qf_alloc_list_ਢੇਰ(ਪੂਰਨ n);
ਸਥਿਰ ਖਾਲੀ	qf_free(qf_list_T *qfl);
ਸਥਿਰ char_u	*qf_types(ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ਸਥਿਰ char_u	*qf_ਧੱਕੋ_dir(char_u *, ਬਣਤਰ dir_ਢੇਰ_T **, ਪੂਰਨ is_file_ਢੇਰ);
ਸਥਿਰ char_u	*qf_ਕੱਢੋ_dir(ਬਣਤਰ dir_ਢੇਰ_T **);
ਸਥਿਰ char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ਸਥਿਰ win_T    *qf_ਲੱਭੋ_win_with_loclist(qf_info_T *ll);
ਸਥਿਰ ਖਾਲੀ	qf_jump_newwin(qf_info_T *qi, ਪੂਰਨ dir, ਪੂਰਨ errornr, ਪੂਰਨ forceit, ਪੂਰਨ newwin);
ਸਥਿਰ ਖਾਲੀ	qf_fmt_ਪਾਠ(garray_T *gap, char_u *ਪਾਠ);
ਸਥਿਰ ਖਾਲੀ	qf_range_ਪਾਠ(garray_T *gap, qfline_T *qfp);
ਸਥਿਰ ਪੂਰਨ	qf_win_pos_update(qf_info_T *qi, ਪੂਰਨ old_qf_ਸੂਚਕਾਂਕ);
ਸਥਿਰ win_T	*qf_ਲੱਭੋ_win(qf_info_T *qi);
ਸਥਿਰ buf_T	*qf_ਲੱਭੋ_buf(qf_info_T *qi);
ਸਥਿਰ ਖਾਲੀ	qf_update_ਬਫਰ(qf_info_T *qi, qfline_T *old_ਆਖਰੀ);
ਸਥਿਰ ਖਾਲੀ	qf_fill_ਬਫਰ(qf_list_T *qfl, buf_T *buf, qfline_T *old_ਆਖਰੀ, ਪੂਰਨ qf_winid);
ਸਥਿਰ buf_T	*ਲੱਦੋ_dummy_ਬਫਰ(char_u *fname, char_u *dirname_ਸ਼ੁਰੂ, char_u *resulting_dir);
ਸਥਿਰ ਖਾਲੀ	wipe_dummy_ਬਫਰ(buf_T *buf, char_u *dirname_ਸ਼ੁਰੂ);
ਸਥਿਰ ਖਾਲੀ	unload_dummy_ਬਫਰ(buf_T *buf, char_u *dirname_ਸ਼ੁਰੂ);
ਸਥਿਰ qf_info_T *ll_get_or_alloc_list(win_T *);
ਸਥਿਰ ਪੂਰਨ	ਇੰਦਰਾਜ_is_closer_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ(qfline_T *ਇੰਦਰਾਜ, qfline_T *other_ਇੰਦਰਾਜ, ਪੂਰਨ ਨਿਸ਼ਾਨਾ_fnum, ਪੂਰਨ ਨਿਸ਼ਾਨਾ_lnum, ਪੂਰਨ ਨਿਸ਼ਾਨਾ_ਖਾਨਾ);

// Quickfix window check helper macro
# define IS_QF_ਖਿੜਕੀ(wp) (bt_quickfix((wp)->w_ਬਫਰ) && (wp)->w_llist_ref == ਨਿਰਾ)
// Location list window check helper macro
# define IS_LL_ਖਿੜਕੀ(wp) (bt_quickfix((wp)->w_ਬਫਰ) && (wp)->w_llist_ref != ਨਿਰਾ)

// Quickfix and location list stack check helper macros
# define IS_QF_ਢੇਰ(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ਢੇਰ(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_ਖਿੜਕੀ(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i) \
		    ਲਈ ((i) = 1, (qfp) = (qfl)->qf_ਸ਼ੁਰੂ; \
			    !got_int && (i) <= (qfl)->qf_ਗਿਣਤੀ && (qfp) != ਨਿਰਾ; \
			    ++(i), (qfp) = (qfp)->qf_ਅਗਲਾ)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ਸਥਿਰ char_u   *qf_ਆਖਰੀ_bufname = ਨਿਰਾ;
ਸਥਿਰ bufref_T  qf_ਆਖਰੀ_bufref = {ਨਿਰਾ, 0, 0};

ਸਥਿਰ garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ਸਥਿਰ garray_T *
qfga_get(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ initialized = FALSE;

    ਜੇ (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    ਵਾਪਸ &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    ਸਥਿਰ ਖਾਲੀ
qfga_clear(ਖਾਲੀ)
{
    ਜੇ (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    ਨਹੀਂਤਾਂ
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define ਸਤਰ_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ਸਥਿਰ ਬਣਤਰ fmtpattern
{
    char_u	convchar;
    ਅੱਖਰ	*ਨਮੂਨਾ;
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
# define FMT_ਨਮੂਨਾ_M 8
	{'m', ".\\+"},		// 8
# define FMT_ਨਮੂਨਾ_R 9
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
    ਸਥਿਰ char_u *
efmpat_ਮੰਜ਼ਿਲ_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	ਪੂਰਨ	idx,
	ਪੂਰਨ	ਗੇੜਾ)
{
    char_u	*srcptr;

    ਜੇ (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_ਵਿੱਚ_format_string), *efmpat);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ ((idx && idx < FMT_ਨਮੂਨਾ_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->ਅਗੇਤਰ) != ਨਿਰਾ)
	    || (idx == FMT_ਨਮੂਨਾ_R
		&& vim_strchr((char_u *)"OPQ", efminfo->ਅਗੇਤਰ) == ਨਿਰਾ))
    {
	semsg(_(e_unexpected_chr_ਵਿੱਚ_format_str), *efmpat);
	ਵਾਪਸ ਨਿਰਾ;
    }
    efminfo->addr[idx] = (char_u)++ਗੇੜਾ;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    ਜੇ (*efmpat == 'f' && efmpat[1] != NUL)
    {
	ਜੇ (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	ਨਹੀਂਤਾਂ
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	srcptr = (char_u *)fmt_pat[idx].ਨਮੂਨਾ;
	ਜਦੋਂ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    ਵਾਪਸ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ਸਥਿਰ char_u *
scanf_fmt_ਮੰਜ਼ਿਲ_regpat(
	char_u	**pefmp,
	char_u	*efm,
	ਪੂਰਨ	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    ਜੇ (*efmp == '[' || *efmp == '\\')
    {
	ਜੇ ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    ਜੇ (efmp[1] == '^')
		*regpat++ = *++efmp;
	    ਜੇ (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		ਜਦੋਂ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		ਜੇ (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_ਵਿੱਚ_format_string));
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    ਨਹੀਂਤਾਂ
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_ਵਿੱਚ_format_string), *efmp);
	ਵਾਪਸ ਨਿਰਾ;
    }

    *pefmp = efmp;

    ਵਾਪਸ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ਸਥਿਰ char_u *
efm_analyze_ਅਗੇਤਰ(char_u *efmp, efm_T *efminfo)
{
    ਜੇ (vim_strchr((char_u *)"+-", *efmp) != ਨਿਰਾ)
	efminfo->flags = *efmp++;
    ਜੇ (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ਨਿਰਾ)
	efminfo->ਅਗੇਤਰ = *efmp;
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_chr_ਵਿੱਚ_format_string_ਅਗੇਤਰ), *efmp);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
efm_ਮੰਜ਼ਿਲ_regpat(
	char_u	*efm,
	ਪੂਰਨ	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    ਗੇੜਾ = 0;
    ਲਈ (efmp = efm; efmp < efm + len; ++efmp)
    {
	ਜੇ (*efmp == '%')
	{
	    ++efmp;
	    ਲਈ (idx = 0; idx < FMT_PATTERNS; ++idx)
		ਜੇ (fmt_pat[idx].convchar == *efmp)
		    ਰੋਕੋ;
	    ਜੇ (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_ਮੰਜ਼ਿਲ_regpat(efmp, ptr, fmt_ptr, idx, ਗੇੜਾ);
		ਜੇ (ptr == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		ਗੇੜਾ++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_ਮੰਜ਼ਿਲ_regpat(&efmp, efm, len, ptr);
		ਜੇ (ptr == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ਨਿਰਾ)
		*ptr++ = *efmp;		// regexp magic characters
	    ਨਹੀਂਤਾਂ ਜੇ (*efmp == '#')
		*ptr++ = '*';
	    ਨਹੀਂਤਾਂ ਜੇ (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_ਅਗੇਤਰ(efmp, fmt_ptr);
		ਜੇ (efmp == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_invalid_chr_ਵਿੱਚ_format_string), *efmp);
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ			// copy normal character
	{
	    ਜੇ (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)".*^$~[", *efmp) != ਨਿਰਾ)
		*ptr++ = '\\';	// escape regexp atoms
	    ਜੇ (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    ਵਾਪਸ OK;
}

/*
 * Free the 'errorformat' information list
 */
    ਸਥਿਰ ਖਾਲੀ
free_efm_list(efm_T **efm_ਪਹਿਲਾ)
{
    efm_T *efm_ptr;

    ਲਈ (efm_ptr = *efm_ਪਹਿਲਾ; efm_ptr != ਨਿਰਾ; efm_ptr = *efm_ਪਹਿਲਾ)
    {
	*efm_ਪਹਿਲਾ = efm_ptr->ਅਗਲਾ;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_ਸ਼ੁਰੂ = ਨਿਰਾ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ਸਥਿਰ ਪੂਰਨ
efm_regpat_bufsz(char_u *efm)
{
    ਪੂਰਨ sz;
    ਪੂਰਨ i;

    sz = (FMT_PATTERNS * 3) + ((ਪੂਰਨ)STRLEN(efm) << 2);
    ਲਈ (i = FMT_PATTERNS; i > 0; )
	sz += (ਪੂਰਨ)STRLEN(fmt_pat[--i].ਨਮੂਨਾ);
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# ਨਹੀਂਤਾਂ
    sz += 2; // "%f" can become two chars longer
# endif

    ਵਾਪਸ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ਸਥਿਰ ਪੂਰਨ
efm_option_part_len(char_u *efm)
{
    ਪੂਰਨ len;

    ਲਈ (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	ਜੇ (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    ਵਾਪਸ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ਸਥਿਰ efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ਨਿਰਾ;
    efm_T	*fmt_ਪਹਿਲਾ = ਨਿਰਾ;
    efm_T	*fmt_ਆਖਰੀ = ਨਿਰਾ;
    char_u	*fmtstr = ਨਿਰਾ;
    ਪੂਰਨ		len;
    ਪੂਰਨ		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    ਜੇ ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ਨਿਰਾ)
	ਜਾਓ parse_efm_error;

    ਜਦੋਂ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	ਜੇ (fmt_ptr == ਨਿਰਾ)
	    ਜਾਓ parse_efm_error;
	ਜੇ (fmt_ਪਹਿਲਾ == ਨਿਰਾ)	    // first one
	    fmt_ਪਹਿਲਾ = fmt_ptr;
	ਨਹੀਂਤਾਂ
	    fmt_ਆਖਰੀ->ਅਗਲਾ = fmt_ptr;
	fmt_ਆਖਰੀ = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	ਜੇ (efm_ਮੰਜ਼ਿਲ_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    ਜਾਓ parse_efm_error;
	ਜੇ ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ਨਿਰਾ)
	    ਜਾਓ parse_efm_error;
	// Advance to next part
	efm = skip_ਮੰਜ਼ਿਲ_option_part(efm + len);	// skip comma and spaces
    }

    ਜੇ (fmt_ਪਹਿਲਾ == ਨਿਰਾ)	// nothing found
	emsg(_(e_errorformat_ਸ਼ਾਮਲ_ਹੈ_no_ਨਮੂਨਾ));

    ਜਾਓ parse_efm_ਸਿਰਾ;

parse_efm_error:
    free_efm_list(&fmt_ਪਹਿਲਾ);

parse_efm_ਸਿਰਾ:
    vim_free(fmtstr);

    ਵਾਪਸ fmt_ਪਹਿਲਾ;
}

ਗਿਣਤੀ {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼ = 2,
    QF_NOMEM = 3,
    QF_IGNORE_ਸਤਰ = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*linebuf;
    ਪੂਰਨ		linelen;
    char_u	*growbuf;
    ਪੂਰਨ		growbufsiz;
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
    ਸਥਿਰ char_u *
qf_grow_linebuf(qfstate_T *state, ਪੂਰਨ newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > ਸਤਰ_MAXLEN ? ਸਤਰ_MAXLEN - 1 : newsz;
    ਜੇ (state->growbuf == ਨਿਰਾ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	ਜੇ (state->growbuf == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	state->growbufsiz = state->linelen;
    }
    ਨਹੀਂਤਾਂ ਜੇ (state->linelen > state->growbufsiz)
    {
	ਜੇ ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    ਵਾਪਸ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_ਅਗਲਾ_str_ਸਤਰ(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    ਪੂਰਨ		len;

    ਜੇ (*p_str == NUL) // Reached the end of the string
	ਵਾਪਸ QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼;

    p = vim_strchr(p_str, '\n');
    ਜੇ (p != ਨਿਰਾ)
	len = (ਪੂਰਨ)(p - p_str) + 1;
    ਨਹੀਂਤਾਂ
	len = (ਪੂਰਨ)STRLEN(p_str);

    ਜੇ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ਜੇ (state->linebuf == ਨਿਰਾ)
	    ਵਾਪਸ QF_NOMEM;
    }
    ਨਹੀਂਤਾਂ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    ਵਾਪਸ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_ਅਗਲਾ_list_ਸਤਰ(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    ਪੂਰਨ		len;

    ਜਦੋਂ (p_li != ਨਿਰਾ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ਨਿਰਾ))
	p_li = p_li->li_ਅਗਲਾ;	// Skip non-string items

    ਜੇ (p_li == ਨਿਰਾ)		// End of the list
    {
	state->p_li = ਨਿਰਾ;
	ਵਾਪਸ QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼;
    }

    len = (ਪੂਰਨ)STRLEN(p_li->li_tv.vval.v_string);
    ਜੇ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ਜੇ (state->linebuf == ਨਿਰਾ)
	    ਵਾਪਸ QF_NOMEM;
    }
    ਨਹੀਂਤਾਂ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_ਅਗਲਾ;	// next item
    ਵਾਪਸ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_ਅਗਲਾ_buf_ਸਤਰ(qfstate_T *state)
{
    char_u	*p_buf = ਨਿਰਾ;
    ਪੂਰਨ		len;

    // Get the next line from the supplied buffer
    ਜੇ (state->buflnum > state->lnumlast)
	ਵਾਪਸ QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    ਜੇ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ਜੇ (state->linebuf == ਨਿਰਾ)
	    ਵਾਪਸ QF_NOMEM;
    }
    ਨਹੀਂਤਾਂ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    ਵਾਪਸ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_ਅਗਲਾ_file_ਸਤਰ(qfstate_T *state)
{
    ਪੂਰਨ	    discard;
    ਪੂਰਨ	    growbuflen;

    ਜੇ (fgets((ਅੱਖਰ *)IObuff, IOSIZE, state->fd) == ਨਿਰਾ)
	ਵਾਪਸ QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼;

    discard = FALSE;
    state->linelen = (ਪੂਰਨ)STRLEN(IObuff);
    ਜੇ (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	ਜੇ (state->growbuf == ਨਿਰਾ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    ਜੇ (state->growbuf == ਨਿਰਾ)
		ਵਾਪਸ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	ਲਈ (;;)
	{
	    char_u	*p;

	    ਜੇ (fgets((ਅੱਖਰ *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ਨਿਰਾ)
		ਰੋਕੋ;
	    state->linelen = (ਪੂਰਨ)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    ਜੇ ((state->growbuf)[growbuflen - 1] == '\n')
		ਰੋਕੋ;
	    ਜੇ (state->growbufsiz == ਸਤਰ_MAXLEN)
	    {
		discard = TRUE;
		ਰੋਕੋ;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < ਸਤਰ_MAXLEN
		? 2 * state->growbufsiz : ਸਤਰ_MAXLEN;
	    ਜੇ ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ਨਿਰਾ)
		ਵਾਪਸ QF_NOMEM;
	    state->growbuf = p;
	}

	ਜਦੋਂ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    ਜੇ (fgets((ਅੱਖਰ *)IObuff, IOSIZE, state->fd) == ਨਿਰਾ
		    || (ਪੂਰਨ)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ਰੋਕੋ;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    ਨਹੀਂਤਾਂ
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    ਜੇ (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*ਸਤਰ;

	ਸਤਰ = string_convert(&state->vc, state->linebuf, &state->linelen);
	ਜੇ (ਸਤਰ != ਨਿਰਾ)
	{
	    ਜੇ (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, ਸਤਰ);
		vim_free(ਸਤਰ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = ਸਤਰ;
		state->growbufsiz = state->linelen < ਸਤਰ_MAXLEN
						? state->linelen : ਸਤਰ_MAXLEN;
	    }
	}
    }

    ਵਾਪਸ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_nextline(qfstate_T *state)
{
    ਪੂਰਨ ਸਥਿਤੀ = QF_FAIL;

    ਜੇ (state->fd == ਨਿਰਾ)
    {
	ਜੇ (state->tv != ਨਿਰਾ)
	{
	    ਜੇ (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		ਸਥਿਤੀ = qf_get_ਅਗਲਾ_str_ਸਤਰ(state);
	    ਨਹੀਂਤਾਂ ਜੇ (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		ਸਥਿਤੀ = qf_get_ਅਗਲਾ_list_ਸਤਰ(state);
	}
	ਨਹੀਂਤਾਂ
	    // Get the next line from the supplied buffer
	    ਸਥਿਤੀ = qf_get_ਅਗਲਾ_buf_ਸਤਰ(state);
    }
    ਨਹੀਂਤਾਂ
	// Get the next line from the supplied file
	ਸਥਿਤੀ = qf_get_ਅਗਲਾ_file_ਸਤਰ(state);

    ਜੇ (ਸਥਿਤੀ != QF_OK)
	ਵਾਪਸ ਸਥਿਤੀ;

    // remove newline/CR from the line
    ਜੇ (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	ਜੇ (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    ਵਾਪਸ QF_OK;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*namebuf;
    ਪੂਰਨ		bnr;
    char_u	*module;
    char_u	*errmsg;
    ਪੂਰਨ		errmsglen;
    ਲੰਮਾ	lnum;
    ਲੰਮਾ	ਸਿਰਾ_lnum;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		ਸਿਰਾ_ਖਾਨਾ;
    char_u	use_viscol;
    char_u	*ਨਮੂਨਾ;
    ਪੂਰਨ		enr;
    ਪੂਰਨ		type;
    typval_T	*user_ਅੰਕੜੇ;
    ਪੂਰਨ		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_f(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields, ਪੂਰਨ ਅਗੇਤਰ)
{
    ਪੂਰਨ c;

    ਜੇ (rmp->startp[midx] == ਨਿਰਾ || rmp->endp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    ਜੇ (vim_strchr((char_u *)"OPQ", ਅਗੇਤਰ) != ਨਿਰਾ
	    && mch_getperm(fields->namebuf) == -1)
	ਵਾਪਸ QF_FAIL;

    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_b(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    ਪੂਰਨ bnr = (ਪੂਰਨ)atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਜੇ (buflist_findnr(bnr) == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->bnr = bnr;
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_n(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->enr = (ਪੂਰਨ)atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_l(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->lnum = atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_e(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->ਸਿਰਾ_lnum = atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_c(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->ਖਾਨਾ = (ਪੂਰਨ)atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_k(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->ਸਿਰਾ_ਖਾਨਾ = (ਪੂਰਨ)atol((ਅੱਖਰ *)rmp->startp[midx]);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_t(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->type = *rmp->startp[midx];
    ਵਾਪਸ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ਸਥਿਰ ਪੂਰਨ
copy_nonerror_ਸਤਰ(char_u *linebuf, ਪੂਰਨ linelen, qffields_T *fields)
{
    char_u	*p;

    ਜੇ (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	ਜੇ ((p = vim_realloc(fields->errmsg, linelen + 1)) == ਨਿਰਾ)
	    ਵਾਪਸ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_m(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    char_u	*p;
    ਪੂਰਨ		len;

    ਜੇ (rmp->startp[midx] == ਨਿਰਾ || rmp->endp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    len = (ਪੂਰਨ)(rmp->endp[midx] - rmp->startp[midx]);
    ਜੇ (len >= fields->errmsglen)
    {
	// len + null terminator
	ਜੇ ((p = vim_realloc(fields->errmsg, len + 1)) == ਨਿਰਾ)
	    ਵਾਪਸ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_r(regmatch_T *rmp, ਪੂਰਨ midx, char_u **ਪੂਛ)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    *ਪੂਛ = rmp->startp[midx];
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_p(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    char_u	*ਮੇਲ_ptr;

    ਜੇ (rmp->startp[midx] == ਨਿਰਾ || rmp->endp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->ਖਾਨਾ = 0;
    ਲਈ (ਮੇਲ_ptr = rmp->startp[midx]; ਮੇਲ_ptr != rmp->endp[midx];
								++ਮੇਲ_ptr)
    {
	++fields->ਖਾਨਾ;
	ਜੇ (*ਮੇਲ_ptr == TAB)
	{
	    fields->ਖਾਨਾ += 7;
	    fields->ਖਾਨਾ -= fields->ਖਾਨਾ % 8;
	}
    }
    ++fields->ਖਾਨਾ;
    fields->use_viscol = TRUE;
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_v(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਜੇ (rmp->startp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    fields->ਖਾਨਾ = (ਪੂਰਨ)atol((ਅੱਖਰ *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_s(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਪੂਰਨ		len;

    ਜੇ (rmp->startp[midx] == ਨਿਰਾ || rmp->endp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    len = (ਪੂਰਨ)(rmp->endp[midx] - rmp->startp[midx]);
    ਜੇ (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->ਨਮੂਨਾ, "^\\V");
    STRNCAT(fields->ਨਮੂਨਾ, rmp->startp[midx], len);
    fields->ਨਮੂਨਾ[len + 3] = '\\';
    fields->ਨਮੂਨਾ[len + 4] = '$';
    fields->ਨਮੂਨਾ[len + 5] = NUL;
    ਵਾਪਸ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_fmt_o(regmatch_T *rmp, ਪੂਰਨ midx, qffields_T *fields)
{
    ਪੂਰਨ		len;

    ਜੇ (rmp->startp[midx] == ਨਿਰਾ || rmp->endp[midx] == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    len = (ਪੂਰਨ)(rmp->endp[midx] - rmp->startp[midx]);
    ਜੇ (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    ਵਾਪਸ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ਸਥਿਰ ਪੂਰਨ (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, ਪੂਰਨ, qffields_T *) =
{
    ਨਿਰਾ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ਨਿਰਾ, // %r
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
    ਸਥਿਰ ਪੂਰਨ
qf_parse_ਮੇਲ(
	char_u		*linebuf,
	ਪੂਰਨ		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	ਪੂਰਨ		qf_multiline,
	ਪੂਰਨ		qf_multiscan,
	char_u		**ਪੂਛ)
{
    ਪੂਰਨ		idx = fmt_ptr->ਅਗੇਤਰ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		midx;
    ਪੂਰਨ		ਸਥਿਤੀ;

    ਜੇ ((idx == 'C' || idx == 'Z') && !qf_multiline)
	ਵਾਪਸ QF_FAIL;
    ਜੇ (vim_strchr((char_u *)"EWIN", idx) != ਨਿਰਾ)
	fields->type = idx;
    ਨਹੀਂਤਾਂ
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    ਲਈ (i = 0; i < FMT_PATTERNS; i++)
    {
	ਸਥਿਤੀ = QF_OK;
	midx = (ਪੂਰਨ)fmt_ptr->addr[i];
	ਜੇ (i == 0 && midx > 0)				// %f
	    ਸਥਿਤੀ = qf_parse_fmt_f(regmatch, midx, fields, idx);
	ਨਹੀਂਤਾਂ ਜੇ (i == FMT_ਨਮੂਨਾ_M)
	{
	    ਜੇ (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		ਸਥਿਤੀ = copy_nonerror_ਸਤਰ(linebuf, linelen, fields);
	    ਨਹੀਂਤਾਂ ਜੇ (midx > 0)				// %m
		ਸਥਿਤੀ = qf_parse_fmt_m(regmatch, midx, fields);
	}
	ਨਹੀਂਤਾਂ ਜੇ (i == FMT_ਨਮੂਨਾ_R && midx > 0)	// %r
	    ਸਥਿਤੀ = qf_parse_fmt_r(regmatch, midx, ਪੂਛ);
	ਨਹੀਂਤਾਂ ਜੇ (midx > 0)				// others
	    ਸਥਿਤੀ = (qf_parse_fmt[i])(regmatch, midx, fields);

	ਜੇ (ਸਥਿਤੀ != QF_OK)
	    ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਵਾਪਸ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_get_fields(
	char_u		*linebuf,
	ਪੂਰਨ		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	ਪੂਰਨ		qf_multiline,
	ਪੂਰਨ		qf_multiscan,
	char_u		**ਪੂਛ)
{
    regmatch_T	regmatch;
    ਪੂਰਨ		ਸਥਿਤੀ = QF_FAIL;
    ਪੂਰਨ		r;

    ਜੇ (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->ਅਗੇਤਰ) == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->ਨਮੂਨਾ[0] = NUL;
    ਜੇ (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->ਸਿਰਾ_lnum = 0;
    fields->ਖਾਨਾ = 0;
    fields->ਸਿਰਾ_ਖਾਨਾ = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *ਪੂਛ = ਨਿਰਾ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    ਜੇ (r)
	ਸਥਿਤੀ = qf_parse_ਮੇਲ(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, ਪੂਛ);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_dir_pfx(ਪੂਰਨ idx, qffields_T *fields, qf_list_T *qfl)
{
    ਜੇ (idx == 'D')				// enter directory
    {
	ਜੇ (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_ਖਾਲੀ_ਥਾਵਾਂ_directory_name));
	    ਵਾਪਸ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ਧੱਕੋ_dir(fields->namebuf, &qfl->qf_dir_ਢੇਰ, FALSE);
	ਜੇ (qfl->qf_directory == ਨਿਰਾ)
	    ਵਾਪਸ QF_FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (idx == 'X')			// leave directory
	qfl->qf_directory = qf_ਕੱਢੋ_dir(&qfl->qf_dir_ਢੇਰ);

    ਵਾਪਸ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_file_pfx(
	ਪੂਰਨ idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *ਪੂਛ)
{
    fields->valid = FALSE;
    ਜੇ (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	ਜੇ (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ਧੱਕੋ_dir(fields->namebuf, &qfl->qf_file_ਢੇਰ, TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (idx == 'Q')
	    qfl->qf_currfile = qf_ਕੱਢੋ_dir(&qfl->qf_file_ਢੇਰ);
	*fields->namebuf = NUL;
	ਜੇ (ਪੂਛ && *ਪੂਛ)
	{
	    qfl->qf_multiscan = TRUE;
	    ਵਾਪਸ QF_MULTISCAN;
	}
    }

    ਵਾਪਸ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_ਸਤਰ_nomatch(char_u *linebuf, ਪੂਰਨ linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    ਵਾਪਸ copy_nonerror_ਸਤਰ(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_multiline_pfx(
	ਪੂਰਨ idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    ਪੂਰਨ			len;

    ਜੇ (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_ਆਖਰੀ;

	ਜੇ (qfprev == ਨਿਰਾ)
	    ਵਾਪਸ QF_FAIL;
	ਜੇ (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (ਪੂਰਨ)STRLEN(qfprev->qf_ਪਾਠ);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    ਜੇ (ptr == ਨਿਰਾ)
		ਵਾਪਸ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_ਪਾਠ);
	    vim_free(qfprev->qf_ਪਾਠ);
	    qfprev->qf_ਪਾਠ = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	ਜੇ (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	ਜੇ (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	ਜੇ (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	ਜੇ (!qfprev->qf_ਸਿਰਾ_lnum)
	    qfprev->qf_ਸਿਰਾ_lnum = fields->ਸਿਰਾ_lnum;
	ਜੇ (!qfprev->qf_ਖਾਨਾ)
	{
	    qfprev->qf_ਖਾਨਾ = fields->ਖਾਨਾ;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	ਜੇ (!qfprev->qf_ਸਿਰਾ_ਖਾਨਾ)
	    qfprev->qf_ਸਿਰਾ_ਖਾਨਾ = fields->ਸਿਰਾ_ਖਾਨਾ;
	ਜੇ (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ਨਿਰਾ
		    ? fields->namebuf
		    : qfl->qf_currfile != ਨਿਰਾ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    ਜੇ (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    ਸਤਰ_breakcheck();

    ਵਾਪਸ QF_IGNORE_ਸਤਰ;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_parse_ਸਤਰ(
	qf_list_T	*qfl,
	char_u		*linebuf,
	ਪੂਰਨ		linelen,
	efm_T		*fmt_ਪਹਿਲਾ,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    ਪੂਰਨ			idx = 0;
    char_u		*ਪੂਛ = ਨਿਰਾ;
    ਪੂਰਨ			ਸਥਿਤੀ;

restofline:
    // If there was no %> item start at the first pattern
    ਜੇ (fmt_ਸ਼ੁਰੂ == ਨਿਰਾ)
	fmt_ptr = fmt_ਪਹਿਲਾ;
    ਨਹੀਂਤਾਂ
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_ਸ਼ੁਰੂ;
	fmt_ਸ਼ੁਰੂ = ਨਿਰਾ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    ਲਈ ( ; fmt_ptr != ਨਿਰਾ; fmt_ptr = fmt_ptr->ਅਗਲਾ)
    {
	idx = fmt_ptr->ਅਗੇਤਰ;
	ਸਥਿਤੀ = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &ਪੂਛ);
	ਜੇ (ਸਥਿਤੀ == QF_NOMEM)
	    ਵਾਪਸ ਸਥਿਤੀ;
	ਜੇ (ਸਥਿਤੀ == QF_OK)
	    ਰੋਕੋ;
    }
    qfl->qf_multiscan = FALSE;

    ਜੇ (fmt_ptr == ਨਿਰਾ || idx == 'D' || idx == 'X')
    {
	ਜੇ (fmt_ptr != ਨਿਰਾ)
	{
	    // 'D' and 'X' directory specifiers
	    ਸਥਿਤੀ = qf_parse_dir_pfx(idx, fields, qfl);
	    ਜੇ (ਸਥਿਤੀ != QF_OK)
		ਵਾਪਸ ਸਥਿਤੀ;
	}

	ਸਥਿਤੀ = qf_parse_ਸਤਰ_nomatch(linebuf, linelen, fields);
	ਜੇ (ਸਥਿਤੀ != QF_OK)
	    ਵਾਪਸ ਸਥਿਤੀ;

	ਜੇ (fmt_ptr == ਨਿਰਾ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (fmt_ptr != ਨਿਰਾ)
    {
	// honor %> item
	ਜੇ (fmt_ptr->conthere)
	    fmt_ਸ਼ੁਰੂ = fmt_ptr;

	ਜੇ (vim_strchr((char_u *)"AEWIN", idx) != ਨਿਰਾ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"CZ", idx) != ਨਿਰਾ)
	{				// continuation of multi-line msg
	    ਸਥਿਤੀ = qf_parse_multiline_pfx(idx, qfl, fields);
	    ਜੇ (ਸਥਿਤੀ != QF_OK)
		ਵਾਪਸ ਸਥਿਤੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"OPQ", idx) != ਨਿਰਾ)
	{				// global file names
	    ਸਥਿਤੀ = qf_parse_file_pfx(idx, fields, qfl, ਪੂਛ);
	    ਜੇ (ਸਥਿਤੀ == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(ਪੂਛ);
		ਪੂਰਨ new_linelen = (ਪੂਰਨ)STRLEN(s);
		ਜੇ (new_linelen >= linelen)
		    ਵਾਪਸ QF_IGNORE_ਸਤਰ;
		linebuf = s;
		linelen = new_linelen;
		ਜਾਓ restofline;
	    }
	}
	ਜੇ (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    ਜੇ (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    ਵਾਪਸ QF_IGNORE_ਸਤਰ;
	}
    }

    ਵਾਪਸ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qf_info_T *qi)
{
    ਵਾਪਸ qi == ਨਿਰਾ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_list_T *qfl)
{
    ਵਾਪਸ qfl == ਨਿਰਾ || qfl->qf_ਗਿਣਤੀ <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_list_has_valid_entries(qf_list_T *qfl)
{
    ਵਾਪਸ !qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ਸਥਿਰ qf_list_T *
qf_get_list(qf_info_T *qi, ਪੂਰਨ idx)
{
    ਵਾਪਸ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->ਨਮੂਨਾ = alloc_id(CMDBUFFSIZE + 1, aid_qf_ਨਮੂਨਾ);
    ਜੇ (pfields->namebuf == ਨਿਰਾ || pfields->errmsg == ਨਿਰਾ
		|| pfields->ਨਮੂਨਾ == ਨਿਰਾ || pfields->module == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free_fields(qffields_T *pfields)
{
    vim_free(pfields->namebuf);
    vim_free(pfields->module);
    vim_free(pfields->errmsg);
    vim_free(pfields->ਨਮੂਨਾ);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਤਿਆਰੀ_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    ਜੇ (enc != ਨਿਰਾ && *enc != NUL)
	convert_ਤਿਆਰੀ(&pstate->vc, enc, p_enc);

    ਜੇ (efile != ਨਿਰਾ && (pstate->fd = mch_fopen((ਅੱਖਰ *)efile, "r")) == ਨਿਰਾ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (tv != ਨਿਰਾ)
    {
	ਜੇ (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_ਪਹਿਲਾ;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    ਵਾਪਸ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_cleanup_state(qfstate_T *pstate)
{
    ਜੇ (pstate->fd != ਨਿਰਾ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    ਜੇ (pstate->vc.vc_type != CONV_NONE)
	convert_ਤਿਆਰੀ(&pstate->vc, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_ਪਹਿਲਾ,
	qfstate_T	*state,
	qffields_T	*fields)
{
    ਪੂਰਨ		    ਸਥਿਤੀ;

    // Get the next line from a file/buffer/list/string
    ਸਥਿਤੀ = qf_get_nextline(state);
    ਜੇ (ਸਥਿਤੀ != QF_OK)
	ਵਾਪਸ ਸਥਿਤੀ;

    ਸਥਿਤੀ = qf_parse_ਸਤਰ(qfl, state->linebuf, state->linelen,
	    fmt_ਪਹਿਲਾ, fields);
    ਜੇ (ਸਥਿਤੀ != QF_OK)
	ਵਾਪਸ ਸਥਿਤੀ;

    ਵਾਪਸ qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ਨਿਰਾ)
		? fields->namebuf
		: ((qfl->qf_currfile != ਨਿਰਾ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ਨਿਰਾ),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->ਸਿਰਾ_lnum,
		fields->ਖਾਨਾ,
		fields->ਸਿਰਾ_ਖਾਨਾ,
		fields->use_viscol,
		fields->ਨਮੂਨਾ,
		fields->enr,
		fields->type,
		fields->user_ਅੰਕੜੇ,
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
    ਸਥਿਰ ਪੂਰਨ
qf_init_ext(
    qf_info_T	    *qi,
    ਪੂਰਨ		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    ਪੂਰਨ		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_ਆਖਰੀ = ਨਿਰਾ;
    ਪੂਰਨ		    adding = FALSE;
    ਸਥਿਰ efm_T    *fmt_ਪਹਿਲਾ = ਨਿਰਾ;
    char_u	    *efm;
    ਸਥਿਰ char_u   *ਆਖਰੀ_efm = ਨਿਰਾ;
    ਪੂਰਨ		    retval = -1;	// default: return error flag
    ਪੂਰਨ		    ਸਥਿਤੀ;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_ਆਖਰੀ_bufname);

    CLEAR_ਖੇਤਰ(state);
    CLEAR_ਖੇਤਰ(fields);
    ਜੇ ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_ਤਿਆਰੀ_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	ਜਾਓ qf_init_ਸਿਰਾ;

    ਜੇ (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ਨਹੀਂਤਾਂ
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	ਜੇ (!qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
	    old_ਆਖਰੀ = qfl->qf_ਆਖਰੀ;
    }

    // Use the local value of 'errorformat' if it's set.
    ਜੇ (errorformat == p_efm && tv == ਨਿਰਾ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    ਨਹੀਂਤਾਂ
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    ਜੇ (ਆਖਰੀ_efm == ਨਿਰਾ || (STRCMP(ਆਖਰੀ_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(ਆਖਰੀ_efm);
	free_efm_list(&fmt_ਪਹਿਲਾ);

	// parse the current 'efm'
	fmt_ਪਹਿਲਾ = parse_efm_option(efm);
	ਜੇ (fmt_ਪਹਿਲਾ != ਨਿਰਾ)
	    ਆਖਰੀ_efm = vim_strsave(efm);
    }

    ਜੇ (fmt_ਪਹਿਲਾ == ਨਿਰਾ)	// nothing found
	ਜਾਓ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    ਜਦੋਂ (!got_int)
    {
	ਸਥਿਤੀ = qf_init_process_nextline(qfl, fmt_ਪਹਿਲਾ, &state, &fields);
	ਜੇ (ਸਥਿਤੀ == QF_NOMEM)		// memory alloc failure
	    ਜਾਓ qf_init_ਸਿਰਾ;
	ਜੇ (ਸਥਿਤੀ == QF_ਸਿਰਾ_ਦਾ_ਨਿਵੇਸ਼)	// end of input
	    ਰੋਕੋ;
	ਜੇ (ਸਥਿਤੀ == QF_FAIL)
	    ਜਾਓ error2;

	ਸਤਰ_breakcheck();
    }
    ਜੇ (state.fd == ਨਿਰਾ || !ferror(state.fd))
    {
	ਜੇ (qfl->qf_ਸੂਚਕਾਂਕ == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_ਸ਼ੁਰੂ;
	    qfl->qf_ਸੂਚਕਾਂਕ = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    qfl->qf_nonevalid = FALSE;
	    ਜੇ (qfl->qf_ptr == ਨਿਰਾ)
		qfl->qf_ptr = qfl->qf_ਸ਼ੁਰੂ;
	}
	// return number of matches
	retval = qfl->qf_ਗਿਣਤੀ;
	ਜਾਓ qf_init_ਸਿਰਾ;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    ਜੇ (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	ਜੇ (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_ਸਿਰਾ:
    ਜੇ (qf_idx == qi->qf_curlist)
	qf_update_ਬਫਰ(qi, old_ਆਖਰੀ);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    ਵਾਪਸ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    ਪੂਰਨ
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	ਪੂਰਨ	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    ਜੇ (wp != ਨਿਰਾ)
	qi = ll_get_or_alloc_list(wp);
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ਨਿਰਾ, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    ਜੇ (title == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    ਜੇ (p != ਨਿਰਾ)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    ਸਥਿਰ char_u *
qf_cmdtitle(char_u *cmd)
{
    ਸਥਿਰ char_u qftitle_str[IOSIZE];

    vim_snprintf((ਅੱਖਰ *)qftitle_str, IOSIZE, ":%s", (ਅੱਖਰ *)cmd);
    ਵਾਪਸ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ਸਥਿਰ qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    ਵਾਪਸ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_ਕੱਢੋ_ਢੇਰ(qf_info_T *qi, ਪੂਰਨ adjust)
{
    ਪੂਰਨ i;
    qf_free(&qi->qf_lists[0]);
    ਲਈ (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, ਆਕਾਰ(*qi->qf_lists));

    ਜੇ (adjust)
    {
	qi->qf_listcount--;
	ਜੇ (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	ਨਹੀਂਤਾਂ
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    ਜਦੋਂ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    ਜੇ (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_ਕੱਢੋ_ਢੇਰ(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    ਨਹੀਂਤਾਂ
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_ਸੂਚਕ(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++ਆਖਰੀ_qf_id;
    qfl->qf_has_user_ਅੰਕੜੇ = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    ਸਥਿਰ ਖਾਲੀ
locstack_ਕਤਾਰ_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    ਜੇ (q == ਨਿਰਾ)
	ਵਾਪਸ;

    q->qi = qi;
    q->ਅਗਲਾ = qf_delq_ਸਿਰ;
    qf_delq_ਸਿਰ = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    ਪੂਰਨ
qf_ਢੇਰ_get_bufnr(ਖਾਲੀ)
{
    ਜੇ (ql_info == ਨਿਰਾ)
	ਵਾਪਸ INVALID_QFBUFNR;
    ਵਾਪਸ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ਸਥਿਰ ਖਾਲੀ
wipe_qf_ਬਫਰ(qf_info_T *qi)
{
    buf_T	*qfbuf;

    ਜੇ (qi->qf_bufnr == INVALID_QFBUFNR)
	ਵਾਪਸ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    ਜੇ (qfbuf != ਨਿਰਾ && qfbuf->b_nwindows == 0)
    {
	ਪੂਰਨ buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ)
	{
	    curwin->w_ਬਫਰ = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_ਬਫਰ(ਨਿਰਾ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	ਜੇ (buf_was_null)
	    curwin->w_ਬਫਰ = ਨਿਰਾ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free_list_ਢੇਰ_ਚੀਜ਼ਾਂ(qf_info_T *qi)
{
    ਲਈ (ਪੂਰਨ i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_ਢੇਰ_ਚੀਜ਼ਾਂ(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    ਸਥਿਰ ਖਾਲੀ
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ;
    *pqi = ਨਿਰਾ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    ਜੇ (quickfix_busy > 0)
    {
	locstack_ਕਤਾਰ_delreq(qi);
	ਵਾਪਸ;
    }

    qi->qf_refcount--;
    ਜੇ (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_ਬਫਰ(qi);

	qf_free_lists(qi);
    }
}

/*
 * Free all the quickfix/location lists in the stack.
 */
    ਖਾਲੀ
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ਜੇ (wp != ਨਿਰਾ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    ਨਹੀਂਤਾਂ ਜੇ (qi != ਨਿਰਾ)
	qf_free_list_ਢੇਰ_ਚੀਜ਼ਾਂ(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    ਸਥਿਰ ਖਾਲੀ
incr_quickfix_busy(ਖਾਲੀ)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    ਸਥਿਰ ਖਾਲੀ
decr_quickfix_busy(ਖਾਲੀ)
{
    ਜੇ (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	ਜਦੋਂ (qf_delq_ਸਿਰ != ਨਿਰਾ)
	{
	    qf_delq_T	*q = qf_delq_ਸਿਰ;

	    qf_delq_ਸਿਰ = q->ਅਗਲਾ;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_ਅੰਦਰੂਨੀ_ERROR
    ਜੇ (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
check_quickfix_busy(ਖਾਲੀ)
{
    ਜੇ (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (ਲੰਮਾ)quickfix_busy);
#  ifdef ABORT_ON_ਅੰਦਰੂਨੀ_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਜੋੜੋ_ਇੰਦਰਾਜ(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    ਪੂਰਨ		bufnum,		// buffer number or zero
    char_u	*mesg,		//ਸੁਨੇਹਾ
    ਲੰਮਾ	lnum,		// line number
    ਲੰਮਾ	ਸਿਰਾ_lnum,	// line number for end
    ਪੂਰਨ		ਖਾਨਾ,		//ਕਾਲਮ
    ਪੂਰਨ		ਸਿਰਾ_ਖਾਨਾ,	// column for end
    ਪੂਰਨ		vis_ਖਾਨਾ,	// using visual column
    char_u	*ਨਮੂਨਾ,	// search pattern
    ਪੂਰਨ		nr,		// error number
    ਪੂਰਨ		type,		// type character
    typval_T	*user_ਅੰਕੜੇ,     // custom user data or NULL
    ਪੂਰਨ		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ਨਿਰਾ;
    char_u	*p = ਨਿਰਾ;

    ਜੇ ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ਨਿਰਾ)
	ਵਾਪਸ QF_FAIL;
    ਜੇ (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	ਜੇ (buf != ਨਿਰਾ)
	    buf->b_has_qf_ਇੰਦਰਾਜ |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ਇੰਦਰਾਜ : BUF_HAS_LL_ਇੰਦਰਾਜ;
    }
    ਨਹੀਂਤਾਂ
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    ਜੇ (fname != ਨਿਰਾ)
	fullname = fix_fname(fname);
    qfp->qf_fname = ਨਿਰਾ;
    ਜੇ (buf != ਨਿਰਾ &&
	buf->b_ffname != ਨਿਰਾ && fullname != ਨਿਰਾ)
    {
	ਜੇ (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    ਜੇ (p != ਨਿਰਾ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    ਜੇ ((qfp->qf_ਪਾਠ = vim_strsave(mesg)) == ਨਿਰਾ)
    {
	vim_free(qfp);
	ਵਾਪਸ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_ਸਿਰਾ_lnum = ਸਿਰਾ_lnum;
    qfp->qf_ਖਾਨਾ = ਖਾਨਾ;
    qfp->qf_ਸਿਰਾ_ਖਾਨਾ = ਸਿਰਾ_ਖਾਨਾ;
    qfp->qf_viscol = vis_ਖਾਨਾ;
    ਜੇ (user_ਅੰਕੜੇ == ਨਿਰਾ || user_ਅੰਕੜੇ->v_type == VAR_UNKNOWN)
	qfp->qf_user_ਅੰਕੜੇ.v_type = VAR_UNKNOWN;
    ਨਹੀਂਤਾਂ
    {
	copy_tv(user_ਅੰਕੜੇ, &qfp->qf_user_ਅੰਕੜੇ);
	qfl->qf_has_user_ਅੰਕੜੇ = TRUE;
    }
    ਜੇ (ਨਮੂਨਾ == ਨਿਰਾ || *ਨਮੂਨਾ == NUL)
	qfp->qf_ਨਮੂਨਾ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ ((qfp->qf_ਨਮੂਨਾ = vim_strsave(ਨਮੂਨਾ)) == ਨਿਰਾ)
    {
	vim_free(qfp->qf_ਪਾਠ);
	vim_free(qfp);
	ਵਾਪਸ QF_FAIL;
    }
    ਜੇ (module == ਨਿਰਾ || *module == NUL)
	qfp->qf_module = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ ((qfp->qf_module = vim_strsave(module)) == ਨਿਰਾ)
    {
	vim_free(qfp->qf_ਪਾਠ);
	vim_free(qfp->qf_ਨਮੂਨਾ);
	vim_free(qfp);
	ਵਾਪਸ QF_FAIL;
    }
    qfp->qf_nr = nr;
    ਜੇ (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_ਆਖਰੀ;
    ਜੇ (qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))		// first element in the list
    {
	qfl->qf_ਸ਼ੁਰੂ = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_ਸੂਚਕਾਂਕ = 0;
	qfp->qf_prev = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_ਅਗਲਾ = qfp;
    }
    qfp->qf_ਅਗਲਾ = ਨਿਰਾ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_ਗਿਣਤੀ;
    ਜੇ (qfl->qf_ਸੂਚਕਾਂਕ == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_ਸੂਚਕਾਂਕ = qfl->qf_ਗਿਣਤੀ;
	qfl->qf_ptr = qfp;
    }

    ਵਾਪਸ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    ਪੂਰਨ
qf_resize_ਢੇਰ(ਪੂਰਨ n)
{
    ਜੇ (ql_info == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (qf_resize_ਢੇਰ_ਅਧਾਰ(ql_info, n) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    ਪੂਰਨ
ll_resize_ਢੇਰ(win_T *wp, ਪੂਰਨ n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    ਜੇ (IS_LL_ਖਿੜਕੀ(wp))
	qf_sync_llw_ਮੰਜ਼ਿਲ_win(wp);
    ਨਹੀਂਤਾਂ
	qf_sync_win_ਮੰਜ਼ਿਲ_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (qf_resize_ਢੇਰ_ਅਧਾਰ(qi, n) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_resize_ਢੇਰ_ਅਧਾਰ(qf_info_T *qi, ਪੂਰਨ n)
{
    qf_list_T *new;
    ਪੂਰਨ amount_ਮੰਜ਼ਿਲ_rm = 0, i;

    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    size_t lsz = ਆਕਾਰ(*qi->qf_lists);

    ਜੇ (n == qi->qf_maxcount)
	ਵਾਪਸ OK;
    ਨਹੀਂਤਾਂ ਜੇ (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_ਮੰਜ਼ਿਲ_rm = qi->qf_listcount - n;

	ਲਈ (i = 0; i < amount_ਮੰਜ਼ਿਲ_rm; i++)
	    qf_ਕੱਢੋ_ਢੇਰ(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    ਜੇ (new == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // fill with zeroes any newly allocated memory
    ਜੇ (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_ਬਫਰ(qi, ਨਿਰਾ);

    ਵਾਪਸ OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   ਖਾਲੀ
qf_init_ਢੇਰ(ਖਾਲੀ)
{
    ql_info = qf_alloc_ਢੇਰ(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    ਸਥਿਰ ਖਾਲੀ
qf_sync_llw_ਮੰਜ਼ਿਲ_win(win_T *llw)
{
    win_T *wp = qf_ਲੱਭੋ_win_with_loclist(llw->w_llist_ref);

    ਜੇ (wp != ਨਿਰਾ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ਸਥਿਰ ਖਾਲੀ
qf_sync_win_ਮੰਜ਼ਿਲ_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    ਜੇ (llw != ਨਿਰਾ)
	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_llist_ref == llw && bt_quickfix(wp->w_ਬਫਰ))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		ਵਾਪਸ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ਸਥਿਰ qf_info_T *
qf_alloc_ਢੇਰ(qfltype_T qfltype, ਪੂਰਨ n)
{
    qf_info_T *qi;

    ਜੇ (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    ਨਹੀਂਤਾਂ
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	ਜੇ (qi == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ਢੇਰ(n);
    ਜੇ (qi->qf_lists == ਨਿਰਾ)
    {
	ਜੇ (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	ਵਾਪਸ ਨਿਰਾ;
    }
    qi->qf_maxcount = n;

    ਵਾਪਸ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ਸਥਿਰ qf_list_T *
qf_alloc_list_ਢੇਰ(ਪੂਰਨ n)
{
    ਵਾਪਸ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ਸਥਿਰ qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    ਜੇ (IS_LL_ਖਿੜਕੀ(wp))
	// For a location list window, use the referenced location list
	ਵਾਪਸ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    ਜੇ (wp->w_llist == ਨਿਰਾ)
	// new location list
	wp->w_llist = qf_alloc_ਢੇਰ(QFLT_LOCATION, wp->w_p_lhi);

    ਵਾਪਸ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ਸਥਿਰ qf_info_T *
qf_cmd_get_ਢੇਰ(exarg_T *eap, ਪੂਰਨ print_emsg)
{
    qf_info_T	*qi = ql_info;

    ਜੇ (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	ਜੇ (qi == ਨਿਰਾ)
	{
	    ਜੇ (print_emsg)
		emsg(_(e_no_location_list));
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਜੇ (qi == ਨਿਰਾ && print_emsg)
	emsg(_(e_no_quickfix_ਢੇਰ));

    ਵਾਪਸ qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    ਸਥਿਰ qf_info_T *
qf_cmd_get_or_alloc_ਢੇਰ(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    ਜੇ (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	ਜੇ (qi == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	*pwinp = curwin;
    }

    ਵਾਪਸ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ਸਥਿਰ ਪੂਰਨ
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *ਮੰਜ਼ਿਲ_qfl)
{
    ਪੂਰਨ		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(from_qfl, from_qfp, i)
    {
	ਜੇ (qf_ਜੋੜੋ_ਇੰਦਰਾਜ(ਮੰਜ਼ਿਲ_qfl,
		    ਨਿਰਾ,
		    ਨਿਰਾ,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_ਪਾਠ,
		    from_qfp->qf_lnum,
		    from_qfp->qf_ਸਿਰਾ_lnum,
		    from_qfp->qf_ਖਾਨਾ,
		    from_qfp->qf_ਸਿਰਾ_ਖਾਨਾ,
		    from_qfp->qf_viscol,
		    from_qfp->qf_ਨਮੂਨਾ,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_ਅੰਕੜੇ,
		    from_qfp->qf_valid) == QF_FAIL)
	    ਵਾਪਸ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = ਮੰਜ਼ਿਲ_qfl->qf_ਆਖਰੀ;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	ਜੇ (from_qfl->qf_ptr == from_qfp)
	    ਮੰਜ਼ਿਲ_qfl->qf_ptr = prevp;		// current location
    }

    ਵਾਪਸ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ਸਥਿਰ ਪੂਰਨ
copy_loclist(qf_list_T *from_qfl, qf_list_T *ਮੰਜ਼ਿਲ_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    ਮੰਜ਼ਿਲ_qfl->qfl_type = from_qfl->qfl_type;
    ਮੰਜ਼ਿਲ_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    ਮੰਜ਼ਿਲ_qfl->qf_has_user_ਅੰਕੜੇ = from_qfl->qf_has_user_ਅੰਕੜੇ;
    ਮੰਜ਼ਿਲ_qfl->qf_ਗਿਣਤੀ = 0;
    ਮੰਜ਼ਿਲ_qfl->qf_ਸੂਚਕਾਂਕ = 0;
    ਮੰਜ਼ਿਲ_qfl->qf_ਸ਼ੁਰੂ = ਨਿਰਾ;
    ਮੰਜ਼ਿਲ_qfl->qf_ਆਖਰੀ = ਨਿਰਾ;
    ਮੰਜ਼ਿਲ_qfl->qf_ptr = ਨਿਰਾ;
    ਜੇ (from_qfl->qf_title != ਨਿਰਾ)
	ਮੰਜ਼ਿਲ_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    ਨਹੀਂਤਾਂ
	ਮੰਜ਼ਿਲ_qfl->qf_title = ਨਿਰਾ;
    ਜੇ (from_qfl->qf_ctx != ਨਿਰਾ)
    {
	ਮੰਜ਼ਿਲ_qfl->qf_ctx = alloc_tv();
	ਜੇ (ਮੰਜ਼ਿਲ_qfl->qf_ctx != ਨਿਰਾ)
	    copy_tv(from_qfl->qf_ctx, ਮੰਜ਼ਿਲ_qfl->qf_ctx);
    }
    ਨਹੀਂਤਾਂ
	ਮੰਜ਼ਿਲ_qfl->qf_ctx = ਨਿਰਾ;
    ਜੇ (from_qfl->qf_qftf_cb.cb_name != ਨਿਰਾ)
	copy_callback(&ਮੰਜ਼ਿਲ_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    ਨਹੀਂਤਾਂ
	ਮੰਜ਼ਿਲ_qfl->qf_qftf_cb.cb_name = ਨਿਰਾ;

    ਜੇ (from_qfl->qf_ਗਿਣਤੀ)
	ਜੇ (copy_loclist_entries(from_qfl, ਮੰਜ਼ਿਲ_qfl) == FAIL)
	    ਵਾਪਸ FAIL;

    ਮੰਜ਼ਿਲ_qfl->qf_ਸੂਚਕਾਂਕ = from_qfl->qf_ਸੂਚਕਾਂਕ;	// current index in the list

    // Assign a new ID for the location list
    ਮੰਜ਼ਿਲ_qfl->qf_id = ++ਆਖਰੀ_qf_id;
    ਮੰਜ਼ਿਲ_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    ਜੇ (ਮੰਜ਼ਿਲ_qfl->qf_nonevalid)
    {
	ਮੰਜ਼ਿਲ_qfl->qf_ptr = ਮੰਜ਼ਿਲ_qfl->qf_ਸ਼ੁਰੂ;
	ਮੰਜ਼ਿਲ_qfl->qf_ਸੂਚਕਾਂਕ = 1;
    }

    ਵਾਪਸ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    ਖਾਲੀ
copy_loclist_ਢੇਰ(win_T *from, win_T *ਮੰਜ਼ਿਲ)
{
    qf_info_T	*qi;
    ਪੂਰਨ		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    ਜੇ (IS_LL_ਖਿੜਕੀ(from))
	qi = from->w_llist_ref;
    ਨਹੀਂਤਾਂ
	qi = from->w_llist;

    ਜੇ (qi == ਨਿਰਾ)		    // no location list to copy
	ਵਾਪਸ;

    // allocate a new location list, set size of stack to 'from' window value
    ਜੇ ((ਮੰਜ਼ਿਲ->w_llist = qf_alloc_ਢੇਰ(QFLT_LOCATION, from->w_p_lhi)) == ਨਿਰਾ)
	ਵਾਪਸ;
    ਨਹੀਂਤਾਂ
	// set 'to' lhi to reflect new value
	ਮੰਜ਼ਿਲ->w_p_lhi = ਮੰਜ਼ਿਲ->w_llist->qf_maxcount;

    ਮੰਜ਼ਿਲ->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    ਲਈ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	ਮੰਜ਼ਿਲ->w_llist->qf_curlist = idx;

	ਜੇ (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(ਮੰਜ਼ਿਲ->w_llist, idx)) == FAIL)
	{
	    qf_free_all(ਮੰਜ਼ਿਲ);
	    ਵਾਪਸ;
	}
    }

    ਮੰਜ਼ਿਲ->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ਨਿਰਾ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    ਜੇ (fname == ਨਿਰਾ || *fname == NUL)		// no file name
	ਵਾਪਸ 0;

# ifdef VMS
    vms_remove_ਸੰਸਕਰਣ(fname);
# endif
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (directory != ਨਿਰਾ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    ਜੇ (directory != ਨਿਰਾ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ਨਿਰਾ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	ਜੇ (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    ਜੇ (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    ਨਹੀਂਤਾਂ
	    {
		ptr.ਲੰਮਾਈ = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    ਜੇ (ptr.string == ਨਿਰਾ)
		ਵਾਪਸ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.ਲੰਮਾਈ = ptr.ਲੰਮਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	bufname.string = fname;
	bufname.ਲੰਮਾਈ = fname_len;
    }

    ਜੇ (qf_ਆਖਰੀ_bufname != ਨਿਰਾ && STRCMP(bufname.string, qf_ਆਖਰੀ_bufname) == 0
	    && bufref_valid(&qf_ਆਖਰੀ_bufref))
    {
	buf = qf_ਆਖਰੀ_bufref.br_buf;
	vim_free(ptr.string);
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(qf_ਆਖਰੀ_bufname);
	buf = buflist_new(bufname.string, ਨਿਰਾ, (linenr_T)0, BLN_NOOPT);
	ਜੇ (bufname.string == ptr.string)
	    qf_ਆਖਰੀ_bufname = bufname.string;
	ਨਹੀਂਤਾਂ
	    qf_ਆਖਰੀ_bufname = vim_strnsave(bufname.string, bufname.ਲੰਮਾਈ);
	ਸੈੱਟ_bufref(&qf_ਆਖਰੀ_bufref, buf);
    }
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ 0;

    buf->b_has_qf_ਇੰਦਰਾਜ =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ਇੰਦਰਾਜ : BUF_HAS_LL_ਇੰਦਰਾਜ;
    ਵਾਪਸ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ਸਥਿਰ char_u *
qf_ਧੱਕੋ_dir(char_u *dirbuf, ਬਣਤਰ dir_ਢੇਰ_T **stackptr, ਪੂਰਨ is_file_ਢੇਰ)
{
    ਬਣਤਰ dir_ਢੇਰ_T  *ds_new;
    ਬਣਤਰ dir_ਢੇਰ_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ਬਣਤਰ dir_ਢੇਰ_T, aid_qf_dirstack);
    ਜੇ (ds_new == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // push the new element onto the stack
    ds_new->ਅਗਲਾ = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    ਜੇ (vim_isAbsName(dirbuf)
	    || (*stackptr)->ਅਗਲਾ == ਨਿਰਾ
	    || is_file_ਢੇਰ)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    ਨਹੀਂਤਾਂ
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->ਅਗਲਾ;
	(*stackptr)->dirname = ਨਿਰਾ;
	ਜਦੋਂ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    ਜੇ (dirname.string == ਨਿਰਾ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->ਅਗਲਾ;
		vim_free(ds_ptr);

		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਜੇ (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ਰੋਕੋ;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->ਅਗਲਾ;
	}

	// clean up all dirs we already left
	ਜਦੋਂ ((*stackptr)->ਅਗਲਾ != ds_new)
	{
	    ds_ptr = (*stackptr)->ਅਗਲਾ;
	    (*stackptr)->ਅਗਲਾ = (*stackptr)->ਅਗਲਾ->ਅਗਲਾ;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	ਜੇ (ds_new == ਨਿਰਾ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    ਜੇ ((*stackptr)->dirname != ਨਿਰਾ)
	ਵਾਪਸ (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->ਅਗਲਾ;
    vim_free(ds_ptr);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ਸਥਿਰ char_u *
qf_ਕੱਢੋ_dir(ਬਣਤਰ dir_ਢੇਰ_T **stackptr)
{
    ਬਣਤਰ dir_ਢੇਰ_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    ਜੇ (*stackptr != ਨਿਰਾ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->ਅਗਲਾ;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    ਵਾਪਸ *stackptr ? (*stackptr)->dirname : ਨਿਰਾ;
}

/*
 * clean up directory stack
 */
    ਸਥਿਰ ਖਾਲੀ
qf_clean_dir_ਢੇਰ(ਬਣਤਰ dir_ਢੇਰ_T **stackptr)
{
    ਬਣਤਰ dir_ਢੇਰ_T  *ds_ptr;

    ਜਦੋਂ ((ds_ptr = *stackptr) != ਨਿਰਾ)
    {
	*stackptr = (*stackptr)->ਅਗਲਾ;
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
    ਸਥਿਰ char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ਬਣਤਰ dir_ਢੇਰ_T     *ds_ptr;
    ਬਣਤਰ dir_ਢੇਰ_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    ਜੇ (qfl->qf_dir_ਢੇਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ds_ptr = qfl->qf_dir_ਢੇਰ->ਅਗਲਾ;
    fullname.string = ਨਿਰਾ;
    fullname.ਲੰਮਾਈ = 0;
    filename_len = STRLEN(filename);
    ਜਦੋਂ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	ਜੇ ((fullname.string != ਨਿਰਾ) && (mch_getperm(fullname.string) >= 0))
	    ਰੋਕੋ;

	ds_ptr = ds_ptr->ਅਗਲਾ;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    ਜਦੋਂ (qfl->qf_dir_ਢੇਰ->ਅਗਲਾ != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ਢੇਰ->ਅਗਲਾ;
	qfl->qf_dir_ਢੇਰ->ਅਗਲਾ = qfl->qf_dir_ਢੇਰ->ਅਗਲਾ->ਅਗਲਾ;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    ਵਾਪਸ ds_ptr == ਨਿਰਾ ? ਨਿਰਾ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ਸਥਿਰ ਪੂਰਨ
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    ਪੂਰਨ		i;

    ਜੇ (wp != ਨਿਰਾ)
    {
	ਜੇ (!win_valid(wp))
	    ਵਾਪਸ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; i < qi->qf_listcount; ++i)
	ਜੇ (qi->qf_lists[i].qf_id == qf_id)
	    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ਸਥਿਰ ਪੂਰਨ
is_qf_ਇੰਦਰਾਜ_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    ਪੂਰਨ		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
	ਜੇ (qfp == qf_ptr)
	    ਰੋਕੋ;

    ਜੇ (i > qfl->qf_ਗਿਣਤੀ) // Entry is not found
	ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ਸਥਿਰ qfline_T *
get_ਅਗਲਾ_valid_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ਪੂਰਨ		*qf_ਸੂਚਕਾਂਕ,
	ਪੂਰਨ		dir)
{
    ਪੂਰਨ			idx;
    ਪੂਰਨ			old_qf_fnum;

    idx = *qf_ਸੂਚਕਾਂਕ;
    old_qf_fnum = qf_ptr->qf_fnum;

    ਕਰੋ
    {
	ਜੇ (idx == qfl->qf_ਗਿਣਤੀ || qf_ptr->qf_ਅਗਲਾ == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	++idx;
	qf_ptr = qf_ptr->qf_ਅਗਲਾ;
    } ਜਦੋਂ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_ਸੂਚਕਾਂਕ = idx;
    ਵਾਪਸ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ਸਥਿਰ qfline_T *
get_prev_valid_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ਪੂਰਨ		*qf_ਸੂਚਕਾਂਕ,
	ਪੂਰਨ		dir)
{
    ਪੂਰਨ			idx;
    ਪੂਰਨ			old_qf_fnum;

    idx = *qf_ਸੂਚਕਾਂਕ;
    old_qf_fnum = qf_ptr->qf_fnum;

    ਕਰੋ
    {
	ਜੇ (idx == 1 || qf_ptr->qf_prev == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } ਜਦੋਂ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_ਸੂਚਕਾਂਕ = idx;
    ਵਾਪਸ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ਸਥਿਰ qfline_T *
get_nth_valid_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	ਪੂਰਨ		errornr,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    ਪੂਰਨ			qf_idx = qfl->qf_ਸੂਚਕਾਂਕ;
    qfline_T		*prev_qf_ptr;
    ਪੂਰਨ			prev_ਸੂਚਕਾਂਕ;
    ਅੱਖਰ		*err = e_no_ਹੋਰ_ਚੀਜ਼ਾਂ;

    ਜਦੋਂ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_ਸੂਚਕਾਂਕ = qf_idx;

	ਜੇ (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_ਅਗਲਾ_valid_ਇੰਦਰਾਜ(qfl, qf_ptr, &qf_idx, dir);
	ਨਹੀਂਤਾਂ
	    qf_ptr = get_prev_valid_ਇੰਦਰਾਜ(qfl, qf_ptr, &qf_idx, dir);
	ਜੇ (qf_ptr == ਨਿਰਾ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_ਸੂਚਕਾਂਕ;
	    ਜੇ (err != ਨਿਰਾ)
	    {
		emsg(_(err));
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ਰੋਕੋ;
	}

	err = ਨਿਰਾ;
    }

    *new_qfidx = qf_idx;
    ਵਾਪਸ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ਸਥਿਰ qfline_T *
get_nth_ਇੰਦਰਾਜ(qf_list_T *qfl, ਪੂਰਨ errornr, ਪੂਰਨ *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ਪੂਰਨ		qf_idx = qfl->qf_ਸੂਚਕਾਂਕ;

    // New error number is less than the current error number
    ਜਦੋਂ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ਨਿਰਾ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    ਜਦੋਂ (errornr > qf_idx && qf_idx < qfl->qf_ਗਿਣਤੀ &&
						qf_ptr->qf_ਅਗਲਾ != ਨਿਰਾ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_ਅਗਲਾ;
    }

    *new_qfidx = qf_idx;
    ਵਾਪਸ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ਸਥਿਰ qfline_T *
qf_get_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	ਪੂਰਨ		errornr,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ਪੂਰਨ		qfidx = qfl->qf_ਸੂਚਕਾਂਕ;

    ਜੇ (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_ਇੰਦਰਾਜ(qfl, errornr, dir, &qfidx);
    ਨਹੀਂਤਾਂ ਜੇ (errornr != 0)	// go to specified number
	qf_ptr = get_nth_ਇੰਦਰਾਜ(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    ਵਾਪਸ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ਸਥਿਰ win_T *
qf_ਲੱਭੋ_help_win(ਖਾਲੀ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (bt_help(wp->w_ਬਫਰ))
	    ਵਾਪਸ wp;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ਸਥਿਰ ਖਾਲੀ
win_ਸੈੱਟ_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ਸਥਿਰ ਪੂਰਨ
jump_ਮੰਜ਼ਿਲ_help_ਖਿੜਕੀ(qf_info_T *qi, ਪੂਰਨ newwin, ਪੂਰਨ *opened_ਖਿੜਕੀ)
{
    win_T	*wp;
    ਪੂਰਨ		flags;

    ਜੇ (cmdmod.cmod_tab != 0 || newwin)
	wp = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	wp = qf_ਲੱਭੋ_help_win();
    ਜੇ (wp != ਨਿਰਾ && wp->w_ਬਫਰ->b_nwindows > 0)
	win_enter(wp, TRUE);
    ਨਹੀਂਤਾਂ
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	ਜੇ (cmdmod.cmod_ਵੰਡੋ == 0 && curwin->w_ਚੌੜਾਈ != Columns
		&& curwin->w_ਚੌੜਾਈ < 80)
	    flags |= WSP_ਸਿਖਰ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	ਜੇ (IS_LL_ਢੇਰ(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	ਜੇ (win_ਵੰਡੋ(0, flags) == FAIL)
	    ਵਾਪਸ FAIL;

	*opened_ਖਿੜਕੀ = TRUE;

	ਜੇ (curwin->w_ਉਚਾਈ < p_hh)
	    win_setheight((ਪੂਰਨ)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	ਜੇ (IS_LL_ਢੇਰ(qi) && !newwin)
	    win_ਸੈੱਟ_loclist(curwin, qi);
    }

    ਜੇ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    ਵਾਪਸ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ਸਥਿਰ win_T *
qf_ਲੱਭੋ_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_llist == ll && !bt_quickfix(wp->w_ਬਫਰ))
	    ਵਾਪਸ wp;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ਸਥਿਰ win_T *
qf_ਲੱਭੋ_win_with_ਸਾਧਾਰਨ_buf(ਖਾਲੀ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (bt_ਸਾਧਾਰਨ(wp->w_ਬਫਰ))
	    ਵਾਪਸ wp;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_goto_tabwin_with_file(ਪੂਰਨ fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_ਬਫਰ->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    ਵਾਪਸ TRUE;
	}

    ਵਾਪਸ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_open_new_file_win(qf_info_T *ll_ref)
{
    ਪੂਰਨ		flags;

    flags = WSP_ABOVE;
    ਜੇ (ll_ref != ਨਿਰਾ)
	flags |= WSP_NEWLOC;
    ਜੇ (win_ਵੰਡੋ(0, flags) == FAIL)
	ਵਾਪਸ FAIL;		// not enough room for window
    p_swb = ਖਾਲੀ_ਥਾਵਾਂ_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    ਜੇ (ll_ref != ਨਿਰਾ)
	// The new window should use the location list from the
	// location list window
	win_ਸੈੱਟ_loclist(curwin, ll_ref);
    ਵਾਪਸ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_goto_win_with_ll_file(win_T *use_win, ਪੂਰਨ qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    ਜੇ (win == ਨਿਰਾ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    ਜੇ (win->w_ਬਫਰ->b_fnum == qf_fnum)
		ਰੋਕੋ;
	ਜੇ (win == ਨਿਰਾ)
	{
	    // Find a previous usable window
	    win = curwin;
	    ਕਰੋ
	    {
		ਜੇ (bt_ਸਾਧਾਰਨ(win->w_ਬਫਰ))
		    ਰੋਕੋ;
		ਜੇ (win->w_prev == ਨਿਰਾ)
		    win = lastwin;	// wrap around the top
		ਨਹੀਂਤਾਂ
		    win = win->w_prev; // go to previous window
	    } ਜਦੋਂ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    ਜੇ (win->w_llist == ਨਿਰਾ && ll_ref != ਨਿਰਾ)
	win_ਸੈੱਟ_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_goto_win_with_qfl_file(ਪੂਰਨ qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ਨਿਰਾ;
    ਲਈ (;;)
    {
	ਜੇ (win->w_ਬਫਰ->b_fnum == qf_fnum)
	    ਰੋਕੋ;
	ਜੇ (win->w_prev == ਨਿਰਾ)
	    win = lastwin;	// wrap around the top
	ਨਹੀਂਤਾਂ
	    win = win->w_prev;	// go to previous window

	ਜੇ (IS_QF_ਖਿੜਕੀ(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    ਜੇ ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    ਨਹੀਂਤਾਂ ਜੇ (altwin != ਨਿਰਾ)
		win = altwin;
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_prev != ਨਿਰਾ)
		win = curwin->w_prev;
	    ਨਹੀਂਤਾਂ
		win = curwin->w_ਅਗਲਾ;
	    ਰੋਕੋ;
	}

	// Remember a usable window.
	ਜੇ (altwin == ਨਿਰਾ && !win->w_p_pvw && !win->w_p_wfb &&
		bt_ਸਾਧਾਰਨ(win->w_ਬਫਰ))
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
    ਸਥਿਰ ਪੂਰਨ
qf_jump_ਮੰਜ਼ਿਲ_usable_ਖਿੜਕੀ(ਪੂਰਨ qf_fnum, ਪੂਰਨ newwin, ਪੂਰਨ *opened_ਖਿੜਕੀ)
{
    win_T	*usable_wp = ਨਿਰਾ;
    ਪੂਰਨ		usable_win = FALSE;
    qf_info_T	*ll_ref = ਨਿਰਾ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    ਜੇ (!newwin)
	ll_ref = curwin->w_llist_ref;

    ਜੇ (ll_ref != ਨਿਰਾ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_ਲੱਭੋ_win_with_loclist(ll_ref);
	ਜੇ (usable_wp != ਨਿਰਾ)
	    usable_win = TRUE;
    }

    ਜੇ (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_ਲੱਭੋ_win_with_ਸਾਧਾਰਨ_buf();
	ਜੇ (win != ਨਿਰਾ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    ਜੇ (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    ਜੇ ((ONE_ਖਿੜਕੀ && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	ਜੇ (qf_open_new_file_win(ll_ref) != OK)
	    ਵਾਪਸ FAIL;
	*opened_ਖਿੜਕੀ = TRUE;	// close it when fail
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (curwin->w_llist_ref != ਨਿਰਾ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	ਨਹੀਂਤਾਂ					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    ਵਾਪਸ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_jump_edit_ਬਫਰ(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ਪੂਰਨ		forceit,
	ਪੂਰਨ		prev_winid,
	ਪੂਰਨ		*opened_ਖਿੜਕੀ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ਪੂਰਨ		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		old_qf_curlist = qi->qf_curlist;
    ਪੂਰਨ		save_qfid = qfl->qf_id;

    ਜੇ (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	ਜੇ (!can_abandon(curbuf, forceit))
	{
	    no_write_ਸੁਨੇਹਾ();
	    ਵਾਪਸ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, (linenr_T)1,
		ECMD_HIDE + ECMD_ਸੈੱਟ_HELP,
		prev_winid == curwin->w_id ? curwin : ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	fnum = qf_ptr->qf_fnum;

	ਜੇ (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    ਜੇ (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_ਬਫਰ))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    ਜੇ (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		ਜੇ (win_ਵੰਡੋ(0, 0) == OK)
		    *opened_ਖਿੜਕੀ = TRUE;

		ਜੇ (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
		    retval = FAIL;
		}
	    }
	}

	ਜੇ (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    ਜੇ (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	ਜੇ (wp == ਨਿਰਾ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_ਖਿੜਕੀ_was_closed));
	    *opened_ਖਿੜਕੀ = FALSE;
	    ਵਾਪਸ QF_ABORT;
	}
    }

    ਜੇ (qfl_type == QFLT_QUICKFIX && !qflist_valid(ਨਿਰਾ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_ਬਦਲਿਆ));
	ਵਾਪਸ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    ਜੇ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ਇੰਦਰਾਜ_present(qfl, qf_ptr))
    {
	ਜੇ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ਬਦਲਿਆ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_current_location_list_was_ਬਦਲਿਆ));
	ਵਾਪਸ QF_ABORT;
    }

    ਵਾਪਸ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_screen_ਖਾਨਾ_ਮੰਜ਼ਿਲ_idx(colnr_T vcol)
{
    char_u	*ਸਤਰ = ml_get_curline();
    char_u	*p = ਸਤਰ;
    colnr_T	ਖਾਨਾ = 0;

    ਜਦੋਂ (*p != NUL && ਖਾਨਾ < vcol)
    {
	ਜੇ (*p == TAB)
	    ਖਾਨਾ += 8 - (ਖਾਨਾ % 8);
	ਨਹੀਂਤਾਂ
	    ਖਾਨਾ += ptr2cells(p);
	ਜੇ (ਖਾਨਾ > vcol)
	    ਰੋਕੋ;
	MB_PTR_ADV(p);
    }
    ਵਾਪਸ (ਪੂਰਨ)(p - ਸਤਰ);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_jump_goto_ਸਤਰ(
	linenr_T	qf_lnum,
	ਪੂਰਨ		qf_ਖਾਨਾ,
	char_u		qf_viscol,
	char_u		*qf_ਨਮੂਨਾ)
{
    linenr_T		i;

    ਜੇ (qf_ਨਮੂਨਾ == ਨਿਰਾ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	ਜੇ (i > 0)
	{
	    ਜੇ (i > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		i = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    curwin->w_cursor.lnum = i;
	}
	ਜੇ (qf_ਖਾਨਾ > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    ਜੇ (qf_viscol == TRUE)
		curwin->w_cursor.ਖਾਨਾ = qf_screen_ਖਾਨਾ_ਮੰਜ਼ਿਲ_idx(qf_ਖਾਨਾ - 1);
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.ਖਾਨਾ = qf_ਖਾਨਾ - 1;
	    curwin->w_ਸੈੱਟ_curswant = true;
	    check_cursor();
	}
	ਨਹੀਂਤਾਂ
	    beginline(BL_WHITE | BL_FIX);
    }
    ਨਹੀਂਤਾਂ
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	ਜੇ (!do_ਖੋਜ(ਨਿਰਾ, '/', '/', qf_ਨਮੂਨਾ, STRLEN(qf_ਨਮੂਨਾ), (ਲੰਮਾ)1, ਖੋਜ_KEEP, ਨਿਰਾ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ਸਥਿਰ ਖਾਲੀ
qf_jump_print_ਸੁਨੇਹਾ(
	qf_info_T	*qi,
	ਪੂਰਨ		qf_ਸੂਚਕਾਂਕ,
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
    ਜੇ (!ਸੁਨੇਹਾ_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_ਸੂਚਕਾਂਕ,
	    qf_get_curlist(qi)->qf_ਗਿਣਤੀ,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (ਅੱਖਰ *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_ਪਾਠ(gap, skipwhite(qf_ptr->qf_ਪਾਠ));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = ਸੁਨੇਹਾ_scroll;
    ਜੇ (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	ਸੁਨੇਹਾ_scroll = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (!ਸੁਨੇਹਾ_scrolled && shortmess(SHM_OVERALL))
	ਸੁਨੇਹਾ_scroll = FALSE;
    ਸੁਨੇਹਾ_attr_keep((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ, 0, TRUE);
    ਸੁਨੇਹਾ_scroll = i;

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
    ਸਥਿਰ ਪੂਰਨ
qf_jump_open_ਖਿੜਕੀ(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ਪੂਰਨ		newwin,
	ਪੂਰਨ		*opened_ਖਿੜਕੀ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ਪੂਰਨ		old_changedtick = qfl->qf_changedtick;
    ਪੂਰਨ		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    ਜੇ (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_ਬਫਰ)
						      || cmdmod.cmod_tab != 0))
	ਜੇ (jump_ਮੰਜ਼ਿਲ_help_ਖਿੜਕੀ(qi, newwin, opened_ਖਿੜਕੀ) == FAIL)
	    ਵਾਪਸ FAIL;
    ਜੇ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ਇੰਦਰਾਜ_present(qfl, qf_ptr))
    {
	ਜੇ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ਬਦਲਿਆ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_current_location_list_was_ਬਦਲਿਆ));
	ਵਾਪਸ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    ਜੇ (bt_quickfix(curbuf) && !*opened_ਖਿੜਕੀ)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	ਜੇ (qf_ptr->qf_fnum == 0)
	    ਵਾਪਸ NOTDONE;

	ਜੇ (qf_jump_ਮੰਜ਼ਿਲ_usable_ਖਿੜਕੀ(qf_ptr->qf_fnum, newwin,
						opened_ਖਿੜਕੀ) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ਇੰਦਰਾਜ_present(qfl, qf_ptr))
    {
	ਜੇ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ਬਦਲਿਆ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_current_location_list_was_ਬਦਲਿਆ));
	ਵਾਪਸ QF_ABORT;
    }

    ਵਾਪਸ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_jump_ਮੰਜ਼ਿਲ_ਬਫਰ(
	qf_info_T	*qi,
	ਪੂਰਨ		qf_ਸੂਚਕਾਂਕ,
	qfline_T	*qf_ptr,
	ਪੂਰਨ		forceit,
	ਪੂਰਨ		prev_winid,
	ਪੂਰਨ		*opened_ਖਿੜਕੀ,
	ਪੂਰਨ		openfold,
	ਪੂਰਨ		print_ਸੁਨੇਹਾ)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    ਪੂਰਨ		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    ਜੇ (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_ਬਫਰ(qi, qf_ptr, forceit, prev_winid,
						opened_ਖਿੜਕੀ);
	ਜੇ (retval != OK)
	    ਵਾਪਸ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    ਜੇ (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_ਸਤਰ(qf_ptr->qf_lnum, qf_ptr->qf_ਖਾਨਾ, qf_ptr->qf_viscol,
	    qf_ptr->qf_ਨਮੂਨਾ);

# ifdef FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    ਜੇ (print_ਸੁਨੇਹਾ)
	qf_jump_print_ਸੁਨੇਹਾ(qi, qf_ਸੂਚਕਾਂਕ, qf_ptr, old_curbuf, old_lnum);

    ਵਾਪਸ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    ਖਾਲੀ
qf_jump(qf_info_T	*qi,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		errornr,
	ਪੂਰਨ		forceit)
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
    ਸਥਿਰ ਖਾਲੀ
qf_jump_newwin(qf_info_T	*qi,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		errornr,
	ਪੂਰਨ		forceit,
	ਪੂਰਨ		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    ਪੂਰਨ			qf_ਸੂਚਕਾਂਕ;
    ਪੂਰਨ			old_qf_ਸੂਚਕਾਂਕ;
    char_u		*old_swb = p_swb;
    ਬਿਨਾਨਿਸ਼ਾਨੀ		old_swb_flags = swb_flags;
    ਪੂਰਨ			prev_winid;
    ਪੂਰਨ			opened_ਖਿੜਕੀ = FALSE;
    ਪੂਰਨ			print_ਸੁਨੇਹਾ = TRUE;
    ਪੂਰਨ			old_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed; // getting file may reset it
    ਪੂਰਨ			retval = OK;

    ਜੇ (qi == ਨਿਰਾ)
    {
	ਜੇ (ql_info == ਨਿਰਾ)
	{
	    emsg(_(e_no_quickfix_ਢੇਰ));
	    ਵਾਪਸ;
	}
	qi = ql_info;
    }

    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi) || qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ਵਾਪਸ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_ਸੂਚਕਾਂਕ = qfl->qf_ਸੂਚਕਾਂਕ;
    old_qf_ਸੂਚਕਾਂਕ = qf_ਸੂਚਕਾਂਕ;

    qf_ptr = qf_get_ਇੰਦਰਾਜ(qfl, errornr, dir, &qf_ਸੂਚਕਾਂਕ);
    ਜੇ (qf_ptr == ਨਿਰਾ)
    {
	qf_ptr = old_qf_ptr;
	qf_ਸੂਚਕਾਂਕ = old_qf_ਸੂਚਕਾਂਕ;
	ਜਾਓ theend;
    }

    qfl->qf_ਸੂਚਕਾਂਕ = qf_ਸੂਚਕਾਂਕ;
    qfl->qf_ptr = qf_ptr;
    ਜੇ (qf_win_pos_update(qi, old_qf_ਸੂਚਕਾਂਕ))
	// No need to print the error message if it's visible in the error
	//ਖਿੜਕੀ
	print_ਸੁਨੇਹਾ = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_ਖਿੜਕੀ(qi, qf_ptr, newwin, &opened_ਖਿੜਕੀ);
    ਜੇ (retval == FAIL)
	ਜਾਓ failed;
    ਜੇ (retval == QF_ABORT)
    {
	qi = ਨਿਰਾ;
	qf_ptr = ਨਿਰਾ;
	ਜਾਓ theend;
    }
    ਜੇ (retval == NOTDONE)
	ਜਾਓ theend;

    retval = qf_jump_ਮੰਜ਼ਿਲ_ਬਫਰ(qi, qf_ਸੂਚਕਾਂਕ, qf_ptr, forceit, prev_winid,
				  &opened_ਖਿੜਕੀ, old_ਕੁੰਜੀ_Typed, print_ਸੁਨੇਹਾ);
    ਜੇ (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ਨਿਰਾ;
	qf_ptr = ਨਿਰਾ;
    }

    ਜੇ (retval != OK)
    {
	ਜੇ (opened_ਖਿੜਕੀ)
	    win_close(curwin, TRUE);    // Close opened window
	ਜੇ (qf_ptr != ਨਿਰਾ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_ਸੂਚਕਾਂਕ = old_qf_ਸੂਚਕਾਂਕ;
	}
    }
theend:
    ਜੇ (qi != ਨਿਰਾ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_ਸੂਚਕਾਂਕ = qf_ਸੂਚਕਾਂਕ;
    }
    ਜੇ (p_swb != old_swb && p_swb == ਖਾਲੀ_ਥਾਵਾਂ_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ਸਥਿਰ ਪੂਰਨ	qfFileAttr;
ਸਥਿਰ ਪੂਰਨ	qfSepAttr;
ਸਥਿਰ ਪੂਰਨ	qf_ਸਤਰ_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_list_ਇੰਦਰਾਜ(qfline_T *qfp, ਪੂਰਨ qf_idx, ਪੂਰਨ cursel)
{
    char_u	*fname;
    buf_T	*buf;
    ਪੂਰਨ		filter_ਇੰਦਰਾਜ;
    garray_T	*gap;

    fname = ਨਿਰਾ;
    ਜੇ (qfp->qf_module != ਨਿਰਾ && *qfp->qf_module != NUL)
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(ਅੱਖਰ *)qfp->qf_module);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ਨਿਰਾ)
	{
	    ਜੇ (qfp->qf_fname == ਨਿਰਾ)
		fname = buf->b_fname;
	    ਨਹੀਂਤਾਂ
		fname = qfp->qf_fname;
	    ਜੇ (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	ਜੇ (fname == ਨਿਰਾ)
	    sprintf((ਅੱਖਰ *)IObuff, "%2d", qf_idx);
	ਨਹੀਂਤਾਂ
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (ਅੱਖਰ *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_ਇੰਦਰਾਜ = TRUE;
    ਜੇ (qfp->qf_module != ਨਿਰਾ && *qfp->qf_module != NUL)
	filter_ਇੰਦਰਾਜ &= ਸੁਨੇਹਾ_filtered(qfp->qf_module);
    ਜੇ (filter_ਇੰਦਰਾਜ && fname != ਨਿਰਾ)
	filter_ਇੰਦਰਾਜ &= ਸੁਨੇਹਾ_filtered(fname);
    ਜੇ (filter_ਇੰਦਰਾਜ && qfp->qf_ਨਮੂਨਾ != ਨਿਰਾ)
	filter_ਇੰਦਰਾਜ &= ਸੁਨੇਹਾ_filtered(qfp->qf_ਨਮੂਨਾ);
    ਜੇ (filter_ਇੰਦਰਾਜ)
	filter_ਇੰਦਰਾਜ &= ਸੁਨੇਹਾ_filtered(qfp->qf_ਪਾਠ);
    ਜੇ (filter_ਇੰਦਰਾਜ)
	ਵਾਪਸ;

    ਸੁਨੇਹਾ_putchar('\n');
    ਸੁਨੇਹਾ_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    ਜੇ (qfp->qf_lnum != 0)
	ਸੁਨੇਹਾ_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    ਜੇ (qfp->qf_lnum != 0)
	qf_range_ਪਾਠ(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ, qf_ਸਤਰ_Attr);
    ਸੁਨੇਹਾ_puts_attr(":", qfSepAttr);
    ਜੇ (qfp->qf_ਨਮੂਨਾ != ਨਿਰਾ)
    {
	gap = qfga_get();
	qf_fmt_ਪਾਠ(gap, qfp->qf_ਨਮੂਨਾ);
	ga_append(gap, NUL);
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ);
	ਸੁਨੇਹਾ_puts_attr(":", qfSepAttr);
    }
    ਸੁਨੇਹਾ_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_ਪਾਠ(gap, (fname != ਨਿਰਾ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_ਪਾਠ) : qfp->qf_ਪਾਠ);
    ga_append(gap, NUL);
    ਸੁਨੇਹਾ_prt_ਸਤਰ((char_u *)gap->ga_ਅੰਕੜੇ, FALSE);
    ਬਾਹਰਲਾ_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    ਖਾਲੀ
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		idx1 = 1;
    ਪੂਰਨ		idx2 = -1;
    char_u	*arg = eap->arg;
    ਪੂਰਨ		plus = FALSE;
    ਪੂਰਨ		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi) || qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ਵਾਪਸ;
    }
    ਜੇ (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    ਜੇ (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	ਵਾਪਸ;
    }
    qfl = qf_get_curlist(qi);
    ਜੇ (plus)
    {
	i = qfl->qf_ਸੂਚਕਾਂਕ;
	idx2 = i + idx1;
	idx1 = i;
    }
    ਨਹੀਂਤਾਂ
    {
	i = qfl->qf_ਗਿਣਤੀ;
	ਜੇ (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	ਜੇ (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    ਜੇ (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    ਜੇ (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_ਸਤਰ_Attr = syn_name2attr((char_u *)"qfLineNr");
    ਜੇ (qf_ਸਤਰ_Attr == 0)
	qf_ਸਤਰ_Attr = HL_ATTR(HLF_N);

    ਜੇ (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
    {
	ਜੇ ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_ਇੰਦਰਾਜ(qfp, i, i == qfl->qf_ਸੂਚਕਾਂਕ);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ਸਥਿਰ ਖਾਲੀ
qf_fmt_ਪਾਠ(garray_T *gap, char_u *ਪਾਠ)
{
    char_u	*p = ਪਾਠ;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (*p == '\n')
	{
	    ga_append(gap, ' ');
	    ਜਦੋਂ (*++p != NUL)
		ਜੇ (!VIM_ISWHITE(*p) && *p != '\n')
		    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ਸਥਿਰ ਖਾਲੀ
qf_range_ਪਾਠ(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    ਜੇ (qfp->qf_ਸਿਰਾ_lnum > 0 && qfp->qf_lnum != qfp->qf_ਸਿਰਾ_lnum)
    {
	buf.ਲੰਮਾਈ += vim_snprintf_safelen((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ,
	    IOSIZE - buf.ਲੰਮਾਈ, "-%ld", qfp->qf_ਸਿਰਾ_lnum);
    }
    ਜੇ (qfp->qf_ਖਾਨਾ > 0)
    {
	buf.ਲੰਮਾਈ += vim_snprintf_safelen((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ,
	    IOSIZE - buf.ਲੰਮਾਈ, " col %d", qfp->qf_ਖਾਨਾ);
	ਜੇ (qfp->qf_ਸਿਰਾ_ਖਾਨਾ > 0 && qfp->qf_ਖਾਨਾ != qfp->qf_ਸਿਰਾ_ਖਾਨਾ)
	    buf.ਲੰਮਾਈ += vim_snprintf_safelen((ਅੱਖਰ *)buf.string + buf.ਲੰਮਾਈ,
		IOSIZE - buf.ਲੰਮਾਈ, "-%d", qfp->qf_ਸਿਰਾ_ਖਾਨਾ);
    }

    ga_concat_len(gap, buf.string, buf.ਲੰਮਾਈ);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_ਸੁਨੇਹਾ(qf_info_T *qi, ਪੂਰਨ which, ਅੱਖਰ *lead)
{
    ਅੱਖਰ   *title = (ਅੱਖਰ *)qi->qf_lists[which].qf_title;
    ਪੂਰਨ    ਗਿਣਤੀ_2 = qi->qf_lists[which].qf_ਗਿਣਤੀ;
    char_u buf[IOSIZE];

    vim_snprintf((ਅੱਖਰ *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    ਗਿਣਤੀ_2);

    ਜੇ (title != ਨਿਰਾ)
    {
	size_t	len = STRLEN(buf);

	ਜੇ (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    ਸੁਨੇਹਾ((ਅੱਖਰ *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    ਖਾਲੀ
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    ਪੂਰਨ		ਗਿਣਤੀ_2;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (eap->addr_ਗਿਣਤੀ != 0)
	ਗਿਣਤੀ_2 = eap->line2;
    ਨਹੀਂਤਾਂ
	ਗਿਣਤੀ_2 = 1;
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	ਜੇ (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    ਜੇ (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_ਦਾ_quickfix_ਢੇਰ));
		ਰੋਕੋ;
	    }
	    --qi->qf_curlist;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_ਸਿਖਰ_ਦਾ_quickfix_ਢੇਰ));
		ਰੋਕੋ;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_ਸੁਨੇਹਾ(qi, qi->qf_curlist, "");
    qf_update_ਬਫਰ(qi, ਨਿਰਾ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    ਖਾਲੀ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ਢੇਰ(eap, FALSE);
    ਪੂਰਨ		i;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	ਜੇ (qi == ਨਿਰਾ)
	{
	    emsg(_(e_no_location_list));
	    ਵਾਪਸ;
	}

	// Jump to the specified quickfix list
	ਜੇ (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_ਸੁਨੇਹਾ(qi, qi->qf_curlist, "");
	    qf_update_ਬਫਰ(qi, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    emsg(_(e_invalid_range));

	ਵਾਪਸ;
    }

    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
	ਸੁਨੇਹਾ(_("No entries"));
    ਨਹੀਂਤਾਂ
	ਲਈ (i = 0; i < qi->qf_listcount; ++i)
	    qf_ਸੁਨੇਹਾ(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free_ਚੀਜ਼ਾਂ(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    ਪੂਰਨ		stop = FALSE;

    ਜਦੋਂ (qfl->qf_ਗਿਣਤੀ && qfl->qf_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	qfp = qfl->qf_ਸ਼ੁਰੂ;
	qfpnext = qfp->qf_ਅਗਲਾ;
	ਜੇ (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_ਪਾਠ);
	    vim_free(qfp->qf_ਨਮੂਨਾ);
	    clear_tv(&qfp->qf_user_ਅੰਕੜੇ);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    ਜੇ (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_ਗਿਣਤੀ = 1;
	    ਨਹੀਂਤਾਂ
		qfl->qf_ਸ਼ੁਰੂ = qfpnext;
	}
	--qfl->qf_ਗਿਣਤੀ;
    }

    qfl->qf_ਸੂਚਕਾਂਕ = 0;
    qfl->qf_ਸ਼ੁਰੂ = ਨਿਰਾ;
    qfl->qf_ਆਖਰੀ = ਨਿਰਾ;
    qfl->qf_ptr = ਨਿਰਾ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ਢੇਰ(&qfl->qf_dir_ਢੇਰ);
    qfl->qf_directory = ਨਿਰਾ;
    qf_clean_dir_ਢੇਰ(&qfl->qf_file_ਢੇਰ);
    qfl->qf_currfile = ਨਿਰਾ;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free(qf_list_T *qfl)
{
    qf_free_ਚੀਜ਼ਾਂ(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = ਨਿਰਾ;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   ਖਾਲੀ
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	ਲੰਮਾ	amount,
	ਲੰਮਾ	amount_after)
{
    ਪੂਰਨ		i;
    qfline_T	*qfp;
    ਪੂਰਨ		idx;
    qf_info_T	*qi = ql_info;
    ਪੂਰਨ		ਮਿਲਿਆ_one = FALSE;
    ਪੂਰਨ		buf_has_flag = wp == ਨਿਰਾ ? BUF_HAS_QF_ਇੰਦਰਾਜ : BUF_HAS_LL_ਇੰਦਰਾਜ;

    ਜੇ (!(curbuf->b_has_qf_ਇੰਦਰਾਜ & buf_has_flag))
	ਵਾਪਸ;
    ਜੇ (wp != ਨਿਰਾ)
    {
	ਜੇ (wp->w_llist == ਨਿਰਾ)
	    ਵਾਪਸ;
	qi = wp->w_llist;
    }
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	ਜੇ (!qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
	    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
		ਜੇ (qfp->qf_fnum == curbuf->b_fnum)
		{
		    ਮਿਲਿਆ_one = TRUE;
		    ਜੇ (qfp->qf_cleared)
			ਜਾਰੀ;

		    ਜੇ (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			ਜੇ (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			ਨਹੀਂਤਾਂ
			    qfp->qf_lnum += amount;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    ਜੇ (!ਮਿਲਿਆ_one)
	curbuf->b_has_qf_ਇੰਦਰਾਜ &= ~buf_has_flag;
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
    ਸਥਿਰ char_u *
qf_types(ਪੂਰਨ c, ਪੂਰਨ nr)
{
    ਸਥਿਰ char_u	buf[20];
    ਸਥਿਰ char_u	cc[3];
    char_u		*p;

    ਜੇ (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    ਨਹੀਂਤਾਂ ਜੇ (c == 'I' || c == 'i')
	p = (char_u *)" info";
    ਨਹੀਂਤਾਂ ਜੇ (c == 'N' || c == 'n')
	p = (char_u *)" note";
    ਨਹੀਂਤਾਂ ਜੇ (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    ਨਹੀਂਤਾਂ ਜੇ (c == 0 || c == 1)
	p = (char_u *)"";
    ਨਹੀਂਤਾਂ
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    ਜੇ (nr <= 0)
	ਵਾਪਸ p;

    sprintf((ਅੱਖਰ *)buf, "%s %3d", (ਅੱਖਰ *)p, nr);
    ਵਾਪਸ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    ਖਾਲੀ
qf_view_ਨਤੀਜਾ(ਪੂਰਨ ਵੰਡੋ)
{
    qf_info_T   *qi = ql_info;

    ਜੇ (IS_LL_ਖਿੜਕੀ(curwin))
	qi = GET_LOC_LIST(curwin);
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
    {
	emsg(_(e_no_quickfix_ਢੇਰ));
	ਵਾਪਸ;
    }

    ਜੇ (qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ਵਾਪਸ;
    }

    ਜੇ (ਵੰਡੋ)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (ਲੰਮਾ)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	ਵਾਪਸ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_ਖਿੜਕੀ(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    ਖਾਲੀ
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_ਲੱਭੋ_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi)
	    || qfl->qf_nonevalid
	    || qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
    {
	ਜੇ (win != ਨਿਰਾ)
	    ex_cclose(eap);
    }
    ਨਹੀਂਤਾਂ ਜੇ (win == ਨਿਰਾ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    ਖਾਲੀ
ex_cclose(exarg_T *eap)
{
    win_T	*win = ਨਿਰਾ;
    qf_info_T	*qi;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, FALSE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    // Find existing quickfix window and close it.
    win = qf_ਲੱਭੋ_win(qi);
    ਜੇ (win != ਨਿਰਾ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_ਸੈੱਟ_title_var(qf_list_T *qfl)
{
    ਜੇ (qfl->qf_title != ਨਿਰਾ)
	ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_goto_cwindow(qf_info_T *qi, ਪੂਰਨ resize, ਪੂਰਨ sz, ਪੂਰਨ vertsplit)
{
    win_T	*win;

    win = qf_ਲੱਭੋ_win(qi);
    ਜੇ (win == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    win_goto(win);
    ਜੇ (resize)
    {
	ਜੇ (vertsplit)
	{
	    ਜੇ (sz != win->w_ਚੌੜਾਈ)
		win_setwidth(sz);
	}
	ਨਹੀਂਤਾਂ ਜੇ (sz != win->w_ਉਚਾਈ && win->w_ਉਚਾਈ
		       + win->w_ਸਥਿਤੀ_ਉਚਾਈ + tabline_ਉਚਾਈ() < cmdline_ਪੰਕਤੀ)
	    win_setheight(sz);
    }

    ਵਾਪਸ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_ਸੈੱਟ_cwindow_options(ਖਾਲੀ)
{
    // switch off 'swapfile'
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"swf", 0L, ਨਿਰਾ, OPT_LOCAL);
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_open_new_cwindow(qf_info_T *qi, ਪੂਰਨ ਉਚਾਈ)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    ਪੂਰਨ		flags = 0;
    win_T	*win;

    qf_buf = qf_ਲੱਭੋ_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    ਜੇ (IS_QF_ਢੇਰ(qi) && cmdmod.cmod_ਵੰਡੋ == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    ਜੇ (cmdmod.cmod_ਵੰਡੋ == 0)
	flags = WSP_ਹੇਠਾਂ;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    ਜੇ (IS_QF_ਢੇਰ(qi))
	flags |= WSP_QUICKFIX;

    ਜੇ (win_ਵੰਡੋ(ਉਚਾਈ, flags) == FAIL)
	ਵਾਪਸ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    ਜੇ (IS_LL_ਢੇਰ(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    ਜੇ (oldwin != curwin)
	oldwin = ਨਿਰਾ;  // don't store info when in another window
    ਜੇ (qf_buf != ਨਿਰਾ)
    {
	// Use the existing quickfix buffer
	ਜੇ (do_ecmd(qf_buf->b_fnum, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	// Create a new quickfix buffer
	ਜੇ (do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    ਵਾਪਸ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    ਜੇ (!bt_quickfix(curbuf))
	qf_ਸੈੱਟ_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    ਜੇ (curtab == prevtab && curwin->w_ਚੌੜਾਈ == Columns)
	win_setheight(ਉਚਾਈ);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    ਜੇ (win_valid(win))
	prevwin = win;

    ਵਾਪਸ OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    ਖਾਲੀ
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ਪੂਰਨ		ਉਚਾਈ;
    ਪੂਰਨ		ਸਥਿਤੀ = FAIL;
    ਪੂਰਨ		lnum;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    incr_quickfix_busy();

    ਜੇ (eap->addr_ਗਿਣਤੀ != 0)
	ਉਚਾਈ = eap->line2;
    ਨਹੀਂਤਾਂ
	ਉਚਾਈ = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    ਜੇ (cmdmod.cmod_tab == 0)
	ਸਥਿਤੀ = qf_goto_cwindow(qi, eap->addr_ਗਿਣਤੀ != 0, ਉਚਾਈ,
						cmdmod.cmod_ਵੰਡੋ & WSP_VERT);
    ਜੇ (ਸਥਿਤੀ == FAIL)
	ਜੇ (qf_open_new_cwindow(qi, ਉਚਾਈ) == FAIL)
	{
	    decr_quickfix_busy();
	    ਵਾਪਸ;
	}

    qfl = qf_get_curlist(qi);
    qf_ਸੈੱਟ_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_ਸੂਚਕਾਂਕ;

    // Fill the buffer with the quickfix list.
    qf_fill_ਬਫਰ(qfl, curbuf, ਨਿਰਾ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ਖਾਨਾ = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    ਸਥਿਰ ਖਾਲੀ
qf_win_goto(win_T *win, linenr_T lnum)
{
    win_T	*old_curwin = curwin;

    curwin = win;
    curbuf = win->w_ਬਫਰ;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ਖਾਨਾ = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_ਸਥਿਤੀ = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_ਬਫਰ;
}

/*
 * :cbottom/:lbottom commands.
 */
    ਖਾਲੀ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    win = qf_ਲੱਭੋ_win(qi);
    ਜੇ (win != ਨਿਰਾ && win->w_cursor.lnum != win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	qf_win_goto(win, win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_ਇੰਦਰਾਜ(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ਜੇ (IS_LL_ਖਿੜਕੀ(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਵਾਪਸ qf_get_curlist(qi)->qf_ਸੂਚਕਾਂਕ;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_win_pos_update(
    qf_info_T	*qi,
    ਪੂਰਨ		old_qf_ਸੂਚਕਾਂਕ)	// previous qf_index or zero
{
    win_T	*win;
    ਪੂਰਨ		qf_ਸੂਚਕਾਂਕ = qf_get_curlist(qi)->qf_ਸੂਚਕਾਂਕ;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_ਲੱਭੋ_win(qi);
    ਜੇ (win != ਨਿਰਾ
	    && qf_ਸੂਚਕਾਂਕ <= win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    && old_qf_ਸੂਚਕਾਂਕ != qf_ਸੂਚਕਾਂਕ)
    {
	ਜੇ (qf_ਸੂਚਕਾਂਕ > old_qf_ਸੂਚਕਾਂਕ)
	{
	    win->w_redraw_ਸਿਖਰ = old_qf_ਸੂਚਕਾਂਕ;
	    win->w_redraw_bot = qf_ਸੂਚਕਾਂਕ;
	}
	ਨਹੀਂਤਾਂ
	{
	    win->w_redraw_ਸਿਖਰ = qf_ਸੂਚਕਾਂਕ;
	    win->w_redraw_bot = old_qf_ਸੂਚਕਾਂਕ;
	}
	qf_win_goto(win, qf_ਸੂਚਕਾਂਕ);
    }
    ਵਾਪਸ win != ਨਿਰਾ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ਸਥਿਰ ਪੂਰਨ
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    ਜੇ (buf_valid(win->w_ਬਫਰ) && bt_quickfix(win->w_ਬਫਰ))
	ਜੇ ((IS_QF_ਢੇਰ(qi) && win->w_llist_ref == ਨਿਰਾ)
		|| (IS_LL_ਢੇਰ(qi) && win->w_llist_ref == qi))
	    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ਸਥਿਰ win_T *
qf_ਲੱਭੋ_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ਜੇ (is_qf_win(win, qi))
	    ਵਾਪਸ win;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ਸਥਿਰ buf_T *
qf_ਲੱਭੋ_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    ਜੇ (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	ਜੇ (qfbuf != ਨਿਰਾ)
	    ਵਾਪਸ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	ਜੇ (is_qf_win(win, qi))
	    ਵਾਪਸ win->w_ਬਫਰ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_quickfixtextfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (option_ਸੈੱਟ_callback_func(p_qftf, &qftf_cb) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ਜੇ (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_ਸੈੱਟ_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_update_ਬਫਰ(qf_info_T *qi, qfline_T *old_ਆਖਰੀ)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_ਲੱਭੋ_buf(qi);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    linenr_T	old_ਸਤਰ_ਗਿਣਤੀ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਪੂਰਨ		qf_winid = 0;

    ਜੇ (IS_LL_ਢੇਰ(qi))
    {
	ਜੇ (curwin->w_llist == qi)
	    win = curwin;
	ਨਹੀਂਤਾਂ
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_ਲੱਭੋ_win_with_loclist(qi);
	    ਜੇ (win == ਨਿਰਾ)
		// File window is not found. Find the location list window.
		win = qf_ਲੱਭੋ_win(qi);
	    ਜੇ (win == ਨਿਰਾ)
		ਵਾਪਸ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    ਪੂਰਨ do_fill = TRUE;
    ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    ਜੇ (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_ਬਫਰ(qf_get_curlist(qi), buf, old_ਆਖਰੀ, qf_winid);
	++CHANGEDTICK(buf);

	ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
	{
	    (ਖਾਲੀ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    ਜੇ ((win = qf_ਲੱਭੋ_win(qi)) != ਨਿਰਾ && old_ਸਤਰ_ਗਿਣਤੀ < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_buf_ਜੋੜੋ_ਸਤਰ(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	ਪੂਰਨ		ਪਹਿਲਾ_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    ਜੇ (qftf_str != ਨਿਰਾ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (qfp->qf_module != ਨਿਰਾ)
	    ga_concat(gap, qfp->qf_module);
	ਨਹੀਂਤਾਂ ਜੇ (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ਨਿਰਾ
		&& errbuf->b_fname != ਨਿਰਾ)
	{
	    ਜੇ (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    ਨਹੀਂਤਾਂ
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		ਜੇ (ਪਹਿਲਾ_bufline && (errbuf->b_sfname == ਨਿਰਾ
				|| mch_is_ਭਰਿਆ_Name(errbuf->b_sfname)))
		{
		    ਜੇ (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		ਜੇ (qfp->qf_fname == ਨਿਰਾ)
		    ga_concat(gap, errbuf->b_fname);
		ਨਹੀਂਤਾਂ
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	ਜੇ (qfp->qf_lnum > 0)
	{
	    qf_range_ਪਾਠ(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	ਨਹੀਂਤਾਂ ਜੇ (qfp->qf_ਨਮੂਨਾ != ਨਿਰਾ)
	    qf_fmt_ਪਾਠ(gap, qfp->qf_ਨਮੂਨਾ);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_ਪਾਠ(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_ਪਾਠ)
							       : qfp->qf_ਪਾਠ);
    }

    ga_append(gap, NUL);
    ਜੇ (ml_append_buf(buf, lnum, gap->ga_ਅੰਕੜੇ, gap->ga_len, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ਸਥਿਰ list_T *
call_qftf_func(qf_list_T *qfl, ਪੂਰਨ qf_winid, ਲੰਮਾ ਸ਼ੁਰੂ_idx, ਲੰਮਾ ਸਿਰਾ_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	recursive = FALSE;

    ਜੇ (recursive)
	ਵਾਪਸ ਨਿਰਾ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    ਜੇ (qfl->qf_qftf_cb.cb_name != ਨਿਰਾ)
	cb = &qfl->qf_qftf_cb;
    ਜੇ (cb->cb_name != ਨਿਰਾ)
    {
	typval_T	ਦਲੀਲਾਂ[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	ਜੇ ((d = dict_alloc_ਤਾਲਾ(VAR_FIXED)) == ਨਿਰਾ)
	{
	    recursive = FALSE;
	    ਵਾਪਸ ਨਿਰਾ;
	}
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "quickfix", (ਲੰਮਾ)IS_QF_LIST(qfl));
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "winid", (ਲੰਮਾ)qf_winid);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "id", (ਲੰਮਾ)qfl->qf_id);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "start_idx", ਸ਼ੁਰੂ_idx);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "end_idx", ਸਿਰਾ_idx);
	++d->dv_refcount;
	ਦਲੀਲਾਂ[0].v_type = VAR_DICT;
	ਦਲੀਲਾਂ[0].vval.v_dict = d;

	qftf_list = ਨਿਰਾ;
	textlock++;
	ਜੇ (call_callback(cb, 0, &rettv, 1, ਦਲੀਲਾਂ) != FAIL)
	{
	    ਜੇ (rettv.v_type == VAR_LIST)
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
    ਵਾਪਸ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_fill_ਬਫਰ(qf_list_T *qfl, buf_T *buf, qfline_T *old_ਆਖਰੀ, ਪੂਰਨ qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    ਪੂਰਨ		old_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    list_T	*qftf_list = ਨਿਰਾ;
    listitem_T	*qftf_li = ਨਿਰਾ;

    ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	ਜੇ (buf != curbuf)
	{
	    ਅੰਦਰੂਨੀ_error("qf_fill_buffer()");
	    ਵਾਪਸ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	ਜਦੋਂ ((curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) == 0)
	    (ਖਾਲੀ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    ਜੇ (qfl != ਨਿਰਾ && qfl->qf_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	char_u		dirname[MAXPATHL];
	ਪੂਰਨ		invalid_val = FALSE;
	ਪੂਰਨ		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
	{
	    qfp = qfl->qf_ਸ਼ੁਰੂ;
	    lnum = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (old_ਆਖਰੀ->qf_ਅਗਲਾ != ਨਿਰਾ)
		qfp = old_ਆਖਰੀ->qf_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
		qfp = old_ਆਖਰੀ;
	    lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (ਲੰਮਾ)(lnum + 1),
							(ਲੰਮਾ)qfl->qf_ਗਿਣਤੀ);
	ਜੇ (qftf_list != ਨਿਰਾ)
	    qftf_li = qftf_list->lv_ਪਹਿਲਾ;

	ਜਦੋਂ (lnum < qfl->qf_ਗਿਣਤੀ)
	{
	    char_u	*qftf_str = ਨਿਰਾ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    ਜੇ (qftf_li != ਨਿਰਾ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		ਜੇ (qftf_str == ਨਿਰਾ)
		    invalid_val = TRUE;
	    }

	    ਜੇ (qf_buf_ਜੋੜੋ_ਸਤਰ(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ਰੋਕੋ;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_ਅਗਲਾ;
	    ਜੇ (qfp == ਨਿਰਾ)
		ਰੋਕੋ;

	    ਜੇ (qftf_li != ਨਿਰਾ)
		qftf_li = qftf_li->li_ਅਗਲਾ;
	}

	ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
	    // Delete the empty line which is now at the end
	    (ਖਾਲੀ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    ਜੇ (old_ਆਖਰੀ == ਨਿਰਾ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_ਤਾਲਾ;
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ਨਿਰਾ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ਨਿਰਾ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_ਤਾਲਾ;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    ਕੁੰਜੀ_Typed = old_ਕੁੰਜੀ_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_list_ਬਦਲਿਆ(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    ਪੂਰਨ		qf_idx;

    ਲਈ (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	ਜੇ (qi->qf_lists[qf_idx].qf_id == qfid)
	    ਵਾਪਸ qf_idx;
    ਵਾਪਸ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    ਪੂਰਨ curlist;

    ਜੇ (qf_get_curlist(qi)->qf_id == save_qfid)
	ਵਾਪਸ OK;

    curlist = qf_id2nr(qi, save_qfid);
    ਜੇ (curlist < 0)
	// list is not present
	ਵਾਪਸ FAIL;
    qi->qf_curlist = curlist;
    ਵਾਪਸ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_jump_ਪਹਿਲਾ(qf_info_T *qi, int_u save_qfid, ਪੂਰਨ forceit)
{
    ਜੇ (qf_restore_list(qi, save_qfid) == FAIL)
	ਵਾਪਸ;


    ਜੇ (!check_can_ਸੈੱਟ_curbuf_forceit(forceit))
	ਵਾਪਸ;


    // Autocommands might have cleared the list, check for that.
    ਜੇ (!qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    ਪੂਰਨ
grep_ਅੰਦਰੂਨੀ(cmdidx_T cmdidx)
{
    ਵਾਪਸ ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    ਸਥਿਰ char_u *
make_get_auname(cmdidx_T cmdidx)
{
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_make:	    ਵਾਪਸ (char_u *)"make";
	ਹਾਲਤ CMD_lmake:	    ਵਾਪਸ (char_u *)"lmake";
	ਹਾਲਤ CMD_grep:	    ਵਾਪਸ (char_u *)"grep";
	ਹਾਲਤ CMD_lgrep:	    ਵਾਪਸ (char_u *)"lgrep";
	ਹਾਲਤ CMD_grepadd:   ਵਾਪਸ (char_u *)"grepadd";
	ਹਾਲਤ CMD_lgrepadd:  ਵਾਪਸ (char_u *)"lgrepadd";
	ਮੂਲ: ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    ਸਥਿਰ char_u *
get_mef_name(ਖਾਲੀ)
{
    char_u	*p;
    char_u	*name;
    ਸਥਿਰ ਪੂਰਨ	ਸ਼ੁਰੂ = -1;
    ਸਥਿਰ ਪੂਰਨ	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    ਜੇ (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	ਵਾਪਸ name;
    }

    ਲਈ (p = p_mef; *p; ++p)
	ਜੇ (p[0] == '#' && p[1] == '#')
	    ਰੋਕੋ;

    ਜੇ (*p == NUL)
	ਵਾਪਸ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    ਲਈ (;;)
    {
	ਜੇ (ਸ਼ੁਰੂ == -1)
	    ਸ਼ੁਰੂ = mch_get_pid();
	ਨਹੀਂਤਾਂ
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	ਜੇ (name == ਨਿਰਾ)
	    ਰੋਕੋ;
	STRCPY(name, p_mef);
	sprintf((ਅੱਖਰ *)name + (p - p_mef), "%d%d", ਸ਼ੁਰੂ, off);
	STRCAT(name, p + 2);
	ਜੇ (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((ਅੱਖਰ *)name, &sb) < 0
# endif
		)
	    ਰੋਕੋ;
	vim_free(name);
    }
    ਵਾਪਸ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ਸਥਿਰ char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	len;

    len = (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(p_shq) * 2 + (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(makecmd) + 1;
    ਜੇ (*p_sp != NUL)
	len += (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(p_sp) + (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    ਜੇ (cmd == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    sprintf((ਅੱਖਰ *)cmd, "%s%s%s", (ਅੱਖਰ *)p_shq, (ਅੱਖਰ *)makecmd,
							       (ਅੱਖਰ *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    ਜੇ (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    ਜੇ (ਸੁਨੇਹਾ_ਖਾਨਾ == 0)
	ਸੁਨੇਹਾ_didout = FALSE;
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_puts(":!");
    ਸੁਨੇਹਾ_outtrans(cmd);		// show what we are doing

    ਵਾਪਸ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    ਖਾਲੀ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ਨਿਰਾ;
    win_T	*wp = ਨਿਰਾ;
    qf_info_T	*qi = ql_info;
    ਪੂਰਨ		res;
    char_u	*au_name = ਨਿਰਾ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    ਪੂਰਨ		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    ਜੇ (grep_ਅੰਦਰੂਨੀ(eap->cmdidx))
    {
	ex_vimgrep(eap);
	ਵਾਪਸ;
    }

    au_name = make_get_auname(eap->cmdidx);
    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ਜੇ (aborting())
	    ਵਾਪਸ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    ਜੇ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    ਜੇ (cmd == ਨਿਰਾ)
    {
	vim_free(fname);
	ਵਾਪਸ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    ਬਾਹਰਲਾ_flush();
		// read window status report and redraw before message
    (ਖਾਲੀ)char_avail();
# endif

    incr_quickfix_busy();

    ਜੇ (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    ਜੇ (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    ਜੇ (wp != ਨਿਰਾ)
    {
	qi = GET_LOC_LIST(wp);
	ਜੇ (qi == ਨਿਰਾ)
	    ਜਾਓ cleanup;
    }
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
	ਜਾਓ cleanup;

    ਜੇ (res >= 0)
	qf_list_ਬਦਲਿਆ(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ਜੇ (au_name != ਨਿਰਾ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    ਜੇ (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ਪਹਿਲਾ(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    ਪੂਰਨ
qf_get_ਮਾਪ(exarg_T *eap)
{
    qf_info_T	*qi;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, FALSE)) == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਵਾਪਸ qf_get_curlist(qi)->qf_ਗਿਣਤੀ;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    ਪੂਰਨ
qf_get_valid_ਮਾਪ(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ਪੂਰਨ		i, sz = 0;
    ਪੂਰਨ		prev_fnum = 0;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, FALSE)) == ਨਿਰਾ)
	ਵਾਪਸ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
    {
	ਜੇ (qfp->qf_valid)
	{
	    ਜੇ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    ਨਹੀਂਤਾਂ ਜੇ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    ਵਾਪਸ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    ਪੂਰਨ
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, FALSE)) == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਵਾਪਸ qf_get_curlist(qi)->qf_ਸੂਚਕਾਂਕ;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    ਪੂਰਨ
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ਪੂਰਨ		i, eidx = 0;
    ਪੂਰਨ		prev_fnum = 0;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, FALSE)) == ਨਿਰਾ)
	ਵਾਪਸ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_ਸ਼ੁਰੂ;

    // check if the list has valid errors
    ਜੇ (!qf_list_has_valid_entries(qfl))
	ਵਾਪਸ 1;

    ਲਈ (i = 1; i <= qfl->qf_ਸੂਚਕਾਂਕ && qfp!= ਨਿਰਾ; i++, qfp = qfp->qf_ਅਗਲਾ)
    {
	ਜੇ (qfp->qf_valid)
	{
	    ਜੇ (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ਜੇ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ਨਹੀਂਤਾਂ
		eidx++;
	}
    }

    ਵਾਪਸ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_nth_valid_ਇੰਦਰਾਜ(qf_list_T *qfl, ਪੂਰਨ n, ਪੂਰਨ fdo)
{
    qfline_T	*qfp;
    ਪੂਰਨ		i, eidx;
    ਪੂਰਨ		prev_fnum = 0;

    // check if the list has valid errors
    ਜੇ (!qf_list_has_valid_entries(qfl))
	ਵਾਪਸ 1;

    eidx = 0;
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
    {
	ਜੇ (qfp->qf_valid)
	{
	    ਜੇ (fdo)
	    {
		ਜੇ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ਨਹੀਂਤਾਂ
		eidx++;
	}

	ਜੇ (eidx == n)
	    ਰੋਕੋ;
    }

    ਜੇ (i <= qfl->qf_ਗਿਣਤੀ)
	ਵਾਪਸ i;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ਖਾਲੀ
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    ਪੂਰਨ		errornr;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	errornr = (ਪੂਰਨ)eap->line2;
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (eap->cmdidx)
	{
	    ਹਾਲਤ CMD_cc: ਹਾਲਤ CMD_ll:
		errornr = 0;
		ਰੋਕੋ;
	    ਹਾਲਤ CMD_crewind: ਹਾਲਤ CMD_lrewind: ਹਾਲਤ CMD_cfirst:
	    ਹਾਲਤ CMD_lfirst:
		errornr = 1;
		ਰੋਕੋ;
	    ਮੂਲ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    ਜੇ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_ਇੰਦਰਾਜ(qf_get_curlist(qi),
		eap->addr_ਗਿਣਤੀ > 0 ? (ਪੂਰਨ)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    ਖਾਲੀ
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    ਪੂਰਨ		errornr;
    ਪੂਰਨ		dir;

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (ਪੂਰਨ)eap->line2;
    ਨਹੀਂਤਾਂ
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_cnext: ਹਾਲਤ CMD_lnext: ਹਾਲਤ CMD_cdo: ਹਾਲਤ CMD_ldo:
	    dir = FORWARD;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_cprevious: ਹਾਲਤ CMD_lprevious: ਹਾਲਤ CMD_c_ਅਗਲਾ:
	ਹਾਲਤ CMD_l_ਅਗਲਾ:
	    dir = BACKWARD;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_cnfile: ਹਾਲਤ CMD_lnfile: ਹਾਲਤ CMD_cfdo: ਹਾਲਤ CMD_lfdo:
	    dir = FORWARD_FILE;
	    ਰੋਕੋ;
	ਹਾਲਤ CMD_cpfile: ਹਾਲਤ CMD_lpfile: ਹਾਲਤ CMD_cNfile: ਹਾਲਤ CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ਰੋਕੋ;
	ਮੂਲ:
	    dir = FORWARD;
	    ਰੋਕੋ;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ_ਵਿੱਚ_buf(qf_list_T *qfl, ਪੂਰਨ bnr, ਪੂਰਨ *errornr)
{
    qfline_T	*qfp = ਨਿਰਾ;
    ਪੂਰਨ		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, idx)
	ਜੇ (qfp->qf_fnum == bnr)
	    ਰੋਕੋ;

    *errornr = idx;
    ਵਾਪਸ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ_on_ਸਤਰ(qfline_T *ਇੰਦਰਾਜ, ਪੂਰਨ *errornr)
{
    ਜਦੋਂ (!got_int
	    && ਇੰਦਰਾਜ->qf_prev != ਨਿਰਾ
	    && ਇੰਦਰਾਜ->qf_fnum == ਇੰਦਰਾਜ->qf_prev->qf_fnum
	    && ਇੰਦਰਾਜ->qf_lnum == ਇੰਦਰਾਜ->qf_prev->qf_lnum)
    {
	ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->qf_prev;
	--*errornr;
    }

    ਵਾਪਸ ਇੰਦਰਾਜ;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_ਆਖਰੀ_ਇੰਦਰਾਜ_on_ਸਤਰ(qfline_T *ਇੰਦਰਾਜ, ਪੂਰਨ *errornr)
{
    ਜਦੋਂ (!got_int &&
	    ਇੰਦਰਾਜ->qf_ਅਗਲਾ != ਨਿਰਾ
	    && ਇੰਦਰਾਜ->qf_fnum == ਇੰਦਰਾਜ->qf_ਅਗਲਾ->qf_fnum
	    && ਇੰਦਰਾਜ->qf_lnum == ਇੰਦਰਾਜ->qf_ਅਗਲਾ->qf_lnum)
    {
	ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->qf_ਅਗਲਾ;
	++*errornr;
    }

    ਵਾਪਸ ਇੰਦਰਾਜ;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਇੰਦਰਾਜ_after_pos(qfline_T *qfp, pos_T *pos, ਪੂਰਨ linewise)
{
    ਜੇ (linewise)
	ਵਾਪਸ qfp->qf_lnum > pos->lnum;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ਖਾਨਾ > pos->ਖਾਨਾ));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਇੰਦਰਾਜ_before_pos(qfline_T *qfp, pos_T *pos, ਪੂਰਨ linewise)
{
    ਜੇ (linewise)
	ਵਾਪਸ qfp->qf_lnum < pos->lnum;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ਖਾਨਾ < pos->ਖਾਨਾ));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਇੰਦਰਾਜ_on_or_after_pos(qfline_T *qfp, pos_T *pos, ਪੂਰਨ linewise)
{
    ਜੇ (linewise)
	ਵਾਪਸ qfp->qf_lnum >= pos->lnum;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ਖਾਨਾ >= pos->ਖਾਨਾ));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਇੰਦਰਾਜ_on_or_before_pos(qfline_T *qfp, pos_T *pos, ਪੂਰਨ linewise)
{
    ਜੇ (linewise)
	ਵਾਪਸ qfp->qf_lnum <= pos->lnum;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ਖਾਨਾ <= pos->ਖਾਨਾ));
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
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_ਇੰਦਰਾਜ_after_pos(
	ਪੂਰਨ		bnr,
	pos_T		*pos,
	ਪੂਰਨ		linewise,
	qfline_T	*qfp,
	ਪੂਰਨ		*errornr)
{
    ਜੇ (qf_ਇੰਦਰਾਜ_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	ਵਾਪਸ qfp;

    // Find the entry just before or at the position 'pos'
    ਜਦੋਂ (qfp->qf_ਅਗਲਾ != ਨਿਰਾ
	    && qfp->qf_ਅਗਲਾ->qf_fnum == bnr
	    && qf_ਇੰਦਰਾਜ_on_or_before_pos(qfp->qf_ਅਗਲਾ, pos, linewise))
    {
	qfp = qfp->qf_ਅਗਲਾ;
	++*errornr;
    }

    ਜੇ (qfp->qf_ਅਗਲਾ == ਨਿਰਾ || qfp->qf_ਅਗਲਾ->qf_fnum != bnr)
	// No entries found after position 'pos'
	ਵਾਪਸ ਨਿਰਾ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_ਅਗਲਾ;
    ++*errornr;

    ਵਾਪਸ qfp;
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
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_ਇੰਦਰਾਜ_before_pos(
	ਪੂਰਨ		bnr,
	pos_T		*pos,
	ਪੂਰਨ		linewise,
	qfline_T	*qfp,
	ਪੂਰਨ		*errornr)
{
    // Find the entry just before the position 'pos'
    ਜਦੋਂ (qfp->qf_ਅਗਲਾ != ਨਿਰਾ
	    && qfp->qf_ਅਗਲਾ->qf_fnum == bnr
	    && qf_ਇੰਦਰਾਜ_before_pos(qfp->qf_ਅਗਲਾ, pos, linewise))
    {
	qfp = qfp->qf_ਅਗਲਾ;
	++*errornr;
    }

    ਜੇ (qf_ਇੰਦਰਾਜ_on_or_after_pos(qfp, pos, linewise))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ_on_ਸਤਰ(qfp, errornr);

    ਵਾਪਸ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ਸਥਿਰ qfline_T *
qf_ਲੱਭੋ_closest_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	ਪੂਰਨ		bnr,
	pos_T		*pos,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		linewise,
	ਪੂਰਨ		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ_ਵਿੱਚ_buf(qfl, bnr, errornr);
    ਜੇ (qfp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;		// no entry in this file

    ਜੇ (dir == FORWARD)
	qfp = qf_ਲੱਭੋ_ਇੰਦਰਾਜ_after_pos(bnr, pos, linewise, qfp, errornr);
    ਨਹੀਂਤਾਂ
	qfp = qf_ਲੱਭੋ_ਇੰਦਰਾਜ_before_pos(bnr, pos, linewise, qfp, errornr);

    ਵਾਪਸ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_get_nth_ਹੇਠਾਂ_ਇੰਦਰਾਜ(qfline_T *ਇੰਦਰਾਜ_arg, ਪੂਰਨ n, ਪੂਰਨ linewise, ਪੂਰਨ *errornr)
{
    qfline_T *ਇੰਦਰਾਜ = ਇੰਦਰਾਜ_arg;

    ਜਦੋਂ (n-- > 0 && !got_int)
    {
	ਪੂਰਨ		ਪਹਿਲਾ_errornr = *errornr;

	ਜੇ (linewise)
	    // Treat all the entries on the same line in this file as one
	    ਇੰਦਰਾਜ = qf_ਲੱਭੋ_ਆਖਰੀ_ਇੰਦਰਾਜ_on_ਸਤਰ(ਇੰਦਰਾਜ, errornr);

	ਜੇ (ਇੰਦਰਾਜ->qf_ਅਗਲਾ == ਨਿਰਾ
		|| ਇੰਦਰਾਜ->qf_ਅਗਲਾ->qf_fnum != ਇੰਦਰਾਜ->qf_fnum)
	{
	    ਜੇ (linewise)
		*errornr = ਪਹਿਲਾ_errornr;
	    ਰੋਕੋ;
	}

	ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->qf_ਅਗਲਾ;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_get_nth_above_ਇੰਦਰਾਜ(qfline_T *ਇੰਦਰਾਜ, ਪੂਰਨ n, ਪੂਰਨ linewise, ਪੂਰਨ *errornr)
{
    ਜਦੋਂ (n-- > 0 && !got_int)
    {
	ਜੇ (ਇੰਦਰਾਜ->qf_prev == ਨਿਰਾ
		|| ਇੰਦਰਾਜ->qf_prev->qf_fnum != ਇੰਦਰਾਜ->qf_fnum)
	    ਰੋਕੋ;

	ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	ਜੇ (linewise)
	    ਇੰਦਰਾਜ = qf_ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ_on_ਸਤਰ(ਇੰਦਰਾਜ, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਲੱਭੋ_nth_adj_ਇੰਦਰਾਜ(
	qf_list_T	*qfl,
	ਪੂਰਨ		bnr,
	pos_T		*pos,
	ਪੂਰਨ		n,
	ਪੂਰਨ		dir,
	ਪੂਰਨ		linewise)
{
    qfline_T	*adj_ਇੰਦਰਾਜ;
    ਪੂਰਨ		errornr;

    // Find an entry closest to the specified position
    adj_ਇੰਦਰਾਜ = qf_ਲੱਭੋ_closest_ਇੰਦਰਾਜ(qfl, bnr, pos, dir, linewise, &errornr);
    ਜੇ (adj_ਇੰਦਰਾਜ == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	ਜੇ (dir == FORWARD)
	    qf_get_nth_ਹੇਠਾਂ_ਇੰਦਰਾਜ(adj_ਇੰਦਰਾਜ, n, linewise, &errornr);
	ਨਹੀਂਤਾਂ
	    qf_get_nth_above_ਇੰਦਰਾਜ(adj_ਇੰਦਰਾਜ, n, linewise, &errornr);
    }

    ਵਾਪਸ errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    ਖਾਲੀ
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    ਪੂਰਨ		dir;
    ਪੂਰਨ		buf_has_flag;
    ਪੂਰਨ		errornr = 0;
    pos_T	pos;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ;
    }

    // Check whether the current buffer has any quickfix entries
    ਜੇ (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ਇੰਦਰਾਜ;
    ਨਹੀਂਤਾਂ
	buf_has_flag = BUF_HAS_LL_ਇੰਦਰਾਜ;
    ਜੇ (!(curbuf->b_has_qf_ਇੰਦਰਾਜ & buf_has_flag))
    {
	emsg(_(e_no_errors));
	ਵਾਪਸ;
    }

    ਜੇ ((qi = qf_cmd_get_ਢੇਰ(eap, TRUE)) == ਨਿਰਾ)
	ਵਾਪਸ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    ਜੇ (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	ਵਾਪਸ;
    }

    ਜੇ (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    ਨਹੀਂਤਾਂ
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.ਖਾਨਾ++;
    errornr = qf_ਲੱਭੋ_nth_adj_ਇੰਦਰਾਜ(qfl, curbuf->b_fnum, &pos,
				eap->addr_ਗਿਣਤੀ > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    ਜੇ (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    ਨਹੀਂਤਾਂ
	emsg(_(e_no_ਹੋਰ_ਚੀਜ਼ਾਂ));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ਸਥਿਰ char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_cfile:	    ਵਾਪਸ (char_u *)"cfile";
	ਹਾਲਤ CMD_cgetfile:  ਵਾਪਸ (char_u *)"cgetfile";
	ਹਾਲਤ CMD_caddfile:  ਵਾਪਸ (char_u *)"caddfile";
	ਹਾਲਤ CMD_lfile:	    ਵਾਪਸ (char_u *)"lfile";
	ਹਾਲਤ CMD_lgetfile:  ਵਾਪਸ (char_u *)"lgetfile";
	ਹਾਲਤ CMD_laddfile:  ਵਾਪਸ (char_u *)"laddfile";
	ਮੂਲ:	    ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    ਖਾਲੀ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ਨਿਰਾ;
    win_T	*wp = ਨਿਰਾ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ਨਿਰਾ;
    int_u	save_qfid = 0;		// init for gcc
    ਪੂਰਨ		res;

    au_name = cfile_get_auname(eap->cmdidx);
    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ਨਿਰਾ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	ਜੇ (aborting())
	    ਵਾਪਸ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ਨਿਰਾ, ਨਿਰਾ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ਨਿਰਾ);
	ਜੇ (browse_file == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    ਨਹੀਂਤਾਂ
# endif
    ਜੇ (*eap->arg != NUL)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    ਜੇ (is_loclist_cmd(eap->cmdidx))
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
    ਜੇ (wp != ਨਿਰਾ)
    {
	qi = GET_LOC_LIST(wp);
	ਜੇ (qi == ਨਿਰਾ)
	{
	    decr_quickfix_busy();
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
    {
	decr_quickfix_busy();
	ਵਾਪਸ;
    }
    ਜੇ (res >= 0)
	qf_list_ਬਦਲਿਆ(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    ਜੇ (au_name != ਨਿਰਾ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ਨਿਰਾ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ਜੇ (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ਪਹਿਲਾ(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ਸਥਿਰ char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_vimgrep:     ਵਾਪਸ (char_u *)"vimgrep";
	ਹਾਲਤ CMD_lvimgrep:    ਵਾਪਸ (char_u *)"lvimgrep";
	ਹਾਲਤ CMD_vimgrepadd:  ਵਾਪਸ (char_u *)"vimgrepadd";
	ਹਾਲਤ CMD_lvimgrepadd: ਵਾਪਸ (char_u *)"lvimgrepadd";
	ਹਾਲਤ CMD_grep:	      ਵਾਪਸ (char_u *)"grep";
	ਹਾਲਤ CMD_lgrep:	      ਵਾਪਸ (char_u *)"lgrep";
	ਹਾਲਤ CMD_grepadd:     ਵਾਪਸ (char_u *)"grepadd";
	ਹਾਲਤ CMD_lgrepadd:    ਵਾਪਸ (char_u *)"lgrepadd";
	ਮੂਲ: ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ਸਥਿਰ ਖਾਲੀ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ਨਿਰਾ;

    ਜੇ (s == ਨਿਰਾ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	ਜੇ (ਆਖਰੀ_ਖੋਜ_pat() == ਨਿਰਾ)
	{
	    emsg(_(e_no_ਪਿਛਲਾ_regular_ਵਿਅੰਜਕ));
	    ਵਾਪਸ;
	}
	regmatch->regprog = vim_regcomp(ਆਖਰੀ_ਖੋਜ_pat(), RE_MAGIC);
    }
    ਨਹੀਂਤਾਂ
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    ਸਥਿਰ ਖਾਲੀ
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    p = ਸੁਨੇਹਾ_strtrunc(fname, TRUE);
    ਜੇ (p == ਨਿਰਾ)
	ਸੁਨੇਹਾ_outtrans(fname);
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_outtrans(p);
	vim_free(p);
    }
    ਸੁਨੇਹਾ_clr_eos();
    ਸੁਨੇਹਾ_didout = FALSE;	    // overwrite this message
    ਸੁਨੇਹਾ_nowait = TRUE;	    // don't wait for this message
    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
    ਬਾਹਰਲਾ_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ਸਥਿਰ buf_T *
vgr_ਲੱਦੋ_dummy_buf(
	char_u *fname,
	char_u *dirname_ਸ਼ੁਰੂ,
	char_u *dirname_now)
{
    ਪੂਰਨ		save_mls;
# ਜੇ defined(FEAT_SYN_HL)
    char_u	*save_ei = ਨਿਰਾ;
# endif
    buf_T	*buf;

# ਜੇ defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = ਲੱਦੋ_dummy_ਬਫਰ(fname, dirname_ਸ਼ੁਰੂ, dirname_now);

    p_mls = save_mls;
# ਜੇ defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    ਵਾਪਸ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ਸਥਿਰ ਪੂਰਨ
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    ਜੇ (!qflist_valid(wp, qfid))
    {
	ਜੇ (wp != ਨਿਰਾ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_ਬਦਲਿਆ));
	    ਵਾਪਸ FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    ਵਾਪਸ TRUE;
	}
    }

    ਜੇ (qf_restore_list(qi, qfid) == FAIL)
	ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ਸਥਿਰ ਪੂਰਨ
vgr_ਮੇਲ_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	ਲੰਮਾ	    *tomatch,
	ਪੂਰਨ	    duplicate_name,
	ਪੂਰਨ	    flags)
{
    ਪੂਰਨ		ਮਿਲਿਆ_ਮੇਲ = FALSE;
    ਲੰਮਾ	lnum;
    colnr_T	ਖਾਨਾ;
    ਪੂਰਨ		pat_len = (ਪੂਰਨ)STRLEN(spat);
    ਜੇ (pat_len > FUZZY_ਮੇਲ_MAX_LEN)
	pat_len = FUZZY_ਮੇਲ_MAX_LEN;

    ਲਈ (lnum = 1; lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ && *tomatch > 0; ++lnum)
    {
	ਖਾਨਾ = 0;
	ਜੇ (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    ਜਦੋਂ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								ਖਾਨਾ, ਨਿਰਾ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ਜੇ (qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qfl,
			    ਨਿਰਾ,	// dir
			    fname,
			    ਨਿਰਾ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].ਖਾਨਾ + 1,
			    regmatch->endpos[0].ਖਾਨਾ + 1,
			    FALSE,	// vis_col
			    ਨਿਰਾ,	// search pattern
			    0,		// nr
			    0,		// type
			    ਨਿਰਾ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ਰੋਕੋ;
		}
		ਮਿਲਿਆ_ਮੇਲ = TRUE;
		ਜੇ (--*tomatch == 0)
		    ਰੋਕੋ;
		ਜੇ ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ਰੋਕੋ;
		ਖਾਨਾ = regmatch->endpos[0].ਖਾਨਾ
		    + (ਖਾਨਾ == regmatch->endpos[0].ਖਾਨਾ);
		ਜੇ (ਖਾਨਾ > ml_get_buf_len(buf, lnum))
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    ਪੂਰਨ	    score;
	    int_u   matches[FUZZY_ਮੇਲ_MAX_LEN];
	    int_u   sz = ARRAY_ਲੰਮਾਈ(matches);

	    // Fuzzy string match
	    CLEAR_ਖੇਤਰ(matches);
	    ਜਦੋਂ (fuzzy_ਮੇਲ(str + ਖਾਨਾ, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ਜੇ (qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qfl,
			    ਨਿਰਾ,	// dir
			    fname,
			    ਨਿਰਾ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + ਖਾਨਾ + 1,
			    0,
			    FALSE,	// vis_col
			    ਨਿਰਾ,	// search pattern
			    0,		// nr
			    0,		// type
			    ਨਿਰਾ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ਰੋਕੋ;
		}
		ਮਿਲਿਆ_ਮੇਲ = TRUE;
		ਜੇ (--*tomatch == 0)
		    ਰੋਕੋ;
		ਜੇ ((flags & VGR_GLOBAL) == 0)
		    ਰੋਕੋ;
		ਖਾਨਾ = matches[pat_len - 1] + ਖਾਨਾ + 1;
		ਜੇ (ਖਾਨਾ > linelen)
		    ਰੋਕੋ;
	    }
	}
	ਸਤਰ_breakcheck();
	ਜੇ (got_int)
	    ਰੋਕੋ;
    }

    ਵਾਪਸ ਮਿਲਿਆ_ਮੇਲ;
}

/*
 * Jump to the first match and update the directory.
 */
    ਸਥਿਰ ਖਾਲੀ
vgr_jump_ਮੰਜ਼ਿਲ_ਮੇਲ(
	qf_info_T   *qi,
	ਪੂਰਨ	    forceit,
	ਪੂਰਨ	    *redraw_for_dummy,
	buf_T	    *ਪਹਿਲਾ_ਮੇਲ_buf,
	char_u	    *ਨਿਸ਼ਾਨਾ_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    ਜੇ (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    ਜੇ (curbuf == ਪਹਿਲਾ_ਮੇਲ_buf && ਨਿਸ਼ਾਨਾ_dir != ਨਿਰਾ)
    {
	exarg_T ea;

	CLEAR_ਖੇਤਰ(ea);
	ea.arg = ਨਿਸ਼ਾਨਾ_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਲੰਮਾ	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    ਪੂਰਨ		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    ਪੂਰਨ		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_ਦਲੀਲਾਂ_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ਸਥਿਰ ਪੂਰਨ
vgr_process_ਦਲੀਲਾਂ(
	exarg_T		*eap,
	vgr_ਦਲੀਲਾਂ_T	*ਦਲੀਲਾਂ)
{
    char_u	*p;

    CLEAR_ਸੂਚਕ(ਦਲੀਲਾਂ);

    ਦਲੀਲਾਂ->regmatch.regprog = ਨਿਰਾ;
    ਦਲੀਲਾਂ->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
	ਦਲੀਲਾਂ->tomatch = eap->line2;
    ਨਹੀਂਤਾਂ
	ਦਲੀਲਾਂ->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &ਦਲੀਲਾਂ->spat, &ਦਲੀਲਾਂ->flags);
    ਜੇ (p == ਨਿਰਾ)
    {
	emsg(_(e_invalid_ਖੋਜ_ਨਮੂਨਾ_or_delimiter));
	ਵਾਪਸ FAIL;
    }

    vgr_init_regmatch(&ਦਲੀਲਾਂ->regmatch, ਦਲੀਲਾਂ->spat);
    ਜੇ (ਦਲੀਲਾਂ->regmatch.regprog == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    p = skipwhite(p);
    ਜੇ (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_ਨਮੂਨਾ));
	ਵਾਪਸ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    ਜੇ ((get_arglist_exp(p, &ਦਲੀਲਾਂ->fcount, &ਦਲੀਲਾਂ->fnames, TRUE) == FAIL) ||
	ਦਲੀਲਾਂ->fcount == 0)
    {
	emsg(_(e_no_ਮੇਲ));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ਸਥਿਰ ਪੂਰਨ
existing_swapfile(buf_T *buf)
{
    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ && buf->b_ml.ml_mfp->mf_fname != ਨਿਰਾ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	ਵਾਪਸ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    ਵਾਪਸ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ਸਥਿਰ ਪੂਰਨ
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_ਦਲੀਲਾਂ_T	*cmd_ਦਲੀਲਾਂ,
	ਪੂਰਨ		*redraw_for_dummy,
	buf_T		**ਪਹਿਲਾ_ਮੇਲ_buf,
	char_u		**ਨਿਸ਼ਾਨਾ_dir)
{
    ਪੂਰਨ		ਸਥਿਤੀ = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    ਪੂਰਨ		fi;
    buf_T	*buf;
    ਪੂਰਨ		duplicate_name = FALSE;
    ਪੂਰਨ		using_dummy;
    char_u	*dirname_ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u	*dirname_now = ਨਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ_ਮੇਲ;
    aco_save_T	aco;

    dirname_ਸ਼ੁਰੂ = alloc_id(MAXPATHL, aid_qf_dirname_ਸ਼ੁਰੂ);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    ਜੇ (dirname_ਸ਼ੁਰੂ == ਨਿਰਾ || dirname_now == ਨਿਰਾ)
	ਜਾਓ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_ਸ਼ੁਰੂ, MAXPATHL);

    seconds = (time_t)0;
    ਲਈ (fi = 0; fi < cmd_ਦਲੀਲਾਂ->fcount && !got_int && cmd_ਦਲੀਲਾਂ->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_ਦਲੀਲਾਂ->fnames[fi]);
	ਜੇ (ਸਮਾਂ(ਨਿਰਾ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = ਸਮਾਂ(ਨਿਰਾ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_ਦਲੀਲਾਂ->fnames[fi]);
	ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ਨਿਰਾ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_ਲੱਦੋ_dummy_buf(fname, dirname_ਸ਼ੁਰੂ, dirname_now);
	}
	ਨਹੀਂਤਾਂ
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	ਜੇ (!vgr_qflist_valid(wp, qi, save_qfid, cmd_ਦਲੀਲਾਂ->qf_title))
	    ਜਾਓ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	ਜੇ (buf == ਨਿਰਾ)
	{
	    ਜੇ (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    ਮਿਲਿਆ_ਮੇਲ = vgr_ਮੇਲ_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_ਦਲੀਲਾਂ->spat, &cmd_ਦਲੀਲਾਂ->regmatch,
		    &cmd_ਦਲੀਲਾਂ->tomatch, duplicate_name, cmd_ਦਲੀਲਾਂ->flags);

	    ਜੇ (using_dummy)
	    {
		ਜੇ (ਮਿਲਿਆ_ਮੇਲ && *ਪਹਿਲਾ_ਮੇਲ_buf == ਨਿਰਾ)
		    *ਪਹਿਲਾ_ਮੇਲ_buf = buf;
		ਜੇ (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_ਬਫਰ(buf, dirname_ਸ਼ੁਰੂ);
		    buf = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ ਜੇ ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    ਜੇ (!ਮਿਲਿਆ_ਮੇਲ)
		    {
			wipe_dummy_ਬਫਰ(buf, dirname_ਸ਼ੁਰੂ);
			buf = ਨਿਰਾ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (buf != *ਪਹਿਲਾ_ਮੇਲ_buf
					|| (cmd_ਦਲੀਲਾਂ->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_ਬਫਰ(buf, dirname_ਸ਼ੁਰੂ);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ਨਿਰਾ;
		    }
		}

		ਜੇ (buf != ਨਿਰਾ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    ਜੇ (buf == *ਪਹਿਲਾ_ਮੇਲ_buf
			    && *ਨਿਸ਼ਾਨਾ_dir == ਨਿਰਾ
			    && STRCMP(dirname_ਸ਼ੁਰੂ, dirname_now) != 0)
			*ਨਿਸ਼ਾਨਾ_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    ਜੇ (curbuf == buf)
		    {
# ਜੇ defined(FEAT_SYN_HL)
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

    ਸਥਿਤੀ = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_ਸ਼ੁਰੂ);
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    ਖਾਲੀ
ex_vimgrep(exarg_T *eap)
{
    vgr_ਦਲੀਲਾਂ_T	ਦਲੀਲਾਂ;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ਨਿਰਾ;
    ਪੂਰਨ		redraw_for_dummy = FALSE;
    buf_T	*ਪਹਿਲਾ_ਮੇਲ_buf = ਨਿਰਾ;
    char_u	*ਨਿਸ਼ਾਨਾ_dir = ਨਿਰਾ;
    char_u	*au_name =  ਨਿਰਾ;
    ਪੂਰਨ		ਸਥਿਤੀ;

    ਜੇ (!check_can_ਸੈੱਟ_curbuf_forceit(eap->forceit))
	ਵਾਪਸ;

    au_name = vgr_get_auname(eap->cmdidx);
    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ਜੇ (aborting())
	    ਵਾਪਸ;
# endif
    }

    qi = qf_cmd_get_or_alloc_ਢੇਰ(eap, &wp);
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (vgr_process_ਦਲੀਲਾਂ(eap, &ਦਲੀਲਾਂ) == FAIL)
	ਜਾਓ theend;

    ਜੇ ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
	// make place for a new list
	qf_new_list(qi, ਦਲੀਲਾਂ.qf_title);

    incr_quickfix_busy();

    ਸਥਿਤੀ = vgr_process_files(wp, qi, &ਦਲੀਲਾਂ, &redraw_for_dummy,
						&ਪਹਿਲਾ_ਮੇਲ_buf, &ਨਿਸ਼ਾਨਾ_dir);
    ਜੇ (ਸਥਿਤੀ != OK)
    {
	FreeWild(ਦਲੀਲਾਂ.fcount, ਦਲੀਲਾਂ.fnames);
	decr_quickfix_busy();
	ਜਾਓ theend;
    }

    FreeWild(ਦਲੀਲਾਂ.fcount, ਦਲੀਲਾਂ.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_ਸ਼ੁਰੂ;
    qfl->qf_ਸੂਚਕਾਂਕ = 1;
    qf_list_ਬਦਲਿਆ(qfl);

    qf_update_ਬਫਰ(qi, ਨਿਰਾ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    ਜੇ (au_name != ਨਿਰਾ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    ਜੇ (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	ਜਾਓ theend;
    }

    // Jump to first match.
    ਜੇ (!qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
    {
	ਜੇ ((ਦਲੀਲਾਂ.flags & VGR_NOJUMP) == 0)
	    vgr_jump_ਮੰਜ਼ਿਲ_ਮੇਲ(qi, eap->forceit, &redraw_for_dummy,
		    ਪਹਿਲਾ_ਮੇਲ_buf, ਨਿਸ਼ਾਨਾ_dir);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_no_ਮੇਲ_str_2), ਦਲੀਲਾਂ.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    ਜੇ (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# ਨਹੀਂਤਾਂ
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(ਦਲੀਲਾਂ.qf_title);
    vim_free(ਨਿਸ਼ਾਨਾ_dir);
    vim_regfree(ਦਲੀਲਾਂ.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_ਸ਼ੁਰੂ_dir(char_u *dirname_ਸ਼ੁਰੂ)
{
    char_u *dirname_now = alloc(MAXPATHL);

    ਜੇ (dirname_now == ਨਿਰਾ)
	ਵਾਪਸ;

    mch_dirname(dirname_now, MAXPATHL);
    ਜੇ (STRCMP(dirname_ਸ਼ੁਰੂ, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_ਖੇਤਰ(ea);
	ea.arg = dirname_ਸ਼ੁਰੂ;
	ea.cmdidx = (curwin->w_localdir == ਨਿਰਾ) ? CMD_cd : CMD_lcd;
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
    ਸਥਿਰ buf_T *
ਲੱਦੋ_dummy_ਬਫਰ(
    char_u	*fname,
    char_u	*dirname_ਸ਼ੁਰੂ,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_ਮੰਜ਼ਿਲ_wipe;
    ਪੂਰਨ		failed = TRUE;
    aco_save_T	aco;
    ਪੂਰਨ		readfile_ਨਤੀਜਾ;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, (linenr_T)1, BLN_DUMMY);
    ਜੇ (newbuf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਸੈੱਟ_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    ਜੇ (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	ਜੇ (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (ਖਾਲੀ)setfname(curbuf, fname, ਨਿਰਾ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_ਅਦਲਾ_ਬਦਲੀ(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_ਮੰਜ਼ਿਲ_wipe.br_buf = ਨਿਰਾ;
	    readfile_ਨਤੀਜਾ = readfile(fname, ਨਿਰਾ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ਨਿਰਾ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    ਜੇ (readfile_ਨਤੀਜਾ == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		ਜੇ (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    ਸੈੱਟ_bufref(&newbuf_ਮੰਜ਼ਿਲ_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    ਜੇ (newbuf_ਮੰਜ਼ਿਲ_wipe.br_buf != ਨਿਰਾ && bufref_valid(&newbuf_ਮੰਜ਼ਿਲ_wipe))
	    {
		ਖੰਡ_autocmds();
		wipe_dummy_ਬਫਰ(newbuf_ਮੰਜ਼ਿਲ_wipe.br_buf, ਨਿਰਾ);
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
    restore_ਸ਼ੁਰੂ_dir(dirname_ਸ਼ੁਰੂ);

    ਜੇ (!bufref_valid(&newbufref))
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (failed)
    {
	wipe_dummy_ਬਫਰ(newbuf, dirname_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ਸਥਿਰ ਖਾਲੀ
wipe_dummy_ਬਫਰ(buf_T *buf, char_u *dirname_ਸ਼ੁਰੂ)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    ਜਦੋਂ (buf->b_nwindows > 0)
    {
	ਪੂਰਨ	    did_one = FALSE;
	win_T	    *wp;

	ਜੇ (firstwin->w_ਅਗਲਾ != ਨਿਰਾ)
	    FOR_ALL_WINDOWS(wp)
		ਜੇ (wp->w_ਬਫਰ == buf)
		{
		    ਜੇ (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ਰੋਕੋ;
		}
	ਜੇ (!did_one)
	    ਜਾਓ fail;
    }

    ਜੇ (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# ਜੇ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_ਬਫਰ(buf, TRUE);

# ਜੇ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	ਜੇ (dirname_ਸ਼ੁਰੂ != ਨਿਰਾ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_ਸ਼ੁਰੂ_dir(dirname_ਸ਼ੁਰੂ);

	ਵਾਪਸ;
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
    ਸਥਿਰ ਖਾਲੀ
unload_dummy_ਬਫਰ(buf_T *buf, char_u *dirname_ਸ਼ੁਰੂ)
{
    ਜੇ (curbuf == buf)		// safety check
	ਵਾਪਸ;

    close_ਬਫਰ(ਨਿਰਾ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_ਸ਼ੁਰੂ_dir(dirname_ਸ਼ੁਰੂ);
}

# ਜੇ defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ਸਥਿਰ ਪੂਰਨ
get_qfline_ਚੀਜ਼ਾਂ(qfline_T *qfp, list_T *list)
{
    ਪੂਰਨ		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    ਜੇ (bufnum != 0 && (buflist_findnr(bufnum) == ਨਿਰਾ))
	bufnum = 0;

    ਜੇ ((dict = dict_alloc()) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	ਵਾਪਸ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ਜੇ (dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "bufnr", (ਲੰਮਾ)bufnum) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum",     (ਲੰਮਾ)qfp->qf_lnum) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "end_lnum", (ਲੰਮਾ)qfp->qf_ਸਿਰਾ_lnum) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col",      (ਲੰਮਾ)qfp->qf_ਖਾਨਾ) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "end_col",  (ਲੰਮਾ)qfp->qf_ਸਿਰਾ_ਖਾਨਾ) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "vcol",     (ਲੰਮਾ)qfp->qf_viscol) == FAIL
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "nr",       (ਲੰਮਾ)qfp->qf_nr) == FAIL
	    || dict_ਜੋੜੋ_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_ਜੋੜੋ_string(dict, "pattern", qfp->qf_ਨਮੂਨਾ) == FAIL
	    || dict_ਜੋੜੋ_string(dict, "text", qfp->qf_ਪਾਠ) == FAIL
	    || dict_ਜੋੜੋ_string_len(dict, "type", buf, (ਪੂਰਨ)buflen) == FAIL
	    || (qfp->qf_user_ਅੰਕੜੇ.v_type != VAR_UNKNOWN
		&& dict_ਜੋੜੋ_tv(dict, "user_data", &qfp->qf_user_ਅੰਕੜੇ) == FAIL )
	    || dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "valid", (ਲੰਮਾ)qfp->qf_valid) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ਸਥਿਰ ਪੂਰਨ
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	ਪੂਰਨ		qf_idx,
	ਪੂਰਨ		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ਪੂਰਨ		i;

    ਜੇ (qi == ਨਿਰਾ)
    {
	qi = ql_info;
	ਜੇ (wp != ਨਿਰਾ)
	    qi = GET_LOC_LIST(wp);
	ਜੇ (qi == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

    }

    ਜੇ (eidx < 0)
	ਵਾਪਸ OK;

    ਜੇ (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    ਜੇ (qf_idx >= qi->qf_listcount)
	ਵਾਪਸ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    ਜੇ (qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
	ਵਾਪਸ FAIL;

    FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, i)
    {
	ਜੇ (eidx > 0)
	{
	    ਜੇ (eidx == i)
		ਵਾਪਸ get_qfline_ਚੀਜ਼ਾਂ(qfp, list);
	}
	ਨਹੀਂਤਾਂ ਜੇ (get_qfline_ਚੀਜ਼ਾਂ(qfp, list) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
ਗਿਣਤੀ {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ਚੀਜ਼ਾਂ	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_ਮਾਪ	= 0x80,
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
    ਸਥਿਰ ਪੂਰਨ
qf_get_list_from_ਸਤਰਾਂ(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    ਪੂਰਨ		ਸਥਿਤੀ = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    ਜੇ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    ਜੇ ((efm_di = dict_ਲੱਭੋ(what, (char_u *)"efm", -1)) != ਨਿਰਾ)
    {
	ਜੇ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    qi = qf_alloc_ਢੇਰ(QFLT_ਅੰਦਰੂਨੀ, 1);
    ਜੇ (qi != ਨਿਰਾ)
    {
	ਜੇ (qf_init_ext(qi, 0, ਨਿਰਾ, ਨਿਰਾ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ਨਿਰਾ, ਨਿਰਾ) > 0)
	{
	    (ਖਾਲੀ)get_errorlist(qi, ਨਿਰਾ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_ਜੋੜੋ_list(retdict, "items", l);
    ਸਥਿਤੀ = OK;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ 0;
    win = qf_ਲੱਭੋ_win(qi);
    ਜੇ (win != ਨਿਰਾ)
	ਵਾਪਸ win->w_id;
    ਵਾਪਸ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    ਪੂਰਨ	bufnum = 0;

    ਜੇ (qi != ਨਿਰਾ && buflist_findnr(qi->qf_bufnr) != ਨਿਰਾ)
	bufnum = qi->qf_bufnr;

    ਵਾਪਸ dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_keys2flags(dict_T *what, ਪੂਰਨ loclist)
{
    ਪੂਰਨ		flags = QF_GETLIST_NONE;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	ਜੇ (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "title"))
	flags |= QF_GETLIST_TITLE;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "nr"))
	flags |= QF_GETLIST_NR;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "winid"))
	flags |= QF_GETLIST_WINID;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "id"))
	flags |= QF_GETLIST_ID;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "items"))
	flags |= QF_GETLIST_ਚੀਜ਼ਾਂ;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "idx"))
	flags |= QF_GETLIST_IDX;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "size"))
	flags |= QF_GETLIST_ਮਾਪ;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    ਜੇ (loclist && dict_has_ਕੁੰਜੀ(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    ਜੇ (dict_has_ਕੁੰਜੀ(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    ਵਾਪਸ flags;
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
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    ਪੂਰਨ		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"nr", -1)) != ਨਿਰਾ)
    {
	// Use the specified quickfix/location list
	ਜੇ (di->di_tv.v_type == VAR_ਸੰਖਿਆ)
	{
	    // for zero use the current list
	    ਜੇ (di->di_tv.vval.v_ਸੰਖਿਆ != 0)
	    {
		qf_idx = di->di_tv.vval.v_ਸੰਖਿਆ - 1;
		ਜੇ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ਨਿਰਾ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	ਨਹੀਂਤਾਂ
	    qf_idx = INVALID_QFIDX;
    }

    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"id", -1)) != ਨਿਰਾ)
    {
	// Look for a list with the specified id
	ਜੇ (di->di_tv.v_type == VAR_ਸੰਖਿਆ)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    ਜੇ (di->di_tv.vval.v_ਸੰਖਿਆ != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_ਸੰਖਿਆ);
	}
	ਨਹੀਂਤਾਂ
	    qf_idx = INVALID_QFIDX;
    }

    ਵਾਪਸ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_defaults(qf_info_T *qi, ਪੂਰਨ flags, ਪੂਰਨ locstack, dict_T *retdict)
{
    ਪੂਰਨ		ਸਥਿਤੀ = OK;

    ਜੇ (flags & QF_GETLIST_TITLE)
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(retdict, "title", (char_u *)"", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ਚੀਜ਼ਾਂ))
    {
	list_T	*l = list_alloc();
	ਜੇ (l != ਨਿਰਾ)
	    ਸਥਿਤੀ = dict_ਜੋੜੋ_list(retdict, "items", l);
	ਨਹੀਂਤਾਂ
	    ਸਥਿਤੀ = FAIL;
    }
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_NR))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "nr", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_WINID))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "winid", qf_winid(qi));
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_CONTEXT))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(retdict, "context", (char_u *)"", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ID))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "id", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_IDX))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "idx", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ਮਾਪ))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "size", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_TICK))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "changedtick", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "filewinid", 0);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_QFBUFNR))
	ਸਥਿਤੀ = qf_getprop_qfbufnr(qi, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_QFTF))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    ਵਾਪਸ dict_ਜੋੜੋ_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    ਪੂਰਨ winid = 0;

    ਜੇ (wp != ਨਿਰਾ && IS_LL_ਖਿੜਕੀ(wp))
    {
	win_T	*ll_wp = qf_ਲੱਭੋ_win_with_loclist(qi);
	ਜੇ (ll_wp != ਨਿਰਾ)
	    winid = ll_wp->w_id;
    }

    ਵਾਪਸ dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_ਚੀਜ਼ਾਂ(qf_info_T *qi, ਪੂਰਨ qf_idx, ਪੂਰਨ eidx, dict_T *retdict)
{
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    list_T	*l = list_alloc();
    ਜੇ (l != ਨਿਰਾ)
    {
	(ਖਾਲੀ)get_errorlist(qi, ਨਿਰਾ, qf_idx, eidx, l);
	dict_ਜੋੜੋ_list(retdict, "items", l);
    }
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = FAIL;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    ਪੂਰਨ		ਸਥਿਤੀ;
    dictitem_T	*di;

    ਜੇ (qfl->qf_ctx != ਨਿਰਾ)
    {
	di = dictitem_alloc((char_u *)"context");
	ਜੇ (di != ਨਿਰਾ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    ਸਥਿਤੀ = dict_ਜੋੜੋ(retdict, di);
	    ਜੇ (ਸਥਿਤੀ == FAIL)
		dictitem_free(di);
	}
	ਨਹੀਂਤਾਂ
	    ਸਥਿਤੀ = FAIL;
    }
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(retdict, "context", (char_u *)"", 0);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_idx(qf_list_T *qfl, ਪੂਰਨ eidx, dict_T *retdict)
{
    ਜੇ (eidx == 0)
    {
	eidx = qfl->qf_ਸੂਚਕਾਂਕ;
	ਜੇ (qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    ਵਾਪਸ dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ਸਥਿਰ ਪੂਰਨ
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    ਪੂਰਨ		ਸਥਿਤੀ;

    ਜੇ (qfl->qf_qftf_cb.cb_name != ਨਿਰਾ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	ਸਥਿਤੀ = dict_ਜੋੜੋ_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = dict_ਜੋੜੋ_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    ਪੂਰਨ		qf_idx = INVALID_QFIDX;
    ਪੂਰਨ		eidx = 0;
    dictitem_T	*di;
    ਪੂਰਨ		flags = QF_GETLIST_NONE;

    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"lines", -1)) != ਨਿਰਾ)
	ਵਾਪਸ qf_get_list_from_ਸਤਰਾਂ(what, di, retdict);

    ਜੇ (wp != ਨਿਰਾ)
	qi = GET_LOC_LIST(wp);
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ਨਿਰਾ));

    ਜੇ (!qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi) || qf_idx == INVALID_QFIDX)
	ਵਾਪਸ qf_getprop_defaults(qi, flags, wp != ਨਿਰਾ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"idx", -1)) != ਨਿਰਾ)
    {
	ਜੇ (di->di_tv.v_type != VAR_ਸੰਖਿਆ)
	    ਵਾਪਸ FAIL;
	eidx = di->di_tv.vval.v_ਸੰਖਿਆ;
    }

    ਜੇ (flags & QF_GETLIST_TITLE)
	ਸਥਿਤੀ = qf_getprop_title(qfl, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_NR))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "nr", qf_idx + 1);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_WINID))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "winid", qf_winid(qi));
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ਚੀਜ਼ਾਂ))
	ਸਥਿਤੀ = qf_getprop_ਚੀਜ਼ਾਂ(qi, qf_idx, eidx, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_CONTEXT))
	ਸਥਿਤੀ = qf_getprop_ctx(qfl, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ID))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "id", qfl->qf_id);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_IDX))
	ਸਥਿਤੀ = qf_getprop_idx(qfl, eidx, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_ਮਾਪ))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "size", qfl->qf_ਗਿਣਤੀ);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_TICK))
	ਸਥਿਤੀ = dict_ਜੋੜੋ_ਸੰਖਿਆ(retdict, "changedtick", qfl->qf_changedtick);
    ਜੇ ((ਸਥਿਤੀ == OK) && (wp != ਨਿਰਾ) && (flags & QF_GETLIST_FILEWINID))
	ਸਥਿਤੀ = qf_getprop_filewinid(wp, qi, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_QFBUFNR))
	ਸਥਿਤੀ = qf_getprop_qfbufnr(qi, retdict);
    ਜੇ ((ਸਥਿਤੀ == OK) && (flags & QF_GETLIST_QFTF))
	ਸਥਿਤੀ = qf_getprop_qftf(qfl, retdict);

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਜੋੜੋ_ਇੰਦਰਾਜ_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	ਪੂਰਨ		ਪਹਿਲਾ_ਇੰਦਰਾਜ,
	ਪੂਰਨ		*valid_ਇੰਦਰਾਜ)
{
    ਸਥਿਰ ਪੂਰਨ	did_bufnr_emsg;
    char_u	*filename, *module, *ਨਮੂਨਾ, *ਪਾਠ, *type;
    ਪੂਰਨ		bufnum, valid, ਸਥਿਤੀ, ਖਾਨਾ, ਸਿਰਾ_ਖਾਨਾ, vcol, nr;
    ਲੰਮਾ	lnum, ਸਿਰਾ_lnum;

    ਜੇ (ਪਹਿਲਾ_ਇੰਦਰਾਜ)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "bufnr");
    lnum = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "lnum");
    ਸਿਰਾ_lnum = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "end_lnum");
    ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "col");
    ਸਿਰਾ_ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "end_col");
    vcol = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "vcol");
    nr = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    ਨਮੂਨਾ = dict_get_string(d, "pattern", TRUE);
    ਪਾਠ = dict_get_string(d, "text", TRUE);
    ਜੇ (ਪਾਠ == ਨਿਰਾ)
	ਪਾਠ = vim_strsave((char_u *)"");
    typval_T user_ਅੰਕੜੇ;
    user_ਅੰਕੜੇ.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_ਅੰਕੜੇ);

    valid = TRUE;
    ਜੇ ((filename == ਨਿਰਾ && bufnum == 0) || (lnum == 0 && ਨਮੂਨਾ == ਨਿਰਾ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    ਜੇ (bufnum != 0 && (buflist_findnr(bufnum) == ਨਿਰਾ))
    {
	ਜੇ (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_ਬਫਰ_nr_not_ਮਿਲਿਆ), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    ਜੇ (dict_has_ਕੁੰਜੀ(d, "valid"))
	valid = (ਪੂਰਨ)dict_get_bool(d, "valid", FALSE);

    ਸਥਿਤੀ =  qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qfl,
			ਨਿਰਾ,		// dir
			filename,
			module,
			bufnum,
			ਪਾਠ,
			lnum,
			ਸਿਰਾ_lnum,
			ਖਾਨਾ,
			ਸਿਰਾ_ਖਾਨਾ,
			vcol,		// vis_col
			ਨਮੂਨਾ,	// search pattern
			nr,
			type == ਨਿਰਾ ? NUL : *type,
			&user_ਅੰਕੜੇ,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(ਨਮੂਨਾ);
    vim_free(ਪਾਠ);
    vim_free(type);
    clear_tv(&user_ਅੰਕੜੇ);

    ਜੇ (valid)
	*valid_ਇੰਦਰਾਜ = TRUE;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
ਇੰਦਰਾਜ_is_closer_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ(
	qfline_T	*ਇੰਦਰਾਜ,
	qfline_T	*other_ਇੰਦਰਾਜ,
	ਪੂਰਨ		ਨਿਸ਼ਾਨਾ_fnum,
	ਪੂਰਨ		ਨਿਸ਼ਾਨਾ_lnum,
	ਪੂਰਨ		ਨਿਸ਼ਾਨਾ_ਖਾਨਾ)
{
    // First, compare entries to target file.
    ਜੇ (!ਨਿਸ਼ਾਨਾ_fnum)
	// Without a target file, we can't know which is closer.
	ਵਾਪਸ FALSE;

    ਪੂਰਨ is_ਨਿਸ਼ਾਨਾ_file = ਇੰਦਰਾਜ->qf_fnum && ਇੰਦਰਾਜ->qf_fnum == ਨਿਸ਼ਾਨਾ_fnum;
    ਪੂਰਨ other_is_ਨਿਸ਼ਾਨਾ_file = other_ਇੰਦਰਾਜ->qf_fnum && other_ਇੰਦਰਾਜ->qf_fnum == ਨਿਸ਼ਾਨਾ_fnum;
    ਜੇ (!is_ਨਿਸ਼ਾਨਾ_file && other_is_ਨਿਸ਼ਾਨਾ_file)
	ਵਾਪਸ FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (is_ਨਿਸ਼ਾਨਾ_file && !other_is_ਨਿਸ਼ਾਨਾ_file)
	ਵਾਪਸ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    ਜੇ (!ਨਿਸ਼ਾਨਾ_lnum)
	// Without a target line number, we can't know which is closer.
	ਵਾਪਸ FALSE;

    ਪੂਰਨ ਸਤਰ_ਦੂਰੀ = ਇੰਦਰਾਜ->qf_lnum ? labs(ਇੰਦਰਾਜ->qf_lnum - ਨਿਸ਼ਾਨਾ_lnum) : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਪੂਰਨ other_ਸਤਰ_ਦੂਰੀ = other_ਇੰਦਰਾਜ->qf_lnum ? labs(other_ਇੰਦਰਾਜ->qf_lnum - ਨਿਸ਼ਾਨਾ_lnum) : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਜੇ (ਸਤਰ_ਦੂਰੀ > other_ਸਤਰ_ਦੂਰੀ)
	ਵਾਪਸ FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ_ਦੂਰੀ < other_ਸਤਰ_ਦੂਰੀ)
	ਵਾਪਸ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    ਜੇ (!ਨਿਸ਼ਾਨਾ_ਖਾਨਾ)
	// Without a target column, we can't know which is closer.
	ਵਾਪਸ FALSE;

    ਪੂਰਨ ਕਾਲਮ_ਦੂਰੀ = ਇੰਦਰਾਜ->qf_ਖਾਨਾ ? ਨਿਰਪੇਖ(ਇੰਦਰਾਜ->qf_ਖਾਨਾ - ਨਿਸ਼ਾਨਾ_ਖਾਨਾ) : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਪੂਰਨ other_ਕਾਲਮ_ਦੂਰੀ = other_ਇੰਦਰਾਜ->qf_ਖਾਨਾ ? ਨਿਰਪੇਖ(other_ਇੰਦਰਾਜ->qf_ਖਾਨਾ - ਨਿਸ਼ਾਨਾ_ਖਾਨਾ): ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    ਜੇ (ਕਾਲਮ_ਦੂਰੀ > other_ਕਾਲਮ_ਦੂਰੀ)
	ਵਾਪਸ FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (ਕਾਲਮ_ਦੂਰੀ < other_ਕਾਲਮ_ਦੂਰੀ)
	ਵਾਪਸ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    ਵਾਪਸ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਜੋੜੋ_entries(
	qf_info_T	*qi,
	ਪੂਰਨ		qf_idx,
	list_T		*list,
	char_u		*title,
	ਪੂਰਨ		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_ਆਖਰੀ = ਨਿਰਾ;
    ਪੂਰਨ		retval = OK;
    ਪੂਰਨ		valid_ਇੰਦਰਾਜ = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    ਪੂਰਨ prev_fnum = 0;
    ਪੂਰਨ prev_lnum = 0;
    ਪੂਰਨ prev_ਖਾਨਾ = 0;
    ਜੇ (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_ਖਾਨਾ = qfl->qf_ptr->qf_ਖਾਨਾ;
    }

    ਪੂਰਨ select_ਪਹਿਲਾ_ਇੰਦਰਾਜ = FALSE;
    ਪੂਰਨ select_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਇੰਦਰਾਜ = FALSE;

    ਜੇ (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_ਪਹਿਲਾ_ਇੰਦਰਾਜ = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ਨਹੀਂਤਾਂ ਜੇ (action == 'a')
    {
	ਜੇ (qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qfl))
	    // Appending to empty list, select first entry.
	    select_ਪਹਿਲਾ_ਇੰਦਰਾਜ = TRUE;
	ਨਹੀਂਤਾਂ
	    // Adding to existing list, use last entry.
	    old_ਆਖਰੀ = qfl->qf_ਆਖਰੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (action == 'r')
    {
	select_ਪਹਿਲਾ_ਇੰਦਰਾਜ = TRUE;
	qf_free_ਚੀਜ਼ਾਂ(qfl);
	qf_store_title(qfl, title);
    }
    ਨਹੀਂਤਾਂ ਜੇ (action == 'u')
    {
	select_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਇੰਦਰਾਜ = TRUE;
	qf_free_ਚੀਜ਼ਾਂ(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select = ਨਿਰਾ;
    ਪੂਰਨ ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select_ਸੂਚਕਾਂਕ = 0;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
    {
	ਜੇ (li->li_tv.v_type != VAR_DICT)
	    ਜਾਰੀ; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	ਜੇ (d == ਨਿਰਾ)
	    ਜਾਰੀ;

	retval = qf_ਜੋੜੋ_ਇੰਦਰਾਜ_from_dict(qfl, d, li == list->lv_ਪਹਿਲਾ,
								&valid_ਇੰਦਰਾਜ);
	ਜੇ (retval == QF_FAIL)
	    ਰੋਕੋ;

	qfline_T *ਇੰਦਰਾਜ = qfl->qf_ਆਖਰੀ;
	ਜੇ (
	    (select_ਪਹਿਲਾ_ਇੰਦਰਾਜ && ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select == ਨਿਰਾ) ||
	    (select_ਸਭ_ਤੋਂ_ਨੇੜੇ_ਇੰਦਰਾਜ &&
		(ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select == ਨਿਰਾ ||
		 ਇੰਦਰਾਜ_is_closer_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ(ਇੰਦਰਾਜ, ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select, prev_fnum,
					   prev_lnum, prev_ਖਾਨਾ))))
	{
	    ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select = ਇੰਦਰਾਜ;
	    ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select_ਸੂਚਕਾਂਕ = qfl->qf_ਗਿਣਤੀ;
	}
    }

    // Check if any valid error entries are added to the list.
    ਜੇ (valid_ਇੰਦਰਾਜ)
	qfl->qf_nonevalid = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (qfl->qf_ਸੂਚਕਾਂਕ == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    ਜੇ (ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select)
    {
	qfl->qf_ptr = ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select;
	qfl->qf_ਸੂਚਕਾਂਕ = ਇੰਦਰਾਜ_ਮੰਜ਼ਿਲ_select_ਸੂਚਕਾਂਕ;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_ਬਫਰ(qi, old_ਆਖਰੀ);

    ਵਾਪਸ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	ਪੂਰਨ		action,
	ਪੂਰਨ		*newlist)
{
    dictitem_T	*di;
    ਪੂਰਨ		qf_idx = qi->qf_curlist;    // default is the current list

    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"nr", -1)) != ਨਿਰਾ)
    {
	// Use the specified quickfix/location list
	ਜੇ (di->di_tv.v_type == VAR_ਸੰਖਿਆ)
	{
	    // for zero use the current list
	    ਜੇ (di->di_tv.vval.v_ਸੰਖਿਆ != 0)
		qf_idx = di->di_tv.vval.v_ਸੰਖਿਆ - 1;

	    ਜੇ ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		ਵਾਪਸ INVALID_QFIDX;
	    ਨਹੀਂਤਾਂ ਜੇ (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ਨਿਰਾ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    ਜੇ (!qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
		qf_idx = qi->qf_listcount - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (*newlist)
		qf_idx = 0;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ INVALID_QFIDX;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ INVALID_QFIDX;
    }

    ਜੇ (!*newlist && (di = dict_ਲੱਭੋ(what, (char_u *)"id", -1)) != ਨਿਰਾ)
    {
	// Use the quickfix/location list with the specified id
	ਜੇ (di->di_tv.v_type != VAR_ਸੰਖਿਆ)
	    ਵਾਪਸ INVALID_QFIDX;

	ਵਾਪਸ qf_id2nr(qi, di->di_tv.vval.v_ਸੰਖਿਆ);
    }

    ਵਾਪਸ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_title(qf_info_T *qi, ਪੂਰਨ qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    ਜੇ (di->di_tv.v_type != VAR_STRING)
	ਵਾਪਸ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    ਜੇ (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    ਵਾਪਸ OK;
}

/*
 * Set quickfix list items/entries.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_ਚੀਜ਼ਾਂ(qf_info_T *qi, ਪੂਰਨ qf_idx, dictitem_T *di, ਪੂਰਨ action)
{
    ਪੂਰਨ		retval = FAIL;
    char_u	*title_save;

    ਜੇ (di->di_tv.v_type != VAR_LIST)
	ਵਾਪਸ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_ਜੋੜੋ_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    ਵਾਪਸ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_ਚੀਜ਼ਾਂ_from_ਸਤਰਾਂ(
	qf_info_T	*qi,
	ਪੂਰਨ		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	ਪੂਰਨ		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    ਪੂਰਨ		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    ਜੇ ((efm_di = dict_ਲੱਭੋ(what, (char_u *)"efm", -1)) != ਨਿਰਾ)
    {
	ਜੇ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    ਜੇ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (action == 'r' || action == 'u')
	qf_free_ਚੀਜ਼ਾਂ(&qi->qf_lists[qf_idx]);
    ਜੇ (qf_init_ext(qi, qf_idx, ਨਿਰਾ, ਨਿਰਾ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ਨਿਰਾ, ਨਿਰਾ) >= 0)
	retval = OK;

    ਵਾਪਸ retval;
}

/*
 * Set quickfix list context.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    ਜੇ (ctx != ਨਿਰਾ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    ਵਾਪਸ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    ਪੂਰਨ		denote = FALSE;
    ਪੂਰਨ		newidx;
    ਪੂਰਨ		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    ਜੇ (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ਨਿਰਾ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ
    {
	// Otherwise use the specified index
	newidx = tv_get_ਸੰਖਿਆ_chk(&di->di_tv, &denote);
	ਜੇ (denote)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (newidx < 1)		// sanity check
	ਵਾਪਸ FAIL;
    ਜੇ (newidx > qfl->qf_ਗਿਣਤੀ)
	newidx = qfl->qf_ਗਿਣਤੀ;

    old_qfidx = qfl->qf_ਸੂਚਕਾਂਕ;
    qf_ptr = get_nth_ਇੰਦਰਾਜ(qfl, newidx, &newidx);
    ਜੇ (qf_ptr == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_ਸੂਚਕਾਂਕ = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    ਜੇ (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    ਵਾਪਸ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ਸਥਿਰ ਪੂਰਨ
qf_setprop_qftf(qf_info_T *qi ਅਣਵਰਤਿਆ, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    ਜੇ (cb.cb_name == ਨਿਰਾ || *cb.cb_name == NUL)
	ਵਾਪਸ OK;
    ਸੈੱਟ_callback(&qfl->qf_qftf_cb, &cb);
    ਵਾਪਸ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ਸਥਿਰ ਪੂਰਨ
qf_ਸੈੱਟ_properties(qf_info_T *qi, dict_T *what, ਪੂਰਨ action, char_u *title)
{
    dictitem_T	*di;
    ਪੂਰਨ		retval = FAIL;
    ਪੂਰਨ		qf_idx;
    ਪੂਰਨ		newlist = FALSE;
    qf_list_T	*qfl;

    ਜੇ (action == ' ' || qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    ਜੇ (qf_idx == INVALID_QFIDX)	// List not found
	ਵਾਪਸ FAIL;

    ਜੇ (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"title", -1)) != ਨਿਰਾ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"items", -1)) != ਨਿਰਾ)
	retval = qf_setprop_ਚੀਜ਼ਾਂ(qi, qf_idx, di, action);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"lines", -1)) != ਨਿਰਾ)
	retval = qf_setprop_ਚੀਜ਼ਾਂ_from_ਸਤਰਾਂ(qi, qf_idx, what, di, action);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"context", -1)) != ਨਿਰਾ)
	retval = qf_setprop_context(qfl, di);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"idx", -1)) != ਨਿਰਾ)
	retval = qf_setprop_curidx(qi, qfl, di);
    ਜੇ ((di = dict_ਲੱਭੋ(what, (char_u *)"quickfixtextfunc", -1)) != ਨਿਰਾ)
	retval = qf_setprop_qftf(qi, qfl, di);

    ਜੇ (newlist || retval == OK)
	qf_list_ਬਦਲਿਆ(qfl);
    ਜੇ (newlist)
	qf_update_ਬਫਰ(qi, ਨਿਰਾ);

    ਵਾਪਸ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ਸਥਿਰ ਖਾਲੀ
qf_free_ਢੇਰ(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_ਲੱਭੋ_win(qi);
    win_T	*llwin = ਨਿਰਾ;

    ਜੇ (qfwin != ਨਿਰਾ)
    {
	// If the quickfix/location list window is open, then clear it
	ਜੇ (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_ਬਫਰ(qi, ਨਿਰਾ);
    }

    ਜੇ (wp != ਨਿਰਾ && IS_LL_ਖਿੜਕੀ(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_ਲੱਭੋ_win_with_loclist(qi);
	ਜੇ (llwin != ਨਿਰਾ)
	    wp = llwin;
    }

    qf_free_all(wp);
    ਜੇ (wp == ਨਿਰਾ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (qfwin != ਨਿਰਾ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ਢੇਰ(QFLT_LOCATION, wp->w_p_lhi);

	ਜੇ (new_ll != ਨਿਰਾ)
	{
	    new_ll->qf_bufnr = qfwin->w_ਬਫਰ->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    ਜੇ (wp != qfwin)
		win_ਸੈੱਟ_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    ਪੂਰਨ
ਸੈੱਟ_errorlist(
	win_T	*wp,
	list_T	*list,
	ਪੂਰਨ	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    ਪੂਰਨ		retval = OK;

    ਜੇ (wp != ਨਿਰਾ)
	qi = ll_get_or_alloc_list(wp);
    ਨਹੀਂਤਾਂ
	qi = ql_info;
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ਢੇਰ(wp, qi);
	ਵਾਪਸ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    ਜੇ (list->lv_len != 0 && what != ਨਿਰਾ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str),
			 _("cannot have both a list and a \"what\" argument"));
	ਵਾਪਸ FAIL;
    }

    incr_quickfix_busy();

    ਜੇ (what != ਨਿਰਾ)
	retval = qf_ਸੈੱਟ_properties(qi, what, action, title);
    ਨਹੀਂਤਾਂ
    {
	retval = qf_ਜੋੜੋ_entries(qi, qi->qf_curlist, list, title, action);
	ਜੇ (retval == OK)
	    qf_list_ਬਦਲਿਆ(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    ਵਾਪਸ retval;
}

ਸਥਿਰ ਪੂਰਨ mark_quickfix_user_ਅੰਕੜੇ(qf_info_T *qi, ਪੂਰਨ copyID)
{
    ਪੂਰਨ abort = FALSE;
    ਲਈ (ਪੂਰਨ i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	ਜੇ (!qfl->qf_has_user_ਅੰਕੜੇ)
	    ਜਾਰੀ;
	qfline_T *qfp;
	ਪੂਰਨ j;
	FOR_ALL_QFL_ਚੀਜ਼ਾਂ(qfl, qfp, j)
	{
	    typval_T* user_ਅੰਕੜੇ = &qfp->qf_user_ਅੰਕੜੇ;
	    ਜੇ (user_ਅੰਕੜੇ != ਨਿਰਾ && user_ਅੰਕੜੇ->v_type != VAR_ਸੰਖਿਆ
		&& user_ਅੰਕੜੇ->v_type != VAR_STRING && user_ਅੰਕੜੇ->v_type != VAR_FLOAT)
		abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(user_ਅੰਕੜੇ, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    }
    ਵਾਪਸ abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ਸਥਿਰ ਪੂਰਨ
mark_quickfix_ctx(qf_info_T *qi, ਪੂਰਨ copyID)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    ਲਈ (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	ਜੇ (ctx != ਨਿਰਾ && ctx->v_type != VAR_ਸੰਖਿਆ
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(ctx, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(cb, copyID);
    }

    ਵਾਪਸ abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_quickfix(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    ਜੇ (ql_info == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    ਜੇ (abort)
	ਵਾਪਸ abort;

    abort = mark_quickfix_user_ਅੰਕੜੇ(ql_info, copyID);
    ਜੇ (abort)
	ਵਾਪਸ abort;

    abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&qftf_cb, copyID);
    ਜੇ (abort)
	ਵਾਪਸ abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ਜੇ (win->w_llist != ਨਿਰਾ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    ਜੇ (abort)
		ਵਾਪਸ abort;

	    abort = mark_quickfix_user_ਅੰਕੜੇ(win->w_llist, copyID);
	    ਜੇ (abort)
		ਵਾਪਸ abort;
	}
	ਜੇ (IS_LL_ਖਿੜਕੀ(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    ਜੇ (abort)
		ਵਾਪਸ abort;

	    abort = mark_quickfix_user_ਅੰਕੜੇ(win->w_llist_ref, copyID);
	    ਜੇ (abort)
		ਵਾਪਸ abort;
	}
    }

    ਵਾਪਸ abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ਸਥਿਰ char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_cbuffer:	ਵਾਪਸ (char_u *)"cbuffer";
	ਹਾਲਤ CMD_cgetbuffer:	ਵਾਪਸ (char_u *)"cgetbuffer";
	ਹਾਲਤ CMD_caddbuffer:	ਵਾਪਸ (char_u *)"caddbuffer";
	ਹਾਲਤ CMD_lbuffer:	ਵਾਪਸ (char_u *)"lbuffer";
	ਹਾਲਤ CMD_lgetbuffer:	ਵਾਪਸ (char_u *)"lgetbuffer";
	ਹਾਲਤ CMD_laddbuffer:	ਵਾਪਸ (char_u *)"laddbuffer";
	ਮੂਲ:		ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ਸਥਿਰ ਪੂਰਨ
cbuffer_process_ਦਲੀਲਾਂ(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ਨਿਰਾ;

    ਜੇ (*eap->arg == NUL)
	buf = curbuf;
    ਨਹੀਂਤਾਂ ਜੇ (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((ਅੱਖਰ *)eap->arg));

    ਜੇ (buf == ਨਿਰਾ)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	emsg(_(e_ਬਫਰ_is_not_loaded));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    }

    ਜੇ (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    ਵਾਪਸ OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    ਖਾਲੀ
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = ਨਿਰਾ;
    qf_info_T	*qi;
    char_u	*au_name = ਨਿਰਾ;
    ਪੂਰਨ		res;
    int_u	save_qfid;
    win_T	*wp = ਨਿਰਾ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ਜੇ (aborting())
	    ਵਾਪਸ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ਢੇਰ(eap, &wp);
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (cbuffer_process_ਦਲੀਲਾਂ(eap, &buf, &line1, &line2) == FAIL)
	ਵਾਪਸ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    ਜੇ (buf->b_sfname)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%s (%s)",
		(ਅੱਖਰ *)qf_title, (ਅੱਖਰ *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ਨਿਰਾ, buf, ਨਿਰਾ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ਨਿਰਾ);
    ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
    {
	decr_quickfix_busy();
	ਵਾਪਸ;
    }
    ਜੇ (res >= 0)
	qf_list_ਬਦਲਿਆ(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ਜੇ (au_name != ਨਿਰਾ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	ਜੇ (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ਜੇ (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ਪਹਿਲਾ(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# ਜੇ defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_cexpr:	    ਵਾਪਸ (char_u *)"cexpr";
	ਹਾਲਤ CMD_cgetexpr:  ਵਾਪਸ (char_u *)"cgetexpr";
	ਹਾਲਤ CMD_caddexpr:  ਵਾਪਸ (char_u *)"caddexpr";
	ਹਾਲਤ CMD_lexpr:	    ਵਾਪਸ (char_u *)"lexpr";
	ਹਾਲਤ CMD_lgetexpr:  ਵਾਪਸ (char_u *)"lgetexpr";
	ਹਾਲਤ CMD_laddexpr:  ਵਾਪਸ (char_u *)"laddexpr";
	ਮੂਲ:	    ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਪੂਰਨ
trigger_cexpr_autocmd(ਪੂਰਨ cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	ਜੇ (aborting())
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ਨਿਰਾ;

    qi = qf_cmd_get_or_alloc_ਢੇਰ(eap, &wp);
    ਜੇ (qi == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ ((tv->v_type == VAR_STRING && tv->vval.v_string != ਨਿਰਾ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ਨਿਰਾ))
    {
	ਪੂਰਨ	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ਨਿਰਾ, ਨਿਰਾ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ਨਿਰਾ);
	ਜੇ (qf_ਢੇਰ_ਖਾਲੀ_ਥਾਵਾਂ(qi))
	{
	    decr_quickfix_busy();
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (res >= 0)
	    qf_list_ਬਦਲਿਆ(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	ਜੇ (au_name != ਨਿਰਾ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	ਜੇ (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_ਪਹਿਲਾ(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	ਵਾਪਸ OK;
    }

    emsg(_(e_string_or_list_expected));
    ਵਾਪਸ FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    ਖਾਲੀ
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    ਜੇ (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	ਵਾਪਸ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_ਵਿਅੰਜਕ(eap->arg, eap);
    ਜੇ (tv == ਨਿਰਾ)
	ਵਾਪਸ;

    (ਖਾਲੀ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ਸਥਿਰ qf_info_T *
hgr_get_ll(ਪੂਰਨ *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    ਜੇ (bt_help(curwin->w_ਬਫਰ))
	wp = curwin;
    ਨਹੀਂਤਾਂ
	// Find an existing help window
	wp = qf_ਲੱਭੋ_help_win();

    ਜੇ (wp == ਨਿਰਾ)	    // Help window not found
	qi = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	qi = wp->w_llist;

    ਜੇ (qi == ਨਿਰਾ)
    {
	// Allocate a new location list for help text matches
	ਜੇ ((qi = qf_alloc_ਢੇਰ(QFLT_LOCATION, 1)) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	*new_ll = TRUE;
    }

    ਵਾਪਸ qi;
}

/*
 * Search for a pattern in a help file.
 */
    ਸਥਿਰ ਖਾਲੀ
hgr_ਖੋਜ_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    ਲੰਮਾ	lnum;

    fd = mch_fopen((ਅੱਖਰ *)fname, "r");
    ਜੇ (fd == ਨਿਰਾ)
	ਵਾਪਸ;

    lnum = 1;
    ਜਦੋਂ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *ਸਤਰ = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	ਜੇ (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    ਸਤਰ = string_convert(p_vc, IObuff, ਨਿਰਾ);
	    ਜੇ (ਸਤਰ == ਨਿਰਾ)
		ਸਤਰ = IObuff;
	}

	ਜੇ (vim_regexec(p_regmatch, ਸਤਰ, (colnr_T)0))
	{
	    ਪੂਰਨ	l = (ਪੂਰਨ)STRLEN(ਸਤਰ);

	    // remove trailing CR, LF, spaces, etc.
	    ਜਦੋਂ (l > 0 && ਸਤਰ[l - 1] <= ' ')
		ਸਤਰ[--l] = NUL;

	    ਜੇ (qf_ਜੋੜੋ_ਇੰਦਰਾਜ(qfl,
			ਨਿਰਾ,	// dir
			fname,
			ਨਿਰਾ,
			0,
			ਸਤਰ,
			lnum,
			0,
			(ਪੂਰਨ)(p_regmatch->startp[0] - ਸਤਰ)
			+ 1,	//ਖਾਨਾ
			(ਪੂਰਨ)(p_regmatch->endp[0] - ਸਤਰ)
			+ 1,	// end_col
			FALSE,	// vis_col
			ਨਿਰਾ,	// search pattern
			0,	// nr
			1,	// type
			ਨਿਰਾ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		ਜੇ (ਸਤਰ != IObuff)
		    vim_free(ਸਤਰ);
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਸਤਰ != IObuff)
	    vim_free(ਸਤਰ);
	++lnum;
	ਸਤਰ_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    ਸਥਿਰ ਖਾਲੀ
hgr_ਖੋਜ_files_ਵਿੱਚ_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    ਪੂਰਨ		fcount;
    char_u	**fnames;
    ਪੂਰਨ		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    ਜੋੜੋ_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    ਜੇ (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	ਲਈ (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    ਜੇ (lang != ਨਿਰਾ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		ਜਾਰੀ;
# endif

	    hgr_ਖੋਜ_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    ਸਥਿਰ ਖਾਲੀ
hgr_ਖੋਜ_ਵਿੱਚ_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    ਜੇ (!enc_utf8)
	convert_ਤਿਆਰੀ(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    ਜਦੋਂ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_ਖੋਜ_files_ਵਿੱਚ_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    ਜੇ (vc.vc_type != CONV_NONE)
	convert_ਤਿਆਰੀ(&vc, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * ":helpgrep {pattern}"
 */
    ਖਾਲੀ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    ਪੂਰਨ		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    ਪੂਰਨ		new_qi = FALSE;
    char_u	*au_name =  ਨਿਰਾ;
    char_u	*lang = ਨਿਰਾ;
    ਪੂਰਨ		updated = FALSE;

    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ਰੋਕੋ;
	ਹਾਲਤ CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ਰੋਕੋ;
	ਮੂਲ: ਰੋਕੋ;
    }
    ਜੇ (au_name != ਨਿਰਾ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ਜੇ (aborting())
	    ਵਾਪਸ;
# endif
    }

    ਜੇ (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	ਜੇ (qi == ਨਿਰਾ)
	    ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (qi == ਨਿਰਾ)
    {
	emsg(_(e_no_quickfix_ਢੇਰ));
	ਵਾਪਸ;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_ਖੋਜ_ਵਿੱਚ_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_ਸ਼ੁਰੂ;
	qfl->qf_ਸੂਚਕਾਂਕ = 1;
	qf_list_ਬਦਲਿਆ(qfl);
	updated = TRUE;
    }

    ਜੇ (p_cpo == ਖਾਲੀ_ਥਾਵਾਂ_option)
	p_cpo = save_cpo;
    ਨਹੀਂਤਾਂ
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	ਜੇ (*p_cpo == NUL)
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	ਜੇ (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    ਜੇ (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_ਬਫਰ(qi, ਨਿਰਾ);

    ਜੇ (au_name != ਨਿਰਾ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	ਜੇ (!new_qi && IS_LL_ਢੇਰ(qi) && qf_ਲੱਭੋ_win_with_loclist(qi) == ਨਿਰਾ)
	{
	    decr_quickfix_busy();
	    ਵਾਪਸ;
	}
    }

    // Jump to first match.
    ਜੇ (!qf_list_ਖਾਲੀ_ਥਾਵਾਂ(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    ਨਹੀਂਤਾਂ
	semsg(_(e_no_ਮੇਲ_str_2), eap->arg);

    decr_quickfix_busy();

    ਜੇ (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	ਜੇ (!bt_help(curwin->w_ਬਫਰ) || curwin->w_llist == qi)
	{
	    ਜੇ (new_qi)
		ll_free_all(&qi);
	}
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_llist == ਨਿਰਾ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_quickfix(ਖਾਲੀ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ਨਿਰਾ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#ਜੇ_ਅੰਤ // FEAT_QUICKFIX

#ਜੇ defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ਸਥਿਰ ਖਾਲੀ
get_qf_loc_list(ਪੂਰਨ is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    ਜੇ (what_arg->v_type == VAR_UNKNOWN)
    {
	ਜੇ (rettv_list_alloc(rettv) == OK)
	    ਜੇ (is_qf || wp != ਨਿਰਾ)
		(ਖਾਲੀ)get_errorlist(ਨਿਰਾ, wp, -1, 0, rettv->vval.v_list);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (rettv_dict_alloc(rettv) == OK)
	    ਜੇ (is_qf || (wp != ਨਿਰਾ))
	    {
		ਜੇ (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    ਜੇ (d != ਨਿਰਾ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		ਨਹੀਂਤਾਂ
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    ਖਾਲੀ
f_getloclist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    ਖਾਲੀ
f_getqflist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_QUICKFIX
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    get_qf_loc_list(TRUE, ਨਿਰਾ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_qf_ll_list(
    win_T	*wp ਅਣਵਰਤਿਆ,
    typval_T	*list_arg ਅਣਵਰਤਿਆ,
    typval_T	*action_arg ਅਣਵਰਤਿਆ,
    typval_T	*what_arg ਅਣਵਰਤਿਆ,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    ਪੂਰਨ		action = 0;
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;
# endif

    rettv->vval.v_ਸੰਖਿਆ = -1;

# ifdef FEAT_QUICKFIX
    ਜੇ (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    ਨਹੀਂਤਾਂ ਜੇ (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    ਨਹੀਂਤਾਂ
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ਨਿਰਾ;
	ਪੂਰਨ	valid_dict = TRUE;

	ਜੇ (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    ਜੇ (act == ਨਿਰਾ)
		ਵਾਪਸ;		// type error; errmsg already given
	    ਜੇ ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_action_str_1), act);
	}
	ਨਹੀਂਤਾਂ ਜੇ (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	ਨਹੀਂਤਾਂ
	    emsg(_(e_string_required));

	ਜੇ (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    ਜੇ (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ਨਿਰਾ)
		what = what_arg->vval.v_dict;
	    ਨਹੀਂਤਾਂ
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	ਜੇ (l != ਨਿਰਾ && action && valid_dict
		    && ਸੈੱਟ_errorlist(wp, l, action,
		     (char_u *)(wp == ਨਿਰਾ ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_ਸੰਖਿਆ = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    ਖਾਲੀ
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	ਵਾਪਸ;

    win = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (win != ਨਿਰਾ)
	ਸੈੱਟ_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    ਖਾਲੀ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ਵਾਪਸ;

    ਸੈੱਟ_qf_ll_list(ਨਿਰਾ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#ਜੇ_ਅੰਤ
