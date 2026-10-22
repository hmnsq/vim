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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_QUICKFIX)

संरचना dir_थाक_T
{
    संरचना dir_थाक_T	*next;
    char_u		*स_नाम;
};

/*
 * For each error the next struct is allocated and linked in a list.
 */
प्रकारनिर्देश संरचना qfline_S qfline_T;
संरचना qfline_S
{
    qfline_T	*qf_next;	// pointer to next error in the list
    qfline_T	*qf_prev;	// pointer to previous error in the list
    linenr_T	qf_lnum;	// line number where the error occurred
    linenr_T	qf_end_lnum;	// line number when the error has range or zero
    पूर्णाङ्क		qf_fnum;	// file number for the line
    पूर्णाङ्क		qf_col;		// column where the error occurred
    पूर्णाङ्क		qf_end_col;	// column when the error has range or zero
    पूर्णाङ्क		qf_nr;		// error number
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
प्रकारनिर्देश गणना
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
प्रकारनिर्देश संरचना qf_list_S
{
    int_u	qf_id;		// Unique identifier for this list
    qfltype_T	qfl_type;
    qfline_T	*qf_start;	// pointer to the first error
    qfline_T	*qf_last;	// pointer to the last error
    qfline_T	*qf_ptr;	// pointer to the current error
    पूर्णाङ्क		qf_गन्ती;	// number of errors (0 means empty list)
    पूर्णाङ्क		qf_index;	// current index in the error list
    पूर्णाङ्क		qf_nonevalid;	// TRUE if not a single valid entry found
    पूर्णाङ्क		qf_has_user_data; // TRUE if at least one item has user_data attached
    char_u	*qf_title;	// title derived from the command that created
				// the error list or set by setqflist
    typval_T	*qf_ctx;	// context set by setqflist/setloclist
    callback_T  qf_qftf_cb;	// 'quickfixtextfunc' callback function

    संरचना dir_थाक_T	*qf_dir_थाक;
    char_u		*qf_directory;
    संरचना dir_थाक_T	*qf_file_थाक;
    char_u		*qf_currfile;
    पूर्णाङ्क			qf_multiline;
    पूर्णाङ्क			qf_multiignore;
    पूर्णाङ्क			qf_multiscan;
    दीर्घ		qf_changedtick;
} qf_list_T;

/*
 * Quickfix/Location list stack definition
 * Contains a list of quickfix/location lists (qf_list_T)
 */
संरचना qf_info_S
{
    // Count of references to this list. Used only for location lists.
    // When a location list window reference this list, qf_refcount
    // will be 2. Otherwise, qf_refcount will be 1. When qf_refcount
    // reaches 0, the list is freed.
    पूर्णाङ्क		qf_refcount;
    पूर्णाङ्क		qf_listcount;	    // current number of lists
    पूर्णाङ्क		qf_curlist;	    // current error list
    पूर्णाङ्क         qf_maxcount;        // maximum number of lists
    qf_list_T	*qf_lists;
    qfltype_T	qfl_type;	    // type of list
    पूर्णाङ्क		qf_bufnr;	    // quickfix window buffer number
};

स्थिर qf_info_T ql_info_actual; // global quickfix list
स्थिर qf_info_T *ql_info;	// points to ql_info_actual if memory allocation is successful.
स्थिर int_u last_qf_id = 0;	// Last used quickfix list id

# define FMT_PATTERNS 14		// maximum number of % recognized

/*
 * Structure used to hold the info of one part of 'errorformat'
 */
प्रकारनिर्देश संरचना efm_S efm_T;
संरचना efm_S
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
    पूर्णाङ्क		    conthere;	// %> used
};

// List of location lists to be deleted.
// Used to delay the deletion of locations lists by autocmds.
प्रकारनिर्देश संरचना qf_delq_S
{
    संरचना qf_delq_S	*next;
    qf_info_T		*qi;
} qf_delq_T;
स्थिर qf_delq_T *qf_delq_head = रिक्त;

// Counter to prevent autocmds from freeing up location lists when they are
// still being used.
स्थिर पूर्णाङ्क	quickfix_busy = 0;

स्थिर efm_T	*fmt_start = रिक्त; // cached across qf_parse_line() calls

// callback function for 'quickfixtextfunc'
स्थिर callback_T qftf_cb;

स्थिर शून्य     qf_झिक_थाक(qf_info_T *qi, पूर्णाङ्क adjust);
स्थिर शून्य	qf_new_list(qf_info_T *qi, char_u *qf_title);
स्थिर पूर्णाङ्क	qf_add_entry(qf_list_T *qfl, char_u *dir, char_u *fname, char_u *module, पूर्णाङ्क bufnum, char_u *mesg, दीर्घ lnum, दीर्घ end_lnum, पूर्णाङ्क col, पूर्णाङ्क end_col, पूर्णाङ्क vis_col, char_u *pattern, पूर्णाङ्क nr, पूर्णाङ्क type, typval_T *user_data, पूर्णाङ्क valid);
स्थिर पूर्णाङ्क      qf_resize_थाक_base(qf_info_T *qi, पूर्णाङ्क n);
स्थिर शून्य     qf_sync_llw_to_win(win_T *llw);
स्थिर शून्य     qf_sync_win_to_llw(win_T *pwp);
स्थिर qf_info_T *qf_alloc_थाक(qfltype_T qfltype, पूर्णाङ्क n);
स्थिर qf_list_T *qf_alloc_list_थाक(पूर्णाङ्क n);
स्थिर शून्य	qf_free(qf_list_T *qfl);
स्थिर char_u	*qf_types(पूर्णाङ्क, पूर्णाङ्क);
स्थिर पूर्णाङ्क	qf_get_fnum(qf_list_T *qfl, char_u *, char_u *);
स्थिर char_u	*qf_थप_dir(char_u *, संरचना dir_थाक_T **, पूर्णाङ्क is_file_थाक);
स्थिर char_u	*qf_झिक_dir(संरचना dir_थाक_T **);
स्थिर char_u	*qf_guess_filepath(qf_list_T *qfl, char_u *);
स्थिर win_T    *qf_find_win_with_loclist(qf_info_T *ll);
स्थिर शून्य	qf_jump_newwin(qf_info_T *qi, पूर्णाङ्क dir, पूर्णाङ्क errornr, पूर्णाङ्क forceit, पूर्णाङ्क newwin);
स्थिर शून्य	qf_fmt_text(garray_T *gap, char_u *text);
स्थिर शून्य	qf_range_text(garray_T *gap, qfline_T *qfp);
स्थिर पूर्णाङ्क	qf_win_pos_update(qf_info_T *qi, पूर्णाङ्क old_qf_index);
स्थिर win_T	*qf_find_win(qf_info_T *qi);
स्थिर buf_T	*qf_find_buf(qf_info_T *qi);
स्थिर शून्य	qf_update_buffer(qf_info_T *qi, qfline_T *old_last);
स्थिर शून्य	qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, पूर्णाङ्क qf_winid);
स्थिर buf_T	*load_dummy_buffer(char_u *fname, char_u *dirname_start, char_u *resulting_dir);
स्थिर शून्य	wipe_dummy_buffer(buf_T *buf, char_u *dirname_start);
स्थिर शून्य	unload_dummy_buffer(buf_T *buf, char_u *dirname_start);
स्थिर qf_info_T *ll_get_or_alloc_list(win_T *);
स्थिर पूर्णाङ्क	entry_is_closer_to_target(qfline_T *entry, qfline_T *other_entry, पूर्णाङ्क target_fnum, पूर्णाङ्क target_lnum, पूर्णाङ्क target_col);

// Quickfix window check helper macro
# define IS_QF_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref == रिक्त)
// Location list window check helper macro
# define IS_LL_WINDOW(wp) (bt_quickfix((wp)->w_buffer) && (wp)->w_llist_ref != रिक्त)

// Quickfix and location list stack check helper macros
# define IS_QF_थाक(qi)		((qi)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_थाक(qi)		((qi)->qfl_type == QFLT_LOCATION)
# define IS_QF_LIST(qfl)		((qfl)->qfl_type == QFLT_QUICKFIX)
# define IS_LL_LIST(qfl)		((qfl)->qfl_type == QFLT_LOCATION)

/*
 * Return location list for window 'wp'
 * For location list window, return the referenced location list
 */
# define GET_LOC_LIST(wp) (IS_LL_WINDOW(wp) ? (wp)->w_llist_ref : (wp)->w_llist)

// Macro to loop through all the items in a quickfix list
// Quickfix item index starts from 1, so i below starts at 1
# define FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i) \
		    लागि ((i) = 1, (qfp) = (qfl)->qf_start; \
			    !got_int && (i) <= (qfl)->qf_गन्ती && (qfp) != रिक्त; \
			    ++(i), (qfp) = (qfp)->qf_next)

/*
 * Looking up a buffer can be slow if there are many.  Remember the last one
 * to make this a lot faster if there are multiple matches in the same file.
 */
स्थिर char_u   *qf_last_bufname = रिक्त;
स्थिर bufref_T  qf_last_bufref = {रिक्त, 0, 0};

स्थिर garray_T qfga;

/*
 * Get a growarray to buffer text in.  Shared between various commands to avoid
 * many alloc/free calls.
 */
    स्थिर garray_T *
qfga_get(शून्य)
{
    स्थिर पूर्णाङ्क initialized = FALSE;

    यदि (!initialized)
    {
	initialized = TRUE;
	ga_init2(&qfga, 1, 256);
    }

    // Reset the length to zero.  Retain ga_data from previous use to avoid
    // many alloc/free calls.
    qfga.ga_len = 0;

    फर्काऊ &qfga;
}

/*
 * The "qfga" grow array buffer is reused across multiple quickfix commands as
 * a temporary buffer to reduce the number of alloc/free calls.  But if the
 * buffer size is large, then to avoid holding on to that memory, clear the
 * grow array.  Otherwise just reset the grow array length.
 */
    स्थिर शून्य
qfga_clear(शून्य)
{
    यदि (qfga.ga_maxlen > 1000)
	ga_clear(&qfga);
    अन्यथा
	qfga.ga_len = 0;
}

/*
 * Maximum number of bytes allowed per line while reading a errorfile.
 */
# define पङ्क्ति_MAXLEN 4096

/*
 * Patterns used.  Keep in sync with qf_parse_fmt[].
 */
स्थिर संरचना fmtpattern
{
    char_u	convchar;
    वर्ण	*pattern;
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
    स्थिर char_u *
efmpat_to_regpat(
	char_u	*efmpat,
	char_u	*regpat,
	efm_T	*efminfo,
	पूर्णाङ्क	idx,
	पूर्णाङ्क	round)
{
    char_u	*srcptr;

    यदि (efminfo->addr[idx])
    {
	// Each errorformat pattern can occur only once
	semsg(_(e_too_many_chr_in_format_string), *efmpat);
	फर्काऊ रिक्त;
    }
    यदि ((idx && idx < FMT_PATTERN_R
		&& vim_strchr((char_u *)"DXOPQ", efminfo->prefix) != रिक्त)
	    || (idx == FMT_PATTERN_R
		&& vim_strchr((char_u *)"OPQ", efminfo->prefix) == रिक्त))
    {
	semsg(_(e_unexpected_chr_in_format_str), *efmpat);
	फर्काऊ रिक्त;
    }
    efminfo->addr[idx] = (char_u)++round;
    *regpat++ = '\\';
    *regpat++ = '(';
# ifdef BACKSLASH_IN_FILENAME
    यदि (*efmpat == 'f')
    {
	// Also match "c:" in the file name, even when
	// checking for a colon next: "%f:".
	// "\%(\a:\)\="
	STRCPY(regpat, "\\%(\\a:\\)\\=");
	regpat += 10;
    }
# endif
    यदि (*efmpat == 'f' && efmpat[1] != NUL)
    {
	यदि (efmpat[1] != '\\' && efmpat[1] != '%')
	{
	    // A file name may contain spaces, but this isn't
	    // in "\f".  For "%f:%l:%m" there may be a ":" in
	    // the file name.  Use ".\{-1,}x" instead (x is
	    // the next character), the requirement that :999:
	    // follows should work.
	    STRCPY(regpat, ".\\{-1,}");
	    regpat += 7;
	}
	अन्यथा
	{
	    // File name followed by '\\' or '%': include as
	    // many file name chars as possible.
	    STRCPY(regpat, "\\f\\+");
	    regpat += 4;
	}
    }
    अन्यथा
    {
	srcptr = (char_u *)fmt_pat[idx].pattern;
	जबसम्म ((*regpat = *srcptr++) != NUL)
	    ++regpat;
    }
    *regpat++ = '\\';
    *regpat++ = ')';

    फर्काऊ regpat;
}

/*
 * Convert a scanf like format in 'errorformat' to a regular expression.
 * Returns a pointer to the location after the pattern.
 */
    स्थिर char_u *
scanf_fmt_to_regpat(
	char_u	**pefmp,
	char_u	*efm,
	पूर्णाङ्क	len,
	char_u	*regpat)
{
    char_u	*efmp = *pefmp;

    यदि (*efmp == '[' || *efmp == '\\')
    {
	यदि ((*regpat++ = *efmp) == '[')	// %*[^a-z0-9] etc.
	{
	    यदि (efmp[1] == '^')
		*regpat++ = *++efmp;
	    यदि (efmp < efm + len)
	    {
		*regpat++ = *++efmp;	    // could be ']'
		जबसम्म (efmp < efm + len
			&& (*regpat++ = *++efmp) != ']')
		    // skip ;
		यदि (efmp == efm + len)
		{
		    emsg(_(e_missing_rsb_in_format_string));
		    फर्काऊ रिक्त;
		}
	    }
	}
	अन्यथा यदि (efmp < efm + len)	// %*\D, %*\s etc.
	    *regpat++ = *++efmp;
	*regpat++ = '\\';
	*regpat++ = '+';
    }
    अन्यथा
    {
	// TODO: scanf()-like: %*ud, %*3c, %*f, ... ?
	semsg(_(e_unsupported_chr_in_format_string), *efmp);
	फर्काऊ रिक्त;
    }

    *pefmp = efmp;

    फर्काऊ regpat;
}

/*
 * Analyze/parse an errorformat prefix.
 */
    स्थिर char_u *
efm_analyze_prefix(char_u *efmp, efm_T *efminfo)
{
    यदि (vim_strchr((char_u *)"+-", *efmp) != रिक्त)
	efminfo->flags = *efmp++;
    यदि (vim_strchr((char_u *)"DXAEWINCZGOPQ", *efmp) != रिक्त)
	efminfo->prefix = *efmp;
    अन्यथा
    {
	semsg(_(e_invalid_chr_in_format_string_prefix), *efmp);
	फर्काऊ रिक्त;
    }

    फर्काऊ efmp;
}

/*
 * Converts a 'errorformat' string part in 'efm' to a regular expression
 * pattern.  The resulting regex pattern is returned in "regpat". Additional
 * information about the 'erroformat' pattern is returned in "fmt_ptr".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
efm_to_regpat(
	char_u	*efm,
	पूर्णाङ्क	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    पूर्णाङ्क		round;
    पूर्णाङ्क		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    लागि (efmp = efm; efmp < efm + len; ++efmp)
    {
	यदि (*efmp == '%')
	{
	    ++efmp;
	    लागि (idx = 0; idx < FMT_PATTERNS; ++idx)
		यदि (fmt_pat[idx].convchar == *efmp)
		    रोक;
	    यदि (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		यदि (ptr == रिक्त)
		    फर्काऊ FAIL;
		round++;
	    }
	    अन्यथा यदि (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		यदि (ptr == रिक्त)
		    फर्काऊ FAIL;
	    }
	    अन्यथा यदि (vim_strchr((char_u *)"%\\.^$~[", *efmp) != रिक्त)
		*ptr++ = *efmp;		// regexp magic characters
	    अन्यथा यदि (*efmp == '#')
		*ptr++ = '*';
	    अन्यथा यदि (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    अन्यथा यदि (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		यदि (efmp == रिक्त)
		    फर्काऊ FAIL;
	    }
	    अन्यथा
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		फर्काऊ FAIL;
	    }
	}
	अन्यथा			// copy normal character
	{
	    यदि (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    अन्यथा यदि (vim_strchr((char_u *)".*^$~[", *efmp) != रिक्त)
		*ptr++ = '\\';	// escape regexp atoms
	    यदि (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    फर्काऊ OK;
}

/*
 * Free the 'errorformat' information list
 */
    स्थिर शून्य
free_efm_list(efm_T **efm_first)
{
    efm_T *efm_ptr;

    लागि (efm_ptr = *efm_first; efm_ptr != रिक्त; efm_ptr = *efm_first)
    {
	*efm_first = efm_ptr->next;
	vim_regfree(efm_ptr->prog);
	vim_free(efm_ptr);
    }
    fmt_start = रिक्त;
}

/*
 * Compute the size of the buffer used to convert a 'errorformat' pattern into
 * a regular expression pattern.
 */
    स्थिर पूर्णाङ्क
efm_regpat_bufsz(char_u *efm)
{
    पूर्णाङ्क sz;
    पूर्णाङ्क i;

    sz = (FMT_PATTERNS * 3) + ((पूर्णाङ्क)STRLEN(efm) << 2);
    लागि (i = FMT_PATTERNS; i > 0; )
	sz += (पूर्णाङ्क)STRLEN(fmt_pat[--i].pattern);
# ifdef BACKSLASH_IN_FILENAME
    sz += 12; // "%f" can become twelve chars longer (see efm_to_regpat)
# अन्यथा
    sz += 2; // "%f" can become two chars longer
# endif

    फर्काऊ sz;
}

/*
 * Return the length of a 'errorformat' option part (separated by ",").
 */
    स्थिर पूर्णाङ्क
efm_option_part_len(char_u *efm)
{
    पूर्णाङ्क len;

    लागि (len = 0; efm[len] != NUL && efm[len] != ','; ++len)
	यदि (efm[len] == '\\' && efm[len + 1] != NUL)
	    ++len;

    फर्काऊ len;
}

/*
 * Parse the 'errorformat' option. Multiple parts in the 'errorformat' option
 * are parsed and converted to regular expressions. Returns information about
 * the parsed 'errorformat' option.
 */
    स्थिर efm_T *
parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = रिक्त;
    efm_T	*fmt_first = रिक्त;
    efm_T	*fmt_last = रिक्त;
    char_u	*fmtstr = रिक्त;
    पूर्णाङ्क		len;
    पूर्णाङ्क		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    यदि ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == रिक्त)
	गच्छ parse_efm_error;

    जबसम्म (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	यदि (fmt_ptr == रिक्त)
	    गच्छ parse_efm_error;
	यदि (fmt_first == रिक्त)	    // first one
	    fmt_first = fmt_ptr;
	अन्यथा
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	यदि (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    गच्छ parse_efm_error;
	यदि ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == रिक्त)
	    गच्छ parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    यदि (fmt_first == रिक्त)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    गच्छ parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    फर्काऊ fmt_first;
}

गणना {
    QF_FAIL = 0,
    QF_OK = 1,
    QF_END_OF_आगत = 2,
    QF_NOMEM = 3,
    QF_IGNORE_पङ्क्ति = 4,
    QF_MULTISCAN = 5,
    QF_ABORT = 6
};

/*
 * State information used to parse lines and add entries to a quickfix/location
 * list.
 */
प्रकारनिर्देश संरचना {
    char_u	*linebuf;
    पूर्णाङ्क		linelen;
    char_u	*growbuf;
    पूर्णाङ्क		growbufsiz;
    खाता	*fd;
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
    स्थिर char_u *
qf_grow_linebuf(qfstate_T *state, पूर्णाङ्क newsz)
{
    char_u	*p;

    // If the line exceeds LINE_MAXLEN exclude the last
    // byte since it's not a NL character.
    state->linelen = newsz > पङ्क्ति_MAXLEN ? पङ्क्ति_MAXLEN - 1 : newsz;
    यदि (state->growbuf == रिक्त)
    {
	state->growbuf = alloc_id(state->linelen + 1, aid_qf_linebuf);
	यदि (state->growbuf == रिक्त)
	    फर्काऊ रिक्त;
	state->growbufsiz = state->linelen;
    }
    अन्यथा यदि (state->linelen > state->growbufsiz)
    {
	यदि ((p = vim_realloc(state->growbuf, state->linelen + 1)) == रिक्त)
	    फर्काऊ रिक्त;
	state->growbuf = p;
	state->growbufsiz = state->linelen;
    }
    फर्काऊ state->growbuf;
}

/*
 * Get the next string (separated by newline) from state->p_str.
 */
    स्थिर पूर्णाङ्क
qf_get_next_str_पङ्क्ति(qfstate_T *state)
{
    // Get the next line from the supplied string
    char_u	*p_str = state->p_str;
    char_u	*p;
    पूर्णाङ्क		len;

    यदि (*p_str == NUL) // Reached the end of the string
	फर्काऊ QF_END_OF_आगत;

    p = vim_strchr(p_str, '\n');
    यदि (p != रिक्त)
	len = (पूर्णाङ्क)(p - p_str) + 1;
    अन्यथा
	len = (पूर्णाङ्क)STRLEN(p_str);

    यदि (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	यदि (state->linebuf == रिक्त)
	    फर्काऊ QF_NOMEM;
    }
    अन्यथा
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_str, state->linelen);

    // Increment using len in order to discard the rest of the
    // line if it exceeds LINE_MAXLEN.
    p_str += len;
    state->p_str = p_str;

    फर्काऊ QF_OK;
}

/*
 * Get the next string from the List item state->p_li.
 */
    स्थिर पूर्णाङ्क
qf_get_next_list_पङ्क्ति(qfstate_T *state)
{
    listitem_T	*p_li = state->p_li;
    पूर्णाङ्क		len;

    जबसम्म (p_li != रिक्त
	    && (p_li->li_tv.v_type != VAR_STRING
		|| p_li->li_tv.vval.v_string == रिक्त))
	p_li = p_li->li_next;	// Skip non-string items

    यदि (p_li == रिक्त)		// End of the list
    {
	state->p_li = रिक्त;
	फर्काऊ QF_END_OF_आगत;
    }

    len = (पूर्णाङ्क)STRLEN(p_li->li_tv.vval.v_string);
    यदि (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	यदि (state->linebuf == रिक्त)
	    फर्काऊ QF_NOMEM;
    }
    अन्यथा
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }

    vim_strncpy(state->linebuf, p_li->li_tv.vval.v_string, state->linelen);

    state->p_li = p_li->li_next;	// next item
    फर्काऊ QF_OK;
}

/*
 * Get the next string from state->buf.
 */
    स्थिर पूर्णाङ्क
qf_get_next_buf_पङ्क्ति(qfstate_T *state)
{
    char_u	*p_buf = रिक्त;
    पूर्णाङ्क		len;

    // Get the next line from the supplied buffer
    यदि (state->buflnum > state->lnumlast)
	फर्काऊ QF_END_OF_आगत;

    p_buf = ml_get_buf(state->buf, state->buflnum, FALSE);
    len = ml_get_buf_len(state->buf, state->buflnum);
    state->buflnum += 1;

    यदि (len > IOSIZE - 2)
    {
	state->linebuf = qf_grow_linebuf(state, len);
	यदि (state->linebuf == रिक्त)
	    फर्काऊ QF_NOMEM;
    }
    अन्यथा
    {
	state->linebuf = IObuff;
	state->linelen = len;
    }
    vim_strncpy(state->linebuf, p_buf, state->linelen);

    फर्काऊ QF_OK;
}

/*
 * Get the next string from file state->fd.
 */
    स्थिर पूर्णाङ्क
qf_get_next_file_पङ्क्ति(qfstate_T *state)
{
    पूर्णाङ्क	    discard;
    पूर्णाङ्क	    growbuflen;

    यदि (ख_माला_लो((वर्ण *)IObuff, IOSIZE, state->fd) == रिक्त)
	फर्काऊ QF_END_OF_आगत;

    discard = FALSE;
    state->linelen = (पूर्णाङ्क)STRLEN(IObuff);
    यदि (state->linelen == IOSIZE - 1 && !(IObuff[state->linelen - 1] == '\n'))
    {
	// The current line exceeds IObuff, continue reading using
	// growbuf until EOL or LINE_MAXLEN bytes is read.
	यदि (state->growbuf == रिक्त)
	{
	    state->growbufsiz = 2 * (IOSIZE - 1);
	    state->growbuf = alloc_id(state->growbufsiz, aid_qf_linebuf);
	    यदि (state->growbuf == रिक्त)
		फर्काऊ QF_NOMEM;
	}

	// Copy the read part of the line, excluding null-terminator
	स_नकल(state->growbuf, IObuff, IOSIZE - 1);
	growbuflen = state->linelen;
	state->growbuf[growbuflen] = NUL;

	लागि (;;)
	{
	    char_u	*p;

	    यदि (ख_माला_लो((वर्ण *)state->growbuf + growbuflen,
			state->growbufsiz - growbuflen, state->fd) == रिक्त)
		रोक;
	    state->linelen = (पूर्णाङ्क)STRLEN(state->growbuf + growbuflen);
	    growbuflen += state->linelen;
	    यदि ((state->growbuf)[growbuflen - 1] == '\n')
		रोक;
	    यदि (state->growbufsiz == पङ्क्ति_MAXLEN)
	    {
		discard = TRUE;
		रोक;
	    }

	    state->growbufsiz = 2 * state->growbufsiz < पङ्क्ति_MAXLEN
		? 2 * state->growbufsiz : पङ्क्ति_MAXLEN;
	    यदि ((p = vim_realloc(state->growbuf, state->growbufsiz)) == रिक्त)
		फर्काऊ QF_NOMEM;
	    state->growbuf = p;
	}

	जबसम्म (discard)
	{
	    // The current line is longer than LINE_MAXLEN, continue
	    // reading but discard everything until EOL or EOF is
	    // reached.
	    यदि (ख_माला_लो((वर्ण *)IObuff, IOSIZE, state->fd) == रिक्त
		    || (पूर्णाङ्क)STRLEN(IObuff) < IOSIZE - 1
		    || IObuff[IOSIZE - 2] == '\n')
		रोक;
	}

	state->linebuf = state->growbuf;
	state->linelen = growbuflen;
    }
    अन्यथा
	state->linebuf = IObuff;

    // Convert a line if it contains a non-ASCII character.
    यदि (state->vc.vc_type != CONV_NONE && has_non_ascii(state->linebuf))
    {
	char_u	*पङ्क्ति;

	पङ्क्ति = string_convert(&state->vc, state->linebuf, &state->linelen);
	यदि (पङ्क्ति != रिक्त)
	{
	    यदि (state->linelen < IOSIZE)
	    {
		STRCPY(state->linebuf, पङ्क्ति);
		vim_free(पङ्क्ति);
	    }
	    अन्यथा
	    {
		vim_free(state->growbuf);
		state->linebuf = state->growbuf = पङ्क्ति;
		state->growbufsiz = state->linelen < पङ्क्ति_MAXLEN
						? state->linelen : पङ्क्ति_MAXLEN;
	    }
	}
    }

    फर्काऊ QF_OK;
}

/*
 * Get the next string from a file/buffer/list/string.
 */
    स्थिर पूर्णाङ्क
qf_get_nextline(qfstate_T *state)
{
    पूर्णाङ्क status = QF_FAIL;

    यदि (state->fd == रिक्त)
    {
	यदि (state->tv != रिक्त)
	{
	    यदि (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_पङ्क्ति(state);
	    अन्यथा यदि (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_पङ्क्ति(state);
	}
	अन्यथा
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_पङ्क्ति(state);
    }
    अन्यथा
	// Get the next line from the supplied file
	status = qf_get_next_file_पङ्क्ति(state);

    यदि (status != QF_OK)
	फर्काऊ status;

    // remove newline/CR from the line
    यदि (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
# ifdef USE_CRNL
	यदि (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
# endif
    }

    remove_bom(state->linebuf);

    फर्काऊ QF_OK;
}

प्रकारनिर्देश संरचना {
    char_u	*namebuf;
    पूर्णाङ्क		bnr;
    char_u	*module;
    char_u	*errmsg;
    पूर्णाङ्क		errmsglen;
    दीर्घ	lnum;
    दीर्घ	end_lnum;
    पूर्णाङ्क		col;
    पूर्णाङ्क		end_col;
    char_u	use_viscol;
    char_u	*pattern;
    पूर्णाङ्क		enr;
    पूर्णाङ्क		type;
    typval_T	*user_data;
    पूर्णाङ्क		valid;
} qffields_T;

/*
 * Parse the match for filename ('%f') pattern in regmatch.
 * Return the matched value in "fields->namebuf".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_f(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields, पूर्णाङ्क prefix)
{
    पूर्णाङ्क c;

    यदि (rmp->startp[midx] == रिक्त || rmp->endp[midx] == रिक्त)
	फर्काऊ QF_FAIL;

    // Expand ~/file and $HOME/file to full path.
    c = *rmp->endp[midx];
    *rmp->endp[midx] = NUL;
    expand_env(rmp->startp[midx], fields->namebuf, CMDBUFFSIZE);
    *rmp->endp[midx] = c;

    // For separate filename patterns (%O, %P and %Q), the specified file
    // should exist.
    यदि (vim_strchr((char_u *)"OPQ", prefix) != रिक्त
	    && mch_getperm(fields->namebuf) == -1)
	फर्काऊ QF_FAIL;

    फर्काऊ QF_OK;
}

/*
 * Parse the match for buffer number ('%b') pattern in regmatch.
 * Return the matched value in "fields->bnr".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_b(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    पूर्णाङ्क bnr = (पूर्णाङ्क)म_से_द((वर्ण *)rmp->startp[midx]);
    यदि (buflist_findnr(bnr) == रिक्त)
	फर्काऊ QF_FAIL;
    fields->bnr = bnr;
    फर्काऊ QF_OK;
}

/*
 * Parse the match for error number ('%n') pattern in regmatch.
 * Return the matched value in "fields->enr".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_n(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->enr = (पूर्णाङ्क)म_से_द((वर्ण *)rmp->startp[midx]);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for line number ('%l') pattern in regmatch.
 * Return the matched value in "fields->lnum".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_l(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->lnum = म_से_द((वर्ण *)rmp->startp[midx]);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for end line number ('%e') pattern in regmatch.
 * Return the matched value in "fields->end_lnum".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_e(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->end_lnum = म_से_द((वर्ण *)rmp->startp[midx]);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for column number ('%c') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_c(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->col = (पूर्णाङ्क)म_से_द((वर्ण *)rmp->startp[midx]);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for end column number ('%k') pattern in regmatch.
 * Return the matched value in "fields->end_col".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_k(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->end_col = (पूर्णाङ्क)म_से_द((वर्ण *)rmp->startp[midx]);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for error type ('%t') pattern in regmatch.
 * Return the matched value in "fields->type".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_t(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->type = *rmp->startp[midx];
    फर्काऊ QF_OK;
}

/*
 * Copy a non-error line into the error string.  Return the matched line in
 * "fields->errmsg".
 */
    स्थिर पूर्णाङ्क
copy_nonerror_पङ्क्ति(char_u *linebuf, पूर्णाङ्क linelen, qffields_T *fields)
{
    char_u	*p;

    यदि (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	यदि ((p = vim_realloc(fields->errmsg, linelen + 1)) == रिक्त)
	    फर्काऊ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    फर्काऊ QF_OK;
}

/*
 * Parse the match for error message ('%m') pattern in regmatch.
 * Return the matched value in "fields->errmsg".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_m(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    char_u	*p;
    पूर्णाङ्क		len;

    यदि (rmp->startp[midx] == रिक्त || rmp->endp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    len = (पूर्णाङ्क)(rmp->endp[midx] - rmp->startp[midx]);
    यदि (len >= fields->errmsglen)
    {
	// len + null terminator
	यदि ((p = vim_realloc(fields->errmsg, len + 1)) == रिक्त)
	    फर्काऊ QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = len + 1;
    }
    vim_strncpy(fields->errmsg, rmp->startp[midx], len);
    फर्काऊ QF_OK;
}

/*
 * Parse the match for rest of a single-line file message ('%r') pattern.
 * Return the matched value in "tail".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_r(regmatch_T *rmp, पूर्णाङ्क midx, char_u **tail)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    *tail = rmp->startp[midx];
    फर्काऊ QF_OK;
}

/*
 * Parse the match for the pointer line ('%p') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_p(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    char_u	*match_ptr;

    यदि (rmp->startp[midx] == रिक्त || rmp->endp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->col = 0;
    लागि (match_ptr = rmp->startp[midx]; match_ptr != rmp->endp[midx];
								++match_ptr)
    {
	++fields->col;
	यदि (*match_ptr == TAB)
	{
	    fields->col += 7;
	    fields->col -= fields->col % 8;
	}
    }
    ++fields->col;
    fields->use_viscol = TRUE;
    फर्काऊ QF_OK;
}

/*
 * Parse the match for the virtual column number ('%v') pattern in regmatch.
 * Return the matched value in "fields->col".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_v(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    यदि (rmp->startp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    fields->col = (पूर्णाङ्क)म_से_द((वर्ण *)rmp->startp[midx]);
    fields->use_viscol = TRUE;
    फर्काऊ QF_OK;
}

/*
 * Parse the match for the search text ('%s') pattern in regmatch.
 * Return the matched value in "fields->pattern".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_s(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    पूर्णाङ्क		len;

    यदि (rmp->startp[midx] == रिक्त || rmp->endp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    len = (पूर्णाङ्क)(rmp->endp[midx] - rmp->startp[midx]);
    यदि (len > CMDBUFFSIZE - 5)
	len = CMDBUFFSIZE - 5;
    STRCPY(fields->pattern, "^\\V");
    STRNCAT(fields->pattern, rmp->startp[midx], len);
    fields->pattern[len + 3] = '\\';
    fields->pattern[len + 4] = '$';
    fields->pattern[len + 5] = NUL;
    फर्काऊ QF_OK;
}

/*
 * Parse the match for the module ('%o') pattern in regmatch.
 * Return the matched value in "fields->module".
 */
    स्थिर पूर्णाङ्क
qf_parse_fmt_o(regmatch_T *rmp, पूर्णाङ्क midx, qffields_T *fields)
{
    पूर्णाङ्क		len;

    यदि (rmp->startp[midx] == रिक्त || rmp->endp[midx] == रिक्त)
	फर्काऊ QF_FAIL;
    len = (पूर्णाङ्क)(rmp->endp[midx] - rmp->startp[midx]);
    यदि (len > CMDBUFFSIZE)
	len = CMDBUFFSIZE;
    STRNCAT(fields->module, rmp->startp[midx], len);
    फर्काऊ QF_OK;
}

/*
 * 'errorformat' format pattern parser functions.
 * The '%f' and '%r' formats are parsed differently from other formats.
 * See qf_parse_match() for details.
 * Keep in sync with fmt_pat[].
 */
स्थिर पूर्णाङ्क (*qf_parse_fmt[FMT_PATTERNS])(regmatch_T *, पूर्णाङ्क, qffields_T *) =
{
    रिक्त, // %f
    qf_parse_fmt_b,
    qf_parse_fmt_n,
    qf_parse_fmt_l,
    qf_parse_fmt_e,
    qf_parse_fmt_c,
    qf_parse_fmt_k,
    qf_parse_fmt_t,
    qf_parse_fmt_m,
    रिक्त, // %r
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
    स्थिर पूर्णाङ्क
qf_parse_match(
	char_u		*linebuf,
	पूर्णाङ्क		linelen,
	efm_T		*fmt_ptr,
	regmatch_T	*regmatch,
	qffields_T	*fields,
	पूर्णाङ्क		qf_multiline,
	पूर्णाङ्क		qf_multiscan,
	char_u		**tail)
{
    पूर्णाङ्क		idx = fmt_ptr->prefix;
    पूर्णाङ्क		i;
    पूर्णाङ्क		midx;
    पूर्णाङ्क		status;

    यदि ((idx == 'C' || idx == 'Z') && !qf_multiline)
	फर्काऊ QF_FAIL;
    यदि (vim_strchr((char_u *)"EWIN", idx) != रिक्त)
	fields->type = idx;
    अन्यथा
	fields->type = 0;

    // Extract error message data from matched line.
    // We check for an actual submatch, because "\[" and "\]" in
    // the 'errorformat' may cause the wrong submatch to be used.
    लागि (i = 0; i < FMT_PATTERNS; i++)
    {
	status = QF_OK;
	midx = (पूर्णाङ्क)fmt_ptr->addr[i];
	यदि (i == 0 && midx > 0)				// %f
	    status = qf_parse_fmt_f(regmatch, midx, fields, idx);
	अन्यथा यदि (i == FMT_PATTERN_M)
	{
	    यदि (fmt_ptr->flags == '+' && !qf_multiscan)	// %+
		status = copy_nonerror_पङ्क्ति(linebuf, linelen, fields);
	    अन्यथा यदि (midx > 0)				// %m
		status = qf_parse_fmt_m(regmatch, midx, fields);
	}
	अन्यथा यदि (i == FMT_PATTERN_R && midx > 0)	// %r
	    status = qf_parse_fmt_r(regmatch, midx, tail);
	अन्यथा यदि (midx > 0)				// others
	    status = (qf_parse_fmt[i])(regmatch, midx, fields);

	यदि (status != QF_OK)
	    फर्काऊ status;
    }

    फर्काऊ QF_OK;
}

/*
 * Parse an error line in 'linebuf' using a single error format string in
 * 'fmt_ptr->prog' and return the matching values in 'fields'.
 * Returns QF_OK if the efm format matches completely and the fields are
 * successfully copied. Otherwise returns QF_FAIL or QF_NOMEM.
 */
    स्थिर पूर्णाङ्क
qf_parse_get_fields(
	char_u		*linebuf,
	पूर्णाङ्क		linelen,
	efm_T		*fmt_ptr,
	qffields_T	*fields,
	पूर्णाङ्क		qf_multiline,
	पूर्णाङ्क		qf_multiscan,
	char_u		**tail)
{
    regmatch_T	regmatch;
    पूर्णाङ्क		status = QF_FAIL;
    पूर्णाङ्क		r;

    यदि (qf_multiscan &&
		vim_strchr((char_u *)"OPQ", fmt_ptr->prefix) == रिक्त)
	फर्काऊ QF_FAIL;

    fields->namebuf[0] = NUL;
    fields->bnr = 0;
    fields->module[0] = NUL;
    fields->pattern[0] = NUL;
    यदि (!qf_multiscan)
	fields->errmsg[0] = NUL;
    fields->lnum = 0;
    fields->end_lnum = 0;
    fields->col = 0;
    fields->end_col = 0;
    fields->use_viscol = FALSE;
    fields->enr = -1;
    fields->type = 0;
    *tail = रिक्त;

    // Always ignore case when looking for a matching error.
    regmatch.rm_ic = TRUE;
    regmatch.regprog = fmt_ptr->prog;
    r = vim_regexec(&regmatch, linebuf, (colnr_T)0);
    fmt_ptr->prog = regmatch.regprog;
    यदि (r)
	status = qf_parse_match(linebuf, linelen, fmt_ptr, &regmatch,
		fields, qf_multiline, qf_multiscan, tail);

    फर्काऊ status;
}

/*
 * Parse directory error format prefixes (%D and %X).
 * Push and pop directories from the directory stack when scanning directory
 * names.
 */
    स्थिर पूर्णाङ्क
qf_parse_dir_pfx(पूर्णाङ्क idx, qffields_T *fields, qf_list_T *qfl)
{
    यदि (idx == 'D')				// enter directory
    {
	यदि (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    फर्काऊ QF_FAIL;
	}
	qfl->qf_directory =
	    qf_थप_dir(fields->namebuf, &qfl->qf_dir_थाक, FALSE);
	यदि (qfl->qf_directory == रिक्त)
	    फर्काऊ QF_FAIL;
    }
    अन्यथा यदि (idx == 'X')			// leave directory
	qfl->qf_directory = qf_झिक_dir(&qfl->qf_dir_थाक);

    फर्काऊ QF_OK;
}

/*
 * Parse global file name error format prefixes (%O, %P and %Q).
 */
    स्थिर पूर्णाङ्क
qf_parse_file_pfx(
	पूर्णाङ्क idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    यदि (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	यदि (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_थप_dir(fields->namebuf, &qfl->qf_file_थाक, TRUE);
	अन्यथा यदि (idx == 'Q')
	    qfl->qf_currfile = qf_झिक_dir(&qfl->qf_file_थाक);
	*fields->namebuf = NUL;
	यदि (tail && *tail)
	{
	    qfl->qf_multiscan = TRUE;
	    फर्काऊ QF_MULTISCAN;
	}
    }

    फर्काऊ QF_OK;
}

/*
 * Parse a non-error line (a line which doesn't match any of the error
 * format in 'efm').
 */
    स्थिर पूर्णाङ्क
qf_parse_पङ्क्ति_nomatch(char_u *linebuf, पूर्णाङ्क linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    फर्काऊ copy_nonerror_पङ्क्ति(linebuf, linelen, fields);
}

/*
 * Parse multi-line error format prefixes (%C and %Z)
 */
    स्थिर पूर्णाङ्क
qf_parse_multiline_pfx(
	पूर्णाङ्क idx,
	qf_list_T *qfl,
	qffields_T *fields)
{
    char_u		*ptr;
    पूर्णाङ्क			len;

    यदि (!qfl->qf_multiignore)
    {
	qfline_T *qfprev = qfl->qf_last;

	यदि (qfprev == रिक्त)
	    फर्काऊ QF_FAIL;
	यदि (*fields->errmsg && !qfl->qf_multiignore)
	{
	    len = (पूर्णाङ्क)STRLEN(qfprev->qf_text);
	    ptr = alloc_id(len + STRLEN(fields->errmsg) + 2,
						aid_qf_multiline_pfx);
	    यदि (ptr == रिक्त)
		फर्काऊ QF_FAIL;
	    STRCPY(ptr, qfprev->qf_text);
	    vim_free(qfprev->qf_text);
	    qfprev->qf_text = ptr;
	    *(ptr += len) = '\n';
	    STRCPY(++ptr, fields->errmsg);
	}
	यदि (qfprev->qf_nr == -1)
	    qfprev->qf_nr = fields->enr;
	यदि (vim_isprintc(fields->type) && !qfprev->qf_type)
	    // only printable chars allowed
	    qfprev->qf_type = fields->type;

	यदि (!qfprev->qf_lnum)
	    qfprev->qf_lnum = fields->lnum;
	यदि (!qfprev->qf_end_lnum)
	    qfprev->qf_end_lnum = fields->end_lnum;
	यदि (!qfprev->qf_col)
	{
	    qfprev->qf_col = fields->col;
	    qfprev->qf_viscol = fields->use_viscol;
	}
	यदि (!qfprev->qf_end_col)
	    qfprev->qf_end_col = fields->end_col;
	यदि (!qfprev->qf_fnum)
	    qfprev->qf_fnum = qf_get_fnum(qfl,
		    qfl->qf_directory,
		    *fields->namebuf || qfl->qf_directory != रिक्त
		    ? fields->namebuf
		    : qfl->qf_currfile != रिक्त && fields->valid
		    ? qfl->qf_currfile : 0);
    }
    यदि (idx == 'Z')
	qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    पङ्क्ति_breakcheck();

    फर्काऊ QF_IGNORE_पङ्क्ति;
}

/*
 * Parse a line and get the quickfix fields.
 * Return the QF_ status.
 */
    स्थिर पूर्णाङ्क
qf_parse_पङ्क्ति(
	qf_list_T	*qfl,
	char_u		*linebuf,
	पूर्णाङ्क		linelen,
	efm_T		*fmt_first,
	qffields_T	*fields)
{
    efm_T		*fmt_ptr;
    पूर्णाङ्क			idx = 0;
    char_u		*tail = रिक्त;
    पूर्णाङ्क			status;

restofline:
    // If there was no %> item start at the first pattern
    यदि (fmt_start == रिक्त)
	fmt_ptr = fmt_first;
    अन्यथा
    {
	// Otherwise start from the last used pattern
	fmt_ptr = fmt_start;
	fmt_start = रिक्त;
    }

    // Try to match each part of 'errorformat' until we find a complete
    // match or no match.
    fields->valid = TRUE;
    लागि ( ; fmt_ptr != रिक्त; fmt_ptr = fmt_ptr->next)
    {
	idx = fmt_ptr->prefix;
	status = qf_parse_get_fields(linebuf, linelen, fmt_ptr, fields,
				qfl->qf_multiline, qfl->qf_multiscan, &tail);
	यदि (status == QF_NOMEM)
	    फर्काऊ status;
	यदि (status == QF_OK)
	    रोक;
    }
    qfl->qf_multiscan = FALSE;

    यदि (fmt_ptr == रिक्त || idx == 'D' || idx == 'X')
    {
	यदि (fmt_ptr != रिक्त)
	{
	    // 'D' and 'X' directory specifiers
	    status = qf_parse_dir_pfx(idx, fields, qfl);
	    यदि (status != QF_OK)
		फर्काऊ status;
	}

	status = qf_parse_पङ्क्ति_nomatch(linebuf, linelen, fields);
	यदि (status != QF_OK)
	    फर्काऊ status;

	यदि (fmt_ptr == रिक्त)
	    qfl->qf_multiline = qfl->qf_multiignore = FALSE;
    }
    अन्यथा यदि (fmt_ptr != रिक्त)
    {
	// honor %> item
	यदि (fmt_ptr->conthere)
	    fmt_start = fmt_ptr;

	यदि (vim_strchr((char_u *)"AEWIN", idx) != रिक्त)
	{
	    qfl->qf_multiline = TRUE;	// start of a multi-line message
	    qfl->qf_multiignore = FALSE;// reset continuation
	}
	अन्यथा यदि (vim_strchr((char_u *)"CZ", idx) != रिक्त)
	{				// continuation of multi-line msg
	    status = qf_parse_multiline_pfx(idx, qfl, fields);
	    यदि (status != QF_OK)
		फर्काऊ status;
	}
	अन्यथा यदि (vim_strchr((char_u *)"OPQ", idx) != रिक्त)
	{				// global file names
	    status = qf_parse_file_pfx(idx, fields, qfl, tail);
	    यदि (status == QF_MULTISCAN)
	    {
		char_u *s = skipwhite(tail);
		पूर्णाङ्क new_linelen = (पूर्णाङ्क)STRLEN(s);
		यदि (new_linelen >= linelen)
		    फर्काऊ QF_IGNORE_पङ्क्ति;
		linebuf = s;
		linelen = new_linelen;
		गच्छ restofline;
	    }
	}
	यदि (fmt_ptr->flags == '-')	// generally exclude this line
	{
	    यदि (qfl->qf_multiline)
		// also exclude continuation lines
		qfl->qf_multiignore = TRUE;
	    फर्काऊ QF_IGNORE_पङ्क्ति;
	}
    }

    फर्काऊ QF_OK;
}

/*
 * Returns TRUE if the specified quickfix/location stack is empty
 */
    स्थिर पूर्णाङ्क
qf_थाक_empty(qf_info_T *qi)
{
    फर्काऊ qi == रिक्त || qi->qf_listcount <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is empty.
 */
    स्थिर पूर्णाङ्क
qf_list_empty(qf_list_T *qfl)
{
    फर्काऊ qfl == रिक्त || qfl->qf_गन्ती <= 0;
}

/*
 * Returns TRUE if the specified quickfix/location list is not empty and
 * has valid entries.
 */
    स्थिर पूर्णाङ्क
qf_list_has_valid_entries(qf_list_T *qfl)
{
    फर्काऊ !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}

/*
 * Return a pointer to a list in the specified quickfix stack
 */
    स्थिर qf_list_T *
qf_get_list(qf_info_T *qi, पूर्णाङ्क idx)
{
    फर्काऊ &qi->qf_lists[idx];
}

/*
 * Allocate the fields used for parsing lines and populating a quickfix list.
 */
    स्थिर पूर्णाङ्क
qf_alloc_fields(qffields_T *pfields)
{
    pfields->namebuf = alloc_id(CMDBUFFSIZE + 1, aid_qf_namebuf);
    pfields->module = alloc_id(CMDBUFFSIZE + 1, aid_qf_module);
    pfields->errmsglen = CMDBUFFSIZE + 1;
    pfields->errmsg = alloc_id(pfields->errmsglen, aid_qf_errmsg);
    pfields->pattern = alloc_id(CMDBUFFSIZE + 1, aid_qf_pattern);
    यदि (pfields->namebuf == रिक्त || pfields->errmsg == रिक्त
		|| pfields->pattern == रिक्त || pfields->module == रिक्त)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * Free the fields used for parsing lines and populating a quickfix list.
 */
    स्थिर शून्य
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
    स्थिर पूर्णाङ्क
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
    यदि (enc != रिक्त && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    यदि (efile != रिक्त && (pstate->fd = mch_fopen((वर्ण *)efile, "r")) == रिक्त)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	फर्काऊ FAIL;
    }

    यदि (tv != रिक्त)
    {
	यदि (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	अन्यथा यदि (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    फर्काऊ OK;
}

/*
 * Cleanup the state information used for parsing lines and populating a
 * quickfix list.
 */
    स्थिर शून्य
qf_cleanup_state(qfstate_T *pstate)
{
    यदि (pstate->fd != रिक्त)
	ख_बंद(pstate->fd);

    vim_free(pstate->growbuf);
    यदि (pstate->vc.vc_type != CONV_NONE)
	convert_setup(&pstate->vc, रिक्त, रिक्त);
}

/*
 * Process the next line from a file/buffer/list/string and add it
 * to the quickfix list 'qfl'.
 */
    स्थिर पूर्णाङ्क
qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    पूर्णाङ्क		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    यदि (status != QF_OK)
	फर्काऊ status;

    status = qf_parse_पङ्क्ति(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    यदि (status != QF_OK)
	फर्काऊ status;

    फर्काऊ qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != रिक्त)
		? fields->namebuf
		: ((qfl->qf_currfile != रिक्त && fields->valid)
		    ? qfl->qf_currfile : (char_u *)रिक्त),
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
    स्थिर पूर्णाङ्क
qf_init_ext(
    qf_info_T	    *qi,
    पूर्णाङ्क		    qf_idx,
    char_u	    *efile,
    buf_T	    *buf,
    typval_T	    *tv,
    char_u	    *errorformat,
    पूर्णाङ्क		    newlist,		// TRUE: start a new error list
    linenr_T	    lnumfirst,		// first line number to use
    linenr_T	    lnumlast,		// last line number to use
    char_u	    *qf_title,
    char_u	    *enc)
{
    qf_list_T	    *qfl;
    qfstate_T	    state;
    qffields_T	    fields;
    qfline_T	    *old_last = रिक्त;
    पूर्णाङ्क		    adding = FALSE;
    स्थिर efm_T    *fmt_first = रिक्त;
    char_u	    *efm;
    स्थिर char_u   *last_efm = रिक्त;
    पूर्णाङ्क		    retval = -1;	// default: return error flag
    पूर्णाङ्क		    status;

    // Do not used the cached buffer, it may have been wiped out.
    VIM_CLEAR(qf_last_bufname);

    CLEAR_FIELD(state);
    CLEAR_FIELD(fields);
    यदि ((qf_alloc_fields(&fields) == FAIL) ||
		(qf_setup_state(&state, enc, efile, tv, buf,
					lnumfirst, lnumlast) == FAIL))
	गच्छ qf_init_end;

    यदि (newlist || qf_idx == qi->qf_listcount)
    {
	// make place for a new list
	qf_new_list(qi, qf_title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    अन्यथा
    {
	// Adding to existing list, use last entry.
	adding = TRUE;
	qfl = qf_get_list(qi, qf_idx);
	यदि (!qf_list_empty(qfl))
	    old_last = qfl->qf_last;
    }

    // Use the local value of 'errorformat' if it's set.
    यदि (errorformat == p_efm && tv == रिक्त && *buf->b_p_efm != NUL)
	efm = buf->b_p_efm;
    अन्यथा
	efm = errorformat;

    // If the errorformat didn't change between calls, then reuse the
    // previously parsed values.
    यदि (last_efm == रिक्त || (STRCMP(last_efm, efm) != 0))
    {
	// free the previously parsed data
	VIM_CLEAR(last_efm);
	free_efm_list(&fmt_first);

	// parse the current 'efm'
	fmt_first = parse_efm_option(efm);
	यदि (fmt_first != रिक्त)
	    last_efm = vim_strsave(efm);
    }

    यदि (fmt_first == रिक्त)	// nothing found
	गच्छ error2;

    // got_int is reset here, because it was probably set when killing the
    // ":make" command, but we still want to read the errorfile then.
    got_int = FALSE;

    // Read the lines in the error file one by one.
    // Try to recognize one of the error formats in each line.
    जबसम्म (!got_int)
    {
	status = qf_init_process_nextline(qfl, fmt_first, &state, &fields);
	यदि (status == QF_NOMEM)		// memory alloc failure
	    गच्छ qf_init_end;
	यदि (status == QF_END_OF_आगत)	// end of input
	    रोक;
	यदि (status == QF_FAIL)
	    गच्छ error2;

	पङ्क्ति_breakcheck();
    }
    यदि (state.fd == रिक्त || !ख_त्रुटि(state.fd))
    {
	यदि (qfl->qf_index == 0)
	{
	    // no valid entry found
	    qfl->qf_ptr = qfl->qf_start;
	    qfl->qf_index = 1;
	    qfl->qf_nonevalid = TRUE;
	}
	अन्यथा
	{
	    qfl->qf_nonevalid = FALSE;
	    यदि (qfl->qf_ptr == रिक्त)
		qfl->qf_ptr = qfl->qf_start;
	}
	// return number of matches
	retval = qfl->qf_गन्ती;
	गच्छ qf_init_end;
    }
    emsg(_(e_error_while_reading_errorfile));
error2:
    यदि (!adding)
    {
	// Error when creating a new list. Free the new list
	qf_free(qfl);
	qi->qf_listcount--;
	यदि (qi->qf_curlist > 0)
	    --qi->qf_curlist;
    }
qf_init_end:
    यदि (qf_idx == qi->qf_curlist)
	qf_update_buffer(qi, old_last);
    qf_cleanup_state(&state);
    qf_free_fields(&fields);

    फर्काऊ retval;
}

/*
 * Read the errorfile "efile" into memory, line by line, building the error
 * list. Set the error list's title to qf_title.
 * Return -1 for error, number of errors for success.
 */
    पूर्णाङ्क
qf_init(win_T	    *wp,
	char_u	    *efile,
	char_u	    *errorformat,
	पूर्णाङ्क	    newlist,		// TRUE: start a new error list
	char_u	    *qf_title,
	char_u	    *enc)
{
    qf_info_T	    *qi = ql_info;

    यदि (wp != रिक्त)
	qi = ll_get_or_alloc_list(wp);
    यदि (qi == रिक्त)
	फर्काऊ FAIL;

    फर्काऊ qf_init_ext(qi, qi->qf_curlist, efile, curbuf, रिक्त, errorformat,
	    newlist, (linenr_T)0, (linenr_T)0, qf_title, enc);
}

/*
 * Set the title of the specified quickfix list. Frees the previous title.
 * Prepends ':' to the title.
 */
    स्थिर शून्य
qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    यदि (title == रिक्त)
	फर्काऊ;

    char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

    qfl->qf_title = p;
    यदि (p != रिक्त)
	STRCPY(p, title);
}

/*
 * The title of a quickfix/location list is set, by default, to the command
 * that created the quickfix list with the ":" prefix.
 * Create a quickfix list title string by prepending ":" to a user command.
 * Returns a pointer to a static buffer with the title.
 */
    स्थिर char_u *
qf_cmdtitle(char_u *cmd)
{
    स्थिर char_u qftitle_str[IOSIZE];

    vim_snprintf((वर्ण *)qftitle_str, IOSIZE, ":%s", (वर्ण *)cmd);
    फर्काऊ qftitle_str;
}

/*
 * Return a pointer to the current list in the specified quickfix stack
 */
    स्थिर qf_list_T *
qf_get_curlist(qf_info_T *qi)
{
    फर्काऊ qf_get_list(qi, qi->qf_curlist);
}

/*
 * Pop a quickfix list from the quickfix/location list stack
 * Automatically adjust qf_curlist so that it stays pointed
 * to the same list, unless it is deleted, if so then use the
 * newest created list instead. qf_listcount will be set correctly.
 * The above will only happen if <adjust> is TRUE.
 */
    स्थिर शून्य
qf_झिक_थाक(qf_info_T *qi, पूर्णाङ्क adjust)
{
    पूर्णाङ्क i;
    qf_free(&qi->qf_lists[0]);
    लागि (i = 1; i < qi->qf_listcount; ++i)
	qi->qf_lists[i - 1] = qi->qf_lists[i];

    // fill with zeroes now unused list at the top
    vim_memset(qi->qf_lists + qi->qf_listcount - 1, 0, परिमाण(*qi->qf_lists));

    यदि (adjust)
    {
	qi->qf_listcount--;
	यदि (qi->qf_curlist == 0)
	    qi->qf_curlist = qi->qf_listcount - 1;
	अन्यथा
	    qi->qf_curlist--;
    }
}

/*
 * Prepare for adding a new quickfix list. If the current list is in the
 * middle of the stack, then all the following lists are freed and then
 * the new list is added.
 */
    स्थिर शून्य
qf_new_list(qf_info_T *qi, char_u *qf_title)
{
    qf_list_T	*qfl;

    // If the current entry is not the last entry, delete entries beyond
    // the current entry.  This makes it possible to browse in a tree-like
    // way with ":grep".
    जबसम्म (qi->qf_listcount > qi->qf_curlist + 1)
	qf_free(&qi->qf_lists[--qi->qf_listcount]);

    // When the stack is full, remove to oldest entry
    // Otherwise, add a new entry.
    यदि (qi->qf_listcount == qi->qf_maxcount)
    {
	qf_झिक_थाक(qi, FALSE);
	qi->qf_curlist = qi->qf_listcount - 1; // point to new empty list
    }
    अन्यथा
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
    स्थिर शून्य
locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    यदि (q == रिक्त)
	फर्काऊ;

    q->qi = qi;
    q->next = qf_delq_head;
    qf_delq_head = q;
}

/*
 * Return the global quickfix stack window buffer number.
 */
    पूर्णाङ्क
qf_थाक_get_bufnr(शून्य)
{
    यदि (ql_info == रिक्त)
	फर्काऊ INVALID_QFBUFNR;
    फर्काऊ ql_info->qf_bufnr;
}

/*
 * Wipe the quickfix window buffer (if present) for the specified
 * quickfix/location list.
 */
    स्थिर शून्य
wipe_qf_buffer(qf_info_T *qi)
{
    buf_T	*qfbuf;

    यदि (qi->qf_bufnr == INVALID_QFBUFNR)
	फर्काऊ;

    qfbuf = buflist_findnr(qi->qf_bufnr);
    यदि (qfbuf != रिक्त && qfbuf->b_nwindows == 0)
    {
	पूर्णाङ्क buf_was_null = FALSE;
	// Can happen when curwin is closing (e.g: w_buffer was unloaded in
	// win_close()) and we are now closing the window-related location list
	// buffer from win_free().  close_buffer() calls CHECK_CURBUF() and
	// requires curwin->w_buffer == curbuf.  Should be OK to not increment
	// b_nwindows, especially as autocmds are blocked in win_free().
	यदि (curwin->w_buffer == रिक्त)
	{
	    curwin->w_buffer = curbuf;
	    buf_was_null = TRUE;
	}

	// If the quickfix buffer is not loaded in any window, then
	// wipe the buffer.
	close_buffer(रिक्त, qfbuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	qi->qf_bufnr = INVALID_QFBUFNR;
	यदि (buf_was_null)
	    curwin->w_buffer = रिक्त;
    }
}


/*
 * Free all lists in the stack (not including the stack)
 */
    स्थिर शून्य
qf_free_list_थाक_वस्तुहरू(qf_info_T *qi)
{
    लागि (पूर्णाङ्क i = 0; i < qi->qf_listcount; ++i)
	qf_free(qf_get_list(qi, i));
}

/*
 * Free a qf_info_T struct completely
 */
    स्थिर शून्य
qf_free_lists(qf_info_T *qi)
{
    qf_free_list_थाक_वस्तुहरू(qi);

    vim_free(qi->qf_lists);
    vim_free(qi);
}

/*
 * Free a location list stack
 */
    स्थिर शून्य
ll_free_all(qf_info_T **pqi)
{
    qf_info_T	*qi;

    qi = *pqi;
    यदि (qi == रिक्त)
	फर्काऊ;
    *pqi = रिक्त;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    यदि (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	फर्काऊ;
    }

    qi->qf_refcount--;
    यदि (qi->qf_refcount < 1)
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
    शून्य
qf_free_all(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    यदि (wp != रिक्त)
    {
	// location list
	ll_free_all(&wp->w_llist);
	ll_free_all(&wp->w_llist_ref);
    }
    अन्यथा यदि (qi != रिक्त)
	qf_free_list_थाक_वस्तुहरू(qi); // quickfix list
}

/*
 * Delay freeing of location list stacks when the quickfix code is running.
 * Used to avoid problems with autocmds freeing location list stacks when the
 * quickfix code is still referencing the stack.
 * Must always call decr_quickfix_busy() exactly once after this.
 */
    स्थिर शून्य
incr_quickfix_busy(शून्य)
{
    quickfix_busy++;
}

/*
 * Safe to free location list stacks. Process any delayed delete requests.
 */
    स्थिर शून्य
decr_quickfix_busy(शून्य)
{
    यदि (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	जबसम्म (qf_delq_head != रिक्त)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    यदि (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	पात();
    }
# endif
}

# यदि defined(EXITFREE)
    शून्य
check_quickfix_busy(शून्य)
{
    यदि (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (दीर्घ)quickfix_busy);
#  ifdef ABORT_ON_INTERNAL_ERROR
	पात();
#  endif
    }
}
# endif

/*
 * Add an entry to the end of the list of errors.
 * Returns QF_OK on success or QF_FAIL on a memory allocation failure.
 */
    स्थिर पूर्णाङ्क
qf_add_entry(
    qf_list_T	*qfl,		// quickfix list entry
    char_u	*dir,		// optional directory name
    char_u	*fname,		// file name or NULL
    char_u	*module,	// module name or NULL
    पूर्णाङ्क		bufnum,		// buffer number or zero
    char_u	*mesg,		// message
    दीर्घ	lnum,		// line number
    दीर्घ	end_lnum,	// line number for end
    पूर्णाङ्क		col,		// column
    पूर्णाङ्क		end_col,	// column for end
    पूर्णाङ्क		vis_col,	// using visual column
    char_u	*pattern,	// search pattern
    पूर्णाङ्क		nr,		// error number
    पूर्णाङ्क		type,		// type character
    typval_T	*user_data,     // custom user data or NULL
    पूर्णाङ्क		valid)		// valid entry
{
    buf_T	*buf;
    qfline_T	*qfp;
    qfline_T	**lastp;	// pointer to qf_last or NULL
    char_u	*fullname = रिक्त;
    char_u	*p = रिक्त;

    यदि ((qfp = ALLOC_ONE_ID(qfline_T, aid_qf_qfline)) == रिक्त)
	फर्काऊ QF_FAIL;
    यदि (bufnum != 0)
    {
	buf = buflist_findnr(bufnum);

	qfp->qf_fnum = bufnum;
	यदि (buf != रिक्त)
	    buf->b_has_qf_entry |=
		IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    }
    अन्यथा
    {
	qfp->qf_fnum = qf_get_fnum(qfl, dir, fname);
	buf = buflist_findnr(qfp->qf_fnum);
    }
    यदि (fname != रिक्त)
	fullname = fix_fname(fname);
    qfp->qf_fname = रिक्त;
    यदि (buf != रिक्त &&
	buf->b_ffname != रिक्त && fullname != रिक्त)
    {
	यदि (fnamecmp(fullname, buf->b_ffname) != 0)
	{
	    p = shorten_fname1(fullname);
	    यदि (p != रिक्त)
		qfp->qf_fname = vim_strsave(p);
	}
    }
    vim_free(fullname);
    यदि ((qfp->qf_text = vim_strsave(mesg)) == रिक्त)
    {
	vim_free(qfp);
	फर्काऊ QF_FAIL;
    }
    qfp->qf_lnum = lnum;
    qfp->qf_end_lnum = end_lnum;
    qfp->qf_col = col;
    qfp->qf_end_col = end_col;
    qfp->qf_viscol = vis_col;
    यदि (user_data == रिक्त || user_data->v_type == VAR_UNKNOWN)
	qfp->qf_user_data.v_type = VAR_UNKNOWN;
    अन्यथा
    {
	copy_tv(user_data, &qfp->qf_user_data);
	qfl->qf_has_user_data = TRUE;
    }
    यदि (pattern == रिक्त || *pattern == NUL)
	qfp->qf_pattern = रिक्त;
    अन्यथा यदि ((qfp->qf_pattern = vim_strsave(pattern)) == रिक्त)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp);
	फर्काऊ QF_FAIL;
    }
    यदि (module == रिक्त || *module == NUL)
	qfp->qf_module = रिक्त;
    अन्यथा यदि ((qfp->qf_module = vim_strsave(module)) == रिक्त)
    {
	vim_free(qfp->qf_text);
	vim_free(qfp->qf_pattern);
	vim_free(qfp);
	फर्काऊ QF_FAIL;
    }
    qfp->qf_nr = nr;
    यदि (type != 1 && !vim_isprintc(type)) // only printable chars allowed
	type = 0;
    qfp->qf_type = type;
    qfp->qf_valid = valid;

    lastp = &qfl->qf_last;
    यदि (qf_list_empty(qfl))		// first element in the list
    {
	qfl->qf_start = qfp;
	qfl->qf_ptr = qfp;
	qfl->qf_index = 0;
	qfp->qf_prev = रिक्त;
    }
    अन्यथा
    {
	qfp->qf_prev = *lastp;
	(*lastp)->qf_next = qfp;
    }
    qfp->qf_next = रिक्त;
    qfp->qf_cleared = FALSE;
    *lastp = qfp;
    ++qfl->qf_गन्ती;
    यदि (qfl->qf_index == 0 && qfp->qf_valid)	// first valid entry
    {
	qfl->qf_index = qfl->qf_गन्ती;
	qfl->qf_ptr = qfp;
    }

    फर्काऊ QF_OK;
}

/*
 * Resize quickfix stack to be able to hold n amount of lists.
 * returns FAIL on failure and OK on success.
 */
    पूर्णाङ्क
qf_resize_थाक(पूर्णाङ्क n)
{
    यदि (ql_info == रिक्त)
	फर्काऊ FAIL;

    यदि (qf_resize_थाक_base(ql_info, n) == FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * Resize location list stack for window 'wp' to be able to
 * hold n amount of lists. Returns FAIL on failure and OK on success
 */
    पूर्णाङ्क
ll_resize_थाक(win_T *wp, पूर्णाङ्क n)
{
    // check if given window is a location list window;
    // if so then sync its 'lhistory' to the parent window or vice versa
    यदि (IS_LL_WINDOW(wp))
	qf_sync_llw_to_win(wp);
    अन्यथा
	qf_sync_win_to_llw(wp);

    qf_info_T *qi = ll_get_or_alloc_list(wp);
    यदि (qi == रिक्त)
	फर्काऊ FAIL;

    यदि (qf_resize_थाक_base(qi, n) == FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * Resize quickfix/location lists stack to be able to hold n amount of lists.
 * Returns FAIL on failure and OK on success.
 */
    स्थिर पूर्णाङ्क
qf_resize_थाक_base(qf_info_T *qi, पूर्णाङ्क n)
{
    qf_list_T *new;
    पूर्णाङ्क amount_to_rm = 0, i;

    यदि (qi == रिक्त)
	फर्काऊ FAIL;

    माप_प्रकार lsz = परिमाण(*qi->qf_lists);

    यदि (n == qi->qf_maxcount)
	फर्काऊ OK;
    अन्यथा यदि (n < qi->qf_maxcount && n < qi->qf_listcount)
    {
	// We have too many lists to store them all in the new stack,
	// pop lists until we can fit them all in the newly resized stack
	amount_to_rm = qi->qf_listcount - n;

	लागि (i = 0; i < amount_to_rm; i++)
	    qf_झिक_थाक(qi, TRUE);
    }

    new = vim_realloc(qi->qf_lists, lsz * n);

    यदि (new == रिक्त)
	फर्काऊ FAIL;

    // fill with zeroes any newly allocated memory
    यदि (n > qi->qf_maxcount)
	vim_memset(new + qi->qf_maxcount, 0, lsz * (n - qi->qf_maxcount));

    qi->qf_lists = new;
    qi->qf_maxcount = n;

    qf_update_buffer(qi, रिक्त);

    फर्काऊ OK;
}

/*
 * Initialize quickfix list, should only be called once.
 */
   शून्य
qf_init_थाक(शून्य)
{
    ql_info = qf_alloc_थाक(QFLT_QUICKFIX, p_chi);
}

/*
 * Sync a location list window's 'lhistory' value to the parent window
 */
    स्थिर शून्य
qf_sync_llw_to_win(win_T *llw)
{
    win_T *wp = qf_find_win_with_loclist(llw->w_llist_ref);

    यदि (wp != रिक्त)
	wp->w_p_lhi = llw->w_p_lhi;
}

/*
 * Sync a window's 'lhistory' value to its location list window, if any
 */
    स्थिर शून्य
qf_sync_win_to_llw(win_T *pwp)
{
    win_T *wp;
    qf_info_T  *llw = pwp->w_llist;

    यदि (llw != रिक्त)
	FOR_ALL_WINDOWS(wp)
	    यदि (wp->w_llist_ref == llw && bt_quickfix(wp->w_buffer))
	    {
		wp->w_p_lhi = pwp->w_p_lhi;
		फर्काऊ;
	    }
}

/*
 * Allocate a new quickfix/location list stack that is able to hold
 * up to n amount of lists
 */
    स्थिर qf_info_T *
qf_alloc_थाक(qfltype_T qfltype, पूर्णाङ्क n)
{
    qf_info_T *qi;

    यदि (qfltype == QFLT_QUICKFIX)
	qi = &ql_info_actual;
    अन्यथा
    {
	qi = ALLOC_CLEAR_ONE_ID(qf_info_T, aid_qf_qfinfo);
	यदि (qi == रिक्त)
	    फर्काऊ रिक्त;
	qi->qf_refcount++;
    }
    qi->qfl_type = qfltype;
    qi->qf_bufnr = INVALID_QFBUFNR;
    qi->qf_lists = qf_alloc_list_थाक(n);
    यदि (qi->qf_lists == रिक्त)
    {
	यदि (qfltype != QFLT_QUICKFIX)
	    vim_free(qi);
	फर्काऊ रिक्त;
    }
    qi->qf_maxcount = n;

    फर्काऊ qi;
}

/*
 * Allocate memory for qf_lists member of qf_info_T struct.
 */
    स्थिर qf_list_T *
qf_alloc_list_थाक(पूर्णाङ्क n)
{
    फर्काऊ ALLOC_CLEAR_MULT(qf_list_T, n);
}

/*
 * Return the location list stack for window 'wp'.
 * If not present, allocate a location list stack
 */
    स्थिर qf_info_T *
ll_get_or_alloc_list(win_T *wp)
{
    यदि (IS_LL_WINDOW(wp))
	// For a location list window, use the referenced location list
	फर्काऊ wp->w_llist_ref;

    // For a non-location list window, w_llist_ref should not point to a
    // location list.
    ll_free_all(&wp->w_llist_ref);

    यदि (wp->w_llist == रिक्त)
	// new location list
	wp->w_llist = qf_alloc_थाक(QFLT_LOCATION, wp->w_p_lhi);

    फर्काऊ wp->w_llist;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.  If
 * the location list is not found, then returns NULL and prints an error
 * message if 'print_emsg' is TRUE.
 */
    स्थिर qf_info_T *
qf_cmd_get_थाक(exarg_T *eap, पूर्णाङ्क print_emsg)
{
    qf_info_T	*qi = ql_info;

    यदि (is_loclist_cmd(eap->cmdidx))
    {
	qi = GET_LOC_LIST(curwin);
	यदि (qi == रिक्त)
	{
	    यदि (print_emsg)
		emsg(_(e_no_location_list));
	    फर्काऊ रिक्त;
	}
    }
    यदि (qi == रिक्त && print_emsg)
	emsg(_(e_no_quickfix_थाक));

    फर्काऊ qi;
}

/*
 * Get the quickfix/location list stack to use for the specified Ex command.
 * For a location list command, returns the stack for the current window.
 * If the location list is not present, then allocates a new one.
 * Returns NULL if the allocation fails.  For a location list command, sets
 * 'pwinp' to curwin.
 */
    स्थिर qf_info_T *
qf_cmd_get_or_alloc_थाक(exarg_T *eap, win_T **pwinp)
{
    qf_info_T	*qi = ql_info;

    यदि (is_loclist_cmd(eap->cmdidx))
    {
	qi = ll_get_or_alloc_list(curwin);
	यदि (qi == रिक्त)
	    फर्काऊ रिक्त;
	*pwinp = curwin;
    }

    फर्काऊ qi;
}

/*
 * Copy location list entries from 'from_qfl' to 'to_qfl'.
 */
    स्थिर पूर्णाङ्क
copy_loclist_entries(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    पूर्णाङ्क		i;
    qfline_T    *from_qfp;
    qfline_T    *prevp;

    // copy all the location entries in this list
    FOR_ALL_QFL_वस्तुहरू(from_qfl, from_qfp, i)
    {
	यदि (qf_add_entry(to_qfl,
		    रिक्त,
		    रिक्त,
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
	    फर्काऊ FAIL;

	// qf_add_entry() will not set the qf_num field, as the
	// directory and file names are not supplied. So the qf_fnum
	// field is copied here.
	prevp = to_qfl->qf_last;
	prevp->qf_fnum = from_qfp->qf_fnum;	// file number
	prevp->qf_type = from_qfp->qf_type;	// error type
	यदि (from_qfl->qf_ptr == from_qfp)
	    to_qfl->qf_ptr = prevp;		// current location
    }

    फर्काऊ OK;
}

/*
 * Copy the specified location list 'from_qfl' to 'to_qfl'.
 */
    स्थिर पूर्णाङ्क
copy_loclist(qf_list_T *from_qfl, qf_list_T *to_qfl)
{
    // Some of the fields are populated by qf_add_entry()
    to_qfl->qfl_type = from_qfl->qfl_type;
    to_qfl->qf_nonevalid = from_qfl->qf_nonevalid;
    to_qfl->qf_has_user_data = from_qfl->qf_has_user_data;
    to_qfl->qf_गन्ती = 0;
    to_qfl->qf_index = 0;
    to_qfl->qf_start = रिक्त;
    to_qfl->qf_last = रिक्त;
    to_qfl->qf_ptr = रिक्त;
    यदि (from_qfl->qf_title != रिक्त)
	to_qfl->qf_title = vim_strsave(from_qfl->qf_title);
    अन्यथा
	to_qfl->qf_title = रिक्त;
    यदि (from_qfl->qf_ctx != रिक्त)
    {
	to_qfl->qf_ctx = alloc_tv();
	यदि (to_qfl->qf_ctx != रिक्त)
	    copy_tv(from_qfl->qf_ctx, to_qfl->qf_ctx);
    }
    अन्यथा
	to_qfl->qf_ctx = रिक्त;
    यदि (from_qfl->qf_qftf_cb.cb_name != रिक्त)
	copy_callback(&to_qfl->qf_qftf_cb, &from_qfl->qf_qftf_cb);
    अन्यथा
	to_qfl->qf_qftf_cb.cb_name = रिक्त;

    यदि (from_qfl->qf_गन्ती)
	यदि (copy_loclist_entries(from_qfl, to_qfl) == FAIL)
	    फर्काऊ FAIL;

    to_qfl->qf_index = from_qfl->qf_index;	// current index in the list

    // Assign a new ID for the location list
    to_qfl->qf_id = ++last_qf_id;
    to_qfl->qf_changedtick = 0L;

    // When no valid entries are present in the list, qf_ptr points to
    // the first item in the list
    यदि (to_qfl->qf_nonevalid)
    {
	to_qfl->qf_ptr = to_qfl->qf_start;
	to_qfl->qf_index = 1;
    }

    फर्काऊ OK;
}

/*
 * Copy the location list stack 'from' window to 'to' window.
 */
    शून्य
copy_loclist_थाक(win_T *from, win_T *to)
{
    qf_info_T	*qi;
    पूर्णाङ्क		idx;

    // When copying from a location list window, copy the referenced
    // location list. For other windows, copy the location list for
    // that window.
    यदि (IS_LL_WINDOW(from))
	qi = from->w_llist_ref;
    अन्यथा
	qi = from->w_llist;

    यदि (qi == रिक्त)		    // no location list to copy
	फर्काऊ;

    // allocate a new location list, set size of stack to 'from' window value
    यदि ((to->w_llist = qf_alloc_थाक(QFLT_LOCATION, from->w_p_lhi)) == रिक्त)
	फर्काऊ;
    अन्यथा
	// set 'to' lhi to reflect new value
	to->w_p_lhi = to->w_llist->qf_maxcount;

    to->w_llist->qf_listcount = qi->qf_listcount;

    // Copy the location lists one at a time
    लागि (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	to->w_llist->qf_curlist = idx;

	यदि (copy_loclist(qf_get_list(qi, idx),
			qf_get_list(to->w_llist, idx)) == FAIL)
	{
	    qf_free_all(to);
	    फर्काऊ;
	}
    }

    to->w_llist->qf_curlist = qi->qf_curlist;	// current list
}

/*
 * Get buffer number for file "directory/fname".
 * Also sets the b_has_qf_entry flag.
 */
    स्थिर पूर्णाङ्क
qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    string_T	ptr = {रिक्त, 0};
    buf_T	*buf;
    string_T	bufname;
    माप_प्रकार	fname_len;

    यदि (fname == रिक्त || *fname == NUL)		// no file name
	फर्काऊ 0;

# ifdef VMS
    vms_remove_version(fname);
# endif
# ifdef BACKSLASH_IN_FILENAME
    यदि (directory != रिक्त)
	slash_adjust(directory);
    slash_adjust(fname);
# endif
    fname_len = STRLEN(fname);
    यदि (directory != रिक्त && !vim_isAbsName(fname)
	    && concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr) != रिक्त)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	यदि (mch_getperm(ptr.string) < 0)
	{
	    vim_free(ptr.string);
	    directory = qf_guess_filepath(qfl, fname);
	    यदि (directory)
		concat_fnames(directory, STRLEN(directory), fname, fname_len, TRUE, &ptr);
	    अन्यथा
	    {
		ptr.length = fname_len;
		ptr.string = vim_strnsave(fname, fname_len);
	    }
	    यदि (ptr.string == रिक्त)
		फर्काऊ 0;
	}
	// Use concatenated directory name and file name
	bufname.string = ptr.string;
	bufname.length = ptr.length;
    }
    अन्यथा
    {
	bufname.string = fname;
	bufname.length = fname_len;
    }

    यदि (qf_last_bufname != रिक्त && STRCMP(bufname.string, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr.string);
    }
    अन्यथा
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname.string, रिक्त, (linenr_T)0, BLN_NOOPT);
	यदि (bufname.string == ptr.string)
	    qf_last_bufname = bufname.string;
	अन्यथा
	    qf_last_bufname = vim_strnsave(bufname.string, bufname.length);
	set_bufref(&qf_last_bufref, buf);
    }
    यदि (buf == रिक्त)
	फर्काऊ 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    फर्काऊ buf->b_fnum;
}

/*
 * Push dirbuf onto the directory stack and return pointer to actual dir or
 * NULL on error.
 */
    स्थिर char_u *
qf_थप_dir(char_u *dirbuf, संरचना dir_थाक_T **stackptr, पूर्णाङ्क is_file_थाक)
{
    संरचना dir_थाक_T  *ds_new;
    संरचना dir_थाक_T  *ds_ptr;

    // allocate new stack element
    ds_new = ALLOC_ONE_ID(संरचना dir_थाक_T, aid_qf_dirstack);
    यदि (ds_new == रिक्त)
	फर्काऊ रिक्त;

    // push the new element onto the stack
    ds_new->next = *stackptr;
    *stackptr = ds_new;

    // store directory on the stack
    यदि (vim_isAbsName(dirbuf)
	    || (*stackptr)->next == रिक्त
	    || is_file_थाक)
	(*stackptr)->स_नाम = vim_strsave(dirbuf);
    अन्यथा
    {
	माप_प्रकार	dirbuf_len;

	// Okay we don't have an absolute path.
	// dirbuf must be a subdir of one of the directories on the stack.
	// Let's search...
	dirbuf_len = STRLEN(dirbuf);
	ds_new = (*stackptr)->next;
	(*stackptr)->स_नाम = रिक्त;
	जबसम्म (ds_new)
	{
	    string_T	स_नाम;

	    concat_fnames(ds_new->स_नाम, STRLEN(ds_new->स_नाम),
		dirbuf, dirbuf_len, TRUE, &स_नाम);
	    यदि (स_नाम.string == रिक्त)
	    {
		// pop the new element from the stack and free it
		ds_ptr = *stackptr;
		*stackptr = (*stackptr)->next;
		vim_free(ds_ptr);

		फर्काऊ रिक्त;
	    }
	    यदि (mch_isdir(स_नाम.string) == TRUE)
	    {
		vim_free((*stackptr)->स_नाम);
		(*stackptr)->स_नाम = स_नाम.string;
		रोक;
	    }
	    vim_free(स_नाम.string);
	    ds_new = ds_new->next;
	}

	// clean up all dirs we already left
	जबसम्म ((*stackptr)->next != ds_new)
	{
	    ds_ptr = (*stackptr)->next;
	    (*stackptr)->next = (*stackptr)->next->next;
	    vim_free(ds_ptr->स_नाम);
	    vim_free(ds_ptr);
	}

	// Nothing found -> it must be on top level
	यदि (ds_new == रिक्त)
	{
	    vim_free((*stackptr)->स_नाम);
	    (*stackptr)->स_नाम = vim_strnsave(dirbuf, dirbuf_len);
	}
    }

    यदि ((*stackptr)->स_नाम != रिक्त)
	फर्काऊ (*stackptr)->स_नाम;

    // pop the new element from the stack and free it
    ds_ptr = *stackptr;
    *stackptr = (*stackptr)->next;
    vim_free(ds_ptr);

    फर्काऊ रिक्त;
}

/*
 * pop dirbuf from the directory stack and return previous directory or NULL if
 * stack is empty
 */
    स्थिर char_u *
qf_झिक_dir(संरचना dir_थाक_T **stackptr)
{
    संरचना dir_थाक_T  *ds_ptr;

    // TODO: Should we check if dirbuf is the directory on top of the stack?
    // What to do if it isn't?

    // pop top element and free it
    यदि (*stackptr != रिक्त)
    {
	ds_ptr = *stackptr;
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->स_नाम);
	vim_free(ds_ptr);
    }

    // return NEW top element as current dir or NULL if stack is empty
    फर्काऊ *stackptr ? (*stackptr)->स_नाम : रिक्त;
}

/*
 * clean up directory stack
 */
    स्थिर शून्य
qf_clean_dir_थाक(संरचना dir_थाक_T **stackptr)
{
    संरचना dir_थाक_T  *ds_ptr;

    जबसम्म ((ds_ptr = *stackptr) != रिक्त)
    {
	*stackptr = (*stackptr)->next;
	vim_free(ds_ptr->स_नाम);
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
    स्थिर char_u *
qf_guess_filepath(qf_list_T *qfl, char_u *filename)
{
    संरचना dir_थाक_T     *ds_ptr;
    संरचना dir_थाक_T     *ds_tmp;
    string_T		   fullname;
    माप_प्रकार		   filename_len;

    // no dirs on the stack - there's nothing we can do
    यदि (qfl->qf_dir_थाक == रिक्त)
	फर्काऊ रिक्त;

    ds_ptr = qfl->qf_dir_थाक->next;
    fullname.string = रिक्त;
    fullname.length = 0;
    filename_len = STRLEN(filename);
    जबसम्म (ds_ptr)
    {
	vim_free(fullname.string);
	concat_fnames(ds_ptr->स_नाम, STRLEN(ds_ptr->स_नाम),
	    filename, filename_len, TRUE, &fullname);

	// If concat_fnames failed, just go on. The worst thing that can happen
	// is that we delete the entire stack.
	यदि ((fullname.string != रिक्त) && (mch_getperm(fullname.string) >= 0))
	    रोक;

	ds_ptr = ds_ptr->next;
    }

    vim_free(fullname.string);

    // clean up all dirs we already left
    जबसम्म (qfl->qf_dir_थाक->next != ds_ptr)
    {
	ds_tmp = qfl->qf_dir_थाक->next;
	qfl->qf_dir_थाक->next = qfl->qf_dir_थाक->next->next;
	vim_free(ds_tmp->स_नाम);
	vim_free(ds_tmp);
    }

    फर्काऊ ds_ptr == रिक्त ? रिक्त : ds_ptr->स_नाम;
}

/*
 * Returns TRUE if a quickfix/location list with the given identifier exists.
 */
    स्थिर पूर्णाङ्क
qflist_valid(win_T *wp, int_u qf_id)
{
    qf_info_T	*qi = ql_info;
    पूर्णाङ्क		i;

    यदि (wp != रिक्त)
    {
	यदि (!win_valid(wp))
	    फर्काऊ FALSE;
	qi = GET_LOC_LIST(wp);	    // Location list
    }
    यदि (qi == रिक्त)
	फर्काऊ FALSE;

    लागि (i = 0; i < qi->qf_listcount; ++i)
	यदि (qi->qf_lists[i].qf_id == qf_id)
	    फर्काऊ TRUE;

    फर्काऊ FALSE;
}

/*
 * When loading a file from the quickfix, the autocommands may modify it.
 * This may invalidate the current quickfix entry.  This function checks
 * whether an entry is still present in the quickfix list.
 * Similar to location list.
 */
    स्थिर पूर्णाङ्क
is_qf_entry_present(qf_list_T *qfl, qfline_T *qf_ptr)
{
    qfline_T	*qfp;
    पूर्णाङ्क		i;

    // Search for the entry in the current list
    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
	यदि (qfp == qf_ptr)
	    रोक;

    यदि (i > qfl->qf_गन्ती) // Entry is not found
	फर्काऊ FALSE;

    फर्काऊ TRUE;
}

/*
 * Get the next valid entry in the current quickfix/location list. The search
 * starts from the current entry.  Returns NULL on failure.
 */
    स्थिर qfline_T *
get_next_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	पूर्णाङ्क		*qf_index,
	पूर्णाङ्क		dir)
{
    पूर्णाङ्क			idx;
    पूर्णाङ्क			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    गर
    {
	यदि (idx == qfl->qf_गन्ती || qf_ptr->qf_next == रिक्त)
	    फर्काऊ रिक्त;
	++idx;
	qf_ptr = qf_ptr->qf_next;
    } जबसम्म ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == FORWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    फर्काऊ qf_ptr;
}

/*
 * Get the previous valid entry in the current quickfix/location list. The
 * search starts from the current entry.  Returns NULL on failure.
 */
    स्थिर qfline_T *
get_prev_valid_entry(
	qf_list_T	*qfl,
	qfline_T	*qf_ptr,
	पूर्णाङ्क		*qf_index,
	पूर्णाङ्क		dir)
{
    पूर्णाङ्क			idx;
    पूर्णाङ्क			old_qf_fnum;

    idx = *qf_index;
    old_qf_fnum = qf_ptr->qf_fnum;

    गर
    {
	यदि (idx == 1 || qf_ptr->qf_prev == रिक्त)
	    फर्काऊ रिक्त;
	--idx;
	qf_ptr = qf_ptr->qf_prev;
    } जबसम्म ((!qfl->qf_nonevalid && !qf_ptr->qf_valid)
	    || (dir == BACKWARD_FILE && qf_ptr->qf_fnum == old_qf_fnum));

    *qf_index = idx;
    फर्काऊ qf_ptr;
}

/*
 * Get the n'th (errornr) previous/next valid entry from the current entry in
 * the quickfix list.
 *   dir == FORWARD or FORWARD_FILE: next valid entry
 *   dir == BACKWARD or BACKWARD_FILE: previous valid entry
 */
    स्थिर qfline_T *
get_nth_valid_entry(
	qf_list_T	*qfl,
	पूर्णाङ्क		errornr,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		*new_qfidx)
{
    qfline_T		*qf_ptr = qfl->qf_ptr;
    पूर्णाङ्क			qf_idx = qfl->qf_index;
    qfline_T		*prev_qf_ptr;
    पूर्णाङ्क			prev_index;
    वर्ण		*err = e_no_more_वस्तुहरू;

    जबसम्म (errornr--)
    {
	prev_qf_ptr = qf_ptr;
	prev_index = qf_idx;

	यदि (dir == FORWARD || dir == FORWARD_FILE)
	    qf_ptr = get_next_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	अन्यथा
	    qf_ptr = get_prev_valid_entry(qfl, qf_ptr, &qf_idx, dir);
	यदि (qf_ptr == रिक्त)
	{
	    qf_ptr = prev_qf_ptr;
	    qf_idx = prev_index;
	    यदि (err != रिक्त)
	    {
		emsg(_(err));
		फर्काऊ रिक्त;
	    }
	    रोक;
	}

	err = रिक्त;
    }

    *new_qfidx = qf_idx;
    फर्काऊ qf_ptr;
}

/*
 * Get n'th (errornr) quickfix entry from the current entry in the quickfix
 * list 'qfl'. Returns a pointer to the new entry and the index in 'new_qfidx'
 */
    स्थिर qfline_T *
get_nth_entry(qf_list_T *qfl, पूर्णाङ्क errornr, पूर्णाङ्क *new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    पूर्णाङ्क		qf_idx = qfl->qf_index;

    // New error number is less than the current error number
    जबसम्म (errornr < qf_idx && qf_idx > 1 && qf_ptr->qf_prev != रिक्त)
    {
	--qf_idx;
	qf_ptr = qf_ptr->qf_prev;
    }
    // New error number is greater than the current error number
    जबसम्म (errornr > qf_idx && qf_idx < qfl->qf_गन्ती &&
						qf_ptr->qf_next != रिक्त)
    {
	++qf_idx;
	qf_ptr = qf_ptr->qf_next;
    }

    *new_qfidx = qf_idx;
    फर्काऊ qf_ptr;
}

/*
 * Get a entry specified by 'errornr' and 'dir' from the current
 * quickfix/location list. 'errornr' specifies the index of the entry and 'dir'
 * specifies the direction (FORWARD/BACKWARD/FORWARD_FILE/BACKWARD_FILE).
 * Returns a pointer to the entry and the index of the new entry is stored in
 * 'new_qfidx'.
 */
    स्थिर qfline_T *
qf_get_entry(
	qf_list_T	*qfl,
	पूर्णाङ्क		errornr,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		*new_qfidx)
{
    qfline_T	*qf_ptr = qfl->qf_ptr;
    पूर्णाङ्क		qfidx = qfl->qf_index;

    यदि (dir != 0)    // next/prev valid entry
	qf_ptr = get_nth_valid_entry(qfl, errornr, dir, &qfidx);
    अन्यथा यदि (errornr != 0)	// go to specified number
	qf_ptr = get_nth_entry(qfl, errornr, &qfidx);

    *new_qfidx = qfidx;
    फर्काऊ qf_ptr;
}

/*
 * Find a window displaying a Vim help file in the current tab page.
 */
    स्थिर win_T *
qf_find_help_win(शून्य)
{
    win_T *wp;

    FOR_ALL_WINDOWS(wp)
	यदि (bt_help(wp->w_buffer))
	    फर्काऊ wp;

    फर्काऊ रिक्त;
}

/*
 * Set the location list for the specified window to 'qi'.
 */
    स्थिर शून्य
win_set_loclist(win_T *wp, qf_info_T *qi)
{
    wp->w_llist = qi;
    qi->qf_refcount++;
}

/*
 * Find a help window or open one. If 'newwin' is TRUE, then open a new help
 * window.
 */
    स्थिर पूर्णाङ्क
jump_to_help_window(qf_info_T *qi, पूर्णाङ्क newwin, पूर्णाङ्क *opened_window)
{
    win_T	*wp;
    पूर्णाङ्क		flags;

    यदि (cmdmod.cmod_tab != 0 || newwin)
	wp = रिक्त;
    अन्यथा
	wp = qf_find_help_win();
    यदि (wp != रिक्त && wp->w_buffer->b_nwindows > 0)
	win_enter(wp, TRUE);
    अन्यथा
    {
	// Split off help window; put it at far top if no position
	// specified, the current window is vertically split and narrow.
	flags = WSP_HELP;
	यदि (cmdmod.cmod_split == 0 && curwin->w_width != Columns
		&& curwin->w_width < 80)
	    flags |= WSP_शीर्ष;
	// If the user asks to open a new window, then copy the location list.
	// Otherwise, don't copy the location list.
	यदि (IS_LL_थाक(qi) && !newwin)
	    flags |= WSP_NEWLOC;

	यदि (win_split(0, flags) == FAIL)
	    फर्काऊ FAIL;

	*opened_window = TRUE;

	यदि (curwin->w_height < p_hh)
	    win_setheight((पूर्णाङ्क)p_hh);

	// When using location list, the new window should use the supplied
	// location list. If the user asks to open a new window, then the new
	// window will get a copy of the location list.
	यदि (IS_LL_थाक(qi) && !newwin)
	    win_set_loclist(curwin, qi);
    }

    यदि (!p_im)
	restart_edit = 0;	    // don't want insert mode in help file

    फर्काऊ OK;
}

/*
 * Find a non-quickfix window using the given location list stack in the
 * current tabpage.
 * Returns NULL if a matching window is not found.
 */
    स्थिर win_T *
qf_find_win_with_loclist(qf_info_T *ll)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (wp->w_llist == ll && !bt_quickfix(wp->w_buffer))
	    फर्काऊ wp;

    फर्काऊ रिक्त;
}

/*
 * Find a window containing a normal buffer in the current tab page.
 */
    स्थिर win_T *
qf_find_win_with_normal_buf(शून्य)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	यदि (bt_normal(wp->w_buffer))
	    फर्काऊ wp;

    फर्काऊ रिक्त;
}

/*
 * Go to a window in any tabpage containing the specified file.  Returns TRUE
 * if successfully jumped to the window. Otherwise returns FALSE.
 */
    स्थिर पूर्णाङ्क
qf_goto_tabwin_with_file(पूर्णाङ्क fnum)
{
    tabpage_T	*tp;
    win_T	*wp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	यदि (wp->w_buffer->b_fnum == fnum)
	{
	    goto_tabpage_win(tp, wp);
	    फर्काऊ TRUE;
	}

    फर्काऊ FALSE;
}

/*
 * Create a new window to show a file above the quickfix window. Called when
 * only the quickfix window is present.
 */
    स्थिर पूर्णाङ्क
qf_open_new_file_win(qf_info_T *ll_ref)
{
    पूर्णाङ्क		flags;

    flags = WSP_ABOVE;
    यदि (ll_ref != रिक्त)
	flags |= WSP_NEWLOC;
    यदि (win_split(0, flags) == FAIL)
	फर्काऊ FAIL;		// not enough room for window
    p_swb = empty_option;	// don't split again
    swb_flags = 0;
    RESET_BINDING(curwin);
    यदि (ll_ref != रिक्त)
	// The new window should use the location list from the
	// location list window
	win_set_loclist(curwin, ll_ref);
    फर्काऊ OK;
}

/*
 * Go to a window that shows the right buffer. If the window is not found, go
 * to the window just above the location list window. This is used for opening
 * a file from a location window and not from a quickfix window. If some usable
 * window is previously found, then it is supplied in 'use_win'.
 */
    स्थिर शून्य
qf_goto_win_with_ll_file(win_T *use_win, पूर्णाङ्क qf_fnum, qf_info_T *ll_ref)
{
    win_T	*win = use_win;

    यदि (win == रिक्त)
    {
	// Find the window showing the selected file in the current tab page.
	FOR_ALL_WINDOWS(win)
	    यदि (win->w_buffer->b_fnum == qf_fnum)
		रोक;
	यदि (win == रिक्त)
	{
	    // Find a previous usable window
	    win = curwin;
	    गर
	    {
		यदि (bt_normal(win->w_buffer))
		    रोक;
		यदि (win->w_prev == रिक्त)
		    win = lastwin;	// wrap around the top
		अन्यथा
		    win = win->w_prev; // go to previous window
	    } जबसम्म (win != curwin);
	}
    }
    win_goto(win);

    // If the location list for the window is not set, then set it
    // to the location list from the location window
    यदि (win->w_llist == रिक्त && ll_ref != रिक्त)
	win_set_loclist(win, ll_ref);
}

/*
 * Go to a window that contains the specified buffer 'qf_fnum'. If a window is
 * not found, then go to the window just above the quickfix window. This is
 * used for opening a file from a quickfix window and not from a location
 * window.
 */
    स्थिर शून्य
qf_goto_win_with_qfl_file(पूर्णाङ्क qf_fnum)
{
    win_T	*win;
    win_T	*altwin;

    win = curwin;
    altwin = रिक्त;
    लागि (;;)
    {
	यदि (win->w_buffer->b_fnum == qf_fnum)
	    रोक;
	यदि (win->w_prev == रिक्त)
	    win = lastwin;	// wrap around the top
	अन्यथा
	    win = win->w_prev;	// go to previous window

	यदि (IS_QF_WINDOW(win))
	{
	    // Didn't find it, go to the window before the quickfix
	    // window, unless 'switchbuf' contains 'uselast': in this case we
	    // try to jump to the previously used window first.
	    यदि ((swb_flags & SWB_USELAST) && win_valid(prevwin) &&
		    !prevwin->w_p_wfb)
		win = prevwin;
	    अन्यथा यदि (altwin != रिक्त)
		win = altwin;
	    अन्यथा यदि (curwin->w_prev != रिक्त)
		win = curwin->w_prev;
	    अन्यथा
		win = curwin->w_next;
	    रोक;
	}

	// Remember a usable window.
	यदि (altwin == रिक्त && !win->w_p_pvw && !win->w_p_wfb &&
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
    स्थिर पूर्णाङ्क
qf_jump_to_usable_window(पूर्णाङ्क qf_fnum, पूर्णाङ्क newwin, पूर्णाङ्क *opened_window)
{
    win_T	*usable_wp = रिक्त;
    पूर्णाङ्क		usable_win = FALSE;
    qf_info_T	*ll_ref = रिक्त;

    // If opening a new window, then don't use the location list referred by
    // the current window.  Otherwise two windows will refer to the same
    // location list.
    यदि (!newwin)
	ll_ref = curwin->w_llist_ref;

    यदि (ll_ref != रिक्त)
    {
	// Find a non-quickfix window with this location list
	usable_wp = qf_find_win_with_loclist(ll_ref);
	यदि (usable_wp != रिक्त)
	    usable_win = TRUE;
    }

    यदि (!usable_win)
    {
	// Locate a window showing a normal buffer
	win_T	*win = qf_find_win_with_normal_buf();
	यदि (win != रिक्त)
	    usable_win = TRUE;
    }

    // If no usable window is found and 'switchbuf' contains "usetab"
    // then search in other tabs.
    यदि (!usable_win && (swb_flags & SWB_USETAB))
	usable_win = qf_goto_tabwin_with_file(qf_fnum);

    // If there is only one window and it is the quickfix window, create a
    // new one above the quickfix window.
    यदि ((ONE_WINDOW && bt_quickfix(curbuf)) || !usable_win || newwin)
    {
	यदि (qf_open_new_file_win(ll_ref) != OK)
	    फर्काऊ FAIL;
	*opened_window = TRUE;	// close it when fail
    }
    अन्यथा
    {
	यदि (curwin->w_llist_ref != रिक्त)	// In a location window
	    qf_goto_win_with_ll_file(usable_wp, qf_fnum, ll_ref);
	अन्यथा					// In a quickfix window
	    qf_goto_win_with_qfl_file(qf_fnum);
    }

    फर्काऊ OK;
}

/*
 * Edit the selected file or help file.
 * Returns OK if successfully edited the file, FAIL on failing to open the
 * buffer and QF_ABORT if the quickfix/location list was freed by an autocmd
 * when opening the buffer.
 */
    स्थिर पूर्णाङ्क
qf_jump_edit_buffer(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	पूर्णाङ्क		forceit,
	पूर्णाङ्क		prev_winid,
	पूर्णाङ्क		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    पूर्णाङ्क		old_changedtick = qfl->qf_changedtick;
    qfltype_T	qfl_type = qfl->qfl_type;
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		old_qf_curlist = qi->qf_curlist;
    पूर्णाङ्क		save_qfid = qfl->qf_id;

    यदि (qf_ptr->qf_type == 1)
    {
	// Open help file (do_ecmd() will set b_help flag, readfile() will
	// set b_p_ro flag).
	यदि (!can_abandon(curbuf, forceit))
	{
	    no_write_message();
	    फर्काऊ FAIL;
	}

	retval = do_ecmd(qf_ptr->qf_fnum, रिक्त, रिक्त, रिक्त, (linenr_T)1,
		ECMD_HIDE + ECMD_SET_HELP,
		prev_winid == curwin->w_id ? curwin : रिक्त);
    }
    अन्यथा
    {
	पूर्णाङ्क	fnum = qf_ptr->qf_fnum;

	यदि (!forceit && curwin->w_p_wfb && curbuf->b_fnum != fnum)
	{
	    यदि (qi->qfl_type == QFLT_LOCATION)
	    {
		// Location lists cannot split or reassign their window
		// so 'winfixbuf' windows must fail
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		फर्काऊ FAIL;
	    }

	    यदि (win_valid(prevwin) && !prevwin->w_p_wfb &&
		    !bt_quickfix(prevwin->w_buffer))
	    {
		// 'winfixbuf' is set; attempt to change to a window without it
		// that isn't a quickfix/location list window.
		win_goto(prevwin);
	    }
	    यदि (curwin->w_p_wfb)
	    {
		// Split the window, which will be 'nowinfixbuf', and set curwin
		// to that
		यदि (win_split(0, 0) == OK)
		    *opened_window = TRUE;

		यदि (curwin->w_p_wfb)
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

	यदि (retval == OK)
	{
	    retval = buflist_getfile(fnum,
		    (linenr_T)1, GETF_SETMARK | GETF_SWITCH, forceit);
	}
    }

    // If a location list, check whether the associated window is still
    // present.
    यदि (qfl_type == QFLT_LOCATION)
    {
	win_T	*wp = win_id2wp(prev_winid);

	यदि (wp == रिक्त && curwin->w_llist != qi)
	{
	    emsg(_(e_current_window_was_closed));
	    *opened_window = FALSE;
	    फर्काऊ QF_ABORT;
	}
    }

    यदि (qfl_type == QFLT_QUICKFIX && !qflist_valid(रिक्त, save_qfid))
    {
	emsg(_(e_current_quickfix_list_was_changed));
	फर्काऊ QF_ABORT;
    }

    // Check if the list was changed.  The pointers may happen to be identical,
    // thus also check qf_changedtick.
    यदि (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	यदि (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	अन्यथा
	    emsg(_(e_current_location_list_was_changed));
	फर्काऊ QF_ABORT;
    }

    फर्काऊ retval;
}

/*
 * Return the byte index in the current line for screen column "vcol"
 * (zero-based).  A <tab> is always counted as 8 screen columns, matching the
 * column numbers compilers report for the "%v" item in 'errorformat',
 * regardless of the buffer's 'tabstop'.
 */
    स्थिर पूर्णाङ्क
qf_screen_col_to_idx(colnr_T vcol)
{
    char_u	*पङ्क्ति = ml_get_curline();
    char_u	*p = पङ्क्ति;
    colnr_T	col = 0;

    जबसम्म (*p != NUL && col < vcol)
    {
	यदि (*p == TAB)
	    col += 8 - (col % 8);
	अन्यथा
	    col += ptr2cells(p);
	यदि (col > vcol)
	    रोक;
	MB_PTR_ADV(p);
    }
    फर्काऊ (पूर्णाङ्क)(p - पङ्क्ति);
}

/*
 * Go to the error line in the current file using either line/column number or
 * a search pattern.
 */
    स्थिर शून्य
qf_jump_goto_पङ्क्ति(
	linenr_T	qf_lnum,
	पूर्णाङ्क		qf_col,
	char_u		qf_viscol,
	char_u		*qf_pattern)
{
    linenr_T		i;

    यदि (qf_pattern == रिक्त)
    {
	// Go to line with error, unless qf_lnum is 0.
	i = qf_lnum;
	यदि (i > 0)
	{
	    यदि (i > curbuf->b_ml.ml_पङ्क्ति_गन्ती)
		i = curbuf->b_ml.ml_पङ्क्ति_गन्ती;
	    curwin->w_cursor.lnum = i;
	}
	यदि (qf_col > 0)
	{
	    curwin->w_cursor.coladd = 0;
	    यदि (qf_viscol == TRUE)
		curwin->w_cursor.col = qf_screen_col_to_idx(qf_col - 1);
	    अन्यथा
		curwin->w_cursor.col = qf_col - 1;
	    curwin->w_set_curswant = true;
	    check_cursor();
	}
	अन्यथा
	    beginline(BL_WHITE | BL_FIX);
    }
    अन्यथा
    {
	pos_T save_cursor;

	// Move the cursor to the first line in the buffer
	save_cursor = curwin->w_cursor;
	curwin->w_cursor.lnum = 0;
	यदि (!do_खोज(रिक्त, '/', '/', qf_pattern, STRLEN(qf_pattern), (दीर्घ)1, खोज_KEEP, रिक्त))
	    curwin->w_cursor = save_cursor;
    }
}

/*
 * Display quickfix list index and size message
 */
    स्थिर शून्य
qf_jump_print_सन्देश(
	qf_info_T	*qi,
	पूर्णाङ्क		qf_index,
	qfline_T	*qf_ptr,
	buf_T		*old_curbuf,
	linenr_T	old_lnum)
{
    linenr_T		i;
    garray_T		*gap;
    माप_प्रकार		IObufflen;

    gap = qfga_get();

    // Update the screen before showing the message, unless the screen
    // scrolled up.
    यदि (!सन्देश_scrolled)
	update_topline_redraw();
    IObufflen = vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE,
	    _("(%d of %d)%s%s: "), qf_index,
	    qf_get_curlist(qi)->qf_गन्ती,
	    qf_ptr->qf_cleared ? _(" (line deleted)") : "",
	    (वर्ण *)qf_types(qf_ptr->qf_type, qf_ptr->qf_nr));
    // Add the message, skipping leading whitespace and newlines.
    ga_concat_len(gap, IObuff, IObufflen);
    qf_fmt_text(gap, skipwhite(qf_ptr->qf_text));
    ga_append(gap, NUL);

    // Output the message.  Overwrite to avoid scrolling when the 'O'
    // flag is present in 'shortmess'; But when not jumping, print the
    // whole message.
    i = सन्देश_scroll;
    यदि (curbuf == old_curbuf && curwin->w_cursor.lnum == old_lnum)
	सन्देश_scroll = TRUE;
    अन्यथा यदि (!सन्देश_scrolled && shortmess(SHM_OVERALL))
	सन्देश_scroll = FALSE;
    सन्देश_attr_keep((वर्ण *)gap->ga_data, 0, TRUE);
    सन्देश_scroll = i;

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
    स्थिर पूर्णाङ्क
qf_jump_open_window(
	qf_info_T	*qi,
	qfline_T	*qf_ptr,
	पूर्णाङ्क		newwin,
	पूर्णाङ्क		*opened_window)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    पूर्णाङ्क		old_changedtick = qfl->qf_changedtick;
    पूर्णाङ्क		old_qf_curlist = qi->qf_curlist;
    qfltype_T	qfl_type = qfl->qfl_type;

    // For ":helpgrep" find a help window or open one.
    यदि (qf_ptr->qf_type == 1 && (!bt_help(curwin->w_buffer)
						      || cmdmod.cmod_tab != 0))
	यदि (jump_to_help_window(qi, newwin, opened_window) == FAIL)
	    फर्काऊ FAIL;
    यदि (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	यदि (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	अन्यथा
	    emsg(_(e_current_location_list_was_changed));
	फर्काऊ QF_ABORT;
    }

    // If currently in the quickfix window, find another window to show the
    // file in.
    यदि (bt_quickfix(curbuf) && !*opened_window)
    {
	// If there is no file specified, we don't know where to go.
	// But do advance, otherwise ":cn" gets stuck.
	यदि (qf_ptr->qf_fnum == 0)
	    फर्काऊ NOTDONE;

	यदि (qf_jump_to_usable_window(qf_ptr->qf_fnum, newwin,
						opened_window) == FAIL)
	    फर्काऊ FAIL;
    }
    यदि (old_qf_curlist != qi->qf_curlist
	    || old_changedtick != qfl->qf_changedtick
	    || !is_qf_entry_present(qfl, qf_ptr))
    {
	यदि (qfl_type == QFLT_QUICKFIX)
	    emsg(_(e_current_quickfix_list_was_changed));
	अन्यथा
	    emsg(_(e_current_location_list_was_changed));
	फर्काऊ QF_ABORT;
    }

    फर्काऊ OK;
}

/*
 * Edit a selected file from the quickfix/location list and jump to a
 * particular line/column, adjust the folds and display a message about the
 * jump.
 * Returns OK on success and FAIL on failing to open the file/buffer.  Returns
 * QF_ABORT if the quickfix/location list is freed by an autocmd when opening
 * the file.
 */
    स्थिर पूर्णाङ्क
qf_jump_to_buffer(
	qf_info_T	*qi,
	पूर्णाङ्क		qf_index,
	qfline_T	*qf_ptr,
	पूर्णाङ्क		forceit,
	पूर्णाङ्क		prev_winid,
	पूर्णाङ्क		*opened_window,
	पूर्णाङ्क		openfold,
	पूर्णाङ्क		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    पूर्णाङ्क		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    यदि (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	यदि (retval != OK)
	    फर्काऊ retval;
    }

    // When not switched to another buffer, still need to set pc mark
    यदि (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_पङ्क्ति(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

# ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
# endif
    यदि (print_message)
	qf_jump_print_सन्देश(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    फर्काऊ retval;
}

/*
 * Jump to a quickfix line and try to use an existing window.
 */
    शून्य
qf_jump(qf_info_T	*qi,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		errornr,
	पूर्णाङ्क		forceit)
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
    स्थिर शून्य
qf_jump_newwin(qf_info_T	*qi,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		errornr,
	पूर्णाङ्क		forceit,
	पूर्णाङ्क		newwin)
{
    qf_list_T		*qfl;
    qfline_T		*qf_ptr;
    qfline_T		*old_qf_ptr;
    पूर्णाङ्क			qf_index;
    पूर्णाङ्क			old_qf_index;
    char_u		*old_swb = p_swb;
    अचिन्हित		old_swb_flags = swb_flags;
    पूर्णाङ्क			prev_winid;
    पूर्णाङ्क			opened_window = FALSE;
    पूर्णाङ्क			print_message = TRUE;
    पूर्णाङ्क			old_साँचो_Typed = साँचो_Typed; // getting file may reset it
    पूर्णाङ्क			retval = OK;

    यदि (qi == रिक्त)
    {
	यदि (ql_info == रिक्त)
	{
	    emsg(_(e_no_quickfix_थाक));
	    फर्काऊ;
	}
	qi = ql_info;
    }

    यदि (qf_थाक_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	फर्काऊ;
    }

    incr_quickfix_busy();

    qfl = qf_get_curlist(qi);

    qf_ptr = qfl->qf_ptr;
    old_qf_ptr = qf_ptr;
    qf_index = qfl->qf_index;
    old_qf_index = qf_index;

    qf_ptr = qf_get_entry(qfl, errornr, dir, &qf_index);
    यदि (qf_ptr == रिक्त)
    {
	qf_ptr = old_qf_ptr;
	qf_index = old_qf_index;
	गच्छ theend;
    }

    qfl->qf_index = qf_index;
    qfl->qf_ptr = qf_ptr;
    यदि (qf_win_pos_update(qi, old_qf_index))
	// No need to print the error message if it's visible in the error
	// window
	print_message = FALSE;

    prev_winid = curwin->w_id;

    retval = qf_jump_open_window(qi, qf_ptr, newwin, &opened_window);
    यदि (retval == FAIL)
	गच्छ failed;
    यदि (retval == QF_ABORT)
    {
	qi = रिक्त;
	qf_ptr = रिक्त;
	गच्छ theend;
    }
    यदि (retval == NOTDONE)
	गच्छ theend;

    retval = qf_jump_to_buffer(qi, qf_index, qf_ptr, forceit, prev_winid,
				  &opened_window, old_साँचो_Typed, print_message);
    यदि (retval == QF_ABORT)
    {
	// Quickfix/location list was modified by an autocmd
	qi = रिक्त;
	qf_ptr = रिक्त;
    }

    यदि (retval != OK)
    {
	यदि (opened_window)
	    win_close(curwin, TRUE);    // Close opened window
	यदि (qf_ptr != रिक्त && qf_ptr->qf_fnum != 0)
	{
	    // Couldn't open file, so put index back where it was.  This could
	    // happen if the file was readonly and we changed something.
failed:
	    qf_ptr = old_qf_ptr;
	    qf_index = old_qf_index;
	}
    }
theend:
    यदि (qi != रिक्त)
    {
	qfl->qf_ptr = qf_ptr;
	qfl->qf_index = qf_index;
    }
    यदि (p_swb != old_swb && p_swb == empty_option)
    {
	// Restore old 'switchbuf' value, but not when an autocommand or
	// modeline has changed the value.
	p_swb = old_swb;
	swb_flags = old_swb_flags;
    }
    decr_quickfix_busy();
}

// Highlight attributes used for displaying entries from the quickfix list.
स्थिर पूर्णाङ्क	qfFileAttr;
स्थिर पूर्णाङ्क	qfSepAttr;
स्थिर पूर्णाङ्क	qf_पङ्क्ति_Attr;

/*
 * Display information about a single entry from the quickfix/location list.
 * Used by ":clist/:llist" commands.
 * 'cursel' will be set to TRUE for the currently selected entry in the
 * quickfix list.
 */
    स्थिर शून्य
qf_list_entry(qfline_T *qfp, पूर्णाङ्क qf_idx, पूर्णाङ्क cursel)
{
    char_u	*fname;
    buf_T	*buf;
    पूर्णाङ्क		filter_entry;
    garray_T	*gap;

    fname = रिक्त;
    यदि (qfp->qf_module != रिक्त && *qfp->qf_module != NUL)
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "%2d %s", qf_idx,
						(वर्ण *)qfp->qf_module);
    अन्यथा
    {
	यदि (qfp->qf_fnum != 0
		&& (buf = buflist_findnr(qfp->qf_fnum)) != रिक्त)
	{
	    यदि (qfp->qf_fname == रिक्त)
		fname = buf->b_fname;
	    अन्यथा
		fname = qfp->qf_fname;
	    यदि (qfp->qf_type == 1)	// :helpgrep
		fname = gettail(fname);
	}
	यदि (fname == रिक्त)
	    प्र_लिखो((वर्ण *)IObuff, "%2d", qf_idx);
	अन्यथा
	    vim_snprintf((वर्ण *)IObuff, IOSIZE, "%2d %s",
		    qf_idx, (वर्ण *)fname);
    }

    // Support for filtering entries using :filter /pat/ clist
    // Match against the module name, file name, search pattern and
    // text of the entry.
    filter_entry = TRUE;
    यदि (qfp->qf_module != रिक्त && *qfp->qf_module != NUL)
	filter_entry &= message_filtered(qfp->qf_module);
    यदि (filter_entry && fname != रिक्त)
	filter_entry &= message_filtered(fname);
    यदि (filter_entry && qfp->qf_pattern != रिक्त)
	filter_entry &= message_filtered(qfp->qf_pattern);
    यदि (filter_entry)
	filter_entry &= message_filtered(qfp->qf_text);
    यदि (filter_entry)
	फर्काऊ;

    सन्देश_putchar('\n');
    सन्देश_outtrans_attr(IObuff, cursel ? HL_ATTR(HLF_QFL) : qfFileAttr);

    यदि (qfp->qf_lnum != 0)
	सन्देश_puts_attr(":", qfSepAttr);
    gap = qfga_get();
    यदि (qfp->qf_lnum != 0)
	qf_range_text(gap, qfp);
    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
    ga_append(gap, NUL);
    सन्देश_puts_attr((वर्ण *)gap->ga_data, qf_पङ्क्ति_Attr);
    सन्देश_puts_attr(":", qfSepAttr);
    यदि (qfp->qf_pattern != रिक्त)
    {
	gap = qfga_get();
	qf_fmt_text(gap, qfp->qf_pattern);
	ga_append(gap, NUL);
	सन्देश_puts((वर्ण *)gap->ga_data);
	सन्देश_puts_attr(":", qfSepAttr);
    }
    सन्देश_puts(" ");

    // Remove newlines and leading whitespace from the text.  For an
    // unrecognized line keep the indent, the compiler may mark a word
    // with ^^^^.
    gap = qfga_get();
    qf_fmt_text(gap, (fname != रिक्त || qfp->qf_lnum != 0)
				     ? skipwhite(qfp->qf_text) : qfp->qf_text);
    ga_append(gap, NUL);
    सन्देश_prt_पङ्क्ति((char_u *)gap->ga_data, FALSE);
    out_flush();		// show one line at a time
}

/*
 * ":clist": list all errors
 * ":llist": list all locations
 */
    शून्य
qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		idx1 = 1;
    पूर्णाङ्क		idx2 = -1;
    char_u	*arg = eap->arg;
    पूर्णाङ्क		plus = FALSE;
    पूर्णाङ्क		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    यदि (qf_थाक_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	फर्काऊ;
    }
    यदि (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    यदि (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	फर्काऊ;
    }
    qfl = qf_get_curlist(qi);
    यदि (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    अन्यथा
    {
	i = qfl->qf_गन्ती;
	यदि (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	यदि (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    यदि (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    यदि (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qf_पङ्क्ति_Attr = syn_name2attr((char_u *)"qfLineNr");
    यदि (qf_पङ्क्ति_Attr == 0)
	qf_पङ्क्ति_Attr = HL_ATTR(HLF_N);

    यदि (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
    {
	यदि ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
    qfga_clear();
}

/*
 * Remove newlines and leading whitespace from an error message.
 * Add the result to the grow array "gap".
 */
    स्थिर शून्य
qf_fmt_text(garray_T *gap, char_u *text)
{
    char_u	*p = text;
    जबसम्म (*p != NUL)
    {
	यदि (*p == '\n')
	{
	    ga_append(gap, ' ');
	    जबसम्म (*++p != NUL)
		यदि (!VIM_ISWHITE(*p) && *p != '\n')
		    रोक;
	}
	अन्यथा
	    ga_append(gap, *p++);
    }
}

/*
 * Add the range information from the lnum, col, end_lnum, and end_col values
 * of a quickfix entry to the grow array "gap".
 */
    स्थिर शून्य
qf_range_text(garray_T *gap, qfline_T *qfp)
{
    string_T	buf = {IObuff, 0};

    buf.length = vim_snprintf_safelen((वर्ण *)buf.string, IOSIZE, "%ld", qfp->qf_lnum);

    यदि (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	buf.length += vim_snprintf_safelen((वर्ण *)buf.string + buf.length,
	    IOSIZE - buf.length, "-%ld", qfp->qf_end_lnum);
    }
    यदि (qfp->qf_col > 0)
    {
	buf.length += vim_snprintf_safelen((वर्ण *)buf.string + buf.length,
	    IOSIZE - buf.length, " col %d", qfp->qf_col);
	यदि (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	    buf.length += vim_snprintf_safelen((वर्ण *)buf.string + buf.length,
		IOSIZE - buf.length, "-%d", qfp->qf_end_col);
    }

    ga_concat_len(gap, buf.string, buf.length);
}

/*
 * Display information (list number, list size and the title) about a
 * quickfix/location list.
 */
    स्थिर शून्य
qf_सन्देश(qf_info_T *qi, पूर्णाङ्क which, वर्ण *lead)
{
    वर्ण   *title = (वर्ण *)qi->qf_lists[which].qf_title;
    पूर्णाङ्क    गन्ती = qi->qf_lists[which].qf_गन्ती;
    char_u buf[IOSIZE];

    vim_snprintf((वर्ण *)buf, IOSIZE, _("%serror list %d of %d; %d errors "),
	    lead,
	    which + 1,
	    qi->qf_listcount,
	    गन्ती);

    यदि (title != रिक्त)
    {
	माप_प्रकार	len = STRLEN(buf);

	यदि (len < 34)
	{
	    vim_memset(buf + len, ' ', 34 - len);
	    buf[34] = NUL;
	}
	vim_strcat(buf, (char_u *)title, IOSIZE);
    }
    trunc_string(buf, buf, Columns - 1, IOSIZE);
    सन्देश((वर्ण *)buf);
}

/*
 * ":colder [count]": Up in the quickfix stack.
 * ":cnewer [count]": Down in the quickfix stack.
 * ":lolder [count]": Up in the location list stack.
 * ":lnewer [count]": Down in the location list stack.
 */
    शून्य
qf_age(exarg_T *eap)
{
    qf_info_T	*qi;
    पूर्णाङ्क		गन्ती;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    यदि (eap->addr_गन्ती != 0)
	गन्ती = eap->line2;
    अन्यथा
	गन्ती = 1;
    जबसम्म (गन्ती--)
    {
	यदि (eap->cmdidx == CMD_colder || eap->cmdidx == CMD_lolder)
	{
	    यदि (qi->qf_curlist == 0)
	    {
		emsg(_(e_at_bottom_of_quickfix_थाक));
		रोक;
	    }
	    --qi->qf_curlist;
	}
	अन्यथा
	{
	    यदि (qi->qf_curlist >= qi->qf_listcount - 1)
	    {
		emsg(_(e_at_शीर्ष_of_quickfix_थाक));
		रोक;
	    }
	    ++qi->qf_curlist;
	}
    }
    qf_सन्देश(qi, qi->qf_curlist, "");
    qf_update_buffer(qi, रिक्त);
}

/*
 * Display the information about all the quickfix/location lists in the stack
 */
    शून्य
qf_history(exarg_T *eap)
{
    qf_info_T	*qi = qf_cmd_get_थाक(eap, FALSE);
    पूर्णाङ्क		i;

    यदि (eap->addr_गन्ती > 0)
    {
	यदि (qi == रिक्त)
	{
	    emsg(_(e_no_location_list));
	    फर्काऊ;
	}

	// Jump to the specified quickfix list
	यदि (eap->line2 > 0 && eap->line2 <= qi->qf_listcount)
	{
	    qi->qf_curlist = eap->line2 - 1;
	    qf_सन्देश(qi, qi->qf_curlist, "");
	    qf_update_buffer(qi, रिक्त);
	}
	अन्यथा
	    emsg(_(e_invalid_range));

	फर्काऊ;
    }

    यदि (qf_थाक_empty(qi))
	सन्देश(_("No entries"));
    अन्यथा
	लागि (i = 0; i < qi->qf_listcount; ++i)
	    qf_सन्देश(qi, i, i == qi->qf_curlist ? "> " : "  ");
}

/*
 * Free all the entries in the error list "idx". Note that other information
 * associated with the list like context and title are not freed.
 */
    स्थिर शून्य
qf_free_वस्तुहरू(qf_list_T *qfl)
{
    qfline_T	*qfp;
    qfline_T	*qfpnext;
    पूर्णाङ्क		stop = FALSE;

    जबसम्म (qfl->qf_गन्ती && qfl->qf_start != रिक्त)
    {
	qfp = qfl->qf_start;
	qfpnext = qfp->qf_next;
	यदि (!stop)
	{
	    vim_free(qfp->qf_fname);
	    vim_free(qfp->qf_module);
	    vim_free(qfp->qf_text);
	    vim_free(qfp->qf_pattern);
	    clear_tv(&qfp->qf_user_data);
	    stop = (qfp == qfpnext);
	    vim_free(qfp);
	    यदि (stop)
		// Somehow qf_count may have an incorrect value, set it to 1
		// to avoid crashing when it's wrong.
		// TODO: Avoid qf_count being incorrect.
		qfl->qf_गन्ती = 1;
	    अन्यथा
		qfl->qf_start = qfpnext;
	}
	--qfl->qf_गन्ती;
    }

    qfl->qf_index = 0;
    qfl->qf_start = रिक्त;
    qfl->qf_last = रिक्त;
    qfl->qf_ptr = रिक्त;
    qfl->qf_nonevalid = TRUE;

    qf_clean_dir_थाक(&qfl->qf_dir_थाक);
    qfl->qf_directory = रिक्त;
    qf_clean_dir_थाक(&qfl->qf_file_थाक);
    qfl->qf_currfile = रिक्त;
    qfl->qf_multiline = FALSE;
    qfl->qf_multiignore = FALSE;
    qfl->qf_multiscan = FALSE;
}

/*
 * Free error list "idx". Frees all the entries in the quickfix list,
 * associated context information and the title.
 */
    स्थिर शून्य
qf_free(qf_list_T *qfl)
{
    qf_free_वस्तुहरू(qfl);

    VIM_CLEAR(qfl->qf_title);
    free_tv(qfl->qf_ctx);
    qfl->qf_ctx = रिक्त;
    free_callback(&qfl->qf_qftf_cb);
    qfl->qf_id = 0;
    qfl->qf_changedtick = 0L;
}

/*
 * qf_mark_adjust: adjust marks
 */
   शून्य
qf_mark_adjust(
	win_T	*wp,
	linenr_T	line1,
	linenr_T	line2,
	दीर्घ	amount,
	दीर्घ	amount_after)
{
    पूर्णाङ्क		i;
    qfline_T	*qfp;
    पूर्णाङ्क		idx;
    qf_info_T	*qi = ql_info;
    पूर्णाङ्क		found_one = FALSE;
    पूर्णाङ्क		buf_has_flag = wp == रिक्त ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;

    यदि (!(curbuf->b_has_qf_entry & buf_has_flag))
	फर्काऊ;
    यदि (wp != रिक्त)
    {
	यदि (wp->w_llist == रिक्त)
	    फर्काऊ;
	qi = wp->w_llist;
    }
    अन्यथा यदि (qi == रिक्त)
	फर्काऊ;

    लागि (idx = 0; idx < qi->qf_listcount; ++idx)
    {
	qf_list_T	*qfl = qf_get_list(qi, idx);

	यदि (!qf_list_empty(qfl))
	    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
		यदि (qfp->qf_fnum == curbuf->b_fnum)
		{
		    found_one = TRUE;
		    यदि (qfp->qf_cleared)
			जारी;

		    यदि (qfp->qf_lnum >= line1 && qfp->qf_lnum <= line2)
		    {
			यदि (amount == MAXLNUM)
			    qfp->qf_cleared = TRUE;
			अन्यथा
			    qfp->qf_lnum += amount;
		    }
		    अन्यथा यदि (amount_after && qfp->qf_lnum > line2)
			qfp->qf_lnum += amount_after;
		}
    }

    यदि (!found_one)
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
    स्थिर char_u *
qf_types(पूर्णाङ्क c, पूर्णाङ्क nr)
{
    स्थिर char_u	buf[20];
    स्थिर char_u	cc[3];
    char_u		*p;

    यदि (c == 'W' || c == 'w')
	p = (char_u *)" warning";
    अन्यथा यदि (c == 'I' || c == 'i')
	p = (char_u *)" info";
    अन्यथा यदि (c == 'N' || c == 'n')
	p = (char_u *)" note";
    अन्यथा यदि (c == 'E' || c == 'e' || (c == 0 && nr > 0))
	p = (char_u *)" error";
    अन्यथा यदि (c == 0 || c == 1)
	p = (char_u *)"";
    अन्यथा
    {
	cc[0] = ' ';
	cc[1] = c;
	cc[2] = NUL;
	p = cc;
    }

    यदि (nr <= 0)
	फर्काऊ p;

    प्र_लिखो((वर्ण *)buf, "%s %3d", (वर्ण *)p, nr);
    फर्काऊ buf;
}

/*
 * When "split" is FALSE: Open the entry/result under the cursor.
 * When "split" is TRUE: Open the entry/result under the cursor in a new window.
 */
    शून्य
qf_view_result(पूर्णाङ्क split)
{
    qf_info_T   *qi = ql_info;

    यदि (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);
    अन्यथा यदि (qi == रिक्त)
    {
	emsg(_(e_no_quickfix_थाक));
	फर्काऊ;
    }

    यदि (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	फर्काऊ;
    }

    यदि (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (दीर्घ)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	फर्काऊ;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}

/*
 * ":cwindow": open the quickfix window if we have errors to display,
 *	       close it if not.
 * ":lwindow": open the location list window if we have locations to display,
 *	       close it if not.
 */
    शून्य
ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    यदि (qf_थाक_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	यदि (win != रिक्त)
	    ex_cclose(eap);
    }
    अन्यथा यदि (win == रिक्त)
	ex_copen(eap);
}

/*
 * ":cclose": close the window showing the list of errors.
 * ":lclose": close the window showing the location list
 */
    शून्य
ex_cclose(exarg_T *eap)
{
    win_T	*win = रिक्त;
    qf_info_T	*qi;

    यदि ((qi = qf_cmd_get_थाक(eap, FALSE)) == रिक्त)
	फर्काऊ;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    यदि (win != रिक्त)
	win_close(win, FALSE);
}

/*
 * Set "w:quickfix_title" if "qi" has a title.
 */
    स्थिर शून्य
qf_set_title_var(qf_list_T *qfl)
{
    यदि (qfl->qf_title != रिक्त)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}

/*
 * Goto a quickfix or location list window (if present).
 * Returns OK if the window is found, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
qf_goto_cwindow(qf_info_T *qi, पूर्णाङ्क resize, पूर्णाङ्क sz, पूर्णाङ्क vertsplit)
{
    win_T	*win;

    win = qf_find_win(qi);
    यदि (win == रिक्त)
	फर्काऊ FAIL;

    win_goto(win);
    यदि (resize)
    {
	यदि (vertsplit)
	{
	    यदि (sz != win->w_width)
		win_setwidth(sz);
	}
	अन्यथा यदि (sz != win->w_height && win->w_height
		       + win->w_status_height + tabline_height() < cmdline_row)
	    win_setheight(sz);
    }

    फर्काऊ OK;
}

/*
 * Set options for the buffer in the quickfix or location list window.
 */
    स्थिर शून्य
qf_set_cwindow_options(शून्य)
{
    // switch off 'swapfile'
    set_option_मान_give_err((char_u *)"swf", 0L, रिक्त, OPT_LOCAL);
    set_option_मान_give_err((char_u *)"bt",
					  0L, (char_u *)"quickfix", OPT_LOCAL);
    set_option_मान_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
    RESET_BINDING(curwin);
# ifdef FEAT_DIFF
    curwin->w_p_diff = FALSE;
# endif
# ifdef FEAT_FOLDING
    set_option_मान_give_err((char_u *)"fdm", 0L, (char_u *)"manual",
	    OPT_LOCAL);
# endif
}

/*
 * Open a new quickfix or location list window, load the quickfix buffer and
 * set the appropriate options for the window.
 * Returns FAIL if the window could not be opened.
 */
    स्थिर पूर्णाङ्क
qf_open_new_cwindow(qf_info_T *qi, पूर्णाङ्क height)
{
    buf_T	*qf_buf;
    win_T	*oldwin = curwin;
    tabpage_T	*prevtab = curtab;
    पूर्णाङ्क		flags = 0;
    win_T	*win;

    qf_buf = qf_find_buf(qi);

    // The current window becomes the previous window afterwards.
    win = curwin;

    यदि (IS_QF_थाक(qi) && cmdmod.cmod_split == 0)
	// Create the new quickfix window at the very bottom, except when
	// :belowright or :aboveleft is used.
	win_goto(lastwin);
    // Default is to open the window below the current window
    यदि (cmdmod.cmod_split == 0)
	flags = WSP_BELOW;
    flags |= WSP_NEWLOC;

    // Create a snapshot for quickfix window (not for location list)
    // so that when closing it, we can restore to the previous window
    यदि (IS_QF_थाक(qi))
	flags |= WSP_QUICKFIX;

    यदि (win_split(height, flags) == FAIL)
	फर्काऊ FAIL;		// not enough room for window
    RESET_BINDING(curwin);

    यदि (IS_LL_थाक(qi))
    {
	// For the location list window, create a reference to the
	// location list stack from the window 'win'.
	curwin->w_llist_ref = qi;
	qi->qf_refcount++;
    }

    यदि (oldwin != curwin)
	oldwin = रिक्त;  // don't store info when in another window
    यदि (qf_buf != रिक्त)
    {
	// Use the existing quickfix buffer
	यदि (do_ecmd(qf_buf->b_fnum, रिक्त, रिक्त, रिक्त, ECMD_ONE,
		    ECMD_HIDE + ECMD_OLDBUF + ECMD_NOWINENTER, oldwin) == FAIL)
	    फर्काऊ FAIL;
    }
    अन्यथा
    {
	// Create a new quickfix buffer
	यदि (do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE, ECMD_HIDE + ECMD_NOWINENTER,
							       oldwin) == FAIL)
	    फर्काऊ FAIL;

	// save the number of the new buffer
	qi->qf_bufnr = curbuf->b_fnum;
    }

    // Set the options for the quickfix buffer/window (if not already done)
    // Do this even if the quickfix buffer was already present, as an autocmd
    // might have previously deleted (:bdelete) the quickfix buffer.
    यदि (!bt_quickfix(curbuf))
	qf_set_cwindow_options();

    // Only set the height when still in the same tab page and there is no
    // window to the side.
    यदि (curtab == prevtab && curwin->w_width == Columns)
	win_setheight(height);
    curwin->w_p_wfh = TRUE;	    // set 'winfixheight'
    यदि (win_valid(win))
	prevwin = win;

    फर्काऊ OK;
}

/*
 * ":copen": open a window that shows the list of errors.
 * ":lopen": open a window that shows the location list.
 */
    शून्य
ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    पूर्णाङ्क		height;
    पूर्णाङ्क		status = FAIL;
    पूर्णाङ्क		lnum;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    incr_quickfix_busy();

    यदि (eap->addr_गन्ती != 0)
	height = eap->line2;
    अन्यथा
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
# ifdef FEAT_GUI
    need_mouse_correct = TRUE;
# endif

    // Find an existing quickfix window, or open a new one.
    यदि (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_गन्ती != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    यदि (status == FAIL)
	यदि (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    फर्काऊ;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, रिक्त, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}

/*
 * Move the cursor in the quickfix window to "lnum".
 */
    स्थिर शून्य
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
    शून्य
ex_cbottom(exarg_T *eap)
{
    qf_info_T	*qi;
    win_T	*win;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    win = qf_find_win(qi);
    यदि (win != रिक्त && win->w_cursor.lnum != win->w_buffer->b_ml.ml_पङ्क्ति_गन्ती)
	qf_win_goto(win, win->w_buffer->b_ml.ml_पङ्क्ति_गन्ती);
}

/*
 * Return the number of the current entry (line number in the quickfix
 * window).
 */
    linenr_T
qf_current_entry(win_T *wp)
{
    qf_info_T	*qi = ql_info;

    यदि (IS_LL_WINDOW(wp))
	// In the location list window, use the referenced location list
	qi = wp->w_llist_ref;
    अन्यथा यदि (qi == रिक्त)
	फर्काऊ 0;

    फर्काऊ qf_get_curlist(qi)->qf_index;
}

/*
 * Update the cursor position in the quickfix window to the current error.
 * Return TRUE if there is a quickfix window.
 */
    स्थिर पूर्णाङ्क
qf_win_pos_update(
    qf_info_T	*qi,
    पूर्णाङ्क		old_qf_index)	// previous qf_index or zero
{
    win_T	*win;
    पूर्णाङ्क		qf_index = qf_get_curlist(qi)->qf_index;

    // Put the cursor on the current error in the quickfix window, so that
    // it's viewable.
    win = qf_find_win(qi);
    यदि (win != रिक्त
	    && qf_index <= win->w_buffer->b_ml.ml_पङ्क्ति_गन्ती
	    && old_qf_index != qf_index)
    {
	यदि (qf_index > old_qf_index)
	{
	    win->w_redraw_शीर्ष = old_qf_index;
	    win->w_redraw_bot = qf_index;
	}
	अन्यथा
	{
	    win->w_redraw_शीर्ष = qf_index;
	    win->w_redraw_bot = old_qf_index;
	}
	qf_win_goto(win, qf_index);
    }
    फर्काऊ win != रिक्त;
}

/*
 * Check whether the given window is displaying the specified quickfix/location
 * stack.
 */
    स्थिर पूर्णाङ्क
is_qf_win(win_T *win, qf_info_T *qi)
{
    // A window displaying the quickfix buffer will have the w_llist_ref field
    // set to NULL.
    // A window displaying a location list buffer will have the w_llist_ref
    // pointing to the location list.
    यदि (buf_valid(win->w_buffer) && bt_quickfix(win->w_buffer))
	यदि ((IS_QF_थाक(qi) && win->w_llist_ref == रिक्त)
		|| (IS_LL_थाक(qi) && win->w_llist_ref == qi))
	    फर्काऊ TRUE;

    फर्काऊ FALSE;
}

/*
 * Find a window displaying the quickfix/location stack 'qi' in the current tab
 * page.
 */
    स्थिर win_T *
qf_find_win(qf_info_T *qi)
{
    win_T	*win;

    FOR_ALL_WINDOWS(win)
	यदि (is_qf_win(win, qi))
	    फर्काऊ win;
    फर्काऊ रिक्त;
}

/*
 * Find a quickfix buffer.
 * Searches in windows opened in all the tab pages.
 */
    स्थिर buf_T *
qf_find_buf(qf_info_T *qi)
{
    tabpage_T	*tp;
    win_T	*win;

    यदि (qi->qf_bufnr != INVALID_QFBUFNR)
    {
	buf_T	*qfbuf;
	qfbuf = buflist_findnr(qi->qf_bufnr);
	यदि (qfbuf != रिक्त)
	    फर्काऊ qfbuf;
	// buffer is no longer present
	qi->qf_bufnr = INVALID_QFBUFNR;
    }

    FOR_ALL_TAB_WINDOWS(tp, win)
	यदि (is_qf_win(win, qi))
	    फर्काऊ win->w_buffer;

    फर्काऊ रिक्त;
}

/*
 * Process the 'quickfixtextfunc' option value.
 * Returns OK or FAIL.
 */
    वर्ण *
did_set_quickfixtextfunc(optset_T *args UNUSED)
{
    यदि (option_set_callback_func(p_qftf, &qftf_cb) == FAIL)
	फर्काऊ e_invalid_argument;

    फर्काऊ रिक्त;
}

/*
 * Update the w:quickfix_title variable in the quickfix/location list window in
 * all the tab pages.
 */
    स्थिर शून्य
qf_update_win_titlevar(qf_info_T *qi)
{
    qf_list_T	*qfl = qf_get_curlist(qi);
    tabpage_T	*tp;
    win_T	*win;
    win_T	*save_curwin = curwin;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	यदि (is_qf_win(win, qi))
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
    स्थिर शून्य
qf_update_buffer(qf_info_T *qi, qfline_T *old_last)
{
    buf_T	*buf;
    win_T	*win;
    aco_save_T	aco;

    // Check if a buffer for the quickfix list exists.  Update it.
    buf = qf_find_buf(qi);
    यदि (buf == रिक्त)
	फर्काऊ;

    linenr_T	old_पङ्क्ति_गन्ती = buf->b_ml.ml_पङ्क्ति_गन्ती;
    पूर्णाङ्क		qf_winid = 0;

    यदि (IS_LL_थाक(qi))
    {
	यदि (curwin->w_llist == qi)
	    win = curwin;
	अन्यथा
	{
	    // Find the file window (non-quickfix) with this location list
	    win = qf_find_win_with_loclist(qi);
	    यदि (win == रिक्त)
		// File window is not found. Find the location list window.
		win = qf_find_win(qi);
	    यदि (win == रिक्त)
		फर्काऊ;
	}
	qf_winid = win->w_id;
    }

    // autocommands may cause trouble
    incr_quickfix_busy();

    पूर्णाङ्क do_fill = TRUE;
    यदि (old_last == रिक्त)
    {
	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, buf);
	यदि (curbuf != buf)
	    do_fill = FALSE;  // failed to find a window for "buf"
    }

    यदि (do_fill)
    {
	qf_update_win_titlevar(qi);

	qf_fill_buffer(qf_get_curlist(qi), buf, old_last, qf_winid);
	++CHANGEDTICK(buf);

	यदि (old_last == रिक्त)
	{
	    (शून्य)qf_win_pos_update(qi, 0);

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    // Only redraw when added lines are visible.  This avoids flickering
    // when the added lines are not visible.
    यदि ((win = qf_find_win(qi)) != रिक्त && old_पङ्क्ति_गन्ती < win->w_botline)
	redraw_buf_later(buf, UPD_NOT_VALID);

    // always called after incr_quickfix_busy()
    decr_quickfix_busy();
}

/*
 * Add an error line to the quickfix buffer.
 */
    स्थिर पूर्णाङ्क
qf_buf_add_पङ्क्ति(
	buf_T		*buf,		// quickfix window buffer
	linenr_T	lnum,
	qfline_T	*qfp,
	char_u		*स_नाम,
	पूर्णाङ्क		first_bufline,
	char_u		*qftf_str)
{
    buf_T	*errbuf;
    garray_T	*gap;

    gap = qfga_get();

    // If the 'quickfixtextfunc' function returned a non-empty custom string
    // for this entry, then use it.
    यदि (qftf_str != रिक्त && *qftf_str != NUL)
    {
	ga_concat(gap, qftf_str);
    }
    अन्यथा
    {
	यदि (qfp->qf_module != रिक्त)
	    ga_concat(gap, qfp->qf_module);
	अन्यथा यदि (qfp->qf_fnum != 0
		&& (errbuf = buflist_findnr(qfp->qf_fnum)) != रिक्त
		&& errbuf->b_fname != रिक्त)
	{
	    यदि (qfp->qf_type == 1)	// :helpgrep
		ga_concat(gap, gettail(errbuf->b_fname));
	    अन्यथा
	    {
		// Shorten the file name if not done already.
		// For optimization, do this only for the first entry in a
		// buffer.
		यदि (first_bufline && (errbuf->b_sfname == रिक्त
				|| mch_isFullName(errbuf->b_sfname)))
		{
		    यदि (*स_नाम == NUL)
			mch_dirname(स_नाम, MAXPATHL);
		    shorten_buf_fname(errbuf, स_नाम, FALSE);
		}
		यदि (qfp->qf_fname == रिक्त)
		    ga_concat(gap, errbuf->b_fname);
		अन्यथा
		    ga_concat(gap, qfp->qf_fname);
	    }
	}

	ga_append(gap, '|');

	यदि (qfp->qf_lnum > 0)
	{
	    qf_range_text(gap, qfp);
	    ga_concat(gap, qf_types(qfp->qf_type, qfp->qf_nr));
	}
	अन्यथा यदि (qfp->qf_pattern != रिक्त)
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
    यदि (ml_append_buf(buf, lnum, gap->ga_data, gap->ga_len, FALSE) == FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * Call the 'quickfixtextfunc' function to get the list of lines to display in
 * the quickfix window for the entries 'start_idx' to 'end_idx'.
 */
    स्थिर list_T *
call_qftf_func(qf_list_T *qfl, पूर्णाङ्क qf_winid, दीर्घ start_idx, दीर्घ end_idx)
{
    callback_T	*cb = &qftf_cb;
    list_T	*qftf_list = रिक्त;
    स्थिर पूर्णाङ्क	recursive = FALSE;

    यदि (recursive)
	फर्काऊ रिक्त;  // this doesn't work properly recursively
    recursive = TRUE;

    // If 'quickfixtextfunc' is set, then use the user-supplied function to get
    // the text to display. Use the local value of 'quickfixtextfunc' if it is
    // set.
    यदि (qfl->qf_qftf_cb.cb_name != रिक्त)
	cb = &qfl->qf_qftf_cb;
    यदि (cb->cb_name != रिक्त)
    {
	typval_T	args[1];
	dict_T		*d;
	typval_T	rettv;

	// create the dict argument
	यदि ((d = dict_alloc_lock(VAR_FIXED)) == रिक्त)
	{
	    recursive = FALSE;
	    फर्काऊ रिक्त;
	}
	dict_add_सङ्ख्या(d, "quickfix", (दीर्घ)IS_QF_LIST(qfl));
	dict_add_सङ्ख्या(d, "winid", (दीर्घ)qf_winid);
	dict_add_सङ्ख्या(d, "id", (दीर्घ)qfl->qf_id);
	dict_add_सङ्ख्या(d, "start_idx", start_idx);
	dict_add_सङ्ख्या(d, "end_idx", end_idx);
	++d->dv_refcount;
	args[0].v_type = VAR_DICT;
	args[0].vval.v_dict = d;

	qftf_list = रिक्त;
	textlock++;
	यदि (call_callback(cb, 0, &rettv, 1, args) != FAIL)
	{
	    यदि (rettv.v_type == VAR_LIST)
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
    फर्काऊ qftf_list;
}

/*
 * Fill current buffer with quickfix errors, replacing any previous contents.
 * curbuf must be the quickfix buffer!
 * If "old_last" is not NULL append the items after this one.
 * When "old_last" is NULL then "buf" must equal "curbuf"!  Because
 * ml_delete() is used and autocommands will be triggered.
 */
    स्थिर शून्य
qf_fill_buffer(qf_list_T *qfl, buf_T *buf, qfline_T *old_last, पूर्णाङ्क qf_winid)
{
    linenr_T	lnum;
    qfline_T	*qfp;
    पूर्णाङ्क		old_साँचो_Typed = साँचो_Typed;
    list_T	*qftf_list = रिक्त;
    listitem_T	*qftf_li = रिक्त;

    यदि (old_last == रिक्त)
    {
	win_T		*wp;
	tabpage_T	*tp;

	यदि (buf != curbuf)
	{
	    internal_error("qf_fill_buffer()");
	    फर्काऊ;
	}

	// delete all existing lines
	//
	// Note: we cannot store undo information, because
	// qf buffer is usually not allowed to be modified.
	//
	// So we need to clean up undo information
	// otherwise autocommands may invalidate the undo stack
	जबसम्म ((curbuf->b_ml.ml_flags & ML_EMPTY) == 0)
	    (शून्य)ml_delete((linenr_T)1);

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    यदि (wp->w_buffer == curbuf)
		wp->w_skipcol = 0;

	// Remove all undo information
	u_clearallandblockfree(curbuf);
    }

    // Check if there is anything to display
    यदि (qfl != रिक्त && qfl->qf_start != रिक्त)
    {
	char_u		स_नाम[MAXPATHL];
	पूर्णाङ्क		invalid_val = FALSE;
	पूर्णाङ्क		prev_bufnr = -1;

	*स_नाम = NUL;

	// Add one line for each error
	यदि (old_last == रिक्त)
	{
	    qfp = qfl->qf_start;
	    lnum = 0;
	}
	अन्यथा
	{
	    यदि (old_last->qf_next != रिक्त)
		qfp = old_last->qf_next;
	    अन्यथा
		qfp = old_last;
	    lnum = buf->b_ml.ml_पङ्क्ति_गन्ती;
	}

	qftf_list = call_qftf_func(qfl, qf_winid, (दीर्घ)(lnum + 1),
							(दीर्घ)qfl->qf_गन्ती);
	यदि (qftf_list != रिक्त)
	    qftf_li = qftf_list->lv_first;

	जबसम्म (lnum < qfl->qf_गन्ती)
	{
	    char_u	*qftf_str = रिक्त;

	    // Use the text supplied by the user defined function (if any).
	    // If the returned value is not string, then ignore the rest
	    // of the returned values and use the default.
	    यदि (qftf_li != रिक्त && !invalid_val)
	    {
		qftf_str = tv_get_string_chk(&qftf_li->li_tv);
		यदि (qftf_str == रिक्त)
		    invalid_val = TRUE;
	    }

	    यदि (qf_buf_add_पङ्क्ति(buf, lnum, qfp, स_नाम,
			prev_bufnr != qfp->qf_fnum, qftf_str) == FAIL)
		रोक;

	    prev_bufnr = qfp->qf_fnum;
	    ++lnum;
	    qfp = qfp->qf_next;
	    यदि (qfp == रिक्त)
		रोक;

	    यदि (qftf_li != रिक्त)
		qftf_li = qftf_li->li_next;
	}

	यदि (old_last == रिक्त)
	    // Delete the empty line which is now at the end
	    (शून्य)ml_delete(lnum + 1);

	list_unref(qftf_list);
	qfga_clear();
    }

    // correct cursor position
    check_lnums(TRUE);

    यदि (old_last == रिक्त)
    {
	// Set the 'filetype' to "qf" each time after filling the buffer.
	// This resembles reading a file into a buffer, it's more logical when
	// using autocommands.
	++curbuf_lock;
	set_option_मान_give_err((char_u *)"ft",
						0L, (char_u *)"qf", OPT_LOCAL);
	curbuf->b_p_ma = FALSE;

	curbuf->b_keep_filetype = true;	// don't detect 'filetype'
	apply_autocmds(EVENT_BUFREADPOST, (char_u *)"quickfix", रिक्त,
							       FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, (char_u *)"quickfix", रिक्त,
							       FALSE, curbuf);
	curbuf->b_keep_filetype = false;
	--curbuf_lock;

	// make sure it will be redrawn
	redraw_curbuf_later(UPD_NOT_VALID);
    }

    // Restore KeyTyped, setting 'filetype' may reset it.
    साँचो_Typed = old_साँचो_Typed;
}

/*
 * For every change made to the quickfix list, update the changed tick.
 */
    स्थिर शून्य
qf_list_changed(qf_list_T *qfl)
{
    qfl->qf_changedtick++;
}

/*
 * Return the quickfix/location list number with the given identifier.
 * Returns -1 if list is not found.
 */
    स्थिर पूर्णाङ्क
qf_id2nr(qf_info_T *qi, int_u qfid)
{
    पूर्णाङ्क		qf_idx;

    लागि (qf_idx = 0; qf_idx < qi->qf_listcount; qf_idx++)
	यदि (qi->qf_lists[qf_idx].qf_id == qfid)
	    फर्काऊ qf_idx;
    फर्काऊ INVALID_QFIDX;
}

/*
 * If the current list is not "save_qfid" and we can find the list with that ID
 * then make it the current list.
 * This is used when autocommands may have changed the current list.
 * Returns OK if successfully restored the list. Returns FAIL if the list with
 * the specified identifier (save_qfid) is not found in the stack.
 */
    स्थिर पूर्णाङ्क
qf_restore_list(qf_info_T *qi, int_u save_qfid)
{
    पूर्णाङ्क curlist;

    यदि (qf_get_curlist(qi)->qf_id == save_qfid)
	फर्काऊ OK;

    curlist = qf_id2nr(qi, save_qfid);
    यदि (curlist < 0)
	// list is not present
	फर्काऊ FAIL;
    qi->qf_curlist = curlist;
    फर्काऊ OK;
}

/*
 * Jump to the first entry if there is one.
 */
    स्थिर शून्य
qf_jump_first(qf_info_T *qi, int_u save_qfid, पूर्णाङ्क forceit)
{
    यदि (qf_restore_list(qi, save_qfid) == FAIL)
	फर्काऊ;


    यदि (!check_can_set_curbuf_forceit(forceit))
	फर्काऊ;


    // Autocommands might have cleared the list, check for that.
    यदि (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, forceit);
}

/*
 * Return TRUE when using ":vimgrep" for ":grep".
 */
    पूर्णाङ्क
grep_internal(cmdidx_T cmdidx)
{
    फर्काऊ ((cmdidx == CMD_grep
		|| cmdidx == CMD_lgrep
		|| cmdidx == CMD_grepadd
		|| cmdidx == CMD_lgrepadd)
	    && STRCMP("internal",
			*curbuf->b_p_gp == NUL ? p_gp : curbuf->b_p_gp) == 0);
}

/*
 * Return the make/grep autocmd name.
 */
    स्थिर char_u *
make_get_auname(cmdidx_T cmdidx)
{
    विकल्प (cmdidx)
    {
	पक्ष CMD_make:	    फर्काऊ (char_u *)"make";
	पक्ष CMD_lmake:	    फर्काऊ (char_u *)"lmake";
	पक्ष CMD_grep:	    फर्काऊ (char_u *)"grep";
	पक्ष CMD_lgrep:	    फर्काऊ (char_u *)"lgrep";
	पक्ष CMD_grepadd:   फर्काऊ (char_u *)"grepadd";
	पक्ष CMD_lgrepadd:  फर्काऊ (char_u *)"lgrepadd";
	स्वभाव: फर्काऊ रिक्त;
    }
}

/*
 * Return the name for the errorfile, in allocated memory.
 * Find a new unique name when 'makeef' contains "##".
 * Returns NULL for error.
 */
    स्थिर char_u *
get_mef_name(शून्य)
{
    char_u	*p;
    char_u	*name;
    स्थिर पूर्णाङ्क	start = -1;
    स्थिर पूर्णाङ्क	off = 0;
# ifdef HAVE_LSTAT
    stat_T	sb;
# endif

    यदि (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	यदि (name == रिक्त)
	    emsg(_(e_cant_get_temp_file_name));
	फर्काऊ name;
    }

    लागि (p = p_mef; *p; ++p)
	यदि (p[0] == '#' && p[1] == '#')
	    रोक;

    यदि (*p == NUL)
	फर्काऊ vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    लागि (;;)
    {
	यदि (start == -1)
	    start = mch_get_pid();
	अन्यथा
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	यदि (name == रिक्त)
	    रोक;
	STRCPY(name, p_mef);
	प्र_लिखो((वर्ण *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	यदि (mch_getperm(name) < 0
# ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((वर्ण *)name, &sb) < 0
# endif
		)
	    रोक;
	vim_free(name);
    }
    फर्काऊ name;
}

/*
 * Form the complete command line to invoke 'make'/'grep'. Quote the command
 * using 'shellquote' and append 'shellpipe'. Echo the fully formed command.
 */
    स्थिर char_u *
make_get_fullcmd(char_u *makecmd, char_u *fname)
{
    char_u	*cmd;
    अचिन्हित	len;

    len = (अचिन्हित)STRLEN(p_shq) * 2 + (अचिन्हित)STRLEN(makecmd) + 1;
    यदि (*p_sp != NUL)
	len += (अचिन्हित)STRLEN(p_sp) + (अचिन्हित)STRLEN(fname) + 3;
    cmd = alloc_id(len, aid_qf_makecmd);
    यदि (cmd == रिक्त)
	फर्काऊ रिक्त;
    प्र_लिखो((वर्ण *)cmd, "%s%s%s", (वर्ण *)p_shq, (वर्ण *)makecmd,
							       (वर्ण *)p_shq);

    // If 'shellpipe' empty: don't redirect to 'errorfile'.
    यदि (*p_sp != NUL)
	append_redir(cmd, len, p_sp, fname);

    // Display the fully formed command.  Output a newline if there's something
    // else than the :make command that was typed (in which case the cursor is
    // in column 0).
    यदि (सन्देश_col == 0)
	सन्देश_didout = FALSE;
    सन्देश_start();
    सन्देश_puts(":!");
    सन्देश_outtrans(cmd);		// show what we are doing

    फर्काऊ cmd;
}

/*
 * Used for ":make", ":lmake", ":grep", ":lgrep", ":grepadd", and ":lgrepadd"
 */
    शून्य
ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = रिक्त;
    win_T	*wp = रिक्त;
    qf_info_T	*qi = ql_info;
    पूर्णाङ्क		res;
    char_u	*au_name = रिक्त;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    पूर्णाङ्क		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    यदि (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	फर्काऊ;
    }

    au_name = make_get_auname(eap->cmdidx);
    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	यदि (aborting())
	    फर्काऊ;
# endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    यदि (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    यदि (fname == रिक्त)
	फर्काऊ;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    यदि (cmd == रिक्त)
    {
	vim_free(fname);
	फर्काऊ;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

# ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (शून्य)char_avail();
# endif

    incr_quickfix_busy();

    यदि (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat =  *curbuf->b_p_gefm != NUL ? curbuf->b_p_gefm : p_gefm;
    यदि (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    यदि (wp != रिक्त)
    {
	qi = GET_LOC_LIST(wp);
	यदि (qi == रिक्त)
	    गच्छ cleanup;
    }
    अन्यथा यदि (qi == रिक्त)
	गच्छ cleanup;

    यदि (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    यदि (au_name != रिक्त)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    यदि (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
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
    पूर्णाङ्क
qf_get_size(exarg_T *eap)
{
    qf_info_T	*qi;

    यदि ((qi = qf_cmd_get_थाक(eap, FALSE)) == रिक्त)
	फर्काऊ 0;
    फर्काऊ qf_get_curlist(qi)->qf_गन्ती;
}

/*
 * Returns the number of valid entries in the current quickfix/location list.
 */
    पूर्णाङ्क
qf_get_valid_size(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    पूर्णाङ्क		i, sz = 0;
    पूर्णाङ्क		prev_fnum = 0;

    यदि ((qi = qf_cmd_get_थाक(eap, FALSE)) == रिक्त)
	फर्काऊ 0;

    qfl = qf_get_curlist(qi);
    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
    {
	यदि (qfp->qf_valid)
	{
	    यदि (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo)
		sz++;	// Count all valid entries
	    अन्यथा यदि (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
	    {
		// Count the number of files
		sz++;
		prev_fnum = qfp->qf_fnum;
	    }
	}
    }

    फर्काऊ sz;
}

/*
 * Returns the current index of the quickfix/location list.
 * Returns 0 if there is an error.
 */
    पूर्णाङ्क
qf_get_cur_idx(exarg_T *eap)
{
    qf_info_T	*qi;

    यदि ((qi = qf_cmd_get_थाक(eap, FALSE)) == रिक्त)
	फर्काऊ 0;

    फर्काऊ qf_get_curlist(qi)->qf_index;
}

/*
 * Returns the current index in the quickfix/location list (counting only valid
 * entries). If no valid entries are in the list, then returns 1.
 */
    पूर्णाङ्क
qf_get_cur_valid_idx(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    पूर्णाङ्क		i, eidx = 0;
    पूर्णाङ्क		prev_fnum = 0;

    यदि ((qi = qf_cmd_get_थाक(eap, FALSE)) == रिक्त)
	फर्काऊ 1;

    qfl = qf_get_curlist(qi);
    qfp = qfl->qf_start;

    // check if the list has valid errors
    यदि (!qf_list_has_valid_entries(qfl))
	फर्काऊ 1;

    लागि (i = 1; i <= qfl->qf_index && qfp!= रिक्त; i++, qfp = qfp->qf_next)
    {
	यदि (qfp->qf_valid)
	{
	    यदि (eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		यदि (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    अन्यथा
		eidx++;
	}
    }

    फर्काऊ eidx ? eidx : 1;
}

/*
 * Get the 'n'th valid error entry in the quickfix or location list.
 * Used by :cdo, :ldo, :cfdo and :lfdo commands.
 * For :cdo and :ldo returns the 'n'th valid error entry.
 * For :cfdo and :lfdo returns the 'n'th valid file entry.
 */
    स्थिर पूर्णाङ्क
qf_get_nth_valid_entry(qf_list_T *qfl, पूर्णाङ्क n, पूर्णाङ्क fdo)
{
    qfline_T	*qfp;
    पूर्णाङ्क		i, eidx;
    पूर्णाङ्क		prev_fnum = 0;

    // check if the list has valid errors
    यदि (!qf_list_has_valid_entries(qfl))
	फर्काऊ 1;

    eidx = 0;
    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
    {
	यदि (qfp->qf_valid)
	{
	    यदि (fdo)
	    {
		यदि (qfp->qf_fnum > 0 && qfp->qf_fnum != prev_fnum)
		{
		    // Count the number of files
		    eidx++;
		    prev_fnum = qfp->qf_fnum;
		}
	    }
	    अन्यथा
		eidx++;
	}

	यदि (eidx == n)
	    रोक;
    }

    यदि (i <= qfl->qf_गन्ती)
	फर्काऊ i;
    अन्यथा
	फर्काऊ 1;
}

/*
 * ":cc", ":crewind", ":cfirst" and ":clast".
 * ":ll", ":lrewind", ":lfirst" and ":llast".
 * ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    शून्य
ex_cc(exarg_T *eap)
{
    qf_info_T	*qi;
    पूर्णाङ्क		errornr;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    यदि (eap->addr_गन्ती > 0)
	errornr = (पूर्णाङ्क)eap->line2;
    अन्यथा
    {
	विकल्प (eap->cmdidx)
	{
	    पक्ष CMD_cc: पक्ष CMD_ll:
		errornr = 0;
		रोक;
	    पक्ष CMD_crewind: पक्ष CMD_lrewind: पक्ष CMD_cfirst:
	    पक्ष CMD_lfirst:
		errornr = 1;
		रोक;
	    स्वभाव:
		errornr = 32767;
	}
    }

    // For cdo and ldo commands, jump to the nth valid error.
    // For cfdo and lfdo commands, jump to the nth valid file entry.
    यदि (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
	    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	errornr = qf_get_nth_valid_entry(qf_get_curlist(qi),
		eap->addr_गन्ती > 0 ? (पूर्णाङ्क)eap->line1 : 1,
		eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo);

    qf_jump(qi, 0, errornr, eap->forceit);
}

/*
 * ":cnext", ":cnfile", ":cNext" and ":cprevious".
 * ":lnext", ":lNext", ":lprevious", ":lnfile", ":lNfile" and ":lpfile".
 * Also, used by ":cdo", ":ldo", ":cfdo" and ":lfdo" commands.
 */
    शून्य
ex_cnext(exarg_T *eap)
{
    qf_info_T	*qi;
    पूर्णाङ्क		errornr;
    पूर्णाङ्क		dir;

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    यदि (eap->addr_गन्ती > 0
	    && (eap->cmdidx != CMD_cdo && eap->cmdidx != CMD_ldo
		&& eap->cmdidx != CMD_cfdo && eap->cmdidx != CMD_lfdo))
	errornr = (पूर्णाङ्क)eap->line2;
    अन्यथा
	errornr = 1;

    // Depending on the command jump to either next or previous entry/file.
    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_cnext: पक्ष CMD_lnext: पक्ष CMD_cdo: पक्ष CMD_ldo:
	    dir = FORWARD;
	    रोक;
	पक्ष CMD_cprevious: पक्ष CMD_lprevious: पक्ष CMD_cNext:
	पक्ष CMD_lNext:
	    dir = BACKWARD;
	    रोक;
	पक्ष CMD_cnfile: पक्ष CMD_lnfile: पक्ष CMD_cfdo: पक्ष CMD_lfdo:
	    dir = FORWARD_FILE;
	    रोक;
	पक्ष CMD_cpfile: पक्ष CMD_lpfile: पक्ष CMD_cNfile: पक्ष CMD_lNfile:
	    dir = BACKWARD_FILE;
	    रोक;
	स्वभाव:
	    dir = FORWARD;
	    रोक;
    }

    qf_jump(qi, dir, errornr, eap->forceit);
}

/*
 * Find the first entry in the quickfix list 'qfl' from buffer 'bnr'.
 * The index of the entry is stored in 'errornr'.
 * Returns NULL if an entry is not found.
 */
    स्थिर qfline_T *
qf_find_first_entry_in_buf(qf_list_T *qfl, पूर्णाङ्क bnr, पूर्णाङ्क *errornr)
{
    qfline_T	*qfp = रिक्त;
    पूर्णाङ्क		idx = 0;

    // Find the first entry in this file
    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, idx)
	यदि (qfp->qf_fnum == bnr)
	    रोक;

    *errornr = idx;
    फर्काऊ qfp;
}

/*
 * Find the first quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the first entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    स्थिर qfline_T *
qf_find_first_entry_on_पङ्क्ति(qfline_T *entry, पूर्णाङ्क *errornr)
{
    जबसम्म (!got_int
	    && entry->qf_prev != रिक्त
	    && entry->qf_fnum == entry->qf_prev->qf_fnum
	    && entry->qf_lnum == entry->qf_prev->qf_lnum)
    {
	entry = entry->qf_prev;
	--*errornr;
    }

    फर्काऊ entry;
}

/*
 * Find the last quickfix entry on the same line as 'entry'. Updates 'errornr'
 * with the error number for the last entry. Assumes the entries are sorted in
 * the quickfix list by line number.
 */
    स्थिर qfline_T *
qf_find_last_entry_on_पङ्क्ति(qfline_T *entry, पूर्णाङ्क *errornr)
{
    जबसम्म (!got_int &&
	    entry->qf_next != रिक्त
	    && entry->qf_fnum == entry->qf_next->qf_fnum
	    && entry->qf_lnum == entry->qf_next->qf_lnum)
    {
	entry = entry->qf_next;
	++*errornr;
    }

    फर्काऊ entry;
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   after the given line (linewise is TRUE)
 *   or after the line and column.
 */
    स्थिर पूर्णाङ्क
qf_entry_after_pos(qfline_T *qfp, pos_T *pos, पूर्णाङ्क linewise)
{
    यदि (linewise)
	फर्काऊ qfp->qf_lnum > pos->lnum;
    अन्यथा
	फर्काऊ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col > pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   before the given line (linewise is TRUE)
 *   or before the line and column.
 */
    स्थिर पूर्णाङ्क
qf_entry_before_pos(qfline_T *qfp, pos_T *pos, पूर्णाङ्क linewise)
{
    यदि (linewise)
	फर्काऊ qfp->qf_lnum < pos->lnum;
    अन्यथा
	फर्काऊ (qfp->qf_lnum < pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col < pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or after the given line (linewise is TRUE)
 *   or on or after the line and column.
 */
    स्थिर पूर्णाङ्क
qf_entry_on_or_after_pos(qfline_T *qfp, pos_T *pos, पूर्णाङ्क linewise)
{
    यदि (linewise)
	फर्काऊ qfp->qf_lnum >= pos->lnum;
    अन्यथा
	फर्काऊ (qfp->qf_lnum > pos->lnum ||
		(qfp->qf_lnum == pos->lnum && qfp->qf_col >= pos->col));
}

/*
 * Returns TRUE if the specified quickfix entry is
 *   on or before the given line (linewise is TRUE)
 *   or on or before the line and column.
 */
    स्थिर पूर्णाङ्क
qf_entry_on_or_before_pos(qfline_T *qfp, pos_T *pos, पूर्णाङ्क linewise)
{
    यदि (linewise)
	फर्काऊ qfp->qf_lnum <= pos->lnum;
    अन्यथा
	फर्काऊ (qfp->qf_lnum < pos->lnum ||
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
    स्थिर qfline_T *
qf_find_entry_after_pos(
	पूर्णाङ्क		bnr,
	pos_T		*pos,
	पूर्णाङ्क		linewise,
	qfline_T	*qfp,
	पूर्णाङ्क		*errornr)
{
    यदि (qf_entry_after_pos(qfp, pos, linewise))
	// First entry is after position 'pos'
	फर्काऊ qfp;

    // Find the entry just before or at the position 'pos'
    जबसम्म (qfp->qf_next != रिक्त
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_on_or_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    यदि (qfp->qf_next == रिक्त || qfp->qf_next->qf_fnum != bnr)
	// No entries found after position 'pos'
	फर्काऊ रिक्त;

    // Use the entry just after position 'pos'
    qfp = qfp->qf_next;
    ++*errornr;

    फर्काऊ qfp;
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
    स्थिर qfline_T *
qf_find_entry_before_pos(
	पूर्णाङ्क		bnr,
	pos_T		*pos,
	पूर्णाङ्क		linewise,
	qfline_T	*qfp,
	पूर्णाङ्क		*errornr)
{
    // Find the entry just before the position 'pos'
    जबसम्म (qfp->qf_next != रिक्त
	    && qfp->qf_next->qf_fnum == bnr
	    && qf_entry_before_pos(qfp->qf_next, pos, linewise))
    {
	qfp = qfp->qf_next;
	++*errornr;
    }

    यदि (qf_entry_on_or_after_pos(qfp, pos, linewise))
	फर्काऊ रिक्त;

    यदि (linewise)
	// If multiple entries are on the same line, then use the first entry
	qfp = qf_find_first_entry_on_पङ्क्ति(qfp, errornr);

    फर्काऊ qfp;
}

/*
 * Find a quickfix entry in 'qfl' closest to position 'pos' in buffer 'bnr' in
 * the direction 'dir'.
 */
    स्थिर qfline_T *
qf_find_closest_entry(
	qf_list_T	*qfl,
	पूर्णाङ्क		bnr,
	pos_T		*pos,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		linewise,
	पूर्णाङ्क		*errornr)
{
    qfline_T	*qfp;

    *errornr = 0;

    // Find the first entry in this file
    qfp = qf_find_first_entry_in_buf(qfl, bnr, errornr);
    यदि (qfp == रिक्त)
	फर्काऊ रिक्त;		// no entry in this file

    यदि (dir == FORWARD)
	qfp = qf_find_entry_after_pos(bnr, pos, linewise, qfp, errornr);
    अन्यथा
	qfp = qf_find_entry_before_pos(bnr, pos, linewise, qfp, errornr);

    फर्काऊ qfp;
}

/*
 * Get the nth quickfix entry below the specified entry.  Searches forward in
 * the list. If linewise is TRUE, then treat multiple entries on a single line
 * as one.
 */
    स्थिर शून्य
qf_get_nth_below_entry(qfline_T *entry_arg, पूर्णाङ्क n, पूर्णाङ्क linewise, पूर्णाङ्क *errornr)
{
    qfline_T *entry = entry_arg;

    जबसम्म (n-- > 0 && !got_int)
    {
	पूर्णाङ्क		first_errornr = *errornr;

	यदि (linewise)
	    // Treat all the entries on the same line in this file as one
	    entry = qf_find_last_entry_on_पङ्क्ति(entry, errornr);

	यदि (entry->qf_next == रिक्त
		|| entry->qf_next->qf_fnum != entry->qf_fnum)
	{
	    यदि (linewise)
		*errornr = first_errornr;
	    रोक;
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
    स्थिर शून्य
qf_get_nth_above_entry(qfline_T *entry, पूर्णाङ्क n, पूर्णाङ्क linewise, पूर्णाङ्क *errornr)
{
    जबसम्म (n-- > 0 && !got_int)
    {
	यदि (entry->qf_prev == रिक्त
		|| entry->qf_prev->qf_fnum != entry->qf_fnum)
	    रोक;

	entry = entry->qf_prev;
	--*errornr;

	// If multiple entries are on the same line, then use the first entry
	यदि (linewise)
	    entry = qf_find_first_entry_on_पङ्क्ति(entry, errornr);
    }
}

/*
 * Find the n'th quickfix entry adjacent to position 'pos' in buffer 'bnr' in
 * the specified direction.  Returns the error number in the quickfix list or 0
 * if an entry is not found.
 */
    स्थिर पूर्णाङ्क
qf_find_nth_adj_entry(
	qf_list_T	*qfl,
	पूर्णाङ्क		bnr,
	pos_T		*pos,
	पूर्णाङ्क		n,
	पूर्णाङ्क		dir,
	पूर्णाङ्क		linewise)
{
    qfline_T	*adj_entry;
    पूर्णाङ्क		errornr;

    // Find an entry closest to the specified position
    adj_entry = qf_find_closest_entry(qfl, bnr, pos, dir, linewise, &errornr);
    यदि (adj_entry == रिक्त)
	फर्काऊ 0;

    यदि (--n > 0)
    {
	// Go to the n'th entry in the current buffer
	यदि (dir == FORWARD)
	    qf_get_nth_below_entry(adj_entry, n, linewise, &errornr);
	अन्यथा
	    qf_get_nth_above_entry(adj_entry, n, linewise, &errornr);
    }

    फर्काऊ errornr;
}

/*
 * Jump to a quickfix entry in the current file nearest to the current line or
 * current line/col.
 * ":cabove", ":cbelow", ":labove", ":lbelow", ":cafter", ":cbefore",
 * ":lafter" and ":lbefore" commands
 */
    शून्य
ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    पूर्णाङ्क		dir;
    पूर्णाङ्क		buf_has_flag;
    पूर्णाङ्क		errornr = 0;
    pos_T	pos;

    यदि (eap->addr_गन्ती > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	फर्काऊ;
    }

    // Check whether the current buffer has any quickfix entries
    यदि (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    अन्यथा
	buf_has_flag = BUF_HAS_LL_ENTRY;
    यदि (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	फर्काऊ;
    }

    यदि ((qi = qf_cmd_get_थाक(eap, TRUE)) == रिक्त)
	फर्काऊ;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    यदि (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	फर्काऊ;
    }

    यदि (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    अन्यथा
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_गन्ती > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    यदि (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    अन्यथा
	emsg(_(e_no_more_वस्तुहरू));
}

/*
 * Return the autocmd name for the :cfile Ex commands
 */
    स्थिर char_u *
cfile_get_auname(cmdidx_T cmdidx)
{
    विकल्प (cmdidx)
    {
	पक्ष CMD_cfile:	    फर्काऊ (char_u *)"cfile";
	पक्ष CMD_cgetfile:  फर्काऊ (char_u *)"cgetfile";
	पक्ष CMD_caddfile:  फर्काऊ (char_u *)"caddfile";
	पक्ष CMD_lfile:	    फर्काऊ (char_u *)"lfile";
	पक्ष CMD_lgetfile:  फर्काऊ (char_u *)"lgetfile";
	पक्ष CMD_laddfile:  फर्काऊ (char_u *)"laddfile";
	स्वभाव:	    फर्काऊ रिक्त;
    }
}

/*
 * ":cfile"/":cgetfile"/":caddfile" commands.
 * ":lfile"/":lgetfile"/":laddfile" commands.
 */
    शून्य
ex_cfile(exarg_T *eap)
{
    char_u	*enc = रिक्त;
    win_T	*wp = रिक्त;
    qf_info_T	*qi = ql_info;
    char_u	*au_name = रिक्त;
    int_u	save_qfid = 0;		// init for gcc
    पूर्णाङ्क		res;

    au_name = cfile_get_auname(eap->cmdidx);
    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							रिक्त, FALSE, curbuf))
    {
# ifdef FEAT_EVAL
	यदि (aborting())
	    फर्काऊ;
# endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
# ifdef FEAT_BROWSE
    यदि (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   रिक्त, रिक्त,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), रिक्त);
	यदि (browse_file == रिक्त)
	    फर्काऊ;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    अन्यथा
# endif
    यदि (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    यदि (is_loclist_cmd(eap->cmdidx))
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
    यदि (wp != रिक्त)
    {
	qi = GET_LOC_LIST(wp);
	यदि (qi == रिक्त)
	{
	    decr_quickfix_busy();
	    फर्काऊ;
	}
    }
    अन्यथा यदि (qi == रिक्त)
    {
	decr_quickfix_busy();
	फर्काऊ;
    }
    यदि (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    यदि (au_name != रिक्त)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, रिक्त, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    यदि (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

/*
 * Return the vimgrep autocmd name.
 */
    स्थिर char_u *
vgr_get_auname(cmdidx_T cmdidx)
{
    विकल्प (cmdidx)
    {
	पक्ष CMD_vimgrep:     फर्काऊ (char_u *)"vimgrep";
	पक्ष CMD_lvimgrep:    फर्काऊ (char_u *)"lvimgrep";
	पक्ष CMD_vimgrepadd:  फर्काऊ (char_u *)"vimgrepadd";
	पक्ष CMD_lvimgrepadd: फर्काऊ (char_u *)"lvimgrepadd";
	पक्ष CMD_grep:	      फर्काऊ (char_u *)"grep";
	पक्ष CMD_lgrep:	      फर्काऊ (char_u *)"lgrep";
	पक्ष CMD_grepadd:     फर्काऊ (char_u *)"grepadd";
	पक्ष CMD_lgrepadd:    फर्काऊ (char_u *)"lgrepadd";
	स्वभाव: फर्काऊ रिक्त;
    }
}

/*
 * Initialize the regmatch used by vimgrep for pattern "s".
 */
    स्थिर शून्य
vgr_init_regmatch(regmmatch_T *regmatch, char_u *s)
{
    // Get the search pattern: either white-separated or enclosed in //
    regmatch->regprog = रिक्त;

    यदि (s == रिक्त || *s == NUL)
    {
	// Pattern is empty, use last search pattern.
	यदि (last_खोज_pat() == रिक्त)
	{
	    emsg(_(e_no_previous_regular_expression));
	    फर्काऊ;
	}
	regmatch->regprog = vim_regcomp(last_खोज_pat(), RE_MAGIC);
    }
    अन्यथा
	regmatch->regprog = vim_regcomp(s, RE_MAGIC);

    regmatch->rmm_ic = p_ic;
    regmatch->rmm_maxcol = 0;
}

/*
 * Display a file name when vimgrep is running.
 */
    स्थिर शून्य
vgr_display_fname(char_u *fname)
{
    char_u	*p;

    सन्देश_start();
    p = सन्देश_strtrunc(fname, TRUE);
    यदि (p == रिक्त)
	सन्देश_outtrans(fname);
    अन्यथा
    {
	सन्देश_outtrans(p);
	vim_free(p);
    }
    सन्देश_clr_eos();
    सन्देश_didout = FALSE;	    // overwrite this message
    सन्देश_nowait = TRUE;	    // don't wait for this message
    सन्देश_col = 0;
    out_flush();
}

/*
 * Load a dummy buffer to search for a pattern using vimgrep.
 */
    स्थिर buf_T *
vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    पूर्णाङ्क		save_mls;
# यदि defined(FEAT_SYN_HL)
    char_u	*save_ei = रिक्त;
# endif
    buf_T	*buf;

# यदि defined(FEAT_SYN_HL)
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
# यदि defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
# endif

    फर्काऊ buf;
}

/*
 * Check whether a quickfix/location list is valid. Autocmds may remove or
 * change a quickfix list when vimgrep is running. If the list is not found,
 * create a new list.
 */
    स्थिर पूर्णाङ्क
vgr_qflist_valid(
	win_T	    *wp,
	qf_info_T   *qi,
	int_u	    qfid,
	char_u	    *title)
{
    // Verify that the quickfix/location list was not freed by an autocmd
    यदि (!qflist_valid(wp, qfid))
    {
	यदि (wp != रिक्त)
	{
	    // An autocmd has freed the location list.
	    emsg(_(e_current_location_list_was_changed));
	    फर्काऊ FALSE;
	}
	अन्यथा
	{
	    // Quickfix list is not found, create a new one.
	    qf_new_list(qi, title);
	    फर्काऊ TRUE;
	}
    }

    यदि (qf_restore_list(qi, qfid) == FAIL)
	फर्काऊ FALSE;

    फर्काऊ TRUE;
}

/*
 * Search for a pattern in all the lines in a buffer and add the matching lines
 * to a quickfix list.
 */
    स्थिर पूर्णाङ्क
vgr_match_buflines(
	qf_list_T   *qfl,
	char_u	    *fname,
	buf_T	    *buf,
	char_u	    *spat,
	regmmatch_T *regmatch,
	दीर्घ	    *tomatch,
	पूर्णाङ्क	    duplicate_name,
	पूर्णाङ्क	    flags)
{
    पूर्णाङ्क		found_match = FALSE;
    दीर्घ	lnum;
    colnr_T	col;
    पूर्णाङ्क		pat_len = (पूर्णाङ्क)STRLEN(spat);
    यदि (pat_len > FUZZY_MATCH_MAX_LEN)
	pat_len = FUZZY_MATCH_MAX_LEN;

    लागि (lnum = 1; lnum <= buf->b_ml.ml_पङ्क्ति_गन्ती && *tomatch > 0; ++lnum)
    {
	col = 0;
	यदि (!(flags & VGR_FUZZY))
	{
	    // Regular expression match
	    जबसम्म (vim_regexec_multi(regmatch, curwin, buf, lnum,
								col, रिक्त) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		यदि (qf_add_entry(qfl,
			    रिक्त,	// dir
			    fname,
			    रिक्त,
			    duplicate_name ? 0 : buf->b_fnum,
			    ml_get_buf(buf,
				regmatch->startpos[0].lnum + lnum, FALSE),
			    regmatch->startpos[0].lnum + lnum,
			    regmatch->endpos[0].lnum + lnum,
			    regmatch->startpos[0].col + 1,
			    regmatch->endpos[0].col + 1,
			    FALSE,	// vis_col
			    रिक्त,	// search pattern
			    0,		// nr
			    0,		// type
			    रिक्त,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    रोक;
		}
		found_match = TRUE;
		यदि (--*tomatch == 0)
		    रोक;
		यदि ((flags & VGR_GLOBAL) == 0
			|| regmatch->endpos[0].lnum > 0)
		    रोक;
		col = regmatch->endpos[0].col
		    + (col == regmatch->endpos[0].col);
		यदि (col > ml_get_buf_len(buf, lnum))
		    रोक;
	    }
	}
	अन्यथा
	{
	    char_u  *str = ml_get_buf(buf, lnum, FALSE);
	    colnr_T linelen = ml_get_buf_len(buf, lnum);
	    पूर्णाङ्क	    score;
	    int_u   matches[FUZZY_MATCH_MAX_LEN];
	    int_u   sz = ARRAY_LENGTH(matches);

	    // Fuzzy string match
	    CLEAR_FIELD(matches);
	    जबसम्म (fuzzy_match(str + col, spat, FALSE, &score,
			matches, sz) > 0)
	    {
		// Pass the buffer number so that it gets used even for a
		// dummy buffer, unless duplicate_name is set, then the
		// buffer will be wiped out below.
		यदि (qf_add_entry(qfl,
			    रिक्त,	// dir
			    fname,
			    रिक्त,
			    duplicate_name ? 0 : buf->b_fnum,
			    str,
			    lnum,
			    0,
			    matches[0] + col + 1,
			    0,
			    FALSE,	// vis_col
			    रिक्त,	// search pattern
			    0,		// nr
			    0,		// type
			    रिक्त,	// user_data
			    TRUE	// valid
			    ) == QF_FAIL)
		{
		    got_int = TRUE;
		    रोक;
		}
		found_match = TRUE;
		यदि (--*tomatch == 0)
		    रोक;
		यदि ((flags & VGR_GLOBAL) == 0)
		    रोक;
		col = matches[pat_len - 1] + col + 1;
		यदि (col > linelen)
		    रोक;
	    }
	}
	पङ्क्ति_breakcheck();
	यदि (got_int)
	    रोक;
    }

    फर्काऊ found_match;
}

/*
 * Jump to the first match and update the directory.
 */
    स्थिर शून्य
vgr_jump_to_match(
	qf_info_T   *qi,
	पूर्णाङ्क	    forceit,
	पूर्णाङ्क	    *redraw_for_dummy,
	buf_T	    *first_match_buf,
	char_u	    *target_dir)
{
    buf_T	*buf;

    buf = curbuf;
    qf_jump(qi, 0, 0, forceit);
    यदि (buf != curbuf)
	// If we jumped to another buffer redrawing will already be
	// taken care of.
	*redraw_for_dummy = FALSE;

    // Jump to the directory used after loading the buffer.
    यदि (curbuf == first_match_buf && target_dir != रिक्त)
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
प्रकारनिर्देश संरचना
{
    दीर्घ	tomatch;	// maximum number of matches to find
    char_u	*spat;		// search pattern
    पूर्णाङ्क		flags;		// search modifier
    char_u	**fnames;	// list of files to search
    पूर्णाङ्क		fcount;		// number of files
    regmmatch_T	regmatch;	// compiled search pattern
    char_u	*qf_title;	// quickfix list title
} vgr_args_T;

/*
 * Process :vimgrep command arguments. The command syntax is:
 *
 *	:{count}vimgrep /{pattern}/[g][j] {file} ...
 */
    स्थिर पूर्णाङ्क
vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    CLEAR_POINTER(args);

    args->regmatch.regprog = रिक्त;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    यदि (eap->addr_गन्ती > 0)
	args->tomatch = eap->line2;
    अन्यथा
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    यदि (p == रिक्त)
    {
	emsg(_(e_invalid_खोज_pattern_or_delimiter));
	फर्काऊ FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    यदि (args->regmatch.regprog == रिक्त)
	फर्काऊ FAIL;

    p = skipwhite(p);
    यदि (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	फर्काऊ FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    यदि ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	फर्काऊ FAIL;
    }

    फर्काऊ OK;
}

/*
 * Return TRUE if "buf" had an existing swap file, the current swap file does
 * not end in ".swp".
 */
    स्थिर पूर्णाङ्क
existing_swapfile(buf_T *buf)
{
    यदि (buf->b_ml.ml_mfp != रिक्त && buf->b_ml.ml_mfp->mf_fname != रिक्त)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	माप_प्रकार len = STRLEN(fname);

	फर्काऊ fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    फर्काऊ FALSE;
}

/*
 * Search for a pattern in a list of files and populate the quickfix list with
 * the matches.
 */
    स्थिर पूर्णाङ्क
vgr_process_files(
	win_T		*wp,
	qf_info_T	*qi,
	vgr_args_T	*cmd_args,
	पूर्णाङ्क		*redraw_for_dummy,
	buf_T		**first_match_buf,
	char_u		**target_dir)
{
    पूर्णाङ्क		status = FAIL;
    int_u	save_qfid = qf_get_curlist(qi)->qf_id;
    समय_प्रकार	seconds = 0;
    char_u	*fname;
    पूर्णाङ्क		fi;
    buf_T	*buf;
    पूर्णाङ्क		duplicate_name = FALSE;
    पूर्णाङ्क		using_dummy;
    char_u	*dirname_start = रिक्त;
    char_u	*dirname_now = रिक्त;
    पूर्णाङ्क		found_match;
    aco_save_T	aco;

    dirname_start = alloc_id(MAXPATHL, aid_qf_dirname_start);
    dirname_now = alloc_id(MAXPATHL, aid_qf_dirname_now);
    यदि (dirname_start == रिक्त || dirname_now == रिक्त)
	गच्छ theend;

    // Remember the current directory, because a BufRead autocommand that does
    // ":lcd %:p:h" changes the meaning of short path names.
    mch_dirname(dirname_start, MAXPATHL);

    seconds = (समय_प्रकार)0;
    लागि (fi = 0; fi < cmd_args->fcount && !got_int && cmd_args->tomatch > 0;
									++fi)
    {
	fname = shorten_fname1(cmd_args->fnames[fi]);
	यदि (समय(रिक्त) > seconds)
	{
	    // Display the file name every second or so, show the user we are
	    // working on it.
	    seconds = समय(रिक्त);
	    vgr_display_fname(fname);
	}

	buf = buflist_findname_exp(cmd_args->fnames[fi]);
	यदि (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	{
	    // Remember that a buffer with this name already exists.
	    duplicate_name = (buf != रिक्त);
	    using_dummy = TRUE;
	    *redraw_for_dummy = TRUE;

	    buf = vgr_load_dummy_buf(fname, dirname_start, dirname_now);
	}
	अन्यथा
	    // Use existing, loaded buffer.
	    using_dummy = FALSE;

	// Check whether the quickfix list is still valid. When loading a
	// buffer above, autocommands might have changed the quickfix list.
	यदि (!vgr_qflist_valid(wp, qi, save_qfid, cmd_args->qf_title))
	    गच्छ theend;

	save_qfid = qf_get_curlist(qi)->qf_id;

	यदि (buf == रिक्त)
	{
	    यदि (!got_int)
		smsg(_("Cannot open file \"%s\""), fname);
	}
	अन्यथा
	{
	    // Try for a match in all lines of the buffer.
	    // For ":1vimgrep" look for first match only.
	    found_match = vgr_match_buflines(qf_get_curlist(qi),
		    fname, buf, cmd_args->spat, &cmd_args->regmatch,
		    &cmd_args->tomatch, duplicate_name, cmd_args->flags);

	    यदि (using_dummy)
	    {
		यदि (found_match && *first_match_buf == रिक्त)
		    *first_match_buf = buf;
		यदि (duplicate_name)
		{
		    // Never keep a dummy buffer if there is another buffer
		    // with the same name.
		    wipe_dummy_buffer(buf, dirname_start);
		    buf = रिक्त;
		}
		अन्यथा यदि ((cmdmod.cmod_flags & CMOD_HIDE) == 0
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
		    यदि (!found_match)
		    {
			wipe_dummy_buffer(buf, dirname_start);
			buf = रिक्त;
		    }
		    अन्यथा यदि (buf != *first_match_buf
					|| (cmd_args->flags & VGR_NOJUMP)
					|| existing_swapfile(buf))
		    {
			unload_dummy_buffer(buf, dirname_start);
			// Keeping the buffer, remove the dummy flag.
			buf->b_flags &= ~BF_DUMMY;
			buf = रिक्त;
		    }
		}

		यदि (buf != रिक्त)
		{
		    // Keeping the buffer, remove the dummy flag.
		    buf->b_flags &= ~BF_DUMMY;

		    // If the buffer is still loaded we need to use the
		    // directory we jumped to below.
		    यदि (buf == *first_match_buf
			    && *target_dir == रिक्त
			    && STRCMP(dirname_start, dirname_now) != 0)
			*target_dir = vim_strsave(dirname_now);

		    // The buffer is still loaded, the Filetype autocommands
		    // need to be done now, in that buffer.  And the modelines
		    // need to be done (again).  But not the window-local
		    // options!
		    aucmd_prepbuf(&aco, buf);
		    यदि (curbuf == buf)
		    {
# यदि defined(FEAT_SYN_HL)
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
    फर्काऊ status;
}

/*
 * ":vimgrep {pattern} file(s)"
 * ":vimgrepadd {pattern} file(s)"
 * ":lvimgrep {pattern} file(s)"
 * ":lvimgrepadd {pattern} file(s)"
 */
    शून्य
ex_vimgrep(exarg_T *eap)
{
    vgr_args_T	args;
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int_u	save_qfid;
    win_T	*wp = रिक्त;
    पूर्णाङ्क		redraw_for_dummy = FALSE;
    buf_T	*first_match_buf = रिक्त;
    char_u	*target_dir = रिक्त;
    char_u	*au_name =  रिक्त;
    पूर्णाङ्क		status;

    यदि (!check_can_set_curbuf_forceit(eap->forceit))
	फर्काऊ;

    au_name = vgr_get_auname(eap->cmdidx);
    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	यदि (aborting())
	    फर्काऊ;
# endif
    }

    qi = qf_cmd_get_or_alloc_थाक(eap, &wp);
    यदि (qi == रिक्त)
	फर्काऊ;

    यदि (vgr_process_args(eap, &args) == FAIL)
	गच्छ theend;

    यदि ((eap->cmdidx != CMD_grepadd && eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_vimgrepadd
		&& eap->cmdidx != CMD_lvimgrepadd)
					|| qf_थाक_empty(qi))
	// make place for a new list
	qf_new_list(qi, args.qf_title);

    incr_quickfix_busy();

    status = vgr_process_files(wp, qi, &args, &redraw_for_dummy,
						&first_match_buf, &target_dir);
    यदि (status != OK)
    {
	FreeWild(args.fcount, args.fnames);
	decr_quickfix_busy();
	गच्छ theend;
    }

    FreeWild(args.fcount, args.fnames);

    qfl = qf_get_curlist(qi);
    qfl->qf_nonevalid = FALSE;
    qfl->qf_ptr = qfl->qf_start;
    qfl->qf_index = 1;
    qf_list_changed(qfl);

    qf_update_buffer(qi, रिक्त);

    // Remember the current quickfix list identifier, so that we can check for
    // autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;

    यदि (au_name != रिक्त)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    // The QuickFixCmdPost autocmd may free the quickfix list. Check the list
    // is still valid.
    यदि (!qflist_valid(wp, save_qfid)
	    || qf_restore_list(qi, save_qfid) == FAIL)
    {
	decr_quickfix_busy();
	गच्छ theend;
    }

    // Jump to first match.
    यदि (!qf_list_empty(qf_get_curlist(qi)))
    {
	यदि ((args.flags & VGR_NOJUMP) == 0)
	    vgr_jump_to_match(qi, eap->forceit, &redraw_for_dummy,
		    first_match_buf, target_dir);
    }
    अन्यथा
	semsg(_(e_no_match_str_2), args.spat);

    decr_quickfix_busy();

    // If we loaded a dummy buffer into the current window, the autocommands
    // may have messed up things, need to redraw and recompute folds.
    यदि (redraw_for_dummy)
    {
# ifdef FEAT_FOLDING
	foldUpdateAll(curwin);
# अन्यथा
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
    स्थिर शून्य
restore_start_dir(char_u *dirname_start)
{
    char_u *dirname_now = alloc(MAXPATHL);

    यदि (dirname_now == रिक्त)
	फर्काऊ;

    mch_dirname(dirname_now, MAXPATHL);
    यदि (STRCMP(dirname_start, dirname_now) != 0)
    {
	// If the directory has changed, change it back by building up an
	// appropriate ex command and executing it.
	exarg_T ea;

	CLEAR_FIELD(ea);
	ea.arg = dirname_start;
	ea.cmdidx = (curwin->w_localdir == रिक्त) ? CMD_cd : CMD_lcd;
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
    स्थिर buf_T *
load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    पूर्णाङ्क		failed = TRUE;
    aco_save_T	aco;
    पूर्णाङ्क		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(रिक्त, रिक्त, (linenr_T)1, BLN_DUMMY);
    यदि (newbuf == रिक्त)
	फर्काऊ रिक्त;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    यदि (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);
	यदि (curbuf == newbuf)
	{
	    // Need to set the filename for autocommands.
	    (शून्य)setfname(curbuf, fname, रिक्त, FALSE);

	    // Create swap file now to avoid the ATTENTION message.
	    check_need_swap(TRUE);

	    // Remove the "dummy" flag, otherwise autocommands may not
	    // work.
	    curbuf->b_flags &= ~BF_DUMMY;

	    newbuf_to_wipe.br_buf = रिक्त;
	    readfile_result = readfile(fname, रिक्त,
			(linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
			रिक्त, READ_NEW | READ_DUMMY);
	    --newbuf->b_locked;
	    यदि (readfile_result == OK
		    && !got_int
		    && !(curbuf->b_flags & BF_NEW))
	    {
		failed = FALSE;
		यदि (curbuf != newbuf)
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

	    यदि (newbuf_to_wipe.br_buf != रिक्त && bufref_valid(&newbuf_to_wipe))
	    {
		block_autocmds();
		wipe_dummy_buffer(newbuf_to_wipe.br_buf, रिक्त);
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

    यदि (!bufref_valid(&newbufref))
	फर्काऊ रिक्त;
    यदि (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	फर्काऊ रिक्त;
    }
    फर्काऊ newbuf;
}

/*
 * Wipe out the dummy buffer that load_dummy_buffer() created. Restores
 * directory to "dirname_start" if not NULL prior to returning, if autocmds or
 * the 'autochdir' option have changed it.
 */
    स्थिर शून्य
wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    जबसम्म (buf->b_nwindows > 0)
    {
	पूर्णाङ्क	    did_one = FALSE;
	win_T	    *wp;

	यदि (firstwin->w_next != रिक्त)
	    FOR_ALL_WINDOWS(wp)
		यदि (wp->w_buffer == buf)
		{
		    यदि (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    रोक;
		}
	यदि (!did_one)
	    गच्छ fail;
    }

    यदि (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
# यदि defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
# endif

	wipe_buffer(buf, TRUE);

# यदि defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
# endif
	यदि (dirname_start != रिक्त)
	    // When autocommands/'autochdir' option changed directory: go back.
	    restore_start_dir(dirname_start);

	फर्काऊ;
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
    स्थिर शून्य
unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    यदि (curbuf == buf)		// safety check
	फर्काऊ;

    close_buffer(रिक्त, buf, DOBUF_UNLOAD, FALSE, TRUE, FALSE);

    // When autocommands/'autochdir' option changed directory: go back.
    restore_start_dir(dirname_start);
}

# यदि defined(FEAT_EVAL)
/*
 * Copy the specified quickfix entry items into a new dict and append the dict
 * to 'list'.  Returns OK on success.
 */
    स्थिर पूर्णाङ्क
get_qfline_वस्तुहरू(qfline_T *qfp, list_T *list)
{
    पूर्णाङ्क		bufnum;
    dict_T	*dict;
    char_u	buf[2];
    माप_प्रकार	buflen;

    // Handle entries with a non-existing buffer number.
    bufnum = qfp->qf_fnum;
    यदि (bufnum != 0 && (buflist_findnr(bufnum) == रिक्त))
	bufnum = 0;

    यदि ((dict = dict_alloc()) == रिक्त)
	फर्काऊ FAIL;
    यदि (list_append_dict(list, dict) == FAIL)
    {
	dict_unref(dict);
	फर्काऊ FAIL;
    }

    buf[0] = qfp->qf_type;
    buf[1] = NUL;
    buflen = (buf[0] == NUL) ? 0 : 1;
    यदि (dict_add_सङ्ख्या(dict, "bufnr", (दीर्घ)bufnum) == FAIL
	    || dict_add_सङ्ख्या(dict, "lnum",     (दीर्घ)qfp->qf_lnum) == FAIL
	    || dict_add_सङ्ख्या(dict, "end_lnum", (दीर्घ)qfp->qf_end_lnum) == FAIL
	    || dict_add_सङ्ख्या(dict, "col",      (दीर्घ)qfp->qf_col) == FAIL
	    || dict_add_सङ्ख्या(dict, "end_col",  (दीर्घ)qfp->qf_end_col) == FAIL
	    || dict_add_सङ्ख्या(dict, "vcol",     (दीर्घ)qfp->qf_viscol) == FAIL
	    || dict_add_सङ्ख्या(dict, "nr",       (दीर्घ)qfp->qf_nr) == FAIL
	    || dict_add_string(dict, "module", qfp->qf_module) == FAIL
	    || dict_add_string(dict, "pattern", qfp->qf_pattern) == FAIL
	    || dict_add_string(dict, "text", qfp->qf_text) == FAIL
	    || dict_add_string_len(dict, "type", buf, (पूर्णाङ्क)buflen) == FAIL
	    || (qfp->qf_user_data.v_type != VAR_UNKNOWN
		&& dict_add_tv(dict, "user_data", &qfp->qf_user_data) == FAIL )
	    || dict_add_सङ्ख्या(dict, "valid", (दीर्घ)qfp->qf_valid) == FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * Add each quickfix error to list "list" as a dictionary.
 * If qf_idx is -1, use the current list. Otherwise, use the specified list.
 * If eidx is not 0, then return only the specified entry. Otherwise return
 * all the entries.
 */
    स्थिर पूर्णाङ्क
get_errorlist(
	qf_info_T	*qi_arg,
	win_T		*wp,
	पूर्णाङ्क		qf_idx,
	पूर्णाङ्क		eidx,
	list_T		*list)
{
    qf_info_T	*qi = qi_arg;
    qf_list_T	*qfl;
    qfline_T	*qfp;
    पूर्णाङ्क		i;

    यदि (qi == रिक्त)
    {
	qi = ql_info;
	यदि (wp != रिक्त)
	    qi = GET_LOC_LIST(wp);
	यदि (qi == रिक्त)
	    फर्काऊ FAIL;

    }

    यदि (eidx < 0)
	फर्काऊ OK;

    यदि (qf_idx == INVALID_QFIDX)
	qf_idx = qi->qf_curlist;

    यदि (qf_idx >= qi->qf_listcount)
	फर्काऊ FAIL;

    qfl = qf_get_list(qi, qf_idx);
    यदि (qf_list_empty(qfl))
	फर्काऊ FAIL;

    FOR_ALL_QFL_वस्तुहरू(qfl, qfp, i)
    {
	यदि (eidx > 0)
	{
	    यदि (eidx == i)
		फर्काऊ get_qfline_वस्तुहरू(qfp, list);
	}
	अन्यथा यदि (get_qfline_वस्तुहरू(qfp, list) == FAIL)
	    फर्काऊ FAIL;
    }

    फर्काऊ OK;
}

// Flags used by getqflist()/getloclist() to determine which fields to return.
गणना {
    QF_GETLIST_NONE	= 0x0,
    QF_GETLIST_TITLE	= 0x1,
    QF_GETLIST_वस्तुहरू	= 0x2,
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
    स्थिर पूर्णाङ्क
qf_get_list_from_पङ्क्तिहरू(dict_T *what, dictitem_T *di, dict_T *retdict)
{
    पूर्णाङ्क		status = FAIL;
    qf_info_T	*qi;
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    list_T	*l;

    // Only a List value is supported
    यदि (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == रिक्त)
	फर्काऊ FAIL;

    // If errorformat is supplied then use it, otherwise use the 'efm'
    // option setting
    यदि ((efm_di = dict_find(what, (char_u *)"efm", -1)) != रिक्त)
    {
	यदि (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == रिक्त)
	    फर्काऊ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    l = list_alloc();
    यदि (l == रिक्त)
	फर्काऊ FAIL;

    qi = qf_alloc_थाक(QFLT_INTERNAL, 1);
    यदि (qi != रिक्त)
    {
	यदि (qf_init_ext(qi, 0, रिक्त, रिक्त, &di->di_tv, errorformat,
		    TRUE, (linenr_T)0, (linenr_T)0, रिक्त, रिक्त) > 0)
	{
	    (शून्य)get_errorlist(qi, रिक्त, 0, 0, l);
	    qf_free(&qi->qf_lists[0]);
	}

	qf_free_lists(qi);
    }
    dict_add_list(retdict, "items", l);
    status = OK;

    फर्काऊ status;
}

/*
 * Return the quickfix/location list window identifier in the current tabpage.
 */
    स्थिर पूर्णाङ्क
qf_winid(qf_info_T *qi)
{
    win_T	*win;

    // The quickfix window can be opened even if the quickfix list is not set
    // using ":copen". This is not true for location lists.
    यदि (qi == रिक्त)
	फर्काऊ 0;
    win = qf_find_win(qi);
    यदि (win != रिक्त)
	फर्काऊ win->w_id;
    फर्काऊ 0;
}

/*
 * Returns the number of the buffer displayed in the quickfix/location list
 * window. If there is no buffer associated with the list or the buffer is
 * wiped out, then returns 0.
 */
    स्थिर पूर्णाङ्क
qf_getprop_qfbufnr(qf_info_T *qi, dict_T *retdict)
{
    पूर्णाङ्क	bufnum = 0;

    यदि (qi != रिक्त && buflist_findnr(qi->qf_bufnr) != रिक्त)
	bufnum = qi->qf_bufnr;

    फर्काऊ dict_add_सङ्ख्या(retdict, "qfbufnr", bufnum);
}

/*
 * Convert the keys in 'what' to quickfix list property flags.
 */
    स्थिर पूर्णाङ्क
qf_getprop_keys2flags(dict_T *what, पूर्णाङ्क loclist)
{
    पूर्णाङ्क		flags = QF_GETLIST_NONE;

    यदि (dict_has_साँचो(what, "all"))
    {
	flags |= QF_GETLIST_ALL;
	यदि (!loclist)
	    // File window ID is applicable only to location list windows
	    flags &= ~ QF_GETLIST_FILEWINID;
    }

    यदि (dict_has_साँचो(what, "title"))
	flags |= QF_GETLIST_TITLE;

    यदि (dict_has_साँचो(what, "nr"))
	flags |= QF_GETLIST_NR;

    यदि (dict_has_साँचो(what, "winid"))
	flags |= QF_GETLIST_WINID;

    यदि (dict_has_साँचो(what, "context"))
	flags |= QF_GETLIST_CONTEXT;

    यदि (dict_has_साँचो(what, "id"))
	flags |= QF_GETLIST_ID;

    यदि (dict_has_साँचो(what, "items"))
	flags |= QF_GETLIST_वस्तुहरू;

    यदि (dict_has_साँचो(what, "idx"))
	flags |= QF_GETLIST_IDX;

    यदि (dict_has_साँचो(what, "size"))
	flags |= QF_GETLIST_SIZE;

    यदि (dict_has_साँचो(what, "changedtick"))
	flags |= QF_GETLIST_TICK;

    यदि (loclist && dict_has_साँचो(what, "filewinid"))
	flags |= QF_GETLIST_FILEWINID;

    यदि (dict_has_साँचो(what, "qfbufnr"))
	flags |= QF_GETLIST_QFBUFNR;

    यदि (dict_has_साँचो(what, "quickfixtextfunc"))
	flags |= QF_GETLIST_QFTF;

    फर्काऊ flags;
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
    स्थिर पूर्णाङ्क
qf_getprop_qfidx(qf_info_T *qi, dict_T *what)
{
    पूर्णाङ्क		qf_idx;
    dictitem_T	*di;

    qf_idx = qi->qf_curlist;	// default is the current list
    यदि ((di = dict_find(what, (char_u *)"nr", -1)) != रिक्त)
    {
	// Use the specified quickfix/location list
	यदि (di->di_tv.v_type == VAR_सङ्ख्या)
	{
	    // for zero use the current list
	    यदि (di->di_tv.vval.v_सङ्ख्या != 0)
	    {
		qf_idx = di->di_tv.vval.v_सङ्ख्या - 1;
		यदि (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		    qf_idx = INVALID_QFIDX;
	    }
	}
	अन्यथा यदि (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != रिक्त
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	    // Get the last quickfix list number
	    qf_idx = qi->qf_listcount - 1;
	अन्यथा
	    qf_idx = INVALID_QFIDX;
    }

    यदि ((di = dict_find(what, (char_u *)"id", -1)) != रिक्त)
    {
	// Look for a list with the specified id
	यदि (di->di_tv.v_type == VAR_सङ्ख्या)
	{
	    // For zero, use the current list or the list specified by 'nr'
	    यदि (di->di_tv.vval.v_सङ्ख्या != 0)
		qf_idx = qf_id2nr(qi, di->di_tv.vval.v_सङ्ख्या);
	}
	अन्यथा
	    qf_idx = INVALID_QFIDX;
    }

    फर्काऊ qf_idx;
}

/*
 * Return default values for quickfix list properties in retdict.
 */
    स्थिर पूर्णाङ्क
qf_getprop_defaults(qf_info_T *qi, पूर्णाङ्क flags, पूर्णाङ्क locstack, dict_T *retdict)
{
    पूर्णाङ्क		status = OK;

    यदि (flags & QF_GETLIST_TITLE)
	status = dict_add_string_len(retdict, "title", (char_u *)"", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_वस्तुहरू))
    {
	list_T	*l = list_alloc();
	यदि (l != रिक्त)
	    status = dict_add_list(retdict, "items", l);
	अन्यथा
	    status = FAIL;
    }
    यदि ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_सङ्ख्या(retdict, "nr", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_सङ्ख्या(retdict, "winid", qf_winid(qi));
    यदि ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_सङ्ख्या(retdict, "id", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_IDX))
	status = dict_add_सङ्ख्या(retdict, "idx", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_सङ्ख्या(retdict, "size", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_सङ्ख्या(retdict, "changedtick", 0);
    यदि ((status == OK) && locstack && (flags & QF_GETLIST_FILEWINID))
	status = dict_add_सङ्ख्या(retdict, "filewinid", 0);
    यदि ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    फर्काऊ status;
}

/*
 * Return the quickfix list title as 'title' in retdict
 */
    स्थिर पूर्णाङ्क
qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    फर्काऊ dict_add_string(retdict, "title", qfl->qf_title);
}

/*
 * Returns the identifier of the window used to display files from a location
 * list.  If there is no associated window, then returns 0. Useful only when
 * called from a location list window.
 */
    स्थिर पूर्णाङ्क
qf_getprop_filewinid(win_T *wp, qf_info_T *qi, dict_T *retdict)
{
    पूर्णाङ्क winid = 0;

    यदि (wp != रिक्त && IS_LL_WINDOW(wp))
    {
	win_T	*ll_wp = qf_find_win_with_loclist(qi);
	यदि (ll_wp != रिक्त)
	    winid = ll_wp->w_id;
    }

    फर्काऊ dict_add_सङ्ख्या(retdict, "filewinid", winid);
}

/*
 * Return the quickfix list items/entries as 'items' in retdict.
 * If eidx is not 0, then return the item at the specified index.
 */
    स्थिर पूर्णाङ्क
qf_getprop_वस्तुहरू(qf_info_T *qi, पूर्णाङ्क qf_idx, पूर्णाङ्क eidx, dict_T *retdict)
{
    पूर्णाङ्क		status = OK;
    list_T	*l = list_alloc();
    यदि (l != रिक्त)
    {
	(शून्य)get_errorlist(qi, रिक्त, qf_idx, eidx, l);
	dict_add_list(retdict, "items", l);
    }
    अन्यथा
	status = FAIL;

    फर्काऊ status;
}

/*
 * Return the quickfix list context (if any) as 'context' in retdict.
 */
    स्थिर पूर्णाङ्क
qf_getprop_ctx(qf_list_T *qfl, dict_T *retdict)
{
    पूर्णाङ्क		status;
    dictitem_T	*di;

    यदि (qfl->qf_ctx != रिक्त)
    {
	di = dictitem_alloc((char_u *)"context");
	यदि (di != रिक्त)
	{
	    copy_tv(qfl->qf_ctx, &di->di_tv);
	    status = dict_add(retdict, di);
	    यदि (status == FAIL)
		dictitem_free(di);
	}
	अन्यथा
	    status = FAIL;
    }
    अन्यथा
	status = dict_add_string_len(retdict, "context", (char_u *)"", 0);

    फर्काऊ status;
}

/*
 * Return the current quickfix list index as 'idx' in retdict.
 * If a specific entry index (eidx) is supplied, then use that.
 */
    स्थिर पूर्णाङ्क
qf_getprop_idx(qf_list_T *qfl, पूर्णाङ्क eidx, dict_T *retdict)
{
    यदि (eidx == 0)
    {
	eidx = qfl->qf_index;
	यदि (qf_list_empty(qfl))
	    // For empty lists, current index is set to 0
	    eidx = 0;
    }
    फर्काऊ dict_add_सङ्ख्या(retdict, "idx", eidx);
}

/*
 * Return the 'quickfixtextfunc' function of a quickfix/location list
 */
    स्थिर पूर्णाङ्क
qf_getprop_qftf(qf_list_T *qfl, dict_T *retdict)
{
    पूर्णाङ्क		status;

    यदि (qfl->qf_qftf_cb.cb_name != रिक्त)
    {
	typval_T	tv;

	put_callback(&qfl->qf_qftf_cb, &tv);
	status = dict_add_tv(retdict, "quickfixtextfunc", &tv);
	clear_tv(&tv);
    }
    अन्यथा
	status = dict_add_string_len(retdict, "quickfixtextfunc", (char_u *)"", 0);

    फर्काऊ status;
}

/*
 * Return quickfix/location list details (title) as a
 * dictionary. 'what' contains the details to return. If 'list_idx' is -1,
 * then current list is used. Otherwise the specified list is used.
 */
    स्थिर पूर्णाङ्क
qf_get_properties(win_T *wp, dict_T *what, dict_T *retdict)
{
    qf_info_T	*qi = ql_info;
    qf_list_T	*qfl;
    पूर्णाङ्क		status = OK;
    पूर्णाङ्क		qf_idx = INVALID_QFIDX;
    पूर्णाङ्क		eidx = 0;
    dictitem_T	*di;
    पूर्णाङ्क		flags = QF_GETLIST_NONE;

    यदि ((di = dict_find(what, (char_u *)"lines", -1)) != रिक्त)
	फर्काऊ qf_get_list_from_पङ्क्तिहरू(what, di, retdict);

    यदि (wp != रिक्त)
	qi = GET_LOC_LIST(wp);
    अन्यथा यदि (qi == रिक्त)
	फर्काऊ FAIL;

    flags = qf_getprop_keys2flags(what, (wp != रिक्त));

    यदि (!qf_थाक_empty(qi))
	qf_idx = qf_getprop_qfidx(qi, what);

    // List is not present or is empty
    यदि (qf_थाक_empty(qi) || qf_idx == INVALID_QFIDX)
	फर्काऊ qf_getprop_defaults(qi, flags, wp != रिक्त, retdict);

    qfl = qf_get_list(qi, qf_idx);

    // If an entry index is specified, use that
    यदि ((di = dict_find(what, (char_u *)"idx", -1)) != रिक्त)
    {
	यदि (di->di_tv.v_type != VAR_सङ्ख्या)
	    फर्काऊ FAIL;
	eidx = di->di_tv.vval.v_सङ्ख्या;
    }

    यदि (flags & QF_GETLIST_TITLE)
	status = qf_getprop_title(qfl, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_NR))
	status = dict_add_सङ्ख्या(retdict, "nr", qf_idx + 1);
    यदि ((status == OK) && (flags & QF_GETLIST_WINID))
	status = dict_add_सङ्ख्या(retdict, "winid", qf_winid(qi));
    यदि ((status == OK) && (flags & QF_GETLIST_वस्तुहरू))
	status = qf_getprop_वस्तुहरू(qi, qf_idx, eidx, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_CONTEXT))
	status = qf_getprop_ctx(qfl, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_ID))
	status = dict_add_सङ्ख्या(retdict, "id", qfl->qf_id);
    यदि ((status == OK) && (flags & QF_GETLIST_IDX))
	status = qf_getprop_idx(qfl, eidx, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_SIZE))
	status = dict_add_सङ्ख्या(retdict, "size", qfl->qf_गन्ती);
    यदि ((status == OK) && (flags & QF_GETLIST_TICK))
	status = dict_add_सङ्ख्या(retdict, "changedtick", qfl->qf_changedtick);
    यदि ((status == OK) && (wp != रिक्त) && (flags & QF_GETLIST_FILEWINID))
	status = qf_getprop_filewinid(wp, qi, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_QFBUFNR))
	status = qf_getprop_qfbufnr(qi, retdict);
    यदि ((status == OK) && (flags & QF_GETLIST_QFTF))
	status = qf_getprop_qftf(qfl, retdict);

    फर्काऊ status;
}

/*
 * Add a new quickfix entry to list at 'qf_idx' in the stack 'qi' from the
 * items in the dict 'd'. If it is a valid error entry, then set 'valid_entry'
 * to TRUE.
 */
    स्थिर पूर्णाङ्क
qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	पूर्णाङ्क		first_entry,
	पूर्णाङ्क		*valid_entry)
{
    स्थिर पूर्णाङ्क	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    पूर्णाङ्क		bufnum, valid, status, col, end_col, vcol, nr;
    दीर्घ	lnum, end_lnum;

    यदि (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "bufnr");
    lnum = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "lnum");
    end_lnum = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "end_lnum");
    col = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "col");
    end_col = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "end_col");
    vcol = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "vcol");
    nr = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    यदि (text == रिक्त)
	text = vim_strsave((char_u *)"");
    typval_T user_data;
    user_data.v_type = VAR_UNKNOWN;
    dict_get_tv(d, "user_data", &user_data);

    valid = TRUE;
    यदि ((filename == रिक्त && bufnum == 0) || (lnum == 0 && pattern == रिक्त))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    यदि (bufnum != 0 && (buflist_findnr(bufnum) == रिक्त))
    {
	यदि (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    यदि (dict_has_साँचो(d, "valid"))
	valid = (पूर्णाङ्क)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			रिक्त,		// dir
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
			type == रिक्त ? NUL : *type,
			&user_data,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);
    clear_tv(&user_data);

    यदि (valid)
	*valid_entry = TRUE;

    फर्काऊ status;
}

/*
 * Check if `entry` is closer to the target than `other_entry`.
 *
 * Only returns TRUE if `entry` is definitively closer. If it's further
 * away, or there's not enough information to tell, return FALSE.
 */
    स्थिर पूर्णाङ्क
entry_is_closer_to_target(
	qfline_T	*entry,
	qfline_T	*other_entry,
	पूर्णाङ्क		target_fnum,
	पूर्णाङ्क		target_lnum,
	पूर्णाङ्क		target_col)
{
    // First, compare entries to target file.
    यदि (!target_fnum)
	// Without a target file, we can't know which is closer.
	फर्काऊ FALSE;

    पूर्णाङ्क is_target_file = entry->qf_fnum && entry->qf_fnum == target_fnum;
    पूर्णाङ्क other_is_target_file = other_entry->qf_fnum && other_entry->qf_fnum == target_fnum;
    यदि (!is_target_file && other_is_target_file)
	फर्काऊ FALSE;
    अन्यथा यदि (is_target_file && !other_is_target_file)
	फर्काऊ TRUE;

    // Both entries are pointing at the exact same file. Now compare line
    // numbers.
    यदि (!target_lnum)
	// Without a target line number, we can't know which is closer.
	फर्काऊ FALSE;

    पूर्णाङ्क पङ्क्ति_distance = entry->qf_lnum ? द_असल(entry->qf_lnum - target_lnum) : पूर्णांक_उच्च;
    पूर्णाङ्क other_पङ्क्ति_distance = other_entry->qf_lnum ? द_असल(other_entry->qf_lnum - target_lnum) : पूर्णांक_उच्च;
    यदि (पङ्क्ति_distance > other_पङ्क्ति_distance)
	फर्काऊ FALSE;
    अन्यथा यदि (पङ्क्ति_distance < other_पङ्क्ति_distance)
	फर्काऊ TRUE;

    // Both entries are pointing at the exact same line number (or no line
    // number at all). Now compare columns.
    यदि (!target_col)
	// Without a target column, we can't know which is closer.
	फर्काऊ FALSE;

    पूर्णाङ्क column_distance = entry->qf_col ? असल(entry->qf_col - target_col) : पूर्णांक_उच्च;
    पूर्णाङ्क other_column_distance = other_entry->qf_col ? असल(other_entry->qf_col - target_col): पूर्णांक_उच्च;
    यदि (column_distance > other_column_distance)
	फर्काऊ FALSE;
    अन्यथा यदि (column_distance < other_column_distance)
	फर्काऊ TRUE;

    // It's a complete tie! The exact same file, line, and column.
    फर्काऊ FALSE;
}

/*
 * Add list of entries to quickfix/location list. Each list entry is
 * a dictionary with item information.
 */
    स्थिर पूर्णाङ्क
qf_add_entries(
	qf_info_T	*qi,
	पूर्णाङ्क		qf_idx,
	list_T		*list,
	char_u		*title,
	पूर्णाङ्क		action)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);
    listitem_T	*li;
    dict_T	*d;
    qfline_T	*old_last = रिक्त;
    पूर्णाङ्क		retval = OK;
    पूर्णाङ्क		valid_entry = FALSE;

    // If there's an entry selected in the quickfix list, remember its location
    // (file, line, column), so we can select the nearest entry in the updated
    // quickfix list.
    पूर्णाङ्क prev_fnum = 0;
    पूर्णाङ्क prev_lnum = 0;
    पूर्णाङ्क prev_col = 0;
    यदि (qfl->qf_ptr)
    {
	prev_fnum = qfl->qf_ptr->qf_fnum;
	prev_lnum = qfl->qf_ptr->qf_lnum;
	prev_col = qfl->qf_ptr->qf_col;
    }

    पूर्णाङ्क select_first_entry = FALSE;
    पूर्णाङ्क select_nearest_entry = FALSE;

    यदि (action == ' ' || qf_idx == qi->qf_listcount)
    {
	select_first_entry = TRUE;
	// make place for a new list
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
	qfl = qf_get_list(qi, qf_idx);
    }
    अन्यथा यदि (action == 'a')
    {
	यदि (qf_list_empty(qfl))
	    // Appending to empty list, select first entry.
	    select_first_entry = TRUE;
	अन्यथा
	    // Adding to existing list, use last entry.
	    old_last = qfl->qf_last;
    }
    अन्यथा यदि (action == 'r')
    {
	select_first_entry = TRUE;
	qf_free_वस्तुहरू(qfl);
	qf_store_title(qfl, title);
    }
    अन्यथा यदि (action == 'u')
    {
	select_nearest_entry = TRUE;
	qf_free_वस्तुहरू(qfl);
	qf_store_title(qfl, title);
    }

    qfline_T *entry_to_select = रिक्त;
    पूर्णाङ्क entry_to_select_index = 0;

    FOR_ALL_LIST_वस्तुहरू(list, li)
    {
	यदि (li->li_tv.v_type != VAR_DICT)
	    जारी; // Skip non-dict items

	d = li->li_tv.vval.v_dict;
	यदि (d == रिक्त)
	    जारी;

	retval = qf_add_entry_from_dict(qfl, d, li == list->lv_first,
								&valid_entry);
	यदि (retval == QF_FAIL)
	    रोक;

	qfline_T *entry = qfl->qf_last;
	यदि (
	    (select_first_entry && entry_to_select == रिक्त) ||
	    (select_nearest_entry &&
		(entry_to_select == रिक्त ||
		 entry_is_closer_to_target(entry, entry_to_select, prev_fnum,
					   prev_lnum, prev_col))))
	{
	    entry_to_select = entry;
	    entry_to_select_index = qfl->qf_गन्ती;
	}
    }

    // Check if any valid error entries are added to the list.
    यदि (valid_entry)
	qfl->qf_nonevalid = FALSE;
    अन्यथा यदि (qfl->qf_index == 0)
	// no valid entry
	qfl->qf_nonevalid = TRUE;

    // Set the current error.
    यदि (entry_to_select)
    {
	qfl->qf_ptr = entry_to_select;
	qfl->qf_index = entry_to_select_index;
    }

    // Don't update the cursor in quickfix window when appending entries
    qf_update_buffer(qi, old_last);

    फर्काऊ retval;
}

/*
 * Get the quickfix list index from 'nr' or 'id'
 */
    स्थिर पूर्णाङ्क
qf_setprop_get_qfidx(
	qf_info_T	*qi,
	dict_T		*what,
	पूर्णाङ्क		action,
	पूर्णाङ्क		*newlist)
{
    dictitem_T	*di;
    पूर्णाङ्क		qf_idx = qi->qf_curlist;    // default is the current list

    यदि ((di = dict_find(what, (char_u *)"nr", -1)) != रिक्त)
    {
	// Use the specified quickfix/location list
	यदि (di->di_tv.v_type == VAR_सङ्ख्या)
	{
	    // for zero use the current list
	    यदि (di->di_tv.vval.v_सङ्ख्या != 0)
		qf_idx = di->di_tv.vval.v_सङ्ख्या - 1;

	    यदि ((action == ' ' || action == 'a') && qf_idx == qi->qf_listcount)
	    {
		// When creating a new list, accept qf_idx pointing to the next
		// non-available list and add the new list at the end of the
		// stack.
		*newlist = TRUE;
		qf_idx = qf_थाक_empty(qi) ? 0 : qi->qf_listcount - 1;
	    }
	    अन्यथा यदि (qf_idx < 0 || qf_idx >= qi->qf_listcount)
		फर्काऊ INVALID_QFIDX;
	    अन्यथा यदि (action != ' ')
		*newlist = FALSE;	// use the specified list
	}
	अन्यथा यदि (di->di_tv.v_type == VAR_STRING
		&& di->di_tv.vval.v_string != रिक्त
		&& STRCMP(di->di_tv.vval.v_string, "$") == 0)
	{
	    यदि (!qf_थाक_empty(qi))
		qf_idx = qi->qf_listcount - 1;
	    अन्यथा यदि (*newlist)
		qf_idx = 0;
	    अन्यथा
		फर्काऊ INVALID_QFIDX;
	}
	अन्यथा
	    फर्काऊ INVALID_QFIDX;
    }

    यदि (!*newlist && (di = dict_find(what, (char_u *)"id", -1)) != रिक्त)
    {
	// Use the quickfix/location list with the specified id
	यदि (di->di_tv.v_type != VAR_सङ्ख्या)
	    फर्काऊ INVALID_QFIDX;

	फर्काऊ qf_id2nr(qi, di->di_tv.vval.v_सङ्ख्या);
    }

    फर्काऊ qf_idx;
}

/*
 * Set the quickfix list title.
 */
    स्थिर पूर्णाङ्क
qf_setprop_title(qf_info_T *qi, पूर्णाङ्क qf_idx, dict_T *what, dictitem_T *di)
{
    qf_list_T	*qfl = qf_get_list(qi, qf_idx);

    यदि (di->di_tv.v_type != VAR_STRING)
	फर्काऊ FAIL;

    vim_free(qfl->qf_title);
    qfl->qf_title = dict_get_string(what, "title", TRUE);
    यदि (qf_idx == qi->qf_curlist)
	qf_update_win_titlevar(qi);

    फर्काऊ OK;
}

/*
 * Set quickfix list items/entries.
 */
    स्थिर पूर्णाङ्क
qf_setprop_वस्तुहरू(qf_info_T *qi, पूर्णाङ्क qf_idx, dictitem_T *di, पूर्णाङ्क action)
{
    पूर्णाङ्क		retval = FAIL;
    char_u	*title_save;

    यदि (di->di_tv.v_type != VAR_LIST)
	फर्काऊ FAIL;

    title_save = vim_strsave(qi->qf_lists[qf_idx].qf_title);
    retval = qf_add_entries(qi, qf_idx, di->di_tv.vval.v_list,
	    title_save, action == ' ' ? 'a' : action);
    vim_free(title_save);

    फर्काऊ retval;
}

/*
 * Set quickfix list items/entries from a list of lines.
 */
    स्थिर पूर्णाङ्क
qf_setprop_वस्तुहरू_from_पङ्क्तिहरू(
	qf_info_T	*qi,
	पूर्णाङ्क		qf_idx,
	dict_T		*what,
	dictitem_T	*di,
	पूर्णाङ्क		action)
{
    char_u	*errorformat = p_efm;
    dictitem_T	*efm_di;
    पूर्णाङ्क		retval = FAIL;

    // Use the user supplied errorformat settings (if present)
    यदि ((efm_di = dict_find(what, (char_u *)"efm", -1)) != रिक्त)
    {
	यदि (efm_di->di_tv.v_type != VAR_STRING ||
		efm_di->di_tv.vval.v_string == रिक्त)
	    फर्काऊ FAIL;
	errorformat = efm_di->di_tv.vval.v_string;
    }

    // Only a List value is supported
    यदि (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == रिक्त)
	फर्काऊ FAIL;

    यदि (action == 'r' || action == 'u')
	qf_free_वस्तुहरू(&qi->qf_lists[qf_idx]);
    यदि (qf_init_ext(qi, qf_idx, रिक्त, रिक्त, &di->di_tv, errorformat,
		FALSE, (linenr_T)0, (linenr_T)0, रिक्त, रिक्त) >= 0)
	retval = OK;

    फर्काऊ retval;
}

/*
 * Set quickfix list context.
 */
    स्थिर पूर्णाङ्क
qf_setprop_context(qf_list_T *qfl, dictitem_T *di)
{
    typval_T	*ctx;

    free_tv(qfl->qf_ctx);
    ctx =  alloc_tv();
    यदि (ctx != रिक्त)
	copy_tv(&di->di_tv, ctx);
    qfl->qf_ctx = ctx;

    फर्काऊ OK;
}

/*
 * Set the current index in the specified quickfix list
 */
    स्थिर पूर्णाङ्क
qf_setprop_curidx(qf_info_T *qi, qf_list_T *qfl, dictitem_T *di)
{
    पूर्णाङ्क		denote = FALSE;
    पूर्णाङ्क		newidx;
    पूर्णाङ्क		old_qfidx;
    qfline_T	*qf_ptr;

    // If the specified index is '$', then use the last entry
    यदि (di->di_tv.v_type == VAR_STRING
	    && di->di_tv.vval.v_string != रिक्त
	    && STRCMP(di->di_tv.vval.v_string, "$") == 0)
	newidx = qfl->qf_गन्ती;
    अन्यथा
    {
	// Otherwise use the specified index
	newidx = tv_get_सङ्ख्या_chk(&di->di_tv, &denote);
	यदि (denote)
	    फर्काऊ FAIL;
    }

    यदि (newidx < 1)		// sanity check
	फर्काऊ FAIL;
    यदि (newidx > qfl->qf_गन्ती)
	newidx = qfl->qf_गन्ती;

    old_qfidx = qfl->qf_index;
    qf_ptr = get_nth_entry(qfl, newidx, &newidx);
    यदि (qf_ptr == रिक्त)
	फर्काऊ FAIL;
    qfl->qf_ptr = qf_ptr;
    qfl->qf_index = newidx;

    // If the current list is modified and it is displayed in the quickfix
    // window, then Update it.
    यदि (qf_get_curlist(qi)->qf_id == qfl->qf_id)
	qf_win_pos_update(qi, old_qfidx);

    फर्काऊ OK;
}

/*
 * Set the 'quickfixtextfunc' in the specified quickfix/location list
 */
    स्थिर पूर्णाङ्क
qf_setprop_qftf(qf_info_T *qi UNUSED, qf_list_T *qfl, dictitem_T *di)
{
    callback_T	cb;

    यदि (check_restricted() || check_secure())
	फर्काऊ FAIL;

    free_callback(&qfl->qf_qftf_cb);
    cb = get_callback(&di->di_tv);
    यदि (cb.cb_name == रिक्त || *cb.cb_name == NUL)
	फर्काऊ OK;
    set_callback(&qfl->qf_qftf_cb, &cb);
    फर्काऊ OK;
}

/*
 * Set quickfix/location list properties (title, items, context).
 * Also used to add items from parsing a list of lines.
 * Used by the setqflist() and setloclist() Vim script functions.
 */
    स्थिर पूर्णाङ्क
qf_set_properties(qf_info_T *qi, dict_T *what, पूर्णाङ्क action, char_u *title)
{
    dictitem_T	*di;
    पूर्णाङ्क		retval = FAIL;
    पूर्णाङ्क		qf_idx;
    पूर्णाङ्क		newlist = FALSE;
    qf_list_T	*qfl;

    यदि (action == ' ' || qf_थाक_empty(qi))
	newlist = TRUE;

    qf_idx = qf_setprop_get_qfidx(qi, what, action, &newlist);
    यदि (qf_idx == INVALID_QFIDX)	// List not found
	फर्काऊ FAIL;

    यदि (newlist)
    {
	qi->qf_curlist = qf_idx;
	qf_new_list(qi, title);
	qf_idx = qi->qf_curlist;
    }

    qfl = qf_get_list(qi, qf_idx);
    यदि ((di = dict_find(what, (char_u *)"title", -1)) != रिक्त)
	retval = qf_setprop_title(qi, qf_idx, what, di);
    यदि ((di = dict_find(what, (char_u *)"items", -1)) != रिक्त)
	retval = qf_setprop_वस्तुहरू(qi, qf_idx, di, action);
    यदि ((di = dict_find(what, (char_u *)"lines", -1)) != रिक्त)
	retval = qf_setprop_वस्तुहरू_from_पङ्क्तिहरू(qi, qf_idx, what, di, action);
    यदि ((di = dict_find(what, (char_u *)"context", -1)) != रिक्त)
	retval = qf_setprop_context(qfl, di);
    यदि ((di = dict_find(what, (char_u *)"idx", -1)) != रिक्त)
	retval = qf_setprop_curidx(qi, qfl, di);
    यदि ((di = dict_find(what, (char_u *)"quickfixtextfunc", -1)) != रिक्त)
	retval = qf_setprop_qftf(qi, qfl, di);

    यदि (newlist || retval == OK)
	qf_list_changed(qfl);
    यदि (newlist)
	qf_update_buffer(qi, रिक्त);

    फर्काऊ retval;
}

/*
 * Free the entire quickfix/location list stack.
 * If the quickfix/location list window is open, then clear it.
 */
    स्थिर शून्य
qf_free_थाक(win_T *wp, qf_info_T *qi)
{
    win_T	*qfwin = qf_find_win(qi);
    win_T	*llwin = रिक्त;

    यदि (qfwin != रिक्त)
    {
	// If the quickfix/location list window is open, then clear it
	यदि (qi->qf_curlist < qi->qf_listcount)
	    qf_free(qf_get_curlist(qi));
	qf_update_buffer(qi, रिक्त);
    }

    यदि (wp != रिक्त && IS_LL_WINDOW(wp))
    {
	// If in the location list window, then use the non-location list
	// window with this location list (if present)
	llwin = qf_find_win_with_loclist(qi);
	यदि (llwin != रिक्त)
	    wp = llwin;
    }

    qf_free_all(wp);
    यदि (wp == रिक्त)
    {
	// quickfix list
	qi->qf_curlist = 0;
	qi->qf_listcount = 0;
    }
    अन्यथा यदि (qfwin != रिक्त)
    {
	// If the location list window is open, then create a new empty
	// location list
	qf_info_T *new_ll = qf_alloc_थाक(QFLT_LOCATION, wp->w_p_lhi);

	यदि (new_ll != रिक्त)
	{
	    new_ll->qf_bufnr = qfwin->w_buffer->b_fnum;

	    // first free the list reference in the location list window
	    ll_free_all(&qfwin->w_llist_ref);

	    qfwin->w_llist_ref = new_ll;
	    यदि (wp != qfwin)
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
    पूर्णाङ्क
set_errorlist(
	win_T	*wp,
	list_T	*list,
	पूर्णाङ्क	action,
	char_u	*title,
	dict_T	*what)
{
    qf_info_T	*qi;
    पूर्णाङ्क		retval = OK;

    यदि (wp != रिक्त)
	qi = ll_get_or_alloc_list(wp);
    अन्यथा
	qi = ql_info;
    यदि (qi == रिक्त)
	फर्काऊ FAIL;

    यदि (action == 'f')
    {
	// Free the entire quickfix or location list stack
	qf_free_थाक(wp, qi);
	फर्काऊ OK;
    }

    // A dict argument cannot be specified with a non-empty list argument
    यदि (list->lv_len != 0 && what != रिक्त)
    {
	semsg(_(e_invalid_argument_str),
			 _("cannot have both a list and a \"what\" argument"));
	फर्काऊ FAIL;
    }

    incr_quickfix_busy();

    यदि (what != रिक्त)
	retval = qf_set_properties(qi, what, action, title);
    अन्यथा
    {
	retval = qf_add_entries(qi, qi->qf_curlist, list, title, action);
	यदि (retval == OK)
	    qf_list_changed(qf_get_curlist(qi));
    }

    decr_quickfix_busy();

    फर्काऊ retval;
}

स्थिर पूर्णाङ्क mark_quickfix_user_data(qf_info_T *qi, पूर्णाङ्क copyID)
{
    पूर्णाङ्क पात = FALSE;
    लागि (पूर्णाङ्क i = 0; i < qi->qf_maxcount && !पात; ++i)
    {
	qf_list_T *qfl = &qi->qf_lists[i];
	यदि (!qfl->qf_has_user_data)
	    जारी;
	qfline_T *qfp;
	पूर्णाङ्क j;
	FOR_ALL_QFL_वस्तुहरू(qfl, qfp, j)
	{
	    typval_T* user_data = &qfp->qf_user_data;
	    यदि (user_data != रिक्त && user_data->v_type != VAR_सङ्ख्या
		&& user_data->v_type != VAR_STRING && user_data->v_type != VAR_FLOAT)
		पात = पात || set_ref_in_item(user_data, copyID, रिक्त, रिक्त, रिक्त);
	}
    }
    फर्काऊ पात;
}

/*
 * Mark the quickfix context and callback function as in use for all the lists
 * in a quickfix stack.
 */
    स्थिर पूर्णाङ्क
mark_quickfix_ctx(qf_info_T *qi, पूर्णाङ्क copyID)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		पात = FALSE;
    typval_T	*ctx;
    callback_T	*cb;

    लागि (i = 0; i < qi->qf_maxcount && !पात; ++i)
    {
	ctx = qi->qf_lists[i].qf_ctx;
	यदि (ctx != रिक्त && ctx->v_type != VAR_सङ्ख्या
		&& ctx->v_type != VAR_STRING && ctx->v_type != VAR_FLOAT)
	    पात = पात || set_ref_in_item(ctx, copyID, रिक्त, रिक्त, रिक्त);

	cb = &qi->qf_lists[i].qf_qftf_cb;
	पात = पात || set_ref_in_callback(cb, copyID);
    }

    फर्काऊ पात;
}

/*
 * Mark the context of the quickfix list and the location lists (if present) as
 * "in use". So that garbage collection doesn't free the context.
 */
    पूर्णाङ्क
set_ref_in_quickfix(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		पात = FALSE;
    tabpage_T	*tp;
    win_T	*win;

    यदि (ql_info == रिक्त)
	फर्काऊ TRUE;

    पात = mark_quickfix_ctx(ql_info, copyID);
    यदि (पात)
	फर्काऊ पात;

    पात = mark_quickfix_user_data(ql_info, copyID);
    यदि (पात)
	फर्काऊ पात;

    पात = set_ref_in_callback(&qftf_cb, copyID);
    यदि (पात)
	फर्काऊ पात;

    FOR_ALL_TAB_WINDOWS(tp, win)
    {
	यदि (win->w_llist != रिक्त)
	{
	    पात = mark_quickfix_ctx(win->w_llist, copyID);
	    यदि (पात)
		फर्काऊ पात;

	    पात = mark_quickfix_user_data(win->w_llist, copyID);
	    यदि (पात)
		फर्काऊ पात;
	}
	यदि (IS_LL_WINDOW(win) && (win->w_llist_ref->qf_refcount == 1))
	{
	    // In a location list window and none of the other windows is
	    // referring to this location list. Mark the location list
	    // context as still in use.
	    पात = mark_quickfix_ctx(win->w_llist_ref, copyID);
	    यदि (पात)
		फर्काऊ पात;

	    पात = mark_quickfix_user_data(win->w_llist_ref, copyID);
	    यदि (पात)
		फर्काऊ पात;
	}
    }

    फर्काऊ पात;
}
# endif

/*
 * Return the autocmd name for the :cbuffer Ex commands
 */
    स्थिर char_u *
cbuffer_get_auname(cmdidx_T cmdidx)
{
    विकल्प (cmdidx)
    {
	पक्ष CMD_cbuffer:	फर्काऊ (char_u *)"cbuffer";
	पक्ष CMD_cgetbuffer:	फर्काऊ (char_u *)"cgetbuffer";
	पक्ष CMD_caddbuffer:	फर्काऊ (char_u *)"caddbuffer";
	पक्ष CMD_lbuffer:	फर्काऊ (char_u *)"lbuffer";
	पक्ष CMD_lgetbuffer:	फर्काऊ (char_u *)"lgetbuffer";
	पक्ष CMD_laddbuffer:	फर्काऊ (char_u *)"laddbuffer";
	स्वभाव:		फर्काऊ रिक्त;
    }
}

/*
 * Process and validate the arguments passed to the :cbuffer, :caddbuffer,
 * :cgetbuffer, :lbuffer, :laddbuffer, :lgetbuffer Ex commands.
 */
    स्थिर पूर्णाङ्क
cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = रिक्त;

    यदि (*eap->arg == NUL)
	buf = curbuf;
    अन्यथा यदि (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(म_से_प((वर्ण *)eap->arg));

    यदि (buf == रिक्त)
    {
	emsg(_(e_invalid_argument));
	फर्काऊ FAIL;
    }

    यदि (buf->b_ml.ml_mfp == रिक्त)
    {
	emsg(_(e_buffer_is_not_loaded));
	फर्काऊ FAIL;
    }

    यदि (eap->addr_गन्ती == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_पङ्क्ति_गन्ती;
    }

    यदि (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_पङ्क्ति_गन्ती
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_पङ्क्ति_गन्ती)
    {
	emsg(_(e_invalid_range));
	फर्काऊ FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    फर्काऊ OK;
}

/*
 * ":[range]cbuffer [bufnr]" command.
 * ":[range]caddbuffer [bufnr]" command.
 * ":[range]cgetbuffer [bufnr]" command.
 * ":[range]lbuffer [bufnr]" command.
 * ":[range]laddbuffer [bufnr]" command.
 * ":[range]lgetbuffer [bufnr]" command.
 */
    शून्य
ex_cbuffer(exarg_T *eap)
{
    buf_T	*buf = रिक्त;
    qf_info_T	*qi;
    char_u	*au_name = रिक्त;
    पूर्णाङ्क		res;
    int_u	save_qfid;
    win_T	*wp = रिक्त;
    char_u	*qf_title;
    linenr_T	line1;
    linenr_T	line2;

    au_name = cbuffer_get_auname(eap->cmdidx);
    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	यदि (aborting())
	    फर्काऊ;
# endif
    }

    // Must come after autocommands.
    qi = qf_cmd_get_or_alloc_थाक(eap, &wp);
    यदि (qi == रिक्त)
	फर्काऊ;

    यदि (cbuffer_process_args(eap, &buf, &line1, &line2) == FAIL)
	फर्काऊ;

    qf_title = qf_cmdtitle(*eap->cmdlinep);

    यदि (buf->b_sfname)
    {
	vim_snprintf((वर्ण *)IObuff, IOSIZE, "%s (%s)",
		(वर्ण *)qf_title, (वर्ण *)buf->b_sfname);
	qf_title = IObuff;
    }

    incr_quickfix_busy();

    res = qf_init_ext(qi, qi->qf_curlist, रिक्त, buf, रिक्त, p_efm,
	    (eap->cmdidx != CMD_caddbuffer
	     && eap->cmdidx != CMD_laddbuffer),
	    line1, line2,
	    qf_title, रिक्त);
    यदि (qf_थाक_empty(qi))
    {
	decr_quickfix_busy();
	फर्काऊ;
    }
    यदि (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    यदि (au_name != रिक्त)
    {
	buf_T *curbuf_old = curbuf;

	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, curbuf->b_fname,
								TRUE, curbuf);
	यदि (curbuf != curbuf_old)
	    // Autocommands changed buffer, don't jump now, "qi" may
	    // be invalid.
	    res = 0;
    }
    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    यदि (res > 0 && (eap->cmdidx == CMD_cbuffer ||
		eap->cmdidx == CMD_lbuffer)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}

# यदि defined(FEAT_EVAL)
/*
 * Return the autocmd name for the :cexpr Ex commands.
 */
    char_u *
cexpr_get_auname(cmdidx_T cmdidx)
{
    विकल्प (cmdidx)
    {
	पक्ष CMD_cexpr:	    फर्काऊ (char_u *)"cexpr";
	पक्ष CMD_cgetexpr:  फर्काऊ (char_u *)"cgetexpr";
	पक्ष CMD_caddexpr:  फर्काऊ (char_u *)"caddexpr";
	पक्ष CMD_lexpr:	    फर्काऊ (char_u *)"lexpr";
	पक्ष CMD_lgetexpr:  फर्काऊ (char_u *)"lgetexpr";
	पक्ष CMD_laddexpr:  फर्काऊ (char_u *)"laddexpr";
	स्वभाव:	    फर्काऊ रिक्त;
    }
}

    पूर्णाङ्क
trigger_cexpr_autocmd(पूर्णाङ्क cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	यदि (aborting())
	    फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

    पूर्णाङ्क
cexpr_core(exarg_T *eap, typval_T *tv)
{
    qf_info_T	*qi;
    win_T	*wp = रिक्त;

    qi = qf_cmd_get_or_alloc_थाक(eap, &wp);
    यदि (qi == रिक्त)
	फर्काऊ FAIL;

    यदि ((tv->v_type == VAR_STRING && tv->vval.v_string != रिक्त)
	    || (tv->v_type == VAR_LIST && tv->vval.v_list != रिक्त))
    {
	पूर्णाङ्क	res;
	int_u	save_qfid;
	char_u	*au_name = cexpr_get_auname(eap->cmdidx);

	incr_quickfix_busy();
	res = qf_init_ext(qi, qi->qf_curlist, रिक्त, रिक्त, tv, p_efm,
			(eap->cmdidx != CMD_caddexpr
			 && eap->cmdidx != CMD_laddexpr),
			     (linenr_T)0, (linenr_T)0,
			     qf_cmdtitle(*eap->cmdlinep), रिक्त);
	यदि (qf_थाक_empty(qi))
	{
	    decr_quickfix_busy();
	    फर्काऊ FAIL;
	}
	यदि (res >= 0)
	    qf_list_changed(qf_get_curlist(qi));

	// Remember the current quickfix list identifier, so that we can
	// check for autocommands changing the current quickfix list.
	save_qfid = qf_get_curlist(qi)->qf_id;
	यदि (au_name != रिक्त)
	    apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					    curbuf->b_fname, TRUE, curbuf);

	// Jump to the first error for a new list and if autocmds didn't
	// free the list.
	यदि (res > 0 && (eap->cmdidx == CMD_cexpr || eap->cmdidx == CMD_lexpr)
		&& qflist_valid(wp, save_qfid))
	    // display the first error
	    qf_jump_first(qi, save_qfid, eap->forceit);
	decr_quickfix_busy();
	फर्काऊ OK;
    }

    emsg(_(e_string_or_list_expected));
    फर्काऊ FAIL;
}

/*
 * ":cexpr {expr}", ":cgetexpr {expr}", ":caddexpr {expr}" command.
 * ":lexpr {expr}", ":lgetexpr {expr}", ":laddexpr {expr}" command.
 * Also: ":caddexpr", ":cgetexpr", "laddexpr" and "laddexpr".
 */
    शून्य
ex_cexpr(exarg_T *eap)
{
    typval_T	*tv;

    यदि (trigger_cexpr_autocmd(eap->cmdidx) == FAIL)
	फर्काऊ;

    // Evaluate the expression.  When the result is a string or a list we can
    // use it to fill the errorlist.
    tv = eval_अभिव्यक्ति(eap->arg, eap);
    यदि (tv == रिक्त)
	फर्काऊ;

    (शून्य)cexpr_core(eap, tv);
    free_tv(tv);
}
# endif

/*
 * Get the location list for ":lhelpgrep"
 */
    स्थिर qf_info_T *
hgr_get_ll(पूर्णाङ्क *new_ll)
{
    win_T	*wp;
    qf_info_T	*qi;

    // If the current window is a help window, then use it
    यदि (bt_help(curwin->w_buffer))
	wp = curwin;
    अन्यथा
	// Find an existing help window
	wp = qf_find_help_win();

    यदि (wp == रिक्त)	    // Help window not found
	qi = रिक्त;
    अन्यथा
	qi = wp->w_llist;

    यदि (qi == रिक्त)
    {
	// Allocate a new location list for help text matches
	यदि ((qi = qf_alloc_थाक(QFLT_LOCATION, 1)) == रिक्त)
	    फर्काऊ रिक्त;
	*new_ll = TRUE;
    }

    फर्काऊ qi;
}

/*
 * Search for a pattern in a help file.
 */
    स्थिर शून्य
hgr_खोज_file(
	qf_list_T *qfl,
	char_u *fname,
	vimconv_T *p_vc,
	regmatch_T *p_regmatch)
{
    खाता	*fd;
    दीर्घ	lnum;

    fd = mch_fopen((वर्ण *)fname, "r");
    यदि (fd == रिक्त)
	फर्काऊ;

    lnum = 1;
    जबसम्म (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
    {
	char_u    *पङ्क्ति = IObuff;

	// Convert a line if 'encoding' is not utf-8 and
	// the line contains a non-ASCII character.
	यदि (p_vc->vc_type != CONV_NONE && has_non_ascii(IObuff))
	{
	    पङ्क्ति = string_convert(p_vc, IObuff, रिक्त);
	    यदि (पङ्क्ति == रिक्त)
		पङ्क्ति = IObuff;
	}

	यदि (vim_regexec(p_regmatch, पङ्क्ति, (colnr_T)0))
	{
	    पूर्णाङ्क	l = (पूर्णाङ्क)STRLEN(पङ्क्ति);

	    // remove trailing CR, LF, spaces, etc.
	    जबसम्म (l > 0 && पङ्क्ति[l - 1] <= ' ')
		पङ्क्ति[--l] = NUL;

	    यदि (qf_add_entry(qfl,
			रिक्त,	// dir
			fname,
			रिक्त,
			0,
			पङ्क्ति,
			lnum,
			0,
			(पूर्णाङ्क)(p_regmatch->startp[0] - पङ्क्ति)
			+ 1,	// col
			(पूर्णाङ्क)(p_regmatch->endp[0] - पङ्क्ति)
			+ 1,	// end_col
			FALSE,	// vis_col
			रिक्त,	// search pattern
			0,	// nr
			1,	// type
			रिक्त,	// user_data
			TRUE	// valid
			) == QF_FAIL)
	    {
		got_int = TRUE;
		यदि (पङ्क्ति != IObuff)
		    vim_free(पङ्क्ति);
		रोक;
	    }
	}
	यदि (पङ्क्ति != IObuff)
	    vim_free(पङ्क्ति);
	++lnum;
	पङ्क्ति_breakcheck();
    }
    ख_बंद(fd);
}

/*
 * Search for a pattern in all the help files in the doc directory under
 * the given directory.
 */
    स्थिर शून्य
hgr_खोज_files_in_dir(
	qf_list_T *qfl,
	char_u *स_नाम,
	regmatch_T *p_regmatch,
	vimconv_T *p_vc
# ifdef FEAT_MULTI_LANG
	, char_u *lang
# endif
	)
{
    पूर्णाङ्क		fcount;
    char_u	**fnames;
    पूर्णाङ्क		fi;

    // Find all "*.txt" and "*.??x" files in the "doc" directory.
    add_pathsep(स_नाम);
    STRCAT(स_नाम, "doc/*.\\(txt\\|??x\\)");
    यदि (gen_expand_wildcards(1, &स_नाम, &fcount,
		&fnames, EW_FILE|EW_SILENT) == OK
	    && fcount > 0)
    {
	लागि (fi = 0; fi < fcount && !got_int; ++fi)
	{
# ifdef FEAT_MULTI_LANG
	    // Skip files for a different language.
	    यदि (lang != रिक्त
		    && STRNICMP(lang, fnames[fi]
				    + STRLEN(fnames[fi]) - 3, 2) != 0
		    && !(STRNICMP(lang, "en", 2) == 0
			&& STRNICMP("txt", fnames[fi]
			    + STRLEN(fnames[fi]) - 3, 3) == 0))
		जारी;
# endif

	    hgr_खोज_file(qfl, fnames[fi], p_vc, p_regmatch);
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
    स्थिर शून्य
hgr_खोज_in_rtp(qf_list_T *qfl, regmatch_T *p_regmatch, char_u *lang)
{
    char_u	*p;

    vimconv_T	vc;

    // Help files are in utf-8 or latin1, convert lines when 'encoding'
    // differs.
    vc.vc_type = CONV_NONE;
    यदि (!enc_utf8)
	convert_setup(&vc, (char_u *)"utf-8", p_enc);

    // Go through all the directories in 'runtimepath'
    p = p_rtp;
    जबसम्म (*p != NUL && !got_int)
    {
	copy_option_part(&p, NameBuff, MAXPATHL, ",");

	hgr_खोज_files_in_dir(qfl, NameBuff, p_regmatch, &vc
# ifdef FEAT_MULTI_LANG
		, lang
# endif
		);
    }

    यदि (vc.vc_type != CONV_NONE)
	convert_setup(&vc, रिक्त, रिक्त);
}

/*
 * ":helpgrep {pattern}"
 */
    शून्य
ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    पूर्णाङ्क		save_cpo_allocated;
    qf_info_T	*qi = ql_info;
    पूर्णाङ्क		new_qi = FALSE;
    char_u	*au_name =  रिक्त;
    char_u	*lang = रिक्त;
    पूर्णाङ्क		updated = FALSE;

    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_helpgrep:  au_name = (char_u *)"helpgrep"; रोक;
	पक्ष CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; रोक;
	स्वभाव: रोक;
    }
    यदि (au_name != रिक्त && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
# ifdef FEAT_EVAL
	यदि (aborting())
	    फर्काऊ;
# endif
    }

    यदि (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	यदि (qi == रिक्त)
	    फर्काऊ;
    }
    अन्यथा यदि (qi == रिक्त)
    {
	emsg(_(e_no_quickfix_थाक));
	फर्काऊ;
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
    यदि (regmatch.regprog != रिक्त)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_खोज_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    यदि (p_cpo == empty_option)
	p_cpo = save_cpo;
    अन्यथा
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	यदि (*p_cpo == NUL)
	    set_option_मान_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	यदि (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    यदि (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, रिक्त);

    यदि (au_name != रिक्त)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	यदि (!new_qi && IS_LL_थाक(qi) && qf_find_win_with_loclist(qi) == रिक्त)
	{
	    decr_quickfix_busy();
	    फर्काऊ;
	}
    }

    // Jump to first match.
    यदि (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    अन्यथा
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    यदि (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	यदि (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    यदि (new_qi)
		ll_free_all(&qi);
	}
	अन्यथा यदि (curwin->w_llist == रिक्त && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}

# यदि defined(EXITFREE)
    शून्य
free_quickfix(शून्य)
{
    win_T	*win;
    tabpage_T	*tab;

    qf_free_all(रिक्त);
    // Free all location lists
    FOR_ALL_TAB_WINDOWS(tab, win)
	qf_free_all(win);

    ga_clear(&qfga);
}
# endif

#endif // FEAT_QUICKFIX

#यदि defined(FEAT_EVAL)
# ifdef FEAT_QUICKFIX
    स्थिर शून्य
get_qf_loc_list(पूर्णाङ्क is_qf, win_T *wp, typval_T *what_arg, typval_T *rettv)
{
    यदि (what_arg->v_type == VAR_UNKNOWN)
    {
	यदि (rettv_list_alloc(rettv) == OK)
	    यदि (is_qf || wp != रिक्त)
		(शून्य)get_errorlist(रिक्त, wp, -1, 0, rettv->vval.v_list);
    }
    अन्यथा
    {
	यदि (rettv_dict_alloc(rettv) == OK)
	    यदि (is_qf || (wp != रिक्त))
	    {
		यदि (what_arg->v_type == VAR_DICT)
		{
		    dict_T	*d = what_arg->vval.v_dict;

		    यदि (d != रिक्त)
			qf_get_properties(wp, d, rettv->vval.v_dict);
		}
		अन्यथा
		    emsg(_(e_dictionary_required));
	    }
    }
}
# endif

/*
 * "getloclist()" function
 */
    शून्य
f_getloclist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    win_T	*wp;

    यदि (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	फर्काऊ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    get_qf_loc_list(FALSE, wp, &argvars[1], rettv);
# endif
}

/*
 * "getqflist()" function
 */
    शून्य
f_getqflist(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_QUICKFIX
    यदि (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	फर्काऊ;

    get_qf_loc_list(TRUE, रिक्त, &argvars[0], rettv);
# endif
}

/*
 * Used by "setqflist()" and "setloclist()" functions
 */
    स्थिर शून्य
set_qf_ll_list(
    win_T	*wp UNUSED,
    typval_T	*list_arg UNUSED,
    typval_T	*action_arg UNUSED,
    typval_T	*what_arg UNUSED,
    typval_T	*rettv)
{
# ifdef FEAT_QUICKFIX
    char_u	*act;
    पूर्णाङ्क		action = 0;
    स्थिर पूर्णाङ्क	recursive = 0;
# endif

    rettv->vval.v_सङ्ख्या = -1;

# ifdef FEAT_QUICKFIX
    यदि (list_arg->v_type != VAR_LIST)
	emsg(_(e_list_required));
    अन्यथा यदि (recursive != 0)
	emsg(_(e_autocommand_caused_recursive_behavior));
    अन्यथा
    {
	list_T  *l = list_arg->vval.v_list;
	dict_T	*what = रिक्त;
	पूर्णाङ्क	valid_dict = TRUE;

	यदि (action_arg->v_type == VAR_STRING)
	{
	    act = tv_get_string_chk(action_arg);
	    यदि (act == रिक्त)
		फर्काऊ;		// type error; errmsg already given
	    यदि ((*act == 'a' || *act == 'r' || *act == 'u' || *act == ' ' || *act == 'f') &&
		    act[1] == NUL)
		action = *act;
	    अन्यथा
		semsg(_(e_invalid_action_str_1), act);
	}
	अन्यथा यदि (action_arg->v_type == VAR_UNKNOWN)
	    action = ' ';
	अन्यथा
	    emsg(_(e_string_required));

	यदि (action_arg->v_type != VAR_UNKNOWN
		&& what_arg->v_type != VAR_UNKNOWN)
	{
	    यदि (what_arg->v_type == VAR_DICT && what_arg->vval.v_dict != रिक्त)
		what = what_arg->vval.v_dict;
	    अन्यथा
	    {
		emsg(_(e_dictionary_required));
		valid_dict = FALSE;
	    }
	}

	++recursive;
	यदि (l != रिक्त && action && valid_dict
		    && set_errorlist(wp, l, action,
		     (char_u *)(wp == रिक्त ? ":setqflist()" : ":setloclist()"),
		     what) == OK)
	    rettv->vval.v_सङ्ख्या = 0;
	--recursive;
    }
# endif
}

/*
 * "setloclist()" function
 */
    शून्य
f_setloclist(typval_T *argvars, typval_T *rettv)
{
    win_T	*win;

    rettv->vval.v_सङ्ख्या = -1;

    यदि (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 3) == FAIL)))
	फर्काऊ;

    win = find_win_by_nr_or_id(&argvars[0]);
    यदि (win != रिक्त)
	set_qf_ll_list(win, &argvars[1], &argvars[2], &argvars[3], rettv);
}

/*
 * "setqflist()" function
 */
    शून्य
f_setqflist(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_dict_arg(argvars, 2) == FAIL)))
	फर्काऊ;

    set_qf_ll_list(रिक्त, &argvars[0], &argvars[1], &argvars[2], rettv);
}
#endif
