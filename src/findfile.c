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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ff_ਢੇਰ
{
    ਬਣਤਰ ff_ਢੇਰ	*ffs_prev;

    // the fix part (no wildcards) and the part containing the wildcards
    // of the search path
    string_T		ffs_fix_ਰਸਤਾ;
    string_T		ffs_wc_ਰਸਤਾ;

    // files/dirs found in the above directory, matched by the first wildcard
    // of wc_part
    char_u		**ffs_filearray;
    ਪੂਰਨ			ffs_filearray_ਮਾਪ;
    ਪੂਰਨ			ffs_filearray_cur;   // needed for partly handled dirs

    // to store status of partly handled directories
    // 0: we work on this directory for the first time
    // 1: this directory was partly searched in an earlier step
    ਪੂਰਨ			ffs_stage;

    // How deep are we in the directory tree?
    // Counts backward from value of level parameter to vim_findfile_init
    ਪੂਰਨ			ffs_level;

    // Did we already expand '**' to an empty string?
    ਪੂਰਨ			ffs_ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ;
} ff_ਢੇਰ_T;

/*
 * type for already visited directories or files.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ff_ਦੇਖਿਆ
{
    ਬਣਤਰ ff_ਦੇਖਿਆ	*ffv_ਅਗਲਾ;

    // Visited directories are different if the wildcard string are
    // different. So we have to save it.
    char_u		*ffv_wc_ਰਸਤਾ;

    // for unix use inode etc for comparison (needed because of links), else
    // use filename.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪੂਰਨ			ffv_dev_valid;	// ffv_dev and ffv_ino were set
    dev_t		ffv_dev;	// device number
    ino_t		ffv_ino;	// inode number
#ਜੇ_ਅੰਤ
    // The memory for this struct is allocated according to the length of
    // ffv_fname.
    char_u		ffv_fname[1];	// actually longer
} ff_ਦੇਖਿਆ_T;

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ff_ਦੇਖਿਆ_list_hdr
{
    ਬਣਤਰ ff_ਦੇਖਿਆ_list_hdr	*ffvl_ਅਗਲਾ;

    // the filename the attached visited list is for
    char_u			*ffvl_filename;

    ff_ਦੇਖਿਆ_T		*ffvl_ਦੇਖਿਆ_list;

} ff_ਦੇਖਿਆ_list_hdr_T;


/*
 * '**' can be expanded to several directory levels.
 * Set the default maximum depth.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FF_MAX_ਤਾਰਾ_ਤਾਰਾ_EXPAND ((char_u)30)

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ff_ਖੋਜ_ctx_T
{
    ff_ਢੇਰ_T			*ffsc_ਢੇਰ_ptr;
    ff_ਦੇਖਿਆ_list_hdr_T	*ffsc_ਦੇਖਿਆ_list;
    ff_ਦੇਖਿਆ_list_hdr_T	*ffsc_dir_ਦੇਖਿਆ_list;
    ff_ਦੇਖਿਆ_list_hdr_T	*ffsc_ਦੇਖਿਆ_lists_list;
    ff_ਦੇਖਿਆ_list_hdr_T	*ffsc_dir_ਦੇਖਿਆ_lists_list;
    string_T			ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ;
    string_T			ffsc_ਸ਼ੁਰੂ_dir;
    string_T			ffsc_fix_ਰਸਤਾ;
    string_T			ffsc_wc_ਰਸਤਾ;
    ਪੂਰਨ				ffsc_level;
    string_T			*ffsc_stopdirs_v;
    ਪੂਰਨ				ffsc_ਲੱਭੋ_what;
    ਪੂਰਨ				ffsc_tagfile;
} ff_ਖੋਜ_ctx_T;

// locally needed functions
ਸਥਿਰ ਪੂਰਨ ff_check_ਦੇਖਿਆ(ff_ਦੇਖਿਆ_T **, char_u *, size_t, char_u *, size_t);
ਸਥਿਰ ਖਾਲੀ vim_findfile_free_ਦੇਖਿਆ(ਖਾਲੀ *ਖੋਜ_ctx_arg);
ਸਥਿਰ ਖਾਲੀ vim_findfile_free_ਦੇਖਿਆ_list(ff_ਦੇਖਿਆ_list_hdr_T **list_headp);
ਸਥਿਰ ਖਾਲੀ ff_free_ਦੇਖਿਆ_list(ff_ਦੇਖਿਆ_T *vl);
ਸਥਿਰ ff_ਦੇਖਿਆ_list_hdr_T* ff_get_ਦੇਖਿਆ_list(char_u *, size_t, ff_ਦੇਖਿਆ_list_hdr_T **list_headp);

ਸਥਿਰ ਖਾਲੀ ff_ਧੱਕੋ(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx, ff_ਢੇਰ_T *ਢੇਰ_ptr);
ਸਥਿਰ ff_ਢੇਰ_T *ff_ਕੱਢੋ(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx);
ਸਥਿਰ ਖਾਲੀ ff_clear(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx);
ਸਥਿਰ ਖਾਲੀ ff_free_ਢੇਰ_element(ff_ਢੇਰ_T *ਢੇਰ_ptr);
ਸਥਿਰ ff_ਢੇਰ_T *ff_create_ਢੇਰ_element(char_u *, size_t, char_u *, size_t, ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ ff_ਰਸਤਾ_ਵਿੱਚ_stoplist(char_u *, ਪੂਰਨ, string_T *);

ਸਥਿਰ string_T ff_expand_ਬਫਰ = {ਨਿਰਾ, 0};	    // used for expanding filenames

#ਜੇ 0
/*
 * if someone likes findfirst/findnext, here are the functions
 * NOT TESTED!!
 */

ਸਥਿਰ ਖਾਲੀ *ff_fn_ਖੋਜ_context = ਨਿਰਾ;

    char_u *
vim_findfirst(char_u *ਰਸਤਾ, char_u *filename, ਪੂਰਨ level)
{
    ff_fn_ਖੋਜ_context =
	vim_findfile_init(ਰਸਤਾ, filename, ਨਿਰਾ, level, TRUE, FALSE,
		ff_fn_ਖੋਜ_context, rel_fname);
    ਜੇ (ਨਿਰਾ == ff_fn_ਖੋਜ_context)
	ਵਾਪਸ ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ vim_findnext()
}

    char_u *
