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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_QUICKFIX)

கட்டமைப்பு dir_அடுக்கு_T
{
    கட்டமைப்பு dir_அடுக்கு_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
வகைவரை கட்டமைப்பு qfline_S qfline_T;
கட்டமைப்பு qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    முழு		qf_fnum;	// file number for the line
    முழு		qf_col;		// column where the error occurred
    முழு		qf_end_col;	// column when the error has range or zero
    முழு		qf_nr;		// error number
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
வகைவரை எண்ணிக்கை
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
வகைவரை கட்டமைப்பு qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    முழு		qf_மொத்தம்;	// number of errors (0 means empty list)
    முழு		qf_index;	// current index in the error list
    முழு		qf_nonevalid;	// TRUE if not a single valid entry found
    முழு		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    கட்டமைப்பு dir_அடுக்கு_T	*qf_dir_அடுக்கு;
    char_u		*qf_directory;
    கட்டமைப்பு dir_அடுக்கு_T	*qf_file_அடுக்கு;
    char_u		*qf_currfile;
    முழு			qf_multiline;
    முழு			qf_multiignore;
    முழு			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
கட்டமைப்பு qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    முழு		qf_refcount;
    முழு		qf_listcount;	    // current number of lists
    முழு		qf_curlist;	    // current error list
    முழு         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    முழு		qf_bufnr;	    // quickfix window buffer number
};

நிலையான qf_info_T ql_info_actual; // global quickfix list
நிலையான qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
நிலையான int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
வகைவரை கட்டமைப்பு efm_S efm_T;
கட்டமைப்பு efm_S
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
    முழு		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
வகைவரை கட்டமைப்பு qf_delq_S
{
    கட்டமைப்பு qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
நிலையான qf_delq_T *qf_delq_head = வெற்று;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
நிலையான முழு	quickfix_busy = 0;

நிலையான efm_T	*fmt_start = வெற்று; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
நிலையான callback_T qftf_cb;

நிலையான வெறுமை     qf_எடு_அடுக்கு(qf_info_T *qi, முழு adjust);
நிலையான வெறுமை	qf_new_list(qf_info_T *qi, char_u *qf_title);
நிலையான முழு	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, முழு bufnum, char_u *mesg, long lnum, long end_lnum, முழு col, முழு end_col, முழு vis_col, char_u *pattern, முழு nr, முழு type, typval_T *user_data, முழு valid);
நிலையான முழு      qf_resize_அடுக்கு_base(qf_info_T *qi, முழு n);
நிலையான வெறுமை     qf_sync_llw_to_win(win_T *llw);
நிலையான வெறுமை     qf_sync_win_to_llw(win_T *pwp);
நிலையான qf_info_T *qf_alloc_அடுக்கு(qfltype_T qfltype, முழு n);
நிலையான qf_list_T *qf_alloc_list_அடுக்கு(முழு n);
நிலையான வெறுமை	qf_free(qf_list_T *qfl);
நிலையான char_u	*qf_types(முழு, முழு);
நிலையான முழு	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
நிலையான char_u	*qf_தள்ளு_dir(char_u *, கட்டமைப்பு dir_அடுக்கு_T **, முழு is_file_அடுக்கு);
நிலையான char_u	*qf_எடு_dir(கட்டமைப்பு dir_அடுக்கு_T **);
நிலையான char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
நிலையான win_T    *qf_find_win_with_loclist(qf_info_T *ll);
நிலையான வெறுமை	qf_jump_newwin(qf_info_T *qi, முழு dir, முழு errornr, முழு forceit, முழு newwin);
நிலையான வெறுமை	qf_fmt_text(garray_T *gap, char_u *text);
நிலையான வெறுமை	qf_range_text(garray_T *gap, qfline_T *qfp);
நிலையான முழு	qf_win_pos_update(qf_info_T *qi, முழு old_qf_index);
நிலையான win_T	*qf_find_win(qf_info_T *qi);
நிலையான buf_T	*qf_find_buf(qf_info_T *qi);
நிலையான வெறுமை	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
நிலையான வெறுமை	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, முழு qf_winid);
நிலையான buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
நிலையான வெறுமை	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
நிலையான வெறுமை	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
நிலையான qf_info_T *ll_get_or_alloc_list(win_T *);
நிலையான முழு	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, முழு target_fnum, முழு target_lnum, முழு target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == வெற்று)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != வெற்று)

// Quickfix and location list stack check helper macros
# define IS_QF_அடுக்கு(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_அடுக்கு(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i) \
		    க்கு ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_மொத்தம் && (qfp) != வெற்று; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
நிலையான char_u   *qf_last_bufname = வெற்று;
நிலையான bufref_T  qf_last_bufref = {வெற்று, 0, 0};

