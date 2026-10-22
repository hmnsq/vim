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

#അന്തര്ഭാവ "vim.h"

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
തരനിർവചനം ഘടന ff_അട്ടി
{
    ഘടന ff_അട്ടി	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_path;
    string_T		ffs_wc_path;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    പൂർണ്ണസംഖ്യ			ffs_filearray_size;
    പൂർണ്ണസംഖ്യ			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    പൂർണ്ണസംഖ്യ			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    പൂർണ്ണസംഖ്യ			ffs_level;

    // Did we already expand '**' to an empty string?
    പൂർണ്ണസംഖ്യ			ffs_star_star_empty;
} ff_അട്ടി_T;

/*
 * type for already visited directories or files.
 */
തരനിർവചനം ഘടന ff_visited
{
    ഘടന ff_visited	*ffv_next;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_path;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ifdef UNIX
    പൂർണ്ണസംഖ്യ			ffv_dev_valid;	// ffv_dev and ffv_ino were set
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
തരനിർവചനം ഘടന ff_visited_list_hdr
{
    ഘടന ff_visited_list_hdr	*ffvl_next;

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
തരനിർവചനം ഘടന ff_തിരയൽ_ctx_T
{
    ff_അട്ടി_T			*ffsc_അട്ടി_ptr;
    ff_visited_list_hdr_T	*ffsc_visited_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_list;
    ff_visited_list_hdr_T	*ffsc_visited_lists_list;
    ff_visited_list_hdr_T	*ffsc_dir_visited_lists_list;
    string_T			ffsc_file_to_തിരയൽ;
    string_T			ffsc_start_dir;
    string_T			ffsc_fix_path;
    string_T			ffsc_wc_path;
    പൂർണ്ണസംഖ്യ				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    പൂർണ്ണസംഖ്യ				ffsc_find_what;
    പൂർണ്ണസംഖ്യ				ffsc_tagfile;
} ff_തിരയൽ_ctx_T;

// locally needed functions
സ്ഥിതം പൂർണ്ണസംഖ്യ ff_check_visited(ff_visited_T **, char_u *, size_t, char_u *, size_t);
സ്ഥിതം ശൂന്യം vim_findfile_free_visited(ശൂന്യം *തിരയൽ_ctx_arg);
സ്ഥിതം ശൂന്യം vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp);
സ്ഥിതം ശൂന്യം ff_free_visited_list(ff_visited_T *vl);
സ്ഥിതം ff_visited_list_hdr_T* ff_get_visited_list(char_u *, size_t, ff_visited_list_hdr_T **list_headp);

സ്ഥിതം ശൂന്യം ff_തള്ളുക(ff_തിരയൽ_ctx_T *തിരയൽ_ctx, ff_അട്ടി_T *അട്ടി_ptr);
സ്ഥിതം ff_അട്ടി_T *ff_എടുക്കുക(ff_തിരയൽ_ctx_T *തിരയൽ_ctx);
സ്ഥിതം ശൂന്യം ff_clear(ff_തിരയൽ_ctx_T *തിരയൽ_ctx);
സ്ഥിതം ശൂന്യം ff_free_അട്ടി_element(ff_അട്ടി_T *അട്ടി_ptr);
സ്ഥിതം ff_അട്ടി_T *ff_create_അട്ടി_element(char_u *, size_t, char_u *, size_t, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ ff_path_in_stoplist(char_u *, പൂർണ്ണസംഖ്യ, string_T *);

സ്ഥിതം string_T ff_expand_buffer = {ശൂന്യ, 0};	    // used for expanding filenames

#എങ്കിൽ 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

സ്ഥിതം ശൂന്യം *ff_fn_തിരയൽ_context = ശൂന്യ;

    char_u *
vim_findfirst(char_u *path, char_u *filename, പൂർണ്ണസംഖ്യ level)
{
    ff_fn_തിരയൽ_context =
	vim_findfile_init(path, filename, ശൂന്യ, level, TRUE, FALSE,
		ff_fn_തിരയൽ_context, rel_fname);
    എങ്കിൽ (ശൂന്യ == ff_fn_തിരയൽ_context)
	തിരികെ ശൂന്യ;
    അല്ലെങ്കിൽ
	തിരികെ vim_findnext()
}

    char_u *
vim_findnext(ശൂന്യം)
{
    char_u *ret = vim_findfile(ff_fn_തിരയൽ_context);

    എങ്കിൽ (ശൂന്യ == ret)
    {
	vim_findfile_cleanup(ff_fn_തിരയൽ_context);
	ff_fn_തിരയൽ_context = ശൂന്യ;
    }
    തിരികെ ret;
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
    ശൂന്യം *
vim_findfile_init(
    char_u	*path,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs UNUSED,
    പൂർണ്ണസംഖ്യ		level,
    പൂർണ്ണസംഖ്യ		free_visited,
    പൂർണ്ണസംഖ്യ		find_what,
    ശൂന്യം	*തിരയൽ_ctx_arg,
    പൂർണ്ണസംഖ്യ		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_അട്ടി_T		*sptr;
    ff_തിരയൽ_ctx_T	*തിരയൽ_ctx;
    പൂർണ്ണസംഖ്യ			add_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    എങ്കിൽ (തിരയൽ_ctx_arg != ശൂന്യ)
	തിരയൽ_ctx = തിരയൽ_ctx_arg;
    അല്ലെങ്കിൽ
    {
	തിരയൽ_ctx = ALLOC_CLEAR_ONE(ff_തിരയൽ_ctx_T);
	എങ്കിൽ (തിരയൽ_ctx == ശൂന്യ)
	    പോകുക error_return;
    }
    തിരയൽ_ctx->ffsc_find_what = find_what;
    തിരയൽ_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(തിരയൽ_ctx);

    // clear visited list if wanted
    എങ്കിൽ (free_visited == TRUE)
	vim_findfile_free_visited(തിരയൽ_ctx);
    അല്ലെങ്കിൽ
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	തിരയൽ_ctx->ffsc_visited_list = ff_get_visited_list(filename,
					filenamelen, &തിരയൽ_ctx->ffsc_visited_lists_list);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_visited_list == ശൂന്യ)
	    പോകുക error_return;
	തിരയൽ_ctx->ffsc_dir_visited_list = ff_get_visited_list(filename,
				    filenamelen, &തിരയൽ_ctx->ffsc_dir_visited_lists_list);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_dir_visited_list == ശൂന്യ)
	    പോകുക error_return;
    }

    എങ്കിൽ (ff_expand_buffer.string == ശൂന്യ)
    {
	ff_expand_buffer.length = 0;
	ff_expand_buffer.string = alloc(MAXPATHL);
	എങ്കിൽ (ff_expand_buffer.string == ശൂന്യ)
	    പോകുക error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    എങ്കിൽ (path[0] == '.'
	    && (vim_ispathsep(path[1]) || path[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == ശൂന്യ)
	    && rel_fname != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	len = (പൂർണ്ണസംഖ്യ)(gettail(rel_fname) - rel_fname);

	എങ്കിൽ (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_buffer.string, rel_fname, len);
	    ff_expand_buffer.length = len;

	    തിരയൽ_ctx->ffsc_start_dir.string = FullName_save(ff_expand_buffer.string, FALSE);
	    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string == ശൂന്യ)
		പോകുക error_return;
	    തിരയൽ_ctx->ffsc_start_dir.length = STRLEN(തിരയൽ_ctx->ffsc_start_dir.string);
	}
	അല്ലെങ്കിൽ
	{
	    തിരയൽ_ctx->ffsc_start_dir.length = len;
	    തിരയൽ_ctx->ffsc_start_dir.string = vim_strnsave(rel_fname,
		തിരയൽ_ctx->ffsc_start_dir.length);
	    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string == ശൂന്യ)
		പോകുക error_return;
	}

	എങ്കിൽ (*++path != NUL)
	    ++path;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*path == NUL || !vim_isAbsName(path))
    {
#ifdef BACKSLASH_IN_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	എങ്കിൽ (*path != NUL && path[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = path[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    എങ്കിൽ (vim_FullName(drive, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
		പോകുക error_return;
	    path += 2;
	}
	അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (mch_dirname(ff_expand_buffer.string, MAXPATHL) == FAIL)
	    പോകുക error_return;

	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);

	തിരയൽ_ctx->ffsc_start_dir.length = ff_expand_buffer.length;
	തിരയൽ_ctx->ffsc_start_dir.string = vim_strnsave(ff_expand_buffer.string,
	    തിരയൽ_ctx->ffsc_start_dir.length);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string == ശൂന്യ)
	    പോകുക error_return;
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
    എങ്കിൽ (stopdirs != ശൂന്യ)
    {
	char_u	*walker = stopdirs;
	പൂർണ്ണസംഖ്യ	dircount;

	ആകുമ്പോൾ (*walker == ';')
	    walker++;

	dircount = 1;
	തിരയൽ_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	എങ്കിൽ (തിരയൽ_ctx->ffsc_stopdirs_v != ശൂന്യ)
	{
	    string_T	*tmp;			// for convenience

	    ചെയ്യുക
	    {
		char_u	*helper;
		ശൂന്യം	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(തിരയൽ_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * വലുപ്പം(string_T));
		എങ്കിൽ (ptr)
		    തിരയൽ_ctx->ffsc_stopdirs_v = ptr;
		അല്ലെങ്കിൽ
		    // ignore, keep what we have and continue
		    നിർത്തുക;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		എങ്കിൽ (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_buffer.string, helper, len);
		    ff_expand_buffer.length = len;

		    tmp = &തിരയൽ_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = FullName_save(ff_expand_buffer.string, FALSE);
		    എങ്കിൽ (tmp->string != ശൂന്യ)
			tmp->length = STRLEN(tmp->string);
		}
		അല്ലെങ്കിൽ
		{
		    tmp = &തിരയൽ_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->length = len;
		    tmp->string = vim_strnsave(helper, tmp->length);
		    എങ്കിൽ (tmp->string == ശൂന്യ)
			tmp->length = 0;
		}
		എങ്കിൽ (walker)
		    walker++;
		dircount++;

	    } ആകുമ്പോൾ (walker != ശൂന്യ);

	    tmp = &തിരയൽ_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = ശൂന്യ;
	    tmp->length = 0;
	}
    }

    തിരയൽ_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(path, '*');
    എങ്കിൽ (wc_part != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	llevel;
	അക്ഷരം	*errpt;

	// save the fix part of the path
	തിരയൽ_ctx->ffsc_fix_path.length = (size_t)(wc_part - path);
	തിരയൽ_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    തിരയൽ_ctx->ffsc_fix_path.length);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_fix_path.string == ശൂന്യ)
	    പോകുക error_return;

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
	ആകുമ്പോൾ (*wc_part != NUL)
	{
	    എങ്കിൽ (ff_expand_buffer.length + 5 >= MAXPATHL)
	    {
		emsg(_(e_path_too_long_for_completion));
		നിർത്തുക;
	    }
	    എങ്കിൽ (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;

		llevel = strtol((അക്ഷരം *)wc_part, &errpt, 10);
		എങ്കിൽ ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_buffer.string[ff_expand_buffer.length++] = llevel;
		അല്ലെങ്കിൽ എങ്കിൽ ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_buffer.length -= 2;
		അല്ലെങ്കിൽ
		    ff_expand_buffer.string[ff_expand_buffer.length++] = FF_MAX_STAR_STAR_EXPAND;
		wc_part = (char_u *)errpt;
		എങ്കിൽ (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_path_സംഖ്യ_must_be_at_end_of_path_or_be_followed_by_str), PATHSEPSTR);
		    പോകുക error_return;
		}
	    }
	    അല്ലെങ്കിൽ
		ff_expand_buffer.string[ff_expand_buffer.length++] = *wc_part++;
	}
	ff_expand_buffer.string[ff_expand_buffer.length] = NUL;

	തിരയൽ_ctx->ffsc_wc_path.length = ff_expand_buffer.length;
	തിരയൽ_ctx->ffsc_wc_path.string = vim_strnsave(ff_expand_buffer.string,
	    തിരയൽ_ctx->ffsc_wc_path.length);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_wc_path.string == ശൂന്യ)
	    പോകുക error_return;
    }
    അല്ലെങ്കിൽ
    {
	തിരയൽ_ctx->ffsc_fix_path.length = STRLEN(path);
	തിരയൽ_ctx->ffsc_fix_path.string = vim_strnsave(path,
	    തിരയൽ_ctx->ffsc_fix_path.length);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_fix_path.string == ശൂന്യ)
	    പോകുക error_return;
    }

    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string == ശൂന്യ)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	തിരയൽ_ctx->ffsc_start_dir.length = തിരയൽ_ctx->ffsc_fix_path.length;
	തിരയൽ_ctx->ffsc_start_dir.string = vim_strnsave(തിരയൽ_ctx->ffsc_fix_path.string,
	    തിരയൽ_ctx->ffsc_start_dir.length);
	എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string == ശൂന്യ)
	    പോകുക error_return;
	തിരയൽ_ctx->ffsc_fix_path.string[0] = NUL;
	തിരയൽ_ctx->ffsc_fix_path.length = 0;
    }

    // create an absolute path
    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.length
	    + തിരയൽ_ctx->ffsc_fix_path.length + 3 >= MAXPATHL)
    {
	emsg(_(e_path_too_long_for_completion));
	പോകുക error_return;
    }

    add_sep = !after_pathsep(തിരയൽ_ctx->ffsc_start_dir.string,
	തിരയൽ_ctx->ffsc_start_dir.string + തിരയൽ_ctx->ffsc_start_dir.length);
    ff_expand_buffer.length = vim_snprintf(
	    (അക്ഷരം *)ff_expand_buffer.string,
	    MAXPATHL,
	    "%s%s",
	    തിരയൽ_ctx->ffsc_start_dir.string,
	    add_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_buffer.length + തിരയൽ_ctx->ffsc_fix_path.length + 1;
	char_u	*buf = alloc(bufsize);

	എങ്കിൽ (buf == ശൂന്യ)
	    പോകുക error_return;

	vim_snprintf(
		(അക്ഷരം *)buf,
		bufsize,
		"%s%s",
		ff_expand_buffer.string,
		തിരയൽ_ctx->ffsc_fix_path.string);
	എങ്കിൽ (mch_isdir(buf))
	{
	    എങ്കിൽ (തിരയൽ_ctx->ffsc_fix_path.length > 0)
	    {
		add_sep = !after_pathsep(തിരയൽ_ctx->ffsc_fix_path.string,
		    തിരയൽ_ctx->ffsc_fix_path.string + തിരയൽ_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(അക്ഷരം *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%s%s",
			തിരയൽ_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }
	}
	അല്ലെങ്കിൽ
	{
	    char_u *p = gettail(തിരയൽ_ctx->ffsc_fix_path.string);
	    പൂർണ്ണസംഖ്യ    len = (പൂർണ്ണസംഖ്യ)തിരയൽ_ctx->ffsc_fix_path.length;

	    എങ്കിൽ (p > തിരയൽ_ctx->ffsc_fix_path.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (പൂർണ്ണസംഖ്യ)(p - തിരയൽ_ctx->ffsc_fix_path.string) - 1;
		എങ്കിൽ ((len >= 2
			&& STRNCMP(തിരയൽ_ctx->ffsc_fix_path.string, "..", 2) == 0)
			&& (len == 2 || തിരയൽ_ctx->ffsc_fix_path.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    പോകുക error_return;
		}

		add_sep = !after_pathsep(തിരയൽ_ctx->ffsc_fix_path.string,
		    തിരയൽ_ctx->ffsc_fix_path.string + തിരയൽ_ctx->ffsc_fix_path.length);
		ff_expand_buffer.length += vim_snprintf(
			(അക്ഷരം *)ff_expand_buffer.string + ff_expand_buffer.length,
			MAXPATHL - ff_expand_buffer.length,
			"%.*s%s",
			len,
			തിരയൽ_ctx->ffsc_fix_path.string,
			add_sep ? PATHSEPSTR : "");
	    }

	    എങ്കിൽ (തിരയൽ_ctx->ffsc_wc_path.string != ശൂന്യ)
	    {
		size_t	tempsize = (തിരയൽ_ctx->ffsc_fix_path.length - len)
				+ തിരയൽ_ctx->ffsc_wc_path.length
				+ 1;
		char_u	*temp = alloc(tempsize);

		എങ്കിൽ (temp == ശൂന്യ)
		{
		    vim_free(buf);
		    vim_free(temp);
		    പോകുക error_return;
		}

		തിരയൽ_ctx->ffsc_wc_path.length = vim_snprintf(
			    (അക്ഷരം *)temp,
			    tempsize,
			    "%s%s",
			    തിരയൽ_ctx->ffsc_fix_path.string + len,
			    തിരയൽ_ctx->ffsc_wc_path.string);
		vim_free(തിരയൽ_ctx->ffsc_wc_path.string);
		തിരയൽ_ctx->ffsc_wc_path.string = temp;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_അട്ടി_element(ff_expand_buffer.string,
			ff_expand_buffer.length,
			തിരയൽ_ctx->ffsc_wc_path.string,
			തിരയൽ_ctx->ffsc_wc_path.length,
			level,
			0);

    എങ്കിൽ (sptr == ശൂന്യ)
	പോകുക error_return;

    ff_തള്ളുക(തിരയൽ_ctx, sptr);

    തിരയൽ_ctx->ffsc_file_to_തിരയൽ.length = filenamelen;
    തിരയൽ_ctx->ffsc_file_to_തിരയൽ.string = vim_strnsave(filename,
	തിരയൽ_ctx->ffsc_file_to_തിരയൽ.length);
    എങ്കിൽ (തിരയൽ_ctx->ffsc_file_to_തിരയൽ.string == ശൂന്യ)
	പോകുക error_return;

    തിരികെ തിരയൽ_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(തിരയൽ_ctx);
    തിരികെ ശൂന്യ;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_end = ശൂന്യ;	    // points to NUL at end of string "r_ptr"

    ആകുമ്പോൾ (*r_ptr != NUL && *r_ptr != ';')
    {
	എങ്കിൽ (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    എങ്കിൽ (r_ptr_end == ശൂന്യ)
		r_ptr_end = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_end - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_end;
	}
	r_ptr++;
    }
    എങ്കിൽ (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*r_ptr == NUL)
	r_ptr = ശൂന്യ;
    തിരികെ r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    ശൂന്യം
vim_findfile_cleanup(ശൂന്യം *ctx)
{
    എങ്കിൽ (ctx == ശൂന്യ)
	തിരികെ;

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
vim_findfile(ശൂന്യം *തിരയൽ_ctx_arg)
{
    string_T	file_path;
    string_T	rest_of_wildcards;
    char_u	*path_end = ശൂന്യ;
    ff_അട്ടി_T	*stackp;
    ff_തിരയൽ_ctx_T *തിരയൽ_ctx;

    എങ്കിൽ (തിരയൽ_ctx_arg == ശൂന്യ)
	തിരികെ ശൂന്യ;

    തിരയൽ_ctx = (ff_തിരയൽ_ctx_T *)തിരയൽ_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    എങ്കിൽ ((file_path.string = alloc(MAXPATHL)) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // store the end of the start dir -- needed for upward search
    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string != ശൂന്യ)
	path_end = &തിരയൽ_ctx->ffsc_start_dir.string[
					    തിരയൽ_ctx->ffsc_start_dir.length];

    // upward search loop
    വേണ്ടി (;;)
    {
	// downward search loop
	വേണ്ടി (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    എങ്കിൽ (got_int)
		നിർത്തുക;

	    // get directory to work on from stack
	    stackp = ff_എടുക്കുക(തിരയൽ_ctx);
	    എങ്കിൽ (stackp == ശൂന്യ)
		നിർത്തുക;

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
	    എങ്കിൽ (stackp->ffs_filearray == ശൂന്യ
		    && ff_check_visited(&തിരയൽ_ctx->ffsc_dir_visited_list
							  ->ffvl_visited_list,
			    stackp->ffs_fix_path.string,
			    stackp->ffs_fix_path.length,
			    stackp->ffs_wc_path.string,
			    stackp->ffs_wc_path.length) == FAIL)
	    {
#ifdef FF_VERBOSE
		എങ്കിൽ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		    // don't overwrite this either
		    സന്ദേശം_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		ff_free_അട്ടി_element(stackp);
		തുടരുക;
	    }
#ifdef FF_VERBOSE
	    അല്ലെങ്കിൽ എങ്കിൽ (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_path.string, stackp->ffs_wc_path.string);
		// don't overwrite this either
		സന്ദേശം_puts("\n");
		verbose_leave_scroll();
	    }
#endif

	    // check depth
	    എങ്കിൽ (stackp->ffs_level <= 0)
	    {
		ff_free_അട്ടി_element(stackp);
		തുടരുക;
	    }

	    file_path.string[0] = NUL;
	    file_path.length = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    എങ്കിൽ (stackp->ffs_filearray == ശൂന്യ)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_path.string;
		dirptrs[1] = ശൂന്യ;

		// if we have a start dir copy it in
		എങ്കിൽ (!vim_isAbsName(stackp->ffs_fix_path.string)
						&& തിരയൽ_ctx->ffsc_start_dir.string)
		{
		    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.length + 1 < MAXPATHL)
		    {
			പൂർണ്ണസംഖ്യ add_sep = !after_pathsep(തിരയൽ_ctx->ffsc_start_dir.string,
			    തിരയൽ_ctx->ffsc_start_dir.string + തിരയൽ_ctx->ffsc_start_dir.length);
			file_path.length = vim_snprintf(
			    (അക്ഷരം *)file_path.string,
			    MAXPATHL,
			    "%s%s",
			    തിരയൽ_ctx->ffsc_start_dir.string,
			    add_sep ? PATHSEPSTR : "");
		    }
		    അല്ലെങ്കിൽ
		    {
			ff_free_അട്ടി_element(stackp);
			പോകുക fail;
		    }
		}

		// append the fix part of the search path
		എങ്കിൽ (file_path.length + stackp->ffs_fix_path.length + 1 < MAXPATHL)
		{
		    പൂർണ്ണസംഖ്യ add_sep = !after_pathsep(stackp->ffs_fix_path.string,
			stackp->ffs_fix_path.string + stackp->ffs_fix_path.length);
		    file_path.length += vim_snprintf(
			(അക്ഷരം *)file_path.string + file_path.length,
			MAXPATHL - file_path.length,
			"%s%s",
			stackp->ffs_fix_path.string,
			add_sep ? PATHSEPSTR : "");
		}
		അല്ലെങ്കിൽ
		{
		    ff_free_അട്ടി_element(stackp);
		    പോകുക fail;
		}

		rest_of_wildcards.string = stackp->ffs_wc_path.string;
		rest_of_wildcards.length = stackp->ffs_wc_path.length;
		എങ്കിൽ (*rest_of_wildcards.string != NUL)
		{
		    എങ്കിൽ (STRNCMP(rest_of_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_of_wildcards.string + 2;

			എങ്കിൽ (*p > 0)
			{
			    (*p)--;
			    എങ്കിൽ (file_path.length + 1 < MAXPATHL)
				file_path.string[file_path.length++] = '*';
			    അല്ലെങ്കിൽ
			    {
				ff_free_അട്ടി_element(stackp);
				പോകുക fail;
			    }
			}

			എങ്കിൽ (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_of_wildcards.string,
					rest_of_wildcards.string + 3,
					(size_t)(rest_of_wildcards.length - 3) + 1);    // +1 for NUL
			    rest_of_wildcards.length -= 3;
			    stackp->ffs_wc_path.length = rest_of_wildcards.length;
			}
			അല്ലെങ്കിൽ
			{
			    rest_of_wildcards.string += 3;
			    rest_of_wildcards.length -= 3;
			}

			എങ്കിൽ (stackp->ffs_star_star_empty == 0)
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
		    ആകുമ്പോൾ (*rest_of_wildcards.string
			    && !vim_ispathsep(*rest_of_wildcards.string))
		    {
			എങ്കിൽ (file_path.length + 1 < MAXPATHL)
			{
			    file_path.string[file_path.length++] = *rest_of_wildcards.string++;
			    --rest_of_wildcards.length;
			}
			അല്ലെങ്കിൽ
			{
			    ff_free_അട്ടി_element(stackp);
			    പോകുക fail;
			}
		    }

		    file_path.string[file_path.length] = NUL;
		    എങ്കിൽ (vim_ispathsep(*rest_of_wildcards.string))
		    {
			rest_of_wildcards.string++;
			rest_of_wildcards.length--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		എങ്കിൽ (path_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    എങ്കിൽ (stackp->ffs_filearray != ശൂന്യ
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_path.length)) != ശൂന്യ)
			stackp->ffs_filearray_size = 1;
		    അല്ലെങ്കിൽ
			stackp->ffs_filearray_size = 0;
		}
		അല്ലെങ്കിൽ
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == ശൂന്യ) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_size,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    അല്ലെങ്കിൽ
	    {
		rest_of_wildcards.string = &stackp->ffs_wc_path.string[
							stackp->ffs_wc_path.length];
		rest_of_wildcards.length = 0;
	    }

	    എങ്കിൽ (stackp->ffs_stage == 0)
	    {
		പൂർണ്ണസംഖ്യ i;

		// this is the first time we work on this directory
		എങ്കിൽ (*rest_of_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    വേണ്ടി (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			എങ്കിൽ (!path_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    തുടരുക;   // not a directory

			// prepare the filename to be checked for existence
			// below
			len = STRLEN(stackp->ffs_filearray[i]);
			എങ്കിൽ (len + 1 + തിരയൽ_ctx->ffsc_file_to_തിരയൽ.length
									< MAXPATHL)
			{
			    പൂർണ്ണസംഖ്യ	add_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_path.length = vim_snprintf(
				    (അക്ഷരം *)file_path.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    add_sep ? PATHSEPSTR : "",
				    തിരയൽ_ctx->ffsc_file_to_തിരയൽ.string);
			}
			അല്ലെങ്കിൽ
			{
			    ff_free_അട്ടി_element(stackp);
			    പോകുക fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_path.length;
			എങ്കിൽ (തിരയൽ_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			അല്ലെങ്കിൽ
			    suf = curbuf->b_p_sua;
			വേണ്ടി (;;)
			{
			    // if file exists and we didn't already find it
			    എങ്കിൽ ((path_with_url(file_path.string)
				  || (mch_getperm(file_path.string) >= 0
				      && (തിരയൽ_ctx->ffsc_find_what
							      == FINDFILE_BOTH
					  || ((തിരയൽ_ctx->ffsc_find_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_path.string)))))
#ifndef FF_VERBOSE
				    && (ff_check_visited(
					    &തിരയൽ_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					    file_path.string,
					    file_path.length,
					    (char_u *)"", 0) == OK)
#endif
			       )
			    {
#ifdef FF_VERBOSE
				എങ്കിൽ (ff_check_visited(
					    &തിരയൽ_ctx->ffsc_visited_list
							   ->ffvl_visited_list,
					      file_path.string,
					      file_path.length,
					      (char_u *)"", 0) == FAIL)
				{
				    എങ്കിൽ (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_path.string);
					// don't overwrite this either
					സന്ദേശം_puts("\n");
					verbose_leave_scroll();
				    }
				    തുടരുക;
				}
#endif

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_തള്ളുക(തിരയൽ_ctx, stackp);

				എങ്കിൽ (!path_with_url(file_path.string))
				    file_path.length = simplify_filename(file_path.string);

				എങ്കിൽ (mch_dirname(ff_expand_buffer.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
				    p = shorten_fname(file_path.string,
							    ff_expand_buffer.string);
				    എങ്കിൽ (p != ശൂന്യ)
				    {
					mch_memmove(file_path.string, p,
					    (size_t)((file_path.string + file_path.length) - p) + 1);  // +1 for NUL
					file_path.length -= (p - file_path.string);
				    }
				}
#ifdef FF_VERBOSE
				എങ്കിൽ (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_path.string);
				    // don't overwrite this either
				    സന്ദേശം_puts("\n");
				    verbose_leave_scroll();
				}
#endif
				തിരികെ file_path.string;
			    }

			    // Not found or found already, try next suffix.
			    എങ്കിൽ (*suf == NUL)
				നിർത്തുക;
			    file_path.length = len + copy_option_part(&suf,
				  file_path.string + len,
				  (പൂർണ്ണസംഖ്യ)(MAXPATHL - len), ",");
			}
		    }
		}
		അല്ലെങ്കിൽ
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    വേണ്ടി (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		    {
			എങ്കിൽ (!mch_isdir(stackp->ffs_filearray[i]))
			    തുടരുക;	// not a directory

			ff_തള്ളുക(തിരയൽ_ctx,
				ff_create_അട്ടി_element(
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
	    എങ്കിൽ (STRNCMP(stackp->ffs_wc_path.string, "**", 2) == 0)
	    {
		പൂർണ്ണസംഖ്യ i;

		വേണ്ടി (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_size; ++i)
		{
		    എങ്കിൽ (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_path.string) == 0)
			തുടരുക; // don't repush same directory
		    എങ്കിൽ (!mch_isdir(stackp->ffs_filearray[i]))
			തുടരുക;   // not a directory
		    ff_തള്ളുക(തിരയൽ_ctx,
			    ff_create_അട്ടി_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_path.string,
				stackp->ffs_wc_path.length,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_അട്ടി_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.string
		&& തിരയൽ_ctx->ffsc_stopdirs_v != ശൂന്യ && !got_int)
	{
	    ff_അട്ടി_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    പൂർണ്ണസംഖ്യ plen = (path_end - തിരയൽ_ctx->ffsc_start_dir.string)
							  + (*path_end != NUL);

	    // is the last starting directory in the stop list?
	    എങ്കിൽ (ff_path_in_stoplist(തിരയൽ_ctx->ffsc_start_dir.string,
				    plen, തിരയൽ_ctx->ffsc_stopdirs_v) == TRUE)
		നിർത്തുക;

	    // cut of last dir
	    ആകുമ്പോൾ (path_end > തിരയൽ_ctx->ffsc_start_dir.string
						  && vim_ispathsep(*path_end))
		path_end--;
	    ആകുമ്പോൾ (path_end > തിരയൽ_ctx->ffsc_start_dir.string
					      && !vim_ispathsep(path_end[-1]))
		path_end--;
	    *path_end = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    തിരയൽ_ctx->ffsc_start_dir.length = (size_t)(path_end - തിരയൽ_ctx->ffsc_start_dir.string);
	    path_end--;

	    എങ്കിൽ (*തിരയൽ_ctx->ffsc_start_dir.string == NUL)
		നിർത്തുക;

	    എങ്കിൽ (തിരയൽ_ctx->ffsc_start_dir.length + 1
		    + തിരയൽ_ctx->ffsc_fix_path.length < MAXPATHL)
	    {
		പൂർണ്ണസംഖ്യ add_sep = !after_pathsep(തിരയൽ_ctx->ffsc_start_dir.string,
			    തിരയൽ_ctx->ffsc_start_dir.string + തിരയൽ_ctx->ffsc_start_dir.length);
		file_path.length = vim_snprintf(
			(അക്ഷരം *)file_path.string,
			MAXPATHL,
			"%s%s%s",
			തിരയൽ_ctx->ffsc_start_dir.string,
			add_sep ? PATHSEPSTR : "",
			തിരയൽ_ctx->ffsc_fix_path.string);
	    }
	    അല്ലെങ്കിൽ
		പോകുക fail;

	    // create a new stack entry
	    sptr = ff_create_അട്ടി_element(file_path.string, file_path.length,
		    തിരയൽ_ctx->ffsc_wc_path.string, തിരയൽ_ctx->ffsc_wc_path.length,
		    തിരയൽ_ctx->ffsc_level, 0);
	    എങ്കിൽ (sptr == ശൂന്യ)
		നിർത്തുക;
	    ff_തള്ളുക(തിരയൽ_ctx, sptr);
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

fail:
    vim_free(file_path.string);
    തിരികെ ശൂന്യ;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    സ്ഥിതം ശൂന്യം
vim_findfile_free_visited(ശൂന്യം *തിരയൽ_ctx_arg)
{
    ff_തിരയൽ_ctx_T *തിരയൽ_ctx;

    എങ്കിൽ (തിരയൽ_ctx_arg == ശൂന്യ)
	തിരികെ;

    തിരയൽ_ctx = (ff_തിരയൽ_ctx_T *)തിരയൽ_ctx_arg;
    vim_findfile_free_visited_list(&തിരയൽ_ctx->ffsc_visited_lists_list);
    vim_findfile_free_visited_list(&തിരയൽ_ctx->ffsc_dir_visited_lists_list);
}

    സ്ഥിതം ശൂന്യം
vim_findfile_free_visited_list(ff_visited_list_hdr_T **list_headp)
{
    ff_visited_list_hdr_T *vp;

    ആകുമ്പോൾ (*list_headp != ശൂന്യ)
    {
	vp = (*list_headp)->ffvl_next;
	ff_free_visited_list((*list_headp)->ffvl_visited_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
ff_free_visited_list(ff_visited_T *vl)
{
    ff_visited_T *vp;

    ആകുമ്പോൾ (vl != ശൂന്യ)
    {
	vp = vl->ffv_next;
	vim_free(vl->ffv_wc_path);
	vim_free(vl);
	vl = vp;
    }
    vl = ശൂന്യ;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    സ്ഥിതം ff_visited_list_hdr_T*
ff_get_visited_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_visited_list_hdr_T	**list_headp)
{
    ff_visited_list_hdr_T  *retptr = ശൂന്യ;

    // check if a visited list for the given filename exists
    എങ്കിൽ (*list_headp != ശൂന്യ)
    {
	retptr = *list_headp;
	ആകുമ്പോൾ (retptr != ശൂന്യ)
	{
	    എങ്കിൽ (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ifdef FF_VERBOSE
		എങ്കിൽ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    സന്ദേശം_puts("\n");
		    verbose_leave_scroll();
		}
#endif
		തിരികെ retptr;
	    }
	    retptr = retptr->ffvl_next;
	}
    }

#ifdef FF_VERBOSE
    എങ്കിൽ (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	സന്ദേശം_puts("\n");
	verbose_leave_scroll();
    }
#endif

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_visited_list_hdr_T);
    എങ്കിൽ (retptr == ശൂന്യ)
	തിരികെ ശൂന്യ;

    retptr->ffvl_visited_list = ശൂന്യ;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    എങ്കിൽ (retptr->ffvl_filename == ശൂന്യ)
    {
	vim_free(retptr);
	തിരികെ ശൂന്യ;
    }
    retptr->ffvl_next = *list_headp;
    *list_headp = retptr;

    തിരികെ retptr;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ff_wc_equal(char_u *s1, char_u *s2)
{
    പൂർണ്ണസംഖ്യ		i, j;
    പൂർണ്ണസംഖ്യ		c1 = NUL;
    പൂർണ്ണസംഖ്യ		c2 = NUL;
    പൂർണ്ണസംഖ്യ		prev1 = NUL;
    പൂർണ്ണസംഖ്യ		prev2 = NUL;

    എങ്കിൽ (s1 == s2)
	തിരികെ TRUE;

    എങ്കിൽ (s1 == ശൂന്യ || s2 == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	എങ്കിൽ ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    തിരികെ FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    തിരികെ s1[i] == s2[j];
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
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
    പൂർണ്ണസംഖ്യ			url = FALSE;
#endif

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    എങ്കിൽ (path_with_url(fname))
    {
	vim_strncpy(ff_expand_buffer.string, fname, fnamelen);
	ff_expand_buffer.length = fnamelen;
#ifdef UNIX
	url = TRUE;
#endif
    }
    അല്ലെങ്കിൽ
    {
	ff_expand_buffer.string[0] = NUL;
	ff_expand_buffer.length = 0;
#ifdef UNIX
	എങ്കിൽ (mch_stat((അക്ഷരം *)fname, &st) < 0)
	    തിരികെ FAIL;
#അല്ലെങ്കിൽ
	എങ്കിൽ (vim_FullName(fname, ff_expand_buffer.string, MAXPATHL, TRUE) == FAIL)
	    തിരികെ FAIL;
	ff_expand_buffer.length = STRLEN(ff_expand_buffer.string);
#endif
    }

    // check against list of already visited files
    വേണ്ടി (vp = *visited_list; vp != ശൂന്യ; vp = vp->ffv_next)
    {
	എങ്കിൽ (
#ifdef UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#endif
		fnamecmp(vp->ffv_fname, ff_expand_buffer.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    എങ്കിൽ (ff_wc_equal(vp->ffv_wc_path, wc_path) == TRUE)
		// already visited
		തിരികെ FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_visited_T, ffv_fname) + ff_expand_buffer.length + 1);
    എങ്കിൽ (vp == ശൂന്യ)
	തിരികെ OK;

#ifdef UNIX
    എങ്കിൽ (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    അല്ലെങ്കിൽ
    {
	vp->ffv_dev_valid = FALSE;
#endif
	STRCPY(vp->ffv_fname, ff_expand_buffer.string);
#ifdef UNIX
    }
#endif
    എങ്കിൽ (wc_path != ശൂന്യ)
	vp->ffv_wc_path = vim_strnsave(wc_path, wc_pathlen);
    അല്ലെങ്കിൽ
	vp->ffv_wc_path = ശൂന്യ;

    vp->ffv_next = *visited_list;
    *visited_list = vp;

    തിരികെ OK;
}

/*
 * create stack element from given path pieces
 */
    സ്ഥിതം ff_അട്ടി_T *
ff_create_അട്ടി_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    പൂർണ്ണസംഖ്യ		level,
    പൂർണ്ണസംഖ്യ		star_star_empty)
{
    ff_അട്ടി_T	*new;

    new = ALLOC_ONE(ff_അട്ടി_T);
    എങ്കിൽ (new == ശൂന്യ)
	തിരികെ ശൂന്യ;

    new->ffs_prev	   = ശൂന്യ;
    new->ffs_filearray	   = ശൂന്യ;
    new->ffs_filearray_size = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_star_star_empty = star_star_empty;

    // the following saves NULL pointer checks in vim_findfile
    എങ്കിൽ (fix_part == ശൂന്യ)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_path.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_path.length = fix_partlen;

    എങ്കിൽ (wc_part == ശൂന്യ)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_path.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_path.length = wc_partlen;

    എങ്കിൽ (new->ffs_fix_path.string == ശൂന്യ || new->ffs_wc_path.string == ശൂന്യ)
    {
	ff_free_അട്ടി_element(new);
	new = ശൂന്യ;
    }

    തിരികെ new;
}

/*
 * Push a dir on the directory stack.
 */
    സ്ഥിതം ശൂന്യം
ff_തള്ളുക(ff_തിരയൽ_ctx_T *തിരയൽ_ctx, ff_അട്ടി_T *അട്ടി_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    എങ്കിൽ (അട്ടി_ptr == ശൂന്യ)
	തിരികെ;

    അട്ടി_ptr->ffs_prev = തിരയൽ_ctx->ffsc_അട്ടി_ptr;
    തിരയൽ_ctx->ffsc_അട്ടി_ptr = അട്ടി_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    സ്ഥിതം ff_അട്ടി_T *
ff_എടുക്കുക(ff_തിരയൽ_ctx_T *തിരയൽ_ctx)
{
    ff_അട്ടി_T  *sptr;

    sptr = തിരയൽ_ctx->ffsc_അട്ടി_ptr;
    എങ്കിൽ (തിരയൽ_ctx->ffsc_അട്ടി_ptr != ശൂന്യ)
	തിരയൽ_ctx->ffsc_അട്ടി_ptr = തിരയൽ_ctx->ffsc_അട്ടി_ptr->ffs_prev;

    തിരികെ sptr;
}

/*
 * free the given stack element
 */
    സ്ഥിതം ശൂന്യം
ff_free_അട്ടി_element(ff_അട്ടി_T *അട്ടി_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(അട്ടി_ptr->ffs_fix_path);
    VIM_CLEAR_STRING(അട്ടി_ptr->ffs_wc_path);

    എങ്കിൽ (അട്ടി_ptr->ffs_filearray != ശൂന്യ)
	FreeWild(അട്ടി_ptr->ffs_filearray_size, അട്ടി_ptr->ffs_filearray);

    vim_free(അട്ടി_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    സ്ഥിതം ശൂന്യം
ff_clear(ff_തിരയൽ_ctx_T *തിരയൽ_ctx)
{
    ff_അട്ടി_T   *sptr;

    // clear up stack
    ആകുമ്പോൾ ((sptr = ff_എടുക്കുക(തിരയൽ_ctx)) != ശൂന്യ)
	ff_free_അട്ടി_element(sptr);

    എങ്കിൽ (തിരയൽ_ctx->ffsc_stopdirs_v != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ  i = 0;

	ആകുമ്പോൾ (തിരയൽ_ctx->ffsc_stopdirs_v[i].string != ശൂന്യ)
	{
	    vim_free(തിരയൽ_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(തിരയൽ_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(തിരയൽ_ctx->ffsc_file_to_തിരയൽ);
    VIM_CLEAR_STRING(തിരയൽ_ctx->ffsc_start_dir);
    VIM_CLEAR_STRING(തിരയൽ_ctx->ffsc_fix_path);
    VIM_CLEAR_STRING(തിരയൽ_ctx->ffsc_wc_path);
    തിരയൽ_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ff_path_in_stoplist(char_u *path, പൂർണ്ണസംഖ്യ path_len, string_T *stopdirs_v)
{
    പൂർണ്ണസംഖ്യ		i = 0;

    // eat up trailing path separators, except the first
    ആകുമ്പോൾ (path_len > 1 && vim_ispathsep(path[path_len - 1]))
	path_len--;

    // if no path consider it as match
    എങ്കിൽ (path_len == 0)
	തിരികെ TRUE;

    വേണ്ടി (i = 0; stopdirs_v[i].string != ശൂന്യ; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	എങ്കിൽ (fnamencmp(stopdirs_v[i].string, path, path_len) == 0
		&& ((പൂർണ്ണസംഖ്യ)stopdirs_v[i].length <= path_len
		    || vim_ispathsep(stopdirs_v[i].string[path_len])))
	    തിരികെ TRUE;

    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ		len,		// length of file name
    പൂർണ്ണസംഖ്യ		options,
    പൂർണ്ണസംഖ്യ		first,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    അക്ഷരം	**തിരയൽ_ctx)	// in/out: state of the search
{
    തിരികെ find_file_in_path_option(ptr, len, options, first,
	    *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_to_find, തിരയൽ_ctx);
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_findfile(ശൂന്യം)
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
    പൂർണ്ണസംഖ്യ		len,		// length of file name
    പൂർണ്ണസംഖ്യ		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_to_find,	// in/out: modified copy of file name
    അക്ഷരം	**തിരയൽ_ctx)	// in/out: state of the search
{
    തിരികെ find_file_in_path_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_to_find, തിരയൽ_ctx);
}

    char_u *
find_file_in_path_option(
    char_u	*ptr,		// file name
    പൂർണ്ണസംഖ്യ		len,		// length of file name
    പൂർണ്ണസംഖ്യ		options,
    പൂർണ്ണസംഖ്യ		first,		// use count'th matching file name
    char_u	*path_option,	// p_path or p_cdpath
    പൂർണ്ണസംഖ്യ		find_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_to_find,	// in/out: modified copy of file name
    അക്ഷരം	**തിരയൽ_ctx_arg) // in/out: state of the search
{
    ff_തിരയൽ_ctx_T	**തിരയൽ_ctx = (ff_തിരയൽ_ctx_T **)തിരയൽ_ctx_arg;
    സ്ഥിതം char_u	*dir;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		did_findfile_init = FALSE;
    char_u		*file_name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			rel_to_curdir;
#ifdef AMIGA
    ഘടന Process	*proc = (ഘടന Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif
    സ്ഥിതം size_t	file_to_findlen = 0;

    എങ്കിൽ (first == TRUE)
    {
	char_u	save_char;

	എങ്കിൽ (len == 0)
	    തിരികെ ശൂന്യ;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_to_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, ശൂന്യ, TRUE, ശൂന്യ);
	ptr[len] = save_char;

	vim_free(*file_to_find);
	*file_to_find = vim_strnsave(NameBuff, file_to_findlen);
	എങ്കിൽ (*file_to_find == ശൂന്യ)	// out of memory
	{
	    file_name = ശൂന്യ;
	    file_to_findlen = 0;
	    പോകുക theend;
	}
	എങ്കിൽ (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    വേണ്ടി (ptr = *file_to_find; *ptr != NUL; ++ptr)
		എങ്കിൽ (ptr[0] == '\\' && ptr[1] == ' ')
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
    എങ്കിൽ (vim_isAbsName(*file_to_find)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_to_curdir
#എങ്കിൽ defined(MSWIN)
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
	എങ്കിൽ (first == TRUE)
	{
	    പൂർണ്ണസംഖ്യ		l;
	    പൂർണ്ണസംഖ്യ		run;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    എങ്കിൽ (path_with_url(*file_to_find))
	    {
		file_name = vim_strnsave(*file_to_find, file_to_findlen);
		പോകുക theend;
	    }

	    എങ്കിൽ (rel_fname != ശൂന്യ)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    വേണ്ടി (run = 1; run <= 2; ++run)
	    {
		l = (പൂർണ്ണസംഖ്യ)file_to_findlen;
		എങ്കിൽ (run == 1
			&& rel_to_curdir
			&& (options & FNAME_REL)
			&& rel_fname != ശൂന്യ
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(അക്ഷരം *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(പൂർണ്ണസംഖ്യ)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_to_find);
		}
		അല്ലെങ്കിൽ
		{
		    STRCPY(NameBuff, *file_to_find);
		    run = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (mch_getperm(NameBuff) >= 0
			     && (find_what == FINDFILE_BOTH
				 || ((find_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			പോകുക theend;
		    }
		    എങ്കിൽ (*suffix == NUL)
			നിർത്തുക;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	എങ്കിൽ (first == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_visited(*തിരയൽ_ctx);
	    dir = path_option;
	    did_findfile_init = FALSE;
	}

	വേണ്ടി (;;)
	{
	    എങ്കിൽ (did_findfile_init)
	    {
		file_name = vim_findfile(*തിരയൽ_ctx);
		എങ്കിൽ (file_name != ശൂന്യ)
		    നിർത്തുക;

		did_findfile_init = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		char_u	*buf;
		char_u  *r_ptr;

		എങ്കിൽ (dir == ശൂന്യ || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*തിരയൽ_ctx);
		    *തിരയൽ_ctx = ശൂന്യ;
		    നിർത്തുക;
		}

		എങ്കിൽ ((buf = alloc(MAXPATHL)) == ശൂന്യ)
		    നിർത്തുക;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*തിരയൽ_ctx = vim_findfile_init(buf, *file_to_find, file_to_findlen,
					    r_ptr, 100, FALSE, find_what,
					   *തിരയൽ_ctx, FALSE, rel_fname);
		എങ്കിൽ (*തിരയൽ_ctx != ശൂന്യ)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    എങ്കിൽ (file_name == ശൂന്യ && (options & FNAME_MESS))
    {
	എങ്കിൽ (first == TRUE)
	{
	    എങ്കിൽ (find_what == FINDFILE_DIR)
		semsg(_(e_cant_find_directory_str_in_cdpath), *file_to_find);
	    അല്ലെങ്കിൽ
		semsg(_(e_cant_find_file_str_in_path), *file_to_find);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (find_what == FINDFILE_DIR)
		semsg(_(e_no_more_directory_str_found_in_cdpath),
								*file_to_find);
	    അല്ലെങ്കിൽ
		semsg(_(e_no_more_file_str_found_in_path), *file_to_find);
	}
    }

theend:
#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif
    തിരികെ file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(long എണ്ണം, linenr_T *file_lnum)
{
    പൂർണ്ണസംഖ്യ options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    എങ്കിൽ (VIsual_active)
    {
	പൂർണ്ണസംഖ്യ	len;
	char_u	*ptr;

	എങ്കിൽ (get_visual_text(ശൂന്യ, &ptr, &len) == FAIL)
	    തിരികെ ശൂന്യ;
	// Only recognize ":123" here
	എങ്കിൽ (file_lnum != ശൂന്യ && ptr[len] == ':' && SAFE_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	തിരികെ find_file_name_in_path(ptr, len, options,
						     എണ്ണം, curbuf->b_ffname);
    }
    തിരികെ file_name_at_cursor(options | FNAME_HYP, എണ്ണം, file_lnum);
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
file_name_at_cursor(പൂർണ്ണസംഖ്യ options, long എണ്ണം, linenr_T *file_lnum)
{
    തിരികെ file_name_in_വരി(ml_get_curline(),
		      curwin->w_cursor.col, options, എണ്ണം, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_in_വരി(
    char_u	*വരി,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		options,
    long	എണ്ണം,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		in_type = TRUE;
    പൂർണ്ണസംഖ്യ		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = വരി + col;
    ആകുമ്പോൾ (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    എങ്കിൽ (*ptr == NUL)		// nothing found
    {
	എങ്കിൽ (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	തിരികെ ശൂന്യ;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    ആകുമ്പോൾ (ptr > വരി)
    {
	എങ്കിൽ (has_mbyte && (len = (*mb_head_off)(വരി, ptr - 1)) > 0)
	    ptr -= len + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && path_is_url(ptr - 1)))
	    --ptr;
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    ആകുമ്പോൾ (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && path_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != ശൂന്യ))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	എങ്കിൽ ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    എങ്കിൽ (in_type && path_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	അല്ലെങ്കിൽ
	    in_type = FALSE;

	എങ്കിൽ (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	എങ്കിൽ (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	അല്ലെങ്കിൽ
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    എങ്കിൽ (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != ശൂന്യ
						       && ptr[len - 2] != '.')
	--len;

    എങ്കിൽ (file_lnum != ശൂന്യ)
    {
	char_u	*p;
	അക്ഷരം	*match_text = " line ";		// english
	size_t	match_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	എങ്കിൽ (STRNCMP(p, match_text, match_textlen) == 0)
	    p += match_textlen;
	അല്ലെങ്കിൽ
	{
	    // no match with english, try localized
	    match_text = _(വരി_സന്ദേശം);
	    match_textlen = STRLEN(match_text);

	    എങ്കിൽ (STRNCMP(p, match_text, match_textlen) == 0)
		p += match_textlen;
	    അല്ലെങ്കിൽ
		p = skipwhite(p);
	}
	എങ്കിൽ (*p != NUL)
	{
	    എങ്കിൽ (!SAFE_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    എങ്കിൽ (SAFE_isdigit(*p))
		*file_lnum = (പൂർണ്ണസംഖ്യ)getdigits(&p);
	}
    }

    തിരികെ find_file_name_in_path(ptr, len, options, എണ്ണം, rel_fname);
}

#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    സ്ഥിതം char_u *
eval_includeexpr(char_u *ptr, പൂർണ്ണസംഖ്യ len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    set_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_to_string_safe(curbuf->b_p_inex,
	    was_set_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    set_vim_var_string(VV_FNAME, ശൂന്യ, 0);
    current_sctx = save_sctx;
    തിരികെ res;
}
#endif

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
find_file_name_in_path(
    char_u	*ptr,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		options,
    long	എണ്ണം,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    പൂർണ്ണസംഖ്യ		c;
#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    char_u	*tofree = ശൂന്യ;
#endif

    എങ്കിൽ (len == 0)
	തിരികെ ശൂന്യ;

#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    എങ്കിൽ ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	എങ്കിൽ (tofree != ശൂന്യ)
	{
	    ptr = tofree;
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(ptr);
	}
    }
#endif

    എങ്കിൽ (options & FNAME_EXP)
    {
	char_u	*file_to_find = ശൂന്യ;
	അക്ഷരം	*തിരയൽ_ctx = ശൂന്യ;

	file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &തിരയൽ_ctx);

#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	എങ്കിൽ (file_name == ശൂന്യ
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		ptr = tofree;
		len = (പൂർണ്ണസംഖ്യ)STRLEN(ptr);
		file_name = find_file_in_path(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_to_find, &തിരയൽ_ctx);
	    }
	}
#endif
	എങ്കിൽ (file_name == ശൂന്യ && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_find_file_str_in_path_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	ആകുമ്പോൾ (file_name != ശൂന്യ && --എണ്ണം > 0)
	{
	    vim_free(file_name);
	    file_name = find_file_in_path(ptr, len, options, FALSE, rel_fname,
						   &file_to_find, &തിരയൽ_ctx);
	}

	vim_free(file_to_find);
	vim_findfile_cleanup(തിരയൽ_ctx);
    }
    അല്ലെങ്കിൽ
	file_name = vim_strnsave(ptr, len);

#എങ്കിൽ defined(FEAT_FIND_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#endif

    തിരികെ file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    സ്ഥിതം char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_end = fname;
    char_u	*next_dir_end = fname;
    പൂർണ്ണസംഖ്യ		look_for_sep = TRUE;
    char_u	*p;

    വേണ്ടി (p = fname; *p != NUL; )
    {
	എങ്കിൽ (vim_ispathsep(*p))
	{
	    എങ്കിൽ (look_for_sep)
	    {
		next_dir_end = p;
		look_for_sep = FALSE;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!look_for_sep)
		dir_end = next_dir_end;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    തിരികെ dir_end;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    പൂർണ്ണസംഖ്യ
vim_ispathlistsep(പൂർണ്ണസംഖ്യ c)
{
#ifdef UNIX
    തിരികെ (c == ':');
#അല്ലെങ്കിൽ
    തിരികെ (c == ';');	// might not be right for every system...
#endif
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_previous_pathsep(char_u *path, char_u **psep)
{
    // skip the current separator
    എങ്കിൽ (*psep > path && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    ആകുമ്പോൾ (*psep > path)
    {
	എങ്കിൽ (vim_ispathsep(**psep))
	    തിരികെ OK;
	MB_PTR_BACK(path, *psep);
    }

    തിരികെ FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_unique(char_u *maybe_unique, garray_T *gap, പൂർണ്ണസംഖ്യ i)
{
    പൂർണ്ണസംഖ്യ	    j;
    പൂർണ്ണസംഖ്യ	    candidate_len = (പൂർണ്ണസംഖ്യ)STRLEN(maybe_unique);
    പൂർണ്ണസംഖ്യ	    other_path_len;
    char_u  **other_paths = (char_u **)gap->ga_data;
    char_u  *rival;

    വേണ്ടി (j = 0; j < gap->ga_len; j++)
    {
	എങ്കിൽ (j == i)
	    തുടരുക;  // don't compare it with itself

	other_path_len = (പൂർണ്ണസംഖ്യ)STRLEN(other_paths[j]);
	എങ്കിൽ (other_path_len < candidate_len)
	    തുടരുക;  // it's different when it's shorter

	rival = other_paths[j] + other_path_len - candidate_len;
	എങ്കിൽ (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    തിരികെ FALSE;  // match
    }

    തിരികെ TRUE;  // no match found
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
    സ്ഥിതം ശൂന്യം
expand_path_option(
	char_u		*curdir,
	char_u		*path_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    എങ്കിൽ ((buf = alloc(MAXPATHL)) == ശൂന്യ)
	തിരികെ;

    ആകുമ്പോൾ (*path_option != NUL)
    {
	buflen = copy_option_part(&path_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	എങ്കിൽ (vim_strchr(buf, '`') != ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    എങ്കിൽ (curbuf->b_ffname == ശൂന്യ)
		തുടരുക;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    എങ്കിൽ (plen + buflen >= MAXPATHL)
		തുടരുക;
	    എങ്കിൽ (buf[1] == NUL)
		buf[plen] = NUL;
	    അല്ലെങ്കിൽ
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    എങ്കിൽ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (path_with_url(buf))
	    // URL can't be used here
	    തുടരുക;
	അല്ലെങ്കിൽ എങ്കിൽ (!mch_isFullName(buf))
	{
	    // Expand relative path to their full path equivalent
	    എങ്കിൽ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    എങ്കിൽ (curdirlen + buflen + 3 > MAXPATHL)
		തുടരുക;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	    നിർത്തുക;

#എങ്കിൽ defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	എങ്കിൽ (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#endif

	p = vim_strnsave(buf, buflen);
	എങ്കിൽ (p == ശൂന്യ)
	    നിർത്തുക;
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
    സ്ഥിതം char_u *
get_path_cutoff(char_u *fname, garray_T *gap)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    maxlen = 0;
    char_u  **path_part = (char_u **)gap->ga_data;
    char_u  *cutoff = ശൂന്യ;

    വേണ്ടി (i = 0; i < gap->ga_len; i++)
    {
	പൂർണ്ണസംഖ്യ j = 0;

	ആകുമ്പോൾ ((fname[j] == path_part[i][j]
#എങ്കിൽ defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(path_part[i][j]))
#endif
			     ) && fname[j] != NUL && path_part[i][j] != NUL)
	    j++;
	എങ്കിൽ (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    എങ്കിൽ (cutoff != ശൂന്യ)
	ആകുമ്പോൾ (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    തിരികെ cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    ശൂന്യം
uniquefy_paths(
	garray_T *gap,
	char_u *pattern,
	char_u *path_option)	// p_path or p_cdpath
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len;
    char_u	**fnames = (char_u **)gap->ga_data;
    പൂർണ്ണസംഖ്യ		sort_again = FALSE;
    char_u	*pat;
    char_u      *file_pattern;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	path_ga;
    char_u	**in_curdir = ശൂന്യ;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&path_ga, വലുപ്പം(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (പൂർണ്ണസംഖ്യ)STRLEN(pattern);
    file_pattern = alloc(len + 2);
    എങ്കിൽ (file_pattern == ശൂന്യ)
	തിരികെ;
    file_pattern[0] = '*';
    STRCPY(file_pattern + 1, pattern);
    pat = file_pat_to_reg_pat(file_pattern, ശൂന്യ, ശൂന്യ, FALSE);
    vim_free(file_pattern);
    എങ്കിൽ (pat == ശൂന്യ)
	തിരികെ;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ ((curdir = alloc(MAXPATHL)) == ശൂന്യ)
	പോകുക theend;
    mch_dirname(curdir, MAXPATHL);
    expand_path_option(curdir, path_option, &path_ga);

    in_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    എങ്കിൽ (in_curdir == ശൂന്യ)
	പോകുക theend;

    വേണ്ടി (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *path = fnames[i];
	പൂർണ്ണസംഖ്യ	    is_in_curdir;
	char_u	    *dir_end = gettail_dir(path);
	char_u	    *path_cutoff;

	len = (പൂർണ്ണസംഖ്യ)STRLEN(path);
	is_in_curdir = fnamencmp(curdir, path, dir_end - path) == 0
					     && curdir[dir_end - path] == NUL;
	എങ്കിൽ (is_in_curdir)
	    in_curdir[i] = vim_strnsave(path, len);

	// Shorten the filename while maintaining its uniqueness
	path_cutoff = get_path_cutoff(path, &path_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	എങ്കിൽ (pattern[0] == '*' && pattern[1] == '*'
		&& vim_ispathsep_nocolon(pattern[2])
		&& path_cutoff != ശൂന്യ
		&& vim_regexec(&regmatch, path_cutoff, (colnr_T)0)
		&& is_unique(path_cutoff, gap, i))
	{
	    sort_again = TRUE;
	    mch_memmove(path, path_cutoff, STRLEN(path_cutoff) + 1);
	}
	അല്ലെങ്കിൽ
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = path + len - 1;

	    ആകുമ്പോൾ (find_previous_pathsep(path, &pathsep_p))
	    {
		എങ്കിൽ (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& path_cutoff != ശൂന്യ && pathsep_p + 1 >= path_cutoff)
		{
		    sort_again = TRUE;
		    mch_memmove(path, pathsep_p + 1,
			    (size_t)((path + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    നിർത്തുക;
		}
	    }
	}

	എങ്കിൽ (mch_isFullName(path))
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
	    എങ്കിൽ (short_name != ശൂന്യ && short_name > path + 1
#എങ്കിൽ defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#endif
		)
	    {
		vim_snprintf((അക്ഷരം *)path, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    വേണ്ടി (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_path;
	char_u	*path = in_curdir[i];

	എങ്കിൽ (path == ശൂന്യ)
	    തുടരുക;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(path, curdir);
	എങ്കിൽ (short_name == ശൂന്യ)
	    short_name = path;
	എങ്കിൽ (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    തുടരുക;
	}

	rel_pathsize = 1 + വലുപ്പം(PATHSEP) + STRLEN(short_name) + 1;
	rel_path = alloc(rel_pathsize);
	എങ്കിൽ (rel_path == ശൂന്യ)
	    പോകുക theend;

	vim_snprintf((അക്ഷരം *)rel_path, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_path;
	sort_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    എങ്കിൽ (in_curdir != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < gap->ga_len; i++)
	    vim_free(in_curdir[i]);
	vim_free(in_curdir);
    }
    ga_clear_strings(&path_ga);
    vim_regfree(regmatch.regprog);

    എങ്കിൽ (sort_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    പൂർണ്ണസംഖ്യ
expand_in_path(
    garray_T	*gap,
    char_u	*pattern,
    പൂർണ്ണസംഖ്യ		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	path_ga;
    char_u	*paths = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		glob_flags = 0;
    char_u	*path_option = *curbuf->b_p_path == NUL ? p_path : curbuf->b_p_path;

    എങ്കിൽ ((curdir = alloc(MAXPATHL)) == ശൂന്യ)
	തിരികെ 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&path_ga, വലുപ്പം(char_u *), 1);
    എങ്കിൽ (flags & EW_CDPATH)
	expand_path_option(curdir, p_cdpath, &path_ga);
    അല്ലെങ്കിൽ
	expand_path_option(curdir, path_option, &path_ga);
    vim_free(curdir);
    എങ്കിൽ (path_ga.ga_len == 0)
	തിരികെ 0;

    paths = ga_concat_strings(&path_ga, ",");
    ga_clear_strings(&path_ga);
    എങ്കിൽ (paths == ശൂന്യ)
	തിരികെ 0;

    എങ്കിൽ (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    എങ്കിൽ (flags & EW_ADDSLASH)
	glob_flags |= WILD_ADD_SLASH;
    globpath(paths, pattern, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    തിരികെ gap->ga_len;
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
    പൂർണ്ണസംഖ്യ		components = 0;
    char_u	*p, *tail, *start;
    char_u	*p_end;			    // point to NUL at end of string "p"
    പൂർണ്ണസംഖ്യ		stripping_disabled = FALSE;
    പൂർണ്ണസംഖ്യ		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_IN_FILENAME
    എങ്കിൽ (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    എങ്കിൽ (vim_ispathsep(*p))
    {
	relative = FALSE;
	ചെയ്യുക
	    ++p;
	ആകുമ്പോൾ (vim_ispathsep(*p));
    }
    start = p;	    // remember start after "c:/" or "/" or "///"
    p_end = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    എങ്കിൽ (start > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_end - p) + 1);	    // +1 for NUL
	p_end -= (size_t)(p - (filename + 1));
	start = p = filename + 1;
    }
# endif

    ചെയ്യുക
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	എങ്കിൽ ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	അല്ലെങ്കിൽ
# endif
	എങ്കിൽ (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_end - (p + 1)) + 1); // remove duplicate "/"
	    --p_end;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    എങ്കിൽ (p == start && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    അല്ലെങ്കിൽ
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		tail = p + 1;
		എങ്കിൽ (p[1] != NUL)
		    ആകുമ്പോൾ (vim_ispathsep(*tail))
			MB_PTR_ADV(tail);
		അല്ലെങ്കിൽ എങ്കിൽ (p > start)
		    --p;		// strip preceding path separator

		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);
		p_end -= (size_t)(tail - p);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    tail = p + 2;
	    ആകുമ്പോൾ (vim_ispathsep(*tail))
		MB_PTR_ADV(tail);

	    എങ്കിൽ (components > 0)		// strip one preceding component
	    {
		പൂർണ്ണസംഖ്യ		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		എങ്കിൽ (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    എങ്കിൽ (mch_lstat((അക്ഷരം *)filename, &st) < 0)
# അല്ലെങ്കിൽ
			എങ്കിൽ (mch_stat((അക്ഷരം *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    ആകുമ്പോൾ (p > start && !after_pathsep(start, p))
			MB_PTR_BACK(start, p);

		    എങ്കിൽ (!do_strip)
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
			എങ്കിൽ (mch_stat((അക്ഷരം *)filename, &st) >= 0)
			    do_strip = TRUE;
			അല്ലെങ്കിൽ
			    stripping_disabled = TRUE;
			*tail = saved_char;
# ifdef UNIX
			എങ്കിൽ (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    എങ്കിൽ (p == start && relative)
				(ശൂന്യം)mch_stat(".", &new_st);
			    അല്ലെങ്കിൽ
			    {
				saved_char = *p;
				*p = NUL;
				(ശൂന്യം)mch_stat((അക്ഷരം *)filename, &new_st);
				*p = saved_char;
			    }

			    എങ്കിൽ (new_st.st_ino != st.st_ino ||
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

		എങ്കിൽ (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = tail;
		    components = 0;
		}
		അല്ലെങ്കിൽ
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    എങ്കിൽ (p == start && relative && tail[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (p > start && tail[-1] == '.')
			    --p;

			mch_memmove(p, tail, (size_t)(p_end - tail) + 1);	// strip previous component
			p_end -= (size_t)(tail - p);
		    }

		    --components;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (p == start && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, tail, (size_t)(p_end - tail) + 1);		// strip ".." or "../"
		p_end -= (size_t)(tail - p);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (p == start + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_end - p) + 1); // strip leading "./"
		    p_end -= 2;
		    tail -= 2;
		}
		p = tail;		// skip to char after ".." or "../"
	    }
	}
	അല്ലെങ്കിൽ
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } ആകുമ്പോൾ (*p != NUL);

    തിരികെ (size_t)(p_end - filename);
#അല്ലെങ്കിൽ
    // Don't touch Amiga filenames
    തിരികെ STRLEN(filename);
#endif // !AMIGA
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    ശൂന്യം
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#endif // FEAT_EVAL
