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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_QUICKFIX)

ഘടന dir_അട്ടി_T
{
    ഘടന dir_അട്ടി_T	*next;
    char_u		*dirname;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
തരനിർവചനം ഘടന qfline_S qfline_T;
ഘടന qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    പൂർണ്ണസംഖ്യ		qf_fnum;	// file number for the line
    പൂർണ്ണസംഖ്യ		qf_col;		// column where the error occurred
    പൂർണ്ണസംഖ്യ		qf_end_col;	// column when the error has range or zero
    പൂർണ്ണസംഖ്യ		qf_nr;		// error number
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
തരനിർവചനം എണ്ണൽ
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
തരനിർവചനം ഘടന qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    പൂർണ്ണസംഖ്യ		qf_എണ്ണം;	// number of errors (0 means empty list)
    പൂർണ്ണസംഖ്യ		qf_index;	// current index in the error list
    പൂർണ്ണസംഖ്യ		qf_nonevalid;	// TRUE if not a single valid entry found
    പൂർണ്ണസംഖ്യ		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    ഘടന dir_അട്ടി_T	*qf_dir_അട്ടി;
    char_u		*qf_directory;
    ഘടന dir_അട്ടി_T	*qf_file_അട്ടി;
    char_u		*qf_currfile;
    പൂർണ്ണസംഖ്യ			qf_multiline;
    പൂർണ്ണസംഖ്യ			qf_multiignore;
    പൂർണ്ണസംഖ്യ			qf_multiscan;
    long		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
ഘടന qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    പൂർണ്ണസംഖ്യ		qf_refcount;
    പൂർണ്ണസംഖ്യ		qf_listcount;	    // current number of lists
    പൂർണ്ണസംഖ്യ		qf_curlist;	    // current error list
    പൂർണ്ണസംഖ്യ         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    പൂർണ്ണസംഖ്യ		qf_bufnr;	    // quickfix window buffer number
};

സ്ഥിതം qf_info_T ql_info_actual; // global quickfix list
സ്ഥിതം qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
സ്ഥിതം int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
തരനിർവചനം ഘടന efm_S efm_T;
ഘടന efm_S
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
    പൂർണ്ണസംഖ്യ		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
തരനിർവചനം ഘടന qf_delq_S
{
    ഘടന qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
സ്ഥിതം qf_delq_T *qf_delq_head = ശൂന്യ;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
സ്ഥിതം പൂർണ്ണസംഖ്യ	quickfix_busy = 0;

സ്ഥിതം efm_T	*fmt_start = ശൂന്യ; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
സ്ഥിതം callback_T qftf_cb;

സ്ഥിതം ശൂന്യം     qf_എടുക്കുക_അട്ടി(qf_info_T *qi, പൂർണ്ണസംഖ്യ adjust);
സ്ഥിതം ശൂന്യം	qf_new_list(qf_info_T *qi, char_u *qf_title);
സ്ഥിതം പൂർണ്ണസംഖ്യ	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, പൂർണ്ണസംഖ്യ bufnum, char_u *mesg, long lnum, long end_lnum, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ end_col, പൂർണ്ണസംഖ്യ vis_col, char_u *pattern, പൂർണ്ണസംഖ്യ nr, പൂർണ്ണസംഖ്യ type, typval_T *user_data, പൂർണ്ണസംഖ്യ valid);
സ്ഥിതം പൂർണ്ണസംഖ്യ      qf_resize_അട്ടി_base(qf_info_T *qi, പൂർണ്ണസംഖ്യ n);
സ്ഥിതം ശൂന്യം     qf_sync_llw_to_win(win_T *llw);
സ്ഥിതം ശൂന്യം     qf_sync_win_to_llw(win_T *pwp);
സ്ഥിതം qf_info_T *qf_alloc_അട്ടി(qfltype_T qfltype, പൂർണ്ണസംഖ്യ n);
സ്ഥിതം qf_list_T *qf_alloc_list_അട്ടി(പൂർണ്ണസംഖ്യ n);
സ്ഥിതം ശൂന്യം	qf_free(qf_list_T *qfl);
സ്ഥിതം char_u	*qf_types(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
സ്ഥിതം char_u	*qf_തള്ളുക_dir(char_u *, ഘടന dir_അട്ടി_T **, പൂർണ്ണസംഖ്യ is_file_അട്ടി);
സ്ഥിതം char_u	*qf_എടുക്കുക_dir(ഘടന dir_അട്ടി_T **);
സ്ഥിതം char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
സ്ഥിതം win_T    *qf_find_win_with_loclist(qf_info_T *ll);
സ്ഥിതം ശൂന്യം	qf_jump_newwin(qf_info_T *qi, പൂർണ്ണസംഖ്യ dir, പൂർണ്ണസംഖ്യ errornr, പൂർണ്ണസംഖ്യ forceit, പൂർണ്ണസംഖ്യ newwin);
സ്ഥിതം ശൂന്യം	qf_fmt_text(garray_T *gap, char_u *text);
സ്ഥിതം ശൂന്യം	qf_range_text(garray_T *gap, qfline_T *qfp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	qf_win_pos_update(qf_info_T *qi, പൂർണ്ണസംഖ്യ old_qf_index);
സ്ഥിതം win_T	*qf_find_win(qf_info_T *qi);
സ്ഥിതം buf_T	*qf_find_buf(qf_info_T *qi);
സ്ഥിതം ശൂന്യം	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
സ്ഥിതം ശൂന്യം	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, പൂർണ്ണസംഖ്യ qf_winid);
സ്ഥിതം buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
സ്ഥിതം ശൂന്യം	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
സ്ഥിതം ശൂന്യം	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
സ്ഥിതം qf_info_T *ll_get_or_alloc_list(win_T *);
സ്ഥിതം പൂർണ്ണസംഖ്യ	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, പൂർണ്ണസംഖ്യ target_fnum, പൂർണ്ണസംഖ്യ target_lnum, പൂർണ്ണസംഖ്യ target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == ശൂന്യ)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != ശൂന്യ)

// Quickfix and location list stack check helper macros
# define IS_QF_അട്ടി(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_അട്ടി(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i) \
		    വേണ്ടി ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_എണ്ണം && (qfp) != ശൂന്യ; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
സ്ഥിതം char_u   *qf_last_bufname = ശൂന്യ;
സ്ഥിതം bufref_T  qf_last_bufref = {ശൂന്യ, 0, 0};

സ്ഥിതം garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    സ്ഥിതം garray_T *
qfga_get(ശൂന്യം)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ initialized = FALSE;

    എങ്കിൽ (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    തിരികെ &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    സ്ഥിതം ശൂന്യം
qfga_clear(ശൂന്യം)
{
    എങ്കിൽ (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    അല്ലെങ്കിൽ
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define വരി_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
സ്ഥിതം ഘടന fmtpattern
{
    char_u	convchar;
    അക്ഷരം	*pattern;
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
    സ്ഥിതം char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	പൂർണ്ണസംഖ്യ	idx,
	പൂർണ്ണസംഖ്യ	round)
{
    char_u	*srcptr;

    എങ്കിൽ (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != ശൂന്യ)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == ശൂന്യ))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	തിരികെ ശൂന്യ;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    എങ്കിൽ (*efmpat == 'f' && efmpat[1] != NUL)
    {
	എങ്കിൽ (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	അല്ലെങ്കിൽ
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    അല്ലെങ്കിൽ
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	ആകുമ്പോൾ ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    തിരികെ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    സ്ഥിതം char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	പൂർണ്ണസംഖ്യ	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    എങ്കിൽ (*efmp == '[' || *efmp == '\\')
    {
	എങ്കിൽ ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    എങ്കിൽ (efmp[1] == '^')
		*regpat++ = *++efmp;
	    എങ്കിൽ (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		ആകുമ്പോൾ (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		എങ്കിൽ (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    തിരികെ ശൂന്യ;
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    അല്ലെങ്കിൽ
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	തിരികെ ശൂന്യ;
    }

    *pefmp = efmp;

    തിരികെ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    സ്ഥിതം char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    എങ്കിൽ (vim_strchr((char_u *)"+-", *efmp) != ശൂന്യ)
	efminfo->flags = *efmp++;
    എങ്കിൽ (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != ശൂന്യ)
	efminfo->prefix = *efmp;
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	തിരികെ ശൂന്യ;
    }

    തിരികെ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
efm_to_regpat(
	char_u	*efm,
	പൂർണ്ണസംഖ്യ	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    പൂർണ്ണസംഖ്യ		round;
    പൂർണ്ണസംഖ്യ		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    വേണ്ടി (efmp = efm; efmp < efm + len; ++efmp)
    {
	എങ്കിൽ (*efmp == '%')
	{
	    ++efmp;
	    വേണ്ടി (idx = 0; idx < FMT_PATTERNS; ++idx)
		എങ്കിൽ (fmt_pat[idx].convchar == *efmp)
		    നിർത്തുക;
	    എങ്കിൽ (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		എങ്കിൽ (ptr == ശൂന്യ)
		    തിരികെ FAIL;
		round++;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		എങ്കിൽ (ptr == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"%\\.^$~[", *efmp) != ശൂന്യ)
		*ptr++ = *efmp;		// regexp magic characters
	    അല്ലെങ്കിൽ എങ്കിൽ (*efmp == '#')
		*ptr++ = '*';
	    അല്ലെങ്കിൽ എങ്കിൽ (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    അല്ലെങ്കിൽ എങ്കിൽ (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		എങ്കിൽ (efmp == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ			// copy normal character
	{
	    എങ്കിൽ (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)".*^$~[", *efmp) != ശൂന്യ)
		*ptr++ = '\\';	// escape regexp atoms
	    എങ്കിൽ (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    തിരികെ OK;
}

/*
 * Free the 'errorformat' information list
 */
    സ്ഥിതം ശൂന്യം
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    വേണ്ടി (efm_ptr = *efm_first; efm_ptr != ശൂന്യ; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = ശൂന്യ;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
efm_regpat_bufsz(char_u *efm)
{
    പൂർണ്ണസംഖ്യ sz;
    പൂർണ്ണസംഖ്യ i;

    sz = (FMT_PATTERNS * 3) + ((പൂർണ്ണസംഖ്യ)STRLEN(efm) << 2);
    വേണ്ടി (i = FMT_PATTERNS; i > 0; )
	sz += (പൂർണ്ണസംഖ്യ)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# അല്ലെങ്കിൽ
    sz += 2; // "%f" can become two chars longer
# endif

    തിരികെ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
efm_option_part_len(char_u *efm)
{
    പൂർണ്ണസംഖ്യ len;

    വേണ്ടി (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	എങ്കിൽ (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    തിരികെ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    സ്ഥിതം efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = ശൂന്യ;
    efm_T	*fmt_first = ശൂന്യ;
    efm_T	*fmt_last = ശൂന്യ;
    char_u	*fmtstr = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    എങ്കിൽ ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == ശൂന്യ)
	പോകുക parse_efm_error;

    ആകുമ്പോൾ (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	എങ്കിൽ (fmt_ptr == ശൂന്യ)
	    പോകുക parse_efm_error;
	എങ്കിൽ (fmt_first == ശൂന്യ)	    // first one
	    fmt_first = fmt_ptr;
	അല്ലെങ്കിൽ
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	എങ്കിൽ (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    പോകുക parse_efm_error;
	എങ്കിൽ ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == ശൂന്യ)
	    പോകുക parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    എങ്കിൽ (fmt_first == ശൂന്യ)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    പോകുക parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    തിരികെ fmt_first;
}

എണ്ണൽ {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_നിവേശം = 2,
    QF_NOMEM = 3,
    QF_IGNORE_വരി = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
തരനിർവചനം ഘടന {
    char_u	*linebuf;
    പൂർണ്ണസംഖ്യ		linelen;
    char_u	*growbuf;
    പൂർണ്ണസംഖ്യ		growbufsiz;
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
    സ്ഥിതം char_u *
qf_grow_linebuf(qfstate_T *state, പൂർണ്ണസംഖ്യ newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > വരി_MAXLEN ? വരി_MAXLEN - 1 : newsz;
    എങ്കിൽ (state->growbuf == ശൂന്യ)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	എങ്കിൽ (state->growbuf == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	state->growbufsiz = state->linelen;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (state->linelen > state->growbufsiz)
    {
	എങ്കിൽ ((p = vim_realloc(state->growbuf, state->linelen + 1)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    തിരികെ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_next_str_വരി(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (*p_str == NUL) // Reached the end of the string
	തിരികെ QF_END_OF_നിവേശം;

    p = vim_strchr(p_str, '\n');
    എങ്കിൽ (p != ശൂന്യ)
	len = (പൂർണ്ണസംഖ്യ)(p - p_str) + 1;
    അല്ലെങ്കിൽ
	len = (പൂർണ്ണസംഖ്യ)STRLEN(p_str);

    എങ്കിൽ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	എങ്കിൽ (state->linebuf == ശൂന്യ)
	    തിരികെ QF_NOMEM;
    }
    അല്ലെങ്കിൽ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    തിരികെ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_next_list_വരി(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    പൂർണ്ണസംഖ്യ		len;

    ആകുമ്പോൾ (p_li != ശൂന്യ
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == ശൂന്യ))
	p_li = p_li->li_next;	// Skip non-string items

    എങ്കിൽ (p_li == ശൂന്യ)		// End of the list
    {
	state->p_li = ശൂന്യ;
	തിരികെ QF_END_OF_നിവേശം;
    }

    len = (പൂർണ്ണസംഖ്യ)STRLEN(p_li->li_tv.vval.v_string);
    എങ്കിൽ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	എങ്കിൽ (state->linebuf == ശൂന്യ)
	    തിരികെ QF_NOMEM;
    }
    അല്ലെങ്കിൽ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    തിരികെ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_next_buf_വരി(qfstate_T *state)
{
    char_u	*p_buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;

    // Get the next line from the supplied buffer
    എങ്കിൽ (state->buflnum > state->lnumlast)
	തിരികെ QF_END_OF_നിവേശം;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    എങ്കിൽ (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	എങ്കിൽ (state->linebuf == ശൂന്യ)
	    തിരികെ QF_NOMEM;
    }
    അല്ലെങ്കിൽ
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    തിരികെ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_next_file_വരി(qfstate_T *state)
{
    പൂർണ്ണസംഖ്യ	    discard;
    പൂർണ്ണസംഖ്യ	    growbuflen;

    എങ്കിൽ (fgets((അക്ഷരം *)IObuff, IOSIZE, state->fd) == ശൂന്യ)
	തിരികെ QF_END_OF_നിവേശം;

    discard = FALSE;
    state->linelen = (പൂർണ്ണസംഖ്യ)STRLEN(IObuff);
    എങ്കിൽ (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	എങ്കിൽ (state->growbuf == ശൂന്യ)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    എങ്കിൽ (state->growbuf == ശൂന്യ)
		തിരികെ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	memcpy(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	വേണ്ടി (;;)
	{
	    char_u	*p;

	    എങ്കിൽ (fgets((അക്ഷരം *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == ശൂന്യ)
		നിർത്തുക;
	    state->linelen = (പൂർണ്ണസംഖ്യ)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    എങ്കിൽ ((state->growbuf)[growbuflen - 1] == '\n')
		നിർത്തുക;
	    എങ്കിൽ (state->growbufsiz == വരി_MAXLEN)
	    {
		discard = TRUE;
		നിർത്തുക;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < വരി_MAXLEN
		? 2 * state->growbufsiz : വരി_MAXLEN;
	    എങ്കിൽ ((p = vim_realloc(state->growbuf, state->growbufsiz)) == ശൂന്യ)
		തിരികെ QF_NOMEM;
	    state->growbuf = p;
	}

	ആകുമ്പോൾ (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    എങ്കിൽ (fgets((അക്ഷരം *)IObuff, IOSIZE, state->fd) == ശൂന്യ
		    || (പൂർണ്ണസംഖ്യ)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		നിർത്തുക;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    അല്ലെങ്കിൽ
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    എങ്കിൽ (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*വരി;

	വരി = string_convert(&state->vc, state->linebuf, &state->linelen);
	എങ്കിൽ (വരി != ശൂന്യ)
	{
	    എങ്കിൽ (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, വരി);
		vim_free(വരി);
	    }
	    അല്ലെങ്കിൽ
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = വരി;
		state->growbufsiz = state->linelen < വരി_MAXLEN
						? state->linelen : വരി_MAXLEN;
	    }
	}
    }

    തിരികെ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_nextline(qfstate_T *state)
{
    പൂർണ്ണസംഖ്യ status = QF_FAIL;

    എങ്കിൽ (state->fd == ശൂന്യ)
    {
	എങ്കിൽ (state->tv != ശൂന്യ)
	{
	    എങ്കിൽ (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_വരി(state);
	    അല്ലെങ്കിൽ എങ്കിൽ (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_വരി(state);
	}
	അല്ലെങ്കിൽ
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_വരി(state);
    }
    അല്ലെങ്കിൽ
	// Get the next line from the supplied file
	status = qf_get_next_file_വരി(state);

    എങ്കിൽ (status != QF_OK)
	തിരികെ status;

    // remove newline/CR from the line
    എങ്കിൽ (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	എങ്കിൽ (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    തിരികെ QF_OK;
}

തരനിർവചനം ഘടന {
    char_u	*namebuf;
    പൂർണ്ണസംഖ്യ		bnr;
    char_u	*module;
    char_u	*errmsg;
    പൂർണ്ണസംഖ്യ		errmsglen;
    long	lnum;
    long	end_lnum;
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    പൂർണ്ണസംഖ്യ		enr;
    പൂർണ്ണസംഖ്യ		type;
    typval_T	*user_data;
    പൂർണ്ണസംഖ്യ		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_f(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields, പൂർണ്ണസംഖ്യ prefix)
{
    പൂർണ്ണസംഖ്യ c;

    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ || rmp->endp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    എങ്കിൽ (vim_strchr((char_u *)"OPQ", prefix) != ശൂന്യ
	    && mch_getperm(fields->namebuf) == -1)
	തിരികെ QF_FAIL;

    തിരികെ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_b(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    പൂർണ്ണസംഖ്യ bnr = (പൂർണ്ണസംഖ്യ)atol((അക്ഷരം *)rmp->startp[midx]);
    എങ്കിൽ (buflist_findnr(bnr) == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->bnr = bnr;
    തിരികെ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_n(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->enr = (പൂർണ്ണസംഖ്യ)atol((അക്ഷരം *)rmp->startp[midx]);
    തിരികെ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_l(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->lnum = atol((അക്ഷരം *)rmp->startp[midx]);
    തിരികെ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_e(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->end_lnum = atol((അക്ഷരം *)rmp->startp[midx]);
    തിരികെ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_c(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->col = (പൂർണ്ണസംഖ്യ)atol((അക്ഷരം *)rmp->startp[midx]);
    തിരികെ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_k(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->end_col = (പൂർണ്ണസംഖ്യ)atol((അക്ഷരം *)rmp->startp[midx]);
    തിരികെ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_t(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->type = *rmp->startp[midx];
    തിരികെ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_nonerror_വരി(char_u *linebuf, പൂർണ്ണസംഖ്യ linelen, qffields_T *fields)
{
    char_u	*p;

    എങ്കിൽ (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	എങ്കിൽ ((p = vim_realloc(fields->errmsg, linelen + 1)) == ശൂന്യ)
	    തിരികെ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    തിരികെ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_m(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ || rmp->endp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    len = (പൂർണ്ണസംഖ്യ)(rmp->endp[midx] - rmp->startp[midx]);
    എങ്കിൽ (len >= fields->errmsglen)
    {
	// len + null terminator
	എങ്കിൽ ((p = vim_realloc(fields->errmsg, len + 1)) == ശൂന്യ)
	    തിരികെ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    തിരികെ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_r(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, char_u **tail)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    *tail = rmp->startp[midx];
    തിരികെ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_p(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    char_u	*match_ptr;

    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ || rmp->endp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->col = 0;
    വേണ്ടി (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	എങ്കിൽ (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    തിരികെ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_v(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    fields->col = (പൂർണ്ണസംഖ്യ)atol((അക്ഷരം *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    തിരികെ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_s(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ || rmp->endp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    len = (പൂർണ്ണസംഖ്യ)(rmp->endp[midx] - rmp->startp[midx]);
    എങ്കിൽ (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    തിരികെ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_fmt_o(regmatch_T *rmp, പൂർണ്ണസംഖ്യ midx, qffields_T *fields)
{
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (rmp->startp[midx] == ശൂന്യ || rmp->endp[midx] == ശൂന്യ)
	തിരികെ QF_FAIL;
    len = (പൂർണ്ണസംഖ്യ)(rmp->endp[midx] - rmp->startp[midx]);
    എങ്കിൽ (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    തിരികെ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, പൂർണ്ണസംഖ്യ, qffields_T *) =
{
    ശൂന്യ, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    ശൂന്യ, // %r
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_match(
	char_u		*linebuf,
	പൂർണ്ണസംഖ്യ		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	പൂർണ്ണസംഖ്യ		qf_multiline,
	പൂർണ്ണസംഖ്യ		qf_multiscan,
	char_u		**tail)
{
    പൂർണ്ണസംഖ്യ		idx = fmt_ptr->prefix;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		midx;
    പൂർണ്ണസംഖ്യ		status;

    എങ്കിൽ ((idx == 'C' || idx == 'Z') && !qf_multiline)
	തിരികെ QF_FAIL;
    എങ്കിൽ (vim_strchr((char_u *)"EWIN", idx) != ശൂന്യ)
	fields->type = idx;
    അല്ലെങ്കിൽ
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    വേണ്ടി (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (പൂർണ്ണസംഖ്യ)fmt_ptr->addr[i];
	എങ്കിൽ (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	അല്ലെങ്കിൽ എങ്കിൽ (i == FMT_PATTERN_M)
	{
	    എങ്കിൽ (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_വരി(linebuf, linelen, fields);
	    അല്ലെങ്കിൽ എങ്കിൽ (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	അല്ലെങ്കിൽ എങ്കിൽ (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	എങ്കിൽ (status != QF_OK)
	    തിരികെ status;
    }

    തിരികെ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_get_fields(
	char_u		*linebuf,
	പൂർണ്ണസംഖ്യ		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	പൂർണ്ണസംഖ്യ		qf_multiline,
	പൂർണ്ണസംഖ്യ		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		status = QF_FAIL;
    പൂർണ്ണസംഖ്യ		r;

    എങ്കിൽ (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == ശൂന്യ)
	തിരികെ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    എങ്കിൽ (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = ശൂന്യ;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    എങ്കിൽ (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    തിരികെ status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_dir_pfx(പൂർണ്ണസംഖ്യ idx, qffields_T *fields, qf_list_T *qfl)
{
    എങ്കിൽ (idx == 'D')				// enter directory
    {
	എങ്കിൽ (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    തിരികെ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_തള്ളുക_dir(fields->namebuf, &qfl->qf_dir_അട്ടി, FALSE);
	എങ്കിൽ (qfl->qf_directory == ശൂന്യ)
	    തിരികെ QF_FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (idx == 'X')			// leave directory
	qfl->qf_directory = qf_എടുക്കുക_dir(&qfl->qf_dir_അട്ടി);

    തിരികെ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_file_pfx(
	പൂർണ്ണസംഖ്യ idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    എങ്കിൽ (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	എങ്കിൽ (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_തള്ളുക_dir(fields->namebuf, &qfl->qf_file_അട്ടി, TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (idx == 'Q')
	    qfl->qf_currfile = qf_എടുക്കുക_dir(&qfl->qf_file_അട്ടി);
	*fields->namebuf = NUL;
	എങ്കിൽ (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    തിരികെ QF_MULTISCAN;
	}
    }

    തിരികെ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_വരി_nomatch(char_u *linebuf, പൂർണ്ണസംഖ്യ linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    തിരികെ copy_nonerror_വരി(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_multiline_pfx(
	പൂർണ്ണസംഖ്യ idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    പൂർണ്ണസംഖ്യ			len;

    എങ്കിൽ (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	എങ്കിൽ (qfprev == ശൂന്യ)
	    തിരികെ QF_FAIL;
	എങ്കിൽ (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    എങ്കിൽ (ptr == ശൂന്യ)
		തിരികെ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	എങ്കിൽ (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	എങ്കിൽ (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	എങ്കിൽ (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	എങ്കിൽ (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	എങ്കിൽ (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	എങ്കിൽ (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	എങ്കിൽ (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != ശൂന്യ
		    ? fields->namebuf
		    : qfl->qf_currfile != ശൂന്യ && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    എങ്കിൽ (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    വരി_breakcheck();

    തിരികെ QF_IGNORE_വരി;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_parse_വരി(
	qf_list_T	*qfl,
	char_u		*linebuf,
	പൂർണ്ണസംഖ്യ		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    പൂർണ്ണസംഖ്യ			idx = 0;
    char_u		*tail = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			status;

restofline:
    // If there was no %> item start at the first pattern
    എങ്കിൽ (fmt_start == ശൂന്യ)
	fmt_ptr = fmt_first;
    അല്ലെങ്കിൽ
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = ശൂന്യ;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    വേണ്ടി ( ; fmt_ptr != ശൂന്യ; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	എങ്കിൽ (status == QF_NOMEM)
	    തിരികെ status;
	എങ്കിൽ (status == QF_OK)
	    നിർത്തുക;
    }
    qfl->qf_multiscan = FALSE;

    എങ്കിൽ (fmt_ptr == ശൂന്യ || idx == 'D' || idx == 'X')
    {
	എങ്കിൽ (fmt_ptr != ശൂന്യ)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    എങ്കിൽ (status != QF_OK)
		തിരികെ status;
	}

	status = qf_parse_വരി_nomatch(linebuf, linelen, fields);
	എങ്കിൽ (status != QF_OK)
	    തിരികെ status;

	എങ്കിൽ (fmt_ptr == ശൂന്യ)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fmt_ptr != ശൂന്യ)
    {
	// honor %> item
	എങ്കിൽ (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	എങ്കിൽ (vim_strchr((char_u *)"AEWIN", idx) != ശൂന്യ)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"CZ", idx) != ശൂന്യ)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    എങ്കിൽ (status != QF_OK)
		തിരികെ status;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"OPQ", idx) != ശൂന്യ)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    എങ്കിൽ (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		പൂർണ്ണസംഖ്യ new_linelen = (പൂർണ്ണസംഖ്യ)STRLEN(s);
		എങ്കിൽ (new_linelen >= linelen)
		    തിരികെ QF_IGNORE_വരി;
		linebuf = s;
		linelen = new_linelen;
		പോകുക restofline;
	    }
	}
	എങ്കിൽ (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    എങ്കിൽ (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    തിരികെ QF_IGNORE_വരി;
	}
    }

    തിരികെ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_അട്ടി_empty(qf_info_T *qi)
{
    തിരികെ qi == ശൂന്യ || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_list_empty(qf_list_T *qfl)
{
    തിരികെ qfl == ശൂന്യ || qfl->qf_എണ്ണം <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_list_has_valid_entries(qf_list_T *qfl)
{
    തിരികെ !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    സ്ഥിതം qf_list_T *
qf_get_list(qf_info_T *qi, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    എങ്കിൽ (pfields->namebuf == ശൂന്യ || pfields->errmsg == ശൂന്യ
		|| pfields->pattern == ശൂന്യ || pfields->module == ശൂന്യ)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
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
    എങ്കിൽ (enc != ശൂന്യ && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    എങ്കിൽ (efile != ശൂന്യ && (pstate->fd = mch_fopen((അക്ഷരം *)efile, "r")) == ശൂന്യ)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	തിരികെ FAIL;
    }

    എങ്കിൽ (tv != ശൂന്യ)
    {
	എങ്കിൽ (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    തിരികെ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    സ്ഥിതം ശൂന്യം
qf_cleanup_state(qfstate_T *pstate)
{
    എങ്കിൽ (pstate->fd != ശൂന്യ)
	fclose(pstate->fd);

    vim_free(pstate->growbuf);
    എങ്കിൽ (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, ശൂന്യ, ശൂന്യ);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    പൂർണ്ണസംഖ്യ		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    എങ്കിൽ (status != QF_OK)
	തിരികെ status;

    status = qf_parse_വരി(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    എങ്കിൽ (status != QF_OK)
	തിരികെ status;

    തിരികെ qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != ശൂന്യ)
		? fields->namebuf
		: ((qfl->qf_currfile != ശൂന്യ && fields->valid)
		    ? qfl->qf_currfile : (char_u *)ശൂന്യ),
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_init_ext(
    qf_info_T	    *qi,
    പൂർണ്ണസംഖ്യ		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    പൂർണ്ണസംഖ്യ		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    adding = FALSE;
    സ്ഥിതം efm_T    *fmt_first = ശൂന്യ;
    char_u	    *efm;
    സ്ഥിതം char_u   *last_efm = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    retval = -1;	// default: return error flag
    പൂർണ്ണസംഖ്യ		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    എങ്കിൽ ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	പോകുക qf_init_end;

    എങ്കിൽ (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    അല്ലെങ്കിൽ
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	എങ്കിൽ (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    എങ്കിൽ (errorformat == p_efm && tv == ശൂന്യ && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    അല്ലെങ്കിൽ
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    എങ്കിൽ (last_efm == ശൂന്യ || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	എങ്കിൽ (fmt_first != ശൂന്യ)
	    last_efm = vim_strsave(efm);
    }

    എങ്കിൽ (fmt_first == ശൂന്യ)	// nothing found
	പോകുക error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    ആകുമ്പോൾ (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	എങ്കിൽ (status == QF_NOMEM)		// memory alloc failure
	    പോകുക qf_init_end;
	എങ്കിൽ (status == QF_END_OF_നിവേശം)	// end of input
	    നിർത്തുക;
	എങ്കിൽ (status == QF_FAIL)
	    പോകുക error2;

	വരി_breakcheck();
    }
    എങ്കിൽ (state.fd == ശൂന്യ || !ferror(state.fd))
    {
	എങ്കിൽ (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    qfl->qf_nonevalid = FALSE;
	    എങ്കിൽ (qfl->qf_ptr == ശൂന്യ)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_എണ്ണം;
	പോകുക qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    എങ്കിൽ (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	എങ്കിൽ (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    എങ്കിൽ (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    തിരികെ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    പൂർണ്ണസംഖ്യ
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	പൂർണ്ണസംഖ്യ	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    എങ്കിൽ (wp != ശൂന്യ)
	qi = ll_get_or_alloc_list(wp);
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    തിരികെ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, ശൂന്യ, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    സ്ഥിതം ശൂന്യം
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    എങ്കിൽ (title == ശൂന്യ)
	തിരികെ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    എങ്കിൽ (p != ശൂന്യ)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    സ്ഥിതം char_u *
qf_cmdtitle(char_u *cmd)
{
    സ്ഥിതം char_u qftitle_str[IOSIZE];

    vim_snprintf((അക്ഷരം *)qftitle_str, IOSIZE, ":%s", (അക്ഷരം *)cmd);
    തിരികെ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    സ്ഥിതം qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    തിരികെ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    സ്ഥിതം ശൂന്യം
qf_എടുക്കുക_അട്ടി(qf_info_T *qi, പൂർണ്ണസംഖ്യ adjust)
{
    പൂർണ്ണസംഖ്യ i;
    qf_free(&qi->qf_lists[0]);
    വേണ്ടി (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, വലുപ്പം(*qi->qf_lists));

    എങ്കിൽ (adjust)
    {
	qi->qf_listcount--;
	എങ്കിൽ (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	അല്ലെങ്കിൽ
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    സ്ഥിതം ശൂന്യം
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    ആകുമ്പോൾ (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    എങ്കിൽ (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_എടുക്കുക_അട്ടി(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    എങ്കിൽ (q == ശൂന്യ)
	തിരികെ;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    പൂർണ്ണസംഖ്യ
qf_അട്ടി_get_bufnr(ശൂന്യം)
{
    എങ്കിൽ (ql_info == ശൂന്യ)
	തിരികെ INVALID_QFBUFNR;
    തിരികെ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    സ്ഥിതം ശൂന്യം
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    എങ്കിൽ (qi->qf_bufnr == INVALID_QFBUFNR)
	തിരികെ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    എങ്കിൽ (qfbuf != ശൂന്യ && qfbuf->b_nwindows == 0)
    {
	പൂർണ്ണസംഖ്യ buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	എങ്കിൽ (curwin->w_buffer == ശൂന്യ)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(ശൂന്യ, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	എങ്കിൽ (buf_was_null)
	    curwin->w_buffer = ശൂന്യ;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    സ്ഥിതം ശൂന്യം
qf_free_list_അട്ടി_ഇനങ്ങൾ(qf_info_T *qi)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    സ്ഥിതം ശൂന്യം
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_അട്ടി_ഇനങ്ങൾ(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    സ്ഥിതം ശൂന്യം
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ;
    *pqi = ശൂന്യ;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    എങ്കിൽ (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	തിരികെ;
    }

    qi->qf_refcount--;
    എങ്കിൽ (qi->qf_refcount < 1)
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
    ശൂന്യം
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    എങ്കിൽ (wp != ശൂന്യ)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qi != ശൂന്യ)
	qf_free_list_അട്ടി_ഇനങ്ങൾ(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    സ്ഥിതം ശൂന്യം
incr_quickfix_busy(ശൂന്യം)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    സ്ഥിതം ശൂന്യം
decr_quickfix_busy(ശൂന്യം)
{
    എങ്കിൽ (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	ആകുമ്പോൾ (qf_delq_head != ശൂന്യ)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    എങ്കിൽ (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
# endif
}

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
check_quickfix_busy(ശൂന്യം)
{
    എങ്കിൽ (quickfix_busy != 0)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    പൂർണ്ണസംഖ്യ		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    long	lnum,		// line number
    long	end_lnum,	// line number for end
    പൂർണ്ണസംഖ്യ		col,		// column
    പൂർണ്ണസംഖ്യ		end_col,	// column for end
    പൂർണ്ണസംഖ്യ		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    പൂർണ്ണസംഖ്യ		nr,		// error number
    പൂർണ്ണസംഖ്യ		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    പൂർണ്ണസംഖ്യ		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = ശൂന്യ;
    char_u	*p = ശൂന്യ;

    എങ്കിൽ ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == ശൂന്യ)
	തിരികെ QF_FAIL;
    എങ്കിൽ (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	എങ്കിൽ (buf != ശൂന്യ)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    അല്ലെങ്കിൽ
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    എങ്കിൽ (fname != ശൂന്യ)
	fullname = fix_fname(fname);
    qfp->qf_fname = ശൂന്യ;
    എങ്കിൽ (buf != ശൂന്യ &&
	buf->b_ffname != ശൂന്യ && fullname != ശൂന്യ)
    {
	എങ്കിൽ (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    എങ്കിൽ (p != ശൂന്യ)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    എങ്കിൽ ((qfp->qf_text = vim_strsave(mesg)) == ശൂന്യ)
    {
	vim_free(qfp);
	തിരികെ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    എങ്കിൽ (user_data == ശൂന്യ || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    അല്ലെങ്കിൽ
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    എങ്കിൽ (pattern == ശൂന്യ || *pattern == NUL)
	qfp->qf_pattern = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ ((qfp->qf_pattern = vim_strsave(pattern)) == ശൂന്യ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	തിരികെ QF_FAIL;
    }
    എങ്കിൽ (module == ശൂന്യ || *module == NUL)
	qfp->qf_module = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ ((qfp->qf_module = vim_strsave(module)) == ശൂന്യ)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	തിരികെ QF_FAIL;
    }
    qfp->qf_nr = nr;
    എങ്കിൽ (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    എങ്കിൽ (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = ശൂന്യ;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_എണ്ണം;
    എങ്കിൽ (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_എണ്ണം;
	qfl->qf_ptr = qfp;
    }

    തിരികെ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    പൂർണ്ണസംഖ്യ
qf_resize_അട്ടി(പൂർണ്ണസംഖ്യ n)
{
    എങ്കിൽ (ql_info == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (qf_resize_അട്ടി_base(ql_info, n) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    പൂർണ്ണസംഖ്യ
ll_resize_അട്ടി(win_T *wp, പൂർണ്ണസംഖ്യ n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    എങ്കിൽ (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    അല്ലെങ്കിൽ
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (qf_resize_അട്ടി_base(qi, n) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_resize_അട്ടി_base(qf_info_T *qi, പൂർണ്ണസംഖ്യ n)
{
    qf_list_T *new;
    പൂർണ്ണസംഖ്യ amount_to_rm = 0, i;

    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    size_t lsz = വലുപ്പം(*qi->qf_lists);

    എങ്കിൽ (n == qi->qf_maxcount)
	തിരികെ OK;
    അല്ലെങ്കിൽ എങ്കിൽ (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	വേണ്ടി (i = 0; i < amount_to_rm; i++)
	    qf_എടുക്കുക_അട്ടി(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    എങ്കിൽ (new == ശൂന്യ)
	തിരികെ FAIL;

    // fill with zeroes any newly allocated memory
    എങ്കിൽ (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, ശൂന്യ);

    തിരികെ OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   ശൂന്യം
qf_init_അട്ടി(ശൂന്യം)
{
    ql_info = qf_alloc_അട്ടി(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    സ്ഥിതം ശൂന്യം
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    എങ്കിൽ (wp != ശൂന്യ)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    സ്ഥിതം ശൂന്യം
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    എങ്കിൽ (llw != ശൂന്യ)
	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		തിരികെ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    സ്ഥിതം qf_info_T *
qf_alloc_അട്ടി(qfltype_T qfltype, പൂർണ്ണസംഖ്യ n)
{
    qf_info_T *qi;

    എങ്കിൽ (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    അല്ലെങ്കിൽ
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	എങ്കിൽ (qi == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_അട്ടി(n);
    എങ്കിൽ (qi->qf_lists == ശൂന്യ)
    {
	എങ്കിൽ (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	തിരികെ ശൂന്യ;
    }
    qi->qf_maxcount = n;

    തിരികെ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    സ്ഥിതം qf_list_T *
qf_alloc_list_അട്ടി(പൂർണ്ണസംഖ്യ n)
{
    തിരികെ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    സ്ഥിതം qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    എങ്കിൽ (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	തിരികെ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    എങ്കിൽ (wp->w_llist == ശൂന്യ)
	// new location list
	wp->w_llist = qf_alloc_അട്ടി(QFLT_LOCATION, wp->w_p_lhi);

    തിരികെ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    സ്ഥിതം qf_info_T *
qf_cmd_get_അട്ടി(exarg_T *eap, പൂർണ്ണസംഖ്യ print_emsg)
{
    qf_info_T	*qi = ql_info;

    എങ്കിൽ (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	എങ്കിൽ (qi == ശൂന്യ)
	{
	    എങ്കിൽ (print_emsg)
		emsg(_(e_no_location_list));
	    തിരികെ ശൂന്യ;
	}
    }
    എങ്കിൽ (qi == ശൂന്യ && print_emsg)
	emsg(_(e_no_quickfix_അട്ടി));

    തിരികെ qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    സ്ഥിതം qf_info_T *
qf_cmd_get_or_alloc_അട്ടി(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    എങ്കിൽ (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	എങ്കിൽ (qi == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	*pwinp = curwin;
    }

    തിരികെ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    പൂർണ്ണസംഖ്യ		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_ഇനങ്ങൾ(from_qfl, from_qfp, i)
    {
	എങ്കിൽ (qf_add_entry(to_qfl,
		    ശൂന്യ,
		    ശൂന്യ,
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
	    തിരികെ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	എങ്കിൽ (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    തിരികെ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_എണ്ണം = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = ശൂന്യ;
    to_qfl->qf_last = ശൂന്യ;
    to_qfl->qf_ptr = ശൂന്യ;
    എങ്കിൽ (from_qfl->qf_title != ശൂന്യ)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    അല്ലെങ്കിൽ
	to_qfl->qf_title = ശൂന്യ;
    എങ്കിൽ (from_qfl->qf_ctx != ശൂന്യ)
    {
	to_qfl->qf_ctx = alloc_tv();
	എങ്കിൽ (to_qfl->qf_ctx != ശൂന്യ)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    അല്ലെങ്കിൽ
	to_qfl->qf_ctx = ശൂന്യ;
    എങ്കിൽ (from_qfl->qf_qftf_cb.cb_name != ശൂന്യ)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    അല്ലെങ്കിൽ
	to_qfl->qf_qftf_cb.cb_name = ശൂന്യ;

    എങ്കിൽ (from_qfl->qf_എണ്ണം)
	എങ്കിൽ (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    തിരികെ FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    എങ്കിൽ (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    തിരികെ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    ശൂന്യം
copy_loclist_അട്ടി(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    പൂർണ്ണസംഖ്യ		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    എങ്കിൽ (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    അല്ലെങ്കിൽ
	qi = from->w_llist;

    എങ്കിൽ (qi == ശൂന്യ)		    // no location list to copy
	തിരികെ;

    // allocate a new location list, set size of stack to 'from' window value
    എങ്കിൽ ((to->w_llist = qf_alloc_അട്ടി(QFLT_LOCATION, from->w_p_lhi)) == ശൂന്യ)
	തിരികെ;
    അല്ലെങ്കിൽ
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    വേണ്ടി (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	എങ്കിൽ (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    തിരികെ;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {ശൂന്യ, 0};
    buf_T	*buf;
    string_T	bufname;
    size_t	fname_len;

    എങ്കിൽ (fname == ശൂന്യ || *fname == NUL)		// no file name
	തിരികെ 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (directory != ശൂന്യ)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    എങ്കിൽ (directory != ശൂന്യ && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != ശൂന്യ)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	എങ്കിൽ (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    എങ്കിൽ (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    അല്ലെങ്കിൽ
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    എങ്കിൽ (ptr.string == ശൂന്യ)
		തിരികെ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    അല്ലെങ്കിൽ
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    എങ്കിൽ (qf_last_bufname != ശൂന്യ && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    അല്ലെങ്കിൽ
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, ശൂന്യ, (linenr_T)0, BLN_NOOPT);
	എങ്കിൽ (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	അല്ലെങ്കിൽ
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    തിരികെ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    സ്ഥിതം char_u *
qf_തള്ളുക_dir(char_u *dirbuf, ഘടന dir_അട്ടി_T **stackptr, പൂർണ്ണസംഖ്യ is_file_അട്ടി)
{
    ഘടന dir_അട്ടി_T  *ds_new;
    ഘടന dir_അട്ടി_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(ഘടന dir_അട്ടി_T, aid_qf_dirstack);
    എങ്കിൽ (ds_new == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    എങ്കിൽ (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == ശൂന്യ
	    || is_file_അട്ടി)
	(*stackptr)->dirname = vim_strsave(dirbuf);
    അല്ലെങ്കിൽ
    {
	size_t	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->dirname = ശൂന്യ;
	ആകുമ്പോൾ (ds_new)
	{
	    string_T	dirname;

	    concat_fnames(ds_new->dirname, STRLEN(ds_new->dirname),
		dirbuf, dirbuf_len, TRUE, &dirname);
	    എങ്കിൽ (dirname.string == ശൂന്യ)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		തിരികെ ശൂന്യ;
	    }
	    എങ്കിൽ (mch_isdir(dirname.string) == TRUE)
	    {
		vim_free((*stackptr)->dirname);
		(*stackptr)->dirname = dirname.string;
		നിർത്തുക;
	    }
	    vim_free(dirname.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	ആകുമ്പോൾ ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->dirname);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	എങ്കിൽ (ds_new == ശൂന്യ)
	{
	    vim_free((*stackptr)->dirname);
	    (*stackptr)->dirname = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    എങ്കിൽ ((*stackptr)->dirname != ശൂന്യ)
	തിരികെ (*stackptr)->dirname;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    തിരികെ ശൂന്യ;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    സ്ഥിതം char_u *
qf_എടുക്കുക_dir(ഘടന dir_അട്ടി_T **stackptr)
{
    ഘടന dir_അട്ടി_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    എങ്കിൽ (*stackptr != ശൂന്യ)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->dirname);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    തിരികെ *stackptr ? (*stackptr)->dirname : ശൂന്യ;
}

/*
 * clean up directory stack
 */
    സ്ഥിതം ശൂന്യം
qf_clean_dir_അട്ടി(ഘടന dir_അട്ടി_T **stackptr)
{
    ഘടന dir_അട്ടി_T  *ds_ptr;

    ആകുമ്പോൾ ((ds_ptr = *stackptr) != ശൂന്യ)
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
    സ്ഥിതം char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    ഘടന dir_അട്ടി_T     *ds_ptr;
    ഘടന dir_അട്ടി_T     *ds_tmp;
    string_T		   fullname;
    size_t		   filename_len;

    // no dirs on the stack - there's nothing we can do
    എങ്കിൽ (qfl->qf_dir_അട്ടി == ശൂന്യ)
	തിരികെ ശൂന്യ;

    ds_ptr = qfl->qf_dir_അട്ടി->next;
    fullname.string = ശൂന്യ;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    ആകുമ്പോൾ (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->dirname, STRLEN(ds_ptr->dirname),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	എങ്കിൽ ((fullname.string != ശൂന്യ) && (mch_getperm(fullname.string) >= 0))
	    നിർത്തുക;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    ആകുമ്പോൾ (qfl->qf_dir_അട്ടി->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_അട്ടി->next;
	qfl->qf_dir_അട്ടി->next = qfl->qf_dir_അട്ടി->next->next;
	vim_free(ds_tmp->dirname);
	vim_free(ds_tmp);
    }

    തിരികെ ds_ptr == ശൂന്യ ? ശൂന്യ : ds_ptr->dirname;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (wp != ശൂന്യ)
    {
	എങ്കിൽ (!win_valid(wp))
	    തിരികെ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (i = 0; i < qi->qf_listcount; ++i)
	എങ്കിൽ (qi->qf_lists[i].qf_id == qf_id)
	    തിരികെ TRUE;

    തിരികെ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
	എങ്കിൽ (qfp == qf_ptr)
	    നിർത്തുക;

    എങ്കിൽ (i > qfl->qf_എണ്ണം) // Entry is not found
	തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    സ്ഥിതം qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	പൂർണ്ണസംഖ്യ		*qf_index,
	പൂർണ്ണസംഖ്യ		dir)
{
    പൂർണ്ണസംഖ്യ			idx;
    പൂർണ്ണസംഖ്യ			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    ചെയ്യുക
    {
	എങ്കിൽ (idx == qfl->qf_എണ്ണം || qf_ptr->qf_next == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } ആകുമ്പോൾ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    തിരികെ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    സ്ഥിതം qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	പൂർണ്ണസംഖ്യ		*qf_index,
	പൂർണ്ണസംഖ്യ		dir)
{
    പൂർണ്ണസംഖ്യ			idx;
    പൂർണ്ണസംഖ്യ			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    ചെയ്യുക
    {
	എങ്കിൽ (idx == 1 || qf_ptr->qf_prev == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } ആകുമ്പോൾ ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    തിരികെ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    സ്ഥിതം qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	പൂർണ്ണസംഖ്യ		errornr,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    പൂർണ്ണസംഖ്യ			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    പൂർണ്ണസംഖ്യ			prev_index;
    അക്ഷരം		*err = e_no_more_ഇനങ്ങൾ;

    ആകുമ്പോൾ (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	എങ്കിൽ (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	അല്ലെങ്കിൽ
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	എങ്കിൽ (qf_ptr == ശൂന്യ)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    എങ്കിൽ (err != ശൂന്യ)
	    {
		emsg(_(err));
		തിരികെ ശൂന്യ;
	    }
	    നിർത്തുക;
	}

	err = ശൂന്യ;
    }

    *new_qfidx = qf_idx;
    തിരികെ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    സ്ഥിതം qfline_T *
get_nth_entry(qf_list_T *qfl, പൂർണ്ണസംഖ്യ errornr, പൂർണ്ണസംഖ്യ *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    പൂർണ്ണസംഖ്യ		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    ആകുമ്പോൾ (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != ശൂന്യ)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    ആകുമ്പോൾ (errornr > qf_idx && qf_idx < qfl->qf_എണ്ണം &&
						qf_ptr->qf_next != ശൂന്യ)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    തിരികെ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    സ്ഥിതം qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	പൂർണ്ണസംഖ്യ		errornr,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    പൂർണ്ണസംഖ്യ		qfidx = qfl->qf_index;

    എങ്കിൽ (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    അല്ലെങ്കിൽ എങ്കിൽ (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    തിരികെ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    സ്ഥിതം win_T *
qf_find_help_win(ശൂന്യം)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (bt_help(wp->w_buffer))
	    തിരികെ wp;

    തിരികെ ശൂന്യ;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    സ്ഥിതം ശൂന്യം
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
jump_to_help_window(qf_info_T *qi, പൂർണ്ണസംഖ്യ newwin, പൂർണ്ണസംഖ്യ *opened_window)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		flags;

    എങ്കിൽ (cmdmod.cmod_tab != 0 || newwin)
	wp = ശൂന്യ;
    അല്ലെങ്കിൽ
	wp = qf_find_help_win();
    എങ്കിൽ (wp != ശൂന്യ && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    അല്ലെങ്കിൽ
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	എങ്കിൽ (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_മുകൾ;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	എങ്കിൽ (IS_LL_അട്ടി(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	എങ്കിൽ (win_split(0, flags) == FAIL)
	    തിരികെ FAIL;

	*opened_window = TRUE;

	എങ്കിൽ (curwin->w_height < p_hh)
	    win_setheight((പൂർണ്ണസംഖ്യ)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	എങ്കിൽ (IS_LL_അട്ടി(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    എങ്കിൽ (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    തിരികെ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    സ്ഥിതം win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    തിരികെ wp;

    തിരികെ ശൂന്യ;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    സ്ഥിതം win_T *
qf_find_win_with_normal_buf(ശൂന്യം)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (bt_normal(wp->w_buffer))
	    തിരികെ wp;

    തിരികെ ശൂന്യ;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_goto_tabwin_with_file(പൂർണ്ണസംഖ്യ fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    തിരികെ TRUE;
	}

    തിരികെ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_open_new_file_win(qf_info_T *ll_ref)
{
    പൂർണ്ണസംഖ്യ		flags;

    flags = WSP_ABOVE;
    എങ്കിൽ (ll_ref != ശൂന്യ)
	flags |= WSP_NEWLOC;
    എങ്കിൽ (win_split(0, flags) == FAIL)
	തിരികെ FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    എങ്കിൽ (ll_ref != ശൂന്യ)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    തിരികെ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    സ്ഥിതം ശൂന്യം
qf_goto_win_with_ll_file(win_T *use_win, പൂർണ്ണസംഖ്യ qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    എങ്കിൽ (win == ശൂന്യ)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    എങ്കിൽ (win->w_buffer->b_fnum == qf_fnum)
		നിർത്തുക;
	എങ്കിൽ (win == ശൂന്യ)
	{
	    // Find a previous usable window
	    win = curwin;
	    ചെയ്യുക
	    {
		എങ്കിൽ (bt_normal(win->w_buffer))
		    നിർത്തുക;
		എങ്കിൽ (win->w_prev == ശൂന്യ)
		    win = lastwin;	// wrap around the top
		അല്ലെങ്കിൽ
		    win = win->w_prev; // go to previous window
	    } ആകുമ്പോൾ (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    എങ്കിൽ (win->w_llist == ശൂന്യ && ll_ref != ശൂന്യ)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    സ്ഥിതം ശൂന്യം
qf_goto_win_with_qfl_file(പൂർണ്ണസംഖ്യ qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = ശൂന്യ;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (win->w_buffer->b_fnum == qf_fnum)
	    നിർത്തുക;
	എങ്കിൽ (win->w_prev == ശൂന്യ)
	    win = lastwin;	// wrap around the top
	അല്ലെങ്കിൽ
	    win = win->w_prev;	// go to previous window

	എങ്കിൽ (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    എങ്കിൽ ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    അല്ലെങ്കിൽ എങ്കിൽ (altwin != ശൂന്യ)
		win = altwin;
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_prev != ശൂന്യ)
		win = curwin->w_prev;
	    അല്ലെങ്കിൽ
		win = curwin->w_next;
	    നിർത്തുക;
	}

	// Remember a usable window.
	എങ്കിൽ (altwin == ശൂന്യ && !win->w_p_pvw && !win->w_p_wfb &&
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_jump_to_usable_window(പൂർണ്ണസംഖ്യ qf_fnum, പൂർണ്ണസംഖ്യ newwin, പൂർണ്ണസംഖ്യ *opened_window)
{
    win_T	*usable_wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		usable_win = FALSE;
    qf_info_T	*ll_ref = ശൂന്യ;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    എങ്കിൽ (!newwin)
	ll_ref = curwin->w_llist_ref;

    എങ്കിൽ (ll_ref != ശൂന്യ)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	എങ്കിൽ (usable_wp != ശൂന്യ)
	    usable_win = TRUE;
    }

    എങ്കിൽ (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	എങ്കിൽ (win != ശൂന്യ)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    എങ്കിൽ (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    എങ്കിൽ ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	എങ്കിൽ (qf_open_new_file_win(ll_ref) != OK)
	    തിരികെ FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (curwin->w_llist_ref != ശൂന്യ)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	അല്ലെങ്കിൽ					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    തിരികെ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	പൂർണ്ണസംഖ്യ		forceit,
	പൂർണ്ണസംഖ്യ		prev_winid,
	പൂർണ്ണസംഖ്യ		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    പൂർണ്ണസംഖ്യ		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    പൂർണ്ണസംഖ്യ		retval = OK;
    പൂർണ്ണസംഖ്യ		old_qf_curlist = qi->qf_curlist;
    പൂർണ്ണസംഖ്യ		save_qfid = qfl->qf_id;

    എങ്കിൽ (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	എങ്കിൽ (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    തിരികെ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, ശൂന്യ, ശൂന്യ, ശൂന്യ, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	fnum = qf_ptr->qf_fnum;

	എങ്കിൽ (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    എങ്കിൽ (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    എങ്കിൽ (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		എങ്കിൽ (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		എങ്കിൽ (curwin->w_p_wfb)
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

	എങ്കിൽ (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    എങ്കിൽ (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	എങ്കിൽ (wp == ശൂന്യ && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    തിരികെ QF_ABORT;
	}
    }

    എങ്കിൽ (qfl_type == QFLT_QUICKFIX && !qflist_valid(ശൂന്യ, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	തിരികെ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    എങ്കിൽ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	എങ്കിൽ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	അല്ലെങ്കിൽ
	    emsg(_(e_current_location_list_was_changed));
	തിരികെ QF_ABORT;
    }

    തിരികെ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*വരി = ml_get_curline();
    char_u	*p = വരി;
    colnr_T	col = 0;

    ആകുമ്പോൾ (*p != NUL && col < vcol)
    {
	എങ്കിൽ (*p == TAB)
	    col += 8 - (col % 8);
	അല്ലെങ്കിൽ
	    col += ptr2cells(p);
	എങ്കിൽ (col > vcol)
	    നിർത്തുക;
	MB_PTR_ADV(p);
    }
    തിരികെ (പൂർണ്ണസംഖ്യ)(p - വരി);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    സ്ഥിതം ശൂന്യം
qf_jump_goto_വരി(
	linenr_T	qf_lnum,
	പൂർണ്ണസംഖ്യ		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    എങ്കിൽ (qf_pattern == ശൂന്യ)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	എങ്കിൽ (i > 0)
	{
	    എങ്കിൽ (i > curbuf->b_ml.ml_വരി_എണ്ണം)
		i = curbuf->b_ml.ml_വരി_എണ്ണം;
	    curwin->w_cursor.lnum = i;
	}
	എങ്കിൽ (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    എങ്കിൽ (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    അല്ലെങ്കിൽ
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	അല്ലെങ്കിൽ
	    beginline(BL_WHITE | BL_FIX);
    }
    അല്ലെങ്കിൽ
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	എങ്കിൽ (!do_തിരയൽ(ശൂന്യ, '/', '/', qf_pattern, STRLEN(qf_pattern), (long)1, തിരയൽ_KEEP, ശൂന്യ))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    സ്ഥിതം ശൂന്യം
qf_jump_print_സന്ദേശം(
	qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		qf_index,
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
    എങ്കിൽ (!സന്ദേശം_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_എണ്ണം,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (അക്ഷരം *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = സന്ദേശം_scroll;
    എങ്കിൽ (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	സന്ദേശം_scroll = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (!സന്ദേശം_scrolled && shortmess(SHM_OVERALL))
	സന്ദേശം_scroll = FALSE;
    സന്ദേശം_attr_keep((അക്ഷരം *)gap->ga_data, 0, TRUE);
    സന്ദേശം_scroll = i;

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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	പൂർണ്ണസംഖ്യ		newwin,
	പൂർണ്ണസംഖ്യ		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    പൂർണ്ണസംഖ്യ		old_changedtick = qfl->qf_changedtick;
    പൂർണ്ണസംഖ്യ		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    എങ്കിൽ (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	എങ്കിൽ (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    തിരികെ FAIL;
    എങ്കിൽ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	എങ്കിൽ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	അല്ലെങ്കിൽ
	    emsg(_(e_current_location_list_was_changed));
	തിരികെ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    എങ്കിൽ (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	എങ്കിൽ (qf_ptr->qf_fnum == 0)
	    തിരികെ NOTDONE;

	എങ്കിൽ (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	എങ്കിൽ (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	അല്ലെങ്കിൽ
	    emsg(_(e_current_location_list_was_changed));
	തിരികെ QF_ABORT;
    }

    തിരികെ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_jump_to_buffer(
	qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		qf_index,
	qfline_T	*qf_ptr,
	പൂർണ്ണസംഖ്യ		forceit,
	പൂർണ്ണസംഖ്യ		prev_winid,
	പൂർണ്ണസംഖ്യ		*opened_window,
	പൂർണ്ണസംഖ്യ		openfold,
	പൂർണ്ണസംഖ്യ		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    പൂർണ്ണസംഖ്യ		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    എങ്കിൽ (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	എങ്കിൽ (retval != OK)
	    തിരികെ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    എങ്കിൽ (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_വരി(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    എങ്കിൽ (print_message)
	qf_jump_print_സന്ദേശം(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    തിരികെ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    ശൂന്യം
qf_jump(qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		errornr,
	പൂർണ്ണസംഖ്യ		forceit)
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
    സ്ഥിതം ശൂന്യം
qf_jump_newwin(qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		errornr,
	പൂർണ്ണസംഖ്യ		forceit,
	പൂർണ്ണസംഖ്യ		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    പൂർണ്ണസംഖ്യ			qf_index;
    പൂർണ്ണസംഖ്യ			old_qf_index;
    char_u		*old_swb = p_swb;
    unsigned		old_swb_flags = swb_flags;
    പൂർണ്ണസംഖ്യ			prev_winid;
    പൂർണ്ണസംഖ്യ			opened_window = FALSE;
    പൂർണ്ണസംഖ്യ			print_message = TRUE;
    പൂർണ്ണസംഖ്യ			old_താക്കോൽ_Typed = താക്കോൽ_Typed; // getting file may reset it
    പൂർണ്ണസംഖ്യ			retval = OK;

    എങ്കിൽ (qi == ശൂന്യ)
    {
	എങ്കിൽ (ql_info == ശൂന്യ)
	{
	    emsg(_(e_no_quickfix_അട്ടി));
	    തിരികെ;
	}
	qi = ql_info;
    }

    എങ്കിൽ (qf_അട്ടി_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	തിരികെ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    എങ്കിൽ (qf_ptr == ശൂന്യ)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	പോകുക theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    എങ്കിൽ (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    എങ്കിൽ (retval == FAIL)
	പോകുക failed;
    എങ്കിൽ (retval == QF_ABORT)
    {
	qi = ശൂന്യ;
	qf_ptr = ശൂന്യ;
	പോകുക theend;
    }
    എങ്കിൽ (retval == NOTDONE)
	പോകുക theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_താക്കോൽ_Typed, print_message);
    എങ്കിൽ (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = ശൂന്യ;
	qf_ptr = ശൂന്യ;
    }

    എങ്കിൽ (retval != OK)
    {
	എങ്കിൽ (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	എങ്കിൽ (qf_ptr != ശൂന്യ && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    എങ്കിൽ (qi != ശൂന്യ)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    എങ്കിൽ (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
സ്ഥിതം പൂർണ്ണസംഖ്യ	qfFileAttr;
സ്ഥിതം പൂർണ്ണസംഖ്യ	qfSepAttr;
സ്ഥിതം പൂർണ്ണസംഖ്യ	qf_വരി_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    സ്ഥിതം ശൂന്യം
qf_list_entry(qfline_T *qfp, പൂർണ്ണസംഖ്യ qf_idx, പൂർണ്ണസംഖ്യ cursel)
{
    char_u	*fname;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		filter_entry;
    garray_T	*gap;

    fname = ശൂന്യ;
    എങ്കിൽ (qfp->qf_module != ശൂന്യ && *qfp->qf_module != NUL)
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(അക്ഷരം *)qfp->qf_module);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != ശൂന്യ)
	{
	    എങ്കിൽ (qfp->qf_fname == ശൂന്യ)
		fname = buf->b_fname;
	    അല്ലെങ്കിൽ
		fname = qfp->qf_fname;
	    എങ്കിൽ (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	എങ്കിൽ (fname == ശൂന്യ)
	    sprintf((അക്ഷരം *)IObuff, "%2d", qf_idx);
	അല്ലെങ്കിൽ
	    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (അക്ഷരം *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    എങ്കിൽ (qfp->qf_module != ശൂന്യ && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    എങ്കിൽ (filter_entry && fname != ശൂന്യ)
	filter_entry &= message_filtered(fname);
    എങ്കിൽ (filter_entry && qfp->qf_pattern != ശൂന്യ)
	filter_entry &= message_filtered(qfp->qf_pattern);
    എങ്കിൽ (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    എങ്കിൽ (filter_entry)
	തിരികെ;

    സന്ദേശം_putchar('\n');
    സന്ദേശം_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    എങ്കിൽ (qfp->qf_lnum != 0)
	സന്ദേശം_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    എങ്കിൽ (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    സന്ദേശം_puts_attr((അക്ഷരം *)gap->ga_data, qf_വരി_Attr);
    സന്ദേശം_puts_attr(":", qfSepAttr);
    എങ്കിൽ (qfp->qf_pattern != ശൂന്യ)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	സന്ദേശം_puts((അക്ഷരം *)gap->ga_data);
	സന്ദേശം_puts_attr(":", qfSepAttr);
    }
    സന്ദേശം_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != ശൂന്യ || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    സന്ദേശം_prt_വരി((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    ശൂന്യം
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		idx1 = 1;
    പൂർണ്ണസംഖ്യ		idx2 = -1;
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		plus = FALSE;
    പൂർണ്ണസംഖ്യ		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (qf_അട്ടി_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	തിരികെ;
    }
    എങ്കിൽ (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    എങ്കിൽ (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	തിരികെ;
    }
    qfl = qf_get_curlist(qi);
    എങ്കിൽ (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    അല്ലെങ്കിൽ
    {
	i = qfl->qf_എണ്ണം;
	എങ്കിൽ (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	എങ്കിൽ (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    എങ്കിൽ (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    എങ്കിൽ (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_വരി_Attr = syn_name2attr((char_u *)"qfLineNr");
    എങ്കിൽ (qf_വരി_Attr == 0)
	qf_വരി_Attr = HL_ATTR(HLF_N);

    എങ്കിൽ (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
    {
	എങ്കിൽ ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    സ്ഥിതം ശൂന്യം
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (*p == '\n')
	{
	    ga_append(gap, ' ');
	    ആകുമ്പോൾ (*++p != NUL)
		എങ്കിൽ (!VIM_ISWHITE(*p) && *p != '\n')
		    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    സ്ഥിതം ശൂന്യം
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((അക്ഷരം *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    എങ്കിൽ (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((അക്ഷരം *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    എങ്കിൽ (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((അക്ഷരം *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	എങ്കിൽ (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((അക്ഷരം *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    സ്ഥിതം ശൂന്യം
qf_സന്ദേശം(qf_info_T *qi, പൂർണ്ണസംഖ്യ which, അക്ഷരം *lead)
{
    അക്ഷരം   *title = (അക്ഷരം *)qi->qf_lists[which].qf_title;
    പൂർണ്ണസംഖ്യ    എണ്ണം = qi->qf_lists[which].qf_എണ്ണം;
    char_u buf[IOSIZE];

    vim_snprintf((അക്ഷരം *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    എണ്ണം);

    എങ്കിൽ (title != ശൂന്യ)
    {
	size_t	len = STRLEN(buf);

	എങ്കിൽ (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    സന്ദേശം((അക്ഷരം *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    ശൂന്യം
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    പൂർണ്ണസംഖ്യ		എണ്ണം;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (eap->addr_എണ്ണം != 0)
	എണ്ണം = eap->line2;
    അല്ലെങ്കിൽ
	എണ്ണം = 1;
    ആകുമ്പോൾ (എണ്ണം--)
    {
	എങ്കിൽ (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    എങ്കിൽ (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_അട്ടി));
		നിർത്തുക;
	    }
	    --qi->qf_curlist;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_മുകൾ_of_quickfix_അട്ടി));
		നിർത്തുക;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_സന്ദേശം(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, ശൂന്യ);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    ശൂന്യം
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_അട്ടി(eap, FALSE);
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (eap->addr_എണ്ണം > 0)
    {
	എങ്കിൽ (qi == ശൂന്യ)
	{
	    emsg(_(e_no_location_list));
	    തിരികെ;
	}

	// Jump to the specified quickfix list
	എങ്കിൽ (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_സന്ദേശം(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	    emsg(_(e_invalid_range));

	തിരികെ;
    }

    എങ്കിൽ (qf_അട്ടി_empty(qi))
	സന്ദേശം(_("No entries"));
    അല്ലെങ്കിൽ
	വേണ്ടി (i = 0; i < qi->qf_listcount; ++i)
	    qf_സന്ദേശം(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    സ്ഥിതം ശൂന്യം
qf_free_ഇനങ്ങൾ(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    പൂർണ്ണസംഖ്യ		stop = FALSE;

    ആകുമ്പോൾ (qfl->qf_എണ്ണം && qfl->qf_start != ശൂന്യ)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	എങ്കിൽ (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    എങ്കിൽ (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_എണ്ണം = 1;
	    അല്ലെങ്കിൽ
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_എണ്ണം;
    }

    qfl->qf_index = 0;
    qfl->qf_start = ശൂന്യ;
    qfl->qf_last = ശൂന്യ;
    qfl->qf_ptr = ശൂന്യ;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_അട്ടി(&qfl->qf_dir_അട്ടി);
    qfl->qf_directory = ശൂന്യ;
    qf_clean_dir_അട്ടി(&qfl->qf_file_അട്ടി);
    qfl->qf_currfile = ശൂന്യ;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    സ്ഥിതം ശൂന്യം
qf_free(qf_list_T *qfl)
{
    qf_free_ഇനങ്ങൾ(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = ശൂന്യ;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   ശൂന്യം
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	long	amount,
	long	amount_after)
{
    പൂർണ്ണസംഖ്യ		i;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		idx;
    qf_info_T	*qi = ql_info;
    പൂർണ്ണസംഖ്യ		found_one = FALSE;
    പൂർണ്ണസംഖ്യ		buf_has_flag = wp == ശൂന്യ ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    എങ്കിൽ (!(curbuf->b_has_qf_entry & buf_has_flag))
	തിരികെ;
    എങ്കിൽ (wp != ശൂന്യ)
    {
	എങ്കിൽ (wp->w_llist == ശൂന്യ)
	    തിരികെ;
	qi = wp->w_llist;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	എങ്കിൽ (!qf_list_empty(qfl))
	    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
		എങ്കിൽ (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    എങ്കിൽ (qfp->qf_cleared)
			തുടരുക;

		    എങ്കിൽ (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			എങ്കിൽ (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			അല്ലെങ്കിൽ
			    qfp->qf_lnum += amount;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    എങ്കിൽ (!found_one)
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
    സ്ഥിതം char_u *
qf_types(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ nr)
{
    സ്ഥിതം char_u	buf[20];
    സ്ഥിതം char_u	cc[3];
    char_u		*p;

    എങ്കിൽ (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'I' || c == 'i')
	p = (char_u *)" info";
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'N' || c == 'n')
	p = (char_u *)" note";
    അല്ലെങ്കിൽ എങ്കിൽ (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    അല്ലെങ്കിൽ എങ്കിൽ (c == 0 || c == 1)
	p = (char_u *)"";
    അല്ലെങ്കിൽ
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    എങ്കിൽ (nr <= 0)
	തിരികെ p;

    sprintf((അക്ഷരം *)buf, "%s %3d", (അക്ഷരം *)p, nr);
    തിരികെ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    ശൂന്യം
qf_view_result(പൂർണ്ണസംഖ്യ split)
{
    qf_info_T   *qi = ql_info;

    എങ്കിൽ (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
    {
	emsg(_(e_no_quickfix_അട്ടി));
	തിരികെ;
    }

    എങ്കിൽ (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	തിരികെ;
    }

    എങ്കിൽ (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	തിരികെ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    ശൂന്യം
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    എങ്കിൽ (qf_അട്ടി_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	എങ്കിൽ (win != ശൂന്യ)
	    ex_cclose(eap);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (win == ശൂന്യ)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    ശൂന്യം
ex_cclose(exarg_T *eap)
{
    win_T	*win = ശൂന്യ;
    qf_info_T	*qi;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, FALSE)) == ശൂന്യ)
	തിരികെ;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    എങ്കിൽ (win != ശൂന്യ)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    സ്ഥിതം ശൂന്യം
qf_set_title_var(qf_list_T *qfl)
{
    എങ്കിൽ (qfl->qf_title != ശൂന്യ)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_goto_cwindow(qf_info_T *qi, പൂർണ്ണസംഖ്യ resize, പൂർണ്ണസംഖ്യ sz, പൂർണ്ണസംഖ്യ vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    എങ്കിൽ (win == ശൂന്യ)
	തിരികെ FAIL;

    win_goto(win);
    എങ്കിൽ (resize)
    {
	എങ്കിൽ (vertsplit)
	{
	    എങ്കിൽ (sz != win->w_width)
		win_setwidth(sz);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    തിരികെ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    സ്ഥിതം ശൂന്യം
qf_set_cwindow_options(ശൂന്യം)
{
    // switch off 'swapfile'
    set_option_മൂല്യം_give_err((char_u *)"swf", 0L, ശൂന്യ, OPT_LOCAL);
    set_option_മൂല്യം_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_മൂല്യം_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_മൂല്യം_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_open_new_cwindow(qf_info_T *qi, പൂർണ്ണസംഖ്യ height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    പൂർണ്ണസംഖ്യ		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    എങ്കിൽ (IS_QF_അട്ടി(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    എങ്കിൽ (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    എങ്കിൽ (IS_QF_അട്ടി(qi))
	flags |= WSP_QUICKFIX;

    എങ്കിൽ (win_split(height, flags) == FAIL)
	തിരികെ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    എങ്കിൽ (IS_LL_അട്ടി(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    എങ്കിൽ (oldwin != curwin)
	oldwin = ശൂന്യ;  // don't store info when in another window
    എങ്കിൽ (qf_buf != ശൂന്യ)
    {
	// Use the existing quickfix buffer
	എങ്കിൽ (do_ecmd(qf_buf->b_fnum, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	// Create a new quickfix buffer
	എങ്കിൽ (do_ecmd(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    തിരികെ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    എങ്കിൽ (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    എങ്കിൽ (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    എങ്കിൽ (win_valid(win))
	prevwin = win;

    തിരികെ OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    ശൂന്യം
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    പൂർണ്ണസംഖ്യ		height;
    പൂർണ്ണസംഖ്യ		status = FAIL;
    പൂർണ്ണസംഖ്യ		lnum;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    incr_quickfix_busy();

    എങ്കിൽ (eap->addr_എണ്ണം != 0)
	height = eap->line2;
    അല്ലെങ്കിൽ
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    എങ്കിൽ (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_എണ്ണം != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    എങ്കിൽ (status == FAIL)
	എങ്കിൽ (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    തിരികെ;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, ശൂന്യ, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    സ്ഥിതം ശൂന്യം
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
    ശൂന്യം
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    win = qf_find_win(qi);
    എങ്കിൽ (win != ശൂന്യ && win->w_cursor.lnum != win->w_buffer->b_ml.ml_വരി_എണ്ണം)
	qf_win_goto(win, win->w_buffer->b_ml.ml_വരി_എണ്ണം);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    എങ്കിൽ (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ 0;

    തിരികെ qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_win_pos_update(
    qf_info_T	*qi,
    പൂർണ്ണസംഖ്യ		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    പൂർണ്ണസംഖ്യ		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    എങ്കിൽ (win != ശൂന്യ
	    && qf_index <= win->w_buffer->b_ml.ml_വരി_എണ്ണം
	    && old_qf_index != qf_index)
    {
	എങ്കിൽ (qf_index > old_qf_index)
	{
	    win->w_redraw_മുകൾ = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	അല്ലെങ്കിൽ
	{
	    win->w_redraw_മുകൾ = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    തിരികെ win != ശൂന്യ;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    എങ്കിൽ (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	എങ്കിൽ ((IS_QF_അട്ടി(qi) && win->w_llist_ref == ശൂന്യ)
		|| (IS_LL_അട്ടി(qi) && win->w_llist_ref == qi))
	    തിരികെ TRUE;

    തിരികെ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    സ്ഥിതം win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	എങ്കിൽ (is_qf_win(win, qi))
	    തിരികെ win;
    തിരികെ ശൂന്യ;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    സ്ഥിതം buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    എങ്കിൽ (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	എങ്കിൽ (qfbuf != ശൂന്യ)
	    തിരികെ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	എങ്കിൽ (is_qf_win(win, qi))
	    തിരികെ win->w_buffer;

    തിരികെ ശൂന്യ;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    അക്ഷരം *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    എങ്കിൽ (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    സ്ഥിതം ശൂന്യം
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	എങ്കിൽ (is_qf_win(win, qi))
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
    സ്ഥിതം ശൂന്യം
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    linenr_T	old_വരി_എണ്ണം = buf->b_ml.ml_വരി_എണ്ണം;
    പൂർണ്ണസംഖ്യ		qf_winid = 0;

    എങ്കിൽ (IS_LL_അട്ടി(qi))
    {
	എങ്കിൽ (curwin->w_llist == qi)
	    win = curwin;
	അല്ലെങ്കിൽ
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    എങ്കിൽ (win == ശൂന്യ)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    എങ്കിൽ (win == ശൂന്യ)
		തിരികെ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    പൂർണ്ണസംഖ്യ do_fill = TRUE;
    എങ്കിൽ (old_last == ശൂന്യ)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	എങ്കിൽ (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    എങ്കിൽ (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	എങ്കിൽ (old_last == ശൂന്യ)
	{
	    (ശൂന്യം)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    എങ്കിൽ ((win = qf_find_win(qi)) != ശൂന്യ && old_വരി_എണ്ണം < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_buf_add_വരി(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*dirname,
	പൂർണ്ണസംഖ്യ		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    എങ്കിൽ (qftf_str != ശൂന്യ && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (qfp->qf_module != ശൂന്യ)
	    ga_concat(gap, qfp->qf_module);
	അല്ലെങ്കിൽ എങ്കിൽ (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != ശൂന്യ
		&& errbuf->b_fname != ശൂന്യ)
	{
	    എങ്കിൽ (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    അല്ലെങ്കിൽ
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		എങ്കിൽ (first_bufline && (errbuf->b_sfname == ശൂന്യ
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    എങ്കിൽ (*dirname == NUL)
			mch_dirname(dirname, MAXPATHL);
		    shorten_buf_fname(errbuf, dirname, FALSE);
		}
		എങ്കിൽ (qfp->qf_fname == ശൂന്യ)
		    ga_concat(gap, errbuf->b_fname);
		അല്ലെങ്കിൽ
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	എങ്കിൽ (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (qfp->qf_pattern != ശൂന്യ)
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
    എങ്കിൽ (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    സ്ഥിതം list_T *
call_qftf_func(qf_list_T *qfl, പൂർണ്ണസംഖ്യ qf_winid, long start_idx, long end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = FALSE;

    എങ്കിൽ (recursive)
	തിരികെ ശൂന്യ;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    എങ്കിൽ (qfl->qf_qftf_cb.cb_name != ശൂന്യ)
	cb = &qfl->qf_qftf_cb;
    എങ്കിൽ (cb->cb_name != ശൂന്യ)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	എങ്കിൽ ((d = dict_alloc_lock(VAR_FIXED)) == ശൂന്യ)
	{
	    recursive = FALSE;
	    തിരികെ ശൂന്യ;
	}
	dict_add_സംഖ്യ(d, "quickfix", (long)IS_QF_LIST(qfl));
	dict_add_സംഖ്യ(d, "winid", (long)qf_winid);
	dict_add_സംഖ്യ(d, "id", (long)qfl->qf_id);
	dict_add_സംഖ്യ(d, "start_idx", start_idx);
	dict_add_സംഖ്യ(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = ശൂന്യ;
	textlock++;
	എങ്കിൽ (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    എങ്കിൽ (rettv.v_type == VAR_LIST)
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
    തിരികെ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    സ്ഥിതം ശൂന്യം
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, പൂർണ്ണസംഖ്യ qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		old_താക്കോൽ_Typed = താക്കോൽ_Typed;
    list_T	*qftf_list = ശൂന്യ;
    listitem_T	*qftf_li = ശൂന്യ;

    എങ്കിൽ (old_last == ശൂന്യ)
    {
	win_T		*wp;
	tabpage_T	*tp;

	എങ്കിൽ (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    തിരികെ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	ആകുമ്പോൾ ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (ശൂന്യം)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    എങ്കിൽ (qfl != ശൂന്യ && qfl->qf_start != ശൂന്യ)
    {
	char_u		dirname[MAXPATHL];
	പൂർണ്ണസംഖ്യ		invalid_val = FALSE;
	പൂർണ്ണസംഖ്യ		prev_bufnr = -1;

	*dirname = NUL;

	// Add one line for each error
	എങ്കിൽ (old_last == ശൂന്യ)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (old_last->qf_next != ശൂന്യ)
		qfp = old_last->qf_next;
	    അല്ലെങ്കിൽ
		qfp = old_last;
	    lnum = buf->b_ml.ml_വരി_എണ്ണം;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (long)(lnum + 1),
							(long)qfl->qf_എണ്ണം);
	എങ്കിൽ (qftf_list != ശൂന്യ)
	    qftf_li = qftf_list->lv_first;

	ആകുമ്പോൾ (lnum < qfl->qf_എണ്ണം)
	{
	    char_u	*qftf_str = ശൂന്യ;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    എങ്കിൽ (qftf_li != ശൂന്യ && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		എങ്കിൽ (qftf_str == ശൂന്യ)
		    invalid_val = TRUE;
	    }

	    എങ്കിൽ (qf_buf_add_വരി(buf, lnum, qfp, dirname,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		നിർത്തുക;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    എങ്കിൽ (qfp == ശൂന്യ)
		നിർത്തുക;

	    എങ്കിൽ (qftf_li != ശൂന്യ)
		qftf_li = qftf_li->li_next;
	}

	എങ്കിൽ (old_last == ശൂന്യ)
	    // Delete the empty line which is now at the end
	    (ശൂന്യം)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    എങ്കിൽ (old_last == ശൂന്യ)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_മൂല്യം_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", ശൂന്യ,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", ശൂന്യ,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    താക്കോൽ_Typed = old_താക്കോൽ_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    സ്ഥിതം ശൂന്യം
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    പൂർണ്ണസംഖ്യ		qf_idx;

    വേണ്ടി (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	എങ്കിൽ (qi->qf_lists[qf_idx].qf_id == qfid)
	    തിരികെ qf_idx;
    തിരികെ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    പൂർണ്ണസംഖ്യ curlist;

    എങ്കിൽ (qf_get_curlist(qi)->qf_id == save_qfid)
	തിരികെ OK;

    curlist = qf_id2nr(qi, save_qfid);
    എങ്കിൽ (curlist < 0)
	// list is not present
	തിരികെ FAIL;
    qi->qf_curlist = curlist;
    തിരികെ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    സ്ഥിതം ശൂന്യം
qf_jump_first(qf_info_T *qi, int_u save_qfid, പൂർണ്ണസംഖ്യ forceit)
{
    എങ്കിൽ (qf_restore_list(qi, save_qfid) == FAIL)
	തിരികെ;


    എങ്കിൽ (!check_can_set_curbuf_forceit(forceit))
	തിരികെ;


    // Autocommands might have cleared the list, check for that.
    എങ്കിൽ (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    പൂർണ്ണസംഖ്യ
grep_internal(cmdidx_T cmdidx)
{
    തിരികെ ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    സ്ഥിതം char_u *
make_get_auname(cmdidx_T cmdidx)
{
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_make:	    തിരികെ (char_u *)"make";
	അവസ്ഥ CMD_lmake:	    തിരികെ (char_u *)"lmake";
	അവസ്ഥ CMD_grep:	    തിരികെ (char_u *)"grep";
	അവസ്ഥ CMD_lgrep:	    തിരികെ (char_u *)"lgrep";
	അവസ്ഥ CMD_grepadd:   തിരികെ (char_u *)"grepadd";
	അവസ്ഥ CMD_lgrepadd:  തിരികെ (char_u *)"lgrepadd";
	സ്ഥിരസ്ഥിതി: തിരികെ ശൂന്യ;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    സ്ഥിതം char_u *
get_mef_name(ശൂന്യം)
{
    char_u	*p;
    char_u	*name;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	start = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    എങ്കിൽ (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_cant_get_temp_file_name));
	തിരികെ name;
    }

    വേണ്ടി (p = p_mef; *p; ++p)
	എങ്കിൽ (p[0] == '#' && p[1] == '#')
	    നിർത്തുക;

    എങ്കിൽ (*p == NUL)
	തിരികെ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    വേണ്ടി (;;)
    {
	എങ്കിൽ (start == -1)
	    start = mch_get_pid();
	അല്ലെങ്കിൽ
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	എങ്കിൽ (name == ശൂന്യ)
	    നിർത്തുക;
	STRCPY(name, p_mef);
	sprintf((അക്ഷരം *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	എങ്കിൽ (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((അക്ഷരം *)name, &sb) < 0
# endif
		)
	    നിർത്തുക;
	vim_free(name);
    }
    തിരികെ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    സ്ഥിതം char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    unsigned	len;

    len = (unsigned)STRLEN(p_shq) * 2 + (unsigned)STRLEN(makecmd) + 1;
    എങ്കിൽ (*p_sp != NUL)
	len += (unsigned)STRLEN(p_sp) + (unsigned)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    എങ്കിൽ (cmd == ശൂന്യ)
	തിരികെ ശൂന്യ;
    sprintf((അക്ഷരം *)cmd, "%s%s%s", (അക്ഷരം *)p_shq, (അക്ഷരം *)makecmd,
							       (അക്ഷരം *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    എങ്കിൽ (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    എങ്കിൽ (സന്ദേശം_col == 0)
	സന്ദേശം_didout = FALSE;
    സന്ദേശം_start();
    സന്ദേശം_puts(":!");
    സന്ദേശം_outtrans(cmd);		// show what we are doing

    തിരികെ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    ശൂന്യം
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = ശൂന്യ;
    win_T	*wp = ശൂന്യ;
    qf_info_T	*qi = ql_info;
    പൂർണ്ണസംഖ്യ		res;
    char_u	*au_name = ശൂന്യ;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    പൂർണ്ണസംഖ്യ		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    എങ്കിൽ (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	തിരികെ;
    }

    au_name = make_get_auname(eap->cmdidx);
    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	എങ്കിൽ (aborting())
	    തിരികെ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    എങ്കിൽ (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    എങ്കിൽ (cmd == ശൂന്യ)
    {
	vim_free(fname);
	തിരികെ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (ശൂന്യം)char_avail();
# endif

    incr_quickfix_busy();

    എങ്കിൽ (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    എങ്കിൽ (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    എങ്കിൽ (wp != ശൂന്യ)
    {
	qi = GET_LOC_LIST(wp);
	എങ്കിൽ (qi == ശൂന്യ)
	    പോകുക cleanup;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
	പോകുക cleanup;

    എങ്കിൽ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    എങ്കിൽ (au_name != ശൂന്യ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    എങ്കിൽ (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    പൂർണ്ണസംഖ്യ
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, FALSE)) == ശൂന്യ)
	തിരികെ 0;
    തിരികെ qf_get_curlist(qi)->qf_എണ്ണം;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    പൂർണ്ണസംഖ്യ
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i, sz = 0;
    പൂർണ്ണസംഖ്യ		prev_fnum = 0;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, FALSE)) == ശൂന്യ)
	തിരികെ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
    {
	എങ്കിൽ (qfp->qf_valid)
	{
	    എങ്കിൽ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    അല്ലെങ്കിൽ എങ്കിൽ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    തിരികെ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    പൂർണ്ണസംഖ്യ
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, FALSE)) == ശൂന്യ)
	തിരികെ 0;

    തിരികെ qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    പൂർണ്ണസംഖ്യ
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i, eidx = 0;
    പൂർണ്ണസംഖ്യ		prev_fnum = 0;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, FALSE)) == ശൂന്യ)
	തിരികെ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    എങ്കിൽ (!qf_list_has_valid_entries(qfl))
	തിരികെ 1;

    വേണ്ടി (i = 1; i <= qfl->qf_index && qfp!= ശൂന്യ; i++, qfp = qfp->qf_next)
    {
	എങ്കിൽ (qfp->qf_valid)
	{
	    എങ്കിൽ (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		എങ്കിൽ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    അല്ലെങ്കിൽ
		eidx++;
	}
    }

    തിരികെ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_nth_valid_entry(qf_list_T *qfl, പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ fdo)
{
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i, eidx;
    പൂർണ്ണസംഖ്യ		prev_fnum = 0;

    // check if the list has valid errors
    എങ്കിൽ (!qf_list_has_valid_entries(qfl))
	തിരികെ 1;

    eidx = 0;
    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
    {
	എങ്കിൽ (qfp->qf_valid)
	{
	    എങ്കിൽ (fdo)
	    {
		എങ്കിൽ (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    അല്ലെങ്കിൽ
		eidx++;
	}

	എങ്കിൽ (eidx == n)
	    നിർത്തുക;
    }

    എങ്കിൽ (i <= qfl->qf_എണ്ണം)
	തിരികെ i;
    അല്ലെങ്കിൽ
	തിരികെ 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ശൂന്യം
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    പൂർണ്ണസംഖ്യ		errornr;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (eap->addr_എണ്ണം > 0)
	errornr = (പൂർണ്ണസംഖ്യ)eap->line2;
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
	{
	    അവസ്ഥ CMD_cc: അവസ്ഥ CMD_ll:
		errornr = 0;
		നിർത്തുക;
	    അവസ്ഥ CMD_crewind: അവസ്ഥ CMD_lrewind: അവസ്ഥ CMD_cfirst:
	    അവസ്ഥ CMD_lfirst:
		errornr = 1;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    എങ്കിൽ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_എണ്ണം > 0 ? (പൂർണ്ണസംഖ്യ)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    ശൂന്യം
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    പൂർണ്ണസംഖ്യ		errornr;
    പൂർണ്ണസംഖ്യ		dir;

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (eap->addr_എണ്ണം > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (പൂർണ്ണസംഖ്യ)eap->line2;
    അല്ലെങ്കിൽ
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
    {
	അവസ്ഥ CMD_cnext: അവസ്ഥ CMD_lnext: അവസ്ഥ CMD_cdo: അവസ്ഥ CMD_ldo:
	    dir = FORWARD;
	    നിർത്തുക;
	അവസ്ഥ CMD_cprevious: അവസ്ഥ CMD_lprevious: അവസ്ഥ CMD_cNext:
	അവസ്ഥ CMD_lNext:
	    dir = BACKWARD;
	    നിർത്തുക;
	അവസ്ഥ CMD_cnfile: അവസ്ഥ CMD_lnfile: അവസ്ഥ CMD_cfdo: അവസ്ഥ CMD_lfdo:
	    dir = FORWARD_FILE;
	    നിർത്തുക;
	അവസ്ഥ CMD_cpfile: അവസ്ഥ CMD_lpfile: അവസ്ഥ CMD_cNfile: അവസ്ഥ CMD_lNfile:
	    dir = BACKWARD_FILE;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    dir = FORWARD;
	    നിർത്തുക;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    സ്ഥിതം qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, പൂർണ്ണസംഖ്യ bnr, പൂർണ്ണസംഖ്യ *errornr)
{
    qfline_T	*qfp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, idx)
	എങ്കിൽ (qfp->qf_fnum == bnr)
	    നിർത്തുക;

    *errornr = idx;
    തിരികെ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    സ്ഥിതം qfline_T *
qf_find_first_entry_on_വരി(qfline_T *entry, പൂർണ്ണസംഖ്യ *errornr)
{
    ആകുമ്പോൾ (!got_int
	    && entry->qf_prev != ശൂന്യ
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    തിരികെ entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    സ്ഥിതം qfline_T *
qf_find_last_entry_on_വരി(qfline_T *entry, പൂർണ്ണസംഖ്യ *errornr)
{
    ആകുമ്പോൾ (!got_int &&
	    entry->qf_next != ശൂന്യ
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    തിരികെ entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, പൂർണ്ണസംഖ്യ linewise)
{
    എങ്കിൽ (linewise)
	തിരികെ qfp->qf_lnum > pos->lnum;
    അല്ലെങ്കിൽ
	തിരികെ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, പൂർണ്ണസംഖ്യ linewise)
{
    എങ്കിൽ (linewise)
	തിരികെ qfp->qf_lnum < pos->lnum;
    അല്ലെങ്കിൽ
	തിരികെ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, പൂർണ്ണസംഖ്യ linewise)
{
    എങ്കിൽ (linewise)
	തിരികെ qfp->qf_lnum >= pos->lnum;
    അല്ലെങ്കിൽ
	തിരികെ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, പൂർണ്ണസംഖ്യ linewise)
{
    എങ്കിൽ (linewise)
	തിരികെ qfp->qf_lnum <= pos->lnum;
    അല്ലെങ്കിൽ
	തിരികെ (qfp->qf_lnum < pos->lnum ||
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
    സ്ഥിതം qfline_T *
qf_find_entry_after_pos(
	പൂർണ്ണസംഖ്യ		bnr,
	pos_T		*pos,
	പൂർണ്ണസംഖ്യ		linewise,
	qfline_T	*qfp,
	പൂർണ്ണസംഖ്യ		*errornr)
{
    എങ്കിൽ (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	തിരികെ qfp;

    // Find the entry just before or at the position 'pos'
    ആകുമ്പോൾ (qfp->qf_next != ശൂന്യ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    എങ്കിൽ (qfp->qf_next == ശൂന്യ || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	തിരികെ ശൂന്യ;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    തിരികെ qfp;
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
    സ്ഥിതം qfline_T *
qf_find_entry_before_pos(
	പൂർണ്ണസംഖ്യ		bnr,
	pos_T		*pos,
	പൂർണ്ണസംഖ്യ		linewise,
	qfline_T	*qfp,
	പൂർണ്ണസംഖ്യ		*errornr)
{
    // Find the entry just before the position 'pos'
    ആകുമ്പോൾ (qfp->qf_next != ശൂന്യ
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    എങ്കിൽ (qf_entry_on_or_after_pos(qfp, pos, linewise))
	തിരികെ ശൂന്യ;

    എങ്കിൽ (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_വരി(qfp, errornr);

    തിരികെ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    സ്ഥിതം qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	പൂർണ്ണസംഖ്യ		bnr,
	pos_T		*pos,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		linewise,
	പൂർണ്ണസംഖ്യ		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    എങ്കിൽ (qfp == ശൂന്യ)
	തിരികെ ശൂന്യ;		// no entry in this file

    എങ്കിൽ (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    അല്ലെങ്കിൽ
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    തിരികെ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    സ്ഥിതം ശൂന്യം
qf_get_nth_below_entry(qfline_T *entry_arg, പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ linewise, പൂർണ്ണസംഖ്യ *errornr)
{
    qfline_T *entry = entry_arg;

    ആകുമ്പോൾ (n-- > 0 && !got_int)
    {
	പൂർണ്ണസംഖ്യ		first_errornr = *errornr;

	എങ്കിൽ (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_വരി(entry, errornr);

	എങ്കിൽ (entry->qf_next == ശൂന്യ
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    എങ്കിൽ (linewise)
		*errornr = first_errornr;
	    നിർത്തുക;
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
    സ്ഥിതം ശൂന്യം
qf_get_nth_above_entry(qfline_T *entry, പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ linewise, പൂർണ്ണസംഖ്യ *errornr)
{
    ആകുമ്പോൾ (n-- > 0 && !got_int)
    {
	എങ്കിൽ (entry->qf_prev == ശൂന്യ
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    നിർത്തുക;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	എങ്കിൽ (linewise)
	    entry = qf_find_first_entry_on_വരി(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	പൂർണ്ണസംഖ്യ		bnr,
	pos_T		*pos,
	പൂർണ്ണസംഖ്യ		n,
	പൂർണ്ണസംഖ്യ		dir,
	പൂർണ്ണസംഖ്യ		linewise)
{
    qfline_T	*adj_entry;
    പൂർണ്ണസംഖ്യ		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    എങ്കിൽ (adj_entry == ശൂന്യ)
	തിരികെ 0;

    എങ്കിൽ (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	എങ്കിൽ (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	അല്ലെങ്കിൽ
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    തിരികെ errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    ശൂന്യം
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    പൂർണ്ണസംഖ്യ		dir;
    പൂർണ്ണസംഖ്യ		buf_has_flag;
    പൂർണ്ണസംഖ്യ		errornr = 0;
    pos_T	pos;

    എങ്കിൽ (eap->addr_എണ്ണം > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	തിരികെ;
    }

    // Check whether the current buffer has any quickfix entries
    എങ്കിൽ (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    അല്ലെങ്കിൽ
	buf_has_flag = BUF_HAS_LL_ENTRY;
    എങ്കിൽ (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	തിരികെ;
    }

    എങ്കിൽ ((qi = qf_cmd_get_അട്ടി(eap, TRUE)) == ശൂന്യ)
	തിരികെ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    എങ്കിൽ (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	തിരികെ;
    }

    എങ്കിൽ (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    അല്ലെങ്കിൽ
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_എണ്ണം > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    എങ്കിൽ (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    അല്ലെങ്കിൽ
	emsg(_(e_no_more_ഇനങ്ങൾ));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    സ്ഥിതം char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_cfile:	    തിരികെ (char_u *)"cfile";
	അവസ്ഥ CMD_cgetfile:  തിരികെ (char_u *)"cgetfile";
	അവസ്ഥ CMD_caddfile:  തിരികെ (char_u *)"caddfile";
	അവസ്ഥ CMD_lfile:	    തിരികെ (char_u *)"lfile";
	അവസ്ഥ CMD_lgetfile:  തിരികെ (char_u *)"lgetfile";
	അവസ്ഥ CMD_laddfile:  തിരികെ (char_u *)"laddfile";
	സ്ഥിരസ്ഥിതി:	    തിരികെ ശൂന്യ;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    ശൂന്യം
ex_cfile(exarg_T *eap)
{
    char_u	*enc = ശൂന്യ;
    win_T	*wp = ശൂന്യ;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = ശൂന്യ;
    int_u	save_qfid = 0;		// init for gcc
    പൂർണ്ണസംഖ്യ		res;

    au_name = cfile_get_auname(eap->cmdidx);
    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							ശൂന്യ, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	എങ്കിൽ (aborting())
	    തിരികെ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    എങ്കിൽ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   ശൂന്യ, ശൂന്യ,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), ശൂന്യ);
	എങ്കിൽ (browse_file == ശൂന്യ)
	    തിരികെ;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    അല്ലെങ്കിൽ
# endif
    എങ്കിൽ (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    എങ്കിൽ (is_loclist_cmd(eap->cmdidx))
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
    എങ്കിൽ (wp != ശൂന്യ)
    {
	qi = GET_LOC_LIST(wp);
	എങ്കിൽ (qi == ശൂന്യ)
	{
	    decr_quickfix_busy();
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
    {
	decr_quickfix_busy();
	തിരികെ;
    }
    എങ്കിൽ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    എങ്കിൽ (au_name != ശൂന്യ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, ശൂന്യ, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    എങ്കിൽ (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    സ്ഥിതം char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_vimgrep:     തിരികെ (char_u *)"vimgrep";
	അവസ്ഥ CMD_lvimgrep:    തിരികെ (char_u *)"lvimgrep";
	അവസ്ഥ CMD_vimgrepadd:  തിരികെ (char_u *)"vimgrepadd";
	അവസ്ഥ CMD_lvimgrepadd: തിരികെ (char_u *)"lvimgrepadd";
	അവസ്ഥ CMD_grep:	      തിരികെ (char_u *)"grep";
	അവസ്ഥ CMD_lgrep:	      തിരികെ (char_u *)"lgrep";
	അവസ്ഥ CMD_grepadd:     തിരികെ (char_u *)"grepadd";
	അവസ്ഥ CMD_lgrepadd:    തിരികെ (char_u *)"lgrepadd";
	സ്ഥിരസ്ഥിതി: തിരികെ ശൂന്യ;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    സ്ഥിതം ശൂന്യം
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = ശൂന്യ;

    എങ്കിൽ (s == ശൂന്യ || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	എങ്കിൽ (last_തിരയൽ_pat() == ശൂന്യ)
	{
	    emsg(_(e_no_previous_regular_expression));
	    തിരികെ;
	}
	regmatch->regprog = vim_regcomp(last_തിരയൽ_pat(), RE_MAGIC);
    }
    അല്ലെങ്കിൽ
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    സ്ഥിതം ശൂന്യം
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    സന്ദേശം_start();
    p = സന്ദേശം_strtrunc(fname, TRUE);
    എങ്കിൽ (p == ശൂന്യ)
	സന്ദേശം_outtrans(fname);
    അല്ലെങ്കിൽ
    {
	സന്ദേശം_outtrans(p);
	vim_free(p);
    }
    സന്ദേശം_clr_eos();
    സന്ദേശം_didout = FALSE;	    // overwrite this message
    സന്ദേശം_nowait = TRUE;	    // don't wait for this message
    സന്ദേശം_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    സ്ഥിതം buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    പൂർണ്ണസംഖ്യ		save_mls;
# എങ്കിൽ defined(FEAT_SYN_HL)
    char_u	*save_ei = ശൂന്യ;
# endif
    buf_T	*buf;

# എങ്കിൽ defined(FEAT_SYN_HL)
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
# എങ്കിൽ defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    തിരികെ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    എങ്കിൽ (!qflist_valid(wp, qfid))
    {
	എങ്കിൽ (wp != ശൂന്യ)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    തിരികെ FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    തിരികെ TRUE;
	}
    }

    എങ്കിൽ (qf_restore_list(qi, qfid) == FAIL)
	തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	long	    *tomatch,
	പൂർണ്ണസംഖ്യ	    duplicate_name,
	പൂർണ്ണസംഖ്യ	    flags)
{
    പൂർണ്ണസംഖ്യ		found_match = FALSE;
    long	lnum;
    colnr_T	col;
    പൂർണ്ണസംഖ്യ		pat_len = (പൂർണ്ണസംഖ്യ)STRLEN(spat);
    എങ്കിൽ (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    വേണ്ടി (lnum = 1; lnum <= buf->b_ml.ml_വരി_എണ്ണം && *tomatch > 0; ++lnum)
    {
	col = 0;
	എങ്കിൽ (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    ആകുമ്പോൾ (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, ശൂന്യ) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		എങ്കിൽ (qf_add_entry(qfl,
			    ശൂന്യ,	// dir
			    fname,
			    ശൂന്യ,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    ശൂന്യ,	// search pattern
			    0,		// nr
			    0,		// type
			    ശൂന്യ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    നിർത്തുക;
		}
		found_match = TRUE;
		എങ്കിൽ (--*tomatch == 0)
		    നിർത്തുക;
		എങ്കിൽ ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    നിർത്തുക;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		എങ്കിൽ (col > ml_get_buf_len(buf, lnum))
		    നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    പൂർണ്ണസംഖ്യ	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    ആകുമ്പോൾ (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		എങ്കിൽ (qf_add_entry(qfl,
			    ശൂന്യ,	// dir
			    fname,
			    ശൂന്യ,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    ശൂന്യ,	// search pattern
			    0,		// nr
			    0,		// type
			    ശൂന്യ,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    നിർത്തുക;
		}
		found_match = TRUE;
		എങ്കിൽ (--*tomatch == 0)
		    നിർത്തുക;
		എങ്കിൽ ((flags & VGR_GLOBAL) == 0)
		    നിർത്തുക;
		col = matches[pat_len - 1] + col + 1;
		എങ്കിൽ (col > linelen)
		    നിർത്തുക;
	    }
	}
	വരി_breakcheck();
	എങ്കിൽ (got_int)
	    നിർത്തുക;
    }

    തിരികെ found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    സ്ഥിതം ശൂന്യം
vgr_jump_to_match(
	qf_info_T   *qi,
	പൂർണ്ണസംഖ്യ	    forceit,
	പൂർണ്ണസംഖ്യ	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    എങ്കിൽ (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    എങ്കിൽ (curbuf == first_match_buf && target_dir != ശൂന്യ)
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
തരനിർവചനം ഘടന
{
    long	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    പൂർണ്ണസംഖ്യ		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    പൂർണ്ണസംഖ്യ		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = ശൂന്യ;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    എങ്കിൽ (eap->addr_എണ്ണം > 0)
	args->tomatch = eap->line2;
    അല്ലെങ്കിൽ
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    എങ്കിൽ (p == ശൂന്യ)
    {
	emsg(_(e_invalid_തിരയൽ_pattern_or_delimiter));
	തിരികെ FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    എങ്കിൽ (args->regmatch.regprog == ശൂന്യ)
	തിരികെ FAIL;

    p = skipwhite(p);
    എങ്കിൽ (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	തിരികെ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    എങ്കിൽ ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
existing_swapfile(buf_T *buf)
{
    എങ്കിൽ (buf->b_ml.ml_mfp != ശൂന്യ && buf->b_ml.ml_mfp->mf_fname != ശൂന്യ)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	തിരികെ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    തിരികെ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	പൂർണ്ണസംഖ്യ		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    പൂർണ്ണസംഖ്യ		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    time_t	seconds = 0;
    char_u	*fname;
    പൂർണ്ണസംഖ്യ		fi;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		duplicate_name = FALSE;
    പൂർണ്ണസംഖ്യ		using_dummy;
    char_u	*dirname_start = ശൂന്യ;
    char_u	*dirname_now = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    എങ്കിൽ (dirname_start == ശൂന്യ || dirname_now == ശൂന്യ)
	പോകുക theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (time_t)0;
    വേണ്ടി (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	എങ്കിൽ (time(ശൂന്യ) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = time(ശൂന്യ);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != ശൂന്യ);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	അല്ലെങ്കിൽ
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	എങ്കിൽ (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    പോകുക theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	എങ്കിൽ (buf == ശൂന്യ)
	{
	    എങ്കിൽ (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	അല്ലെങ്കിൽ
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    എങ്കിൽ (using_dummy)
	    {
		എങ്കിൽ (found_match && *first_match_buf == ശൂന്യ)
		    *first_match_buf = buf;
		എങ്കിൽ (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = ശൂന്യ;
		}
		അല്ലെങ്കിൽ എങ്കിൽ ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    എങ്കിൽ (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = ശൂന്യ;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = ശൂന്യ;
		    }
		}

		എങ്കിൽ (buf != ശൂന്യ)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    എങ്കിൽ (buf == *first_match_buf
			    && *target_dir == ശൂന്യ
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    എങ്കിൽ (curbuf == buf)
		    {
# എങ്കിൽ defined(FEAT_SYN_HL)
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
    തിരികെ status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    ശൂന്യം
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = ശൂന്യ;
    char_u	*target_dir = ശൂന്യ;
    char_u	*au_name =  ശൂന്യ;
    പൂർണ്ണസംഖ്യ		status;

    എങ്കിൽ (!check_can_set_curbuf_forceit(eap->forceit))
	തിരികെ;

    au_name = vgr_get_auname(eap->cmdidx);
    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	എങ്കിൽ (aborting())
	    തിരികെ;
# endif
    }

    qi = qf_cmd_get_or_alloc_അട്ടി(eap, &wp);
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (vgr_process_args(eap, &args) == FAIL)
	പോകുക theend;

    എങ്കിൽ ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_അട്ടി_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    എങ്കിൽ (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	പോകുക theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, ശൂന്യ);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    എങ്കിൽ (au_name != ശൂന്യ)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    എങ്കിൽ (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	പോകുക theend;
    }

    // Jump to first match.
    എങ്കിൽ (!qf_list_empty(qf_get_curlist(qi)))
    {
	എങ്കിൽ ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    എങ്കിൽ (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    എങ്കിൽ (dirname_now == ശൂന്യ)
	തിരികെ;

    mch_dirname(dirname_now, MAXPATHL);
    എങ്കിൽ (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == ശൂന്യ) ? CMD_cd : CMD_lcd;
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
    സ്ഥിതം buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    പൂർണ്ണസംഖ്യ		failed = TRUE;
    aco_save_T	aco;
    പൂർണ്ണസംഖ്യ		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ശൂന്യ, ശൂന്യ, (linenr_T)1, BLN_DUMMY);
    എങ്കിൽ (newbuf == ശൂന്യ)
	തിരികെ ശൂന്യ;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    എങ്കിൽ (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	എങ്കിൽ (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (ശൂന്യം)setfname(curbuf, fname, ശൂന്യ, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = ശൂന്യ;
	    readfile_result = readfile(fname, ശൂന്യ,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			ശൂന്യ, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    എങ്കിൽ (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		എങ്കിൽ (curbuf != newbuf)
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

	    എങ്കിൽ (newbuf_to_wipe.br_buf != ശൂന്യ && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, ശൂന്യ);
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

    എങ്കിൽ (!bufref_valid(&newbufref))
	തിരികെ ശൂന്യ;
    എങ്കിൽ (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	തിരികെ ശൂന്യ;
    }
    തിരികെ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    സ്ഥിതം ശൂന്യം
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    ആകുമ്പോൾ (buf->b_nwindows > 0)
    {
	പൂർണ്ണസംഖ്യ	    did_one = FALSE;
	win_T	    *wp;

	എങ്കിൽ (firstwin->w_next != ശൂന്യ)
	    FOR_ALL_WINDOWS(wp)
		എങ്കിൽ (wp->w_buffer == buf)
		{
		    എങ്കിൽ (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    നിർത്തുക;
		}
	എങ്കിൽ (!did_one)
	    പോകുക fail;
    }

    എങ്കിൽ (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# എങ്കിൽ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# എങ്കിൽ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	എങ്കിൽ (dirname_start != ശൂന്യ)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	തിരികെ;
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
    സ്ഥിതം ശൂന്യം
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    എങ്കിൽ (curbuf == buf)		// safety check
	തിരികെ;

    close_buffer(ശൂന്യ, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# എങ്കിൽ defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_qfline_ഇനങ്ങൾ(qfline_T *qfp, list_T *list)
{
    പൂർണ്ണസംഖ്യ		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    size_t	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    എങ്കിൽ (bufnum != 0 && (buflist_findnr(bufnum) == ശൂന്യ))
	bufnum = 0;

    എങ്കിൽ ((dict = dict_alloc()) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	തിരികെ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    എങ്കിൽ (dict_add_സംഖ്യ(dict, "bufnr", (long)bufnum) == FAIL
	    || dict_add_സംഖ്യ(dict, "lnum",     (long)qfp->qf_lnum) == FAIL
	    || dict_add_സംഖ്യ(dict, "end_lnum", (long)qfp->qf_end_lnum) == FAIL
	    || dict_add_സംഖ്യ(dict, "col",      (long)qfp->qf_col) == FAIL
	    || dict_add_സംഖ്യ(dict, "end_col",  (long)qfp->qf_end_col) == FAIL
	    || dict_add_സംഖ്യ(dict, "vcol",     (long)qfp->qf_viscol) == FAIL
	    || dict_add_സംഖ്യ(dict, "nr",       (long)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (പൂർണ്ണസംഖ്യ)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_സംഖ്യ(dict, "valid", (long)qfp->qf_valid) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	പൂർണ്ണസംഖ്യ		qf_idx,
	പൂർണ്ണസംഖ്യ		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (qi == ശൂന്യ)
    {
	qi = ql_info;
	എങ്കിൽ (wp != ശൂന്യ)
	    qi = GET_LOC_LIST(wp);
	എങ്കിൽ (qi == ശൂന്യ)
	    തിരികെ FAIL;

    }

    എങ്കിൽ (eidx < 0)
	തിരികെ OK;

    എങ്കിൽ (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    എങ്കിൽ (qf_idx >= qi->qf_listcount)
	തിരികെ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    എങ്കിൽ (qf_list_empty(qfl))
	തിരികെ FAIL;

    FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, i)
    {
	എങ്കിൽ (eidx > 0)
	{
	    എങ്കിൽ (eidx == i)
		തിരികെ get_qfline_ഇനങ്ങൾ(qfp, list);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (get_qfline_ഇനങ്ങൾ(qfp, list) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
എണ്ണൽ {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_ഇനങ്ങൾ	= 0x2,
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_list_from_വരികൾ(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    എങ്കിൽ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ശൂന്യ)
	തിരികെ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    എങ്കിൽ ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ശൂന്യ)
    {
	എങ്കിൽ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ശൂന്യ)
	    തിരികെ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ FAIL;

    qi = qf_alloc_അട്ടി(QFLT_INTERNAL, 1);
    എങ്കിൽ (qi != ശൂന്യ)
    {
	എങ്കിൽ (qf_init_ext(qi, 0, ശൂന്യ, ശൂന്യ, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, ശൂന്യ, ശൂന്യ) > 0)
	{
	    (ശൂന്യം)get_errorlist(qi, ശൂന്യ, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    തിരികെ status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ 0;
    win = qf_find_win(qi);
    എങ്കിൽ (win != ശൂന്യ)
	തിരികെ win->w_id;
    തിരികെ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ	bufnum = 0;

    എങ്കിൽ (qi != ശൂന്യ && buflist_findnr(qi->qf_bufnr) != ശൂന്യ)
	bufnum = qi->qf_bufnr;

    തിരികെ dict_add_സംഖ്യ(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_keys2flags(dict_T *what, പൂർണ്ണസംഖ്യ loclist)
{
    പൂർണ്ണസംഖ്യ		flags = QF_GETLIST_NONE;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	എങ്കിൽ (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    എങ്കിൽ (dict_has_താക്കോൽ(what, "title"))
	flags |= QF_GETLIST_TITLE;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "nr"))
	flags |= QF_GETLIST_NR;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "winid"))
	flags |= QF_GETLIST_WINID;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "id"))
	flags |= QF_GETLIST_ID;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "items"))
	flags |= QF_GETLIST_ഇനങ്ങൾ;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "idx"))
	flags |= QF_GETLIST_IDX;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "size"))
	flags |= QF_GETLIST_SIZE;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    എങ്കിൽ (loclist && dict_has_താക്കോൽ(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    എങ്കിൽ (dict_has_താക്കോൽ(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    തിരികെ flags;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    പൂർണ്ണസംഖ്യ		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    എങ്കിൽ ((di = dict_find(what, (char_u *)"nr", -1)) != ശൂന്യ)
    {
	// Use the specified quickfix/location list
	എങ്കിൽ (di->di_tv.v_type == VAR_സംഖ്യ)
	{
	    // for zero use the current list
	    എങ്കിൽ (di->di_tv.vval.v_സംഖ്യ != 0)
	    {
		qf_idx = di->di_tv.vval.v_സംഖ്യ - 1;
		എങ്കിൽ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ശൂന്യ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	അല്ലെങ്കിൽ
	    qf_idx = INVALID_QFIDX;
    }

    എങ്കിൽ ((di = dict_find(what, (char_u *)"id", -1)) != ശൂന്യ)
    {
	// Look for a list with the specified id
	എങ്കിൽ (di->di_tv.v_type == VAR_സംഖ്യ)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    എങ്കിൽ (di->di_tv.vval.v_സംഖ്യ != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_സംഖ്യ);
	}
	അല്ലെങ്കിൽ
	    qf_idx = INVALID_QFIDX;
    }

    തിരികെ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_defaults(qf_info_T *qi, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ locstack, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		status = OK;

    എങ്കിൽ (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_ഇനങ്ങൾ))
    {
	list_T	*l = list_alloc();
	എങ്കിൽ (l != ശൂന്യ)
	    status = dict_add_list(retdict, "items", l);
	അല്ലെങ്കിൽ
	    status = FAIL;
    }
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_സംഖ്യ(retdict, "nr", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_സംഖ്യ(retdict, "winid", qf_winid(qi));
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_സംഖ്യ(retdict, "id", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_സംഖ്യ(retdict, "idx", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_സംഖ്യ(retdict, "size", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_സംഖ്യ(retdict, "changedtick", 0);
    എങ്കിൽ ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_സംഖ്യ(retdict, "filewinid", 0);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    തിരികെ status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    തിരികെ dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ winid = 0;

    എങ്കിൽ (wp != ശൂന്യ && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	എങ്കിൽ (ll_wp != ശൂന്യ)
	    winid = ll_wp->w_id;
    }

    തിരികെ dict_add_സംഖ്യ(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_ഇനങ്ങൾ(qf_info_T *qi, പൂർണ്ണസംഖ്യ qf_idx, പൂർണ്ണസംഖ്യ eidx, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		status = OK;
    list_T	*l = list_alloc();
    എങ്കിൽ (l != ശൂന്യ)
    {
	(ശൂന്യം)get_errorlist(qi, ശൂന്യ, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    അല്ലെങ്കിൽ
	status = FAIL;

    തിരികെ status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		status;
    dictitem_T	*di;

    എങ്കിൽ (qfl->qf_ctx != ശൂന്യ)
    {
	di = dictitem_alloc((char_u *)"context");
	എങ്കിൽ (di != ശൂന്യ)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    എങ്കിൽ (status == FAIL)
		dictitem_free(di);
	}
	അല്ലെങ്കിൽ
	    status = FAIL;
    }
    അല്ലെങ്കിൽ
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    തിരികെ status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_idx(qf_list_T *qfl, പൂർണ്ണസംഖ്യ eidx, dict_T *retdict)
{
    എങ്കിൽ (eidx == 0)
    {
	eidx = qfl->qf_index;
	എങ്കിൽ (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    തിരികെ dict_add_സംഖ്യ(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    പൂർണ്ണസംഖ്യ		status;

    എങ്കിൽ (qfl->qf_qftf_cb.cb_name != ശൂന്യ)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    അല്ലെങ്കിൽ
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    തിരികെ status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    പൂർണ്ണസംഖ്യ		status = OK;
    പൂർണ്ണസംഖ്യ		qf_idx = INVALID_QFIDX;
    പൂർണ്ണസംഖ്യ		eidx = 0;
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		flags = QF_GETLIST_NONE;

    എങ്കിൽ ((di = dict_find(what, (char_u *)"lines", -1)) != ശൂന്യ)
	തിരികെ qf_get_list_from_വരികൾ(what, di, retdict);

    എങ്കിൽ (wp != ശൂന്യ)
	qi = GET_LOC_LIST(wp);
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != ശൂന്യ));

    എങ്കിൽ (!qf_അട്ടി_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    എങ്കിൽ (qf_അട്ടി_empty(qi) || qf_idx == INVALID_QFIDX)
	തിരികെ qf_getprop_defaults(qi, flags, wp != ശൂന്യ, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    എങ്കിൽ ((di = dict_find(what, (char_u *)"idx", -1)) != ശൂന്യ)
    {
	എങ്കിൽ (di->di_tv.v_type != VAR_സംഖ്യ)
	    തിരികെ FAIL;
	eidx = di->di_tv.vval.v_സംഖ്യ;
    }

    എങ്കിൽ (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_സംഖ്യ(retdict, "nr", qf_idx + 1);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_സംഖ്യ(retdict, "winid", qf_winid(qi));
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_ഇനങ്ങൾ))
	status = qf_getprop_ഇനങ്ങൾ(qi, qf_idx, eidx, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_സംഖ്യ(retdict, "id", qfl->qf_id);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_സംഖ്യ(retdict, "size", qfl->qf_എണ്ണം);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_സംഖ്യ(retdict, "changedtick", qfl->qf_changedtick);
    എങ്കിൽ ((status == OK) && (wp != ശൂന്യ) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    എങ്കിൽ ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    തിരികെ status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	പൂർണ്ണസംഖ്യ		first_entry,
	പൂർണ്ണസംഖ്യ		*valid_entry)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    പൂർണ്ണസംഖ്യ		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    എങ്കിൽ (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "bufnr");
    lnum = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "lnum");
    end_lnum = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "end_lnum");
    col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "col");
    end_col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "end_col");
    vcol = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "vcol");
    nr = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    എങ്കിൽ (text == ശൂന്യ)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    എങ്കിൽ ((filename == ശൂന്യ && bufnum == 0) || (lnum == 0 && pattern == ശൂന്യ))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    എങ്കിൽ (bufnum != 0 && (buflist_findnr(bufnum) == ശൂന്യ))
    {
	എങ്കിൽ (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    എങ്കിൽ (dict_has_താക്കോൽ(d, "valid"))
	valid = (പൂർണ്ണസംഖ്യ)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			ശൂന്യ,		// dir
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
			type == ശൂന്യ ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    എങ്കിൽ (valid)
	*valid_entry = TRUE;

    തിരികെ status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	പൂർണ്ണസംഖ്യ		target_fnum,
	പൂർണ്ണസംഖ്യ		target_lnum,
	പൂർണ്ണസംഖ്യ		target_col)
{
    // First, compare entries to target file.
    എങ്കിൽ (!target_fnum)
	// Without a target file, we can't know which is closer.
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    പൂർണ്ണസംഖ്യ other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    എങ്കിൽ (!is_target_file && other_is_target_file)
	തിരികെ FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (is_target_file && !other_is_target_file)
	തിരികെ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    എങ്കിൽ (!target_lnum)
	// Without a target line number, we can't know which is closer.
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ വരി_distance = entry->qf_lnum ? labs(entry->qf_lnum - target_lnum) : INT_MAX;
    പൂർണ്ണസംഖ്യ other_വരി_distance = other_entry->qf_lnum ? labs(other_entry->qf_lnum - target_lnum) : INT_MAX;
    എങ്കിൽ (വരി_distance > other_വരി_distance)
	തിരികെ FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (വരി_distance < other_വരി_distance)
	തിരികെ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    എങ്കിൽ (!target_col)
	// Without a target column, we can't know which is closer.
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ column_distance = entry->qf_col ? abs(entry->qf_col - target_col) : INT_MAX;
    പൂർണ്ണസംഖ്യ other_column_distance = other_entry->qf_col ? abs(other_entry->qf_col - target_col): INT_MAX;
    എങ്കിൽ (column_distance > other_column_distance)
	തിരികെ FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (column_distance < other_column_distance)
	തിരികെ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    തിരികെ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_add_entries(
	qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		qf_idx,
	list_T		*list,
	char_u		*title,
	പൂർണ്ണസംഖ്യ		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retval = OK;
    പൂർണ്ണസംഖ്യ		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    പൂർണ്ണസംഖ്യ prev_fnum = 0;
    പൂർണ്ണസംഖ്യ prev_lnum = 0;
    പൂർണ്ണസംഖ്യ prev_col = 0;
    എങ്കിൽ (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    പൂർണ്ണസംഖ്യ select_first_entry = FALSE;
    പൂർണ്ണസംഖ്യ select_nearest_entry = FALSE;

    എങ്കിൽ (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (action == 'a')
    {
	എങ്കിൽ (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	അല്ലെങ്കിൽ
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_ഇനങ്ങൾ(qfl);
	qf_store_title(qfl, title);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_ഇനങ്ങൾ(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = ശൂന്യ;
    പൂർണ്ണസംഖ്യ entry_to_select_index = 0;

    FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
    {
	എങ്കിൽ (li->li_tv.v_type != VAR_DICT)
	    തുടരുക; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	എങ്കിൽ (d == ശൂന്യ)
	    തുടരുക;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	എങ്കിൽ (retval == QF_FAIL)
	    നിർത്തുക;

	qfline_T *entry = qfl->qf_last;
	എങ്കിൽ (
	    (select_first_entry && entry_to_select == ശൂന്യ) ||
	    (select_nearest_entry &&
		(entry_to_select == ശൂന്യ ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_എണ്ണം;
	}
    }

    // Check if any valid error entries are added to the list.
    എങ്കിൽ (valid_entry)
	qfl->qf_nonevalid = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    എങ്കിൽ (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    തിരികെ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	പൂർണ്ണസംഖ്യ		action,
	പൂർണ്ണസംഖ്യ		*newlist)
{
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		qf_idx = qi->qf_curlist;    // default is the current list

    എങ്കിൽ ((di = dict_find(what, (char_u *)"nr", -1)) != ശൂന്യ)
    {
	// Use the specified quickfix/location list
	എങ്കിൽ (di->di_tv.v_type == VAR_സംഖ്യ)
	{
	    // for zero use the current list
	    എങ്കിൽ (di->di_tv.vval.v_സംഖ്യ != 0)
		qf_idx = di->di_tv.vval.v_സംഖ്യ - 1;

	    എങ്കിൽ ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_അട്ടി_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		തിരികെ INVALID_QFIDX;
	    അല്ലെങ്കിൽ എങ്കിൽ (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	അല്ലെങ്കിൽ എങ്കിൽ (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != ശൂന്യ
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    എങ്കിൽ (!qf_അട്ടി_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (*newlist)
		qf_idx = 0;
	    അല്ലെങ്കിൽ
		തിരികെ INVALID_QFIDX;
	}
	അല്ലെങ്കിൽ
	    തിരികെ INVALID_QFIDX;
    }

    എങ്കിൽ (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != ശൂന്യ)
    {
	// Use the quickfix/location list with the specified id
	എങ്കിൽ (di->di_tv.v_type != VAR_സംഖ്യ)
	    തിരികെ INVALID_QFIDX;

	തിരികെ qf_id2nr(qi, di->di_tv.vval.v_സംഖ്യ);
    }

    തിരികെ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_title(qf_info_T *qi, പൂർണ്ണസംഖ്യ qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    എങ്കിൽ (di->di_tv.v_type != VAR_STRING)
	തിരികെ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    എങ്കിൽ (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    തിരികെ OK;
}

/*
 * Set quickfix list items/entries.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_ഇനങ്ങൾ(qf_info_T *qi, പൂർണ്ണസംഖ്യ qf_idx, dictitem_T *di, പൂർണ്ണസംഖ്യ action)
{
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    char_u	*title_save;

    എങ്കിൽ (di->di_tv.v_type != VAR_LIST)
	തിരികെ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    തിരികെ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_ഇനങ്ങൾ_from_വരികൾ(
	qf_info_T	*qi,
	പൂർണ്ണസംഖ്യ		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	പൂർണ്ണസംഖ്യ		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    പൂർണ്ണസംഖ്യ		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    എങ്കിൽ ((efm_di = dict_find(what, (char_u *)"efm", -1)) != ശൂന്യ)
    {
	എങ്കിൽ (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == ശൂന്യ)
	    തിരികെ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    എങ്കിൽ (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (action == 'r' || action == 'u')
	qf_free_ഇനങ്ങൾ(&qi->qf_lists[qf_idx]);
    എങ്കിൽ (qf_init_ext(qi, qf_idx, ശൂന്യ, ശൂന്യ, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, ശൂന്യ, ശൂന്യ) >= 0)
	retval = OK;

    തിരികെ retval;
}

/*
 * Set quickfix list context.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    എങ്കിൽ (ctx != ശൂന്യ)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    തിരികെ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    പൂർണ്ണസംഖ്യ		denote = FALSE;
    പൂർണ്ണസംഖ്യ		newidx;
    പൂർണ്ണസംഖ്യ		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    എങ്കിൽ (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != ശൂന്യ
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_എണ്ണം;
    അല്ലെങ്കിൽ
    {
	// Otherwise use the specified index
	newidx = tv_get_സംഖ്യ_chk(&di->di_tv, &denote);
	എങ്കിൽ (denote)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (newidx < 1)		// sanity check
	തിരികെ FAIL;
    എങ്കിൽ (newidx > qfl->qf_എണ്ണം)
	newidx = qfl->qf_എണ്ണം;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    എങ്കിൽ (qf_ptr == ശൂന്യ)
	തിരികെ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    എങ്കിൽ (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    തിരികെ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    എങ്കിൽ (cb.cb_name == ശൂന്യ || *cb.cb_name == NUL)
	തിരികെ OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    തിരികെ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
qf_set_properties(qf_info_T *qi, dict_T *what, പൂർണ്ണസംഖ്യ action, char_u *title)
{
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    പൂർണ്ണസംഖ്യ		qf_idx;
    പൂർണ്ണസംഖ്യ		newlist = FALSE;
    qf_list_T	*qfl;

    എങ്കിൽ (action == ' ' || qf_അട്ടി_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    എങ്കിൽ (qf_idx == INVALID_QFIDX)	// List not found
	തിരികെ FAIL;

    എങ്കിൽ (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"title", -1)) != ശൂന്യ)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"items", -1)) != ശൂന്യ)
	retval = qf_setprop_ഇനങ്ങൾ(qi, qf_idx, di, action);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"lines", -1)) != ശൂന്യ)
	retval = qf_setprop_ഇനങ്ങൾ_from_വരികൾ(qi, qf_idx, what, di, action);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"context", -1)) != ശൂന്യ)
	retval = qf_setprop_context(qfl, di);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"idx", -1)) != ശൂന്യ)
	retval = qf_setprop_curidx(qi, qfl, di);
    എങ്കിൽ ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != ശൂന്യ)
	retval = qf_setprop_qftf(qi, qfl, di);

    എങ്കിൽ (newlist || retval == OK)
	qf_list_changed(qfl);
    എങ്കിൽ (newlist)
	qf_update_buffer(qi, ശൂന്യ);

    തിരികെ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    സ്ഥിതം ശൂന്യം
qf_free_അട്ടി(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = ശൂന്യ;

    എങ്കിൽ (qfwin != ശൂന്യ)
    {
	// If the quickfix/location list window is open, then clear it
	എങ്കിൽ (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, ശൂന്യ);
    }

    എങ്കിൽ (wp != ശൂന്യ && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	എങ്കിൽ (llwin != ശൂന്യ)
	    wp = llwin;
    }

    qf_free_all(wp);
    എങ്കിൽ (wp == ശൂന്യ)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qfwin != ശൂന്യ)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_അട്ടി(QFLT_LOCATION, wp->w_p_lhi);

	എങ്കിൽ (new_ll != ശൂന്യ)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    എങ്കിൽ (wp != qfwin)
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
    പൂർണ്ണസംഖ്യ
set_errorlist(
	win_T	*wp,
	list_T	*list,
	പൂർണ്ണസംഖ്യ	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    പൂർണ്ണസംഖ്യ		retval = OK;

    എങ്കിൽ (wp != ശൂന്യ)
	qi = ll_get_or_alloc_list(wp);
    അല്ലെങ്കിൽ
	qi = ql_info;
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_അട്ടി(wp, qi);
	തിരികെ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    എങ്കിൽ (list->lv_len != 0 && what != ശൂന്യ)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	തിരികെ FAIL;
    }

    incr_quickfix_busy();

    എങ്കിൽ (what != ശൂന്യ)
	retval = qf_set_properties(qi, what, action, title);
    അല്ലെങ്കിൽ
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	എങ്കിൽ (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    തിരികെ retval;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mark_quickfix_user_data(qf_info_T *qi, പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	എങ്കിൽ (!qfl->qf_has_user_data)
	    തുടരുക;
	qfline_T *qfp;
	പൂർണ്ണസംഖ്യ j;
	FOR_ALL_QFL_ഇനങ്ങൾ(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    എങ്കിൽ (user_data != ശൂന്യ && user_data->v_type != VAR_സംഖ്യ
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		abort = abort || set_ref_in_item(user_data, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    }
    തിരികെ abort;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mark_quickfix_ctx(qf_info_T *qi, പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    വേണ്ടി (i = 0; i < qi->qf_maxcount && !abort; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	എങ്കിൽ (ctx != ശൂന്യ && ctx->v_type != VAR_സംഖ്യ
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    abort = abort || set_ref_in_item(ctx, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	abort = abort || set_ref_in_callback(cb, copyID);
    }

    തിരികെ abort;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_quickfix(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    എങ്കിൽ (ql_info == ശൂന്യ)
	തിരികെ TRUE;

    abort = mark_quickfix_ctx(ql_info, copyID);
    എങ്കിൽ (abort)
	തിരികെ abort;

    abort = mark_quickfix_user_data(ql_info, copyID);
    എങ്കിൽ (abort)
	തിരികെ abort;

    abort = set_ref_in_callback(&qftf_cb, copyID);
    എങ്കിൽ (abort)
	തിരികെ abort;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	എങ്കിൽ (win->w_llist != ശൂന്യ)
	{
	    abort = mark_quickfix_ctx(win->w_llist, copyID);
	    എങ്കിൽ (abort)
		തിരികെ abort;

	    abort = mark_quickfix_user_data(win->w_llist, copyID);
	    എങ്കിൽ (abort)
		തിരികെ abort;
	}
	എങ്കിൽ (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    abort = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    എങ്കിൽ (abort)
		തിരികെ abort;

	    abort = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    എങ്കിൽ (abort)
		തിരികെ abort;
	}
    }

    തിരികെ abort;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    സ്ഥിതം char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_cbuffer:	തിരികെ (char_u *)"cbuffer";
	അവസ്ഥ CMD_cgetbuffer:	തിരികെ (char_u *)"cgetbuffer";
	അവസ്ഥ CMD_caddbuffer:	തിരികെ (char_u *)"caddbuffer";
	അവസ്ഥ CMD_lbuffer:	തിരികെ (char_u *)"lbuffer";
	അവസ്ഥ CMD_lgetbuffer:	തിരികെ (char_u *)"lgetbuffer";
	അവസ്ഥ CMD_laddbuffer:	തിരികെ (char_u *)"laddbuffer";
	സ്ഥിരസ്ഥിതി:		തിരികെ ശൂന്യ;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = ശൂന്യ;

    എങ്കിൽ (*eap->arg == NUL)
	buf = curbuf;
    അല്ലെങ്കിൽ എങ്കിൽ (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((അക്ഷരം *)eap->arg));

    എങ്കിൽ (buf == ശൂന്യ)
    {
	emsg(_(e_invalid_argument));
	തിരികെ FAIL;
    }

    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
    {
	emsg(_(e_buffer_is_not_loaded));
	തിരികെ FAIL;
    }

    എങ്കിൽ (eap->addr_എണ്ണം == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_വരി_എണ്ണം;
    }

    എങ്കിൽ (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_വരി_എണ്ണം
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_വരി_എണ്ണം)
    {
	emsg(_(e_invalid_range));
	തിരികെ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    തിരികെ OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    ശൂന്യം
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = ശൂന്യ;
    qf_info_T	*qi;
    char_u	*au_name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res;
    int_u	save_qfid;
    win_T	*wp = ശൂന്യ;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	എങ്കിൽ (aborting())
	    തിരികെ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_അട്ടി(eap, &wp);
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	തിരികെ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    എങ്കിൽ (buf->b_sfname)
    {
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%s (%s)",
		(അക്ഷരം *)qf_title, (അക്ഷരം *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, ശൂന്യ, buf, ശൂന്യ, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, ശൂന്യ);
    എങ്കിൽ (qf_അട്ടി_empty(qi))
    {
	decr_quickfix_busy();
	തിരികെ;
    }
    എങ്കിൽ (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    എങ്കിൽ (au_name != ശൂന്യ)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	എങ്കിൽ (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    എങ്കിൽ (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_cexpr:	    തിരികെ (char_u *)"cexpr";
	അവസ്ഥ CMD_cgetexpr:  തിരികെ (char_u *)"cgetexpr";
	അവസ്ഥ CMD_caddexpr:  തിരികെ (char_u *)"caddexpr";
	അവസ്ഥ CMD_lexpr:	    തിരികെ (char_u *)"lexpr";
	അവസ്ഥ CMD_lgetexpr:  തിരികെ (char_u *)"lgetexpr";
	അവസ്ഥ CMD_laddexpr:  തിരികെ (char_u *)"laddexpr";
	സ്ഥിരസ്ഥിതി:	    തിരികെ ശൂന്യ;
    }
}

    പൂർണ്ണസംഖ്യ
trigger_cexpr_autocmd(പൂർണ്ണസംഖ്യ cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	എങ്കിൽ (aborting())
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = ശൂന്യ;

    qi = qf_cmd_get_or_alloc_അട്ടി(eap, &wp);
    എങ്കിൽ (qi == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ ((tv->v_type == VAR_STRING && tv->vval.v_string != ശൂന്യ)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != ശൂന്യ))
    {
	പൂർണ്ണസംഖ്യ	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, ശൂന്യ, ശൂന്യ, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), ശൂന്യ);
	എങ്കിൽ (qf_അട്ടി_empty(qi))
	{
	    decr_quickfix_busy();
	    തിരികെ FAIL;
	}
	എങ്കിൽ (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	എങ്കിൽ (au_name != ശൂന്യ)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	എങ്കിൽ (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	തിരികെ OK;
    }

    emsg(_(e_string_or_list_expected));
    തിരികെ FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    ശൂന്യം
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    എങ്കിൽ (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	തിരികെ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_വ്യഞ്ജകം(eap->arg, eap);
    എങ്കിൽ (tv == ശൂന്യ)
	തിരികെ;

    (ശൂന്യം)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    സ്ഥിതം qf_info_T *
hgr_get_ll(പൂർണ്ണസംഖ്യ *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    എങ്കിൽ (bt_help(curwin->w_buffer))
	wp = curwin;
    അല്ലെങ്കിൽ
	// Find an existing help window
	wp = qf_find_help_win();

    എങ്കിൽ (wp == ശൂന്യ)	    // Help window not found
	qi = ശൂന്യ;
    അല്ലെങ്കിൽ
	qi = wp->w_llist;

    എങ്കിൽ (qi == ശൂന്യ)
    {
	// Allocate a new location list for help text matches
	എങ്കിൽ ((qi = qf_alloc_അട്ടി(QFLT_LOCATION, 1)) == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	*new_ll = TRUE;
    }

    തിരികെ qi;
}

/*
 * Search for a pattern in a help file.
 */
    സ്ഥിതം ശൂന്യം
hgr_തിരയൽ_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    FILE	*fd;
    long	lnum;

    fd = mch_fopen((അക്ഷരം *)fname, "r");
    എങ്കിൽ (fd == ശൂന്യ)
	തിരികെ;

    lnum = 1;
    ആകുമ്പോൾ (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *വരി = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	എങ്കിൽ (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    വരി = string_convert(p_vc, IObuff, ശൂന്യ);
	    എങ്കിൽ (വരി == ശൂന്യ)
		വരി = IObuff;
	}

	എങ്കിൽ (vim_regexec(p_regmatch, വരി, (colnr_T)0))
	{
	    പൂർണ്ണസംഖ്യ	l = (പൂർണ്ണസംഖ്യ)STRLEN(വരി);

	    // remove trailing CR, LF, spaces, etc.
	    ആകുമ്പോൾ (l > 0 && വരി[l - 1] <= ' ')
		വരി[--l] = NUL;

	    എങ്കിൽ (qf_add_entry(qfl,
			ശൂന്യ,	// dir
			fname,
			ശൂന്യ,
			0,
			വരി,
			lnum,
			0,
			(പൂർണ്ണസംഖ്യ)(p_regmatch->startp[0] - വരി)
			+ 1,	// col
			(പൂർണ്ണസംഖ്യ)(p_regmatch->endp[0] - വരി)
			+ 1,	// end_col
			FALSE,	// vis_col
			ശൂന്യ,	// search pattern
			0,	// nr
			1,	// type
			ശൂന്യ,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		എങ്കിൽ (വരി != IObuff)
		    vim_free(വരി);
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (വരി != IObuff)
	    vim_free(വരി);
	++lnum;
	വരി_breakcheck();
    }
    fclose(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    സ്ഥിതം ശൂന്യം
hgr_തിരയൽ_files_in_dir(
	qf_list_T *qfl,
	char_u *dirname,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    പൂർണ്ണസംഖ്യ		fcount;
    char_u	**fnames;
    പൂർണ്ണസംഖ്യ		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(dirname);
    STRCAT(dirname, "doc/*.\\(txt\\|??x\\)");
    എങ്കിൽ (gen_expand_wildcards(1, &dirname, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	വേണ്ടി (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    എങ്കിൽ (lang != ശൂന്യ
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		തുടരുക;
# endif

	    hgr_തിരയൽ_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    സ്ഥിതം ശൂന്യം
hgr_തിരയൽ_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    എങ്കിൽ (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    ആകുമ്പോൾ (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_തിരയൽ_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    എങ്കിൽ (vc.vc_type != CONV_NONE)
	convert_setup(&vc, ശൂന്യ, ശൂന്യ);
}

/*
 * ":helpgrep {pattern}"
 */
    ശൂന്യം
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    പൂർണ്ണസംഖ്യ		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    പൂർണ്ണസംഖ്യ		new_qi = FALSE;
    char_u	*au_name =  ശൂന്യ;
    char_u	*lang = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		updated = FALSE;

    തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
    {
	അവസ്ഥ CMD_helpgrep:  au_name = (char_u *)"helpgrep"; നിർത്തുക;
	അവസ്ഥ CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: നിർത്തുക;
    }
    എങ്കിൽ (au_name != ശൂന്യ && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	എങ്കിൽ (aborting())
	    തിരികെ;
# endif
    }

    എങ്കിൽ (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	എങ്കിൽ (qi == ശൂന്യ)
	    തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (qi == ശൂന്യ)
    {
	emsg(_(e_no_quickfix_അട്ടി));
	തിരികെ;
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
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_തിരയൽ_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    എങ്കിൽ (p_cpo == empty_option)
	p_cpo = save_cpo;
    അല്ലെങ്കിൽ
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	എങ്കിൽ (*p_cpo == NUL)
	    set_option_മൂല്യം_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	എങ്കിൽ (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    എങ്കിൽ (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, ശൂന്യ);

    എങ്കിൽ (au_name != ശൂന്യ)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	എങ്കിൽ (!new_qi && IS_LL_അട്ടി(qi) && qf_find_win_with_loclist(qi) == ശൂന്യ)
	{
	    decr_quickfix_busy();
	    തിരികെ;
	}
    }

    // Jump to first match.
    എങ്കിൽ (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    അല്ലെങ്കിൽ
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    എങ്കിൽ (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	എങ്കിൽ (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    എങ്കിൽ (new_qi)
		ll_free_all(&qi);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_llist == ശൂന്യ && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_quickfix(ശൂന്യം)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(ശൂന്യ);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#എങ്കിൽ defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    സ്ഥിതം ശൂന്യം
get_qf_loc_list(പൂർണ്ണസംഖ്യ is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    എങ്കിൽ (what_arg->v_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	    എങ്കിൽ (is_qf || wp != ശൂന്യ)
		(ശൂന്യം)get_errorlist(ശൂന്യ, wp, -1, 0, rettv->vval.v_list);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (rettv_dict_alloc(rettv) == OK)
	    എങ്കിൽ (is_qf || (wp != ശൂന്യ))
	    {
		എങ്കിൽ (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    എങ്കിൽ (d != ശൂന്യ)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		അല്ലെങ്കിൽ
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    ശൂന്യം
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    ശൂന്യം
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    എങ്കിൽ (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    get_qf_loc_list(TRUE, ശൂന്യ, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    സ്ഥിതം ശൂന്യം
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    പൂർണ്ണസംഖ്യ		action = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;
# endif

    rettv->vval.v_സംഖ്യ = -1;

# ifdef FEAT_QUICKFIX
    എങ്കിൽ (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    അല്ലെങ്കിൽ എങ്കിൽ (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    അല്ലെങ്കിൽ
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	valid_dict = TRUE;

	എങ്കിൽ (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    എങ്കിൽ (act == ശൂന്യ)
		തിരികെ;		// type error; errmsg already given
	    എങ്കിൽ ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_action_str_1), act);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	അല്ലെങ്കിൽ
	    emsg(_(e_string_required));

	എങ്കിൽ (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    എങ്കിൽ (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != ശൂന്യ)
		what = what_arg->vval.v_dict;
	    അല്ലെങ്കിൽ
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	എങ്കിൽ (l != ശൂന്യ && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == ശൂന്യ ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_സംഖ്യ = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    ശൂന്യം
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	തിരികെ;

    win = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (win != ശൂന്യ)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    ശൂന്യം
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	തിരികെ;

    set_qf_ll_list(ശൂന്യ, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
