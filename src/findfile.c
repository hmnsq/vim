/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * findfile.c: Search for files in directories listed in 'path'
 */

#অন্তর্ভাব "vim.h"

/*
 * File searching functions for 'path', 'tags' and 'cdpath' options.
 * External visible functions:
 * vim_findfile_init()		creates/initialises the search context
 * vim_findfile_free_visited()	free list of visited files/dirs of search
 *				context
 * vim_findfile()		find a file in the search context
 * vim_findfile_cleanup()	cleanup/free search context created by
 *				vim_findfile_init()
 *
 * All static functions and variables start with 'ff_'
 *
 * In general it works like this:
 * First you create yourself a search context by calling vim_findfile_init().
 * It is possible to give a search context from a previous call to
 * vim_findfile_init(), so it can be reused. After this you call vim_findfile()
 * until you are satisfied with the result or it returns NULL. On every call it
 * returns the next file which matches the conditions given to
 * vim_findfile_init(). If it doesn't find a next file it returns NULL.
 *
 * It is possible to call vim_findfile_init() again to reinitialise your search
 * with some new parameters. Don't forget to pass your old search context to
 * it, so it can reuse it and especially reuse the list of already visited
 * directories. If you want to delete the list of already visited directories
 * simply call vim_findfile_free_visited().
 *
 * When you are done call vim_findfile_cleanup() to free the search context.
 *
 * The function vim_findfile_init() has a long comment, which describes the
 * needed parameters.
 *
 *
 *
 * ATTENTION:
 * ==========
 *	Also we use an allocated search context here, these functions are NOT
 *	thread-safe!
 *
 *	To minimize parameter passing (or because I'm to lazy), only the
 *	external visible functions get a search context as a parameter. This is
 *	then assigned to a static global, which is used throughout the local
 *	functions.
 */

/*
 * type for the directory search stack
 */
প্রকারনির্দেশ সংরচনা ff_স্তূপ
{
    সংরচনা ff_স্তূপ	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_path;
    string_T		ffs_wc_path;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    পূর্ণাঙ্ক			ffs_filearray_size;
    পূর্ণাঙ্ক			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    পূর্ণাঙ্ক			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    পূর্ণাঙ্ক			ffs_level;

    // Did we already expand '**' to an empty string?
    পূর্ণাঙ্ক			ffs_star_star_empty;
} ff_স্তূপ_T;

/*
 * type for already visited directories or files.
 */
প্রকারনির্দেশ সংরচনা ff_visited
{
    সংরচনা ff_visited	*ffv_next;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_path;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ifdef UNIX
    পূর্ণাঙ্ক			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#endif
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_visited_T;

/*
 * We might have to manage several visited lists during a search.
 * This is especially needed for the tags option. If tags is set to:
 *      "./++/tags,./++/TAGS,++/tags"  (replace + with *)
 * So we have to do 3 searches:
 *   1) search from the current files directory downward for the file "tags"
 *   2) search from the current files directory downward for the file "TAGS"
 *   3) search from Vims current directory downwards for the file "tags"
 * As you can see, the first and the third search are for the same file, so for
 * the third search we can use the visited list of the first search. For the
 * second search we must start from a empty visited list.
 * The struct ff_visited_list_hdr is used to manage a linked list of already
 * visited lists.
 */
প্রকারনির্দেশ সংরচনা ff_visited_list_hdr
{
    সংরচনা ff_visited_list_hdr	*ffvl_next;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_visited_T		*ffvl_visited_list;

} ff_visited_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#define FF_MAX_STAR_STAR_EXPAND ((char_u)30)

/*
 * The search context:
 *   ffsc_stack_ptr:	the stack for the dirs to search
 *   ffsc_visited_list: the currently active visited list
 *   ffsc_dir_visited_list: the currently active visited list for search dirs
 *   ffsc_visited_lists_list: the list of all visited lists
 *   ffsc_dir_visited_lists_list: the list of all visited lists for search dirs
 *   ffsc_file_to_search:     the file to search for
 *   ffsc_start_dir:	the starting directory, if search path was relative
 *   ffsc_fix_path:	the fix part of the given path (without wildcards)
 *			Needed for upward search.
 *   ffsc_wc_path:	the part of the given path containing wildcards
 *   ffsc_level:	how many levels of dirs to search downwards
 *   ffsc_stopdirs_v:	array of stop directories for upward search
 *   ffsc_find_what:	FINDFILE_BOTH, FINDFILE_DIR or FINDFILE_FILE
 *   ffsc_tagfile:	searching for tags file, don't use 'suffixesadd'
 */
প্রকারনির্দেশ সংরচনা ff_অনুসন্ধান_ctx_T
{
    ff_স্তূপ_T			*ffsc_স্তূপ_ptr;
    ff_visited_list_hdr_T	*ffsc_visited_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_list;
    ff_visited_list_hdr_T	*ffsc_visited_lists_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_lists_list;
    string_T			ffsc_file_to_অনুসন্ধান;
    string_T			ffsc_start_dir;
    string_T			ffsc_fix_path;
    string_T			ffsc_wc_path;
    পূর্ণাঙ্ক				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    পূর্ণাঙ্ক				ffsc_find_what;
    পূর্ণাঙ্ক				ffsc_tagfile;
} ff_অনুসন্ধান_ctx_T;

// locally needed functions
স্থির পূর্ণাঙ্ক ff_check_visited(ff_visited_T **, char_u *, size_t, char_u *, size_t);
স্থির শূন্য vim_findfile_free_visited(শূন্য *অনুসন্ধান_ctx_arg);
স্থির শূন্য vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp);
স্থির শূন্য ff_free_visited_list(ff_visited_T *vl);
স্থির ff_visited_list_hdr_T* ff_get_visited_list(char_u *, size_t, ff_visited_list_hdr_T **list_headp);