நிலையான garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    நிலையான garray_T *
qfga_get(வெறுமை)
{
    நிலையான முழு initialized = FALSE;

    எனில் (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    திரும்பு &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    நிலையான வெறுமை
qfga_clear(வெறுமை)
{
    எனில் (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    இல்லையெனில்
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define வரி_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
நிலையான கட்டமைப்பு fmtpattern
{
    char_u	convchar;
    எழுத்து	*pattern;
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
    நிலையான char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	முழு	idx,
	முழு	round)
{
    char_u	*srcptr;

    எனில் (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	திரும்பு வெற்று;
    }
    எனில் ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != வெற்று)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == வெற்று))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	திரும்பு வெற்று;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    எனில் (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    எனில் (*efmpat == 'f' && efmpat[1] != NUL)
    {
	எனில் (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	இல்லையெனில்
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    இல்லையெனில்
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	போது ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    திரும்பு regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    நிலையான char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	முழு	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    எனில் (*efmp == '[' || *efmp == '\\')
    {
	எனில் ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    எனில் (efmp[1] == '^')
		*regpat++ = *++efmp;
	    எனில் (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		போது (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		எனில் (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    திரும்பு வெற்று;
		}
	    }
	}
	இல்லையெனில் எனில் (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    இல்லையெனில்
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	திரும்பு வெற்று;
    }

    *pefmp = efmp;

    திரும்பு regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    நிலையான char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    எனில் (vim_strchr((char_u *)"+-", *efmp) != வெற்று)
	efminfo->flags = *efmp++;
    எனில் (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != வெற்று)
	efminfo->prefix = *efmp;
    இல்லையெனில்
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	திரும்பு வெற்று;
    }

    திரும்பு efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    நிலையான முழு
efm_to_regpat(
	char_u	*efm,
	முழு	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    முழு		round;
    முழு		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    க்கு (efmp = efm; efmp < efm + len; ++efmp)
    {
	எனில் (*efmp == '%')
	{
	    ++efmp;
	    க்கு (idx = 0; idx < FMT_PATTERNS; ++idx)
		எனில் (fmt_pat[idx].convchar == *efmp)
		    நிறுத்து;
	    எனில் (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		எனில் (ptr == வெற்று)
		    திரும்பு FAIL;
		round++;
	    }
	    இல்லையெனில் எனில் (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		எனில் (ptr == வெற்று)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (vim_strchr((char_u *)"%\\.^$~[", *efmp) != வெற்று)
		*ptr++ = *efmp;		// regexp magic characters
	    இல்லையெனில் எனில் (*efmp == '#')
		*ptr++ = '*';
	    இல்லையெனில் எனில் (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    இல்லையெனில் எனில் (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		எனில் (efmp == வெற்று)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில்			// copy normal character
	{
	    எனில் (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    இல்லையெனில் எனில் (vim_strchr((char_u *)".*^$~[", *efmp) != வெற்று)
		*ptr++ = '\\';	// escape regexp atoms
	    எனில் (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    திரும்பு OK;
}

/*
 * Free the 'errorformat' information list
 */
    நிலையான வெறுமை
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    க்கு (efm_ptr = *efm_first; efm_ptr != வெற்று; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = வெற்று;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    நிலையான முழு
efm_regpat_bufsz(char_u *efm)
{
    முழு sz;
    முழு i;

    sz = (FMT_PATTERNS * 3) + ((முழு)STRLEN(efm) << 2);
    க்கு (i = FMT_PATTERNS; i > 0; )
	sz += (முழு)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# இல்லையெனில்
    sz += 2; // "%f" can become two chars longer
# endif

    திரும்பு sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    நிலையான முழு
efm_option_part_len(char_u *efm)
{
    முழு len;

    க்கு (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	எனில் (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    திரும்பு len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    நிலையான efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = வெற்று;
    efm_T	*fmt_first = வெற்று;
    efm_T	*fmt_last = வெற்று;
    char_u	*fmtstr = வெற்று;
    முழு		len;
    முழு		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    எனில் ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == வெற்று)
	செல் parse_efm_error;

    போது (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	எனில் (fmt_ptr == வெற்று)
	    செல் parse_efm_error;
	எனில் (fmt_first == வெற்று)	    // first one
	    fmt_first = fmt_ptr;
	இல்லையெனில்
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	எனில் (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    செல் parse_efm_error;
	எனில் ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == வெற்று)
	    செல் parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    எனில் (fmt_first == வெற்று)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    செல் parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    திரும்பு fmt_first;
}

எண்ணிக்கை {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_உள்ளீடு = 2,
    QF_NOMEM = 3,
    QF_IGNORE_வரி = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
வகைவரை கட்டமைப்பு {
    char_u	*linebuf;
    முழு		linelen;
    char_u	*growbuf;
    முழு		growbufsiz;
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
    நிலையான char_u *
qf_grow_linebuf(qfstate_T *state, முழு newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > வரி_MAXLEN ? வரி_MAXLEN - 1 : newsz;
    எனில் (state->growbuf == வெற்று)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	எனில் (state->growbuf == வெற்று)
	    திரும்பு வெற்று;
	state->growbufsiz = state->linelen;
    }
    இல்லையெனில் எனில் (state->linelen > state->growbufsiz)
    {
	எனில் ((p = vim_realloc(state->growbuf, state->linelen + 1)) == வெற்று)
	    திரும்பு வெற்று;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    திரும்பு state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    நிலையான முழு
qf_get_next_str_வரி(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    முழு		len;

    எனில் (*p_str == NUL) // Reached the end of the string
	திரும்பு QF_END_OF_உள்ளீடு;

    p = vim_strchr(p_str, '\n');
    எனில் (p != வெற்று)
	len = (முழு)(p - p_str) + 1;
    இல்லையெனில்
	len = (முழு)STRLEN(p_str);

    எனில் (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	எனில் (state->linebuf == வெற்று)
	    திரும்பு QF_NOMEM;
    }
    இல்லையெனில்
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    திரும்பு QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    நிலையான முழு
qf_get_next_list_வரி(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    முழு		len;

    போது (p_li != வெற்று
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == வெற்று))
	p_li = p_li->li_next;	// Skip non-string items

    எனில் (p_li == வெற்று)		// End of the list
    {
	state->p_li = வெற்று;
	திரும்பு QF_END_OF_உள்ளீடு;
    }

    len = (முழு)STRLEN(p_li->li_tv.vval.v_string);
    எனில் (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	எனில் (state->linebuf == வெற்று)
	    திரும்பு QF_NOMEM;
    }
    இல்லையெனில்
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    திரும்பு QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    நிலையான முழு
qf_get_next_buf_வரி(qfstate_T *state)
{
    char_u	*p_buf = வெற்று;
    முழு		len;

    // Get the next line from the supplied buffer
    எனில் (state->buflnum > state->lnumlast)
	திரும்பு QF_END_OF_உள்ளீடு;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    எனில் (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	எனில் (state->linebuf == வெற்று)
	    திரும்பு QF_NOMEM;
    }
    இல்லையெனில்
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    திரும்பு QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    நிலையான முழு
qf_get_next_file_வரி(qfstate_T *state)
{
    முழு	    discard;
    முழு	    growbuflen;

    எனில் (fgets((எழுத்து *)IObuff, IOSIZE, state->fd) == வெற்று)
	திரும்பு QF_END_OF_உள்ளீடு;

    discard = FALSE;
    state->linelen = (முழு)STRLEN(IObuff);
    எனில் (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	எனில் (state->growbuf == வெற்று)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    எனில் (state->growbuf == வெற்று)
		திரும்பு QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	க்கு (;;)
	{
	    char_u	*p;

	    எனில் (fgets((எழுத்து *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == வெற்று)
		நிறுத்து;
	    state->linelen = (முழு)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    எனில் ((state->growbuf)[growbuflen - 1] == '\n')
		நிறுத்து;
	    எனில் (state->growbufsiz == வரி_MAXLEN)
	    {
		discard = TRUE;
		நிறுத்து;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < வரி_MAXLEN
		? 2 * state->growbufsiz : வரி_MAXLEN;
	    எனில் ((p = vim_realloc(state->growbuf, state->growbufsiz)) == வெற்று)
		திரும்பு QF_NOMEM;
	    state->growbuf = p;
	}

	போது (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    எனில் (fgets((எழுத்து *)IObuff, IOSIZE, state->fd) == வெற்று
		    || (முழு)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		நிறுத்து;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    இல்லையெனில்
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    எனில் (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*வரி;

	வரி = string_convert(&state->vc, state->linebuf, &state->linelen);
	எனில் (வரி != வெற்று)
	{
	    எனில் (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, வரி);
		vim_free(வரி);
	    }
	    இல்லையெனில்
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = வரி;
		state->growbufsiz = state->linelen < வரி_MAXLEN
						? state->linelen : வரி_MAXLEN;
	    }
	}
    }

    திரும்பு QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    நிலையான முழு
qf_get_nextline(qfstate_T *state)
{
    முழு status = QF_FAIL;

    எனில் (state->fd == வெற்று)
    {
	எனில் (state->tv != வெற்று)
	{
	    எனில் (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_வரி(state);
	    இல்லையெனில் எனில் (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_வரி(state);
	}
	இல்லையெனில்
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_வரி(state);
    }
    இல்லையெனில்
	// Get the next line from the supplied file
	status = qf_get_next_file_வரி(state);

    எனில் (status != QF_OK)
	திரும்பு status;

    // remove newline/CR from the line
    எனில் (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	எனில் (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    திரும்பு QF_OK;
}

வகைவரை கட்டமைப்பு {
    char_u	*namebuf;
    முழு		bnr;
    char_u	*module;
    char_u	*errmsg;
    முழு		errmsglen;
    long	lnum;
    long	end_lnum;
    முழு		col;
    முழு		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    முழு		enr;
    முழு		type;
    typval_T	*user_data;
    முழு		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    நிலையான முழு
qf_parse_fmt_f(regmatch_T *rmp, முழு midx, qffields_T *fields, முழு prefix)
{
    முழு c;

    எனில் (rmp->startp[midx] == வெற்று || rmp->endp[midx] == வெற்று)
	திரும்பு QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    எனில் (vim_strchr((char_u *)"OPQ", prefix) != வெற்று
	    && mch_getperm(fields->namebuf) == -1)
	திரும்பு QF_FAIL;

    திரும்பு QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    நிலையான முழு
qf_parse_fmt_b(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    முழு bnr = (முழு)atol((எழுத்து *)rmp->startp[midx]);
    எனில் (buflist_findnr(bnr) == வெற்று)
	திரும்பு QF_FAIL;
    fields->bnr = bnr;
    திரும்பு QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    நிலையான முழு
qf_parse_fmt_n(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->enr = (முழு)atol((எழுத்து *)rmp->startp[midx]);
    திரும்பு QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    நிலையான முழு
qf_parse_fmt_l(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->lnum = atol((எழுத்து *)rmp->startp[midx]);
    திரும்பு QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    நிலையான முழு
qf_parse_fmt_e(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->end_lnum = atol((எழுத்து *)rmp->startp[midx]);
    திரும்பு QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    நிலையான முழு
qf_parse_fmt_c(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->col = (முழு)atol((எழுத்து *)rmp->startp[midx]);
    திரும்பு QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    நிலையான முழு
qf_parse_fmt_k(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->end_col = (முழு)atol((எழுத்து *)rmp->startp[midx]);
    திரும்பு QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    நிலையான முழு
qf_parse_fmt_t(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->type = *rmp->startp[midx];
    திரும்பு QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    நிலையான முழு
copy_nonerror_வரி(char_u *linebuf, முழு linelen, qffields_T *fields)
{
    char_u	*p;

    எனில் (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	எனில் ((p = vim_realloc(fields->errmsg, linelen + 1)) == வெற்று)
	    திரும்பு QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    திரும்பு QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    நிலையான முழு
qf_parse_fmt_m(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    char_u	*p;
    முழு		len;

    எனில் (rmp->startp[midx] == வெற்று || rmp->endp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    len = (முழு)(rmp->endp[midx] - rmp->startp[midx]);
    எனில் (len >= fields->errmsglen)
    {
	// len + null terminator
	எனில் ((p = vim_realloc(fields->errmsg, len + 1)) == வெற்று)
	    திரும்பு QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    திரும்பு QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    நிலையான முழு
qf_parse_fmt_r(regmatch_T *rmp, முழு midx, char_u **tail)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    *tail = rmp->startp[midx];
    திரும்பு QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    நிலையான முழு
qf_parse_fmt_p(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    char_u	*match_ptr;

    எனில் (rmp->startp[midx] == வெற்று || rmp->endp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->col = 0;
    க்கு (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	எனில் (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    திரும்பு QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    நிலையான முழு
qf_parse_fmt_v(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    எனில் (rmp->startp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    fields->col = (முழு)atol((எழுத்து *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    திரும்பு QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    நிலையான முழு
qf_parse_fmt_s(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    முழு		len;

    எனில் (rmp->startp[midx] == வெற்று || rmp->endp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    len = (முழு)(rmp->endp[midx] - rmp->startp[midx]);
    எனில் (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    திரும்பு QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    நிலையான முழு
qf_parse_fmt_o(regmatch_T *rmp, முழு midx, qffields_T *fields)
{
    முழு		len;

    எனில் (rmp->startp[midx] == வெற்று || rmp->endp[midx] == வெற்று)
	திரும்பு QF_FAIL;
    len = (முழு)(rmp->endp[midx] - rmp->startp[midx]);
    எனில் (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    திரும்பு QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
நிலையான முழு (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, முழு, qffields_T *) =
{
    வெற்று, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    வெற்று, // %r
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
    நிலையான முழு
qf_parse_match(
	char_u		*linebuf,
	முழு		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	முழு		qf_multiline,
	முழு		qf_multiscan,
	char_u		**tail)
{
    முழு		idx = fmt_ptr->prefix;
    முழு		i;
    முழு		midx;
    முழு		status;

    எனில் ((idx == 'C' || idx == 'Z') && !qf_multiline)
	திரும்பு QF_FAIL;
    எனில் (vim_strchr((char_u *)"EWIN", idx) != வெற்று)
	fields->type = idx;
    இல்லையெனில்
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    க்கு (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (முழு)fmt_ptr->addr[i];
	எனில் (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	இல்லையெனில் எனில் (i == FMT_PATTERN_M)
	{
	    எனில் (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_வரி(linebuf, linelen, fields);
	    இல்லையெனில் எனில் (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	இல்லையெனில் எனில் (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	இல்லையெனில் எனில் (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	எனில் (status != QF_OK)
	    திரும்பு status;
    }

    திரும்பு QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    நிலையான முழு
qf_parse_get_fields(
	char_u		*linebuf,
	முழு		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	முழு		qf_multiline,
	முழு		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    முழு		status = QF_FAIL;
    முழு		r;

    எனில் (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == வெற்று)
	திரும்பு QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    எனில் (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = வெற்று;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    எனில் (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    திரும்பு status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    நிலையான முழு
qf_parse_dir_pfx(முழு idx, qffields_T *fields, qf_list_T *qfl)
{
    எனில் (idx == 'D')				// enter directory
    {
	எனில் (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    திரும்பு QF_FAIL;
	}
	qfl->qf_directory =
	    qf_தள்ளு_dir(fields->namebuf, &qfl->qf_dir_அடுக்கு, FALSE);
	எனில் (qfl->qf_directory == வெற்று)
	    திரும்பு QF_FAIL;
    }
    இல்லையெனில் எனில் (idx == 'X')			// leave directory
	qfl->qf_directory = qf_எடு_dir(&qfl->qf_dir_அடுக்கு);

    திரும்பு QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    நிலையான முழு
qf_parse_file_pfx(
	முழு idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    எனில் (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	எனில் (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_தள்ளு_dir(fields->namebuf, &qfl->qf_file_அடுக்கு, TRUE);
	இல்லையெனில் எனில் (idx == 'Q')
	    qfl->qf_currfile = qf_எடு_dir(&qfl->qf_file_அடுக்கு);
	*fields->namebuf = NUL;
	எனில் (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    திரும்பு QF_MULTISCAN;
	}
    }

    திரும்பு QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    நிலையான முழு
qf_parse_வரி_nomatch(char_u *linebuf, முழு linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    திரும்பு copy_nonerror_வரி(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    நிலையான முழு
qf_parse_multiline_pfx(
	முழு idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    முழு			len;

    எனில் (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	எனில் (qfprev == வெற்று)
	    திரும்பு QF_FAIL;
	எனில் (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (முழு)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    எனில் (ptr == வெற்று)
		திரும்பு QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	எனில் (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	எனில் (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	எனில் (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	எனில் (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	எனில் (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	எனில் (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	எனில் (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != வெற்று
		    ? fields->namebuf
		    : qfl->qf_currfile != வெற்று && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    எனில் (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    வரி_breakcheck();

    திரும்பு QF_IGNORE_வரி;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    நிலையான முழு
qf_parse_வரி(
	qf_list_T	*qfl,
	char_u		*linebuf,
	முழு		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    முழு			idx = 0;
    char_u		*tail = வெற்று;
    முழு			status;

restofline:
    // If there was no %> item start at the first pattern
    எனில் (fmt_start == வெற்று)
	fmt_ptr = fmt_first;
    இல்லையெனில்
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = வெற்று;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    க்கு ( ; fmt_ptr != வெற்று; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	எனில் (status == QF_NOMEM)
	    திரும்பு status;
	எனில் (status == QF_OK)
	    நிறுத்து;
    }
    qfl->qf_multiscan = FALSE;

    எனில் (fmt_ptr == வெற்று || idx == 'D' || idx == 'X')
    {
	எனில் (fmt_ptr != வெற்று)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    எனில் (status != QF_OK)
		திரும்பு status;
	}

	status = qf_parse_வரி_nomatch(linebuf, linelen, fields);
	எனில் (status != QF_OK)
	    திரும்பு status;

	எனில் (fmt_ptr == வெற்று)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    இல்லையெனில் எனில் (fmt_ptr != வெற்று)
    {
	// honor %> item
	எனில் (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	எனில் (vim_strchr((char_u *)"AEWIN", idx) != வெற்று)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	இல்லையெனில் எனில் (vim_strchr((char_u *)"CZ", idx) != வெற்று)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    எனில் (status != QF_OK)
		திரும்பு status;
	}
	இல்லையெனில் எனில் (vim_strchr((char_u *)"OPQ", idx) != வெற்று)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    எனில் (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		முழு new_linelen = (முழு)STRLEN(s);
		எனில் (new_linelen >= linelen)
		    திரும்பு QF_IGNORE_வரி;
		linebuf = s;
		linelen = new_linelen;
		செல் restofline;
	    }
	}
	எனில் (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    எனில் (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    திரும்பு QF_IGNORE_வரி;
	}
    }

    திரும்பு QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    நிலையான முழு
qf_அடுக்கு_empty(qf_info_T *qi)
{
    திரும்பு qi == வெற்று || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    நிலையான முழு
qf_list_empty(qf_list_T *qfl)
{
    திரும்பு qfl == வெற்று || qfl->qf_மொத்தம் <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    நிலையான முழு
qf_list_has_valid_entries(qf_list_T *qfl)
{
    திரும்பு !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    நிலையான qf_list_T *
qf_get_list(qf_info_T *qi, முழு idx)
{
    திரும்பு &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    நிலையான முழு
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    எனில் (pfields->namebuf == வெற்று || pfields->errmsg == வெற்று
		|| pfields->pattern == வெற்று || pfields->module == வெற்று)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    நிலையான வெறுமை
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
    நிலையான முழு
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
    எனில் (enc != வெற்று && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    எனில் (efile != வெற்று && (pstate->fd = mch_fopen((எழுத்து *)efile, "r")) == வெற்று)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	திரும்பு FAIL;
    }

    எனில் (tv != வெற்று)
    {
	எனில் (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	இல்லையெனில் எனில் (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    திரும்பு OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    நிலையான வெறுமை
qf_cleanup_state(qfstate_T *pstate)
{
    எனில் (pstate->fd != வெற்று)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    எனில் (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, வெற்று, வெற்று);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    நிலையான முழு
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    முழு		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    எனில் (status != QF_OK)
	திரும்பு status;

    status = qf_parse_வரி(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    எனில் (status != QF_OK)
	திரும்பு status;

    திரும்பு qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != வெற்று)
		? fields->namebuf
		: ((qfl->qf_currfile != வெற்று && fields->valid)
		    ? qfl->qf_currfile : (char_u *)வெற்று),
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
    நிலையான முழு
qf_init_ext(
    qf_info_T	    *qi,
    முழு		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    முழு		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = வெற்று;
    முழு		    adding = FALSE;
    நிலையான efm_T    *fmt_first = வெற்று;
    char_u	    *efm;
    நிலையான char_u   *last_efm = வெற்று;
    முழு		    retval = -1;	// default: return error flag
    முழு		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    எனில் ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	செல் qf_init_end;

    எனில் (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    இல்லையெனில்
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	எனில் (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    எனில் (errorformat == p_efm && tv == வெற்று && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    இல்லையெனில்
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    எனில் (last_efm == வெற்று || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	எனில் (fmt_first != வெற்று)
	    last_efm = vim_strsave(efm);
    }

    எனில் (fmt_first == வெற்று)	// nothing found
	செல் error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    போது (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	எனில் (status == QF_NOMEM)		// memory alloc failure
	    செல் qf_init_end;
	எனில் (status == QF_END_OF_உள்ளீடு)	// end of input
	    நிறுத்து;
	எனில் (status == QF_FAIL)
	    செல் error2;

	வரி_breakcheck();
    }
    எனில் (state.fd == வெற்று || !ferror(state.fd))
    {
	எனில் (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	இல்லையெனில்
	{
	    qfl->qf_nonevalid = FALSE;
	    எனில் (qfl->qf_ptr == வெற்று)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_மொத்தம்;
	செல் qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    எனில் (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	எனில் (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    எனில் (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    திரும்பு retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    முழு
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	முழு	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    எனில் (wp != வெற்று)
	qi = ll_get_or_alloc_list(wp);
    எனில் (qi == வெற்று)
	திரும்பு FAIL;

    திரும்பு qf_init_ext(qi, qi->qf_curlist, efile, curbuf, வெற்று, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    நிலையான வெறுமை
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    எனில் (title == வெற்று)
	திரும்பு;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    எனில் (p != வெற்று)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    நிலையான char_u *
qf_cmdtitle(char_u *cmd)
{
    நிலையான char_u qftitle_str[IOSIZE];

    vim_snprintf((எழுத்து *)qftitle_str, IOSIZE, ":%s", (எழுத்து *)cmd);
    திரும்பு qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    நிலையான qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    திரும்பு qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    நிலையான வெறுமை
qf_எடு_அடுக்கு(qf_info_T *qi, முழு adjust)
{
    முழு i;
    qf_free(&qi->qf_lists[0]);
    க்கு (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, அளவு(*qi->qf_lists));

    எனில் (adjust)
    {
	qi->qf_listcount--;
	எனில் (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	இல்லையெனில்
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    நிலையான வெறுமை
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    போது (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    எனில் (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_எடு_அடுக்கு(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    இல்லையெனில்
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
    நிலையான வெறுமை
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    எனில் (q == வெற்று)
	திரும்பு;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    முழு
qf_அடுக்கு_get_bufnr(வெறுமை)
{
    எனில் (ql_info == வெற்று)
	திரும்பு INVALID_QFBUFNR;
    திரும்பு ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    நிலையான வெறுமை
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    எனில் (qi->qf_bufnr == INVALID_QFBUFNR)
	திரும்பு;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    எனில் (qfbuf != வெற்று && qfbuf->b_nwindows == 0)
    {
	முழு buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	எனில் (curwin->w_buffer == வெற்று)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(வெற்று, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	எனில் (buf_was_null)
	    curwin->w_buffer = வெற்று;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    நிலையான வெறுமை
qf_free_list_அடுக்கு_பொருட்கள்(qf_info_T *qi)
{
    க்கு (முழு i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    நிலையான வெறுமை
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_அடுக்கு_பொருட்கள்(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    நிலையான வெறுமை
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    எனில் (qi == வெற்று)
	திரும்பு;
    *pqi = வெற்று;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    எனில் (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	திரும்பு;
    }

    qi->qf_refcount--;
    எனில் (qi->qf_refcount < 1)
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
    வெறுமை
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    எனில் (wp != வெற்று)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    இல்லையெனில் எனில் (qi != வெற்று)
	qf_free_list_அடுக்கு_பொருட்கள்(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    நிலையான வெறுமை
incr_quickfix_busy(வெறுமை)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    நிலையான வெறுமை
decr_quickfix_busy(வெறுமை)
{
    எனில் (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	போது (qf_delq_head != வெற்று)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    எனில் (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# எனில் defined(EXITFREE)
    வெறுமை
check_quickfix_busy(வெறுமை)
{
    எனில் (quickfix_busy != 0)
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
    நிலையான முழு
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    முழு		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    முழு		col,		// column
    முழு		end_col,	// column for end
    முழு		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    முழு		nr,		// error number
    முழு		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    முழு		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = வெற்று;
    char_u	*p = வெற்று;

    எனில் ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == வெற்று)
	திரும்பு QF_FAIL;
    எனில் (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	எனில் (buf != வெற்று)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    இல்லையெனில்
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    எனில் (fname != வெற்று)
	fullname = fix_fname(fname);
    qfp->qf_fname = வெற்று;
    எனில் (buf != வெற்று &&
	buf->b_ffname != வெற்று && fullname != வெற்று)
    {
	எனில் (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    எனில் (p != வெற்று)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    எனில் ((qfp->qf_text = vim_strsave(mesg)) == வெற்று)
    {
	vim_free(qfp);
	திரும்பு QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    எனில் (user_data == வெற்று || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    இல்லையெனில்
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    எனில் (pattern == வெற்று || *pattern == NUL)
	qfp->qf_pattern = வெற்று;
    இல்லையெனில் எனில் ((qfp->qf_pattern = vim_strsave(pattern)) == வெற்று)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	திரும்பு QF_FAIL;
    }
    எனில் (module == வெற்று || *module == NUL)
	qfp->qf_module = வெற்று;
    இல்லையெனில் எனில் ((qfp->qf_module = vim_strsave(module)) == வெற்று)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	திரும்பு QF_FAIL;
    }
    qfp->qf_nr = nr;
    எனில் (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    எனில் (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = வெற்று;
    }
    இல்லையெனில்
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = வெற்று;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_மொத்தம்;
    எனில் (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_மொத்தம்;
	qfl->qf_ptr = qfp;
    }

    திரும்பு QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    முழு
qf_resize_அடுக்கு(முழு n)
{
    எனில் (ql_info == வெற்று)
	திரும்பு FAIL;

    எனில் (qf_resize_அடுக்கு_base(ql_info, n) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    முழு
ll_resize_அடுக்கு(win_T *wp, முழு n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    எனில் (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    இல்லையெனில்
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    எனில் (qi == வெற்று)
	திரும்பு FAIL;

    எனில் (qf_resize_அடுக்கு_base(qi, n) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    நிலையான முழு
qf_resize_அடுக்கு_base(qf_info_T *qi, முழு n)
{
    qf_list_T *new;
    முழு amount_to_rm = 0, i;

    எனில் (qi == வெற்று)
	திரும்பு FAIL;

    size_t lsz = அளவு(*qi->qf_lists);

    எனில் (n == qi->qf_maxcount)
	திரும்பு OK;
    இல்லையெனில் எனில் (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	க்கு (i = 0; i < amount_to_rm; i++)
	    qf_எடு_அடுக்கு(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    எனில் (new == வெற்று)
	திரும்பு FAIL;

    // fill with zeroes any newly allocated memory
    எனில் (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, வெற்று);

    திரும்பு OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   வெறுமை
qf_init_அடுக்கு(வெறுமை)
{
    ql_info = qf_alloc_அடுக்கு(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    நிலையான வெறுமை
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    எனில் (wp != வெற்று)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    நிலையான வெறுமை
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    எனில் (llw != வெற்று)
	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		திரும்பு;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    நிலையான qf_info_T *
qf_alloc_அடுக்கு(qfltype_T qfltype, முழு n)
{
    qf_info_T *qi;

    எனில் (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    இல்லையெனில்
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	எனில் (qi == வெற்று)
	    திரும்பு வெற்று;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_அடுக்கு(n);
    எனில் (qi->qf_lists == வெற்று)
    {
	எனில் (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	திரும்பு வெற்று;
    }
    qi->qf_maxcount = n;

    திரும்பு qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    நிலையான qf_list_T *
qf_alloc_list_அடுக்கு(முழு n)
{
    திரும்பு ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    நிலையான qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    எனில் (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	திரும்பு wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    எனில் (wp->w_llist == வெற்று)
	// new location list
	wp->w_llist = qf_alloc_அடுக்கு(QFLT_LOCATION, wp->w_p_lhi);

    திரும்பு wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    நிலையான qf_info_T *
qf_cmd_get_அடுக்கு(exarg_T *eap, முழு print_emsg)
{
    qf_info_T	*qi = ql_info;

    எனில் (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	எனில் (qi == வெற்று)
	{
	    எனில் (print_emsg)
		emsg(_(e_no_location_list));
	    திரும்பு வெற்று;
	}
    }
    எனில் (qi == வெற்று && print_emsg)
	emsg(_(e_no_quickfix_அடுக்கு));

    திரும்பு qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    நிலையான qf_info_T *
qf_cmd_get_or_alloc_அடுக்கு(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    எனில் (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	எனில் (qi == வெற்று)
	    திரும்பு வெற்று;
	*pwinp = curwin;
    }

    திரும்பு qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    நிலையான முழு
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    முழு		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_பொருட்கள்(from_qfl, from_qfp, i)
    {
	எனில் (qf_add_entry(to_qfl,
		    வெற்று,
		    வெற்று,
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
	    திரும்பு FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	எனில் (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    திரும்பு OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    நிலையான முழு
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_மொத்தம் = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = வெற்று;
    to_qfl->qf_last = வெற்று;
    to_qfl->qf_ptr = வெற்று;
    எனில் (from_qfl->qf_title != வெற்று)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    இல்லையெனில்
	to_qfl->qf_title = வெற்று;
    எனில் (from_qfl->qf_ctx != வெற்று)
    {
	to_qfl->qf_ctx = alloc_tv();
	எனில் (to_qfl->qf_ctx != வெற்று)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    இல்லையெனில்
	to_qfl->qf_ctx = வெற்று;
    எனில் (from_qfl->qf_qftf_cb.cb_name != வெற்று)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    இல்லையெனில்
	to_qfl->qf_qftf_cb.cb_name = வெற்று;

    எனில் (from_qfl->qf_மொத்தம்)
	எனில் (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    திரும்பு FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    எனில் (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    திரும்பு OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    வெறுமை
copy_loclist_அடுக்கு(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    முழு		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    எனில் (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    இல்லையெனில்
	qi = from->w_llist;

    எனில் (qi == வெற்று)		    // no location list to copy
	திரும்பு;

    // allocate a new location list, set size of stack to 'from' window value
    எனில் ((to->w_llist = qf_alloc_அடுக்கு(QFLT_LOCATION, from->w_p_lhi)) == வெற்று)
	திரும்பு;
    இல்லையெனில்
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    க்கு (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	எனில் (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    திரும்பு;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    நிலையான முழு
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {வெற்று, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    எனில் (fname == வெற்று || *fname == NUL)		// no file name
	திரும்பு 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    எனில் (directory != வெற்று)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    எனில் (directory != வெற்று && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != வெற்று)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	எனில் (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    எனில் (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    இல்லையெனில்
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    எனில் (ptr.string == வெற்று)
		திரும்பு 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    இல்லையெனில்
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    எனில் (qf_last_bufname != வெற்று && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    இல்லையெனில்
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, வெற்று, (linenr_T)0, BLN_NOOPT);
	எனில் (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	இல்லையெனில்
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    எனில் (buf == வெற்று)
	திரும்பு 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    திரும்பு buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    நிலையான char_u *
qf_தள்ளு_dir(char_u *dirbuf, கட்டமைப்பு dir_அடுக்கு_T **stackptr, முழு is_file_அடுக்கு)
{
    கட்டமைப்பு dir_அடுக்கு_T  *ds_new;
    கட்டமைப்பு dir_அடுக்கு_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(கட்டமைப்பு dir_அடுக்கு_T, aid_qf_dirstack);
    எனில் (ds_new == வெற்று)
	திரும்பு வெற்று;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    எனில் (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == வெற்று
	    || is_file_அடுக்கு)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    இல்லையெனில்
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = வெற்று;
	போது (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    எனில் (dirname.string == வெற்று)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		திரும்பு வெற்று;
	    }
	    எனில் (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		நிறுத்து;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	போது ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	எனில் (ds_new == வெற்று)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    எனில் ((*stackptr)->dirname != வெற்று)
	திரும்பு (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    திரும்பு வெற்று;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    நிலையான char_u *
qf_எடு_dir(கட்டமைப்பு dir_அடுக்கு_T **stackptr)
{
    கட்டமைப்பு dir_அடுக்கு_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    எனில் (*stackptr != வெற்று)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    திரும்பு *stackptr ? (*stackptr)->dirname : வெற்று;
}

/*
 * clean up directory stack
 */
    நிலையான வெறுமை
qf_clean_dir_அடுக்கு(கட்டமைப்பு dir_அடுக்கு_T **stackptr)
{
    கட்டமைப்பு dir_அடுக்கு_T  *ds_ptr;

    போது ((ds_ptr = *stackptr) != வெற்று)
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
    நிலையான char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    கட்டமைப்பு dir_அடுக்கு_T     *ds_ptr;
    கட்டமைப்பு dir_அடுக்கு_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    எனில் (qfl->qf_dir_அடுக்கு == வெற்று)
	திரும்பு வெற்று;

    ds_ptr = qfl->qf_dir_அடுக்கு->next;
    fullname.string = வெற்று;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    போது (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	எனில் ((fullname.string != வெற்று) && (mch_getperm(fullname.string) >= 0))
	    நிறுத்து;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    போது (qfl->qf_dir_அடுக்கு->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_அடுக்கு->next;
	qfl->qf_dir_அடுக்கு->next = qfl->qf_dir_அடுக்கு->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    திரும்பு ds_ptr == வெற்று ? வெற்று : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    நிலையான முழு
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    முழு		i;

    எனில் (wp != வெற்று)
    {
	எனில் (!win_valid(wp))
	    திரும்பு FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    எனில் (qi == வெற்று)
	திரும்பு FALSE;

    க்கு (i = 0; i < qi->qf_listcount; ++i)
	எனில் (qi->qf_lists[i].qf_id == qf_id)
	    திரும்பு TRUE;

    திரும்பு FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    நிலையான முழு
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    முழு		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
	எனில் (qfp == qf_ptr)
	    நிறுத்து;

    எனில் (i > qfl->qf_மொத்தம்) // Entry is not found
	திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    நிலையான qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	முழு		*qf_index,
	முழு		dir)
{
    முழு			idx;
    முழு			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    செய்
    {
	எனில் (idx == qfl->qf_மொத்தம் || qf_ptr->qf_next == வெற்று)
	    திரும்பு வெற்று;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } போது ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    திரும்பு qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    நிலையான qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	முழு		*qf_index,
	முழு		dir)
{
    முழு			idx;
    முழு			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    செய்
    {
	எனில் (idx == 1 || qf_ptr->qf_prev == வெற்று)
	    திரும்பு வெற்று;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } போது ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    திரும்பு qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    நிலையான qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	முழு		errornr,
	முழு		dir,
	முழு		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    முழு			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    முழு			prev_index;
    எழுத்து		*err = e_no_more_பொருட்கள்;

    போது (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	எனில் (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	இல்லையெனில்
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	எனில் (qf_ptr == வெற்று)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    எனில் (err != வெற்று)
	    {
		emsg(_(err));
		திரும்பு வெற்று;
	    }
	    நிறுத்து;
	}

	err = வெற்று;
    }

    *new_qfidx = qf_idx;
    திரும்பு qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    நிலையான qfline_T *
get_nth_entry(qf_list_T *qfl, முழு errornr, முழு *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    முழு		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    போது (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != வெற்று)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    போது (errornr > qf_idx && qf_idx < qfl->qf_மொத்தம் &&
						qf_ptr->qf_next != வெற்று)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    திரும்பு qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    நிலையான qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	முழு		errornr,
	முழு		dir,
	முழு		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    முழு		qfidx = qfl->qf_index;

    எனில் (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    இல்லையெனில் எனில் (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    திரும்பு qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    நிலையான win_T *
qf_find_help_win(வெறுமை)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (bt_help(wp->w_buffer))
	    திரும்பு wp;

    திரும்பு வெற்று;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    நிலையான வெறுமை
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    நிலையான முழு
jump_to_help_window(qf_info_T *qi, முழு newwin, முழு *opened_window)
{
    win_T	*wp;
    முழு		flags;

    எனில் (cmdmod.cmod_tab != 0 || newwin)
	wp = வெற்று;
    இல்லையெனில்
	wp = qf_find_help_win();
    எனில் (wp != வெற்று && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    இல்லையெனில்
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	எனில் (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_உச்சி;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	எனில் (IS_LL_அடுக்கு(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	எனில் (win_split(0, flags) == FAIL)
	    திரும்பு FAIL;

	*opened_window = TRUE;

	எனில் (curwin->w_height < p_hh)
	    win_setheight((முழு)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	எனில் (IS_LL_அடுக்கு(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    எனில் (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    திரும்பு OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    நிலையான win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    திரும்பு wp;

    திரும்பு வெற்று;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    நிலையான win_T *
qf_find_win_with_normal_buf(வெறுமை)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	எனில் (bt_normal(wp->w_buffer))
	    திரும்பு wp;

    திரும்பு வெற்று;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    நிலையான முழு
qf_goto_tabwin_with_file(முழு fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    திரும்பு TRUE;
	}

    திரும்பு FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    நிலையான முழு
qf_open_new_file_win(qf_info_T *ll_ref)
{
    முழு		flags;

    flags = WSP_ABOVE;
    எனில் (ll_ref != வெற்று)
	flags |= WSP_NEWLOC;
    எனில் (win_split(0, flags) == FAIL)
	திரும்பு FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    எனில் (ll_ref != வெற்று)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    திரும்பு OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    நிலையான வெறுமை
qf_goto_win_with_ll_file(win_T *use_win, முழு qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    எனில் (win == வெற்று)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    எனில் (win->w_buffer->b_fnum == qf_fnum)
		நிறுத்து;
	எனில் (win == வெற்று)
	{
	    // Find a previous usable window
	    win = curwin;
	    செய்
	    {
		எனில் (bt_normal(win->w_buffer))
		    நிறுத்து;
		எனில் (win->w_prev == வெற்று)
		    win = lastwin;	// wrap around the top
		இல்லையெனில்
		    win = win->w_prev; // go to previous window
	    } போது (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    எனில் (win->w_llist == வெற்று && ll_ref != வெற்று)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    நிலையான வெறுமை
qf_goto_win_with_qfl_file(முழு qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = வெற்று;
    க்கு (;;)
    {
	எனில் (win->w_buffer->b_fnum == qf_fnum)
	    நிறுத்து;
	எனில் (win->w_prev == வெற்று)
	    win = lastwin;	// wrap around the top
	இல்லையெனில்
	    win = win->w_prev;	// go to previous window

	எனில் (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    எனில் ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    இல்லையெனில் எனில் (altwin != வெற்று)
		win = altwin;
	    இல்லையெனில் எனில் (curwin->w_prev != வெற்று)
		win = curwin->w_prev;
	    இல்லையெனில்
		win = curwin->w_next;
	    நிறுத்து;
	}

	// Remember a usable window.
	எனில் (altwin == வெற்று && !win->w_p_pvw && !win->w_p_wfb &&
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
    நிலையான முழு
qf_jump_to_usable_window(முழு qf_fnum, முழு newwin, முழு *opened_window)
{
    win_T	*usable_wp = வெற்று;
    முழு		usable_win = FALSE;
    qf_info_T	*ll_ref = வெற்று;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    எனில் (!newwin)
	ll_ref = curwin->w_llist_ref;

    எனில் (ll_ref != வெற்று)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	எனில் (usable_wp != வெற்று)
	    usable_win = TRUE;
    }

    எனில் (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	எனில் (win != வெற்று)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    எனில் (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    எனில் ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	எனில் (qf_open_new_file_win(ll_ref) != OK)
	    திரும்பு FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    இல்லையெனில்
    {
	எனில் (curwin->w_llist_ref != வெற்று)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	இல்லையெனில்					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    திரும்பு OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    நிலையான முழு
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	முழு		forceit,
	முழு		prev_winid,
	முழு		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    முழு		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    முழு		retval = OK;
    முழு		old_qf_curlist = qi->qf_curlist;
    முழு		save_qfid = qfl->qf_id;

    எனில் (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	எனில் (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    திரும்பு FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, வெற்று, வெற்று, வெற்று, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : வெற்று);
    }
    இல்லையெனில்
    {
	முழு	fnum = qf_ptr->qf_fnum;

	எனில் (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    எனில் (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		திரும்பு FAIL;
	    }

	    எனில் (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    எனில் (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		எனில் (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		எனில் (curwin->w_p_wfb)
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

	எனில் (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    எனில் (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	எனில் (wp == வெற்று && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    திரும்பு QF_ABORT;
	}
    }

    எனில் (qfl_type == QFLT_QUICKFIX && !qflist_valid(வெற்று, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	திரும்பு QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    எனில் (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	எனில் (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	இல்லையெனில்
	    emsg(_(e_current_location_list_was_changed));
	திரும்பு QF_ABORT;
    }

    திரும்பு retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    நிலையான முழு
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*வரி = ml_get_curline();
    char_u	*p = வரி;
    colnr_T	col = 0;

    போது (*p != NUL && col < vcol)
    {
	எனில் (*p == TAB)
	    col += 8 - (col % 8);
	இல்லையெனில்
	    col += ptr2cells(p);
	எனில் (col > vcol)
	    நிறுத்து;
	MB_PTR_ADV(p);
    }
    திரும்பு (முழு)(p - வரி);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    நிலையான வெறுமை
qf_jump_goto_வரி(
	linenr_T	qf_lnum,
	முழு		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    எனில் (qf_pattern == வெற்று)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	எனில் (i > 0)
	{
	    எனில் (i > curbuf->b_ml.ml_வரி_மொத்தம்)
		i = curbuf->b_ml.ml_வரி_மொத்தம்;
	    curwin->w_cursor.lnum = i;
	}
	எனில் (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    எனில் (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    இல்லையெனில்
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	இல்லையெனில்
	    beginline(BL_WHITE | BL_FIX);
    }
    இல்லையெனில்
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	எனில் (!do_தேடல்(வெற்று, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, தேடல்_KEEP, வெற்று))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    நிலையான வெறுமை
qf_jump_print_செய்தி(
	qf_info_T	*qi,
	முழு		qf_index,
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
    எனில் (!செய்தி_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_மொத்தம்,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (எழுத்து *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = செய்தி_scroll;
    எனில் (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	செய்தி_scroll = TRUE;
    இல்லையெனில் எனில் (!செய்தி_scrolled && shortmess(SHM_OVERALL))
	செய்தி_scroll = FALSE;
    செய்தி_attr_keep((எழுத்து *)gap->ga_data, 0, TRUE);
    செய்தி_scroll = i;

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
    நிலையான முழு
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	முழு		newwin,
	முழு		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    முழு		old_changedtick = qfl->qf_changedtick;
    முழு		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    எனில் (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	எனில் (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    திரும்பு FAIL;
    எனில் (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	எனில் (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	இல்லையெனில்
	    emsg(_(e_current_location_list_was_changed));
	திரும்பு QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    எனில் (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	எனில் (qf_ptr->qf_fnum == 0)
	    திரும்பு NOTDONE;

	எனில் (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    திரும்பு FAIL;
    }
    எனில் (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	எனில் (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	இல்லையெனில்
	    emsg(_(e_current_location_list_was_changed));
	திரும்பு QF_ABORT;
    }

    திரும்பு OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    நிலையான முழு
qf_jump_to_buffer(
	qf_info_T	*qi,
	முழு		qf_index,
	qfline_T	*qf_ptr,
	முழு		forceit,
	முழு		prev_winid,
	முழு		*opened_window,
	முழு		openfold,
	முழு		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    முழு		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    எனில் (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	எனில் (retval != OK)
	    திரும்பு retval;
    }

    // When not switched to another buffer, still need to set pc mark
    எனில் (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_வரி(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    எனில் (print_message)
	qf_jump_print_செய்தி(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    திரும்பு retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    வெறுமை
qf_jump(qf_info_T	*qi,
	முழு		dir,
	முழு		errornr,
	முழு		forceit)
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
    நிலையான வெறுமை
qf_jump_newwin(qf_info_T	*qi,
	முழு		dir,
	முழு		errornr,
	முழு		forceit,
	முழு		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    முழு			qf_index;
    முழு			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    முழு			prev_winid;
    முழு			opened_window = FALSE;
    முழு			print_message = TRUE;
    முழு			old_சாவி_Typed = சாவி_Typed; // getting file may reset it
    முழு			retval = OK;

    எனில் (qi == வெற்று)
    {
	எனில் (ql_info == வெற்று)
	{
	    emsg(_(e_no_quickfix_அடுக்கு));
	    திரும்பு;
	}
	qi = ql_info;
    }

    எனில் (qf_அடுக்கு_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	திரும்பு;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    எனில் (qf_ptr == வெற்று)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	செல் theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    எனில் (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    எனில் (retval == FAIL)
	செல் failed;
    எனில் (retval == QF_ABORT)
    {
	qi = வெற்று;
	qf_ptr = வெற்று;
	செல் theend;
    }
    எனில் (retval == NOTDONE)
	செல் theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_சாவி_Typed, print_message);
    எனில் (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = வெற்று;
	qf_ptr = வெற்று;
    }

    எனில் (retval != OK)
    {
	எனில் (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	எனில் (qf_ptr != வெற்று && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    எனில் (qi != வெற்று)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    எனில் (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
நிலையான முழு	qfFileAttr;
நிலையான முழு	qfSepAttr;
நிலையான முழு	qf_வரி_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    நிலையான வெறுமை
qf_list_entry(qfline_T *qfp, முழு qf_idx, முழு cursel)
{
    char_u	*fname;
    buf_T	*buf;
    முழு		filter_entry;
    garray_T	*gap;

    fname = வெற்று;
    எனில் (qfp->qf_module != வெற்று && *qfp->qf_module != NUL)
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(எழுத்து *)qfp->qf_module);
    இல்லையெனில்
    {
	எனில் (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != வெற்று)
	{
	    எனில் (qfp->qf_fname == வெற்று)
		fname = buf->b_fname;
	    இல்லையெனில்
		fname = qfp->qf_fname;
	    எனில் (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	எனில் (fname == வெற்று)
	    sprintf((எழுத்து *)IObuff, "%2d", qf_idx);
	இல்லையெனில்
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (எழுத்து *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    எனில் (qfp->qf_module != வெற்று && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    எனில் (filter_entry && fname != வெற்று)
	filter_entry &= message_filtered(fname);
    எனில் (filter_entry && qfp->qf_pattern != வெற்று)
	filter_entry &= message_filtered(qfp->qf_pattern);
    எனில் (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    எனில் (filter_entry)
	திரும்பு;

    செய்தி_putchar('\n');
    செய்தி_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    எனில் (qfp->qf_lnum != 0)
	செய்தி_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    எனில் (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    செய்தி_puts_attr((எழுத்து *)gap->ga_data, qf_வரி_Attr);
    செய்தி_puts_attr(":", qfSepAttr);
    எனில் (qfp->qf_pattern != வெற்று)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	செய்தி_puts((எழுத்து *)gap->ga_data);
	செய்தி_puts_attr(":", qfSepAttr);
    }
    செய்தி_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != வெற்று || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    செய்தி_prt_வரி((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    வெறுமை
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    முழு		i;
    முழு		idx1 = 1;
    முழு		idx2 = -1;
    char_u	*arg = eap->arg;
    முழு		plus = FALSE;
    முழு		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    எனில் (qf_அடுக்கு_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	திரும்பு;
    }
    எனில் (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    எனில் (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	திரும்பு;
    }
    qfl = qf_get_curlist(qi);
    எனில் (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    இல்லையெனில்
    {
	i = qfl->qf_மொத்தம்;
	எனில் (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	எனில் (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    எனில் (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    எனில் (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_வரி_Attr = syn_name2attr((char_u *)"qfLineNr");
    எனில் (qf_வரி_Attr == 0)
	qf_வரி_Attr = HL_ATTR(HLF_N);

    எனில் (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
    {
	எனில் ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    நிலையான வெறுமை
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    போது (*p != NUL)
    {
	எனில் (*p == '\n')
	{
	    ga_append(gap, ' ');
	    போது (*++p != NUL)
		எனில் (!VIM_ISWHITE(*p) && *p != '\n')
		    நிறுத்து;
	}
	இல்லையெனில்
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    நிலையான வெறுமை
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((எழுத்து *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    எனில் (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((எழுத்து *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    எனில் (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((எழுத்து *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	எனில் (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((எழுத்து *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    நிலையான வெறுமை
qf_செய்தி(qf_info_T *qi, முழு which, எழுத்து *lead)
{
    எழுத்து   *title = (எழுத்து *)qi->qf_lists[which].qf_title;
    முழு    மொத்தம் = qi->qf_lists[which].qf_மொத்தம்;
    char_u buf[IOSIZE];

    vim_snprintf((எழுத்து *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    மொத்தம்);

    எனில் (title != வெற்று)
    {
	size_t	len = STRLEN(buf);

	எனில் (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    செய்தி((எழுத்து *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    வெறுமை
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    முழு		மொத்தம்;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    எனில் (eap->addr_மொத்தம் != 0)
	மொத்தம் = eap->line2;
    இல்லையெனில்
	மொத்தம் = 1;
    போது (மொத்தம்--)
    {
	எனில் (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    எனில் (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_அடுக்கு));
		நிறுத்து;
	    }
	    --qi->qf_curlist;
	}
	இல்லையெனில்
	{
	    எனில் (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_உச்சி_of_quickfix_அடுக்கு));
		நிறுத்து;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_செய்தி(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, வெற்று);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    வெறுமை
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_அடுக்கு(eap, FALSE);
    முழு		i;

    எனில் (eap->addr_மொத்தம் > 0)
    {
	எனில் (qi == வெற்று)
	{
	    emsg(_(e_no_location_list));
	    திரும்பு;
	}

	// Jump to the specified quickfix list
	எனில் (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_செய்தி(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, வெற்று);
	}
	இல்லையெனில்
	    emsg(_(e_invalid_range));

	திரும்பு;
    }

    எனில் (qf_அடுக்கு_empty(qi))
	செய்தி(_("No entries"));
    இல்லையெனில்
	க்கு (i = 0; i < qi->qf_listcount; ++i)
	    qf_செய்தி(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    நிலையான வெறுமை
qf_free_பொருட்கள்(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    முழு		stop = FALSE;

    போது (qfl->qf_மொத்தம் && qfl->qf_start != வெற்று)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	எனில் (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    எனில் (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_மொத்தம் = 1;
	    இல்லையெனில்
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_மொத்தம்;
    }

    qfl->qf_index = 0;
    qfl->qf_start = வெற்று;
    qfl->qf_last = வெற்று;
    qfl->qf_ptr = வெற்று;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_அடுக்கு(&qfl->qf_dir_அடுக்கு);
    qfl->qf_directory = வெற்று;
    qf_clean_dir_அடுக்கு(&qfl->qf_file_அடுக்கு);
    qfl->qf_currfile = வெற்று;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    நிலையான வெறுமை
qf_free(qf_list_T *qfl)
{
    qf_free_பொருட்கள்(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = வெற்று;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   வெறுமை
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    முழு		i;
    qfline_T	*qfp;
    முழு		idx;
    qf_info_T	*qi = ql_info;
    முழு		found_one = FALSE;
    முழு		buf_has_flag = wp == வெற்று ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    எனில் (!(curbuf->b_has_qf_entry & buf_has_flag))
	திரும்பு;
    எனில் (wp != வெற்று)
    {
	எனில் (wp->w_llist == வெற்று)
	    திரும்பு;
	qi = wp->w_llist;
    }
    இல்லையெனில் எனில் (qi == வெற்று)
	திரும்பு;

    க்கு (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	எனில் (!qf_list_empty(qfl))
	    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
		எனில் (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    எனில் (qfp->qf_cleared)
			தொடர்;

		    எனில் (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			எனில் (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			இல்லையெனில்
			    qfp->qf_lnum += amount;
		    }
		    இல்லையெனில் எனில் (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    எனில் (!found_one)
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
    நிலையான char_u *
qf_types(முழு c, முழு nr)
{
    நிலையான char_u	buf[20];
    நிலையான char_u	cc[3];
    char_u		*p;

    எனில் (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    இல்லையெனில் எனில் (c == 'I' || c == 'i')
	p = (char_u *)" info";
    இல்லையெனில் எனில் (c == 'N' || c == 'n')
	p = (char_u *)" note";
    இல்லையெனில் எனில் (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    இல்லையெனில் எனில் (c == 0 || c == 1)
	p = (char_u *)"";
    இல்லையெனில்
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    எனில் (nr <= 0)
	திரும்பு p;

    sprintf((எழுத்து *)buf, "%s %3d", (எழுத்து *)p, nr);
    திரும்பு buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    வெறுமை
qf_view_result(முழு split)
{
    qf_info_T   *qi = ql_info;

    எனில் (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    இல்லையெனில் எனில் (qi == வெற்று)
    {
	emsg(_(e_no_quickfix_அடுக்கு));
	திரும்பு;
    }

    எனில் (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	திரும்பு;
    }

    எனில் (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	திரும்பு;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    வெறுமை
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    எனில் (qf_அடுக்கு_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	எனில் (win != வெற்று)
	    ex_cclose(eap);
    }
    இல்லையெனில் எனில் (win == வெற்று)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    வெறுமை
ex_cclose(exarg_T *eap)
{
    win_T	*win = வெற்று;
    qf_info_T	*qi;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, FALSE)) == வெற்று)
	திரும்பு;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    எனில் (win != வெற்று)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    நிலையான வெறுமை
qf_set_title_var(qf_list_T *qfl)
{
    எனில் (qfl->qf_title != வெற்று)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    நிலையான முழு
qf_goto_cwindow(qf_info_T *qi, முழு resize, முழு sz, முழு vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    எனில் (win == வெற்று)
	திரும்பு FAIL;

    win_goto(win);
    எனில் (resize)
    {
	எனில் (vertsplit)
	{
	    எனில் (sz != win->w_width)
		win_setwidth(sz);
	}
	இல்லையெனில் எனில் (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    திரும்பு OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    நிலையான வெறுமை
qf_set_cwindow_options(வெறுமை)
{
    // switch off 'swapfile'
    set_option_மதிப்பு_give_err((char_u *)"swf", 0L, வெற்று, OPT_LOCAL);
    set_option_மதிப்பு_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_மதிப்பு_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_மதிப்பு_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    நிலையான முழு
qf_open_new_cwindow(qf_info_T *qi, முழு height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    முழு		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    எனில் (IS_QF_அடுக்கு(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    எனில் (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    எனில் (IS_QF_அடுக்கு(qi))
	flags |= WSP_QUICKFIX;

    எனில் (win_split(height, flags) == FAIL)
	திரும்பு FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    எனில் (IS_LL_அடுக்கு(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    எனில் (oldwin != curwin)
	oldwin = வெற்று;  // don't store info when in another window
    எனில் (qf_buf != வெற்று)
    {
	// Use the existing quickfix buffer
	எனில் (do_ecmd(qf_buf->b_fnum, வெற்று, வெற்று, வெற்று, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	// Create a new quickfix buffer
	எனில் (do_ecmd(0, வெற்று, வெற்று, வெற்று, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    திரும்பு FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    எனில் (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    எனில் (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    எனில் (win_valid(win))
	prevwin = win;

    திரும்பு OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    வெறுமை
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    முழு		height;
    முழு		status = FAIL;
    முழு		lnum;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    incr_quickfix_busy();

    எனில் (eap->addr_மொத்தம் != 0)
	height = eap->line2;
    இல்லையெனில்
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    எனில் (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_மொத்தம் != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    எனில் (status == FAIL)
	எனில் (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    திரும்பு;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, வெற்று, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    நிலையான வெறுமை
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
    வெறுமை
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    win = qf_find_win(qi);
    எனில் (win != வெற்று && win->w_cursor.lnum != win->w_buffer->b_ml.ml_வரி_மொத்தம்)
	qf_win_goto(win, win->w_buffer->b_ml.ml_வரி_மொத்தம்);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    எனில் (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    இல்லையெனில் எனில் (qi == வெற்று)
	திரும்பு 0;

    திரும்பு qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    நிலையான முழு
qf_win_pos_update(
    qf_info_T	*qi,
    முழு		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    முழு		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    எனில் (win != வெற்று
	    && qf_index <= win->w_buffer->b_ml.ml_வரி_மொத்தம்
	    && old_qf_index != qf_index)
    {
	எனில் (qf_index > old_qf_index)
	{
	    win->w_redraw_உச்சி = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	இல்லையெனில்
	{
	    win->w_redraw_உச்சி = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    திரும்பு win != வெற்று;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    நிலையான முழு
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    எனில் (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	எனில் ((IS_QF_அடுக்கு(qi) && win->w_llist_ref == வெற்று)
		|| (IS_LL_அடுக்கு(qi) && win->w_llist_ref == qi))
	    திரும்பு TRUE;

    திரும்பு FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    நிலையான win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	எனில் (is_qf_win(win, qi))
	    திரும்பு win;
    திரும்பு வெற்று;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    நிலையான buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    எனில் (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	எனில் (qfbuf != வெற்று)
	    திரும்பு qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	எனில் (is_qf_win(win, qi))
	    திரும்பு win->w_buffer;

    திரும்பு வெற்று;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    எழுத்து *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    எனில் (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    நிலையான வெறுமை
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	எனில் (is_qf_win(win, qi))
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
    நிலையான வெறுமை
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    எனில் (buf == வெற்று)
	திரும்பு;

    linenr_T	old_வரி_மொத்தம் = buf->b_ml.ml_வரி_மொத்தம்;
    முழு		qf_winid = 0;

    எனில் (IS_LL_அடுக்கு(qi))
    {
	எனில் (curwin->w_llist == qi)
	    win = curwin;
	இல்லையெனில்
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    எனில் (win == வெற்று)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    எனில் (win == வெற்று)
		திரும்பு;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    முழு do_fill = TRUE;
    எனில் (old_last == வெற்று)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    எனில் (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	எனில் (old_last == வெற்று)
	{
	    (வெறுமை)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    எனில் ((win = qf_find_win(qi)) != வெற்று && old_வரி_மொத்தம் < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    நிலையான முழு
qf_buf_add_வரி(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	முழு		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    எனில் (qftf_str != வெற்று && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    இல்லையெனில்
    {
	எனில் (qfp->qf_module != வெற்று)
	    ga_concat(gap, qfp->qf_module);
	இல்லையெனில் எனில் (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != வெற்று
		&& errbuf->b_fname != வெற்று)
	{
	    எனில் (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    இல்லையெனில்
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		எனில் (first_bufline && (errbuf->b_sfname == வெற்று
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    எனில் (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		எனில் (qfp->qf_fname == வெற்று)
		    ga_concat(gap, errbuf->b_fname);
		இல்லையெனில்
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	எனில் (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	இல்லையெனில் எனில் (qfp->qf_pattern != வெற்று)
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
    எனில் (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    நிலையான list_T *
call_qftf_func(qf_list_T *qfl, முழு qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = வெற்று;
    நிலையான முழு	recursive = FALSE;

    எனில் (recursive)
	திரும்பு வெற்று;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    எனில் (qfl->qf_qftf_cb.cb_name != வெற்று)
	cb = &qfl->qf_qftf_cb;
    எனில் (cb->cb_name != வெற்று)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	எனில் ((d = dict_alloc_lock(VAR_FIXED)) == வெற்று)
	{
	    recursive = FALSE;
	    திரும்பு வெற்று;
	}
	dict_add_எண்(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_எண்(d, "winid", (long)qf_winid);
	dict_add_எண்(d, "id", (long)qfl->qf_id);
	dict_add_எண்(d, "start_idx", start_idx);
	dict_add_எண்(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = வெற்று;
	textlock++;
	எனில் (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    எனில் (rettv.v_type == VAR_LIST)
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
    திரும்பு qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    நிலையான வெறுமை
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, முழு qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    முழு		old_சாவி_Typed = சாவி_Typed;
    list_T	*qftf_list = வெற்று;
    listitem_T	*qftf_li = வெற்று;

    எனில் (old_last == வெற்று)
    {
	win_T		*wp;
	tabpage_T	*tp;

	எனில் (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    திரும்பு;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	போது ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (வெறுமை)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    எனில் (qfl != வெற்று && qfl->qf_start != வெற்று)
    {
	char_u		dirname[MAXPATHL];
	முழு		invalid_val = FALSE;
	முழு		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	எனில் (old_last == வெற்று)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	இல்லையெனில்
	{
	    எனில் (old_last->qf_next != வெற்று)
		qfp = old_last->qf_next;
	    இல்லையெனில்
		qfp = old_last;
	    lnum = buf->b_ml.ml_வரி_மொத்தம்;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_மொத்தம்);
	எனில் (qftf_list != வெற்று)
	    qftf_li = qftf_list->lv_first;

	போது (lnum < qfl->qf_மொத்தம்)
	{
	    char_u	*qftf_str = வெற்று;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    எனில் (qftf_li != வெற்று && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		எனில் (qftf_str == வெற்று)
		    invalid_val = TRUE;
	    }

	    எனில் (qf_buf_add_வரி(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		நிறுத்து;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    எனில் (qfp == வெற்று)
		நிறுத்து;

	    எனில் (qftf_li != வெற்று)
		qftf_li = qftf_li->li_next;
	}

	எனில் (old_last == வெற்று)
	    // Delete the empty line which is now at the end
	    (வெறுமை)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    எனில் (old_last == வெற்று)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_மதிப்பு_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", வெற்று,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", வெற்று,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    சாவி_Typed = old_சாவி_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    நிலையான வெறுமை
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    நிலையான முழு
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    முழு		qf_idx;

    க்கு (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	எனில் (qi->qf_lists[qf_idx].qf_id == qfid)
	    திரும்பு qf_idx;
    திரும்பு INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    நிலையான முழு
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    முழு curlist;

    எனில் (qf_get_curlist(qi)->qf_id == save_qfid)
	திரும்பு OK;

    curlist = qf_id2nr(qi, save_qfid);
    எனில் (curlist < 0)
	// list is not present
	திரும்பு FAIL;
    qi->qf_curlist = curlist;
    திரும்பு OK;
}

/*
 * Jump to the first entry if there is one.
 */
    நிலையான வெறுமை
qf_jump_first(qf_info_T *qi, int_u save_qfid, முழு forceit)
{
    எனில் (qf_restore_list(qi, save_qfid) == FAIL)
	திரும்பு;


    எனில் (!check_can_set_curbuf_forceit(forceit))
	திரும்பு;


    // Autocommands might have cleared the list, check for that.
    எனில் (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    முழு
grep_internal(cmdidx_T cmdidx)
{
    திரும்பு ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    நிலையான char_u *
make_get_auname(cmdidx_T cmdidx)
{
    தேர்வு (cmdidx)
    {
	நிலை CMD_make:	    திரும்பு (char_u *)"make";
	நிலை CMD_lmake:	    திரும்பு (char_u *)"lmake";
	நிலை CMD_grep:	    திரும்பு (char_u *)"grep";
	நிலை CMD_lgrep:	    திரும்பு (char_u *)"lgrep";
	நிலை CMD_grepadd:   திரும்பு (char_u *)"grepadd";
	நிலை CMD_lgrepadd:  திரும்பு (char_u *)"lgrepadd";
	இயல்பு: திரும்பு வெற்று;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    நிலையான char_u *
get_mef_name(வெறுமை)
{
    char_u	*p;
    char_u	*name;
    நிலையான முழு	start = -1;
    நிலையான முழு	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    எனில் (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	எனில் (name == வெற்று)
	    emsg(_(e_cant_get_temp_file_name));
	திரும்பு name;
    }

    க்கு (p = p_mef; *p; ++p)
	எனில் (p[0] == '#' && p[1] == '#')
	    நிறுத்து;

    எனில் (*p == NUL)
	திரும்பு vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    க்கு (;;)
    {
	எனில் (start == -1)
	    start = mch_get_pid();
	இல்லையெனில்
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	எனில் (name == வெற்று)
	    நிறுத்து;
	STRCPY(name, p_mef);
	sprintf((எழுத்து *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	எனில் (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((எழுத்து *)name, &sb) < 0
# endif
		)
	    நிறுத்து;
	vim_free(name);
    }
    திரும்பு name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    நிலையான char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    எனில் (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    எனில் (cmd == வெற்று)
	திரும்பு வெற்று;
    sprintf((எழுத்து *)cmd, "%s%s%s", (எழுத்து *)p_shq, (எழுத்து *)makecmd,
							       (எழுத்து *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    எனில் (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    எனில் (செய்தி_col == 0)
	செய்தி_didout = FALSE;
    செய்தி_start();
    செய்தி_puts(":!");
    செய்தி_outtrans(cmd);		// show what we are doing

    திரும்பு cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    வெறுமை
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = வெற்று;
    win_T	*wp = வெற்று;
    qf_info_T	*qi = ql_info;
    முழு		res;
    char_u	*au_name = வெற்று;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    முழு		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    எனில் (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	திரும்பு;
    }

    au_name = make_get_auname(eap->cmdidx);
    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	எனில் (aborting())
	    திரும்பு;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    எனில் (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    எனில் (fname == வெற்று)
	திரும்பு;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    எனில் (cmd == வெற்று)
    {
	vim_free(fname);
	திரும்பு;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (வெறுமை)char_avail();
# endif

    incr_quickfix_busy();

    எனில் (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    எனில் (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    எனில் (wp != வெற்று)
    {
	qi = GET_LOC_LIST(wp);
	எனில் (qi == வெற்று)
	    செல் cleanup;
    }
    இல்லையெனில் எனில் (qi == வெற்று)
	செல் cleanup;

    எனில் (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    எனில் (au_name != வெற்று)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    எனில் (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    முழு
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, FALSE)) == வெற்று)
	திரும்பு 0;
    திரும்பு qf_get_curlist(qi)->qf_மொத்தம்;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    முழு
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    முழு		i, sz = 0;
    முழு		prev_fnum = 0;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, FALSE)) == வெற்று)
	திரும்பு 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
    {
	எனில் (qfp->qf_valid)
	{
	    எனில் (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    இல்லையெனில் எனில் (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    திரும்பு sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    முழு
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, FALSE)) == வெற்று)
	திரும்பு 0;

    திரும்பு qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    முழு
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    முழு		i, eidx = 0;
    முழு		prev_fnum = 0;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, FALSE)) == வெற்று)
	திரும்பு 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    எனில் (!qf_list_has_valid_entries(qfl))
	திரும்பு 1;

    க்கு (i = 1; i <= qfl->qf_index && qfp!= வெற்று; i++, qfp = qfp->qf_next)
    {
	எனில் (qfp->qf_valid)
	{
	    எனில் (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		எனில் (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    இல்லையெனில்
		eidx++;
	}
    }

    திரும்பு eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    நிலையான முழு
qf_get_nth_valid_entry(qf_list_T *qfl, முழு n, முழு fdo)
{
    qfline_T	*qfp;
    முழு		i, eidx;
    முழு		prev_fnum = 0;

    // check if the list has valid errors
    எனில் (!qf_list_has_valid_entries(qfl))
	திரும்பு 1;

    eidx = 0;
    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
    {
	எனில் (qfp->qf_valid)
	{
	    எனில் (fdo)
	    {
		எனில் (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    இல்லையெனில்
		eidx++;
	}

	எனில் (eidx == n)
	    நிறுத்து;
    }

    எனில் (i <= qfl->qf_மொத்தம்)
	திரும்பு i;
    இல்லையெனில்
	திரும்பு 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    வெறுமை
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    முழு		errornr;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    எனில் (eap->addr_மொத்தம் > 0)
	errornr = (முழு)eap->line2;
    இல்லையெனில்
    {
	தேர்வு (eap->cmdidx)
	{
	    நிலை CMD_cc: நிலை CMD_ll:
		errornr = 0;
		நிறுத்து;
	    நிலை CMD_crewind: நிலை CMD_lrewind: நிலை CMD_cfirst:
	    நிலை CMD_lfirst:
		errornr = 1;
		நிறுத்து;
	    இயல்பு:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    எனில் (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_மொத்தம் > 0 ? (முழு)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    வெறுமை
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    முழு		errornr;
    முழு		dir;

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    எனில் (eap->addr_மொத்தம் > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (முழு)eap->line2;
    இல்லையெனில்
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    தேர்வு (eap->cmdidx)
    {
	நிலை CMD_cnext: நிலை CMD_lnext: நிலை CMD_cdo: நிலை CMD_ldo:
	    dir = FORWARD;
	    நிறுத்து;
	நிலை CMD_cprevious: நிலை CMD_lprevious: நிலை CMD_cNext:
	நிலை CMD_lNext:
	    dir = BACKWARD;
	    நிறுத்து;
	நிலை CMD_cnfile: நிலை CMD_lnfile: நிலை CMD_cfdo: நிலை CMD_lfdo:
	    dir = FORWARD_FILE;
	    நிறுத்து;
	நிலை CMD_cpfile: நிலை CMD_lpfile: நிலை CMD_cNfile: நிலை CMD_lNfile:
	    dir = BACKWARD_FILE;
	    நிறுத்து;
	இயல்பு:
	    dir = FORWARD;
	    நிறுத்து;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    நிலையான qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, முழு bnr, முழு *errornr)
{
    qfline_T	*qfp = வெற்று;
    முழு		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, idx)
	எனில் (qfp->qf_fnum == bnr)
	    நிறுத்து;

    *errornr = idx;
    திரும்பு qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    நிலையான qfline_T *
qf_find_first_entry_on_வரி(qfline_T *entry, முழு *errornr)
{
    போது (!got_int
	    && entry->qf_prev != வெற்று
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    திரும்பு entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    நிலையான qfline_T *
qf_find_last_entry_on_வரி(qfline_T *entry, முழு *errornr)
{
    போது (!got_int &&
	    entry->qf_next != வெற்று
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    திரும்பு entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    நிலையான முழு
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, முழு linewise)
{
    எனில் (linewise)
	திரும்பு qfp->qf_lnum > pos->lnum;
    இல்லையெனில்
	திரும்பு (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    நிலையான முழு
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, முழு linewise)
{
    எனில் (linewise)
	திரும்பு qfp->qf_lnum < pos->lnum;
    இல்லையெனில்
	திரும்பு (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    நிலையான முழு
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, முழு linewise)
{
    எனில் (linewise)
	திரும்பு qfp->qf_lnum >= pos->lnum;
    இல்லையெனில்
	திரும்பு (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    நிலையான முழு
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, முழு linewise)
{
    எனில் (linewise)
	திரும்பு qfp->qf_lnum <= pos->lnum;
    இல்லையெனில்
	திரும்பு (qfp->qf_lnum < pos->lnum ||
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
    நிலையான qfline_T *
qf_find_entry_after_pos(
	முழு		bnr,
	pos_T		*pos,
	முழு		linewise,
	qfline_T	*qfp,
	முழு		*errornr)
{
    எனில் (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	திரும்பு qfp;

    // Find the entry just before or at the position 'pos'
    போது (qfp->qf_next != வெற்று
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    எனில் (qfp->qf_next == வெற்று || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	திரும்பு வெற்று;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    திரும்பு qfp;
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
    நிலையான qfline_T *
qf_find_entry_before_pos(
	முழு		bnr,
	pos_T		*pos,
	முழு		linewise,
	qfline_T	*qfp,
	முழு		*errornr)
{
    // Find the entry just before the position 'pos'
    போது (qfp->qf_next != வெற்று
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    எனில் (qf_entry_on_or_after_pos(qfp, pos, linewise))
	திரும்பு வெற்று;

    எனில் (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_வரி(qfp, errornr);

    திரும்பு qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    நிலையான qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	முழு		bnr,
	pos_T		*pos,
	முழு		dir,
	முழு		linewise,
	முழு		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    எனில் (qfp == வெற்று)
	திரும்பு வெற்று;		// no entry in this file

    எனில் (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    இல்லையெனில்
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    திரும்பு qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    நிலையான வெறுமை
qf_get_nth_below_entry(qfline_T *entry_arg, முழு n, முழு linewise, முழு *errornr)
{
    qfline_T *entry = entry_arg;

    போது (n-- > 0 && !got_int)
    {
	முழு		first_errornr = *errornr;

	எனில் (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_வரி(entry, errornr);

	எனில் (entry->qf_next == வெற்று
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    எனில் (linewise)
		*errornr = first_errornr;
	    நிறுத்து;
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
    நிலையான வெறுமை
qf_get_nth_above_entry(qfline_T *entry, முழு n, முழு linewise, முழு *errornr)
{
    போது (n-- > 0 && !got_int)
    {
	எனில் (entry->qf_prev == வெற்று
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    நிறுத்து;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	எனில் (linewise)
	    entry = qf_find_first_entry_on_வரி(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    நிலையான முழு
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	முழு		bnr,
	pos_T		*pos,
	முழு		n,
	முழு		dir,
	முழு		linewise)
{
    qfline_T	*adj_entry;
    முழு		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    எனில் (adj_entry == வெற்று)
	திரும்பு 0;

    எனில் (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	எனில் (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	இல்லையெனில்
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    திரும்பு errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    வெறுமை
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    முழு		dir;
    முழு		buf_has_flag;
    முழு		errornr = 0;
    pos_T	pos;

    எனில் (eap->addr_மொத்தம் > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	திரும்பு;
    }

    // Check whether the current buffer has any quickfix entries
    எனில் (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    இல்லையெனில்
	buf_has_flag = BUF_HAS_LL_ENTRY;
    எனில் (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	திரும்பு;
    }

    எனில் ((qi = qf_cmd_get_அடுக்கு(eap, TRUE)) == வெற்று)
	திரும்பு;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    எனில் (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	திரும்பு;
    }

    எனில் (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    இல்லையெனில்
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_மொத்தம் > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    எனில் (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    இல்லையெனில்
	emsg(_(e_no_more_பொருட்கள்));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    நிலையான char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    தேர்வு (cmdidx)
    {
	நிலை CMD_cfile:	    திரும்பு (char_u *)"cfile";
	நிலை CMD_cgetfile:  திரும்பு (char_u *)"cgetfile";
	நிலை CMD_caddfile:  திரும்பு (char_u *)"caddfile";
	நிலை CMD_lfile:	    திரும்பு (char_u *)"lfile";
	நிலை CMD_lgetfile:  திரும்பு (char_u *)"lgetfile";
	நிலை CMD_laddfile:  திரும்பு (char_u *)"laddfile";
	இயல்பு:	    திரும்பு வெற்று;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    வெறுமை
ex_cfile(exarg_T *eap)
{
    char_u	*enc = வெற்று;
    win_T	*wp = வெற்று;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = வெற்று;
    int_u	save_qfid = 0;		// init for gcc
    முழு		res;

    au_name = cfile_get_auname(eap->cmdidx);
    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							வெற்று, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	எனில் (aborting())
	    திரும்பு;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   வெற்று, வெற்று,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), வெற்று);
	எனில் (browse_file == வெற்று)
	    திரும்பு;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    இல்லையெனில்
# endif
    எனில் (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    எனில் (is_loclist_cmd(eap->cmdidx))
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
    எனில் (wp != வெற்று)
    {
	qi = GET_LOC_LIST(wp);
	எனில் (qi == வெற்று)
	{
	    decr_quickfix_busy();
	    திரும்பு;
	}
    }
    இல்லையெனில் எனில் (qi == வெற்று)
    {
	decr_quickfix_busy();
	திரும்பு;
    }
    எனில் (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    எனில் (au_name != வெற்று)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, வெற்று, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    எனில் (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    நிலையான char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    தேர்வு (cmdidx)
    {
	நிலை CMD_vimgrep:     திரும்பு (char_u *)"vimgrep";
	நிலை CMD_lvimgrep:    திரும்பு (char_u *)"lvimgrep";
	நிலை CMD_vimgrepadd:  திரும்பு (char_u *)"vimgrepadd";
	நிலை CMD_lvimgrepadd: திரும்பு (char_u *)"lvimgrepadd";
	நிலை CMD_grep:	      திரும்பு (char_u *)"grep";
	நிலை CMD_lgrep:	      திரும்பு (char_u *)"lgrep";
	நிலை CMD_grepadd:     திரும்பு (char_u *)"grepadd";
	நிலை CMD_lgrepadd:    திரும்பு (char_u *)"lgrepadd";
	இயல்பு: திரும்பு வெற்று;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    நிலையான வெறுமை
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = வெற்று;

    எனில் (s == வெற்று || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	எனில் (last_தேடல்_pat() == வெற்று)
	{
	    emsg(_(e_no_previous_regular_expression));
	    திரும்பு;
	}
	regmatch->regprog = vim_regcomp(last_தேடல்_pat(), RE_MAGIC);
    }
    இல்லையெனில்
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    நிலையான வெறுமை
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    செய்தி_start();
    p = செய்தி_strtrunc(fname, TRUE);
    எனில் (p == வெற்று)
	செய்தி_outtrans(fname);
    இல்லையெனில்
    {
	செய்தி_outtrans(p);
	vim_free(p);
    }
    செய்தி_clr_eos();
    செய்தி_didout = FALSE;	    // overwrite this message
    செய்தி_nowait = TRUE;	    // don't wait for this message
    செய்தி_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    நிலையான buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    முழு		save_mls;
# எனில் defined(FEAT_SYN_HL)
    char_u	*save_ei = வெற்று;
# endif
    buf_T	*buf;

# எனில் defined(FEAT_SYN_HL)
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
# எனில் defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    திரும்பு buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    நிலையான முழு
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    எனில் (!qflist_valid(wp, qfid))
    {
	எனில் (wp != வெற்று)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    திரும்பு FALSE;
	}
	இல்லையெனில்
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    திரும்பு TRUE;
	}
    }

    எனில் (qf_restore_list(qi, qfid) == FAIL)
	திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    நிலையான முழு
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	முழு	    duplicate_name,
	முழு	    flags)
{
    முழு		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    முழு		pat_len = (முழு)STRLEN(spat);
    எனில் (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    க்கு (lnum = 1; lnum <= buf->b_ml.ml_வரி_மொத்தம் && *tomatch > 0; ++lnum)
    {
	col = 0;
	எனில் (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    போது (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, வெற்று) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		எனில் (qf_add_entry(qfl,
			    வெற்று,	// dir
			    fname,
			    வெற்று,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    வெற்று,	// search pattern
			    0,		// nr
			    0,		// type
			    வெற்று,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    நிறுத்து;
		}
		found_match = TRUE;
		எனில் (--*tomatch == 0)
		    நிறுத்து;
		எனில் ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    நிறுத்து;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		எனில் (col > ml_get_buf_len(buf, lnum))
		    நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    முழு	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    போது (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		எனில் (qf_add_entry(qfl,
			    வெற்று,	// dir
			    fname,
			    வெற்று,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    வெற்று,	// search pattern
			    0,		// nr
			    0,		// type
			    வெற்று,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    நிறுத்து;
		}
		found_match = TRUE;
		எனில் (--*tomatch == 0)
		    நிறுத்து;
		எனில் ((flags & VGR_GLOBAL) == 0)
		    நிறுத்து;
		col = matches[pat_len - 1] + col + 1;
		எனில் (col > linelen)
		    நிறுத்து;
	    }
	}
	வரி_breakcheck();
	எனில் (got_int)
	    நிறுத்து;
    }

    திரும்பு found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    நிலையான வெறுமை
vgr_jump_to_match(
	qf_info_T   *qi,
	முழு	    forceit,
	முழு	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    எனில் (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    எனில் (curbuf == first_match_buf && target_dir != வெற்று)
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
வகைவரை கட்டமைப்பு
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    முழு		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    முழு		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    நிலையான முழு
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = வெற்று;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    எனில் (eap->addr_மொத்தம் > 0)
	args->tomatch = eap->line2;
    இல்லையெனில்
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    எனில் (p == வெற்று)
    {
	emsg(_(e_invalid_தேடல்_pattern_or_delimiter));
	திரும்பு FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    எனில் (args->regmatch.regprog == வெற்று)
	திரும்பு FAIL;

    p = skipwhite(p);
    எனில் (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	திரும்பு FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    எனில் ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    நிலையான முழு
existing_swapfile(buf_T *buf)
{
    எனில் (buf->b_ml.ml_mfp != வெற்று && buf->b_ml.ml_mfp->mf_fname != வெற்று)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	திரும்பு fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    திரும்பு FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    நிலையான முழு
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	முழு		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    முழு		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    முழு		fi;
    buf_T	*buf;
    முழு		duplicate_name = FALSE;
    முழு		using_dummy;
    char_u	*dirname_start = வெற்று;
    char_u	*dirname_now = வெற்று;
    முழு		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    எனில் (dirname_start == வெற்று || dirname_now == வெற்று)
	செல் theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    க்கு (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	எனில் (time(வெற்று) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(வெற்று);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != வெற்று);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	இல்லையெனில்
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	எனில் (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    செல் theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	எனில் (buf == வெற்று)
	{
	    எனில் (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	இல்லையெனில்
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    எனில் (using_dummy)
	    {
		எனில் (found_match && *first_match_buf == வெற்று)
		    *first_match_buf = buf;
		எனில் (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = வெற்று;
		}
		இல்லையெனில் எனில் ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    எனில் (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = வெற்று;
		    }
		    இல்லையெனில் எனில் (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = வெற்று;
		    }
		}

		எனில் (buf != வெற்று)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    எனில் (buf == *first_match_buf
			    && *target_dir == வெற்று
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    எனில் (curbuf == buf)
		    {
# எனில் defined(FEAT_SYN_HL)
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
    திரும்பு status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    வெறுமை
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = வெற்று;
    முழு		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = வெற்று;
    char_u	*target_dir = வெற்று;
    char_u	*au_name =  வெற்று;
    முழு		status;

    எனில் (!check_can_set_curbuf_forceit(eap->forceit))
	திரும்பு;

    au_name = vgr_get_auname(eap->cmdidx);
    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	எனில் (aborting())
	    திரும்பு;
# endif
    }

    qi = qf_cmd_get_or_alloc_அடுக்கு(eap, &wp);
    எனில் (qi == வெற்று)
	திரும்பு;

    எனில் (vgr_process_args(eap, &args) == FAIL)
	செல் theend;

    எனில் ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_அடுக்கு_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    எனில் (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	செல் theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, வெற்று);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    எனில் (au_name != வெற்று)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    எனில் (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	செல் theend;
    }

    // Jump to first match.
    எனில் (!qf_list_empty(qf_get_curlist(qi)))
    {
	எனில் ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    இல்லையெனில்
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    எனில் (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# இல்லையெனில்
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
    நிலையான வெறுமை
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    எனில் (dirname_now == வெற்று)
	திரும்பு;

    mch_dirname(dirname_now, MAXPATHL);
    எனில் (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == வெற்று) ? CMD_cd : CMD_lcd;
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
    நிலையான buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    முழு		failed = TRUE;
    aco_save_T	aco;
    முழு		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(வெற்று, வெற்று, (linenr_T)1, BLN_DUMMY);
    எனில் (newbuf == வெற்று)
	திரும்பு வெற்று;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    எனில் (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	எனில் (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (வெறுமை)setfname(curbuf, fname, வெற்று, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = வெற்று;
	    readfile_result = readfile(fname, வெற்று,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			வெற்று, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    எனில் (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		எனில் (curbuf != newbuf)
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

	    எனில் (newbuf_to_wipe.br_buf != வெற்று && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, வெற்று);
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

    எனில் (!bufref_valid(&newbufref))
	திரும்பு வெற்று;
    எனில் (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	திரும்பு வெற்று;
    }
    திரும்பு newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    நிலையான வெறுமை
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    போது (buf->b_nwindows > 0)
    {
	முழு	    did_one = FALSE;
	win_T	    *wp;

	எனில் (firstwin->w_next != வெற்று)
	    FOR_ALL_WINDOWS(wp)
		எனில் (wp->w_buffer == buf)
		{
		    எனில் (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    நிறுத்து;
		}
	எனில் (!did_one)
	    செல் fail;
    }

    எனில் (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# எனில் defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# எனில் defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	எனில் (dirname_start != வெற்று)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	திரும்பு;
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
    நிலையான வெறுமை
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    எனில் (curbuf == buf)		// safety check
	திரும்பு;

    close_buffer(வெற்று, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# எனில் defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    நிலையான முழு
get_qfline_பொருட்கள்(qfline_T *qfp, list_T *list)
{
    முழு		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    எனில் (bufnum != 0 && (buflist_findnr(bufnum) == வெற்று))
	bufnum = 0;

    எனில் ((dict = dict_alloc()) == வெற்று)
	திரும்பு FAIL;
    எனில் (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	திரும்பு FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    எனில் (dict_add_எண்(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_எண்(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_எண்(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_எண்(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_எண்(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_எண்(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_எண்(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (முழு)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_எண்(dict, "valid", (long)qfp->qf_valid) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    நிலையான முழு
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	முழு		qf_idx,
	முழு		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    முழு		i;

    எனில் (qi == வெற்று)
    {
	qi = ql_info;
	எனில் (wp != வெற்று)
	    qi = GET_LOC_LIST(wp);
	எனில் (qi == வெற்று)
	    திரும்பு FAIL;

    }

    எனில் (eidx < 0)
	திரும்பு OK;

    எனில் (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    எனில் (qf_idx >= qi->qf_listcount)
	திரும்பு FAIL;

    qfl = qf_get_list(qi, qf_idx);
    எனில் (qf_list_empty(qfl))
	திரும்பு FAIL;

    FOR_ALL_QFL_பொருட்கள்(qfl, qfp, i)
    {
	எனில் (eidx > 0)
	{
	    எனில் (eidx == i)
		திரும்பு get_qfline_பொருட்கள்(qfp, list);
	}
	இல்லையெனில் எனில் (get_qfline_பொருட்கள்(qfp, list) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
எண்ணிக்கை {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_பொருட்கள்	= 0x2,
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
    நிலையான முழு
qf_get_list_from_வரிகள்(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    முழு		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    எனில் (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == வெற்று)
	திரும்பு FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    எனில் ((efm_di = dict_find(what, (char_u *)"efm", -1)) != வெற்று)
    {
	எனில் (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == வெற்று)
	    திரும்பு FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு FAIL;

    qi = qf_alloc_அடுக்கு(QFLT_INTERNAL, 1);
    எனில் (qi != வெற்று)
    {
	எனில் (qf_init_ext(qi, 0, வெற்று, வெற்று, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, வெற்று, வெற்று) > 0)
	{
	    (வெறுமை)get_errorlist(qi, வெற்று, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    திரும்பு status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    நிலையான முழு
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    எனில் (qi == வெற்று)
	திரும்பு 0;
    win = qf_find_win(qi);
    எனில் (win != வெற்று)
	திரும்பு win->w_id;
    திரும்பு 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    நிலையான முழு
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    முழு	bufnum = 0;

    எனில் (qi != வெற்று && buflist_findnr(qi->qf_bufnr) != வெற்று)
	bufnum = qi->qf_bufnr;

    திரும்பு dict_add_எண்(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    நிலையான முழு
qf_getprop_keys2flags(dict_T *what, முழு loclist)
{
    முழு		flags = QF_GETLIST_NONE;

    எனில் (dict_has_சாவி(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	எனில் (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    எனில் (dict_has_சாவி(what, "title"))
	flags |= QF_GETLIST_TITLE;

    எனில் (dict_has_சாவி(what, "nr"))
	flags |= QF_GETLIST_NR;

    எனில் (dict_has_சாவி(what, "winid"))
	flags |= QF_GETLIST_WINID;

    எனில் (dict_has_சாவி(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    எனில் (dict_has_சாவி(what, "id"))
	flags |= QF_GETLIST_ID;

    எனில் (dict_has_சாவி(what, "items"))
	flags |= QF_GETLIST_பொருட்கள்;

    எனில் (dict_has_சாவி(what, "idx"))
	flags |= QF_GETLIST_IDX;

    எனில் (dict_has_சாவி(what, "size"))
	flags |= QF_GETLIST_SIZE;

    எனில் (dict_has_சாவி(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    எனில் (loclist && dict_has_சாவி(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    எனில் (dict_has_சாவி(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    எனில் (dict_has_சாவி(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    திரும்பு flags;
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
    நிலையான முழு
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    முழு		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    எனில் ((di = dict_find(what, (char_u *)"nr", -1)) != வெற்று)
    {
	// Use the specified quickfix/location list
	எனில் (di->di_tv.v_type == VAR_எண்)
	{
	    // for zero use the current list
	    எனில் (di->di_tv.vval.v_எண் != 0)
	    {
		qf_idx = di->di_tv.vval.v_எண் - 1;
		எனில் (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	இல்லையெனில் எனில் (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != வெற்று
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	இல்லையெனில்
	    qf_idx = INVALID_QFIDX;
    }

    எனில் ((di = dict_find(what, (char_u *)"id", -1)) != வெற்று)
    {
	// Look for a list with the specified id
	எனில் (di->di_tv.v_type == VAR_எண்)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    எனில் (di->di_tv.vval.v_எண் != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_எண்);
	}
	இல்லையெனில்
	    qf_idx = INVALID_QFIDX;
    }

    திரும்பு qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    நிலையான முழு
qf_getprop_defaults(qf_info_T *qi, முழு flags, முழு locstack, dict_T *retdict)
{
    முழு		status = OK;

    எனில் (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_பொருட்கள்))
    {
	list_T	*l = list_alloc();
	எனில் (l != வெற்று)
	    status = dict_add_list(retdict, "items", l);
	இல்லையெனில்
	    status = FAIL;
    }
    எனில் ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_எண்(retdict, "nr", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_எண்(retdict, "winid", qf_winid(qi));
    எனில் ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_எண்(retdict, "id", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_எண்(retdict, "idx", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_எண்(retdict, "size", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_எண்(retdict, "changedtick", 0);
    எனில் ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_எண்(retdict, "filewinid", 0);
    எனில் ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    திரும்பு status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    நிலையான முழு
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    திரும்பு dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    நிலையான முழு
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    முழு winid = 0;

    எனில் (wp != வெற்று && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	எனில் (ll_wp != வெற்று)
	    winid = ll_wp->w_id;
    }

    திரும்பு dict_add_எண்(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    நிலையான முழு
qf_getprop_பொருட்கள்(qf_info_T *qi, முழு qf_idx, முழு eidx, dict_T *retdict)
{
    முழு		status = OK;
    list_T	*l = list_alloc();
    எனில் (l != வெற்று)
    {
	(வெறுமை)get_errorlist(qi, வெற்று, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    இல்லையெனில்
	status = FAIL;

    திரும்பு status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    நிலையான முழு
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    முழு		status;
    dictitem_T	*di;

    எனில் (qfl->qf_ctx != வெற்று)
    {
	di = dictitem_alloc((char_u *)"context");
	எனில் (di != வெற்று)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    எனில் (status == FAIL)
		dictitem_free(di);
	}
	இல்லையெனில்
	    status = FAIL;
    }
    இல்லையெனில்
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    திரும்பு status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    நிலையான முழு
qf_getprop_idx(qf_list_T *qfl, முழு eidx, dict_T *retdict)
{
    எனில் (eidx == 0)
    {
	eidx = qfl->qf_index;
	எனில் (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    திரும்பு dict_add_எண்(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    நிலையான முழு
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    முழு		status;

    எனில் (qfl->qf_qftf_cb.cb_name != வெற்று)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    இல்லையெனில்
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    திரும்பு status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    நிலையான முழு
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    முழு		status = OK;
    முழு		qf_idx = INVALID_QFIDX;
    முழு		eidx = 0;
    dictitem_T	*di;
    முழு		flags = QF_GETLIST_NONE;

    எனில் ((di = dict_find(what, (char_u *)"lines", -1)) != வெற்று)
	திரும்பு qf_get_list_from_வரிகள்(what, di, retdict);

    எனில் (wp != வெற்று)
	qi = GET_LOC_LIST(wp);
    இல்லையெனில் எனில் (qi == வெற்று)
	திரும்பு FAIL;

    flags = qf_getprop_keys2flags(what, (wp != வெற்று));

    எனில் (!qf_அடுக்கு_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    எனில் (qf_அடுக்கு_empty(qi) || qf_idx == INVALID_QFIDX)
	திரும்பு qf_getprop_defaults(qi, flags, wp != வெற்று, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    எனில் ((di = dict_find(what, (char_u *)"idx", -1)) != வெற்று)
    {
	எனில் (di->di_tv.v_type != VAR_எண்)
	    திரும்பு FAIL;
	eidx = di->di_tv.vval.v_எண்;
    }

    எனில் (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_எண்(retdict, "nr", qf_idx + 1);
    எனில் ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_எண்(retdict, "winid", qf_winid(qi));
    எனில் ((status == OK) && (flags & QF_GETLIST_பொருட்கள்))
	status = qf_getprop_பொருட்கள்(qi, qf_idx, eidx, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_எண்(retdict, "id", qfl->qf_id);
    எனில் ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_எண்(retdict, "size", qfl->qf_மொத்தம்);
    எனில் ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_எண்(retdict, "changedtick", qfl->qf_changedtick);
    எனில் ((status == OK) && (wp != வெற்று) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    எனில் ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    திரும்பு status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    நிலையான முழு
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	முழு		first_entry,
	முழு		*valid_entry)
{
    நிலையான முழு	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    முழு		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    எனில் (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (முழு)dict_get_எண்(d, "bufnr");
    lnum = (முழு)dict_get_எண்(d, "lnum");
    end_lnum = (முழு)dict_get_எண்(d, "end_lnum");
    col = (முழு)dict_get_எண்(d, "col");
    end_col = (முழு)dict_get_எண்(d, "end_col");
    vcol = (முழு)dict_get_எண்(d, "vcol");
    nr = (முழு)dict_get_எண்(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    எனில் (text == வெற்று)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    எனில் ((filename == வெற்று && bufnum == 0) || (lnum == 0 && pattern == வெற்று))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    எனில் (bufnum != 0 && (buflist_findnr(bufnum) == வெற்று))
    {
	எனில் (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    எனில் (dict_has_சாவி(d, "valid"))
	valid = (முழு)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			வெற்று,		// dir
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
			type == வெற்று ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    எனில் (valid)
	*valid_entry = TRUE;

    திரும்பு status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    நிலையான முழு
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	முழு		target_fnum,
	முழு		target_lnum,
	முழு		target_col)
{
    // First, compare entries to target file.
    எனில் (!target_fnum)
	// Without a target file, we can't know which is closer.
	திரும்பு FALSE;

    முழு is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    முழு other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    எனில் (!is_target_file && other_is_target_file)
	திரும்பு FALSE;
    இல்லையெனில் எனில் (is_target_file && !other_is_target_file)
	திரும்பு TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    எனில் (!target_lnum)
	// Without a target line number, we can't know which is closer.
	திரும்பு FALSE;

    முழு வரி_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    முழு other_வரி_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    எனில் (வரி_distance > other_வரி_distance)
	திரும்பு FALSE;
    இல்லையெனில் எனில் (வரி_distance < other_வரி_distance)
	திரும்பு TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    எனில் (!target_col)
	// Without a target column, we can't know which is closer.
	திரும்பு FALSE;

    முழு column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    முழு other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    எனில் (column_distance > other_column_distance)
	திரும்பு FALSE;
    இல்லையெனில் எனில் (column_distance < other_column_distance)
	திரும்பு TRUE;

    // It's a complete tie! The exact same file, line, and column.
    திரும்பு FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    நிலையான முழு
qf_add_entries(
	qf_info_T	*qi,
	முழு		qf_idx,
	list_T		*list,
	char_u		*title,
	முழு		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = வெற்று;
    முழு		retval = OK;
    முழு		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    முழு prev_fnum = 0;
    முழு prev_lnum = 0;
    முழு prev_col = 0;
    எனில் (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    முழு select_first_entry = FALSE;
    முழு select_nearest_entry = FALSE;

    எனில் (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    இல்லையெனில் எனில் (action == 'a')
    {
	எனில் (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	இல்லையெனில்
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    இல்லையெனில் எனில் (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_பொருட்கள்(qfl);
	qf_store_title(qfl, title);
    }
    இல்லையெனில் எனில் (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_பொருட்கள்(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = வெற்று;
    முழு entry_to_select_index = 0;

    FOR_ALL_LIST_பொருட்கள்(list, li)
    {
	எனில் (li->li_tv.v_type != VAR_DICT)
	    தொடர்; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	எனில் (d == வெற்று)
	    தொடர்;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	எனில் (retval == QF_FAIL)
	    நிறுத்து;

	qfline_T *entry = qfl->qf_last;
	எனில் (
	    (select_first_entry && entry_to_select == வெற்று) ||
	    (select_nearest_entry &&
		(entry_to_select == வெற்று ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_மொத்தம்;
	}
    }

    // Check if any valid error entries are added to the list.
    எனில் (valid_entry)
	qfl->qf_nonevalid = FALSE;
    இல்லையெனில் எனில் (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    எனில் (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    திரும்பு retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    நிலையான முழு
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	முழு		action,
	முழு		*newlist)
{
    dictitem_T	*di;
    முழு		qf_idx = qi->qf_curlist;    // default is the current list

    எனில் ((di = dict_find(what, (char_u *)"nr", -1)) != வெற்று)
    {
	// Use the specified quickfix/location list
	எனில் (di->di_tv.v_type == VAR_எண்)
	{
	    // for zero use the current list
	    எனில் (di->di_tv.vval.v_எண் != 0)
		qf_idx = di->di_tv.vval.v_எண் - 1;

	    எனில் ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_அடுக்கு_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    இல்லையெனில் எனில் (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		திரும்பு INVALID_QFIDX;
	    இல்லையெனில் எனில் (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	இல்லையெனில் எனில் (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != வெற்று
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    எனில் (!qf_அடுக்கு_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    இல்லையெனில் எனில் (*newlist)
		qf_idx = 0;
	    இல்லையெனில்
		திரும்பு INVALID_QFIDX;
	}
	இல்லையெனில்
	    திரும்பு INVALID_QFIDX;
    }

    எனில் (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != வெற்று)
    {
	// Use the quickfix/location list with the specified id
	எனில் (di->di_tv.v_type != VAR_எண்)
	    திரும்பு INVALID_QFIDX;

	திரும்பு qf_id2nr(qi, di->di_tv.vval.v_எண்);
    }

    திரும்பு qf_idx;
}

/*
 * Set the quickfix list title.
 */
    நிலையான முழு
qf_setprop_title(qf_info_T *qi, முழு qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    எனில் (di->di_tv.v_type != VAR_STRING)
	திரும்பு FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    எனில் (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    திரும்பு OK;
}

/*
 * Set quickfix list items/entries.
 */
    நிலையான முழு
qf_setprop_பொருட்கள்(qf_info_T *qi, முழு qf_idx, dictitem_T *di, முழு action)
{
    முழு		retval = FAIL;
    char_u	*title_save;

    எனில் (di->di_tv.v_type != VAR_LIST)
	திரும்பு FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    திரும்பு retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    நிலையான முழு
qf_setprop_பொருட்கள்_from_வரிகள்(
	qf_info_T	*qi,
	முழு		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	முழு		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    முழு		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    எனில் ((efm_di = dict_find(what, (char_u *)"efm", -1)) != வெற்று)
    {
	எனில் (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == வெற்று)
	    திரும்பு FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    எனில் (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == வெற்று)
	திரும்பு FAIL;

    எனில் (action == 'r' || action == 'u')
	qf_free_பொருட்கள்(&qi->qf_lists[qf_idx]);
    எனில் (qf_init_ext(qi, qf_idx, வெற்று, வெற்று, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, வெற்று, வெற்று) >= 0)
	retval = OK;

    திரும்பு retval;
}

/*
 * Set quickfix list context.
 */
    நிலையான முழு
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    எனில் (ctx != வெற்று)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    திரும்பு OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    நிலையான முழு
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    முழு		denote = FALSE;
    முழு		newidx;
    முழு		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    எனில் (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != வெற்று
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_மொத்தம்;
    இல்லையெனில்
    {
	// Otherwise use the specified index
	newidx = tv_get_எண்_chk(&di->di_tv, &denote);
	எனில் (denote)
	    திரும்பு FAIL;
    }

    எனில் (newidx < 1)		// sanity check
	திரும்பு FAIL;
    எனில் (newidx > qfl->qf_மொத்தம்)
	newidx = qfl->qf_மொத்தம்;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    எனில் (qf_ptr == வெற்று)
	திரும்பு FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    எனில் (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    திரும்பு OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    நிலையான முழு
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    எனில் (check_restricted() || check_secure())
	திரும்பு FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    எனில் (cb.cb_name == வெற்று || *cb.cb_name == NUL)
	திரும்பு OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    திரும்பு OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    நிலையான முழு
qf_set_properties(qf_info_T *qi, dict_T *what, முழு action, char_u *title)
{
    dictitem_T	*di;
    முழு		retval = FAIL;
    முழு		qf_idx;
    முழு		newlist = FALSE;
    qf_list_T	*qfl;

    எனில் (action == ' ' || qf_அடுக்கு_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    எனில் (qf_idx == INVALID_QFIDX)	// List not found
	திரும்பு FAIL;

    எனில் (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    எனில் ((di = dict_find(what, (char_u *)"title", -1)) != வெற்று)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    எனில் ((di = dict_find(what, (char_u *)"items", -1)) != வெற்று)
	retval = qf_setprop_பொருட்கள்(qi, qf_idx, di, action);
    எனில் ((di = dict_find(what, (char_u *)"lines", -1)) != வெற்று)
	retval = qf_setprop_பொருட்கள்_from_வரிகள்(qi, qf_idx, what, di, action);
    எனில் ((di = dict_find(what, (char_u *)"context", -1)) != வெற்று)
	retval = qf_setprop_context(qfl, di);
    எனில் ((di = dict_find(what, (char_u *)"idx", -1)) != வெற்று)
	retval = qf_setprop_curidx(qi, qfl, di);
    எனில் ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != வெற்று)
	retval = qf_setprop_qftf(qi, qfl, di);

    எனில் (newlist || retval == OK)
	qf_list_changed(qfl);
    எனில் (newlist)
	qf_update_buffer(qi, வெற்று);

    திரும்பு retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    நிலையான வெறுமை
qf_free_அடுக்கு(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = வெற்று;

    எனில் (qfwin != வெற்று)
    {
	// If the quickfix/location list window is open, then clear it
	எனில் (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, வெற்று);
    }

    எனில் (wp != வெற்று && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	எனில் (llwin != வெற்று)
	    wp = llwin;
    }

    qf_free_all(wp);
    எனில் (wp == வெற்று)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    இல்லையெனில் எனில் (qfwin != வெற்று)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_அடுக்கு(QFLT_LOCATION, wp->w_p_lhi);

	எனில் (new_ll != வெற்று)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    எனில் (wp != qfwin)
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
    முழு
set_errorlist(
	win_T	*wp,
	list_T	*list,
	முழு	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    முழு		retval = OK;

    எனில் (wp != வெற்று)
	qi = ll_get_or_alloc_list(wp);
    இல்லையெனில்
	qi = ql_info;
    எனில் (qi == வெற்று)
	திரும்பு FAIL;

    எனில் (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_அடுக்கு(wp, qi);
	திரும்பு OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    எனில் (list->lv_len != 0 && what != வெற்று)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	திரும்பு FAIL;
    }

    incr_quickfix_busy();

    எனில் (what != வெற்று)
	retval = qf_set_properties(qi, what, action, title);
    இல்லையெனில்
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	எனில் (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    திரும்பு retval;
}

நிலையான முழு mark_quickfix_user_data(qf_info_T *qi, முழு copyID)
{
    முழு abort = FALSE;
    க்கு (முழு i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	எனில் (!qfl->qf_has_user_data)
	    தொடர்;
	qfline_T *qfp;
	முழு j;
	FOR_ALL_QFL_பொருட்கள்(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    எனில் (user_data != வெற்று && user_data->v_type != VAR_எண்
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, வெற்று, வெற்று, வெற்று);
	}
    }
    திரும்பு abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    நிலையான முழு
mark_quickfix_ctx(qf_info_T *qi, முழு copyID)
{
    முழு		i;
    முழு		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    க்கு (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	எனில் (ctx != வெற்று && ctx->v_type != VAR_எண்
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, வெற்று, வெற்று, வெற்று);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    திரும்பு abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    முழு
set_ref_in_quickfix(முழு copyID)
{
    முழு		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    எனில் (ql_info == வெற்று)
	திரும்பு TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    எனில் (abort)
	திரும்பு abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    எனில் (abort)
	திரும்பு abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    எனில் (abort)
	திரும்பு abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	எனில் (win->w_llist != வெற்று)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    எனில் (abort)
		திரும்பு abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    எனில் (abort)
		திரும்பு abort;
	}
	எனில் (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    எனில் (abort)
		திரும்பு abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    எனில் (abort)
		திரும்பு abort;
	}
    }

    திரும்பு abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    நிலையான char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    தேர்வு (cmdidx)
    {
	நிலை CMD_cbuffer:	திரும்பு (char_u *)"cbuffer";
	நிலை CMD_cgetbuffer:	திரும்பு (char_u *)"cgetbuffer";
	நிலை CMD_caddbuffer:	திரும்பு (char_u *)"caddbuffer";
	நிலை CMD_lbuffer:	திரும்பு (char_u *)"lbuffer";
	நிலை CMD_lgetbuffer:	திரும்பு (char_u *)"lgetbuffer";
	நிலை CMD_laddbuffer:	திரும்பு (char_u *)"laddbuffer";
	இயல்பு:		திரும்பு வெற்று;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    நிலையான முழு
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = வெற்று;

    எனில் (*eap->arg == NUL)
	buf = curbuf;
    இல்லையெனில் எனில் (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((எழுத்து *)eap->arg));

    எனில் (buf == வெற்று)
    {
	emsg(_(e_invalid_argument));
	திரும்பு FAIL;
    }

    எனில் (buf->b_ml.ml_mfp == வெற்று)
    {
	emsg(_(e_buffer_is_not_loaded));
	திரும்பு FAIL;
    }

    எனில் (eap->addr_மொத்தம் == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_வரி_மொத்தம்;
    }

    எனில் (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_வரி_மொத்தம்
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_வரி_மொத்தம்)
    {
	emsg(_(e_invalid_range));
	திரும்பு FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    திரும்பு OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    வெறுமை
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = வெற்று;
    qf_info_T	*qi;
    char_u	*au_name = வெற்று;
    முழு		res;
    int_u	save_qfid;
    win_T	*wp = வெற்று;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	எனில் (aborting())
	    திரும்பு;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_அடுக்கு(eap, &wp);
    எனில் (qi == வெற்று)
	திரும்பு;

    எனில் (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	திரும்பு;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    எனில் (buf->b_sfname)
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%s (%s)",
		(எழுத்து *)qf_title, (எழுத்து *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, வெற்று, buf, வெற்று, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, வெற்று);
    எனில் (qf_அடுக்கு_empty(qi))
    {
	decr_quickfix_busy();
	திரும்பு;
    }
    எனில் (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    எனில் (au_name != வெற்று)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	எனில் (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    எனில் (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# எனில் defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    தேர்வு (cmdidx)
    {
	நிலை CMD_cexpr:	    திரும்பு (char_u *)"cexpr";
	நிலை CMD_cgetexpr:  திரும்பு (char_u *)"cgetexpr";
	நிலை CMD_caddexpr:  திரும்பு (char_u *)"caddexpr";
	நிலை CMD_lexpr:	    திரும்பு (char_u *)"lexpr";
	நிலை CMD_lgetexpr:  திரும்பு (char_u *)"lgetexpr";
	நிலை CMD_laddexpr:  திரும்பு (char_u *)"laddexpr";
	இயல்பு:	    திரும்பு வெற்று;
    }
}

    முழு
trigger_cexpr_autocmd(முழு cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	எனில் (aborting())
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

    முழு
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = வெற்று;

    qi = qf_cmd_get_or_alloc_அடுக்கு(eap, &wp);
    எனில் (qi == வெற்று)
	திரும்பு FAIL;

    எனில் ((tv->v_type == VAR_STRING && tv->vval.v_string != வெற்று)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != வெற்று))
    {
	முழு	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, வெற்று, வெற்று, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), வெற்று);
	எனில் (qf_அடுக்கு_empty(qi))
	{
	    decr_quickfix_busy();
	    திரும்பு FAIL;
	}
	எனில் (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	எனில் (au_name != வெற்று)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	எனில் (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	திரும்பு OK;
    }

    emsg(_(e_string_or_list_expected));
    திரும்பு FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    வெறுமை
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    எனில் (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	திரும்பு;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_கோவை(eap->arg, eap);
    எனில் (tv == வெற்று)
	திரும்பு;

    (வெறுமை)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    நிலையான qf_info_T *
hgr_get_ll(முழு *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    எனில் (bt_help(curwin->w_buffer))
	wp = curwin;
    இல்லையெனில்
	// Find an existing help window
	wp = qf_find_help_win();

    எனில் (wp == வெற்று)	    // Help window not found
	qi = வெற்று;
    இல்லையெனில்
	qi = wp->w_llist;

    எனில் (qi == வெற்று)
    {
	// Allocate a new location list for help text matches
	எனில் ((qi = qf_alloc_அடுக்கு(QFLT_LOCATION, 1)) == வெற்று)
	    திரும்பு வெற்று;
	*new_ll = TRUE;
    }

    திரும்பு qi;
}

/*
 * Search for a pattern in a help file.
 */
    நிலையான வெறுமை
hgr_தேடல்_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((எழுத்து *)fname, "r");
    எனில் (fd == வெற்று)
	திரும்பு;

    lnum = 1;
    போது (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *வரி = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	எனில் (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    வரி = string_convert(p_vc, IObuff, வெற்று);
	    எனில் (வரி == வெற்று)
		வரி = IObuff;
	}

	எனில் (vim_regexec(p_regmatch, வரி, (colnr_T)0))
	{
	    முழு	l = (முழு)STRLEN(வரி);

	    // remove trailing CR, LF, spaces, etc.
	    போது (l > 0 && வரி[l - 1] <= ' ')
		வரி[--l] = NUL;

	    எனில் (qf_add_entry(qfl,
			வெற்று,	// dir
			fname,
			வெற்று,
			0,
			வரி,
			lnum,
			0,
			(முழு)(p_regmatch->startp[0] - வரி)
			+ 1,	// col
			(முழு)(p_regmatch->endp[0] - வரி)
			+ 1,	// end_col
			FALSE,	// vis_col
			வெற்று,	// search pattern
			0,	// nr
			1,	// type
			வெற்று,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		எனில் (வரி != IObuff)
		    vim_free(வரி);
		நிறுத்து;
	    }
	}
	எனில் (வரி != IObuff)
	    vim_free(வரி);
	++lnum;
	வரி_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    நிலையான வெறுமை
hgr_தேடல்_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    முழு		fcount;
    char_u	**fnames;
    முழு		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    எனில் (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	க்கு (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    எனில் (lang != வெற்று
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		தொடர்;
# endif

	    hgr_தேடல்_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    நிலையான வெறுமை
hgr_தேடல்_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    எனில் (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    போது (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_தேடல்_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    எனில் (vc.vc_type != CONV_NONE)
	convert_setup(&vc, வெற்று, வெற்று);
}

/*
 * ":helpgrep {pattern}"
 */
    வெறுமை
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    முழு		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    முழு		new_qi = FALSE;
    char_u	*au_name =  வெற்று;
    char_u	*lang = வெற்று;
    முழு		updated = FALSE;

    தேர்வு (eap->cmdidx)
    {
	நிலை CMD_helpgrep:  au_name = (char_u *)"helpgrep"; நிறுத்து;
	நிலை CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; நிறுத்து;
	இயல்பு: நிறுத்து;
    }
    எனில் (au_name != வெற்று && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	எனில் (aborting())
	    திரும்பு;
# endif
    }

    எனில் (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	எனில் (qi == வெற்று)
	    திரும்பு;
    }
    இல்லையெனில் எனில் (qi == வெற்று)
    {
	emsg(_(e_no_quickfix_அடுக்கு));
	திரும்பு;
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
    எனில் (regmatch.regprog != வெற்று)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_தேடல்_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    எனில் (p_cpo == empty_option)
	p_cpo = save_cpo;
    இல்லையெனில்
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	எனில் (*p_cpo == NUL)
	    set_option_மதிப்பு_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	எனில் (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    எனில் (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, வெற்று);

    எனில் (au_name != வெற்று)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	எனில் (!new_qi && IS_LL_அடுக்கு(qi) && qf_find_win_with_loclist(qi) == வெற்று)
	{
	    decr_quickfix_busy();
	    திரும்பு;
	}
    }

    // Jump to first match.
    எனில் (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    இல்லையெனில்
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    எனில் (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	எனில் (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    எனில் (new_qi)
		ll_free_all(&qi);
	}
	இல்லையெனில் எனில் (curwin->w_llist == வெற்று && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# எனில் defined(EXITFREE)
    வெறுமை
free_quickfix(வெறுமை)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(வெற்று);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#எனில் defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    நிலையான வெறுமை
get_qf_loc_list(முழு is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    எனில் (what_arg->v_type == VAR_UNKNOWN)
    {
	எனில் (rettv_list_alloc(rettv) == OK)
	    எனில் (is_qf || wp != வெற்று)
		(வெறுமை)get_errorlist(வெற்று, wp, -1, 0, rettv->vval.v_list);
    }
    இல்லையெனில்
    {
	எனில் (rettv_dict_alloc(rettv) == OK)
	    எனில் (is_qf || (wp != வெற்று))
	    {
		எனில் (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    எனில் (d != வெற்று)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		இல்லையெனில்
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    வெறுமை
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    வெறுமை
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    எனில் (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    get_qf_loc_list(TRUE, வெற்று, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    நிலையான வெறுமை
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    முழு		action = 0;
    நிலையான முழு	recursive = 0;
# endif

    rettv->vval.v_எண் = -1;

# ifdef FEAT_QUICKFIX
    எனில் (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    இல்லையெனில் எனில் (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    இல்லையெனில்
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = வெற்று;
	முழு	valid_dict = TRUE;

	எனில் (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    எனில் (act == வெற்று)
		திரும்பு;		// type error; errmsg already given
	    எனில் ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    இல்லையெனில்
		semsg(_(e_invalid_action_str_1), act);
	}
	இல்லையெனில் எனில் (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	இல்லையெனில்
	    emsg(_(e_string_required));

	எனில் (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    எனில் (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != வெற்று)
		what = what_arg->vval.v_dict;
	    இல்லையெனில்
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	எனில் (l != வெற்று && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == வெற்று ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_எண் = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    வெறுமை
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	திரும்பு;

    win = find_win_by_nr_or_id(&argvars[0]);
    எனில் (win != வெற்று)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    வெறுமை
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	திரும்பு;

    set_qf_ll_list(வெற்று, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
