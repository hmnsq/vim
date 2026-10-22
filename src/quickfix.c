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

#ಆದರೆ defined(FEAT_QUICKFIX)

ರಚನೆ dir_ರಾಶಿ_T
{
    ರಚನೆ dir_ರಾಶಿ_T	*ಮುಂದಿನ;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ qfline_S qfline_T;
ರಚನೆ qfline_S
{
    qfline_T	*qf_ಮುಂದಿನ;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_ಕೊನೆ_lnum;	// line number when the error has range or zero
    ಪೂರ್ಣಾಂಕ		qf_fnum;	// file number for the line
    ಪೂರ್ಣಾಂಕ		qf_ಲಂಬಸಾಲು;		// column where the error occurred
    ಪೂರ್ಣಾಂಕ		qf_ಕೊನೆ_ಲಂಬಸಾಲು;	// column when the error has range or zero
    ಪೂರ್ಣಾಂಕ		qf_nr;		// error number
    char_u	*qf_module;	// module name for this error
    char_u	*qf_fname;	// different filename if there're hard links
    char_u	*qf_ನಮೂನೆ;	// search pattern for the error
    char_u	*qf_ಪಠ್ಯ;	// description of the error
    char_u	qf_viscol;	// set to TRUE if qf_col and qf_end_col is
				// screen column
    char_u	qf_cleared;	// set to TRUE if line has been deleted
    char_u	qf_type;	// type of the error (mostly 'E'); 1 for
				// :helpgrep
    typval_T	qf_user_ದತ್ತಾಂಶ;	// custom user data associated with this item
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    QFLT_QUICKFIX, // Quickfix list - global list
    QFLT_LOCATION, // Location list - per window list
    QFLT_ಆಂತರಿಕ  // Internal - Temporary list used by getqflist()/getloclist()
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_ಆರಂಭ;	// pointer to the first error
    qfline_T	*qf_ಕೊನೆಯ;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    ಪೂರ್ಣಾಂಕ		qf_ಎಣಿಕೆ;	// number of errors (0 means empty list)
    ಪೂರ್ಣಾಂಕ		qf_ಸೂಚ್ಯಂಕ;	// current index in the error list
    ಪೂರ್ಣಾಂಕ		qf_nonevalid;	// TRUE if not a single valid entry found
    ಪೂರ್ಣಾಂಕ		qf_has_user_ದತ್ತಾಂಶ; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ರಚನೆ dir_ರಾಶಿ_T	*qf_dir_ರಾಶಿ;
    char_u		*qf_directory;
    ರಚನೆ dir_ರಾಶಿ_T	*qf_file_ರಾಶಿ;
    char_u		*qf_currfile;
    ಪೂರ್ಣಾಂಕ			qf_multiline;
    ಪೂರ್ಣಾಂಕ			qf_multiignore;
    ಪೂರ್ಣಾಂಕ			qf_multiscan;
    ದೀರ್ಘ		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ರಚನೆ qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    ಪೂರ್ಣಾಂಕ		qf_refcount;
    ಪೂರ್ಣಾಂಕ		qf_listcount;	    // current number of lists
    ಪೂರ್ಣಾಂಕ		qf_curlist;	    // current error list
    ಪೂರ್ಣಾಂಕ         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    ಪೂರ್ಣಾಂಕ		qf_bufnr;	    // quickfix window buffer number
};

ಸ್ಥಿರ qf_info_T ql_info_actual; // global quickfix list
ಸ್ಥಿರ qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
ಸ್ಥಿರ int_u ಕೊನೆಯ_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ efm_S efm_T;
ರಚನೆ efm_S
{
    regprog_T	    *prog;	// pre-formatted part of 'errorformat'
    efm_T	    *ಮುಂದಿನ;	// pointer to next (NULL if last)
    char_u	    addr[FMT_PATTERNS]; // indices of used % patterns
    char_u	    ಪೂರ್ವಪ್ರತ್ಯಯ;	// prefix of this format line:
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
    ಪೂರ್ಣಾಂಕ		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ qf_delq_S
{
    ರಚನೆ qf_delq_S	*ಮುಂದಿನ;
    qf_info_T		*qi;
} qf_delq_T;
ಸ್ಥಿರ qf_delq_T *qf_delq_ತಲೆ = ಖಾಲಿ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	quickfix_busy = 0;

ಸ್ಥಿರ efm_T	*fmt_ಆರಂಭ = ಖಾಲಿ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
ಸ್ಥಿರ callback_T qftf_cb;

ಸ್ಥಿರ ಶೂನ್ಯ     qf_ತೆಗೆ_ರಾಶಿ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ adjust);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_new_list(qf_info_T *qi, char_u *qf_title);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qf_ಕೂಡಿಸು_ನಮೂದು(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, ಪೂರ್ಣಾಂಕ bufnum, char_u *mesg, ದೀರ್ಘ lnum, ದೀರ್ಘ ಕೊನೆ_lnum, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ vis_ಲಂಬಸಾಲು, char_u *ನಮೂನೆ, ಪೂರ್ಣಾಂಕ nr, ಪೂರ್ಣಾಂಕ type, typval_T *user_ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ valid);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      qf_resize_ರಾಶಿ_ತಳ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ     qf_sync_llw_ಗಮ್ಯ_win(win_T *llw);
ಸ್ಥಿರ ಶೂನ್ಯ     qf_sync_win_ಗಮ್ಯ_llw(win_T *pwp);
ಸ್ಥಿರ qf_info_T *qf_alloc_ರಾಶಿ(qfltype_T qfltype, ಪೂರ್ಣಾಂಕ n);
ಸ್ಥಿರ qf_list_T *qf_alloc_list_ರಾಶಿ(ಪೂರ್ಣಾಂಕ n);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_free(qf_list_T *qfl);
ಸ್ಥಿರ char_u	*qf_types(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
ಸ್ಥಿರ char_u	*qf_ತಳ್ಳು_dir(char_u *, ರಚನೆ dir_ರಾಶಿ_T **, ಪೂರ್ಣಾಂಕ is_file_ರಾಶಿ);
ಸ್ಥಿರ char_u	*qf_ತೆಗೆ_dir(ರಚನೆ dir_ರಾಶಿ_T **);
ಸ್ಥಿರ char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
ಸ್ಥಿರ win_T    *qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qf_info_T *ll);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_jump_newwin(qf_info_T *qi, ಪೂರ್ಣಾಂಕ dir, ಪೂರ್ಣಾಂಕ errornr, ಪೂರ್ಣಾಂಕ forceit, ಪೂರ್ಣಾಂಕ newwin);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_fmt_ಪಠ್ಯ(garray_T *gap, char_u *ಪಠ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_range_ಪಠ್ಯ(garray_T *gap, qfline_T *qfp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qf_win_pos_update(qf_info_T *qi, ಪೂರ್ಣಾಂಕ old_qf_ಸೂಚ್ಯಂಕ);
ಸ್ಥಿರ win_T	*qf_ಕಂಡುಹಿಡಿ_win(qf_info_T *qi);
ಸ್ಥಿರ buf_T	*qf_ಕಂಡುಹಿಡಿ_buf(qf_info_T *qi);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_update_ಬಫರ್(qf_info_T *qi, qfline_T *old_ಕೊನೆಯ);
ಸ್ಥಿರ ಶೂನ್ಯ	qf_fill_ಬಫರ್(qf_list_T *qfl, buf_T *buf, qfline_T *old_ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ qf_winid);
ಸ್ಥಿರ buf_T	*ತುಂಬು_dummy_ಬಫರ್(char_u *fname, char_u *dirname_ಆರಂಭ, char_u *resulting_dir);
ಸ್ಥಿರ ಶೂನ್ಯ	wipe_dummy_ಬಫರ್(buf_T *buf, char_u *dirname_ಆರಂಭ);
ಸ್ಥಿರ ಶೂನ್ಯ	unload_dummy_ಬಫರ್(buf_T *buf, char_u *dirname_ಆರಂಭ);
ಸ್ಥಿರ qf_info_T *ll_get_or_alloc_list(win_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ನಮೂದು_is_closer_ಗಮ್ಯ_ಗುರಿ(qfline_T *ನಮೂದು, qfline_T *other_ನಮೂದು, ಪೂರ್ಣಾಂಕ ಗುರಿ_fnum, ಪೂರ್ಣಾಂಕ ಗುರಿ_lnum, ಪೂರ್ಣಾಂಕ ಗುರಿ_ಲಂಬಸಾಲು);

// Quickfix window check helper macro
# define IS_QF_ಕಿಟಕಿ(wp) (bt_quickfix((wp)->w_ಬಫರ್) && (wp)->w_llist_ref == ಖಾಲಿ)
// Location list window check helper macro
# define IS_LL_ಕಿಟಕಿ(wp) (bt_quickfix((wp)->w_ಬಫರ್) && (wp)->w_llist_ref != ಖಾಲಿ)

// Quickfix and location list stack check helper macros
# define IS_QF_ರಾಶಿ(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_ರಾಶಿ(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_ಕಿಟಕಿ(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i) \
		    ಗಾಗಿ ((i) = 1, (qfp) = (qfl)->qf_ಆರಂಭ; \
			    !got_int && (i) <= (qfl)->qf_ಎಣಿಕೆ && (qfp) != ಖಾಲಿ; \
			    ++(i), (qfp) = (qfp)->qf_ಮುಂದಿನ)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
ಸ್ಥಿರ char_u   *qf_ಕೊನೆಯ_bufname = ಖಾಲಿ;
ಸ್ಥಿರ bufref_T  qf_ಕೊನೆಯ_bufref = {ಖಾಲಿ, 0, 0};

ಸ್ಥಿರ garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    ಸ್ಥಿರ garray_T *
qfga_get(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initialized = FALSE;

    ಆದರೆ (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    ಹಿಂತಿರುಗಿ &qfga;
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
    ಆದರೆ (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    ಇಲ್ಲದಿದ್ದರೆ
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define ಸಾಲು_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
ಸ್ಥಿರ ರಚನೆ fmtpattern
{
    char_u	convchar;
    ಅಕ್ಷರ	*ನಮೂನೆ;
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
# define FMT_ನಮೂನೆ_M 8
	{'m', ".\\+"},		// 8
# define FMT_ನಮೂನೆ_R 9
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
efmpat_ಗಮ್ಯ_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	ಪೂರ್ಣಾಂಕ	idx,
	ಪೂರ್ಣಾಂಕ	ಸುತ್ತು)
{
    char_u	*srcptr;

    ಆದರೆ (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_ಒಳ_format_string), *efmpat);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ ((idx && idx < FMT_ನಮೂನೆ_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->ಪೂರ್ವಪ್ರತ್ಯಯ) != ಖಾಲಿ)
	    || (idx == FMT_ನಮೂನೆ_R
		&& vim_strchr((char_u *)"OPQ", efminfo->ಪೂರ್ವಪ್ರತ್ಯಯ) == ಖಾಲಿ))
    {
	semsg(_(e_unexpected_chr_ಒಳ_format_str), *efmpat);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    efminfo->addr[idx] = (char_u)++ಸುತ್ತು;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    ಆದರೆ (*efmpat == 'f' && efmpat[1] != NUL)
    {
	ಆದರೆ (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	srcptr = (char_u *)fmt_pat[idx].ನಮೂನೆ;
	ಆಗ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    ಹಿಂತಿರುಗಿ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    ಸ್ಥಿರ char_u *
scanf_fmt_ಗಮ್ಯ_regpat(
	char_u	**pefmp,
	char_u	*efm,
	ಪೂರ್ಣಾಂಕ	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    ಆದರೆ (*efmp == '[' || *efmp == '\\')
    {
	ಆದರೆ ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    ಆದರೆ (efmp[1] == '^')
		*regpat++ = *++efmp;
	    ಆದರೆ (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		ಆಗ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		ಆದರೆ (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_ಒಳ_format_string));
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_ಒಳ_format_string), *efmp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    *pefmp = efmp;

    ಹಿಂತಿರುಗಿ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    ಸ್ಥಿರ char_u *
efm_analyze_ಪೂರ್ವಪ್ರತ್ಯಯ(char_u *efmp, efm_T *efminfo)
{
    ಆದರೆ (vim_strchr((char_u *)"+-", *efmp) != ಖಾಲಿ)
	efminfo->flags = *efmp++;
    ಆದರೆ (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ಖಾಲಿ)
	efminfo->ಪೂರ್ವಪ್ರತ್ಯಯ = *efmp;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_chr_ಒಳ_format_string_ಪೂರ್ವಪ್ರತ್ಯಯ), *efmp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
efm_ಗಮ್ಯ_regpat(
	char_u	*efm,
	ಪೂರ್ಣಾಂಕ	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    ಸುತ್ತು = 0;
    ಗಾಗಿ (efmp = efm; efmp < efm + len; ++efmp)
    {
	ಆದರೆ (*efmp == '%')
	{
	    ++efmp;
	    ಗಾಗಿ (idx = 0; idx < FMT_PATTERNS; ++idx)
		ಆದರೆ (fmt_pat[idx].convchar == *efmp)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_ಗಮ್ಯ_regpat(efmp, ptr, fmt_ptr, idx, ಸುತ್ತು);
		ಆದರೆ (ptr == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಸುತ್ತು++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_ಗಮ್ಯ_regpat(&efmp, efm, len, ptr);
		ಆದರೆ (ptr == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ಖಾಲಿ)
		*ptr++ = *efmp;		// regexp magic characters
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*efmp == '#')
		*ptr++ = '*';
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_ಪೂರ್ವಪ್ರತ್ಯಯ(efmp, fmt_ptr);
		ಆದರೆ (efmp == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_invalid_chr_ಒಳ_format_string), *efmp);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ			// copy normal character
	{
	    ಆದರೆ (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)".*^$~[", *efmp) != ಖಾಲಿ)
		*ptr++ = '\\';	// escape regexp atoms
	    ಆದರೆ (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Free the 'errorformat' information list
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_efm_list(efm_T **efm_ಮೊದಲ)
{
    efm_T *efm_ptr;

    ಗಾಗಿ (efm_ptr = *efm_ಮೊದಲ; efm_ptr != ಖಾಲಿ; efm_ptr = *efm_ಮೊದಲ)
    {
	*efm_ಮೊದಲ = efm_ptr->ಮುಂದಿನ;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_ಆರಂಭ = ಖಾಲಿ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
efm_regpat_bufsz(char_u *efm)
{
    ಪೂರ್ಣಾಂಕ sz;
    ಪೂರ್ಣಾಂಕ i;

    sz = (FMT_PATTERNS * 3) + ((ಪೂರ್ಣಾಂಕ)STRLEN(efm) << 2);
    ಗಾಗಿ (i = FMT_PATTERNS; i > 0; )
	sz += (ಪೂರ್ಣಾಂಕ)STRLEN(fmt_pat[--i].ನಮೂನೆ);
# ifdef BACKSLASH_ಒಳ_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# ಇಲ್ಲದಿದ್ದರೆ
    sz += 2; // "%f" can become two chars longer
# endif

    ಹಿಂತಿರುಗಿ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
efm_option_part_len(char_u *efm)
{
    ಪೂರ್ಣಾಂಕ len;

    ಗಾಗಿ (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	ಆದರೆ (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    ಹಿಂತಿರುಗಿ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    ಸ್ಥಿರ efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ಖಾಲಿ;
    efm_T	*fmt_ಮೊದಲ = ಖಾಲಿ;
    efm_T	*fmt_ಕೊನೆಯ = ಖಾಲಿ;
    char_u	*fmtstr = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    ಆದರೆ ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ಖಾಲಿ)
	ಹೋಗು parse_efm_error;

    ಆಗ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	ಆದರೆ (fmt_ptr == ಖಾಲಿ)
	    ಹೋಗು parse_efm_error;
	ಆದರೆ (fmt_ಮೊದಲ == ಖಾಲಿ)	    // first one
	    fmt_ಮೊದಲ = fmt_ptr;
	ಇಲ್ಲದಿದ್ದರೆ
	    fmt_ಕೊನೆಯ->ಮುಂದಿನ = fmt_ptr;
	fmt_ಕೊನೆಯ = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	ಆದರೆ (efm_ಗಮ್ಯ_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    ಹೋಗು parse_efm_error;
	ಆದರೆ ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ಖಾಲಿ)
	    ಹೋಗು parse_efm_error;
	// Advance to next part
	efm = skip_ಗಮ್ಯ_option_part(efm + len);	// skip comma and spaces
    }

    ಆದರೆ (fmt_ಮೊದಲ == ಖಾಲಿ)	// nothing found
	emsg(_(e_errorformat_ಒಳಗೊಂಡಿದೆ_no_ನಮೂನೆ));

    ಹೋಗು parse_efm_ಕೊನೆ;

parse_efm_error:
    free_efm_list(&fmt_ಮೊದಲ);

parse_efm_ಕೊನೆ:
    vim_free(fmtstr);

    ಹಿಂತಿರುಗಿ fmt_ಮೊದಲ;
}

ಎಣಿಕೆ {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_ಕೊನೆ_ದ_ಒಳಹರಿವು = 2,
    QF_NOMEM = 3,
    QF_IGNORE_ಸಾಲು = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*linebuf;
    ಪೂರ್ಣಾಂಕ		linelen;
    char_u	*growbuf;
    ಪೂರ್ಣಾಂಕ		growbufsiz;
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
qf_grow_linebuf(qfstate_T *state, ಪೂರ್ಣಾಂಕ newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > ಸಾಲು_MAXLEN ? ಸಾಲು_MAXLEN - 1 : newsz;
    ಆದರೆ (state->growbuf == ಖಾಲಿ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	ಆದರೆ (state->growbuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	state->growbufsiz = state->linelen;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->linelen > state->growbufsiz)
    {
	ಆದರೆ ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    ಹಿಂತಿರುಗಿ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_ಮುಂದಿನ_str_ಸಾಲು(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (*p_str == NUL) // Reached the end of the string
	ಹಿಂತಿರುಗಿ QF_ಕೊನೆ_ದ_ಒಳಹರಿವು;

    p = vim_strchr(p_str, '\n');
    ಆದರೆ (p != ಖಾಲಿ)
	len = (ಪೂರ್ಣಾಂಕ)(p - p_str) + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_str);

    ಆದರೆ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಆದರೆ (state->linebuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_NOMEM;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_ಮುಂದಿನ_list_ಸಾಲು(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    ಪೂರ್ಣಾಂಕ		len;

    ಆಗ (p_li != ಖಾಲಿ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ಖಾಲಿ))
	p_li = p_li->li_ಮುಂದಿನ;	// Skip non-string items

    ಆದರೆ (p_li == ಖಾಲಿ)		// End of the list
    {
	state->p_li = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ QF_ಕೊನೆ_ದ_ಒಳಹರಿವು;
    }

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_li->li_tv.vval.v_string);
    ಆದರೆ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಆದರೆ (state->linebuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_NOMEM;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_ಮುಂದಿನ;	// next item
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_ಮುಂದಿನ_buf_ಸಾಲು(qfstate_T *state)
{
    char_u	*p_buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;

    // Get the next line from the supplied buffer
    ಆದರೆ (state->buflnum > state->lnumlast)
	ಹಿಂತಿರುಗಿ QF_ಕೊನೆ_ದ_ಒಳಹರಿವು;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    ಆದರೆ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	ಆದರೆ (state->linebuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_NOMEM;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_ಮುಂದಿನ_file_ಸಾಲು(qfstate_T *state)
{
    ಪೂರ್ಣಾಂಕ	    discard;
    ಪೂರ್ಣಾಂಕ	    growbuflen;

    ಆದರೆ (fgets((ಅಕ್ಷರ *)IObuff, IOSIZE, state->fd) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_ಕೊನೆ_ದ_ಒಳಹರಿವು;

    discard = FALSE;
    state->linelen = (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff);
    ಆದರೆ (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	ಆದರೆ (state->growbuf == ಖಾಲಿ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    ಆದರೆ (state->growbuf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	ಸ್ಮರಣೆ_ನಕಲು(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	ಗಾಗಿ (;;)
	{
	    char_u	*p;

	    ಆದರೆ (fgets((ಅಕ್ಷರ *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    state->linelen = (ಪೂರ್ಣಾಂಕ)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    ಆದರೆ ((state->growbuf)[growbuflen - 1] == '\n')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (state->growbufsiz == ಸಾಲು_MAXLEN)
	    {
		discard = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < ಸಾಲು_MAXLEN
		? 2 * state->growbufsiz : ಸಾಲು_MAXLEN;
	    ಆದರೆ ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ QF_NOMEM;
	    state->growbuf = p;
	}

	ಆಗ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    ಆದರೆ (fgets((ಅಕ್ಷರ *)IObuff, IOSIZE, state->fd) == ಖಾಲಿ
		    || (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		ನಿಲ್ಲಿಸು;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    ಆದರೆ (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*ಸಾಲು;

	ಸಾಲು = string_convert(&state->vc, state->linebuf, &state->linelen);
	ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
	{
	    ಆದರೆ (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, ಸಾಲು);
		vim_free(ಸಾಲು);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = ಸಾಲು;
		state->growbufsiz = state->linelen < ಸಾಲು_MAXLEN
						? state->linelen : ಸಾಲು_MAXLEN;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_nextline(qfstate_T *state)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = QF_FAIL;

    ಆದರೆ (state->fd == ಖಾಲಿ)
    {
	ಆದರೆ (state->tv != ಖಾಲಿ)
	{
	    ಆದರೆ (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		ಸ್ಥಿತಿ_2 = qf_get_ಮುಂದಿನ_str_ಸಾಲು(state);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		ಸ್ಥಿತಿ_2 = qf_get_ಮುಂದಿನ_list_ಸಾಲು(state);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Get the next line from the supplied buffer
	    ಸ್ಥಿತಿ_2 = qf_get_ಮುಂದಿನ_buf_ಸಾಲು(state);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Get the next line from the supplied file
	ಸ್ಥಿತಿ_2 = qf_get_ಮುಂದಿನ_file_ಸಾಲು(state);

    ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

    // remove newline/CR from the line
    ಆದರೆ (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	ಆದರೆ (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    ಹಿಂತಿರುಗಿ QF_OK;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*namebuf;
    ಪೂರ್ಣಾಂಕ		bnr;
    char_u	*module;
    char_u	*errmsg;
    ಪೂರ್ಣಾಂಕ		errmsglen;
    ದೀರ್ಘ	lnum;
    ದೀರ್ಘ	ಕೊನೆ_lnum;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಲಂಬಸಾಲು;
    char_u	use_viscol;
    char_u	*ನಮೂನೆ;
    ಪೂರ್ಣಾಂಕ		enr;
    ಪೂರ್ಣಾಂಕ		type;
    typval_T	*user_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_f(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields, ಪೂರ್ಣಾಂಕ ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    ಪೂರ್ಣಾಂಕ c;

    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ || rmp->endp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    ಆದರೆ (vim_strchr((char_u *)"OPQ", ಪೂರ್ವಪ್ರತ್ಯಯ) != ಖಾಲಿ
	    && mch_getperm(fields->namebuf) == -1)
	ಹಿಂತಿರುಗಿ QF_FAIL;

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_b(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    ಪೂರ್ಣಾಂಕ bnr = (ಪೂರ್ಣಾಂಕ)atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಆದರೆ (buflist_findnr(bnr) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->bnr = bnr;
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_n(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->enr = (ಪೂರ್ಣಾಂಕ)atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_l(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->lnum = atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_e(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->ಕೊನೆ_lnum = atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_c(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_k(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->ಕೊನೆ_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_t(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->type = *rmp->startp[midx];
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_nonerror_ಸಾಲು(char_u *linebuf, ಪೂರ್ಣಾಂಕ linelen, qffields_T *fields)
{
    char_u	*p;

    ಆದರೆ (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	ಆದರೆ ((p = vim_realloc(fields->errmsg, linelen + 1)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_m(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ || rmp->endp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    len = (ಪೂರ್ಣಾಂಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಆದರೆ (len >= fields->errmsglen)
    {
	// len + null terminator
	ಆದರೆ ((p = vim_realloc(fields->errmsg, len + 1)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_r(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, char_u **ಬಾಲ)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    *ಬಾಲ = rmp->startp[midx];
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_p(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    char_u	*ಹೊಂದಿಕೆ_ptr;

    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ || rmp->endp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->ಲಂಬಸಾಲು = 0;
    ಗಾಗಿ (ಹೊಂದಿಕೆ_ptr = rmp->startp[midx]; ಹೊಂದಿಕೆ_ptr != rmp->endp[midx];
								++ಹೊಂದಿಕೆ_ptr)
    {
	++fields->ಲಂಬಸಾಲು;
	ಆದರೆ (*ಹೊಂದಿಕೆ_ptr == TAB)
	{
	    fields->ಲಂಬಸಾಲು += 7;
	    fields->ಲಂಬಸಾಲು -= fields->ಲಂಬಸಾಲು % 8;
	}
    }
    ++fields->ಲಂಬಸಾಲು;
    fields->use_viscol = TRUE;
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_v(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    fields->ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)atol((ಅಕ್ಷರ *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_s(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ || rmp->endp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    len = (ಪೂರ್ಣಾಂಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಆದರೆ (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->ನಮೂನೆ, "^\\V");
    STRNCAT(fields->ನಮೂನೆ, rmp->startp[midx], len);
    fields->ನಮೂನೆ[len + 3] = '\\';
    fields->ನಮೂನೆ[len + 4] = '$';
    fields->ನಮೂನೆ[len + 5] = NUL;
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_fmt_o(regmatch_T *rmp, ಪೂರ್ಣಾಂಕ midx, qffields_T *fields)
{
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (rmp->startp[midx] == ಖಾಲಿ || rmp->endp[midx] == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    len = (ಪೂರ್ಣಾಂಕ)(rmp->endp[midx] - rmp->startp[midx]);
    ಆದರೆ (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, ಪೂರ್ಣಾಂಕ, qffields_T *) =
{
    ಖಾಲಿ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ಖಾಲಿ, // %r
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_ಹೊಂದಿಕೆ(
	char_u		*linebuf,
	ಪೂರ್ಣಾಂಕ		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	ಪೂರ್ಣಾಂಕ		qf_multiline,
	ಪೂರ್ಣಾಂಕ		qf_multiscan,
	char_u		**ಬಾಲ)
{
    ಪೂರ್ಣಾಂಕ		idx = fmt_ptr->ಪೂರ್ವಪ್ರತ್ಯಯ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		midx;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;

    ಆದರೆ ((idx == 'C' || idx == 'Z') && !qf_multiline)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    ಆದರೆ (vim_strchr((char_u *)"EWIN", idx) != ಖಾಲಿ)
	fields->type = idx;
    ಇಲ್ಲದಿದ್ದರೆ
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    ಗಾಗಿ (i = 0; i < FMT_PATTERNS; i++)
    {
	ಸ್ಥಿತಿ_2 = QF_OK;
	midx = (ಪೂರ್ಣಾಂಕ)fmt_ptr->addr[i];
	ಆದರೆ (i == 0 && midx > 0)				// %f
	    ಸ್ಥಿತಿ_2 = qf_parse_fmt_f(regmatch, midx, fields, idx);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == FMT_ನಮೂನೆ_M)
	{
	    ಆದರೆ (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		ಸ್ಥಿತಿ_2 = copy_nonerror_ಸಾಲು(linebuf, linelen, fields);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (midx > 0)				// %m
		ಸ್ಥಿತಿ_2 = qf_parse_fmt_m(regmatch, midx, fields);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == FMT_ನಮೂನೆ_R && midx > 0)	// %r
	    ಸ್ಥಿತಿ_2 = qf_parse_fmt_r(regmatch, midx, ಬಾಲ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (midx > 0)				// others
	    ಸ್ಥಿತಿ_2 = (qf_parse_fmt[i])(regmatch, midx, fields);

	ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
	    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_get_fields(
	char_u		*linebuf,
	ಪೂರ್ಣಾಂಕ		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	ಪೂರ್ಣಾಂಕ		qf_multiline,
	ಪೂರ್ಣಾಂಕ		qf_multiscan,
	char_u		**ಬಾಲ)
{
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = QF_FAIL;
    ಪೂರ್ಣಾಂಕ		r;

    ಆದರೆ (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->ಪೂರ್ವಪ್ರತ್ಯಯ) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->ನಮೂನೆ[0] = NUL;
    ಆದರೆ (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->ಕೊನೆ_lnum = 0;
    fields->ಲಂಬಸಾಲು = 0;
    fields->ಕೊನೆ_ಲಂಬಸಾಲು = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *ಬಾಲ = ಖಾಲಿ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    ಆದರೆ (r)
	ಸ್ಥಿತಿ_2 = qf_parse_ಹೊಂದಿಕೆ(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, ಬಾಲ);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_dir_pfx(ಪೂರ್ಣಾಂಕ idx, qffields_T *fields, qf_list_T *qfl)
{
    ಆದರೆ (idx == 'D')				// enter directory
    {
	ಆದರೆ (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_ಖಾಲಿಗಳು_directory_name));
	    ಹಿಂತಿರುಗಿ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_ತಳ್ಳು_dir(fields->namebuf, &qfl->qf_dir_ರಾಶಿ, FALSE);
	ಆದರೆ (qfl->qf_directory == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 'X')			// leave directory
	qfl->qf_directory = qf_ತೆಗೆ_dir(&qfl->qf_dir_ರಾಶಿ);

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_file_pfx(
	ಪೂರ್ಣಾಂಕ idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *ಬಾಲ)
{
    fields->valid = FALSE;
    ಆದರೆ (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	ಆದರೆ (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_ತಳ್ಳು_dir(fields->namebuf, &qfl->qf_file_ರಾಶಿ, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 'Q')
	    qfl->qf_currfile = qf_ತೆಗೆ_dir(&qfl->qf_file_ರಾಶಿ);
	*fields->namebuf = NUL;
	ಆದರೆ (ಬಾಲ && *ಬಾಲ)
	{
	    qfl->qf_multiscan = TRUE;
	    ಹಿಂತಿರುಗಿ QF_MULTISCAN;
	}
    }

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_ಸಾಲು_nomatch(char_u *linebuf, ಪೂರ್ಣಾಂಕ linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    ಹಿಂತಿರುಗಿ copy_nonerror_ಸಾಲು(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_multiline_pfx(
	ಪೂರ್ಣಾಂಕ idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    ಪೂರ್ಣಾಂಕ			len;

    ಆದರೆ (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_ಕೊನೆಯ;

	ಆದರೆ (qfprev == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ QF_FAIL;
	ಆದರೆ (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(qfprev->qf_ಪಠ್ಯ);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    ಆದರೆ (ptr == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_ಪಠ್ಯ);
	    vim_free(qfprev->qf_ಪಠ್ಯ);
	    qfprev->qf_ಪಠ್ಯ = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	ಆದರೆ (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	ಆದರೆ (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	ಆದರೆ (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	ಆದರೆ (!qfprev->qf_ಕೊನೆ_lnum)
	    qfprev->qf_ಕೊನೆ_lnum = fields->ಕೊನೆ_lnum;
	ಆದರೆ (!qfprev->qf_ಲಂಬಸಾಲು)
	{
	    qfprev->qf_ಲಂಬಸಾಲು = fields->ಲಂಬಸಾಲು;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	ಆದರೆ (!qfprev->qf_ಕೊನೆ_ಲಂಬಸಾಲು)
	    qfprev->qf_ಕೊನೆ_ಲಂಬಸಾಲು = fields->ಕೊನೆ_ಲಂಬಸಾಲು;
	ಆದರೆ (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ಖಾಲಿ
		    ? fields->namebuf
		    : qfl->qf_currfile != ಖಾಲಿ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    ಆದರೆ (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    ಸಾಲು_breakcheck();

    ಹಿಂತಿರುಗಿ QF_IGNORE_ಸಾಲು;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_parse_ಸಾಲು(
	qf_list_T	*qfl,
	char_u		*linebuf,
	ಪೂರ್ಣಾಂಕ		linelen,
	efm_T		*fmt_ಮೊದಲ,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    ಪೂರ್ಣಾಂಕ			idx = 0;
    char_u		*ಬಾಲ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			ಸ್ಥಿತಿ_2;

restofline:
    // If there was no %> item start at the first pattern
    ಆದರೆ (fmt_ಆರಂಭ == ಖಾಲಿ)
	fmt_ptr = fmt_ಮೊದಲ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_ಆರಂಭ;
	fmt_ಆರಂಭ = ಖಾಲಿ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    ಗಾಗಿ ( ; fmt_ptr != ಖಾಲಿ; fmt_ptr = fmt_ptr->ಮುಂದಿನ)
    {
	idx = fmt_ptr->ಪೂರ್ವಪ್ರತ್ಯಯ;
	ಸ್ಥಿತಿ_2 = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &ಬಾಲ);
	ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_NOMEM)
	    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
	ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_OK)
	    ನಿಲ್ಲಿಸು;
    }
    qfl->qf_multiscan = FALSE;

    ಆದರೆ (fmt_ptr == ಖಾಲಿ || idx == 'D' || idx == 'X')
    {
	ಆದರೆ (fmt_ptr != ಖಾಲಿ)
	{
	    // 'D' and 'X' directory specifiers
	    ಸ್ಥಿತಿ_2 = qf_parse_dir_pfx(idx, fields, qfl);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
		ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
	}

	ಸ್ಥಿತಿ_2 = qf_parse_ಸಾಲು_nomatch(linebuf, linelen, fields);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
	    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

	ಆದರೆ (fmt_ptr == ಖಾಲಿ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt_ptr != ಖಾಲಿ)
    {
	// honor %> item
	ಆದರೆ (fmt_ptr->conthere)
	    fmt_ಆರಂಭ = fmt_ptr;

	ಆದರೆ (vim_strchr((char_u *)"AEWIN", idx) != ಖಾಲಿ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"CZ", idx) != ಖಾಲಿ)
	{				// continuation of multi-line msg
	    ಸ್ಥಿತಿ_2 = qf_parse_multiline_pfx(idx, qfl, fields);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
		ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"OPQ", idx) != ಖಾಲಿ)
	{				// global file names
	    ಸ್ಥಿತಿ_2 = qf_parse_file_pfx(idx, fields, qfl, ಬಾಲ);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(ಬಾಲ);
		ಪೂರ್ಣಾಂಕ new_linelen = (ಪೂರ್ಣಾಂಕ)STRLEN(s);
		ಆದರೆ (new_linelen >= linelen)
		    ಹಿಂತಿರುಗಿ QF_IGNORE_ಸಾಲು;
		linebuf = s;
		linelen = new_linelen;
		ಹೋಗು restofline;
	    }
	}
	ಆದರೆ (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    ಆದರೆ (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    ಹಿಂತಿರುಗಿ QF_IGNORE_ಸಾಲು;
	}
    }

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ರಾಶಿ_ಖಾಲಿಗಳು(qf_info_T *qi)
{
    ಹಿಂತಿರುಗಿ qi == ಖಾಲಿ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_list_ಖಾಲಿಗಳು(qf_list_T *qfl)
{
    ಹಿಂತಿರುಗಿ qfl == ಖಾಲಿ || qfl->qf_ಎಣಿಕೆ <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_list_has_valid_entries(qf_list_T *qfl)
{
    ಹಿಂತಿರುಗಿ !qf_list_ಖಾಲಿಗಳು(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    ಸ್ಥಿರ qf_list_T *
qf_get_list(qf_info_T *qi, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->ನಮೂನೆ = alloc_id(CMDBUFFSIZE + 1, aid_qf_ನಮೂನೆ);
    ಆದರೆ (pfields->namebuf == ಖಾಲಿ || pfields->errmsg == ಖಾಲಿ
		|| pfields->ನಮೂನೆ == ಖಾಲಿ || pfields->module == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
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
    vim_free(pfields->ನಮೂನೆ);
}

/*
 * Setup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಸಿದ್ಧತೆ_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    ಆದರೆ (enc != ಖಾಲಿ && *enc != NUL)
	convert_ಸಿದ್ಧತೆ(&pstate->vc, enc, p_enc);

    ಆದರೆ (efile != ಖಾಲಿ && (pstate->fd = mch_fopen((ಅಕ್ಷರ *)efile, "r")) == ಖಾಲಿ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (tv != ಖಾಲಿ)
    {
	ಆದರೆ (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_ಮೊದಲ;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_cleanup_state(qfstate_T *pstate)
{
    ಆದರೆ (pstate->fd != ಖಾಲಿ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    ಆದರೆ (pstate->vc.vc_type != CONV_NONE)
	convert_ಸಿದ್ಧತೆ(&pstate->vc, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_ಮೊದಲ,
	qfstate_T	*state,
	qffields_T	*fields)
{
    ಪೂರ್ಣಾಂಕ		    ಸ್ಥಿತಿ_2;

    // Get the next line from a file/buffer/list/string
    ಸ್ಥಿತಿ_2 = qf_get_nextline(state);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

    ಸ್ಥಿತಿ_2 = qf_parse_ಸಾಲು(qfl, state->linebuf, state->linelen,
	    fmt_ಮೊದಲ, fields);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != QF_OK)
	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

    ಹಿಂತಿರುಗಿ qf_ಕೂಡಿಸು_ನಮೂದು(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ಖಾಲಿ)
		? fields->namebuf
		: ((qfl->qf_currfile != ಖಾಲಿ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ಖಾಲಿ),
		fields->module,
		fields->bnr,
		fields->errmsg,
		fields->lnum,
		fields->ಕೊನೆ_lnum,
		fields->ಲಂಬಸಾಲು,
		fields->ಕೊನೆ_ಲಂಬಸಾಲು,
		fields->use_viscol,
		fields->ನಮೂನೆ,
		fields->enr,
		fields->type,
		fields->user_ದತ್ತಾಂಶ,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_init_ext(
    qf_info_T	    *qi,
    ಪೂರ್ಣಾಂಕ		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    ಪೂರ್ಣಾಂಕ		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_ಕೊನೆಯ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    adding = FALSE;
    ಸ್ಥಿರ efm_T    *fmt_ಮೊದಲ = ಖಾಲಿ;
    char_u	    *efm;
    ಸ್ಥಿರ char_u   *ಕೊನೆಯ_efm = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    retval = -1;	// default: return error flag
    ಪೂರ್ಣಾಂಕ		    ಸ್ಥಿತಿ_2;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_ಕೊನೆಯ_bufname);

    CLEAR_ಕ್ಷೇತ್ರ(state);
    CLEAR_ಕ್ಷೇತ್ರ(fields);
    ಆದರೆ ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_ಸಿದ್ಧತೆ_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	ಹೋಗು qf_init_ಕೊನೆ;

    ಆದರೆ (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	ಆದರೆ (!qf_list_ಖಾಲಿಗಳು(qfl))
	    old_ಕೊನೆಯ = qfl->qf_ಕೊನೆಯ;
    }

    // Use the local value of 'errorformat' if it's set.
    ಆದರೆ (errorformat == p_efm && tv == ಖಾಲಿ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    ಇಲ್ಲದಿದ್ದರೆ
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    ಆದರೆ (ಕೊನೆಯ_efm == ಖಾಲಿ || (STRCMP(ಕೊನೆಯ_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(ಕೊನೆಯ_efm);
	free_efm_list(&fmt_ಮೊದಲ);

	// parse the current 'efm'
	fmt_ಮೊದಲ = parse_efm_option(efm);
	ಆದರೆ (fmt_ಮೊದಲ != ಖಾಲಿ)
	    ಕೊನೆಯ_efm = vim_strsave(efm);
    }

    ಆದರೆ (fmt_ಮೊದಲ == ಖಾಲಿ)	// nothing found
	ಹೋಗು error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    ಆಗ (!got_int)
    {
	ಸ್ಥಿತಿ_2 = qf_init_process_nextline(qfl, fmt_ಮೊದಲ, &state, &fields);
	ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_NOMEM)		// memory alloc failure
	    ಹೋಗು qf_init_ಕೊನೆ;
	ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_ಕೊನೆ_ದ_ಒಳಹರಿವು)	// end of input
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಸ್ಥಿತಿ_2 == QF_FAIL)
	    ಹೋಗು error2;

	ಸಾಲು_breakcheck();
    }
    ಆದರೆ (state.fd == ಖಾಲಿ || !ferror(state.fd))
    {
	ಆದರೆ (qfl->qf_ಸೂಚ್ಯಂಕ == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_ಆರಂಭ;
	    qfl->qf_ಸೂಚ್ಯಂಕ = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    qfl->qf_nonevalid = FALSE;
	    ಆದರೆ (qfl->qf_ptr == ಖಾಲಿ)
		qfl->qf_ptr = qfl->qf_ಆರಂಭ;
	}
	// return number of matches
	retval = qfl->qf_ಎಣಿಕೆ;
	ಹೋಗು qf_init_ಕೊನೆ;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    ಆದರೆ (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	ಆದರೆ (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_ಕೊನೆ:
    ಆದರೆ (qf_idx == qi->qf_curlist)
	qf_update_ಬಫರ್(qi, old_ಕೊನೆಯ);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    ಪೂರ್ಣಾಂಕ
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	ಪೂರ್ಣಾಂಕ	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    ಆದರೆ (wp != ಖಾಲಿ)
	qi = ll_get_or_alloc_list(wp);
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ಖಾಲಿ, errorformat,
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

    ಆದರೆ (title == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    ಆದರೆ (p != ಖಾಲಿ)
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

    vim_snprintf((ಅಕ್ಷರ *)qftitle_str, IOSIZE, ":%s", (ಅಕ್ಷರ *)cmd);
    ಹಿಂತಿರುಗಿ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    ಸ್ಥಿರ qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    ಹಿಂತಿರುಗಿ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ತೆಗೆ_ರಾಶಿ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ adjust)
{
    ಪೂರ್ಣಾಂಕ i;
    qf_free(&qi->qf_lists[0]);
    ಗಾಗಿ (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, ಗಾತ್ರ(*qi->qf_lists));

    ಆದರೆ (adjust)
    {
	qi->qf_listcount--;
	ಆದರೆ (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಆಗ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    ಆದರೆ (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_ತೆಗೆ_ರಾಶಿ(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    ಇಲ್ಲದಿದ್ದರೆ
	qi->qf_curlist = qi->qf_listcount++;

    qfl = qf_get_curlist(qi);
    CLEAR_ಸೂಚಕ(qfl);
    qf_store_title(qfl, qf_title);
    qfl->qfl_type = qi->qfl_type;
    qfl->qf_id = ++ಕೊನೆಯ_qf_id;
    qfl->qf_has_user_ದತ್ತಾಂಶ = FALSE;
}

/*
 * Queue location list stack delete request.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
locstack_ಸರತಿ_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    ಆದರೆ (q == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    q->qi = qi;
    q->ಮುಂದಿನ = qf_delq_ತಲೆ;
    qf_delq_ತಲೆ = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    ಪೂರ್ಣಾಂಕ
qf_ರಾಶಿ_get_bufnr(ಶೂನ್ಯ)
{
    ಆದರೆ (ql_info == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ INVALID_QFBUFNR;
    ಹಿಂತಿರುಗಿ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wipe_qf_ಬಫರ್(qf_info_T *qi)
{
    buf_T	*qfbuf;

    ಆದರೆ (qi->qf_bufnr == INVALID_QFBUFNR)
	ಹಿಂತಿರುಗಿ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    ಆದರೆ (qfbuf != ಖಾಲಿ && qfbuf->b_nwindows == 0)
    {
	ಪೂರ್ಣಾಂಕ buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ)
	{
	    curwin->w_ಬಫರ್ = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_ಬಫರ್(ಖಾಲಿ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	ಆದರೆ (buf_was_null)
	    curwin->w_ಬಫರ್ = ಖಾಲಿ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_list_ರಾಶಿ_ವಸ್ತುಗಳು(qf_info_T *qi)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < qi->qf_listcount; ++i)
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
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    *pqi = ಖಾಲಿ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    ಆದರೆ (quickfix_busy > 0)
    {
	locstack_ಸರತಿ_delreq(qi);
	ಹಿಂತಿರುಗಿ;
    }

    qi->qf_refcount--;
    ಆದರೆ (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_ಬಫರ್(qi);

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

    ಆದರೆ (wp != ಖಾಲಿ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi != ಖಾಲಿ)
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
    ಆದರೆ (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	ಆಗ (qf_delq_ತಲೆ != ಖಾಲಿ)
	{
	    qf_delq_T	*q = qf_delq_ತಲೆ;

	    qf_delq_ತಲೆ = q->ಮುಂದಿನ;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_ಆಂತರಿಕ_ERROR
    ಆದರೆ (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
check_quickfix_busy(ಶೂನ್ಯ)
{
    ಆದರೆ (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (ದೀರ್ಘ)quickfix_busy);
#  ifdef ABORT_ON_ಆಂತರಿಕ_ERROR
	abort();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಕೂಡಿಸು_ನಮೂದು(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    ಪೂರ್ಣಾಂಕ		bufnum,		// buffer number or zero
    char_u	*mesg,		//ಸಂದೇಶ
    ದೀರ್ಘ	lnum,		// line number
    ದೀರ್ಘ	ಕೊನೆ_lnum,	// line number for end
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,		//ಸ್ತಂಭ
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಲಂಬಸಾಲು,	// column for end
    ಪೂರ್ಣಾಂಕ		vis_ಲಂಬಸಾಲು,	// using visual column
    char_u	*ನಮೂನೆ,	// search pattern
    ಪೂರ್ಣಾಂಕ		nr,		// error number
    ಪೂರ್ಣಾಂಕ		type,		// type character
    typval_T	*user_ದತ್ತಾಂಶ,     // custom user data or NULL
    ಪೂರ್ಣಾಂಕ		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ಖಾಲಿ;
    char_u	*p = ಖಾಲಿ;

    ಆದರೆ ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ QF_FAIL;
    ಆದರೆ (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	ಆದರೆ (buf != ಖಾಲಿ)
	    buf->b_has_qf_ನಮೂದು |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ನಮೂದು : BUF_HAS_LL_ನಮೂದು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    ಆದರೆ (fname != ಖಾಲಿ)
	fullname = fix_fname(fname);
    qfp->qf_fname = ಖಾಲಿ;
    ಆದರೆ (buf != ಖಾಲಿ &&
	buf->b_ffname != ಖಾಲಿ && fullname != ಖಾಲಿ)
    {
	ಆದರೆ (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    ಆದರೆ (p != ಖಾಲಿ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    ಆದರೆ ((qfp->qf_ಪಠ್ಯ = vim_strsave(mesg)) == ಖಾಲಿ)
    {
	vim_free(qfp);
	ಹಿಂತಿರುಗಿ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_ಕೊನೆ_lnum = ಕೊನೆ_lnum;
    qfp->qf_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು = ಕೊನೆ_ಲಂಬಸಾಲು;
    qfp->qf_viscol = vis_ಲಂಬಸಾಲು;
    ಆದರೆ (user_ದತ್ತಾಂಶ == ಖಾಲಿ || user_ದತ್ತಾಂಶ->v_type == VAR_UNKNOWN)
	qfp->qf_user_ದತ್ತಾಂಶ.v_type = VAR_UNKNOWN;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	copy_tv(user_ದತ್ತಾಂಶ, &qfp->qf_user_ದತ್ತಾಂಶ);
	qfl->qf_has_user_ದತ್ತಾಂಶ = TRUE;
    }
    ಆದರೆ (ನಮೂನೆ == ಖಾಲಿ || *ನಮೂನೆ == NUL)
	qfp->qf_ನಮೂನೆ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((qfp->qf_ನಮೂನೆ = vim_strsave(ನಮೂನೆ)) == ಖಾಲಿ)
    {
	vim_free(qfp->qf_ಪಠ್ಯ);
	vim_free(qfp);
	ಹಿಂತಿರುಗಿ QF_FAIL;
    }
    ಆದರೆ (module == ಖಾಲಿ || *module == NUL)
	qfp->qf_module = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((qfp->qf_module = vim_strsave(module)) == ಖಾಲಿ)
    {
	vim_free(qfp->qf_ಪಠ್ಯ);
	vim_free(qfp->qf_ನಮೂನೆ);
	vim_free(qfp);
	ಹಿಂತಿರುಗಿ QF_FAIL;
    }
    qfp->qf_nr = nr;
    ಆದರೆ (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_ಕೊನೆಯ;
    ಆದರೆ (qf_list_ಖಾಲಿಗಳು(qfl))		// first element in the list
    {
	qfl->qf_ಆರಂಭ = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_ಸೂಚ್ಯಂಕ = 0;
	qfp->qf_prev = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_ಮುಂದಿನ = qfp;
    }
    qfp->qf_ಮುಂದಿನ = ಖಾಲಿ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_ಎಣಿಕೆ;
    ಆದರೆ (qfl->qf_ಸೂಚ್ಯಂಕ == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_ಸೂಚ್ಯಂಕ = qfl->qf_ಎಣಿಕೆ;
	qfl->qf_ptr = qfp;
    }

    ಹಿಂತಿರುಗಿ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಂಕ
qf_resize_ರಾಶಿ(ಪೂರ್ಣಾಂಕ n)
{
    ಆದರೆ (ql_info == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (qf_resize_ರಾಶಿ_ತಳ(ql_info, n) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    ಪೂರ್ಣಾಂಕ
ll_resize_ರಾಶಿ(win_T *wp, ಪೂರ್ಣಾಂಕ n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    ಆದರೆ (IS_LL_ಕಿಟಕಿ(wp))
	qf_sync_llw_ಗಮ್ಯ_win(wp);
    ಇಲ್ಲದಿದ್ದರೆ
	qf_sync_win_ಗಮ್ಯ_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (qf_resize_ರಾಶಿ_ತಳ(qi, n) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_resize_ರಾಶಿ_ತಳ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ n)
{
    qf_list_T *new;
    ಪೂರ್ಣಾಂಕ amount_ಗಮ್ಯ_rm = 0, i;

    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    size_t lsz = ಗಾತ್ರ(*qi->qf_lists);

    ಆದರೆ (n == qi->qf_maxcount)
	ಹಿಂತಿರುಗಿ OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_ಗಮ್ಯ_rm = qi->qf_listcount - n;

	ಗಾಗಿ (i = 0; i < amount_ಗಮ್ಯ_rm; i++)
	    qf_ತೆಗೆ_ರಾಶಿ(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    ಆದರೆ (new == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // fill with zeroes any newly allocated memory
    ಆದರೆ (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_ಬಫರ್(qi, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ OK;
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
qf_sync_llw_ಗಮ್ಯ_win(win_T *llw)
{
    win_T *wp = qf_ಕಂಡುಹಿಡಿ_win_with_loclist(llw->w_llist_ref);

    ಆದರೆ (wp != ಖಾಲಿ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_sync_win_ಗಮ್ಯ_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    ಆದರೆ (llw != ಖಾಲಿ)
	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_llist_ref == llw && bt_quickfix(wp->w_ಬಫರ್))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		ಹಿಂತಿರುಗಿ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    ಸ್ಥಿರ qf_info_T *
qf_alloc_ರಾಶಿ(qfltype_T qfltype, ಪೂರ್ಣಾಂಕ n)
{
    qf_info_T *qi;

    ಆದರೆ (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	ಆದರೆ (qi == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_ರಾಶಿ(n);
    ಆದರೆ (qi->qf_lists == ಖಾಲಿ)
    {
	ಆದರೆ (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    qi->qf_maxcount = n;

    ಹಿಂತಿರುಗಿ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    ಸ್ಥಿರ qf_list_T *
qf_alloc_list_ರಾಶಿ(ಪೂರ್ಣಾಂಕ n)
{
    ಹಿಂತಿರುಗಿ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    ಸ್ಥಿರ qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    ಆದರೆ (IS_LL_ಕಿಟಕಿ(wp))
	// For a location list window, use the referenced location list
	ಹಿಂತಿರುಗಿ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    ಆದರೆ (wp->w_llist == ಖಾಲಿ)
	// new location list
	wp->w_llist = qf_alloc_ರಾಶಿ(QFLT_LOCATION, wp->w_p_lhi);

    ಹಿಂತಿರುಗಿ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    ಸ್ಥಿರ qf_info_T *
qf_cmd_get_ರಾಶಿ(exarg_T *eap, ಪೂರ್ಣಾಂಕ print_emsg)
{
    qf_info_T	*qi = ql_info;

    ಆದರೆ (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	ಆದರೆ (qi == ಖಾಲಿ)
	{
	    ಆದರೆ (print_emsg)
		emsg(_(e_no_location_list));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಆದರೆ (qi == ಖಾಲಿ && print_emsg)
	emsg(_(e_no_quickfix_ರಾಶಿ));

    ಹಿಂತಿರುಗಿ qi;
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

    ಆದರೆ (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	ಆದರೆ (qi == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*pwinp = curwin;
    }

    ಹಿಂತಿರುಗಿ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *ಗಮ್ಯ_qfl)
{
    ಪೂರ್ಣಾಂಕ		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ವಸ್ತುಗಳು(from_qfl, from_qfp, i)
    {
	ಆದರೆ (qf_ಕೂಡಿಸು_ನಮೂದು(ಗಮ್ಯ_qfl,
		    ಖಾಲಿ,
		    ಖಾಲಿ,
		    from_qfp->qf_module,
		    0,
		    from_qfp->qf_ಪಠ್ಯ,
		    from_qfp->qf_lnum,
		    from_qfp->qf_ಕೊನೆ_lnum,
		    from_qfp->qf_ಲಂಬಸಾಲು,
		    from_qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು,
		    from_qfp->qf_viscol,
		    from_qfp->qf_ನಮೂನೆ,
		    from_qfp->qf_nr,
		    0,
		    &from_qfp->qf_user_ದತ್ತಾಂಶ,
		    from_qfp->qf_valid) == QF_FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = ಗಮ್ಯ_qfl->qf_ಕೊನೆಯ;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	ಆದರೆ (from_qfl->qf_ptr == from_qfp)
	    ಗಮ್ಯ_qfl->qf_ptr = prevp;		// current location
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
copy_loclist(qf_list_T *from_qfl, qf_list_T *ಗಮ್ಯ_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    ಗಮ್ಯ_qfl->qfl_type = from_qfl->qfl_type;
    ಗಮ್ಯ_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    ಗಮ್ಯ_qfl->qf_has_user_ದತ್ತಾಂಶ = from_qfl->qf_has_user_ದತ್ತಾಂಶ;
    ಗಮ್ಯ_qfl->qf_ಎಣಿಕೆ = 0;
    ಗಮ್ಯ_qfl->qf_ಸೂಚ್ಯಂಕ = 0;
    ಗಮ್ಯ_qfl->qf_ಆರಂಭ = ಖಾಲಿ;
    ಗಮ್ಯ_qfl->qf_ಕೊನೆಯ = ಖಾಲಿ;
    ಗಮ್ಯ_qfl->qf_ptr = ಖಾಲಿ;
    ಆದರೆ (from_qfl->qf_title != ಖಾಲಿ)
	ಗಮ್ಯ_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಮ್ಯ_qfl->qf_title = ಖಾಲಿ;
    ಆದರೆ (from_qfl->qf_ctx != ಖಾಲಿ)
    {
	ಗಮ್ಯ_qfl->qf_ctx = alloc_tv();
	ಆದರೆ (ಗಮ್ಯ_qfl->qf_ctx != ಖಾಲಿ)
	    copy_tv(from_qfl->qf_ctx, ಗಮ್ಯ_qfl->qf_ctx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಮ್ಯ_qfl->qf_ctx = ಖಾಲಿ;
    ಆದರೆ (from_qfl->qf_qftf_cb.cb_name != ಖಾಲಿ)
	copy_callback(&ಗಮ್ಯ_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಮ್ಯ_qfl->qf_qftf_cb.cb_name = ಖಾಲಿ;

    ಆದರೆ (from_qfl->qf_ಎಣಿಕೆ)
	ಆದರೆ (copy_loclist_entries(from_qfl, ಗಮ್ಯ_qfl) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

    ಗಮ್ಯ_qfl->qf_ಸೂಚ್ಯಂಕ = from_qfl->qf_ಸೂಚ್ಯಂಕ;	// current index in the list

    // Assign a new ID for the location list
    ಗಮ್ಯ_qfl->qf_id = ++ಕೊನೆಯ_qf_id;
    ಗಮ್ಯ_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    ಆದರೆ (ಗಮ್ಯ_qfl->qf_nonevalid)
    {
	ಗಮ್ಯ_qfl->qf_ptr = ಗಮ್ಯ_qfl->qf_ಆರಂಭ;
	ಗಮ್ಯ_qfl->qf_ಸೂಚ್ಯಂಕ = 1;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    ಶೂನ್ಯ
copy_loclist_ರಾಶಿ(win_T *from, win_T *ಗಮ್ಯ)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಂಕ		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    ಆದರೆ (IS_LL_ಕಿಟಕಿ(from))
	qi = from->w_llist_ref;
    ಇಲ್ಲದಿದ್ದರೆ
	qi = from->w_llist;

    ಆದರೆ (qi == ಖಾಲಿ)		    // no location list to copy
	ಹಿಂತಿರುಗಿ;

    // allocate a new location list, set size of stack to 'from' window value
    ಆದರೆ ((ಗಮ್ಯ->w_llist = qf_alloc_ರಾಶಿ(QFLT_LOCATION, from->w_p_lhi)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	// set 'to' lhi to reflect new value
	ಗಮ್ಯ->w_p_lhi = ಗಮ್ಯ->w_llist->qf_maxcount;

    ಗಮ್ಯ->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    ಗಾಗಿ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	ಗಮ್ಯ->w_llist->qf_curlist = idx;

	ಆದರೆ (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(ಗಮ್ಯ->w_llist, idx)) == FAIL)
	{
	    qf_free_all(ಗಮ್ಯ);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಗಮ್ಯ->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ಖಾಲಿ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    ಆದರೆ (fname == ಖಾಲಿ || *fname == NUL)		// no file name
	ಹಿಂತಿರುಗಿ 0;

# ifdef VMS
    vms_remove_ಆವೃತ್ತಿ(fname);
# endif
# ifdef BACKSLASH_ಒಳ_FILENAME
    ಆದರೆ (directory != ಖಾಲಿ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    ಆದರೆ (directory != ಖಾಲಿ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ಖಾಲಿ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	ಆದರೆ (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    ಆದರೆ (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ptr.ಉದ್ದಳತೆ = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    ಆದರೆ (ptr.string == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.ಉದ್ದಳತೆ = ptr.ಉದ್ದಳತೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	bufname.string = fname;
	bufname.ಉದ್ದಳತೆ = fname_len;
    }

    ಆದರೆ (qf_ಕೊನೆಯ_bufname != ಖಾಲಿ && STRCMP(bufname.string, qf_ಕೊನೆಯ_bufname) == 0
	    && bufref_valid(&qf_ಕೊನೆಯ_bufref))
    {
	buf = qf_ಕೊನೆಯ_bufref.br_buf;
	vim_free(ptr.string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(qf_ಕೊನೆಯ_bufname);
	buf = buflist_new(bufname.string, ಖಾಲಿ, (linenr_T)0, BLN_NOOPT);
	ಆದರೆ (bufname.string == ptr.string)
	    qf_ಕೊನೆಯ_bufname = bufname.string;
	ಇಲ್ಲದಿದ್ದರೆ
	    qf_ಕೊನೆಯ_bufname = vim_strnsave(bufname.string, bufname.ಉದ್ದಳತೆ);
	ಗಣ_bufref(&qf_ಕೊನೆಯ_bufref, buf);
    }
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    buf->b_has_qf_ನಮೂದು =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ನಮೂದು : BUF_HAS_LL_ನಮೂದು;
    ಹಿಂತಿರುಗಿ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    ಸ್ಥಿರ char_u *
qf_ತಳ್ಳು_dir(char_u *dirbuf, ರಚನೆ dir_ರಾಶಿ_T **stackptr, ಪೂರ್ಣಾಂಕ is_file_ರಾಶಿ)
{
    ರಚನೆ dir_ರಾಶಿ_T  *ds_new;
    ರಚನೆ dir_ರಾಶಿ_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ರಚನೆ dir_ರಾಶಿ_T, aid_qf_dirstack);
    ಆದರೆ (ds_new == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // push the new element onto the stack
    ds_new->ಮುಂದಿನ = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    ಆದರೆ (vim_isAbsName(dirbuf)
	    || (*stackptr)->ಮುಂದಿನ == ಖಾಲಿ
	    || is_file_ರಾಶಿ)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->ಮುಂದಿನ;
	(*stackptr)->dirname = ಖಾಲಿ;
	ಆಗ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    ಆದರೆ (dirname.string == ಖಾಲಿ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->ಮುಂದಿನ;
		vim_free(ds_ptr);

		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ಆದರೆ (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		ನಿಲ್ಲಿಸು;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->ಮುಂದಿನ;
	}

	// clean up all dirs we already left
	ಆಗ ((*stackptr)->ಮುಂದಿನ != ds_new)
	{
	    ds_ptr = (*stackptr)->ಮುಂದಿನ;
	    (*stackptr)->ಮುಂದಿನ = (*stackptr)->ಮುಂದಿನ->ಮುಂದಿನ;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	ಆದರೆ (ds_new == ಖಾಲಿ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    ಆದರೆ ((*stackptr)->dirname != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->ಮುಂದಿನ;
    vim_free(ds_ptr);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    ಸ್ಥಿರ char_u *
qf_ತೆಗೆ_dir(ರಚನೆ dir_ರಾಶಿ_T **stackptr)
{
    ರಚನೆ dir_ರಾಶಿ_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    ಆದರೆ (*stackptr != ಖಾಲಿ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->ಮುಂದಿನ;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    ಹಿಂತಿರುಗಿ *stackptr ? (*stackptr)->dirname : ಖಾಲಿ;
}

/*
 * clean up directory stack
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_clean_dir_ರಾಶಿ(ರಚನೆ dir_ರಾಶಿ_T **stackptr)
{
    ರಚನೆ dir_ರಾಶಿ_T  *ds_ptr;

    ಆಗ ((ds_ptr = *stackptr) != ಖಾಲಿ)
    {
	*stackptr = (*stackptr)->ಮುಂದಿನ;
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
    ರಚನೆ dir_ರಾಶಿ_T     *ds_ptr;
    ರಚನೆ dir_ರಾಶಿ_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    ಆದರೆ (qfl->qf_dir_ರಾಶಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ds_ptr = qfl->qf_dir_ರಾಶಿ->ಮುಂದಿನ;
    fullname.string = ಖಾಲಿ;
    fullname.ಉದ್ದಳತೆ = 0;
    filename_len = STRLEN(filename);
    ಆಗ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	ಆದರೆ ((fullname.string != ಖಾಲಿ) && (mch_getperm(fullname.string) >= 0))
	    ನಿಲ್ಲಿಸು;

	ds_ptr = ds_ptr->ಮುಂದಿನ;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    ಆಗ (qfl->qf_dir_ರಾಶಿ->ಮುಂದಿನ != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_ರಾಶಿ->ಮುಂದಿನ;
	qfl->qf_dir_ರಾಶಿ->ಮುಂದಿನ = qfl->qf_dir_ರಾಶಿ->ಮುಂದಿನ->ಮುಂದಿನ;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    ಹಿಂತಿರುಗಿ ds_ptr == ಖಾಲಿ ? ಖಾಲಿ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (wp != ಖಾಲಿ)
    {
	ಆದರೆ (!win_valid(wp))
	    ಹಿಂತಿರುಗಿ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; i < qi->qf_listcount; ++i)
	ಆದರೆ (qi->qf_lists[i].qf_id == qf_id)
	    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_qf_ನಮೂದು_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
	ಆದರೆ (qfp == qf_ptr)
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (i > qfl->qf_ಎಣಿಕೆ) // Entry is not found
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    ಸ್ಥಿರ qfline_T *
get_ಮುಂದಿನ_valid_ನಮೂದು(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಂಕ		*qf_ಸೂಚ್ಯಂಕ,
	ಪೂರ್ಣಾಂಕ		dir)
{
    ಪೂರ್ಣಾಂಕ			idx;
    ಪೂರ್ಣಾಂಕ			old_qf_fnum;

    idx = *qf_ಸೂಚ್ಯಂಕ;
    old_qf_fnum = qf_ptr->qf_fnum;

    ಮಾಡು
    {
	ಆದರೆ (idx == qfl->qf_ಎಣಿಕೆ || qf_ptr->qf_ಮುಂದಿನ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	++idx;
	qf_ptr = qf_ptr->qf_ಮುಂದಿನ;
    } ಆಗ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_ಸೂಚ್ಯಂಕ = idx;
    ಹಿಂತಿರುಗಿ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    ಸ್ಥಿರ qfline_T *
get_prev_valid_ನಮೂದು(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಂಕ		*qf_ಸೂಚ್ಯಂಕ,
	ಪೂರ್ಣಾಂಕ		dir)
{
    ಪೂರ್ಣಾಂಕ			idx;
    ಪೂರ್ಣಾಂಕ			old_qf_fnum;

    idx = *qf_ಸೂಚ್ಯಂಕ;
    old_qf_fnum = qf_ptr->qf_fnum;

    ಮಾಡು
    {
	ಆದರೆ (idx == 1 || qf_ptr->qf_prev == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } ಆಗ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_ಸೂಚ್ಯಂಕ = idx;
    ಹಿಂತಿರುಗಿ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    ಸ್ಥಿರ qfline_T *
get_nth_valid_ನಮೂದು(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಂಕ		errornr,
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಂಕ			qf_idx = qfl->qf_ಸೂಚ್ಯಂಕ;
    qfline_T		*prev_qf_ptr;
    ಪೂರ್ಣಾಂಕ			prev_ಸೂಚ್ಯಂಕ;
    ಅಕ್ಷರ		*err = e_no_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು;

    ಆಗ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_ಸೂಚ್ಯಂಕ = qf_idx;

	ಆದರೆ (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_ಮುಂದಿನ_valid_ನಮೂದು(qfl, qf_ptr, &qf_idx, dir);
	ಇಲ್ಲದಿದ್ದರೆ
	    qf_ptr = get_prev_valid_ನಮೂದು(qfl, qf_ptr, &qf_idx, dir);
	ಆದರೆ (qf_ptr == ಖಾಲಿ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_ಸೂಚ್ಯಂಕ;
	    ಆದರೆ (err != ಖಾಲಿ)
	    {
		emsg(_(err));
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}

	err = ಖಾಲಿ;
    }

    *new_qfidx = qf_idx;
    ಹಿಂತಿರುಗಿ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    ಸ್ಥಿರ qfline_T *
get_nth_ನಮೂದು(qf_list_T *qfl, ಪೂರ್ಣಾಂಕ errornr, ಪೂರ್ಣಾಂಕ *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಂಕ		qf_idx = qfl->qf_ಸೂಚ್ಯಂಕ;

    // New error number is less than the current error number
    ಆಗ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ಖಾಲಿ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    ಆಗ (errornr > qf_idx && qf_idx < qfl->qf_ಎಣಿಕೆ &&
						qf_ptr->qf_ಮುಂದಿನ != ಖಾಲಿ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_ಮುಂದಿನ;
    }

    *new_qfidx = qf_idx;
    ಹಿಂತಿರುಗಿ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    ಸ್ಥಿರ qfline_T *
qf_get_ನಮೂದು(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಂಕ		errornr,
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    ಪೂರ್ಣಾಂಕ		qfidx = qfl->qf_ಸೂಚ್ಯಂಕ;

    ಆದರೆ (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_ನಮೂದು(qfl, errornr, dir, &qfidx);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errornr != 0)	// go to specified number
	qf_ptr = get_nth_ನಮೂದು(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    ಹಿಂತಿರುಗಿ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    ಸ್ಥಿರ win_T *
qf_ಕಂಡುಹಿಡಿ_help_win(ಶೂನ್ಯ)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (bt_help(wp->w_ಬಫರ್))
	    ಹಿಂತಿರುಗಿ wp;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
win_ಗಣ_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
jump_ಗಮ್ಯ_help_ಕಿಟಕಿ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ newwin, ಪೂರ್ಣಾಂಕ *opened_ಕಿಟಕಿ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		flags;

    ಆದರೆ (cmdmod.cmod_tab != 0 || newwin)
	wp = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	wp = qf_ಕಂಡುಹಿಡಿ_help_win();
    ಆದರೆ (wp != ಖಾಲಿ && wp->w_ಬಫರ್->b_nwindows > 0)
	win_enter(wp, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	ಆದರೆ (cmdmod.cmod_ವಿಭಜಿಸು == 0 && curwin->w_ಅಗಲ != Columns
		&& curwin->w_ಅಗಲ < 80)
	    flags |= WSP_ತುದಿ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	ಆದರೆ (IS_LL_ರಾಶಿ(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	ಆದರೆ (win_ವಿಭಜಿಸು(0, flags) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	*opened_ಕಿಟಕಿ = TRUE;

	ಆದರೆ (curwin->w_ಎತ್ತರ < p_hh)
	    win_setheight((ಪೂರ್ಣಾಂಕ)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	ಆದರೆ (IS_LL_ರಾಶಿ(qi) && !newwin)
	    win_ಗಣ_loclist(curwin, qi);
    }

    ಆದರೆ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    ಸ್ಥಿರ win_T *
qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_llist == ll && !bt_quickfix(wp->w_ಬಫರ್))
	    ಹಿಂತಿರುಗಿ wp;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    ಸ್ಥಿರ win_T *
qf_ಕಂಡುಹಿಡಿ_win_with_ಸಾಮಾನ್ಯ_buf(ಶೂನ್ಯ)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (bt_ಸಾಮಾನ್ಯ(wp->w_ಬಫರ್))
	    ಹಿಂತಿರುಗಿ wp;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_goto_tabwin_with_file(ಪೂರ್ಣಾಂಕ fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_ಬಫರ್->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    ಹಿಂತಿರುಗಿ TRUE;
	}

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_open_new_file_win(qf_info_T *ll_ref)
{
    ಪೂರ್ಣಾಂಕ		flags;

    flags = WSP_ABOVE;
    ಆದರೆ (ll_ref != ಖಾಲಿ)
	flags |= WSP_NEWLOC;
    ಆದರೆ (win_ವಿಭಜಿಸು(0, flags) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;		// not enough room for window
    p_swb = ಖಾಲಿಗಳು_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    ಆದರೆ (ll_ref != ಖಾಲಿ)
	// The new window should use the location list from the
	// location list window
	win_ಗಣ_loclist(curwin, ll_ref);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_goto_win_with_ll_file(win_T *use_win, ಪೂರ್ಣಾಂಕ qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    ಆದರೆ (win == ಖಾಲಿ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    ಆದರೆ (win->w_ಬಫರ್->b_fnum == qf_fnum)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (win == ಖಾಲಿ)
	{
	    // Find a previous usable window
	    win = curwin;
	    ಮಾಡು
	    {
		ಆದರೆ (bt_ಸಾಮಾನ್ಯ(win->w_ಬಫರ್))
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (win->w_prev == ಖಾಲಿ)
		    win = lastwin;	// wrap around the top
		ಇಲ್ಲದಿದ್ದರೆ
		    win = win->w_prev; // go to previous window
	    } ಆಗ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    ಆದರೆ (win->w_llist == ಖಾಲಿ && ll_ref != ಖಾಲಿ)
	win_ಗಣ_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_goto_win_with_qfl_file(ಪೂರ್ಣಾಂಕ qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ಖಾಲಿ;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (win->w_ಬಫರ್->b_fnum == qf_fnum)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (win->w_prev == ಖಾಲಿ)
	    win = lastwin;	// wrap around the top
	ಇಲ್ಲದಿದ್ದರೆ
	    win = win->w_prev;	// go to previous window

	ಆದರೆ (IS_QF_ಕಿಟಕಿ(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    ಆದರೆ ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (altwin != ಖಾಲಿ)
		win = altwin;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_prev != ಖಾಲಿ)
		win = curwin->w_prev;
	    ಇಲ್ಲದಿದ್ದರೆ
		win = curwin->w_ಮುಂದಿನ;
	    ನಿಲ್ಲಿಸು;
	}

	// Remember a usable window.
	ಆದರೆ (altwin == ಖಾಲಿ && !win->w_p_pvw && !win->w_p_wfb &&
		bt_ಸಾಮಾನ್ಯ(win->w_ಬಫರ್))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_jump_ಗಮ್ಯ_usable_ಕಿಟಕಿ(ಪೂರ್ಣಾಂಕ qf_fnum, ಪೂರ್ಣಾಂಕ newwin, ಪೂರ್ಣಾಂಕ *opened_ಕಿಟಕಿ)
{
    win_T	*usable_wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		usable_win = FALSE;
    qf_info_T	*ll_ref = ಖಾಲಿ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    ಆದರೆ (!newwin)
	ll_ref = curwin->w_llist_ref;

    ಆದರೆ (ll_ref != ಖಾಲಿ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_ಕಂಡುಹಿಡಿ_win_with_loclist(ll_ref);
	ಆದರೆ (usable_wp != ಖಾಲಿ)
	    usable_win = TRUE;
    }

    ಆದರೆ (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_ಕಂಡುಹಿಡಿ_win_with_ಸಾಮಾನ್ಯ_buf();
	ಆದರೆ (win != ಖಾಲಿ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    ಆದರೆ (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    ಆದರೆ ((ONE_ಕಿಟಕಿ && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	ಆದರೆ (qf_open_new_file_win(ll_ref) != OK)
	    ಹಿಂತಿರುಗಿ FAIL;
	*opened_ಕಿಟಕಿ = TRUE;	// close it when fail
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (curwin->w_llist_ref != ಖಾಲಿ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	ಇಲ್ಲದಿದ್ದರೆ					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_jump_edit_ಬಫರ್(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಂಕ		forceit,
	ಪೂರ್ಣಾಂಕ		prev_winid,
	ಪೂರ್ಣಾಂಕ		*opened_ಕಿಟಕಿ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ಪೂರ್ಣಾಂಕ		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಪೂರ್ಣಾಂಕ		old_qf_curlist = qi->qf_curlist;
    ಪೂರ್ಣಾಂಕ		save_qfid = qfl->qf_id;

    ಆದರೆ (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	ಆದರೆ (!can_abandon(curbuf, forceit))
	{
	    no_write_ಸಂದೇಶ();
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, (linenr_T)1,
		ECMD_HIDE + ECMD_ಗಣ_HELP,
		prev_winid == curwin->w_id ? curwin : ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	fnum = qf_ptr->qf_fnum;

	ಆದರೆ (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    ಆದರೆ (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_ಬಫರ್))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    ಆದರೆ (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		ಆದರೆ (win_ವಿಭಜಿಸು(0, 0) == OK)
		    *opened_ಕಿಟಕಿ = TRUE;

		ಆದರೆ (curwin->w_p_wfb)
		{
		    // Autocommands set 'winfixbuf' or sent us to another window
		    // with it set, or we failed to split the window.  Give up,
		    // but don't return immediately, as they may have messed
		    // with the list.
		    emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
		    retval = FAIL;
		}
	    }
	}

	ಆದರೆ (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    ಆದರೆ (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	ಆದರೆ (wp == ಖಾಲಿ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_ಕಿಟಕಿ_was_closed));
	    *opened_ಕಿಟಕಿ = FALSE;
	    ಹಿಂತಿರುಗಿ QF_ABORT;
	}
    }

    ಆದರೆ (qfl_type == QFLT_QUICKFIX && !qflist_valid(ಖಾಲಿ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_ಬದಲಾಯಿತು));
	ಹಿಂತಿರುಗಿ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    ಆದರೆ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ನಮೂದು_present(qfl, qf_ptr))
    {
	ಆದರೆ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ಬದಲಾಯಿತು));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_current_location_list_was_ಬದಲಾಯಿತು));
	ಹಿಂತಿರುಗಿ QF_ABORT;
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_screen_ಲಂಬಸಾಲು_ಗಮ್ಯ_idx(colnr_T vcol)
{
    char_u	*ಸಾಲು = ml_get_curline();
    char_u	*p = ಸಾಲು;
    colnr_T	ಲಂಬಸಾಲು = 0;

    ಆಗ (*p != NUL && ಲಂಬಸಾಲು < vcol)
    {
	ಆದರೆ (*p == TAB)
	    ಲಂಬಸಾಲು += 8 - (ಲಂಬಸಾಲು % 8);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಲಂಬಸಾಲು += ptr2cells(p);
	ಆದರೆ (ಲಂಬಸಾಲು > vcol)
	    ನಿಲ್ಲಿಸು;
	MB_PTR_ADV(p);
    }
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_goto_ಸಾಲು(
	linenr_T	qf_lnum,
	ಪೂರ್ಣಾಂಕ		qf_ಲಂಬಸಾಲು,
	char_u		qf_viscol,
	char_u		*qf_ನಮೂನೆ)
{
    linenr_T		i;

    ಆದರೆ (qf_ನಮೂನೆ == ಖಾಲಿ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	ಆದರೆ (i > 0)
	{
	    ಆದರೆ (i > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		i = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    curwin->w_cursor.lnum = i;
	}
	ಆದರೆ (qf_ಲಂಬಸಾಲು > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    ಆದರೆ (qf_viscol == TRUE)
		curwin->w_cursor.ಲಂಬಸಾಲು = qf_screen_ಲಂಬಸಾಲು_ಗಮ್ಯ_idx(qf_ಲಂಬಸಾಲು - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.ಲಂಬಸಾಲು = qf_ಲಂಬಸಾಲು - 1;
	    curwin->w_ಗಣ_curswant = true;
	    check_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    beginline(BL_WHITE | BL_FIX);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	ಆದರೆ (!do_ಹುಡುಕು(ಖಾಲಿ, '/', '/', qf_ನಮೂನೆ, STRLEN(qf_ನಮೂನೆ), (ದೀರ್ಘ)1, ಹುಡುಕು_KEEP, ಖಾಲಿ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_print_ಸಂದೇಶ(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಂಕ		qf_ಸೂಚ್ಯಂಕ,
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
    ಆದರೆ (!ಸಂದೇಶ_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_ಸೂಚ್ಯಂಕ,
	    qf_get_curlist(qi)->qf_ಎಣಿಕೆ,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (ಅಕ್ಷರ *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_ಪಠ್ಯ(gap, skipwhite(qf_ptr->qf_ಪಠ್ಯ));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = ಸಂದೇಶ_scroll;
    ಆದರೆ (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	ಸಂದೇಶ_scroll = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಸಂದೇಶ_scrolled && shortmess(SHM_OVERALL))
	ಸಂದೇಶ_scroll = FALSE;
    ಸಂದೇಶ_attr_keep((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ, 0, TRUE);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_jump_open_ಕಿಟಕಿ(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಂಕ		newwin,
	ಪೂರ್ಣಾಂಕ		*opened_ಕಿಟಕಿ)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    ಪೂರ್ಣಾಂಕ		old_changedtick = qfl->qf_changedtick;
    ಪೂರ್ಣಾಂಕ		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    ಆದರೆ (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_ಬಫರ್)
						      || cmdmod.cmod_tab != 0))
	ಆದರೆ (jump_ಗಮ್ಯ_help_ಕಿಟಕಿ(qi, newwin, opened_ಕಿಟಕಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ನಮೂದು_present(qfl, qf_ptr))
    {
	ಆದರೆ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ಬದಲಾಯಿತು));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_current_location_list_was_ಬದಲಾಯಿತು));
	ಹಿಂತಿರುಗಿ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    ಆದರೆ (bt_quickfix(curbuf) && !*opened_ಕಿಟಕಿ)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	ಆದರೆ (qf_ptr->qf_fnum == 0)
	    ಹಿಂತಿರುಗಿ NOTDONE;

	ಆದರೆ (qf_jump_ಗಮ್ಯ_usable_ಕಿಟಕಿ(qf_ptr->qf_fnum, newwin,
						opened_ಕಿಟಕಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_ನಮೂದು_present(qfl, qf_ptr))
    {
	ಆದರೆ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_ಬದಲಾಯಿತು));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_current_location_list_was_ಬದಲಾಯಿತು));
	ಹಿಂತಿರುಗಿ QF_ABORT;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_jump_ಗಮ್ಯ_ಬಫರ್(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಂಕ		qf_ಸೂಚ್ಯಂಕ,
	qfline_T	*qf_ptr,
	ಪೂರ್ಣಾಂಕ		forceit,
	ಪೂರ್ಣಾಂಕ		prev_winid,
	ಪೂರ್ಣಾಂಕ		*opened_ಕಿಟಕಿ,
	ಪೂರ್ಣಾಂಕ		openfold,
	ಪೂರ್ಣಾಂಕ		print_ಸಂದೇಶ)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    ಪೂರ್ಣಾಂಕ		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    ಆದರೆ (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_ಬಫರ್(qi, qf_ptr, forceit, prev_winid,
						opened_ಕಿಟಕಿ);
	ಆದರೆ (retval != OK)
	    ಹಿಂತಿರುಗಿ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    ಆದರೆ (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_ಸಾಲು(qf_ptr->qf_lnum, qf_ptr->qf_ಲಂಬಸಾಲು, qf_ptr->qf_viscol,
	    qf_ptr->qf_ನಮೂನೆ);

# ifdef FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    ಆದರೆ (print_ಸಂದೇಶ)
	qf_jump_print_ಸಂದೇಶ(qi, qf_ಸೂಚ್ಯಂಕ, qf_ptr, old_curbuf, old_lnum);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    ಶೂನ್ಯ
qf_jump(qf_info_T	*qi,
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		errornr,
	ಪೂರ್ಣಾಂಕ		forceit)
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
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		errornr,
	ಪೂರ್ಣಾಂಕ		forceit,
	ಪೂರ್ಣಾಂಕ		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    ಪೂರ್ಣಾಂಕ			qf_ಸೂಚ್ಯಂಕ;
    ಪೂರ್ಣಾಂಕ			old_qf_ಸೂಚ್ಯಂಕ;
    char_u		*old_swb = p_swb;
    ಚಿಹ್ನೆರಹಿತ		old_swb_flags = swb_flags;
    ಪೂರ್ಣಾಂಕ			prev_winid;
    ಪೂರ್ಣಾಂಕ			opened_ಕಿಟಕಿ = FALSE;
    ಪೂರ್ಣಾಂಕ			print_ಸಂದೇಶ = TRUE;
    ಪೂರ್ಣಾಂಕ			old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed; // getting file may reset it
    ಪೂರ್ಣಾಂಕ			retval = OK;

    ಆದರೆ (qi == ಖಾಲಿ)
    {
	ಆದರೆ (ql_info == ಖಾಲಿ)
	{
	    emsg(_(e_no_quickfix_ರಾಶಿ));
	    ಹಿಂತಿರುಗಿ;
	}
	qi = ql_info;
    }

    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi) || qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಹಿಂತಿರುಗಿ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_ಸೂಚ್ಯಂಕ = qfl->qf_ಸೂಚ್ಯಂಕ;
    old_qf_ಸೂಚ್ಯಂಕ = qf_ಸೂಚ್ಯಂಕ;

    qf_ptr = qf_get_ನಮೂದು(qfl, errornr, dir, &qf_ಸೂಚ್ಯಂಕ);
    ಆದರೆ (qf_ptr == ಖಾಲಿ)
    {
	qf_ptr = old_qf_ptr;
	qf_ಸೂಚ್ಯಂಕ = old_qf_ಸೂಚ್ಯಂಕ;
	ಹೋಗು theend;
    }

    qfl->qf_ಸೂಚ್ಯಂಕ = qf_ಸೂಚ್ಯಂಕ;
    qfl->qf_ptr = qf_ptr;
    ಆದರೆ (qf_win_pos_update(qi, old_qf_ಸೂಚ್ಯಂಕ))
	// No need to print the error message if it's visible in the error
	//ಕಿಟಕಿ
	print_ಸಂದೇಶ = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_ಕಿಟಕಿ(qi, qf_ptr, newwin, &opened_ಕಿಟಕಿ);
    ಆದರೆ (retval == FAIL)
	ಹೋಗು failed;
    ಆದರೆ (retval == QF_ABORT)
    {
	qi = ಖಾಲಿ;
	qf_ptr = ಖಾಲಿ;
	ಹೋಗು theend;
    }
    ಆದರೆ (retval == NOTDONE)
	ಹೋಗು theend;

    retval = qf_jump_ಗಮ್ಯ_ಬಫರ್(qi, qf_ಸೂಚ್ಯಂಕ, qf_ptr, forceit, prev_winid,
				  &opened_ಕಿಟಕಿ, old_ಕೀಲಿ_Typed, print_ಸಂದೇಶ);
    ಆದರೆ (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ಖಾಲಿ;
	qf_ptr = ಖಾಲಿ;
    }

    ಆದರೆ (retval != OK)
    {
	ಆದರೆ (opened_ಕಿಟಕಿ)
	    win_close(curwin, TRUE);    // Close opened window
	ಆದರೆ (qf_ptr != ಖಾಲಿ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_ಸೂಚ್ಯಂಕ = old_qf_ಸೂಚ್ಯಂಕ;
	}
    }
theend:
    ಆದರೆ (qi != ಖಾಲಿ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_ಸೂಚ್ಯಂಕ = qf_ಸೂಚ್ಯಂಕ;
    }
    ಆದರೆ (p_swb != old_swb && p_swb == ಖಾಲಿಗಳು_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qfFileAttr;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qfSepAttr;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	qf_ಸಾಲು_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_list_ನಮೂದು(qfline_T *qfp, ಪೂರ್ಣಾಂಕ qf_idx, ಪೂರ್ಣಾಂಕ cursel)
{
    char_u	*fname;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		filter_ನಮೂದು;
    garray_T	*gap;

    fname = ಖಾಲಿ;
    ಆದರೆ (qfp->qf_module != ಖಾಲಿ && *qfp->qf_module != NUL)
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(ಅಕ್ಷರ *)qfp->qf_module);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ಖಾಲಿ)
	{
	    ಆದರೆ (qfp->qf_fname == ಖಾಲಿ)
		fname = buf->b_fname;
	    ಇಲ್ಲದಿದ್ದರೆ
		fname = qfp->qf_fname;
	    ಆದರೆ (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	ಆದರೆ (fname == ಖಾಲಿ)
	    sprintf((ಅಕ್ಷರ *)IObuff, "%2d", qf_idx);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (ಅಕ್ಷರ *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_ನಮೂದು = TRUE;
    ಆದರೆ (qfp->qf_module != ಖಾಲಿ && *qfp->qf_module != NUL)
	filter_ನಮೂದು &= ಸಂದೇಶ_filtered(qfp->qf_module);
    ಆದರೆ (filter_ನಮೂದು && fname != ಖಾಲಿ)
	filter_ನಮೂದು &= ಸಂದೇಶ_filtered(fname);
    ಆದರೆ (filter_ನಮೂದು && qfp->qf_ನಮೂನೆ != ಖಾಲಿ)
	filter_ನಮೂದು &= ಸಂದೇಶ_filtered(qfp->qf_ನಮೂನೆ);
    ಆದರೆ (filter_ನಮೂದು)
	filter_ನಮೂದು &= ಸಂದೇಶ_filtered(qfp->qf_ಪಠ್ಯ);
    ಆದರೆ (filter_ನಮೂದು)
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ_putchar('\n');
    ಸಂದೇಶ_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    ಆದರೆ (qfp->qf_lnum != 0)
	ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    ಆದರೆ (qfp->qf_lnum != 0)
	qf_range_ಪಠ್ಯ(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ, qf_ಸಾಲು_Attr);
    ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    ಆದರೆ (qfp->qf_ನಮೂನೆ != ಖಾಲಿ)
    {
	gap = qfga_get();
	qf_fmt_ಪಠ್ಯ(gap, qfp->qf_ನಮೂನೆ);
	ga_append(gap, NUL);
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ);
	ಸಂದೇಶ_puts_attr(":", qfSepAttr);
    }
    ಸಂದೇಶ_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_ಪಠ್ಯ(gap, (fname != ಖಾಲಿ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_ಪಠ್ಯ) : qfp->qf_ಪಠ್ಯ);
    ga_append(gap, NUL);
    ಸಂದೇಶ_prt_ಸಾಲು((char_u *)gap->ga_ದತ್ತಾಂಶ, FALSE);
    ಹೊರಹೋಗು_flush();		// show one line at a time
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
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		idx1 = 1;
    ಪೂರ್ಣಾಂಕ		idx2 = -1;
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		plus = FALSE;
    ಪೂರ್ಣಾಂಕ		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi) || qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    ಆದರೆ (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	ಹಿಂತಿರುಗಿ;
    }
    qfl = qf_get_curlist(qi);
    ಆದರೆ (plus)
    {
	i = qfl->qf_ಸೂಚ್ಯಂಕ;
	idx2 = i + idx1;
	idx1 = i;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	i = qfl->qf_ಎಣಿಕೆ;
	ಆದರೆ (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	ಆದರೆ (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    ಆದರೆ (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    ಆದರೆ (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_ಸಾಲು_Attr = syn_name2attr((char_u *)"qfLineNr");
    ಆದರೆ (qf_ಸಾಲು_Attr == 0)
	qf_ಸಾಲು_Attr = HL_ATTR(HLF_N);

    ಆದರೆ (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಆದರೆ ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_ನಮೂದು(qfp, i, i == qfl->qf_ಸೂಚ್ಯಂಕ);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_fmt_ಪಠ್ಯ(garray_T *gap, char_u *ಪಠ್ಯ)
{
    char_u	*p = ಪಠ್ಯ;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (*p == '\n')
	{
	    ga_append(gap, ' ');
	    ಆಗ (*++p != NUL)
		ಆದರೆ (!VIM_ISWHITE(*p) && *p != '\n')
		    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_range_ಪಠ್ಯ(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    ಆದರೆ (qfp->qf_ಕೊನೆ_lnum > 0 && qfp->qf_lnum != qfp->qf_ಕೊನೆ_lnum)
    {
	buf.ಉದ್ದಳತೆ += vim_snprintf_safelen((ಅಕ್ಷರ *)buf.string + buf.ಉದ್ದಳತೆ,
	    IOSIZE - buf.ಉದ್ದಳತೆ, "-%ld", qfp->qf_ಕೊನೆ_lnum);
    }
    ಆದರೆ (qfp->qf_ಲಂಬಸಾಲು > 0)
    {
	buf.ಉದ್ದಳತೆ += vim_snprintf_safelen((ಅಕ್ಷರ *)buf.string + buf.ಉದ್ದಳತೆ,
	    IOSIZE - buf.ಉದ್ದಳತೆ, " col %d", qfp->qf_ಲಂಬಸಾಲು);
	ಆದರೆ (qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು > 0 && qfp->qf_ಲಂಬಸಾಲು != qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು)
	    buf.ಉದ್ದಳತೆ += vim_snprintf_safelen((ಅಕ್ಷರ *)buf.string + buf.ಉದ್ದಳತೆ,
		IOSIZE - buf.ಉದ್ದಳತೆ, "-%d", qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು);
    }

    ga_concat_len(gap, buf.string, buf.ಉದ್ದಳತೆ);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ಸಂದೇಶ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ which, ಅಕ್ಷರ *lead)
{
    ಅಕ್ಷರ   *title = (ಅಕ್ಷರ *)qi->qf_lists[which].qf_title;
    ಪೂರ್ಣಾಂಕ    ಎಣಿಕೆ_2 = qi->qf_lists[which].qf_ಎಣಿಕೆ;
    char_u buf[IOSIZE];

    vim_snprintf((ಅಕ್ಷರ *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    ಎಣಿಕೆ_2);

    ಆದರೆ (title != ಖಾಲಿ)
    {
	size_t	len = STRLEN(buf);

	ಆದರೆ (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    ಸಂದೇಶ((ಅಕ್ಷರ *)buf);
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
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ != 0)
	ಎಣಿಕೆ_2 = eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
	ಎಣಿಕೆ_2 = 1;
    ಆಗ (ಎಣಿಕೆ_2--)
    {
	ಆದರೆ (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    ಆದರೆ (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_ದ_quickfix_ರಾಶಿ));
		ನಿಲ್ಲಿಸು;
	    }
	    --qi->qf_curlist;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_ತುದಿ_ದ_quickfix_ರಾಶಿ));
		ನಿಲ್ಲಿಸು;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_ಸಂದೇಶ(qi, qi->qf_curlist, "");
    qf_update_ಬಫರ್(qi, ಖಾಲಿ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    ಶೂನ್ಯ
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_ರಾಶಿ(eap, FALSE);
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	ಆದರೆ (qi == ಖಾಲಿ)
	{
	    emsg(_(e_no_location_list));
	    ಹಿಂತಿರುಗಿ;
	}

	// Jump to the specified quickfix list
	ಆದರೆ (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_ಸಂದೇಶ(qi, qi->qf_curlist, "");
	    qf_update_ಬಫರ್(qi, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_invalid_range));

	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
	ಸಂದೇಶ(_("No entries"));
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (i = 0; i < qi->qf_listcount; ++i)
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
    ಪೂರ್ಣಾಂಕ		stop = FALSE;

    ಆಗ (qfl->qf_ಎಣಿಕೆ && qfl->qf_ಆರಂಭ != ಖಾಲಿ)
    {
	qfp = qfl->qf_ಆರಂಭ;
	qfpnext = qfp->qf_ಮುಂದಿನ;
	ಆದರೆ (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_ಪಠ್ಯ);
	    vim_free(qfp->qf_ನಮೂನೆ);
	    clear_tv(&qfp->qf_user_ದತ್ತಾಂಶ);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    ಆದರೆ (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_ಎಣಿಕೆ = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		qfl->qf_ಆರಂಭ = qfpnext;
	}
	--qfl->qf_ಎಣಿಕೆ;
    }

    qfl->qf_ಸೂಚ್ಯಂಕ = 0;
    qfl->qf_ಆರಂಭ = ಖಾಲಿ;
    qfl->qf_ಕೊನೆಯ = ಖಾಲಿ;
    qfl->qf_ptr = ಖಾಲಿ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_ರಾಶಿ(&qfl->qf_dir_ರಾಶಿ);
    qfl->qf_directory = ಖಾಲಿ;
    qf_clean_dir_ರಾಶಿ(&qfl->qf_file_ರಾಶಿ);
    qfl->qf_currfile = ಖಾಲಿ;
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
    qfl->qf_ctx = ಖಾಲಿ;
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
	ದೀರ್ಘ	amount,
	ದೀರ್ಘ	amount_after)
{
    ಪೂರ್ಣಾಂಕ		i;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		idx;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_one = FALSE;
    ಪೂರ್ಣಾಂಕ		buf_has_flag = wp == ಖಾಲಿ ? BUF_HAS_QF_ನಮೂದು : BUF_HAS_LL_ನಮೂದು;

    ಆದರೆ (!(curbuf->b_has_qf_ನಮೂದು & buf_has_flag))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	ಆದರೆ (wp->w_llist == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	qi = wp->w_llist;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	ಆದರೆ (!qf_list_ಖಾಲಿಗಳು(qfl))
	    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
		ಆದರೆ (qfp->qf_fnum == curbuf->b_fnum)
		{
		    ಸಿಕ್ಕಿತು_one = TRUE;
		    ಆದರೆ (qfp->qf_cleared)
			ಮುಂದುವರಿ;

		    ಆದರೆ (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			ಆದರೆ (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			    qfp->qf_lnum += amount;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    ಆದರೆ (!ಸಿಕ್ಕಿತು_one)
	curbuf->b_has_qf_ನಮೂದು &= ~buf_has_flag;
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
qf_types(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ nr)
{
    ಸ್ಥಿರ char_u	buf[20];
    ಸ್ಥಿರ char_u	cc[3];
    char_u		*p;

    ಆದರೆ (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'I' || c == 'i')
	p = (char_u *)" info";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'N' || c == 'n')
	p = (char_u *)" note";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 0 || c == 1)
	p = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    ಆದರೆ (nr <= 0)
	ಹಿಂತಿರುಗಿ p;

    sprintf((ಅಕ್ಷರ *)buf, "%s %3d", (ಅಕ್ಷರ *)p, nr);
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    ಶೂನ್ಯ
qf_view_ಫಲಿತಾಂಶ(ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು)
{
    qf_info_T   *qi = ql_info;

    ಆದರೆ (IS_LL_ಕಿಟಕಿ(curwin))
	qi = GET_LOC_LIST(curwin);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
    {
	emsg(_(e_no_quickfix_ರಾಶಿ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ವಿಭಜಿಸು)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (ದೀರ್ಘ)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	ಹಿಂತಿರುಗಿ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_ಕಿಟಕಿ(curwin) ? ".ll" : ".cc"));
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

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_ಕಂಡುಹಿಡಿ_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi)
	    || qfl->qf_nonevalid
	    || qf_list_ಖಾಲಿಗಳು(qfl))
    {
	ಆದರೆ (win != ಖಾಲಿ)
	    ex_cclose(eap);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win == ಖಾಲಿ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    ಶೂನ್ಯ
ex_cclose(exarg_T *eap)
{
    win_T	*win = ಖಾಲಿ;
    qf_info_T	*qi;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Find existing quickfix window and close it.
    win = qf_ಕಂಡುಹಿಡಿ_win(qi);
    ಆದರೆ (win != ಖಾಲಿ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ಗಣ_title_var(qf_list_T *qfl)
{
    ಆದರೆ (qfl->qf_title != ಖಾಲಿ)
	ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_goto_cwindow(qf_info_T *qi, ಪೂರ್ಣಾಂಕ resize, ಪೂರ್ಣಾಂಕ sz, ಪೂರ್ಣಾಂಕ vertsplit)
{
    win_T	*win;

    win = qf_ಕಂಡುಹಿಡಿ_win(qi);
    ಆದರೆ (win == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    win_goto(win);
    ಆದರೆ (resize)
    {
	ಆದರೆ (vertsplit)
	{
	    ಆದರೆ (sz != win->w_ಅಗಲ)
		win_setwidth(sz);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sz != win->w_ಎತ್ತರ && win->w_ಎತ್ತರ
		       + win->w_ಸ್ಥಿತಿ_ಎತ್ತರ + tabline_ಎತ್ತರ() < cmdline_ಅಡ್ಡಸಾಲು)
	    win_setheight(sz);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_ಗಣ_cwindow_options(ಶೂನ್ಯ)
{
    // switch off 'swapfile'
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"swf", 0L, ಖಾಲಿ, OPT_LOCAL);
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_open_new_cwindow(qf_info_T *qi, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    win_T	*win;

    qf_buf = qf_ಕಂಡುಹಿಡಿ_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    ಆದರೆ (IS_QF_ರಾಶಿ(qi) && cmdmod.cmod_ವಿಭಜಿಸು == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    ಆದರೆ (cmdmod.cmod_ವಿಭಜಿಸು == 0)
	flags = WSP_ಕೆಳಗೆ;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    ಆದರೆ (IS_QF_ರಾಶಿ(qi))
	flags |= WSP_QUICKFIX;

    ಆದರೆ (win_ವಿಭಜಿಸು(ಎತ್ತರ, flags) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    ಆದರೆ (IS_LL_ರಾಶಿ(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    ಆದರೆ (oldwin != curwin)
	oldwin = ಖಾಲಿ;  // don't store info when in another window
    ಆದರೆ (qf_buf != ಖಾಲಿ)
    {
	// Use the existing quickfix buffer
	ಆದರೆ (do_ecmd(qf_buf->b_fnum, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Create a new quickfix buffer
	ಆದರೆ (do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    ಆದರೆ (!bt_quickfix(curbuf))
	qf_ಗಣ_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    ಆದರೆ (curtab == prevtab && curwin->w_ಅಗಲ == Columns)
	win_setheight(ಎತ್ತರ);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    ಆದರೆ (win_valid(win))
	prevwin = win;

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = FAIL;
    ಪೂರ್ಣಾಂಕ		lnum;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    incr_quickfix_busy();

    ಆದರೆ (eap->addr_ಎಣಿಕೆ != 0)
	ಎತ್ತರ = eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
	ಎತ್ತರ = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    ಆದರೆ (cmdmod.cmod_tab == 0)
	ಸ್ಥಿತಿ_2 = qf_goto_cwindow(qi, eap->addr_ಎಣಿಕೆ != 0, ಎತ್ತರ,
						cmdmod.cmod_ವಿಭಜಿಸು & WSP_VERT);
    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
	ಆದರೆ (qf_open_new_cwindow(qi, ಎತ್ತರ) == FAIL)
	{
	    decr_quickfix_busy();
	    ಹಿಂತಿರುಗಿ;
	}

    qfl = qf_get_curlist(qi);
    qf_ಗಣ_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_ಸೂಚ್ಯಂಕ;

    // Fill the buffer with the quickfix list.
    qf_fill_ಬಫರ್(qfl, curbuf, ಖಾಲಿ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
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
    curbuf = win->w_ಬಫರ್;
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    update_topline();		// scroll to show the line
    redraw_later(UPD_VALID);
    curwin->w_redr_ಸ್ಥಿತಿ = true;	// update ruler
    curwin = old_curwin;
    curbuf = curwin->w_ಬಫರ್;
}

/*
 * :cbottom/:lbottom commands.
 */
    ಶೂನ್ಯ
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    win = qf_ಕಂಡುಹಿಡಿ_win(qi);
    ಆದರೆ (win != ಖಾಲಿ && win->w_cursor.lnum != win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	qf_win_goto(win, win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_ನಮೂದು(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    ಆದರೆ (IS_LL_ಕಿಟಕಿ(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ qf_get_curlist(qi)->qf_ಸೂಚ್ಯಂಕ;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_win_pos_update(
    qf_info_T	*qi,
    ಪೂರ್ಣಾಂಕ		old_qf_ಸೂಚ್ಯಂಕ)	// previous qf_index or zero
{
    win_T	*win;
    ಪೂರ್ಣಾಂಕ		qf_ಸೂಚ್ಯಂಕ = qf_get_curlist(qi)->qf_ಸೂಚ್ಯಂಕ;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_ಕಂಡುಹಿಡಿ_win(qi);
    ಆದರೆ (win != ಖಾಲಿ
	    && qf_ಸೂಚ್ಯಂಕ <= win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && old_qf_ಸೂಚ್ಯಂಕ != qf_ಸೂಚ್ಯಂಕ)
    {
	ಆದರೆ (qf_ಸೂಚ್ಯಂಕ > old_qf_ಸೂಚ್ಯಂಕ)
	{
	    win->w_redraw_ತುದಿ = old_qf_ಸೂಚ್ಯಂಕ;
	    win->w_redraw_bot = qf_ಸೂಚ್ಯಂಕ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    win->w_redraw_ತುದಿ = qf_ಸೂಚ್ಯಂಕ;
	    win->w_redraw_bot = old_qf_ಸೂಚ್ಯಂಕ;
	}
	qf_win_goto(win, qf_ಸೂಚ್ಯಂಕ);
    }
    ಹಿಂತಿರುಗಿ win != ಖಾಲಿ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    ಆದರೆ (buf_valid(win->w_ಬಫರ್) && bt_quickfix(win->w_ಬಫರ್))
	ಆದರೆ ((IS_QF_ರಾಶಿ(qi) && win->w_llist_ref == ಖಾಲಿ)
		|| (IS_LL_ರಾಶಿ(qi) && win->w_llist_ref == qi))
	    ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    ಸ್ಥಿರ win_T *
qf_ಕಂಡುಹಿಡಿ_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	ಆದರೆ (is_qf_win(win, qi))
	    ಹಿಂತಿರುಗಿ win;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    ಸ್ಥಿರ buf_T *
qf_ಕಂಡುಹಿಡಿ_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    ಆದರೆ (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	ಆದರೆ (qfbuf != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	ಆದರೆ (is_qf_win(win, qi))
	    ಹಿಂತಿರುಗಿ win->w_ಬಫರ್;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    ಅಕ್ಷರ *
did_ಗಣ_quickfixtextfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (option_ಗಣ_callback_func(p_qftf, &qftf_cb) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
	ಆದರೆ (is_qf_win(win, qi))
	{
	    curwin = win;
	    qf_ಗಣ_title_var(qfl);
	}
    }
    curwin = save_curwin;
}

/*
 * Find the quickfix buffer.  If it exists, update the contents.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_update_ಬಫರ್(qf_info_T *qi, qfline_T *old_ಕೊನೆಯ)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_ಕಂಡುಹಿಡಿ_buf(qi);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    linenr_T	old_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		qf_winid = 0;

    ಆದರೆ (IS_LL_ರಾಶಿ(qi))
    {
	ಆದರೆ (curwin->w_llist == qi)
	    win = curwin;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qi);
	    ಆದರೆ (win == ಖಾಲಿ)
		// File window is not found. Find the location list window.
		win = qf_ಕಂಡುಹಿಡಿ_win(qi);
	    ಆದರೆ (win == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    ಪೂರ್ಣಾಂಕ do_fill = TRUE;
    ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    ಆದರೆ (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_ಬಫರ್(qf_get_curlist(qi), buf, old_ಕೊನೆಯ, qf_winid);
	++CHANGEDTICK(buf);

	ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    ಆದರೆ ((win = qf_ಕಂಡುಹಿಡಿ_win(qi)) != ಖಾಲಿ && old_ಸಾಲು_ಎಣಿಕೆ < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_buf_ಕೂಡಿಸು_ಸಾಲು(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	ಪೂರ್ಣಾಂಕ		ಮೊದಲ_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    ಆದರೆ (qftf_str != ಖಾಲಿ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (qfp->qf_module != ಖಾಲಿ)
	    ga_concat(gap, qfp->qf_module);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ಖಾಲಿ
		&& errbuf->b_fname != ಖಾಲಿ)
	{
	    ಆದರೆ (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		ಆದರೆ (ಮೊದಲ_bufline && (errbuf->b_sfname == ಖಾಲಿ
				|| mch_is_ತುಂಬಿದ_Name(errbuf->b_sfname)))
		{
		    ಆದರೆ (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		ಆದರೆ (qfp->qf_fname == ಖಾಲಿ)
		    ga_concat(gap, errbuf->b_fname);
		ಇಲ್ಲದಿದ್ದರೆ
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	ಆದರೆ (qfp->qf_lnum > 0)
	{
	    qf_range_ಪಠ್ಯ(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qfp->qf_ನಮೂನೆ != ಖಾಲಿ)
	    qf_fmt_ಪಠ್ಯ(gap, qfp->qf_ನಮೂನೆ);
	ga_append(gap, '|');
	ga_append(gap, ' ');

	// Remove newlines and leading whitespace from the text.
	// For an unrecognized line keep the indent, the compiler may
	// mark a word with ^^^^.
	qf_fmt_ಪಠ್ಯ(gap, gap->ga_len > 3 ? skipwhite(qfp->qf_ಪಠ್ಯ)
							       : qfp->qf_ಪಠ್ಯ);
    }

    ga_append(gap, NUL);
    ಆದರೆ (ml_append_buf(buf, lnum, gap->ga_ದತ್ತಾಂಶ, gap->ga_len, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    ಸ್ಥಿರ list_T *
call_qftf_func(qf_list_T *qfl, ಪೂರ್ಣಾಂಕ qf_winid, ದೀರ್ಘ ಆರಂಭ_idx, ದೀರ್ಘ ಕೊನೆ_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = FALSE;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    ಆದರೆ (qfl->qf_qftf_cb.cb_name != ಖಾಲಿ)
	cb = &qfl->qf_qftf_cb;
    ಆದರೆ (cb->cb_name != ಖಾಲಿ)
    {
	typval_T	ವಾದಗಳು[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	ಆದರೆ ((d = dict_alloc_ಬೀಗ(VAR_FIXED)) == ಖಾಲಿ)
	{
	    recursive = FALSE;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "quickfix", (ದೀರ್ಘ)IS_QF_LIST(qfl));
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "winid", (ದೀರ್ಘ)qf_winid);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "id", (ದೀರ್ಘ)qfl->qf_id);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "start_idx", ಆರಂಭ_idx);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "end_idx", ಕೊನೆ_idx);
	++d->dv_refcount;
	ವಾದಗಳು[0].v_type = VAR_DICT;
	ವಾದಗಳು[0].vval.v_dict = d;

	qftf_list = ಖಾಲಿ;
	textlock++;
	ಆದರೆ (call_callback(cb, 0, &rettv, 1, ವಾದಗಳು) != FAIL)
	{
	    ಆದರೆ (rettv.v_type == VAR_LIST)
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
    ಹಿಂತಿರುಗಿ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_fill_ಬಫರ್(qf_list_T *qfl, buf_T *buf, qfline_T *old_ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		old_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    list_T	*qftf_list = ಖಾಲಿ;
    listitem_T	*qftf_li = ಖಾಲಿ;

    ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	ಆದರೆ (buf != curbuf)
	{
	    ಆಂತರಿಕ_error("qf_fill_buffer()");
	    ಹಿಂತಿರುಗಿ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	ಆಗ ((curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) == 0)
	    (ಶೂನ್ಯ)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    ಆದರೆ (qfl != ಖಾಲಿ && qfl->qf_ಆರಂಭ != ಖಾಲಿ)
    {
	char_u		dirname[MAXPATHL];
	ಪೂರ್ಣಾಂಕ		invalid_val = FALSE;
	ಪೂರ್ಣಾಂಕ		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
	{
	    qfp = qfl->qf_ಆರಂಭ;
	    lnum = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (old_ಕೊನೆಯ->qf_ಮುಂದಿನ != ಖಾಲಿ)
		qfp = old_ಕೊನೆಯ->qf_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		qfp = old_ಕೊನೆಯ;
	    lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (ದೀರ್ಘ)(lnum + 1),
							(ದೀರ್ಘ)qfl->qf_ಎಣಿಕೆ);
	ಆದರೆ (qftf_list != ಖಾಲಿ)
	    qftf_li = qftf_list->lv_ಮೊದಲ;

	ಆಗ (lnum < qfl->qf_ಎಣಿಕೆ)
	{
	    char_u	*qftf_str = ಖಾಲಿ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    ಆದರೆ (qftf_li != ಖಾಲಿ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		ಆದರೆ (qftf_str == ಖಾಲಿ)
		    invalid_val = TRUE;
	    }

	    ಆದರೆ (qf_buf_ಕೂಡಿಸು_ಸಾಲು(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		ನಿಲ್ಲಿಸು;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_ಮುಂದಿನ;
	    ಆದರೆ (qfp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (qftf_li != ಖಾಲಿ)
		qftf_li = qftf_li->li_ಮುಂದಿನ;
	}

	ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
	    // Delete the empty line which is now at the end
	    (ಶೂನ್ಯ)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    ಆದರೆ (old_ಕೊನೆಯ == ಖಾಲಿ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_ಬೀಗ;
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ಖಾಲಿ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ಖಾಲಿ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_ಬೀಗ;

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
qf_list_ಬದಲಾಯಿತು(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    ಪೂರ್ಣಾಂಕ		qf_idx;

    ಗಾಗಿ (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	ಆದರೆ (qi->qf_lists[qf_idx].qf_id == qfid)
	    ಹಿಂತಿರುಗಿ qf_idx;
    ಹಿಂತಿರುಗಿ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    ಪೂರ್ಣಾಂಕ curlist;

    ಆದರೆ (qf_get_curlist(qi)->qf_id == save_qfid)
	ಹಿಂತಿರುಗಿ OK;

    curlist = qf_id2nr(qi, save_qfid);
    ಆದರೆ (curlist < 0)
	// list is not present
	ಹಿಂತಿರುಗಿ FAIL;
    qi->qf_curlist = curlist;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_jump_ಮೊದಲ(qf_info_T *qi, int_u save_qfid, ಪೂರ್ಣಾಂಕ forceit)
{
    ಆದರೆ (qf_restore_list(qi, save_qfid) == FAIL)
	ಹಿಂತಿರುಗಿ;


    ಆದರೆ (!check_can_ಗಣ_curbuf_forceit(forceit))
	ಹಿಂತಿರುಗಿ;


    // Autocommands might have cleared the list, check for that.
    ಆದರೆ (!qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    ಪೂರ್ಣಾಂಕ
grep_ಆಂತರಿಕ(cmdidx_T cmdidx)
{
    ಹಿಂತಿರುಗಿ ((cmdidx == CMD_grep
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
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_make:	    ಹಿಂತಿರುಗಿ (char_u *)"make";
	ಸ್ಥಿತಿ CMD_lmake:	    ಹಿಂತಿರುಗಿ (char_u *)"lmake";
	ಸ್ಥಿತಿ CMD_grep:	    ಹಿಂತಿರುಗಿ (char_u *)"grep";
	ಸ್ಥಿತಿ CMD_lgrep:	    ಹಿಂತಿರುಗಿ (char_u *)"lgrep";
	ಸ್ಥಿತಿ CMD_grepadd:   ಹಿಂತಿರುಗಿ (char_u *)"grepadd";
	ಸ್ಥಿತಿ CMD_lgrepadd:  ಹಿಂತಿರುಗಿ (char_u *)"lgrepadd";
	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಆರಂಭ = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    ಆದರೆ (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	ಹಿಂತಿರುಗಿ name;
    }

    ಗಾಗಿ (p = p_mef; *p; ++p)
	ಆದರೆ (p[0] == '#' && p[1] == '#')
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (ಆರಂಭ == -1)
	    ಆರಂಭ = mch_get_pid();
	ಇಲ್ಲದಿದ್ದರೆ
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	ಆದರೆ (name == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	STRCPY(name, p_mef);
	sprintf((ಅಕ್ಷರ *)name + (p - p_mef), "%d%d", ಆರಂಭ, off);
	STRCAT(name, p + 2);
	ಆದರೆ (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((ಅಕ್ಷರ *)name, &sb) < 0
# endif
		)
	    ನಿಲ್ಲಿಸು;
	vim_free(name);
    }
    ಹಿಂತಿರುಗಿ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    ಸ್ಥಿರ char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    ಚಿಹ್ನೆರಹಿತ	len;

    len = (ಚಿಹ್ನೆರಹಿತ)STRLEN(p_shq) * 2 + (ಚಿಹ್ನೆರಹಿತ)STRLEN(makecmd) + 1;
    ಆದರೆ (*p_sp != NUL)
	len += (ಚಿಹ್ನೆರಹಿತ)STRLEN(p_sp) + (ಚಿಹ್ನೆರಹಿತ)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    ಆದರೆ (cmd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    sprintf((ಅಕ್ಷರ *)cmd, "%s%s%s", (ಅಕ್ಷರ *)p_shq, (ಅಕ್ಷರ *)makecmd,
							       (ಅಕ್ಷರ *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    ಆದರೆ (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    ಆದರೆ (ಸಂದೇಶ_ಲಂಬಸಾಲು == 0)
	ಸಂದೇಶ_didout = FALSE;
    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_puts(":!");
    ಸಂದೇಶ_outtrans(cmd);		// show what we are doing

    ಹಿಂತಿರುಗಿ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    ಶೂನ್ಯ
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ಖಾಲಿ;
    win_T	*wp = ಖಾಲಿ;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಂಕ		res;
    char_u	*au_name = ಖಾಲಿ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    ಪೂರ್ಣಾಂಕ		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    ಆದರೆ (grep_ಆಂತರಿಕ(eap->cmdidx))
    {
	ex_vimgrep(eap);
	ಹಿಂತಿರುಗಿ;
    }

    au_name = make_get_auname(eap->cmdidx);
    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    ಆದರೆ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    ಆದರೆ (cmd == ಖಾಲಿ)
    {
	vim_free(fname);
	ಹಿಂತಿರುಗಿ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    ಹೊರಹೋಗು_flush();
		// read window status report and redraw before message
    (ಶೂನ್ಯ)char_avail();
# endif

    incr_quickfix_busy();

    ಆದರೆ (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    ಆದರೆ (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	qi = GET_LOC_LIST(wp);
	ಆದರೆ (qi == ಖಾಲಿ)
	    ಹೋಗು cleanup;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
	ಹೋಗು cleanup;

    ಆದರೆ (res >= 0)
	qf_list_ಬದಲಾಯಿತು(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಆದರೆ (au_name != ಖಾಲಿ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    ಆದರೆ (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ಮೊದಲ(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}

/*
 * Returns the number of entries in the current quickfix/location list.
 */
    ಪೂರ್ಣಾಂಕ
qf_get_ಗಾತ್ರ(exarg_T *eap)
{
    qf_info_T	*qi;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ qf_get_curlist(qi)->qf_ಎಣಿಕೆ;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    ಪೂರ್ಣಾಂಕ
qf_get_valid_ಗಾತ್ರ(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		i, sz = 0;
    ಪೂರ್ಣಾಂಕ		prev_fnum = 0;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಆದರೆ (qfp->qf_valid)
	{
	    ಆದರೆ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    ಪೂರ್ಣಾಂಕ
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಹಿಂತಿರುಗಿ qf_get_curlist(qi)->qf_ಸೂಚ್ಯಂಕ;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    ಪೂರ್ಣಾಂಕ
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		i, eidx = 0;
    ಪೂರ್ಣಾಂಕ		prev_fnum = 0;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, FALSE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_ಆರಂಭ;

    // check if the list has valid errors
    ಆದರೆ (!qf_list_has_valid_entries(qfl))
	ಹಿಂತಿರುಗಿ 1;

    ಗಾಗಿ (i = 1; i <= qfl->qf_ಸೂಚ್ಯಂಕ && qfp!= ಖಾಲಿ; i++, qfp = qfp->qf_ಮುಂದಿನ)
    {
	ಆದರೆ (qfp->qf_valid)
	{
	    ಆದರೆ (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ಆದರೆ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		eidx++;
	}
    }

    ಹಿಂತಿರುಗಿ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_nth_valid_ನಮೂದು(qf_list_T *qfl, ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ fdo)
{
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		i, eidx;
    ಪೂರ್ಣಾಂಕ		prev_fnum = 0;

    // check if the list has valid errors
    ಆದರೆ (!qf_list_has_valid_entries(qfl))
	ಹಿಂತಿರುಗಿ 1;

    eidx = 0;
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಆದರೆ (qfp->qf_valid)
	{
	    ಆದರೆ (fdo)
	    {
		ಆದರೆ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		eidx++;
	}

	ಆದರೆ (eidx == n)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (i <= qfl->qf_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ i;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 1;
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
    ಪೂರ್ಣಾಂಕ		errornr;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	errornr = (ಪೂರ್ಣಾಂಕ)eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (eap->cmdidx)
	{
	    ಸ್ಥಿತಿ CMD_cc: ಸ್ಥಿತಿ CMD_ll:
		errornr = 0;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_crewind: ಸ್ಥಿತಿ CMD_lrewind: ಸ್ಥಿತಿ CMD_cfirst:
	    ಸ್ಥಿತಿ CMD_lfirst:
		errornr = 1;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    ಆದರೆ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_ನಮೂದು(qf_get_curlist(qi),
		eap->addr_ಎಣಿಕೆ > 0 ? (ಪೂರ್ಣಾಂಕ)eap->line1 : 1,
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
    ಪೂರ್ಣಾಂಕ		errornr;
    ಪೂರ್ಣಾಂಕ		dir;

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (ಪೂರ್ಣಾಂಕ)eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    ಆಯ್ಕೆ (eap->cmdidx)
    {
	ಸ್ಥಿತಿ CMD_cnext: ಸ್ಥಿತಿ CMD_lnext: ಸ್ಥಿತಿ CMD_cdo: ಸ್ಥಿತಿ CMD_ldo:
	    dir = FORWARD;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_cprevious: ಸ್ಥಿತಿ CMD_lprevious: ಸ್ಥಿತಿ CMD_c_ಮುಂದಿನ:
	ಸ್ಥಿತಿ CMD_l_ಮುಂದಿನ:
	    dir = BACKWARD;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_cnfile: ಸ್ಥಿತಿ CMD_lnfile: ಸ್ಥಿತಿ CMD_cfdo: ಸ್ಥಿತಿ CMD_lfdo:
	    dir = FORWARD_FILE;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_cpfile: ಸ್ಥಿತಿ CMD_lpfile: ಸ್ಥಿತಿ CMD_cNfile: ಸ್ಥಿತಿ CMD_lNfile:
	    dir = BACKWARD_FILE;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    dir = FORWARD;
	    ನಿಲ್ಲಿಸು;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    ಸ್ಥಿರ qfline_T *
qf_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು_ಒಳ_buf(qf_list_T *qfl, ಪೂರ್ಣಾಂಕ bnr, ಪೂರ್ಣಾಂಕ *errornr)
{
    qfline_T	*qfp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, idx)
	ಆದರೆ (qfp->qf_fnum == bnr)
	    ನಿಲ್ಲಿಸು;

    *errornr = idx;
    ಹಿಂತಿರುಗಿ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ಸ್ಥಿರ qfline_T *
qf_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು_on_ಸಾಲು(qfline_T *ನಮೂದು, ಪೂರ್ಣಾಂಕ *errornr)
{
    ಆಗ (!got_int
	    && ನಮೂದು->qf_prev != ಖಾಲಿ
	    && ನಮೂದು->qf_fnum == ನಮೂದು->qf_prev->qf_fnum
	    && ನಮೂದು->qf_lnum == ನಮೂದು->qf_prev->qf_lnum)
    {
	ನಮೂದು = ನಮೂದು->qf_prev;
	--*errornr;
    }

    ಹಿಂತಿರುಗಿ ನಮೂದು;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    ಸ್ಥಿರ qfline_T *
qf_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ನಮೂದು_on_ಸಾಲು(qfline_T *ನಮೂದು, ಪೂರ್ಣಾಂಕ *errornr)
{
    ಆಗ (!got_int &&
	    ನಮೂದು->qf_ಮುಂದಿನ != ಖಾಲಿ
	    && ನಮೂದು->qf_fnum == ನಮೂದು->qf_ಮುಂದಿನ->qf_fnum
	    && ನಮೂದು->qf_lnum == ನಮೂದು->qf_ಮುಂದಿನ->qf_lnum)
    {
	ನಮೂದು = ನಮೂದು->qf_ಮುಂದಿನ;
	++*errornr;
    }

    ಹಿಂತಿರುಗಿ ನಮೂದು;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ನಮೂದು_after_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಂಕ linewise)
{
    ಆದರೆ (linewise)
	ಹಿಂತಿರುಗಿ qfp->qf_lnum > pos->lnum;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ಲಂಬಸಾಲು > pos->ಲಂಬಸಾಲು));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ನಮೂದು_before_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಂಕ linewise)
{
    ಆದರೆ (linewise)
	ಹಿಂತಿರುಗಿ qfp->qf_lnum < pos->lnum;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ಲಂಬಸಾಲು < pos->ಲಂಬಸಾಲು));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ನಮೂದು_on_or_after_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಂಕ linewise)
{
    ಆದರೆ (linewise)
	ಹಿಂತಿರುಗಿ qfp->qf_lnum >= pos->lnum;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ಲಂಬಸಾಲು >= pos->ಲಂಬಸಾಲು));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ನಮೂದು_on_or_before_pos(qfline_T *qfp, pos_T *pos, ಪೂರ್ಣಾಂಕ linewise)
{
    ಆದರೆ (linewise)
	ಹಿಂತಿರುಗಿ qfp->qf_lnum <= pos->lnum;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_ಲಂಬಸಾಲು <= pos->ಲಂಬಸಾಲು));
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
qf_ಕಂಡುಹಿಡಿ_ನಮೂದು_after_pos(
	ಪೂರ್ಣಾಂಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಂಕ		linewise,
	qfline_T	*qfp,
	ಪೂರ್ಣಾಂಕ		*errornr)
{
    ಆದರೆ (qf_ನಮೂದು_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	ಹಿಂತಿರುಗಿ qfp;

    // Find the entry just before or at the position 'pos'
    ಆಗ (qfp->qf_ಮುಂದಿನ != ಖಾಲಿ
	    && qfp->qf_ಮುಂದಿನ->qf_fnum == bnr
	    && qf_ನಮೂದು_on_or_before_pos(qfp->qf_ಮುಂದಿನ, pos, linewise))
    {
	qfp = qfp->qf_ಮುಂದಿನ;
	++*errornr;
    }

    ಆದರೆ (qfp->qf_ಮುಂದಿನ == ಖಾಲಿ || qfp->qf_ಮುಂದಿನ->qf_fnum != bnr)
	// No entries found after position 'pos'
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_ಮುಂದಿನ;
    ++*errornr;

    ಹಿಂತಿರುಗಿ qfp;
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
qf_ಕಂಡುಹಿಡಿ_ನಮೂದು_before_pos(
	ಪೂರ್ಣಾಂಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಂಕ		linewise,
	qfline_T	*qfp,
	ಪೂರ್ಣಾಂಕ		*errornr)
{
    // Find the entry just before the position 'pos'
    ಆಗ (qfp->qf_ಮುಂದಿನ != ಖಾಲಿ
	    && qfp->qf_ಮುಂದಿನ->qf_fnum == bnr
	    && qf_ನಮೂದು_before_pos(qfp->qf_ಮುಂದಿನ, pos, linewise))
    {
	qfp = qfp->qf_ಮುಂದಿನ;
	++*errornr;
    }

    ಆದರೆ (qf_ನಮೂದು_on_or_after_pos(qfp, pos, linewise))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು_on_ಸಾಲು(qfp, errornr);

    ಹಿಂತಿರುಗಿ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    ಸ್ಥಿರ qfline_T *
qf_ಕಂಡುಹಿಡಿ_closest_ನಮೂದು(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಂಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		linewise,
	ಪೂರ್ಣಾಂಕ		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು_ಒಳ_buf(qfl, bnr, errornr);
    ಆದರೆ (qfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;		// no entry in this file

    ಆದರೆ (dir == FORWARD)
	qfp = qf_ಕಂಡುಹಿಡಿ_ನಮೂದು_after_pos(bnr, pos, linewise, qfp, errornr);
    ಇಲ್ಲದಿದ್ದರೆ
	qfp = qf_ಕಂಡುಹಿಡಿ_ನಮೂದು_before_pos(bnr, pos, linewise, qfp, errornr);

    ಹಿಂತಿರುಗಿ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_get_nth_ಕೆಳಗೆ_ನಮೂದು(qfline_T *ನಮೂದು_arg, ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ linewise, ಪೂರ್ಣಾಂಕ *errornr)
{
    qfline_T *ನಮೂದು = ನಮೂದು_arg;

    ಆಗ (n-- > 0 && !got_int)
    {
	ಪೂರ್ಣಾಂಕ		ಮೊದಲ_errornr = *errornr;

	ಆದರೆ (linewise)
	    // Treat all the entries on the same line in this file as one
	    ನಮೂದು = qf_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ನಮೂದು_on_ಸಾಲು(ನಮೂದು, errornr);

	ಆದರೆ (ನಮೂದು->qf_ಮುಂದಿನ == ಖಾಲಿ
		|| ನಮೂದು->qf_ಮುಂದಿನ->qf_fnum != ನಮೂದು->qf_fnum)
	{
	    ಆದರೆ (linewise)
		*errornr = ಮೊದಲ_errornr;
	    ನಿಲ್ಲಿಸು;
	}

	ನಮೂದು = ನಮೂದು->qf_ಮುಂದಿನ;
	++*errornr;
    }
}

/*
 * Get the nth quickfix entry above the specified entry.  Searches backwards in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_get_nth_above_ನಮೂದು(qfline_T *ನಮೂದು, ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ linewise, ಪೂರ್ಣಾಂಕ *errornr)
{
    ಆಗ (n-- > 0 && !got_int)
    {
	ಆದರೆ (ನಮೂದು->qf_prev == ಖಾಲಿ
		|| ನಮೂದು->qf_prev->qf_fnum != ನಮೂದು->qf_fnum)
	    ನಿಲ್ಲಿಸು;

	ನಮೂದು = ನಮೂದು->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	ಆದರೆ (linewise)
	    ನಮೂದು = qf_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು_on_ಸಾಲು(ನಮೂದು, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಕಂಡುಹಿಡಿ_nth_adj_ನಮೂದು(
	qf_list_T	*qfl,
	ಪೂರ್ಣಾಂಕ		bnr,
	pos_T		*pos,
	ಪೂರ್ಣಾಂಕ		n,
	ಪೂರ್ಣಾಂಕ		dir,
	ಪೂರ್ಣಾಂಕ		linewise)
{
    qfline_T	*adj_ನಮೂದು;
    ಪೂರ್ಣಾಂಕ		errornr;

    // Find an entry closest to the specified position
    adj_ನಮೂದು = qf_ಕಂಡುಹಿಡಿ_closest_ನಮೂದು(qfl, bnr, pos, dir, linewise, &errornr);
    ಆದರೆ (adj_ನಮೂದು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	ಆದರೆ (dir == FORWARD)
	    qf_get_nth_ಕೆಳಗೆ_ನಮೂದು(adj_ನಮೂದು, n, linewise, &errornr);
	ಇಲ್ಲದಿದ್ದರೆ
	    qf_get_nth_above_ನಮೂದು(adj_ನಮೂದು, n, linewise, &errornr);
    }

    ಹಿಂತಿರುಗಿ errornr;
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
    ಪೂರ್ಣಾಂಕ		dir;
    ಪೂರ್ಣಾಂಕ		buf_has_flag;
    ಪೂರ್ಣಾಂಕ		errornr = 0;
    pos_T	pos;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ;
    }

    // Check whether the current buffer has any quickfix entries
    ಆದರೆ (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ನಮೂದು;
    ಇಲ್ಲದಿದ್ದರೆ
	buf_has_flag = BUF_HAS_LL_ನಮೂದು;
    ಆದರೆ (!(curbuf->b_has_qf_ನಮೂದು & buf_has_flag))
    {
	emsg(_(e_no_errors));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ ((qi = qf_cmd_get_ರಾಶಿ(eap, TRUE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    ಆದರೆ (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    ಇಲ್ಲದಿದ್ದರೆ
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.ಲಂಬಸಾಲು++;
    errornr = qf_ಕಂಡುಹಿಡಿ_nth_adj_ನಮೂದು(qfl, curbuf->b_fnum, &pos,
				eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    ಆದರೆ (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_no_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    ಸ್ಥಿರ char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_cfile:	    ಹಿಂತಿರುಗಿ (char_u *)"cfile";
	ಸ್ಥಿತಿ CMD_cgetfile:  ಹಿಂತಿರುಗಿ (char_u *)"cgetfile";
	ಸ್ಥಿತಿ CMD_caddfile:  ಹಿಂತಿರುಗಿ (char_u *)"caddfile";
	ಸ್ಥಿತಿ CMD_lfile:	    ಹಿಂತಿರುಗಿ (char_u *)"lfile";
	ಸ್ಥಿತಿ CMD_lgetfile:  ಹಿಂತಿರುಗಿ (char_u *)"lgetfile";
	ಸ್ಥಿತಿ CMD_laddfile:  ಹಿಂತಿರುಗಿ (char_u *)"laddfile";
	ಪೂರ್ವನಿಯೋಜಿತ:	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    ಶೂನ್ಯ
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ಖಾಲಿ;
    win_T	*wp = ಖಾಲಿ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ಖಾಲಿ;
    int_u	save_qfid = 0;		// init for gcc
    ಪೂರ್ಣಾಂಕ		res;

    au_name = cfile_get_auname(eap->cmdidx);
    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ಖಾಲಿ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ಖಾಲಿ, ಖಾಲಿ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ಖಾಲಿ);
	ಆದರೆ (browse_file == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಗಣ_string_option_ನೇರ((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    ಆದರೆ (*eap->arg != NUL)
	ಗಣ_string_option_ನೇರ((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    ಆದರೆ (is_loclist_cmd(eap->cmdidx))
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
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	qi = GET_LOC_LIST(wp);
	ಆದರೆ (qi == ಖಾಲಿ)
	{
	    decr_quickfix_busy();
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
    {
	decr_quickfix_busy();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (res >= 0)
	qf_list_ಬದಲಾಯಿತು(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಆದರೆ (au_name != ಖಾಲಿ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ಖಾಲಿ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ಆದರೆ (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ಮೊದಲ(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    ಸ್ಥಿರ char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_vimgrep:     ಹಿಂತಿರುಗಿ (char_u *)"vimgrep";
	ಸ್ಥಿತಿ CMD_lvimgrep:    ಹಿಂತಿರುಗಿ (char_u *)"lvimgrep";
	ಸ್ಥಿತಿ CMD_vimgrepadd:  ಹಿಂತಿರುಗಿ (char_u *)"vimgrepadd";
	ಸ್ಥಿತಿ CMD_lvimgrepadd: ಹಿಂತಿರುಗಿ (char_u *)"lvimgrepadd";
	ಸ್ಥಿತಿ CMD_grep:	      ಹಿಂತಿರುಗಿ (char_u *)"grep";
	ಸ್ಥಿತಿ CMD_lgrep:	      ಹಿಂತಿರುಗಿ (char_u *)"lgrep";
	ಸ್ಥಿತಿ CMD_grepadd:     ಹಿಂತಿರುಗಿ (char_u *)"grepadd";
	ಸ್ಥಿತಿ CMD_lgrepadd:    ಹಿಂತಿರುಗಿ (char_u *)"lgrepadd";
	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ಖಾಲಿ;

    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	ಆದರೆ (ಕೊನೆಯ_ಹುಡುಕು_pat() == ಖಾಲಿ)
	{
	    emsg(_(e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ));
	    ಹಿಂತಿರುಗಿ;
	}
	regmatch->regprog = vim_regcomp(ಕೊನೆಯ_ಹುಡುಕು_pat(), RE_MAGIC);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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

    ಸಂದೇಶ_ಆರಂಭ();
    p = ಸಂದೇಶ_strtrunc(fname, TRUE);
    ಆದರೆ (p == ಖಾಲಿ)
	ಸಂದೇಶ_outtrans(fname);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_outtrans(p);
	vim_free(p);
    }
    ಸಂದೇಶ_clr_eos();
    ಸಂದೇಶ_didout = FALSE;	    // overwrite this message
    ಸಂದೇಶ_nowait = TRUE;	    // don't wait for this message
    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
    ಹೊರಹೋಗು_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    ಸ್ಥಿರ buf_T *
vgr_ತುಂಬು_dummy_buf(
	char_u *fname,
	char_u *dirname_ಆರಂಭ,
	char_u *dirname_now)
{
    ಪೂರ್ಣಾಂಕ		save_mls;
# ಆದರೆ defined(FEAT_SYN_HL)
    char_u	*save_ei = ಖಾಲಿ;
# endif
    buf_T	*buf;

# ಆದರೆ defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
# endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = ತುಂಬು_dummy_ಬಫರ್(fname, dirname_ಆರಂಭ, dirname_now);

    p_mls = save_mls;
# ಆದರೆ defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    ಆದರೆ (!qflist_valid(wp, qfid))
    {
	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_ಬದಲಾಯಿತು));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }

    ಆದರೆ (qf_restore_list(qi, qfid) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vgr_ಹೊಂದಿಕೆ_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	ದೀರ್ಘ	    *tomatch,
	ಪೂರ್ಣಾಂಕ	    duplicate_name,
	ಪೂರ್ಣಾಂಕ	    flags)
{
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = FALSE;
    ದೀರ್ಘ	lnum;
    colnr_T	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		pat_len = (ಪೂರ್ಣಾಂಕ)STRLEN(spat);
    ಆದರೆ (pat_len > FUZZY_ಹೊಂದಿಕೆ_MAX_LEN)
	pat_len = FUZZY_ಹೊಂದಿಕೆ_MAX_LEN;

    ಗಾಗಿ (lnum = 1; lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && *tomatch > 0; ++lnum)
    {
	ಲಂಬಸಾಲು = 0;
	ಆದರೆ (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    ಆಗ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								ಲಂಬಸಾಲು, ಖಾಲಿ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ಆದರೆ (qf_ಕೂಡಿಸು_ನಮೂದು(qfl,
			    ಖಾಲಿ,	// dir
			    fname,
			    ಖಾಲಿ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].ಲಂಬಸಾಲು + 1,
			    regmatch->endpos[0].ಲಂಬಸಾಲು + 1,
			    FALSE,	// vis_col
			    ಖಾಲಿ,	// search pattern
			    0,		// nr
			    0,		// type
			    ಖಾಲಿ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = TRUE;
		ಆದರೆ (--*tomatch == 0)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    ನಿಲ್ಲಿಸು;
		ಲಂಬಸಾಲು = regmatch->endpos[0].ಲಂಬಸಾಲು
		    + (ಲಂಬಸಾಲು == regmatch->endpos[0].ಲಂಬಸಾಲು);
		ಆದರೆ (ಲಂಬಸಾಲು > ml_get_buf_len(buf, lnum))
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    ಪೂರ್ಣಾಂಕ	    score;
	    int_u   matches[FUZZY_ಹೊಂದಿಕೆ_MAX_LEN];
	    int_u   sz = ARRAY_ಉದ್ದಳತೆ(matches);

	    // Fuzzy string match
	    CLEAR_ಕ್ಷೇತ್ರ(matches);
	    ಆಗ (fuzzy_ಹೊಂದಿಕೆ(str + ಲಂಬಸಾಲು, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		ಆದರೆ (qf_ಕೂಡಿಸು_ನಮೂದು(qfl,
			    ಖಾಲಿ,	// dir
			    fname,
			    ಖಾಲಿ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + ಲಂಬಸಾಲು + 1,
			    0,
			    FALSE,	// vis_col
			    ಖಾಲಿ,	// search pattern
			    0,		// nr
			    0,		// type
			    ಖಾಲಿ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = TRUE;
		ಆದರೆ (--*tomatch == 0)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ ((flags & VGR_GLOBAL) == 0)
		    ನಿಲ್ಲಿಸು;
		ಲಂಬಸಾಲು = matches[pat_len - 1] + ಲಂಬಸಾಲು + 1;
		ಆದರೆ (ಲಂಬಸಾಲು > linelen)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಸಾಲು_breakcheck();
	ಆದರೆ (got_int)
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ;
}

/*
 * Jump to the first match and update the directory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vgr_jump_ಗಮ್ಯ_ಹೊಂದಿಕೆ(
	qf_info_T   *qi,
	ಪೂರ್ಣಾಂಕ	    forceit,
	ಪೂರ್ಣಾಂಕ	    *redraw_for_dummy,
	buf_T	    *ಮೊದಲ_ಹೊಂದಿಕೆ_buf,
	char_u	    *ಗುರಿ_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    ಆದರೆ (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    ಆದರೆ (curbuf == ಮೊದಲ_ಹೊಂದಿಕೆ_buf && ಗುರಿ_dir != ಖಾಲಿ)
    {
	exarg_T ea;

	CLEAR_ಕ್ಷೇತ್ರ(ea);
	ea.arg = ಗುರಿ_dir;
	ea.cmdidx = CMD_lcd;
	ex_cd(&ea);
    }
}

/*
 * :vimgrep command arguments
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ದೀರ್ಘ	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    ಪೂರ್ಣಾಂಕ		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    ಪೂರ್ಣಾಂಕ		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_ವಾದಗಳು_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vgr_process_ವಾದಗಳು(
	exarg_T		*eap,
	vgr_ವಾದಗಳು_T	*ವಾದಗಳು)
{
    char_u	*p;

    CLEAR_ಸೂಚಕ(ವಾದಗಳು);

    ವಾದಗಳು->regmatch.regprog = ಖಾಲಿ;
    ವಾದಗಳು->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
	ವಾದಗಳು->tomatch = eap->line2;
    ಇಲ್ಲದಿದ್ದರೆ
	ವಾದಗಳು->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &ವಾದಗಳು->spat, &ವಾದಗಳು->flags);
    ಆದರೆ (p == ಖಾಲಿ)
    {
	emsg(_(e_invalid_ಹುಡುಕು_ನಮೂನೆ_or_delimiter));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    vgr_init_regmatch(&ವಾದಗಳು->regmatch, ವಾದಗಳು->spat);
    ಆದರೆ (ವಾದಗಳು->regmatch.regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    p = skipwhite(p);
    ಆದರೆ (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_ನಮೂನೆ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    ಆದರೆ ((get_arglist_exp(p, &ವಾದಗಳು->fcount, &ವಾದಗಳು->fnames, TRUE) == FAIL) ||
	ವಾದಗಳು->fcount == 0)
    {
	emsg(_(e_no_ಹೊಂದಿಕೆ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
existing_swapfile(buf_T *buf)
{
    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ && buf->b_ml.ml_mfp->mf_fname != ಖಾಲಿ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	ಹಿಂತಿರುಗಿ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_ವಾದಗಳು_T	*cmd_ವಾದಗಳು,
	ಪೂರ್ಣಾಂಕ		*redraw_for_dummy,
	buf_T		**ಮೊದಲ_ಹೊಂದಿಕೆ_buf,
	char_u		**ಗುರಿ_dir)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    ಪೂರ್ಣಾಂಕ		fi;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		duplicate_name = FALSE;
    ಪೂರ್ಣಾಂಕ		using_dummy;
    char_u	*dirname_ಆರಂಭ = ಖಾಲಿ;
    char_u	*dirname_now = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ;
    aco_save_T	aco;

    dirname_ಆರಂಭ = alloc_id(MAXPATHL, aid_qf_dirname_ಆರಂಭ);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    ಆದರೆ (dirname_ಆರಂಭ == ಖಾಲಿ || dirname_now == ಖಾಲಿ)
	ಹೋಗು theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_ಆರಂಭ, MAXPATHL);

    seconds = (time_t)0;
    ಗಾಗಿ (fi = 0; fi < cmd_ವಾದಗಳು->fcount && !got_int && cmd_ವಾದಗಳು->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_ವಾದಗಳು->fnames[fi]);
	ಆದರೆ (ಸಮಯ(ಖಾಲಿ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = ಸಮಯ(ಖಾಲಿ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_ವಾದಗಳು->fnames[fi]);
	ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ಖಾಲಿ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_ತುಂಬು_dummy_buf(fname, dirname_ಆರಂಭ, dirname_now);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	ಆದರೆ (!vgr_qflist_valid(wp, qi, save_qfid, cmd_ವಾದಗಳು->qf_title))
	    ಹೋಗು theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    ಆದರೆ (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ = vgr_ಹೊಂದಿಕೆ_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_ವಾದಗಳು->spat, &cmd_ವಾದಗಳು->regmatch,
		    &cmd_ವಾದಗಳು->tomatch, duplicate_name, cmd_ವಾದಗಳು->flags);

	    ಆದರೆ (using_dummy)
	    {
		ಆದರೆ (ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ && *ಮೊದಲ_ಹೊಂದಿಕೆ_buf == ಖಾಲಿ)
		    *ಮೊದಲ_ಹೊಂದಿಕೆ_buf = buf;
		ಆದರೆ (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_ಬಫರ್(buf, dirname_ಆರಂಭ);
		    buf = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    ಆದರೆ (!ಸಿಕ್ಕಿತು_ಹೊಂದಿಕೆ)
		    {
			wipe_dummy_ಬಫರ್(buf, dirname_ಆರಂಭ);
			buf = ಖಾಲಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf != *ಮೊದಲ_ಹೊಂದಿಕೆ_buf
					|| (cmd_ವಾದಗಳು->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_ಬಫರ್(buf, dirname_ಆರಂಭ);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ಖಾಲಿ;
		    }
		}

		ಆದರೆ (buf != ಖಾಲಿ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    ಆದರೆ (buf == *ಮೊದಲ_ಹೊಂದಿಕೆ_buf
			    && *ಗುರಿ_dir == ಖಾಲಿ
			    && STRCMP(dirname_ಆರಂಭ, dirname_now) != 0)
			*ಗುರಿ_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    ಆದರೆ (curbuf == buf)
		    {
# ಆದರೆ defined(FEAT_SYN_HL)
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

    ಸ್ಥಿತಿ_2 = OK;

theend:
    vim_free(dirname_now);
    vim_free(dirname_ಆರಂಭ);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
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
    vgr_ವಾದಗಳು_T	ವಾದಗಳು;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		redraw_for_dummy = FALSE;
    buf_T	*ಮೊದಲ_ಹೊಂದಿಕೆ_buf = ಖಾಲಿ;
    char_u	*ಗುರಿ_dir = ಖಾಲಿ;
    char_u	*au_name =  ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;

    ಆದರೆ (!check_can_ಗಣ_curbuf_forceit(eap->forceit))
	ಹಿಂತಿರುಗಿ;

    au_name = vgr_get_auname(eap->cmdidx);
    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ;
# endif
    }

    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (vgr_process_ವಾದಗಳು(eap, &ವಾದಗಳು) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
	// make place for a new list
	qf_new_list(qi, ವಾದಗಳು.qf_title);

    incr_quickfix_busy();

    ಸ್ಥಿತಿ_2 = vgr_process_files(wp, qi, &ವಾದಗಳು, &redraw_for_dummy,
						&ಮೊದಲ_ಹೊಂದಿಕೆ_buf, &ಗುರಿ_dir);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != OK)
    {
	FreeWild(ವಾದಗಳು.fcount, ವಾದಗಳು.fnames);
	decr_quickfix_busy();
	ಹೋಗು theend;
    }

    FreeWild(ವಾದಗಳು.fcount, ವಾದಗಳು.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_ಆರಂಭ;
    qfl->qf_ಸೂಚ್ಯಂಕ = 1;
    qf_list_ಬದಲಾಯಿತು(qfl);

    qf_update_ಬಫರ್(qi, ಖಾಲಿ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    ಆದರೆ (au_name != ಖಾಲಿ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    ಆದರೆ (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	ಹೋಗು theend;
    }

    // Jump to first match.
    ಆದರೆ (!qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
    {
	ಆದರೆ ((ವಾದಗಳು.flags & VGR_NOJUMP) == 0)
	    vgr_jump_ಗಮ್ಯ_ಹೊಂದಿಕೆ(qi, eap->forceit, &redraw_for_dummy,
		    ಮೊದಲ_ಹೊಂದಿಕೆ_buf, ಗುರಿ_dir);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_no_ಹೊಂದಿಕೆ_str_2), ವಾದಗಳು.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    ಆದರೆ (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# ಇಲ್ಲದಿದ್ದರೆ
	redraw_later(UPD_NOT_VALID);
# endif
    }

theend:
    vim_free(ವಾದಗಳು.qf_title);
    vim_free(ಗುರಿ_dir);
    vim_regfree(ವಾದಗಳು.regmatch.regprog);
}

/*
 * Restore current working directory to "dirname_start" if they differ, taking
 * into account whether it is set locally or globally.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_ಆರಂಭ_dir(char_u *dirname_ಆರಂಭ)
{
    char_u *dirname_now = alloc(MAXPATHL);

    ಆದರೆ (dirname_now == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    mch_dirname(dirname_now, MAXPATHL);
    ಆದರೆ (STRCMP(dirname_ಆರಂಭ, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_ಕ್ಷೇತ್ರ(ea);
	ea.arg = dirname_ಆರಂಭ;
	ea.cmdidx = (curwin->w_localdir == ಖಾಲಿ) ? CMD_cd : CMD_lcd;
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
ತುಂಬು_dummy_ಬಫರ್(
    char_u	*fname,
    char_u	*dirname_ಆರಂಭ,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_ಗಮ್ಯ_wipe;
    ಪೂರ್ಣಾಂಕ		failed = TRUE;
    aco_save_T	aco;
    ಪೂರ್ಣಾಂಕ		readfile_ಫಲಿತಾಂಶ;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, (linenr_T)1, BLN_DUMMY);
    ಆದರೆ (newbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಗಣ_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    ಆದರೆ (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	ಆದರೆ (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (ಶೂನ್ಯ)setfname(curbuf, fname, ಖಾಲಿ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_ಅದಲುಬದಲು(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_ಗಮ್ಯ_wipe.br_buf = ಖಾಲಿ;
	    readfile_ಫಲಿತಾಂಶ = readfile(fname, ಖಾಲಿ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ಖಾಲಿ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    ಆದರೆ (readfile_ಫಲಿತಾಂಶ == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		ಆದರೆ (curbuf != newbuf)
		{
		    // Bloody autocommands changed the buffer!  Can happen when
		    // using netrw and editing a remote file.  Use the current
		    // buffer instead, delete the dummy one after restoring the
		    // window stuff.
		    ಗಣ_bufref(&newbuf_ಗಮ್ಯ_wipe, newbuf);
		    newbuf = curbuf;
		}
	    }

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);

	    ಆದರೆ (newbuf_ಗಮ್ಯ_wipe.br_buf != ಖಾಲಿ && bufref_valid(&newbuf_ಗಮ್ಯ_wipe))
	    {
		ಖಂಡ_autocmds();
		wipe_dummy_ಬಫರ್(newbuf_ಗಮ್ಯ_wipe.br_buf, ಖಾಲಿ);
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
    restore_ಆರಂಭ_dir(dirname_ಆರಂಭ);

    ಆದರೆ (!bufref_valid(&newbufref))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (failed)
    {
	wipe_dummy_ಬಫರ್(newbuf, dirname_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wipe_dummy_ಬಫರ್(buf_T *buf, char_u *dirname_ಆರಂಭ)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    ಆಗ (buf->b_nwindows > 0)
    {
	ಪೂರ್ಣಾಂಕ	    did_one = FALSE;
	win_T	    *wp;

	ಆದರೆ (firstwin->w_ಮುಂದಿನ != ಖಾಲಿ)
	    FOR_ALL_WINDOWS(wp)
		ಆದರೆ (wp->w_ಬಫರ್ == buf)
		{
		    ಆದರೆ (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	ಆದರೆ (!did_one)
	    ಹೋಗು fail;
    }

    ಆದರೆ (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# ಆದರೆ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_ಬಫರ್(buf, TRUE);

# ಆದರೆ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	ಆದರೆ (dirname_ಆರಂಭ != ಖಾಲಿ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_ಆರಂಭ_dir(dirname_ಆರಂಭ);

	ಹಿಂತಿರುಗಿ;
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
unload_dummy_ಬಫರ್(buf_T *buf, char_u *dirname_ಆರಂಭ)
{
    ಆದರೆ (curbuf == buf)		// safety check
	ಹಿಂತಿರುಗಿ;

    close_ಬಫರ್(ಖಾಲಿ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_ಆರಂಭ_dir(dirname_ಆರಂಭ);
}

# ಆದರೆ defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_qfline_ವಸ್ತುಗಳು(qfline_T *qfp, list_T *list)
{
    ಪೂರ್ಣಾಂಕ		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    ಆದರೆ (bufnum != 0 && (buflist_findnr(bufnum) == ಖಾಲಿ))
	bufnum = 0;

    ಆದರೆ ((dict = dict_alloc()) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    ಆದರೆ (dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "bufnr", (ದೀರ್ಘ)bufnum) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum",     (ದೀರ್ಘ)qfp->qf_lnum) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "end_lnum", (ದೀರ್ಘ)qfp->qf_ಕೊನೆ_lnum) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col",      (ದೀರ್ಘ)qfp->qf_ಲಂಬಸಾಲು) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "end_col",  (ದೀರ್ಘ)qfp->qf_ಕೊನೆ_ಲಂಬಸಾಲು) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "vcol",     (ದೀರ್ಘ)qfp->qf_viscol) == FAIL
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "nr",       (ದೀರ್ಘ)qfp->qf_nr) == FAIL
	    || dict_ಕೂಡಿಸು_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_ಕೂಡಿಸು_string(dict, "pattern", qfp->qf_ನಮೂನೆ) == FAIL
	    || dict_ಕೂಡಿಸು_string(dict, "text", qfp->qf_ಪಠ್ಯ) == FAIL
	    || dict_ಕೂಡಿಸು_string_len(dict, "type", buf, (ಪೂರ್ಣಾಂಕ)buflen) == FAIL
	    || (qfp->qf_user_ದತ್ತಾಂಶ.v_type != VAR_UNKNOWN
		&& dict_ಕೂಡಿಸು_tv(dict, "user_data", &qfp->qf_user_ದತ್ತಾಂಶ) == FAIL )
	    || dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "valid", (ದೀರ್ಘ)qfp->qf_valid) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	ಪೂರ್ಣಾಂಕ		qf_idx,
	ಪೂರ್ಣಾಂಕ		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (qi == ಖಾಲಿ)
    {
	qi = ql_info;
	ಆದರೆ (wp != ಖಾಲಿ)
	    qi = GET_LOC_LIST(wp);
	ಆದರೆ (qi == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

    }

    ಆದರೆ (eidx < 0)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    ಆದರೆ (qf_idx >= qi->qf_listcount)
	ಹಿಂತಿರುಗಿ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    ಆದರೆ (qf_list_ಖಾಲಿಗಳು(qfl))
	ಹಿಂತಿರುಗಿ FAIL;

    FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, i)
    {
	ಆದರೆ (eidx > 0)
	{
	    ಆದರೆ (eidx == i)
		ಹಿಂತಿರುಗಿ get_qfline_ವಸ್ತುಗಳು(qfp, list);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_qfline_ವಸ್ತುಗಳು(qfp, list) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
ಎಣಿಕೆ {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ವಸ್ತುಗಳು	= 0x2,
    QF_GETLIST_NR	= 0x4,
    QF_GETLIST_WINID	= 0x8,
    QF_GETLIST_CONTEXT	= 0x10,
    QF_GETLIST_ID	= 0x20,
    QF_GETLIST_IDX	= 0x40,
    QF_GETLIST_ಗಾತ್ರ	= 0x80,
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_list_from_ಸಾಲುಗಳು(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    ಆದರೆ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    ಆದರೆ ((efm_di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"efm", -1)) != ಖಾಲಿ)
    {
	ಆದರೆ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    qi = qf_alloc_ರಾಶಿ(QFLT_ಆಂತರಿಕ, 1);
    ಆದರೆ (qi != ಖಾಲಿ)
    {
	ಆದರೆ (qf_init_ext(qi, 0, ಖಾಲಿ, ಖಾಲಿ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ಖಾಲಿ, ಖಾಲಿ) > 0)
	{
	    (ಶೂನ್ಯ)get_errorlist(qi, ಖಾಲಿ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_ಕೂಡಿಸು_list(retdict, "items", l);
    ಸ್ಥಿತಿ_2 = OK;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    win = qf_ಕಂಡುಹಿಡಿ_win(qi);
    ಆದರೆ (win != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ win->w_id;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ	bufnum = 0;

    ಆದರೆ (qi != ಖಾಲಿ && buflist_findnr(qi->qf_bufnr) != ಖಾಲಿ)
	bufnum = qi->qf_bufnr;

    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_keys2flags(dict_T *what, ಪೂರ್ಣಾಂಕ loclist)
{
    ಪೂರ್ಣಾಂಕ		flags = QF_GETLIST_NONE;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	ಆದರೆ (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "title"))
	flags |= QF_GETLIST_TITLE;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "nr"))
	flags |= QF_GETLIST_NR;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "winid"))
	flags |= QF_GETLIST_WINID;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "id"))
	flags |= QF_GETLIST_ID;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "items"))
	flags |= QF_GETLIST_ವಸ್ತುಗಳು;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "idx"))
	flags |= QF_GETLIST_IDX;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "size"))
	flags |= QF_GETLIST_ಗಾತ್ರ;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    ಆದರೆ (loclist && dict_has_ಕೀಲಿ(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    ಆದರೆ (dict_has_ಕೀಲಿ(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    ಹಿಂತಿರುಗಿ flags;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    ಪೂರ್ಣಾಂಕ		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"nr", -1)) != ಖಾಲಿ)
    {
	// Use the specified quickfix/location list
	ಆದರೆ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // for zero use the current list
	    ಆದರೆ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
	    {
		qf_idx = di->di_tv.vval.v_ಸಂಖ್ಯೆ - 1;
		ಆದರೆ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ಖಾಲಿ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    qf_idx = INVALID_QFIDX;
    }

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"id", -1)) != ಖಾಲಿ)
    {
	// Look for a list with the specified id
	ಆದರೆ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    ಆದರೆ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_ಸಂಖ್ಯೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    qf_idx = INVALID_QFIDX;
    }

    ಹಿಂತಿರುಗಿ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_defaults(qf_info_T *qi, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ locstack, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;

    ಆದರೆ (flags & QF_GETLIST_TITLE)
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(retdict, "title", (char_u *)"", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ವಸ್ತುಗಳು))
    {
	list_T	*l = list_alloc();
	ಆದರೆ (l != ಖಾಲಿ)
	    ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_list(retdict, "items", l);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ_2 = FAIL;
    }
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_NR))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "nr", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_WINID))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "winid", qf_winid(qi));
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(retdict, "context", (char_u *)"", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ID))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "id", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_IDX))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "idx", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ಗಾತ್ರ))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "size", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_TICK))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "changedtick", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "filewinid", 0);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	ಸ್ಥಿತಿ_2 = qf_getprop_qfbufnr(qi, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_QFTF))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ winid = 0;

    ಆದರೆ (wp != ಖಾಲಿ && IS_LL_ಕಿಟಕಿ(wp))
    {
	win_T	*ll_wp = qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qi);
	ಆದರೆ (ll_wp != ಖಾಲಿ)
	    winid = ll_wp->w_id;
    }

    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_ವಸ್ತುಗಳು(qf_info_T *qi, ಪೂರ್ಣಾಂಕ qf_idx, ಪೂರ್ಣಾಂಕ eidx, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    list_T	*l = list_alloc();
    ಆದರೆ (l != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)get_errorlist(qi, ಖಾಲಿ, qf_idx, eidx, l);
	dict_ಕೂಡಿಸು_list(retdict, "items", l);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = FAIL;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    dictitem_T	*di;

    ಆದರೆ (qfl->qf_ctx != ಖಾಲಿ)
    {
	di = dictitem_alloc((char_u *)"context");
	ಆದರೆ (di != ಖಾಲಿ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು(retdict, di);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
		dictitem_free(di);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ_2 = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(retdict, "context", (char_u *)"", 0);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_idx(qf_list_T *qfl, ಪೂರ್ಣಾಂಕ eidx, dict_T *retdict)
{
    ಆದರೆ (eidx == 0)
    {
	eidx = qfl->qf_ಸೂಚ್ಯಂಕ;
	ಆದರೆ (qf_list_ಖಾಲಿಗಳು(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;

    ಆದರೆ (qfl->qf_qftf_cb.cb_name != ಖಾಲಿ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    ಪೂರ್ಣಾಂಕ		qf_idx = INVALID_QFIDX;
    ಪೂರ್ಣಾಂಕ		eidx = 0;
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		flags = QF_GETLIST_NONE;

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"lines", -1)) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ qf_get_list_from_ಸಾಲುಗಳು(what, di, retdict);

    ಆದರೆ (wp != ಖಾಲಿ)
	qi = GET_LOC_LIST(wp);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ಖಾಲಿ));

    ಆದರೆ (!qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi) || qf_idx == INVALID_QFIDX)
	ಹಿಂತಿರುಗಿ qf_getprop_defaults(qi, flags, wp != ಖಾಲಿ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"idx", -1)) != ಖಾಲಿ)
    {
	ಆದರೆ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಹಿಂತಿರುಗಿ FAIL;
	eidx = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
    }

    ಆದರೆ (flags & QF_GETLIST_TITLE)
	ಸ್ಥಿತಿ_2 = qf_getprop_title(qfl, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_NR))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "nr", qf_idx + 1);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_WINID))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "winid", qf_winid(qi));
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ವಸ್ತುಗಳು))
	ಸ್ಥಿತಿ_2 = qf_getprop_ವಸ್ತುಗಳು(qi, qf_idx, eidx, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_CONTEXT))
	ಸ್ಥಿತಿ_2 = qf_getprop_ctx(qfl, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ID))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "id", qfl->qf_id);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_IDX))
	ಸ್ಥಿತಿ_2 = qf_getprop_idx(qfl, eidx, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_ಗಾತ್ರ))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "size", qfl->qf_ಎಣಿಕೆ);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_TICK))
	ಸ್ಥಿತಿ_2 = dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(retdict, "changedtick", qfl->qf_changedtick);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (wp != ಖಾಲಿ) && (flags & QF_GETLIST_FILEWINID))
	ಸ್ಥಿತಿ_2 = qf_getprop_filewinid(wp, qi, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_QFBUFNR))
	ಸ್ಥಿತಿ_2 = qf_getprop_qfbufnr(qi, retdict);
    ಆದರೆ ((ಸ್ಥಿತಿ_2 == OK) && (flags & QF_GETLIST_QFTF))
	ಸ್ಥಿತಿ_2 = qf_getprop_qftf(qfl, retdict);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಕೂಡಿಸು_ನಮೂದು_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	ಪೂರ್ಣಾಂಕ		ಮೊದಲ_ನಮೂದು,
	ಪೂರ್ಣಾಂಕ		*valid_ನಮೂದು)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_bufnr_emsg;
    char_u	*filename, *module, *ನಮೂನೆ, *ಪಠ್ಯ, *type;
    ಪೂರ್ಣಾಂಕ		bufnum, valid, ಸ್ಥಿತಿ_2, ಲಂಬಸಾಲು, ಕೊನೆ_ಲಂಬಸಾಲು, vcol, nr;
    ದೀರ್ಘ	lnum, ಕೊನೆ_lnum;

    ಆದರೆ (ಮೊದಲ_ನಮೂದು)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "bufnr");
    lnum = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "lnum");
    ಕೊನೆ_lnum = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "end_lnum");
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "col");
    ಕೊನೆ_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "end_col");
    vcol = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "vcol");
    nr = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    ನಮೂನೆ = dict_get_string(d, "pattern", TRUE);
    ಪಠ್ಯ = dict_get_string(d, "text", TRUE);
    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
	ಪಠ್ಯ = vim_strsave((char_u *)"");
    typval_T user_ದತ್ತಾಂಶ;
    user_ದತ್ತಾಂಶ.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_ದತ್ತಾಂಶ);

    valid = TRUE;
    ಆದರೆ ((filename == ಖಾಲಿ && bufnum == 0) || (lnum == 0 && ನಮೂನೆ == ಖಾಲಿ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    ಆದರೆ (bufnum != 0 && (buflist_findnr(bufnum) == ಖಾಲಿ))
    {
	ಆದರೆ (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_ಬಫರ್_nr_not_ಸಿಕ್ಕಿತು), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    ಆದರೆ (dict_has_ಕೀಲಿ(d, "valid"))
	valid = (ಪೂರ್ಣಾಂಕ)dict_get_bool(d, "valid", FALSE);

    ಸ್ಥಿತಿ_2 =  qf_ಕೂಡಿಸು_ನಮೂದು(qfl,
			ಖಾಲಿ,		// dir
			filename,
			module,
			bufnum,
			ಪಠ್ಯ,
			lnum,
			ಕೊನೆ_lnum,
			ಲಂಬಸಾಲು,
			ಕೊನೆ_ಲಂಬಸಾಲು,
			vcol,		// vis_col
			ನಮೂನೆ,	// search pattern
			nr,
			type == ಖಾಲಿ ? NUL : *type,
			&user_ದತ್ತಾಂಶ,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(ನಮೂನೆ);
    vim_free(ಪಠ್ಯ);
    vim_free(type);
    clear_tv(&user_ದತ್ತಾಂಶ);

    ಆದರೆ (valid)
	*valid_ನಮೂದು = TRUE;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ನಮೂದು_is_closer_ಗಮ್ಯ_ಗುರಿ(
	qfline_T	*ನಮೂದು,
	qfline_T	*other_ನಮೂದು,
	ಪೂರ್ಣಾಂಕ		ಗುರಿ_fnum,
	ಪೂರ್ಣಾಂಕ		ಗುರಿ_lnum,
	ಪೂರ್ಣಾಂಕ		ಗುರಿ_ಲಂಬಸಾಲು)
{
    // First, compare entries to target file.
    ಆದರೆ (!ಗುರಿ_fnum)
	// Without a target file, we can't know which is closer.
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ is_ಗುರಿ_file = ನಮೂದು->qf_fnum && ನಮೂದು->qf_fnum == ಗುರಿ_fnum;
    ಪೂರ್ಣಾಂಕ other_is_ಗುರಿ_file = other_ನಮೂದು->qf_fnum && other_ನಮೂದು->qf_fnum == ಗುರಿ_fnum;
    ಆದರೆ (!is_ಗುರಿ_file && other_is_ಗುರಿ_file)
	ಹಿಂತಿರುಗಿ FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಗುರಿ_file && !other_is_ಗುರಿ_file)
	ಹಿಂತಿರುಗಿ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    ಆದರೆ (!ಗುರಿ_lnum)
	// Without a target line number, we can't know which is closer.
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ ಸಾಲು_ದೂರ = ನಮೂದು->qf_lnum ? labs(ನಮೂದು->qf_lnum - ಗುರಿ_lnum) : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಪೂರ್ಣಾಂಕ other_ಸಾಲು_ದೂರ = other_ನಮೂದು->qf_lnum ? labs(other_ನಮೂದು->qf_lnum - ಗುರಿ_lnum) : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಆದರೆ (ಸಾಲು_ದೂರ > other_ಸಾಲು_ದೂರ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು_ದೂರ < other_ಸಾಲು_ದೂರ)
	ಹಿಂತಿರುಗಿ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    ಆದರೆ (!ಗುರಿ_ಲಂಬಸಾಲು)
	// Without a target column, we can't know which is closer.
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ ಸ್ತಂಭ_ದೂರ = ನಮೂದು->qf_ಲಂಬಸಾಲು ? ನಿರಪೇಕ್ಷ(ನಮೂದು->qf_ಲಂಬಸಾಲು - ಗುರಿ_ಲಂಬಸಾಲು) : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಪೂರ್ಣಾಂಕ other_ಸ್ತಂಭ_ದೂರ = other_ನಮೂದು->qf_ಲಂಬಸಾಲು ? ನಿರಪೇಕ್ಷ(other_ನಮೂದು->qf_ಲಂಬಸಾಲು - ಗುರಿ_ಲಂಬಸಾಲು): ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    ಆದರೆ (ಸ್ತಂಭ_ದೂರ > other_ಸ್ತಂಭ_ದೂರ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ತಂಭ_ದೂರ < other_ಸ್ತಂಭ_ದೂರ)
	ಹಿಂತಿರುಗಿ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಕೂಡಿಸು_entries(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಂಕ		qf_idx,
	list_T		*list,
	char_u		*title,
	ಪೂರ್ಣಾಂಕ		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_ಕೊನೆಯ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ಪೂರ್ಣಾಂಕ		valid_ನಮೂದು = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    ಪೂರ್ಣಾಂಕ prev_fnum = 0;
    ಪೂರ್ಣಾಂಕ prev_lnum = 0;
    ಪೂರ್ಣಾಂಕ prev_ಲಂಬಸಾಲು = 0;
    ಆದರೆ (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_ಲಂಬಸಾಲು = qfl->qf_ptr->qf_ಲಂಬಸಾಲು;
    }

    ಪೂರ್ಣಾಂಕ select_ಮೊದಲ_ನಮೂದು = FALSE;
    ಪೂರ್ಣಾಂಕ select_ಹತ್ತಿರದ_ನಮೂದು = FALSE;

    ಆದರೆ (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_ಮೊದಲ_ನಮೂದು = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == 'a')
    {
	ಆದರೆ (qf_list_ಖಾಲಿಗಳು(qfl))
	    // Appending to empty list, select first entry.
	    select_ಮೊದಲ_ನಮೂದು = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    // Adding to existing list, use last entry.
	    old_ಕೊನೆಯ = qfl->qf_ಕೊನೆಯ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == 'r')
    {
	select_ಮೊದಲ_ನಮೂದು = TRUE;
	qf_free_ವಸ್ತುಗಳು(qfl);
	qf_store_title(qfl, title);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == 'u')
    {
	select_ಹತ್ತಿರದ_ನಮೂದು = TRUE;
	qf_free_ವಸ್ತುಗಳು(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *ನಮೂದು_ಗಮ್ಯ_select = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ ನಮೂದು_ಗಮ್ಯ_select_ಸೂಚ್ಯಂಕ = 0;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಆದರೆ (li->li_tv.v_type != VAR_DICT)
	    ಮುಂದುವರಿ; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	ಆದರೆ (d == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	retval = qf_ಕೂಡಿಸು_ನಮೂದು_from_dict(qfl, d, li == list->lv_ಮೊದಲ,
								&valid_ನಮೂದು);
	ಆದರೆ (retval == QF_FAIL)
	    ನಿಲ್ಲಿಸು;

	qfline_T *ನಮೂದು = qfl->qf_ಕೊನೆಯ;
	ಆದರೆ (
	    (select_ಮೊದಲ_ನಮೂದು && ನಮೂದು_ಗಮ್ಯ_select == ಖಾಲಿ) ||
	    (select_ಹತ್ತಿರದ_ನಮೂದು &&
		(ನಮೂದು_ಗಮ್ಯ_select == ಖಾಲಿ ||
		 ನಮೂದು_is_closer_ಗಮ್ಯ_ಗುರಿ(ನಮೂದು, ನಮೂದು_ಗಮ್ಯ_select, prev_fnum,
					   prev_lnum, prev_ಲಂಬಸಾಲು))))
	{
	    ನಮೂದು_ಗಮ್ಯ_select = ನಮೂದು;
	    ನಮೂದು_ಗಮ್ಯ_select_ಸೂಚ್ಯಂಕ = qfl->qf_ಎಣಿಕೆ;
	}
    }

    // Check if any valid error entries are added to the list.
    ಆದರೆ (valid_ನಮೂದು)
	qfl->qf_nonevalid = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qfl->qf_ಸೂಚ್ಯಂಕ == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    ಆದರೆ (ನಮೂದು_ಗಮ್ಯ_select)
    {
	qfl->qf_ptr = ನಮೂದು_ಗಮ್ಯ_select;
	qfl->qf_ಸೂಚ್ಯಂಕ = ನಮೂದು_ಗಮ್ಯ_select_ಸೂಚ್ಯಂಕ;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_ಬಫರ್(qi, old_ಕೊನೆಯ);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	ಪೂರ್ಣಾಂಕ		action,
	ಪೂರ್ಣಾಂಕ		*newlist)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		qf_idx = qi->qf_curlist;    // default is the current list

    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"nr", -1)) != ಖಾಲಿ)
    {
	// Use the specified quickfix/location list
	ಆದರೆ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    // for zero use the current list
	    ಆದರೆ (di->di_tv.vval.v_ಸಂಖ್ಯೆ != 0)
		qf_idx = di->di_tv.vval.v_ಸಂಖ್ಯೆ - 1;

	    ಆದರೆ ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		ಹಿಂತಿರುಗಿ INVALID_QFIDX;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ಖಾಲಿ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    ಆದರೆ (!qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
		qf_idx = qi->qf_listcount - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*newlist)
		qf_idx = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ INVALID_QFIDX;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ INVALID_QFIDX;
    }

    ಆದರೆ (!*newlist && (di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"id", -1)) != ಖಾಲಿ)
    {
	// Use the quickfix/location list with the specified id
	ಆದರೆ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
	    ಹಿಂತಿರುಗಿ INVALID_QFIDX;

	ಹಿಂತಿರುಗಿ qf_id2nr(qi, di->di_tv.vval.v_ಸಂಖ್ಯೆ);
    }

    ಹಿಂತಿರುಗಿ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_title(qf_info_T *qi, ಪೂರ್ಣಾಂಕ qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    ಆದರೆ (di->di_tv.v_type != VAR_STRING)
	ಹಿಂತಿರುಗಿ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    ಆದರೆ (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set quickfix list items/entries.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_ವಸ್ತುಗಳು(qf_info_T *qi, ಪೂರ್ಣಾಂಕ qf_idx, dictitem_T *di, ಪೂರ್ಣಾಂಕ action)
{
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    char_u	*title_save;

    ಆದರೆ (di->di_tv.v_type != VAR_LIST)
	ಹಿಂತಿರುಗಿ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_ಕೂಡಿಸು_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_ವಸ್ತುಗಳು_from_ಸಾಲುಗಳು(
	qf_info_T	*qi,
	ಪೂರ್ಣಾಂಕ		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	ಪೂರ್ಣಾಂಕ		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    ಆದರೆ ((efm_di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"efm", -1)) != ಖಾಲಿ)
    {
	ಆದರೆ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    ಆದರೆ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (action == 'r' || action == 'u')
	qf_free_ವಸ್ತುಗಳು(&qi->qf_lists[qf_idx]);
    ಆದರೆ (qf_init_ext(qi, qf_idx, ಖಾಲಿ, ಖಾಲಿ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ಖಾಲಿ, ಖಾಲಿ) >= 0)
	retval = OK;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Set quickfix list context.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    ಆದರೆ (ctx != ಖಾಲಿ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    ಪೂರ್ಣಾಂಕ		denote = FALSE;
    ಪೂರ್ಣಾಂಕ		newidx;
    ಪೂರ್ಣಾಂಕ		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    ಆದರೆ (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ಖಾಲಿ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_ಎಣಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Otherwise use the specified index
	newidx = tv_get_ಸಂಖ್ಯೆ_chk(&di->di_tv, &denote);
	ಆದರೆ (denote)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (newidx < 1)		// sanity check
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (newidx > qfl->qf_ಎಣಿಕೆ)
	newidx = qfl->qf_ಎಣಿಕೆ;

    old_qfidx = qfl->qf_ಸೂಚ್ಯಂಕ;
    qf_ptr = get_nth_ನಮೂದು(qfl, newidx, &newidx);
    ಆದರೆ (qf_ptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_ಸೂಚ್ಯಂಕ = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    ಆದರೆ (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_setprop_qftf(qf_info_T *qi ಬಳಸದ, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    ಆದರೆ (cb.cb_name == ಖಾಲಿ || *cb.cb_name == NUL)
	ಹಿಂತಿರುಗಿ OK;
    ಗಣ_callback(&qfl->qf_qftf_cb, &cb);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
qf_ಗಣ_properties(qf_info_T *qi, dict_T *what, ಪೂರ್ಣಾಂಕ action, char_u *title)
{
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ಪೂರ್ಣಾಂಕ		qf_idx;
    ಪೂರ್ಣಾಂಕ		newlist = FALSE;
    qf_list_T	*qfl;

    ಆದರೆ (action == ' ' || qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    ಆದರೆ (qf_idx == INVALID_QFIDX)	// List not found
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"title", -1)) != ಖಾಲಿ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"items", -1)) != ಖಾಲಿ)
	retval = qf_setprop_ವಸ್ತುಗಳು(qi, qf_idx, di, action);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"lines", -1)) != ಖಾಲಿ)
	retval = qf_setprop_ವಸ್ತುಗಳು_from_ಸಾಲುಗಳು(qi, qf_idx, what, di, action);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"context", -1)) != ಖಾಲಿ)
	retval = qf_setprop_context(qfl, di);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"idx", -1)) != ಖಾಲಿ)
	retval = qf_setprop_curidx(qi, qfl, di);
    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(what, (char_u *)"quickfixtextfunc", -1)) != ಖಾಲಿ)
	retval = qf_setprop_qftf(qi, qfl, di);

    ಆದರೆ (newlist || retval == OK)
	qf_list_ಬದಲಾಯಿತು(qfl);
    ಆದರೆ (newlist)
	qf_update_ಬಫರ್(qi, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
qf_free_ರಾಶಿ(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_ಕಂಡುಹಿಡಿ_win(qi);
    win_T	*llwin = ಖಾಲಿ;

    ಆದರೆ (qfwin != ಖಾಲಿ)
    {
	// If the quickfix/location list window is open, then clear it
	ಆದರೆ (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_ಬಫರ್(qi, ಖಾಲಿ);
    }

    ಆದರೆ (wp != ಖಾಲಿ && IS_LL_ಕಿಟಕಿ(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qi);
	ಆದರೆ (llwin != ಖಾಲಿ)
	    wp = llwin;
    }

    qf_free_all(wp);
    ಆದರೆ (wp == ಖಾಲಿ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qfwin != ಖಾಲಿ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_ರಾಶಿ(QFLT_LOCATION, wp->w_p_lhi);

	ಆದರೆ (new_ll != ಖಾಲಿ)
	{
	    new_ll->qf_bufnr = qfwin->w_ಬಫರ್->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    ಆದರೆ (wp != qfwin)
		win_ಗಣ_loclist(wp, new_ll);
	}
    }
}

/*
 * Populate the quickfix list with the items supplied in the list
 * of dictionaries. "title" will be copied to w:quickfix_title.
 * "action" is 'a' for add, 'r' for replace, 'u' for update.  Otherwise
 * create a new list. When "what" is not NULL then only set some properties.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_errorlist(
	win_T	*wp,
	list_T	*list,
	ಪೂರ್ಣಾಂಕ	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    ಪೂರ್ಣಾಂಕ		retval = OK;

    ಆದರೆ (wp != ಖಾಲಿ)
	qi = ll_get_or_alloc_list(wp);
    ಇಲ್ಲದಿದ್ದರೆ
	qi = ql_info;
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_ರಾಶಿ(wp, qi);
	ಹಿಂತಿರುಗಿ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    ಆದರೆ (list->lv_len != 0 && what != ಖಾಲಿ)
    {
	semsg(_(e_invalid_ವಾದ_str),
			 _("cannot have both a list and a \"what\" argument"));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    incr_quickfix_busy();

    ಆದರೆ (what != ಖಾಲಿ)
	retval = qf_ಗಣ_properties(qi, what, action, title);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = qf_ಕೂಡಿಸು_entries(qi, qi->qf_curlist, list, title, action);
	ಆದರೆ (retval == OK)
	    qf_list_ಬದಲಾಯಿತು(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    ಹಿಂತಿರುಗಿ retval;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mark_quickfix_user_ದತ್ತಾಂಶ(qf_info_T *qi, ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	ಆದರೆ (!qfl->qf_has_user_ದತ್ತಾಂಶ)
	    ಮುಂದುವರಿ;
	qfline_T *qfp;
	ಪೂರ್ಣಾಂಕ j;
	FOR_ALL_QFL_ವಸ್ತುಗಳು(qfl, qfp, j)
	{
	    typval_T* user_ದತ್ತಾಂಶ = &qfp->qf_user_ದತ್ತಾಂಶ;
	    ಆದರೆ (user_ದತ್ತಾಂಶ != ಖಾಲಿ && user_ದತ್ತಾಂಶ->v_type != VAR_ಸಂಖ್ಯೆ
		&& user_ದತ್ತಾಂಶ->v_type != VAR_STRING && user_ದತ್ತಾಂಶ->v_type != VAR_FLOAT)
		abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(user_ದತ್ತಾಂಶ, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    }
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mark_quickfix_ctx(qf_info_T *qi, ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    ಗಾಗಿ (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	ಆದರೆ (ctx != ಖಾಲಿ && ctx->v_type != VAR_ಸಂಖ್ಯೆ
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(ctx, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || ಗಣ_ref_ಒಳ_callback(cb, copyID);
    }

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_quickfix(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    ಆದರೆ (ql_info == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    ಆದರೆ (abort)
	ಹಿಂತಿರುಗಿ abort;

    abort = mark_quickfix_user_ದತ್ತಾಂಶ(ql_info, copyID);
    ಆದರೆ (abort)
	ಹಿಂತಿರುಗಿ abort;

    abort = ಗಣ_ref_ಒಳ_callback(&qftf_cb, copyID);
    ಆದರೆ (abort)
	ಹಿಂತಿರುಗಿ abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	ಆದರೆ (win->w_llist != ಖಾಲಿ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    ಆದರೆ (abort)
		ಹಿಂತಿರುಗಿ abort;

	    abort = mark_quickfix_user_ದತ್ತಾಂಶ(win->w_llist, copyID);
	    ಆದರೆ (abort)
		ಹಿಂತಿರುಗಿ abort;
	}
	ಆದರೆ (IS_LL_ಕಿಟಕಿ(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    ಆದರೆ (abort)
		ಹಿಂತಿರುಗಿ abort;

	    abort = mark_quickfix_user_ದತ್ತಾಂಶ(win->w_llist_ref, copyID);
	    ಆದರೆ (abort)
		ಹಿಂತಿರುಗಿ abort;
	}
    }

    ಹಿಂತಿರುಗಿ abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    ಸ್ಥಿರ char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_cbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"cbuffer";
	ಸ್ಥಿತಿ CMD_cgetbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"cgetbuffer";
	ಸ್ಥಿತಿ CMD_caddbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"caddbuffer";
	ಸ್ಥಿತಿ CMD_lbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"lbuffer";
	ಸ್ಥಿತಿ CMD_lgetbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"lgetbuffer";
	ಸ್ಥಿತಿ CMD_laddbuffer:	ಹಿಂತಿರುಗಿ (char_u *)"laddbuffer";
	ಪೂರ್ವನಿಯೋಜಿತ:		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cbuffer_process_ವಾದಗಳು(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ಖಾಲಿ;

    ಆದರೆ (*eap->arg == NUL)
	buf = curbuf;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((ಅಕ್ಷರ *)eap->arg));

    ಆದರೆ (buf == ಖಾಲಿ)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	emsg(_(e_ಬಫರ್_is_not_loaded));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    }

    ಆದರೆ (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    ಹಿಂತಿರುಗಿ OK;
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
    buf_T	*buf = ಖಾಲಿ;
    qf_info_T	*qi;
    char_u	*au_name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res;
    int_u	save_qfid;
    win_T	*wp = ಖಾಲಿ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cbuffer_process_ವಾದಗಳು(eap, &buf, &line1, &line2) == FAIL)
	ಹಿಂತಿರುಗಿ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    ಆದರೆ (buf->b_sfname)
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%s (%s)",
		(ಅಕ್ಷರ *)qf_title, (ಅಕ್ಷರ *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ಖಾಲಿ, buf, ಖಾಲಿ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ಖಾಲಿ);
    ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
    {
	decr_quickfix_busy();
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (res >= 0)
	qf_list_ಬದಲಾಯಿತು(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    ಆದರೆ (au_name != ಖಾಲಿ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	ಆದರೆ (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    ಆದರೆ (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_ಮೊದಲ(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_cexpr:	    ಹಿಂತಿರುಗಿ (char_u *)"cexpr";
	ಸ್ಥಿತಿ CMD_cgetexpr:  ಹಿಂತಿರುಗಿ (char_u *)"cgetexpr";
	ಸ್ಥಿತಿ CMD_caddexpr:  ಹಿಂತಿರುಗಿ (char_u *)"caddexpr";
	ಸ್ಥಿತಿ CMD_lexpr:	    ಹಿಂತಿರುಗಿ (char_u *)"lexpr";
	ಸ್ಥಿತಿ CMD_lgetexpr:  ಹಿಂತಿರುಗಿ (char_u *)"lgetexpr";
	ಸ್ಥಿತಿ CMD_laddexpr:  ಹಿಂತಿರುಗಿ (char_u *)"laddexpr";
	ಪೂರ್ವನಿಯೋಜಿತ:	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಪೂರ್ಣಾಂಕ
trigger_cexpr_autocmd(ಪೂರ್ಣಾಂಕ cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ಖಾಲಿ;

    qi = qf_cmd_get_or_alloc_ರಾಶಿ(eap, &wp);
    ಆದರೆ (qi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ ((tv->v_type == VAR_STRING && tv->vval.v_string != ಖಾಲಿ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ಖಾಲಿ))
    {
	ಪೂರ್ಣಾಂಕ	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ಖಾಲಿ, ಖಾಲಿ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ಖಾಲಿ);
	ಆದರೆ (qf_ರಾಶಿ_ಖಾಲಿಗಳು(qi))
	{
	    decr_quickfix_busy();
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (res >= 0)
	    qf_list_ಬದಲಾಯಿತು(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	ಆದರೆ (au_name != ಖಾಲಿ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	ಆದರೆ (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_ಮೊದಲ(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	ಹಿಂತಿರುಗಿ OK;
    }

    emsg(_(e_string_or_list_expected));
    ಹಿಂತಿರುಗಿ FAIL;
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

    ಆದರೆ (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ(eap->arg, eap);
    ಆದರೆ (tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    (ಶೂನ್ಯ)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    ಸ್ಥಿರ qf_info_T *
hgr_get_ll(ಪೂರ್ಣಾಂಕ *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    ಆದರೆ (bt_help(curwin->w_ಬಫರ್))
	wp = curwin;
    ಇಲ್ಲದಿದ್ದರೆ
	// Find an existing help window
	wp = qf_ಕಂಡುಹಿಡಿ_help_win();

    ಆದರೆ (wp == ಖಾಲಿ)	    // Help window not found
	qi = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	qi = wp->w_llist;

    ಆದರೆ (qi == ಖಾಲಿ)
    {
	// Allocate a new location list for help text matches
	ಆದರೆ ((qi = qf_alloc_ರಾಶಿ(QFLT_LOCATION, 1)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*new_ll = TRUE;
    }

    ಹಿಂತಿರುಗಿ qi;
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
    ದೀರ್ಘ	lnum;

    fd = mch_fopen((ಅಕ್ಷರ *)fname, "r");
    ಆದರೆ (fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    lnum = 1;
    ಆಗ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *ಸಾಲು = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	ಆದರೆ (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    ಸಾಲು = string_convert(p_vc, IObuff, ಖಾಲಿ);
	    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
		ಸಾಲು = IObuff;
	}

	ಆದರೆ (vim_regexec(p_regmatch, ಸಾಲು, (colnr_T)0))
	{
	    ಪೂರ್ಣಾಂಕ	l = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು);

	    // remove trailing CR, LF, spaces, etc.
	    ಆಗ (l > 0 && ಸಾಲು[l - 1] <= ' ')
		ಸಾಲು[--l] = NUL;

	    ಆದರೆ (qf_ಕೂಡಿಸು_ನಮೂದು(qfl,
			ಖಾಲಿ,	// dir
			fname,
			ಖಾಲಿ,
			0,
			ಸಾಲು,
			lnum,
			0,
			(ಪೂರ್ಣಾಂಕ)(p_regmatch->startp[0] - ಸಾಲು)
			+ 1,	//ಲಂಬಸಾಲು
			(ಪೂರ್ಣಾಂಕ)(p_regmatch->endp[0] - ಸಾಲು)
			+ 1,	// end_col
			FALSE,	// vis_col
			ಖಾಲಿ,	// search pattern
			0,	// nr
			1,	// type
			ಖಾಲಿ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		ಆದರೆ (ಸಾಲು != IObuff)
		    vim_free(ಸಾಲು);
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ಸಾಲು != IObuff)
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
hgr_ಹುಡುಕು_files_ಒಳ_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    ಪೂರ್ಣಾಂಕ		fcount;
    char_u	**fnames;
    ಪೂರ್ಣಾಂಕ		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    ಕೂಡಿಸು_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    ಆದರೆ (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	ಗಾಗಿ (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    ಆದರೆ (lang != ಖಾಲಿ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		ಮುಂದುವರಿ;
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
hgr_ಹುಡುಕು_ಒಳ_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    ಆದರೆ (!enc_utf8)
	convert_ಸಿದ್ಧತೆ(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    ಆಗ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_ಹುಡುಕು_files_ಒಳ_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    ಆದರೆ (vc.vc_type != CONV_NONE)
	convert_ಸಿದ್ಧತೆ(&vc, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * ":helpgrep {pattern}"
 */
    ಶೂನ್ಯ
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    ಪೂರ್ಣಾಂಕ		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    ಪೂರ್ಣಾಂಕ		new_qi = FALSE;
    char_u	*au_name =  ಖಾಲಿ;
    char_u	*lang = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		updated = FALSE;

    ಆಯ್ಕೆ (eap->cmdidx)
    {
	ಸ್ಥಿತಿ CMD_helpgrep:  au_name = (char_u *)"helpgrep"; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (au_name != ಖಾಲಿ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	ಆದರೆ (aborting())
	    ಹಿಂತಿರುಗಿ;
# endif
    }

    ಆದರೆ (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	ಆದರೆ (qi == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (qi == ಖಾಲಿ)
    {
	emsg(_(e_no_quickfix_ರಾಶಿ));
	ಹಿಂತಿರುಗಿ;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = ಖಾಲಿಗಳು_option;

    incr_quickfix_busy();

# ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
# endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_ಹುಡುಕು_ಒಳ_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_ಆರಂಭ;
	qfl->qf_ಸೂಚ್ಯಂಕ = 1;
	qf_list_ಬದಲಾಯಿತು(qfl);
	updated = TRUE;
    }

    ಆದರೆ (p_cpo == ಖಾಲಿಗಳು_option)
	p_cpo = save_cpo;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	ಆದರೆ (*p_cpo == NUL)
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	ಆದರೆ (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    ಆದರೆ (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_ಬಫರ್(qi, ಖಾಲಿ);

    ಆದರೆ (au_name != ಖಾಲಿ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	ಆದರೆ (!new_qi && IS_LL_ರಾಶಿ(qi) && qf_ಕಂಡುಹಿಡಿ_win_with_loclist(qi) == ಖಾಲಿ)
	{
	    decr_quickfix_busy();
	    ಹಿಂತಿರುಗಿ;
	}
    }

    // Jump to first match.
    ಆದರೆ (!qf_list_ಖಾಲಿಗಳು(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_no_ಹೊಂದಿಕೆ_str_2), eap->arg);

    decr_quickfix_busy();

    ಆದರೆ (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	ಆದರೆ (!bt_help(curwin->w_ಬಫರ್) || curwin->w_llist == qi)
	{
	    ಆದರೆ (new_qi)
		ll_free_all(&qi);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_llist == ಖಾಲಿ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_quickfix(ಶೂನ್ಯ)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ಖಾಲಿ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#ಆದರೆ_ಅಂತ್ಯ // FEAT_QUICKFIX

#ಆದರೆ defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    ಸ್ಥಿರ ಶೂನ್ಯ
get_qf_loc_list(ಪೂರ್ಣಾಂಕ is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    ಆದರೆ (what_arg->v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (rettv_list_alloc(rettv) == OK)
	    ಆದರೆ (is_qf || wp != ಖಾಲಿ)
		(ಶೂನ್ಯ)get_errorlist(ಖಾಲಿ, wp, -1, 0, rettv->vval.v_list);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (rettv_dict_alloc(rettv) == OK)
	    ಆದರೆ (is_qf || (wp != ಖಾಲಿ))
	    {
		ಆದರೆ (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    ಆದರೆ (d != ಖಾಲಿ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    ಶೂನ್ಯ
f_getloclist(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    ಶೂನ್ಯ
f_getqflist(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_QUICKFIX
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    get_qf_loc_list(TRUE, ಖಾಲಿ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_qf_ll_list(
    win_T	*wp ಬಳಸದ,
    typval_T	*list_arg ಬಳಸದ,
    typval_T	*action_arg ಬಳಸದ,
    typval_T	*what_arg ಬಳಸದ,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    ಪೂರ್ಣಾಂಕ		action = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;
# endif

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

# ifdef FEAT_QUICKFIX
    ಆದರೆ (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	valid_dict = TRUE;

	ಆದರೆ (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    ಆದರೆ (act == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
	    ಆದರೆ ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_action_str_1), act);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_string_required));

	ಆದರೆ (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    ಆದರೆ (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ಖಾಲಿ)
		what = what_arg->vval.v_dict;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	ಆದರೆ (l != ಖಾಲಿ && action && valid_dict
		    && ಗಣ_errorlist(wp, l, action,
		     (char_u *)(wp == ಖಾಲಿ ? ":setqflist()" : ":setloclist()"),
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

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    win = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (win != ಖಾಲಿ)
	ಗಣ_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    ಶೂನ್ಯ
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಗಣ_qf_ll_list(ಖಾಲಿ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#ಆದರೆ_ಅಂತ್ಯ