স্থির শূন্য ff_ঠেলো(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx, ff_স্তূপ_T *স্তূপ_ptr);
স্থির ff_স্তূপ_T *ff_বের_করো(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx);
স্থির শূন্য ff_clear(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx);
স্থির শূন্য ff_free_স্তূপ_element(ff_স্তূপ_T *স্তূপ_ptr);
স্থির ff_স্তূপ_T *ff_create_স্তূপ_element(char_u *, size_t, char_u *, size_t, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক ff_path_in_stoplist(char_u *, পূর্ণাঙ্ক, string_T *);

স্থির string_T ff_expand_buffer = {রিক্ত, 0};	    // used for expanding filenames

#যদি 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

স্থির শূন্য *ff_fn_অনুসন্ধান_context = রিক্ত;

    char_u *
vim_findfirst(char_u *path, char_u *filename, পূর্ণাঙ্ক level)
{
    ff_fn_অনুসন্ধান_context =
	vim_findfile_init(path, filename, রিক্ত, level, TRUE, FALSE,
		ff_fn_অনুসন্ধান_context, rel_fname);
    যদি (রিক্ত == ff_fn_অনুসন্ধান_context)
	প্রত্যাগম রিক্ত;
    অন্যথা
	প্রত্যাগম vim_findnext()
}

    char_u *
vim_findnext(শূন্য)
{
    char_u *ret = vim_findfile(ff_fn_অনুসন্ধান_context);

    যদি (রিক্ত == ret)
    {
	vim_findfile_cleanup(ff_fn_অনুসন্ধান_context);
	ff_fn_অনুসন্ধান_context = রিক্ত;
    }
    প্রত্যাগম ret;
}
#endif

/*
 * Initialization routine for vim_findfile().
 *
 * Returns the newly allocated search context or NULL if an error occurred.
 *
 * Don't forget to clean up by calling vim_findfile_cleanup() if you are done
 * with the search context.
 *
 * Find the file 'filename' in the directory 'path'.
 * The parameter 'path' may contain wildcards. If so only search 'level'
 * directories deep. The parameter 'level' is the absolute maximum and is
 * not related to restricts given to the '**' wildcard. If 'level' is 100
 * and you use '**200' vim_findfile() will stop after 100 levels.
 *
 * 'filename' cannot contain wildcards!  It is used as-is, no backslashes to
 * escape special characters.
 *
 * If 'stopdirs' is not NULL and nothing is found downward, the search is
 * restarted on the next higher directory level. This is repeated until the
 * start-directory of a search is contained in 'stopdirs'. 'stopdirs' has the
 * format ";*<dirname>*\(;<dirname>\)*;\=$".
 *
 * If the 'path' is relative, the starting dir for the search is either VIM's
 * current dir or if the path starts with "./" the current files dir.
 * If the 'path' is absolute, the starting dir is that part of the path before
 * the first wildcard.
 *
 * Upward search is only done on the starting dir.
 *
 * If 'free_visited' is TRUE the list of already visited files/directories is
 * cleared. Set this to FALSE if you just want to search from another
 * directory, but want to be sure that no directory from a previous search is
 * searched again. This is useful if you search for a file at different places.
 * The list of visited files/dirs can also be cleared with the function
 * vim_findfile_free_visited().
 *
 * Set the parameter 'find_what' to FINDFILE_DIR if you want to search for
 * directories only, FINDFILE_FILE for files only, FINDFILE_BOTH for both.
 *
 * A search context returned by a previous call to vim_findfile_init() can be
 * passed in the parameter "search_ctx_arg".  This context is reused and
 * reinitialized with the new parameters.  The list of already visited
 * directories from this context is only deleted if the parameter
 * "free_visited" is true.  Be aware that the passed "search_ctx_arg" is freed
 * if the reinitialization fails.
 *
 * If you don't have a search context from a previous call "search_ctx_arg"
 * must be NULL.
 *
 * This function silently ignores a few errors, vim_findfile() will have
 * limited functionality then.
 */
    শূন্য *
vim_findfile_init(
    char_u	*path,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs UNUSED,
    পূর্ণাঙ্ক		level,
    পূর্ণাঙ্ক		free_visited,
    পূর্ণাঙ্ক		find_what,
    শূন্য	*অনুসন্ধান_ctx_arg,
    পূর্ণাঙ্ক		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_স্তূপ_T		*sptr;
    ff_অনুসন্ধান_ctx_T	*অনুসন্ধান_ctx;
    পূর্ণাঙ্ক			add_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    যদি (অনুসন্ধান_ctx_arg != রিক্ত)
	অনুসন্ধান_ctx = অনুসন্ধান_ctx_arg;
    অন্যথা
    {
	অনুসন্ধান_ctx = ALLOC_CLEAR_ONE(ff_অনুসন্ধান_ctx_T);
	যদি (অনুসন্ধান_ctx == রিক্ত)
	    গচ্ছ error_return;
    }
    অনুসন্ধান_ctx->ffsc_find_what = find_what;
    অনুসন্ধান_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(অনুসন্ধান_ctx);

    // clear visited list if wanted
    যদি (free_visited == TRUE)
	vim_findfile_free_visited(অনুসন্ধান_ctx);
    অন্যথা
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	অনুসন্ধান_ctx->ffsc_visited_list = ff_get_visited_list(filename,
					filenamelen, &অনুসন্ধান_ctx->ffsc_visited_lists_list);
	যদি (অনুসন্ধান_ctx->ffsc_visited_list == রিক্ত)
	    গচ্ছ error_return;
	অনুসন্ধান_ctx->ffsc_dir_visited_list = ff_get_visited_list(filename,
				    filenamelen, &অনুসন্ধান_ctx->ffsc_dir_visited_lists_list);
	যদি (অনুসন্ধান_ctx->ffsc_dir_visited_list == রিক্ত)
	    গচ্ছ error_return;
    }

    যদি (ff_expand_buffer.string == রিক্ত)
    {
	ff_expand_buffer.length = 0;
	ff_expand_buffer.string = alloc(MAXPATHL);
	যদি (ff_expand_buffer.string == রিক্ত)
	    গচ্ছ error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    যদি (path[0] == '.'
	    && (vim_ispathsep(path[1]) || path[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == রিক্ত)
	    && rel_fname != রিক্ত)
    {
	পূর্ণাঙ্ক	len = (পূর্ণাঙ্ক)(gettail(rel_fname) - rel_fname);

	যদি (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_buffer.string, rel_fname, len);
	    ff_expand_buffer.length = len;

	    অনুসন্ধান_ctx->ffsc_start_dir.string = FullName_save(ff_expand_buffer.string, FALSE);
	    যদি (অনুসন্ধান_ctx->ffsc_start_dir.string == রিক্ত)
		গচ্ছ error_return;
	    অনুসন্ধান_ctx->ffsc_start_dir.length = STRLEN(অনুসন্ধান_ctx->ffsc_start_dir.string);
	}
	অন্যথা
	{
	    অনুসন্ধান_ctx->ffsc_start_dir.length = len;
	    অনুসন্ধান_ctx->ffsc_start_dir.string = vim_strnsave(rel_fname,
		অনুসন্ধান_ctx->ffsc_start_dir.length);
	    যদি (অনুসন্ধান_ctx->ffsc_start_dir.string == রিক্ত)
		গচ্ছ error_return;
	}

	যদি (*++path != NUL)
	    ++path;
    }
    অন্যথা যদি (*path == NUL || !vim_isAbsName(path))
    {
#ifdef BACKSLASH_IN_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	যদি (*path != NUL && path[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = path[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    যদি (vim_FullName(drive, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
		গচ্ছ error_return;
	    path += 2;
	}
	অন্যথা
#endif
	যদি (mch_dirname(ff_expand_buffer.string, MAXPATHL) == FAIL)
	    গচ্ছ error_return;

	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);

	অনুসন্ধান_ctx->ffsc_start_dir.length = ff_expand_buffer.length;
	অনুসন্ধান_ctx->ffsc_start_dir.string = vim_strnsave(ff_expand_buffer.string,
	    অনুসন্ধান_ctx->ffsc_start_dir.length);
	যদি (অনুসন্ধান_ctx->ffsc_start_dir.string == রিক্ত)
	    গচ্ছ error_return;
    }

    /*
     * If stopdirs are given, split them into an array of pointers.
     * If this fails (mem allocation), there is no upward search at all or a
     * stop directory is not recognized -> continue silently.
     * If stopdirs just contains a ";" or is empty,
     * search_ctx->ffsc_stopdirs_v will only contain a  NULL pointer. This
     * is handled as unlimited upward search.  See function
     * ff_path_in_stoplist() for details.
     */
    যদি (stopdirs != রিক্ত)
    {
	char_u	*walker = stopdirs;
	পূর্ণাঙ্ক	dircount;

	যাবত্ (*walker == ';')
	    walker++;

	dircount = 1;
	অনুসন্ধান_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	যদি (অনুসন্ধান_ctx->ffsc_stopdirs_v != রিক্ত)
	{
	    string_T	*tmp;			// for convenience

	    কুরু
	    {
		char_u	*helper;
		শূন্য	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(অনুসন্ধান_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * পরিমাণ(string_T));
		যদি (ptr)
		    অনুসন্ধান_ctx->ffsc_stopdirs_v = ptr;
		অন্যথা
		    // ignore, keep what we have and continue
		    ভঙ্গ;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		যদি (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_buffer.string, helper, len);
		    ff_expand_buffer.length = len;

		    tmp = &অনুসন্ধান_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = FullName_save(ff_expand_buffer.string, FALSE);
		    যদি (tmp->string != রিক্ত)
			tmp->length = STRLEN(tmp->string);
		}
		অন্যথা
		{
		    tmp = &অনুসন্ধান_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->length = len;
		    tmp->string = vim_strnsave(helper, tmp->length);
		    যদি (tmp->string == রিক্ত)
			tmp->length = 0;
		}
		যদি (walker)
		    walker++;
		dircount++;

	    } যাবত্ (walker != রিক্ত);

	    tmp = &অনুসন্ধান_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = রিক্ত;
	    tmp->length = 0;
	}
    }

    অনুসন্ধান_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(path, '*');
    যদি (wc_part != রিক্ত)
    {
	পূর্ণাঙ্ক	llevel;
	বর্ণ	*errpt;

	// save the fix part of the path
	অনুসন্ধান_ctx->ffsc_fix_path.length = (size_t)(wc_part - path);
	অনুসন্ধান_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    অনুসন্ধান_ctx->ffsc_fix_path.length);
	যদি (অনুসন্ধান_ctx->ffsc_fix_path.string == রিক্ত)
	    গচ্ছ error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_buffer.length = 0;
	যাবত্ (*wc_part != NUL)
	{
	    যদি (ff_expand_buffer.length + 5 >= MAXPATHL)
	    {
		emsg(_(e_path_too_long_for_completion));
		ভঙ্গ;
	    }
	    যদি (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;

		llevel = strtol((বর্ণ *)wc_part, &errpt, 10);
		যদি ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_buffer.string[ff_expand_buffer.length++] = llevel;
		অন্যথা যদি ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_buffer.length -= 2;
		অন্যথা
		    ff_expand_buffer.string[ff_expand_buffer.length++] = FF_MAX_STAR_STAR_EXPAND;
		wc_part = (char_u *)errpt;
		যদি (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_path_সংখ্যা_must_be_at_end_of_path_or_be_followed_by_str), PATHSEPSTR);
		    গচ্ছ error_return;
		}
	    }
	    অন্যথা
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
	}
	ff_expand_buffer.string[ff_expand_buffer.length] = NUL;

	অনুসন্ধান_ctx->ffsc_wc_path.length = ff_expand_buffer.length;
	অনুসন্ধান_ctx->ffsc_wc_path.string = vim_strnsave(ff_expand_buffer.string,
	    অনুসন্ধান_ctx->ffsc_wc_path.length);
	যদি (অনুসন্ধান_ctx->ffsc_wc_path.string == রিক্ত)
	    গচ্ছ error_return;
    }
    অন্যথা
    {
	অনুসন্ধান_ctx->ffsc_fix_path.length = STRLEN(path);
	অনুসন্ধান_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    অনুসন্ধান_ctx->ffsc_fix_path.length);
	যদি (অনুসন্ধান_ctx->ffsc_fix_path.string == রিক্ত)
	    গচ্ছ error_return;
    }

    যদি (অনুসন্ধান_ctx->ffsc_start_dir.string == রিক্ত)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	অনুসন্ধান_ctx->ffsc_start_dir.length = অনুসন্ধান_ctx->ffsc_fix_path.length;
	অনুসন্ধান_ctx->ffsc_start_dir.string = vim_strnsave(অনুসন্ধান_ctx->ffsc_fix_path.string,
	    অনুসন্ধান_ctx->ffsc_start_dir.length);
	যদি (অনুসন্ধান_ctx->ffsc_start_dir.string == রিক্ত)
	    গচ্ছ error_return;
	অনুসন্ধান_ctx->ffsc_fix_path.string[0] = NUL;
	অনুসন্ধান_ctx->ffsc_fix_path.length = 0;
    }

    // create an absolute path
    যদি (অনুসন্ধান_ctx->ffsc_start_dir.length
	    + অনুসন্ধান_ctx->ffsc_fix_path.length + 3 >= MAXPATHL)
    {
	emsg(_(e_path_too_long_for_completion));
	গচ্ছ error_return;
    }

    add_sep = !after_pathsep(অনুসন্ধান_ctx->ffsc_start_dir.string,
	অনুসন্ধান_ctx->ffsc_start_dir.string + অনুসন্ধান_ctx->ffsc_start_dir.length);
    ff_expand_buffer.length = vim_snprintf(
	    (বর্ণ *)ff_expand_buffer.string,
	    MAXPATHL,
	    "%s%s",
	    অনুসন্ধান_ctx->ffsc_start_dir.string,
	    add_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_buffer.length + অনুসন্ধান_ctx->ffsc_fix_path.length + 1;
	char_u	*buf = alloc(bufsize);

	যদি (buf == রিক্ত)
	    গচ্ছ error_return;

	vim_snprintf(
		(বর্ণ *)buf,
		bufsize,
		"%s%s",
		ff_expand_buffer.string,
		অনুসন্ধান_ctx->ffsc_fix_path.string);
	যদি (mch_isdir(buf))
	{
	    যদি (অনুসন্ধান_ctx->ffsc_fix_path.length > 0)
	    {
		add_sep = !after_pathsep(অনুসন্ধান_ctx->ffsc_fix_path.string,
		    অনুসন্ধান_ctx->ffsc_fix_path.string + অনুসন্ধান_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(বর্ণ *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%s%s",
			অনুসন্ধান_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }
	}
	অন্যথা
	{
	    char_u *p = gettail(অনুসন্ধান_ctx->ffsc_fix_path.string);
	    পূর্ণাঙ্ক    len = (পূর্ণাঙ্ক)অনুসন্ধান_ctx->ffsc_fix_path.length;

	    যদি (p > অনুসন্ধান_ctx->ffsc_fix_path.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (পূর্ণাঙ্ক)(p - অনুসন্ধান_ctx->ffsc_fix_path.string) - 1;
		যদি ((len >= 2
			&& STRNCMP(অনুসন্ধান_ctx->ffsc_fix_path.string, "..", 2) == 0)
			&& (len == 2 || অনুসন্ধান_ctx->ffsc_fix_path.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    গচ্ছ error_return;
		}

		add_sep = !after_pathsep(অনুসন্ধান_ctx->ffsc_fix_path.string,
		    অনুসন্ধান_ctx->ffsc_fix_path.string + অনুসন্ধান_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(বর্ণ *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%.*s%s",
			len,
			অনুসন্ধান_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }

	    যদি (অনুসন্ধান_ctx->ffsc_wc_path.string != রিক্ত)
	    {
		size_t	tempsize = (অনুসন্ধান_ctx->ffsc_fix_path.length - len)
				+ অনুসন্ধান_ctx->ffsc_wc_path.length
				+ 1;
		char_u	*temp = alloc(tempsize);

		যদি (temp == রিক্ত)
		{
		    vim_free(buf);
		    vim_free(temp);
		    গচ্ছ error_return;
		}

		অনুসন্ধান_ctx->ffsc_wc_path.length = vim_snprintf(
			    (বর্ণ *)temp,
			    tempsize,
			    "%s%s",
			    অনুসন্ধান_ctx->ffsc_fix_path.string + len,
			    অনুসন্ধান_ctx->ffsc_wc_path.string);
		vim_free(অনুসন্ধান_ctx->ffsc_wc_path.string);
		অনুসন্ধান_ctx->ffsc_wc_path.string = temp;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_স্তূপ_element(ff_expand_buffer.string,
			ff_expand_buffer.length,
			অনুসন্ধান_ctx->ffsc_wc_path.string,
			অনুসন্ধান_ctx->ffsc_wc_path.length,
			level,
			0);

    যদি (sptr == রিক্ত)
	গচ্ছ error_return;

    ff_ঠেলো(অনুসন্ধান_ctx, sptr);

    অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.length = filenamelen;
    অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.string = vim_strnsave(filename,
	অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.length);
    যদি (অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.string == রিক্ত)
	গচ্ছ error_return;

    প্রত্যাগম অনুসন্ধান_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(অনুসন্ধান_ctx);
    প্রত্যাগম রিক্ত;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_end = রিক্ত;	    // points to NUL at end of string "r_ptr"

    যাবত্ (*r_ptr != NUL && *r_ptr != ';')
    {
	যদি (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    যদি (r_ptr_end == রিক্ত)
		r_ptr_end = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_end - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_end;
	}
	r_ptr++;
    }
    যদি (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    অন্যথা যদি (*r_ptr == NUL)
	r_ptr = রিক্ত;
    প্রত্যাগম r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    শূন্য
vim_findfile_cleanup(শূন্য *ctx)
{
    যদি (ctx == রিক্ত)
	প্রত্যাগম;

    vim_findfile_free_visited(ctx);
    ff_clear(ctx);
    vim_free(ctx);
}

/*
 * Find a file in a search context.
 * The search context was created with vim_findfile_init() above.
 * Return a pointer to an allocated file name or NULL if nothing found.
 * To get all matching files call this function until you get NULL.
 *
 * If the passed search_context is NULL, NULL is returned.
 *
 * The search algorithm is depth first. To change this replace the
 * stack with a list (don't forget to leave partly searched directories on the
 * top of the list).
 */
    char_u *
vim_findfile(শূন্য *অনুসন্ধান_ctx_arg)
{
    string_T	file_path;
    string_T	rest_of_wildcards;
    char_u	*path_end = রিক্ত;
    ff_স্তূপ_T	*stackp;
    ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx;

    যদি (অনুসন্ধান_ctx_arg == রিক্ত)
	প্রত্যাগম রিক্ত;

    অনুসন্ধান_ctx = (ff_অনুসন্ধান_ctx_T *)অনুসন্ধান_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    যদি ((file_path.string = alloc(MAXPATHL)) == রিক্ত)
	প্রত্যাগম রিক্ত;

    // store the end of the start dir -- needed for upward search
    যদি (অনুসন্ধান_ctx->ffsc_start_dir.string != রিক্ত)
	path_end = &অনুসন্ধান_ctx->ffsc_start_dir.string[
					    অনুসন্ধান_ctx->ffsc_start_dir.length];

    // upward search loop
    প্রতি (;;)
    {
	// downward search loop
	প্রতি (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    যদি (got_int)
		ভঙ্গ;

	    // get directory to work on from stack
	    stackp = ff_বের_করো(অনুসন্ধান_ctx);
	    যদি (stackp == রিক্ত)
		ভঙ্গ;

	    /*
	     * TODO: decide if we leave this test in
	     *
	     * GOOD: don't search a directory(-tree) twice.
	     * BAD:  - check linked list for every new directory entered.
	     *       - check for double files also done below
	     *
	     * Here we check if we already searched this directory.
	     * We already searched a directory if:
	     * 1) The directory is the same.
	     * 2) We would use the same wildcard string.
	     *
	     * Good if you have links on same directory via several ways
	     *  or you have selfreferences in directories (e.g. SuSE Linux 6.3:
	     *  /etc/rc.d/init.d is linked to /etc/rc.d -> endless loop)
	     *
	     * This check is only needed for directories we work on for the
	     * first time (hence stackp->ff_filearray == NULL)
	     */
	    যদি (stackp->ffs_filearray == রিক্ত
		    && ff_check_visited(&অনুসন্ধান_ctx->ffsc_dir_visited_list
							  ->ffvl_visited_list,
			    stackp->ffs_fix_path.string,
			    stackp->ffs_fix_path.length,
			    stackp->ffs_wc_path.string,
			    stackp->ffs_wc_path.length) == FAIL)
	    {
#ifdef FF_VERBOSE
		যদি (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		    // don't overwrite this either
		    বার্তা_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		ff_free_স্তূপ_element(stackp);
		অনুবর্ত;
	    }
#ifdef FF_VERBOSE
	    অন্যথা যদি (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		// don't overwrite this either
		বার্তা_puts("\n");
		verbose_leave_scroll();
	    }
#endif

	    // check depth
	    যদি (stackp->ffs_level <= 0)
	    {
		ff_free_স্তূপ_element(stackp);
		অনুবর্ত;
	    }

	    file_path.string[0] = NUL;
	    file_path.length = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    যদি (stackp->ffs_filearray == রিক্ত)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_path.string;
		dirptrs[1] = রিক্ত;

		// if we have a start dir copy it in
		যদি (!vim_isAbsName(stackp->ffs_fix_path.string)
						&& অনুসন্ধান_ctx->ffsc_start_dir.string)
		{
		    যদি (অনুসন্ধান_ctx->ffsc_start_dir.length + 1 < MAXPATHL)
		    {
			পূর্ণাঙ্ক add_sep = !after_pathsep(অনুসন্ধান_ctx->ffsc_start_dir.string,
			    অনুসন্ধান_ctx->ffsc_start_dir.string + অনুসন্ধান_ctx->ffsc_start_dir.length);
			file_path.length = vim_snprintf(
			    (বর্ণ *)file_path.string,
			    MAXPATHL,
			    "%s%s",
			    অনুসন্ধান_ctx->ffsc_start_dir.string,
			    add_sep ? PATHSEPSTR : "");
		    }
		    অন্যথা
		    {
			ff_free_স্তূপ_element(stackp);
			গচ্ছ fail;
		    }
		}

		// append the fix part of the search path
		যদি (file_path.length + stackp->ffs_fix_path.length + 1 < MAXPATHL)
		{
		    পূর্ণাঙ্ক add_sep = !after_pathsep(stackp->ffs_fix_path.string,
			stackp->ffs_fix_path.string + stackp->ffs_fix_path.length);
		    file_path.length += vim_snprintf(
			(বর্ণ *)file_path.string + file_path.length,
			MAXPATHL - file_path.length,
			"%s%s",
			stackp->ffs_fix_path.string,
			add_sep ? PATHSEPSTR : "");
		}
		অন্যথা
		{
		    ff_free_স্তূপ_element(stackp);
		    গচ্ছ fail;
		}

		rest_of_wildcards.string = stackp->ffs_wc_path.string;
		rest_of_wildcards.length = stackp->ffs_wc_path.length;
		যদি (*rest_of_wildcards.string != NUL)
		{
		    যদি (STRNCMP(rest_of_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_of_wildcards.string + 2;

			যদি (*p > 0)
			{
			    (*p)--;
			    যদি (file_path.length + 1 < MAXPATHL)
				file_path.string[file_path.length++] = '*';
			    অন্যথা
			    {
				ff_free_স্তূপ_element(stackp);
				গচ্ছ fail;
			    }
			}

			যদি (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_of_wildcards.string,
					rest_of_wildcards.string + 3,
					(size_t)(rest_of_wildcards.length - 3) + 1);    // +1 for NUL
			    rest_of_wildcards.length -= 3;
			    stackp->ffs_wc_path.length = rest_of_wildcards.length;
			}
			অন্যথা
			{
			    rest_of_wildcards.string += 3;
			    rest_of_wildcards.length -= 3;
			}

			যদি (stackp->ffs_star_star_empty == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_star_star_empty = 1;
			    dirptrs[1] = stackp->ffs_fix_path.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    যাবত্ (*rest_of_wildcards.string
			    && !vim_ispathsep(*rest_of_wildcards.string))
		    {
			যদি (file_path.length + 1 < MAXPATHL)
			{
			    file_path.string[file_path.length++] = *rest_of_wildcards.string++;
			    --rest_of_wildcards.length;
			}
			অন্যথা
			{
			    ff_free_স্তূপ_element(stackp);
			    গচ্ছ fail;
			}
		    }

		    file_path.string[file_path.length] = NUL;
		    যদি (vim_ispathsep(*rest_of_wildcards.string))
		    {
			rest_of_wildcards.string++;
			rest_of_wildcards.length--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		যদি (path_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    যদি (stackp->ffs_filearray != রিক্ত
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_path.length)) != রিক্ত)
			stackp->ffs_filearray_size = 1;
		    অন্যথা
			stackp->ffs_filearray_size = 0;
		}
		অন্যথা
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == রিক্ত) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_size,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    অন্যথা
	    {
		rest_of_wildcards.string = &stackp->ffs_wc_path.string[
							stackp->ffs_wc_path.length];
		rest_of_wildcards.length = 0;
	    }

	    যদি (stackp->ffs_stage == 0)
	    {
		পূর্ণাঙ্ক i;

		// this is the first time we work on this directory
		যদি (*rest_of_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    প্রতি (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			যদি (!path_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    অনুবর্ত;   // not a directory

			// prepare the filename to be checked for existence
			// below
			len = STRLEN(stackp->ffs_filearray[i]);
			যদি (len + 1 + অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.length
									< MAXPATHL)
			{
			    পূর্ণাঙ্ক	add_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_path.length = vim_snprintf(
				    (বর্ণ *)file_path.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    add_sep ? PATHSEPSTR : "",
				    অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান.string);
			}
			অন্যথা
			{
			    ff_free_স্তূপ_element(stackp);
			    গচ্ছ fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_path.length;
			যদি (অনুসন্ধান_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			অন্যথা
			    suf = curbuf->b_p_sua;
			প্রতি (;;)
			{
			    // if file exists and we didn't already find it
			    যদি ((path_with_url(file_path.string)
				  || (mch_getperm(file_path.string) >= 0
				      && (অনুসন্ধান_ctx->ffsc_find_what
							      == FINDFILE_BOTH
					  || ((অনুসন্ধান_ctx->ffsc_find_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_path.string)))))
#ifndef FF_VERBOSE
				    && (ff_check_visited(
					    &অনুসন্ধান_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					    file_path.string,
					    file_path.length,
					    (char_u *)"", 0) == OK)
#endif
			       )
			    {
#ifdef FF_VERBOSE
				যদি (ff_check_visited(
					    &অনুসন্ধান_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					      file_path.string,
					      file_path.length,
					      (char_u *)"", 0) == FAIL)
				{
				    যদি (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_path.string);
					// don't overwrite this either
					বার্তা_puts("\n");
					verbose_leave_scroll();
				    }
				    অনুবর্ত;
				}
#endif

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_ঠেলো(অনুসন্ধান_ctx, stackp);

				যদি (!path_with_url(file_path.string))
				    file_path.length = simplify_filename(file_path.string);

				যদি (mch_dirname(ff_expand_buffer.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
				    p = shorten_fname(file_path.string,
							    ff_expand_buffer.string);
				    যদি (p != রিক্ত)
				    {
					mch_memmove(file_path.string, p,
					    (size_t)((file_path.string + file_path.length) - p) + 1);  // +1 for NUL
					file_path.length -= (p - file_path.string);
				    }
				}
#ifdef FF_VERBOSE
				যদি (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_path.string);
				    // don't overwrite this either
				    বার্তা_puts("\n");
				    verbose_leave_scroll();
				}
#endif
				প্রত্যাগম file_path.string;
			    }

			    // Not found or found already, try next suffix.
			    যদি (*suf == NUL)
				ভঙ্গ;
			    file_path.length = len + copy_option_part(&suf,
				  file_path.string + len,
				  (পূর্ণাঙ্ক)(MAXPATHL - len), ",");
			}
		    }
		}
		অন্যথা
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    প্রতি (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			যদি (!mch_isdir(stackp->ffs_filearray[i]))
			    অনুবর্ত;	// not a directory

			ff_ঠেলো(অনুসন্ধান_ctx,
				ff_create_স্তূপ_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_of_wildcards.string,
						     rest_of_wildcards.length,
						     stackp->ffs_level - 1, 0));
		    }
		}
		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 1;
	    }

	    /*
	     * if wildcards contains '**' we have to descent till we reach the
	     * leaves of the directory tree.
	     */
	    যদি (STRNCMP(stackp->ffs_wc_path.string, "**", 2) == 0)
	    {
		পূর্ণাঙ্ক i;

		প্রতি (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		{
		    যদি (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_path.string) == 0)
			অনুবর্ত; // don't repush same directory
		    যদি (!mch_isdir(stackp->ffs_filearray[i]))
			অনুবর্ত;   // not a directory
		    ff_ঠেলো(অনুসন্ধান_ctx,
			    ff_create_স্তূপ_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_path.string,
				stackp->ffs_wc_path.length,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_স্তূপ_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	যদি (অনুসন্ধান_ctx->ffsc_start_dir.string
		&& অনুসন্ধান_ctx->ffsc_stopdirs_v != রিক্ত && !got_int)
	{
	    ff_স্তূপ_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    পূর্ণাঙ্ক plen = (path_end - অনুসন্ধান_ctx->ffsc_start_dir.string)
							  + (*path_end != NUL);

	    // is the last starting directory in the stop list?
	    যদি (ff_path_in_stoplist(অনুসন্ধান_ctx->ffsc_start_dir.string,
				    plen, অনুসন্ধান_ctx->ffsc_stopdirs_v) == TRUE)
		ভঙ্গ;

	    // cut of last dir
	    যাবত্ (path_end > অনুসন্ধান_ctx->ffsc_start_dir.string
						  && vim_ispathsep(*path_end))
		path_end--;
	    যাবত্ (path_end > অনুসন্ধান_ctx->ffsc_start_dir.string
					      && !vim_ispathsep(path_end[-1]))
		path_end--;
	    *path_end = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    অনুসন্ধান_ctx->ffsc_start_dir.length = (size_t)(path_end - অনুসন্ধান_ctx->ffsc_start_dir.string);
	    path_end--;

	    যদি (*অনুসন্ধান_ctx->ffsc_start_dir.string == NUL)
		ভঙ্গ;

	    যদি (অনুসন্ধান_ctx->ffsc_start_dir.length + 1
		    + অনুসন্ধান_ctx->ffsc_fix_path.length < MAXPATHL)
	    {
		পূর্ণাঙ্ক add_sep = !after_pathsep(অনুসন্ধান_ctx->ffsc_start_dir.string,
			    অনুসন্ধান_ctx->ffsc_start_dir.string + অনুসন্ধান_ctx->ffsc_start_dir.length);
		file_path.length = vim_snprintf(
			(বর্ণ *)file_path.string,
			MAXPATHL,
			"%s%s%s",
			অনুসন্ধান_ctx->ffsc_start_dir.string,
			add_sep ? PATHSEPSTR : "",
			অনুসন্ধান_ctx->ffsc_fix_path.string);
	    }
	    অন্যথা
		গচ্ছ fail;

	    // create a new stack entry
	    sptr = ff_create_স্তূপ_element(file_path.string, file_path.length,
		    অনুসন্ধান_ctx->ffsc_wc_path.string, অনুসন্ধান_ctx->ffsc_wc_path.length,
		    অনুসন্ধান_ctx->ffsc_level, 0);
	    যদি (sptr == রিক্ত)
		ভঙ্গ;
	    ff_ঠেলো(অনুসন্ধান_ctx, sptr);
	}
	অন্যথা
	    ভঙ্গ;
    }

fail:
    vim_free(file_path.string);
    প্রত্যাগম রিক্ত;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    স্থির শূন্য
vim_findfile_free_visited(শূন্য *অনুসন্ধান_ctx_arg)
{
    ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx;

    যদি (অনুসন্ধান_ctx_arg == রিক্ত)
	প্রত্যাগম;

    অনুসন্ধান_ctx = (ff_অনুসন্ধান_ctx_T *)অনুসন্ধান_ctx_arg;
    vim_findfile_free_visited_list(&অনুসন্ধান_ctx->ffsc_visited_lists_list);
    vim_findfile_free_visited_list(&অনুসন্ধান_ctx->ffsc_dir_visited_lists_list);
}

    স্থির শূন্য
vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp)
{
    ff_visited_list_hdr_T *vp;

    যাবত্ (*list_headp != রিক্ত)
    {
	vp = (*list_headp)->ffvl_next;
	ff_free_visited_list((*list_headp)->ffvl_visited_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = রিক্ত;
}

    স্থির শূন্য
ff_free_visited_list(ff_visited_T *vl)
{
    ff_visited_T *vp;

    যাবত্ (vl != রিক্ত)
    {
	vp = vl->ffv_next;
	vim_free(vl->ffv_wc_path);
	vim_free(vl);
	vl = vp;
    }
    vl = রিক্ত;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    স্থির ff_visited_list_hdr_T*
ff_get_visited_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_visited_list_hdr_T	**list_headp)
{
    ff_visited_list_hdr_T  *retptr = রিক্ত;

    // check if a visited list for the given filename exists
    যদি (*list_headp != রিক্ত)
    {
	retptr = *list_headp;
	যাবত্ (retptr != রিক্ত)
	{
	    যদি (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ifdef FF_VERBOSE
		যদি (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    বার্তা_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		প্রত্যাগম retptr;
	    }
	    retptr = retptr->ffvl_next;
	}
    }

#ifdef FF_VERBOSE
    যদি (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	বার্তা_puts("\n");
	verbose_leave_scroll();
    }
#endif

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_visited_list_hdr_T);
    যদি (retptr == রিক্ত)
	প্রত্যাগম রিক্ত;

    retptr->ffvl_visited_list = রিক্ত;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    যদি (retptr->ffvl_filename == রিক্ত)
    {
	vim_free(retptr);
	প্রত্যাগম রিক্ত;
    }
    retptr->ffvl_next = *list_headp;
    *list_headp = retptr;

    প্রত্যাগম retptr;
}

/*
 * check if two wildcard paths are equal. Returns TRUE or FALSE.
 * They are equal if:
 *  - both paths are NULL
 *  - they have the same length
 *  - char by char comparison is OK
 *  - the only differences are in the counters behind a '**', so
 *    '**\20' is equal to '**\24'
 */
    স্থির পূর্ণাঙ্ক
ff_wc_equal(char_u *s1, char_u *s2)
{
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		c1 = NUL;
    পূর্ণাঙ্ক		c2 = NUL;
    পূর্ণাঙ্ক		prev1 = NUL;
    পূর্ণাঙ্ক		prev2 = NUL;

    যদি (s1 == s2)
	প্রত্যাগম TRUE;

    যদি (s1 == রিক্ত || s2 == রিক্ত)
	প্রত্যাগম FALSE;

    প্রতি (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	যদি ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    প্রত্যাগম FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    প্রত্যাগম s1[i] == s2[j];
}

/*
 * maintains the list of already visited files and dirs
 * returns FAIL if the given file/dir is already in the list
 * returns OK if it is newly added
 *
 * TODO: What to do on memory allocation problems?
 *	 -> return TRUE - Better the file is found several times instead of
 *	    never.
 */
    স্থির পূর্ণাঙ্ক
ff_check_visited(
    ff_visited_T	**visited_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_path,
    size_t		wc_pathlen)
{
    ff_visited_T	*vp;
#ifdef UNIX
    stat_T		st;
    পূর্ণাঙ্ক			url = FALSE;
#endif

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    যদি (path_with_url(fname))
    {
	vim_strncpy(ff_expand_buffer.string, fname, fnamelen);
	ff_expand_buffer.length = fnamelen;
#ifdef UNIX
	url = TRUE;
#endif
    }
    অন্যথা
    {
	ff_expand_buffer.string[0] = NUL;
	ff_expand_buffer.length = 0;
#ifdef UNIX
	যদি (mch_stat((বর্ণ *)fname, &st) < 0)
	    প্রত্যাগম FAIL;
#অন্যথা
	যদি (vim_FullName(fname, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
	    প্রত্যাগম FAIL;
	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
#endif
    }

    // check against list of already visited files
    প্রতি (vp = *visited_list; vp != রিক্ত; vp = vp->ffv_next)
    {
	যদি (
#ifdef UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#endif
		fnamecmp(vp->ffv_fname, ff_expand_buffer.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    যদি (ff_wc_equal(vp->ffv_wc_path, wc_path) == TRUE)
		// already visited
		প্রত্যাগম FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_visited_T, ffv_fname) + ff_expand_buffer.length + 1);
    যদি (vp == রিক্ত)
	প্রত্যাগম OK;

#ifdef UNIX
    যদি (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    অন্যথা
    {
	vp->ffv_dev_valid = FALSE;
#endif
	STRCPY(vp->ffv_fname, ff_expand_buffer.string);
#ifdef UNIX
    }
#endif
    যদি (wc_path != রিক্ত)
	vp->ffv_wc_path = vim_strnsave(wc_path, wc_pathlen);
    অন্যথা
	vp->ffv_wc_path = রিক্ত;

    vp->ffv_next = *visited_list;
    *visited_list = vp;

    প্রত্যাগম OK;
}

/*
 * create stack element from given path pieces
 */
    স্থির ff_স্তূপ_T *
ff_create_স্তূপ_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    পূর্ণাঙ্ক		level,
    পূর্ণাঙ্ক		star_star_empty)
{
    ff_স্তূপ_T	*new;

    new = ALLOC_ONE(ff_স্তূপ_T);
    যদি (new == রিক্ত)
	প্রত্যাগম রিক্ত;

    new->ffs_prev	   = রিক্ত;
    new->ffs_filearray	   = রিক্ত;
    new->ffs_filearray_size = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_star_star_empty = star_star_empty;

    // the following saves NULL pointer checks in vim_findfile
    যদি (fix_part == রিক্ত)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_path.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_path.length = fix_partlen;

    যদি (wc_part == রিক্ত)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_path.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_path.length = wc_partlen;

    যদি (new->ffs_fix_path.string == রিক্ত || new->ffs_wc_path.string == রিক্ত)
    {
	ff_free_স্তূপ_element(new);
	new = রিক্ত;
    }

    প্রত্যাগম new;
}

/*
 * Push a dir on the directory stack.
 */
    স্থির শূন্য
ff_ঠেলো(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx, ff_স্তূপ_T *স্তূপ_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    যদি (স্তূপ_ptr == রিক্ত)
	প্রত্যাগম;

    স্তূপ_ptr->ffs_prev = অনুসন্ধান_ctx->ffsc_স্তূপ_ptr;
    অনুসন্ধান_ctx->ffsc_স্তূপ_ptr = স্তূপ_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    স্থির ff_স্তূপ_T *
ff_বের_করো(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx)
{
    ff_স্তূপ_T  *sptr;

    sptr = অনুসন্ধান_ctx->ffsc_স্তূপ_ptr;
    যদি (অনুসন্ধান_ctx->ffsc_স্তূপ_ptr != রিক্ত)
	অনুসন্ধান_ctx->ffsc_স্তূপ_ptr = অনুসন্ধান_ctx->ffsc_স্তূপ_ptr->ffs_prev;

    প্রত্যাগম sptr;
}

/*
 * free the given stack element
 */
    স্থির শূন্য
ff_free_স্তূপ_element(ff_স্তূপ_T *স্তূপ_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(স্তূপ_ptr->ffs_fix_path);
    VIM_CLEAR_STRING(স্তূপ_ptr->ffs_wc_path);

    যদি (স্তূপ_ptr->ffs_filearray != রিক্ত)
	FreeWild(স্তূপ_ptr->ffs_filearray_size, স্তূপ_ptr->ffs_filearray);

    vim_free(স্তূপ_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    স্থির শূন্য
ff_clear(ff_অনুসন্ধান_ctx_T *অনুসন্ধান_ctx)
{
    ff_স্তূপ_T   *sptr;

    // clear up stack
    যাবত্ ((sptr = ff_বের_করো(অনুসন্ধান_ctx)) != রিক্ত)
	ff_free_স্তূপ_element(sptr);

    যদি (অনুসন্ধান_ctx->ffsc_stopdirs_v != রিক্ত)
    {
	পূর্ণাঙ্ক  i = 0;

	যাবত্ (অনুসন্ধান_ctx->ffsc_stopdirs_v[i].string != রিক্ত)
	{
	    vim_free(অনুসন্ধান_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(অনুসন্ধান_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(অনুসন্ধান_ctx->ffsc_file_to_অনুসন্ধান);
    VIM_CLEAR_STRING(অনুসন্ধান_ctx->ffsc_start_dir);
    VIM_CLEAR_STRING(অনুসন্ধান_ctx->ffsc_fix_path);
    VIM_CLEAR_STRING(অনুসন্ধান_ctx->ffsc_wc_path);
    অনুসন্ধান_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    স্থির পূর্ণাঙ্ক
ff_path_in_stoplist(char_u *path, পূর্ণাঙ্ক path_len, string_T *stopdirs_v)
{
    পূর্ণাঙ্ক		i = 0;

    // eat up trailing path separators, except the first
    যাবত্ (path_len > 1 && vim_ispathsep(path[path_len - 1]))
	path_len--;

    // if no path consider it as match
    যদি (path_len == 0)
	প্রত্যাগম TRUE;

    প্রতি (i = 0; stopdirs_v[i].string != রিক্ত; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	যদি (fnamencmp(stopdirs_v[i].string, path, path_len) == 0
		&& ((পূর্ণাঙ্ক)stopdirs_v[i].length <= path_len
		    || vim_ispathsep(stopdirs_v[i].string[path_len])))
	    প্রত্যাগম TRUE;

    প্রত্যাগম FALSE;
}

/*
 * Find the file name "ptr[len]" in the path.  Also finds directory names.
 *
 * On the first call set the parameter 'first' to TRUE to initialize
 * the search.  For repeating calls to FALSE.
 *
 * Repeating calls will return other files called 'ptr[len]' from the path.
 *
 * Only on the first call 'ptr' and 'len' are used.  For repeating calls they
 * don't need valid values.
 *
 * If nothing found on the first call the option FNAME_MESS will issue the
 * message:
 *	    'Can't find file "<file>" in path'
 * On repeating calls:
 *	    'No more file "<file>" found in path'
 *
 * options:
 * FNAME_MESS	    give error message when not found
 *
 * Uses NameBuff[]!
 *
 * Returns an allocated string for the file name.  NULL for error.
 *
 */
    char_u *
find_file_in_path(
    char_u	*ptr,		// file name
    পূর্ণাঙ্ক		len,		// length of file name
    পূর্ণাঙ্ক		options,
    পূর্ণাঙ্ক		first,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    বর্ণ	**অনুসন্ধান_ctx)	// in/out: state of the search
{
    প্রত্যাগম find_file_in_path_option(ptr, len, options, first,
	    *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_to_find, অনুসন্ধান_ctx);
}

#যদি defined(EXITFREE)
    শূন্য
free_findfile(শূন্য)
{
    VIM_CLEAR_STRING(ff_expand_buffer);
}
#endif

/*
 * Find the directory name "ptr[len]" in the path.
 *
 * options:
 * FNAME_MESS	    give error message when not found
 * FNAME_UNESC	    unescape backslashes.
 *
 * Uses NameBuff[]!
 *
 * Returns an allocated string for the file name.  NULL for error.
 */
    char_u *
find_directory_in_path(
    char_u	*ptr,		// file name
    পূর্ণাঙ্ক		len,		// length of file name
    পূর্ণাঙ্ক		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    বর্ণ	**অনুসন্ধান_ctx)	// in/out: state of the search
{
    প্রত্যাগম find_file_in_path_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_to_find, অনুসন্ধান_ctx);
}

    char_u *
find_file_in_path_option(
    char_u	*ptr,		// file name
    পূর্ণাঙ্ক		len,		// length of file name
    পূর্ণাঙ্ক		options,
    পূর্ণাঙ্ক		first,		// use count'th matching file name
    char_u	*path_option,	// p_path or p_cdpath
    পূর্ণাঙ্ক		find_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_to_find,	// in/out: modified copy of file name
    বর্ণ	**অনুসন্ধান_ctx_arg) // in/out: state of the search
{
    ff_অনুসন্ধান_ctx_T	**অনুসন্ধান_ctx = (ff_অনুসন্ধান_ctx_T **)অনুসন্ধান_ctx_arg;
    স্থির char_u	*dir;
    স্থির পূর্ণাঙ্ক		did_findfile_init = FALSE;
    char_u		*file_name = রিক্ত;
    পূর্ণাঙ্ক			rel_to_curdir;
#ifdef AMIGA
    সংরচনা Process	*proc = (সংরচনা Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif
    স্থির size_t	file_to_findlen = 0;

    যদি (first == TRUE)
    {
	char_u	save_char;

	যদি (len == 0)
	    প্রত্যাগম রিক্ত;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_to_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, রিক্ত, TRUE, রিক্ত);
	ptr[len] = save_char;

	vim_free(*file_to_find);
	*file_to_find = vim_strnsave(NameBuff, file_to_findlen);
	যদি (*file_to_find == রিক্ত)	// out of memory
	{
	    file_name = রিক্ত;
	    file_to_findlen = 0;
	    গচ্ছ theend;
	}
	যদি (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    প্রতি (ptr = *file_to_find; *ptr != NUL; ++ptr)
		যদি (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_to_find + file_to_findlen) - (ptr + 1)) + 1);
		    --file_to_findlen;
		}
	}
    }

    rel_to_curdir = ((*file_to_find)[0] == '.'
		    && ((*file_to_find)[1] == NUL
			|| vim_ispathsep((*file_to_find)[1])
			|| ((*file_to_find)[1] == '.'
			    && ((*file_to_find)[2] == NUL
				|| vim_ispathsep((*file_to_find)[2])))));
    যদি (vim_isAbsName(*file_to_find)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_to_curdir
#যদি defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_to_find)[0])
	    // handle "c:name" as absolute path
	    || ((*file_to_find)[0] != NUL && (*file_to_find)[1] == ':')
#endif
#ifdef AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_to_find)[0] == ':'
#endif
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	যদি (first == TRUE)
	{
	    পূর্ণাঙ্ক		l;
	    পূর্ণাঙ্ক		run;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    যদি (path_with_url(*file_to_find))
	    {
		file_name = vim_strnsave(*file_to_find, file_to_findlen);
		গচ্ছ theend;
	    }

	    যদি (rel_fname != রিক্ত)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    প্রতি (run = 1; run <= 2; ++run)
	    {
		l = (পূর্ণাঙ্ক)file_to_findlen;
		যদি (run == 1
			&& rel_to_curdir
			&& (options & FNAME_REL)
			&& rel_fname != রিক্ত
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(বর্ণ *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(পূর্ণাঙ্ক)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_to_find);
		}
		অন্যথা
		{
		    STRCPY(NameBuff, *file_to_find);
		    run = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		প্রতি (;;)
		{
		    যদি (mch_getperm(NameBuff) >= 0
			     && (find_what == FINDFILE_BOTH
				 || ((find_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			গচ্ছ theend;
		    }
		    যদি (*suffix == NUL)
			ভঙ্গ;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    অন্যথা
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	যদি (first == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_visited(*অনুসন্ধান_ctx);
	    dir = path_option;
	    did_findfile_init = FALSE;
	}

	প্রতি (;;)
	{
	    যদি (did_findfile_init)
	    {
		file_name = vim_findfile(*অনুসন্ধান_ctx);
		যদি (file_name != রিক্ত)
		    ভঙ্গ;

		did_findfile_init = FALSE;
	    }
	    অন্যথা
	    {
		char_u	*buf;
		char_u  *r_ptr;

		যদি (dir == রিক্ত || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*অনুসন্ধান_ctx);
		    *অনুসন্ধান_ctx = রিক্ত;
		    ভঙ্গ;
		}

		যদি ((buf = alloc(MAXPATHL)) == রিক্ত)
		    ভঙ্গ;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*অনুসন্ধান_ctx = vim_findfile_init(buf, *file_to_find, file_to_findlen,
					    r_ptr, 100, FALSE, find_what,
					   *অনুসন্ধান_ctx, FALSE, rel_fname);
		যদি (*অনুসন্ধান_ctx != রিক্ত)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    যদি (file_name == রিক্ত && (options & FNAME_MESS))
    {
	যদি (first == TRUE)
	{
	    যদি (find_what == FINDFILE_DIR)
		semsg(_(e_cant_find_directory_str_in_cdpath), *file_to_find);
	    অন্যথা
		semsg(_(e_cant_find_file_str_in_path), *file_to_find);
	}
	অন্যথা
	{
	    যদি (find_what == FINDFILE_DIR)
		semsg(_(e_no_more_directory_str_found_in_cdpath),
								*file_to_find);
	    অন্যথা
		semsg(_(e_no_more_file_str_found_in_path), *file_to_find);
	}
    }

theend:
#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif
    প্রত্যাগম file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(long গুনতি, linenr_T *file_lnum)
{
    পূর্ণাঙ্ক options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    যদি (VIsual_active)
    {
	পূর্ণাঙ্ক	len;
	char_u	*ptr;

	যদি (get_visual_text(রিক্ত, &ptr, &len) == FAIL)
	    প্রত্যাগম রিক্ত;
	// Only recognize ":123" here
	যদি (file_lnum != রিক্ত && ptr[len] == ':' && SAFE_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	প্রত্যাগম find_file_name_in_path(ptr, len, options,
						     গুনতি, curbuf->b_ffname);
    }
    প্রত্যাগম file_name_at_cursor(options | FNAME_HYP, গুনতি, file_lnum);
}

/*
 * Return the file name under or after the cursor.
 *
 * The 'path' option is searched if the file name is not absolute.
 * The string returned has been alloc'ed and should be freed by the caller.
 * NULL is returned if the file name or file is not found.
 *
 * options:
 * FNAME_MESS	    give error messages
 * FNAME_EXP	    expand to path
 * FNAME_HYP	    check for hypertext link
 * FNAME_INCL	    apply "includeexpr"
 */
    char_u *
file_name_at_cursor(পূর্ণাঙ্ক options, long গুনতি, linenr_T *file_lnum)
{
    প্রত্যাগম file_name_in_সারি(ml_get_curline(),
		      curwin->w_cursor.col, options, গুনতি, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_in_সারি(
    char_u	*সারি,
    পূর্ণাঙ্ক		col,
    পূর্ণাঙ্ক		options,
    long	গুনতি,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		in_type = TRUE;
    পূর্ণাঙ্ক		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = সারি + col;
    যাবত্ (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    যদি (*ptr == NUL)		// nothing found
    {
	যদি (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	প্রত্যাগম রিক্ত;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    যাবত্ (ptr > সারি)
    {
	যদি (has_mbyte && (len = (*mb_head_off)(সারি, ptr - 1)) > 0)
	    ptr -= len + 1;
	অন্যথা যদি (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && path_is_url(ptr - 1)))
	    --ptr;
	অন্যথা
	    ভঙ্গ;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    যাবত্ (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && path_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != রিক্ত))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	যদি ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    যদি (in_type && path_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	অন্যথা
	    in_type = FALSE;

	যদি (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	যদি (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	অন্যথা
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    যদি (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != রিক্ত
						       && ptr[len - 2] != '.')
	--len;

    যদি (file_lnum != রিক্ত)
    {
	char_u	*p;
	বর্ণ	*match_text = " line ";		// english
	size_t	match_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	যদি (STRNCMP(p, match_text, match_textlen) == 0)
	    p += match_textlen;
	অন্যথা
	{
	    // no match with english, try localized
	    match_text = _(সারি_বার্তা);
	    match_textlen = STRLEN(match_text);

	    যদি (STRNCMP(p, match_text, match_textlen) == 0)
		p += match_textlen;
	    অন্যথা
		p = skipwhite(p);
	}
	যদি (*p != NUL)
	{
	    যদি (!SAFE_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    যদি (SAFE_isdigit(*p))
		*file_lnum = (পূর্ণাঙ্ক)getdigits(&p);
	}
    }

    প্রত্যাগম find_file_name_in_path(ptr, len, options, গুনতি, rel_fname);
}

#যদি defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    স্থির char_u *
eval_includeexpr(char_u *ptr, পূর্ণাঙ্ক len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    set_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_to_string_safe(curbuf->b_p_inex,
	    was_set_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    set_vim_var_string(VV_FNAME, রিক্ত, 0);
    current_sctx = save_sctx;
    প্রত্যাগম res;
}
#endif

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
find_file_name_in_path(
    char_u	*ptr,
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		options,
    long	গুনতি,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    পূর্ণাঙ্ক		c;
#যদি defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    char_u	*tofree = রিক্ত;
#endif

    যদি (len == 0)
	প্রত্যাগম রিক্ত;

#যদি defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    যদি ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	যদি (tofree != রিক্ত)
	{
	    ptr = tofree;
	    len = (পূর্ণাঙ্ক)STRLEN(ptr);
	}
    }
#endif

    যদি (options & FNAME_EXP)
    {
	char_u	*file_to_find = রিক্ত;
	বর্ণ	*অনুসন্ধান_ctx = রিক্ত;

	file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &অনুসন্ধান_ctx);

#যদি defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	যদি (file_name == রিক্ত
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    যদি (tofree != রিক্ত)
	    {
		ptr = tofree;
		len = (পূর্ণাঙ্ক)STRLEN(ptr);
		file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &অনুসন্ধান_ctx);
	    }
	}
#endif
	যদি (file_name == রিক্ত && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_find_file_str_in_path_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	যাবত্ (file_name != রিক্ত && --গুনতি > 0)
	{
	    vim_free(file_name);
	    file_name = find_file_in_path(ptr, len, options, FALSE, rel_fname,
						   &file_to_find, &অনুসন্ধান_ctx);
	}

	vim_free(file_to_find);
	vim_findfile_cleanup(অনুসন্ধান_ctx);
    }
    অন্যথা
	file_name = vim_strnsave(ptr, len);

#যদি defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#endif

    প্রত্যাগম file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    স্থির char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_end = fname;
    char_u	*next_dir_end = fname;
    পূর্ণাঙ্ক		look_for_sep = TRUE;
    char_u	*p;

    প্রতি (p = fname; *p != NUL; )
    {
	যদি (vim_ispathsep(*p))
	{
	    যদি (look_for_sep)
	    {
		next_dir_end = p;
		look_for_sep = FALSE;
	    }
	}
	অন্যথা
	{
	    যদি (!look_for_sep)
		dir_end = next_dir_end;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    প্রত্যাগম dir_end;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    পূর্ণাঙ্ক
vim_ispathlistsep(পূর্ণাঙ্ক c)
{
#ifdef UNIX
    প্রত্যাগম (c == ':');
#অন্যথা
    প্রত্যাগম (c == ';');	// might not be right for every system...
#endif
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    স্থির পূর্ণাঙ্ক
find_previous_pathsep(char_u *path, char_u **psep)
{
    // skip the current separator
    যদি (*psep > path && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    যাবত্ (*psep > path)
    {
	যদি (vim_ispathsep(**psep))
	    প্রত্যাগম OK;
	MB_PTR_BACK(path, *psep);
    }

    প্রত্যাগম FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    স্থির পূর্ণাঙ্ক
is_unique(char_u *maybe_unique, garray_T *gap, পূর্ণাঙ্ক i)
{
    পূর্ণাঙ্ক	    j;
    পূর্ণাঙ্ক	    candidate_len = (পূর্ণাঙ্ক)STRLEN(maybe_unique);
    পূর্ণাঙ্ক	    other_path_len;
    char_u  **other_paths = (char_u **)gap->ga_data;
    char_u  *rival;

    প্রতি (j = 0; j < gap->ga_len; j++)
    {
	যদি (j == i)
	    অনুবর্ত;  // don't compare it with itself

	other_path_len = (পূর্ণাঙ্ক)STRLEN(other_paths[j]);
	যদি (other_path_len < candidate_len)
	    অনুবর্ত;  // it's different when it's shorter

	rival = other_paths[j] + other_path_len - candidate_len;
	যদি (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    প্রত্যাগম FALSE;  // match
    }

    প্রত্যাগম TRUE;  // no match found
}

/*
 * Split the 'path' option into an array of strings in garray_T.  Relative
 * paths are expanded to their equivalent fullpath.  This includes the "."
 * (relative to current buffer directory) and empty path (relative to current
 * directory) notations.
 *
 * TODO: handle upward search (;) and path limiter (**N) notations by
 * expanding each into their equivalent path(s).
 */
    স্থির শূন্য
expand_path_option(
	char_u		*curdir,
	char_u		*path_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    যদি ((buf = alloc(MAXPATHL)) == রিক্ত)
	প্রত্যাগম;

    যাবত্ (*path_option != NUL)
    {
	buflen = copy_option_part(&path_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	যদি (vim_strchr(buf, '`') != রিক্ত)
	    অনুবর্ত;

	যদি (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    যদি (curbuf->b_ffname == রিক্ত)
		অনুবর্ত;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    যদি (plen + buflen >= MAXPATHL)
		অনুবর্ত;
	    যদি (buf[1] == NUL)
		buf[plen] = NUL;
	    অন্যথা
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	অন্যথা যদি (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    যদি (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	অন্যথা যদি (path_with_url(buf))
	    // URL can't be used here
	    অনুবর্ত;
	অন্যথা যদি (!mch_isFullName(buf))
	{
	    // Expand relative path to their full path equivalent
	    যদি (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    যদি (curdirlen + buflen + 3 > MAXPATHL)
		অনুবর্ত;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	যদি (ga_grow(gap, 1) == FAIL)
	    ভঙ্গ;

#যদি defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	যদি (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#endif

	p = vim_strnsave(buf, buflen);
	যদি (p == রিক্ত)
	    ভঙ্গ;
	((char_u **)gap->ga_data)[gap->ga_len++] = p;
    }

    vim_free(buf);
}

/*
 * Returns a pointer to the file or directory name in "fname" that matches the
 * longest path in "ga"p, or NULL if there is no match. For example:
 *
 *    path: /foo/bar/baz
 *   fname: /foo/bar/baz/quux.txt
 * returns:		 ^this
 */
    স্থির char_u *
get_path_cutoff(char_u *fname, garray_T *gap)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    maxlen = 0;
    char_u  **path_part = (char_u **)gap->ga_data;
    char_u  *cutoff = রিক্ত;

    প্রতি (i = 0; i < gap->ga_len; i++)
    {
	পূর্ণাঙ্ক j = 0;

	যাবত্ ((fname[j] == path_part[i][j]
#যদি defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(path_part[i][j]))
#endif
			     ) && fname[j] != NUL && path_part[i][j] != NUL)
	    j++;
	যদি (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    যদি (cutoff != রিক্ত)
	যাবত্ (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    প্রত্যাগম cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    শূন্য
uniquefy_paths(
	garray_T *gap,
	char_u *pattern,
	char_u *path_option)	// p_path or p_cdpath
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;
    char_u	**fnames = (char_u **)gap->ga_data;
    পূর্ণাঙ্ক		sort_again = FALSE;
    char_u	*pat;
    char_u      *file_pattern;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	path_ga;
    char_u	**in_curdir = রিক্ত;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&path_ga, পরিমাণ(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (পূর্ণাঙ্ক)STRLEN(pattern);
    file_pattern = alloc(len + 2);
    যদি (file_pattern == রিক্ত)
	প্রত্যাগম;
    file_pattern[0] = '*';
    STRCPY(file_pattern + 1, pattern);
    pat = file_pat_to_reg_pat(file_pattern, রিক্ত, রিক্ত, FALSE);
    vim_free(file_pattern);
    যদি (pat == রিক্ত)
	প্রত্যাগম;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    যদি (regmatch.regprog == রিক্ত)
	প্রত্যাগম;

    যদি ((curdir = alloc(MAXPATHL)) == রিক্ত)
	গচ্ছ theend;
    mch_dirname(curdir, MAXPATHL);
    expand_path_option(curdir, path_option, &path_ga);

    in_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    যদি (in_curdir == রিক্ত)
	গচ্ছ theend;

    প্রতি (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *path = fnames[i];
	পূর্ণাঙ্ক	    is_in_curdir;
	char_u	    *dir_end = gettail_dir(path);
	char_u	    *path_cutoff;

	len = (পূর্ণাঙ্ক)STRLEN(path);
	is_in_curdir = fnamencmp(curdir, path, dir_end - path) == 0
					     && curdir[dir_end - path] == NUL;
	যদি (is_in_curdir)
	    in_curdir[i] = vim_strnsave(path, len);

	// Shorten the filename while maintaining its uniqueness
	path_cutoff = get_path_cutoff(path, &path_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	যদি (pattern[0] == '*' && pattern[1] == '*'
		&& vim_ispathsep_nocolon(pattern[2])
		&& path_cutoff != রিক্ত
		&& vim_regexec(&regmatch, path_cutoff, (colnr_T)0)
		&& is_unique(path_cutoff, gap, i))
	{
	    sort_again = TRUE;
	    mch_memmove(path, path_cutoff, STRLEN(path_cutoff) + 1);
	}
	অন্যথা
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = path + len - 1;

	    যাবত্ (find_previous_pathsep(path, &pathsep_p))
	    {
		যদি (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& path_cutoff != রিক্ত && pathsep_p + 1 >= path_cutoff)
		{
		    sort_again = TRUE;
		    mch_memmove(path, pathsep_p + 1,
			    (size_t)((path + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    ভঙ্গ;
		}
	    }
	}

	যদি (mch_isFullName(path))
	{
	    /*
	     * Last resort: shorten relative to curdir if possible.
	     * 'possible' means:
	     * 1. It is under the current directory.
	     * 2. The result is actually shorter than the original.
	     *
	     *	    Before		  curdir	After
	     *	    /foo/bar/file.txt	  /foo/bar	./file.txt
	     *	    c:\foo\bar\file.txt   c:\foo\bar	.\file.txt
	     *	    /file.txt		  /		/file.txt
	     *	    c:\file.txt		  c:\		.\file.txt
	     */
	    short_name = shorten_fname(path, curdir);
	    যদি (short_name != রিক্ত && short_name > path + 1
#যদি defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#endif
		)
	    {
		vim_snprintf((বর্ণ *)path, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    প্রতি (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_path;
	char_u	*path = in_curdir[i];

	যদি (path == রিক্ত)
	    অনুবর্ত;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(path, curdir);
	যদি (short_name == রিক্ত)
	    short_name = path;
	যদি (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    অনুবর্ত;
	}

	rel_pathsize = 1 + পরিমাণ(PATHSEP) + STRLEN(short_name) + 1;
	rel_path = alloc(rel_pathsize);
	যদি (rel_path == রিক্ত)
	    গচ্ছ theend;

	vim_snprintf((বর্ণ *)rel_path, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_path;
	sort_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    যদি (in_curdir != রিক্ত)
    {
	প্রতি (i = 0; i < gap->ga_len; i++)
	    vim_free(in_curdir[i]);
	vim_free(in_curdir);
    }
    ga_clear_strings(&path_ga);
    vim_regfree(regmatch.regprog);

    যদি (sort_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    পূর্ণাঙ্ক
expand_in_path(
    garray_T	*gap,
    char_u	*pattern,
    পূর্ণাঙ্ক		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	path_ga;
    char_u	*paths = রিক্ত;
    পূর্ণাঙ্ক		glob_flags = 0;
    char_u	*path_option = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;

    যদি ((curdir = alloc(MAXPATHL)) == রিক্ত)
	প্রত্যাগম 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&path_ga, পরিমাণ(char_u *), 1);
    যদি (flags & EW_CDPATH)
	expand_path_option(curdir, p_cdpath, &path_ga);
    অন্যথা
	expand_path_option(curdir, path_option, &path_ga);
    vim_free(curdir);
    যদি (path_ga.ga_len == 0)
	প্রত্যাগম 0;

    paths = ga_concat_strings(&path_ga, ",");
    ga_clear_strings(&path_ga);
    যদি (paths == রিক্ত)
	প্রত্যাগম 0;

    যদি (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    যদি (flags & EW_ADDSLASH)
	glob_flags |= WILD_ADD_SLASH;
    globpath(paths, pattern, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    প্রত্যাগম gap->ga_len;
}


/*
 * Converts a file name into a canonical form. It simplifies a file name into
 * its simplest form by stripping out unneeded components, if any.  The
 * resulting file name is simplified in place and will either be the same
 * length as that supplied, or shorter.
 */
    size_t
simplify_filename(char_u *filename)
{
#ifndef AMIGA	    // Amiga doesn't have "..", it uses "/"
    পূর্ণাঙ্ক		components = 0;
    char_u	*p, *tail, *start;
    char_u	*p_end;			    // point to NUL at end of string "p"
    পূর্ণাঙ্ক		stripping_disabled = FALSE;
    পূর্ণাঙ্ক		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_IN_FILENAME
    যদি (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    যদি (vim_ispathsep(*p))
    {
	relative = FALSE;
	কুরু
	    ++p;
	যাবত্ (vim_ispathsep(*p));
    }
    start = p;	    // remember start after "c:/" or "/" or "///"
    p_end = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    যদি (start > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_end - p) + 1);	    // +1 for NUL
	p_end -= (size_t)(p - (filename + 1));
	start = p = filename + 1;
    }
# endif

    কুরু
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	যদি ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	অন্যথা যদি (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	অন্যথা
# endif
	যদি (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_end - (p + 1)) + 1); // remove duplicate "/"
	    --p_end;
	}
	অন্যথা যদি (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    যদি (p == start && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    অন্যথা
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		tail = p + 1;
		যদি (p[1] != NUL)
		    যাবত্ (vim_ispathsep(*tail))
			MB_PTR_ADV(tail);
		অন্যথা যদি (p > start)
		    --p;		// strip preceding path separator

		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);
		p_end -= (size_t)(tail - p);
	    }
	}
	অন্যথা যদি (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    tail = p + 2;
	    যাবত্ (vim_ispathsep(*tail))
		MB_PTR_ADV(tail);

	    যদি (components > 0)		// strip one preceding component
	    {
		পূর্ণাঙ্ক		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		যদি (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    যদি (mch_lstat((বর্ণ *)filename, &st) < 0)
# অন্যথা
			যদি (mch_stat((বর্ণ *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    যাবত্ (p > start && !after_pathsep(start, p))
			MB_PTR_BACK(start, p);

		    যদি (!do_strip)
		    {
			// If the component exists in the file system, check
			// that stripping it won't change the meaning of the
			// file name.  First get information about the
			// unstripped file name.  This may fail if the component
			// to strip is not a searchable directory (but a regular
			// file, for instance), since the trailing "/.." cannot
			// be applied then.  We don't strip it then since we
			// don't want to replace an erroneous file name by
			// a valid one, and we disable stripping of later
			// components.
			saved_char = *tail;
			*tail = NUL;
			যদি (mch_stat((বর্ণ *)filename, &st) >= 0)
			    do_strip = TRUE;
			অন্যথা
			    stripping_disabled = TRUE;
			*tail = saved_char;
# ifdef UNIX
			যদি (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    যদি (p == start && relative)
				(শূন্য)mch_stat(".", &new_st);
			    অন্যথা
			    {
				saved_char = *p;
				*p = NUL;
				(শূন্য)mch_stat((বর্ণ *)filename, &new_st);
				*p = saved_char;
			    }

			    যদি (new_st.st_ino != st.st_ino ||
				new_st.st_dev != st.st_dev)
			    {
				do_strip = FALSE;
				// We don't disable stripping of later
				// components since the unstripped path name is
				// still valid.
			    }
			}
# endif
		    }
		}

		যদি (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = tail;
		    components = 0;
		}
		অন্যথা
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    যদি (p == start && relative && tail[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    অন্যথা
		    {
			যদি (p > start && tail[-1] == '.')
			    --p;

			mch_memmove(p, tail, (size_t)(p_end - tail) + 1);	// strip previous component
			p_end -= (size_t)(tail - p);
		    }

		    --components;
		}
	    }
	    অন্যথা যদি (p == start && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);		// strip ".." or "../"
		p_end -= (size_t)(tail - p);
	    }
	    অন্যথা
	    {
		যদি (p == start + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_end - p) + 1); // strip leading "./"
		    p_end -= 2;
		    tail -= 2;
		}
		p = tail;		// skip to char after ".." or "../"
	    }
	}
	অন্যথা
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } যাবত্ (*p != NUL);

    প্রত্যাগম (size_t)(p_end - filename);
#অন্যথা
    // Don't touch Amiga filenames
    প্রত্যাগম STRLEN(filename);
#endif // !AMIGA
}

#যদি defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    শূন্য
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#endif // FEAT_EVAL