vim_findnext(ਖਾਲੀ)
{
    char_u *ret = vim_findfile(ff_fn_ਖੋਜ_context);

    ਜੇ (ਨਿਰਾ == ret)
    {
	vim_findfile_cleanup(ff_fn_ਖੋਜ_context);
	ff_fn_ਖੋਜ_context = ਨਿਰਾ;
    }
    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ *
vim_findfile_init(
    char_u	*ਰਸਤਾ,
    char_u	*filename,
    size_t	filenamelen,
    char_u	*stopdirs ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		level,
    ਪੂਰਨ		free_ਦੇਖਿਆ,
    ਪੂਰਨ		ਲੱਭੋ_what,
    ਖਾਲੀ	*ਖੋਜ_ctx_arg,
    ਪੂਰਨ		tagfile,	// expanding names of tags files
    char_u	*rel_fname)	// file name to use for "."
{
    char_u		*wc_part;
    ff_ਢੇਰ_T		*sptr;
    ff_ਖੋਜ_ctx_T	*ਖੋਜ_ctx;
    ਪੂਰਨ			ਜੋੜੋ_sep;

    // If a search context is given by the caller, reuse it, else allocate a
    // new one.
    ਜੇ (ਖੋਜ_ctx_arg != ਨਿਰਾ)
	ਖੋਜ_ctx = ਖੋਜ_ctx_arg;
    ਨਹੀਂਤਾਂ
    {
	ਖੋਜ_ctx = ALLOC_CLEAR_ONE(ff_ਖੋਜ_ctx_T);
	ਜੇ (ਖੋਜ_ctx == ਨਿਰਾ)
	    ਜਾਓ error_return;
    }
    ਖੋਜ_ctx->ffsc_ਲੱਭੋ_what = ਲੱਭੋ_what;
    ਖੋਜ_ctx->ffsc_tagfile = tagfile;

    // clear the search context, but NOT the visited lists
    ff_clear(ਖੋਜ_ctx);

    // clear visited list if wanted
    ਜੇ (free_ਦੇਖਿਆ == TRUE)
	vim_findfile_free_ਦੇਖਿਆ(ਖੋਜ_ctx);
    ਨਹੀਂਤਾਂ
    {
	// Reuse old visited lists. Get the visited list for the given
	// filename. If no list for the current filename exists, creates a new
	// one.
	ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_list = ff_get_ਦੇਖਿਆ_list(filename,
					filenamelen, &ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_lists_list);
	ਜੇ (ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_list == ਨਿਰਾ)
	    ਜਾਓ error_return;
	ਖੋਜ_ctx->ffsc_dir_ਦੇਖਿਆ_list = ff_get_ਦੇਖਿਆ_list(filename,
				    filenamelen, &ਖੋਜ_ctx->ffsc_dir_ਦੇਖਿਆ_lists_list);
	ਜੇ (ਖੋਜ_ctx->ffsc_dir_ਦੇਖਿਆ_list == ਨਿਰਾ)
	    ਜਾਓ error_return;
    }

    ਜੇ (ff_expand_ਬਫਰ.string == ਨਿਰਾ)
    {
	ff_expand_ਬਫਰ.ਲੰਮਾਈ = 0;
	ff_expand_ਬਫਰ.string = alloc(MAXPATHL);
	ਜੇ (ff_expand_ਬਫਰ.string == ਨਿਰਾ)
	    ਜਾਓ error_return;
    }

    // Store information on starting dir now if path is relative.
    // If path is absolute, we do that later.
    ਜੇ (ਰਸਤਾ[0] == '.'
	    && (vim_ispathsep(ਰਸਤਾ[1]) || ਰਸਤਾ[1] == NUL)
	    && (!tagfile || vim_strchr(p_cpo, CPO_DOTTAG) == ਨਿਰਾ)
	    && rel_fname != ਨਿਰਾ)
    {
	ਪੂਰਨ	len = (ਪੂਰਨ)(gettail(rel_fname) - rel_fname);

	ਜੇ (!vim_isAbsName(rel_fname) && len + 1 < MAXPATHL)
	{
	    // Make the start dir an absolute path name.
	    vim_strncpy(ff_expand_ਬਫਰ.string, rel_fname, len);
	    ff_expand_ਬਫਰ.ਲੰਮਾਈ = len;

	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string = ਭਰਿਆ_Name_save(ff_expand_ਬਫਰ.string, FALSE);
	    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == ਨਿਰਾ)
		ਜਾਓ error_return;
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ = STRLEN(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ = len;
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string = vim_strnsave(rel_fname,
		ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
	    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == ਨਿਰਾ)
		ਜਾਓ error_return;
	}

	ਜੇ (*++ਰਸਤਾ != NUL)
	    ++ਰਸਤਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਰਸਤਾ == NUL || !vim_isAbsName(ਰਸਤਾ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	// "c:dir" needs "c:" to be expanded, otherwise use current dir
	ਜੇ (*ਰਸਤਾ != NUL && ਰਸਤਾ[1] == ':')
	{
	    char_u  drive[3];

	    drive[0] = ਰਸਤਾ[0];
	    drive[1] = ':';
	    drive[2] = NUL;
	    ਜੇ (vim_ਭਰਿਆ_Name(drive, ff_expand_ਬਫਰ.string, MAXPATHL, TRUE) == FAIL)
		ਜਾਓ error_return;
	    ਰਸਤਾ += 2;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (mch_dirname(ff_expand_ਬਫਰ.string, MAXPATHL) == FAIL)
	    ਜਾਓ error_return;

	ff_expand_ਬਫਰ.ਲੰਮਾਈ = STRLEN(ff_expand_ਬਫਰ.string);

	ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ = ff_expand_ਬਫਰ.ਲੰਮਾਈ;
	ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string = vim_strnsave(ff_expand_ਬਫਰ.string,
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
	ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == ਨਿਰਾ)
	    ਜਾਓ error_return;
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
    ਜੇ (stopdirs != ਨਿਰਾ)
    {
	char_u	*walker = stopdirs;
	ਪੂਰਨ	dircount;

	ਜਦੋਂ (*walker == ';')
	    walker++;

	dircount = 1;
	ਖੋਜ_ctx->ffsc_stopdirs_v = ALLOC_ONE(string_T);

	ਜੇ (ਖੋਜ_ctx->ffsc_stopdirs_v != ਨਿਰਾ)
	{
	    string_T	*tmp;			// for convenience

	    ਕਰੋ
	    {
		char_u	*helper;
		ਖਾਲੀ	*ptr;
		size_t	len;

		helper = walker;
		ptr = vim_realloc(ਖੋਜ_ctx->ffsc_stopdirs_v,
					   (dircount + 1) * ਆਕਾਰ(string_T));
		ਜੇ (ptr)
		    ਖੋਜ_ctx->ffsc_stopdirs_v = ptr;
		ਨਹੀਂਤਾਂ
		    // ignore, keep what we have and continue
		    ਰੋਕੋ;
		walker = vim_strchr(walker, ';');
		len = walker ? (size_t)(walker - helper) : STRLEN(helper);
		// "" means ascent till top of directory tree.

		ਜੇ (*helper != NUL && !vim_isAbsName(helper)
							 && len + 1 < MAXPATHL)
		{
		    // Make the stop dir an absolute path name.
		    vim_strncpy(ff_expand_ਬਫਰ.string, helper, len);
		    ff_expand_ਬਫਰ.ਲੰਮਾਈ = len;

		    tmp = &ਖੋਜ_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->string = ਭਰਿਆ_Name_save(ff_expand_ਬਫਰ.string, FALSE);
		    ਜੇ (tmp->string != ਨਿਰਾ)
			tmp->ਲੰਮਾਈ = STRLEN(tmp->string);
		}
		ਨਹੀਂਤਾਂ
		{
		    tmp = &ਖੋਜ_ctx->ffsc_stopdirs_v[dircount - 1];
		    tmp->ਲੰਮਾਈ = len;
		    tmp->string = vim_strnsave(helper, tmp->ਲੰਮਾਈ);
		    ਜੇ (tmp->string == ਨਿਰਾ)
			tmp->ਲੰਮਾਈ = 0;
		}
		ਜੇ (walker)
		    walker++;
		dircount++;

	    } ਜਦੋਂ (walker != ਨਿਰਾ);

	    tmp = &ਖੋਜ_ctx->ffsc_stopdirs_v[dircount - 1];
	    tmp->string = ਨਿਰਾ;
	    tmp->ਲੰਮਾਈ = 0;
	}
    }

    ਖੋਜ_ctx->ffsc_level = level;

    /*
     * split into:
     *  -fix path
     *  -wildcard_stuff (might be NULL)
     */
    wc_part = vim_strchr(ਰਸਤਾ, '*');
    ਜੇ (wc_part != ਨਿਰਾ)
    {
	ਪੂਰਨ	llevel;
	ਅੱਖਰ	*errpt;

	// save the fix part of the path
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ = (size_t)(wc_part - ਰਸਤਾ);
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string = vim_strnsave(ਰਸਤਾ,
	    ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ);
	ਜੇ (ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string == ਨਿਰਾ)
	    ਜਾਓ error_return;

	/*
	 * copy wc_path and add restricts to the '**' wildcard.
	 * The octet after a '**' is used as a (binary) counter.
	 * So '**3' is transposed to '**^C' ('^C' is ASCII value 3)
	 * or '**76' is transposed to '**N'( 'N' is ASCII value 76).
	 * If no restrict is given after '**' the default is used.
	 * Due to this technique the path looks awful if you print it as a
	 * string.
	 */
	ff_expand_ਬਫਰ.ਲੰਮਾਈ = 0;
	ਜਦੋਂ (*wc_part != NUL)
	{
	    ਜੇ (ff_expand_ਬਫਰ.ਲੰਮਾਈ + 5 >= MAXPATHL)
	    {
		emsg(_(e_ਰਸਤਾ_too_long_for_completion));
		ਰੋਕੋ;
	    }
	    ਜੇ (STRNCMP(wc_part, "**", 2) == 0)
	    {
		ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ++] = *wc_part++;
		ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ++] = *wc_part++;

		llevel = strtol((ਅੱਖਰ *)wc_part, &errpt, 10);
		ਜੇ ((char_u *)errpt != wc_part && llevel > 0 && llevel < 255)
		    ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ++] = llevel;
		ਨਹੀਂਤਾਂ ਜੇ ((char_u *)errpt != wc_part && llevel == 0)
		    // restrict is 0 -> remove already added '**'
		    ff_expand_ਬਫਰ.ਲੰਮਾਈ -= 2;
		ਨਹੀਂਤਾਂ
		    ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ++] = FF_MAX_ਤਾਰਾ_ਤਾਰਾ_EXPAND;
		wc_part = (char_u *)errpt;
		ਜੇ (*wc_part != NUL && !vim_ispathsep(*wc_part))
		{
		    semsg(_(e_invalid_ਰਸਤਾ_ਸੰਖਿਆ_must_be_at_ਸਿਰਾ_ਦਾ_ਰਸਤਾ_or_be_followed_by_str), PATHSEPSTR);
		    ਜਾਓ error_return;
		}
	    }
	    ਨਹੀਂਤਾਂ
		ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ++] = *wc_part++;
	}
	ff_expand_ਬਫਰ.string[ff_expand_ਬਫਰ.ਲੰਮਾਈ] = NUL;

	ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ = ff_expand_ਬਫਰ.ਲੰਮਾਈ;
	ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string = vim_strnsave(ff_expand_ਬਫਰ.string,
	    ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ);
	ਜੇ (ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string == ਨਿਰਾ)
	    ਜਾਓ error_return;
    }
    ਨਹੀਂਤਾਂ
    {
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ = STRLEN(ਰਸਤਾ);
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string = vim_strnsave(ਰਸਤਾ,
	    ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ);
	ਜੇ (ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string == ਨਿਰਾ)
	    ਜਾਓ error_return;
    }

    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == ਨਿਰਾ)
    {
	// store the fix part as startdir.
	// This is needed if the parameter path is fully qualified.
	ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ = ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ;
	ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string = vim_strnsave(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string,
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
	ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == ਨਿਰਾ)
	    ਜਾਓ error_return;
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string[0] = NUL;
	ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ = 0;
    }

    // create an absolute path
    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ
	    + ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ + 3 >= MAXPATHL)
    {
	emsg(_(e_ਰਸਤਾ_too_long_for_completion));
	ਜਾਓ error_return;
    }

    ਜੋੜੋ_sep = !after_pathsep(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
	ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string + ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
    ff_expand_ਬਫਰ.ਲੰਮਾਈ = vim_snprintf(
	    (ਅੱਖਰ *)ff_expand_ਬਫਰ.string,
	    MAXPATHL,
	    "%s%s",
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
	    ਜੋੜੋ_sep ? PATHSEPSTR : "");

    {
	size_t	bufsize = ff_expand_ਬਫਰ.ਲੰਮਾਈ + ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ + 1;
	char_u	*buf = alloc(bufsize);

	ਜੇ (buf == ਨਿਰਾ)
	    ਜਾਓ error_return;

	vim_snprintf(
		(ਅੱਖਰ *)buf,
		bufsize,
		"%s%s",
		ff_expand_ਬਫਰ.string,
		ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string);
	ਜੇ (mch_isdir(buf))
	{
	    ਜੇ (ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ > 0)
	    {
		ਜੋੜੋ_sep = !after_pathsep(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string,
		    ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string + ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ);
		ff_expand_ਬਫਰ.ਲੰਮਾਈ += vim_snprintf(
			(ਅੱਖਰ *)ff_expand_ਬਫਰ.string + ff_expand_ਬਫਰ.ਲੰਮਾਈ,
			MAXPATHL - ff_expand_ਬਫਰ.ਲੰਮਾਈ,
			"%s%s",
			ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string,
			ਜੋੜੋ_sep ? PATHSEPSTR : "");
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *p = gettail(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string);
	    ਪੂਰਨ    len = (ਪੂਰਨ)ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ;

	    ਜੇ (p > ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string)
	    {
		// do not add '..' to the path and start upwards searching
		len = (ਪੂਰਨ)(p - ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string) - 1;
		ਜੇ ((len >= 2
			&& STRNCMP(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string, "..", 2) == 0)
			&& (len == 2 || ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string[2] == PATHSEP))
		{
		    vim_free(buf);
		    ਜਾਓ error_return;
		}

		ਜੋੜੋ_sep = !after_pathsep(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string,
		    ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string + ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ);
		ff_expand_ਬਫਰ.ਲੰਮਾਈ += vim_snprintf(
			(ਅੱਖਰ *)ff_expand_ਬਫਰ.string + ff_expand_ਬਫਰ.ਲੰਮਾਈ,
			MAXPATHL - ff_expand_ਬਫਰ.ਲੰਮਾਈ,
			"%.*s%s",
			len,
			ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string,
			ਜੋੜੋ_sep ? PATHSEPSTR : "");
	    }

	    ਜੇ (ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string != ਨਿਰਾ)
	    {
		size_t	tempsize = (ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ - len)
				+ ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ
				+ 1;
		char_u	*ਆਰਜ਼ੀ = alloc(tempsize);

		ਜੇ (ਆਰਜ਼ੀ == ਨਿਰਾ)
		{
		    vim_free(buf);
		    vim_free(ਆਰਜ਼ੀ);
		    ਜਾਓ error_return;
		}

		ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf(
			    (ਅੱਖਰ *)ਆਰਜ਼ੀ,
			    tempsize,
			    "%s%s",
			    ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string + len,
			    ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string);
		vim_free(ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string);
		ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string = ਆਰਜ਼ੀ;
	    }
	}
	vim_free(buf);
    }

    sptr = ff_create_ਢੇਰ_element(ff_expand_ਬਫਰ.string,
			ff_expand_ਬਫਰ.ਲੰਮਾਈ,
			ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string,
			ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ,
			level,
			0);

    ਜੇ (sptr == ਨਿਰਾ)
	ਜਾਓ error_return;

    ff_ਧੱਕੋ(ਖੋਜ_ctx, sptr);

    ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.ਲੰਮਾਈ = filenamelen;
    ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.string = vim_strnsave(filename,
	ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.ਲੰਮਾਈ);
    ਜੇ (ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.string == ਨਿਰਾ)
	ਜਾਓ error_return;

    ਵਾਪਸ ਖੋਜ_ctx;

error_return:
    /*
     * We clear the search context now!
     * Even when the caller gave us a (perhaps valid) context we free it here,
     * as we might have already destroyed it.
     */
    vim_findfile_cleanup(ਖੋਜ_ctx);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the stopdir string.  Check that ';' is not escaped.
 */
    char_u *
vim_findfile_stopdir(char_u *buf)
{
    char_u	*r_ptr = buf;
    char_u	*r_ptr_ਸਿਰਾ = ਨਿਰਾ;	    // points to NUL at end of string "r_ptr"

    ਜਦੋਂ (*r_ptr != NUL && *r_ptr != ';')
    {
	ਜੇ (r_ptr[0] == '\\' && r_ptr[1] == ';')
	{
	    // Overwrite the escape char,
	    // use STRLEN(r_ptr) to move the trailing '\0'.
	    ਜੇ (r_ptr_ਸਿਰਾ == ਨਿਰਾ)
		r_ptr_ਸਿਰਾ = r_ptr + STRLEN(r_ptr);
	    mch_memmove(r_ptr, r_ptr + 1,
		(size_t)(r_ptr_ਸਿਰਾ - (r_ptr + 1)) + 1);	// +1 for NUL
	    r_ptr++;
	    --r_ptr_ਸਿਰਾ;
	}
	r_ptr++;
    }
    ਜੇ (*r_ptr == ';')
    {
	*r_ptr = NUL;
	r_ptr++;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*r_ptr == NUL)
	r_ptr = ਨਿਰਾ;
    ਵਾਪਸ r_ptr;
}

/*
 * Clean up the given search context. Can handle a NULL pointer.
 */
    ਖਾਲੀ
vim_findfile_cleanup(ਖਾਲੀ *ctx)
{
    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_findfile_free_ਦੇਖਿਆ(ctx);
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
vim_findfile(ਖਾਲੀ *ਖੋਜ_ctx_arg)
{
    string_T	file_ਰਸਤਾ;
    string_T	rest_ਦਾ_wildcards;
    char_u	*ਰਸਤਾ_ਸਿਰਾ = ਨਿਰਾ;
    ff_ਢੇਰ_T	*stackp;
    ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx;

    ਜੇ (ਖੋਜ_ctx_arg == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਖੋਜ_ctx = (ff_ਖੋਜ_ctx_T *)ਖੋਜ_ctx_arg;

    /*
     * filepath is used as buffer for various actions and as the storage to
     * return a found filename.
     */
    ਜੇ ((file_ਰਸਤਾ.string = alloc(MAXPATHL)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // store the end of the start dir -- needed for upward search
    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string != ਨਿਰਾ)
	ਰਸਤਾ_ਸਿਰਾ = &ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string[
					    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ];

    // upward search loop
    ਲਈ (;;)
    {
	// downward search loop
	ਲਈ (;;)
	{
	    // check if user wants to stop the search
	    ui_breakcheck();
	    ਜੇ (got_int)
		ਰੋਕੋ;

	    // get directory to work on from stack
	    stackp = ff_ਕੱਢੋ(ਖੋਜ_ctx);
	    ਜੇ (stackp == ਨਿਰਾ)
		ਰੋਕੋ;

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
	    ਜੇ (stackp->ffs_filearray == ਨਿਰਾ
		    && ff_check_ਦੇਖਿਆ(&ਖੋਜ_ctx->ffsc_dir_ਦੇਖਿਆ_list
							  ->ffvl_ਦੇਖਿਆ_list,
			    stackp->ffs_fix_ਰਸਤਾ.string,
			    stackp->ffs_fix_ਰਸਤਾ.ਲੰਮਾਈ,
			    stackp->ffs_wc_ਰਸਤਾ.string,
			    stackp->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ) == FAIL)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
		ਜੇ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("Already Searched: %s (%s)",
				   stackp->ffs_fix_ਰਸਤਾ.string, stackp->ffs_wc_ਰਸਤਾ.string);
		    // don't overwrite this either
		    ਸੁਨੇਹਾ_puts("\n");
		    verbose_leave_scroll();
		}
#ਜੇ_ਅੰਤ
		ff_free_ਢੇਰ_element(stackp);
		ਜਾਰੀ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
	    ਨਹੀਂਤਾਂ ਜੇ (p_verbose >= 5)
	    {
		verbose_enter_scroll();
		smsg("Searching: %s (%s)",
				   stackp->ffs_fix_ਰਸਤਾ.string, stackp->ffs_wc_ਰਸਤਾ.string);
		// don't overwrite this either
		ਸੁਨੇਹਾ_puts("\n");
		verbose_leave_scroll();
	    }
#ਜੇ_ਅੰਤ

	    // check depth
	    ਜੇ (stackp->ffs_level <= 0)
	    {
		ff_free_ਢੇਰ_element(stackp);
		ਜਾਰੀ;
	    }

	    file_ਰਸਤਾ.string[0] = NUL;
	    file_ਰਸਤਾ.ਲੰਮਾਈ = 0;

	    /*
	     * If no filearray till now expand wildcards
	     * The function expand_wildcards() can handle an array of paths
	     * and all possible expands are returned in one array. We use this
	     * to handle the expansion of '**' into an empty string.
	     */
	    ਜੇ (stackp->ffs_filearray == ਨਿਰਾ)
	    {
		char_u *dirptrs[2];

		// we use filepath to build the path expand_wildcards() should
		// expand.
		dirptrs[0] = file_ਰਸਤਾ.string;
		dirptrs[1] = ਨਿਰਾ;

		// if we have a start dir copy it in
		ਜੇ (!vim_isAbsName(stackp->ffs_fix_ਰਸਤਾ.string)
						&& ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string)
		{
		    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ + 1 < MAXPATHL)
		    {
			ਪੂਰਨ ਜੋੜੋ_sep = !after_pathsep(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
			    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string + ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
			file_ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf(
			    (ਅੱਖਰ *)file_ਰਸਤਾ.string,
			    MAXPATHL,
			    "%s%s",
			    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
			    ਜੋੜੋ_sep ? PATHSEPSTR : "");
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ff_free_ਢੇਰ_element(stackp);
			ਜਾਓ fail;
		    }
		}

		// append the fix part of the search path
		ਜੇ (file_ਰਸਤਾ.ਲੰਮਾਈ + stackp->ffs_fix_ਰਸਤਾ.ਲੰਮਾਈ + 1 < MAXPATHL)
		{
		    ਪੂਰਨ ਜੋੜੋ_sep = !after_pathsep(stackp->ffs_fix_ਰਸਤਾ.string,
			stackp->ffs_fix_ਰਸਤਾ.string + stackp->ffs_fix_ਰਸਤਾ.ਲੰਮਾਈ);
		    file_ਰਸਤਾ.ਲੰਮਾਈ += vim_snprintf(
			(ਅੱਖਰ *)file_ਰਸਤਾ.string + file_ਰਸਤਾ.ਲੰਮਾਈ,
			MAXPATHL - file_ਰਸਤਾ.ਲੰਮਾਈ,
			"%s%s",
			stackp->ffs_fix_ਰਸਤਾ.string,
			ਜੋੜੋ_sep ? PATHSEPSTR : "");
		}
		ਨਹੀਂਤਾਂ
		{
		    ff_free_ਢੇਰ_element(stackp);
		    ਜਾਓ fail;
		}

		rest_ਦਾ_wildcards.string = stackp->ffs_wc_ਰਸਤਾ.string;
		rest_ਦਾ_wildcards.ਲੰਮਾਈ = stackp->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ;
		ਜੇ (*rest_ਦਾ_wildcards.string != NUL)
		{
		    ਜੇ (STRNCMP(rest_ਦਾ_wildcards.string, "**", 2) == 0)
		    {
			char_u	*p;

			// pointer to the restrict byte
			// The restrict byte is not a character!
			p = rest_ਦਾ_wildcards.string + 2;

			ਜੇ (*p > 0)
			{
			    (*p)--;
			    ਜੇ (file_ਰਸਤਾ.ਲੰਮਾਈ + 1 < MAXPATHL)
				file_ਰਸਤਾ.string[file_ਰਸਤਾ.ਲੰਮਾਈ++] = '*';
			    ਨਹੀਂਤਾਂ
			    {
				ff_free_ਢੇਰ_element(stackp);
				ਜਾਓ fail;
			    }
			}

			ਜੇ (*p == 0)
			{
			    // remove '**<numb> from wildcards
			    mch_memmove(rest_ਦਾ_wildcards.string,
					rest_ਦਾ_wildcards.string + 3,
					(size_t)(rest_ਦਾ_wildcards.ਲੰਮਾਈ - 3) + 1);    // +1 for NUL
			    rest_ਦਾ_wildcards.ਲੰਮਾਈ -= 3;
			    stackp->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ = rest_ਦਾ_wildcards.ਲੰਮਾਈ;
			}
			ਨਹੀਂਤਾਂ
			{
			    rest_ਦਾ_wildcards.string += 3;
			    rest_ਦਾ_wildcards.ਲੰਮਾਈ -= 3;
			}

			ਜੇ (stackp->ffs_ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ == 0)
			{
			    // if not done before, expand '**' to empty
			    stackp->ffs_ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ = 1;
			    dirptrs[1] = stackp->ffs_fix_ਰਸਤਾ.string;
			}
		    }

		    /*
		     * Here we copy until the next path separator or the end of
		     * the path. If we stop at a path separator, there is
		     * still something else left. This is handled below by
		     * pushing every directory returned from expand_wildcards()
		     * on the stack again for further search.
		     */
		    ਜਦੋਂ (*rest_ਦਾ_wildcards.string
			    && !vim_ispathsep(*rest_ਦਾ_wildcards.string))
		    {
			ਜੇ (file_ਰਸਤਾ.ਲੰਮਾਈ + 1 < MAXPATHL)
			{
			    file_ਰਸਤਾ.string[file_ਰਸਤਾ.ਲੰਮਾਈ++] = *rest_ਦਾ_wildcards.string++;
			    --rest_ਦਾ_wildcards.ਲੰਮਾਈ;
			}
			ਨਹੀਂਤਾਂ
			{
			    ff_free_ਢੇਰ_element(stackp);
			    ਜਾਓ fail;
			}
		    }

		    file_ਰਸਤਾ.string[file_ਰਸਤਾ.ਲੰਮਾਈ] = NUL;
		    ਜੇ (vim_ispathsep(*rest_ਦਾ_wildcards.string))
		    {
			rest_ਦਾ_wildcards.string++;
			rest_ਦਾ_wildcards.ਲੰਮਾਈ--;
		    }
		}

		/*
		 * Expand wildcards like "*" and "$VAR".
		 * If the path is a URL don't try this.
		 */
		ਜੇ (ਰਸਤਾ_with_url(dirptrs[0]))
		{
		    stackp->ffs_filearray = ALLOC_ONE(char_u *);
		    ਜੇ (stackp->ffs_filearray != ਨਿਰਾ
			    && (stackp->ffs_filearray[0]
				= vim_strnsave(dirptrs[0], file_ਰਸਤਾ.ਲੰਮਾਈ)) != ਨਿਰਾ)
			stackp->ffs_filearray_ਮਾਪ = 1;
		    ਨਹੀਂਤਾਂ
			stackp->ffs_filearray_ਮਾਪ = 0;
		}
		ਨਹੀਂਤਾਂ
		    // Add EW_NOTWILD because the expanded path may contain
		    // wildcard characters that are to be taken literally.
		    // This is a bit of a hack.
		    expand_wildcards((dirptrs[1] == ਨਿਰਾ) ? 1 : 2, dirptrs,
			    &stackp->ffs_filearray_ਮਾਪ,
			    &stackp->ffs_filearray,
			    EW_DIR|EW_ADDSLASH|EW_SILENT|EW_NOTWILD);

		stackp->ffs_filearray_cur = 0;
		stackp->ffs_stage = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		rest_ਦਾ_wildcards.string = &stackp->ffs_wc_ਰਸਤਾ.string[
							stackp->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ];
		rest_ਦਾ_wildcards.ਲੰਮਾਈ = 0;
	    }

	    ਜੇ (stackp->ffs_stage == 0)
	    {
		ਪੂਰਨ i;

		// this is the first time we work on this directory
		ਜੇ (*rest_ਦਾ_wildcards.string == NUL)
		{
		    size_t  len;
		    char_u  *suf;

		    /*
		     * We don't have further wildcards to expand, so we have to
		     * check for the final file now.
		     */
		    ਲਈ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ਮਾਪ; ++i)
		    {
			ਜੇ (!ਰਸਤਾ_with_url(stackp->ffs_filearray[i])
				      && !mch_isdir(stackp->ffs_filearray[i]))
			    ਜਾਰੀ;   // not a directory

			// prepare the filename to be checked for existence
			//ਹੇਠਾਂ
			len = STRLEN(stackp->ffs_filearray[i]);
			ਜੇ (len + 1 + ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.ਲੰਮਾਈ
									< MAXPATHL)
			{
			    ਪੂਰਨ	ਜੋੜੋ_sep = !after_pathsep(stackp->ffs_filearray[i],
				    stackp->ffs_filearray[i] + len);
			    file_ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf(
				    (ਅੱਖਰ *)file_ਰਸਤਾ.string,
				    MAXPATHL,
				    "%s%s%s",
				    stackp->ffs_filearray[i],
				    ਜੋੜੋ_sep ? PATHSEPSTR : "",
				    ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ.string);
			}
			ਨਹੀਂਤਾਂ
			{
			    ff_free_ਢੇਰ_element(stackp);
			    ਜਾਓ fail;
			}

			/*
			 * Try without extra suffix and then with suffixes
			 * from 'suffixesadd'.
			 */
			len = file_ਰਸਤਾ.ਲੰਮਾਈ;
			ਜੇ (ਖੋਜ_ctx->ffsc_tagfile)
			    suf = (char_u *)"";
			ਨਹੀਂਤਾਂ
			    suf = curbuf->b_p_sua;
			ਲਈ (;;)
			{
			    // if file exists and we didn't already find it
			    ਜੇ ((ਰਸਤਾ_with_url(file_ਰਸਤਾ.string)
				  || (mch_getperm(file_ਰਸਤਾ.string) >= 0
				      && (ਖੋਜ_ctx->ffsc_ਲੱਭੋ_what
							      == FINDFILE_BOTH
					  || ((ਖੋਜ_ctx->ffsc_ਲੱਭੋ_what
							      == FINDFILE_DIR)
						   == mch_isdir(file_ਰਸਤਾ.string)))))
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
				    && (ff_check_ਦੇਖਿਆ(
					    &ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_list
							   ->ffvl_ਦੇਖਿਆ_list,
					    file_ਰਸਤਾ.string,
					    file_ਰਸਤਾ.ਲੰਮਾਈ,
					    (char_u *)"", 0) == OK)
#ਜੇ_ਅੰਤ
			       )
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
				ਜੇ (ff_check_ਦੇਖਿਆ(
					    &ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_list
							   ->ffvl_ਦੇਖਿਆ_list,
					      file_ਰਸਤਾ.string,
					      file_ਰਸਤਾ.ਲੰਮਾਈ,
					      (char_u *)"", 0) == FAIL)
				{
				    ਜੇ (p_verbose >= 5)
				    {
					verbose_enter_scroll();
					smsg("Already: %s", file_ਰਸਤਾ.string);
					// don't overwrite this either
					ਸੁਨੇਹਾ_puts("\n");
					verbose_leave_scroll();
				    }
				    ਜਾਰੀ;
				}
#ਜੇ_ਅੰਤ

				// push dir to examine rest of subdirs later
				stackp->ffs_filearray_cur = i + 1;
				ff_ਧੱਕੋ(ਖੋਜ_ctx, stackp);

				ਜੇ (!ਰਸਤਾ_with_url(file_ਰਸਤਾ.string))
				    file_ਰਸਤਾ.ਲੰਮਾਈ = simplify_filename(file_ਰਸਤਾ.string);

				ਜੇ (mch_dirname(ff_expand_ਬਫਰ.string, MAXPATHL)
									== OK)
				{
				    char_u  *p;

				    ff_expand_ਬਫਰ.ਲੰਮਾਈ = STRLEN(ff_expand_ਬਫਰ.string);
				    p = shorten_fname(file_ਰਸਤਾ.string,
							    ff_expand_ਬਫਰ.string);
				    ਜੇ (p != ਨਿਰਾ)
				    {
					mch_memmove(file_ਰਸਤਾ.string, p,
					    (size_t)((file_ਰਸਤਾ.string + file_ਰਸਤਾ.ਲੰਮਾਈ) - p) + 1);  // +1 for NUL
					file_ਰਸਤਾ.ਲੰਮਾਈ -= (p - file_ਰਸਤਾ.string);
				    }
				}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
				ਜੇ (p_verbose >= 5)
				{
				    verbose_enter_scroll();
				    smsg("HIT: %s", file_ਰਸਤਾ.string);
				    // don't overwrite this either
				    ਸੁਨੇਹਾ_puts("\n");
				    verbose_leave_scroll();
				}
#ਜੇ_ਅੰਤ
				ਵਾਪਸ file_ਰਸਤਾ.string;
			    }

			    // Not found or found already, try next suffix.
			    ਜੇ (*suf == NUL)
				ਰੋਕੋ;
			    file_ਰਸਤਾ.ਲੰਮਾਈ = len + copy_option_part(&suf,
				  file_ਰਸਤਾ.string + len,
				  (ਪੂਰਨ)(MAXPATHL - len), ",");
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    /*
		     * still wildcards left, push the directories for further
		     * search
		     */
		    ਲਈ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ਮਾਪ; ++i)
		    {
			ਜੇ (!mch_isdir(stackp->ffs_filearray[i]))
			    ਜਾਰੀ;	// not a directory

			ff_ਧੱਕੋ(ਖੋਜ_ctx,
				ff_create_ਢੇਰ_element(
						     stackp->ffs_filearray[i],
						     STRLEN(stackp->ffs_filearray[i]),
						     rest_ਦਾ_wildcards.string,
						     rest_ਦਾ_wildcards.ਲੰਮਾਈ,
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
	    ਜੇ (STRNCMP(stackp->ffs_wc_ਰਸਤਾ.string, "**", 2) == 0)
	    {
		ਪੂਰਨ i;

		ਲਈ (i = stackp->ffs_filearray_cur;
					  i < stackp->ffs_filearray_ਮਾਪ; ++i)
		{
		    ਜੇ (fnamecmp(stackp->ffs_filearray[i],
						   stackp->ffs_fix_ਰਸਤਾ.string) == 0)
			ਜਾਰੀ; // don't repush same directory
		    ਜੇ (!mch_isdir(stackp->ffs_filearray[i]))
			ਜਾਰੀ;   // not a directory
		    ff_ਧੱਕੋ(ਖੋਜ_ctx,
			    ff_create_ਢੇਰ_element(
				stackp->ffs_filearray[i],
				STRLEN(stackp->ffs_filearray[i]),
				stackp->ffs_wc_ਰਸਤਾ.string,
				stackp->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ,
				stackp->ffs_level - 1, 1));
		}
	    }

	    // we are done with the current directory
	    ff_free_ਢੇਰ_element(stackp);
	}

	// If we reached this, we didn't find anything downwards.
	// Let's check if we should do an upward search.
	ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string
		&& ਖੋਜ_ctx->ffsc_stopdirs_v != ਨਿਰਾ && !got_int)
	{
	    ff_ਢੇਰ_T  *sptr;
	    // path_end may point to the NUL or the previous path separator
	    ਪੂਰਨ plen = (ਰਸਤਾ_ਸਿਰਾ - ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string)
							  + (*ਰਸਤਾ_ਸਿਰਾ != NUL);

	    // is the last starting directory in the stop list?
	    ਜੇ (ff_ਰਸਤਾ_ਵਿੱਚ_stoplist(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
				    plen, ਖੋਜ_ctx->ffsc_stopdirs_v) == TRUE)
		ਰੋਕੋ;

	    // cut of last dir
	    ਜਦੋਂ (ਰਸਤਾ_ਸਿਰਾ > ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string
						  && vim_ispathsep(*ਰਸਤਾ_ਸਿਰਾ))
		ਰਸਤਾ_ਸਿਰਾ--;
	    ਜਦੋਂ (ਰਸਤਾ_ਸਿਰਾ > ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string
					      && !vim_ispathsep(ਰਸਤਾ_ਸਿਰਾ[-1]))
		ਰਸਤਾ_ਸਿਰਾ--;
	    *ਰਸਤਾ_ਸਿਰਾ = NUL;

	    // we may have shortened search_ctx->ffsc_start_dir, so update it's length
	    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ = (size_t)(ਰਸਤਾ_ਸਿਰਾ - ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string);
	    ਰਸਤਾ_ਸਿਰਾ--;

	    ਜੇ (*ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string == NUL)
		ਰੋਕੋ;

	    ਜੇ (ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ + 1
		    + ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.ਲੰਮਾਈ < MAXPATHL)
	    {
		ਪੂਰਨ ਜੋੜੋ_sep = !after_pathsep(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
			    ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string + ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.ਲੰਮਾਈ);
		file_ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf(
			(ਅੱਖਰ *)file_ਰਸਤਾ.string,
			MAXPATHL,
			"%s%s%s",
			ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir.string,
			ਜੋੜੋ_sep ? PATHSEPSTR : "",
			ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ.string);
	    }
	    ਨਹੀਂਤਾਂ
		ਜਾਓ fail;

	    // create a new stack entry
	    sptr = ff_create_ਢੇਰ_element(file_ਰਸਤਾ.string, file_ਰਸਤਾ.ਲੰਮਾਈ,
		    ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.string, ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ.ਲੰਮਾਈ,
		    ਖੋਜ_ctx->ffsc_level, 0);
	    ਜੇ (sptr == ਨਿਰਾ)
		ਰੋਕੋ;
	    ff_ਧੱਕੋ(ਖੋਜ_ctx, sptr);
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

fail:
    vim_free(file_ਰਸਤਾ.string);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Free the list of lists of visited files and directories
 * Can handle it if the passed search_context is NULL;
 */
    ਸਥਿਰ ਖਾਲੀ
vim_findfile_free_ਦੇਖਿਆ(ਖਾਲੀ *ਖੋਜ_ctx_arg)
{
    ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx;

    ਜੇ (ਖੋਜ_ctx_arg == ਨਿਰਾ)
	ਵਾਪਸ;

    ਖੋਜ_ctx = (ff_ਖੋਜ_ctx_T *)ਖੋਜ_ctx_arg;
    vim_findfile_free_ਦੇਖਿਆ_list(&ਖੋਜ_ctx->ffsc_ਦੇਖਿਆ_lists_list);
    vim_findfile_free_ਦੇਖਿਆ_list(&ਖੋਜ_ctx->ffsc_dir_ਦੇਖਿਆ_lists_list);
}

    ਸਥਿਰ ਖਾਲੀ
vim_findfile_free_ਦੇਖਿਆ_list(ff_ਦੇਖਿਆ_list_hdr_T **list_headp)
{
    ff_ਦੇਖਿਆ_list_hdr_T *vp;

    ਜਦੋਂ (*list_headp != ਨਿਰਾ)
    {
	vp = (*list_headp)->ffvl_ਅਗਲਾ;
	ff_free_ਦੇਖਿਆ_list((*list_headp)->ffvl_ਦੇਖਿਆ_list);

	vim_free((*list_headp)->ffvl_filename);
	vim_free(*list_headp);
	*list_headp = vp;
    }
    *list_headp = ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
ff_free_ਦੇਖਿਆ_list(ff_ਦੇਖਿਆ_T *vl)
{
    ff_ਦੇਖਿਆ_T *vp;

    ਜਦੋਂ (vl != ਨਿਰਾ)
    {
	vp = vl->ffv_ਅਗਲਾ;
	vim_free(vl->ffv_wc_ਰਸਤਾ);
	vim_free(vl);
	vl = vp;
    }
    vl = ਨਿਰਾ;
}

/*
 * Returns the already visited list for the given filename. If none is found it
 * allocates a new one.
 */
    ਸਥਿਰ ff_ਦੇਖਿਆ_list_hdr_T*
ff_get_ਦੇਖਿਆ_list(
    char_u			*filename,
    size_t			filenamelen,
    ff_ਦੇਖਿਆ_list_hdr_T	**list_headp)
{
    ff_ਦੇਖਿਆ_list_hdr_T  *retptr = ਨਿਰਾ;

    // check if a visited list for the given filename exists
    ਜੇ (*list_headp != ਨਿਰਾ)
    {
	retptr = *list_headp;
	ਜਦੋਂ (retptr != ਨਿਰਾ)
	{
	    ਜੇ (fnamecmp(filename, retptr->ffvl_filename) == 0)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
		ਜੇ (p_verbose >= 5)
		{
		    verbose_enter_scroll();
		    smsg("ff_get_visited_list: FOUND list for %s",
								    filename);
		    // don't overwrite this either
		    ਸੁਨੇਹਾ_puts("\n");
		    verbose_leave_scroll();
		}
#ਜੇ_ਅੰਤ
		ਵਾਪਸ retptr;
	    }
	    retptr = retptr->ffvl_ਅਗਲਾ;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FF_VERBOSE
    ਜੇ (p_verbose >= 5)
    {
	verbose_enter_scroll();
	smsg("ff_get_visited_list: new list for %s", filename);
	// don't overwrite this either
	ਸੁਨੇਹਾ_puts("\n");
	verbose_leave_scroll();
    }
#ਜੇ_ਅੰਤ

    /*
     * if we reach this we didn't find a list and we have to allocate new list
     */
    retptr = ALLOC_ONE(ff_ਦੇਖਿਆ_list_hdr_T);
    ਜੇ (retptr == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    retptr->ffvl_ਦੇਖਿਆ_list = ਨਿਰਾ;
    retptr->ffvl_filename = vim_strnsave(filename, filenamelen);
    ਜੇ (retptr->ffvl_filename == ਨਿਰਾ)
    {
	vim_free(retptr);
	ਵਾਪਸ ਨਿਰਾ;
    }
    retptr->ffvl_ਅਗਲਾ = *list_headp;
    *list_headp = retptr;

    ਵਾਪਸ retptr;
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
    ਸਥਿਰ ਪੂਰਨ
ff_wc_equal(char_u *s1, char_u *s2)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		c1 = NUL;
    ਪੂਰਨ		c2 = NUL;
    ਪੂਰਨ		prev1 = NUL;
    ਪੂਰਨ		prev2 = NUL;

    ਜੇ (s1 == s2)
	ਵਾਪਸ TRUE;

    ਜੇ (s1 == ਨਿਰਾ || s2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0, j = 0; s1[i] != NUL && s2[j] != NUL;)
    {
	c1 = PTR2CHAR(s1 + i);
	c2 = PTR2CHAR(s2 + j);

	ਜੇ ((p_fic ? MB_TOLOWER(c1) != MB_TOLOWER(c2) : c1 != c2)
		&& (prev1 != '*' || prev2 != '*'))
	    ਵਾਪਸ FALSE;
	prev2 = prev1;
	prev1 = c1;

	i += mb_ptr2len(s1 + i);
	j += mb_ptr2len(s2 + j);
    }
    ਵਾਪਸ s1[i] == s2[j];
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
    ਸਥਿਰ ਪੂਰਨ
ff_check_ਦੇਖਿਆ(
    ff_ਦੇਖਿਆ_T	**ਦੇਖਿਆ_list,
    char_u		*fname,
    size_t		fnamelen,
    char_u		*wc_ਰਸਤਾ,
    size_t		wc_pathlen)
{
    ff_ਦੇਖਿਆ_T	*vp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    stat_T		st;
    ਪੂਰਨ			url = FALSE;
#ਜੇ_ਅੰਤ

    // For a URL we only compare the name, otherwise we compare the
    // device/inode (unix) or the full path name (not Unix).
    ਜੇ (ਰਸਤਾ_with_url(fname))
    {
	vim_strncpy(ff_expand_ਬਫਰ.string, fname, fnamelen);
	ff_expand_ਬਫਰ.ਲੰਮਾਈ = fnamelen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	url = TRUE;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ff_expand_ਬਫਰ.string[0] = NUL;
	ff_expand_ਬਫਰ.ਲੰਮਾਈ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) < 0)
	    ਵਾਪਸ FAIL;
#ਨਹੀਂਤਾਂ
	ਜੇ (vim_ਭਰਿਆ_Name(fname, ff_expand_ਬਫਰ.string, MAXPATHL, TRUE) == FAIL)
	    ਵਾਪਸ FAIL;
	ff_expand_ਬਫਰ.ਲੰਮਾਈ = STRLEN(ff_expand_ਬਫਰ.string);
#ਜੇ_ਅੰਤ
    }

    // check against list of already visited files
    ਲਈ (vp = *ਦੇਖਿਆ_list; vp != ਨਿਰਾ; vp = vp->ffv_ਅਗਲਾ)
    {
	ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		!url ? (vp->ffv_dev_valid && vp->ffv_dev == st.st_dev
						  && vp->ffv_ino == st.st_ino)
		     :
#ਜੇ_ਅੰਤ
		fnamecmp(vp->ffv_fname, ff_expand_ਬਫਰ.string) == 0
	   )
	{
	    // are the wildcard parts equal
	    ਜੇ (ff_wc_equal(vp->ffv_wc_ਰਸਤਾ, wc_ਰਸਤਾ) == TRUE)
		// already visited
		ਵਾਪਸ FAIL;
	}
    }

    /*
     * New file/dir.  Add it to the list of visited files/dirs.
     */
    vp = alloc(
	     offsetof(ff_ਦੇਖਿਆ_T, ffv_fname) + ff_expand_ਬਫਰ.ਲੰਮਾਈ + 1);
    ਜੇ (vp == ਨਿਰਾ)
	ਵਾਪਸ OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਜੇ (!url)
    {
	vp->ffv_dev_valid = TRUE;
	vp->ffv_dev = st.st_dev;
	vp->ffv_ino = st.st_ino;
	vp->ffv_fname[0] = NUL;
    }
    ਨਹੀਂਤਾਂ
    {
	vp->ffv_dev_valid = FALSE;
#ਜੇ_ਅੰਤ
	STRCPY(vp->ffv_fname, ff_expand_ਬਫਰ.string);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    }
#ਜੇ_ਅੰਤ
    ਜੇ (wc_ਰਸਤਾ != ਨਿਰਾ)
	vp->ffv_wc_ਰਸਤਾ = vim_strnsave(wc_ਰਸਤਾ, wc_pathlen);
    ਨਹੀਂਤਾਂ
	vp->ffv_wc_ਰਸਤਾ = ਨਿਰਾ;

    vp->ffv_ਅਗਲਾ = *ਦੇਖਿਆ_list;
    *ਦੇਖਿਆ_list = vp;

    ਵਾਪਸ OK;
}

/*
 * create stack element from given path pieces
 */
    ਸਥਿਰ ff_ਢੇਰ_T *
ff_create_ਢੇਰ_element(
    char_u	*fix_part,
    size_t	fix_partlen,
    char_u	*wc_part,
    size_t	wc_partlen,
    ਪੂਰਨ		level,
    ਪੂਰਨ		ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ)
{
    ff_ਢੇਰ_T	*new;

    new = ALLOC_ONE(ff_ਢੇਰ_T);
    ਜੇ (new == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    new->ffs_prev	   = ਨਿਰਾ;
    new->ffs_filearray	   = ਨਿਰਾ;
    new->ffs_filearray_ਮਾਪ = 0;
    new->ffs_filearray_cur  = 0;
    new->ffs_stage	   = 0;
    new->ffs_level	   = level;
    new->ffs_ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ = ਤਾਰਾ_ਤਾਰਾ_ਖਾਲੀ_ਥਾਵਾਂ;

    // the following saves NULL pointer checks in vim_findfile
    ਜੇ (fix_part == ਨਿਰਾ)
    {
	fix_part = (char_u *)"";
	fix_partlen = 0;
    }
    new->ffs_fix_ਰਸਤਾ.string = vim_strnsave(fix_part, fix_partlen);
    new->ffs_fix_ਰਸਤਾ.ਲੰਮਾਈ = fix_partlen;

    ਜੇ (wc_part == ਨਿਰਾ)
    {
	wc_part = (char_u *)"";
	wc_partlen = 0;
    }
    new->ffs_wc_ਰਸਤਾ.string = vim_strnsave(wc_part, wc_partlen);
    new->ffs_wc_ਰਸਤਾ.ਲੰਮਾਈ = wc_partlen;

    ਜੇ (new->ffs_fix_ਰਸਤਾ.string == ਨਿਰਾ || new->ffs_wc_ਰਸਤਾ.string == ਨਿਰਾ)
    {
	ff_free_ਢੇਰ_element(new);
	new = ਨਿਰਾ;
    }

    ਵਾਪਸ new;
}

/*
 * Push a dir on the directory stack.
 */
    ਸਥਿਰ ਖਾਲੀ
ff_ਧੱਕੋ(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx, ff_ਢੇਰ_T *ਢੇਰ_ptr)
{
    // check for NULL pointer, not to return an error to the user, but
    // to prevent a crash
    ਜੇ (ਢੇਰ_ptr == ਨਿਰਾ)
	ਵਾਪਸ;

    ਢੇਰ_ptr->ffs_prev = ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr;
    ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr = ਢੇਰ_ptr;
}

/*
 * Pop a dir from the directory stack.
 * Returns NULL if stack is empty.
 */
    ਸਥਿਰ ff_ਢੇਰ_T *
ff_ਕੱਢੋ(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx)
{
    ff_ਢੇਰ_T  *sptr;

    sptr = ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr;
    ਜੇ (ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr != ਨਿਰਾ)
	ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr = ਖੋਜ_ctx->ffsc_ਢੇਰ_ptr->ffs_prev;

    ਵਾਪਸ sptr;
}

/*
 * free the given stack element
 */
    ਸਥਿਰ ਖਾਲੀ
ff_free_ਢੇਰ_element(ff_ਢੇਰ_T *ਢੇਰ_ptr)
{
    // VIM_CLEAR_STRING handles possible NULL pointers
    VIM_CLEAR_STRING(ਢੇਰ_ptr->ffs_fix_ਰਸਤਾ);
    VIM_CLEAR_STRING(ਢੇਰ_ptr->ffs_wc_ਰਸਤਾ);

    ਜੇ (ਢੇਰ_ptr->ffs_filearray != ਨਿਰਾ)
	FreeWild(ਢੇਰ_ptr->ffs_filearray_ਮਾਪ, ਢੇਰ_ptr->ffs_filearray);

    vim_free(ਢੇਰ_ptr);
}

/*
 * Clear the search context, but NOT the visited list.
 */
    ਸਥਿਰ ਖਾਲੀ
ff_clear(ff_ਖੋਜ_ctx_T *ਖੋਜ_ctx)
{
    ff_ਢੇਰ_T   *sptr;

    // clear up stack
    ਜਦੋਂ ((sptr = ff_ਕੱਢੋ(ਖੋਜ_ctx)) != ਨਿਰਾ)
	ff_free_ਢੇਰ_element(sptr);

    ਜੇ (ਖੋਜ_ctx->ffsc_stopdirs_v != ਨਿਰਾ)
    {
	ਪੂਰਨ  i = 0;

	ਜਦੋਂ (ਖੋਜ_ctx->ffsc_stopdirs_v[i].string != ਨਿਰਾ)
	{
	    vim_free(ਖੋਜ_ctx->ffsc_stopdirs_v[i].string);
	    i++;
	}
	VIM_CLEAR(ਖੋਜ_ctx->ffsc_stopdirs_v);
    }

    // reset everything
    VIM_CLEAR_STRING(ਖੋਜ_ctx->ffsc_file_ਮੰਜ਼ਿਲ_ਖੋਜ);
    VIM_CLEAR_STRING(ਖੋਜ_ctx->ffsc_ਸ਼ੁਰੂ_dir);
    VIM_CLEAR_STRING(ਖੋਜ_ctx->ffsc_fix_ਰਸਤਾ);
    VIM_CLEAR_STRING(ਖੋਜ_ctx->ffsc_wc_ਰਸਤਾ);
    ਖੋਜ_ctx->ffsc_level = 0;
}

/*
 * check if the given path is in the stopdirs
 * returns TRUE if yes else FALSE
 */
    ਸਥਿਰ ਪੂਰਨ
ff_ਰਸਤਾ_ਵਿੱਚ_stoplist(char_u *ਰਸਤਾ, ਪੂਰਨ ਰਸਤਾ_len, string_T *stopdirs_v)
{
    ਪੂਰਨ		i = 0;

    // eat up trailing path separators, except the first
    ਜਦੋਂ (ਰਸਤਾ_len > 1 && vim_ispathsep(ਰਸਤਾ[ਰਸਤਾ_len - 1]))
	ਰਸਤਾ_len--;

    // if no path consider it as match
    ਜੇ (ਰਸਤਾ_len == 0)
	ਵਾਪਸ TRUE;

    ਲਈ (i = 0; stopdirs_v[i].string != ਨਿਰਾ; i++)
	// match for parent directory. So '/home' also matches
	// '/home/rks'. Check for PATHSEP in stopdirs_v[i], else
	// '/home/r' would also match '/home/rks'
	ਜੇ (fnamencmp(stopdirs_v[i].string, ਰਸਤਾ, ਰਸਤਾ_len) == 0
		&& ((ਪੂਰਨ)stopdirs_v[i].ਲੰਮਾਈ <= ਰਸਤਾ_len
		    || vim_ispathsep(stopdirs_v[i].string[ਰਸਤਾ_len])))
	    ਵਾਪਸ TRUE;

    ਵਾਪਸ FALSE;
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
ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(
    char_u	*ptr,		// file name
    ਪੂਰਨ		len,		// length of file name
    ਪੂਰਨ		options,
    ਪੂਰਨ		ਪਹਿਲਾ,		// use count'th matching file name
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_ਮੰਜ਼ਿਲ_ਲੱਭੋ,	// in/out: modified copy of file name
    ਅੱਖਰ	**ਖੋਜ_ctx)	// in/out: state of the search
{
    ਵਾਪਸ ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ_option(ptr, len, options, ਪਹਿਲਾ,
	    *curbuf->b_p_ਰਸਤਾ == NUL ? p_ਰਸਤਾ : curbuf->b_p_ਰਸਤਾ,
	    FINDFILE_BOTH, rel_fname, curbuf->b_p_sua,
	    file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, ਖੋਜ_ctx);
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_findfile(ਖਾਲੀ)
{
    VIM_CLEAR_STRING(ff_expand_ਬਫਰ);
}
#ਜੇ_ਅੰਤ

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
ਲੱਭੋ_directory_ਵਿੱਚ_ਰਸਤਾ(
    char_u	*ptr,		// file name
    ਪੂਰਨ		len,		// length of file name
    ਪੂਰਨ		options,
    char_u	*rel_fname,	// file name searching relative to
    char_u	**file_ਮੰਜ਼ਿਲ_ਲੱਭੋ,	// in/out: modified copy of file name
    ਅੱਖਰ	**ਖੋਜ_ctx)	// in/out: state of the search
{
    ਵਾਪਸ ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ_option(ptr, len, options, TRUE, p_cdpath,
				       FINDFILE_DIR, rel_fname, (char_u *)"",
				       file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, ਖੋਜ_ctx);
}

    char_u *
ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ_option(
    char_u	*ptr,		// file name
    ਪੂਰਨ		len,		// length of file name
    ਪੂਰਨ		options,
    ਪੂਰਨ		ਪਹਿਲਾ,		// use count'th matching file name
    char_u	*ਰਸਤਾ_option,	// p_path or p_cdpath
    ਪੂਰਨ		ਲੱਭੋ_what,	// FINDFILE_FILE, _DIR or _BOTH
    char_u	*rel_fname,	// file name we are looking relative to.
    char_u	*suffixes,	// list of suffixes, 'suffixesadd' option
    char_u	**file_ਮੰਜ਼ਿਲ_ਲੱਭੋ,	// in/out: modified copy of file name
    ਅੱਖਰ	**ਖੋਜ_ctx_arg) // in/out: state of the search
{
    ff_ਖੋਜ_ctx_T	**ਖੋਜ_ctx = (ff_ਖੋਜ_ctx_T **)ਖੋਜ_ctx_arg;
    ਸਥਿਰ char_u	*dir;
    ਸਥਿਰ ਪੂਰਨ		did_findfile_init = FALSE;
    char_u		*file_name = ਨਿਰਾ;
    ਪੂਰਨ			rel_ਮੰਜ਼ਿਲ_curdir;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    ਬਣਤਰ Process	*proc = (ਬਣਤਰ Process *)ਲੱਭੋ_Task(0L);
    APTR		save_winptr = proc->pr_ਖਿੜਕੀ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_ਖਿੜਕੀ_Ptr = (APTR)-1L;
#ਜੇ_ਅੰਤ
    ਸਥਿਰ size_t	file_ਮੰਜ਼ਿਲ_findlen = 0;

    ਜੇ (ਪਹਿਲਾ == TRUE)
    {
	char_u	save_char;

	ਜੇ (len == 0)
	    ਵਾਪਸ ਨਿਰਾ;

	// copy file name into NameBuff, expanding environment variables
	save_char = ptr[len];
	ptr[len] = NUL;
	file_ਮੰਜ਼ਿਲ_findlen = expand_env_esc(ptr, NameBuff, MAXPATHL, ਨਿਰਾ, TRUE, ਨਿਰਾ);
	ptr[len] = save_char;

	vim_free(*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = vim_strnsave(NameBuff, file_ਮੰਜ਼ਿਲ_findlen);
	ਜੇ (*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ == ਨਿਰਾ)	// out of memory
	{
	    file_name = ਨਿਰਾ;
	    file_ਮੰਜ਼ਿਲ_findlen = 0;
	    ਜਾਓ theend;
	}
	ਜੇ (options & FNAME_UNESC)
	{
	    // Change all "\ " to " ".
	    ਲਈ (ptr = *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ; *ptr != NUL; ++ptr)
		ਜੇ (ptr[0] == '\\' && ptr[1] == ' ')
		{
		    mch_memmove(ptr, ptr + 1,
			(size_t)((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ + file_ਮੰਜ਼ਿਲ_findlen) - (ptr + 1)) + 1);
		    --file_ਮੰਜ਼ਿਲ_findlen;
		}
	}
    }

    rel_ਮੰਜ਼ਿਲ_curdir = ((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[0] == '.'
		    && ((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[1] == NUL
			|| vim_ispathsep((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[1])
			|| ((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[1] == '.'
			    && ((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[2] == NUL
				|| vim_ispathsep((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[2])))));
    ਜੇ (vim_isAbsName(*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)
	    // "..", "../path", "." and "./path": don't use the path_option
	    || rel_ਮੰਜ਼ਿਲ_curdir
#ਜੇ defined(MSWIN)
	    // handle "\tmp" as absolute path
	    || vim_ispathsep((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[0])
	    // handle "c:name" as absolute path
	    || ((*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[0] != NUL && (*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[1] == ':')
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
	    // handle ":tmp" as absolute path
	    || (*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ)[0] == ':'
#ਜੇ_ਅੰਤ
       )
    {
	/*
	 * Absolute path, no need to use "path_option".
	 * If this is not a first call, return NULL.  We already returned a
	 * filename on the first call.
	 */
	ਜੇ (ਪਹਿਲਾ == TRUE)
	{
	    ਪੂਰਨ		l;
	    ਪੂਰਨ		ਲੜੀਵਾਰ;
	    size_t	rel_fnamelen = 0;
	    char_u	*suffix;

	    ਜੇ (ਰਸਤਾ_with_url(*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ))
	    {
		file_name = vim_strnsave(*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, file_ਮੰਜ਼ਿਲ_findlen);
		ਜਾਓ theend;
	    }

	    ਜੇ (rel_fname != ਨਿਰਾ)
		rel_fnamelen = STRLEN(rel_fname);

	    // When FNAME_REL flag given first use the directory of the file.
	    // Otherwise or when this fails use the current directory.
	    ਲਈ (ਲੜੀਵਾਰ = 1; ਲੜੀਵਾਰ <= 2; ++ਲੜੀਵਾਰ)
	    {
		l = (ਪੂਰਨ)file_ਮੰਜ਼ਿਲ_findlen;
		ਜੇ (ਲੜੀਵਾਰ == 1
			&& rel_ਮੰਜ਼ਿਲ_curdir
			&& (options & FNAME_REL)
			&& rel_fname != ਨਿਰਾ
			&& rel_fnamelen + l < MAXPATHL)
		{
		    l = vim_snprintf(
			(ਅੱਖਰ *)NameBuff,
			MAXPATHL,
			"%.*s%s",
			(ਪੂਰਨ)(gettail(rel_fname) - rel_fname),
			rel_fname,
			*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
		}
		ਨਹੀਂਤਾਂ
		{
		    STRCPY(NameBuff, *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
		    ਲੜੀਵਾਰ = 2;
		}

		// When the file doesn't exist, try adding parts of
		// 'suffixesadd'.
		suffix = suffixes;
		ਲਈ (;;)
		{
		    ਜੇ (mch_getperm(NameBuff) >= 0
			     && (ਲੱਭੋ_what == FINDFILE_BOTH
				 || ((ਲੱਭੋ_what == FINDFILE_DIR)
						    == mch_isdir(NameBuff))))
		    {
			file_name = vim_strnsave(NameBuff,
						  simplify_filename(NameBuff));
			ਜਾਓ theend;
		    }
		    ਜੇ (*suffix == NUL)
			ਰੋਕੋ;
		    copy_option_part(&suffix, NameBuff + l, MAXPATHL - l, ",");
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Loop over all paths in the 'path' or 'cdpath' option.
	 * When "first" is set, first setup to the start of the option.
	 * Otherwise continue to find the next match.
	 */
	ਜੇ (ਪਹਿਲਾ == TRUE)
	{
	    // vim_findfile_free_visited can handle a possible NULL pointer
	    vim_findfile_free_ਦੇਖਿਆ(*ਖੋਜ_ctx);
	    dir = ਰਸਤਾ_option;
	    did_findfile_init = FALSE;
	}

	ਲਈ (;;)
	{
	    ਜੇ (did_findfile_init)
	    {
		file_name = vim_findfile(*ਖੋਜ_ctx);
		ਜੇ (file_name != ਨਿਰਾ)
		    ਰੋਕੋ;

		did_findfile_init = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u	*buf;
		char_u  *r_ptr;

		ਜੇ (dir == ਨਿਰਾ || *dir == NUL)
		{
		    // We searched all paths of the option, now we can
		    // free the search context.
		    vim_findfile_cleanup(*ਖੋਜ_ctx);
		    *ਖੋਜ_ctx = ਨਿਰਾ;
		    ਰੋਕੋ;
		}

		ਜੇ ((buf = alloc(MAXPATHL)) == ਨਿਰਾ)
		    ਰੋਕੋ;

		// copy next path
		buf[0] = NUL;
		copy_option_part(&dir, buf, MAXPATHL, " ,");

		// get the stopdir string
		r_ptr = vim_findfile_stopdir(buf);
		*ਖੋਜ_ctx = vim_findfile_init(buf, *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, file_ਮੰਜ਼ਿਲ_findlen,
					    r_ptr, 100, FALSE, ਲੱਭੋ_what,
					   *ਖੋਜ_ctx, FALSE, rel_fname);
		ਜੇ (*ਖੋਜ_ctx != ਨਿਰਾ)
		    did_findfile_init = TRUE;
		vim_free(buf);
	    }
	}
    }
    ਜੇ (file_name == ਨਿਰਾ && (options & FNAME_MESS))
    {
	ਜੇ (ਪਹਿਲਾ == TRUE)
	{
	    ਜੇ (ਲੱਭੋ_what == FINDFILE_DIR)
		semsg(_(e_cant_ਲੱਭੋ_directory_str_ਵਿੱਚ_cdpath), *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_cant_ਲੱਭੋ_file_str_ਵਿੱਚ_ਰਸਤਾ), *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਲੱਭੋ_what == FINDFILE_DIR)
		semsg(_(e_no_ਹੋਰ_directory_str_ਮਿਲਿਆ_ਵਿੱਚ_cdpath),
								*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_no_ਹੋਰ_file_str_ਮਿਲਿਆ_ਵਿੱਚ_ਰਸਤਾ), *file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	}
    }

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    proc->pr_ਖਿੜਕੀ_Ptr = save_winptr;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ file_name;
}

/*
 * Get the file name at the cursor.
 * If Visual mode is active, use the selected text if it's in one line.
 * Returns the name in allocated memory, NULL for failure.
 */
    char_u *
grab_file_name(ਲੰਮਾ ਗਿਣਤੀ_2, linenr_T *file_lnum)
{
    ਪੂਰਨ options = FNAME_MESS|FNAME_EXP|FNAME_REL|FNAME_UNESC;

    ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਪੂਰਨ	len;
	char_u	*ptr;

	ਜੇ (get_visual_ਪਾਠ(ਨਿਰਾ, &ptr, &len) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	// Only recognize ":123" here
	ਜੇ (file_lnum != ਨਿਰਾ && ptr[len] == ':' && ਸੁਰੱਖਿਅਤ_isdigit(ptr[len + 1]))
	{
	    char_u *p = ptr + len + 1;

	    *file_lnum = getdigits(&p);
	}
	ਵਾਪਸ ਲੱਭੋ_file_name_ਵਿੱਚ_ਰਸਤਾ(ptr, len, options,
						     ਗਿਣਤੀ_2, curbuf->b_ffname);
    }
    ਵਾਪਸ file_name_at_cursor(options | FNAME_HYP, ਗਿਣਤੀ_2, file_lnum);
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
file_name_at_cursor(ਪੂਰਨ options, ਲੰਮਾ ਗਿਣਤੀ_2, linenr_T *file_lnum)
{
    ਵਾਪਸ file_name_ਵਿੱਚ_ਸਤਰ(ml_get_curline(),
		      curwin->w_cursor.ਖਾਨਾ, options, ਗਿਣਤੀ_2, curbuf->b_ffname,
		      file_lnum);
}

/*
 * Return the name of the file under or after ptr[col].
 * Otherwise like file_name_at_cursor().
 */
    char_u *
file_name_ਵਿੱਚ_ਸਤਰ(
    char_u	*ਸਤਰ,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		options,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    char_u	*rel_fname,	// file we are searching relative to
    linenr_T	*file_lnum)	// line number after the file name
{
    char_u	*ptr;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ਵਿੱਚ_type = TRUE;
    ਪੂਰਨ		is_url = FALSE;

    /*
     * search forward for what could be the start of a file name
     */
    ptr = ਸਤਰ + ਖਾਨਾ;
    ਜਦੋਂ (*ptr != NUL && !vim_isfilec(*ptr))
	MB_PTR_ADV(ptr);
    ਜੇ (*ptr == NUL)		// nothing found
    {
	ਜੇ (options & FNAME_MESS)
	    emsg(_(e_no_file_name_under_cursor));
	ਵਾਪਸ ਨਿਰਾ;
    }

    /*
     * Search backward for first char of the file name.
     * Go one char back to ":" before "//" even when ':' is not in 'isfname'.
     */
    ਜਦੋਂ (ptr > ਸਤਰ)
    {
	ਜੇ (has_mbyte && (len = (*mb_ਸਿਰ_off)(ਸਤਰ, ptr - 1)) > 0)
	    ptr -= len + 1;
	ਨਹੀਂਤਾਂ ਜੇ (vim_isfilec(ptr[-1])
		|| ((options & FNAME_HYP) && ਰਸਤਾ_is_url(ptr - 1)))
	    --ptr;
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    /*
     * Search forward for the last char of the file name.
     * Also allow "://" when ':' is not in 'isfname'.
     */
    len = 0;
    ਜਦੋਂ (vim_isfilec(ptr[len]) || (ptr[len] == '\\' && ptr[len + 1] == ' ')
		 || ((options & FNAME_HYP) && ਰਸਤਾ_is_url(ptr + len))
		 || (is_url && vim_strchr((char_u *)":?&=", ptr[len]) != ਨਿਰਾ))
    {
	// After type:// we also include :, ?, & and = as valid characters, so
	// that http://google.com:8080?q=this&that=ok works.
	ਜੇ ((ptr[len] >= 'A' && ptr[len] <= 'Z')
				       || (ptr[len] >= 'a' && ptr[len] <= 'z'))
	{
	    ਜੇ (ਵਿੱਚ_type && ਰਸਤਾ_is_url(ptr + len + 1))
		is_url = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    ਵਿੱਚ_type = FALSE;

	ਜੇ (ptr[len] == '\\')
	    // Skip over the "\" in "\ ".
	    ++len;
	ਜੇ (has_mbyte)
	    len += (*mb_ptr2len)(ptr + len);
	ਨਹੀਂਤਾਂ
	    ++len;
    }

    /*
     * If there is trailing punctuation, remove it.
     * But don't remove "..", could be a directory name.
     */
    ਜੇ (len > 2 && vim_strchr((char_u *)".,:;!", ptr[len - 1]) != ਨਿਰਾ
						       && ptr[len - 2] != '.')
	--len;

    ਜੇ (file_lnum != ਨਿਰਾ)
    {
	char_u	*p;
	ਅੱਖਰ	*ਮੇਲ_ਪਾਠ = " line ";		// english
	size_t	ਮੇਲ_textlen = 6;

	// Get the number after the file name and a separator character.
	// Also accept " line 999" with and without the same translation as
	// used in last_set_msg().
	p = ptr + len;
	ਜੇ (STRNCMP(p, ਮੇਲ_ਪਾਠ, ਮੇਲ_textlen) == 0)
	    p += ਮੇਲ_textlen;
	ਨਹੀਂਤਾਂ
	{
	    // no match with english, try localized
	    ਮੇਲ_ਪਾਠ = _(ਸਤਰ_ਸੁਨੇਹਾ);
	    ਮੇਲ_textlen = STRLEN(ਮੇਲ_ਪਾਠ);

	    ਜੇ (STRNCMP(p, ਮੇਲ_ਪਾਠ, ਮੇਲ_textlen) == 0)
		p += ਮੇਲ_textlen;
	    ਨਹੀਂਤਾਂ
		p = skipwhite(p);
	}
	ਜੇ (*p != NUL)
	{
	    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*p))
		++p;		    // skip the separator
	    p = skipwhite(p);
	    ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*p))
		*file_lnum = (ਪੂਰਨ)getdigits(&p);
	}
    }

    ਵਾਪਸ ਲੱਭੋ_file_name_ਵਿੱਚ_ਰਸਤਾ(ptr, len, options, ਗਿਣਤੀ_2, rel_fname);
}

#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
    ਸਥਿਰ char_u *
eval_includeexpr(char_u *ptr, ਪੂਰਨ len)
{
    char_u	*res;
    sctx_T	save_sctx = current_sctx;

    ਸੈੱਟ_vim_var_string(VV_FNAME, ptr, len);
    current_sctx = curbuf->b_p_script_ctx[BV_INEX];

    res = eval_ਮੰਜ਼ਿਲ_string_ਸੁਰੱਖਿਅਤ(curbuf->b_p_inex,
	    was_ਸੈੱਟ_insecurely(curwin, (char_u *)"includeexpr", OPT_LOCAL),
								   TRUE, TRUE);

    ਸੈੱਟ_vim_var_string(VV_FNAME, ਨਿਰਾ, 0);
    current_sctx = save_sctx;
    ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

/*
 * Return the name of the file ptr[len] in 'path'.
 * Otherwise like file_name_at_cursor().
 */
    char_u *
ਲੱਭੋ_file_name_ਵਿੱਚ_ਰਸਤਾ(
    char_u	*ptr,
    ਪੂਰਨ		len,
    ਪੂਰਨ		options,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    char_u	*rel_fname)	// file we are searching relative to
{
    char_u	*file_name;
    ਪੂਰਨ		c;
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
    char_u	*tofree = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਜੇ (len == 0)
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
    ਜੇ ((options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
    {
	tofree = eval_includeexpr(ptr, len);
	ਜੇ (tofree != ਨਿਰਾ)
	{
	    ptr = tofree;
	    len = (ਪੂਰਨ)STRLEN(ptr);
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (options & FNAME_EXP)
    {
	char_u	*file_ਮੰਜ਼ਿਲ_ਲੱਭੋ = ਨਿਰਾ;
	ਅੱਖਰ	*ਖੋਜ_ctx = ਨਿਰਾ;

	file_name = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);

#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
	/*
	 * If the file could not be found in a normal way, try applying
	 * 'includeexpr' (unless done already).
	 */
	ਜੇ (file_name == ਨਿਰਾ
		&& !(options & FNAME_INCL) && *curbuf->b_p_inex != NUL)
	{
	    tofree = eval_includeexpr(ptr, len);
	    ਜੇ (tofree != ਨਿਰਾ)
	    {
		ptr = tofree;
		len = (ਪੂਰਨ)STRLEN(ptr);
		file_name = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(ptr, len, options & ~FNAME_MESS,
				  TRUE, rel_fname, &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	    }
	}
#ਜੇ_ਅੰਤ
	ਜੇ (file_name == ਨਿਰਾ && (options & FNAME_MESS))
	{
	    c = ptr[len];
	    ptr[len] = NUL;
	    semsg(_(e_cant_ਲੱਭੋ_file_str_ਵਿੱਚ_ਰਸਤਾ_2), ptr);
	    ptr[len] = c;
	}

	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	ਜਦੋਂ (file_name != ਨਿਰਾ && --ਗਿਣਤੀ_2 > 0)
	{
	    vim_free(file_name);
	    file_name = ਲੱਭੋ_file_ਵਿੱਚ_ਰਸਤਾ(ptr, len, options, FALSE, rel_fname,
						   &file_ਮੰਜ਼ਿਲ_ਲੱਭੋ, &ਖੋਜ_ctx);
	}

	vim_free(file_ਮੰਜ਼ਿਲ_ਲੱਭੋ);
	vim_findfile_cleanup(ਖੋਜ_ctx);
    }
    ਨਹੀਂਤਾਂ
	file_name = vim_strnsave(ptr, len);

#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
    vim_free(tofree);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ file_name;
}

/*
 * Return the end of the directory name, on the first path
 * separator:
 * "/path/file", "/path/dir/", "/path//dir", "/file"
 *	 ^	       ^	     ^	      ^
 */
    ਸਥਿਰ char_u *
gettail_dir(char_u *fname)
{
    char_u	*dir_ਸਿਰਾ = fname;
    char_u	*ਅਗਲਾ_dir_ਸਿਰਾ = fname;
    ਪੂਰਨ		look_for_sep = TRUE;
    char_u	*p;

    ਲਈ (p = fname; *p != NUL; )
    {
	ਜੇ (vim_ispathsep(*p))
	{
	    ਜੇ (look_for_sep)
	    {
		ਅਗਲਾ_dir_ਸਿਰਾ = p;
		look_for_sep = FALSE;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!look_for_sep)
		dir_ਸਿਰਾ = ਅਗਲਾ_dir_ਸਿਰਾ;
	    look_for_sep = TRUE;
	}
	MB_PTR_ADV(p);
    }
    ਵਾਪਸ dir_ਸਿਰਾ;
}

/*
 * return TRUE if 'c' is a path list separator.
 */
    ਪੂਰਨ
vim_ispathlistsep(ਪੂਰਨ c)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਵਾਪਸ (c == ':');
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (c == ';');	// might not be right for every system...
#ਜੇ_ਅੰਤ
}

/*
 * Moves "*psep" back to the previous path separator in "path".
 * Returns FAIL is "*psep" ends up at the beginning of "path".
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਪਿਛਲਾ_pathsep(char_u *ਰਸਤਾ, char_u **psep)
{
    // skip the current separator
    ਜੇ (*psep > ਰਸਤਾ && vim_ispathsep(**psep))
	--*psep;

    // find the previous separator
    ਜਦੋਂ (*psep > ਰਸਤਾ)
    {
	ਜੇ (vim_ispathsep(**psep))
	    ਵਾਪਸ OK;
	MB_PTR_ਪਿੱਠ(ਰਸਤਾ, *psep);
    }

    ਵਾਪਸ FAIL;
}

/*
 * Returns TRUE if "maybe_unique" is unique wrt other_paths in "gap".
 * "maybe_unique" is the end portion of "((char_u **)gap->ga_data)[i]".
 */
    ਸਥਿਰ ਪੂਰਨ
is_unique(char_u *maybe_unique, garray_T *gap, ਪੂਰਨ i)
{
    ਪੂਰਨ	    j;
    ਪੂਰਨ	    candidate_len = (ਪੂਰਨ)STRLEN(maybe_unique);
    ਪੂਰਨ	    other_ਰਸਤਾ_len;
    char_u  **other_paths = (char_u **)gap->ga_ਅੰਕੜੇ;
    char_u  *rival;

    ਲਈ (j = 0; j < gap->ga_len; j++)
    {
	ਜੇ (j == i)
	    ਜਾਰੀ;  // don't compare it with itself

	other_ਰਸਤਾ_len = (ਪੂਰਨ)STRLEN(other_paths[j]);
	ਜੇ (other_ਰਸਤਾ_len < candidate_len)
	    ਜਾਰੀ;  // it's different when it's shorter

	rival = other_paths[j] + other_ਰਸਤਾ_len - candidate_len;
	ਜੇ (fnamecmp(maybe_unique, rival) == 0
		&& (rival == other_paths[j] || vim_ispathsep(*(rival - 1))))
	    ਵਾਪਸ FALSE;  //ਮੇਲ
    }

    ਵਾਪਸ TRUE;  // no match found
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
    ਸਥਿਰ ਖਾਲੀ
expand_ਰਸਤਾ_option(
	char_u		*curdir,
	char_u		*ਰਸਤਾ_option,	// p_path or p_cdpath
	garray_T	*gap)
{
    char_u	*buf;
    size_t	buflen;
    char_u	*p;
    size_t	curdirlen = 0;

    ਜੇ ((buf = alloc(MAXPATHL)) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜਦੋਂ (*ਰਸਤਾ_option != NUL)
    {
	buflen = copy_option_part(&ਰਸਤਾ_option, buf, MAXPATHL, " ,");

	// do not expand backticks, could have been set via a modeline
	ਜੇ (vim_strchr(buf, '`') != ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (buf[0] == '.' && (buf[1] == NUL || vim_ispathsep(buf[1])))
	{
	    size_t  plen;

	    // Relative to current buffer:
	    // "/path/file" + "." -> "/path/"
	    // "/path/file"  + "./subdir" -> "/path/subdir"
	    ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
		ਜਾਰੀ;
	    p = gettail(curbuf->b_ffname);
	    plen = (size_t)(p - curbuf->b_ffname);
	    ਜੇ (plen + buflen >= MAXPATHL)
		ਜਾਰੀ;
	    ਜੇ (buf[1] == NUL)
		buf[plen] = NUL;
	    ਨਹੀਂਤਾਂ
		mch_memmove(buf + plen, buf + 2, (buflen - 2) + 1); // +1 for NUL
	    mch_memmove(buf, curbuf->b_ffname, plen);
	    buflen = simplify_filename(buf);
	}
	ਨਹੀਂਤਾਂ ਜੇ (buf[0] == NUL)
	{
	    // relative to current directory
	    STRCPY(buf, curdir);
	    ਜੇ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    buflen = curdirlen;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਰਸਤਾ_with_url(buf))
	    // URL can't be used here
	    ਜਾਰੀ;
	ਨਹੀਂਤਾਂ ਜੇ (!mch_is_ਭਰਿਆ_Name(buf))
	{
	    // Expand relative path to their full path equivalent
	    ਜੇ (curdirlen == 0)
		curdirlen = STRLEN(curdir);
	    ਜੇ (curdirlen + buflen + 3 > MAXPATHL)
		ਜਾਰੀ;

	    mch_memmove(buf + curdirlen + 1, buf, buflen + 1);	    // +1 for NUL
	    STRCPY(buf, curdir);
	    buf[curdirlen] = PATHSEP;
	    buflen = simplify_filename(buf);
	}

	ਜੇ (ga_grow(gap, 1) == FAIL)
	    ਰੋਕੋ;

#ਜੇ defined(MSWIN)
	// Avoid the path ending in a backslash, it fails when a comma is
	// appended.
	ਜੇ (buf[buflen - 1] == '\\')
	    buf[buflen - 1] = '/';
#ਜੇ_ਅੰਤ

	p = vim_strnsave(buf, buflen);
	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;
	((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len++] = p;
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
    ਸਥਿਰ char_u *
get_ਰਸਤਾ_cutoff(char_u *fname, garray_T *gap)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    maxlen = 0;
    char_u  **ਰਸਤਾ_part = (char_u **)gap->ga_ਅੰਕੜੇ;
    char_u  *cutoff = ਨਿਰਾ;

    ਲਈ (i = 0; i < gap->ga_len; i++)
    {
	ਪੂਰਨ j = 0;

	ਜਦੋਂ ((fname[j] == ਰਸਤਾ_part[i][j]
#ਜੇ defined(MSWIN)
		|| (vim_ispathsep(fname[j]) && vim_ispathsep(ਰਸਤਾ_part[i][j]))
#ਜੇ_ਅੰਤ
			     ) && fname[j] != NUL && ਰਸਤਾ_part[i][j] != NUL)
	    j++;
	ਜੇ (j > maxlen)
	{
	    maxlen = j;
	    cutoff = &fname[j];
	}
    }

    // skip to the file or directory name
    ਜੇ (cutoff != ਨਿਰਾ)
	ਜਦੋਂ (vim_ispathsep(*cutoff))
	    MB_PTR_ADV(cutoff);

    ਵਾਪਸ cutoff;
}

/*
 * Sorts, removes duplicates and modifies all the fullpath names in "gap" so
 * that they are unique with respect to each other while conserving the part
 * that matches the pattern. Beware, this is at least O(n^2) wrt "gap->ga_len".
 */
    ਖਾਲੀ
uniquefy_paths(
	garray_T *gap,
	char_u *ਨਮੂਨਾ,
	char_u *ਰਸਤਾ_option)	// p_path or p_cdpath
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;
    char_u	**fnames = (char_u **)gap->ga_ਅੰਕੜੇ;
    ਪੂਰਨ		ਛਾਂਟੋ_again = FALSE;
    char_u	*pat;
    char_u      *file_ਨਮੂਨਾ;
    char_u	*curdir;
    regmatch_T	regmatch;
    garray_T	ਰਸਤਾ_ga;
    char_u	**ਵਿੱਚ_curdir = ਨਿਰਾ;
    char_u	*short_name;

    remove_duplicates(gap);
    ga_init2(&ਰਸਤਾ_ga, ਆਕਾਰ(char_u *), 1);

    /*
     * We need to prepend a '*' at the beginning of file_pattern so that the
     * regex matches anywhere in the path. FIXME: is this valid for all
     * possible patterns?
     */
    len = (ਪੂਰਨ)STRLEN(ਨਮੂਨਾ);
    file_ਨਮੂਨਾ = alloc(len + 2);
    ਜੇ (file_ਨਮੂਨਾ == ਨਿਰਾ)
	ਵਾਪਸ;
    file_ਨਮੂਨਾ[0] = '*';
    STRCPY(file_ਨਮੂਨਾ + 1, ਨਮੂਨਾ);
    pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(file_ਨਮੂਨਾ, ਨਿਰਾ, ਨਿਰਾ, FALSE);
    vim_free(file_ਨਮੂਨਾ);
    ਜੇ (pat == ਨਿਰਾ)
	ਵਾਪਸ;

    regmatch.rm_ic = TRUE;		// always ignore case
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    vim_free(pat);
    ਜੇ (regmatch.regprog == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ ((curdir = alloc(MAXPATHL)) == ਨਿਰਾ)
	ਜਾਓ theend;
    mch_dirname(curdir, MAXPATHL);
    expand_ਰਸਤਾ_option(curdir, ਰਸਤਾ_option, &ਰਸਤਾ_ga);

    ਵਿੱਚ_curdir = ALLOC_CLEAR_MULT(char_u *, gap->ga_len);
    ਜੇ (ਵਿੱਚ_curdir == ਨਿਰਾ)
	ਜਾਓ theend;

    ਲਈ (i = 0; i < gap->ga_len && !got_int; i++)
    {
	char_u	    *ਰਸਤਾ = fnames[i];
	ਪੂਰਨ	    is_ਵਿੱਚ_curdir;
	char_u	    *dir_ਸਿਰਾ = gettail_dir(ਰਸਤਾ);
	char_u	    *ਰਸਤਾ_cutoff;

	len = (ਪੂਰਨ)STRLEN(ਰਸਤਾ);
	is_ਵਿੱਚ_curdir = fnamencmp(curdir, ਰਸਤਾ, dir_ਸਿਰਾ - ਰਸਤਾ) == 0
					     && curdir[dir_ਸਿਰਾ - ਰਸਤਾ] == NUL;
	ਜੇ (is_ਵਿੱਚ_curdir)
	    ਵਿੱਚ_curdir[i] = vim_strnsave(ਰਸਤਾ, len);

	// Shorten the filename while maintaining its uniqueness
	ਰਸਤਾ_cutoff = get_ਰਸਤਾ_cutoff(ਰਸਤਾ, &ਰਸਤਾ_ga);

	// Don't assume all files can be reached without path when search
	// pattern starts with star star slash, so only remove path_cutoff
	// when possible.
	ਜੇ (ਨਮੂਨਾ[0] == '*' && ਨਮੂਨਾ[1] == '*'
		&& vim_ispathsep_nocolon(ਨਮੂਨਾ[2])
		&& ਰਸਤਾ_cutoff != ਨਿਰਾ
		&& vim_regexec(&regmatch, ਰਸਤਾ_cutoff, (colnr_T)0)
		&& is_unique(ਰਸਤਾ_cutoff, gap, i))
	{
	    ਛਾਂਟੋ_again = TRUE;
	    mch_memmove(ਰਸਤਾ, ਰਸਤਾ_cutoff, STRLEN(ਰਸਤਾ_cutoff) + 1);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Here all files can be reached without path, so get shortest
	    // unique path.  We start at the end of the path.
	    char_u  *pathsep_p = ਰਸਤਾ + len - 1;

	    ਜਦੋਂ (ਲੱਭੋ_ਪਿਛਲਾ_pathsep(ਰਸਤਾ, &pathsep_p))
	    {
		ਜੇ (vim_regexec(&regmatch, pathsep_p + 1, (colnr_T)0)
			&& is_unique(pathsep_p + 1, gap, i)
			&& ਰਸਤਾ_cutoff != ਨਿਰਾ && pathsep_p + 1 >= ਰਸਤਾ_cutoff)
		{
		    ਛਾਂਟੋ_again = TRUE;
		    mch_memmove(ਰਸਤਾ, pathsep_p + 1,
			    (size_t)((ਰਸਤਾ + len) - (pathsep_p + 1)) + 1);  // +1 for NUL
		    ਰੋਕੋ;
		}
	    }
	}

	ਜੇ (mch_is_ਭਰਿਆ_Name(ਰਸਤਾ))
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
	    short_name = shorten_fname(ਰਸਤਾ, curdir);
	    ਜੇ (short_name != ਨਿਰਾ && short_name > ਰਸਤਾ + 1
#ਜੇ defined(MSWIN)
		    // On windows,
		    //	    shorten_fname("c:\a\a.txt", "c:\a\b")
		    // returns "\a\a.txt", which is not really the short
		    // name, hence:
		    && !vim_ispathsep(*short_name)
#ਜੇ_ਅੰਤ
		)
	    {
		vim_snprintf((ਅੱਖਰ *)ਰਸਤਾ, MAXPATHL, ".%s%s", PATHSEPSTR, short_name);
	    }
	}
	ui_breakcheck();
    }

    // Shorten filenames in /in/current/directory/{filename}
    ਲਈ (i = 0; i < gap->ga_len && !got_int; i++)
    {
	size_t	rel_pathsize;
	char_u	*rel_ਰਸਤਾ;
	char_u	*ਰਸਤਾ = ਵਿੱਚ_curdir[i];

	ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
	    ਜਾਰੀ;

	// If the {filename} is not unique, change it to ./{filename}.
	// Else reduce it to {filename}
	short_name = shorten_fname(ਰਸਤਾ, curdir);
	ਜੇ (short_name == ਨਿਰਾ)
	    short_name = ਰਸਤਾ;
	ਜੇ (is_unique(short_name, gap, i))
	{
	    STRCPY(fnames[i], short_name);
	    ਜਾਰੀ;
	}

	rel_pathsize = 1 + ਆਕਾਰ(PATHSEP) + STRLEN(short_name) + 1;
	rel_ਰਸਤਾ = alloc(rel_pathsize);
	ਜੇ (rel_ਰਸਤਾ == ਨਿਰਾ)
	    ਜਾਓ theend;

	vim_snprintf((ਅੱਖਰ *)rel_ਰਸਤਾ, rel_pathsize, ".%s%s", PATHSEPSTR, short_name);

	vim_free(fnames[i]);
	fnames[i] = rel_ਰਸਤਾ;
	ਛਾਂਟੋ_again = TRUE;
	ui_breakcheck();
    }

theend:
    vim_free(curdir);
    ਜੇ (ਵਿੱਚ_curdir != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < gap->ga_len; i++)
	    vim_free(ਵਿੱਚ_curdir[i]);
	vim_free(ਵਿੱਚ_curdir);
    }
    ga_clear_ਲੜੀਆਂ(&ਰਸਤਾ_ga);
    vim_regfree(regmatch.regprog);

    ਜੇ (ਛਾਂਟੋ_again)
	remove_duplicates(gap);
}

/*
 * Calls globpath() with 'path' values for the given pattern and stores the
 * result in "gap".
 * Returns the total number of matches.
 */
    ਪੂਰਨ
expand_ਵਿੱਚ_ਰਸਤਾ(
    garray_T	*gap,
    char_u	*ਨਮੂਨਾ,
    ਪੂਰਨ		flags)		// EW_* flags
{
    char_u	*curdir;
    garray_T	ਰਸਤਾ_ga;
    char_u	*paths = ਨਿਰਾ;
    ਪੂਰਨ		glob_flags = 0;
    char_u	*ਰਸਤਾ_option = *curbuf->b_p_ਰਸਤਾ == NUL ? p_ਰਸਤਾ : curbuf->b_p_ਰਸਤਾ;

    ਜੇ ((curdir = alloc(MAXPATHL)) == ਨਿਰਾ)
	ਵਾਪਸ 0;
    mch_dirname(curdir, MAXPATHL);

    ga_init2(&ਰਸਤਾ_ga, ਆਕਾਰ(char_u *), 1);
    ਜੇ (flags & EW_CDPATH)
	expand_ਰਸਤਾ_option(curdir, p_cdpath, &ਰਸਤਾ_ga);
    ਨਹੀਂਤਾਂ
	expand_ਰਸਤਾ_option(curdir, ਰਸਤਾ_option, &ਰਸਤਾ_ga);
    vim_free(curdir);
    ਜੇ (ਰਸਤਾ_ga.ga_len == 0)
	ਵਾਪਸ 0;

    paths = ga_concat_ਲੜੀਆਂ(&ਰਸਤਾ_ga, ",");
    ga_clear_ਲੜੀਆਂ(&ਰਸਤਾ_ga);
    ਜੇ (paths == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (flags & EW_ICASE)
	glob_flags |= WILD_ICASE;
    ਜੇ (flags & EW_ADDSLASH)
	glob_flags |= WILD_ਜੋੜੋ_SLASH;
    globpath(paths, ਨਮੂਨਾ, gap, glob_flags, !!(flags & EW_CDPATH));
    vim_free(paths);

    ਵਾਪਸ gap->ga_len;
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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AMIGA	    // Amiga doesn't have "..", it uses "/"
    ਪੂਰਨ		components = 0;
    char_u	*p, *ਪੂਛ, *ਸ਼ੁਰੂ;
    char_u	*p_ਸਿਰਾ;			    // point to NUL at end of string "p"
    ਪੂਰਨ		stripping_disabled = FALSE;
    ਪੂਰਨ		relative = TRUE;

    p = filename;
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
    ਜੇ (p[0] != NUL && p[1] == ':')	    // skip "x:"
	p += 2;
# endif

    ਜੇ (vim_ispathsep(*p))
    {
	relative = FALSE;
	ਕਰੋ
	    ++p;
	ਜਦੋਂ (vim_ispathsep(*p));
    }
    ਸ਼ੁਰੂ = p;	    // remember start after "c:/" or "/" or "///"
    p_ਸਿਰਾ = p + STRLEN(p);
# ifdef UNIX
    // Posix says that "//path" is unchanged but "///path" is "/path".
    ਜੇ (ਸ਼ੁਰੂ > filename + 2)
    {
	mch_memmove(filename + 1, p, (size_t)(p_ਸਿਰਾ - p) + 1);	    // +1 for NUL
	p_ਸਿਰਾ -= (size_t)(p - (filename + 1));
	ਸ਼ੁਰੂ = p = filename + 1;
    }
# endif

    ਕਰੋ
    {
	// At this point "p" is pointing to the char following a single "/"
	// or "p" is at the "start" of the (absolute or relative) path name.
# ifdef VMS
	// VMS allows device:[path] - don't strip the [ in directory
	ਜੇ ((*p == '[' || *p == '<') && p > filename && p[-1] == ':')
	{
	    // :[ or :< composition: vms directory component
	    ++components;
	    p = getnextcomp(p + 1);
	}
	// allow remote calls as host"user passwd"::device:[path]
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == ':' && p[1] == ':' && p > filename && p[-1] == '"' )
	{
	    // ":: composition: vms host/passwd component
	    ++components;
	    p = getnextcomp(p + 2);
	}
	ਨਹੀਂਤਾਂ
# endif
	ਜੇ (vim_ispathsep(*p))
	{
	    mch_memmove(p, p + 1, (size_t)(p_ਸਿਰਾ - (p + 1)) + 1); // remove duplicate "/"
	    --p_ਸਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '.' && (vim_ispathsep(p[1]) || p[1] == NUL))
	{
	    ਜੇ (p == ਸ਼ੁਰੂ && relative)
		p += 1 + (p[1] != NUL);	// keep single "." or leading "./"
	    ਨਹੀਂਤਾਂ
	    {
		// Strip "./" or ".///".  If we are at the end of the file name
		// and there is no trailing path separator, either strip "/." if
		// we are after "start", or strip "." if we are at the beginning
		// of an absolute path name .
		ਪੂਛ = p + 1;
		ਜੇ (p[1] != NUL)
		    ਜਦੋਂ (vim_ispathsep(*ਪੂਛ))
			MB_PTR_ADV(ਪੂਛ);
		ਨਹੀਂਤਾਂ ਜੇ (p > ਸ਼ੁਰੂ)
		    --p;		// strip preceding path separator

		mch_memmove(p, ਪੂਛ, (size_t)(p_ਸਿਰਾ - ਪੂਛ) + 1);
		p_ਸਿਰਾ -= (size_t)(ਪੂਛ - p);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '.' && p[1] == '.' &&
	    (vim_ispathsep(p[2]) || p[2] == NUL))
	{
	    // Skip to after ".." or "../" or "..///".
	    ਪੂਛ = p + 2;
	    ਜਦੋਂ (vim_ispathsep(*ਪੂਛ))
		MB_PTR_ADV(ਪੂਛ);

	    ਜੇ (components > 0)		// strip one preceding component
	    {
		ਪੂਰਨ		do_strip = FALSE;
		char_u		saved_char;
		stat_T		st;

		// Don't strip for an erroneous file name.
		ਜੇ (!stripping_disabled)
		{
		    // If the preceding component does not exist in the file
		    // system, we strip it.  On Unix, we don't accept a symbolic
		    // link that refers to a non-existent file.
		    saved_char = p[-1];
		    p[-1] = NUL;
# ifdef UNIX
		    ਜੇ (mch_lstat((ਅੱਖਰ *)filename, &st) < 0)
# ਨਹੀਂਤਾਂ
			ਜੇ (mch_stat((ਅੱਖਰ *)filename, &st) < 0)
# endif
			    do_strip = TRUE;
		    p[-1] = saved_char;

		    --p;
		    // Skip back to after previous '/'.
		    ਜਦੋਂ (p > ਸ਼ੁਰੂ && !after_pathsep(ਸ਼ੁਰੂ, p))
			MB_PTR_ਪਿੱਠ(ਸ਼ੁਰੂ, p);

		    ਜੇ (!do_strip)
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
			saved_char = *ਪੂਛ;
			*ਪੂਛ = NUL;
			ਜੇ (mch_stat((ਅੱਖਰ *)filename, &st) >= 0)
			    do_strip = TRUE;
			ਨਹੀਂਤਾਂ
			    stripping_disabled = TRUE;
			*ਪੂਛ = saved_char;
# ifdef UNIX
			ਜੇ (do_strip)
			{
			    stat_T	new_st;

			    // On Unix, the check for the unstripped file name
			    // above works also for a symbolic link pointing to
			    // a searchable directory.  But then the parent of
			    // the directory pointed to by the link must be the
			    // same as the stripped file name.  (The latter
			    // exists in the file system since it is the
			    // component's parent directory.)
			    ਜੇ (p == ਸ਼ੁਰੂ && relative)
				(ਖਾਲੀ)mch_stat(".", &new_st);
			    ਨਹੀਂਤਾਂ
			    {
				saved_char = *p;
				*p = NUL;
				(ਖਾਲੀ)mch_stat((ਅੱਖਰ *)filename, &new_st);
				*p = saved_char;
			    }

			    ਜੇ (new_st.st_ino != st.st_ino ||
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

		ਜੇ (!do_strip)
		{
		    // Skip the ".." or "../" and reset the counter for the
		    // components that might be stripped later on.
		    p = ਪੂਛ;
		    components = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Strip previous component.  If the result would get empty
		    // and there is no trailing path separator, leave a single
		    // "." instead.  If we are at the end of the file name and
		    // there is no trailing path separator and a preceding
		    // component is left after stripping, strip its trailing
		    // path separator as well.
		    ਜੇ (p == ਸ਼ੁਰੂ && relative && ਪੂਛ[-1] == '.')
		    {
			*p++ = '.';
			*p = NUL;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (p > ਸ਼ੁਰੂ && ਪੂਛ[-1] == '.')
			    --p;

			mch_memmove(p, ਪੂਛ, (size_t)(p_ਸਿਰਾ - ਪੂਛ) + 1);	// strip previous component
			p_ਸਿਰਾ -= (size_t)(ਪੂਛ - p);
		    }

		    --components;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (p == ਸ਼ੁਰੂ && !relative)	// leading "/.." or "/../"
	    {
		mch_memmove(p, ਪੂਛ, (size_t)(p_ਸਿਰਾ - ਪੂਛ) + 1);		// strip ".." or "../"
		p_ਸਿਰਾ -= (size_t)(ਪੂਛ - p);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (p == ਸ਼ੁਰੂ + 2 && p[-2] == '.')	// leading "./../"
		{
		    mch_memmove(p - 2, p, (size_t)(p_ਸਿਰਾ - p) + 1); // strip leading "./"
		    p_ਸਿਰਾ -= 2;
		    ਪੂਛ -= 2;
		}
		p = ਪੂਛ;		// skip to char after ".." or "../"
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ++components;		// simple path component
	    p = getnextcomp(p);
	}
    } ਜਦੋਂ (*p != NUL);

    ਵਾਪਸ (size_t)(p_ਸਿਰਾ - filename);
#ਨਹੀਂਤਾਂ
    // Don't touch Amiga filenames
    ਵਾਪਸ STRLEN(filename);
#ਜੇ_ਅੰਤ // !AMIGA
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "simplify()" function
 */
    ਖਾਲੀ
f_simplify(typval_T *argvars, typval_T *rettv)
{
    char_u	*p;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    p = tv_get_string_strict(&argvars[0]);
    rettv->vval.v_string = vim_strsave(p);
    simplify_filename(rettv->vval.v_string);	// simplify in place
    rettv->v_type = VAR_STRING;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL
